
#include "stdafx.h"
#include "CardReaderTestHelpers.h"
#include "TestUtil.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;
using namespace TestUtil;

namespace CardReaderTestHelpers
    {
		const CardReaderColor colorNone =
		{//     Red           Green         Blue
			{ {   0,   0 }, {   0,   0 }, {   0, 255 } },
			{ {   0,   0 }, {   0,   0 }, {   0,   0 } },
			0,
			4, "Blue"
		};

		const CardReaderColor colorPlayer =
		{//     Red           Green         Blue
			{ {   0,   0 }, {   0, 255 }, {   0,   0 } },
			{ {   0,   0 }, {   0,   0 }, {   0,   0 } },
			0,
			3, "Green"
		};

		const CardReaderColor colorEmployee =
		{//     Red           Green         Blue
			{ {   0,  32 }, {   0, 255 }, {   0,  32 } },
			{ {   0,   0 }, {   0,   0 }, {   0,   0 } },
			0,
			3, "Green"
		};

		const CardReaderColor colorEmployeeDefault =
		{//     Red           Green         Blue
			{ {   0,   0 }, {   0, 255 }, {   0,   0 } },
			{ {   0,   0 }, {   0,   0 }, {   0,   0 } },
			0,
			3, "Green"
		};

		const CardReaderColor colorBadRead =
		{//     Red           Green         Blue
			{ {   0, 255 }, {   0,   0 }, {   0,   0 } },
			{ {   0,   0 }, {   0,   0 }, {   0,   0 } },
			2,
			16, "Flashing Red"
		};

		const CardReaderColor colorRank0 =
		{//     Red           Green         Blue
			{ {   0, 255 }, {   0,   0 }, {   0,   0 } },
			{ {   0,   0 }, {   0,   0 }, {   0,   0 } },
			0,
			1,  "Red"
		};

		const CardReaderColor colorRank1 =
		{//     Red           Green         Blue
			{ {   0, 255 }, {   0,  64 }, {   0,   0 } },
			{ {   0,   0 }, {   0,   0 }, {   0,   0 } },
			0,
			7, "Orange"
		};

		const CardReaderColor colorRank2 =
		{//     Red           Green         Blue
			{ {   0, 255 }, {   0, 192 }, {   0,   0 } },
			{ {   0,   0 }, {   0,   0 }, {   0,   0 } },
			0,
			6, "Yellow"
		};

		const char* pConfigXml(
			"<CardReaderConfig>\n"
			"  <BezelColors>\n"
			"	<BezelColor ColorID=\"0\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Dark</BezelColor>\n"
			"	<BezelColor ColorID=\"1\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">White</BezelColor>\n"
			"	<BezelColor ColorID=\"2\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Red</BezelColor>\n"
			"	<BezelColor ColorID=\"3\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Green</BezelColor>\n"
			"	<BezelColor ColorID=\"4\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Blue</BezelColor>\n"
			"	<BezelColor ColorID=\"5\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Purple</BezelColor>\n"
			"	<BezelColor ColorID=\"6\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"192\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Yellow</BezelColor>\n"
			"	<BezelColor ColorID=\"7\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"64\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Orange</BezelColor>\n"
			"	<BezelColor ColorID=\"8\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Cyan</BezelColor>\n"
			"	<BezelColor ColorID=\"9\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"128\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Teal</BezelColor>\n"
			"	<BezelColor ColorID=\"10\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"128\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Light Blue</BezelColor>\n"
			"	<BezelColor ColorID=\"11\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Lime Green</BezelColor>\n"
			"	<BezelColor ColorID=\"12\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"32\" Blue1Low=\"0\" Blue1High=\"32\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Coral</BezelColor>\n"
			"	<BezelColor ColorID=\"13\" Red1Low=\"0\" Red1High=\"32\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"32\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Light Green</BezelColor>\n"
			"	<BezelColor ColorID=\"14\" Red1Low=\"0\" Red1High=\"64\" Green1Low=\"0\" Green1High=\"32\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Lavender</BezelColor>\n"
			"	<BezelColor ColorID=\"15\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"32\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Red Orange</BezelColor>\n"
			"	<BezelColor ColorID=\"16\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"2\">Flashing Red</BezelColor>\n"
			"   <BezelColor ColorID=\"17\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"192\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"2\">Flashing Yellow</BezelColor>\n"
			"  </BezelColors>\n"
			"  <CardTypeColors>\n"
			"	<CardColor ColorID=\"4\" CardType=\"None\" />\n"
			"	<CardColor ColorID=\"3\" CardType=\"Player\" />\n"
			"	<CardColor ColorID=\"13\" CardType=\"Employee\" />\n"
			"	<CardColor ColorID=\"16\" CardType=\"BadRead\" />\n"
			"   <CardColor ColorID=\"0\" CardType=\"SocialDistance\"/>\n"
			"   <CardColor ColorID=\"17\" CardType=\"CleaningInProgress\"/>\n"
			"  </CardTypeColors>\n"
			"  <RankColors>\n"
			"	<RankColor ColorID=\"2\" RankID=\"0\" />\n"
			"	<RankColor ColorID=\"7\" RankID=\"1\" />\n"
			"	<RankColor ColorID=\"6\" RankID=\"2\" />\n"
			"  </RankColors>\n"
			"</CardReaderConfig>");

		const char* pConfigXmlNoRanks(
			"<CardReaderConfig>\n"
			"  <BezelColors>\n"
			"	<BezelColor ColorID=\"0\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Dark</BezelColor>\n"
			"	<BezelColor ColorID=\"3\" Red1Low =\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Green</BezelColor>\n"
			"	<BezelColor ColorID=\"4\" Red1Low =\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Blue</BezelColor>\n"
			"	<BezelColor ColorID=\"13\" Red1Low =\"0\" Red1High=\"32\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"32\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Light Green</BezelColor>\n"
			"	<BezelColor ColorID=\"16\" Red1Low =\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"2\">Flashing Red</BezelColor>\n"
			"   <BezelColor ColorID=\"17\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"192\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"2\">Flashing Yellow</BezelColor>\n"
			"  </BezelColors>\n"
			"  <CardTypeColors>\n"
			"	<CardColor ColorID=\"4\" CardType=\"None\" />\n"
			"	<CardColor ColorID=\"3\" CardType=\"Player\" />\n"
			"	<CardColor ColorID=\"13\" CardType=\"Employee\" />\n"
			"	<CardColor ColorID=\"16\" CardType=\"BadRead\" />\n"
			"   <CardColor ColorID=\"0\" CardType=\"SocialDistance\"/>\n"
			"   <CardColor ColorID=\"17\" CardType=\"CleaningInProgress\"/>\n"
			"  </CardTypeColors>\n"
			"</CardReaderConfig>");

		const char* pConfigXmlNoRanksDiffEmployee(
			"<CardReaderConfig>\n"
			"  <BezelColors>\n"
			"	<BezelColor ColorID=\"3\" Red1Low =\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Green</BezelColor>\n"
			"	<BezelColor ColorID=\"4\" Red1Low =\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Blue</BezelColor>\n"
			"	<BezelColor ColorID=\"12\" Red1Low =\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"32\" Blue1Low=\"0\" Blue1High=\"32\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Coral</BezelColor>\n"
			"	<BezelColor ColorID=\"16\" Red1Low =\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"2\">Flashing Red</BezelColor>\n"
			"  </BezelColors>\n"
			"  <CardTypeColors>\n"
			"	<CardColor ColorID=\"4\" CardType=\"None\" />\n"
			"	<CardColor ColorID=\"3\" CardType=\"Player\" />\n"
			"	<CardColor ColorID=\"12\" CardType=\"Employee\" />\n"
			"	<CardColor ColorID=\"16\" CardType=\"BadRead\" />\n"
			"  </CardTypeColors>\n"
			"</CardReaderConfig>");

		const char* pConfigXmlNoRanksMissingEmployee(
			"<CardReaderConfig>\n"
			"  <BezelColors>\n"
			"	<BezelColor ColorID=\"3\" Red1Low =\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Green</BezelColor>\n"
			"	<BezelColor ColorID=\"4\" Red1Low =\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Blue</BezelColor>\n"
			"	<BezelColor ColorID=\"16\" Red1Low =\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"2\">Flashing Red</BezelColor>\n"
			"  </BezelColors>\n"
			"  <CardTypeColors>\n"
			"	<CardColor ColorID=\"4\" CardType=\"None\" />\n"
			"	<CardColor ColorID=\"3\" CardType=\"Player\" />\n"
			"	<CardColor ColorID=\"16\" CardType=\"BadRead\" />\n"
			"  </CardTypeColors>\n"
			"</CardReaderConfig>");

		const char* pConfigXmlNoRanksExtraCardTypeColorsElement(
			"<CardReaderConfig>\n"
			"  <BezelColors>\n"
			"	<BezelColor ColorID=\"3\" Red1Low =\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low =\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Green</BezelColor>\n"
			"  </BezelColors>\n"
			"  <CardTypeColors>\n"
			"	<CardColor ColorID=\"3\" CardType=\"None\" />\n"
			"	<CardColor ColorID=\"3\" CardType=\"Player\" />\n"
			"	<CardColor ColorID=\"3\" CardType=\"Employee\" />\n"
			"	<CardColor ColorID=\"3\" CardType=\"BadRead\" />\n"
			"   <CardColor ColorID=\"3\" CardType=\"SocialDistance\"/>\n"
			"   <CardColor ColorID=\"3\" CardType=\"CleaningInProgress\"/>\n"
			"	<CardColor ColorID=\"3\" CardType=\"Future\" />\n"
			"  </CardTypeColors>\n"
			"</CardReaderConfig>");

		const char* pCardReadersXmlNoUicMcr =
			"<CardReaders>\n"
			"  <Vendors>\n"
			"	<Vendor VendorID=\"25426\">UIC</Vendor>\n"
			"	<Vendor VendorID=\"10020\">XST</Vendor>\n"
			"  </Vendors>\n"
			"  <Readers>\n"
			"	<Reader Vendor=\"XST\" ProductID=\"24577\" Type=\"MCR\">XST Magnetic Stripe Card Reader</Reader>\n"
			"	<Reader Vendor=\"UIC\" ProductID=\"9258\" Type=\"RFID\">UIC RFID Card Reader</Reader>\n"
			"  </Readers>\n"
			"</CardReaders>";

		const char* pCardReadersXmlBad =
			"<CardReaders>\n"
			"  <Vendors>\n"
			"	<Vendor VendorID=\"25426\">UIC</Vendor>\n"
			"	<Vendor VendorID=\"10020\">XST</Vendor>\n"
			"  </Vendors>\n"
			"  <Readers>\n"
			"	<Reader Vendor=\"UIC\" ProductID=\"9226\" Type=\"MCR\">UIC Magnetic Stripe Card Reader</Reader>\n"
			"	<Reader Vendor=\"XST\" ProductID=\"24577\" Type=\"MCR\">XST Magnetic Stripe Card Reader</Reader>\n"
			"	<Reader Vendor=\"UIc\" ProductID=\"9258\" Type=\"RFID\">UIC RFID Card Reader</Reader>\n"
			"  </Readers>\n"
			"</CardReaders>";

		void Verify::AreEqual(int line, const CardReaderConfig& expectedConfig, shared_ptr<CardReaderConfig>& actualConfig)
		{
			ASSERT_EQ(expectedConfig.m_cardTypeColors.size(), actualConfig->m_cardTypeColors.size())
				<< AssertMsg::Format(line, L"CardReaderConfig::m_cardTypeColors.size()");


            ASSERT_EQ(expectedConfig.m_rankColors.size(), actualConfig->m_rankColors.size())                    
                << AssertMsg::Format(line, L"CardReaderConfig::m_rankColors.size()");                                                    
                                                                                                             
            ASSERT_TRUE(AreBitEqual(*expectedConfig.m_cardTypeColors[(int) CardReaderCardType::None],           
                *actualConfig->m_cardTypeColors[(int) CardReaderCardType::None]))                               
                << AssertMsg::Format(line, L"CardReaderConfig::m_cardTypeColors[None]");                                                 
                                                                                                                  
            ASSERT_TRUE(AreBitEqual(*expectedConfig.m_cardTypeColors[(int) CardReaderCardType::Player],         
                *actualConfig->m_cardTypeColors[(int) CardReaderCardType::Player]))                             
                << AssertMsg::Format(line, L"CardReaderConfig::m_cardTypeColors[Player]");                                               
                                                                                                              
            ASSERT_TRUE(AreBitEqual(*expectedConfig.m_cardTypeColors[(int) CardReaderCardType::Employee],       
                *actualConfig->m_cardTypeColors[(int) CardReaderCardType::Employee]))                           
                << AssertMsg::Format(line, L"CardReaderConfig::m_cardTypeColors[Employee]");                                             
                                                                                                                 
            ASSERT_TRUE(AreBitEqual(*expectedConfig.m_cardTypeColors[(int) CardReaderCardType::BadRead],        
                *actualConfig->m_cardTypeColors[(int) CardReaderCardType::BadRead]))                            
                << AssertMsg::Format(line, L"CardReaderConfig::m_cardTypeColors[BadRead]");                                              
                                                                                                                 
            for (auto it = expectedConfig.m_rankColors.cbegin(); it != expectedConfig.m_rankColors.cend(); ++it)
            {                                                                                                   
                auto actualRank = actualConfig->GetRankColor(it->first);                                        
                ASSERT_NE(actualRank, nullptr) << L"CardReaderConfig::GetRankColor[" << it->first << L"]";      
                ASSERT_TRUE(AreBitEqual(*it->second, *actualRank))                                              
                    << AssertMsg::Format(line, nullptr, L"CardReaderConfig::GetRankColor[%u]", it->first);                                 
            }                                                                                                   

		}

		void Verify::AreEqual(int line, const char* pExpectedXml, shared_ptr<CardReaderConfig>& actualConfig)
		{
			CardReaderConfig expectedConfig(nullptr, pExpectedXml);
			AreEqual(line, expectedConfig, actualConfig);
		}
	}
