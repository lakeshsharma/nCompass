
#include "stdafx.h"

#include "../UI/AttractorSequenceItem.h"
#include "../UI/ServiceWindowManager.h"

#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{

		void DefaultValuesTest(CAttractorSequenceItem *attractorSequenceItem)
		{
			ASSERT_TRUE(((string)attractorSequenceItem->GetFilepath()).empty())<<
						   L"m_filepath was not properly initialized in the constructor";
			ASSERT_FALSE(attractorSequenceItem->GetDuration() == 0)<<
						   L"m_duration was not properly initialized in the constructor";
			ASSERT_TRUE(((string)attractorSequenceItem->GetParameters()).empty())<<
						   L"m_parameters was not properly initialized in the constructor";
			ASSERT_TRUE(attractorSequenceItem->GetCustomMessages() == nullptr)<<
						   L"m_customMessages was not properly initialized in the constructor";	
		}

		TEST(AttractorSequenceItemUnitTests, AttractorSequenceItemDefaultConstructorTest)
		{
			//Setup
			CAttractorSequenceItem attractorSequenceItem;

			//Tests
			DefaultValuesTest(&attractorSequenceItem);		
		}

		TEST(AttractorSequenceItemUnitTests, AttractorSequenceItemConstructorWith4ParametersTest)
		{
			//Setup
			string filePath("C:/Test/FilePath");
			DWORD	duration(3000);
			string params("TestParam1 TestParam2");
			int ctrlID1(1), ctrlID2(2);
			CCtrlInfoList *ctrlInfoList(new CCtrlInfoList());
			CCtrlInfoList *returnedCtrlInfoList(NULL);
			CCtrlInfo *ctrlInfo1(new CCtrlInfo());
			CCtrlInfo *ctrlInfo2(new CCtrlInfo());
			ctrlInfo1->SetCtrlID(ctrlID1);			
			ctrlInfo2->SetCtrlID(ctrlID2);
			ctrlInfoList->AddCtrl(ctrlInfo1);
			ctrlInfoList->AddCtrl(ctrlInfo2);			
			CAttractorSequenceItem test2(filePath, duration, params, ctrlInfoList);

			//Tests
			ASSERT_TRUE(test2.GetFilepath() == filePath)<<
						   L"m_filepath was not properly initialized in the constructor";
			ASSERT_TRUE(test2.GetDuration() == duration)<<
						   L"m_duration was not properly initialized in the constructor";
			ASSERT_TRUE(test2.GetParameters() == params)<<
						   L"m_parameters was not properly initialized in the constructor";
			returnedCtrlInfoList = test2.GetCustomMessages();
			ASSERT_TRUE(returnedCtrlInfoList->GetCount() == ctrlInfoList->GetCount())<<
				L"CAttractorSequenceItem::SetCustomMessages function failed";
			ASSERT_TRUE(ctrlID1 == ((CCtrlInfo*)returnedCtrlInfoList->GetCtrlAt(0))->GetCtrlID())<<
				L"CAttractorSequenceItem::SetCustomMessages function failed";
			ASSERT_TRUE(ctrlID2 == ((CCtrlInfo*)returnedCtrlInfoList->GetCtrlAt(1))->GetCtrlID())<<
				L"CAttractorSequenceItem::SetCustomMessages function failed";
			
			delete ctrlInfoList;//this will also delete ctrlInfo1 and ctrlInfo2 objects
							    //deleting  ctrlInfo1 and ctrlInfo2 will cause a crash
		}

		TEST(AttractorSequenceItemUnitTests, AttractorSequenceItemConstructorWithImageInfoTest)
		{
			//Setup
			CImageInfo *imageInfo(new CImageInfo());
			string imageFileName("c:/xyz/imageFileName");
			DWORD duration(3000);
			imageInfo->SetImage(imageFileName);
			imageInfo->SetDuration(duration);
			CAttractorSequenceItem attractorSequenceItem( imageInfo, NULL );

			//Tests
			ASSERT_TRUE(attractorSequenceItem.GetFilepath() == imageFileName)<<
				L"AttractorSequenceItemConstructorWithImageInfo incorrectly initialized m_filepath.";
			ASSERT_TRUE(attractorSequenceItem.GetDuration() == duration)<<
				L"AttractorSequenceItemConstructorWithImageInfo incorrectly initialized m_duration.";
			ASSERT_FALSE(attractorSequenceItem.HasParameters())<<
				L"AttractorSequenceItemConstructorWithImageInfo incorrectly initialized m_parameters.";
			ASSERT_FALSE(attractorSequenceItem.HasCustomMessages())<<
				L"AttractorSequenceItemConstructorWithImageInfo incorrectly initialized m_customMessages.";

			delete imageInfo;
		}

		TEST(AttractorSequenceItemUnitTests, AttractorSequenceItemConstructorWithVideoInfoTest)
		{
			//Setup
			CVideoImageInfo *videoInfo(new CVideoImageInfo());
			string videoFileName("c:/xyz/videoFileName.mp4");
			string params("param1 param2");
			DWORD duration(3000);
			videoInfo->SetDuration(duration);
			videoInfo->SetVideoFilename(videoFileName);
			CAttractorSequenceItem attractorSequenceItem(videoInfo, params);

			//Tests
			ASSERT_TRUE(attractorSequenceItem.GetFilepath() == videoFileName)<<
				L"AttractorSequenceItemConstructorWithVideoInfo incorrectly initialized m_filepath.";
			ASSERT_TRUE(attractorSequenceItem.GetDuration() == duration)<<
				L"AttractorSequenceItemConstructorWithVideoInfo incorrectly initialized m_duration.";
			ASSERT_FALSE(attractorSequenceItem.HasParameters())<<
				L"AttractorSequenceItemConstructorWithVideoInfo incorrectly initialized m_parameters.";

			ASSERT_FALSE(attractorSequenceItem.HasCustomMessages())<<
				L"AttractorSequenceItemConstructorWithVideoInfo incorrectly initialized m_customMessages.";

			delete videoInfo;
		}

		TEST(AttractorSequenceItemUnitTests, AttractorSequenceItemConstructionNullTest)
		{
			//Test #1
			CAttractorSequenceItem obj1("", 3000, "", nullptr);
			DefaultValuesTest(&obj1);

			//Test #2 CAttractorSequenceItem( CImageInfo *imageInfo, CCtrlInfoList *customMessages );
			CImageInfo *imageInfo(nullptr);
			CAttractorSequenceItem obj2(imageInfo, nullptr);
			DefaultValuesTest(&obj2);

			//Test #3 CAttractorSequenceItem::CAttractorSequenceItem( CVideoImageInfo *videoInfo, const std::string& parms );
			CVideoImageInfo *videoInfo(nullptr);
			CAttractorSequenceItem obj3(videoInfo, "");
			DefaultValuesTest(&obj3);
		}

		TEST(AttractorSequenceItemUnitTests, AttractorSequenceItemParametersFunctionTest)
		{
			string params("Param1 Param2 Param3");
			CAttractorSequenceItem attractorSequenceItem;

			//Tests
			attractorSequenceItem.SetParameters("");
			ASSERT_FALSE(attractorSequenceItem.HasParameters())<<
						   L"CAttractorSequenceItem::SetParameters function failed";

			attractorSequenceItem.SetParameters(params);
			ASSERT_TRUE(attractorSequenceItem.HasParameters())<<
						   L"CAttractorSequenceItem::SetParameters function failed";
			
			ASSERT_TRUE(attractorSequenceItem.GetParameters() == params)<<
						   L"CAttractorSequenceItem::SetParameters function failed";
		}

		TEST(AttractorSequenceItemUnitTests, AttractorSequenceItemDurationFunctionTest)
		{
			DWORD duration(3000);
			CAttractorSequenceItem attractorSequenceItem;//default constructor has duration set not to do 
														 //infinite video duration

			//Tests
			ASSERT_FALSE(attractorSequenceItem.IsDurationInfinite())<<//Infinite is when duration is to 0
						   L"CAttractorSequenceItem::IsDurationInfinite function failed";

			attractorSequenceItem.SetDuration(duration);
			ASSERT_TRUE(attractorSequenceItem.GetDuration() == duration)<<
						   L"CAttractorSequenceItem::SetDuration function failed";
		}

		TEST(AttractorSequenceItemUnitTests, AttractorSequenceItemCustomMessagesFunctionsTest)
		{
			//Setup
			CAttractorSequenceItem attractorSequenceItem;
			int ctrlID1(1), ctrlID2(2);
			CCtrlInfoList *ctrlInfoList(new CCtrlInfoList());
			CCtrlInfoList *returnedCtrlInfoList(NULL);
			CCtrlInfo *ctrlInfo1(new CCtrlInfo());
			CCtrlInfo *ctrlInfo2(new CCtrlInfo());
			ctrlInfo1->SetCtrlID(ctrlID1);			
			ctrlInfo2->SetCtrlID(ctrlID2);
			ctrlInfoList->AddCtrl(ctrlInfo1);
			ctrlInfoList->AddCtrl(ctrlInfo2);

			//Tests
			ASSERT_FALSE(attractorSequenceItem.HasCustomMessages())<<
				L"CAttractorSequenceItem::HasCustomMessages function failed";

			attractorSequenceItem.SetCustomMessages(ctrlInfoList);
			ASSERT_TRUE(attractorSequenceItem.HasCustomMessages())<<
				L"CAttractorSequenceItem::SetCustomMessages function failed";

			returnedCtrlInfoList = attractorSequenceItem.GetCustomMessages();
			ASSERT_TRUE(returnedCtrlInfoList->GetCount() == ctrlInfoList->GetCount())<<
				L"CAttractorSequenceItem::SetCustomMessages function failed";
			ASSERT_TRUE(ctrlID1 == ((CCtrlInfo*)returnedCtrlInfoList->GetCtrlAt(0))->GetCtrlID())<<
				L"CAttractorSequenceItem::SetCustomMessages function failed";
			ASSERT_TRUE(ctrlID2 == ((CCtrlInfo*)returnedCtrlInfoList->GetCtrlAt(1))->GetCtrlID())<<
				L"CAttractorSequenceItem::SetCustomMessages function failed";

			delete ctrlInfoList;//this will also delete ctrlInfo1 and ctrlInfo2 objects
							    //deleting  ctrlInfo1 and ctrlInfo2 will cause a crash
		}
}

