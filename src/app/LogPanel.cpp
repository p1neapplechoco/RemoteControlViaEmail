#include "LogPanel.h"

LogPanel::LogPanel(wxWindow* parent, const wxString &IP_Address) : wxPanel(parent, wxID_ANY) {
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
    // keyLoggerPanel = new wxPanel(bottomPanel, wxID_ANY);
    // keyLogoutPanel = new wxPanel(bottomPanel, wxID_ANY);
    // bottomSizer->Add(keyLoggerPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);
    // bottomSizer->Add(keyLogoutPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

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

}

void LogPanel::OnCancelClick(wxCommandEvent& event) {
    AppendLog("Operation cancelled\n");
}

void LogPanel::OnSendClick(wxCommandEvent& event) {
    AppendLog("Sending command...\n");
}

void LogPanel::AppendLog(const wxString& message) {
    logTextCtrl->AppendText(wxDateTime::Now().FormatTime() + ": " + message);
}
