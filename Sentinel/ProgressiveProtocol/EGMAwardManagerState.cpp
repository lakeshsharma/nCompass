#include "stdafx.h"
#include "EGMAwardManagerState.h"
#include "SASProtocol/SASProtocol.h"
#include "GlobalDefs.h"
#include "Logging/LogString.h"

const DWORD CEGMAwardManagerState::SIGNAL_TICK_COUNT_EXPIRED = 20 * MILLISECONDS_PER_SECOND;
const DWORD CEGMAwardManagerState::AWARD_TICK_COUNT_EXPIRED = 10 * MILLISECONDS_PER_MINUTE;
const DWORD CEGMAwardManagerState::GCF_DELAY_TICK_COUNT_EXPIRED = 3 * MILLISECONDS_PER_SECOND;
const DWORD CEGMAwardManagerState::PRIZE_AWARD_LOCK_REFRESH = 90 * MILLISECONDS_PER_SECOND;

// For our AFT lock pending timeout for retries, double the timeout used by the SAS protocol, to allow the SAS protocol to report lock results first.
const DWORD CEGMAwardManagerState::AFT_LOCK_PENDING_TICK_COUNT_EXPIRED = 2 * LOCK_DURATION * MILLISECONDS_PER_HUNDRETH_SECOND;
