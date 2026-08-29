/****************************************************************************
 * drivers/misc/thermal_printer.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/kmalloc.h>
#include <nuttx/fs/fs.h>
#include <nuttx/mutex.h>
#include <nuttx/spi/spi.h>
#include <nuttx/misc/thermal_printer.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define THERMAL_PRINTER_DEVNAME  "/dev/tp0"
#define SPI_FREQUENCY            1000000  /* 1 MHz SPI clock */
#define SPI_DEVID                0        /* Device ID for CS selection */

/* SPI transfer chunk size to avoid large allocations */

#define SPI_CHUNK_SIZE           64

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* Thermal printer device state */

struct thermal_printer_dev_s
{
  FAR struct spi_dev_s *spi;   /* SPI device handle */
  mutex_t               lock;  /* Mutual exclusion */
  int16_t               crefs; /* Number of open references */
  bool                  unlinked; /* True if device unlinked */
  uint8_t               density;  /* Heating density (0-255) */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int     tp_open(FAR struct file *filep);
static int     tp_close(FAR struct file *filep);
static ssize_t tp_write(FAR struct file *filep, FAR const char *buffer,
                 size_t buflen);
static int     tp_ioctl(FAR struct file *filep, int cmd,
                 unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct file_operations g_tp_fops =
{
  tp_open,     /* open */
  tp_close,    /* close */
  NULL,        /* read  - printer is write-only */
  tp_write,    /* write */
  NULL,        /* seek  */
  tp_ioctl,    /* ioctl */
  NULL,        /* mmap  */
  NULL,        /* truncate */
  NULL,        /* poll  */
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: tp_spi_send
 *
 * Description:
 *   Send a single byte over SPI.
 *   Uses SPI_SEND for single-byte transfers.
 *
 ****************************************************************************/

static inline void tp_spi_send(FAR struct spi_dev_s *spi, uint8_t data)
{
  SPI_SEND(spi, (uint16_t)data);
}

/****************************************************************************
 * Name: tp_spi_sendbuf
 *
 * Description:
 *   Send a buffer of bytes over SPI.
 *   Uses SPI_SNDBLOCK for multi-byte transfers.
 *
 ****************************************************************************/

static void tp_spi_sendbuf(FAR struct spi_dev_s *spi,
                           FAR const uint8_t *buf, size_t len)
{
  while (len > 0)
  {
    size_t chunk = (len > SPI_CHUNK_SIZE) ? SPI_CHUNK_SIZE : len;
    SPI_SNDBLOCK(spi, (FAR const void *)buf, chunk);
    buf += chunk;
    len -= chunk;
  }
}

/****************************************************************************
 * Name: tp_send_cmd
 *
 * Description:
 *   Send a short ESC/POS command (typically 2-3 bytes).
 *   Locks SPI, selects device, sends command, deselects, unlocks.
 *
 ****************************************************************************/

static void tp_send_cmd(FAR struct thermal_printer_dev_s *priv,
                        FAR const uint8_t *cmd, size_t len)
{
  FAR struct spi_dev_s *spi = priv->spi;

  SPI_LOCK(spi, true);
  SPI_SETFREQUENCY(spi, SPI_FREQUENCY);
  SPI_SETMODE(spi, SPIDEV_MODE0);
  SPI_SETBITS(spi, 8);
  SPI_SELECT(spi, SPI_DEVID, true);

  tp_spi_sendbuf(spi, cmd, len);

  SPI_SELECT(spi, SPI_DEVID, false);
  SPI_LOCK(spi, false);
}

/****************************************************************************
 * Name: tp_send_init
 *
 * Description:
 *   Send the ESC/POS initialization sequence to the printer.
 *
 ****************************************************************************/

static void tp_send_init(FAR struct thermal_printer_dev_s *priv)
{
  uint8_t init_cmd[] = { ESC_AT };
  tp_send_cmd(priv, init_cmd, sizeof(init_cmd));
}

/****************************************************************************
 * Name: tp_send_cut
 *
 * Description:
 *   Send the paper cut command.
 *
 ****************************************************************************/

static void tp_send_cut(FAR struct thermal_printer_dev_s *priv,
                        uint8_t style)
{
  uint8_t cut_cmd[3];

  cut_cmd[0] = 0x1d;  /* GS */
  cut_cmd[1] = 0x56;  /* V */
  cut_cmd[2] = (style == 0) ? 0x00 : 0x01;

  tp_send_cmd(priv, cut_cmd, sizeof(cut_cmd));
}

/****************************************************************************
 * Name: tp_send_feed
 *
 * Description:
 *   Feed n lines of blank paper.
 *
 ****************************************************************************/

static void tp_send_feed(FAR struct thermal_printer_dev_s *priv,
                         uint8_t lines)
{
  uint8_t feed_cmd[] = { 0x1b, 0x64, lines };  /* ESC d n */
  tp_send_cmd(priv, feed_cmd, sizeof(feed_cmd));
}

/****************************************************************************
 * Name: tp_set_density
 *
 * Description:
 *   Set the heating density using GS 8 command.
 *
 ****************************************************************************/

static void tp_set_density(FAR struct thermal_printer_dev_s *priv,
                           uint8_t density)
{
  uint8_t density_cmd[] = { 0x1d, 0x38, density };  /* GS 8 n */
  priv->density = density;
  tp_send_cmd(priv, density_cmd, sizeof(density_cmd));
}

/****************************************************************************
 * Name: tp_open
 *
 * Description:
 *   Open the thermal printer device.
 *
 ****************************************************************************/

static int tp_open(FAR struct file *filep)
{
  FAR struct inode *inode = filep->f_inode;
  FAR struct thermal_printer_dev_s *priv = inode->i_private;
  int ret;

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  if (priv->unlinked)
    {
      nxmutex_unlock(&priv->lock);
      return -ENODEV;
    }

  priv->crefs++;
  nxmutex_unlock(&priv->lock);

  spiinfo("thermal_printer: opened (refs=%d)\n", priv->crefs);
  return OK;
}

/****************************************************************************
 * Name: tp_close
 *
 * Description:
 *   Close the thermal printer device.
 *
 ****************************************************************************/

static int tp_close(FAR struct file *filep)
{
  FAR struct inode *inode = filep->f_inode;
  FAR struct thermal_printer_dev_s *priv = inode->i_private;
  int ret;

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  if (priv->crefs > 0)
    {
      priv->crefs--;
    }

  nxmutex_unlock(&priv->lock);

  spiinfo("thermal_printer: closed (refs=%d)\n", priv->crefs);
  return OK;
}

/****************************************************************************
 * Name: tp_write
 *
 * Description:
 *   Write data to the thermal printer.  Data is sent directly over SPI
 *   as raw bytes.  ESC/POS commands can be embedded in the data stream.
 *
 ****************************************************************************/

static ssize_t tp_write(FAR struct file *filep, FAR const char *buffer,
                        size_t buflen)
{
  FAR struct inode *inode = filep->f_inode;
  FAR struct thermal_printer_dev_s *priv = inode->i_private;
  FAR struct spi_dev_s *spi;
  int ret;

  if (buflen == 0)
    {
      return 0;
    }

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  if (priv->unlinked)
    {
      nxmutex_unlock(&priv->lock);
      return -ENODEV;
    }

  spi = priv->spi;

  /* Lock the SPI bus, configure, select, send data, deselect, unlock */

  SPI_LOCK(spi, true);
  SPI_SETFREQUENCY(spi, SPI_FREQUENCY);
  SPI_SETMODE(spi, SPIDEV_MODE0);
  SPI_SETBITS(spi, 8);
  SPI_SELECT(spi, SPI_DEVID, true);

  tp_spi_sendbuf(spi, (FAR const uint8_t *)buffer, buflen);

  SPI_SELECT(spi, SPI_DEVID, false);
  SPI_LOCK(spi, false);

  nxmutex_unlock(&priv->lock);

  spiinfo("thermal_printer: wrote %zu bytes\n", buflen);
  return (ssize_t)buflen;
}

/****************************************************************************
 * Name: tp_ioctl
 *
 * Description:
 *   Handle ioctl commands for the thermal printer.
 *
 ****************************************************************************/

static int tp_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
  FAR struct inode *inode = filep->f_inode;
  FAR struct thermal_printer_dev_s *priv = inode->i_private;
  int ret;

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  if (priv->unlinked)
    {
      nxmutex_unlock(&priv->lock);
      return -ENODEV;
    }

  switch (cmd)
    {
      case TPIOC_SET_DENSITY:
        {
          uint8_t density = (uint8_t)(arg & 0xff);
          tp_set_density(priv, density);
          spiinfo("thermal_printer: density=%d\n", density);
          ret = OK;
        }
        break;

      case TPIOC_CUT:
        {
          uint8_t style = (uint8_t)(arg & 0xff);
          tp_send_cut(priv, style);
          spiinfo("thermal_printer: cut (style=%d)\n", style);
          ret = OK;
        }
        break;

      case TPIOC_FEED:
        {
          uint8_t lines = (uint8_t)(arg & 0xff);
          tp_send_feed(priv, lines);
          spiinfo("thermal_printer: feed %d lines\n", lines);
          ret = OK;
        }
        break;

      case TPIOC_INIT:
        {
          tp_send_init(priv);
          tp_set_density(priv, THERMAL_PRINTER_DEFAULT_DENSITY);
          spiinfo("thermal_printer: re-initialized\n");
          ret = OK;
        }
        break;

      default:
        spierr("thermal_printer: unknown ioctl cmd=%d\n", cmd);
        ret = -ENOTTY;
        break;
    }

  nxmutex_unlock(&priv->lock);
  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: thermal_printer_register
 *
 * Description:
 *   Register the thermal printer character device (/dev/tp0).
 *
 * Input Parameters:
 *   spi - An instance of the SPI device used to communicate with the
 *         thermal printer.
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int thermal_printer_register(FAR struct spi_dev_s *spi)
{
  FAR struct thermal_printer_dev_s *priv;
  int ret;

  if (spi == NULL)
    {
      spierr("thermal_printer: NULL SPI device\n");
      return -EINVAL;
    }

  /* Allocate the device state structure */

  priv = kmm_zalloc(sizeof(struct thermal_printer_dev_s));
  if (priv == NULL)
    {
      spierr("thermal_printer: failed to allocate device state\n");
      return -ENOMEM;
    }

  /* Initialize the device state */

  priv->spi     = spi;
  priv->crefs   = 0;
  priv->unlinked = false;
  priv->density = THERMAL_PRINTER_DEFAULT_DENSITY;
  nxmutex_init(&priv->lock);

  /* Register the character device */

  ret = register_driver(THERMAL_PRINTER_DEVNAME, &g_tp_fops, 0666, priv);
  if (ret < 0)
    {
      spierr("thermal_printer: register_driver failed: %d\n", ret);
      nxmutex_destroy(&priv->lock);
      kmm_free(priv);
      return ret;
    }

  /* Send the initialization sequence to the printer */

  tp_send_init(priv);

  syslog(LOG_INFO, "thermal_printer: registered %s\n", THERMAL_PRINTER_DEVNAME);
  return OK;
}
