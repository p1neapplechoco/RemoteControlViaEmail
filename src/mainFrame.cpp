#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/sizer.h>
#include <vector>
using namespace std;

enum class InputType {
  SingleLine,
  Multiline
};

MainFrame::MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE)
    : wxFrame(nullptr, wxID_ANY, TITLE, POS, SIZE) {

    const auto margin = FromDIP(10);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    // Top panel
    wxPanel* topPanel = new wxPanel(this, wxID_ANY);
    auto topSizer = new wxBoxSizer(wxHORIZONTAL);

    auto topLeftSizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto titleText = new wxStaticText(topPanel, wxID_ANY, "Email Remote Control");
    titleText->SetFont(titleFont);

    auto emailText = new wxStaticText(topPanel, wxID_ANY, "example@email.com");

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

        auto btn = new CustomBitmapButton(btnPanel, wxID_ANY, label);
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
    wxPanel* rightPanel = new wxPanel(this, wxID_ANY);
    rightPanel->SetBackgroundColour(wxColor(255, 255, 255));
    auto rightSizer = new wxBoxSizer(wxVERTICAL);

    // Form panel
    wxPanel *formPanel = new wxPanel(rightPanel, wxID_ANY);
    auto formSizer = new wxBoxSizer(wxVERTICAL);

    vector<pair<wxString, InputType>> form = {
        {"To:", InputType::SingleLine},
        {"Content:", InputType::Multiline}};

    for(const auto &value : form) {
        auto label = value.first;
        auto type = value.second;

        auto labelCtrl = new wxStaticText(formPanel, wxID_ANY, label);
        labelCtrl->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);
        formSizer->Add(labelCtrl, 0, wxLEFT, margin);

        auto style = type == InputType::SingleLine ? 0 : wxTE_MULTILINE;
        auto inputCtrl = new wxTextCtrl(formPanel, wxID_ANY, wxEmptyString,
            wxDefaultPosition,
            type == InputType::SingleLine ?
                wxSize(FromDIP(400), FromDIP(25)) :
                wxSize(FromDIP(400), FromDIP(200)),
            style);

        formSizer->Add(inputCtrl, type == InputType::Multiline ? 1 : 0,
            wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, margin);
    }

    formPanel->SetSizer(formSizer);
    rightSizer->Add(formPanel, 1, wxEXPAND | wxALL, margin);

    // Button panel ở dưới
    auto buttonPanel = new wxPanel(rightPanel, wxID_ANY);
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonSizer->AddStretchSpacer();  // Đẩy buttons sang phải

    auto cancelButton = new CustomBitmapButton(buttonPanel, wxID_ANY, "cancel");    // Cancel
    cancelButton->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);
    auto sendButton = new CustomBitmapButton(buttonPanel, wxID_ANY, "send");    // Send
    sendButton->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClick, this);

    buttonSizer->Add(cancelButton, 0, wxRIGHT, margin);
    buttonSizer->Add(sendButton, 0);

    buttonPanel->SetSizer(buttonSizer);
    rightSizer->Add(buttonPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, margin);

    rightPanel->SetSizer(rightSizer);
    contentSizer->Add(rightPanel, 1, wxEXPAND | wxALL, margin);

    mainSizer->Add(contentSizer, 1, wxEXPAND);
    this->SetSizer(mainSizer);

    this->SetMinSize(wxSize(FromDIP(800), FromDIP(600)));
}

void MainFrame::HighlightButton(wxPanel* selectedPanel) {
    // Reset màu của tất cả các panels về mặc định
    for (auto panel : buttonPanels) {
        panel->SetBackgroundColour(wxColor(240, 240, 240)); // Màu mặc định
        panel->Refresh();
    }

    // Set màu cho panel được chọn
    if (selectedPanel != nullptr) {
        selectedPanel->SetBackgroundColour(wxColor(200, 200, 200)); // Màu xám mờ
        selectedPanel->Refresh();
    }

    currentSelectedPanel = selectedPanel;
}