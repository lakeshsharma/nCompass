#include "stdafx.h"
#include "Convert.h"
#include "Currency.h"
#include "GlobalDefs.h"

using namespace std;

namespace Currency
{
	bool ToInt64(const char* pStr, int64_t& val)
	{
		bool err = true;
		int64_t cents = 0;

		if (pStr != nullptr && *pStr != 0)
		{
			err = false;
			const char* pEnd;
			if (!Convert::ToInt64(pStr, val, &pEnd))
			{
				if (*pEnd == '.')
				{
					if (isdigit(*++pEnd))
					{
						cents = (*pEnd - '0') * Convert::BaseTen;
						if (isdigit(*++pEnd))
							cents += *pEnd++ - '0';
					}
				}
				err = *pEnd != 0;
			}
		}
		else
		{
			val = 0;
		}

		val = (val * OneDollarInCents) + (val < 0 ? -cents : cents);

		return !err;
	}

	int64_t ToInt64(double val)
	{
		return val >= 0.0 ? (int64_t) ((val * OneDollarInCents) + 0.5) : (int64_t) ((val * OneDollarInCents) - 0.5);
	}

	const size_t MaxCurrencyStringLen = 256;

	string ToString(double val)
	{
		stringstream stream;
		stream << fixed << setprecision(2) << val;
		return stream.str();
	}

	string ToString(int64_t val)
	{
		stringstream stream;
		stream << val / OneDollarInCents << ".";
		stream << fixed << setprecision(2) << abs(val) % OneDollarInCents;
		return stream.str();
	}

	double Int64ToCurrency(int64_t val)
	{
		return (double) val / OneDollarInCents;
	}
}
