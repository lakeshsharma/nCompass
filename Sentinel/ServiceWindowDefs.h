#pragma once

const int INFINITE_DURATION = INT_MAX;
const WORD DEFAULT_DURATION = 5000;

typedef enum
{
	AccountBalancesText,
	AvailableBalancePointsCashText,
	AvailableBalanceCashText,
	AvailableBalancePromoText,
	BalanceTierPointText,
	BalancePointText,
	BalancePointsCashText,
	BalanceCompText,
	BalanceCashText,
	BalancePromoText,
	MyAccountText,
	ResortMenuText,
	Custom1Text,
	Custom2Text,
	PersonalBankerText,
	PBT_TransferFromGameText,
	PBT_TransferToGameText,
	TransferText,
	CanceledText,
	PleaseWaitText,
	TransferInProgressText,
	TransferPendingText,
	CallAttendantText,
	ErrorText,
	BartGameText,
	DenomMismatchText,
	CreditTransferText,
	RoomChargeText,
	ValidatingPinText,
	SuccessfulText,
	TransferCompletedText,
	TransferredText,
	ExitingText,
	PersonalBankerFeatureText,
	TimedOutText,
	InvalidTransferText,
	UnderTransferText,
	OverTransferText,
	LimitOfText,
	OverAvailableText,
	BalanceOfText,
	PleaseEnterValueGreaterThanZeroText,
	SomePointsAreUnavailableText,
	FeeVerificationText,
	NoFeeVerificationText,
	SystemBusyText,
	UJPErrorText,
	UJPErrorInsufficientInfoText,
	UJPErrorEGMNotCapableText,
	UJPErrorJackpotAboveLimitText,
	UJPErrorAccountLockedText,
	UJPErrorSessionExpiredText,
	UJPErrorPlayerInVirtualSessionText,
	UJPErrorCreditResetText,
	UJPAmountHeaderText,
	UJPTaxableAmountHeaderText,
	UJPEnterPinText,
	UJPAfterTaxTotalText,
	UJPTaxAdjustmentInProgressText,
	UJPDepositedToSlotMachineText,
	UJPJackpotCouldNotBePaidText,
	UJPTaxFormIsReadyText,
	UJPFederalTaxesText,
	UJPStateTaxesText,
	UJPChangeFedTaxAmountText,
	UJPChangeStateTaxAmountText,
	UJPPaidToSlotMachineText,
	UJPTransferCompleteText,
	UJPTaxableJackpotAmountText,
	UJPAfterTaxAdjustmentInProgressText,
	UJPJackpotIsAdustingWinningsText,
	UJPNoTaxAdjustmentInProgressText,
	UJPJackpotIsNotAdustingWinningsText,
	UJPPaymentWithoutTaxInProgressText,
	UJPTaxWithholdingFailedText,
	UJPAttendantConfirmationText,
	AuthorizationDeniedText,
	SeeAttendantText,
	ErrorInvalidPin,
	ErrorAccessDenied,
	ErrorAccountInUse,
	ErrorNewPINRequired,
	ErrorEnterNewPin,
	ErrorAccessError,
	ErrorAccountHasNoPIN,
	ErrorInvalidID,
	ErrorGoToClubBooth,
	ErrorTryAgainLater,
	ErrorS01,
	ErrorS02,
	ErrorEnteredPinMismatch1,
	ErrorEnteredPinMismatch2,
	ErrorEnteredPinMismatch3,
	PinResetSuccessText1,
	PinResetSuccessText2,
	PinResetErrorSequenceText1,
	PinResetErrorSequenceText2,
	PinResetErrorSequenceText3,
	PinResetErrorBirthdayText1,
	PinResetErrorBirthdayText2,
	PinResetErrorUnknownText,
	EgmAwardFailedText,
	InactiveCashAccount,
	AutoCardoutWarning,
	AutoCardoutInstruction,
	AutoCardoutSessionEnding,
	ExtendedBucketsLoadingText,
	ExtendedBucketsListViewTitle,
	ExtendedBucketsBalanceTitle,
	ExtendedBucketsEarnedTitle,
	ExtendedBucketsExpireTitle,
	CashBalanceNA,
	CashUnavailableMessage,
	ReasonText,
	CashoutErrorText,
	CashoutError_CashUnavailable,
	CashoutError_MaxCardLimit,
	CashoutError_GuestPlayer,
	CashoutError_CashInactive,
	CashoutError_CashUnderTransferLimit,
	CashoutError_CashOverTransferLimit,
	CashoutError_PromoUnderTransferLimit,
	CashoutError_PromoOverTransferLimit,
	LookoutForVoucherOrHandpayText,
	AutoUploadErrorText,
	AutoUploadError_CashUnavailable,
	AutoUploadError_CashInactive,
	AutoUploadError_CashUnderTransferLimit,
	AutoUploadError_CashOverTransferLimit,
	AutoUploadError_PromoUnderTransferLimit,
	AutoUploadError_PromoOverTransferLimit,
	AutoUploadError_GuestPlayer,
	AutoUploadError_YouMayGetVoucherPrintedText,
	EmptyLabel,
	NumberOfLabelType// MUST be last item.  Not valid for use as an enumeration.
} LabelType;

