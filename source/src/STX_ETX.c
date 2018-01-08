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

static bool STX_ETX_IsCRCEnable(STX_ETX_t * p_instance)
{
  STX_ETX_Config_t const * p_config = p_instance->p_config;

  return (NULL != p_config->init_crc16) && (NULL != p_config->update_crc16);
}

static void STX_ETX_InitCRC(STX_ETX_t * p_instance)
{
  STX_ETX_Config_t const * p_config = p_instance->p_config;

  if (STX_ETX_IsCRCEnable(p_instance))
  {
    p_config->init_crc16(&(p_instance->crc16));
  }
}

static void STX_ETX_UpdateCRC(STX_ETX_t * p_instance, char value)
{
  STX_ETX_Config_t const * p_config = p_instance->p_config;

  if (STX_ETX_IsCRCEnable(p_instance))
  {
    p_config->update_crc16(&p_instance->crc16, value);
  }
}

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
  p_instance->state = STX_ETX_STATE_IDLE;
  STX_ETX_InitCRC(p_instance);
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
    char value = p_in[in_index];

    switch (value)
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
        status = STX_ETX_DecodeOnNotSpecial(p_instance, p_out, *p_out_len, &out_index, value);
        break;
    }

    if (STX_ETX_STATUS_OVERFLOW != status)
    {
      STX_ETX_UpdateCRC(p_instance, value);
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

  if (p_instance->state == STX_ETX_STATE_IDLE)
  {
    status = STX_ETX_EncodeStart(p_instance, p_out, *p_out_len, &out_index);
  }

  while ((in_index < *p_in_len) && (STX_ETX_STATUS_CONTINUE == status))
  {
    char value = p_in[in_index];

    if ((STX == value)
     || (ETX == value)
     || (DLE == value))
    {
      status = STX_ETX_EncodeSpecial(p_instance, p_out, *p_out_len, &out_index, value);
    }
    else
    {
      status = STX_ETX_EncodeNotSpecial(p_instance, p_out, *p_out_len, &out_index, value);
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
  if (p_instance->state == STX_ETX_STATE_IDLE)
  {
    p_instance->state = STX_ETX_STATE_STARTED;
    return STX_ETX_STATUS_CONTINUE;
  }

  if (p_instance->state == STX_ETX_STATE_DLE_LATCHED)
  {
    if (!STX_ETX_Write(p_out, out_len, p_index, STX))
    {
      return STX_ETX_STATUS_OVERFLOW;
    }

    p_instance->state = STX_ETX_STATE_STARTED;
    return STX_ETX_STATUS_CONTINUE;
  }

  return STX_ETX_STATUS_INV_CHAR;
}


static STX_ETX_Status_t STX_ETX_DecodeOnETX(STX_ETX_t * p_instance,
                                            char *      p_out,
                                            size_t      out_len,
                                            size_t *    p_index)
{
  if (p_instance->state == STX_ETX_STATE_IDLE)
  {
    return STX_ETX_STATUS_INV_CHAR;
  }

  if (p_instance->state == STX_ETX_STATE_DLE_LATCHED)
  {
    if (!STX_ETX_Write(p_out, out_len, p_index, ETX))
    {
      return STX_ETX_STATUS_OVERFLOW;
    }

    p_instance->state = STX_ETX_STATE_STARTED;
    return STX_ETX_STATUS_CONTINUE;
  }

    p_instance->state = STX_ETX_STATE_IDLE;
  return STX_ETX_STATUS_DONE;
}


static STX_ETX_Status_t STX_ETX_DecodeOnDLE(STX_ETX_t * p_instance,
                                            char *      p_out,
                                            size_t      out_len,
                                            size_t *    p_index)
{
  if (p_instance->state == STX_ETX_STATE_IDLE)
  {
    return STX_ETX_STATUS_INV_CHAR;
  }

  if (p_instance->state == STX_ETX_STATE_DLE_LATCHED)
  {
    if (!STX_ETX_Write(p_out, out_len, p_index, DLE))
    {
      return STX_ETX_STATUS_OVERFLOW;
    }

    p_instance->state = STX_ETX_STATE_STARTED;
    return STX_ETX_STATUS_CONTINUE;
  }

  p_instance->state = STX_ETX_STATE_DLE_LATCHED;
  return STX_ETX_STATUS_CONTINUE;

}


static STX_ETX_Status_t STX_ETX_DecodeOnNotSpecial(STX_ETX_t * p_instance,
                                                  char *       p_out,
                                                  size_t       out_len,
                                                  size_t *     p_index,
                                                  char         value)
{
  if (p_instance->state == STX_ETX_STATE_IDLE)
  {
    return STX_ETX_STATUS_INV_CHAR;
  }

  if (p_instance->state == STX_ETX_STATE_DLE_LATCHED)
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
  if (p_instance->state == STX_ETX_STATE_STARTED)
  {
    if (!STX_ETX_Write(p_out, out_len, p_index, DLE))
    {
      return STX_ETX_STATUS_OVERFLOW;
    }

    STX_ETX_UpdateCRC(p_instance, DLE);
  p_instance->state = STX_ETX_STATE_DLE_LATCHED;
  }

  if (!STX_ETX_Write(p_out, out_len, p_index, value))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  STX_ETX_UpdateCRC(p_instance, value);
  p_instance->state = STX_ETX_STATE_STARTED;
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

  STX_ETX_UpdateCRC(p_instance, value);
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

  STX_ETX_UpdateCRC(p_instance, STX);
  p_instance->state = STX_ETX_STATE_STARTED;
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

  STX_ETX_UpdateCRC(p_instance, ETX);
  p_instance->state = STX_ETX_STATE_IDLE;
  return STX_ETX_STATUS_DONE;
}
