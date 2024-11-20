#include "./app/App.h"
#include "./utils/Client.h"
#include "./utils/Server.h"
#include "./utils/GetWinDirectory.h"

bool App::OnInit() {
    wxInitAllImageHandlers();
    // MainFrame* mainFrame = new MainFrame("Remote Control Via Desktop", wxDefaultPosition, wxDefaultSize, "0", "0");
    // mainFrame->Fit();
    // mainFrame->Center();
    // mainFrame->Show();

    LoginFrame* loginFrame = new LoginFrame("Remote Control Via Desktop", wxDefaultPosition, wxDefaultSize);
    loginFrame->Fit();
    loginFrame->Center();
    loginFrame->Show();

    return true;
}

wxIMPLEMENT_APP(App);

// int main()
// {
//     // Client client;
//     // client.startClient();
//
//     //Server server;
//
//     return 0;
// }