typedef enum
{
	Custom1aParamPrefix,
	Custom1bParamPrefix,
	Custom1cParamPrefix,
	Custom1dParamPrefix,
	Custom1eParamPrefix,
	Custom1fParamPrefix,
	Custom2aParamPrefix,
	Custom2bParamPrefix,
	Custom2cParamPrefix,
	Custom2dParamPrefix,
	Custom2eParamPrefix,
	Custom2fParamPrefix,
	ResortParamPrefix,
	ResortaParamPrefix,
	ResortbParamPrefix,
	ResortcParamPrefix,
	ResortdParamPrefix,
	ResorteParamPrefix,
	ResortfParamPrefix,
	PteParamPrefix,
	UncardedNoSessionPrefix,
	UncardedNoSessionMobilePrefix,
	MobileParamPrefix,
	NumberOfPrefixType// MUST be last item.  Not valid for use as an enumeration.
} PrefixType;

typedef enum
{
	AccessAsset,
	AccessAssetSide,
	AnniverssaryAsset,
	AnniverssaryAssetSide,
	AttractorAsset,
	AttractorAssetSide,
	AutoCardoutAsset,
	AutoCardoutAssetSide,
	BadCardAsset,
	BadCardAssetSide,
	BirthdayAsset,
	BirthdayAssetSide,
	CardedSessionInProgressAsset,
	CardedSessionInProgressAssetSide,
	CongratsAsset,
	CongratAssetSide,
	IneligiblePlayerAsset,
	IneligiblePlayerAssetSide,
	PromotionEligibilityAsset,
	PromotionEligibilityAssetSide,
	PromotionProximityAsset,
	PromotionProximityAssetSide,
	PromotionTriggeredAsset,
	PromotionTriggeredAssetSide,
	NonWinnerFinaleAsset,
	NonWinnerFinaleAssetSide,
	AwardAsset,
	AwardAssetSide,
	JackpotAnnouncementAsset,
	JackpotAnnouncementAssetSide,
	PromotionBlankAsset,
	PromotionBlankAssetSide,
	KeepPlayingAsset,
	KeepPlayingAssetSide,
	ScheduleEndAsset,
	ScheduleEndAssetSide,
	SplashdownWinnerEndAsset,
	SplashdownWinnerEndAssetSide,
	ResortAsset,
	ResortAssetSide,
	ResortaAsset,
	ResortbAsset,
	ResortcAsset,
	ResortdAsset,
	ResorteAsset,
	ResortfAsset,
	ResortaAssetSide,
	ResortbAssetSide,
	ResortcAssetSide,
	ResortdAssetSide,
	ResorteAssetSide,
	ResortfAssetSide,
	Custom1aAsset,
	Custom1bAsset,
	Custom1cAsset,
	Custom1dAsset,
	Custom1eAsset,
	Custom1fAsset,
	Custom2aAsset,
	Custom2bAsset,
	Custom2cAsset,
	Custom2dAsset,
	Custom2eAsset,
	Custom2fAsset,
	Custom1aAssetSide,
	Custom1bAssetSide,
	Custom1cAssetSide,
	Custom1dAssetSide,
	Custom1eAssetSide,
	Custom1fAssetSide,
	Custom2aAssetSide,
	Custom2bAssetSide,
	Custom2cAssetSide,
	Custom2dAssetSide,
	Custom2eAssetSide,
	Custom2fAssetSide,
	WinnerPickAsset,
	WinnerPickAssetSide,
	WinnerPickedAsset,
	WinnerPickedAssetSide,
	nRichLevelProgressAsset,
	SplashdowncountdowntimerAsset,
	OdometerAsset,
	PlayXGetYCounterAsset,
	PteCounterAsset,
	MassMarketingAsset,
	MassMarketingAssetSide,
	UJP,
	UJPSide,
	UJPCardedTeaserAsset,
	UJPCardedTeaserAssetSide,
	UJPUncardedTeaserAsset,
	UJPUncardedTeaserAssetSide,
	UJPSuccessAsset,
	UJPSuccessAssetSide,
	UJPErrorAsset,
	UJPErrorAssetSide,
	PTEAnticipation,
	PTEAnticipationSide,
	PTEAward,
	PTEAwardSide,
	PTEEnd,
	PTEEndSide,
	PTEResults,
	PTEResultsSide,
	PTELevelUp,
	PTELevelUpSide,
	PTELeaderboard,
	PTELeaderboardSide,
	Profile,
	ProfileSide,
	UncardedNoSession,
	Notification,
	Mobile,
	MobileSide,
	UncardedNoSessionMobile,
	PromoGame,
	PromoGameSide,
	ScatterCounter,
	ProgressivePrize,
	ProgressivePrizeSide,
	PrizeOdometer,
	Blank,
	ExtendedBuckets,
	ExtendedBucketsSide,
	InvalidAssetType,//MUST be last item, Not valid for use as an enumeration.
	NumberOfAssetType = InvalidAssetType// MUST be last item.  Not valid for use as an enumeration.
} AssetType;

