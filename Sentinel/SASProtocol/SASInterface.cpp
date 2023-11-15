#include "stdafx.h"
#include "SASInterface.h"
#include "SASPollMessage.h"
#include "../Sentinel.h"
#include "LongPoll01Message.h"
#include "LongPoll1AMessage.h"
#include "LongPoll2FMessage.h"
#include "LongPoll6FMessage.h"
#include "SASGeneralPoll.h"
#include "Logging/LogString.h"

using namespace std;

const int MAXSASRETRYATTEMPTS = 9;
const int SASGAMECOMDOWNERRORCOUNT = 30;
const int GAMEADDRESS = 1;

const byte CSASInterface::DEFAULT_VERSION[CSASInterface::VERSION_LENGTH] = { '0' };

CSASInterface::CSASInterface(CSASSerialWrapper *serial, CSASConsumer *sasProtocol, CQueueList *gcfToInternalsQueue, CQueueList *internalsToGCFQueue) :
m_gcfToInternalsQueue(gcfToInternalsQueue), m_internalsToGCFQueue(internalsToGCFQueue), m_sasProtocol(sasProtocol), m_serial(serial), m_gameAddress(GAMEADDRESS)
{
	m_serial->OpenSAS();
	m_findingEGM = false;
	m_gameComStatus = false;
	m_sasStartupQueueEmpty = false;
	m_consecutiveCommErrorCount = 0;
	m_totalRetryCount = 0;
	memcpy(m_version, DEFAULT_VERSION, sizeof(m_version));
	m_ticketExpirationDuration = 0;
}

CSASInterface::~CSASInterface()
{
	delete m_serial;
}

