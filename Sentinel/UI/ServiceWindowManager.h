#pragma once
#include "../ContentDeliverySystem/FlashPlayer.h"
#include "../ContentDeliverySystem/FlashComServer.h"	
#include "../InternalMessage.h"
#include "../DisplayManager.h"
#include "ServiceWindowDefs.h"
#include "Hardware/CardReaderLayer.h"
#include "FullScreenAttractorCommand.h"

// forward declaration
class CUIProtocol;

class CServiceWindowManager
{
public:
	/// <summary>
	/// Ctor with parameters
	/// </summary>
	CServiceWindowManager();

	/// <summary>
	/// Default Dtor
	/// </summary>
	virtual ~CServiceWindowManager(void);

	/// <summary>
	/// Starts the service window.
	/// </summary>
	/// <param name="uiProtocol">The UI Protocol object.</param>
	void Start(CUIProtocol *uiProtocol);

	virtual void ReadAndProcessFlashServer(void *parent);
	bool IsServiceWindowListenerThreadClosed();
	void SetUIConfig(CUIConfig &uiConfig);

	/// <summary>
	/// Sends the message to angular UI.
	/// </summary>
	/// <param name="message">The message to send to the UI.</param>
	void SendMessageToAngularUI(const std::string& message);
	
    /// <summary>
	/// Checks to see if ServiceWindowListenerThread exited.
	/// </summary>
    ///  <returns>true if ServiceWindowListenerThread exited, false otherwise.</returns>
    bool ServiceWindowListenerThreadExited() const;

	/// <summary>
	/// Launches chrome
	/// </summary>
	void LaunchChrome();

	/// <summary>
	/// Kills chrome
	/// </summary>
	void KillChrome();

	/// <summary>
	/// Creates CFullScreenAttractor for the given screen.
	/// </summary>
	/// <param name="screen">IN - Screen ID of the attractor.</param>
	/// <param name="dynamicContentPath">IN - dynamic content path.</param>
	/// <param name="displayDurationOverride">IN - display duration.</param>
	/// <returns>CFullScreenAttractorCommand object.</returns>
	CFullScreenAttractorCommand* CreateFullScreenAttractor(screenID screen,
		const std::string& dynamicContentPath = "", DWORD displayDurationOverride = 0) const;

protected:
	void init();
	char* base64(const unsigned char* input, int length);
	string getHandshakeResponse(string key);
	void ProcessWebSocketFrames(CUIProtocol* uiProtocol, const BYTE* messageData, long dataLength);
	BYTE* ParseFrame(const BYTE* buffer, size_t bufferLength, bool& isPingFrame, bool& isFinalPartOfPayload, size_t& parsedLength, size_t& payloadLength);
	BYTE* BuildFrame(BYTE opCode, BYTE* payload, size_t payloadLength, size_t& outputFrameLength);
	void SendMessage(string& message);
	void GetAssetTagAndDuration(screenID screen, std::string& assetTag, DWORD& duration) const;

private:
	CUIConfig m_uiConfig;
	CFlashComServer* m_flashServer;
	std::unique_ptr<std::thread> m_serviceWindowListenerThread;
	std::recursive_mutex m_criticalSection;
	std::string m_partialMessage;
	BYTE* m_partialWebsocketFrame;
	size_t m_partialWebsocketFrameLen;
	bool m_firstDataReceivedFromUI;
	bool m_serviceWindowListenerThreadExited;
};
