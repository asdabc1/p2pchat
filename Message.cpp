#include "Message.h"

std::ostream &operator<<(std::ostream &os, Message &msg) {
    std::string temp;
    temp.resize(msg.body.size());

    memcpy(temp.data(), msg.body.data(), msg.body.size());

    os << temp;
    return os;
}

Message& Message::operator<<(std::string input) {
    head.size = input.size();
    head.messageType = types::message;
    update();

    memcpy(body.data(), input.data(), head.size);
    return *this;
}