// NOTE: This function is recursive.
void CSASInterface::SendToSAS(CSASPollMessage *msg, byte retryCount)
{
	DWORD numberBytesRead = 0;
	try
	{
		// Make sure we do not send two of the same polls back-to-back && last response was not NACK, retries are considered NACK's
		msg->SetReturnValue(SASPOLL_SUCCESS);
		if (((retryCount == 0 && msg->IncRetryOnNoResponse()) || (!msg->IncRetryOnNoResponse() && m_totalRetryCount == 0)) && GetLastPoll() == msg->GetPollValue() && msg->GetReturnValue() != SASPOLL_NACK && msg->GetPollValue() != 0x63)
		{
			//LogString(L"SASGENERAL", L"Sending Swap poll");

			// See if the message can swap the poll value (i.e. 6F to AF)
			// Added special check for 6f/af, if Af isn't supported then make sure to do dummy poll
			bool bSkipAf = false;
			if (!m_isAfSupported && msg->GetPollValue() == 0x6f)
			{
				bSkipAf = true;
			}
			// if bSkipAf is true, need to make sure that SwapPollValue doesn't get called
			// Tracker# 017973
			if (bSkipAf || !msg->SwapPollValue())
			{
				// Need to do some fake poll so there is not two of the same polls.

				CLongPoll1AMessage *lp = new CLongPoll1AMessage(m_gameAddress, NULL);
				lp->Fill();
				m_serial->WriteSAS(lp->GetSendBuffer(), lp->GetSendBufferSize(), true);

				// TR# 33707 Wait for game to respond so we don't send the next message too soon.
				m_serial->ReadSAS(lp->GetResponseBuffer(), lp->GetResponseBufferSize(), numberBytesRead);

            // simulate response received so binsem is cleared.
            lp->ResponseReceived();
				delete lp;

			}
		}

		// Write the byte stream of data to the serial port.
		m_serial->WriteSAS(msg->GetSendBuffer(), msg->GetSendBufferSize(), true);

		// Set last poll value
		SetLastPoll(msg->GetPollValue());
	}
	catch (...)
	{
	}

	// See if something is waiting for a response from SAS.
	if (retryCount != 2 && msg->IsWaitingForSASResponse())
	{
		// Read the response from SAS. Blocking call
		// TBD. Will this always return a full message? What if we time out?
		uint maxResponseSize = msg->GetResponseBufferSize();
		if (msg->m_isVariableResponse)
		{
			try
			{
				// special handling of response since we don't know how much data is to be read.
				// read through length field
				m_serial->ReadSAS(msg->GetResponseBuffer(), msg->m_variableResponseLenghLocation + msg->m_variableResponseLengthSize, numberBytesRead);

				// inc number of total attempts
				m_totalRetryCount++;
				if (!msg->IncRetryOnNoResponse() && numberBytesRead == 0)
				{
					if (m_totalRetryCount < MAXSASRETRYATTEMPTS)
					{
						LogString("SASGENERAL", "%x Poll ignored, retrying without incrementing retry count.", msg->GetPollValue());
						SendToSAS(msg, retryCount);
					}
					else
					{
						// Reached max attempts
						msg->SetReturnValue(SASPOLL_NACK);
					}

				}
				else if (numberBytesRead < msg->m_variableResponseLenghLocation + msg->m_variableResponseLengthSize)
				{
					// didn't read enough.
					if (numberBytesRead == 0)
					{
						m_consecutiveCommErrorCount++;
					}
					// try to send again
					msg->IncRetryCount();
					msg->SetReturnValue(SASPOLL_INCOMPLETE_DATA);
					LogString("SASGENERAL", "Incomplete data reading %x poll", msg->GetPollValue());
					if (msg->GetRetryCount() <= SASPOLL_RETRY_COUNT)
					{
						LogString("SASGENERAL", "Would have been a 1A poll here");
						LogString("SASGENERAL", "1 Resending %x poll", msg->GetPollValue());
						SendToSAS(msg, retryCount + 1);
					}
				}
				else
				{
					// received enough characters, look for NACK
					if (msg->IsNACK(numberBytesRead))
					{
						// received NACK, could be botched send, retry it
						msg->IncRetryCount();
						msg->SetReturnValue(SASPOLL_NACK);
						if (msg->GetRetryCount() <= SASPOLL_RETRY_COUNT)
						{
							LogString("SASGENERAL", "NACK reading %x poll", msg->GetPollValue());
							LogString("SASGENERAL", "2 Resending %x poll", msg->GetPollValue());
							SendToSAS(msg, retryCount + 1);
						}
					}
					else
					{
						// Response looks good so far
						// read remainder
						UINT remainingSize = 0;
						switch (msg->m_variableResponseLengthSize)
						{
						case 1:
							// byte size
							memcpy(&remainingSize, msg->GetResponseBuffer() + msg->m_variableResponseLenghLocation, 1);
							break;
						case 2:
							// 2 byte size
							memcpy(&remainingSize, msg->GetResponseBuffer() + msg->m_variableResponseLenghLocation, sizeof(WORD));
							break;
						}
						remainingSize += (msg->m_variableResponseSizeConstant - (msg->m_variableResponseLenghLocation + msg->m_variableResponseLengthSize));
						if (remainingSize > maxResponseSize)
						{
							// not enough space, do not continue
							LogString("SASGENERAL", "Not enough space reading %x poll", msg->GetPollValue());
							msg->SetReturnValue(SASPOLL_DATA_TOO_LARGE);
						}
						else
						{
							m_serial->ReadSAS(msg->GetResponseBuffer() + msg->m_variableResponseLenghLocation + msg->m_variableResponseLengthSize, remainingSize, numberBytesRead);

							if (numberBytesRead < remainingSize)
							{
								// didn't read enough.
								if (numberBytesRead == 0)
								{
									m_consecutiveCommErrorCount++;
								}
								// try to send again
								msg->IncRetryCount();
								msg->SetReturnValue(SASPOLL_INCOMPLETE_DATA);
								LogString("SASGENERAL", "Incomplete data reading %x poll", msg->GetPollValue());
								if (msg->GetRetryCount() <= SASPOLL_RETRY_COUNT)
								{
									LogString("SASGENERAL", "3 Resending %x poll", msg->GetPollValue());
									SendToSAS(msg, retryCount + 1);
								}
							}
							else
							{
								// received enough characters, look for NACK
								if (msg->IsNACK(numberBytesRead))
								{
									// received NACK, could be botched send, retry it
									msg->IncRetryCount();
									msg->SetReturnValue(SASPOLL_NACK);
									LogString("SASGENERAL", "NACK reading %x poll", msg->GetPollValue());
									if (msg->GetRetryCount() <= SASPOLL_RETRY_COUNT)
									{
										LogString("SASGENERAL", "4 Resending %x poll", msg->GetPollValue());
										SendToSAS(msg, retryCount + 1);
									}
								}
								else
								{
									// Response looks good so far
									msg->SetReturnValue(SASPOLL_SUCCESS);
								}
							}
							// set new buffersize
							UINT bufferSize = (UINT)numberBytesRead + msg->m_variableResponseLenghLocation + msg->m_variableResponseLengthSize;
							msg->SetResponseBuffer(msg->GetResponseBuffer(), bufferSize);
						}
					}
				}

			}
			catch (...)
			{
			}


		}
		else
		{
			try {
				m_serial->ReadSAS(msg->GetResponseBuffer(), msg->GetResponseBufferSize(), numberBytesRead);

				// inc number of total attempts
				m_totalRetryCount++;
				if (!msg->IncRetryOnNoResponse() && numberBytesRead == 0)
				{
					if (m_totalRetryCount < MAXSASRETRYATTEMPTS)
					{
						LogString("SASGENERAL", "%x Poll ignored, retrying without incrementing retry count.", msg->GetPollValue());
						SendToSAS(msg, retryCount);
					}
					else
					{
						// Reached max attempts
						msg->SetReturnValue(SASPOLL_NACK);
					}

				}
				else if (numberBytesRead < msg->GetResponseBufferSize())
				{
					// didn't read enough.
					if (numberBytesRead == 0)
					{
						m_consecutiveCommErrorCount++;
					}
					// try to send again
					msg->IncRetryCount();
					msg->SetReturnValue(SASPOLL_INCOMPLETE_DATA);
					LogString("SASGENERAL", "Incomplete data reading %x poll", msg->GetPollValue());
					if (!m_findingEGM && msg->GetRetryCount() <= SASPOLL_RETRY_COUNT)
					{
						LogString("SASGENERAL", "Would have been a 1A poll here");
						LogString("SASGENERAL", "5 Resending %x poll", msg->GetPollValue());
						SendToSAS(msg, retryCount + 1);
					}
				}
				else
				{
					// received enough characters, look for NACK
					if (msg->IsNACK(numberBytesRead))
					{
						// received NACK, could be botched send, retry it
						msg->IncRetryCount();
						msg->SetReturnValue(SASPOLL_NACK);
						LogString("SASGENERAL", "NACK reading %x poll", msg->GetPollValue());
						if (msg->GetRetryCount() <= SASPOLL_RETRY_COUNT)
						{
							LogString("SASGENERAL", "6 Resending %x poll", msg->GetPollValue());
							SendToSAS(msg, retryCount + 1);
						}
					}
					else
					{
						// Response looks good so far
						msg->SetReturnValue(SASPOLL_SUCCESS);
					}
				}
			}
			catch (...)
			{
			}
		}
	}


	if (retryCount == 2)
	{
		// Read data that might be returned with this implied NACK. Need to wait for unused data so it isn't processed as a response to a later poll.
		if (msg->IsWaitingForSASResponse())
		{
			if (msg->m_isVariableResponse)
			{
				// read through length field
				m_serial->ReadSAS(msg->GetResponseBuffer(), msg->m_variableResponseLenghLocation + msg->m_variableResponseLengthSize, numberBytesRead);
				if (numberBytesRead == msg->m_variableResponseLenghLocation + msg->m_variableResponseLengthSize)
				{
					// Read remainder
					UINT remainingSize = 0;
					memcpy(&remainingSize, msg->GetResponseBuffer() + msg->m_variableResponseLenghLocation, msg->m_variableResponseLengthSize);
					remainingSize += (msg->m_variableResponseSizeConstant - (msg->m_variableResponseLenghLocation + msg->m_variableResponseLengthSize));
					if (remainingSize < msg->GetResponseBufferSize())
					{
						m_serial->ReadSAS(msg->GetResponseBuffer(), remainingSize, numberBytesRead);
					}
				}
			}
			else 
			{
				m_serial->ReadSAS(msg->GetResponseBuffer(), msg->GetResponseBufferSize(), numberBytesRead);
			}
		}

		// if this is the third attempt, then this poll fails
		msg->SetReturnValue(SASPOLL_NACK);
		// clean up communications by sending resync poll.
		// send resync poll
		// Trying general poll to game 12
		CSASGeneralPoll *gp = new CSASGeneralPoll(12);
		gp->Fill();
		// send general poll to game address 0 so EGM doesn't see implied NACK
		m_serial->WriteSAS(gp->GetSendBuffer(), gp->GetSendBufferSize(), true);

      // Since no response is actually received, need to simulate response to signal binsem
      gp->ResponseReceived();

		delete gp;
	}

	if (msg->IsWaitingForSASResponse() && msg->GetReturnValue() == SASPOLL_SUCCESS)
	{
		if (!msg->VerifyResponsePollValue())
		{
			// Poll value of received data not what was expected.
			SendToSAS(msg, retryCount + 1);
			LogString("SASGENERAL", "SAS Poll Value error: Long Poll %X", msg->GetPollValue());
		}
		else if (!msg->VerifyResponseChecksum())
		{
			// Checksum of received data not what was expected
			SendToSAS(msg, retryCount + 1);
			LogString("SASGENERAL", "SAS Checksum error: Long Poll %X", msg->GetPollValue());
		}
	}

	m_totalRetryCount = 0;

	// if there was an error, inc consecutive error count and notify internals
	if (msg->GetReturnValue() != SASPOLL_SUCCESS)
	{
		m_consecutiveCommErrorCount++;

		CInternalMessage *iMsg = new CInternalMessage(INT_MSG_GAME_COMM_ERROR, m_consecutiveCommErrorCount);

		m_gcfToInternalsQueue->Add(iMsg, QUEUE_PRIORITY_NORMAL);

		// if communications just went down, report it
		if (m_gameComStatus && m_consecutiveCommErrorCount > SASGAMECOMDOWNERRORCOUNT)
		{
			CInternalMessage *iMsg = SetGameComStatus(false);
			m_sasProtocol->ResetStepdowns();

			m_gcfToInternalsQueue->Add(iMsg, QUEUE_PRIORITY_NORMAL);
			LogString("SASGENERAL", "GAME_COMM_DOWN long poll %x", msg->GetPollValue());
		}
		if (!m_gameComStatus)
		{
			// send resync poll
			// Trying general poll to game 12
			CSASGeneralPoll *gp = new CSASGeneralPoll(12);
			gp->Fill();

         // send general poll to game address 0 so EGM doesn't see implied NACK
			m_serial->WriteSAS(gp->GetSendBuffer(), gp->GetSendBufferSize(), true);

         // No response will be received so need to simulate response received
         gp->ResponseReceived();

			delete gp;
		}
	}
	else if (numberBytesRead >= 3 && msg->IsWaitingForSASResponse()) // do not send success if no response was accepted
	{
		// if consecutive error > 0, then notify internals of zero count
		if (m_consecutiveCommErrorCount)
		{
			m_consecutiveCommErrorCount = 0;
			CInternalMessage *iMsg = new CInternalMessage(INT_MSG_GAME_COMM_ERROR, m_consecutiveCommErrorCount);
			m_gcfToInternalsQueue->Add(iMsg, QUEUE_PRIORITY_NORMAL);

			if (!m_gameComStatus)
			{
				iMsg = SetGameComStatus(true);
				m_gcfToInternalsQueue->Add(iMsg, QUEUE_PRIORITY_NORMAL);
				LogString("SASGENERAL", "GAME_COMM_UP long poll %x", msg->GetPollValue());

				// Reset pbt tick count to give egm time to report last transfer, if there is one not yet reported
				m_pbtStartTickCount = CUtilities::GetTickCount();

				// GetEGMCapabilities since something may have changed
				CInternalMessage *egmMsg = new CInternalMessage(INT_MSG_GET_EGM_CAPS);
				m_internalsToGCFQueue->Add(egmMsg, QUEUE_PRIORITY_NORMAL);
			}
		}
	}

}

