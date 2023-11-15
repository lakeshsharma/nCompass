
#include "AES128EncryptionUtil.h"

/// \fn void CAES128EncryptionUtil::UpdateKeys(std::initializer_list<std::string> keys)
/// \brief Updates the keys used for encryption
/// \param  keys The keys.
void CAES128EncryptionUtil::UpdateKeys(std::initializer_list<std::string> keys)
{
    this->keys.clear();
    for (auto key : keys) {
        if (!key.empty())
        {
            unsigned char keybuf[SHA_DIGEST_LENGTH];
            SHA1((const unsigned char*)key.c_str(), key.length(), keybuf);
            std::unique_ptr<unsigned char> p(new unsigned char[AES_BLOCK_SIZE]);
            memcpy(p.get(), keybuf, AES_BLOCK_SIZE);
            this->keys.push_back(std::move(p));
        }
    }
}

/// \fn std::shared_ptr<unsigned char> CAES128EncryptionUtil::Decrypt(const unsigned char* encryptedText)
/// \brief Decrypts the given encrypted text
/// \param  encryptedText The encrypted text.
/// \returns A std::shared_ptr&lt;unsigned char&gt;
std::shared_ptr<unsigned char> CAES128EncryptionUtil::Decrypt(const unsigned char* encryptedText)
{
    int decodedLength;
    auto encryptedData = decode64(encryptedText, decodedLength);

    //loop through all the keys we have incase the message is old
    for (auto key : this->keys)
    {
        /* Create and initialise the context */
        EVP_CIPHER_CTX* ctx;
        if (!(ctx = EVP_CIPHER_CTX_new()))
        {
            handleErrors();
            continue;
        }

        //Init the encryption context to aes 128
        if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.get(), NULL))
        {
            handleErrors();
            continue;
        }

        //Decrypt the data
        int len;
        int plaintext_len;
        std::shared_ptr<unsigned char> buffer(new unsigned char[decodedLength + AES_BLOCK_SIZE], std::default_delete<unsigned char[]>());
        if (1 != EVP_DecryptUpdate(ctx, buffer.get(), &len, encryptedData.get(), decodedLength))
        {
            handleErrors();
            continue;
        }
        plaintext_len = len;

        //Finalize the decryption of the data incase any data left.
        if (1 != EVP_DecryptFinal_ex(ctx, buffer.get() + len, &len))
        {
            handleErrors();
            continue;
        }
        plaintext_len += len; //keep track of the total decrypted text length.

        //Because the library using this needs a 0 terminated string
        buffer.get()[plaintext_len] = 0;

        //Free the context
        EVP_CIPHER_CTX_free(ctx);
        return buffer;
    }

    //If there were no keys then just pass back the old data. There is a little inefficiency if encryption is off, but
    // is turned on in most cases so will not matter.
    auto oldLength = strlen((char*)encryptedText);
    std::shared_ptr<unsigned char> oldBuffer(new unsigned char[oldLength + 1], std::default_delete<unsigned char[]>());
    memcpy(oldBuffer.get(), encryptedText, oldLength);
    oldBuffer.get()[oldLength] = 0;
    return oldBuffer;
}

/// \fn std::shared_ptr<unsigned char> CAES128EncryptionUtil::decode64(const unsigned char* encodedInput, int& decodedLength)
/// \brief Decodes a base 64 encoded string.
/// \param 		    encodedInput The encoded input.
/// \param [in,out] decodedLength Length of the decoded.
/// \returns A std::shared_ptr&lt;unsigned char&gt;
std::shared_ptr<unsigned char> CAES128EncryptionUtil::decode64(const unsigned char* encodedInput, int& decodedLength)
{
    //The past in data is a 0 terminated string
    auto inputLength = strlen((char*)encodedInput);
    int lengthToRemove = 0;
    const size_t pl = 3 * inputLength / 4; //This is how the padding is figured out.

    //The openssl block decoder used for decode of base 64, decodes the entire string. Thus adding 2 characters
    // at the end because of the = or ==. The = or == is how base64 encoding ensures proper padding. So this checks
    // the end of the encoded input for on or 2 equal signs and sets a variable so we can shrink the size based on them.
    if (inputLength > 3 && encodedInput[inputLength - 1] == '=' && encodedInput[inputLength - 2] == '=')
        lengthToRemove = 2;
    else if (inputLength > 2 && encodedInput[inputLength - 1] == '=')
        lengthToRemove = 1;

    //Create a new buffer in a shared pointer to hold the decoded data.
    std::shared_ptr<unsigned char> buffer(new unsigned char[pl], std::default_delete<unsigned char[]>());

    //use open ssl to decode the encoded text.
    const auto ol = EVP_DecodeBlock(buffer.get(), encodedInput, static_cast<int>(inputLength));

    //show error if predicted not match returned.
    if (pl != ol) 
    {
        LogString(PROGRESSIVE, "CAES128EncryptionUtil: decode64 predicted %d but we got %d.", pl,ol);
    }

    //Set the decodedLength smaller based on the equal signs(= or ==) at the end.
    decodedLength = ol - lengthToRemove;
    return buffer;
}

/// \fn void CAES128EncryptionUtil::handleErrors(void)
/// \brief Handles the errors
void CAES128EncryptionUtil::handleErrors(void)
{
    LogString(PROGRESSIVE, "CAES128EncryptionUtil: error processing encryption. Reason: %s", ERR_error_string(ERR_get_error(), NULL));
    ERR_clear_error();
}