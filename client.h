#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <qxmpp/QXmppClient.h>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    
    void connectToServer( const QString &jid, const QString &password );
    void disconnectServer();

    inline QString clientAddress()const { return m_clientAddr;}
    inline QString clientName()const { return m_clientName;}

    inline QString errorString()  const{ return m_errorString; }

    QXmppClient* client() const { return m_client; }

    bool isConnected() const;

signals:
    void loginFailed();
    void loginSuccessed();

public slots:
    void messageReceived(const QXmppMessage&message);
    void clientConnected();
    void clientDisconnected();

    void error(QXmppClient::Error);

    void parseRoster();

private:
    QString localHost();

    QXmppClient* m_client;

    QString     m_clientAddr;
    QString     m_clientName;
    QString     m_errorString;
};

#endif // CLIENT_H
