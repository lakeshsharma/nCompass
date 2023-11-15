
#include "stdafx.h"
#include "SpeedMediaCacheListMock.h"
#include "DirectoryManager.h"

using ::testing::Mock;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;

#if 0
namespace SentinelNativeUnitTests
{

    TEST(SpeedMediaCacheListTests, SpeedMediaCacheListStaticCacheNone)
    {
        //Setup
        CDirectoryManager::MockInstance();
        NiceMock<CSpeedMediaCacheListMock> cacheListMock;
        DWORD NewTime = 1000;

        ON_CALL(cacheListMock, GetTick()).WillByDefault(Return(NewTime));

        DWORD lastCheckTime = 0;
        cacheListMock.CheckStaticCache(lastCheckTime);

        vector<CIPItem> list;
        cacheListMock.GetCacheIpList(list);

        ASSERT_TRUE(0 == list.size()) << "No items should be added";
        ASSERT_TRUE(0 != lastCheckTime) << "lastCheckTime should have not have been reset.";
        list.clear();


        NewTime = 70000;
        ON_CALL(cacheListMock, GetTick()).WillByDefault(Return(NewTime));
        ON_CALL(cacheListMock, OpenStaticCacheFile(_)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::OpenStaticCacheFileFailure));

        lastCheckTime = 0;
        cacheListMock.CheckStaticCache(lastCheckTime);
        cacheListMock.GetCacheIpList(list);

        ASSERT_TRUE(0 == list.size()) << "No items should be added";
        ASSERT_EQ(NewTime, lastCheckTime) << "lastCheckTime should have been reset to NewTime";
        list.clear();

    }

    TEST(SpeedMediaCacheListTests, SpeedMediaCacheListStaticCache1)
    {
        //Setup
        CDirectoryManager::MockInstance();
        NiceMock<CSpeedMediaCacheListMock> cacheListMock;
        DWORD NewTime = 70000;

        ON_CALL(cacheListMock, OpenStaticCacheFile(_)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::OpenStaticCacheFileSuccess));
        ON_CALL(cacheListMock, ReadNextStaticCacheIP(_, _)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::ReadNextStaticCacheIPMockGood1));
        ON_CALL(cacheListMock, GetTick()).WillByDefault(Return(NewTime));

        DWORD lastCheckTime = 0;
        cacheListMock.CheckStaticCache(lastCheckTime);

        vector<CIPItem> list;
        cacheListMock.GetCacheIpList(list);

        ASSERT_TRUE(1 == list.size()) << "One items should be added";
        list.clear();

    }


    TEST(SpeedMediaCacheListTests, SpeedMediaCacheListStaticCache2)
    {
        //Setup
        CDirectoryManager::MockInstance();
        NiceMock<CSpeedMediaCacheListMock> cacheListMock;
        DWORD NewTime = 70000;

        ON_CALL(cacheListMock, OpenStaticCacheFile(_)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::OpenStaticCacheFileSuccess));
        ON_CALL(cacheListMock, ReadNextStaticCacheIP(_, _)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::ReadNextStaticCacheIPMockGood2));
        ON_CALL(cacheListMock, GetTick()).WillByDefault(Return(NewTime));

        DWORD lastCheckTime = 0;
        cacheListMock.CheckStaticCache(lastCheckTime);

        vector<CIPItem> list;
        cacheListMock.GetCacheIpList(list);

        ASSERT_TRUE(2 == list.size()) << "Two items should be added";
        list.clear();

    }

    TEST(SpeedMediaCacheListTests, SpeedMediaCacheListStaticCachePadded)
    {
        //Setup
        CDirectoryManager::MockInstance();
        NiceMock<CSpeedMediaCacheListMock> cacheListMock;
        DWORD NewTime = 70000;

        ON_CALL(cacheListMock, OpenStaticCacheFile(_)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::OpenStaticCacheFileSuccess));
        ON_CALL(cacheListMock, ReadNextStaticCacheIP(_, _)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::ReadNextStaticCacheIPMockGoodPadded));
        ON_CALL(cacheListMock, GetTick()).WillByDefault(Return(NewTime));

        DWORD lastCheckTime = 0;
        cacheListMock.CheckStaticCache(lastCheckTime);

        vector<CIPItem> list;
        cacheListMock.GetCacheIpList(list);

        ASSERT_TRUE(1 == list.size()) << "One items should be added";
        list.clear();

    }

    TEST(SpeedMediaCacheListTests, SpeedMediaCacheListStaticCacheBad)
    {
        //Setup
        CDirectoryManager::MockInstance();
        NiceMock<CSpeedMediaCacheListMock> cacheListMock;
        DWORD NewTime = 70000;

        ON_CALL(cacheListMock, OpenStaticCacheFile(_)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::OpenStaticCacheFileSuccess));
        ON_CALL(cacheListMock, ReadNextStaticCacheIP(_, _)).WillByDefault(Invoke(&cacheListMock, &CSpeedMediaCacheListMock::ReadNextStaticCacheIPMockBad));
        ON_CALL(cacheListMock, GetTick()).WillByDefault(Return(NewTime));

        DWORD lastCheckTime = 0;
        cacheListMock.CheckStaticCache(lastCheckTime);

        vector<CIPItem> list;
        cacheListMock.GetCacheIpList(list);

        ASSERT_TRUE(0 == list.size()) << "No items should be added";
        list.clear();

    }

}
#endif
