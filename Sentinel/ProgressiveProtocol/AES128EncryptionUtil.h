#pragma once
#include <string>
#include <iostream>
#include "openssl/aes.h"
#include "openssl/sha.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <vector>
#include <cassert>
#include "Logging/LogString.h"

/// \class CAES128EncryptionUtil
/// \brief This class mimics the AES encryption used in onelink/megamic for multicast security.
class CAES128EncryptionUtil
{
private:

	/// \fn std::shared_ptr<unsigned char> CAES128EncryptionUtil::decode64(const unsigned char* encodedInput, int& decodedLength);
	/// \brief Decodes a base64 string
	/// \param 		    encodedInput The encoded input.
	/// \param [in,out] decodedLength Length of the decoded.
	/// \returns A std::shared_ptr&lt;unsigned char&gt;
	std::shared_ptr<unsigned char> decode64(const unsigned char* encodedInput, int& decodedLength);

	/// \fn void CAES128EncryptionUtil::handleErrors(void);
	/// \brief Handles the errors
	void handleErrors(void);
	std::vector<std::shared_ptr<unsigned char>> keys;

public:

	/// \fn std::shared_ptr<unsigned char> CAES128EncryptionUtil::Decrypt(const unsigned char* encryptedText);
	/// \brief Decrypts the given encrypted text
	/// \param  encryptedText The encrypted text.
	/// \returns A std::shared_ptr&lt;unsigned char&gt;
	std::shared_ptr<unsigned char> Decrypt(const unsigned char* encryptedText);

	/// \fn void CAES128EncryptionUtil::UpdateKeys(std::initializer_list<std::string> keys);
	/// \brief Updates the keys used for decryption
	/// \param  keys The keys.
	void UpdateKeys(std::initializer_list<std::string> keys);
};