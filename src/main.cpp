#include "./utils/EmailRetrieval.h"
#include <iostream>
#include <vector>

// g++ EmailRetrievalUsage.cpp ../EmailRetrievalUsage.cpp -o EmailRetrievalUsage; ./EmailRetrievalUsage

int main()
{
    UserCredentials user;
    user.loadCredentials();
    EmailRetrieval emailRetrieval(user);

    emailRetrieval.setupCurl();

    std::vector<std::string> contents_of_mails;

    std::string prev_mail_id = " ";
    std::string sender = " ";
    while (contents_of_mails.size() < 5)
    {
        emailRetrieval.retrieveEmail();
        if (prev_mail_id != emailRetrieval.getMailID())
        {
            std::cout << emailRetrieval.getMailContent() << std::endl;
            contents_of_mails.push_back(emailRetrieval.getMailContent());
            prev_mail_id = emailRetrieval.getMailID();
            sender = emailRetrieval.getMailSender();

            emailRetrieval.respond(sender.c_str(), "dit me may", "test.txt");
        }
        Sleep(1000);
    }

    for (const std::string& mail : contents_of_mails)
    {
        std::cout << mail << std::endl;
    }
    return 1;
}

