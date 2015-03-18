#include "rostermodel.h"
#include "rostertreeitem.h"
#include "Preferences.h"

#include <QXmpp/QXmppClient.h>
#include <QXmpp/QXmppRosterIq.h>
#include <QXmpp/QXmppRosterManager.h>
#include <QXmpp/QXmppUtils.h>
#include <QXmpp/QXmppVCardManager.h>
#include <QXmpp/QXmppVCardIq.h>
#include <QXmpp/QXmppPresence.h>

#include <QIcon>
#include <QBuffer>
#include <QImageReader>
#include <QMessageBox>

#include <QDebug>

RosterModel::RosterModel(QXmppClient *client, QObject *parent) :
    QAbstractItemModel(parent),
    m_hideOffline(false),
    m_showResources(false)
{
    setClient(client);
    m_rootItem = new RosterTreeItem(Root, "root");
}

RosterModel::~RosterModel()
{
    delete m_rootItem;
}

QModelIndex RosterModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0)
        return QModelIndex();

    RosterTreeItem *parentItem = getItem(parent);

    RosterTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

Qt::ItemFlags RosterModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

}

QVariant RosterModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    RosterTreeItem *item = getItem(index);
    ItemType type = item->type();

    if (role == Qt::DisplayRole)
    {
        return displayData(index);
    }

    if (role == Qt::ToolTipRole)
    {
        return toolTipData(index);

    }

    if (role == Qt::DecorationRole)
    {
        if (type == RosterModel::Group)
        {
            QImage image(":/images/folder.png");
            return QIcon(QPixmap::fromImage(image.scaled(QSize(24, 24))));
        }
        else if (type == RosterModel::Contact)
        {
            if (item->isUnread())
            {
                return QIcon(":/images/mail-unread-new.png");
            }
//            if (m_vCards.contains(jidAt(index))
//                && !m_vCards[jidAt(index)].photo().isNull())
//            {
//                qDebug() << "reate a image.";
//                QBuffer buffer;
//                buffer.setData(m_vCards[jidAt(index)].photo());
//                buffer.open(QIODevice::ReadOnly);
//                QImageReader imageReader(&buffer);
//                QImage image = imageReader.read().scaled(QSize(64, 64));
//                return QIcon(QPixmap::fromImage(image));
//            }
//            else
            {
                if (item->childCount() == 0)
                    return QIcon(":/images/user-identity-grey.png");
                else
                    return QIcon(":/images/user-identity.png");
            }
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

QVariant RosterModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,
                                 int /*role*/) const
{
    return QVariant();
}

QModelIndex RosterModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    RosterTreeItem *childItem = getItem(index);
    RosterTreeItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int RosterModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return 0;

    RosterTreeItem *parentItem = getItem(parent);

    return parentItem->childCount();

}

int RosterModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

RosterModel::ItemType RosterModel::itemTypeAt(const QModelIndex &index) const
{
    return ((RosterTreeItem *)index.internalPointer())->type();
}

QString RosterModel::jidAt(const QModelIndex &index) const
{
    RosterTreeItem *item = getItem(index);
    if (itemTypeAt(index) == RosterModel::Contact)
    {
        return item->data();
    } else if (item->type() == RosterModel::Resource)
    {
        return item->parent()->data() + "/" + item->data();
    }
    return QString();
}

QString RosterModel::groupAt(const QModelIndex &index) const
{
    if (itemTypeAt(index) == RosterModel::Contact)
    {
        return getItem(index)->parent()->data();
    }
    return QString();
}

void RosterModel::messageUnread(const QString &bareJid, const QString &resource)
{
    qDebug() << "messageUnread";
    QList<QModelIndex> contactIndexList = indexsForBareJid(bareJid);
    foreach (QModelIndex contactIndex, contactIndexList)
    {
        if (getItem(contactIndex)->hasChlidContain(resource))
        {
            QModelIndex resourceIndex = index(getItem(contactIndex)->childIndexOfData(resource), 0, contactIndex);
            getItem(resourceIndex)->setUnread();
            dataChanged(resourceIndex, resourceIndex);
        }
        else
        {
            // resource unknow, let contact unread.
            getItem(contactIndex)->setUnread();
        }
        dataChanged(contactIndex, contactIndex);
    }
}

