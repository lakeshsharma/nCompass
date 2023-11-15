#pragma once

class CStringParser
{
public:
	CStringParser(const std::string& stringToParse);
	CStringParser();
	~CStringParser();

	void SetStringToParse(const std::string& stringToParse);

	void AddTokenValue(const std::string& token, const std::string& value);

	std::string Parse();

private:
	std::string m_stringToParse;
	std::vector<std::string> m_tokens;
	std::vector<std::string> m_values;
};
