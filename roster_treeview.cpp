#include "roster_treeview.h"
#include "rostermodel.h"

#include <QXmpp/QXmppClient.h>

RosterTreeView::RosterTreeView(QWidget *parent) :
    QTreeView(parent)
{
    this->setHeaderHidden(true);
    this->setAnimated(true);
    this->setRootIsDecorated(false);
    this->setWordWrap(true);
    this->setAlternatingRowColors(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

void RosterTreeView::setClient(QXmppClient *client)
{
    m_client = client;
    m_rosterModel = new RosterModel(client);
    this->setModel(m_rosterModel);

    // roster model and view
//    connect(m_rosterModel, SIGNAL(parseDone()),
//            this, SLOT(changeToRoster()) );
    connect(m_rosterModel, SIGNAL(parseDone()),
            this, SLOT(rosterViewHiddenUpdate()) );
//    connect(m_rosterModel, SIGNAL(hiddenUpdate()),
//            this, SLOT(rosterViewHiddenUpdate()) );

}

void RosterTreeView::rosterViewHiddenUpdate()
{
    foreach (QModelIndex contactIndex, m_rosterModel->allIndex())
    {
        this->setRowHidden(contactIndex.row(),
                                       contactIndex.parent(),
                                       m_rosterModel->isIndexHidden(contactIndex));;
    }
}
