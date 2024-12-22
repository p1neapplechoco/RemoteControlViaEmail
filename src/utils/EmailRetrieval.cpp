#include "EmailRetrieval.h"

// Don't touch this function
static size_t WriteCallback(void *contents, const size_t size, const size_t nmemb, void *userp)
{
    static_cast<std::string *>(userp)->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}

// Struct to handle the upload status
struct upload_status
{
    int lines_read;
    const char **payload_text;
};

// Function to read the payload
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    const char *data;
    size_t room = size * nmemb;

    data = upload_ctx->payload_text[upload_ctx->lines_read];

    if (data)
    {
        size_t len = strlen(data);
        if (len > room)
        {
            len = room;
        }
        memcpy(ptr, data, len);
        upload_ctx->lines_read++;
        return len;
    }

    return 0;
}
// Just don't

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

EmailRetrieval::EmailRetrieval() = default;

EmailRetrieval::EmailRetrieval(const UserCredentials &user)
{
    user_credentials = user;
};

EmailRetrieval::~EmailRetrieval() = default;

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

std::string EmailRetrieval::parseSender(const std::string &raw_mail)
{
    // std::cout << raw_mail << std::endl;
    std::istringstream iss(raw_mail);
    std::string line;
    std::string sender;
    while (std::getline(iss, line, '\n'))
    {
        if (line.find("From:") != std::string::npos)
        {
            size_t start = line.find('<');
            size_t end = line.find('>');
            if (start != std::string::npos && end != std::string::npos)
            {
                sender = line.substr(start + 1, end - start - 1);
            }
            break;
        }
    }
    return sender;
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

    if (!curl)
        setupCurl();

    const std::string url = "imaps://imap.gmail.com:993/INBOX/;UID=*"; // Fetch latest email

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &raw_mail);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        cleanUpCurl();
        return false;
    }

    mail_id = parseEmailID(raw_mail);
    mail_sender = parseSender(raw_mail);
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

std::string EmailRetrieval::getMailSender()
{
    return mail_sender;
}

void EmailRetrieval::respond(const char *to, const char *content, const char *attachment_path)
{
    const char *from = user_credentials.getUsername().c_str();
    const std::string ca_bundle_path = user_credentials.getCaBundlePath();

    CURL *curl_send = curl_easy_init();
    if (!curl_send)
    {
        std::cerr << "Failed to initialize CURL for sending." << std::endl;
        return;
    }

    curl_easy_setopt(curl_send, CURLOPT_URL, "smtp://smtp.gmail.com:587");
    curl_easy_setopt(curl_send, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    curl_easy_setopt(curl_send, CURLOPT_USERNAME, from);
    curl_easy_setopt(curl_send, CURLOPT_PASSWORD, user_credentials.getPassword().c_str());
    curl_easy_setopt(curl_send, CURLOPT_CAINFO, ca_bundle_path.c_str());

    struct curl_slist *recipients = nullptr;
    recipients = curl_slist_append(recipients, to);

    curl_easy_setopt(curl_send, CURLOPT_MAIL_FROM, from);
    curl_easy_setopt(curl_send, CURLOPT_MAIL_RCPT, recipients);

    curl_mime *mime = curl_mime_init(curl_send);

    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_data(part, content, CURL_ZERO_TERMINATED);
    curl_mime_type(part, "text/plain");

    if (attachment_path != NULL && attachment_path[0] != '\0')
    {
        part = curl_mime_addpart(mime);
        curl_mime_filedata(part, attachment_path);

        std::string file_extension = attachment_path;
        file_extension = file_extension.substr(file_extension.find_last_of('.') + 1);
        if (file_extension == "png")
        {
            curl_mime_type(part, "image/png");
        }
        else if (file_extension == "jpg" || file_extension == "jpeg")
        {
            curl_mime_type(part, "image/jpeg");
        }
        else if (file_extension == "pdf")
        {
            curl_mime_type(part, "application/pdf");
        }
        else
        {
            curl_mime_type(part, "application/octet-stream"); // Default for unknown file types
        }

        curl_mime_encoder(part, "base64"); // Encode the file as base64
        const std::string filename = std::string(attachment_path).substr(std::string(attachment_path).find_last_of('/') + 1);
        curl_mime_filename(part, filename.c_str()); // Optional: set the filename as seen by the recipient
    }

    // Attach the MIME structure to the CURL handle
    curl_easy_setopt(curl_send, CURLOPT_MIMEPOST, mime);

    // Send the email
    CURLcode res = curl_easy_perform(curl_send);

    if (res != CURLE_OK)
    {
        std::cerr << "curl_easy_perform() for sending failed: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        std::cout << "Email sent successfully with attachment!" << std::endl;
    }

    // Clean up the recipients list and CURL handle
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl_send);

    // Free the MIME structure
    curl_mime_free(mime);
}