BYTE CSASInterface::GetLastPoll(void) const
{
	return m_lastPollValue;
}

void CSASInterface::SetLastPoll(BYTE pollValue)
{
	m_lastPollValue = pollValue;
}

bool CSASInterface::GameComStatus()
{
	return m_gameComStatus;
}

bool CSASInterface::GetSasStartupQueueEmpty() const
{
	return m_sasStartupQueueEmpty;
}

bool CSASInterface::HasNoErrors() const
{
	return m_consecutiveCommErrorCount == 0;
}

WORD CSASInterface::GetConsecutiveCommErrorCount() const
{
	return m_consecutiveCommErrorCount;
}

CInternalMessage *CSASInterface::SetGameComStatus(bool status)
{
	m_gameComStatus = status;

	// If game comm is set to true reset error count to 0
	if (status)
	{
		m_consecutiveCommErrorCount = 0;
	}
	else
	{
		m_sasStartupQueueEmpty = false;
	}

	CInternalMessage *msg = new CInternalMessage(INT_MSG_GAME_COMM_STATUS,
		(DWORD)m_gameComStatus,
		(DWORD)m_sasStartupQueueEmpty);
	return msg;
}

CInternalMessage *CSASInterface::ProcessReadQueueIsEmpty()
{
	CInternalMessage *msg(NULL);

	if (m_gameComStatus && !m_sasStartupQueueEmpty)
	{
		m_sasStartupQueueEmpty = true;
		msg = new CInternalMessage(INT_MSG_GAME_COMM_STATUS,
			(DWORD)m_gameComStatus,
			(DWORD)m_sasStartupQueueEmpty);
	}

	return msg;
}

