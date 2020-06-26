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
    void initStyleOption(QStyleOptionViewItem *_styleOption, const QModelIndex &_modInd) const override
    {
      QStyledItemDelegate::initStyleOption(_styleOption, _modInd);
      if (!displayRoleEnabled)
        _styleOption->features &= ~QStyleOptionViewItem::HasDisplay;
    }
};

#endif // ITEMDELEGATE_H
