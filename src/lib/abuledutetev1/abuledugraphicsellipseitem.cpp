#include "abuledugraphicsellipseitem.h"

#ifndef DEBUG_ABULEDUTETEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduGraphicsEllipseItem::AbulEduGraphicsEllipseItem(int number, qreal x, qreal y, qreal s, QGraphicsItem *parent)
    : QGraphicsEllipseItem(x,y,s,s,parent)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_number = number;
    m_size = s;
}

void AbulEduGraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    painter->setRenderHint(QPainter::Antialiasing);
    QGraphicsEllipseItem::paint(painter,option,widget);
    QFont f = abeApp->font();
    f.setPointSizeF(m_size/2);
    painter->setFont(f);
    painter->drawText((m_size-f.pointSizeF())/2,(m_size-f.pointSizeF())*1.4,QString::number(m_number));
}
