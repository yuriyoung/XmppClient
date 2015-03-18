#ifndef CLIENT_MAINWINDOW_H
#define CLIENT_MAINWINDOW_H

#include <QMainWindow>
//#include "client.h"

class RosterModel;
class QXmppClient;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setClient(QXmppClient* client);
    
public slots:

private:
    Ui::MainWindow  *ui;
    QXmppClient     *m_client;
    RosterModel     *m_rosterModel;
};

#endif // CLIENT_MAINWINDOW_H
