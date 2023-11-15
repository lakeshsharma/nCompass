#pragma once

#include "PlayerSession.h"
#include "ProcessDump/WebDiagnosticsCommonInfo.h"
#include "ProcessDump/WebDiagnosticsStatusInfo.h"
#include "ProcessDump/ProcessHandler.h"
#include "ProgressiveProtocol/EGMAwardManager.h"
#include "ProgressiveProtocol/ProgressiveConfig.h"
#include "ProgressiveProtocol/ProgressiveConnectionInfo.h"
#include "ProgressiveProtocol/ProgressiveHost.h"
#include "Hardware/MCRWrapper.h"
#include "PlayerSession.h"
#include "QueuingConfig.h"
#include "PollerProtocol/MobileConnectProtocol/PairingInformation.h"
#include "PollerProtocol/MobileConnectProtocol/Beacon.h"
#include "../ExcessiveVoucherOut.h"

class CWebDiagnostics
{
public:
	/// <summary>
	/// Constructor. Initializes a new instance of the <see cref="CWebDiagnostics"/> class.
	/// </summary>
	/// <param name="fileSystem">IN - The file system to use for file I/O, etc.  Release code can use the default value.</param>
	CWebDiagnostics(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem,
		UINT bufferSize = MAX_ERROR_HTML_SIZE);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CWebDiagnostics"/> class.
	/// </summary>
	~CWebDiagnostics(void);

	/// <summary>
	/// Starts this instance.
	/// </summary>
	/// <remarks>
	/// Normally called immediately after constructor.
	/// Normally called only once per instance.
	/// Loads the Error.txt file from the MEDIA disk and determines the number of CPU "processors".
	/// </remarks>
	void Start();

	/// <summary>
	/// Dumps information about active processes, plus some Sentinel and extra information.
	/// </summary>
	/// <param name="commonInfo">IN - Common info to be dumped.</param>
	/// <param name="statusInfo">IN - Status info to be dumped.</param>
	/// <param name="playerSession">IN - PlayerSession state/status information to be dumped.</param>
	/// <param name="excessiveVoucherOut">IN - Excessive Voucher Out Info to be dumped.</param>
	void Update(const CWebDiagnosticsCommonInfo &commonInfo,
		const CWebDiagnosticsStatusInfo &statusInfo,
		const CPlayerSession &playerSession,
		const CEGMConfig &egmConfig,
		const CExcessiveVoucherOut& excessiveVoucherOut);

	/// <summary>
	/// Dumps information about certificates.
	/// </summary>
	/// <param name="commonInfo">IN - Common info to be dumped.</param>
	/// <param name="listOfCerts">IN - List Of Certs.</param>
	void UpdateCertificatesInfo(const CWebDiagnosticsCommonInfo &commonInfo, const vector<string>& listOfCerts);

	/// <summary>
	/// Dumps information about progressive.
	/// </summary>
	/// <param name="commonInfo">IN - Common info to be dumped.</param>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="localTime">IN - The current LOCAL time.</param>
	/// <param name="progConfig">IN - Progressive configuration information to be dumped.</param>
	/// <param name="progInfo">IN - Progressive connection information to be dumped.</param>
	/// <param name="progHost">IN - Progressive host information to be dumped.</param>
	/// <param name="progAward">IN - Progressive award information to be dumped.</param>
	/// <param name="lastStandardWin">IN - The last standard SAS progressive win.</param>
	/// <param name="standardWins">The list of standard SAS progressive wins currently being processed.</param>
	/// <param name="playerSession">IN - PlayerSession state/status information to be dumped.</param>
	/// <param name="progState">IN - Progressive award manager state/status information to be dumped.</param>
	void UpdateProgressive(const CWebDiagnosticsCommonInfo &commonInfo,
		DWORD tickCount,
		const SYSTEMTIME &localTime,
		const CProgressiveConfig &progConfig,
		const CProgressiveConnectionInfo &progInfo,
		const CProgressiveHost &progHost,
		const CEGMAward& progAward,
		const std::shared_ptr<const CEGMAward> &lastStandardWin,
		const std::vector<std::shared_ptr<CEGMAward>> &standardWins,
		const CPlayerSession &playerSession,
		const CEGMAwardManagerState& progState,
		const ProgressiveLevelManager& progressiveLevelManager);

