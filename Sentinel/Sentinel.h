#pragma once

#include "InternalsFramework.h"
#include "SystemProtocol.h"
#include "UI/UIProtocol.h"
#include "ProgressiveProtocol/ProgressiveProtocol.h"
#include "IProtocolManager.h"

// CSentinelApp:
// See Sentinel.cpp for the implementation of this class
//
class CSentinelApp : public IProtocolManager
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CSentinelApp"/> class.
	/// </summary>
	CSentinelApp();
    
	/// <summary> 
	/// Starts the specified protocol. 
	/// </summary>
	/// <param name="protocolType">The protocol where the message is to be 
	/// sent</param>
	virtual void StartProtocol(ProtocolType protocolType);
	
     /// <summary> 
    /// Send message to the specified protocol's inbound queue. 
	/// </summary>
	/// <param name="protocolType">The protocol where the message is to be 
	/// sent</param> 
	/// <param name="msg">The message to be placed on the protocol's inbound 
	/// queue</param> 
	/// <param name="priorityLevel">The priority level the message is to be taken 
	/// off the queue </param> 
	virtual void SendMessageToProtocol(ProtocolType protocolType, CInternalMessage *msg, int priorityLevel );

	/// <summary>
	/// Removed the specified protocol
	/// </summary>
	virtual void RemoveProtocol(ProtocolType protocolType);
	   
	/// <summary>
	/// Blocks until the protocol is started
	/// </summary>
	virtual void WaitForProtocol(ProtocolType protocolType);

	/// <summary> Clears the queue starvation for the specified protocol's inbound 
	/// queue. 
	/// </summary>
	/// <param name="protocolType">The protocol where the inbound starvation 
	/// is to be cleared</param> 
	/// <param name="persistType">The persist type that is to be cleared </param>
	virtual void ClearInboundQueueStarvation(ProtocolType protocolType, MessagePersistType persistType = PERSIST_NONE ); 

    ///<param name="protocolType">The protocol to check</param> 
    /// <param name="persistType">The MessagePersistType to 
    /// check</param> 
    /// <returns>true if the queu is starved, false 
    /// otherwise</returns> 
    virtual bool IsQueueStarved(ProtocolType protocolType, MessagePersistType persistType);

    /// <summary> Determins whether the specified protocol is
    /// initialized
    /// </summary> 
    ///<param name="protocolType">The protocol to check</param>  
    /// <returns>true if the protocol is initialized, false 
    /// otherwise</returns> 
    virtual bool IsProtocolInitialized(ProtocolType protocolType);

	/// <summary>
    /// Flushes all persisted messages from the specified protocol's
    /// inbound queue, but leaves specified flush type messages. 
	/// </summary>
	/// <param name="protocolType">The protocol to flush</param> 
	/// <param name="flushTypeIDToNotFlush">The flush type to not flush</param>
    virtual void FlushAllPersistedButTypeFromInboundQueue(ProtocolType protocolType, int flushTypeIDToNotFlush);

	/// <summary> Sets whether the specified protocol's inbound queue is 
	/// interruptible by an equal priority. 
	/// </summary>
	/// <param name="protocolType">The protocol where the equal priority 
	/// flag is to be set</param> 
	/// <param name="isEqualPriorityInterruptible">true if messages are to interrupt
	/// messages of equal priority</param> 
	virtual void SetInboundQueueEqualPriorityInterrupt(ProtocolType protocolType, bool isEqualPriorityInterruptible);

    CQueueList* GetInboundQueueForProtocol(ProtocolType protocolType);

	/// <summary> Sets whether the specified protocol's inbound queue is ready
	/// </summary>
	/// <param name="protocolType">The protocol to check</param> 
    /// <returns>true if the protocolType is ready, false otherwise</returns> 
	virtual bool InboundQueueForProtocolIsReady(ProtocolType protocolType);

	/// <summary>
	/// Removes the UI (Sentinel dialog) instance.
	/// </summary>
	void RemoveUIProtocol(void);

	/// <summary>
	/// Removes the (Paltronics) progressive protocol instance.
	/// </summary>
	void RemoveProgressiveProtocol(void);

    /// <summary>
    /// Gets the internals to (Paltronics) progressive protocol queue.
    /// </summary>
    /// <returns>A pointer to the queue if it exists, NULL otherwise.</returns>
    CQueueList *GetInternalsToProgressiveQueue(void);

private:
	CInternalsFramework *m_internals;
	CSystemProtocol *m_egmProtocol;
	CSystemProtocol *m_systemProtocol;
	CSystemProtocol *m_progressiveProtocol;
	CUIProtocol *m_uiProtocol;

	BOOL InitInstance();

	/// <summary>
	/// Starts the first part of the Sentinel application instance.
	/// </summary>
	/// <param name="firmwareVersionString">OUT - The firmware version.</param>
	void Start1(std::string& firmwareVersionString);

	/// <summary>
	/// Starts the second part of the Sentinel application instance.
	/// </summary>
	/// <returns>TRUE if successful, FALSE otherwise.</returns>
	BOOL Start2();

	/// <summary>
	/// Starts the third part of the Sentinel application instance.
	/// </summary>
	void Start3();

	/// <summary>
	/// Starts the fourth part of the Sentinel application instance.
	/// </summary>
	/// <param name="firmwareVersion">IN - The firmware version.</param>
	void Start4(const std::string& firmwareVersion);

	/// <summary>
	/// Performs the main loop of the Sentinel application instance.
	/// </summary>
	void Main();
           
	/// <summary>
	/// Starts the EGM protocol, if it has not already been started and it is configured to be startable.
	/// </summary>
	/// <returns>true if the protocol swas tarted during this call, false otherwise.</returns>
	bool StartGCF(void);
	
	/// <summary>
	/// Gets the internals to GCF (EGM/SAS protocol) queue.
	/// </summary>
	/// <returns>A pointer to the queue if it exists, NULL otherwise.</returns>
	CQueueList *GetInternalsToGCFQueue(void);

	/// <summary>
	/// Gets the internals to SCF (system/poller protocol) queue.
	/// </summary>
	/// <returns>A pointer to the queue if it exists, NULL otherwise.</returns>
	CQueueList *GetInternalsToSCFQueue(void);

	/// <summary>
	/// Gets the internals to UI (Sentinel dialog) queue.
	/// </summary>
	/// <returns>A pointer to the queue if it exists, NULL otherwise.</returns>
	CInternalMessageQueueList *GetInternalsToUIQueue(void);

	/// <summary>
	/// Removes the GCF (EGM/SAS) protocol instance.
	/// </summary>
	void RemoveGCFProtocol(void);

	/// <summary>
	/// Removes the SCF (system/poller) protocol instance.
	/// </summary>
	void RemoveSCFProtocol(void);
		
	/// <summary>
	/// Shuts down the Sentinel application instance.
	/// </summary>
	void Shutdown();

	template<typename T, typename... Targs>
	void LogS5String(std::stringstream ss, const char* format, T value, Targs... Fargs);

	void LogS5String(std::stringstream ss, const char* format);

	void CheckSystemTimeAndInitLogger(CNVRAMConfig& nvramConfig);

	// m_endCalibrationTask is used asynchronously by multiple threads.
	bool m_endCalibrationTask;
};
