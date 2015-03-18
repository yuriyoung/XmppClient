#include "rostertreeitem.h"

#include <QStringList>
#include <QDebug>

bool TreeItemCompare(RosterTreeItem *s1, RosterTreeItem *s2)
{
    if (s1->type() == RosterModel::Contact && s2->type() == RosterModel::Contact)
    {
        //return s1->childCount() >= s2->childCount();
        if (s1->childCount() == s2->childCount())
            return s1 < s2;
        else
            return s1->childCount() >= s2->childCount();
    }
    return true;
}

RosterTreeItem::RosterTreeItem(RosterModel::ItemType type, QString data, RosterTreeItem *parent)
    : m_type(type), m_data(data), m_parent(parent), m_unread(false)
{

}

RosterTreeItem::~RosterTreeItem()
{
    qDeleteAll( m_childItems );
}

RosterTreeItem *RosterTreeItem::child(int row)
{
    return m_childItems.value( row );
}

void RosterTreeItem::appendChild(RosterTreeItem *child)
{
    m_childItems.append(child);
}

bool RosterTreeItem::removeOne(RosterTreeItem *child)
{
    bool ok = m_childItems.removeOne(child);
    if (ok)
        delete child;
    return ok;
}

int RosterTreeItem::childCount(bool hideOffline) const
{
    if (hideOffline && m_type == RosterModel::Group)
    {
        return onlineChildItems().count();
    }

    return m_childItems.count();
}

QString RosterTreeItem::data() const
{

    return m_data;
}

RosterTreeItem *RosterTreeItem::parent()
{
    return m_parent;
}

int RosterTreeItem::childNumber() const
{
    if (m_parent)
        return m_parent->childItems().indexOf(const_cast<RosterTreeItem*>(this));

    return 0;
}

void RosterTreeItem::sortChildren()
{
    qSort(m_childItems.begin(), m_childItems.end(), TreeItemCompare);
}

void RosterTreeItem::setUnread(bool unread)
{
    m_unread = unread;
}

bool RosterTreeItem::isUnread() const
{
    if (m_type == RosterModel::Contact)
    {
        if (m_childItems.count() != 0)
        {
            foreach (RosterTreeItem *childItem, m_childItems)
            {
                if (childItem->isUnread())
                    return true;
            }
        }
    }
    else if (m_type == RosterModel::Resource)
    {
        return m_unread;
    }
    return false;
}

bool RosterTreeItem::hasChlidContain(const QString &data) const
{
    foreach (RosterTreeItem *resourceItem, childItems())
    {
        if (resourceItem->data() == data)
        {
            return true;
        }
    }
    return false;
}

int RosterTreeItem::childIndexOfData(const QString &data) const
{
    if (hasChlidContain(data))
    {
        foreach (RosterTreeItem *resourceItem, childItems())
        {
            if (resourceItem->data() == data)
            {
                return m_childItems.indexOf(resourceItem);
            }
        }
    }
    return -1;
}

void RosterTreeItem::clear()
{
    qDeleteAll(m_childItems);
    m_childItems.clear();
}

QList<RosterTreeItem *> RosterTreeItem::onlineChildItems() const
{
    QList<RosterTreeItem *> items;
    if (m_type != RosterModel::Group)
        return items;
    foreach (RosterTreeItem *item, m_childItems)
    {
        if (item->childCount() != 0)
        {
            items.append(item);
        }
    }

    return items;
}
