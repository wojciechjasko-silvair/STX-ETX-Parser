/********************************************
 * INCLUDES                                 *
 ********************************************/

#include "STX_ETX.h"

/********************************************
 * LOCAL FUNCTIONS PROTOTYPES               *
 ********************************************/

/** @brief Write byte to output buffer.
 *
 *  @param [in]      p_out    Pointer to output buffer.
 *  @param [in]      out_len  Output buffer length.
 *  @param [in,out]  p_index  Write index.
 *  @param [in]      value    Value to be written.
 *
 *  @return bool True, if succeeded.
 */
static bool STX_ETX_Write(char * p_out, size_t out_len, size_t * p_index, char value);


/** @brief Decode STX character.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_DecodeOnSTX(STX_ETX_t * p_instance,
                                            char *      p_out,
                                            size_t      out_len,
                                            size_t *    p_index);


/** @brief Decode ETX character.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_DecodeOnETX(STX_ETX_t * p_instance,
                                            char *      p_out,
                                            size_t      out_len,
                                            size_t *    p_index);

/** @brief Decode DLE character.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_DecodeOnDLE(STX_ETX_t * p_instance,
                                            char *      p_out,
                                            size_t      out_len,
                                            size_t *    p_index);


/** @brief Decode not spectial character.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *  @param [in]      value      Value to be written.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_DecodeOnNotSpecial(STX_ETX_t * p_instance,
                                                  char *       p_out,
                                                  size_t       out_len,
                                                  size_t *     p_index,
                                                  char         value);


/** @brief Decode spectial character.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *  @param [in]      value      Value to be written.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_EncodeSpecial(STX_ETX_t * p_instance,
                                              char *      p_out,
                                              size_t      out_len,
                                              size_t *    p_index,
                                              char        value);


/** @brief Decode not spectial character.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *  @param [in]      value      Value to be written.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_EncodeNotSpecial(STX_ETX_t * p_instance,
                                                 char *      p_out,
                                                 size_t      out_len,
                                                 size_t *    p_index,
                                                 char        value);


/** @brief Start encoding.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_EncodeStart(STX_ETX_t * p_instance,
                                           char *       p_out,
                                           size_t       out_len,
                                           size_t *     p_index);


/** @brief End encoding.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_EncodeEnd(STX_ETX_t * p_instance,
                                          char *       p_out,
                                          size_t       out_len,
                                          size_t *     p_index);

/********************************************
 * EXPORTED FUNCTION DEFINITIONS            *
 ********************************************/

void STX_ETX_Init(STX_ETX_t *                 p_instance,
                  STX_ETX_Config_t const *    p_config)
{
  p_instance->p_config = p_config;

  STX_ETX_Reset(p_instance);
}


void STX_ETX_Reset(STX_ETX_t * p_instance)
{
  STX_ETX_Config_t const * p_config = p_instance->p_config;

  p_instance->is_started     = false;
  p_instance->is_dle_latched = false;
  if (p_config->crc16_init)
  {
    p_config->crc16_init(&(p_instance->crc16));
  }
}


bool STX_ETX_IsError(STX_ETX_Status_t status)
{
  return status >= STX_ETX_STATUS_ERR_BASE;
}


STX_ETX_Status_t STX_ETX_Decode(STX_ETX_t *  p_instance,
                                char const * p_in,
                                size_t *     p_in_len,
                                char *       p_out,
                                size_t *     p_out_len)
{
  STX_ETX_Status_t status    = STX_ETX_STATUS_CONTINUE;
  size_t           in_index  = 0;
  size_t           out_index = 0;

  while ((in_index < *p_in_len) && (STX_ETX_STATUS_CONTINUE == status))
  {
    switch (p_in[in_index])
    {
      case STX:
        status = STX_ETX_DecodeOnSTX(p_instance, p_out, *p_out_len, &out_index);
        break;

      case ETX:
        status = STX_ETX_DecodeOnETX(p_instance, p_out, *p_out_len, &out_index);
        break;

      case DLE:
        status = STX_ETX_DecodeOnDLE(p_instance, p_out, *p_out_len, &out_index);
        break;

      default:
        status = STX_ETX_DecodeOnNotSpecial(p_instance, p_out, *p_out_len, &out_index, p_in[in_index]);
        break;
    }

    if (STX_ETX_STATUS_OVERFLOW != status)
    {
      in_index++;
    }
  }

  if (STX_ETX_IsError(status))
  {
    STX_ETX_Reset(p_instance);
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
    status = STX_ETX_EncodeStart(p_instance, p_out, *p_out_len, &out_index);
  }

  while ((in_index < *p_in_len) && (STX_ETX_STATUS_CONTINUE == status))
  {
    if ((STX == p_in[in_index])
     || (ETX == p_in[in_index])
     || (DLE == p_in[in_index]))
    {
      status = STX_ETX_EncodeSpecial(p_instance, p_out, *p_out_len, &out_index, p_in[in_index]);
    }
    else
    {
      status = STX_ETX_EncodeNotSpecial(p_instance, p_out, *p_out_len, &out_index, p_in[in_index]);
    }

    if (STX_ETX_STATUS_OVERFLOW != status)
    {
      in_index++;
    }
  }

  if (*p_in_len == in_index)
  {
    status = STX_ETX_EncodeEnd(p_instance, p_out, *p_out_len, &out_index);
  }

  *p_in_len  = in_index;
  *p_out_len = out_index;
  return status;
}

