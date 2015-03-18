#include "global.h"
#include "client_mainwindow.h"
#include "logindlg.h"
//#include "client.h"

#include <QApplication>
#include <QXmpp/QXmppClient.h>

using namespace Global;

MainWindow            *Global::mainWindow;
LoginDlg              *Global::loginDlg;
QXmppClient           *Global::client = 0;

void Global::startClient()
{
    mainWindow = new MainWindow;
    client = new QXmppClient(mainWindow);
    loginDlg = new LoginDlg;

}


void Global::endClient()
{
    delete mainWindow;
    qApp->quit();
}