	/// <summary>
	/// Dumps information about OneLink Content.
	/// </summary>
	/// <param name="commonInfo">IN - Common info to be dumped.</param>
	/// <param name="contentInfo">IN - Custom content Information.</param>
	void UpdateOneLinkContentInfo(const CWebDiagnosticsCommonInfo &commonInfo, const std::string& contentInfo);

	/// <summary>
	/// Dumps information about Player.
	/// </summary>
	/// <param name="commonInfo">IN - Common info to be dumped.</param>
	/// <param name="player">IN - The player.</param>
	/// <param name="pairingInfo">IN - The pairing information.</param>
	/// <param name="playerSession">IN - The player session.</param>
	/// <param name="beacon">IN - The connected beacon.</param>
	void UpdatePlayerInfo(const CWebDiagnosticsCommonInfo &commonInfo,
			const CPlayer *player,
			const CPairingInformation &pairingInfo,
			const CPlayerSession &playerSession,
			const IBeacon* beacon,
			uint8_t transmitPower);

	/// <summary>
	/// Dumps options and settings information.
	/// </summary>
	/// <param name="commonInfo">IN - Common info to be dumped.</param>
	/// <param name="optionsAndSettingsString">Options and Settings info to be dumped.</param>
	/// <param name="extensibleSystemSettings">Extensible System Settings info to be dumped.</param>	
	void UpdateOptionsAndSettings(const CWebDiagnosticsCommonInfo& commonInfo, const std::string& optionsAndSettingsString, const std::string& extensibleSystemSettings);

	/// <summary>
	/// Dumps information about configuration extensions.
	/// </summary>
	/// <param name="commonInfo">IN - Common info to be dumped.</param>
	/// <param name="cdmInfo">IN - The configuration extensions info to be dumped.</param>
	void UpdateConfigExt(const CWebDiagnosticsCommonInfo &commonInfo,
		const CConfigExtFile &configExt);

	/// <summary>
	/// Dumps information about queuing configuration.
	/// </summary>
	/// <param name="queueConfig">IN - The queuing configuration info to be dumped.</param>
	void UpdateQueuingConfig(const CWebDiagnosticsCommonInfo &commonInfo,
		const CQueuingConfig &queueConfig);

	/// <summary>
	/// Dumps information about Card Reader.
	/// </summary>
	/// <param name="commonInfo">IN - Common info to be dumped.</param>
	/// <param name="pMcr">IN - MCR Wrapper info to be dumped.</param>
	void UpdateCardReader(const CWebDiagnosticsCommonInfo &commonInfo,
		const CMCRWrapper* pMcr);

	/// <summary>
	/// Writes the string to the specified file, converting each character to UTF-8.
	/// </summary>
	/// <param name="file">IN/OUT - The (currently open) file instance to write to.</param>
	/// <param name="string">IN - The string.</param>
	static void Write(const std::shared_ptr<std::ostream> &file, LPCTSTR string);

	/// <summary>
	/// Writes the string to the specified file, converting each character to UTF-8.
	/// </summary>
	/// <param name="file">IN/OUT - The (currently open) file instance to write to.</param>
	/// <param name="string">IN - The string.</param>
	static void Write(const std::shared_ptr<std::ostream> &file, const std::string &string);

	/// <summary>
	/// Gets the formatted HTML diagnostic string for the common information.
	/// </summary>
	/// <param name="info">IN - The common information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetDiagString(const CWebDiagnosticsCommonInfo &info);

	/// <summary>
	/// Gets the formatted HTML header string for the common information.
	/// </summary>
	/// <param name="info">IN - The common information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetCommonHeaderHtml(const CWebDiagnosticsCommonInfo &info);

	/// <summary>
	/// Gets the formatted HTML string for the download status.
	/// </summary>
	/// <param name="downloadStatus">IN - download status.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetDownloadStatusHtml(const CFlashStatus &downloadStatus);

	/// <summary>
	/// Gets the formatted HTML string for the errors found status.
	/// </summary>
	/// <param name="errorsFound">IN - Whether errors were found.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetErrorsFoundHtml(bool errorsFound);

	/// <summary>
	/// Gets the formatted HTML trailer string for the common information.
	/// </summary>
	/// <param name="info">IN - The common information.</param>
	/// <param name="errorsFound">IN - Whether errors were found.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetCommonTrailerHtml(const CWebDiagnosticsCommonInfo &info, bool errorsFound);

