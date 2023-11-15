#pragma once
#include <stdafx.h>

///	<summary>
///	An exception class unique to Sentinel
///	</summary>
///	<remarks>
/// This derives from the Visual Studio std::exception.
///	</remarks>
class SentinelException : public std::runtime_error
{
public:
	int m_code;

public:
	///	<summary>
	/// SentinelException constructor.
	///	</summary>
	///	<param name="code">The code to save with this instance.</param>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	SentinelException(int code = 0, const char* pMsg = nullptr) throw();

	///	<summary>
	/// SentinelException constructor.
	///	</summary>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	/// <remarks>
	///	The code will be set to GetLastError(). Use another constructor to explicitly
	///	set the code.
	/// </remarks>
	explicit SentinelException(const char* pMsg) throw();

	///	<summary>
	/// SentinelException constructor.
	///	</summary>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	///	<param name="code">The code to save with this instance.</param>
	SentinelException(const char* pMsg, int code) throw();

	///	<summary>
	/// SentinelException copy constructor.
	///	</summary>
	///	<param name="other">The SentinelException object to copy.</param>
	SentinelException(const SentinelException& other) throw();

	///	<summary>
	/// SentinelException assignment operator.
	///	</summary>
	///	<param name="other">The SentinelException object to copy.</param>
	///	<returns>A reference to this instance.</returns>
	SentinelException& operator=(const SentinelException& other) throw();

	///	<summary>
	///	Returns the stored code.
	///	</summary>
	///	<returns>The stored code</returns>
	/// <remarks>Uses lower case to match 'what()'</remarks>
	virtual int code() const throw();

	///	<summary>
	/// Indicates whether the exception has non-zero data.
	///	</summary>
	///	<param name="all">If <b>true</b>, checks if any of the data
	///		fields contain non-zero data.</param>
	///	<returns><b>true</b> if there is non-zero data.</returns>
	virtual bool HasNonZeroData(bool all = false) const throw();
};

///	<summary>
/// A template derivation of SentinelException
///	</summary>
///	<param name="T_VAL">A simple value less than or equal in size to int.</param>
template<class T_VAL>
class SentinelExceptionT : public SentinelException
{
public:
	///	<summary>
	/// SentinelExceptionT constructor.
	///	</summary>
	///	<param name="code">The code to save with this instance.</param>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	SentinelExceptionT(T_VAL code, const char* pMsg = nullptr) throw()
		: SentinelException((int) code, pMsg)
	{
	}

	///	<summary>
	/// SentinelExceptionT constructor.
	///	</summary>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	///	<param name="code">The code to save with this instance.</param>
	SentinelExceptionT(const char* pMsg, T_VAL code = (T_VAL) -1) throw()
		: SentinelException(pMsg, (int) code)
	{
	}

	///	<summary>
	/// SentinelExceptionT copy constructor.
	///	</summary>
	///	<param name="other">The SentinelExceptionT object to copy.</param>
	SentinelExceptionT(const SentinelExceptionT& other) throw()
		: SentinelException(other)
	{
	}

	///	<summary>
	/// SentinelExceptionT assignment operator.
	///	</summary>
	///	<param name="other">The SentinelExceptionT object to copy.</param>
	///	<returns>A reference to this instance.</returns>
	SentinelExceptionT& operator=(const SentinelExceptionT& other) throw()
	{
		SentinelException::operator=(other);
		return *this;
	}
};

class SentinelCreateThreadException : public SentinelException
{
public:
	///	<summary>
	/// SentinelCreateThreadException constructor.
	///	</summary>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	/// <remarks>
	///	The 'code()' will be set to GetLastError().
	/// </remarks>
	explicit SentinelCreateThreadException(const char* pMsg) throw();

	///	<summary>
	/// SentinelCreateThreadException constructor.
	///	</summary>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	///	<param name="code">The code to save with this instance.</param>
	SentinelCreateThreadException(const char* pMsg, int code) throw();

	///	<summary>
	/// SentinelCreateThreadException copy constructor.
	///	</summary>
	///	<param name="other">The SentinelCreateThreadException object to copy.</param>
	SentinelCreateThreadException(const SentinelCreateThreadException& other) throw();

