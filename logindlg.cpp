#include "logindlg.h"
#include "ui_logindlg.h"
#include "client_mainwindow.h"
#include "global.h"

#include <QMessageBox>
#include <QTimer>
#include <QDebug>

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
    ui->usernameEdt->setText("test");
    ui->passwordEdt->setText("123456");

//    m_client = new Client(this);

    connect( ui->loginBtn, SIGNAL(clicked()), this, SLOT( loginToServer() ) );
    connect( ui->cancelBtn, SIGNAL(clicked()), qApp, SLOT( quit() ) );

    connect(Global::client, SIGNAL(connected()), this,
            SLOT(login()) );
    connect(Global::client, SIGNAL(disconnected()), this,
            SLOT(logout()) );
    connect(Global::client, SIGNAL(error(QXmppClient::Error)), this,
            SLOT(loginError(QXmppClient::Error)) );
//    connect( Global::client, SIGNAL(loginSuccessed()), this, SLOT( login() ) );
//    connect( Global::client, SIGNAL(loginFailed()), this, SLOT( loginError() ) );
}

LoginDlg::~LoginDlg()
{
    delete ui;
}

void LoginDlg::loginToServer()
{
    QString username = ui->usernameEdt->text();
    QString password = ui->passwordEdt->text();
    if( username.isEmpty())
    {
        QString txt = tr("Please input user name.");
        QMessageBox::information(this, APP_TITLE, txt, QMessageBox::Ok);

        return;
    }
    if( password.isEmpty() )
    {
        QString txt = tr("Please input password.");
        QMessageBox::information(this, APP_TITLE, txt, QMessageBox::Ok);
        return;
    }
    ui->loginBtn->setEnabled(false);

    QString jid = username + "@yvi-pc";
    QXmppConfiguration conf;
    conf.setHost("yvi-pc");
    conf.setDomain("yvi-pc");
    conf.setUser(jid);
    conf.setJid(jid);
    conf.setPort(5222);
    conf.setPassword(password);
    conf.setResource(jid);
    Global::client->connectToServer(conf);
}

void LoginDlg::login()
{
    this->close();
    Global::mainWindow->show();
    Global::mainWindow->setClient( Global::client );
}

void LoginDlg::logout()
{

}

void LoginDlg::loginError(QXmppClient::Error error)
{
    QString m_errorString;
    switch ( error )
    {
    case QXmppClient::NoError:
        m_errorString = "No error";
        break;
    case QXmppClient::SocketError:
        m_errorString = tr("Error due to TCP socket");
        break;
    case QXmppClient::KeepAliveError:
        m_errorString = tr("Error due to no response to a keep alive");
        break;
    case QXmppClient::XmppStreamError:
        m_errorString = tr("Error due to XML stream");
        break;
    default:
        m_errorString = tr("Unknow error!");
        break;
    }

    if( !m_errorString.isEmpty() )
    {
        ui->loginBtn->setEnabled(true);
        QMessageBox::information(this, APP_TITLE, m_errorString, QMessageBox::Ok);
    }
}
