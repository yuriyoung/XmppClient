#ifndef GLOBAL_H
#define GLOBAL_H
#include "constances.h"

class MainWindow;
class LoginDlg;
class QXmppClient;

//class Client;

namespace Global
{
    extern MainWindow*      mainWindow;
    extern LoginDlg*        loginDlg;
    extern QXmppClient*     client;

    void startClient();
    void endClient();
}

#endif // GLOBAL_H
