#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/graphics.h>

class RoundedButton : public wxButton {
public:
    RoundedButton(wxWindow* parent, wxWindowID id, const wxString& label,
                  const wxPoint& pos, const wxSize& size)
        : wxButton(parent, id, label, pos, size) {
        SetBackgroundColour(*wxBLUE);
        SetForegroundColour(*wxWHITE);
    }

protected:
    void DoGetSize(int* width, int* height) const override {
        wxButton::DoGetSize(width, height);
        if (width) *width += 20;  // Thêm khoảng cách cho biên
        if (height) *height += 10; // Thêm khoảng cách cho biên
    }

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        wxSize size = GetSize();

        // Vẽ hình chữ nhật bo góc
        wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
        if (gc) {
            gc->SetBrush(*wxLIGHT_GREY);
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), 100); // Bo góc 10px
            gc->DrawText(GetLabel(), 10, 10); // Vẽ chữ
            delete gc;
        }
    }

    void Init() {
        Bind(wxEVT_PAINT, &RoundedButton::OnPaint, this);
    }
};

enum IDs {
    BUTTON_ID = 2,
    TEXT_ID = 3
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(BUTTON_ID, MainFrame::OnButtonClicked)
    EVT_TEXT(TEXT_ID, MainFrame::OnTextChanged)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title): wxFrame(nullptr, wxID_ANY, title) {
    wxPanel* panel = new wxPanel(this);

    RoundedButton* button = new RoundedButton(panel, BUTTON_ID, "Log In",
        wxPoint(300, 400), wxSize(200, 35));

    wxTextCtrl* textCtrl = new wxTextCtrl(panel, TEXT_ID, "",
        wxPoint(300, 200), wxSize(200, 35));

    CreateStatusBar();
}

void MainFrame::OnButtonClicked(wxCommandEvent &evt) {
    wxLogStatus("ButtonClicked");
}

void MainFrame::OnTextChanged(wxCommandEvent &evt) {
    wxString str = wxString::Format("Text: %s", evt.GetString());
    wxLogStatus(str);
}
