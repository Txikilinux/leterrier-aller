#ifndef ABULEDUPIECEBUTTONV1_H
#define ABULEDUPIECEBUTTONV1_H
#include "abuledupiecev1.h"

class AbulEduPieceButtonV1 : public AbulEduPieceV1
{
    Q_OBJECT
public:
    AbulEduPieceButtonV1(const QSizeF& rect, QGraphicsObject *parent = 0);
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
signals:
    /** @note le paramètre de type AbulEduPieceV1 n'est peut être pas forcément indispensable, mais c'est pour être cohérent avec les signaux signalPiecePressed et signalPieceReleased */
    void signalAbePieceButtonEnterHover(AbulEduPieceV1*);
    void signalAbePieceButtonLeaveHover(AbulEduPieceV1*);
};

#endif // ABULEDUPIECEBUTTONV1_H
