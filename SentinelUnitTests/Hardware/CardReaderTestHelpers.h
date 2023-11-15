#pragma once
#include "Hardware/CardReaderConfig.h"
#include "TestUtil.h"


namespace CardReaderTestHelpers
    {
    	extern const CardReaderColor colorNone;
    	extern const CardReaderColor colorPlayer;
    	extern const CardReaderColor colorEmployee;
    	extern const CardReaderColor colorEmployeeDefault;
    	extern const CardReaderColor colorBadRead;

    	extern const CardReaderColor colorRank0;
    	extern const CardReaderColor colorRank1;
    	extern const CardReaderColor colorRank2;

    	extern const char* pConfigXml;
    	extern const char* pConfigXmlNoRanks;
    	extern const char* pConfigXmlNoRanksDiffEmployee;
    	extern const char* pConfigXmlNoRanksMissingEmployee;
    	extern const char* pConfigXmlNoRanksExtraCardTypeColorsElement;
    	extern const char* pCardReadersXmlNoUicMcr;
    	extern const char* pCardReadersXmlBad;

    	struct Verify
    	{
    		static void AreEqual(int line, const CardReaderConfig& expectedConfig, std::shared_ptr<CardReaderConfig>& actualConfig);
    		static void AreEqual(int line, const char* pExpectedXml, std::shared_ptr<CardReaderConfig>& actualConfig);
    	};
    }

