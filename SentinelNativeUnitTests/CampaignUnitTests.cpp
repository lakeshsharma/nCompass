#if 000//TODO-PORT

#include "stdafx.h"
#include "UI/Campaign.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(CampaignUnitTests)
	{

	public:
		TEST_METHOD(CampaignConstructor)
		{
			CCampaign campaign;

			Assert::IsTrue(campaign.IsDefault() == false);
			Assert::IsTrue(campaign.GetID() == -1);
		}

		TEST_METHOD(CampaignSetID)
		{
			CCampaign campaign;
			int newID = 1234;

			Assert::IsTrue(campaign.GetID() == -1);
			campaign.SetID(newID);
			Assert::IsTrue(campaign.GetID() == newID);
		}

		TEST_METHOD(CampaignDefault)
		{
			CCampaign campaign;

			Assert::IsTrue(campaign.IsDefault() == false);
			campaign.SetDefault(true);
			Assert::IsTrue(campaign.IsDefault() == true);
		}


	};
}

#endif //TODO-PORT