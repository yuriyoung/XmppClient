#include "client_mainwindow.h"
#include "logindlg.h"
#include "global.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Global::startClient();

    Global::loginDlg->show();

    int ret = a.exec();
    if( ret == 0 )
        Global::endClient();

    return ret;
}
