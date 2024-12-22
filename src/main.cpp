#include <wx/wx.h>
#include "utils/Client.h"
#include "app/App.h"

#pragma comment(lib, "ws2_32.lib")

bool App::OnInit() {
    wxInitAllImageHandlers();
    LoginFrame *loginFrame = new LoginFrame("Remote Control Via Desktop", wxDefaultPosition, wxDefaultSize);
    loginFrame->Fit();
    loginFrame->Center();
    loginFrame->Show();
    return true;
}

wxIMPLEMENT_APP(App);