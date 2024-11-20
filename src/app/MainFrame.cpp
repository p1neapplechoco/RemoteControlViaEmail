#include "MainFrame.h"
using namespace std;

struct ButtonInfo {
    wxString label;
    int id;
};

vector<ButtonInfo> buttons = {
    {"KeyLogger", ID_KEY_LOGGER},
    {"CaptureScreen", ID_CAPTURE_SCREEN},
    {"CaptureWebcam", ID_CAPTURE_WEBCAM},
    {"MACAddress", ID_MAC_ADDRESS},
    {"DirectoryTree", ID_DIRECTORY_TREE},
    {"Process", ID_PROCESS},
    {"Registry", ID_REGISTRY},
    {"Logout", ID_LOGOUT}
};

RightPanel::RightPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    SetBackgroundColour(wxColor(255, 255, 255));

    const auto margin = FromDIP(10);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    logTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    mainSizer->Add(logTextCtrl, 1, wxEXPAND | wxALL, margin);

    // Bottom panel
    auto bottomPanel = new wxPanel(this);
    auto bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    // Phần keyLogger (sẽ được thêm vào từ CreateKeyLogger)
    keyLoggerPanel = new wxPanel(bottomPanel, wxID_ANY);
    keyLogoutPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(keyLoggerPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);
    bottomSizer->Add(keyLogoutPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();

    auto cancelButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "cancel");
    cancelButton->Bind(wxEVT_BUTTON, &RightPanel::OnCancelClick, this);
    auto sendButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "send");
    sendButton->Bind(wxEVT_BUTTON, &RightPanel::OnSendClick, this);

    buttonSizer->Add(cancelButton, 0, wxRIGHT, margin);
    buttonSizer->Add(sendButton, 0);

    bottomSizer->Add(buttonSizer, 1, wxEXPAND);
    bottomPanel->SetSizer(bottomSizer);

    mainSizer->Add(bottomPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, margin);
    SetSizer(mainSizer);

    CreateKeyLogger();
    CreateKeyLogout();
}

void RightPanel::CreateKeyLogger() {
    auto keyLoggerSizer = new wxBoxSizer(wxVERTICAL);

    auto timeLabel = new wxStaticText(keyLoggerPanel, wxID_ANY, "Time to log (ms):");
    timeInputCtrl = new wxTextCtrl(keyLoggerPanel, wxID_ANY, "1000",
        wxDefaultPosition, wxSize(200, -1), wxTE_LEFT | wxTE_RICH);
    timeInputCtrl->SetValidator(wxTextValidator(wxFILTER_DIGITS));

    keyLoggerSizer->Add(timeLabel, 0, wxBOTTOM, 5);
    keyLoggerSizer->Add(timeInputCtrl, 0);

    keyLoggerPanel->SetSizer(keyLoggerSizer);
    keyLoggerPanel->Hide();
}

void RightPanel::CreateKeyLogout() {
    auto keyLogoutSizer = new wxBoxSizer(wxVERTICAL);

    wxArrayString choices;
    choices.Add("Shutdown");
    choices.Add("Logout");

    auto Label = new wxStaticText(keyLogoutPanel, wxID_ANY, "Choose Shutdown/Logout:");
    choice = new wxChoice(keyLogoutPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);

    keyLogoutSizer->Add(Label, 0, wxBOTTOM, 5);
    keyLogoutSizer->Add(choice, 0, wxEXPAND | wxALL);

    keyLogoutPanel->SetSizer(keyLogoutSizer);
    keyLogoutPanel->Hide();
}

void RightPanel::UpdatePanelVisibility(int selectedPanel) {
    if (keyLoggerPanel) {
        keyLoggerPanel->Show(false);
    }

    if(keyLogoutPanel) {
        keyLogoutPanel->Show(false);
    }

    switch (selectedPanel) {
        case ID_KEY_LOGGER:
            keyLoggerPanel->Show(true);
        break;
        case ID_CAPTURE_SCREEN:
            // Show capture screen panel
                break;
        case ID_CAPTURE_WEBCAM:
            // Show webcam panel
                break;
        case ID_LOGOUT:
            keyLogoutPanel->Show(true);
        // Add cases for other panels
    }
    Layout();
}

void RightPanel::OnCancelClick(wxCommandEvent& event) {
    AppendLog("Operation cancelled\n");
}

void RightPanel::OnSendClick(wxCommandEvent& event) {
    AppendLog("Sending command...\n");
}

void RightPanel::AppendLog(const wxString& message) {
    logTextCtrl->AppendText(wxDateTime::Now().FormatTime() + ": " + message);
}

