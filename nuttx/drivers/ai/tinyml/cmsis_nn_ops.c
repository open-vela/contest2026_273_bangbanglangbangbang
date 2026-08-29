/****************************************************************************
 * drivers/ai/tinyml/cmsis_nn_ops.c
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
#include <nuttx/ai/cmsis_nn_wrapper.h>
#include <nuttx/log.h>
#include <string.h>
#include <math.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CMSIS_NN_LOG_TAG "cmsis_nn"

/* Check for Helium MVE support */

#if defined(__ARM_FEATURE_MVE) && defined(CONFIG_AI_CMSIS_NN_HELUM)
#  define USE_MVE_INTRINSICS  1
#  include <arm_mve.h>
#else
#  define USE_MVE_INTRINSICS  0
#endif

/****************************************************************************
 * Private Functions - Scalar Implementations
 ****************************************************************************/

static void convolve_s8_scalar(
    FAR const struct cmsis_nn_conv_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  int32_t input_h = input_dims->h;
  int32_t input_w = input_dims->w;
  int32_t input_c = input_dims->c;
  int32_t filter_h = filter_dims->h;
  int32_t filter_w = filter_dims->w;
  int32_t output_h = output_dims->h;
  int32_t output_w = output_dims->w;
  int32_t output_c = output_dims->c;
  int32_t stride = params->stride;
  int32_t padding = params->padding;

  for (int32_t oc = 0; oc < output_c; oc++)
    {
      for (int32_t oh = 0; oh < output_h; oh++)
        {
          for (int32_t ow = 0; ow < output_w; ow++)
            {
              int32_t sum = bias_data ? bias_data[oc] : 0;

              for (int32_t fh = 0; fh < filter_h; fh++)
                {
                  for (int32_t fw = 0; fw < filter_w; fw++)
                    {
                      for (int32_t ic = 0; ic < input_c; ic++)
                        {
                          int32_t ih = oh * stride - padding + fh;
                          int32_t iw = ow * stride - padding + fw;

                          if (ih >= 0 && ih < input_h &&
                              iw >= 0 && iw < input_w)
                            {
                              int32_t input_idx =
                                  (ih * input_w + iw) * input_c + ic;
                              int32_t filter_idx =
                                  ((oc * filter_h + fh) * filter_w + fw) *
                                  input_c + ic;

                              sum += (int32_t)input_data[input_idx] *
                                     (int32_t)filter_data[filter_idx];
                            }
                        }
                    }
                }

              /* Apply activation and quantize */

              sum += params->input_offset;
              if (sum < params->activation_min)
                {
                  sum = params->activation_min;
                }
              if (sum > params->activation_max)
                {
                  sum = params->activation_max;
                }

              output_data[(oh * output_w + ow) * output_c + oc] =
                  (int8_t)(sum + params->output_offset);
            }
        }
    }
}

static void fully_connected_s8_scalar(
    FAR const struct cmsis_nn_fc_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  int32_t input_size = input_dims->w;
  int32_t output_size = output_dims->w;

  for (int32_t oc = 0; oc < output_size; oc++)
    {
      int32_t sum = bias_data ? bias_data[oc] : 0;

      for (int32_t ic = 0; ic < input_size; ic++)
        {
          sum += (int32_t)input_data[ic] *
                 (int32_t)filter_data[oc * input_size + ic];
        }

      /* Apply activation and quantize */

      sum += params->input_offset;
      if (sum < params->activation_min)
        {
          sum = params->activation_min;
        }
      if (sum > params->activation_max)
        {
          sum = params->activation_max;
        }

      output_data[oc] = (int8_t)(sum + params->output_offset);
    }
}

static void max_pool_s8_scalar(
    FAR const struct cmsis_nn_pool_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  int32_t input_h = input_dims->h;
  int32_t input_w = input_dims->w;
  int32_t input_c = input_dims->c;
  int32_t filter_h = filter_dims->h;
  int32_t filter_w = filter_dims->w;
  int32_t output_h = output_dims->h;
  int32_t output_w = output_dims->w;
  int32_t stride = params->stride;
  int32_t padding = params->padding;

  for (int32_t oc = 0; oc < input_c; oc++)
    {
      for (int32_t oh = 0; oh < output_h; oh++)
        {
          for (int32_t ow = 0; ow < output_w; ow++)
            {
              int8_t max_val = -128;

              for (int32_t fh = 0; fh < filter_h; fh++)
                {
                  for (int32_t fw = 0; fw < filter_w; fw++)
                    {
                      int32_t ih = oh * stride - padding + fh;
                      int32_t iw = ow * stride - padding + fw;

                      if (ih >= 0 && ih < input_h &&
                          iw >= 0 && iw < input_w)
                        {
                          int8_t val =
                              input_data[(ih * input_w + iw) * input_c + oc];
                          if (val > max_val)
                            {
                              max_val = val;
                            }
                        }
                    }
                }

              if (max_val < params->activation_min)
                {
                  max_val = params->activation_min;
                }
              if (max_val > params->activation_max)
                {
                  max_val = params->activation_max;
                }

              output_data[(oh * output_w + ow) * input_c + oc] = max_val;
            }
        }
    }
}