	/// <summary>
	/// Gets the formatted HTML string for the common information.
	/// </summary>
	/// <param name="info">IN - The common information.</param>
	/// <param name="errorFileSizeUsed">IN - The number of bytes of the error file that were used to set m_buffer.</param>
	/// <param name="errorFileSize">IN - The size of the error file that was used to set m_buffer.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetErrorHeaderHtml(const CWebDiagnosticsCommonInfo &info, UINT errorFileSizeUsed, ULONGLONG errorFileSize);

	/// <summary>
	/// Gets the formatted HTML string for status information.
	/// </summary>
	/// <param name="statusInfo">IN - status info to be formatted.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetStatusHtml(const CWebDiagnosticsStatusInfo &statusInfo);

	/// <summary>
	/// Gets the localized string for pending action.
	/// </summary>
	/// <param name="pendingAction">IN - pending action.</param>
	/// <returns>Localized string for pending action.</returns>
	static std::string GetPendingActionText(const std::string pendingAction);

	/// <summary>
	/// Gets the formatted HTML string for the process.
	/// </summary>
	/// <param name="process">IN - The process.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetProcessHtml(const CProcess &process);

	/// <summary>
	/// Saves the status data.
	/// </summary>
	/// <param name="file">IN/OUT - The (unopened) file instance to use for saving the data.</param>
	/// <param name="commonInfo">IN - The common information.</param>
	/// <param name="statusInfo">IN - The status information.</param>
	/// <param name="playerSession">IN - PlayerSession state/status information to be dumped.</param>
	/// <param name="egmConfig">IN - EGM configuration to be dumped.</param>
	/// <param name="errorsFound">IN - Whether errors were found.</param>
	static void SaveStatusData(const std::shared_ptr<std::ostream> &file,
		const CWebDiagnosticsCommonInfo &commonInfo,
		const CWebDiagnosticsStatusInfo &statusInfo,
		const CPlayerSession &playerSession,
		const CEGMConfig &egmConfig,
		const CExcessiveVoucherOut& excessiveVoucherOut,
		bool errorsFound);

	/// <summary>
	/// Saves the memory data.
	/// </summary>
	/// <param name="file">IN/OUT - The (unopened) file instance to use for saving the data.</param>
	/// <param name="commonInfo">IN - The common information.</param>
	/// <param name="processHandler">IN - The process handler.</param>
	/// <param name="errorsFound">IN - Whether errors were found.</param>
	static void SaveMemoryData(const std::shared_ptr<std::ostream> &file,
		const CWebDiagnosticsCommonInfo &commonInfo,
		const CProcessHandler &processHandler,
		bool errorsFound);

	/// <summary>
	/// For web diagnostic pages, "format" a bool value into a string.
	/// </summary>
	/// <param name="value">IN - The bool value.</param>
	/// <returns>An const std::string &to the "formatted" string value.</returns>
	static LPCTSTR GetBoolString(bool value);

	/// <summary>
	/// For web diagnostic pages, "format" a bool value into a enabled/disabled string.
	/// </summary>
	/// <param name="value">IN - The bool value.</param>
	/// <returns>An const std::string &to the "formatted" enabled/disabled string value.</returns>
	static LPCTSTR GetEnabledDisabledString(bool value);

	/// <summary>
	/// For web diagnostic pages, "format" a bool value into an ANSI string.
	/// </summary>
	/// <param name="value">IN - The bool value.</param>
	/// <returns>An LPCSTR to the "formatted" string value.</returns>
	static LPCSTR GetBoolStringA(bool value);

	/// <summary>
	/// For web diagnostic pages, "format" a bool value into a enabled/disabled an ASNI string.
	/// </summary>
	/// <param name="value">IN - The bool value.</param>
	/// <returns>An LPCSTR to the "formatted" enabled/disabled string value.</returns>
	static LPCSTR GetEnabledDisabledStringA(bool value);

	/// <summary>
	/// Gets the formatted HTML string for the progressive configuration.
	/// </summary>
	/// <param name="progConfig">IN - The progressive configuration.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetProgressiveConfigHtml(const CProgressiveConfig &progConfig);
	
