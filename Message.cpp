#include "Message.h"

Message& Message::operator<<(std::string input) {
    head.size = input.size();
    head.messageType = types::message;
    update();

    memcpy(body.data(), input.data(), head.size);
    return *this;
}

std::string Message::string() const {
    std::string val;
    val.resize(head.size);

    memcpy(val.data(), body.data(), head.size);
    return val;
}

Message::Message(types type, uint8_t val) {
    head.messageType = type;
    body.push_back(val);
    head.size = body.size();
}

bool Message::operator==(const Message& otherMsg) const {
    return this->head.messageType == otherMsg.head.messageType && this->head.size && otherMsg.head.size && this->body == otherMsg.body;
}