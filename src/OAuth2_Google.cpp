#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

class GoogleAuth {
private:
    const std::string redirect_uri = "http://localhost:8080";
    const std::string auth_scope = "https://www.googleapis.com/auth/userinfo.profile email";
    std::string access_token;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    std::string urlEncode(const std::string& str) {
        CURL* curl = curl_easy_init();
        char* encoded = curl_easy_escape(curl, str.c_str(), str.length());
        std::string result(encoded);
        curl_free(encoded);
        curl_easy_cleanup(curl);
        return result;
    }

public:
    bool getAuthorizationCode() {
        std::string auth_url = "https://accounts.google.com/o/oauth2/v2/auth?"
            "client_id=" + client_id +
            "&redirect_uri=" + urlEncode(redirect_uri) +
            "&response_type=code" +
            "&scope=" + urlEncode(auth_scope) +
            "&access_type=offline" +
            "&prompt=consent";

        std::cout << "Please visit this URL to authorize the application:\n" << auth_url << std::endl;
        std::cout << "Enter the authorization code: ";
        std::string auth_code;
        std::getline(std::cin, auth_code);

        return getAccessToken(auth_code);
    }

    bool getAccessToken(const std::string& auth_code) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to initialize CURL" << std::endl;
            return false;
        }

        std::string post_data = "code=" + auth_code +
            "&client_id=" + client_id +
            "&client_secret=" + client_secret +
            "&redirect_uri=" + redirect_uri +
            "&grant_type=authorization_code";

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to get access token: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        try {
            auto json_response = json::parse(response);
            if (json_response.contains("access_token")) {
                access_token = json_response["access_token"].get<std::string>();
                return true;
            }
        } catch (const json::parse_error& e) {
            std::cerr << "Failed to parse response: " << e.what() << std::endl;
        }

        return false;
    }

    bool getUserInfo() {
        if (access_token.empty()) {
            std::cerr << "No access token available" << std::endl;
            return false;
        }

        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to initialize CURL" << std::endl;
            return false;
        }

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/oauth2/v1/userinfo");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to get user info: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        try {
            auto user_info = json::parse(response);
            std::cout << "User Info:" << std::endl;
            if (user_info.contains("email")) {
                std::cout << "Email: " << user_info["email"].get<std::string>() << std::endl;
            }
            if (user_info.contains("name")) {
                std::cout << "Name: " << user_info["name"].get<std::string>() << std::endl;
            }
            return true;
        } catch (const json::parse_error& e) {
            std::cerr << "Failed to parse user info: " << e.what() << std::endl;
        }

        return false;
    }
};

int main() {
    curl_global_init(CURL_GLOBAL_ALL);

    GoogleAuth auth;

    if (auth.getAuthorizationCode()) {
        std::cout << "Successfully authenticated!" << std::endl;
        auth.getUserInfo();
    } else {
        std::cout << "Authentication failed!" << std::endl;
    }

    curl_global_cleanup();
    return 0;
}
