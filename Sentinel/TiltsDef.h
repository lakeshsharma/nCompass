#pragma once

const int TILT_TIMEOUT = 2; //minutes

/// <summary>
/// Tilt code definitions.
/// Values must fit in 2 bytes for compatibility throughout the system.
/// </summary>
typedef enum
{
    tltUnknown = 0,

    // Game tilts
    tltGameUnknown = 0x100,
    tltGameRam = tltGameUnknown + 1,
    tltGameRamRecover = tltGameUnknown + 2,
    tltGameRamIrrecover = tltGameUnknown + 3,
    tltGameRamHardFail = tltGameUnknown + 4,
    tltGameRamBattery = tltGameUnknown + 5,
    tltGameEeprom = tltGameUnknown + 11,
    tltGameEepromRecover = tltGameUnknown + 12,
    tltGameEepromIrrecover = tltGameUnknown + 13,
    tltGameEepromHardFail = tltGameUnknown + 14,
    tltGameRom = tltGameUnknown + 21,
    tltGameRomWarning = tltGameUnknown + 22,
    tltGameRomError = tltGameUnknown + 23,
    tltGameAssetZero = tltGameUnknown + 24,
    tltGameCommIn = tltGameUnknown + 31,
    tltGameCommOut = tltGameUnknown + 32,
    tltJckpotReset = tltGameUnknown + 33,
    tltEepromDenomErr = tltGameUnknown + 34,
    tltEepromSizeErr = tltGameUnknown + 35,
    tltTicketInMismatch = tltGameUnknown + 36,
    tltGamePowerOn = tltGameUnknown + 37,
    tltGamePowerOff = tltGameUnknown + 38,
    tltMeterChangeRequest = tltGameUnknown + 39,
    tltMeterChangeCancelled = tltGameUnknown + 40,
    tltMeterChangeComplete = tltGameUnknown + 41,
    tltSubgameCountMismatch = tltGameUnknown + 42,
    // Tracker 22847
    tltOpChangedOption = tltGameUnknown + 60,
    tltSubgameCaptureFailed = tltGameUnknown + 61,

    // Coin in tilts
    tltCiUnknown         = 0x200,
    tltCiJam             = tltCiUnknown + 1,
    tltCiYoyo            = tltCiUnknown + 2,
    tltCiHardFail        = tltCiUnknown + 3,
    tltCiLockFail        = tltCiUnknown + 4,
    tltCiDiverter        = tltCiUnknown + 5,
    tltCiLong            = tltCiUnknown + 6,

    // Coin out tilts
    tltCoUnknown         = 0x300,
    tltCoJam             = tltCoUnknown + 1,
    tltCoRunaway         = tltCoUnknown + 2,
    tltCoOverpay         = tltCoUnknown + 3,

	// Hopper tilts
    tltHopUnknown        = 0x400,
    tltHopEmpty          = tltHopUnknown + 1,
    tltHopLow            = tltHopUnknown + 2,
    tltHopHardFail       = tltHopUnknown + 3,
    tltPrnLow            = tltHopUnknown + 4,
    tltPrnEmpty          = tltHopUnknown + 5,
    tltPrnJam            = tltHopUnknown + 6,
    tltPrnHardFail       = tltHopUnknown + 7,
    tltPrnReject         = tltHopUnknown + 8,
    tltPrnCommFail       = tltHopUnknown + 9,
    tltPrnRemoved        = tltHopUnknown + 10,

	// Reel tilts
    tltReelUnknown       = 0x500,
    tltReelJam1          = tltReelUnknown + 1,  
    tltReelJam2          = tltReelUnknown + 2,  
    tltReelJam3          = tltReelUnknown + 3,  
    tltReelJam4          = tltReelUnknown + 4,  
    tltReelJam5          = tltReelUnknown + 5,  
    tltReelJam6          = tltReelUnknown + 6,  
    tltReelJam7          = tltReelUnknown + 7,  
    tltReelJam8          = tltReelUnknown + 8,  
    tltReelJam9          = tltReelUnknown + 9,  
    tltReelJam10         = tltReelUnknown + 10,  
    tltReelMotion1       = tltReelUnknown + 11, 
    tltReelMotion2       = tltReelUnknown + 12, 
    tltReelMotion3       = tltReelUnknown + 13, 
    tltReelMotion4       = tltReelUnknown + 14, 
    tltReelMotion5       = tltReelUnknown + 15, 
    tltReelMotion6       = tltReelUnknown + 16, 
    tltReelMotion7       = tltReelUnknown + 17, 
    tltReelMotion8       = tltReelUnknown + 18, 
    tltReelMotion9       = tltReelUnknown + 19, 
    tltReelMotion10      = tltReelUnknown + 20, 
    tltReelRemoved       = tltReelUnknown + 21, 

	// Bill tilts
    tltBillUnknown       = 0x600,
    tltBillJam           = tltBillUnknown + 1,
    tltBillFull          = tltBillUnknown + 2,
    tltBillHardFail      = tltBillUnknown + 3,
    tltBillNoComm        = tltBillUnknown + 4,
    tltBillStackerOut    = tltBillUnknown + 5,
    tltBillStackerIn     = tltBillUnknown + 6,

	// Program tilts
    tltProgUnknown       = 0x700,
    tltProgNoReturn      = tltProgUnknown + 0,  //{add level}
    tltProgExtraData     = tltProgUnknown + 10, //{add level}

	// Retro tilts
    tltRetroUnknown       = 0x800,
    tltRetroLifeMeterErr  = tltRetroUnknown + 1,
    tltRetroCurencyErr    = tltRetroUnknown + 2,
    tltRetroTktInErr      = tltRetroUnknown + 3,
    tltRetroTktOutErr     = tltRetroUnknown + 4,
    tltRetroVersionErr    = tltRetroUnknown + 5,

	// Reboot tilts or codes
	tltRebootUnknown = 0x900,
	tltRebootButton,
	tltRebootSoftUpgrade,
	tltRebootFirmUpgrade,
	tltRebootMemoryClear,
	tltRebootSkinChange,
	tltRebootDispChange,
	tltRebootWarning,
	tltRebootAsap,
	tltRebootException,
	tltRebootThreadTimeout,
	tltRebootMemoryError,
	tltRebootOffline,
	tltRebootRemote,
	tltRebootUiRestart,
	tltRebootRemoteRequest,
	tltRebootRemotePending,
	//tltRebootLastCode is used to check if Reboot Tilt Code is valid, 
	//a new reboot tilt code entry must be inserted above this line
	tltRebootLastCode,

	// Aberration Diagnositc codes
	tltInvalidPbIdBef								= 0xb00,
	tltInvalidPbIdAfter								= tltInvalidPbIdBef + 1,
	tltTransferOvrLmtBef							= tltInvalidPbIdBef + 2,
	tltTransferOvrLmtAftr							= tltInvalidPbIdBef + 3,
	tltInvalidPbAmount								= tltInvalidPbIdBef + 4,
	tltVoucherOvrLmtBef								= tltInvalidPbIdBef + 5,
	tltVoucherOvrLmtAftr							= tltInvalidPbIdBef + 6,
	tltVoucherMismatch								= tltInvalidPbIdBef + 7,
	tltInvalidPlayerPointInc						= tltInvalidPbIdBef + 8,	// no longer used
	tltPollerProtocolCreatePacket					= tltInvalidPbIdBef + 9,
	tltPollerProtocolCheckPacket					= tltInvalidPbIdBef + 10,
	tltPollerProtocolPacketSentVerification			= tltInvalidPbIdBef + 11,
	tltNegativeCoinInOutMeterDelta					= tltInvalidPbIdBef + 12,
	tltProbePBTAccessError							= tltInvalidPbIdBef + 13,
	tltPollerLoadBalanceReconnect					= tltInvalidPbIdBef + 14,
	tltPollerLoadBalanceReconnectDenied				= tltInvalidPbIdBef + 15,
	tltTaxDeductionFailed							= tltInvalidPbIdBef + 16,
	tltAutoCardOutCountdownStartWithCredits			= tltInvalidPbIdBef + 17,
	tltAutoCardOutCountdownStartWithNoCredits		= tltInvalidPbIdBef + 18,
	tltAutoCardOutTimerReset						= tltInvalidPbIdBef + 19,
	tltAutoCardOutTimerExpired						= tltInvalidPbIdBef + 20,
	tltAutoCardOutTimerEndedByPlayer				= tltInvalidPbIdBef + 21,
	tltCardReaderError								= tltInvalidPbIdBef + 22,
	tltFinalTicketInStatusNotReceived				= tltInvalidPbIdBef + 23,
	tltCashoutBtnPressed							= tltInvalidPbIdBef + 24,
	tltArchivingLogsDeniedAlreadyInProgress   		= tltInvalidPbIdBef + 25,
	tltArchivingLogsDeniedRemoteDebuggingDisabled	= tltInvalidPbIdBef + 26,
	tltArchivingLogsFailedNoDisk					= tltInvalidPbIdBef + 27,
	tltArchivingLogsFailedNoDiskSpace				= tltInvalidPbIdBef + 28,
	tltTouchscreenDoesNotWorkWithCurrentOS			= tltInvalidPbIdBef + 29,
	tltEnabledSubGameCountChanged					= tltInvalidPbIdBef + 30,
	tltBadPacketReceivedFromPoller					= tltInvalidPbIdBef + 31,
    tltTicketInRejectedDueToSystemTimeout           = tltInvalidPbIdBef + 32,
    tltReceivedBadDenom                             = tltInvalidPbIdBef + 33,
} TiltCode;
