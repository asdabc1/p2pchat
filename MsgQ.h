#ifndef CHAT_MSGQ_H
#define CHAT_MSGQ_H

#include <deque>
#include <mutex>

#include "Message.h"

class MsgQ {
public:

    std::deque<Message> queue;
    std::mutex mutex;

    std::vector<Message> awaitingMsgs;

    void addToQueue(Message& msg);
    Message getFromQueue();
    void dumpAwaiting();
};


#endif //CHAT_MSGQ_H
