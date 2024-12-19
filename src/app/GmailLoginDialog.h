#pragma once
#include "App.h"

#ifndef GMAILLOGINDIALOG_H
#define GMAILLOGINDIALOG_H

class GmailLoginDialog : public wxDialog {
public:
    GmailLoginDialog(wxWindow* parent);
    bool IsLoginSuccessful() const { return loginSuccessful; }
    wxString email, password;

private:
    wxTextCtrl* emailInput;
    wxTextCtrl* passwordInput;
    wxBitmapButton* loginButton;
    bool loginSuccessful = false;

    void OnLogin(wxCommandEvent& evt);
    bool AuthenticateUser(const wxString& email, const wxString& password);
};

#endif //GMAILLOGINDIALOG_H
