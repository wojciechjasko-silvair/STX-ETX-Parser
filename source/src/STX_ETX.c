/********************************************
 * INCLUDES                                 *
 ********************************************/

#include "STX_ETX.h"

/********************************************
 * EXPORTED FUNCTION DEFINITIONS            *
 ********************************************/

void STX_ETX_Init(STX_ETX_t *                 p_instance,
                  STX_ETX_Config_t const *    p_config,
                  STX_ETX_Interface_t const * p_interface)
{
  p_instance->p_config    = p_config;
  p_instance->p_interface = p_interface;

  STX_ETX_Reset(p_instance);
}

void STX_ETX_Reset(STX_ETX_t * p_instance)
{
  STX_ETX_Config_t const *    p_config    = p_instance->p_config;
  STX_ETX_Interface_t const * p_interface = p_instance->p_interface;

  p_instance->is_started     = false;
  p_instance->is_dle_latched = false;
  if (p_config->include_crc)
  {
    p_interface->crc16_init(&(p_instance->crc16));
  }
}

static STX_ETX_Status_t STX_ETX_DecodeOnSTX(STX_ETX_t * p_instance, char * p_out, size_t * p_index)
{
  if (!p_instance->is_started)
  {
    p_instance->is_started = true;
    return STX_ETX_STATUS_CONTINUE;
  }
  else if (p_instance->is_dle_latched)
  {
    p_out[(*p_index)++]        = STX;
    p_instance->is_dle_latched = false;
    return STX_ETX_STATUS_CONTINUE;
  }
  else
  {
    STX_ETX_Reset(p_instance);
    return STX_ETX_STATUS_INV_CHAR;
  }
}

static STX_ETX_Status_t STX_ETX_DecodeOnETX(STX_ETX_t * p_instance, char * p_out, size_t * p_index)
{
  if (!p_instance->is_started)
  {
    STX_ETX_Reset(p_instance);
    return STX_ETX_STATUS_INV_CHAR;
  }
  else if (p_instance->is_dle_latched)
  {
    p_out[(*p_index)++]        = ETX;
    p_instance->is_dle_latched = false;
    return STX_ETX_STATUS_CONTINUE;
  }
  else
  {
    p_instance->is_started = false;
    return STX_ETX_STATUS_DONE;
  }
}

static STX_ETX_Status_t STX_ETX_DecodeOnDLE(STX_ETX_t * p_instance, char * p_out, size_t * p_index)
{
  if (!p_instance->is_started)
  {
    STX_ETX_Reset(p_instance);
    return STX_ETX_STATUS_INV_CHAR;
  }
  else if (p_instance->is_dle_latched)
  {
    p_out[(*p_index)++]        = DLE;
    p_instance->is_dle_latched = false;
    return STX_ETX_STATUS_CONTINUE;
  }
  else
  {
    p_instance->is_dle_latched = true;
    return STX_ETX_STATUS_CONTINUE;
  }
}

static STX_ETX_Status_t STX_ETX_DecodeOnNotSpecial(STX_ETX_t * p_instance, char * p_out, size_t * p_index, char value)
{
  if (!p_instance->is_started)
  {
    STX_ETX_Reset(p_instance);
    return STX_ETX_STATUS_INV_CHAR;
  }
  else if (p_instance->is_dle_latched)
  {
    STX_ETX_Reset(p_instance);
    return STX_ETX_STATUS_INV_CHAR;
  }
  else
  {
    p_out[(*p_index)++] = value;
    return STX_ETX_STATUS_CONTINUE;
  }
}

STX_ETX_Status_t STX_ETX_Decode(STX_ETX_t *  p_instance,
                                char const * p_in,
                                size_t *     p_in_len,
                                char *       p_out,
                                size_t *     p_out_len)
{
  STX_ETX_Status_t status;
  size_t           in_index  = 0;
  size_t           out_index = 0;

  while ((in_index < *p_in_len) && (out_index < *p_out_len))
  {
    if (STX == p_in[in_index])
    {
      status = STX_ETX_DecodeOnSTX(p_instance, p_out, &out_index);
    }
    else if (ETX == p_in[in_index])
    {
      status = STX_ETX_DecodeOnETX(p_instance, p_out, &out_index);
    }
    else if (DLE == p_in[in_index])
    {
      status = STX_ETX_DecodeOnDLE(p_instance, p_out, &out_index);
    }
    else
    {
      status = STX_ETX_DecodeOnNotSpecial(p_instance, p_out, &out_index, p_in[in_index]);
    }

    in_index++;
    if (STX_ETX_STATUS_CONTINUE != status) break;
  }

  *p_in_len  = in_index;
  *p_out_len = out_index;
  return status;
}

STX_ETX_Status_t STX_ETX_Encode(STX_ETX_t *  p_instance,
                                char const * p_in,
                                size_t *     p_in_len,
                                char *       p_out,
                                size_t *     p_out_len)
{
  STX_ETX_Status_t status    = STX_ETX_STATUS_CONTINUE;
  size_t           in_index  = 0;
  size_t           out_index = 0;

  if (!p_instance->is_started)
  {
    p_out[out_index++]     = STX;
    p_instance->is_started = true;
  }

  while ((in_index < *p_in_len) && (out_index < *p_out_len))
  {
    if ((STX == p_in[in_index])
     || (ETX == p_in[in_index])
     || (DLE == p_in[in_index]))
    {
      p_out[out_index++] = DLE;
      p_out[out_index++] = p_in[in_index];
    }
    else
    {
      p_out[out_index++] = p_in[in_index];
    }

    in_index++;
  }

  if (*p_in_len == in_index)
  {
    p_out[out_index++]     = ETX;
    p_instance->is_started = false;
    status                 = STX_ETX_STATUS_DONE;
  }

  *p_in_len  = in_index;
  *p_out_len = out_index;
  return status;
}
