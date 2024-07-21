#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <vector>

enum class types {
    verification,
    message,
    null
};

struct header {
    size_t size;
    types messageType;

    header(size_t size) : size(size) {messageType = types::null;}
};

const size_t HEADER_SIZE = sizeof(header);

class Message {
public:

    header head;
    std::vector<uint8_t> body;

    void update() {body.resize(this->head.size);}

    Message();
    Message(size_t size) : head(size) {this->update();}
};

#endif //CHAT_MESSAGE_H
