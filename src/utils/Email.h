#pragma once
#ifndef EMAIL_H

#include <string>
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include "IniParser.h"

#define EMAIL_H

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

    void setUsername(const std::string& user) { username = user; }
    void setPassword(const std::string& pass) { password = pass; }
};

class EMAIL
{
private:
    CURL *curl = nullptr;
    UserCredentials user_credentials;

    std::string mail_sender;
    std::string mail_content;
    std::string mail_id;

public:
    EMAIL();

    ~EMAIL();

    explicit EMAIL(const UserCredentials &user);

    void setupCurl();

    void cleanUpCurl() const;

    static std::string parseSender(const std::string& raw_mail);

    bool retrieveEmail();

    void respond(const char* to, const char* content, const char* attachment_path=nullptr);

    static std::string parseEmailContent(const std::string &raw_mail);

    static std::string parseEmailID(const std::string &raw_mail);

    std::string getMailContent();

    std::string getMailID();

    std::string getMailSender();
};

#endif // EMAIL_H