	///	<summary>
	/// SentinelCreateThreadException assignment operator.
	///	</summary>
	///	<param name="other">The SentinelCreateThreadException object to copy.</param>
	///	<returns>A reference to this instance.</returns>
	SentinelCreateThreadException& operator=(const SentinelCreateThreadException& other) throw();
};

class SentinelCancelException : public SentinelException
{
public:
	///	<summary>
	/// SentinelCancelException constructor.
	///	</summary>
	///	<param name="code">The code to save with this instance.</param>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	/// <remarks>
	///	The 'what()' will be set to an empty string.
	/// </remarks>
	SentinelCancelException(int code = 0, const char* pMsg = nullptr) throw();

	///	<summary>
	/// SentinelCancelException constructor.
	///	</summary>
	///	<param name="pMsg">The message to save with this instance. If nullptr is passed
	///		an empty string will be saved.</param>
	///	<param name="code">The code to save with this instance.</param>
	SentinelCancelException(const char* pMsg, int code = 0) throw();

	///	<summary>
	/// SentinelCancelException copy constructor.
	///	</summary>
	///	<param name="other">The SentinelCancelException object to copy.</param>
	SentinelCancelException(const SentinelCancelException& other) throw();

	///	<summary>
	/// SentinelCancelException assignment operator.
	///	</summary>
	///	<param name="other">The SentinelCancelException object to copy.</param>
	///	<returns>A reference to this instance.</returns>
	SentinelCancelException& operator=(const SentinelCancelException& other) throw();
};

class SentinelFileException : public SentinelException
{
private:
	char* m_pFilename;
	void SetFilename(const char* pFilename);

public:
	///	<summary>
	/// SentinelFileException constructor.
	///	</summary>
	///	<param name="pFilename">The filename to save with this instance. If nullptr is
	///		passed "<nullptr>" will be saved.</param>
	///	<param name="pMsg">The message to save with this instance as the 'what()' member.
	//		If nullptr is passed an empty string will be saved.</param>
	/// <remarks>
	///	The code will be set to GetLastError(). Use another constructor to explicitly
	///	set the code.
	/// </remarks>
	SentinelFileException(const char* pFilename, const char* pMsg = nullptr) noexcept;

	///	<summary>
	/// SentinelFileException constructor.
	///	</summary>
	///	<param name="pFilename">The filename to save with this instance. If nullptr is
	///		passed "<nullptr>" will be saved.</param>
	///	<param name="code">The code to save with this instance.</param>
	SentinelFileException(const char* pFilename, int code) throw();

	///	<summary>
	/// SentinelFileException constructor.
	///	</summary>
	///	<param name="pFilename">The filename to save with this instance. If nullptr is
	///		passed "<nullptr>" will be saved.</param>
	///	<param name="code">The code to save with this instance.</param>
	///	<param name="pMsg">The message to save with this instance as the 'what()' member.
	//		If nullptr is passed an empty string will be saved.</param>
	SentinelFileException(const char* pFilename, int code, const char* pMsg) throw();

	///	<summary>
	/// SentinelFileException copy constructor.
	///	</summary>
	///	<param name="other">The SentinelFileException object to copy.</param>
	SentinelFileException(const SentinelFileException& other) throw();

	///	<summary>
	/// SentinelFileException destructor.
	///	</summary>
	~SentinelFileException() throw();

	///	<summary>
	/// SentinelFileException assignment operator.
	///	</summary>
	///	<param name="other">The SentinelFileException object to copy.</param>
	///	<returns>A reference to this instance.</returns>
	SentinelFileException& operator=(const SentinelFileException& other) throw();

	///	<summary>
	///	Returns the stored filename. The return value will always be a valid string.
	///	</summary>
	///	<returns>The stored filename. If the internal bufer is nullptr, this will
	///		return an empty string.</returns>
	/// <remarks>Uses lower case to match 'what()' and 'code()'</remarks>
	const char* filename() const throw();

	///	<summary>
	///	Returns the stored filename. The return value may be nullptr.
	///	</summary>
	///	<returns>The stored filename. If the internal bufer is nullptr, this will
	///		return a nullptr.</returns>
	/// <remarks>Uses lower case to match 'what()' and 'code()'</remarks>
	const char* rawfilename() const throw();
};


