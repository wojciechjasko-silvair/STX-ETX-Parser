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
  STX_ETX_STATUS_INV_CHAR,          /**< Invalid character. */
  STX_ETX_STATUS_INV_CRC,           /**< Invalid CRC. */
  STX_ETX_STATUS_ERR_LAST,
} STX_ETX_Status_t;


/** @brief STX ETX Config. */
typedef struct
{
  /** @brief  Called to initialize crc16 value.
   *
   *  @note NULL if CRC is not used.
   *
   *  @param  p_crc     Pointer to crc16 value.
   *
   *  @return void.
   **/
  void (*crc16_init)(uint16_t * p_crc);

  /** @brief  Update crc16.
   *
   *  @note NULL if CRC is not used.
   *
   *  @param  p_crc     Pointer to crc16 value.
   *  @param  value     New byte.
   *
   *  @return void.
   **/
  void (*crc16_update)(uint16_t * p_crc, char value);
} STX_ETX_Config_t;


/** @brief STX ETX Instance. */
typedef struct
{
  bool                           is_started;      //!< True, if conversion is started.
  bool                           is_dle_latched;  //!< True, if dle is latched.
  uint16_t                       crc16;           //!< CRC.
  STX_ETX_Config_t const *       p_config;        //!< Pointer to configuration.
} STX_ETX_t;

/********************************************
 * EXPORTED #define CONSTANTS AND MACROS    *
 ********************************************/

#define STX   0x02  /** Start of text character. */
#define ETX   0x03  /** End of text character. */
#define DLE   0x10  /** Data link escape character. */

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
STX_ETX_Status_t STX_ETX_Decode(STX_ETX_t *  p_instance,
                                char const * p_in,
                                size_t *     p_in_len,
                                char *       p_out,
                                size_t *     p_out_len);


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
STX_ETX_Status_t STX_ETX_Encode(STX_ETX_t *  p_instance,
                                char const * p_in,
                                size_t *     p_in_len,
                                char *       p_out,
                                size_t *     p_out_len);

#endif /* #ifndef STX_ETX_H */
