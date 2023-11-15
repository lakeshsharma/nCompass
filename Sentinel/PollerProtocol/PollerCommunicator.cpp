#include "stdafx.h"
#include "PollerCommunicator.h"

#include "Diagnostics.h"
#include "Logging/LogString.h"
#include "PollerCheckMessage.h"
#include "Utilities.h"
#include "PollerMessage.h"

using namespace std;

PollerCommunicator::PollerCommunicator()
	: _socket(INVALID_SOCKET), _connected(false),
	_cancelall(false), _stxindex(0), _etxindex(0), _buffer(NULL),
	m_pollerPacketSentVerifyErrors(0)
{
	pbExchangePubKeyBlob = NULL;
	hExchangePubKey = NULL;
	hSessionKey = NULL;
	hSessionVerifyKey = NULL;
	_buffer = new BYTE[MAXPACKETLENGTH];
	_tempbuffer = new BYTE[MAXPACKETLENGTH];
	_tempbufferforsend = new BYTE[MAXPACKETLENGTH];
	_extendedpacket = new BYTE[MAXPACKETLENGTH];
	_tempbufferforrecv = new BYTE[MAXPACKETLENGTH];
	_tempbufferforCreate = new BYTE[MAXPACKETLENGTH];
	ZeroMemory(_buffer, MAXPACKETLENGTH);

	_bufferlength = 0;
	FD_ZERO(&write_fds);
	FD_ZERO(&read_fds);
	FD_ZERO(&writeerror_fds);
	FD_ZERO(&readerror_fds);
	FD_ZERO(&listen_fds);

	//ERR_load_crypto_strings();

	pbExchangePubKeyBlob = NULL;
	dwExchangePubKeyBlobLength = 0;
	bEncryptionSetup = false;
	bpublickeywassent = false;
	bsessionkeywassent = false;

	_reading = false;
	_sending = false;

	_port = 0;

	SetUpEncryption();
}

//This destructor will close the socket, the handle to the cancel event and Clean up the WSA.
PollerCommunicator::~PollerCommunicator()
{
	try
	{

		delete[] _buffer;
		delete[] _tempbuffer;
		delete[] _tempbufferforsend;
		delete[] _extendedpacket;
		delete[] _tempbufferforrecv;
		delete[] _tempbufferforCreate;

		Close();
		TearDownEncryption();
	}
	catch (...)
	{/*Here just so a destructor does not throw an exception*/
	}
}

// <OpenSSL Utility Functions>
// These are copied verbatim from OpenSSL 1.1.1h's "crypto/pem/pvkfmt.c" file:
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/crypto/pem/pvkfmt.c#L23-L38

# define MS_PUBLICKEYBLOB        0x6
# define MS_RSA1MAGIC            0x31415352L
# define MS_KEYALG_RSA_KEYX      0xa400

static unsigned int read_ledword(const unsigned char** in) {
	const unsigned char* p = *in;
	unsigned int ret;
	ret = (unsigned int)*p++;
	ret |= (unsigned int)*p++ << 8;
	ret |= (unsigned int)*p++ << 16;
	ret |= (unsigned int)*p++ << 24;
	*in = p;
	return ret;
}

static void write_ledword(unsigned char** out, unsigned int dw) {
	unsigned char* p = *out;
	*p++ = dw & 0xff;
	*p++ = (dw >> 8) & 0xff;
	*p++ = (dw >> 16) & 0xff;
	*p++ = (dw >> 24) & 0xff;
	*out = p;
}

// When building on Yocto, we have OpenSSL 1.0.0, which is missing a few bits we require that were added in 1.1.0+.
// (https://www.openssl.org/docs/man1.1.1/man3/OPENSSL_VERSION_NUMBER.html)
#if OPENSSL_VERSION_NUMBER < 0x10100000L
// https://www.openssl.org/docs/man1.1.1/man3/RSA_bits.html#HISTORY
// "The RSA_bits() function was added in OpenSSL 1.1.0."
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/crypto/rsa/rsa_crpt.c#L17-L20
int RSA_bits(const RSA* r) {
	return BN_num_bits(r->n);
}
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/crypto/bn/bn_lib.c#L589-L594
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/crypto/bn/bn_lib.c#L479-L528
int BN_bn2lebinpad(const BIGNUM* a, unsigned char* to, int tolen) {
	if (tolen < 0)
		return -1;
	// return bn2binpad(a, to, tolen, little);
	int n;
	size_t i, lasti, j, atop, mask;
	BN_ULONG l;
	/*
	 * In case |a| is fixed-top, BN_num_bytes can return bogus length,
	 * but it's assumed that fixed-top inputs ought to be "nominated"
	 * even for padded output, so it works out...
	 */
	n = BN_num_bytes(a);
	if (tolen == -1) {
		tolen = n;
	}
	else if (tolen < n) {     /* uncommon/unlike case */
		BIGNUM temp = *a;
		bn_correct_top(&temp);
		n = BN_num_bytes(&temp);
		if (tolen < n)
			return -1;
	}
	/* Swipe through whole available data and don't give away padded zero. */
	atop = a->dmax * BN_BYTES;
	if (atop == 0) {
		OPENSSL_cleanse(to, tolen);
		return tolen;
	}
	lasti = atop - 1;
	atop = a->top * BN_BYTES;
	for (i = 0, j = 0; j < (size_t)tolen; j++) {
		unsigned char val;
		l = a->d[i / BN_BYTES];
		mask = 0 - ((j - atop) >> (8 * sizeof(i) - 1));
		val = (unsigned char)(l >> (8 * (i % BN_BYTES)) & mask);
		*to++ = val;
		i += (i - lasti) >> (8 * sizeof(i) - 1); /* stay on last limb */
	}
	return tolen;
}
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/crypto/evp/p_lib.c#L466-L473
RSA* EVP_PKEY_get0_RSA(EVP_PKEY* pkey) {
	return pkey->pkey.rsa;
}
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/crypto/rsa/rsa_lib.c#L327-L336
void RSA_get0_key(const RSA* r, const BIGNUM** n, const BIGNUM** e, const BIGNUM** d) {
	if (n != NULL)
		*n = r->n;
	if (e != NULL)
		*e = r->e;
	if (d != NULL)
		*d = r->d;
}
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/include/openssl/pemerr.h#L36
# define PEM_F_DO_I2B                                     146
#endif

static void write_lebn(unsigned char** out, const BIGNUM* bn, int len) {
	BN_bn2lebinpad(bn, *out, len);
	*out += len;
}

// </OpenSSL Utilities Functions>>

