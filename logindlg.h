#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
//#include "client.h"
#include <QXmpp/QXmppClient.h>

namespace Ui {
class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();
    
private slots:
    void loginToServer();
    void login();
    void logout();
    void loginError(QXmppClient::Error error);

private:
    Ui::LoginDlg *ui;
//    Client*         m_client;
};

#endif // LOGINDLG_H