	/// <summary>
	/// Gets the formatted HTML string for the Pairing Information.
	/// </summary>
	/// <param name="pairingInfo">IN - The Pairing Information.</param>
	/// <param name="mobileConnected">IN - Is mobile player.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetPairingInformationHtml(const CPairingInformation &pairingInfo, bool mobileConnected);

	/// <summary>
	/// Gets the formatted HTML string for the Player Information.
	/// </summary>
	/// <param name="player">IN - The active playern.</param>
	/// <param name="playerSession">IN - Player session.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetPlayerInformationHtml(const CPlayer *player, const CPlayerSession &playerSession);

	/// <summary>
	/// Gets the formatted HTML string for the Beacon Information.
	/// </summary>
	/// <param name="beacon">IN - The connected beacon, can be nullptr.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetBeaconInformationHtml(const IBeacon *beacon, uint8_t transmitPower, uint8_t measuredPower);

	/// <summary>
	/// Gets the formatted HTML string for the progressive connection information.
	/// </summary>
	/// <param name="progConfig">IN - The progressive configuration.</param>
	/// <param name="progInfo">IN - The progressive connection information.</param>
	/// <param name="progHost">IN - The progressive host information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetProgressiveConnectionInfoHtml(const CProgressiveConfig &progConfig,
		const CProgressiveConnectionInfo &progInfo,
		const CProgressiveHost &progHost);

	/// <summary>
	/// Gets the formatted HTML string for the progressive host list configuration.
	/// </summary>
	/// <param name="progConfig">IN - The progressive configuration.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetProgressiveHostListHtml(const CProgressiveConfig &progConfig);

	/// <summary>
	/// Gets the formatted HTML string for the current progressive host addresses.
	/// </summary>
	/// <param name="progHost">IN - The progressive host information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetProgressiveHostAddressesHtml(const CProgressiveHost &progHost);

	/// <summary>
	/// Gets the formatted HTML string for the last progressive award.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="progAward">IN - Progressive award information.</param>
	/// <param name="progState">IN - Progressive award manager state/status information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetBonusAwardHtml(DWORD tickCount,
		const CEGMAward& progAward,
		const CEGMAwardManagerState& progState);

	/// <summary>
	/// Gets the formatted HTML string for a standard SAS progressive win.
	/// </summary>
	/// <param name="standardWin">IN - A standard SAS progressive win.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetStandardProgressiveWinHtml(const std::shared_ptr<const CEGMAward> &standardWin);

	/// <summary>
	/// Gets the formatted HTML string for a list of standard SAS progressive wins.
	/// </summary>
	/// <param name="standardWins">IN - The list of Standard SAS progressive wins.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetStandardProgressiveWinsHtml(const std::vector<std::shared_ptr<CEGMAward>> &standardWins);

	/// <summary>
	/// Gets the newest standard SAS progressive win.
	/// </summary>
	/// <param name="lastStandardWin">IN - The previous standard SAS progressive win, if any.</param>
	/// <param name="standardWins">The list of Standard SAS progressive wins currently in progress.</param>
	/// <returns>The newest standard SAS progressive win.</returns>
	static std::shared_ptr<const CEGMAward> GetNewestStandardProgressiveWin(const std::shared_ptr<const CEGMAward> &lastStandardWin,
		const std::vector<std::shared_ptr<CEGMAward>> &standardWins);

	/// <summary>
	/// Gets the formatted HTML string for the last progressive award.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="progAward">IN - Progressive award information.</param>
	/// <param name="progState">IN - Progressive award manager state/status information.</param>
	/// <param name="lastStandardWin">IN - The last standard SAS progressive win.</param>
	/// <param name="standardWins">The list of standard SAS progressive wins currently being processed.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetLastProgressiveAwardHtml(DWORD tickCount,
		const CEGMAward &progAward,
		const CEGMAwardManagerState &progState,
		const std::shared_ptr<const CEGMAward> &lastStandardWin,
		const std::vector<std::shared_ptr<CEGMAward>> &standardWins);

	/// <summary>
	/// Gets the formatted HTML string for the progressive player info (sessionId, ...).
	/// </summary>
	/// <param name="playerSession">IN - Player session information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetProgressivePlayerInfoHtml(const CPlayerSession &playerSession);

	/// <summary>
	/// Gets the formatted HTML string for the progressive down info.
	/// </summary>
	/// <param name="localTime">IN - The local time.</param>
	/// <param name="progState">IN - Progressive award manager state/status information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetProgressiveDownInfoHtml(const SYSTEMTIME& localTime, const ProgressiveLevelManager& levelManager);

	/// <summary>
	/// Gets the formatted HTML string for the progressive data (level, amount, ...).
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="localTime">IN - The local time.</param>
	/// <param name="progState">IN - Progressive award manager state/status information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetProgressiveLevelsHtml(DWORD tickCount, const SYSTEMTIME& localTime, const ProgressiveLevelManager& levelManager);

	/// <summary>
	/// Gets the last game information.
	/// </summary>
	/// <param name="playerSession">IN - The player session.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetLastGameInfo(const CPlayerSession &playerSession);
	
	/// <summary>
	/// Gets the web diagnostics resource string for the EGM type.
	/// </summary>
	/// <param name="egmType">IN - The EGM type.</param>
	/// <returns>The web diagnostics resource string for the EGM type.</returns>
	static std::string GetEgmTypeString(const EGMType egmType);

	/// <summary>
	/// Gets the web diagnostics resource string for the EGM protocol.
	/// </summary>
	/// <param name="egmProtocol">IN - The EGM protocol.</param>
	/// <returns>The web diagnostics resource string for the EGM protocol.</returns>
	static std::string GetEgmProtocolString(EGMProtocol egmProtocol);

	/// <summary>
	/// Gets the current active multiplier and divider to print in HTML format.
	/// </summary>
	/// <param name="statusInfo">IN - The status information.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetCurrentMultiplierAndDivider(const CWebDiagnosticsStatusInfo &statusInfo);

	/// <summary>
	/// Gets the EGM configuration HTML.
	/// </summary>
	/// <param name="statusInfo">IN - The status information.</param>
	/// <param name="egmConfig">IN - EGM configuration information to be dumped.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetEgmConfigHtml(const CWebDiagnosticsStatusInfo &statusInfo, const CEGMConfig &egmConfig);

	/// <summary>
	/// Saves the progressive information.
	/// </summary>
	/// <param name="file">IN/OUT - The (unopened) file instance to use for saving the data.</param>
	/// <param name="dumpFileName">IN - Name of the dump file.</param>
	/// <param name="commonInfo">IN - The common information.</param>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="localTime">IN - The current local time.</param>
	/// <param name="progConfig">IN - The progressive configuration.</param>
	/// <param name="progInfo">IN - The progressive connection information.</param>
	/// <param name="progHost">IN - The progressive host information.</param>
	/// <param name="progAward">IN - Progressive award information to be dumped.</param>
	/// <param name="lastStandardWin">IN - The last Standard SAS progressive win processed.</param>
	/// <param name="standardWins">The list of Standard SAS progressive wins currently being processed.</param>
	/// <param name="playerSession">IN - PlayerSession state/status information to be dumped.</param>
	/// <param name="progState">IN - Progressive award manager state/status information to be dumped.</param>
	static void SaveProgressiveData(std::shared_ptr<std::ostream> &file,
		const CWebDiagnosticsCommonInfo &commonInfo,
		DWORD tickCount,
		const SYSTEMTIME& localTime,
		const CProgressiveConfig &progConfig,
		const CProgressiveConnectionInfo &progInfo,
		const CProgressiveHost &progHost,
		const CEGMAward& progAward,
		const std::shared_ptr<const CEGMAward> &lastStandardWin,
		const std::vector<std::shared_ptr<CEGMAward>> &standardWins,
		const CPlayerSession &playerSession,
		const CEGMAwardManagerState& progState,
		const ProgressiveLevelManager& progressiveLevelManager);

	/// <summary>
	/// Saves the Player information.
	/// </summary>
	/// <param name="file">IN/OUT - The (unopened) file instance to use for saving the data.</param>
	/// <param name="dumpFileName">IN - Name of the dump file.</param>
	/// <param name="commonInfo">IN - The common information.</param>
	/// <param name="player">IN - The player.</param>
	/// <param name="pairingInfo">IN - The pairing information.</param>
	/// <param name="playerSession">IN - The player session.</param>
	/// <param name="beacon">IN - The connected beacon.</param>
	static void SavePlayerData(std::shared_ptr<std::ostream> &file,
		const CWebDiagnosticsCommonInfo &commonInfo,
		const CPlayer *player,
		const CPairingInformation &pairingInfo,
		const CPlayerSession &playerSession,
		const IBeacon* beacon,
		uint8_t transmitPower
	);

	/// <summary>
	/// Gets the formatted HTML string for the configuration extensions.
	/// </summary>
	/// <param name="info">IN - The configuration extensions.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetConfigExtHtml(const CConfigExtFile &configExt);

	/// <summary>
	/// Gets the formatted HTML string for the queuing configuration.
	/// </summary>
	/// <param name="queuingConfig">IN - The queuing configuration.</param>
	/// <returns>The formatted HTML string.</returns>
	static std::string GetQueuingConfigHtml(const CQueuingConfig &queuingConfig);

	/// <summary>
	/// Saves the configuration extensions data.
	/// </summary>
	/// <param name="file">IN/OUT - The (unopened) file instance to use for saving the data.</param>
	/// <param name="dumpFileName">IN - Name of the dump file.</param>
	/// <param name="commonInfo">IN - The common information.</param>
	/// <param name="configExt">IN - The configuration extensions info to be dumped.</param>
	static void SaveConfigExtData(const std::shared_ptr<std::ostream> &file,
		const CWebDiagnosticsCommonInfo &commonInfo,
		const CConfigExtFile &configExt);

	/// <summary>
	/// Saves the queuing configuration data.
	/// </summary>
	/// <param name="file">IN/OUT - The (unopened) file instance to use for saving the data.</param>
	/// <param name="dumpFileName">IN - Name of the dump file.</param>
	/// <param name="commonInfo">IN - The common information.</param>
	/// <param name="queuingConfig">IN - The queuing configuration info to be dumped.</param>
	static void SaveQueuingConfigData(const std::shared_ptr<std::ostream> &file,
		const CWebDiagnosticsCommonInfo &commonInfo,
		const CQueuingConfig &queuingConfig);

	/// <summary>
	/// Saves the error data.
	/// </summary>
	/// <param name="file">IN/OUT - The (unopened) file instance to use for saving the data.</param>
	/// <param name="commonInfo">IN - The common information.</param>
	/// <param name="errorFileSizeUsed">IN - The number of bytes of the error file that were used to set m_buffer.</param>
	/// <param name="errorFileSize">IN - The size of the error file that was used to set m_buffer.</param>
	/// <param name="errorBuffer">IN - The error buffer.</param>
	/// <param name="errorBufferLength">IN - Length of the error buffer.</param>
	static void SaveErrorData(const std::shared_ptr<std::ostream> &file,
		const CWebDiagnosticsCommonInfo &commonInfo,
		UINT errorFileSizeUsed,
		ULONGLONG errorFileSize,
		const byte *errorBuffer,
		int errorBufferLength);

	/// <summary>
	/// Loads the error file into the error buffer.
	/// </summary>
	/// <param name="file">IN/OUT - The (unopened) file instance to use for loading the file.</param>
	/// <param name="errorFileName">IN - Name of the error file.</param>
	/// <param name="errorFileSizeUsed">OUT - The number of bytes of the error file that were used to set m_buffer.</param>
	/// <param name="errorFileSize">OUT - The size of the error file that was used to set m_buffer.</param>
	/// <param name="buffer">OUT - error buffer to load file into.</param>
	/// <param name="bufferSize">IN - Size of buffer and count  of elements in buffer, because sizeof(*buffer) == 1.</param>
	/// <returns>The number of valid bytes in the error buffer.</returns>
	UINT LoadErrorFile(const std::shared_ptr<std::istream> &file,
		UINT &errorFileSizeUsed,
		ULONGLONG &errorFileSize,
		BYTE *buffer,
		UINT bufferSize);
	
	/// <summary>
	/// Gets the error buffer.
	/// </summary>
	/// <param name="bytesRead">OUT - The number of valid bytes in the error buffer.</param>
	/// <returns>A pointer to the error buffer</returns>
	const BYTE * GetErrorBuffer(int &bytesRead) const;

	/// <summary>
	/// Saves the CardReader data.
	/// </summary>
	/// <param name="os">The ostream to write to.</param>
	/// <param name="commonInfo">IN - The common information.</param>
	/// <param name="pMcr">IN - The CMCRWrapper instance to use for logging.</param>
	static void SaveCardReaderData(
		std::ostream& os,
		const CWebDiagnosticsCommonInfo &commonInfo,
		const CMCRWrapper* pMcr);

	// Consider MEMORY_DUMP_REFRESH_SECONDS when specifying TIME_BETWEEN_MEMORY_DUMPS.
	static const DWORD TIME_BETWEEN_MEMORY_DUMPS = 10000; // In milliseconds.