/****************************************************************************
 * Private Functions - MVE Accelerated Implementations
 ****************************************************************************/

#if USE_MVE_INTRINSICS

static void convolve_s8_mve(
    FAR const struct cmsis_nn_conv_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  int32_t input_h = input_dims->h;
  int32_t input_w = input_dims->w;
  int32_t input_c = input_dims->c;
  int32_t filter_h = filter_dims->h;
  int32_t filter_w = filter_dims->w;
  int32_t output_h = output_dims->h;
  int32_t output_w = output_dims->w;
  int32_t output_c = output_dims->c;
  int32_t stride = params->stride;
  int32_t padding = params->padding;

  /* Process 16 output channels at a time using MVE */

  for (int32_t oc = 0; oc < output_c; oc += 16)
    {
      int32_t channels = (output_c - oc) > 16 ? 16 : (output_c - oc);

      for (int32_t oh = 0; oh < output_h; oh++)
        {
          for (int32_t ow = 0; ow < output_w; ow++)
            {
              int32x4_t sum0 = vdupq_n_s32(0);
              int32x4_t sum1 = vdupq_n_s32(0);
              int32x4_t sum2 = vdupq_n_s32(0);
              int32x4_t sum3 = vdupq_n_s32(0);

              /* Add bias if present */

              if (bias_data)
                {
                  sum0 = vldrwq_s32(&bias_data[oc]);
                  if (channels > 4)
                    sum1 = vldrwq_s32(&bias_data[oc + 4]);
                  if (channels > 8)
                    sum2 = vldrwq_s32(&bias_data[oc + 8]);
                  if (channels > 12)
                    sum3 = vldrwq_s32(&bias_data[oc + 12]);
                }

              for (int32_t fh = 0; fh < filter_h; fh++)
                {
                  for (int32_t fw = 0; fw < filter_w; fw++)
                    {
                      int32_t ih = oh * stride - padding + fh;
                      int32_t iw = ow * stride - padding + fw;

                      if (ih >= 0 && ih < input_h &&
                          iw >= 0 && iw < input_w)
                        {
                          int32_t input_idx =
                              (ih * input_w + iw) * input_c;

                          /* Load input values and broadcast */

                          for (int32_t ic = 0; ic < input_c; ic++)
                            {
                              int32_t in_val =
                                  (int32_t)input_data[input_idx + ic];

                              int32_t filter_base =
                                  ((oc * filter_h + fh) * filter_w + fw) *
                                  input_c + ic;

                              /* Process 4 output channels at a time */

                              for (int32_t k = 0; k < channels && k < 4;
                                   k++)
                                {
                                  int32_t f_val = (int32_t)
                                      filter_data[filter_base + k * input_c];
                                  sum0[k] += in_val * f_val;
                                }

                              for (int32_t k = 0; k < (channels - 4) && k < 4;
                                   k++)
                                {
                                  int32_t f_val = (int32_t)
                                      filter_data[filter_base +
                                                  (k + 4) * input_c];
                                  sum1[k] += in_val * f_val;
                                }

                              for (int32_t k = 0; k < (channels - 8) && k < 4;
                                   k++)
                                {
                                  int32_t f_val = (int32_t)
                                      filter_data[filter_base +
                                                  (k + 8) * input_c];
                                  sum2[k] += in_val * f_val;
                                }

                              for (int32_t k = 0; k < (channels - 12) && k < 4;
                                   k++)
                                {
                                  int32_t f_val = (int32_t)
                                      filter_data[filter_base +
                                                  (k + 12) * input_c];
                                  sum3[k] += in_val * f_val;
                                }
                            }
                        }
                    }
                }

              /* Apply activation and store results */

              int32_t out_idx = (oh * output_w + ow) * output_c + oc;

              for (int32_t k = 0; k < channels && k < 4; k++)
                {
                  int32_t val = sum0[k] + params->input_offset +
                                params->output_offset;
                  val = val < params->activation_min ?
                        params->activation_min : val;
                  val = val > params->activation_max ?
                        params->activation_max : val;
                  output_data[out_idx + k] = (int8_t)val;
                }

              for (int32_t k = 0; k < (channels - 4) && k < 4; k++)
                {
                  int32_t val = sum1[k] + params->input_offset +
                                params->output_offset;
                  val = val < params->activation_min ?
                        params->activation_min : val;
                  val = val > params->activation_max ?
                        params->activation_max : val;
                  output_data[out_idx + k + 4] = (int8_t)val;
                }

              for (int32_t k = 0; k < (channels - 8) && k < 4; k++)
                {
                  int32_t val = sum2[k] + params->input_offset +
                                params->output_offset;
                  val = val < params->activation_min ?
                        params->activation_min : val;
                  val = val > params->activation_max ?
                        params->activation_max : val;
                  output_data[out_idx + k + 8] = (int8_t)val;
                }

              for (int32_t k = 0; k < (channels - 12) && k < 4; k++)
                {
                  int32_t val = sum3[k] + params->input_offset +
                                params->output_offset;
                  val = val < params->activation_min ?
                        params->activation_min : val;
                  val = val > params->activation_max ?
                        params->activation_max : val;
                  output_data[out_idx + k + 12] = (int8_t)val;
                }
            }
        }
    }
}

