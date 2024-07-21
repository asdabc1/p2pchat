#ifndef CHAT_MSGQ_H
#define CHAT_MSGQ_H

#include <deque>
#include <mutex>

#include "Message.h"

class MsgQ {
private:

    std::deque<Message> queue;

public:

    std::mutex mutex;
};


#endif //CHAT_MSGQ_H