private:
	// Disable default copy constructor, and default assignment operator.
	CWebDiagnostics(const CWebDiagnostics &rhs) = delete;
	CWebDiagnostics& operator=(const CWebDiagnostics &rhs) = delete;

public:
	static LPCTSTR const FMT_HTML_HEADER; // printf-syle format specifiction.
	static LPCTSTR const HTML_TRAILER;
	static LPCTSTR const BR_TAG;

	static LPCTSTR const LPCTSTR_FALSE;
	static LPCTSTR const LPCTSTR_TRUE;

	static LPCTSTR const LPCTSTR_DISABLED;
	static LPCTSTR const LPCTSTR_ENABLED;

	static LPCTSTR const LPCTSTR_NA;

	static LPCSTR const LPCSTR_FALSE;
	static LPCSTR const LPCSTR_TRUE;
	static LPCSTR TrueFalseStr(bool val);

	static LPCSTR const LPCSTR_DISABLED;
	static LPCSTR const LPCSTR_ENABLED;
	static LPCSTR EnabledDisabledStr(bool val);

	static LPCSTR const LPCSTR_NA;

private:
	static const int DefaultRefreshSeconds = 60;

	static const int CERTS_INFO_REFRESH_SECONDS = 60;
	static const int STATUS_DUMP_REFRESH_SECONDS = 30;
	static const int MEMORY_DUMP_REFRESH_SECONDS = 30;
	static const int PROGRESSIVE_INFO_REFRESH_SECONDS = 30;
	static const int ONELINK_INFO_REFRESH_SECONDS = 60;
	static const int PLAYER_INFO_REFRESH_SECONDS = 30;
	static const int CONFIG_EXTENSIONS_REFRESH_SECONDS = 60;
	static const int QUEUINGCONFIG_REFRESH_SECONDS = 60;