MainFrame::MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE, const wxString &currentEmail,
    const wxString &serverAddress, const wxString &portStr)
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

    auto emailText = new wxStaticText(topPanel, wxID_ANY, currentEmail);

    topLeftSizer->Add(titleText, 0, wxALIGN_CENTER_HORIZONTAL, 5);
    topLeftSizer->Add(emailText, 0);

    auto topRightSizer = new wxBoxSizer(wxHORIZONTAL);

    auto teamButton = new CustomBitmapButton(topPanel, ID_TEAM, "team");  // Team
    teamButton->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);
    auto helpButton = new CustomBitmapButton(topPanel, ID_INSTRUCTION, "instruction");  // Help
    helpButton->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);
    auto exitButton = new CustomBitmapButton(topPanel, ID_EXIT, "exit");  // Exit
    exitButton->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);

    topRightSizer->Add(teamButton, 0, wxRIGHT, margin);
    topRightSizer->Add(helpButton, 0, wxRIGHT, margin);
    topRightSizer->Add(exitButton, 0);

    topSizer->Add(topLeftSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, margin);
    topSizer->AddStretchSpacer();
    topSizer->Add(topRightSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, margin);

    topPanel->SetSizer(topSizer);
    mainSizer->Add(topPanel, 0, wxEXPAND);

    // Content panel
    auto contentSizer = new wxBoxSizer(wxHORIZONTAL);

    // Left panel với các buttons
    wxPanel* leftPanel = new wxPanel(this, wxID_ANY);
    auto leftSizer = new wxBoxSizer(wxVERTICAL);
    leftSizer->AddStretchSpacer();

    for (const auto& buttonInfo : buttons) {
        auto btnPanel = new wxPanel(leftPanel);
        btnPanel->SetBackgroundColour(wxColor(240, 240, 240)); // Màu mặc định

        auto btn = new CustomBitmapButton(btnPanel, buttonInfo.id, buttonInfo.label);
        btn->Bind(wxEVT_BUTTON, [this, btnPanel](wxCommandEvent& event) {
            HighlightButton(btnPanel);
            OnButtonClick(event);
        });

        auto btnSizer = new wxBoxSizer(wxHORIZONTAL);
        btnSizer->Add(btn, 1, wxALL, margin / 2);

        btnPanel->SetSizer(btnSizer);
        leftSizer->Add(btnPanel, 0, wxEXPAND | wxALL | wxALIGN_LEFT, margin / 2);

        buttonPanels.push_back(btnPanel); // Thêm vào vector
    }

    leftSizer->AddStretchSpacer();

    leftPanel->SetSizer(leftSizer);
    contentSizer->Add(leftPanel, 0, wxEXPAND | wxALL, margin);

    // Right panel với form
    rightPanel = new RightPanel(this);
    contentSizer->Add(rightPanel, 1, wxEXPAND | wxALL, margin);

    mainSizer->Add(contentSizer, 1, wxEXPAND);
    this->SetSizer(mainSizer);

    this->SetMinSize(wxSize(FromDIP(800), FromDIP(600)));

    if(ConnectToServer(currentEmail, serverAddress, portStr)) {
        rightPanel->AppendLog("Connect to server IP " + serverAddress + " is Successfully!! Wait for response!\n"
                                "------------------------------------------------------------------------------------\n");

    } else {
        rightPanel->AppendLog("Connect to server IP " + serverAddress + " is Unsuccessfully!! Try again!\n"
                                "------------------------------------------------------------------------------\n");
    }
}

bool MainFrame::ConnectToServer(const wxString &currentEmail, const wxString &serverAddress, const wxString &portStr) {
    if (!client.setupWSA())
    {
        std::cerr << "Failed to setup WSA" << std::endl;
        return false;
    }
    if (!client.setupSocket())
    {
        std::cerr << "Failed to setup socket" << std::endl;
        WSACleanup();
        return false;
    }

    long port;
    portStr.ToLong(&port);
    client.setServerPort(port);
    client.setServerIP(serverAddress.ToStdString());
    return client.connectToServer();
}

void MainFrame::HighlightButton(wxPanel* selectedPanel) {
    for (auto panel : buttonPanels) {
        panel->SetBackgroundColour(wxColor(240, 240, 240)); // Màu mặc định
        panel->Refresh();
    }

    if (selectedPanel != nullptr) {
        selectedPanel->SetBackgroundColour(wxColor(200, 200, 200)); // Màu xám mờ
        selectedPanel->Refresh();
    }

    currentSelectedPanel = selectedPanel;
}

void MainFrame::OnButtonClick(wxCommandEvent& evt) {
    int id = evt.GetId();

    // Handle specific button actions
    switch (id) {
        case ID_EXIT:
            Close();
            break;
        case ID_INSTRUCTION:

            break;
        case ID_TEAM: {
            wxFrame* imgFrame = new wxFrame(this, wxID_ANY, "Team", wxDefaultPosition, wxSize(578, 425));
            wxImage image("./assert/background/TeamRef.png");

            if (image.IsOk()) {
                wxStaticBitmap* staticBitmap = new wxStaticBitmap(imgFrame, wxID_ANY, wxBitmap(image));
            } else {
                wxMessageBox("Error loading image!", "Error", wxICON_ERROR);
            }
            imgFrame->Show();
            imgFrame->Center();
        }
            break;
        default:
            rightPanel->UpdatePanelVisibility(id);
    }

    evt.Skip();
}