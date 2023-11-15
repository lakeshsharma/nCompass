#pragma once

#include "ProgressiveKeys.h"
#include "Stomp/StompFrame.h"


using namespace Stomp;

class ProgressiveMulticastFrame : public Stomp::Frame
{
public:
	static const size_t FrameLen = 1024 * 64;
    ProgressiveMulticastFrame(const std::string& id);

protected:
	enum MulticastErr
	{
		InvalidId = Stomp::__ErrLast__,
		InvalidType
	};

	std::string			 m_id;
	bool				 m_receivedId;
	FromOneLinkUdp::Type m_type;
	const char*			 m_pTypeStr;
	bool				 m_encryptedBody;

public:
	virtual ~ProgressiveMulticastFrame();

	void Reset();

	Stomp::Err Parse();

	FromOneLinkUdp::Type GetType() const;
	const char* GetTypeStr() const;
	bool HasEncryptedBody() const;

protected:
	virtual Stomp::Err OnCommand(Stomp::RecvCommand::Type command);
	virtual Stomp::Err OnHeader(char* pKey, size_t keylen, char* pValue, size_t valueLen);
	virtual Stomp::Err OnBody(char* pBody, size_t bodyLen);

private:
	ProgressiveMulticastFrame(const ProgressiveMulticastFrame&);
	void operator=(const ProgressiveMulticastFrame&);
};


