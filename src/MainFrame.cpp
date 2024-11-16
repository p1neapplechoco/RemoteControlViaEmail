#include "MainFrame.h"
using namespace std;

enum {
    ID_KEY_LOGGER = wxID_HIGHEST + 1
};

RightPanel::RightPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
    SetBackgroundColour(wxColor(255, 255, 255));

    const auto margin = FromDIP(10);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    logTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    mainSizer->Add(logTextCtrl, 1, wxEXPAND | wxALL, margin);

    // Bottom buttons
    auto buttonPanel = new wxPanel(this);
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonSizer->AddStretchSpacer();  // Đẩy buttons sang phải

    auto cancelButton = new CustomBitmapButton(buttonPanel, wxID_ANY, "cancel");    // Cancel
    cancelButton->Bind(wxEVT_BUTTON, &RightPanel::OnCancelClick, this);
    auto sendButton = new CustomBitmapButton(buttonPanel, wxID_ANY, "send");    // Send
    sendButton->Bind(wxEVT_BUTTON, &RightPanel::OnSendClick, this);

    buttonSizer->Add(cancelButton, 0, wxRIGHT, margin);
    buttonSizer->Add(sendButton, 0);

    buttonPanel->SetSizer(buttonSizer);
    mainSizer->Add(buttonPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, margin);

    // Create buttons
    SetSizer(mainSizer);
    CreateKeyLogger();
}

void RightPanel::CreateKeyLogger() {
    keyLoggerPanel = new wxPanel(this, wxID_ANY);
    auto keyLoggerSizer = new wxBoxSizer(wxVERTICAL);

    timeInputSizer = new wxBoxSizer(wxVERTICAL);
    const auto margin = FromDIP(10);

    timeLabel = new wxStaticText(keyLoggerPanel, wxID_ANY, "Time to log in milliseconds");
    timeInputCtrl = new wxTextCtrl(keyLoggerPanel, wxID_ANY, "1000",
        wxDefaultPosition, wxSize(200, -1), wxTE_RIGHT);

    timeInputCtrl->SetValidator(wxTextValidator(wxFILTER_DIGITS));

    timeInputSizer->Add(timeLabel, 0, wxBOTTOM, 5);
    timeInputSizer->Add(timeInputCtrl, 0);

    keyLoggerSizer->Add(timeInputSizer, 0, wxALIGN_LEFT | wxALL, margin * 2);

    keyLoggerPanel->SetSizer(keyLoggerSizer);
    GetSizer()->Add(keyLoggerPanel, 1, wxEXPAND);
    keyLoggerPanel->Hide();
}

void RightPanel::UpdatePanelVisibility(int selectedPanel) {
    if (keyLoggerPanel) {
        keyLoggerPanel->Show(selectedPanel == ID_KEY_LOGGER);
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

MainFrame::MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE, const wxString &currentEmail, const wxString &serverAddress)
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

    auto teamButton = new CustomBitmapButton(topPanel, wxID_ANY, "team");  // Team
    teamButton->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);
    auto helpButton = new CustomBitmapButton(topPanel, wxID_ANY, "instruction");  // Help
    helpButton->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);
    auto exitButton = new CustomBitmapButton(topPanel, wxID_ANY, "exit");  // Exit
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

    vector<wxString> buttons = {
        "KeyLogger",
        "CaptureScreen",
        "CaptureWebcam",
        "MACAddress",
        "DirectoryTree",
        "Process",
        "Registry",
        "Logout"
    };

    for (const auto& label : buttons) {
        auto btnPanel = new wxPanel(leftPanel);
        btnPanel->SetBackgroundColour(wxColor(240, 240, 240)); // Màu mặc định

        auto btn = new CustomBitmapButton(btnPanel, ID_KEY_LOGGER, label);
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
    wxMessageBox(selectedPanel);
}

void MainFrame::OnButtonClick(wxCommandEvent& evt) {
    currentSelectedPanel = evt.GetId();
    rightPanel->UpdatePanelVisibility(currentSelectedPanel);
    evt.Skip();
}