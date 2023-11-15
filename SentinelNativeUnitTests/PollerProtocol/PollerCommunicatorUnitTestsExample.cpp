#if 000//TODO-PORT
#include "stdafx.h"
//#include "PollerCommunicator.h" // Normally use the Sentinel's class.

#include "Sentinel.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	#define USE_SESSION_KEY_CRITICAL_SECTION
	#define USE_READ_THREAD

// For multi-threaded tests without critical sections:
// Running the test on a multi-CPU multi-threaded box (support for running threads simultaneously) would probably cause failures sooner on the #4 test.
//	#define MAXPACKETLENGTH 4096 // HAVE run #4 Test with 10,000 iterations with no encryption errors. [On a 1-CPU 1-thread VM]
//	#define MAXPACKETLENGTH (4096 * 2) // SOMETIMES have run #4 Test with 10,000 iterations with no encryption errors. [On a 1-CPU 1-thread VM]
//	#define MAXPACKETLENGTH (1024*16) // SOMETIMES have run #4 Test with 10,000 iterations with no encryption errors.
//	#define MAXPACKETLENGTH (1024*32) // have NOT run #4 Test with 10,000 iterations with no encryption errors.  Some test engine failures -- "vstest.executionengine.x86.exe has stopped working".
	// Often runs for 15-35 seconds before failures occur?

	#define MAXPACKETLENGTH (256*256*4) // Failures typically occur after 5 -15 seconds, for multi-threaded tests without critical sections. [On a 1-CPU 1-thread VM]
	#define MAXMESSAGELENGTH (MAXPACKETLENGTH / 2 - 256) // Recommended.  Allow worst-case headers, empty extended field, and DLE encoding.

	#define DEFAULTDLEREPLACEMENT 0xE0
	#define STX 0xEB
	#define ETX 0xEE

	// Use large counts (e.g., 10000 or more) for endurance testing.
	// Timeout periods for endurance tests may need to be adjusted for large counts.
	#define ENDURANCE_TEST_LOOP_COUNT 10
	static const DWORD ENDURANCE_TEST_MINIMUM_PERIOD = 100; // 100 milliseconds == 0.1 second.  Minimum period that an endurance test is allowed to run.
	static const DWORD ENDURANCE_TEST_MAXIMUM_PERIOD = (10 * 1000); // 10000 milliseconds == 10 seconds.  Maximum period that an endurance test is allowed to run.
	static const DWORD MAXIMUM_SHUTDOWN_TICK_COUNT = (10 * 1000); // 10000 ticks == 10 seconds. Maximum number of tick counts to wait for shutdown to complete.

	static const CTime CTIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
	static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.

	volatile bool shutdown(false);
	volatile unsigned createCount(0);
	volatile unsigned importCount(0);

	// Class CAsyncError used to report errors that occur on worker threads.
	// 2013-10-28 Using Assert:IsTrue() and similar methods is currently unreliable.
	class CAsyncError
	{
	public:
		CAsyncError()
		{
			m_errorOccurred = false;
		}
		
		~CAsyncError()
		{
		}

	public:
		volatile bool m_errorOccurred;
		CString m_errorStr;
	};

	class PollerCommunicator
	{
	public:
		BYTE* _extendedpacket;
		long extendedpacketlength;
		BYTE* _buffer;
		BYTE* _tempbuffer;
		BYTE* _tempbufferforsend;
		BYTE* _tempbufferforrecv;
		BYTE* _tempbufferforCreate;
		long _tempbufferforrecvlength;
		long _bufferlength;

		HCRYPTPROV hProv;
		HCRYPTKEY hExchangePubKey;
		HCRYPTKEY hRemotePubKey;
		HCRYPTKEY hSessionKey;
		HCRYPTKEY hSessionVerifyKey; // Duplicate session key to decrypt sent messages.
		PBYTE pbExchangePubKeyBlob;
		PBYTE pbRemotePubKeyBlob;
		PBYTE pbSessionKeyBlob;
		DWORD dwExchangePubKeyBlobLength;
		DWORD dwRemoteKeyBlobLength;
		DWORD dwSessionKeyBlobLength;
		bool bEncryptionSetup;
		bool bpublickeywassent;
		bool bsessionkeywassent;

		CCriticalSection m_sessionKeyCriticalSection;
		CAsyncError m_readThreadError;

		PollerCommunicator() :
			_extendedpacket(NULL),
			extendedpacketlength(0),
			_buffer(NULL),
			_tempbuffer(NULL),
			_tempbufferforsend(NULL),
			_tempbufferforrecv(NULL),
			_tempbufferforCreate(NULL),
			_tempbufferforrecvlength(0),
			_bufferlength(0),

			hProv(NULL),
			hExchangePubKey(NULL),
			hRemotePubKey(NULL),
			hSessionKey(NULL),
			hSessionVerifyKey(NULL),
			pbExchangePubKeyBlob(NULL),
			pbRemotePubKeyBlob(NULL),
			pbSessionKeyBlob(NULL),
			dwExchangePubKeyBlobLength(0),
			dwRemoteKeyBlobLength(0),
			dwSessionKeyBlobLength(0),
			bEncryptionSetup(false),
			bpublickeywassent(false),
			bsessionkeywassent(false)
		{
			_buffer = new BYTE[MAXPACKETLENGTH];
			_tempbuffer = new BYTE[MAXPACKETLENGTH];
			_tempbufferforsend = new BYTE[MAXPACKETLENGTH];
			_extendedpacket = new BYTE[MAXPACKETLENGTH];
			_tempbufferforrecv = new BYTE[MAXPACKETLENGTH];
			_tempbufferforCreate = new BYTE[MAXPACKETLENGTH];
			ZeroMemory(_buffer,MAXPACKETLENGTH);
		}

		~PollerCommunicator()
		{
			TearDownEncryption();

			delete [] _buffer;
			delete [] _tempbuffer;
			delete [] _tempbufferforsend;
			delete [] _extendedpacket;
			delete [] _tempbufferforrecv;
			delete [] _tempbufferforCreate;
		}

		bool PollerCommunicator::SetUpEncryption()
		{
			//Initialize Encryption provider
			if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0)) 
			{
				DWORD ret = GetLastError();
				if (ret !=  NTE_BAD_KEYSET)
				{
					return false;
				}
				else
				{
					if (!CryptAcquireContext (&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) 
					{
						return false;
					}
				}
			}


			//Create Pub/Priv key set
			if (!CryptGenKey (hProv, CALG_RSA_KEYX, CRYPT_EXPORTABLE, &hExchangePubKey)) 
			{
				return false;
			}
			//Export public key into blob
			if (!CryptExportKey (hExchangePubKey, 0, PUBLICKEYBLOB, 0, NULL, 
				&dwExchangePubKeyBlobLength)) 
			{
				return false;
			}
			if ( (pbExchangePubKeyBlob = (PBYTE)malloc(dwExchangePubKeyBlobLength) ) == NULL) 
			{
				return false;
			}
			ZeroMemory(pbExchangePubKeyBlob,dwExchangePubKeyBlobLength);
			// Export the public key into a simple key BLOB.
			if (!CryptExportKey (hExchangePubKey, 0, PUBLICKEYBLOB, 0, pbExchangePubKeyBlob, 
				&dwExchangePubKeyBlobLength)) 
			{
				return false;
			}

			bEncryptionSetup = true;
			return true;
		}

		//DLE(Data length encoding) is an attempt at making the packet parsable by receiving end.
		//For instance your parsing bytes are 0xEB and 0xEE if a packet contains one of these two bytes
		//internally the parsing on the receiving end would parse out bad data. So we change any bytes
		//between the Header(stx) and footer(etx) into two bytes. So an 0xEB would become 0xE0 and 0xE0
		//- replacement byte.
		long PollerCommunicator::DLEEncode(BYTE *pabyPacketData, BYTE *pabyEncodeBuffer, long lPacketLength)
		{
			long lEncodeDataLength = 0;
			BYTE *pData = pabyPacketData;
			BYTE *pEncodeBuffer = pabyEncodeBuffer;

			while (lPacketLength-- > 0)
			{
				switch (*pData)
				{
				case DEFAULTDLEREPLACEMENT:
				case STX:
				case ETX:
					*(pEncodeBuffer++) = DEFAULTDLEREPLACEMENT;						//replace it with replacement byte
					*(pEncodeBuffer++) = *(pData++) - DEFAULTDLEREPLACEMENT;	//replace it with byte - replacement byte
					lEncodeDataLength += 2;
					break;
				default:
					*(pEncodeBuffer++) = *pData++;
					lEncodeDataLength++;
					break;
				}
			}
			return lEncodeDataLength;
		}

		void PollerCommunicator::TearDownEncryption()
		{
			if(pbExchangePubKeyBlob) free(pbExchangePubKeyBlob);
			if (hProv) CryptReleaseContext (hProv, 0);
			if (hExchangePubKey) CryptDestroyKey(hExchangePubKey);
			if (hRemotePubKey) CryptDestroyKey(hRemotePubKey);
			if (hSessionKey) CryptDestroyKey(hSessionKey);
			if (hSessionVerifyKey != NULL) CryptDestroyKey(hSessionVerifyKey);
		}

		bool CreatePacketToSend(const BYTE *packettomassage,BYTE *massagedpacket, long packetlength ,long *massagedpacketlength)
		{
			bool retval = false;
			long overallpacketlength = 0;

			memcpy(_tempbufferforCreate,packettomassage,packetlength);
			ZeroMemory(massagedpacket,MAXPACKETLENGTH);

			//set length of extended to zero if no data needs to go
			DWORD extendedlength = 0;
			memcpy( &massagedpacket[overallpacketlength],&extendedlength, sizeof(DWORD));
			overallpacketlength += sizeof(DWORD);

			if(packetlength > 0)
			{
				//((CSentinelApp *)AfxGetApp())->LogString(L"GENERAL", L"PollerCommunicator::CreatePacketToSend Data being encrypted\n");			
				DWORD testit = packetlength;
				// Encrypt the data.
				if (!CryptEncrypt (hSessionKey, 0, TRUE, 0, _tempbufferforCreate, &testit, packetlength))
				{	
					((CSentinelApp *)AfxGetApp())->LogString(L"ERROR", L"PollerCommunicator::CreatePacketToSend Data failed to be encrypted\n");
					DWORD ret = GetLastError();
					int x = NTE_BAD_LEN;

					retval = false;
				}
				//((CSentinelApp *)AfxGetApp())->LogString(L"GENERAL", L"PollerCommunicator::CreatePacketToSend Data encrypted\n");

				//copy packet data to packet
				memcpy( &massagedpacket[overallpacketlength], _tempbufferforCreate, packetlength );
				overallpacketlength += packetlength;
			}

			long templength = DLEEncode(massagedpacket,  _tempbufferforsend, overallpacketlength);

			massagedpacket[0] = STX;

			memcpy( &massagedpacket[1],_tempbufferforsend,templength);
			//overallpacketlength += templength;

			massagedpacket[ templength+1 ] = ETX;
			*massagedpacketlength = templength + 2;
			retval = true;

			return retval;
		}

		//DLE(Data length decoding) is an attempt at making the packet parsable by receiving end.
		//For instance your parsing bytes are 0xEB and 0xEE if a packet contains one of these two bytes
		//internally the parsing on the receiving end would parse out bad data. So we change any bytes
		//between the Header(stx) and footer(etx) into two bytes. So an 0xEB would become 0xE0 and E0 - EB.
		long PollerCommunicator::DLEDecode(const BYTE *pabyPacketData, BYTE *pabyDecodeBuffer, long lPacketLength)
		{
			const BYTE *pDecodeData = pabyPacketData;
			BYTE *pDecodeBuffer = pabyDecodeBuffer;
			long lDecodeDataLength = 0;


			while (lPacketLength > 0)
			{
				if (*pDecodeData == DEFAULTDLEREPLACEMENT)
				{
					pDecodeData++;
					*(pDecodeBuffer++) = DEFAULTDLEREPLACEMENT + *(pDecodeData++);
					lPacketLength -= 2;
				}
				else
				{
					*(pDecodeBuffer++) = *(pDecodeData++);
					lPacketLength -= 1;
				}

				lDecodeDataLength++;
			}

			return lDecodeDataLength;
		}
	}; // class CPollerCommunicator.

	TEST_CLASS(PollerCommunicatorExample)
	{


		TEST_METHOD(SetUpEncryptionTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
		}

		TEST_METHOD(CryptImportKey_RemotePublicTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			const DWORD dwRemoteKeyBlobLength = 0x00000094;
			BYTE pbRemotePubKeyBlob[dwRemoteKeyBlobLength] = {
				0x06, 0x02, 0x00, 0x00, 0x00, 0xa4, 0x00, 0x00, 0x52, 0x53, 0x41, 0x31, 0x00, 0x04, 0x00, 0x00,
				0x01, 0x00, 0x01, 0x00, 0x0d, 0x90, 0xe0, 0x7e, 0xfc, 0x73, 0x38, 0xab, 0x03, 0xf6, 0xd9, 0xf3,
				0xa3, 0xa2, 0xdc, 0xca, 0xfe, 0x1b, 0x80, 0xb7, 0x3e, 0xd9, 0x52, 0xa8, 0xde, 0xd2, 0x12, 0x2f,
				0x78, 0x15, 0x18, 0x99, 0xaf, 0x0f, 0x49, 0xe5, 0x08, 0x57, 0x93, 0x2e, 0x2d, 0x16, 0xa0, 0x38,
				0x22, 0xc4, 0x6f, 0x6e, 0x0c, 0x8a, 0xea, 0xf6, 0x1e, 0x2b, 0xcb, 0x4b, 0xb7, 0xc7, 0xb2, 0xfe,
				0x37, 0x57, 0x67, 0x47, 0xca, 0x4b, 0x35, 0x42, 0x36, 0x9f, 0xc4, 0x41, 0x98, 0x93, 0xe8, 0x30,
				0x53, 0x31, 0x66, 0xd2, 0x9f, 0x2e, 0x66, 0x59, 0x12, 0xf7, 0xdb, 0x4e, 0x49, 0xce, 0x57, 0x5a,
				0x70, 0xae, 0xc9, 0xd8, 0x47, 0x9b, 0x0b, 0xe4, 0x7b, 0xf0, 0xee, 0xba, 0xe5, 0x93, 0x48, 0x01,
				0x33, 0x20, 0x62, 0x47, 0x75, 0x9c, 0x79, 0xe6, 0x17, 0x51, 0xa4, 0x45, 0x12, 0xd6, 0xec, 0x18,
				0xab, 0x0e, 0xa3, 0xec };
			if (!CryptImportKey(pc.hProv, pbRemotePubKeyBlob, dwRemoteKeyBlobLength, 0, 0, &pc.hRemotePubKey))
			{
				DWORD lastError = GetLastError();
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("CryptImportKey(pbRemotePubKeyBlob) lastError=%8.8x in %s"),
					(unsigned)lastError,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}
		}

		TEST_METHOD(CreatePacketToSend1Test)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Generate a session key, as if it was received from a Poller.
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if ( (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					Assert::IsTrue(retval, CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
				}
				ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);

				// Export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			// Import the session key.
			if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
			{
				DWORD lastError = GetLastError();
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("CryptImportKey(pbSessionKeyBlob) lastError=%8.8x in %s"),
					(unsigned)lastError,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			long tosendit = 0;
			const long msgToSendLength = 4;
			const BYTE msgToSend[msgToSendLength] = { 1, 2, 3, 4 };
			memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);
			retval = pc.CreatePacketToSend(msgToSend,pc._tempbuffer,msgToSendLength,&tosendit);
			Assert::IsTrue(retval, CString(_T("CreatePacketToSend in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
		}

		TEST_METHOD(CreatePacketToSend2_DuplicateTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Generate a session key, as if it was received from a Poller.
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if ( (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					Assert::IsTrue(retval, CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
				}
				ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);

				// Export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			const long msgToSendLength = MAXMESSAGELENGTH;
			BYTE *varMsgToSend = new BYTE[msgToSendLength];

			// Init msgToSend[] with a variety of values.
			for (int index = 0; index < msgToSendLength; ++index)
			{
				if (0 == (index & 1))
				{
					// Low byte of half-speed counter.
					varMsgToSend[index] = (BYTE)(index / 2);
				}
				else
				{
					// Low byte of one third-speed counter.
					varMsgToSend[index] = (BYTE)(index / 3);
				}
			}
			const BYTE *msgToSend(varMsgToSend);

			// Import the session key.
			if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
			{
				DWORD lastError = GetLastError();
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("CryptImportKey(pbSessionKeyBlob) lastError=%8.8x in %s"),
					(unsigned)lastError,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Create packet to send.
			long tosendit = 0;
			memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);
			retval = pc.CreatePacketToSend(msgToSend, pc._tempbuffer, msgToSendLength, &tosendit);
			Assert::IsTrue(retval, CString(_T("CreatePacketToSend in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			// Save the result from 1st CreatePacketToSend() call.
			const long firstMsgToSendLength = tosendit;
			BYTE *varFirstMsgToSend = new BYTE[firstMsgToSendLength];
			memcpy_s(varFirstMsgToSend, firstMsgToSendLength, pc._tempbuffer, tosendit);
			const BYTE *firstMsgToSend(varFirstMsgToSend);

			// Import the session key, to get same encrypted session output again.
			if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
			{
				DWORD lastError = GetLastError();
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("CryptImportKey(pbSessionKeyBlob) lastError=%8.8x in %s"),
					(unsigned)lastError,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Create packet to send, again.
			/*long*/ tosendit = 0;
			memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);
			retval = pc.CreatePacketToSend(msgToSend, pc._tempbuffer, msgToSendLength, &tosendit);
			Assert::IsTrue(retval, CString(_T("CreatePacketToSend in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			if (firstMsgToSendLength != tosendit)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("firstMsgToSendLength=%ld != tosendit=%ld in %s"),
					(long)firstMsgToSendLength,
					(long)tosendit,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			const int memcmpResult = memcmp(firstMsgToSend, pc._tempbuffer, firstMsgToSendLength);
			if (memcmpResult != MEMCMP_EQUAL)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("memcmp(firstMsgToSend, pc._tempbuffer) in %s"),
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Cleanup.
			delete [] msgToSend;
			delete [] firstMsgToSend;
		}

		TEST_METHOD(CreatePacketToSend2_VerifyTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Generate a session key, as if it was received from a Poller.
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if ( (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					Assert::IsTrue(retval, CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
				}
				ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);

				// Export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			const long msgToSendLength = MAXMESSAGELENGTH;
			BYTE *varMsgToSend = new BYTE[msgToSendLength];

			// Init msgToSend[] with a variety of values.
			for (int index = 0; index < msgToSendLength; ++index)
			{
				if (0 == (index & 1))
				{
					// Low byte of half-speed counter.
					varMsgToSend[index] = (BYTE)(index / 2);
				}
				else
				{
					// Low byte of one third-speed counter.
					varMsgToSend[index] = (BYTE)(index / 3);
				}
			}
			const BYTE *msgToSend(varMsgToSend);

			// Import the session key.
			if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
			{
				DWORD lastError = GetLastError();
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("CryptImportKey(pbSessionKeyBlob) lastError=%8.8x in %s"),
					(unsigned)lastError,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Create packet to send.
			long tosendit = 0;
			memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);
			retval = pc.CreatePacketToSend(msgToSend, pc._tempbuffer, msgToSendLength, &tosendit);
			Assert::IsTrue(retval, CString(_T("CreatePacketToSend in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			// Verify packet.

			// Verify minimum required total length.  Note that both signed and unsigned values are used.
			const DWORD extendedlengthRequired = 0;
			const long sizeofSTX = sizeof(byte);
			const long sizeofETX = sizeof(byte);
			const long minimumRequiredLength = sizeof(extendedlengthRequired) + extendedlengthRequired + msgToSendLength +
				sizeofSTX + sizeofETX;
			if (tosendit < minimumRequiredLength)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("tosend=%ld < minimumRequiredLength=%ld in %s"),
					(long)tosendit,
					(long)minimumRequiredLength,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Verify STX.
			if (pc._tempbuffer[0] != STX)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("pc._tempbuffer[0]=%u != STX=%u in %s"),
					(unsigned)pc._tempbuffer[0],
					(unsigned)STX,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Verify ETX.
			if (pc._tempbuffer[tosendit - 1] != ETX)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("pc._tempbuffer[tosendit - 1]=%u != ETX=%u in %s"),
					(unsigned)pc._tempbuffer[tosendit - 1],
					(unsigned)ETX,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// DLE Decode message sent.
			BYTE *varMsgSent = new BYTE[MAXPACKETLENGTH];
			const long msgSentLength = pc.DLEDecode(pc._tempbuffer + sizeofSTX, varMsgSent, tosendit - sizeofSTX - sizeofETX);

			// Verify message sent length after decoding.
			const long requiredDecodeLength = sizeof(extendedlengthRequired) + extendedlengthRequired + msgToSendLength;
			if (msgSentLength != requiredDecodeLength)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("msgSentLength=%ld != requiredDecodeLength=%ld in %s"),
					(long)msgSentLength,
					(long)requiredDecodeLength,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Verify extended length field after decoding.
			int memcmpExtendedLengthResult = memcmp(varMsgSent, &extendedlengthRequired, sizeof(extendedlengthRequired));
			if (memcmpExtendedLengthResult != MEMCMP_EQUAL)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("extendedlength field != extendedlengthRequired=%u in %s"),
					(unsigned)extendedlengthRequired,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			if (extendedlengthRequired > 0)
			{
				// Get substitute for the extended field sent.
				BYTE *extendedFieldToSend = new BYTE[extendedlengthRequired];

				// Verify extended field sent after decoding.
				BYTE *extendedFieldSent = varMsgSent + sizeof(extendedlengthRequired);
				int memcmpFieldResult = memcmp(extendedFieldSent, extendedFieldToSend, sizeof(extendedlengthRequired));
				if (memcmpFieldResult != MEMCMP_EQUAL)
				{
					CString extendedFieldSentStr(CUtilities::ToHexString(extendedFieldSent, extendedlengthRequired));
					CString extendedFieldToSendStr(CUtilities::ToHexString(extendedFieldToSend, extendedlengthRequired));
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("message field sent[%s] != message field to send[%s] in %s"),
						static_cast<LPCTSTR>(extendedFieldSentStr),
						static_cast<LPCTSTR>(extendedFieldToSendStr),
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
				delete extendedFieldToSend;
			}

			// Calculate length of the message field sent.
			const long msgFieldLength = msgSentLength - sizeof(extendedlengthRequired) - extendedlengthRequired;

			DWORD msgFieldLengthResult((DWORD)msgFieldLength);
			BYTE *msgFieldSent = varMsgSent + sizeof(extendedlengthRequired) + extendedlengthRequired;
			if (msgFieldLength > 0)
			{
				if (!CryptDecrypt (pc.hSessionKey, 0, TRUE, 0, msgFieldSent, &msgFieldLengthResult))
				{
					// Failed to decrypt message field.
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptDecrypt(msgFieldSent) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			// Verify message field length after decryption.
			if (msgFieldLengthResult != msgFieldLength)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("msgFieldLengthResult=%ld != msgFieldLength=%u in %s"),
					(long)msgFieldLengthResult,
					(unsigned)msgFieldLength,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Verify message field length vs msgToSendLength after decryption..
			if (msgFieldLength != msgToSendLength)
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("msgFieldLength=%ld != msgToSendLength=%ld in %s"),
					(long)msgFieldLength,
					(long)msgToSendLength,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Verify message field after decryption.
			int memcmpMsgFieldResult = memcmp(msgFieldSent, msgToSend, msgFieldLength);
			if (memcmpMsgFieldResult != MEMCMP_EQUAL)
			{
				CString msgFieldSentStr(CUtilities::ToHexString(msgFieldSent, msgFieldLength));
				CString msgToSendStr(CUtilities::ToHexString(msgToSend, msgToSendLength));
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("message field sent[%s] != message field to send[%s] in %s"),
					static_cast<LPCTSTR>(msgFieldSentStr),
					static_cast<LPCTSTR>(msgToSendStr),
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Cleanup.
			delete [] msgToSend;
			delete [] varMsgSent;
		}

		TEST_METHOD(CreatePacketToSend3_DuplicateTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Generate a session key, as if it was received from a Poller.
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if ( (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					Assert::IsTrue(retval, CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
				}
				ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);

				// Export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			const long msgToSendLength = MAXMESSAGELENGTH;
			BYTE *varMsgToSend = new BYTE[msgToSendLength];

			// Init msgToSend[] with a variety of values.
			for (int index = 0; index < msgToSendLength; ++index)
			{
				if (0 == (index & 1))
				{
					// Low byte of half-speed counter.
					varMsgToSend[index] = (BYTE)(index / 2);
				}
				else
				{
					// Low byte of one third-speed counter.
					varMsgToSend[index] = (BYTE)(index / 3);
				}
			}
			const BYTE *msgToSend(varMsgToSend);

			long firstMsgToSendLength(0);
			BYTE *firstMsgToSend(NULL);

			for (int createIndex = 0; createIndex < 1000; ++createIndex)
			{
				// Import the session key.
				if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptImportKey(pbSessionKeyBlob) lastError=%8.8x createIndex=%d in %s"),
						(unsigned)lastError,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Create packet to send.
				long tosendit = 0;
				memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);
				retval = pc.CreatePacketToSend(msgToSend, pc._tempbuffer, msgToSendLength, &tosendit);
				{
					CString failedMsg;
					failedMsg.Format(_T("CreatePacketToSend createIndex=%d in %s"),
					(int)createIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				if (0 == createIndex)
				{
					// Save the result from the first CreatePacketToSend() call.
					firstMsgToSendLength = tosendit;
					firstMsgToSend = new BYTE[firstMsgToSendLength];
					memcpy_s(firstMsgToSend, firstMsgToSendLength, pc._tempbuffer, tosendit);
				}
				else
				{
					// Compare the result from the first CreatePacketToSend() call to the current call.
					if (firstMsgToSendLength != tosendit)
					{
						retval = false;
						CString failedMsg;
						failedMsg.Format(_T("firstMsgToSendLength=%ld != tosendit=%ld createIndex=%d in %s"),
							(long)firstMsgToSendLength,
							(long)tosendit,
							(int)createIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}

					const int memcmpResult = memcmp(firstMsgToSend, pc._tempbuffer, firstMsgToSendLength);
					if (memcmpResult != MEMCMP_EQUAL)
					{
						retval = false;
						CString failedMsg;
						failedMsg.Format(_T("memcmp(firstMsgToSend, pc._tempbuffer) createIndex=%d in %s"),
							(int)createIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
				}
			}


			// Cleanup.
			delete [] msgToSend;
			delete [] firstMsgToSend;
		}

		TEST_METHOD(CreatePacketToSend3_VerifyImportManyTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Generate a session key, as if it was received from a Poller.
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if ( (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					Assert::IsTrue(retval, CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
				}
				ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);

				// Export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			const long msgToSendLength = MAXMESSAGELENGTH;
			BYTE *varMsgToSend = new BYTE[msgToSendLength];

			// Init msgToSend[] with a variety of values.
			for (int index = 0; index < msgToSendLength; ++index)
			{
				if (0 == (index & 1))
				{
					// Low byte of half-speed counter.
					varMsgToSend[index] = (BYTE)(index / 2);
				}
				else
				{
					// Low byte of one third-speed counter.
					varMsgToSend[index] = (BYTE)(index / 3);
				}
			}
			const BYTE *msgToSend(varMsgToSend);

			for (int createIndex = 0; createIndex < 1000; ++createIndex)
			{
				// Import the session key.
				if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptImportKey(pbSessionKeyBlob) lastError=%8.8x createIndex=%d in %s"),
						(unsigned)lastError,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Create packet to send.
				long tosendit = 0;
				memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);
				retval = pc.CreatePacketToSend(msgToSend, pc._tempbuffer, msgToSendLength, &tosendit);
				{
					CString failedMsg;
					failedMsg.Format(_T("CreatePacketToSend createIndex=%d in %s"),
					(int)createIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify packet.

				// Verify minimum required total length.  Note that both signed and unsigned values are used.
				const DWORD extendedlengthRequired = 0;
				const long sizeofSTX = sizeof(byte);
				const long sizeofETX = sizeof(byte);
				const long minimumRequiredLength = sizeof(extendedlengthRequired) + extendedlengthRequired + msgToSendLength +
					sizeofSTX + sizeofETX;
				if (tosendit < minimumRequiredLength)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("tosend=%ld < minimumRequiredLength=%ld createIndex=%d in %s"),
						(long)tosendit,
						(long)minimumRequiredLength,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify STX.
				if (pc._tempbuffer[0] != STX)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("pc._tempbuffer[0]=%u != STX=%u createIndex=%d in %s"),
						(unsigned)pc._tempbuffer[0],
						(unsigned)STX,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify ETX.
				if (pc._tempbuffer[tosendit - 1] != ETX)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("pc._tempbuffer[tosendit - 1]=%u != ETX=%u createIndex=%d in %s"),
						(unsigned)pc._tempbuffer[tosendit - 1],
						(unsigned)ETX,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// DLE Decode message sent.
				BYTE *varMsgSent = new BYTE[MAXPACKETLENGTH];
				const long msgSentLength = pc.DLEDecode(pc._tempbuffer + sizeofSTX, varMsgSent, tosendit - sizeofSTX - sizeofETX);

				// Verify message sent length after decoding.
				const long requiredDecodeLength = sizeof(extendedlengthRequired) + extendedlengthRequired + msgToSendLength;
				if (msgSentLength != requiredDecodeLength)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("msgSentLength=%ld != requiredDecodeLength=%ld createIndex=%d in %s"),
						(long)msgSentLength,
						(long)requiredDecodeLength,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify extended length field after decoding.
				int memcmpExtendedLengthResult = memcmp(varMsgSent, &extendedlengthRequired, sizeof(extendedlengthRequired));
				if (memcmpExtendedLengthResult != MEMCMP_EQUAL)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("extendedlength field != extendedlengthRequired=%u createIndex=%d in %s"),
						(unsigned)extendedlengthRequired,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				if (extendedlengthRequired > 0)
				{
					// Get substitute for the extended field sent.
					BYTE *extendedFieldToSend = new BYTE[extendedlengthRequired];

					// Verify extended field sent after decoding.
					BYTE *extendedFieldSent = varMsgSent + sizeof(extendedlengthRequired);
					int memcmpFieldResult = memcmp(extendedFieldSent, extendedFieldToSend, sizeof(extendedlengthRequired));
					if (memcmpFieldResult != MEMCMP_EQUAL)
					{
						CString extendedFieldSentStr(CUtilities::ToHexString(extendedFieldSent, extendedlengthRequired));
						CString extendedFieldToSendStr(CUtilities::ToHexString(extendedFieldToSend, extendedlengthRequired));
						retval = false;
						CString failedMsg;
						failedMsg.Format(_T("message field sent[%s] != message field to send[%s] createIndex=%d in %s"),
							static_cast<LPCTSTR>(extendedFieldSentStr),
							static_cast<LPCTSTR>(extendedFieldToSendStr),
							(int)createIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
					delete extendedFieldToSend;
				}

				// Calculate length of the message field sent.
				const long msgFieldLength = msgSentLength - sizeof(extendedlengthRequired) - extendedlengthRequired;

				DWORD msgFieldLengthResult((DWORD)msgFieldLength);
				BYTE *msgFieldSent = varMsgSent + sizeof(extendedlengthRequired) + extendedlengthRequired;
				if (msgFieldLength > 0)
				{
					if (!CryptDecrypt (pc.hSessionKey, 0, TRUE, 0, msgFieldSent, &msgFieldLengthResult))
					{
						// Failed to decrypt message field.
						DWORD lastError = GetLastError();
						retval = false;
						CString failedMsg;
						failedMsg.Format(_T("CryptDecrypt(msgFieldSent) lastError=%8.8x createIndex=%d in %s"),
							(unsigned)lastError,
							(int)createIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
				}

				// Verify message field length after decryption.
				if (msgFieldLengthResult != msgFieldLength)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("msgFieldLengthResult=%ld != msgFieldLength=%u createIndex=%d in %s"),
						(long)msgFieldLengthResult,
						(unsigned)msgFieldLength,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify message field length vs msgToSendLength after decryption..
				if (msgFieldLength != msgToSendLength)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("msgFieldLength=%ld != msgToSendLength=%ld createIndex=%d in %s"),
						(long)msgFieldLength,
						(long)msgToSendLength,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify message field after decryption.
				int memcmpMsgFieldResult = memcmp(msgFieldSent, msgToSend, msgFieldLength);
				if (memcmpMsgFieldResult != MEMCMP_EQUAL)
				{
					CString msgFieldSentStr(CUtilities::ToHexString(msgFieldSent, msgFieldLength));
					CString msgToSendStr(CUtilities::ToHexString(msgToSend, msgToSendLength));
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("message field sent[%s] != message field to send[%s] createIndex=%d in %s"),
						static_cast<LPCTSTR>(msgFieldSentStr),
						static_cast<LPCTSTR>(msgToSendStr),
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				delete [] varMsgSent;
			}

			// Cleanup.
			delete [] msgToSend;
		}

		TEST_METHOD(CreatePacketToSend3_VerifyImportOnceTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Generate a session key, as if it was received from a Poller.
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if ( (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					Assert::IsTrue(retval, CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
				}
				ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);

				// Export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			const long msgToSendLength = MAXMESSAGELENGTH;
			BYTE *varMsgToSend = new BYTE[msgToSendLength];

			// Init msgToSend[] with a variety of values.
			for (int index = 0; index < msgToSendLength; ++index)
			{
				if (0 == (index & 1))
				{
					// Low byte of half-speed counter.
					varMsgToSend[index] = (BYTE)(index / 2);
				}
				else
				{
					// Low byte of one third-speed counter.
					varMsgToSend[index] = (BYTE)(index / 3);
				}
			}
			const BYTE *msgToSend(varMsgToSend);

			// Import the session key.
			if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
			{
				DWORD lastError = GetLastError();
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("CryptImportKey(pbSessionKeyBlob) lastError=%8.8x in %s"),
					(unsigned)lastError,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			for (int createIndex = 0; createIndex < 1000; ++createIndex)
			{
				// Create packet to send.
				long tosendit = 0;
				memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);
				retval = pc.CreatePacketToSend(msgToSend, pc._tempbuffer, msgToSendLength, &tosendit);
				{
					CString failedMsg;
					failedMsg.Format(_T("CreatePacketToSend createIndex=%d in %s"),
					(int)createIndex,
					static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify packet.

				// Verify minimum required total length.  Note that both signed and unsigned values are used.
				const DWORD extendedlengthRequired = 0;
				const long sizeofSTX = sizeof(byte);
				const long sizeofETX = sizeof(byte);
				const long minimumRequiredLength = sizeof(extendedlengthRequired) + extendedlengthRequired + msgToSendLength +
					sizeofSTX + sizeofETX;
				if (tosendit < minimumRequiredLength)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("tosend=%ld < minimumRequiredLength=%ld createIndex=%d in %s"),
						(long)tosendit,
						(long)minimumRequiredLength,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify STX.
				if (pc._tempbuffer[0] != STX)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("pc._tempbuffer[0]=%u != STX=%u createIndex=%d in %s"),
						(unsigned)pc._tempbuffer[0],
						(unsigned)STX,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify ETX.
				if (pc._tempbuffer[tosendit - 1] != ETX)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("pc._tempbuffer[tosendit - 1]=%u != ETX=%u createIndex=%d in %s"),
						(unsigned)pc._tempbuffer[tosendit - 1],
						(unsigned)ETX,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// DLE Decode message sent.
				BYTE *varMsgSent = new BYTE[MAXPACKETLENGTH];
				const long msgSentLength = pc.DLEDecode(pc._tempbuffer + sizeofSTX, varMsgSent, tosendit - sizeofSTX - sizeofETX);

				// Verify message sent length after decoding.
				const long requiredDecodeLength = sizeof(extendedlengthRequired) + extendedlengthRequired + msgToSendLength;
				if (msgSentLength != requiredDecodeLength)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("msgSentLength=%ld != requiredDecodeLength=%ld createIndex=%d in %s"),
						(long)msgSentLength,
						(long)requiredDecodeLength,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify extended length field after decoding.
				int memcmpExtendedLengthResult = memcmp(varMsgSent, &extendedlengthRequired, sizeof(extendedlengthRequired));
				if (memcmpExtendedLengthResult != MEMCMP_EQUAL)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("extendedlength field != extendedlengthRequired=%u createIndex=%d in %s"),
						(unsigned)extendedlengthRequired,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				if (extendedlengthRequired > 0)
				{
					// Get substitute for the extended field sent.
					BYTE *extendedFieldToSend = new BYTE[extendedlengthRequired];

					// Verify extended field sent after decoding.
					BYTE *extendedFieldSent = varMsgSent + sizeof(extendedlengthRequired);
					int memcmpFieldResult = memcmp(extendedFieldSent, extendedFieldToSend, sizeof(extendedlengthRequired));
					if (memcmpFieldResult != MEMCMP_EQUAL)
					{
						CString extendedFieldSentStr(CUtilities::ToHexString(extendedFieldSent, extendedlengthRequired));
						CString extendedFieldToSendStr(CUtilities::ToHexString(extendedFieldToSend, extendedlengthRequired));
						retval = false;
						CString failedMsg;
						failedMsg.Format(_T("message field sent[%s] != message field to send[%s] createIndex=%d in %s"),
							static_cast<LPCTSTR>(extendedFieldSentStr),
							static_cast<LPCTSTR>(extendedFieldToSendStr),
							(int)createIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
					delete extendedFieldToSend;
				}

				// Calculate length of the message field sent.
				const long msgFieldLength = msgSentLength - sizeof(extendedlengthRequired) - extendedlengthRequired;

				DWORD msgFieldLengthResult((DWORD)msgFieldLength);
				BYTE *msgFieldSent = varMsgSent + sizeof(extendedlengthRequired) + extendedlengthRequired;
				if (msgFieldLength > 0)
				{
					if (!CryptDecrypt (pc.hSessionKey, 0, TRUE, 0, msgFieldSent, &msgFieldLengthResult))
					{
						// Failed to decrypt message field.
						DWORD lastError = GetLastError();
						retval = false;
						CString failedMsg;
						failedMsg.Format(_T("CryptDecrypt(msgFieldSent) lastError=%8.8x createIndex=%d in %s"),
							(unsigned)lastError,
							(int)createIndex,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
				}

				// Verify message field length after decryption.
				if (msgFieldLengthResult != msgFieldLength)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("msgFieldLengthResult=%ld != msgFieldLength=%u createIndex=%d in %s"),
						(long)msgFieldLengthResult,
						(unsigned)msgFieldLength,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify message field length vs msgToSendLength after decryption..
				if (msgFieldLength != msgToSendLength)
				{
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("msgFieldLength=%ld != msgToSendLength=%ld createIndex=%d in %s"),
						(long)msgFieldLength,
						(long)msgToSendLength,
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify message field after decryption.
				int memcmpMsgFieldResult = memcmp(msgFieldSent, msgToSend, msgFieldLength);
				if (memcmpMsgFieldResult != MEMCMP_EQUAL)
				{
					CString msgFieldSentStr(CUtilities::ToHexString(msgFieldSent, msgFieldLength));
					CString msgToSendStr(CUtilities::ToHexString(msgToSend, msgToSendLength));
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("message field sent[%s] != message field to send[%s] createIndex=%d in %s"),
						static_cast<LPCTSTR>(msgFieldSentStr),
						static_cast<LPCTSTR>(msgToSendStr),
						(int)createIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				delete [] varMsgSent;
			}

			// Cleanup.
			delete [] msgToSend;
		}

#if 000
		// Assert functions don't work the same for worker threads.
		static UINT AFX_CDECL ReadFromPollerThread( PVOID threadParam )
		{
			bool retval(true);
			PollerCommunicator &pc(*(PollerCommunicator *)threadParam);
			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Initialize COM.
			bool comInitialized = false;
			HRESULT hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
			if (!FAILED(hResult))
			{
				comInitialized = true;
			}
			else
			{
				retval = false;
				CString failedMsg;
				failedMsg.Format(_T("CoInitializeEx failed in %s -- hResult=0x%8.8x"), static_cast<LPCTSTR>(__TFUNCTION__), (unsigned)hResult);
				if (!pc.m_readThreadError.m_errorOccurred)
				{
					// Do NOT change m_errorStr after setting flag true.
					pc.m_readThreadError.m_errorStr = failedMsg;
					pc.m_readThreadError.m_errorOccurred = true;
				}
			}

			// Generate a session key, as if it was received from a Poller.
			if (retval)
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					if (!pc.m_readThreadError.m_errorOccurred)
					{
						// Do NOT change m_errorStr after setting flag true.
						pc.m_readThreadError.m_errorStr = failedMsg;
						pc.m_readThreadError.m_errorOccurred = true;
					}
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (retval && !CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					if (!pc.m_readThreadError.m_errorOccurred)
					{
						// Do NOT change m_errorStr after setting flag true.
						pc.m_readThreadError.m_errorStr = failedMsg;
						pc.m_readThreadError.m_errorOccurred = true;
					}
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if (retval && (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					if (!pc.m_readThreadError.m_errorOccurred)
					{
						// Do NOT change m_errorStr after setting flag true.
						pc.m_readThreadError.m_errorStr = CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__);
						pc.m_readThreadError.m_errorOccurred = true;
					}
				}
				else
				{
					ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);
				}

				// Export the session key into a simple key BLOB.
				if (retval && !CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					if (!pc.m_readThreadError.m_errorOccurred)
					{
						// Do NOT change m_errorStr after setting flag true.
						pc.m_readThreadError.m_errorStr = failedMsg;
						pc.m_readThreadError.m_errorOccurred = true;
					}
				}
			}

			if (retval)
			{
				for (; !shutdown; ++importCount)
				{
					Sleep(0);
#if 1
#ifdef USE_SESSION_KEY_CRITICAL_SECTION
					pc.m_sessionKeyCriticalSection.Lock();
#endif // USE_SESSION_KEY_CRITICAL_SECTION.
					{
						// Import the session key.
						if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
						{
							DWORD lastError = GetLastError();
							retval = false;
							CString failedMsg;
							failedMsg.Format(_T("CryptImportKey(pbSessionKeyBlob) lastError=%8.8x createCount=%d importCount=%u in %s"),
								(unsigned)lastError,
								(unsigned)createCount,
								(unsigned)importCount,
								static_cast<LPCTSTR>(__TFUNCTION__));
							if (!pc.m_readThreadError.m_errorOccurred)
							{
								// Do NOT change m_errorStr after setting flag true.
								pc.m_readThreadError.m_errorStr = failedMsg;
								pc.m_readThreadError.m_errorOccurred = true;
							}
						}
					}
#ifdef USE_SESSION_KEY_CRITICAL_SECTION
					pc.m_sessionKeyCriticalSection.Unlock();
#endif // USE_SESSION_KEY_CRITICAL_SECTION.
#endif

				}
			}

			delete [] pbSessionKeyBlob;
			return (DWORD)retval;

			if (comInitialized)
			{
				CoUninitialize();
			}
		}

		TEST_METHOD(CreatePacketToSend4_DuplicateTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Generate a session key, as if it was received from a Poller.
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if ( (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					Assert::IsTrue(retval, CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
				}
				ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);

				// Export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			const long msgToSendLength = MAXMESSAGELENGTH;
			BYTE *varMsgToSend = new BYTE[msgToSendLength];

			// Init msgToSend[] with a variety of values.
			for (int index = 0; index < msgToSendLength; ++index)
			{
				if (0 == (index & 1))
				{
					// Low byte of half-speed counter.
					varMsgToSend[index] = (BYTE)(index / 2);
				}
				else
				{
					// Low byte of one third-speed counter.
					varMsgToSend[index] = (BYTE)(index / 3);
				}
			}
			const BYTE *msgToSend(varMsgToSend);

			long firstMsgToSendLength(0);
			BYTE *firstMsgToSend(NULL);

			shutdown = false;
			createCount = 0;
			importCount = 0;
#ifdef USE_READ_THREAD
			// Start the Read thread.
			CWinThread *readThread = AfxBeginThread( ReadFromPollerThread, (LPVOID)&pc, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			readThread->m_bAutoDelete = false; // We will delete the thread explicitly.
			readThread->ResumeThread();
#endif // USE_READ_THREAD.
			const DWORD startTestTickCount(GetTickCount());

			for (;
				createCount < ENDURANCE_TEST_LOOP_COUNT ||
					importCount < ENDURANCE_TEST_LOOP_COUNT ||
					GetTickCount() - startTestTickCount < ENDURANCE_TEST_MINIMUM_PERIOD;
				++createCount)
			{
				Sleep(0);

				// Setup before creating packet to send.
				long tosendit = 0;
				memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);

#ifdef USE_SESSION_KEY_CRITICAL_SECTION
				pc.m_sessionKeyCriticalSection.Lock();
#endif // USE_SESSION_KEY_CRITICAL_SECTION.
				{
					// Import the session key.
					if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey))
					{
						DWORD lastError = GetLastError();
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sCryptImportKey(pbSessionKeyBlob) lastError=%8.8x createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							(unsigned)lastError,
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}

					// Create packet to send.
					retval = pc.CreatePacketToSend(msgToSend, pc._tempbuffer, msgToSendLength, &tosendit);
					{
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sCreatePacketToSend createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
				}
#ifdef USE_SESSION_KEY_CRITICAL_SECTION
				pc.m_sessionKeyCriticalSection.Unlock();
#endif // USE_SESSION_KEY_CRITICAL_SECTION.

				if (0 == createCount)
				{
					// Save the result from the first CreatePacketToSend() call.
					firstMsgToSendLength = tosendit;
					firstMsgToSend = new BYTE[firstMsgToSendLength];
					memcpy_s(firstMsgToSend, firstMsgToSendLength, pc._tempbuffer, tosendit);
				}
				else
				{
					// Compare the result from the first CreatePacketToSend() call to the current call.
					if (firstMsgToSendLength != tosendit)
					{
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sfirstMsgToSendLength=%ld != tosendit=%ld createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							(long)firstMsgToSendLength,
							(long)tosendit,
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}

					const int memcmpResult = memcmp(firstMsgToSend, pc._tempbuffer, firstMsgToSendLength);
					if (memcmpResult != MEMCMP_EQUAL)
					{
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%smemcmp(firstMsgToSend, pc._tempbuffer) createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
				}

				const DWORD currentTickCount(GetTickCount());
				const DWORD elapsedTickCount(currentTickCount - startTestTickCount);
				if (elapsedTickCount >= ENDURANCE_TEST_MAXIMUM_PERIOD)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%sTEST TIMEOUT %u >= ENDURANCE_TEST_MAXIMUM_PERIOD createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)elapsedTickCount,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
					break;
				}

				if (pc.m_readThreadError.m_errorOccurred)
				{
					retval = false;
					// Report async read thread error either with our next error, or before we exit.
					break;
				}
			}

#ifdef USE_READ_THREAD
			// Wait for worker thread(s) to complete before resources go out of scope.
			shutdown = true;
			const DWORD startShutdownTickCount(GetTickCount());
			bool shutdownComplete(false);
			while (!shutdownComplete)
			{
				DWORD exitCode(0);
				const BOOL ok = GetExitCodeThread(readThread->m_hThread, &exitCode);
				if (!ok)
				{
					const DWORD lastError = GetLastError();
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%sGetExitCodeProcess(readThread) lastError=%8.8x createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)lastError,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
				else if (exitCode != STILL_ACTIVE)
				{
					shutdownComplete = true;
					delete readThread; // Don't delete until the thread has exited.
				}
				else
				{
					const DWORD currentTickCount(GetTickCount());
					const DWORD elapsedTickCount(currentTickCount - startShutdownTickCount);
					if (elapsedTickCount >= MAXIMUM_SHUTDOWN_TICK_COUNT)
					{
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sSHUTDOWN TIMEOUT %u >= MAXIMUM_SHUTDOWN_TICK_COUNT createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							(unsigned)elapsedTickCount,
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
						break;
					}

					Sleep(10);
				}

			}
#endif // USE_READ_THREAD.

			if (pc.m_readThreadError.m_errorOccurred)
			{
				retval = false;
				CString asyncMsg(_T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- "));
				CString failedMsg;
				failedMsg.Format(_T("%screateCount=%u importCount=%u in %s"),
					static_cast<LPCTSTR>(asyncMsg),
					(unsigned)createCount,
					(unsigned)importCount,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Cleanup.
			delete [] msgToSend;
			delete [] firstMsgToSend;
#undef USE_READ_THREAD
		}

		TEST_METHOD(CreatePacketToSend4_VerifyTest)
		{
			bool retval(true);
			PollerCommunicator pc;
			retval = pc.SetUpEncryption();
			Assert::IsTrue(retval, CString(_T("SetUpEncryption in ")) + static_cast<LPCTSTR>(__TFUNCTION__));

			DWORD dwSessionKeyBlobLength = 0;
			PBYTE pbSessionKeyBlob = NULL;

			// Generate a session key, as if it was received from a Poller.
			{
				ALG_ID Algid = CALG_RC4;
				DWORD dwFlags = CRYPT_EXPORTABLE;
				HCRYPTKEY hKey(NULL);

				// Generate a session key.
				if (!CryptGenKey(pc.hProv, Algid, dwFlags, &hKey))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptGenKey(CALG_RC4) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Get the data length required to export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, NULL, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, NULL) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Allocate the data required to export the session key into a simple key BLOB.
				if ( (pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength) ) == NULL)
				{
					retval = false;
					Assert::IsTrue(retval, CString(_T("malloc(dwSessionKeyBlobLength) in ")) + static_cast<LPCTSTR>(__TFUNCTION__));
				}
				ZeroMemory(pbSessionKeyBlob, dwSessionKeyBlobLength);

				// Export the session key into a simple key BLOB.
				if (!CryptExportKey(hKey, pc.hExchangePubKey, SIMPLEBLOB, 0, pbSessionKeyBlob, &dwSessionKeyBlobLength))
				{
					DWORD lastError = GetLastError();
					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("CryptExportKey(hExchangePubKey, pbSessionKeyBlob) lastError=%8.8x in %s"),
						(unsigned)lastError,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
			}

			const long msgToSendLength = MAXMESSAGELENGTH;
			BYTE *varMsgToSend = new BYTE[msgToSendLength];

			// Init msgToSend[] with a variety of values.
			for (int index = 0; index < msgToSendLength; ++index)
			{
				if (0 == (index & 1))
				{
					// Low byte of half-speed counter.
					varMsgToSend[index] = (BYTE)(index / 2);
				}
				else
				{
					// Low byte of one third-speed counter.
					varMsgToSend[index] = (BYTE)(index / 3);
				}
			}
			const BYTE *msgToSend(varMsgToSend);

			long firstMsgToSendLength(0);
			BYTE *firstMsgToSend(NULL);

			shutdown = false;
			createCount = 0; // Init test loop counter before starting thread(s).
			importCount = 0; // Init test loop counter before starting thread(s).
#define USE_READ_THREAD
#ifdef USE_READ_THREAD
			// Start the Read thread.
			CWinThread *readThread = AfxBeginThread( ReadFromPollerThread, (LPVOID)&pc, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			readThread->m_bAutoDelete = false; // We will delete the thread explicitly.
			readThread->ResumeThread();
#endif // USE_READ_THREAD.
			const DWORD startTestTickCount(GetTickCount());

			for (;
				createCount < ENDURANCE_TEST_LOOP_COUNT ||
					importCount < ENDURANCE_TEST_LOOP_COUNT ||
					GetTickCount() - startTestTickCount < ENDURANCE_TEST_MINIMUM_PERIOD;
				++createCount)
			{
				Sleep(0);

				// Setup before creating packet to send.
				long tosendit = 0;
				memset(pc._tempbuffer, MEMSET_DEFAULT_VALUE, MAXPACKETLENGTH);

#ifdef USE_SESSION_KEY_CRITICAL_SECTION
				pc.m_sessionKeyCriticalSection.Lock();
#endif // USE_SESSION_KEY_CRITICAL_SECTION.
				{
					// Import the session key.
					if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionKey)) 
					{
						DWORD lastError = GetLastError();
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sCryptImportKey(hSessionKey) lastError=%8.8x createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							(unsigned)lastError,
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}

					// Since the session key changed, create a duplicate session key to use for verification.
					// This allows connections to proceed normally after a packet encryption is completed.
					// This allows verification to be correct even if session key is changed during verification.
					if (!CryptImportKey (pc.hProv, pbSessionKeyBlob, dwSessionKeyBlobLength, 0, 0, &pc.hSessionVerifyKey)) 
					{
						DWORD lastError = GetLastError();
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sCryptImportKey(hSessionVerifyKey) lastError=%8.8x createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							(unsigned)lastError,
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}

					// Create packet to send.
					retval = pc.CreatePacketToSend(msgToSend, pc._tempbuffer, msgToSendLength, &tosendit);
					{
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sCreatePacketToSend createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
				}
#ifdef USE_SESSION_KEY_CRITICAL_SECTION
				pc.m_sessionKeyCriticalSection.Unlock();
#endif // USE_SESSION_KEY_CRITICAL_SECTION.

				// Verify packet.

				// Verify minimum required total length.  Note that both signed and unsigned values are used.
				const DWORD extendedlengthRequired = 0;
				const long sizeofSTX = sizeof(byte);
				const long sizeofETX = sizeof(byte);
				const long minimumRequiredLength = sizeof(extendedlengthRequired) + extendedlengthRequired + msgToSendLength +
					sizeofSTX + sizeofETX;
				if (tosendit < minimumRequiredLength)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%stosend=%ld < minimumRequiredLength=%ld createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(long)tosendit,
						(long)minimumRequiredLength,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify STX.
				if (pc._tempbuffer[0] != STX)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%spc._tempbuffer[0]=%u != STX=%u createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)pc._tempbuffer[0],
						(unsigned)STX,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify ETX.
				if (pc._tempbuffer[tosendit - 1] != ETX)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%spc._tempbuffer[tosendit - 1]=%u != ETX=%u createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)pc._tempbuffer[tosendit - 1],
						(unsigned)ETX,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// DLE Decode message sent.
				BYTE *varMsgSent = new BYTE[MAXPACKETLENGTH];
				const long msgSentLength = pc.DLEDecode(pc._tempbuffer + sizeofSTX, varMsgSent, tosendit - sizeofSTX - sizeofETX);

				// Verify message sent length after decoding.
				const long requiredDecodeLength = sizeof(extendedlengthRequired) + extendedlengthRequired + msgToSendLength;
				if (msgSentLength != requiredDecodeLength)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%smsgSentLength=%ld != requiredDecodeLength=%ld createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(long)msgSentLength,
						(long)requiredDecodeLength,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify extended length field after decoding.
				int memcmpExtendedLengthResult = memcmp(varMsgSent, &extendedlengthRequired, sizeof(extendedlengthRequired));
				if (memcmpExtendedLengthResult != MEMCMP_EQUAL)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%sextendedlength field != extendedlengthRequired=%u createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)extendedlengthRequired,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				if (extendedlengthRequired > 0)
				{
					// Get substitute for the extended field sent.
					BYTE *extendedFieldToSend = new BYTE[extendedlengthRequired];

					// Verify extended field sent after decoding.
					BYTE *extendedFieldSent = varMsgSent + sizeof(extendedlengthRequired);
					int memcmpFieldResult = memcmp(extendedFieldSent, extendedFieldToSend, sizeof(extendedlengthRequired));
					if (memcmpFieldResult != MEMCMP_EQUAL)
					{
						CString extendedFieldSentStr(CUtilities::ToHexString(extendedFieldSent, extendedlengthRequired));
						CString extendedFieldToSendStr(CUtilities::ToHexString(extendedFieldToSend, extendedlengthRequired));
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%smessage field sent[%s] != message field to send[%s] createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							static_cast<LPCTSTR>(extendedFieldSentStr),
							static_cast<LPCTSTR>(extendedFieldToSendStr),
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
					delete extendedFieldToSend;
				}

				// Calculate length of the message field sent.
				const long msgFieldLength = msgSentLength - sizeof(extendedlengthRequired) - extendedlengthRequired;

				DWORD msgFieldLengthResult((DWORD)msgFieldLength);
				BYTE *msgFieldSent = varMsgSent + sizeof(extendedlengthRequired) + extendedlengthRequired;
				if (msgFieldLength > 0)
				{
					if (!CryptDecrypt (pc.hSessionVerifyKey, 0, TRUE, 0, msgFieldSent, &msgFieldLengthResult))
					{
						// Failed to decrypt message field.
						DWORD lastError = GetLastError();
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sCryptDecrypt(msgFieldSent) lastError=%8.8x createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							(unsigned)lastError,
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
					}
				}

				// Verify message field length after decryption.
				if (msgFieldLengthResult != msgFieldLength)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%smsgFieldLengthResult=%ld != msgFieldLength=%u createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(long)msgFieldLengthResult,
						(unsigned)msgFieldLength,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify message field length vs msgToSendLength after decryption..
				if (msgFieldLength != msgToSendLength)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%smsgFieldLength=%ld != msgToSendLength=%ld createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(long)msgFieldLength,
						(long)msgToSendLength,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				// Verify message field after decryption.
				int memcmpMsgFieldResult = memcmp(msgFieldSent, msgToSend, msgFieldLength);
				if (memcmpMsgFieldResult != MEMCMP_EQUAL)
				{
					CString msgFieldSentStr(CUtilities::ToHexString(msgFieldSent, msgFieldLength));
					CString msgToSendStr(CUtilities::ToHexString(msgToSend, msgToSendLength));
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%smessage field sent[%s] != message field to send[%s] createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						static_cast<LPCTSTR>(msgFieldSentStr),
						static_cast<LPCTSTR>(msgToSendStr),
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}

				delete [] varMsgSent;

				const DWORD currentTickCount(GetTickCount());
				const DWORD elapsedTickCount(currentTickCount - startTestTickCount);
				if (elapsedTickCount >= ENDURANCE_TEST_MAXIMUM_PERIOD)
				{
					retval = false;
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					CString failedMsg;
					failedMsg.Format(_T("%sTEST TIMEOUT %u >= ENDURANCE_TEST_MAXIMUM_PERIOD createCount=%d importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)elapsedTickCount,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
					break;
				}
			}

#ifdef USE_READ_THREAD
			// Wait for worker thread(s) to complete before resources go out of scope.
			shutdown = true;
			const DWORD startShutdownTickCount(GetTickCount());
			bool shutdownComplete(false);
			while (!shutdownComplete)
			{
				DWORD exitCode(0);
				const BOOL ok = GetExitCodeThread(readThread->m_hThread, &exitCode);
				if (!ok)
				{
					const DWORD lastError = GetLastError();
					CString asyncMsg;
					if (pc.m_readThreadError.m_errorOccurred)
					{
						asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
					}

					retval = false;
					CString failedMsg;
					failedMsg.Format(_T("%sGetExitCodeProcess(readThread) lastError=%8.8x createCount=%u importCount=%u in %s"),
						static_cast<LPCTSTR>(asyncMsg),
						(unsigned)lastError,
						(unsigned)createCount,
						(unsigned)importCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					Assert::IsTrue(retval, failedMsg);
				}
				else if (exitCode != STILL_ACTIVE)
				{
					shutdownComplete = true;
					delete readThread; // Don't delete until the thread has exited.
				}
				else
				{
					const DWORD currentTickCount(GetTickCount());
					const DWORD elapsedTickCount(currentTickCount - startShutdownTickCount);
					if (elapsedTickCount >= MAXIMUM_SHUTDOWN_TICK_COUNT)
					{
						retval = false;
						CString asyncMsg;
						if (pc.m_readThreadError.m_errorOccurred)
						{
							asyncMsg = _T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- ");
						}

						CString failedMsg;
						failedMsg.Format(_T("%sSHUTDOWN TIMEOUT %u >= MAXIMUM_SHUTDOWN_TICK_COUNT createCount=%u importCount=%u in %s"),
							static_cast<LPCTSTR>(asyncMsg),
							(unsigned)elapsedTickCount,
							(unsigned)createCount,
							(unsigned)importCount,
							static_cast<LPCTSTR>(__TFUNCTION__));
						Assert::IsTrue(retval, failedMsg);
						break;
					}

					Sleep(10);
				}

			}
#endif // USE_READ_THREAD.

			if (pc.m_readThreadError.m_errorOccurred)
			{
				retval = false;
				CString asyncMsg(_T("ReadThread: ") + pc.m_readThreadError.m_errorStr + _T(" -- "));
				CString failedMsg;
				failedMsg.Format(_T("%screateCount=%u importCount=%u in %s"),
					static_cast<LPCTSTR>(asyncMsg),
					(unsigned)createCount,
					(unsigned)importCount,
					static_cast<LPCTSTR>(__TFUNCTION__));
				Assert::IsTrue(retval, failedMsg);
			}

			// Cleanup.
			delete [] msgToSend;
			delete [] firstMsgToSend;
#undef USE_READ_THREAD
		}
#endif // 000
	};
}
#endif //TODO-PORT