// This is a "clone" of OpenSSL's "do_blob_header" function, but only supporting the Microsoft "SIMPLEBLOB" type:
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-mqqb/c2ccaf27-dd27-49b2-8e86-2f0fcb169487
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/crypto/pem/pvkfmt.c#L86-L147
static int do_blob_header_simple(const unsigned char** in, unsigned int length,
	unsigned int* pmagic, unsigned int* pbitlen,
	int* pisdss, int* pispub)
{
	if (length < 16)
		return 0;

	if (**in != 0x1) // "MS_SIMPLEBLOB"
		return 0;

	const unsigned char* p = *in;
	p++;

	/* Version */
	if (*p++ != 0x2) {
		PEMerr(PEM_F_DO_BLOB_HEADER, PEM_R_BAD_VERSION_NUMBER);
		return 0;
	}

	p += 2;
	*pmagic = read_ledword(&p); // session key algo
	p += 4;

	*in = p;
	return 1;
}

// This is a clone of OpenSSL's "do_i2b", but only the bits we need for converting an RSA EVP_PKEY into a Microsoft PUBLICKEYBLOB.
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-mqqb/ade9efde-3ec8-4e47-9ae9-34b64d8081bb
// https://github.com/openssl/openssl/blob/OpenSSL_1_1_1h/crypto/pem/pvkfmt.c#L427-L474
static int rsaPubToMSPub(unsigned char** out, EVP_PKEY* pk) {
	if (out == NULL)
		return -1;
	if (*out != NULL)
		return -1;

	int pktype = EVP_PKEY_id(pk);
	if (pktype != EVP_PKEY_RSA)
		return -1;

	RSA* rsa = EVP_PKEY_get0_RSA(pk);

	unsigned int bitlen = RSA_bits(rsa);
	if (bitlen == 0)
		return -1;

	unsigned int keyalg = MS_KEYALG_RSA_KEYX;
	unsigned int magic = MS_RSA1MAGIC;

	int outlen = 16 + 4 + ((bitlen + 7) >> 3);

	unsigned char* p = (unsigned char*)OPENSSL_malloc(outlen);
	if (p == NULL) {
		PEMerr(PEM_F_DO_I2B, ERR_R_MALLOC_FAILURE);
		return -1;
	}
	*out = p;

	*p++ = MS_PUBLICKEYBLOB;
	*p++ = 0x2;
	*p++ = 0;
	*p++ = 0;

	write_ledword(&p, keyalg);
	write_ledword(&p, magic);
	write_ledword(&p, bitlen);

	// write_rsa(&p, rsa, 1)
	int nbyte = RSA_size(rsa);
	const BIGNUM* n, * d, * e;
	RSA_get0_key(rsa, &n, &e, &d);
	write_lebn(&p, e, 4);
	write_lebn(&p, n, nbyte);

	return outlen;
}

// Extracts and decrypts a session key out of a Microsoft SIMPLEBLOB, 
// encrypted by the other side with our exchange key.
// warning: alters the BLOB passed in the "in" argument
static RC4_KEY* blobToRC4(const unsigned char* in, unsigned int length, RSA* key)
{
	const unsigned char* p = in;
	unsigned int bitlen, magic;
	RC4_KEY* result = NULL;
	int isdss;
	int ispub;
	if (do_blob_header_simple(&p, length, &magic, &bitlen, &isdss, &ispub) <= 0)
	{
		PEMerr(PEM_F_DO_B2I, PEM_R_KEYBLOB_HEADER_PARSE_ERROR);
		return NULL;
	}

	// make sure what we got back is RC4 like we expect/assume it to be
	if (magic != 0x00006801) {
		PEMerr(PEM_F_DO_B2I, PEM_R_KEYBLOB_HEADER_PARSE_ERROR);
		return NULL;
	}

	int keylen = length - (p - in);
	BYTE* rc4Key = new BYTE[keylen];

	result = new RC4_KEY();

	// Reverse encrypted bytes of session key.
	// Microsoft's crypt library exports keys in little endian format whereas
	//   OpenSSL generally uses big endian.
	unsigned char* ps = (unsigned char*)p;
	unsigned char* pe = (unsigned char*)(p + keylen - 1);

	while (ps < pe) {
		unsigned char c = *ps;
		*ps = *pe;
		*pe = c;
		ps++;
		pe--;
	}

	int keyleno = RSA_private_decrypt(keylen, p, rc4Key, key, RSA_PKCS1_PADDING);

	if (keyleno <= 0)
	{
		delete result;
		result = nullptr;
	}
	else
	{
		RC4_set_key(result, keyleno, rc4Key);
	}

	delete[] rc4Key;
	return result;
}

//Called to connect to a remote computer
bool PollerCommunicator::Connect(std::string HostName, int Port, TiltCode& tiltCode)
{
	lock_guard<recursive_mutex> lock(_connectioncritsec);

	Close();

	bpublickeywassent = false;

	_cancelall = false;
	_port = Port;
	rem.sin_port = (uint16_t)htons((uint16_t)Port);
	rem.sin_family = AF_INET;
	rem.sin_addr.s_addr = inet_addr(HostName.c_str());
	if (rem.sin_addr.s_addr == INADDR_NONE)
	{
		hostent* lpHostEnt;
		if ((lpHostEnt = gethostbyname(HostName.c_str())) != NULL)
		{
			memcpy(&rem.sin_addr, lpHostEnt->h_addr_list[0], lpHostEnt->h_length);
		}
	}

	if (false == _connected)
	{
		_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (INVALID_SOCKET == _socket)
		{
			LogString(ERROR_LOG, "PollerCommunicator::Connect socket FAILED lastError=%d", (int)errno);
			_connected = false;
			Close();
		}

		fcntl(_socket, F_SETFL, O_NONBLOCK);

		int error = connect(_socket, (struct sockaddr*)&rem, sizeof(rem));
		if (error != 0 && errno != SOCKET_STATUS_CONNECT_IN_PROGRESS)
		{
			LogString(ERROR_LOG, "PollerCommunicator::Connect CONNECT FAILED lastError=%d", (int)errno);
			_connected = false;
			Close();
		}
		else
		{
			fd_set fdset;
			FD_ZERO(&fdset);
			FD_SET(_socket, &fdset);

			timeval tv;
			tv.tv_sec = CONNECT_TIMEOUT_SECONDS;
			tv.tv_usec = 0;

			if (select(_socket + 1, nullptr, &fdset, nullptr, &tv) == 1)
			{
				int so_error;
				socklen_t len = sizeof so_error;

				getsockopt(_socket, SOL_SOCKET, SO_ERROR, &so_error, &len);

				if (so_error == 0)
				{
					// Turn off send data buffering
					int tcpnodelay = true;
					error = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&tcpnodelay, sizeof(int));
					if (0 != error)
					{
						LogString(ERROR_LOG, "PollerCommunicator::Connect setsockopt FAILED lastError=%d", (int)errno);
					}
					FD_SET(_socket, &write_fds);
					FD_SET(_socket, &read_fds);
					FD_SET(_socket, &writeerror_fds);
					FD_SET(_socket, &readerror_fds);

					ZeroMemory(_tempbufferforsend, MAXPACKETLENGTH);

					LogString("GENERAL", "PollerCommunicator::Connect handshake sent\n");
					long messageLength = 0;
					long bytesSent = 0;
					long tosendit = 0;

					bool createOk = CreatePacketToSend(_tempbufferforsend, _tempbuffer, messageLength, &tosendit, false, nullptr, tiltCode, 0, MAXPACKETLENGTH);
					if (tosendit > MAXPACKETLENGTH)
					{
						tiltCode = tltPollerProtocolCreatePacket;
						std::string reason = FormatString("PollerCommunicator::Connect packet buffer overrun tosendit=%ld > MAXPACKETLENGTH=%ld",
							(long)tosendit,
							(long)MAXPACKETLENGTH);
						CDiagnostics::ReportBadPollerProtocolSendPacket(tiltCode, reason, nullptr, (WORD)messageLength);
					}

					if (createOk)
					{
						if (tosendit > 0)
						{
							Send((char*)_tempbuffer, tosendit, bytesSent);
						}

						if (!VerifyPacketSent(_tempbuffer, tosendit, _tempbufferforsend, messageLength))
						{
							LPCTSTR reason(_T("Packet sent verification"));
							tiltCode = tltPollerProtocolPacketSentVerification;
							CDiagnostics::ReportBadPollerProtocolSendPacket(tiltCode, reason, nullptr, (WORD)messageLength);
						}
					}

					long tempbufferforrecvlength;
					if (SOCKET_READ_SUCCESS == Read((char*)_tempbufferforrecv, MAXPACKETLENGTH, tempbufferforrecvlength))
					{
						long thelength = 0;
						AddPacket(_tempbufferforrecv, tempbufferforrecvlength, _tempbuffer, MAXPACKETLENGTH, &thelength);

						if (bsessionkeywassent == false)
						{
							LogString(ERROR_LOG, "PollerCommunicator::Connect handshake not recvd\n");
							_connected = false;
							Close();
						}
						else
						{
							LogString("GENERAL", "PollerCommunicator::Connect handshake recvd\n");
							_connected = true;
							_cancelall = false;
						}
					}
					else
					{
						_connected = false;
						Close();
					}
				}
				else
				{
					LogString(ERROR_LOG, "PollerCommunicator::Connect socket not open lastError=%d", (int)errno);
					_connected = false;
					Close();
				}
			}
			else
			{
				LogString(ERROR_LOG, "PollerCommunicator::Connect select lastError=%d", (int)errno);
				_connected = false;
				Close();
			}
		}
	}

	if (_connected == true)
		LogString("GENERAL", "PollerCommunicator::Connect CONNECTED\n");
	return _connected;
}


