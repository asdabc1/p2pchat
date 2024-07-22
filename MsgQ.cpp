#include "MsgQ.h"

void MsgQ::addToQueue(Message& msg) {
    if (mutex.try_lock()) {
        queue.emplace_back(msg);

        if (!awaitingMsgs.empty()) {
            for (Message& m : awaitingMsgs)
                queue.emplace_back(m);

            awaitingMsgs.clear();
        }
    }

    else
        awaitingMsgs.push_back(msg);
}

void MsgQ::dumpAwaiting() {
    while (!mutex.try_lock()) {}

    for (Message& m : awaitingMsgs)
        queue.emplace_back(m);

    awaitingMsgs.clear();
}