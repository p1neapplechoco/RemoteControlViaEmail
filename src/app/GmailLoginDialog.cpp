#include "GmailLoginDialog.h"

GmailLoginDialog::GmailLoginDialog(wxWindow* parent)
        : wxDialog(parent, wxID_ANY, "Login with Google", wxDefaultPosition, wxDefaultSize) {
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    const auto margin = FromDIP(10);

    wxFont loginFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto loginText = new wxStaticText(this, wxID_ANY, "Google Login");
    loginText->SetFont(loginFont);
    mainSizer->Add(loginText, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, margin);

    // Gmail Controls
    auto gridSizer = new wxFlexGridSizer(2, 2, 5, 5); // 2 rows, 2 columns

    wxStaticText* emailLabel = new wxStaticText(this, wxID_ANY, "Username:");
    emailInput = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));

    wxStaticText* appPasswordLabel = new wxStaticText(this, wxID_ANY, "Password:");
    passwordInput = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1), wxTE_PASSWORD);

    loginButton = new CustomBitmapButton(this, wxID_ANY, "login");
    loginButton->Bind(wxEVT_BUTTON, &GmailLoginDialog::OnLogin, this);

    gridSizer->Add(emailLabel, 0, wxALIGN_RIGHT | wxALL, 5); // Align label to the right
    gridSizer->Add(emailInput, 1, wxEXPAND | wxALL, 5);     // TextCtrl expands to fill space
    gridSizer->Add(appPasswordLabel, 0, wxALIGN_RIGHT | wxALL, 5);
    gridSizer->Add(passwordInput, 1, wxEXPAND | wxALL, 5);
    gridSizer->AddGrowableCol(1);

    mainSizer->Add(gridSizer, 0, wxEXPAND | wxALL);
    mainSizer->Add(loginButton, 0, wxALIGN_RIGHT | wxALL, 10);

    SetSizer(mainSizer);
    Fit();
    Center();
}

bool GmailLoginDialog::AuthenticateUser(const wxString& email, const wxString& password) {
//    UserCredentials credentials;
//    credentials.loadCredentials();
//
//    credentials = UserCredentials();
//    credentials.setUsername(email.ToStdString());
//    credentials.setPassword(password.ToStdString());
//
//    try {
//        EmailRetrieval emailRetrieval(credentials);
//        emailRetrieval.setupCurl();
//
//        emailRetrieval.retrieveEmail();
//
//        emailRetrieval.cleanUpCurl();
//
//        return true;
//    }
//    catch (const std::exception& e) {
//        return false;
//    }
    return true;
}

void GmailLoginDialog::OnLogin(wxCommandEvent& evt) {
    wxString emailStr = emailInput->GetValue();
    wxString passwordStr = passwordInput->GetValue();

    if(emailStr.IsEmpty() || passwordStr.IsEmpty()) {
        wxMessageBox("Please enter both email and password!", "Error",
                    wxOK | wxICON_ERROR);
        return;
    }

    loginButton->Enable(false);

    wxProgressDialog progress("Connecting...", "Authenticating with Google Account...",
                            100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE);
    progress.Pulse();

    bool authenticated = AuthenticateUser(emailStr, passwordStr);

    if (authenticated) {
        loginSuccessful = true;
        email = emailStr;
        EndModal(wxID_OK);
    } else {
        wxMessageBox("Login failed. Please check your credentials.", "Error",
                    wxOK | wxICON_ERROR);
        loginSuccessful = false;
    }

    loginButton->Enable(true);
    progress.Hide();
}