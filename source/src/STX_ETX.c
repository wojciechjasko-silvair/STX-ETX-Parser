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
static bool STX_ETX_Write(uint8_t * p_out, size_t out_len, size_t * p_index, uint8_t value);


/** @brief Decode CRC byte 0.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [in]      value      Value to be written.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_DecodeCrcByte0(STX_ETX_t * p_instance, uint8_t value);


/** @brief Decode CRC byte 1.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [in]      value      Value to be written.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_DecodeCrcByte1(STX_ETX_t * p_instance, uint8_t value);


/** @brief Decode character.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *  @param [in]      value      Value to be written.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_DecodeInternal(STX_ETX_t * p_instance,
                                               uint8_t *   p_out,
                                               size_t      out_len,
                                               size_t *    p_index,
                                               uint8_t     value);


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
                                            uint8_t *   p_out,
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
                                            uint8_t *   p_out,
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
                                            uint8_t *   p_out,
                                            size_t      out_len,
                                            size_t *    p_index);


/** @brief Decode not special character.
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
                                                  uint8_t *    p_out,
                                                  size_t       out_len,
                                                  size_t *     p_index,
                                                  uint8_t      value);


/** @brief Encode CRC byte 0.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_EncodeCrcByte0(STX_ETX_t * p_instance,
                                               uint8_t *   p_out,
                                               size_t      out_len,
                                               size_t *    p_index);


/** @brief Encode CRC byte 1.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in]      out_len    Output buffer length.
 *  @param [in,out]  p_index    Write index.
 *
 *  @return STX_ETX_Status_t.
 */
static STX_ETX_Status_t STX_ETX_EncodeCrcByte1(STX_ETX_t * p_instance,
                                               uint8_t *   p_out,
                                               size_t      out_len,
                                               size_t *    p_index);


/** @brief Encode special character.
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
                                              uint8_t *   p_out,
                                              size_t      out_len,
                                              size_t *    p_index,
                                              uint8_t     value);


/** @brief Encode not special character.
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
                                                 uint8_t *   p_out,
                                                 size_t      out_len,
                                                 size_t *    p_index,
                                                 uint8_t     value);


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
                                           uint8_t *    p_out,
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
                                          uint8_t *       p_out,
                                          size_t       out_len,
                                          size_t *     p_index);


/** @brief Check if CRC is enable.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *
 *  @return bool  True, when CRC is enable.
 */
static bool STX_ETX_IsCRCEnable(STX_ETX_t * p_instance);


/** @brief Initialize CRC.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *
 *  @return void.
 */
static void STX_ETX_InitCRC(STX_ETX_t * p_instance);


/** @brief Update CRC.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [in]      value      New value.
 *
 *  @return void.
 */
static void STX_ETX_UpdateCRC(STX_ETX_t * p_instance, uint8_t value);

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

STX_ETX_Status_t STX_ETX_Decode(STX_ETX_t *     p_instance,
                                uint8_t const * p_in,
                                size_t *        p_in_len,
                                uint8_t *       p_out,
                                size_t *        p_out_len)
{
  STX_ETX_Status_t status    = STX_ETX_STATUS_CONTINUE;
  size_t           in_index  = 0;
  size_t           out_index = 0;

  while ((in_index < *p_in_len) && (STX_ETX_STATUS_CONTINUE == status))
  {
    uint8_t value = p_in[in_index];

    switch (p_instance->state)
    {
      case STX_ETX_STATE_CRC_BYTE_0:
        status = STX_ETX_DecodeCrcByte0(p_instance, value);
        break;

      case STX_ETX_STATE_CRC_BYTE_1:
        status = STX_ETX_DecodeCrcByte1(p_instance, value);
        break;

      default:
        status = STX_ETX_DecodeInternal(p_instance, p_out, *p_out_len, &out_index, value);
        break;
    }

    if (STX_ETX_STATUS_OVERFLOW != status)
    {
      in_index++;
    }
  }

  if (STX_ETX_IsError(status) || (STX_ETX_STATUS_DONE == status))
  {
    STX_ETX_Reset(p_instance);
  }

  *p_in_len  = in_index;
  *p_out_len = out_index;
  return status;
}


STX_ETX_Status_t STX_ETX_Encode(STX_ETX_t *     p_instance,
                                uint8_t const * p_in,
                                size_t *        p_in_len,
                                uint8_t *       p_out,
                                size_t *        p_out_len)
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
    uint8_t value = p_in[in_index];

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

  if ((p_instance->state == STX_ETX_STATE_STARTED) && (*p_in_len == in_index))
  {
    status = STX_ETX_EncodeEnd(p_instance, p_out, *p_out_len, &out_index);
  }

  if (p_instance->state == STX_ETX_STATE_CRC_BYTE_0)
  {
    status = STX_ETX_EncodeCrcByte0(p_instance, p_out, *p_out_len, &out_index);
  }

  if (p_instance->state == STX_ETX_STATE_CRC_BYTE_1)
  {
    status = STX_ETX_EncodeCrcByte1(p_instance, p_out, *p_out_len, &out_index);
  }

  *p_in_len  = in_index;
  *p_out_len = out_index;
  return status;
}

/********************************************
 * LOCAL FUNCTION DEFINITIONS               *
 *******************************************/

static bool STX_ETX_Write(uint8_t * p_out, size_t out_len, size_t * p_index, uint8_t value)
{
  if (*p_index < out_len)
  {
    p_out[(*p_index)++] = value;
    return true;
  }
  return false;
}

