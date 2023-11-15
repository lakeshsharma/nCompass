#include "stdafx.h"
#include "StringParser.h"


#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
		TEST(StringParserUnitTests, StringParserParseDefaultConstructorUnitTest)
		{
			CStringParser *parser = new CStringParser();

			parser->AddTokenValue("{token1}", "token_1_value");
			parser->AddTokenValue("{token2}", "token_2_value");
			parser->AddTokenValue("{token3}", "token_3_value");

			parser->SetStringToParse("Parsing string looking for {token1} {token2} and token3");

			string strResult = parser->Parse();
			ASSERT_TRUE(strResult == "Parsing string looking for token_1_value token_2_value and token3") << "String parsing incorrect";
			delete parser;

		}

		TEST(StringParserUnitTests, StringParserParseStringInitConstructorUnitTest)
		{
			CStringParser *parser = new CStringParser("Parsing string looking for {token1} {token2} and token3");

			parser->AddTokenValue("{token1}", "token_1_value");
			parser->AddTokenValue("{token2}", "token_2_value");
			parser->AddTokenValue("{token3}", "token_3_value");

			string strResult = parser->Parse();
			ASSERT_TRUE(strResult == "Parsing string looking for token_1_value token_2_value and token3") << "String parsing incorrect";
			delete parser;
		}

		TEST(StringParserUnitTests, StringParserParseNonMatchingTokensUnitTest)
		{
			CStringParser *parser = new CStringParser("Parsing string looking for {token11} {token21} and token3");

			parser->AddTokenValue("{token1}", "token_1_value");
			parser->AddTokenValue("{token2}", "token_2_value");
			parser->AddTokenValue("{token3}", "token_3_value");

			string strResult = parser->Parse();
			ASSERT_TRUE(strResult == "Parsing string looking for {token11} {token21} and token3") << "String parsing incorrect";
			delete parser;
		}

		TEST(StringParserUnitTests, StringParserParseNoTokensAndEmptyStringUnitTest)
		{
			CStringParser *parser = new CStringParser("Parsing string looking for {token11} {token21} and token3");

			string strResult = parser->Parse();
			ASSERT_TRUE(strResult == "Parsing string looking for {token11} {token21} and token3") << "String parsing incorrect";

			parser->SetStringToParse("");
			parser->AddTokenValue("{token1}" ,"token_1_value");
			parser->AddTokenValue("{token2}", "token_2_value");
			parser->AddTokenValue("{token3}", "token_3_value");
			strResult = parser->Parse();
			ASSERT_TRUE(strResult == "") << "Non empty string returned";
			delete parser;
		}
}
