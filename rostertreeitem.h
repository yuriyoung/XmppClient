#ifndef TREEITEM_H
#define TREEITEM_H

#include "rostermodel.h"

class RosterTreeItem
{
public:
    RosterTreeItem(RosterModel::ItemType type, QString data, RosterTreeItem *parent = 0);
    ~RosterTreeItem();
    RosterTreeItem *child(int row);
    void appendChild(RosterTreeItem *child);
    bool removeOne(RosterTreeItem *child);
    int childCount(bool hideOffline = false) const;
    QString data() const;
    RosterTreeItem *parent();
    int childNumber() const;
    RosterModel::ItemType type() const { return m_type; }
    QList<RosterTreeItem *> childItems() const { return m_childItems; }
    void sortChildren();
    void setUnread(bool unread = true);
    bool isUnread() const;
    bool hasChlidContain(const QString &data) const;
    int childIndexOfData(const QString &data) const;
    void clear();

private:
    RosterModel::ItemType m_type;
    QString m_data;
    QList<RosterTreeItem*> m_childItems;
    RosterTreeItem *m_parent;
    bool m_unread;

    QList<RosterTreeItem *> onlineChildItems() const; // only use for group
};

#endif // TREEITEM_H
