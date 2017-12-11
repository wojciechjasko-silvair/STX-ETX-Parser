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

typedef enum
{
  STX_ETX_STATUS_DONE,
  STX_ETX_STATUS_CONTINUE,
  STX_ETX_STATUS_ERR_BASE = 0xF0,
  STX_ETX_STATUS_INV_CHAR,
  STX_ETX_STATUS_INV_CRC,
} STX_ETX_Status_t;

typedef struct
{
  bool include_crc;
} STX_ETX_Config_t;

typedef struct
{
  void (*crc16_init)(uint16_t * p_crc);
  void (*crc16_update)(uint16_t * p_crc, char value);
} STX_ETX_Interface_t;

typedef struct
{
  bool                        is_started;
  bool                        is_dle_latched;
  uint16_t                    crc16;
  STX_ETX_Config_t    const * p_config;
  STX_ETX_Interface_t const * p_interface;
} STX_ETX_t;

/********************************************
 * EXPORTED #define CONSTANTS AND MACROS    *
 ********************************************/

#define STX   0x02
#define ETX   0x03
#define DLE   0x10

/********************************************
 * EXPORTED FUNCTIONS PROTOTYPES            *
 ********************************************/

void STX_ETX_Init(STX_ETX_t *                 p_instance,
                  STX_ETX_Config_t const *    p_config,
                  STX_ETX_Interface_t const * p_interface);


void STX_ETX_Reset(STX_ETX_t * p_instance);

STX_ETX_Status_t STX_ETX_Decode(STX_ETX_t *  p_instance,
                                char const * p_in,
                                size_t *     p_in_len,
                                char *       p_out,
                                size_t *     p_out_len);

STX_ETX_Status_t STX_ETX_Encode(STX_ETX_t *  p_instance,
                                char const * p_in,
                                size_t *     p_in_len,
                                char *       p_out,
                                size_t *     p_out_len);

#endif /* #ifndef STX_ETX_H */
