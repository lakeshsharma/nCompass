#pragma once
#include "ConfigFile.h"

const std::string TICKETPBTMAXAMOUNTSFILE = "/TicketPBTMax.txt";
const std::string PBTMAXNAME = "PBTMAX";
const std::string TICKETMAXNAME = "TICKETMAX";
const int CONFIG_READ_PERIOD = 15;
const long PBTMAX = 300000;
const long TICKETMAX = 1000000;

class CTicketPBTMaxAmount : CConfigFile
{
private:

	// Disable default constructor
	CTicketPBTMaxAmount();
	void SetDefaults();

	DWORD m_TicketMax;
	DWORD m_PBTMax;

public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CTicketPBTMaxAmount"/> class.
	/// </summary>
	/// <param name="startupTickcount">IN - The startup tickcount.</param>
	CTicketPBTMaxAmount(DWORD startupTickcount);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CTicketPBTMaxAmount"/> class.
	/// </summary>
	~CTicketPBTMaxAmount();

	/// <summary>
	/// Starts initial config file load for this instance.
	/// </summary>
	/// <returns>true if any name/value pairs were processed from the configuration file, false otherwise.</returns>
	bool Start();

	/// <summary>
	/// Interprets the name and value.
	/// </summary>
	/// <param name="name">The name.</param>
	/// <param name="value">The value.</param>
	/// <returns>True if the name/value match any of the expected config parameters</returns>
	bool InterpretNameValue(const std::string& name, const std::string& value);

	/// <summary>
	/// Loads the configuration file.
	/// </summary>
	/// <param name="configFileName">Name of the configuration file.</param>
	void LogConfigValues(LPCTSTR sourceDesc, bool valid);

	/// <summary>
	/// Gets the maximum amounts.
	/// </summary>
	/// <param name="pbtMax">The PBT maximum.</param>
	/// <param name="ticketMax">The ticket maximum.</param>
	void GetMaxAmounts(long &pbtMax, long &ticketMax);

	/// <summary>
	/// Gets the maximum PBT amount. If not configured returns LONG_MAX.
	/// </summary>
	/// <returns>Maximum PBT amount.</returns>
	long GetMaxPBTAmount();

	/// <summary>
	/// Gets the maximum ticket amount. If not configured returns LONG_MAX.
	/// </summary>
	/// <returns>Maximum ticket amount.</returns>
	long GetMaxTicketAmount();
};