typedef enum
{
	None,
	ATI_Button_TaskbarMain,
	ATI_Button_Resort,
	ATI_Button_Account,
	ATI_Button_Custom1,
	ATI_Button_Custom2,
	ATI_Button_PersonalBankerInProgress,
	ATI_Button_AccountBack,
	ATI_Button_AccountExit,
	ATI_Button_TaskbarLogo,
	ATI_Button_Custom1a,
	ATI_Button_Custom1b,
	ATI_Button_Custom1c,
	ATI_Button_Custom1d,
	ATI_Button_Custom1e,
	ATI_Button_Custom1f,
	ATI_Button_Custom2a,
	ATI_Button_Custom2b,
	ATI_Button_Custom2c,
	ATI_Button_Custom2d,
	ATI_Button_Custom2e,
	ATI_Button_Custom2f,
	ATI_Button_AccountBalance,
	ATI_Button_PersonalBanker,
	ATI_Button_Goodbye,
	ATI_Button_PBTBalances,
	ATI_Button_PBTTransferToGame,
	ATI_Button_PBTTransferFromGame,
	ATI_Button_PBTCashToGame,
	ATI_Button_PBTPointsToGame,
	ATI_Button_PBTPromoToGame,
	ATI_Button_PBTCashFromGame,
	ATI_Button_PBTPromoFromGame,
	ATI_Button_PBTFive,
	ATI_Button_PBTTen,
	ATI_Button_PBTTwenty,
	ATI_Button_PBTFifty,
	ATI_Button_PBTHundred,
	ATI_Button_PBTOther,
	ATI_Button_PBTOtherCash,
	ATI_Button_PBTAll,
	ATI_Button_PBTCashFromGameAll,
	ATI_Button_PBTCreditTransfers,
	ATI_Button_PBTCreditTransferToGame,
	ATI_Button_PBTCreditTransferTwenty,
	ATI_Button_PBTCreditTransferForty,
	ATI_Button_PBTCreditTransferSixty,
	ATI_Button_PBTCreditTransferEighty,
	ATI_Button_PBTCreditTransferHundred,
	ATI_Button_PBTCreditTransferOther,
	ATI_Button_PBTCreditTransferYes,
	ATI_Button_PBTCreditTransferNo,
	ATI_Button_PBAcknowledgePointsUnavailable,
	ATI_Button_PBTEnter,
	ATI_Button_CardedSessionInProgress,
	ATI_Button_Resorta,
	ATI_Button_Resortb,
	ATI_Button_Resortc,
	ATI_Button_Resortd,
	ATI_Button_Resorte,
	ATI_Button_Resortf,
	ATI_Button_UJP_CallHost,
	ATI_Button_EnterNewPinOkay,
	ATI_Button_Profile,
	ATI_Button_Mobile,
	ATI_Button_ContentWindowCloseLeftSide,
	ATI_Button_ContentWindowCloseRightSide,
	ATI_Button_ContentWindowCloseBottom,
	ATI_Button_MobileConnecting,
	ATI_Button_EgmAwardFailedOkay,
	ATI_Button_MoreBalances
}ButtonID;

