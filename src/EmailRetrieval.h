#pragma once
#ifndef EMAILRETRIEVAL_H

#include <string>
#include <curl/curl.h>

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
    EmailRetrieval();

    explicit EmailRetrieval(const UserCredentials &user);


    void setupCurl();

    void cleanUpCurl() const;

    void retrieveEmail();

    static std::string parseEmailContent(const std::string &raw_mail);

    static std::string parseEmailID(const std::string &raw_mail);

    std::string getMailContent();

    std::string getMailID();
};

#endif //EMAILRETRIEVAL_H
