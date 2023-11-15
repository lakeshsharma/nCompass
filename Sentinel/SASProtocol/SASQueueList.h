#pragma once
#include "QueueList.h"

class CSASPollMessage;

class CSASQueueList : public CQueueList
{

public:
   /// <summary>
   ///  Standard constructor. Initializes a new instance of the
   ///  <see cref="CSASQueueList"/> class. Use this constructor
   ///  when the item-added event handle is to be created by this
   ///  class.
    /// </summary>
    /// <param name="createItemAddedEventHandle">IN - true if the item-added event is to be created, false otherwise.</param>
    /// <param name="logIdentifier">IN - The identifier used for logging, nullptr indicates no logging.</param>
    CSASQueueList(const std::string& logIdentifier = "");

    /// <summary>
   /// Adds a queue item to the list.
   /// </summary>
   /// <param name="item">IN - The queue item.</param>
   /// <param name="priorityLevel">IN - The priority level for the queue item.</param>
   /// <returns>true if the queue item was added to the list, false otherwise.</returns>
   bool Add(CSASPollMessage *item, int priorityLevel);

   bool ForceRemoveItem(CSASPollMessage *item, int priorityLevel);

};
