#include "../EmailRetrieval.h"
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
    while (contents_of_mails.size() < 5)
    {
        emailRetrieval.retrieveEmail();
        if (prev_mail_id != emailRetrieval.getMailID())
        {
            std::cout << emailRetrieval.getMailContent() << std::endl;
            contents_of_mails.push_back(emailRetrieval.getMailContent());
            prev_mail_id = emailRetrieval.getMailID();
        }
        Sleep(1000);
    }
    for (std::string mail : contents_of_mails)
    {
        std::cout << mail << std::endl;
    }
    return 1;
}
