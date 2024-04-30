#ifndef ABULEDUGRAPHICSTEXTITEM_H
#define ABULEDUGRAPHICSTEXTITEM_H

#include <QGraphicsTextItem>
#include "abuleduloggerv1.h"

class AbulEduGraphicsTextItem : public QGraphicsTextItem
{
public:
    AbulEduGraphicsTextItem(QGraphicsItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // ABULEDUGRAPHICSTEXTITEM_H
