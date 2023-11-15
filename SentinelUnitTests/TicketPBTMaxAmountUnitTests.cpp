#include "stdafx.h"
#include "TicketPBTMaxAmount.h"
#include "DirectoryManager.h"

#include "gtest/gtest.h"

namespace SentinelUnitTests
{
        TEST(TicketPBTMaxAmountUnitTests, TicketPBTMaxAmountInterpretNameValue)
        {
			// Init CDirectoryManager, prevent full setup
			CDirectoryManager::MockInstance();

			CTicketPBTMaxAmount maxVals(CUtilities::GetTickCount());
			maxVals.Start();

			// Good name/values
			ASSERT_TRUE(maxVals.InterpretNameValue("PBTMAX", "12345"));
			ASSERT_TRUE(maxVals.InterpretNameValue("TICKETMAX", "12345"));

			// Bad name
			ASSERT_FALSE(maxVals.InterpretNameValue("MAX", "12345"));

			// Negative values
			ASSERT_FALSE(maxVals.InterpretNameValue("PBTMAX", "-12345"));
			ASSERT_FALSE(maxVals.InterpretNameValue("TICKETMAX", "-12345"));

			// Good name/bad values
			ASSERT_FALSE(maxVals.InterpretNameValue("PBTMAX", "ONETWO"));
			ASSERT_FALSE(maxVals.InterpretNameValue("TICKETMAX", "ONETWO"));
			ASSERT_FALSE(maxVals.InterpretNameValue("PBTMAX", "ON2345"));
			ASSERT_FALSE(maxVals.InterpretNameValue("TICKETMAX", "ONE33333"));
		}

		TEST(TicketPBTMaxAmountUnitTests, TicketPBTMaxGetMaxValues)
		{
			// Init CDirectoryManager, prevent full setup
			CDirectoryManager::MockInstance();

			CTicketPBTMaxAmount maxValsGood(CUtilities::GetTickCount());
			maxValsGood.Start();
			CTicketPBTMaxAmount maxValsBad(CUtilities::GetTickCount());
			maxValsBad.Start();

			// Good name/values
			maxValsGood.InterpretNameValue("PBTMAX", "12345");
			maxValsGood.InterpretNameValue("TICKETMAX", "12345");
			ASSERT_TRUE(maxValsGood.GetMaxPBTAmount() == 12345);
			ASSERT_TRUE(maxValsGood.GetMaxTicketAmount() == 12345);

			// Bad name/values
			maxValsBad.InterpretNameValue("PBTMAX", "aaa12345");
			maxValsBad.InterpretNameValue("TICKETMAX", "aaa12345");
			ASSERT_TRUE(maxValsBad.GetMaxPBTAmount() == PBTMAX);
			ASSERT_TRUE(maxValsBad.GetMaxTicketAmount() == TICKETMAX);
		}
}
