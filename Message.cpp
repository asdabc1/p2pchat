#include "Message.h"

std::ostream &operator<<(std::ostream &os, Message &msg) {
    std::string temp;
    temp.resize(msg.body.size());

    memcpy(temp.data(), msg.body.data(), msg.body.size());

    os << temp;
    return os;
}
