#include "stdafx.h"
#include "GlobalDefs.h"
#include "Utilities.h"
#include <string>
#include "ProgressiveProtocol/AES128EncryptionUtil.h"


#include "gtest/gtest.h"


namespace SentinelUnitTests
{
    class AES128EncryptionUtilTests : public ::testing::Test
    {
    };

    TEST(AES128EncryptionUtilTests, CAES128EncryptionUtil_decrypt)
    {
        typedef struct
        {
            // Inputs.
            std::string key;
            const char* encryptedData;
            const char* expectedDecryptedData;

            // Expected results.
        } TestDataType;

        const TestDataType TEST_DATA[] =
        {
            {
                "eBfEcK$$cNk-Y*7xnGpccb",
                "7oRGUcbrUc/sTjuh3LBw0WVRO/Z725LJSpiT+LMArkhJg6+yS/hs6OCthnIUlg76+X9oWVNg/loEGzPROtzDzJ/uv0Wn2DOROYqVzx8GHely5IuDnVFpz0E7IQvTiR3ej+Zv4v3VfVzzWXcgFGhzlzFr1A/FQ0v6NeJ2r784vwytEYiqbLPtPKoMbbyBsajBc4I21/gD+8Ixqf472HT/qCU7D1FrCHkqPvzxn2vojxAwQPlMt2xgfK3vznC4luG+fxBXpkBpgerj5Zpf9nJb9bVwSFVvw1ww5Q66Qte3nBWezVpVNi1nlr8QXHyyRP+/EgX455wymSJdZDI+3MXpAA==",
                "{\"LevelValues\":[{\"LevelId\":7,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":9,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":11,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1679998659613}"
            },
            {
                "?Ze-++cn5mJtF",
                "4wyR4qo52IBmgG7PnGpHXbK9nteCKy8BGBfYkoUUjduGZ9rpQTbwIbNc1MW6JzeRAipe4av/ie8rpHuzSqejWhWgh1iX8qpl7slGGvAGdwNy80xLbnl4YRih04hwPIifOkrhgQsyWt5QcdllSn/sYvb2qEKfNj1Z2VEb1CW0LmWnrHVLsE+964EZGsX3zM1kNs9RI7sc/eLeP524ZZUVT/eKH2LAoeAe+djDPV3kMOqi4cJNKLIwhZYP9D5+MFpUn+CuJPJT3m4/qhb1qRlanE9vclycle7ZaJof8W9zwG4Y20QSU1oCS/f9pToIjesI5ir3uhswjv6FpL9mIsPO9A==",
                "{\"LevelValues\":[{\"LevelId\":6,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":8,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":10,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1680803503017}"
            },
            {
                "*WJRX$r*g33o-",
                "eYKkDOXUR+RQ/cj7VuH4Me/oTpi+7aomowTUIvCWe4rPjw95WVsUwtF+PnIvSuM8nFNnVWoYW2KAkw/iPtkjGmdzGUAjGxSE9iu4lZWUcnrRqSYpwyiV+75x/1aDYpxYwIeHa9Zu92eALw7S3vd/WuGTvU9z8IXBfw0aDMXqF7fACSWSiz0J8237iErzDKDBxA++2PgcivuqwsoxjHCgRPXGvg/F2Lg1psClqdR5XabHAVrDOxOHM1ATZPLmoecAAWm2CC9R2t1mo73kPDBap8qB+SxGK2RdS5iR+J7/Yjt1wDiGnpHU+qi0uIQaBOvHeEdBKeJmx4QlwOE705TV1w==",
                "{\"LevelValues\":[{\"LevelId\":6,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":8,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":10,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1680803976191}"
            },
            {
                "*WJRX$r*g33o-",
                "eYKkDOXUR+RQ/cj7VuH4Me/oTpi+7aomowTUIvCWe4r0+2i9pYBBO+Qh4DzPruSGnFNnVWoYW2KAkw/iPtkjGmdzGUAjGxSE9iu4lZWUcnrNNH6PBnysFAmdxKuIQTo4wIeHa9Zu92eALw7S3vd/WuGTvU9z8IXBfw0aDMXqF7fACSWSiz0J8237iErzDKDBxA++2PgcivuqwsoxjHCgRPXGvg/F2Lg1psClqdR5XabHAVrDOxOHM1ATZPLmoecAAWm2CC9R2t1mo73kPDBap8qB+SxGK2RdS5iR+J7/Yjt1wDiGnpHU+qi0uIQaBOvHkEANhiXUEvs9Nwv8wNIDBA==",
                "{\"LevelValues\":[{\"LevelId\":6,\"Value\":100.19,\"Name\":\"Prog100\"},{\"LevelId\":8,\"Value\":200.19,\"Name\":\"Prog200\"},{\"LevelId\":10,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1680804078444}"
            },
        };

        CAES128EncryptionUtil m_aes128EncryptionUtil;

        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
        {
            //update the keys
            m_aes128EncryptionUtil.UpdateKeys({ TEST_DATA[testIndex].key });

            //decrypt
            auto newBody = m_aes128EncryptionUtil.Decrypt(reinterpret_cast<const unsigned char*>(TEST_DATA[testIndex].encryptedData));

            ASSERT_TRUE(0 == strcmp(reinterpret_cast<const char*>(newBody.get()), TEST_DATA[testIndex].expectedDecryptedData)) << "the decrypted text does not match the expected text";
        }
        // Cleanup.
    }

