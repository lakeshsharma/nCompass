#pragma once

// Do not put any additional headers in this file.

///	<summary>
///	Currency functions.
///	</summary>
namespace Currency
{
	const unsigned OneDollarInCents =
#ifdef ONE_DOLLAR_IN_CENTS
		ONE_DOLLAR_IN_CENTS;
#else
		100U;
#endif

	///	<summary>
	///	Converts a string to currency in pennies.
	///	</summary>
	///	<param name="pStr">The string to convert.</param>
	///	<param name="val">The value calculated until the error.</param>
	///	<returns><b>true</b> if conversion was successful, else <b>false</b>.</returns>
	bool ToInt64(const char* pStr, int64_t& val);

	///	<summary>
	///	Converts a currency in dollars to pennies.
	///	</summary>
	///	<param name="pStr">The value to convert.</param>
	///	<returns>Returns value</returns>
	/// <remarks>This will round to the nearest cent.</remarks>
	int64_t ToInt64(double val);

	///	<summary>
	///	Converts currency to a string.
	///	</summary>
	///	<param name="pStr">The value to convert.</param>
	///	<returns>Returns value as a string.</returns>
	/// <remarks>This will round to the nearest cent.</remarks>
	std::string ToString(double val);

	///	<summary>
	///	Converts a currency in pennies to a string in dollars.
	///	</summary>
	///	<param name="pStr">The value to convert.</param>
	///	<returns>Returns value as a string.</returns>
	/// <remarks>This will round to the nearest cent.</remarks>
	std::string ToString(int64_t val);

	///	<summary>
	///	Converts a currency in pennies to a string in dollars.
	///	</summary>
	///	<param name="val">The value to convert.</param>
	///	<returns>Returns value as a string.</returns>
	/// <remarks>This will round to the nearest cent.</remarks>
	double Int64ToCurrency(int64_t val);
}
