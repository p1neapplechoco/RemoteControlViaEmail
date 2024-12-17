#include "MainFrame.h"
using namespace std;

struct ButtonInfo {
    wxString label;
    int id;
};

vector<ButtonInfo> buttons = {
    {"listProcesses", ID_LIST_PROCESSES},
    {"listServices", ID_LIST_SERVICES},
    {"screenshot", ID_SCREENSHOT},
    {"toggleWebcam", ID_TOGGLE_WEBCAM},
    {"captureWebcam", ID_CAPTURE_WEBCAM},
    {"fileExplorer", ID_FILE_EXPLORER},
    {"power", ID_POWER}
};

MainFrame::MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE, const wxString &currentEmail,
                     const wxString &IP_Address) : wxFrame(nullptr, wxID_ANY, TITLE, POS, SIZE) {

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
        btnPanel->SetBackgroundColour(wxColor(240, 240, 240));

        auto btn = new CustomBitmapButton(btnPanel, buttonInfo.id, buttonInfo.label);
        btn->Bind(wxEVT_BUTTON, [this, btnPanel](wxCommandEvent& event) {
            HighlightButton(btnPanel);
            OnButtonClick(event);
        });
        sideButtons.push_back(btn);

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
    rightPanel = new LogPanel(this, IP_Address);
    contentSizer->Add(rightPanel, 1, wxEXPAND | wxALL, margin);

    mainSizer->Add(contentSizer, 1, wxEXPAND);
    this->SetSizer(mainSizer);

    this->SetMinSize(wxSize(FromDIP(800), FromDIP(600)));
}

void MainFrame::UpdateStatusBar(const wxString& text) {
    SetStatusText(text);
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

void MainFrame::EnableSideButtons(bool enable) {
    for (auto btn : sideButtons) {
        btn->Enable(enable);
    }
}

void MainFrame::OnButtonClick(wxCommandEvent& evt) {
    int id = evt.GetId();
    switch (id) {
        case ID_EXIT: {
            LoginFrame* loginFrame = new LoginFrame("Remote Control Via Desktop", wxDefaultPosition, wxDefaultSize);
            loginFrame->Fit();
            loginFrame->Center();
            loginFrame->Show();
            Close();
        }
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