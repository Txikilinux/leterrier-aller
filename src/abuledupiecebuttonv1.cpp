#include "abuledupiecebuttonv1.h"

AbulEduPieceButtonV1::AbulEduPieceButtonV1(const QSizeF& rect, QGraphicsObject *parent)
    : AbulEduPieceV1(rect,parent)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable,false);
    abePieceSetPen(Qt::NoPen);
    setZValue(+1000);
    QGraphicsDropShadowEffect *ombreTransparente = new QGraphicsDropShadowEffect(this);
    setGraphicsEffect(ombreTransparente);
    ombreTransparente->setEnabled(false);
    ombreTransparente->setColor(QColor(0,0,0,0));
    setGraphicsEffect(ombreTransparente);
}

void AbulEduPieceButtonV1::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    qDebug()<< __FILE__ <<  __LINE__ << __FUNCTION__;
    emit signalAbePieceButtonEnterHover(this);
}

void AbulEduPieceButtonV1::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    qDebug()<< __FILE__ <<  __LINE__ << __FUNCTION__;
    emit signalAbePieceButtonLeaveHover(this);
}
