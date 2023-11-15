#pragma once
#include <stdafx.h>
/*
These are hashes and predicates which can be used by various STL map classes.
The are maintained entirely in the header for clarity.

str_equal_to and str_hash_sdbm are tested in EnumMap and CardReader tests.
*/

///	<summary>
/// A binary predicate to compare two strings.
///	</summary>
struct str_equal_to 
{
	///	<summary>
	/// The predicate operator.
	///	</summary>
	///	<param name="p1">The first string to compare.</param>
	///	<param name="p2">The second string to compare.</param>
	///	<returns><b>true</b> if strings are equal.</returns>
	///	<remarks>
	/// This is case sensitive.
	///	</remarks>
	bool operator()(const char* p1, const char* p2) const
	{
		return strcmp(p1, p2) == 0;
	}
};

///	<summary>
/// A binary predicate to compare two strings.
///	</summary>
struct wstr_equal_to 
{
	///	<summary>
	/// The predicate operator.
	///	</summary>
	///	<param name="p1">The first string to compare.</param>
	///	<param name="p2">The second string to compare.</param>
	///	<returns><b>true</b> if strings are equal.</returns>
	///	<remarks>
	/// This is case sensitive.
	///	</remarks>
	bool operator()(const wchar_t* p1, const wchar_t* p2) const
	{
		return wcscmp(p1, p2) == 0;
	}
};

///	<summary>
/// A binary predicate to compare two strings.
///	</summary>
struct str_less_than
{
	///	<summary>
	/// The predicate operator.
	///	</summary>
	///	<param name="p1">The first string to compare.</param>
	///	<param name="p2">The second string to compare.</param>
	///	<returns><b>true</b> if p1 is less than p2.</returns>
	///	<remarks>
	/// This is case sensitive.
	///	</remarks>
	bool operator()(const char* p1, const char* p2) const
	{
		return strcmp(p1, p2) < 0;
	}
};

///	<summary>
/// A binary predicate to compare two strings ignoring case.
///	</summary>
struct str_less_than_nocase
{
	bool operator() (const std::string& s1, const std::string& s2) const
	{
		return strcasecmp(s1.c_str(), s2.c_str()) < 0;
	}
};


///	<summary>
/// A binary predicate to compare two strings.
///	</summary>
struct wstr_less_than
{
	///	<summary>
	/// The predicate operator.
	///	</summary>
	///	<param name="p1">The first string to compare.</param>
	///	<param name="p2">The second string to compare.</param>
	///	<returns><b>true</b> if p1 is less than p2.</returns>
	///	<remarks>
	/// This is case sensitive.
	///	</remarks>
	bool operator()(const wchar_t* p1, const wchar_t* p2) const
	{
		return wcscmp(p1, p2) < 0;
	}
};

///	<summary>
/// A simple unary hash for a const char*.
///	</summary>
///	<remarks>
/// This is a very fast, commonly used public domain hash.
///	</remarks>
struct str_hash_sdbm
{
	///	<summary>
	/// The unary hash operator.
	///	</summary>
	///	<param name="pStr">The string to hash.</param>
	///	<returns>The hash of pStr.</returns>
	int operator()(const char* pStr) const
	{
		int hash = 0;
		while (*pStr)
			hash = *pStr++ + (hash << 6) + (hash << 16) - hash;

		return hash & INT_MAX;
	}
};

///	<summary>
/// A simple unary hash for a const wchar_t*.
///	</summary>
///	<remarks>
/// This is a very fast, commonly used public domain hash.
///	</remarks>
struct wstr_hash_sdbm
{
	///	<summary>
	/// The unary hash operator.
	///	</summary>
	///	<param name="pStr">The string to hash.</param>
	///	<returns>The hash of pStr.</returns>
	int operator()(const wchar_t* pStr) const
	{
		int hash = 0;
		while (*pStr)
			hash = *pStr++ + (hash << 6) + (hash << 16) - hash;

		return hash & INT_MAX;
	}
};
