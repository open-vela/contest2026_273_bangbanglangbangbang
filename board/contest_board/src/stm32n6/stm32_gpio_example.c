/****************************************************************************
 * arch/arm/src/stm32n6/stm32_gpio_example.c
 *
 * GPIO Driver Usage Example for STM32N647
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <unistd.h>

#include "stm32_gpio.h"
#include "hardware/stm32_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Example GPIO pin definitions */

#define LED_PORT      GPIO_PORTA
#define LED_PIN       GPIO_PIN5

#define BUTTON_PORT   GPIO_PORTC
#define BUTTON_PIN    GPIO_PIN13

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: led_init
 *
 * Description:
 *   Initialize LED GPIO pin as output.
 *
 ****************************************************************************/

static void led_init(void)
{
  /* Configure PA5 as output, push-pull, low speed, no pull */

  stm32_gpio_config(LED_PORT, LED_PIN,
                    GPIO_MODER_OUTPUT,   /* Output mode */
                    GPIO_OTYPER_PP,      /* Push-pull */
                    GPIO_OSPEEDR_LOW,    /* Low speed */
                    GPIO_PUPDR_NONE,     /* No pull-up/down */
                    0);                  /* AF not used */
}

/****************************************************************************
 * Name: button_init
 *
 * Description:
 *   Initialize button GPIO pin as input with pull-up.
 *
 ****************************************************************************/

static void button_init(void)
{
  /* Configure PC13 as input, no output type, low speed, pull-up */

  stm32_gpio_config(BUTTON_PORT, BUTTON_PIN,
                    GPIO_MODER_INPUT,    /* Input mode */
                    GPIO_OTYPER_PP,      /* Push-pull (not used for input) */
                    GPIO_OSPEEDR_LOW,    /* Low speed */
                    GPIO_PUPDR_UP,       /* Pull-up */
                    0);                  /* AF not used */
}

/****************************************************************************
 * Name: button_isr
 *
 * Description:
 *   Button interrupt handler.
 *
 ****************************************************************************/

static int button_isr(int irq, void *context, void *arg)
{
  printf("Button pressed!\n");
  return OK;
}

/****************************************************************************
 * Name: button_irq_init
 *
 * Description:
 *   Configure button GPIO interrupt.
 *
 ****************************************************************************/

static void button_irq_init(void)
{
  int ret;

  /* Configure interrupt on falling edge (button press) */

  ret = stm32_gpio_irq_config(BUTTON_PORT, BUTTON_PIN,
                               EXTI_TRIGGER_FALLING,
                               button_isr, NULL);
  if (ret < 0)
    {
      printf("Failed to configure button IRQ: %d\n", ret);
      return;
    }

  /* Enable interrupt */

  ret = stm32_gpio_irq_enable(BUTTON_PORT, BUTTON_PIN);
  if (ret < 0)
    {
      printf("Failed to enable button IRQ: %d\n", ret);
      return;
    }

  printf("Button IRQ configured on PC13 (falling edge)\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_gpio_example
 *
 * Description:
 *   GPIO driver usage example.
 *
 ****************************************************************************/

int stm32_gpio_example(void)
{
  bool button_state;
  int ret;
  int i;

  printf("STM32N6 GPIO Driver Example\n");
  printf("===========================\n\n");

  /* Initialize GPIO subsystem */

  stm32_gpioinit();

  /* Initialize LED and button */

  led_init();
  button_init();

  printf("LED initialized on PA5\n");
  printf("Button initialized on PC13\n\n");

  /* Example 1: Simple LED blink */

  printf("Example 1: LED Blink\n");
  for (i = 0; i < 5; i++)
    {
      /* Turn LED on */

      ret = stm32_gpio_write(LED_PORT, LED_PIN, true);
      if (ret < 0)
        {
          printf("Failed to write GPIO: %d\n", ret);
          return ret;
        }

      printf("LED ON\n");
      usleep(500000);  /* 500ms */

      /* Turn LED off */

      ret = stm32_gpio_write(LED_PORT, LED_PIN, false);
      if (ret < 0)
        {
          printf("Failed to write GPIO: %d\n", ret);
          return ret;
        }

      printf("LED OFF\n");
      usleep(500000);  /* 500ms */
    }

  printf("\n");

  /* Example 2: Read button state */

  printf("Example 2: Button Read\n");
  for (i = 0; i < 3; i++)
    {
      ret = stm32_gpio_read(BUTTON_PORT, BUTTON_PIN, &button_state);
      if (ret < 0)
        {
          printf("Failed to read GPIO: %d\n", ret);
          return ret;
        }

      printf("Button state: %s\n", button_state ? "Released" : "Pressed");
      usleep(1000000);  /* 1 second */
    }

  printf("\n");

  /* Example 3: Button interrupt */

  printf("Example 3: Button Interrupt\n");
  button_irq_init();

  printf("Waiting for button presses (10 seconds)...\n");
  sleep(10);

  /* Disable interrupt */

  ret = stm32_gpio_irq_disable(BUTTON_PORT, BUTTON_PIN);
  if (ret < 0)
    {
      printf("Failed to disable button IRQ: %d\n", ret);
      return ret;
    }

  printf("\nGPIO example completed!\n");
  return OK;
}
