#include "ServiceManager.h"

enum {
    ID_EndTask = 1
};

wxBEGIN_EVENT_TABLE(ServiceManager, wxPanel)
    EVT_MENU(ID_EndTask, ServiceManager::OnEndTask)
wxEND_EVENT_TABLE()

ServiceManager::ServiceManager(wxWindow* parent)
    : wxPanel(parent, wxID_ANY) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    serviceList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    serviceList->InsertColumn(0, "Service Name", wxLIST_FORMAT_LEFT, 200);
    serviceList->InsertColumn(1, "Display Name", wxLIST_FORMAT_LEFT, 400);
    serviceList->InsertColumn(2, "State", wxLIST_FORMAT_LEFT, 100);

    mainSizer->Add(serviceList, 1, wxEXPAND | wxALL, 5);

    totalServiceText = new wxStaticText(this, wxID_ANY, "Total service: 0");
    mainSizer->Add(totalServiceText, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizerAndFit(mainSizer);

    // Create context menu
    contextMenu = new wxMenu;
    contextMenu->Append(ID_EndTask, "End Task");

    // Bind right-click event
    serviceList->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &ServiceManager::OnRightClick, this);
}

wxString processLine(const wxString& line);
void ServiceManager::LoadServiceFromFile(const wxString& filename) {
    serviceList->DeleteAllItems();

    wxFileInputStream input(filename);
    if (!input.IsOk()) {
        wxMessageBox("Cannot open log file!", "Error", wxICON_ERROR);
        return;
    }

    wxTextInputStream text(input);
    wxString line, totalServices;

    while (!input.Eof()) {
        line = text.ReadLine();

        if (line.StartsWith("Service Name")) {
            continue;
        } else if (line.StartsWith("---")) {
            continue;
        } else if (line.StartsWith("Total services")) {
            totalServices = line.AfterLast(' ');
            continue;
        }

        // Xử lý dòng chứa thông tin process
        if (!line.IsEmpty() && serviceList != nullptr) {
            wxString service_name = processLine(line.BeforeFirst('.').Trim());
            wxString state = processLine(line.AfterLast('.').Trim());
            wxString display_name = processLine(line.Mid(service_name.Length(), line.Length() - service_name.Length() - state.Length()).Trim());
            display_name = display_name.Mid(1, display_name.Length() - 2);

            if (!service_name.IsEmpty() && !display_name.IsEmpty() && !state.IsEmpty()) {
                AddServiceToPage(service_name, display_name, state);
            }
        }
    }

    wxString totalLine = wxString::Format("Total services: %s", totalServices);
    totalServiceText->SetLabel(totalLine);
}

void ServiceManager::AddServiceToPage(const wxString& service_name, const wxString& display_name, const wxString& state) {
    long index = serviceList->GetItemCount();
    long item = serviceList->InsertItem(index, service_name);
    serviceList->SetItem(item, 1, display_name);
    serviceList->SetItem(item, 2, state);
}

void ServiceManager::OnRightClick(wxListEvent& event) {
    PopupMenu(contextMenu, event.GetPoint());
}

void ServiceManager::OnEndTask(wxCommandEvent& event) {
    long itemIndex = -1;
    wxListCtrl* currentPage = serviceList;
    if (currentPage) {
        itemIndex = currentPage->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }

    if (itemIndex == -1) {
        wxMessageBox("No service selected", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int checkbox = wxMessageBox("Are you sure you want to end this service?", "Confirm", wxYES_NO | wxICON_QUESTION);
    if (checkbox != wxYES) {
        return;
    }

    wxString pidStr = currentPage->GetItemText(itemIndex);
    if(((LogPanel*)GetParent())-> EndService(pidStr)) {
        currentPage->DeleteItem(itemIndex);
        wxString totalServices = totalServiceText->GetLabel();
        long total = wxAtoi(totalServices.AfterLast(' '));
        totalServiceText->SetLabel(wxString::Format("Total services: %ld", total - 1));
    } else {
        wxMessageBox("Failed to end service", "Error", wxOK | wxICON_ERROR);
    }
}

// Function to trim leading and trailing spaces
wxString ServiceManager::trim(const wxString& str) {
    size_t first = str.find_first_not_of(' ');
    if (wxString::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Function to remove extra spaces between words
wxString ServiceManager::removeExtraSpaces(const wxString& str) {
    wxString result;
    bool inSpaces = false;
    for (char ch : str) {
        if (isspace(ch)) {
            if (!inSpaces) {
                result += ' ';
                inSpaces = true;
            }
        } else {
            result += ch;
            inSpaces = false;
        }
    }
    return result;
}

// Function to process the line by trimming and removing extra spaces
wxString ServiceManager::processLine(const wxString& line) {
    wxString trimmedLine = trim(line);
    return removeExtraSpaces(trimmedLine);
}