public:
	static const int CardreaderInfoRefreshSeconds = DefaultRefreshSeconds;
	static const int JackpotInfoRefreshSeconds = DefaultRefreshSeconds;

	static LPCTSTR HTML_FILE_ROOT_PATH;
	static LPCTSTR CardReaderInfoFilename;
	static LPCTSTR CertificatesInfoFilename;
	static LPCTSTR OptionsAndSettingsFilename;
	static LPCTSTR ProgressiveInfoFilename;
	static LPCTSTR OneLinkContentFilename;	
	static LPCTSTR PlayerInfoFilename;
	static LPCTSTR JackpotInfoFilename;

private:
	BYTE * const m_buffer;
	const UINT m_bufferSize;
	UINT m_bytesRead; // Number of bytes in m_buffer that are valid.
	ULONGLONG m_errorFileSize; // The error file could be larger than the number of bytes read.
	UINT m_errorFileSizeUsed;
	DWORD m_fileUpdateCount;
	bool m_errorsFound;

	CProcessHandler m_processHandler;
	IOperatingSystem &m_operatingSystem;

	static time_t m_lastDumpMemorySystemCall;
	static std::string m_sizeOS, m_usedOS, m_availableOS, m_sizeSentinelData, m_usedSentinelData, m_availableSentinelData,
		m_sizeContent, m_usedContent, m_availableContent, m_sizeUSB, m_usedUSB, m_availableUSB;

	static std::string GetConfigExt1Html(const CConfigExtFile &configExt);
	static std::string GetConfigExt2Html(const CConfigExtFile &configExt);
	std::string m_lastOptionsAndSettingsString;
public:
	static const uint32_t None = 0;
	static const uint32_t UseTableStyle = 0x00000001;
	static const uint32_t UseRgbColorStyle = 0x00000002;
	static const uint32_t UseStyles = UseTableStyle | UseRgbColorStyle;

	static void WriteHtmlHeader(
		std::ostream& os,
		int refresh = DefaultRefreshSeconds,
		const char* pTitle = nullptr, 
		uint32_t flags = None, 
		const char* pCustomHeadElement = nullptr,
		const char* pCustomStyle = nullptr);

	static void WriteHtmlClose(std::ostream& os);
};
