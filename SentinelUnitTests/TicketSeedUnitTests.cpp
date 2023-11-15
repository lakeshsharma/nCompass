#include "stdafx.h"
#include "SASProtocol/TicketSeed.h"

#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

const DWORD MACHINEID = 0x00a3c064;

class TicketSeedUnitTests : public ::testing::Test
{
		
public:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

};

TEST_F(TicketSeedUnitTests, TicketSeedGetMachineIDTest)
{
	CTicketSeed *ticketSeed = new CTicketSeed( 1215 );
	long slotmastID = 100;
	byte casinoID = 4;
	DWORD machineID = ticketSeed->GetMachineID( slotmastID, casinoID );
	ASSERT_TRUE( machineID == MACHINEID );

	delete ticketSeed;
}

TEST_F(TicketSeedUnitTests, TicketSeedIncSeedTest)
{
	CTicketSeed *ticketSeed = new CTicketSeed( 1215 );
	DWORD sequence = 0x010203;

	ticketSeed->IncAndRandSequence( (byte *)&sequence, false );
	ASSERT_TRUE( (sequence & 0xffff) == 0x0204 );
	delete ticketSeed;

}

TEST_F(TicketSeedUnitTests,ZeroMachineIDWithPrevSequenceTest)
{
	CTicketSeed *ticketSeed = new CTicketSeed( 1215 );
	DWORD sequence = 0x010203;

	ticketSeed->IncAndRandSequence( (byte *)&sequence, false ); // results in 0x..0204

	ticketSeed->IncAndRandSequence( (byte *)&sequence, true);
	ASSERT_TRUE( (sequence & 0xffff) == 0x0207 );
	delete ticketSeed;
			
}

TEST_F(TicketSeedUnitTests, ZeroMachineIDNoPrevSequenceTest)
{
	CTicketSeed *ticketSeed = new CTicketSeed( 1215 );
	DWORD sequence = 0x018203;

	ticketSeed->IncAndRandSequence( (byte *)&sequence, true );
	ASSERT_TRUE( (sequence & 0xffff) != 0x8204 );
	delete ticketSeed;

}
