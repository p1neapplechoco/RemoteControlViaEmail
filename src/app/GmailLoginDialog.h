#pragma once
#include "App.h"

#ifndef GMAILLOGINDIALOG_H
#define GMAILLOGINDIALOG_H

class GmailLoginDialog : public wxDialog {
public:
    GmailLoginDialog(wxWindow* parent);
    bool IsLoginSuccessful() const { return loginSuccessful; }
    wxString GetEmail() const { return emailInput->GetValue(); }

private:
    wxTextCtrl* emailInput;
    wxTextCtrl* passwordInput;
    wxBitmapButton* loginButton;
    bool loginSuccessful = false;
    wxString email;

    void OnLogin(wxCommandEvent& evt);
    bool AuthenticateUser(const wxString& email, const wxString& password);
};

#endif //GMAILLOGINDIALOG_H
