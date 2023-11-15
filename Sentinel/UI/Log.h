#pragma once

class CLog
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CLog"/> class.
	/// </summary>
	CLog(void);

	/// <summary>
	/// Desctructor. Finalizes an instance of the <see cref="CLog"/> class.
	/// </summary>
	virtual ~CLog(void);

	/// <summary>
	/// Gets the date time as a string.
	/// </summary>
	/// <returns></returns>
	std::string GetDateTimeAsString( void ) const;

	/// <summary>
	/// Gets the date time.
	/// </summary>
	/// <returns></returns>
	time_t GetDateTime( void ) const;

protected:
	time_t m_dateTime;
};
