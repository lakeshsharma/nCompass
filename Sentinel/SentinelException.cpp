#include "stdafx.h"
#include "SentinelException.h"
#include "Utilities.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// SentinelException

SentinelException::SentinelException(int code, const char* pMsg) throw()
	: runtime_error(pMsg != nullptr ? pMsg : "")
	, m_code(code)
{
}

SentinelException::SentinelException(const char* pMsg) throw()
	: runtime_error(pMsg != nullptr ? pMsg : "")
	, m_code(CUtilities::GetLastError())
{
}

SentinelException::SentinelException(const char* pMsg, int code) throw()
	: runtime_error(pMsg != nullptr ? pMsg : "")
	, m_code(code)
{
}

SentinelException::SentinelException(const SentinelException& other) throw()
	: runtime_error(other)
	, m_code(other.m_code)
{
}

SentinelException& SentinelException::operator=(const SentinelException& other) throw()
{
	if (this != &other)
	{
		exception::operator=(other);
		m_code = other.m_code;
	}
	return *this;
}

int SentinelException::code() const throw()
{
	return m_code;
}

bool SentinelException::HasNonZeroData(bool all) const throw()
{
	return all ? code() != 0 && *what() != 0 : code() != 0 || *what() != 0;
}

///////////////////////////////////////////////////////////////////////////
// SentinelCreateThreadException

SentinelCreateThreadException::SentinelCreateThreadException(const char* pMsg) throw()
	: SentinelException(CUtilities::GetLastError(), pMsg)
{
}

SentinelCreateThreadException::SentinelCreateThreadException(const char* pMsg, int code) throw()
	: SentinelException(pMsg, code)
{
}

SentinelCreateThreadException::SentinelCreateThreadException(const SentinelCreateThreadException& other) throw()
	: SentinelException(other)
{
}

SentinelCreateThreadException& SentinelCreateThreadException::operator=(const SentinelCreateThreadException& other) throw()
{
	SentinelException::operator=(other);
	return *this;
}

///////////////////////////////////////////////////////////////////////////
// SentinelCancelException

SentinelCancelException::SentinelCancelException(int code, const char* pMsg) throw()
	: SentinelException(code, pMsg)
{
}

SentinelCancelException::SentinelCancelException(const char* pMsg, int code) throw()
	: SentinelException(pMsg, code)
{
}

SentinelCancelException::SentinelCancelException(const SentinelCancelException& other) throw()
	: SentinelException(other)
{
}

SentinelCancelException& SentinelCancelException::operator=(const SentinelCancelException& other) throw()
{
	SentinelException::operator=(other);
	return *this;
}

///////////////////////////////////////////////////////////////////////////
// SentinelFileException

void SentinelFileException::SetFilename(const char* pFilename)
{
    if (m_pFilename) {
        delete[] m_pFilename;
        m_pFilename = nullptr;
    }

	if (pFilename != nullptr)
	{
		size_t len = strlen(pFilename) + 1;
		m_pFilename = new char[len];
		memset(m_pFilename, 0 , len);
		strncpy(m_pFilename, pFilename, len);
	}
}

SentinelFileException::SentinelFileException(const char* pFilename, const char* pMsg) throw()
	: SentinelException(pMsg, CUtilities::GetLastError())
	, m_pFilename(nullptr)
{
	SetFilename(pFilename);
}

SentinelFileException::SentinelFileException(const char* pFilename, int code) noexcept
	: SentinelException((const char*) nullptr, code)
	, m_pFilename(nullptr)
{
	SetFilename(pFilename);
}

SentinelFileException::SentinelFileException(const char* pFilename, int code, const char* pMsg) throw()
	: SentinelException(pMsg, code)
	, m_pFilename(nullptr)
{
	SetFilename(pFilename);
}

SentinelFileException::SentinelFileException(const SentinelFileException& other) throw()
	: SentinelException(other)
	, m_pFilename(nullptr)
{
	SetFilename(other.m_pFilename);
}

SentinelFileException& SentinelFileException::operator=(const SentinelFileException& other) throw()
{
	if (this != &other)
	{
		SentinelException::operator=(other);
		SetFilename(other.m_pFilename);
	}
	return *this;
}

SentinelFileException::~SentinelFileException() throw()
{
	delete[] m_pFilename;
}

const char* SentinelFileException::filename() const throw()
{
	return m_pFilename == nullptr ? "" : m_pFilename;
}


const char* SentinelFileException::rawfilename() const throw()
{
	return m_pFilename;
}
