#include "FileExplorer.h"
#include <wx/artprov.h>
#include <wx/volume.h>

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

    // Bind events
    m_listCtrl->Bind(wxEVT_LIST_ITEM_ACTIVATED, &FileExplorer::OnItemActivated, this);
    m_listCtrl->Bind(wxEVT_CONTEXT_MENU, &FileExplorer::OnContextMenu, this);

    DragAcceptFiles(true);
    Bind(wxEVT_DROP_FILES, &FileExplorer::OnDragDrop, this);

    m_currentPath = "";
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
    string filePath = ((LogPanel*)GetParent())-> scanFolder(m_currentPath);
    FOLDER folder = FOLDER::readCacheFile(filePath);

    m_listCtrl->DeleteAllItems();

    // Add parent directory entry (..)
    if(!m_currentPath.empty()) {
        long itemIndex = m_listCtrl->InsertItem(0, "..", ICON_UP);
        m_listCtrl->SetItem(itemIndex, 1, "Parent Directory");
    }

    // List subfolders
    for (const auto& subfolder : folder.subfolders) {
        long itemIndex = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), subfolder.name, (m_currentPath.empty()) ? ICON_DRIVE : ICON_FOLDER);
        m_listCtrl->SetItem(itemIndex, 1, "Folder");
    }

    // List files
    for (const auto& file : folder.files) {
        long itemIndex = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), file.name, ICON_FILE);
        m_listCtrl->SetItem(itemIndex, 1, "File");
    }
}

void FileExplorer::EnableMenuFunctions(bool enable) {
    // Implement logic to enable/disable menu functions
}

void FileExplorer::Reset() {
    m_currentPath = "";
    PopulateList();
}

void FileExplorer::OnItemActivated(wxListEvent& event) {
    wxString item = m_listCtrl->GetItemText(event.GetIndex());

    if (item == "..")
    {
        m_currentPath.pop_back();
        while(!m_currentPath.empty() && m_currentPath.back() != '\\')
            m_currentPath.pop_back();
    } else {
        m_currentPath += '\\' + item;


    }

    PopulateList();
}

void FileExplorer::OnContextMenu(wxContextMenuEvent& event)
{
    wxPoint point = event.GetPosition();
    point = m_listCtrl->ScreenToClient(point);

    int flags;
    long subItem;
    long index = m_listCtrl->HitTest(point, flags, &subItem);
    if (index != wxNOT_FOUND)
    {
        menu.Append(wxID_COPY, "&Copy");
        menu.Append(wxID_CUT, "&Move");
        menu.Append(wxID_DELETE, "&Delete");

        PopupMenu(&menu);
    }
}

void FileExplorer::OnCopy(wxCommandEvent& event)
{
    long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1)
    {
        wxString filename = m_listCtrl->GetItemText(item);
        wxString sourcePath = m_currentPath + wxFILE_SEP_PATH + filename;

        // Implement copy logic here
    }
}

void FileExplorer::OnMove(wxCommandEvent& event)
{
    long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1)
    {
        wxString filename = m_listCtrl->GetItemText(item);
        wxString sourcePath = m_currentPath + wxFILE_SEP_PATH + filename;

        // Implement move logic here
    }
}

void FileExplorer::OnDelete(wxCommandEvent& event)
{
    long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1)
    {
        wxString filename = m_listCtrl->GetItemText(item);
        wxString path = m_currentPath + wxFILE_SEP_PATH + filename;

        if (wxMessageBox("Are you sure you want to delete this item?",
                        "Confirm Delete",
                        wxYES_NO | wxICON_QUESTION) == wxYES)
        {
            if (wxDirExists(path))
                wxRmdir(path);
            else
                wxRemoveFile(path);

            PopulateList();
        }
    }
}

void FileExplorer::OnDragDrop(wxDropFilesEvent& event)
{
    wxString* files = event.GetFiles();
    int count = event.GetNumberOfFiles();

    for (int i = 0; i < count; i++)
    {
        wxString filename = wxFileName(files[i]).GetFullName();
        wxString destPath = m_currentPath + wxFILE_SEP_PATH + filename;

        // Implement file copy logic here
        wxCopyFile(files[i], destPath);
    }

    PopulateList();
}

FileExplorer::~FileExplorer()
{
    delete m_imageList;
}