//called to close a connection
void PollerCommunicator::Close()
{
	LogString("GENERAL", "PollerCommunicator::Close\n");
	_cancelall = true;
	FD_CLR(_socket, &write_fds);
	FD_CLR(_socket, &read_fds);
	FD_CLR(_socket, &writeerror_fds);
	FD_CLR(_socket, &readerror_fds);
	close(_socket);

	_socket = INVALID_SOCKET;
	_connected = false;
}


//Called to send data to a connected socket
bool PollerCommunicator::Send(char* datatosend, long lengthofdatatosend, long& bytessent)
{
	bool ireturn = false;

	fd_set tempwrite_fds;
	fd_set temperror_fds;

	try
	{
		_sending = true;

		lock_guard<recursive_mutex> lock(_sendcritsec);

		tempwrite_fds = write_fds; //select modifies the original so we need a copy
		temperror_fds = readerror_fds;

		timeval tv;
		tv.tv_sec = SEND_TIMEOUT_SECONDS;
		tv.tv_usec = 0;

		int result = select(_socket + 1, NULL, &tempwrite_fds, &temperror_fds, &tv);
		if (result == SOCKET_ERROR)
		{
			LogString(ERROR_LOG, "PollerCommunicator::Send Failed on select with -1 lastError=%d", (int)errno);
			Close();
			_sending = false;
			return false;
		}
		else if (0 == result) //timeout
		{
			LogString(ERROR_LOG, "PollerCommunicator::Send Failed on select with 0-Timeout\n");
			Close();
			_sending = false;
			return false;
		}
		// Else select() returned the total number of socket handles that are ready and contained in the fd_set structures (tempwrite_fds and temperror_fds).

		if (FD_ISSET(_socket, &temperror_fds))
		{
			LogString(ERROR_LOG, "PollerCommunicator::Send Failed on FD_ISSET for error_fds\n");
			Close();
			_sending = false;
			return false;
		}


		// The socket is expected to be configured as blocking, so, if no error occurs then sending less than the full length should not occur.
		if (FD_ISSET(_socket, &tempwrite_fds))
		{
			int ibytesent = 0;

			if ((ibytesent = send(_socket, datatosend, lengthofdatatosend, 0)) == SOCKET_ERROR)
			{
				LogString(ERROR_LOG, "PollerCommunicator::Send Failed on send with SOCKET_ERROR lastError=%d", (int)errno);
				Close();
				_sending = false;
				return false;
			}
			else
			{
				if (ibytesent != lengthofdatatosend)
				{
					// If the socket is configured as blocking then sending less than the full length should not occur.
					// Report an error, because we do not have code to send the rest of the buffer.
					// 2013-10-07 Failure to send the rest of buffer should cause incorrect decryptions by the poller for subsequent messages.
					LogString(ERROR_LOG, "PollerCommunicator::Send Partial %ld/%ld", (long)ibytesent, (long)lengthofdatatosend);
				}

				bytessent = ibytesent;
				ireturn = true;
			}
		}
		else
		{
			LogString(GENERAL, "PollerCommunicator::Send Failed on FD_ISSET for tempwrite_fds errno=%d", (int)errno);
			LogString(ERROR_LOG, "PollerCommunicator::Send Failed on FD_ISSET for tempwrite_fds errno=%d", (int)errno);
			Close();
		}

		_sending = false;
	}
	catch (...)
	{
		Close();
		_sending = false;
		return false;
	}
	_sending = false;

	return ireturn;
}

