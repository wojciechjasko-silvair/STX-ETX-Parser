#include <stdio.h>

#include "STX_ETX.h"

#include "unity.h"

const STX_ETX_Config_t TC_ConfigNoCRC = 
{
  .crc16_init   = NULL,
  .crc16_update = NULL,
};

void setUp(void)
{

}

void tearDown(void)
{

}

static void TC_Decode(STX_ETX_t *      p_instance,
                      STX_ETX_Status_t expected_status,
                      const char *     p_expected_encoded,
                      size_t           expected_encoded_len,
                      size_t           provided_encoded_len,
                      const char *     p_expeced_decoded,
                      size_t           expected_decoded_len,
                      size_t           provided_decode_len)
{
  char decoded[provided_decode_len];

  size_t           encoded_len  = provided_encoded_len;
  size_t           decoded_len  = provided_decode_len;
  STX_ETX_Status_t status       = STX_ETX_Decode(p_instance,
                                                 p_expected_encoded,
                                                 &encoded_len,
                                                 decoded,
                                                 &decoded_len);

  TEST_ASSERT_EQUAL_HEX8(expected_status, status);
  TEST_ASSERT_EQUAL(expected_encoded_len, encoded_len);
  TEST_ASSERT_EQUAL(expected_decoded_len, decoded_len);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(p_expeced_decoded, decoded, decoded_len);
}

static void TC_Encode(STX_ETX_t *      p_instance,
                      STX_ETX_Status_t expected_status,
                      const char *     p_expected_encoded,
                      size_t           expected_encoded_len,
                      size_t           provided_encoded_len,
                      const char *     p_expeced_decoded,
                      size_t           expected_decoded_len,
                      size_t           provided_decode_len)
{
  char encoded[provided_encoded_len];

  size_t           encoded_len  = provided_encoded_len;
  size_t           decoded_len  = provided_decode_len;
  STX_ETX_Status_t status       = STX_ETX_Encode(p_instance,
                                                 p_expeced_decoded,
                                                 &decoded_len,
                                                 encoded,
                                                 &encoded_len);

  TEST_ASSERT_EQUAL(expected_status, status);
  TEST_ASSERT_EQUAL(expected_encoded_len, encoded_len);
  TEST_ASSERT_EQUAL(expected_decoded_len, decoded_len);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(p_expected_encoded, encoded, encoded_len);
}

static void TC_SingleProcessNoCRCSuccess(const char * p_expected_encoded,
                                         size_t       expected_encoded_len,
                                         const char * p_expeced_decoded,
                                         size_t       expected_decoded_len)
{
  STX_ETX_t stx_etx;
  STX_ETX_Init(&stx_etx, &TC_ConfigNoCRC);

  TC_Decode(&stx_etx,
            STX_ETX_STATUS_DONE,
            p_expected_encoded,
            expected_encoded_len,
            expected_encoded_len,
            p_expeced_decoded,
            expected_decoded_len,
            expected_decoded_len);

  TC_Encode(&stx_etx,
            STX_ETX_STATUS_DONE,
            p_expected_encoded,
            expected_encoded_len,
            expected_encoded_len,
            p_expeced_decoded,
            expected_decoded_len,
            expected_decoded_len);
}

