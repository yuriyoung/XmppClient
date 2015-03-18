#ifndef ROSTER_TREEVIEW_H
#define ROSTER_TREEVIEW_H

#include <QTreeView>

class RosterModel;
class QXmppClient;

class RosterTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit RosterTreeView(QWidget *parent = 0);
    
    void setClient(QXmppClient *client);

signals:
    
public slots:
    void rosterViewHiddenUpdate();

private:
    QXmppClient*    m_client;
    RosterModel     *m_rosterModel;
};

#endif // ROSTER_TREEVIEW_H
