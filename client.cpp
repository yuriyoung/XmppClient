#include "client.h"
#include <qxmpp/QXmppConfiguration.h>
#include <qxmpp/QXmppMessage.h>
#include <QXmpp/QXmppPasswordChecker.h>
#include <QXmpp/QXmppRosterIq.h>
#include <QXmpp/QXmppRosterManager.h>

#include <QtNetwork>
#include <QTimer>

#include <QDebug>

Client::Client(QObject *parent) :
    QObject(parent),
    m_client( new QXmppClient(this) )
{
    m_clientAddr = localHost();

    connect(m_client, SIGNAL(connected()), this,
            SLOT(clientConnected()) );
    connect(m_client, SIGNAL(disconnected()), this,
            SLOT(clientDisconnected()) );

    connect(m_client, SIGNAL(error(QXmppClient::Error)), this,
            SLOT(error(QXmppClient::Error)) );
    connect(m_client, SIGNAL(messageReceived(QXmppMessage)), this,
            SLOT(messageReceived(QXmppMessage)) );
}

void Client::connectToServer(const QString &jid, const QString &password)
{
    QXmppConfiguration conf;
    conf.setHost("yvi-pc");
    conf.setDomain("yvi-pc");
    conf.setUser(jid);
    conf.setJid(jid);
    conf.setPort(5222);
    conf.setPassword(password);
//    conf.setSaslAuthMechanism("mechanism");
    m_client->connectToServer(conf);
}

void Client::disconnectServer()
{
    m_client->disconnectFromServer();
}

bool Client::isConnected() const
{
    return m_client->isConnected();
}

void Client::messageReceived(const QXmppMessage &message)
{
    qDebug() << message.body();
    QString from = message.from();
    QString msg = message.body();
    m_client->sendPacket(QXmppMessage("", from, "Your message: " + msg));
}

void Client::clientConnected()
{
    qDebug() << "Client has connected server.";
     QXmppRosterManager *roster = &m_client->rosterManager();
     qDebug() << roster->getRosterBareJids();
     connect(roster, SIGNAL(rosterReceived()),
             this, SLOT(parseRoster()) );

    QTimer::singleShot(1000, this, SIGNAL( loginSuccessed() ));
}

void Client::clientDisconnected()
{
    qDebug() << "Client has disconnected server.";
    QTimer::singleShot(0, this, SIGNAL( loginFailed() ));
}

void Client::error(QXmppClient::Error error)
{
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
}

void Client::parseRoster()
{
    qDebug() << "parseRoster";
}

QString Client::localHost()
{
    QString ipAddress = "0:0:0:0";
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address())
        {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    if (ipAddress == "0:0:0:0")
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    return ipAddress;
}
