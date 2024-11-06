#pragma once
#ifndef EMAILRETRIEVAL_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <sstream>

#define EMAILRETRIEVAL_H

class UserCredentials
{
private:
    std::string username;
    std::string password;
    std::string ca_bundle_path;

public:
    void loadCredentials();

    std::string getUsername();

    std::string getPassword();

    std::string getCaBundlePath();
};

class EmailRetrieval
{
private:
    CURL *curl = nullptr;
    UserCredentials user_credentials;
    std::string mail_content;
    std::string mail_id;

public:
    explicit EmailRetrieval(const UserCredentials &user)
    {
        user_credentials = user;
    };

    void setupCurl();

    void cleanUpCurl() const;

    void retrieveEmail();

    static std::string parseEmailContent(const std::string &raw_mail);

    static std::string parseEmailID(const std::string &raw_mail);

    std::string getMailContent();

    std::string getMailID();
};

#endif //EMAILRETRIEVAL_H
