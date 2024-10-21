#include <wx/wx.h>
#include <wx/statline.h>

class LoginFrame : public wxFrame
{
public:
    LoginFrame() : wxFrame(nullptr, wxID_ANY, "Email Client", wxDefaultPosition, wxSize(400, 300))
    {
        SetBackgroundColour(wxColour(40, 40, 40));

        wxPanel* panel = new wxPanel(this);
        panel->SetBackgroundColour(wxColour(40, 40, 40));

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Title
        wxStaticText* title = new wxStaticText(panel, wxID_ANY, "Welcome to app remote desktop control");
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

    wxString GetIP() const { return ipInput->GetValue(); }
    wxString GetPort() const { return portInput->GetValue(); }

private:
    void OnConnect(wxCommandEvent& event)
    {
        wxString ip = GetIP();
        wxString port = GetPort();
        wxMessageBox("Connecting to " + ip + ":" + port, "Connection Info", wxOK | wxICON_INFORMATION);

        // Call main function or start connection logic
        Close(); // Close the login frame
    }

    wxTextCtrl* ipInput;
    wxTextCtrl* portInput;
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        LoginFrame* frame = new LoginFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);