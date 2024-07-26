#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <vector>
#include <iostream>
#include <string>

enum class types {
    verification,
    message,
    null
};

struct header {
    size_t size;
    types messageType;

    explicit header(size_t size) : size(size) {messageType = types::null;}
    header() : size(0), messageType(types::null) {}
};

const size_t HEADER_SIZE = sizeof(header);

class Message {
public:

    header head;
    std::vector<uint8_t> body;

    void update() {body.resize(this->head.size);}

    Message() = default;
    explicit Message(size_t size) : head(size) {this->update();}

    friend std::ostream& operator<<(std::ostream& os, Message& msg);
    Message& operator<<(std::string input);
};

#endif //CHAT_MESSAGE_H