static STX_ETX_Status_t STX_ETX_DecodeCrcByte0(STX_ETX_t * p_instance, uint8_t value)
{
  p_instance->crc16 = value;
  p_instance->state = STX_ETX_STATE_CRC_BYTE_1;

  return STX_ETX_STATUS_CONTINUE;
}

static STX_ETX_Status_t STX_ETX_DecodeCrcByte1(STX_ETX_t * p_instance, uint8_t value)
{
  p_instance->crc16 |= (uint16_t)value << 8;
  p_instance->state = STX_ETX_STATE_IDLE;

  if (p_instance->crc16 != p_instance->computed_crc16)
  {
    return STX_ETX_STATUS_INV_CRC;
  }

  return STX_ETX_STATUS_DONE;
}

static STX_ETX_Status_t STX_ETX_DecodeInternal(STX_ETX_t * p_instance,
                                               uint8_t *   p_out,
                                               size_t      out_len,
                                               size_t *    p_index,
                                               uint8_t     value)
{
  STX_ETX_Status_t status;

  switch (value)
  {
    case STX:
      status = STX_ETX_DecodeOnSTX(p_instance, p_out, out_len, p_index);
      break;

    case ETX:
      status = STX_ETX_DecodeOnETX(p_instance, p_out, out_len, p_index);
      break;

    case DLE:
      status = STX_ETX_DecodeOnDLE(p_instance, p_out, out_len, p_index);
      break;

    default:
      status = STX_ETX_DecodeOnNotSpecial(p_instance, p_out, out_len, p_index, value);
      break;
  }

  if (STX_ETX_STATUS_OVERFLOW != status)
  {
    STX_ETX_UpdateCRC(p_instance, value);
  }
  return status;
}

static STX_ETX_Status_t STX_ETX_DecodeOnSTX(STX_ETX_t * p_instance,
                                            uint8_t *   p_out,
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
                                            uint8_t *   p_out,
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

  if (STX_ETX_IsCRCEnable(p_instance))
  {
    p_instance->state = STX_ETX_STATE_CRC_BYTE_0;
    return STX_ETX_STATUS_CONTINUE;
  }

  p_instance->state = STX_ETX_STATE_IDLE;
  return STX_ETX_STATUS_DONE;
}

static STX_ETX_Status_t STX_ETX_DecodeOnDLE(STX_ETX_t * p_instance,
                                            uint8_t *   p_out,
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
                                                  uint8_t *    p_out,
                                                  size_t       out_len,
                                                  size_t *     p_index,
                                                  uint8_t      value)
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

static STX_ETX_Status_t STX_ETX_EncodeCrcByte0(STX_ETX_t * p_instance,
                                               uint8_t *   p_out,
                                               size_t      out_len,
                                               size_t *    p_index)
{
  uint8_t value = (p_instance->computed_crc16 & UINT8_MAX);

  if (!STX_ETX_Write(p_out, out_len, p_index, value))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  p_instance->state = STX_ETX_STATE_CRC_BYTE_1;
  return STX_ETX_STATUS_CONTINUE;
}

static STX_ETX_Status_t STX_ETX_EncodeCrcByte1(STX_ETX_t * p_instance,
                                               uint8_t *   p_out,
                                               size_t      out_len,
                                               size_t *    p_index)
{
  uint8_t value = (p_instance->computed_crc16 >> 8);

  if (!STX_ETX_Write(p_out, out_len, p_index, value))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  p_instance->state = STX_ETX_STATE_IDLE;
  return STX_ETX_STATUS_DONE;
}

static STX_ETX_Status_t STX_ETX_EncodeSpecial(STX_ETX_t * p_instance,
                                              uint8_t *   p_out,
                                              size_t      out_len,
                                              size_t *    p_index,
                                              uint8_t     value)
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
                                                 uint8_t *   p_out,
                                                 size_t      out_len,
                                                 size_t *    p_index,
                                                 uint8_t     value)
{
  if (!STX_ETX_Write(p_out, out_len, p_index, value))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  STX_ETX_UpdateCRC(p_instance, value);
  return STX_ETX_STATUS_CONTINUE;
}

static STX_ETX_Status_t STX_ETX_EncodeStart(STX_ETX_t * p_instance,
                                           uint8_t *    p_out,
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
                                          uint8_t *   p_out,
                                          size_t      out_len,
                                          size_t *    p_index)
{
  if (!STX_ETX_Write(p_out, out_len, p_index, ETX))
  {
    return STX_ETX_STATUS_OVERFLOW;
  }

  STX_ETX_UpdateCRC(p_instance, ETX);

  if (STX_ETX_IsCRCEnable(p_instance))
  {
    p_instance->state = STX_ETX_STATE_CRC_BYTE_0;
    return STX_ETX_STATUS_CONTINUE;
  }

  p_instance->state = STX_ETX_STATE_IDLE;
  return STX_ETX_STATUS_DONE;
}

static bool STX_ETX_IsCRCEnable(STX_ETX_t * p_instance)
{
  STX_ETX_Config_t const * p_config = p_instance->p_config;

  return (NULL != p_config->update_crc16);
}

static void STX_ETX_InitCRC(STX_ETX_t * p_instance)
{
  STX_ETX_Config_t const * p_config = p_instance->p_config;

  if (STX_ETX_IsCRCEnable(p_instance))
  {
    p_instance->computed_crc16 = p_config->initial_crc16;
  }
}

static void STX_ETX_UpdateCRC(STX_ETX_t * p_instance, uint8_t value)
{
  STX_ETX_Config_t const * p_config = p_instance->p_config;

  if (STX_ETX_IsCRCEnable(p_instance))
  {
    p_instance->computed_crc16 = p_config->update_crc16(p_instance->computed_crc16, value);
  }
}