    TEST(AES128EncryptionUtilTests, CAES128EncryptionUtil_multiple_keys_correct_one_first)
    {
        const char* encryptedData = "7oRGUcbrUc/sTjuh3LBw0WVRO/Z725LJSpiT+LMArkhJg6+yS/hs6OCthnIUlg76+X9oWVNg/loEGzPROtzDzJ/uv0Wn2DOROYqVzx8GHely5IuDnVFpz0E7IQvTiR3ej+Zv4v3VfVzzWXcgFGhzlzFr1A/FQ0v6NeJ2r784vwytEYiqbLPtPKoMbbyBsajBc4I21/gD+8Ixqf472HT/qCU7D1FrCHkqPvzxn2vojxAwQPlMt2xgfK3vznC4luG+fxBXpkBpgerj5Zpf9nJb9bVwSFVvw1ww5Q66Qte3nBWezVpVNi1nlr8QXHyyRP+/EgX455wymSJdZDI+3MXpAA==";
        const char* expectedDecryptedData = "{\"LevelValues\":[{\"LevelId\":7,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":9,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":11,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1679998659613}";

        CAES128EncryptionUtil m_aes128EncryptionUtil;
        m_aes128EncryptionUtil.UpdateKeys({ "eBfEcK$$cNk-Y*7xnGpccb", "?Ze-++cn5mJtF"});
        auto newBody = m_aes128EncryptionUtil.Decrypt(reinterpret_cast<const unsigned char*>(encryptedData));

        ASSERT_TRUE(0 == strcmp(reinterpret_cast<const char*>(newBody.get()), expectedDecryptedData)) << "the decrypted text does not match the expected text";
    }

    TEST(AES128EncryptionUtilTests, CAES128EncryptionUtil_multiple_keys_correct_one_second)
    {
        const char* encryptedData = "7oRGUcbrUc/sTjuh3LBw0WVRO/Z725LJSpiT+LMArkhJg6+yS/hs6OCthnIUlg76+X9oWVNg/loEGzPROtzDzJ/uv0Wn2DOROYqVzx8GHely5IuDnVFpz0E7IQvTiR3ej+Zv4v3VfVzzWXcgFGhzlzFr1A/FQ0v6NeJ2r784vwytEYiqbLPtPKoMbbyBsajBc4I21/gD+8Ixqf472HT/qCU7D1FrCHkqPvzxn2vojxAwQPlMt2xgfK3vznC4luG+fxBXpkBpgerj5Zpf9nJb9bVwSFVvw1ww5Q66Qte3nBWezVpVNi1nlr8QXHyyRP+/EgX455wymSJdZDI+3MXpAA==";
        const char* expectedDecryptedData = "{\"LevelValues\":[{\"LevelId\":7,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":9,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":11,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1679998659613}";

        CAES128EncryptionUtil m_aes128EncryptionUtil;
        m_aes128EncryptionUtil.UpdateKeys({"?Ze-++cn5mJtF", "eBfEcK$$cNk-Y*7xnGpccb"});
        auto newBody = m_aes128EncryptionUtil.Decrypt(reinterpret_cast<const unsigned char*>(encryptedData));

        ASSERT_TRUE(0 == strcmp(reinterpret_cast<const char*>(newBody.get()), expectedDecryptedData)) << "the decrypted text does not match the expected text";
    }