SocketReadStatus PollerCommunicator::Read(char* buftorecv, long maxlengthofdatatorecv, long& bytesrecv, long timeout)
{
	SocketReadStatus ireturn = SOCKET_READ_ERROR;
	int nbytes = 0;
	fd_set tempread_fds;
	fd_set temperror_fds;

	try
	{
		_reading = true;

		lock_guard<recursive_mutex> lock(_recvcritsec);

		tempread_fds = read_fds; //select modifies the original so we need a copy
		temperror_fds = readerror_fds;

		timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = 0;

		int result = select(_socket + 1, &tempread_fds, NULL, &temperror_fds, &tv);
		if (result == -1)
		{
			LogString(ERROR_LOG, "PollerCommunicator::Recv Failed on Select with -1\n");
			Close();
			_reading = false;
			return SOCKET_READ_ERROR;
		}
		else if (0 == result) //timeout
		{
			LogString(POLLER_LOG, "PollerCommunicator::Recv TimedOut");
			_reading = false;
			return SOCKET_READ_TIMEOUT;
		}


		if (FD_ISSET(_socket, &temperror_fds))
		{
			LogString(ERROR_LOG, "PollerCommunicator::Recv Failed on FD_ISSET with error_fds\n");
			Close();
			_reading = false;
			return SOCKET_READ_ERROR;
		}

		if (FD_ISSET(_socket, &tempread_fds))
		{
			if ((nbytes = recv(_socket, (char*)buftorecv, maxlengthofdatatorecv, 0)) <= 0)
			{
				LogString(POLLER_LOG, "PollerCommunicator::Recv Failed on Recv with <=0");
				_reading = false;
				return SOCKET_READ_ZERO;
			}
			else
			{
				bytesrecv = nbytes;
				ireturn = SOCKET_READ_SUCCESS;
			}
		}
		else
		{
			LogString(GENERAL, "PollerCommunicator::Read FD_ISSET(_socket, &tempread_fds) errorCode=%d", (int)errno);
			LogString(ERROR_LOG, "PollerCommunicator::Read FD_ISSET(_socket, &tempread_fds) errorCode=%d", (int)errno);
		}

		_reading = false;
	}
	catch (...)
	{
		Close();
		_reading = false;
	}
	_reading = false;
	return ireturn;
}


void PollerCommunicator::Cancel()
{
	_cancelall = true;
}

bool PollerCommunicator::Connected()
{
	return _connected;
}

bool PollerCommunicator::Reading()
{
	return _reading;
}

bool PollerCommunicator::Sending()
{
	return _sending;
}

