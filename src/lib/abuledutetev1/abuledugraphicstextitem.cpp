#include "abuledugraphicstextitem.h"
#include <QPainter>

#ifndef DEBUG_ABULEDUTETEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduGraphicsTextItem::AbulEduGraphicsTextItem(QGraphicsItem *parent)
    :QGraphicsTextItem(parent)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
}

void AbulEduGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    painter->setRenderHint(QPainter::Antialiasing);
    QGraphicsTextItem::paint(painter,option,widget);
}
