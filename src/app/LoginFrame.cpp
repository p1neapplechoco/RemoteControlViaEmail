#include "LoginFrame.h"
#include "App.h"
using namespace std;

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
    UserCredentials credentials;
    credentials.loadCredentials();

    credentials = UserCredentials();
    credentials.setUsername(email.ToStdString());
    credentials.setPassword(password.ToStdString());

    try {
        EmailRetrieval emailRetrieval(credentials);
        emailRetrieval.setupCurl();

        emailRetrieval.retrieveEmail();

        emailRetrieval.cleanUpCurl();

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
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

LoginFrame::LoginFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE)
    : wxFrame(nullptr, wxID_ANY, TITLE, POS, SIZE) {

    const auto margin = FromDIP(10);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    // Top panel
    wxPanel* topPanel = new wxPanel(this, wxID_ANY);
    auto topSizer = new wxBoxSizer(wxHORIZONTAL);

    auto topLeftSizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto titleText = new wxStaticText(topPanel, wxID_ANY, "Remote Control Via Email");
    titleText->SetFont(titleFont);

    topLeftSizer->Add(titleText, 0, wxALIGN_CENTER_HORIZONTAL, 5);
    auto topRightSizer = new wxBoxSizer(wxHORIZONTAL);

    auto teamButton = new CustomBitmapButton(topPanel, wxID_ANY, "team");
    teamButton->Bind(wxEVT_BUTTON, &LoginFrame::OnButtonClick, this);
    auto helpButton = new CustomBitmapButton(topPanel, wxID_ANY, "instruction");
    helpButton->Bind(wxEVT_BUTTON, &LoginFrame::OnButtonClick, this);

    topRightSizer->Add(teamButton, 0, wxRIGHT, margin);
    topRightSizer->Add(helpButton, 0, wxRIGHT, margin);

    topSizer->Add(topLeftSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, margin);
    topSizer->AddStretchSpacer();
    topSizer->Add(topRightSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, margin);

    topPanel->SetSizer(topSizer);
    mainSizer->Add(topPanel, 0, wxEXPAND);

    // Add separator
    mainSizer->AddSpacer(margin);

    // Bottom panel
    bottomPanel = new wxPanel(this, wxID_ANY);
    bottomSizer = new wxBoxSizer(wxVERTICAL);

    // Google Login Panel
    googlePanel = new wxPanel(bottomPanel, wxID_ANY);
    auto googleSizer = new wxBoxSizer(wxVERTICAL);

    wxFont loginFont(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto welcomeText = new wxStaticText(googlePanel, wxID_ANY, "Welcome to RCVE");
    welcomeText->SetFont(loginFont);
    googleSizer->Add(welcomeText, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, margin * 2);

    auto noAccLoginBtn = new CustomBitmapButton(googlePanel, wxID_ANY, "LoginNoAccount");
    auto googleLoginBtn = new CustomBitmapButton(googlePanel, wxID_ANY, "LoginWithGoogle");

    noAccLoginBtn->Bind(wxEVT_BUTTON, &LoginFrame::OnGoogleLogin, this);
    googleLoginBtn->Bind(wxEVT_BUTTON, &LoginFrame::OnGoogleLogin, this);

    googleSizer->Add(noAccLoginBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, margin);
    googleSizer->Add(googleLoginBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, margin);
    googlePanel->SetSizer(googleSizer);

    // IP Selection Panel
    ipPanel = new wxPanel(bottomPanel, wxID_ANY);
    auto ipPanelSizer = new wxBoxSizer(wxVERTICAL);

    auto ipTitleText = new wxStaticText(ipPanel, wxID_ANY, "Server Connection");
    ipTitleText->SetFont(loginFont);
    ipPanelSizer->Add(ipTitleText, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, margin * 2);

    auto ipSizer = new wxBoxSizer(wxHORIZONTAL);
    auto ipLabel = new wxStaticText(ipPanel, wxID_ANY, "Choose IP address:");
    ipComboBox = new wxComboBox(ipPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                               0, NULL, wxCB_DROPDOWN);
    auto refreshButton = new CustomBitmapButton(ipPanel, wxID_ANY, "refresh");

    ipSizer->Add(ipLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);
    ipSizer->Add(ipComboBox, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);
    ipSizer->Add(refreshButton, 0, wxALIGN_CENTER_VERTICAL);

    ipPanelSizer->Add(ipSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, margin * 20);

    // Navigation Buttons
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    auto backButton = new CustomBitmapButton(ipPanel, wxID_ANY, "back");
    auto connectButton = new CustomBitmapButton(ipPanel, wxID_ANY, "connect");

    buttonSizer->Add(backButton, 0, wxRIGHT, margin);
    buttonSizer->Add(connectButton);

    ipPanelSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, margin);

    ipPanel->SetSizer(ipPanelSizer);
    ipPanel->Hide(); // Initially hide IP panel

    // Add both panels to bottom sizer
    bottomSizer->AddStretchSpacer(1);
    bottomSizer->Add(googlePanel, 0, wxALIGN_CENTER);
    bottomSizer->Add(ipPanel, 0, wxALIGN_CENTER);
    bottomSizer->AddStretchSpacer(1);

    bottomPanel->SetSizer(bottomSizer);
    mainSizer->Add(bottomPanel, 1, wxEXPAND);

    // Bind events
    backButton->Bind(wxEVT_BUTTON, &LoginFrame::OnBackClick, this);
    connectButton->Bind(wxEVT_BUTTON, &LoginFrame::OnConnectClick, this);
    refreshButton->Bind(wxEVT_BUTTON, &LoginFrame::OnRefreshClick, this);
    noAccLoginBtn->Bind(wxEVT_BUTTON, &LoginFrame::OnNoAccountLogin, this);

    this->SetSizer(mainSizer);
    this->SetMinSize(wxSize(FromDIP(800), FromDIP(600)));
}

