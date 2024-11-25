#include "LogPanel.h"

LogPanel::LogPanel(wxWindow* parent, const wxString &IP_Address, const wxString &IP_Port) : wxPanel(parent, wxID_ANY) {
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

    // Build UI for func
    SCREENSHOTPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(SCREENSHOTPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();

    auto cancelButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "cancel");
    cancelButton->Bind(wxEVT_BUTTON, &LogPanel::OnCancelClick, this);
    auto sendButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "send");
    sendButton->Bind(wxEVT_BUTTON, &LogPanel::OnSendClick, this);

    buttonSizer->Add(cancelButton, 0, wxRIGHT, margin);
    buttonSizer->Add(sendButton, 0);

    bottomSizer->Add(buttonSizer, 1, wxEXPAND);
    bottomPanel->SetSizer(bottomSizer);

    mainSizer->Add(bottomPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, margin);
    SetSizer(mainSizer);

    CreateSCREENSHOT();

    // Process
    long port;
    IP_Port.ToLong(&port);
    client.setServerPort(port);
    client.setServerIP(IP_Address.ToStdString());

    // if(ConnectToServer(IP_Address, IP_Port)) {
    //     AppendLog("Connect to server IP " + IP_Address + " is Successfully!! Wait for response!\n");
    // } else {
    //     AppendLog("Connect to server IP " + IP_Address + " is Unsuccessfully!! Wait for response!\n");
    // }
}

void LogPanel::CreateSCREENSHOT() {
    auto SCREENSHOTSizer = new wxBoxSizer(wxVERTICAL);

    auto timeLabel = new wxStaticText(SCREENSHOTPanel, wxID_ANY, "SCREENSHOT");
    SCREENSHOTSizer->Add(timeLabel, 0, wxBOTTOM, 5);

    SCREENSHOTPanel->SetSizer(SCREENSHOTSizer);
    SCREENSHOTPanel->Hide();
}

void LogPanel::UpdatePanelVisibility(int selectedPanel) {
    if (SCREENSHOTPanel) {
        SCREENSHOTPanel->Show(false);
    }

    wxString myString = wxString::Format(wxT("Selected Panel ID: %d"), selectedPanel);
    wxMessageBox(myString, wxT("Selected Panel"), wxOK | wxCANCEL);
    ID_SelectPanel = selectedPanel;

    switch (selectedPanel) {
        case ID_SCREENSHOT:
            SCREENSHOTPanel->Show(true);
        break;
        case ID_SHUTDOWN:
            // Show capture screen panel
                break;
        case ID_CAPTURE_WEBCAM:
            // Show webcam panel
                break;
        // Add cases for other panels
    }
    Layout();
}

bool LogPanel::ConnectToServer(const wxString &IP_Address, const wxString &IP_Port) {
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
    return client.connectToServer();
}

void LogPanel::OnCancelClick(wxCommandEvent& event) {
    AppendLog("Operation cancelled\n");
}

void LogPanel::OnSendClick(wxCommandEvent& event) {
    switch (ID_SelectPanel) {
        case ID_TOGGLE_WEBCAM: {
            AppendLog("Sending command Toggle Webcam\n");
            client.handleCommand("webcam");
        }
    }

}

void LogPanel::AppendLog(const wxString& message) {
    logTextCtrl->AppendText(wxDateTime::Now().FormatTime() + ": " + message);
}
