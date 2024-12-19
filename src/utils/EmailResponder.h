#ifndef EMAILRESPONDER_H

#include "EmailRetrieval.h"
#include <iostream>

#define EMAILRESPONDER_H


class EmailResponder {
public:
    EmailResponder();
    EmailResponder(const UserCredentials &user);
    ~EmailResponder();

    void setupCurl();
    void cleanUpCurl() const;
    void sendEmail(const std::string &to, const std::string &subject, const std::string &body);

private:
    CURL *curl;
    UserCredentials user_credentials;
    std::string payload;
    static size_t payloadSource(void *ptr, size_t size, size_t nmemb, void *userp);
};

#endif //EMAILRESPONDER_H
