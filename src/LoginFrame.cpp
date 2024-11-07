#include "LoginFrame.h"
#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/statline.h>

LoginFrame::LoginFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE)
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

    // Add separator between panels
    mainSizer->AddSpacer(margin);

    // Bottom panel
    wxPanel* bottomPanel = new wxPanel(this, wxID_ANY);
    auto bottomSizer = new wxBoxSizer(wxVERTICAL);

    // Center content vertically
    bottomSizer->AddStretchSpacer(1);

    // Login title with reduced margin
    wxFont loginFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto loginText = new wxStaticText(bottomPanel, wxID_ANY, "Login");
    loginText->SetFont(loginFont);
    bottomSizer->Add(loginText, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, margin * 2);

    // IP address selection
    auto ipSizer = new wxBoxSizer(wxHORIZONTAL);
    auto ipLabel = new wxStaticText(bottomPanel, wxID_ANY, "Choose IP address:");
    auto ipComboBox = new wxComboBox(bottomPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                                    0, NULL, wxCB_DROPDOWN);
    auto refreshButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "refresh");

    ipSizer->Add(ipLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);
    ipSizer->Add(ipComboBox, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);
    ipSizer->Add(refreshButton, 0, wxALIGN_CENTER_VERTICAL);

    bottomSizer->Add(ipSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, margin * 20);

    // Buttons with reduced top spacing
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    auto backButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "back");
    auto connectButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "connect");

    buttonSizer->Add(backButton, 0, wxRIGHT, margin);
    buttonSizer->Add(connectButton);

    bottomSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxLEFT | wxRIGHT, margin);

    // Bottom spacing
    bottomSizer->AddStretchSpacer(1);

    bottomPanel->SetSizer(bottomSizer);
    mainSizer->Add(bottomPanel, 1, wxEXPAND);

    // Bind events
    backButton->Bind(wxEVT_BUTTON, &LoginFrame::OnButtonClick, this);
    connectButton->Bind(wxEVT_BUTTON, &LoginFrame::OnButtonClick, this);
    refreshButton->Bind(wxEVT_BUTTON, &LoginFrame::OnButtonClick, this);

    this->SetSizer(mainSizer);
    this->SetMinSize(wxSize(FromDIP(800), FromDIP(600)));
}

/*
LoginFrame::LoginFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE)
    : wxFrame(nullptr, wxID_ANY, TITLE, POS, SIZE) {

    SetBackgroundColour(wxColour(40, 40, 40));

    wxPanel* panel = new wxPanel(this);
    panel->SetBackgroundColour(wxColour(40, 40, 40));

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Title
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, "Remote Control with Email Service");
    title->SetForegroundColour(wxColour(200, 200, 200));
    mainSizer->Add(title, 0, wxALIGN_CENTER | wxTOP, 20);

    // Instruction
    wxStaticText* instruction = new wxStaticText(panel, wxID_ANY, "Input the server's IP address and Port in the box below to connect");
    instruction->SetForegroundColour(wxColour(200, 200, 200));
    mainSizer->Add(instruction, 0, wxALIGN_CENTER | wxTOP, 10);

    // Connection time note
    wxStaticText* timeNote = new wxStaticText(panel, wxID_ANY, "It may takes up to 15 seconds to connect");
    timeNote->SetForegroundColour(wxColour(200, 200, 200));
    mainSizer->Add(timeNote, 0, wxALIGN_CENTER | wxTOP, 10);

    // Horizontal line
    wxStaticLine* line = new wxStaticLine(panel, wxID_ANY, wxDefaultPosition, wxSize(350, 1));
    mainSizer->Add(line, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);

    // IP input and connect button
    wxBoxSizer* inputSizerIP = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* ipLabel = new wxStaticText(panel, wxID_ANY, "IP Address");
    ipLabel->SetForegroundColour(wxColour(200, 200, 200));
    inputSizerIP->Add(ipLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    ipInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(280, -1));
    inputSizerIP->Add(ipInput, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    wxBoxSizer* inputSizerPort = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* portLabel = new wxStaticText(panel, wxID_ANY, "Port        ");
    portLabel->SetForegroundColour(wxColour(200, 200, 200));
    inputSizerPort->Add(portLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    portInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    inputSizerPort->Add(portInput, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    wxButton* connectButton = new wxButton(panel, wxID_ANY, "Connect", wxDefaultPosition, wxSize(70, -1));
    inputSizerPort->Add(connectButton, 0, wxALIGN_CENTER_VERTICAL);

    mainSizer->Add(inputSizerIP, 0, wxALIGN_CENTER);
    mainSizer->AddSpacer(10);
    mainSizer->Add(inputSizerPort, 0, wxALIGN_CENTER);

    panel->SetSizer(mainSizer);

    connectButton->Bind(wxEVT_BUTTON, &LoginFrame::OnConnect, this);

    Centre();
}

void LoginFrame::OnConnect(wxCommandEvent& event) {
    wxString ip = GetIP();
    wxString port = GetPort();
    wxMessageBox("Connecting to " + ip + ":" + port, "Connection Info", wxOK | wxICON_INFORMATION);

    // Call main function or start connection logic
    Close(); // Close the login frame
}
*/