void CSASInterface::SetFindingEGMStatus(bool status)
{
	m_findingEGM = status;
}

void CSASInterface::SetPollingSpeed(short pollingSpeed)
{
	m_serial->SetPollingSpeed(pollingSpeed);
}

short CSASInterface::GetPollingSpeed() const
{
	return m_serial->GetPollingSpeed();
}

void CSASInterface::SetAfSupported(bool supported)
{
	m_isAfSupported = supported;
}

void CSASInterface::SetVersion(const char *version)
{
	if (version != nullptr)
	{
		memcpy(&m_version, version, sizeof(m_version));
	}

	CInternalMessage *msg = new CInternalMessage(INT_MSG_EGM_PROTOCOL_VERSION, GetVersionString());
	m_gcfToInternalsQueue->Add(msg, QUEUE_PRIORITY_NORMAL);
}

const byte *CSASInterface::GetVersion() const
{
	return m_version;
}

byte CSASInterface::GetMajorVersion() const
{
	return m_version[0];
}

string CSASInterface::GetVersionString() const
{
	std::stringstream ss;
	ss << m_version[0] << m_version[1] << m_version[2];
	return ss.str();
}

void CSASInterface::SetTicketExpirationDuration(WORD ticketExpirationDuration)
{
	m_ticketExpirationDuration = ticketExpirationDuration;
}

WORD CSASInterface::GetTicketExpirationDuration() const
{
	return m_ticketExpirationDuration;
}

CInternalMessage *CSASInterface::SetEgmSupportsLP7B(bool egmSupportsLP7B)
{
	CInternalMessage *msg(new CInternalMessage(INT_MSG_EGM_SUPPORTS_LP_7B, (WORD)egmSupportsLP7B));
	return msg;
}

void CSASInterface::UpdateSasReadTimeout()
{
	if (m_serial)
	{
		m_serial->UpdateSasReadTimeout();
	}
}