DWORD PollerCommunicator::GetPollerPacketSentVerifyErrors()
{
	return m_pollerPacketSentVerifyErrors;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//zero out the internal buffers
void PollerCommunicator::ResetBufferToZero()
{

#ifdef DEBUG
	FillMemory(_buffer, (unsigned int)_bufferlength, DEBUGMEMFILLBYTE);
	ZeroMemory(_buffer, (unsigned int)_bufferlength);
#endif

	_bufferlength = 0;
}


//Check to see if the size of the new packet will put us over the limited size
bool PollerCommunicator::WillNewPacketExceedBufferMax(long NewPacketLength)
{
	bool retval = false; //Set to false so we wont always exceed unless math works

	if (((_bufferlength + NewPacketLength) > MAXPACKETLENGTH) || ((_bufferlength + NewPacketLength) <= 0))	//check if we are greater than or less than the allowed values
	{
		retval = true;
	}

	return retval;
}

//loop through _buffer and find first occurence of stx
bool PollerCommunicator::FindSTX(long startindex)
{
	bool retval = false;

	if (true == FindByte(_buffer, _bufferlength, STX, startindex, &_stxindex))		//check the incoming for stx
	{
		retval = true;
	}
	return retval;
}


//loop through _buffer and find first occurence of etx
bool PollerCommunicator::FindETX(long startindex)
{
	bool retval = false;

	if (true == FindByte(_buffer, _bufferlength, ETX, startindex, &_etxindex))		//check the incoming for etx
	{
		retval = true;
	}

	return retval;
}


//Check if there is another STX between the first STX and the First ETX
bool PollerCommunicator::CheckIfSecondSTX()
{
	bool retval = false;
	long tempstxindex = 0;

	//look for another stx byte in the buffer.
	if (true == FindByte(_buffer, _bufferlength - _stxindex, STX, _stxindex + 1, &tempstxindex))		//make sure there isnt another stx before etx
	{
		if (_etxindex < tempstxindex)
		{
			//did not find second stx
			retval = false;
		}
		else
			retval = true;
	}

	return retval;
}

//copy the found packet into the output packet
bool PollerCommunicator::CopyFoundPacketToOutputPacketAndDLEDecode(BYTE* extendedpacket, long extendedPacketBufferSize, long* extendedPacketReturnLength, BYTE* OutputPacket, long OutputPacketLength, long* OutputPacketReturnLength)
{
#ifdef DEBUG
	FillMemory(_tempbuffer, MAXPACKETLENGTH, DEBUGMEMFILLBYTE);
	ZeroMemory(_tempbuffer, MAXPACKETLENGTH);

	FillMemory(OutputPacket, OutputPacketLength, DEBUGMEMFILLBYTE);
	ZeroMemory(OutputPacket, OutputPacketLength);
#endif

	if (NULL == extendedpacket || NULL == extendedPacketReturnLength || NULL == OutputPacket)
	{
		LogString(ERROR_LOG, "PollerCommunicator::CopyFoundPacketToOutputPacketAndDLEDecode NULL pointer passed\n");
		return false;
	}

	*extendedPacketReturnLength = 0;
	*OutputPacketReturnLength = 0;

	//calculate the size of packet
	long tempsize = (_etxindex - _stxindex + 1);

	memcpy(OutputPacket, &_buffer[_stxindex + 1], tempsize - 2);
	tempsize -= 2;

	DWORD templength = DLEDecode(OutputPacket, _tempbuffer, tempsize);

	memcpy(extendedPacketReturnLength, _tempbuffer, sizeof(uint32_t));

	if (*extendedPacketReturnLength > 0)
	{
		// Make sure extended packet length does not over flow the extendedpacket buffer
		if (extendedPacketBufferSize < *extendedPacketReturnLength)
		{
			LogString(ERROR_LOG, "PollerCommunicator::CopyFoundPacketToOutputPacketAndDLEDecode extend packet length too large\n");
			return false;
		}

		memcpy(extendedpacket, &_tempbuffer[4], *extendedPacketReturnLength);
	}

	templength -= 4 + *extendedPacketReturnLength;

	// Make sure templength does not over flow the OutputPacket buffer
	if (OutputPacketLength < (long)templength)
	{
		LogString(ERROR_LOG, "PollerCommunicator::CopyFoundPacketToOutputPacketAndDLEDecode DLEDecode length too large\n");
		return false;
	}

#ifdef DEBUG
	FillMemory(OutputPacket, OutputPacketLength, DEBUGMEMFILLBYTE);
	ZeroMemory(OutputPacket, OutputPacketLength);
#endif

	memcpy(OutputPacket, &_tempbuffer[*extendedPacketReturnLength + 4], templength);

	*OutputPacketReturnLength = templength;

	return true;
}


//copy everything after the last packet to the beginning of buffer.
void PollerCommunicator::CopyRemainingDataToFrontOfBuffer()
{
	if (_etxindex < _bufferlength)
	{
		// there is more data after packet

		//copy remaining data to the beginning of buffer
		memcpy(_buffer, &_buffer[_etxindex + 1], _bufferlength - _etxindex - 1); // _etx is zero based and when doing size math with it we need to subtract one

		//set new buffer size
		_bufferlength = _bufferlength - _etxindex - 1;

#ifdef DEBUG
		FillMemory(&_buffer[_bufferlength], MAXPACKETLENGTH - _bufferlength, DEBUGMEMFILLBYTE);
		ZeroMemory(&_buffer[_bufferlength], MAXPACKETLENGTH - _bufferlength);
#endif
	}
	else
	{
		//there is no more data after this packet

		//set new buffer size
		_bufferlength = 0;
	}
}

//Puts the new data on the buffer
void PollerCommunicator::PushNewDataOnBuffer(BYTE* InputPacket, long InputPacketLength)
{
	if (0 < InputPacketLength)
	{
		//add new data to internal buffer
		memcpy(&_buffer[_bufferlength], InputPacket, InputPacketLength);
		_bufferlength += InputPacketLength;
	}
}

//This is the public function used to push data on to buffer and true is returned if a packet
//is found. The packet is defined by the _stx and _etx passed in to SetStartNFinish or defaulted
//in construct
bool PollerCommunicator::AddPacket(BYTE* InputPacket, long InputPacketLength, BYTE* OutputPacket, long OutputPacketLength, long* OutputPacketReturnLength)
{
	bool retval = false;
	bool goodPacket = false;

	_stxindex = 0;
	_etxindex = 0;

#ifdef DEBUG
	//used to easily look at the mem windows and determine what changed.
	FillMemory(OutputPacket, OutputPacketLength, DEBUGMEMFILLBYTE);
	ZeroMemory(OutputPacket, OutputPacketLength);
#endif

	try
	{
		//make sure what we are pushing doesnt exceed our boundaries.
		if (true == WillNewPacketExceedBufferMax(InputPacketLength))
		{
			ResetBufferToZero();
		}

		//get the data on the buffer for parsing
		PushNewDataOnBuffer(InputPacket, InputPacketLength);

		if (0 < _bufferlength)
		{
			if (true == FindSTX())
			{
				//it should be at the beginning of the buffer
				if (_stxindex != 0)
				{
					LogData(BADPOLLERDATA_LOG, DATA_READ | DATA_ETHERNET, _buffer, _stxindex);
				}

				//we found an stx now lets look for etx
				if (true == FindETX(_stxindex))
				{

					do	//loop till we have agood formed packet
					{
						if (false == CheckIfSecondSTX())
						{
							retval = true;		//we got our packet
							goodPacket = true;	//get out of loop
						}
						else
						{
							//we need to know if we're dropping data
							LogData(BADPOLLERDATA_LOG, DATA_READ | DATA_ETHERNET, _buffer, _bufferlength);

							//we got two stx's then an etx Need to jump to next stx
							if (false == FindSTX(_stxindex))
							{
								//this should never happen. If it does it means something(thread) modified the input buffer
								//From the last CheckIfSecondSTX till this line of code.

								retval = false;		//we do not have our packet
								goodPacket = true;	//get out of loop anyways
								ResetBufferToZero();	//something is wrong clear the buffer
							}
						}
					} while (goodPacket == false);

				} //false == FindETX(_stxindex)
				else
				{
					//packet not complete yet
					retval = false;
				}

			} //false == FindSTX()
			else
			{
				//if there is no stx then the data is no good.
				retval = false;
				ResetBufferToZero();

			}
		}// if( 0 < _bufferlength)

		if (true == retval)
		{
			long extendedpacketlength;

			//Copy packet to output and move the rest of data to beginning of packet
			if (true == CopyFoundPacketToOutputPacketAndDLEDecode(_extendedpacket, MAXPACKETLENGTH, &extendedpacketlength, OutputPacket, OutputPacketLength, OutputPacketReturnLength))
			{
				CopyRemainingDataToFrontOfBuffer();

				if (extendedpacketlength > 0)
				{
					DWORD packettype;
					memcpy(&packettype, _extendedpacket, sizeof(DWORD));

					switch (packettype)
					{
					case 1:	//exchange with pub and session

						LogString(GENERAL, "PollerCommunicator::AddPacket EPI Exchange Packet Recvd\n");

						//Poller sends us a public key BLOB here, but we don't use/need it, so we just
						//parse out the length so that we can skip over it.

						//parse remote pub key length
						DWORD dwRemoteKeyBlobLength;
						memcpy(&dwRemoteKeyBlobLength, &_extendedpacket[12], sizeof(DWORD));

						//parse session key length
						DWORD dwSessionKeyBlobLength;
						memcpy(&dwSessionKeyBlobLength, &_extendedpacket[16 + dwRemoteKeyBlobLength], sizeof(DWORD));

						{

							lock_guard<recursive_mutex> lock(m_sessionKeyCriticalSection);

							PBYTE pbSessionKeyBlob;

							//create space for session key
							if ((pbSessionKeyBlob = (PBYTE)malloc(dwSessionKeyBlobLength)) == NULL)
							{
								LogString(ERROR_LOG, _T("PollerCommunicator::AddPacket malloc(dwSessionKeyBlobLength=%u) FAILED"), (unsigned)dwSessionKeyBlobLength);
								//failed to allocate memory
								retval = false;
							}
							else
							{
								//parse session key
								memcpy(pbSessionKeyBlob, &_extendedpacket[16 + dwRemoteKeyBlobLength + 4], dwSessionKeyBlobLength);

								// If a session key is imported then the preceding diagnostic information("EPI Exchange Packet Recvd") was provided first.
								// If a session key was imported then the following diagnostic information("EPI Exchange Keys imported") was provided afterwards.
								//import session pub key and decrypt with local private exchange key

								if (hSessionKey)
								{
									free(hSessionKey);
									hSessionKey = NULL;
								}
								if (hSessionVerifyKey)
								{
									free(hSessionVerifyKey);
									hSessionVerifyKey = NULL;
								}

								hSessionKey = blobToRC4(pbSessionKeyBlob, dwSessionKeyBlobLength, hExchangePubKey);

								free(pbSessionKeyBlob);

								if (hSessionKey) {
									hSessionVerifyKey = (RC4_KEY*)malloc(sizeof(RC4_KEY));
									memcpy(hSessionVerifyKey, hSessionKey, sizeof(RC4_KEY));
								}
							}
						}

						LogString(GENERAL, "PollerCommunicator::AddPacket EPI Exchange Keys imported\n");
						bsessionkeywassent = true;
						break;
					case 2://parse out just exchange pub key
						break;
					}
				}

				if (*OutputPacketReturnLength > 0)
				{
					RC4_KEY hSessionKey_local;

					// This kludge is due to the fact that Poller resets the session key after each message
					// and expects us to do the same.
					{
						lock_guard<recursive_mutex> lock(m_sessionKeyCriticalSection);
						memcpy(&hSessionKey_local, hSessionKey, sizeof(hSessionKey_local));
					}

					RC4(&hSessionKey_local, *OutputPacketReturnLength, OutputPacket, OutputPacket);

				}
				else
					retval = false;

			}
			else
			{
				retval = false;
			}
		}
	}
	catch (...)
	{
		bsessionkeywassent = false;
		retval = false;
	}

	return retval;
}

//search a byte array for a certain byte from an offset
bool PollerCommunicator::FindByte(BYTE* PacketToCheck, long PacketToCheckLength, BYTE byteToFind, long SearchStartIndex, long* ReturnIndex)
{
	bool retval = false;

	if (SearchStartIndex > PacketToCheckLength)
		retval = false;
	else
	{
		for (long i = SearchStartIndex; i < PacketToCheckLength; i++)
		{
			if (PacketToCheck[i] == byteToFind)
			{
				retval = true;
				*ReturnIndex = i;
				break;
			}
		}
	}
	return retval;
}

bool PollerCommunicator::CreatePacketToSend(const BYTE* packettomassage,
	BYTE* massagedpacket,
	long packetlength,
	long* massagedpacketlength,
	bool alreadySuspicious,
	const BYTE nCompassMacAddress[],
	TiltCode& tiltCode,
	WORD messageID,
	DWORD newMessageSize)
{
	string reason;
	if (!alreadySuspicious)
	{
		CPollerCheckMessage::CheckMessage(packettomassage, (WORD)packetlength, nCompassMacAddress, tiltCode, messageID, reason);
		if (tiltCode != tltUnknown)
		{
			CDiagnostics::ReportBadPollerProtocolSendPacket(tiltCode, reason.c_str(), packettomassage, (WORD)packetlength);
		}
	}
	bool retval = false;
	long overallpacketlength = 0;
	bool newMessageAllocated = true;

	//If message check passes then only proceed forward
	if (messageID == POLLER_MSG_TRANSACTION || tltPollerProtocolCheckPacket != tiltCode)
	{
		retval = true;
		try
		{
			ResizePacketToSendIfRequired(&_tempbufferforCreate, packetlength, messageID, newMessageSize);
			ResizePacketToSendIfRequired(&_tempbufferforsend, packetlength, messageID, newMessageSize);

			memcpy(_tempbufferforCreate, packettomassage, packetlength);
			ZeroMemory(massagedpacket, newMessageSize);

			//send our public key if it hasn't already happened
			if (bpublickeywassent == false)
			{
				//set length of extended
				DWORD extendedlength = 16 + dwExchangePubKeyBlobLength;
				memcpy(&massagedpacket[overallpacketlength], &extendedlength, sizeof(DWORD));
				overallpacketlength += sizeof(DWORD);

				//set protocol type
				DWORD protocolNum = 1;
				memcpy(&massagedpacket[overallpacketlength], &protocolNum, sizeof(DWORD));
				overallpacketlength += sizeof(DWORD);

				//set message count. Number of messages in the extended packet. there can only be one right now
				DWORD MessageCount = 1;
				memcpy(&massagedpacket[overallpacketlength], &MessageCount, sizeof(DWORD));
				overallpacketlength += sizeof(DWORD);

				//set extended packet type
				DWORD packetnum = 2;
				memcpy(&massagedpacket[overallpacketlength], &packetnum, sizeof(DWORD));
				overallpacketlength += sizeof(DWORD);

				//set the length of the key
				memcpy(&massagedpacket[overallpacketlength], &dwExchangePubKeyBlobLength, sizeof(DWORD));
				overallpacketlength += sizeof(DWORD);

				//set the key
				memcpy(&massagedpacket[overallpacketlength], pbExchangePubKeyBlob, dwExchangePubKeyBlobLength);
				overallpacketlength += dwExchangePubKeyBlobLength;
				bpublickeywassent = true;
				LogString(GENERAL, "PollerCommunicator::CreatePacketToSend Pub Key being sent\n");
			}
			else
			{
				//set length of extended to zero if no data needs to go
				DWORD extendedlength = 0;
				memcpy(&massagedpacket[overallpacketlength], &extendedlength, sizeof(DWORD));
				overallpacketlength += sizeof(DWORD);
			}

			if (packetlength > 0)
			{
				{
					// This kludge is due to the fact that Poller resets the session key after each message
					// and expects us to do the same.
					RC4_KEY hSessionKey_local;
					if (hSessionKey != nullptr)
					{
						lock_guard<recursive_mutex> lock(m_sessionKeyCriticalSection);
						memcpy(&hSessionKey_local, hSessionKey, sizeof(hSessionKey_local));
					}
					else
					{
						LogString(ERROR_LOG, "PollerCommunicator::CreatePacketToSend() - hSessionKey is NULL.");
						ZeroMemory(&hSessionKey_local, sizeof(hSessionKey_local));
					}

					RC4(&hSessionKey_local, packetlength, _tempbufferforCreate, _tempbufferforCreate);
				}

				//copy packet data to packet
				memcpy(&massagedpacket[overallpacketlength], _tempbufferforCreate, packetlength);
				overallpacketlength += packetlength;
			}

			long templength = DLEEncode(massagedpacket, _tempbufferforsend, overallpacketlength);

			massagedpacket[0] = STX;

			memcpy(&massagedpacket[1], _tempbufferforsend, templength);

			massagedpacket[templength + 1] = ETX;
			*massagedpacketlength = templength + 2;
		}
		catch (...)
		{
			reason = "PollerCommunicator::CreatePacketToSend Error creating packet";
			tiltCode = tltPollerProtocolCreatePacket;
			CDiagnostics::ReportBadPollerProtocolSendPacket(tiltCode, reason.c_str(), packettomassage, (DWORD)packetlength);
			retval = false;
		}
	}

	//Reset the buffer to older size if it had to be increased 
	ResizePacketToSendIfRequired(&_tempbufferforCreate, packetlength, messageID, SIZE_OF_READ_BUFFER);
	ResizePacketToSendIfRequired(&_tempbufferforsend, packetlength, messageID, SIZE_OF_READ_BUFFER);

	return retval;
}


bool PollerCommunicator::ResizePacketToSendIfRequired(BYTE** packet, DWORD packetlength, WORD messageID, DWORD newPacketSize)
{
	bool newMessageAllocated = true;
	if (packetlength > SIZE_OF_READ_BUFFER / 2 && POLLER_MSG_ALL_GAME_LIST == messageID)
	{
		LogString(POLLER_LOG, "%s", __FUNCTION__);
		if (*packet)
		{
			delete[] * packet;
			*packet = nullptr;
		}

		*packet = new BYTE[newPacketSize];

		if (nullptr == *packet)
		{
			LogString(ERROR_LOG, "%s:%d Memory allocation failed for message: %u", __FUNCTION__, __LINE__, messageID);
			newMessageAllocated = false;
		}
	}

	return newMessageAllocated;
}

void PollerCommunicator::LogBuffer(LPCTSTR prefix, LPCTSTR bufferDesc, const BYTE* buffer, long bufferLength, LPCTSTR callerDesc)
{
	LogString(ERROR_LOG, _T("%s: %s={%s} in %s"),
		static_cast<LPCTSTR>(prefix),
		static_cast<LPCTSTR>(bufferDesc),
		static_cast<LPCTSTR>(CUtilities::ToHexString(buffer, (DWORD)bufferLength, TO_HEX_STRING_MAXIMUM_DATA_LENGTH).c_str()),
		static_cast<LPCTSTR>(callerDesc));
}

void PollerCommunicator::LogInfoBuffer(LPCTSTR bufferDesc, const BYTE* buffer, long bufferLength, LPCTSTR callerDesc)
{
	LogBuffer(_T("Info"), bufferDesc, buffer, bufferLength, callerDesc);
}

void PollerCommunicator::LogInvalidBuffer(LPCTSTR bufferDesc, const BYTE* buffer, long bufferLength, LPCTSTR callerDesc)
{
	LogBuffer(_T("Invalid"), bufferDesc, buffer, bufferLength, callerDesc);
}

// Verify that the inputs meet minimal requirements before proceding.
bool PollerCommunicator::VerifyPacketSentInputs(const BYTE* massagedPacket, long massagedPacketLength, const BYTE* message, long messageLength)
{
	bool ok(true);

	if (ok)
	{
		// Verify length of sent packet buffer before proceeding with verification.
		if (massagedPacketLength < 0)
		{
			ok = false;
			LogString(ERROR_LOG, _T("massagedPacketLength=%ld < 0 in %s"),
				(long)massagedPacketLength,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	if (ok)
	{
		// Verify sent packet buffer pointer if length > 0 before proceeding with verification.
		if (massagedPacket == NULL && massagedPacketLength > 0)
		{
			ok = false;
			LogString(ERROR_LOG, _T("massagedPacket=%p when massagedPacketLength=%ld > 0 in %s"),
				(const void*)massagedPacket,
				(long)massagedPacketLength,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	if (ok)
	{
		// Verify length of matching buffer before proceeding with verification.
		if (messageLength < 0)
		{
			ok = false;
			LogString(ERROR_LOG, _T("messageLength=%ld < 0 in %s"),
				(long)messageLength,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	if (ok)
	{
		// Verify sent packet buffer pointer if length > 0 before proceeding with verification.
		if (message == NULL && messageLength > 0)
		{
			ok = false;
			LogString(ERROR_LOG, _T("message=%p when messageLength=%ld > 0 in %s"),
				(const void*)message,
				(long)messageLength,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	return ok;
}

// Verify that the massagedPacket has an STX and ETX.
bool PollerCommunicator::VerifyPacketSentStxEtx(const BYTE* massagedPacket, long massagedPacketLength)
{
	bool ok(true);
	const long minimumDLEEncodedLength = SIZEOF_STX + SIZEOF_ETX;

	if (ok)
	{
		// Verify minimum length to proceed with STX and ETX verification.
		if (massagedPacketLength < minimumDLEEncodedLength)
		{
			ok = false;
			LogInvalidBuffer(_T("massagedPacket"), massagedPacket, massagedPacketLength, __TFUNCTION__);
			LogString(ERROR_LOG, _T("Invalid: massagedPacketLength=%ld < minimumDLEEncodedLength=%ld in %s"),
				(long)massagedPacketLength,
				(long)minimumDLEEncodedLength,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	if (ok)
	{
		// Verify STX.
		if (massagedPacket[0] != STX)
		{
			ok = false;
			LogInvalidBuffer(_T("massagedPacket"), massagedPacket, massagedPacketLength, __TFUNCTION__);
			LogString(ERROR_LOG, _T("massagedPacket[0]=%u != STX=%u in %s"),
				(unsigned)massagedPacket[0],
				(unsigned)STX,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	if (ok)
	{
		// Verify ETX.
		if (massagedPacket[massagedPacketLength - 1] != ETX)
		{
			ok = false;
			LogInvalidBuffer(_T("massagedPacket"), massagedPacket, massagedPacketLength, __TFUNCTION__);
			LogString(ERROR_LOG, _T("massagedPacket[massagedPacketLength - 1]=%u != ETX=%u in %s"),
				(unsigned)massagedPacket[massagedPacketLength - 1],
				(unsigned)ETX,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	return ok;
}

// Verify that the DLE decoded packet has a valid extended length field.
// The massagedPacket and massagedPacketLength are only used for diagnostic output.
bool PollerCommunicator::VerifyPacketSentExtendedLength(const BYTE* massagedPacket, long massagedPacketLength,
	const BYTE* packetSent, long packetSentLength, DWORD& extendedLength)
{
	bool ok(true);

	if (ok)
	{
		// Verify the minimum required total length after DLE decoding to contain an extended length field.
		if (packetSentLength < SIZEOF_EXTENDED_LENGTH)
		{
			ok = false;
			LogInvalidBuffer(_T("massagedPacket"), massagedPacket, massagedPacketLength, __TFUNCTION__);
			LogInvalidBuffer(_T("packetSent"), packetSent, packetSentLength, __TFUNCTION__);
			LogString(ERROR_LOG, _T("packetSentLength=%ld < SIZEOF_EXTENDED_LENGTH=%ld in %s"),
				(long)packetSentLength,
				(long)SIZEOF_EXTENDED_LENGTH,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	if (ok)
	{
		// Get the length of extended field.
		memcpy_s(&extendedLength, sizeof(extendedLength), packetSent, sizeof(extendedLength));

		// packetSentLength >= SIZEOF_EXTENDED_LENGTH if we got here.
		DWORD remainingLength(packetSentLength - SIZEOF_EXTENDED_LENGTH);

		// Verify the length of extended field.
		if (extendedLength > remainingLength)
		{
			ok = false;
			LogInvalidBuffer(_T("massagedPacket"), massagedPacket, massagedPacketLength, __TFUNCTION__);
			LogInvalidBuffer(_T("packetSent"), packetSent, packetSentLength, __TFUNCTION__);
			LogString(ERROR_LOG, _T("extendedLength=%u > remainingLength=%ld with packetSentLength=%ld in %s"),
				(unsigned)extendedLength,
				(long)remainingLength,
				(long)packetSentLength,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	return ok;
}

// Decrypt the message sent, in place.
// The massagedPacket, massagedPacketLength, packetSent and packetSentLength are only used for diagnostic output.
void PollerCommunicator::VerifyPacketSentDecrypt(BYTE* messageSent, DWORD messageSentLengthEncrypted)
{
	// Decrypt the message field if it is not empty.
	if (messageSentLengthEncrypted > 0)
	{
		lock_guard<recursive_mutex> lock(m_sessionVerifyKeyCriticalSection);
		{
			// Decrypt in place.
			{
				// This kludge is due to the fact that Poller resets the session key after each message
				// and expects us to do the same.

				RC4_KEY hSessionKey_local;
				memcpy(&hSessionKey_local, hSessionVerifyKey, sizeof(hSessionKey_local));

				RC4(&hSessionKey_local, messageSentLengthEncrypted, messageSent, messageSent);
			}
		}
	}
}

// Verify that the message sent matches the message used to create the message sent.
// The massagedPacket, massagedPacketLength, packetSent and packetSentLength are only used for diagnostic output.
bool PollerCommunicator::VerifyPacketSentMessage(const BYTE* massagedPacket, long massagedPacketLength, const BYTE* packetSent, long packetSentLength,
	const BYTE* messageSent, DWORD messageSentLength, const BYTE* message, long messageLength)
{
	bool ok(true);

	if (ok)
	{
		// Verify message field length after decryption.
		if (((long)messageSentLength) != messageLength || messageLength < 0)
		{
			ok = false;
			LogInvalidBuffer(_T("massagedPacket"), massagedPacket, massagedPacketLength, __TFUNCTION__);
			LogInvalidBuffer(_T("packetSent"), packetSent, packetSentLength, __TFUNCTION__);
			LogString(ERROR_LOG, _T("messageSentLength=%u != messageLength=%ld in %s"),
				(unsigned)messageSentLength,
				(long)messageLength,
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	if (ok)
	{
		// Verify message field after decryption.
		int memcmpMsgFieldResult = memcmp(messageSent, message, messageLength);
		if (memcmpMsgFieldResult != MEMCMP_EQUAL)
		{
			ok = false;
			LogInvalidBuffer(_T("massagedPacket"), massagedPacket, massagedPacketLength, __TFUNCTION__);
			LogInvalidBuffer(_T("packetSent"), packetSent, packetSentLength, __TFUNCTION__);
			LogInvalidBuffer(_T("messageSent"), messageSent, messageSentLength, __TFUNCTION__);
			LogInfoBuffer(_T("message"), message, messageLength, __TFUNCTION__);
			LogString(ERROR_LOG, _T("messageSent != message in %s"),
				static_cast<LPCTSTR>(__TFUNCTION__));
		}
	}

	return ok;
}

// Verify the packet sent.
// Don't verify the contents of the extended field, as we allow the source values to change between calls to CreatePacketToSend() and VerifyPacketSent().
bool PollerCommunicator::VerifyPacketSent(const BYTE* massagedPacket, long massagedPacketLength, const BYTE* message, long messageLength)
{
	bool ok(true);
	BYTE* packetSent(NULL);
	DWORD extendedLength(0);
	long packetSentLength(0);
	BYTE* messageSent(NULL); // Not allocated; do not free.

	if (ok)
	{
		ok = VerifyPacketSentInputs(massagedPacket, massagedPacketLength, message, messageLength);
	}

	if (ok)
	{
		ok = VerifyPacketSentStxEtx(massagedPacket, massagedPacketLength);
	}

	if (ok)
	{
		// massagedPacketLength >= SIZEOF_STX + SIZEOF_ETX if we got here.

		// DLE Decode message sent into a temporary buffer.
		// Length of the result data is always less than or equal to the length of the source data.
		packetSent = new BYTE[massagedPacketLength];
		packetSentLength = DLEDecode(massagedPacket + SIZEOF_STX, packetSent, massagedPacketLength - SIZEOF_STX - SIZEOF_ETX);

		ok = VerifyPacketSentExtendedLength(massagedPacket, massagedPacketLength,
			packetSent, packetSentLength, extendedLength);
	}

	if (ok)
	{
		// packetSentLength >= SIZEOF_EXTENDED_LENGTH + extendedLength if we got here.
		DWORD totalExtendedLength = SIZEOF_EXTENDED_LENGTH + (long)extendedLength;
		DWORD messageSentLengthEncrypted = (DWORD)packetSentLength - totalExtendedLength;
		messageSent = packetSent + totalExtendedLength;

		VerifyPacketSentDecrypt(messageSent, messageSentLengthEncrypted);
	}

	if (ok)
	{
		ok = VerifyPacketSentMessage(massagedPacket, massagedPacketLength, packetSent, packetSentLength,
			messageSent, messageLength, message, messageLength);
	}

	if (packetSent)
		delete[] packetSent;

	if (!ok)
	{
		++m_pollerPacketSentVerifyErrors;
	}
	return ok;
}

//DLE(Data length encoding) is an attempt at making the packet parsable by receiving end.
//For instance your parsing bytes are 0xEB and 0xEE if a packet contains one of these two bytes
//internally the parsing on the receiving end would parse out bad data. So we change any parsing bytes (plus the escape byte, 0xE0)
//between the Header(stx) and footer(etx) into two bytes. So an 0xEB would become 0xE0 and 0xEB - 0xE0.
long PollerCommunicator::DLEEncode(const BYTE* pabyPacketData, BYTE* pabyEncodeBuffer, long lPacketLength)
{
	long lEncodeDataLength = 0;
	const BYTE* pData = pabyPacketData;
	BYTE* pEncodeBuffer = pabyEncodeBuffer;

	while (lPacketLength-- > 0)
	{
		switch (*pData)
		{
		case DEFAULTDLEREPLACEMENT:
		case STX:
		case ETX:
			*(pEncodeBuffer++) = DEFAULTDLEREPLACEMENT;					//replace it with replacement byte
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


//DLE(Data length decoding) is an attempt at making the packet parsable by receiving end.
//For instance your parsing bytes are 0xEB and 0xEE if a packet contains one of these two bytes
//internally the parsing on the receiving end would parse out bad data. So we change any escape byte (0xE0) sequences
//between the Header(stx) and footer(etx) from two bytes back to the original byte. So an 0xE0 0x0B sequence would become 0xEB.
// 2013-10-09 If the last byte of input buffer is the escape byte (0xE0 then it is combined with the following byte and placed in the output buffer.
long PollerCommunicator::DLEDecode(const BYTE* pabyPacketData, BYTE* pabyDecodeBuffer, long lPacketLength)
{
	const BYTE* pDecodeData = pabyPacketData;
	BYTE* pDecodeBuffer = pabyDecodeBuffer;
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






//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool PollerCommunicator::SetUpEncryption()
{
#ifdef LEAK_CHECK
	// The OpenSSL library allocates heap memory that can't be explicitly released.
	// We temporarily disable heap tracking to hide this leak from our report.
	LockHeapAndDisableTracking();
#endif

	hExchangePubKey = RSA_generate_key(1024, 65537, NULL, NULL);

	dwExchangePubKeyBlobLength = 1024;

	EVP_PKEY* pk = EVP_PKEY_new();
	EVP_PKEY_set1_RSA(pk, hExchangePubKey);
	pbExchangePubKeyBlob = NULL;
	dwExchangePubKeyBlobLength = (DWORD)rsaPubToMSPub(&pbExchangePubKeyBlob, pk);
	EVP_PKEY_free(pk);

	bEncryptionSetup = true;

#ifdef LEAK_CHECK
	UnlockHeapAndReenableTracking();
#endif

	return true;
}

void PollerCommunicator::TearDownEncryption()
{
	if (pbExchangePubKeyBlob) OPENSSL_free(pbExchangePubKeyBlob);
	if (hExchangePubKey) RSA_free(hExchangePubKey);
	if (hSessionKey) free(hSessionKey);
	if (hSessionVerifyKey) free(hSessionVerifyKey);
}
