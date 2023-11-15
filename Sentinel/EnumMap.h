#pragma once

#include <stdafx.h>
#include "SentinelException.h"
#include "Hash.h"

///	<summary>
///	A class which maps enum and string values;
///	</summary>
///	<remarks>
/// 
/// enum Numbers
/// {
///		One = 1,
///		Two
///	};
///
///	static EnumMap<Numbers>::EnumPair numbers[] =
///	{
///		{ One,	"one" },
///		{ Two,	"two" }
/// };
///
///	EnumMap<Numbers> nummap(numbers, One);
/// 
///	</remarks>
template<class T_ENUM>
class EnumMap
{
public:
	///	<summary>
	///	Represents an enum value/string pair
	///	</summary>
	struct EnumPair
	{
		T_ENUM _enum;
		const char* pStr;
	};

	typedef SentinelExceptionT<T_ENUM> Exception;

private:
	std::map<const char*, T_ENUM, str_less_than> m_strmap;
	const EnumPair* m_pPairs;
	size_t m_base;

public:
	///	<summary>
	/// EnumMap constructor.
	///	</summary>
	///	<param name="pPairs">An array of EnumPairs with consecutive enumerated values.</param>
	///	<param name="base">The starting value of the enums.</param>
	///	<remarks>
	///	The array must be declared and initialized as follows:
	///		static EnumMap<enum>::EnumPair varname[] = {};
	///	</remarks>
	template <size_t numPairs>
	EnumMap(const EnumPair (&pPairs)[numPairs], T_ENUM base = (T_ENUM) 0)
		: m_pPairs(pPairs)
		, m_base((size_t) base)
	{
		for (size_t i = 0; i < numPairs; i++)
			m_strmap[pPairs[i].pStr] = pPairs[i]._enum;
	}

	///	<summary>
	/// EnumMap destructor.
	///	</summary>
	virtual ~EnumMap()
	{
	}

	///	<summary>
	/// Returns the string associated with the enumerated value.
	///	</summary>
	///	<param name="_enum">The enumerated value.</param>
	///	<param name="pDefaultStr">The value to return if the enumerated value is invalid.</param>
	///	<param name="throwOnError">If <b>true</b> and there is an error, throw an exception.</param>
	///	<returns>The string associated with the enumerated value. If the enumerated
	///		value is invalid, pDefaultStr will be returned.</returns>
	/// <exception cref="SentinelException">Thrown _enum is invalid and throwOnError is true.</exception>
	virtual const char* At(T_ENUM _enum, const char* pDefaultStr = "", bool throwOnError = false) const
	{
		if (((size_t) _enum < m_base || (size_t) _enum >= m_base + m_strmap.size()))
		{
			if (throwOnError)
				throw Exception(_enum, "EnumMap.At(_enum)");

			return pDefaultStr;
		}

		return m_pPairs[(size_t) _enum - m_base].pStr;
	}

	///	<summary>
	/// Returns the string associated with the enumerated value.
	///	</summary>
	///	<param name="_enum">The enumerated value.</param>
	///	<param name="pDefaultStr">The value to return if the enumerated value is invalid.</param>
	///	<returns>The string associated with the enumerated value. If the enumerated
	///		value is invalid, pDefaultStr will be returned.</returns>
	/// <exception cref="SentinelException">Thrown _enum is invalid.</exception>
	const char* AtEx(T_ENUM _enum, const char* pDefaultStr = "") const
	{
		return At(_enum, pDefaultStr, true);
	}

	///	<summary>
	/// Returns the string associated with the enumerated value.
	///	</summary>
	///	<param name="_enum">The enumerated value.</param>
	///	<param name="pDefaultStr">The value if the enumerated value is invalid.</param>
	///	<returns>The string associated with the enumerated value</returns>
	/// <exception cref="SentinelException">Thrown _enum is invalid.</exception>
	const char* operator[](T_ENUM _enum) const
	{
		return AtEx(_enum);
	}

	///	<summary>
	/// Returns the enumeration associated with the string.
	///	</summary>
	///	<param name="pStr">The string.</param>
	///	<param name="defaultEnum">The value if pStr is invalid.</param>
	///	<param name="throwOnError">If <b>true</b> and there is an error, throw an exception.</param>
	///	<returns>The enumeration associated with the string.</returns>
	/// <exception cref="SentinelException">Thrown _enum is invalid and throwOnError is true.</exception>
	virtual T_ENUM At(const char* pStr, T_ENUM defaultEnum = (T_ENUM) -1, bool throwOnError = false) const
	{
		if (pStr != nullptr && *pStr != 0)
		{
			auto it = m_strmap.find(pStr);
			if (it != m_strmap.end())
				return it->second;
		}

		if (throwOnError)
			throw Exception(pStr);

		return defaultEnum;
	}