    TEST(AES128EncryptionUtilTests, CAES128EncryptionUtil_Fail_Data)
    {
        const char* encryptedData = "eYKkDOXUR+RQ/cj7VuH4Me/oTpi+7aomowTUIvCWe4r0+2i9pYBBO+Qh4DzPruSGnFNnVWoYW2KAkw/iPtkjGmdzGUAjGxSE9iu4lZWUcnrNNH6PBnysFAmdxKuIQTo4wIeHa9Zu92eALw7S3vd/WuGTvU9z8IXBfw0aDMXqF7fACSWSiz0J8237iErzDKDBxA++2PgcivuqwsoxjHCgRPXGvg/F2Lg1psClqdR5XabHAVrDOxOHM1ATZPLmoecAAWm2CC9R2t1mo73kPDBap8qB+SxGK2RdS5iR+J7/Yjt1wDiGnpHU+qi0uIQaBOvHkEANhiXUEvs9Nwv8wNIDBA==";
        const char* expectedDecryptedData = "{\"LevelValues\":[{\"LevelId\":7,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":9,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":11,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1679998659613}";

        CAES128EncryptionUtil m_aes128EncryptionUtil;
        m_aes128EncryptionUtil.UpdateKeys({ "eBfEcK$$cNk-Y*7xnGpccb" });
        auto newBody = m_aes128EncryptionUtil.Decrypt(reinterpret_cast<const unsigned char*>(encryptedData));

        ASSERT_FALSE(0 == strcmp(reinterpret_cast<const char*>(newBody.get()), expectedDecryptedData)) << "the decrypted text does not match the expected text";
    }

    TEST(AES128EncryptionUtilTests, CAES128EncryptionUtil_Fail_Key)
    {
        const char* encryptedData = "eYKkDOXUR+RQ/cj7VuH4Me/oTpi+7aomowTUIvCWe4r0+2i9pYBBO+Qh4DzPruSGnFNnVWoYW2KAkw/iPtkjGmdzGUAjGxSE9iu4lZWUcnrNNH6PBnysFAmdxKuIQTo4wIeHa9Zu92eALw7S3vd/WuGTvU9z8IXBfw0aDMXqF7fACSWSiz0J8237iErzDKDBxA++2PgcivuqwsoxjHCgRPXGvg/F2Lg1psClqdR5XabHAVrDOxOHM1ATZPLmoecAAWm2CC9R2t1mo73kPDBap8qB+SxGK2RdS5iR+J7/Yjt1wDiGnpHU+qi0uIQaBOvHkEANhiXUEvs9Nwv8wNIDBA==";
        const char* expectedDecryptedData = "{\"LevelValues\":[{\"LevelId\":7,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":9,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":11,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1679998659613}";

        CAES128EncryptionUtil m_aes128EncryptionUtil;
        m_aes128EncryptionUtil.UpdateKeys({ "ThisIsABadKey" });
        auto newBody = m_aes128EncryptionUtil.Decrypt(reinterpret_cast<const unsigned char*>(encryptedData));

        ASSERT_FALSE(0 == strcmp(reinterpret_cast<const char*>(newBody.get()), expectedDecryptedData)) << "the decrypted text does not match the expected text";
    }
}