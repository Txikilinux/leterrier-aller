#ifndef ABULEDUGRAPHICSELLIPSEITEM_H
#define ABULEDUGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QPainter>
#include "abuleduapplicationv1.h"
#include "abuleduloggerv1.h"

class AbulEduGraphicsEllipseItem : public QGraphicsEllipseItem
{
public:
    /** Constructeur de la classe, un QGraphicsEllipseItem rond avec un entier écrit dedans */
    AbulEduGraphicsEllipseItem(int number,qreal x, qreal y, qreal s, QGraphicsItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    /** Elle est destinée à afficher un entier */
    int m_number;
    /** On la construit carrée */
    int m_size;
};

#endif // ABULEDUGRAPHICSELLIPSEITEM_H