void RosterModel::messageReaded(const QString &bareJid, const QString &resource)
{
    qDebug() << "messageReaded";

    QList<QModelIndex> contactIndexList = indexsForBareJid(bareJid);
    foreach (QModelIndex contactIndex, contactIndexList)
    {
        if (getItem(contactIndex)->hasChlidContain(resource))
        {
            QModelIndex resourceIndex = index(getItem(contactIndex)->childIndexOfData(resource), 0, contactIndex);
            getItem(resourceIndex)->setUnread(false);
            //dataChanged(resourceIndex, resourceIndex);
        }
        // resource unknow, do nothng.
        dataChanged(contactIndex, contactIndex);
    }
}

void RosterModel::messageReadedAll(const QString &bareJid)
{
    qDebug() << "messageReadedAll";

    QList<QModelIndex> contactIndexList = indexsForBareJid(bareJid);
    foreach (QModelIndex contactIndex, contactIndexList)
    {
        getItem(contactIndex)->setUnread(false);
        foreach(RosterTreeItem *resourceItem, getItem(contactIndex)->childItems())
        {
            resourceItem->setUnread(false);
        }

        if (getItem(contactIndex)->childCount() != 0)
        {
            dataChanged(index(0, 0, contactIndex),
                        index(rowCount(contactIndex), 0, contactIndex));
        }

        dataChanged(contactIndex, contactIndex);
    }
}
/*
 *
 */
void RosterModel::readPref(Preferences *pref)
{
    if (pref->hideOffline != m_hideOffline
        || pref->showResources != m_showResources
        || pref->showSingleResource != m_showSingleResource)
    {
        m_hideOffline = pref->hideOffline;
        m_showResources = pref->showResources;
        m_showSingleResource = pref->showSingleResource;
    }
}

QList<QModelIndex> RosterModel::allIndex(const QModelIndex &parent) const
{
    QList<QModelIndex> results;
    for (int i = 0; i < rowCount(parent); i++)
    {
        QModelIndex child = index(i, 0, parent);
        results << child;
        if (rowCount(child) != 0)
            results << allIndex(child);
    }
    return results;
}

