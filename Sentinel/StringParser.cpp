#include "stdafx.h"
#include "StringParser.h"

using namespace std;

CStringParser::CStringParser(const string& stringToParse)
    :   m_stringToParse(stringToParse)
{
}

CStringParser::CStringParser()
{
	m_stringToParse.clear();
}

CStringParser::~CStringParser(void)
{
}

void CStringParser::SetStringToParse(const string& stringToParse)
{
	m_stringToParse = stringToParse;
}

void CStringParser::AddTokenValue(const string& token, const string& value)
{
	m_tokens.push_back(token);
	m_values.push_back(value);
}

string CStringParser::Parse()
{
	string parsedStr;

	for (unsigned int parseIndex = 0; parseIndex < m_stringToParse.length();)
	{
		int tokenIndexFound = -1;
		for (unsigned int tokenIndex = 0; tokenIndex < m_tokens.size() && tokenIndexFound == -1; tokenIndex++)
		{
			if (m_stringToParse.length() - parseIndex >= m_tokens[tokenIndex].size())
			{
				tokenIndexFound = tokenIndex;
				for (unsigned int tokenStringIndex = 0;
					(int)tokenStringIndex < (int)m_tokens[tokenIndex].size() && tokenIndexFound == (int)tokenIndex;
					tokenStringIndex++)
				{
					if (m_stringToParse[parseIndex + tokenStringIndex] != m_tokens[tokenIndex].at(tokenStringIndex))
					{
						tokenIndexFound = -1;
					}
				}
			}
		}

		if (tokenIndexFound == -1)
		{
			parsedStr.push_back(m_stringToParse[parseIndex++]);
		}

		else
		{
			parsedStr += m_values[tokenIndexFound];
			parseIndex += m_tokens[tokenIndexFound].length();
		}
	}

	return parsedStr;
}