#endif /* USE_MVE_INTRINSICS */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int32_t cmsis_nn_convolve_s8(
    FAR const struct cmsis_nn_conv_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const struct cmsis_nn_dims_s *bias_dims,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  if (params == NULL || input_dims == NULL || input_data == NULL ||
      filter_dims == NULL || filter_data == NULL || output_dims == NULL ||
      output_data == NULL)
    {
      return CMSIS_NN_ARG_ERROR;
    }

#if USE_MVE_INTRINSICS && defined(CONFIG_AI_CMSIS_NN_HELUM)
  convolve_s8_mve(params, input_dims, input_data, filter_dims,
                  filter_data, bias_data, output_dims, output_data);
#else
  convolve_s8_scalar(params, input_dims, input_data, filter_dims,
                     filter_data, bias_data, output_dims, output_data);
#endif

  return CMSIS_NN_SUCCESS;
}

int32_t cmsis_nn_depthwise_convolve_s8(
    FAR const struct cmsis_nn_conv_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const struct cmsis_nn_dims_s *bias_dims,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  if (params == NULL || input_dims == NULL || input_data == NULL ||
      filter_dims == NULL || filter_data == NULL || output_dims == NULL ||
      output_data == NULL)
    {
      return CMSIS_NN_ARG_ERROR;
    }

  int32_t input_h = input_dims->h;
  int32_t input_w = input_dims->w;
  int32_t channels = input_dims->c;
  int32_t filter_h = filter_dims->h;
  int32_t filter_w = filter_dims->w;
  int32_t output_h = output_dims->h;
  int32_t output_w = output_dims->w;
  int32_t stride = params->stride;
  int32_t padding = params->padding;

  for (int32_t c = 0; c < channels; c++)
    {
      for (int32_t oh = 0; oh < output_h; oh++)
        {
          for (int32_t ow = 0; ow < output_w; ow++)
            {
              int32_t sum = bias_data ? bias_data[c] : 0;

              for (int32_t fh = 0; fh < filter_h; fh++)
                {
                  for (int32_t fw = 0; fw < filter_w; fw++)
                    {
                      int32_t ih = oh * stride - padding + fh;
                      int32_t iw = ow * stride - padding + fw;

                      if (ih >= 0 && ih < input_h &&
                          iw >= 0 && iw < input_w)
                        {
                          int32_t input_idx =
                              (ih * input_w + iw) * channels + c;
                          int32_t filter_idx =
                              (fh * filter_w + fw) * channels + c;

                          sum += (int32_t)input_data[input_idx] *
                                 (int32_t)filter_data[filter_idx];
                        }
                    }
                }

              sum += params->input_offset;
              if (sum < params->activation_min)
                {
                  sum = params->activation_min;
                }
              if (sum > params->activation_max)
                {
                  sum = params->activation_max;
                }

              output_data[(oh * output_w + ow) * channels + c] =
                  (int8_t)(sum + params->output_offset);
            }
        }
    }

  return CMSIS_NN_SUCCESS;
}

int32_t cmsis_nn_max_pool_s8(
    FAR const struct cmsis_nn_pool_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  if (params == NULL || input_dims == NULL || input_data == NULL ||
      filter_dims == NULL || output_dims == NULL || output_data == NULL)
    {
      return CMSIS_NN_ARG_ERROR;
    }

  max_pool_s8_scalar(params, input_dims, input_data, filter_dims,
                     output_dims, output_data);

  return CMSIS_NN_SUCCESS;
}

