#include "FileExplorer.h"
#include <wx/artprov.h>
#include <wx/volume.h>

enum {
    ID_DELETE = 1,
};

wxBEGIN_EVENT_TABLE(FileExplorer, wxPanel)
    EVT_MENU(ID_DELETE, FileExplorer::OnDelete)
wxEND_EVENT_TABLE()

FileExplorer::FileExplorer(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    m_listCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                               wxLC_REPORT | wxLC_SINGLE_SEL);

    m_listCtrl->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 300);
    m_listCtrl->InsertColumn(1, "Type", wxLIST_FORMAT_LEFT, 100);

    InitializeIcons();
    mainSizer->Add(m_listCtrl, 1, wxEXPAND | wxALL, 5);
    SetSizerAndFit(mainSizer);

    // Menu
    contextMenu = new wxMenu;
    contextMenu->Append(ID_DELETE, "Delete");

    // Bind events
    m_listCtrl->Bind(wxEVT_LIST_ITEM_ACTIVATED, &FileExplorer::OnItemActivated, this);
    m_listCtrl->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &FileExplorer::OnRightClick, this);

    m_folder.clear();
    PopulateList();
}

void FileExplorer::InitializeIcons()
{
    m_imageList = new wxImageList(16, 16);

    wxString iconPath = wxT("assert/icon/");
    wxSize iconSize(16, 16);
    wxBitmap bitmap;

    m_imageList->Add(wxBitmap(wxImage(iconPath + wxT("file.png"), wxBITMAP_TYPE_PNG).Rescale(iconSize.GetWidth(), iconSize.GetHeight())));
    m_imageList->Add(wxBitmap(wxImage(iconPath + wxT("folder.png"), wxBITMAP_TYPE_PNG).Rescale(iconSize.GetWidth(), iconSize.GetHeight())));
    m_imageList->Add(wxBitmap(wxImage(iconPath + wxT("drive.png"), wxBITMAP_TYPE_PNG).Rescale(iconSize.GetWidth(), iconSize.GetHeight())));
    m_imageList->Add(wxBitmap(wxImage(iconPath + wxT("parent.png"), wxBITMAP_TYPE_PNG).Rescale(iconSize.GetWidth(), iconSize.GetHeight())));

    m_listCtrl->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
}

void FileExplorer::PopulateList() {
    m_listCtrl->DeleteAllItems();
    if (m_folder.empty())
        return;

    // Add parent directory entry (..)
    if (m_folder.size() > 1) {
        long itemIndex = m_listCtrl->InsertItem(0, "..", ICON_UP);
        m_listCtrl->SetItem(itemIndex, 1, "Parent Directory");
    }

    FOLDER folder = m_folder.back();
    // List subfolders
    for (const auto& subfolder : folder.subfolders) {
        long itemIndex = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), subfolder.name, (m_folder.size() <= 1) ? ICON_DRIVE : ICON_FOLDER);
        m_listCtrl->SetItem(itemIndex, 1, (m_folder.size() <= 1) ? "Drive" : "Folder");
    }

    // List files
    for (const auto& file : folder.files) {
        long itemIndex = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), file.name, ICON_FILE);
        m_listCtrl->SetItem(itemIndex, 1, "File");
    }
}

void FileExplorer::LoadDisksFromFile(const string &filename) {
    m_folder.clear();

    wxFileInputStream input(filename);
    if (!input.IsOk()) {
        wxMessageBox("Cannot open file!", "Error", wxICON_ERROR);
        return;
    }

    wxTextInputStream text(input);
    wxString line;

    FOLDER newFolder;
    newFolder.path = "";
    newFolder.name = "Disks";

    while (!input.Eof()) {
        line = text.ReadLine();

        if (line.StartsWith("Available Disks")) {
            continue;
        }

        if (!line.IsEmpty()) {
            FOLDER folder;
            folder.path = line.ToStdString();
            folder.name = line.substr(0, line.size() - 1).ToStdString();
            newFolder.subfolders.push_back(folder);
        }
    }

    m_folder.push_back(newFolder);
    PopulateList();
}

void FileExplorer::OnItemActivated(wxListEvent& event) {
    wxString item = m_listCtrl->GetItemText(event.GetIndex());

    if (item == "..")
    {
        m_folder.pop_back();
    } else {
        bool isSelected = false;
        FOLDER folder = m_folder.back();

        for (const auto& subfolder : folder.subfolders)
            if (subfolder.name == item) {
                isSelected = true;
                if(m_folder.size() > 1) {
                    m_folder.push_back(subfolder);
                } else {
                    string fileName = ((LogPanel*)GetParent())-> scanDrive(subfolder.path);
                    FOLDER::readCacheFile(fileName);
                    m_folder.push_back(FOLDER::readCacheFile(fileName));
                } break;
            }

        // List files
        if (!isSelected) {
            for (const auto& file : folder.files)
                if (file.name == item) {
                    if (item.EndsWith(".txt") || item.EndsWith(".exe")) {
                        if (wxMessageBox("Are you sure you want to get this item?", "Confirm Get",
                                        wxYES_NO | wxICON_QUESTION) == wxYES)
                        {
                            if (((LogPanel*)GetParent())->GetAndSendFile(file.path))
                                wxMessageBox("File has been got to the client!", "Success", wxICON_INFORMATION);
                            else
                                wxMessageBox("Failed to get file!", "Error", wxICON_ERROR);
                        }
                    }
                    // else if (item.EndsWith(".exe")) {
                    //     if (wxMessageBox("Are you sure you want to open this item?", "Confirm Open",
                    //                     wxYES_NO | wxICON_QUESTION) == wxYES)
                    //     {
                    //         if (((LogPanel*)GetParent())->GetAndSendFile(file.path))
                    //             wxMessageBox("File has been opened in the server!", "Success", wxICON_INFORMATION);
                    //         else
                    //             wxMessageBox("Failed to open file!", "Error", wxICON_ERROR);
                    //     }
                    // }
                    break;
                }
        }
    }
    PopulateList();
}

void FileExplorer::OnRightClick(wxListEvent& event) {
    PopupMenu(contextMenu, event.GetPoint());
}

void FileExplorer::OnDelete(wxCommandEvent& event)
{
    long itemIndex = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (itemIndex != -1) {
        wxString filename = m_listCtrl->GetItemText(itemIndex);
        FOLDER& folder = m_folder.back();

        auto it = std::find_if(folder.files.begin(), folder.files.end(), [&](const auto& file) {
            return file.name == filename;
        });

        if (it != folder.files.end()) {
            string path = it->path;

            if (wxMessageBox("Are you sure you want to delete this item?",
                            "Confirm Delete",
                            wxYES_NO | wxICON_QUESTION) == wxYES)
            {
                if (((LogPanel*)GetParent())->Remove(path)) {
                    wxMessageBox("Item has been deleted!", "Success", wxICON_INFORMATION);
                    folder.files.erase(it); // Remove the file from the list
                } else {
                    wxMessageBox("Failed to delete item!", "Error", wxICON_ERROR);
                }
                PopulateList();
            }
        }
    }
}

FileExplorer::~FileExplorer()
{
    delete m_imageList;
}