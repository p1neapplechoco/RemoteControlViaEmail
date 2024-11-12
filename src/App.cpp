#include "App.h"
using namespace std;

CustomBitmapButton::CustomBitmapButton(wxWindow* parent, wxWindowID id, const wxString& nameImage,
       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize)
       : wxBitmapButton(parent, id, wxBitmap(), pos, size, wxBORDER_NONE | wxBU_AUTODRAW)
{
    auto LoadBitmapWithTransparency = [&](const wxString& filename, wxBitmap& bitmap) {
        if (!wxFileExists(filename)) {
            wxLogError("Image file not found: %s", filename);
            return false;
        }

        bitmap = wxBitmap(filename, wxBITMAP_TYPE_PNG);

        if (!bitmap.IsOk()) {
            wxLogError("Failed to load image: %s", filename);
            return false;
        }

        // Check for alpha channel and create mask if not present
        if (!bitmap.HasAlpha()) {
            // Use a color for the mask if the image has a solid background color for transparency.
            bitmap.SetMask(new wxMask(bitmap, wxColor(255, 0, 255))); // Magenta mask or find the correct background color
        }
        return true;
    };

    if (!LoadBitmapWithTransparency(path_buttons + "normal/" + nameImage + ".png", m_normalBitmap) ||
        !LoadBitmapWithTransparency(path_buttons + "hover/" + nameImage + ".png", m_hoverBitmap) ||
        !LoadBitmapWithTransparency(path_buttons + "pressed/" + nameImage + ".png", m_pressedBitmap)) {
        return; // Or handle the error in another way
        }

    SetBitmap(m_normalBitmap); // Set initial bitmap

    // Bind events
    Bind(wxEVT_ENTER_WINDOW, &CustomBitmapButton::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &CustomBitmapButton::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &CustomBitmapButton::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &CustomBitmapButton::OnMouseUp, this);

    this->SetBackgroundStyle(wxBG_STYLE_PAINT);
}

CustomBitmapButton::~CustomBitmapButton() {
    //Delete the masks to prevent memory leaks
    delete m_normalBitmap.GetMask();
    delete m_hoverBitmap.GetMask();
    delete m_pressedBitmap.GetMask();
}

void CustomBitmapButton::OnMouseEnter(wxMouseEvent& event) {
    SetBitmap(m_hoverBitmap);
    Refresh();
}

void CustomBitmapButton::OnMouseLeave(wxMouseEvent& event) {
    SetBitmap(m_normalBitmap);
    Refresh();
}

void CustomBitmapButton::OnMouseDown(wxMouseEvent& event) {
    SetBitmap(m_pressedBitmap);
    Refresh();
    event.Skip();
}

void CustomBitmapButton::OnMouseUp(wxMouseEvent& event) {
    if (GetClientRect().Contains(event.GetPosition()))
        SetBitmap(m_hoverBitmap);
    else
        SetBitmap(m_normalBitmap);
    Refresh();
    event.Skip();
}

bool App::OnInit() {
    wxInitAllImageHandlers();
    MainFrame* mainFrame = new MainFrame("C++ GUI", wxDefaultPosition, wxDefaultSize, "0", "0");
    mainFrame->Fit();
    mainFrame->Center();
    mainFrame->Show();

    LoginFrame* loginFrame = new LoginFrame("Remote Control Via Desktop", wxDefaultPosition, wxDefaultSize);
    loginFrame->Fit();
    loginFrame->Center();
    loginFrame->Show();

    return true;
}

wxIMPLEMENT_APP(App);