int32_t cmsis_nn_avg_pool_s8(
    FAR const struct cmsis_nn_pool_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  if (params == NULL || input_dims == NULL || input_data == NULL ||
      filter_dims == NULL || output_dims == NULL || output_data == NULL)
    {
      return CMSIS_NN_ARG_ERROR;
    }

  int32_t input_h = input_dims->h;
  int32_t input_w = input_dims->w;
  int32_t input_c = input_dims->c;
  int32_t filter_h = filter_dims->h;
  int32_t filter_w = filter_dims->w;
  int32_t output_h = output_dims->h;
  int32_t output_w = output_dims->w;
  int32_t stride = params->stride;
  int32_t padding = params->padding;
  int32_t filter_size = filter_h * filter_w;

  for (int32_t oc = 0; oc < input_c; oc++)
    {
      for (int32_t oh = 0; oh < output_h; oh++)
        {
          for (int32_t ow = 0; ow < output_w; ow++)
            {
              int32_t sum = 0;
              int32_t count = 0;

              for (int32_t fh = 0; fh < filter_h; fh++)
                {
                  for (int32_t fw = 0; fw < filter_w; fw++)
                    {
                      int32_t ih = oh * stride - padding + fh;
                      int32_t iw = ow * stride - padding + fw;

                      if (ih >= 0 && ih < input_h &&
                          iw >= 0 && iw < input_w)
                        {
                          sum += (int32_t)
                              input_data[(ih * input_w + iw) * input_c + oc];
                          count++;
                        }
                    }
                }

              int8_t avg = (int8_t)(sum / count);

              if (avg < params->activation_min)
                {
                  avg = params->activation_min;
                }
              if (avg > params->activation_max)
                {
                  avg = params->activation_max;
                }

              output_data[(oh * output_w + ow) * input_c + oc] = avg;
            }
        }
    }

  return CMSIS_NN_SUCCESS;
}

int32_t cmsis_nn_fully_connected_s8(
    FAR const struct cmsis_nn_fc_params_s *params,
    FAR const struct cmsis_nn_dims_s *input_dims,
    FAR const int8_t *input_data,
    FAR const struct cmsis_nn_dims_s *filter_dims,
    FAR const int8_t *filter_data,
    FAR const struct cmsis_nn_dims_s *bias_dims,
    FAR const int32_t *bias_data,
    FAR const struct cmsis_nn_dims_s *output_dims,
    FAR int8_t *output_data)
{
  if (params == NULL || input_dims == NULL || input_data == NULL ||
      filter_dims == NULL || filter_data == NULL || output_dims == NULL ||
      output_data == NULL)
    {
      return CMSIS_NN_ARG_ERROR;
    }

  fully_connected_s8_scalar(params, input_dims, input_data, filter_dims,
                            filter_data, bias_data, output_dims,
                            output_data);

  return CMSIS_NN_SUCCESS;
}

int32_t cmsis_nn_relu_s8(FAR int8_t *data, uint32_t size)
{
  if (data == NULL)
    {
      return CMSIS_NN_ARG_ERROR;
    }

  for (uint32_t i = 0; i < size; i++)
    {
      if (data[i] < 0)
        {
          data[i] = 0;
        }
    }

  return CMSIS_NN_SUCCESS;
}

int32_t cmsis_nn_softmax_s8(FAR const int8_t *input, uint32_t size,
                            FAR int8_t *output)
{
  if (input == NULL || output == NULL)
    {
      return CMSIS_NN_ARG_ERROR;
    }

  /* Find max for numerical stability */

  int8_t max_val = input[0];
  for (uint32_t i = 1; i < size; i++)
    {
      if (input[i] > max_val)
        {
          max_val = input[i];
        }
    }

  /* Compute exp and sum */

  float sum = 0.0f;
  FAR float *exp_vals = (FAR float *)kmm_zalloc(size * sizeof(float));
  if (exp_vals == NULL)
    {
      return CMSIS_NN_ARG_ERROR;
    }

  for (uint32_t i = 0; i < size; i++)
    {
      exp_vals[i] = expf((float)(input[i] - max_val));
      sum += exp_vals[i];
    }

  /* Normalize and convert back to int8 */

  for (uint32_t i = 0; i < size; i++)
    {
      float normalized = exp_vals[i] / sum;
      output[i] = (int8_t)(normalized * 127.0f);
    }

  kmm_free(exp_vals);
  return CMSIS_NN_SUCCESS;
}
