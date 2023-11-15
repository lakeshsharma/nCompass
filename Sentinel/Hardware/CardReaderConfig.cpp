#include "stdafx.h"
#include "Util/Convert.h"
#include "CardReader.h"
#include "CardReaderConfig.h"

using namespace pugi;
using namespace std;

bool CardReaderColor::CompareHidData(const CardReaderColor& other) const
{
	return memcmp(this, &other, HidDataLen) == 0;
}

bool CardReaderColor::operator==(const CardReaderColor& other) const
{
	return CompareHidData(other) && id == other.id && name == other.name;
}

namespace CardReaderCardType
{
	static EnumMap<Type>::EnumPair types[] =
	{
		{ None		, pNone	    },
		{ Player	, pPlayer	},
		{ Employee	, pEmployee },
		{ BadRead	, pBadRead	},
		{ SocialDistance, pSocialDistance },
		{ CleaningInProgress , pCleaningInProgress }
	};

	EnumMap<Type> emap(types);

	static const char* pDefault =
		"<CardReaderConfig>"
			"<BezelColors>"
				"<BezelColor ColorID=\"0\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Dark</BezelColor>"
				"<BezelColor ColorID=\"3\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"255\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Green</BezelColor>"
				"<BezelColor ColorID=\"4\" Red1Low=\"0\" Red1High=\"0\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"255\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"0\">Blue</BezelColor>"
				"<BezelColor ColorID=\"16\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"0\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"2\">Flashing Red</BezelColor>"
				"<BezelColor ColorID=\"17\" Red1Low=\"0\" Red1High=\"255\" Green1Low=\"0\" Green1High=\"192\" Blue1Low=\"0\" Blue1High=\"0\" Red2Low=\"0\" Red2High=\"0\" Green2Low=\"0\" Green2High=\"0\" Blue2Low=\"0\" Blue2High=\"0\" Rate=\"2\">Flashing Yellow</BezelColor>"
			"</BezelColors>"
			"<CardTypeColors>"
				"<CardColor ColorID=\"4\" CardType=\"None\" />"
				"<CardColor ColorID=\"3\" CardType=\"Player\" />"
				"<CardColor ColorID=\"3\" CardType=\"Employee\" />"
				"<CardColor ColorID=\"16\" CardType=\"BadRead\" />"
				"<CardColor ColorID=\"0\" CardType=\"SocialDistance\" />"
				"<CardColor ColorID=\"17\" CardType=\"CleaningInProgress\" />"
			"</CardTypeColors>"
		"</CardReaderConfig>";
}

static const char* pDefaultFilename = "Used Internal Default";
static const char* pBufferFilename = "Used Buffer";

CardReaderConfig::CardReaderConfig(std::string pathname, const char* pBuffer)
{
	m_cardTypeColors.resize(CardReaderCardType::__last__);

	xml_document xmlDoc;
	xml_parse_result result;

	if (!pathname.empty())
	{
		result = xmlDoc.load_file(pathname.c_str());
		m_pathname = pathname;
	}
	else
	{
		if (pBuffer == nullptr)
		{
			pBuffer = CardReaderCardType::pDefault;
			m_pathname = pDefaultFilename;
		}
		else
		{
			m_pathname = pBufferFilename;
		}

		result = xmlDoc.load_buffer(pBuffer, strlen(pBuffer));
	}

	if (!result)
		throw SentinelException();

	map<int64_t, shared_ptr<CardReaderColor>> bezelColors;

	xml_node node = xmlDoc.child("CardReaderConfig").child("BezelColors");
	for_each(node.begin(), node.end(), [&bezelColors](const xml_node& node)
	{
		auto colorId = node.attribute("ColorID").as_llong();
		if (colorId >= 0 && colorId <= BYTE_MAX)
		{
			shared_ptr<CardReaderColor> color(new CardReaderColor);

			color->color1.r.lsb = (byte) node.attribute("Red1Low").as_uint();
			color->color1.r.msb = (byte) node.attribute("Red1High").as_uint();
			color->color1.g.lsb = (byte) node.attribute("Green1Low").as_uint();
			color->color1.g.msb = (byte) node.attribute("Green1High").as_uint();
			color->color1.b.lsb = (byte) node.attribute("Blue1Low").as_uint();
			color->color1.b.msb = (byte) node.attribute("Blue1High").as_uint();
			color->color2.r.lsb = (byte) node.attribute("Red2Low").as_uint();
			color->color2.r.msb = (byte) node.attribute("Red2High").as_uint();
			color->color2.g.lsb = (byte) node.attribute("Green2Low").as_uint();
			color->color2.g.msb = (byte) node.attribute("Green2High").as_uint();
			color->color2.b.lsb = (byte) node.attribute("Blue2Low").as_uint();
			color->color2.b.msb = (byte) node.attribute("Blue2High").as_uint();

			color->rate = (byte) node.attribute("Rate").as_uint();

			color->id = colorId;
			color->name = node.text().as_string();
			bezelColors[color->id] = color;
		}
	});

	node = xmlDoc.child("CardReaderConfig").child("CardTypeColors");
	for_each(node.begin(), node.end(), [this, &bezelColors](const xml_node& node)
	{
		auto colorId = node.attribute("ColorID").as_llong();
		if (colorId >= 0 && colorId <= BYTE_MAX)
		{
			auto pType = node.attribute("CardType").as_string();
			CardReaderCardType::Type type = CardReaderCardType::emap.At(pType, CardReaderCardType::__last__, false);
			if (type != CardReaderCardType::__last__)
			{
				// Ensure a CardType entry doesn't already exists.
				if (m_cardTypeColors[(int) type])
					throw SentinelException();

				m_cardTypeColors[(int) type] = bezelColors.at(colorId);
			}
		}
	});

	// Verify that all CardReaderCardType entries are there
	for (int i = 0; i < (int) CardReaderCardType::__last__; i++)
	{
		if (!m_cardTypeColors[i])
			throw SentinelException();
	}

	node = xmlDoc.child("CardReaderConfig").child("RankColors");
	for_each(node.begin(), node.end(), [this, &bezelColors](const xml_node& node)
	{
		auto colorId = node.attribute("ColorID").as_llong();
		if (colorId >= 0 && colorId <= BYTE_MAX)
		{
			auto rank = node.attribute("RankID").as_int();

			m_rankColors[rank] = bezelColors.at(colorId);
		}
	});
}

CardReaderConfig::~CardReaderConfig()
{
}

const CardReaderColor* CardReaderConfig::GetCardTypeColor(CardReaderCardType::Type type) const
{
	return (size_t) type < m_cardTypeColors.size() ? m_cardTypeColors[type].get() : nullptr;
}

const CardReaderColor* CardReaderConfig::GetRankColor(int rankId) const
{
	const CardReaderColor* pColor(nullptr);
	
	auto rankColorIter = m_rankColors.find(rankId);
	if (rankColorIter != m_rankColors.end())
	{
		pColor = rankColorIter->second.get();
	}

	return pColor;
}

