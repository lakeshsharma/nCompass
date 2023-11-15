#pragma once


#include "CardReader.h"
#include "EnumMap.h"

std::string pCardReaderFilename __attribute__ ((weak)) = _T("/CardReaders.xml");
std::string pCardReaderMemoryFilepath __attribute__ ((weak)) = "Memory";

namespace CardReaderType
{
	const char* pMCR __attribute__ ((weak)) = "MCR";
	const char* pRFID __attribute__ ((weak)) = "RFID";

	enum Type
	{
		MCR,
		RFID
	};
	extern EnumMap<Type> emap;
}

class CardReader
{
private:
	std::string m_vendor;
	CardReaderType::Type m_type;
	WORD m_vendorId;
	WORD m_productId;

public:
	///	<summary>
	/// Constructor.
	///	</summary>
	///	<remarks>
	/// This does nothing. Data members remain uninitialized.
	///	</remarks>
	CardReader();

	///	<summary>
	/// Copy constructor.
	///	</summary>
	///	<param name="other">The object to copy.</param>
	CardReader(const CardReader& other);

	///	<summary>
	/// Assignment operator.
	///	</summary>
	///	<param name="other">The object to copy.</param>
	///	<returns>A reference to this instance.</param>
	CardReader& operator=(const CardReader& other);

	///	<summary>
	///	Returns the vendor ID.
	///	</summary>
	///	<returns>The vendor ID.</returns>
	WORD GetVendorId() const;

	///	<summary>
	///	Returns the product ID.
	///	</summary>
	///	<returns>The product ID.</returns>
	WORD GetProductId() const;

	///	<summary>
	///	Returns the card reader type.
	///	</summary>
	///	<returns>The card reader type.</returns>
	CardReaderType::Type GetType() const;

	///	<summary>
	///	Returns the vendor string.
	///	</summary>
	///	<returns>The vendor string.</returns>
	LPCSTR GetVendor() const;

	///	<summary>
	///	Returns a vector of the CardReaders found in CardReaders.xml.
	///	</summary>
	///	<param name="pathname">The pathname of the xml file from which the data was
	///		obtained. If the data is obtained from memory, pCardReaderMemoryFilepath
	///		will be used.</param>
	///	<returns>A vector of the CardReaders found in CardReaders.xml.</returns>
	static std::vector<CardReader> GetCardReaders(std::string& pathname);

	///	<summary>
	/// Writes HTML.
	///	</summary>
	/// <param name="os">The ostream to write to.</param>
	///	<param name="pathname">The pathname of the xml file from which the data was
	///		obtained. If the data is obtained from memory, pCardReaderMemoryFilepath
	///		will be used.</param>
	///	<remarks>
	/// This is implemented in ProcessDump\WebDiagnosticsCardReader.cpp
	///	</remarks>
	void WriteHtml(std::ostream& os, const std::string& pathname) const;
};

