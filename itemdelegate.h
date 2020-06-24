#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QStyledItemDelegate>

class ItemDelegate : public QStyledItemDelegate
{
  public:
    ItemDelegate();
    using QStyledItemDelegate::QStyledItemDelegate;
    bool displayRoleEnabled = false;

  protected:
    void initStyleOption(QStyleOptionViewItem *o, const QModelIndex &idx) const override
    {
      QStyledItemDelegate::initStyleOption(o, idx);
      if (!displayRoleEnabled)
        o->features &= ~QStyleOptionViewItem::HasDisplay;
    }
};

#endif // ITEMDELEGATE_H
