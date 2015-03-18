#include "client_mainwindow.h"
#include "ui_client_mainwindow.h"
#include "global.h"
#include "rostermodel.h"

#include <QXmpp/QXmppClient.h>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() &~Qt::WindowMaximizeButtonHint);
    this->setWindowTitle( APP_TITLE );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setClient(QXmppClient *client)
{
    m_client = client;
    ui->rosterTreeView->setClient(client);
//    m_rosterModel = new RosterModel(client);
//    ui->rosterTreeView->setModel(m_rosterModel);
}
