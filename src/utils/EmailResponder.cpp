#include "EmailResponder.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include "IniParser.h"
#include <cstring>

static size_t WriteCallback(void *contents, const size_t size, const size_t nmemb, void *userp)
{
    static_cast<std::string *>(userp)->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}

EmailResponder::EmailResponder() = default;

EmailResponder::EmailResponder(const UserCredentials &user) {
    user_credentials = user;
};

EmailResponder::~EmailResponder() {
    cleanUpCurl();
}

void EmailResponder::setupCurl() {
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

void EmailResponder::cleanUpCurl() const {
    curl_easy_cleanup(curl);
}

size_t EmailResponder::payloadSource(void *ptr, size_t size, size_t nmemb, void *userp) {
    auto upload_ctx = static_cast<std::string*>(userp);
    size_t curl_size = size * nmemb;
    size_t copy_size = std::min(upload_ctx->size(), curl_size);

    std::memcpy(ptr, upload_ctx->c_str(), copy_size);
    upload_ctx->erase(0, copy_size);

    return copy_size;
}

void EmailResponder::sendEmail(const std::string &to, const std::string &subject, const std::string &body) {
    if (!curl) {
        setupCurl();
    }

    payload = "To: " + to + "\r\n" +
              "From: " + user_credentials.getUsername() + "\r\n" +
              "Subject: " + subject + "\r\n" +
              "\r\n" +
              body + "\r\n";

    curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + user_credentials.getUsername() + ">").c_str());

    struct curl_slist *recipients = nullptr;
    recipients = curl_slist_append(recipients, ("<" + to + ">").c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payloadSource);
    curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL); // Use SSL/TLS
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // Enable verbose mode for debugging

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_slist_free_all(recipients);
    cleanUpCurl();
}
