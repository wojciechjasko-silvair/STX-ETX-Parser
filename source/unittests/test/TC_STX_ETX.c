#include <stdio.h>

#include "STX_ETX.h"

#include "unity.h"

const STX_ETX_Interface_t TC_Interface =
{
  .crc16_init   = NULL, // TODO
  .crc16_update = NULL, // TODO
};

void setUp(void)
{

}

void tearDown(void)
{

}

static void TC_SingleProcessNoCRCSuccess(const char * p_expected_encoded,
                                         size_t       encoded_size,
                                         const char * p_expeced_decoded,
                                         size_t       decoded_size)
{
  char encoded[encoded_size + 1];
  char decoded[decoded_size + 1];

  const STX_ETX_Config_t config = 
  {
    .include_crc = false,
  };

  STX_ETX_t stx_etx;
  STX_ETX_Init(&stx_etx, &config, &TC_Interface);

  size_t           encoded_len  = encoded_size;
  size_t           decoded_len  = sizeof(decoded);
  STX_ETX_Status_t status       = STX_ETX_Decode(&stx_etx,
                                                 p_expected_encoded,
                                                 &encoded_len,
                                                 decoded,
                                                 &decoded_len);

  TEST_ASSERT_EQUAL_HEX8(STX_ETX_STATUS_DONE, status);
  TEST_ASSERT_EQUAL(encoded_size, encoded_len);
  TEST_ASSERT_EQUAL(decoded_size, decoded_len);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(p_expeced_decoded, decoded, decoded_size);

  encoded_len  = sizeof(encoded);
  decoded_len  = decoded_size;
  status       = STX_ETX_Encode(&stx_etx,
                                p_expeced_decoded,
                                &decoded_len,
                                encoded,
                                &encoded_len);

  TEST_ASSERT_EQUAL(STX_ETX_STATUS_DONE, status);
  TEST_ASSERT_EQUAL(encoded_size, encoded_len);
  TEST_ASSERT_EQUAL(decoded_size, decoded_len);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(p_expected_encoded, encoded, encoded_size);
}

void test_SingleProcessNoCRCSuccess_1(void)
{
  const char expected_encoded[] = {STX, 0x00, ETX};
  const char expected_decoded[] = {0x00};

  TC_SingleProcessNoCRCSuccess(expected_encoded,
                               sizeof(expected_encoded),
                               expected_decoded,
                               sizeof(expected_decoded));
}

void test_SingleProcessNoCRCSuccess_2(void)
{
  const char expected_encoded[] = {STX, 0x00, 0x01, ETX};
  const char expected_decoded[] = {0x00, 0x01};

  TC_SingleProcessNoCRCSuccess(expected_encoded,
                               sizeof(expected_encoded),
                               expected_decoded,
                               sizeof(expected_decoded));
}

void test_SingleProcessNoCRCSuccess_3(void)
{
  const char expected_encoded[] = {STX, 0x00, 0x01, DLE, DLE, ETX};
  const char expected_decoded[] = {0x00, 0x01, DLE};

  TC_SingleProcessNoCRCSuccess(expected_encoded,
                               sizeof(expected_encoded),
                               expected_decoded,
                               sizeof(expected_decoded));
}

void test_SingleProcessNoCRCSuccess_4(void)
{
  const char expected_encoded[] = {STX, 0x00, 0x01, DLE, STX, ETX};
  const char expected_decoded[] = {0x00, 0x01, STX};

  TC_SingleProcessNoCRCSuccess(expected_encoded,
                               sizeof(expected_encoded),
                               expected_decoded,
                               sizeof(expected_decoded));
}

void test_SingleProcessNoCRCSuccess_5(void)
{
  const char expected_encoded[] = {STX, 0x00, 0x01, DLE, ETX, ETX};
  const char expected_decoded[] = {0x00, 0x01, ETX};

  TC_SingleProcessNoCRCSuccess(expected_encoded,
                               sizeof(expected_encoded),
                               expected_decoded,
                               sizeof(expected_decoded));
}

// // void test_SingleProcessNoCRCSuccess_6(void)
// // {
// //   const char expected_encoded[] = {0xFF, STX, 0x00, ETX};
// //   const char expected_decoded[] = {0x00};

// //   TC_SingleProcessNoCRCSuccess(expected_encoded,
// //                                sizeof(expected_encoded),
// //                                expected_decoded,
// //                                sizeof(expected_decoded));
// // }
