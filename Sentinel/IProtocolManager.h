#pragma once

#include "InternalMessage.h"

class IProtocolManager
{
   
public:
    
   typedef enum
   {
        PollerProtocol,
        EGMProtocol,
        ProgressiveProtocol,
        UIProtocol,
    }ProtocolType;
    
	
	/// <summary> 
	/// Starts the specified protocol. 
	/// </summary>
	/// <param name="protocolType">The protocol that is to be started</param>
	virtual void StartProtocol(ProtocolType protocolType) = 0;
	    
	/// <summary> 
	/// Send message to the specified protocol's inbound queue. 
	/// </summary>
	/// <param name="protocolType">The protocol where the message is to be 
	/// sent</param> 
	/// <param name="msg">The message to be placed on the protocol's inbound 
	/// queue</param> 
	/// <param name="priorityLevel">The priority level for the message
    /// off the queue </param> 
    virtual void SendMessageToProtocol(ProtocolType protocolType, CInternalMessage *msg, int priorityLevel ) = 0; 
	
   /// <summary>
   /// Clears the has cumulative meter status. This is used when an account
   /// only progressive was configured, but then reconfigured to not be accounting only.
   /// </summary>
	/// <param name="protocolType">The protocol where the message is to be 
	/// sent</param> 
   virtual void RemoveProtocol(ProtocolType protocolType) = 0;
   
   /// <summary>
   /// Blocks until the protocol is started
   /// </summary>
   /// <param name="protocolType">The protocol where the message is to be 
   /// sent</param> 
   virtual void WaitForProtocol(ProtocolType protocolType) = 0;
   
   /// <summary> Clears the queue starvation for the specified protocol's inbound 
   /// queue. 
   /// </summary>
   /// <param name="protocolType">The protocol where the inbound starvation 
   /// is to be cleared</param> 
   /// <param name="persistType">The persist type that is to be cleared</param>
   virtual void ClearInboundQueueStarvation(ProtocolType protocolType, MessagePersistType persistType = PERSIST_NONE ) = 0;
   
   /// <summary> Sets whether the specified protocol's inbound queue is 
   /// interruptible by an equal priority. 
   /// </summary>
   /// <param name="protocolType">The protocol where the equal priority 
   /// flag is to be set</param> 
   /// <param name="isEqualPriorityInterruptible">true if messages are to interrupt
   /// messages of equal priority</param> 
   virtual void SetInboundQueueEqualPriorityInterrupt(ProtocolType protocolType, bool isEqualPriorityInterruptible) = 0;

  	/// <summary> Sets whether the specified protocol's inbound queue is ready
	/// </summary>
	/// <param name="protocolType">The protocol to check</param> 
	/// <returns>true if the protocolType is ready, false otherwise</returns>
	virtual bool InboundQueueForProtocolIsReady(ProtocolType protocolType) = 0;

    /// <summary> Determins whether the specified protocol's inbound
    /// queue is starved for the given MessagePersistType 
    /// </summary> 
    ///<param name="protocolType">The protocol to check</param> 
    /// <param name="persistType">The MessagePersistType to 
    /// check</param> 
    /// <returns>true if the queue is starved, false 
    /// otherwise</returns> 
    virtual bool IsQueueStarved(ProtocolType protocolType, MessagePersistType persistType) = 0;

    /// <summary> Determines whether the specified protocol is
    /// initialized
    /// </summary> 
    ///<param name="protocolType">The protocol to check</param>  
    /// <returns>true if the protocol is initialized, false 
    /// otherwise</returns> 
    virtual bool IsProtocolInitialized(ProtocolType protocolType) = 0;

	/// <summary>
    /// Flushes all persisted messages from the specified protocol's
    /// inbound queue, but leaves specified flush type messages. 
	/// </summary>
	/// <param name="protocolType">The protocol to flush</param> 
	/// <param name="flushTypeIDToNotFlush">The flush type to not flush</param>
    virtual void FlushAllPersistedButTypeFromInboundQueue(ProtocolType protocolType, int flushTypeIDToNotFlush) = 0;
};