bool RosterModel::isIndexHidden(const QModelIndex &index)
{
    if (index == QModelIndex())
        return false;

    RosterTreeItem *item = getItem(index);
    switch (item->type())
    {
    case Root:
        return false;
    case Group:
        if (item->childCount(m_hideOffline) == 0)
            return true;
        else
            return false;
    case Contact:
        if (m_hideOffline && item->childCount() == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    case Resource:
        if (!m_showResources)
        {
            return true;
        } else if (!m_showSingleResource
                   && item->parent()->childCount() < 2)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool RosterModel::hasVCard(const QString &bareJid) const
{
    return m_vCards.contains(bareJid);
}

QXmppVCardIq RosterModel::getVCard(const QString &bareJid) const
{
    if (hasVCard(bareJid))
        return m_vCards[bareJid];
    else
        return QXmppVCardIq();
}

void RosterModel::clear()
{
    m_vCards.clear();
    m_rootItem->clear();
    reset();
}

QSet<QString> RosterModel::getGroups() const
{
    QSet<QString> sets;
    foreach (RosterTreeItem *item, m_rootItem->childItems())
    {
        sets << item->data();
    }
    //sets.remove(m_noGroupItem->data());
    return sets;
}

// slot
void RosterModel::parseRoster()
{
    qDebug() << "parseRoster" << m_roster->getRosterBareJids();
    initNoGroup();

    foreach (QString bareJid, m_roster->getRosterBareJids())
    {
        QXmppRosterIq::Item entry = m_roster->getRosterEntry(bareJid);
        if (entry.groups().isEmpty())
        {
            RosterTreeItem *item = new RosterTreeItem(RosterModel::Contact, bareJid, m_noGroupItem);
            m_noGroupItem->appendChild(item);
        }
        else
        {
            foreach (QString groupName, entry.groups())
            {
                RosterTreeItem *groupItem;
                if (m_rootItem->hasChlidContain(groupName))
                {
                    groupItem = m_rootItem->child(m_rootItem->childIndexOfData(groupName));
                }
                else
                {
                    groupItem = new RosterTreeItem(Group, groupName, m_rootItem);
                    m_rootItem->appendChild(groupItem);
                }
                RosterTreeItem *item = new RosterTreeItem(RosterModel::Contact, entry.bareJid(), groupItem);
                groupItem->appendChild(item);
            }
        }
    }
    reset();
    emit parseDone();
}

void RosterModel::presenceChangedSlot(const QString &bareJid, const QString &resource)
{
    qDebug() << "presence changed slot";
    QXmppPresence presence = m_roster->getPresence(bareJid, resource);
    //QXmppRoster::QXmppRosterEntry entry = m_roster->getRosterEntry(bareJid);

    QList<QModelIndex> contactIndexList = indexsForBareJid(bareJid);
    foreach (QModelIndex index, contactIndexList)
    {
        parsePresence(index, resource, presence);
    }

    // request vcard if no exist
    if (!m_vCards.contains(bareJid))
    {
        m_vCardManager->requestVCard(bareJid);
    }
}

void RosterModel::rosterChangedSlot(const QString &bareJid)
{
    /*
     * new contact    : add new contact to it's groups, if no group, add to "No Group" group
     * remove contact : remove all contact in groups
     * contact groups changed : remove group no exist, add to new group
     */

    qDebug() << "rosterChangedSlot";
    QList<QModelIndex> indexs = indexsForBareJid(bareJid);
    if (indexs.isEmpty()) {
        qDebug() << QString("[RosterModel] Add New roster: ") << bareJid;
        newContact(bareJid);
    }
    else
    {
        QXmppRosterIq::Item entry = m_roster->getRosterEntry(bareJid);

        if (entry.subscriptionType() == QXmppRosterIq::Item::Remove)
        {
            // clear
            qDebug() << QString("[RosterModel] Clear %1").arg(bareJid);
            foreach (QModelIndex index, indexs)
        {
                removeRow(index.row(), parent(index));
            }
        }
        else
        {
            // add/remove group, update

            // need to parse groups
            QSet<QString> groups = entry.groups();

            if (groups.isEmpty())
            {
                // add group "No Group"
                groups << m_noGroupItem->data();
            }

            foreach (QModelIndex index, indexsForBareJid(bareJid))
            {
                QModelIndex groupIndex = parent(index);
                QString group = getItem(groupIndex)->data();

                if (groups.contains(group))
                {
                    // update a contact in group
                    qDebug() << QString("[RosterModel] Update %1 in %2").arg(bareJid).arg(group);
                    dataChanged(index, index);

                    // had parse
                    groups.remove(group);
                }
                else
                {
                    // remove from a group
                    removeRosterFromGroup(bareJid, group);
                }
                dataChanged(groupIndex, groupIndex);
            }

            // add to new groups
            foreach (QString group, groups)
            {
                insertRosterToGroup(bareJid, group);
            }
        }
    }
    emit hiddenUpdate();
}

void RosterModel::vCardRecived(const QXmppVCardIq &vCard)
{
    m_vCards[vCard.from()] = vCard;
    foreach (QModelIndex index, indexsForBareJid(vCard.from()))
    {
        dataChanged(index, index);
    }

}

void RosterModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    RosterTreeItem *parentItem = getItem(parent);
    RosterTreeItem *childItem = parentItem->child(row);
    parentItem->removeOne(childItem);
    endRemoveRows();
}

void RosterModel::initNoGroup()
{
    qDebug() << "initNoGroup";

    m_noGroupItem = new RosterTreeItem(RosterModel::Group, QString(tr("No Group")), m_rootItem);
    m_rootItem->appendChild(m_noGroupItem);
}

void RosterModel::setClient(QXmppClient *client)
{
    m_client = client;
    m_roster = &client->rosterManager();
    if( m_client->isConnected() )
        qDebug() << "is connected.";
    if( m_roster->isRosterReceived() )
        qDebug() << "is roster received.";

    m_vCardManager = &client->vCardManager();

    connect( m_roster, SIGNAL(presenceChanged(const QString, const QString)),
            this, SLOT(presenceChangedSlot(const QString, const QString)) );
    connect( m_roster, SIGNAL(rosterReceived()),
            this, SLOT(parseRoster()) );
    connect( m_roster, SIGNAL(itemChanged(QString)),
             this, SLOT(rosterChangedSlot(QString)) );

    connect( m_vCardManager, SIGNAL(vCardReceived(const QXmppVCardIq&)),
            this, SLOT(vCardRecived(const QXmppVCardIq&)) );
}

QModelIndex RosterModel::findOrCreateGroup(QString group)
{
    int row;
    RosterTreeItem *groupItem;
    if (m_rootItem->hasChlidContain(group))
    {
        row = m_rootItem->childIndexOfData(group);
        groupItem = m_rootItem->child(row);
    }
    else
    {
        row = rowCount(QModelIndex());
        beginInsertRows(QModelIndex(), row, row);
        groupItem = new RosterTreeItem(Group, group, m_rootItem);
        m_rootItem->appendChild(groupItem);
        endInsertRows();
    }
    return createIndex(row, 0, groupItem);
}

QModelIndex RosterModel::groupIndexFor(const QString &groupName) const
{
    if (hasGroup(groupName))
    {
        int row = m_rootItem->childIndexOfData(groupName);
        return createIndex(row, 0, m_rootItem->child(row));
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex RosterModel::nogroupIndex() const
{
    return createIndex(m_noGroupItem->childNumber(), 0, m_noGroupItem);
}

void RosterModel::insertRosterToGroup(QString bareJid, QString group)
{
    qDebug() << "insertRosterToGroup.";

    QModelIndex groupIndex = findOrCreateGroup(group);
    RosterTreeItem *groupItem = getItem(groupIndex);
    if (groupItem->hasChlidContain(bareJid))
    {
        qDebug() << QString("[RosterModel] Exist %1 in group %2").arg(bareJid).arg(group);
    }
    else
    {
        qDebug() << QString("[RosterModel] Insert %1 to group %2").arg(bareJid).arg(group);
        int row = rowCount(groupIndex);
        beginInsertRows(groupIndex, row, row);
        RosterTreeItem *contactItem = new RosterTreeItem(RosterModel::Contact, bareJid, groupItem);
        groupItem->appendChild(contactItem);
        endInsertRows();
        dataChanged(groupIndex, groupIndex);
        checkRosources(createIndex(contactItem->childNumber(), 0, contactItem));
        sortContact(groupIndex);
    }
}

void RosterModel::removeRosterFromGroup(QString bareJid, QString group)
{
    qDebug() << "removeRosterFromGroup.";

    if (m_rootItem->hasChlidContain(group))
    {
        qDebug() << QString("[RosterModel] Remove %1 from %2").arg(bareJid).arg(group);
        QModelIndex groupIndex = groupIndexFor(group);
        RosterTreeItem *groupItem = getItem(groupIndex);
        if (groupItem->hasChlidContain(bareJid))
        {
            removeRow(groupItem->childIndexOfData(bareJid), groupIndex);
        }
    }
}

bool RosterModel::hasGroup(const QString &groupName) const
{
    return m_rootItem->hasChlidContain(groupName);
}

void RosterModel::newContact(const QString &bareJid)
{
    qDebug() << "new contact.";
    QXmppRosterIq::Item entry = m_roster->getRosterEntry(bareJid);
    if (entry.groups().isEmpty())
    {
        insertRosterToGroup(bareJid, m_noGroupItem->data());
    }
    else
    {
        foreach (QString groupName, entry.groups())
        {
            QModelIndex groupIndex = findOrCreateGroup(groupName);
            insertRosterToGroup(bareJid, groupName);
        }
    }
}

void RosterModel::checkRosources(const QModelIndex &index)
{
    qDebug() << "check rosurces.";
    // only use for contact index
    if (itemTypeAt(index) == RosterModel::Contact)
    {
        RosterTreeItem *contactItem = getItem(index);
        QString bareJid = contactItem->data();
        QStringList resources = m_roster->getResources(bareJid);
        if (!resources.isEmpty())
        {
            beginInsertRows(index, 0, resources.count() -1 );
            foreach (QString resourceName, resources)
            {
                RosterTreeItem *resourceItem = new RosterTreeItem(RosterModel::Resource, resourceName, contactItem);
                contactItem->appendChild(resourceItem);
            }
            endInsertRows();
        }
    }
}

void RosterModel::parsePresence(const QModelIndex &contactIndex, const QString &resource, const QXmppPresence &presence)
{
    qDebug() << "parsePresence.";

    QModelIndex groupIndex = contactIndex.parent();
    RosterTreeItem *contactItem = getItem(contactIndex);

    if (presence.from().isEmpty())
    {
        // Unavaliable
        foreach (RosterTreeItem *resourceItem, contactItem->childItems())
        {
            if (resourceItem->data() == resource)
            {
                removeRow(resourceItem->childNumber(), contactIndex);
                sortContact(groupIndex);
            }
        }
        emit hiddenUpdate();
    }
    else
    {
        if (contactItem->hasChlidContain(resource))
        {
            qDebug() << "hasChlidContain.";
            // update resource
            QModelIndex currentIndex = index(contactItem->childIndexOfData(resource), 0, contactIndex);
            emit dataChanged(currentIndex, currentIndex);
        }
        else
        {
            qDebug() << "add resource.";

            // add resource
            int row = contactItem->childCount();
            beginInsertRows(contactIndex, row, row);
//            RosterTreeItem *resourceItem = new RosterTreeItem(RosterModel::Resource, resource, contactItem);
//            contactItem->appendChild(resourceItem);
            endInsertRows();
            sortContact(groupIndex);
            emit hiddenUpdate();
        }
    }
    emit dataChanged(contactIndex, contactIndex);
}

RosterTreeItem *RosterModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        RosterTreeItem *item = static_cast<RosterTreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return m_rootItem;
}

void RosterModel::sortContact(const QModelIndex &groupIndex)
{
    qDebug() << "sortContact.";
    emit layoutAboutToBeChanged();
    RosterTreeItem *item = getItem(groupIndex);

    if (item->type() == RosterModel::Group)
    {
        item->sortChildren();
    }
    emit layoutChanged();
}

QList<QModelIndex> RosterModel::indexsForBareJid(const QString &bareJid)
{
    qDebug() << "indexsForBareJid.";

    QList<QModelIndex> results;
    foreach (RosterTreeItem *item, m_rootItem->childItems())
    {
        if (item->type() == RosterModel::Contact && item->data() == bareJid)
            results << createIndex(item->childNumber(), 0, item);

        if (item->type() == RosterModel::Group)
        {
            if (item->hasChlidContain(bareJid))
            {
                int row = item->childIndexOfData(bareJid);
                results << createIndex(row, 0, item->child(row));
            }
        }
    }
    return results;
}

QString RosterModel::displayData(const QModelIndex &index) const
{
    qDebug() << "displayData.";

    RosterTreeItem *item = getItem(index);
    if (item->type() == RosterModel::Group)
    {
        return QString("%1 [ %2 / %3 ]").arg(item->data()).arg(item->childCount(true)).arg(item->childCount());
    }
    else if (item->type() == RosterModel::Contact)
    {
        QXmppVCardIq vcard = getVCard(item->data());
        QString name;
        QString rosterName = m_roster->getRosterEntry(item->data()).name();
        QString nickName = vcard.nickName();
        QString fullName = vcard.fullName();
        if (!rosterName.isEmpty())
            name = rosterName;
        else if (!nickName.isEmpty())
            name = nickName;
        else if (!fullName.isEmpty())
            name = fullName;
        else
            name = item->data();

        QString statusText = statusTextAt(index);
        if (statusText.isEmpty())
            return name;
        else
            return QString("%1 \n%2").arg(name).arg(statusTextAt(index));
    }
    else if (item->type() == RosterModel::Resource)
    {
        QString str = item->data();
        if (item->isUnread())
            str = "[*]" + str;
        QString statusText = statusTextAt(index);
        if (statusText.isEmpty())
            return str;
        else
            return QString("%1 \n%2").arg(str).arg(statusText);
    }
    else
    {
        return QString();
    }

}

QString RosterModel::toolTipData(const QModelIndex &index) const
{
    RosterTreeItem *item = getItem(index);
    ItemType type = item->type();

    if (type == Group)
        return QString();

    QXmppRosterIq::Item entry = m_roster->getRosterEntry( jidAt(index) );
    QString subscriptionStr = "";
    switch (entry.subscriptionType()) {
    case QXmppRosterIq::Item::NotSet:
        subscriptionStr = QString(tr("NotSet"));
        break;
    case QXmppRosterIq::Item::None:
        subscriptionStr = QString(tr("None"));
        break;
    case QXmppRosterIq::Item::Both:
        subscriptionStr = QString(tr("Both"));
        break;
    case QXmppRosterIq::Item::From:
        subscriptionStr = QString(tr("From"));
        break;
    case QXmppRosterIq::Item::To:
        subscriptionStr = QString(tr("To"));
        break;
    case QXmppRosterIq::Item::Remove:
        subscriptionStr = QString(tr("Remove"));
        break;
    }

    if (type == RosterModel::Contact)
    {
        QString resource;
        if (item->childCount() == 0)
        {
            resource = QString(tr("No Resource"));
        } else if (item->childCount() == 1)
        {
            resource = item->child(0)->data();
        }
        else
        {
            resource = QString(tr("Multi Resources"));
        }
        return QString(tr("Name: ")) + entry.name() + "\n"
                + QString(tr("Jabber ID: ")) + entry.bareJid() + "\n"
                + QString(tr("Status: ")) + statusTextAt(index) + "\n"
                + QString(tr("Resource: ")) + resource + "\n"
                + QString(tr("Subscription Type: ")) + subscriptionStr;
    }

    if (type == RosterModel::Resource)
    {
        return QString(tr("Name: ")) + entry.name() + "\n"
                + QString(tr("Jabber ID: ")) + entry.bareJid() + "\n"
                + QString(tr("Status: ")) + statusTextAt(index) + "\n"
                + QString(tr("Resource: ")) + item->data() + "\n"
                + QString(tr("Subscription Type: ")) + subscriptionStr;
    }

    return QString();
}

QString RosterModel::statusTextAt(const QModelIndex &index) const
{
    qDebug() << "statusTextAt.";

    RosterTreeItem *item = getItem(index);
    if (item->type() == Group)
        return QString();

    if (item->type() == RosterModel::Resource)
    {
        QXmppPresence presence = m_roster->getPresence( jidAt(index), jidAt(index) );
        if (presence.statusText().isEmpty())
            return QString();
        else
            return QString("%1 %2").arg(presence.type()).arg(presence.statusText());
    }
    else
    {
        if (item->childCount() == 0)
        {
            return QString(tr("Offline"));
        }
        else if (item->childCount() == 1)
        {
            QXmppPresence presence = m_roster->getPresence(item->data(), item->child(0)->data());
            if (presence.statusText().isEmpty())
                return QString();
            else
            {
                return QString("%1 %2").arg(presence.type()).arg(presence.statusText());

            }
        }
        else
        {
            return QString(tr("Multi Status"));
        }
    }
}