	///	<summary>
	/// Returns the enumeration associated with the string.
	///	</summary>
	///	<param name="pStr">The string.</param>
	///	<param name="defaultEnum">The value if pStr is invalid.</param>
	///	<returns>The enumeration associated with the string.</returns>
	/// <exception cref="SentinelException">Thrown _enum is invalid.</exception>
	T_ENUM AtEx(const char* pStr, T_ENUM defaultEnum = (T_ENUM) -1) const
	{
		return At(pStr, defaultEnum, true);
	}

	///	<summary>
	/// Returns the enumeration associated with the string.
	///	</summary>
	///	<param name="pStr">The string.</param>
	/// <exception cref="SentinelException">Thrown _enum is invalid.</exception>
	T_ENUM operator[](const char* pStr) const
	{
		return AtEx(pStr);
	}

	///	<summary>
	///	Indicates whether the map contains the enumeration.
	///	</summary>
	///	<param name="pStr">The enumeration</param>
	///	<returns><b>true</b> if enumeration is in the map, else <b>false</b>.</returns>
	virtual bool Has(T_ENUM _enum) const
	{
		return (size_t) _enum >= m_base && (size_t) _enum <= m_base + m_strmap.size();
	}

	///	<summary>
	///	Indicates whether the map contains pStr.
	///	</summary>
	///	<param name="pStr">The string</param>
	///	<returns><b>true</b> if pStr is in the map, else <b>false</b>.</returns>
	bool Has(const char* pStr) const
	{
		return m_strmap.find(pStr) != m_strmap.end();
	}

private:
	EnumMap(const EnumMap&);
	void operator=(const EnumMap&);
};

///	<summary>
///	A class which maps non-consecutive enum and string values;
///	</summary>
///	<remarks>
/// 
/// enum Numbers
/// {
///		One = 1,
///		Six = 6
///	};
///
///	static EnumMap<Numbers>::EnumPair numbers[] =
///	{
///		{ One,	"one" },
///		{ Six,	"six" }
/// };
///
///	EnumSparseMap nummap(numbers);
/// 
///	</remarks>
template<class T_ENUM>
class EnumSparseMap : public EnumMap<T_ENUM>
{
private:
	std::map<T_ENUM, const char*> m_enummap;

public:
	///	<summary>
	/// EnumSparseMap constructor.
	///	</summary>
	///	<param name="pPairs">An array of EnumPairs with non-consecutive enumerated values.</param>
	///	<remarks>
	///	The array must be declared and initialized as follows:
	///		static EnumSparseMap<enum>::EnumPair varname[] = {};
	///	</remarks>
	template <size_t numPairs>
	EnumSparseMap(const typename EnumMap<T_ENUM>::EnumPair (&pPairs)[numPairs])
		: EnumMap<T_ENUM>(pPairs)
	{
		for (size_t i = 0; i < numPairs; i++)
			m_enummap[pPairs[i]._enum] = pPairs[i].pStr;
	}

	///	<summary>
	/// EnumSparseMap destructor.
	///	</summary>
	virtual ~EnumSparseMap()
	{
	}

	///	<summary>
	/// Returns the string associated with the enumerated value.
	///	</summary>
	///	<param name="_enum">The enumerated value.</param>
	///	<param name="pDefaultStr">The value if _enum is invalid.</param>
	///	<param name="throwOnError">If <b>true</b> and there is an error, throw an exception.</param>
	///	<returns>The string associated with the enumerated value</returns>
	/// <exception cref="SentinelException">Thrown if _enum is invalid.</exception>
	virtual const char* At(T_ENUM _enum, const char* pDefaultStr = "", bool throwOnError = false) const
	{
		auto it = m_enummap.find(_enum);
		if (it != m_enummap.end())
			return it->second;

		if (throwOnError)
			throw typename EnumMap<T_ENUM>::Exception(_enum);

		return pDefaultStr;
	}

	///	<summary>
	/// Returns the enumeration associated with the string.
	///	</summary>
	///	<param name="pStr">The string.</param>
	///	<param name="defaultEnum">The value if pStr is invalid.</param>
	///	<param name="throwOnError">If <b>true</b> and there is an error, throw an exception.</param>
	///	<returns>The enumeration associated with the string.</returns>
	/// <exception cref="SentinelException">Thrown _enum is invalid and throwOnError is true.</exception>
	virtual T_ENUM At(const char* pStr, T_ENUM defaultEnum = (T_ENUM) -1, bool throwOnError = false) const
	{
		return EnumMap<T_ENUM>::At(pStr, defaultEnum, throwOnError);
	}

	///	<summary>
	///	Indicates whether the map contains the enumeration.
	///	</summary>
	///	<param name="pStr">The enumeration</param>
	///	<returns><b>true</b> if enumeration is in the map, else <b>false</b>.</returns>
	virtual bool Contains(T_ENUM _enum) const
	{
		return m_enummap.find(_enum) != m_enummap.end();
	}

private:
	EnumSparseMap(const EnumSparseMap&);
	void operator=(const EnumSparseMap&);
};