/********************************************
 * LOCAL FUNCTION DEFINITIONS               *
 *******************************************/

static bool STX_ETX_Write(char * p_out, size_t out_len, size_t * p_index, char value)
{
  if (*p_index < out_len)
  {
    p_out[(*p_index)++] = value;
    return true;
  }
  return false;
}


static STX_ETX_Status_t STX_ETX_DecodeOnSTX(STX_ETX_t * p_instance,
                                            char *      p_out,
                                            size_t      out_len,
                                            size_t *    p_index)
{
  if (!p_instance->is_started)
  {
    p_instance->is_started = true;
    return STX_ETX_STATUS_CONTINUE;
  }

  if (p_instance->is_dle_latched)
  {
    if (!STX_ETX_Write(p_out, out_len, p_index, STX))
    {
      return STX_ETX_STATUS_OVERFLOW;
    }

    p_instance->is_dle_latched = false;
    return STX_ETX_STATUS_CONTINUE;
  }

  return STX_ETX_STATUS_INV_CHAR;
}


static STX_ETX_Status_t STX_ETX_DecodeOnETX(STX_ETX_t * p_instance,
                                            char *      p_out,
                                            size_t      out_len,
                                            size_t *    p_index)
{
  if (!p_instance->is_started)
  {
    return STX_ETX_STATUS_INV_CHAR;
  }

  if (p_instance->is_dle_latched)
  {
    if (!STX_ETX_Write(p_out, out_len, p_index, ETX))
    {
      return STX_ETX_STATUS_OVERFLOW;
    }

    p_instance->is_dle_latched = false;
    return STX_ETX_STATUS_CONTINUE;
  }

  p_instance->is_started = false;
  return STX_ETX_STATUS_DONE;
}


static STX_ETX_Status_t STX_ETX_DecodeOnDLE(STX_ETX_t * p_instance,
                                            char *      p_out,
                                            size_t      out_len,
                                            size_t *    p_index)
{
  if (!p_instance->is_started)
  {
    return STX_ETX_STATUS_INV_CHAR;
  }
  
  if (p_instance->is_dle_latched)
  {
    if (!STX_ETX_Write(p_out, out_len, p_index, DLE))
    {
      return STX_ETX_STATUS_OVERFLOW;
    }

    p_instance->is_dle_latched = false;
    return STX_ETX_STATUS_CONTINUE;
  }

  p_instance->is_dle_latched = true;
  return STX_ETX_STATUS_CONTINUE;

}


static STX_ETX_Status_t STX_ETX_DecodeOnNotSpecial(STX_ETX_t * p_instance,
                                                  char *       p_out,
                                                  size_t       out_len,
                                                  size_t *     p_index,
                                                  char         value)
{
  if (!p_instance->is_started)
  {
    return STX_ETX_STATUS_INV_CHAR;
  }

  if (p_instance->is_dle_latched)
  {
    return STX_ETX_STATUS_INV_CHAR;
  }

  if (!STX_ETX_Write(p_out, out_len, p_index, value))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  return STX_ETX_STATUS_CONTINUE;
}


static STX_ETX_Status_t STX_ETX_EncodeSpecial(STX_ETX_t * p_instance,
                                              char *      p_out,
                                              size_t      out_len,
                                              size_t *    p_index,
                                              char        value)
{
  if (!p_instance->is_dle_latched)
  {
    if (!STX_ETX_Write(p_out, out_len, p_index, DLE))
    {
      return STX_ETX_STATUS_OVERFLOW;
    }

    p_instance->is_dle_latched = true;
  }

  if (!STX_ETX_Write(p_out, out_len, p_index, value))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  p_instance->is_dle_latched = false;
  return STX_ETX_STATUS_CONTINUE;
}


static STX_ETX_Status_t STX_ETX_EncodeNotSpecial(STX_ETX_t * p_instance,
                                                 char *      p_out,
                                                 size_t      out_len,
                                                 size_t *    p_index,
                                                 char        value)
{
  if (!STX_ETX_Write(p_out, out_len, p_index, value))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  return STX_ETX_STATUS_CONTINUE;
}


static STX_ETX_Status_t STX_ETX_EncodeStart(STX_ETX_t * p_instance,
                                           char *       p_out,
                                           size_t       out_len,
                                           size_t *     p_index)
{
  if (!STX_ETX_Write(p_out, out_len, p_index, STX))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  p_instance->is_started = true;
  return STX_ETX_STATUS_CONTINUE;
}


static STX_ETX_Status_t STX_ETX_EncodeEnd(STX_ETX_t * p_instance,
                                          char *       p_out,
                                          size_t       out_len,
                                          size_t *     p_index)
{
  if (!STX_ETX_Write(p_out, out_len, p_index, ETX))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  p_instance->is_started = false;
  return STX_ETX_STATUS_DONE;
}
