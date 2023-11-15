#include "stdafx.h"
#include "CardReader.h"
#include "Utilities.h"
#include "Util/Convert.h"
#include "DirectoryManager.h"
#include "../../CardReaderLayer/CardReaderLayerAPI.h"

using namespace std;
using namespace pugi;

// This is needed for testing
struct
{
	LPCSTR pVendor;
	CardReaderType::Type type;
	WORD vendorId;
	WORD productId;
}
static defaultReaders[] =
{
	{ "UIC", CardReaderType::MCR,  USB_VENDOR_ID_UIC, USB_PRODUCT_ID_UIC_CRDRDR1 },
	{ "XST", CardReaderType::MCR,  USB_VENDOR_ID_XST, USB_PRODUCT_ID_XST_CRDRDR1 },
	{ "UIC", CardReaderType::RFID, USB_VENDOR_ID_UIC, USB_PRODUCT_ID_UIC_RFID    }
};

namespace CardReaderType
{
	static EnumMap<Type>::EnumPair types[] =
	{
		{ MCR, pMCR },
		{ RFID, pRFID }
	};

	EnumMap<Type> emap(types);
}

CardReader::CardReader()
{
}

CardReader::CardReader(const CardReader& other)
{
	*this = other;
}

CardReader& CardReader::operator=(const CardReader& other)
{
	if (this != &other)
	{
		m_vendor = other.m_vendor;
		m_type = other.m_type;
		m_vendorId = other.m_vendorId;
		m_productId = other.m_productId;
	}
	return *this;
}

WORD CardReader::GetVendorId() const
{
	return m_vendorId;
}

WORD CardReader::GetProductId() const
{
	return m_productId;
}

CardReaderType::Type CardReader::GetType() const
{
	return m_type;
}

LPCSTR CardReader::GetVendor() const
{
	return m_vendor.c_str();
}

vector<CardReader> CardReader::GetCardReaders(string& pathname)
{
	vector<CardReader> readers;
	try
	{
		std::string tmpPathname = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + pCardReaderFilename;
		xml_document xmlDoc;
		xml_parse_result result = xmlDoc.load_file(tmpPathname.c_str());
		if (!result)
		{
			tmpPathname = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_APPLICATIONS) + pCardReaderFilename;
			result = xmlDoc.load_file(tmpPathname.c_str());
			if (!result)
				throw exception();
		}

		map<const char*, WORD, str_less_than> vendorMap;

		xml_node node = xmlDoc.child("CardReaders").child("Vendors");
		for_each(node.begin(), node.end(), [&](const xml_node& node)//xml_node_iterator it)
		{
			vendorMap[node.child_value()] = (WORD) node.attribute("VendorID").as_uint();
		});

		node = xmlDoc.child("CardReaders").child("Readers");
		for_each(node.begin(), node.end(), [&](const xml_node& node)//xml_node_iterator it)
		{
			CardReader reader;

			const char* pVendor = node.attribute("Vendor").as_string();
			reader.m_vendor = pVendor;
			reader.m_vendorId = vendorMap.at(pVendor);
			reader.m_productId = (WORD) node.attribute("ProductID").as_uint();
			reader.m_type = CardReaderType::emap[node.attribute("Type").as_string()];

			readers.push_back(move(reader));
		});

		pathname = tmpPathname;
	}
	catch (...)
	{
		readers.clear();

		for (size_t i = 0; i < _countof(defaultReaders); i++)
		{
			CardReader reader;

			reader.m_vendor = defaultReaders[i].pVendor;
			reader.m_type = defaultReaders[i].type;
			reader.m_vendorId = defaultReaders[i].vendorId;
			reader.m_productId = defaultReaders[i].productId;

			readers.push_back(move(reader));
		}

		pathname = pCardReaderMemoryFilepath;
	}
	return readers;
}

