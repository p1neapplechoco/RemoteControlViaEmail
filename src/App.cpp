#include "App.h"
#include "MainFrame.h"
#include "LoginFrame.h"
#include <wx/wx.h>

bool App::OnInit() {
    wxInitAllImageHandlers();
    MainFrame* mainFrame = new MainFrame("C++ GUI", wxDefaultPosition, wxDefaultSize);
    mainFrame->Center();
    mainFrame->Show();
    return true;
}

wxIMPLEMENT_APP(App);