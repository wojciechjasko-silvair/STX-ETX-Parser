#ifndef STX_ETX_H
#define STX_ETX_H

/**
 *  @file STX_ETX.h
 *  @brief Header file for STX-ETX Parser
 *
 *         This file contains API of STX-ETX Parser
 *
 *  @author Wojciech Jasko
 */

/********************************************
 * INCLUDES                                 *
 ********************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/********************************************
 * EXPORTED TYPES DEFINITIONS               *
 ********************************************/

/** @brief STX ETX Status. */
typedef enum
{
  STX_ETX_STATUS_DONE,              /**< Conversion finished. */
  STX_ETX_STATUS_CONTINUE,          /**< Conversion not finished. Waiting for input. */
  STX_ETX_STATUS_OVERFLOW,          /**< Conversion not finished. Output overflow. */
  STX_ETX_STATUS_ERR_BASE = 0xF0,
  STX_ETX_STATUS_INV_CHAR,          /**< Invalid uint8_tacter. */
  STX_ETX_STATUS_INV_CRC,           /**< Invalid CRC. */
  STX_ETX_STATUS_ERR_LAST,
} STX_ETX_Status_t;


/** @brief STX ETX States. */
typedef enum
{
  STX_ETX_STATE_IDLE,         /**< Parser is ready. */
  STX_ETX_STATE_STARTED,      /**< Conversion is started. */
  STX_ETX_STATE_DLE_LATCHED,  /**< Conversion is started. Special character is expected. */
  STX_ETX_STATE_CRC_BYTE_0,   /**< Conversion is started. CRC byte 0. */
  STX_ETX_STATE_CRC_BYTE_1,   /**< Conversion is started. CRC byte 1. */
} STX_ETX_State_t;


/** @brief STX ETX Config. */
typedef struct
{
  uint16_t initial_crc16; //!< Initial value.

  /** @brief  Update crc16.
   *
   *  @note NULL if CRC is not used.
   *
   *  @param  crc16     Previous value of crc.
   *  @param  value     New byte.
   *
   *  @return uint16_t Updated CRC.
   **/
  uint16_t (*update_crc16)(uint16_t crc16, uint8_t value);
} STX_ETX_Config_t;


/** @brief STX ETX Instance. */
typedef struct
{
  STX_ETX_State_t                state;           //!< State.
  uint16_t                       computed_crc16;  //!< Computed CRC.
  uint16_t                       crc16;           //!< Decoded CRC.
  STX_ETX_Config_t const *       p_config;        //!< Pointer to configuration.
} STX_ETX_t;

/********************************************
 * EXPORTED #define CONSTANTS AND MACROS    *
 ********************************************/

#define STX   0x02  /** Start of text uint8_tacter. */
#define ETX   0x03  /** End of text uint8_tacter. */
#define DLE   0x10  /** Data link escape uint8_tacter. */

/********************************************
 * EXPORTED FUNCTIONS PROTOTYPES            *
 ********************************************/

/** @brief Initialize STX-ETX Parser.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [in]      p_config   Pointer to parser configuration.
 *
 *  @return void.
 */
void STX_ETX_Init(STX_ETX_t *              p_instance,
                  STX_ETX_Config_t const * p_config);


/** @brief Reset STX-ETX Parser.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *
 *  @return void.
 */
void STX_ETX_Reset(STX_ETX_t * p_instance);


/** @brief Check if the status indicates an error.
 *
 *  @param [in]      status   Status.
 *
 *  @return bool  True, if it is error.
 */
bool STX_ETX_IsError(STX_ETX_Status_t status);


/** @brief Decode STX-ETX data.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [in]      p_in       Pointer to input buffer.
 *  @param [in,out]  p_in_len   in:  Input buffer length.
 *                              out: Number of bytes read from input buffer.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in,out]  p_out_len  in:  Output buffer length.
 *                              out: Number of bytes read from output buffer.
 *
 *  @return STX_ETX_Status_t.
 */
STX_ETX_Status_t STX_ETX_Decode(STX_ETX_t *     p_instance,
                                uint8_t const * p_in,
                                size_t *        p_in_len,
                                uint8_t *       p_out,
                                size_t *        p_out_len);


/** @brief Encode STX-ETX data.
 *
 *  @param [in]      p_instance Pointer to parser instance.
 *  @param [in]      p_in       Pointer to input buffer.
 *  @param [in,out]  p_in_len   in:  Input buffer length.
 *                              out: Number of bytes read from input buffer.
 *  @param [out]     p_out      Pointer to output buffer.
 *  @param [in,out]  p_out_len  in:  Output buffer length.
 *                              out: Number of bytes read from output buffer.
 *
 *  @return STX_ETX_Status_t.
 */
STX_ETX_Status_t STX_ETX_Encode(STX_ETX_t *     p_instance,
                                uint8_t const * p_in,
                                size_t *        p_in_len,
                                uint8_t *       p_out,
                                size_t *        p_out_len);

#endif /* #ifndef STX_ETX_H */