static void TC_DecodeFail(const char * p_expected_encoded,
                          size_t       expected_encoded_len,
                          size_t       expected_decoded_len)
{
  char decoded[expected_decoded_len];

  STX_ETX_t stx_etx;
  STX_ETX_Init(&stx_etx, &TC_ConfigNoCRC);

  size_t           encoded_len  = expected_encoded_len;
  size_t           decoded_len  = expected_decoded_len;
  STX_ETX_Status_t status       = STX_ETX_Decode(&stx_etx,
                                                 p_expected_encoded,
                                                 &encoded_len,
                                                 decoded,
                                                 &decoded_len);

  TEST_ASSERT_EQUAL_HEX8(STX_ETX_STATUS_INV_CHAR, status);
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

void test_DecodeNoSTX(void)
{
  const char expected_encoded[] = {0xFF, STX, 0x00, ETX};

  TC_DecodeFail(expected_encoded,
                sizeof(expected_encoded),
                sizeof(expected_encoded));
}

void test_DecodeSTXwithoutDLE(void)
{
  const char expected_encoded[] = {STX, STX, 0x00, ETX};

  TC_DecodeFail(expected_encoded,
                sizeof(expected_encoded),
                sizeof(expected_encoded));
}

void test_DecodeSingleDLE(void)
{
  const char expected_encoded[] = {STX, 0x00, DLE, 0x12, ETX};

  TC_DecodeFail(expected_encoded,
                sizeof(expected_encoded),
                sizeof(expected_encoded));
}

void test_StreamDecodeNoCRCSuccess_SplitInput(void)
{
  STX_ETX_t stx_etx;
  STX_ETX_Init(&stx_etx, &TC_ConfigNoCRC);

  const char expected_encoded0[] = {STX, 0x00, 0x01};
  const char expected_decoded0[] = {0x00, 0x01};

  const char expected_encoded1[] = {DLE, DLE, ETX};
  const char expected_decoded1[] = {DLE};

  TC_Decode(&stx_etx,
            STX_ETX_STATUS_CONTINUE,
            expected_encoded0,
            sizeof(expected_encoded0),
            sizeof(expected_encoded0),
            expected_decoded0,
            sizeof(expected_decoded0),
            sizeof(expected_decoded0));

  TC_Decode(&stx_etx,
            STX_ETX_STATUS_DONE,
            expected_encoded1,
            sizeof(expected_encoded1),
            sizeof(expected_encoded1),
            expected_decoded1,
            sizeof(expected_decoded1),
            sizeof(expected_decoded1));
}

void test_StreamDecodeNoCRCSuccess_SplitOutput(void)
{
  STX_ETX_t stx_etx;
  STX_ETX_Init(&stx_etx, &TC_ConfigNoCRC);

  const char expected_encoded0[] = {STX, 0x00, 0x01, DLE, DLE, ETX};
  const char expected_decoded0[] = {0x00, 0x01};

  const char expected_encoded1[] = {DLE, ETX};
  const char expected_decoded1[] = {DLE};

  TC_Decode(&stx_etx,
            STX_ETX_STATUS_OVERFLOW,
            expected_encoded0,
            4,
            sizeof(expected_encoded0),
            expected_decoded0,
            sizeof(expected_decoded0),
            sizeof(expected_decoded0));

  TC_Decode(&stx_etx,
            STX_ETX_STATUS_DONE,
            expected_encoded1,
            sizeof(expected_encoded1),
            sizeof(expected_encoded1),
            expected_decoded1,
            sizeof(expected_decoded1),
            sizeof(expected_decoded1));
}

void test_StreamEncodeNoCRCSuccess_SplitOutput1(void)
{
  STX_ETX_t stx_etx;
  STX_ETX_Init(&stx_etx, &TC_ConfigNoCRC);

  const char expected_encoded0[] = {STX, 0x00, 0x01, DLE};
  const char expected_decoded0[] = {0x00, 0x01, DLE};

  const char expected_encoded1[] = {DLE, ETX};
  const char expected_decoded1[] = {DLE};

  TC_Encode(&stx_etx,
            STX_ETX_STATUS_OVERFLOW,
            expected_encoded0,
            sizeof(expected_encoded0),
            sizeof(expected_encoded0),
            expected_decoded0,
            2,
            sizeof(expected_decoded0));

  TC_Encode(&stx_etx,
            STX_ETX_STATUS_DONE,
            expected_encoded1,
            sizeof(expected_encoded1),
            sizeof(expected_encoded1),
            expected_decoded1,
            sizeof(expected_decoded1),
            sizeof(expected_decoded1));
}

void test_StreamEncodeNoCRCSuccess_SplitOutput2(void)
{
  STX_ETX_t stx_etx;
  STX_ETX_Init(&stx_etx, &TC_ConfigNoCRC);

  const char expected_encoded0[] = {STX, 0x00, 0x01};
  const char expected_decoded0[] = {0x00, 0x01};

  const char expected_encoded1[] = {ETX};


  TC_Encode(&stx_etx,
            STX_ETX_STATUS_OVERFLOW,
            expected_encoded0,
            sizeof(expected_encoded0),
            sizeof(expected_encoded0),
            expected_decoded0,
            2,
            sizeof(expected_decoded0));

  TC_Encode(&stx_etx,
            STX_ETX_STATUS_DONE,
            expected_encoded1,
            sizeof(expected_encoded1),
            sizeof(expected_encoded1),
            NULL,
            0,
            0);
}
