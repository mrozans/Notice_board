#ifndef TIN_TABLICA_OGLOSZEN_REQUESTHANDLER_H
#define TIN_TABLICA_OGLOSZEN_REQUESTHANDLER_H


class RequestHandler {
private:
    int socket;
    static const unsigned int INPUT_MESSAGE_SIZE = 1024;
    //static const unsigned int OUTPUT_MESSAGE_SIZE = 1024;
    char input_message[INPUT_MESSAGE_SIZE]{};

public:
    explicit RequestHandler(int socket);

    void send_message(const char *message, size_t message_size) const;

    char *read_message();

    char *send_and_receive_message(const char *message, size_t message_size);

};
#endif //TIN_TABLICA_OGLOSZEN_REQUESTHANDLER_H
