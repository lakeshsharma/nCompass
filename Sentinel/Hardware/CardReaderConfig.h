#pragma once
#include "EnumMap.h"
#include "../../CardReaderLayer/CardReaderLayerAPI.h"
#include "DirectoryManager.h"


class CardReaderConfigTests;
namespace CardReaderTestHelpers
{
	struct Verify;
}


std::string pCardReaderConfigFilename __attribute__ ((weak)) = "/CardReaderConfig.xml";

#pragma pack(push, 1)
struct CardReaderColor
{
	struct channel
	{
		byte lsb;
		byte msb;
	};

	struct rgb
	{
		channel r;
		channel g;
		channel b;

		void WriteHtml(std::ostream& os) const;
	};

	rgb color1;
	rgb color2;
	byte rate;

	static const int HidDataLen = 13;

	int64_t id;
	std::string name;

	bool CompareHidData(const CardReaderColor& other) const;
	bool operator==(const CardReaderColor& other) const;

	void WriteHtml(std::ostream& os, const char* pType) const;
};
#pragma pack(pop)

namespace CardReaderCardType
{
	const char* pNone __attribute__ ((weak))		= "None";
	const char* pPlayer	__attribute__ ((weak)) = "Player";
	const char* pEmployee __attribute__ ((weak)) = "Employee";
	const char* pBadRead __attribute__ ((weak)) = "BadRead";
	const char* pSocialDistance __attribute__((weak)) = "SocialDistance";
	const char* pCleaningInProgress __attribute__((weak)) = "CleaningInProgress";

	enum Type
	{
		None,
		Player,
		Employee,
		BadRead,
		SocialDistance,
		CleaningInProgress,
		__last__
	};
	extern EnumMap<Type> emap;
}

class CardReaderConfig
{
private:
	mutable std::vector<std::shared_ptr<CardReaderColor>> m_cardTypeColors;
	mutable std::map<int, std::shared_ptr<CardReaderColor>> m_rankColors;
	std::string m_pathname;

public:
	///	<summary>
	/// Constructor.
	///	</summary>
	/// <param name="pathname">The pathname of the config
	/// xml.</param>
	/// <param name="pBuffer">If pathname is nullptr and this is valid, parse this
	///		buffer of XML content. If this is also nullptr, parse the default XML.</param>
	/// <exception cref="SentinelException">Thrown when load attempts fail and 
	///		<paramref name="createDefault"/> if <b>false</b>.</exception>
	CardReaderConfig(std::string pathname, const char* pBuffer = nullptr);

	///	<summary>
	/// Destructor.
	///	</summary>
	virtual ~CardReaderConfig();

	/// <summary>
	/// Returns the color of the bezel.
	/// </summary>
	/// <param name="type">The CardReaderCardType::Type.</param>
	/// <returns>A pointer to the color of the bezel.</returns>
	const CardReaderColor* GetCardTypeColor(CardReaderCardType::Type type) const;

	/// <summary>
	/// Returns the color of the bezel.
	/// </summary>
	/// <param name="rankID">The rank ID.</param>
	/// <returns>A pointer to the color of the bezel.</returns>
	const CardReaderColor* GetRankColor(int rankId) const;

	///	<summary>
	/// Writes HTML.
	///	</summary>
	/// <param name="os">The ostream to write to.</param>
	///	<remarks>
	/// This is implemented in ProcessDump\WebDiagnosticsCardReader.cpp
	///	</remarks>
	void WriteHtml(std::ostream& os) const;

private:
	CardReaderConfig(const CardReaderConfig&);
	void operator=(const CardReaderConfig&);

	friend CardReaderConfigTests;
	friend CardReaderTestHelpers::Verify;
};