void LoginFrame::UpdateIPList() {
    wxProgressDialog progress("Scanning Network", "Searching for available servers...",
                            100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE);
    progress.Pulse();

    try {
        ipComboBox->Clear();

        vector<string> ipAddresses = client.scanIP();

        for(const auto& ip : ipAddresses) {
            ipComboBox->Append(wxString(ip));
        }

        if(ipAddresses.empty()) {
            wxMessageBox("No servers found on the network", "Information",
                        wxOK | wxICON_INFORMATION);
        } else {
            ipComboBox->SetSelection(0);
        }
    }
    catch (const exception& e) {
        wxMessageBox(wxString(e.what()), "Error", wxOK | wxICON_ERROR);
    }

    progress.Hide();
}

void LoginFrame::OnRefreshClick(wxCommandEvent& evt) {
    UpdateIPList();
}

void LoginFrame::ShowGooglePanel() {
    ipPanel->Hide();
    googlePanel->Show();
    bottomPanel->Layout();
}

void LoginFrame::ShowIPPanel() {
    googlePanel->Hide();
    ipPanel->Show();
    bottomPanel->Layout();
}

void LoginFrame::OnNoAccountLogin(wxCommandEvent& evt) {
    currentEmail = "tester@gmail.com";
    ShowIPPanel();
    UpdateIPList();
}

void LoginFrame::OnBackClick(wxCommandEvent& evt) {
    ShowGooglePanel();
}

void LoginFrame::OnGoogleLogin(wxCommandEvent& evt) {
    GmailLoginDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK && dialog.IsLoginSuccessful()) {
        currentEmail = dialog.GetEmail();
        ShowIPPanel();
        UpdateIPList();
    }
}

void LoginFrame::OnConnectClick(wxCommandEvent& evt) {
    if (ipComboBox->GetValue().IsEmpty()) {
        wxMessageBox("Please select a server IP address", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // MainFrame* mainFrame = new MainFrame("Remote Control Via Email", wxDefaultPosition, wxDefaultSize, currentEmail, serverAddress);
    // mainFrame->Fit();
    // mainFrame->Center();
    // mainFrame->Show();
}