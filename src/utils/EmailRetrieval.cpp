#include "EmailRetrieval.h"
#include <iostream>
#include <sstream>
#include "IniParser.h"

// Don't touch this function
static size_t WriteCallback(void *contents, const size_t size, const size_t nmemb, void *userp)
{
    static_cast<std::string *>(userp)->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}

void UserCredentials::loadCredentials()
{
    IniParser parser("../config.ini");
    username = parser.get("Credentials", "username");
    password = parser.get("Credentials", "password");
    ca_bundle_path = parser.get("MISCs", "ca_bundle_path");
}

std::string UserCredentials::getUsername()
{
    return username;
}

std::string UserCredentials::getPassword()
{
    return password;
}

std::string UserCredentials::getCaBundlePath()
{
    return ca_bundle_path;
}

EmailRetrieval::EmailRetrieval()
= default;

EmailRetrieval::EmailRetrieval(const UserCredentials &user)
{
    user_credentials = user;
};

EmailRetrieval::~EmailRetrieval()
= default;


void EmailRetrieval::setupCurl()
{
    curl = curl_easy_init();
    const std::string username = user_credentials.getUsername();
    const std::string password = user_credentials.getPassword();
    const std::string ca_bundle_path = user_credentials.getCaBundlePath();

    curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    // Configure SSL
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, ca_bundle_path.c_str());
    curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);

    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // For debugging purpose
}

void EmailRetrieval::cleanUpCurl() const
{
    curl_easy_cleanup(curl);
}

std::string EmailRetrieval::parseEmailID(const std::string &raw_mail)
{
    std::istringstream iss(raw_mail);
    std::string line;
    std::string messageID;
    while (std::getline(iss, line))
    {
        if (line.find("Message-ID:") != std::string::npos)
        {
            size_t start = line.find('<');
            size_t end = line.find('>');
            if (start != std::string::npos && end != std::string::npos)
            {
                messageID = line.substr(start, end - start + 1);
            }
            break;
        }
    }
    return messageID;
}

std::string EmailRetrieval::parseEmailContent(const std::string &raw_mail)
{
    std::istringstream iss(raw_mail);
    std::string line;
    std::string content;
    bool isTextPlain = false;
    bool isTextHtml = false;

    while (std::getline(iss, line))
    {
        if (line.find("Content-Type: text/plain") != std::string::npos)
        {
            isTextPlain = true;
            isTextHtml = false;
            continue;
        }
        if (line.find("Content-Type: text/html") != std::string::npos)
        {
            isTextPlain = false;
            isTextHtml = true;
            continue;
        }
        if (line.find("--") == 0)
        {
            // Stop at boundary lines
            if (isTextPlain || isTextHtml)
            {
                break;
            }
        }
        if (isTextPlain || isTextHtml)
        {
            content += line;
        }
    }
    return content;
}

bool EmailRetrieval::retrieveEmail()
{
    std::string raw_mail;

    const std::string url = "imaps://imap.gmail.com:993/INBOX;UID=*";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &raw_mail);

    const CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        cleanUpCurl();
        return false;
    }

    mail_id = parseEmailID(raw_mail);
    mail_content = parseEmailContent(raw_mail);
    return true;
}

std::string EmailRetrieval::getMailContent()
{
    return mail_content;
}

std::string EmailRetrieval::getMailID()
{
    return mail_id;
}
