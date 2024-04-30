/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @see The GNU Public License (GPL)
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program; if not, write to the Free Software Foundation, Inc.,
  * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  */

#include "abuleducellulev1.h"

AbulEduCelluleV1::AbulEduCelluleV1(const QSizeF &taille, QGraphicsObject *parent) :
    QGraphicsObject(parent)
{
    m_localDebug = false;
    m_hoverBrosseFond = QBrush(QColor("#eec77c"));
    m_taille = QRectF(0,0,taille.width(), taille.height());
    m_piece = 0;
    m_marge = 2;
    m_xRadius = 0;
    m_yRadius = 0;
    m_acceptePiece = true;
    m_image = QImage();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotAbeCelluleEndTimer()),Qt::UniqueConnection);
    abeCelluleSetImageAlignement(Qt::AlignCenter);
    m_imageAlignement = Qt::AlignCenter;
    abeCelluleSetTexte("");
    abeCelluleSetTexteAlignement(Qt::AlignCenter);
    abeCelluleAcceptePiece(true);
    setAcceptHoverEvents(true);
}

AbulEduCelluleV1::~AbulEduCelluleV1()
{
}

void AbulEduCelluleV1::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //    painter->drawRect(0, 0, m_taille.width(), m_taille.height());
    //Les calculs des rectangles, des images mises à l'échelle se font en dehors du paint
    //Afin d'accélérer le dessin lors des refresh

    // Dessin du fond et du contour
    QPainterPath zoneDessin;
    zoneDessin.addRect(boundingRect());
    painter->setClipPath(zoneDessin); // Limite le dessin à l'"intérieur" de l'étiquette
    painter->setPen(m_penContour); // Le crayon de contour
    painter->setBrush(m_brosseFond); // La couleur du fond
    painter->drawRoundedRect(m_taille,m_xRadius,m_yRadius, Qt::RelativeSize);

    // Dessin d'une image au dessus du fond  mise à l'échelle et proportionnelle
    if(!m_image.isNull()){
        painter->drawImage(m_positionImage, m_image);
    }

    // Dessin du texte
    if(m_texte.size() > 0){
        painter->setFont(m_font);
        painter->setPen(m_penTexte);
        painter->drawText(m_boundingTexteRect, Qt::TextWordWrap,m_texte);
    }
}

void AbulEduCelluleV1::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    emit signalAbeCelluleMouseDoubleClicked();
}

QRectF AbulEduCelluleV1::boundingRect() const
{
    return QRectF((m_taille.x() - (m_penContour.width() + 1)/2),
                  (m_taille.y() - (m_penContour.width() + 1)/2),
                  (m_taille.width() + 2 + m_penContour.width() + 1),
                  (m_taille.height() + 2 + m_penContour.width() + 1));

}

void AbulEduCelluleV1::abeCelluleSetPiece(AbulEduPieceV1 *piece)
{
    m_piece = piece;
    if (m_piece)
    {
    if (m_localDebug) qDebug() <<"On a glisse la piece"<<m_piece->objectName()<<"sur moi :"<<objectName();

        if (QString::compare(m_piece->objectName(), objectName()) == 0)
        {
            emit signalAbeCelluleDropResult(abe::evalA);
        }
        else
        {
            emit signalAbeCelluleDropResult(abe::evalD);
        }
        abeCelluleSetBrush(m_initialBrosseFond);
        update(boundingRect());
    }
}

AbulEduPieceV1 * AbulEduCelluleV1::abeCelluleHasPiece()
{
    return m_piece;
}

void AbulEduCelluleV1::abeCelluleAcceptePiece(bool yesNo)
{
    m_acceptePiece = yesNo;
}

bool AbulEduCelluleV1::abeCelluleIsAcceptePiece()
{
    return m_acceptePiece;
}

void AbulEduCelluleV1::abeCelluleSetImage(const QString &cheminImage, const QPointF &positionImage, const QSize &tailleImage)
{
    m_image = QImage(cheminImage);
    abeCelluleSetImagePosition(positionImage);
    abeCelluleSetImageTaille(tailleImage);
    abeCelluleSetImageAlignement(m_imageAlignement);
}

void AbulEduCelluleV1::abeCelluleSetPixmap(const QPixmap pixmap, const QPointF &positionImage, const QSize &tailleImage)
{
    m_image = QImage(pixmap.toImage());
    abeCelluleSetImagePosition(positionImage);
    abeCelluleSetImageTaille(tailleImage);
    abeCelluleSetImageAlignement(m_imageAlignement);
}

void AbulEduCelluleV1::abeCelluleSetImageAlignement(const Qt::Alignment &alignement)
{
    m_imageAlignement = alignement;
    int VAlignment = (m_imageAlignement/0x20)*0x20; // Supprime les chiffres inférieurs à 0x20

    switch (VAlignment)
    {
    case Qt::AlignBottom :
        m_positionImage.setY(m_taille.height() - m_marge - m_image.height());
        break;
    case Qt::AlignVCenter :
        if (m_localDebug)
      qDebug()<<"AlignVCenter";
        m_positionImage.setY((m_taille.height()-m_image.size().height())/2);

        break;
    case Qt::AlignCenter :
        if (m_localDebug)
      qDebug()<<"AlignCenter";
        m_positionImage.setY((m_taille.height()-m_image.height())/2);
        m_positionImage.setX(m_taille.size().width()-m_image.size().width()/2);
        break;
    case Qt::AlignTop :
        if (m_localDebug)
      qDebug()<<"AlignTop";
        m_positionImage.setY(m_marge);
        break;
    default :
        m_positionImage.setY((m_taille.size().height()-m_image.size().height())/2);
    }

    // Alignement Horizontal
    int hAlignment = m_imageAlignement%8; // Garde les chiffres inférieurs à 8
    switch(hAlignment)
    {
    case Qt::AlignLeft :
        if (m_localDebug)
      qDebug()<<"AlignLeft";
        m_positionImage.setX(m_marge);
        break;
    case Qt::AlignHCenter :
        if (m_localDebug)
      qDebug()<<"AlignHCenter";
        m_positionImage.setX((m_taille.width()-m_image.width())/2);
        break;
    case (Qt::AlignRight) :
        if (m_localDebug)
      qDebug()<<"AlignRight";
        m_positionImage.setX(m_taille.width()-m_image.width()-m_marge);
        break;
    default :
        m_positionImage.setX((m_taille.width()-m_image.size().width())/2);
    }
}

void AbulEduCelluleV1::abeCelluleSetImageTaille(const QSize &tailleImage, const Qt::AspectRatioMode &modeRatio)
{
        if(!tailleImage.isNull())
        {
            m_image = m_image.scaled(tailleImage, modeRatio, Qt::SmoothTransformation);
        }
        else{
            m_image = m_image.scaled(m_taille.width()-m_marge, m_taille.height()-m_marge, modeRatio, Qt::SmoothTransformation); // il faudrait aussi tenir compte de la demi épaisseur du Pen !!!
        }
}

void AbulEduCelluleV1::abeCelluleSetImagePosition(const QPointF &positionImage)
{
    if(!positionImage.isNull()){
        abeCelluleSetImageAlignement(0);
        m_positionImage = positionImage;
        m_positionImage.setX(m_positionImage.x() + m_marge); // Ici, on tient compte de la marge
        m_positionImage.setY(m_positionImage.y() + m_marge);
    }
}

void AbulEduCelluleV1::abeCelluleSetMarge(int marge)
{
    m_marge = marge;
}

void AbulEduCelluleV1::abeCelluleSetTexte(const QString &texte, const QPointF &positionTexte, const QSize &tailleRectTexte)
{
    m_texte = texte;
    m_positionTexte = positionTexte;
    if(tailleRectTexte.isNull()){
        m_tailleRectTexte = QSize(m_taille.width() - m_marge*2,
                                  m_taille.height() - m_marge*2);
    }
    else{
        m_tailleRectTexte = tailleRectTexte;
    }
    abeCelluleSetTexteAlignement(0);
}

void AbulEduCelluleV1::abeCelluleSetTexteAlignement(const Qt::Alignment &alignement)
{
    m_texteAlignement = alignement;
    m_boundingTexteRect = abeCelluleTexteBoundingRect();
}

void AbulEduCelluleV1::abeCelluleSetTextePosition(const QPointF &position)
{
    m_positionTexte = position;
    m_texteAlignement = 0;
}

void AbulEduCelluleV1::abeCelluleSetTexteSize(const QSize &tailleTexteRect)
{
    m_tailleRectTexte = tailleTexteRect;
    m_texteAlignement = 0;
}

QRectF AbulEduCelluleV1::abeCelluleTexteBoundingRect()
{
    QFontMetrics metrics = QFontMetrics(m_font);
    QRectF rect;
    if(m_texteAlignement != 0){
        QRectF rectUtile = QRectF(m_taille.x() + m_marge,
                                  m_taille.y() + m_marge,
                                  m_taille.width() - m_marge*2,
                                  m_taille.height() - m_marge*2);

        rect = metrics.boundingRect(rectUtile.toRect(), Qt::TextWordWrap | m_texteAlignement, m_texte);
    }
    else
    {
        rect = QRectF(qMax(m_positionTexte.x(), m_taille.x() + m_marge),
                      qMax(m_positionTexte.y(), m_taille.y() + m_marge),
                      qMin((qreal)m_tailleRectTexte.width(), m_taille.width() - m_marge*2),
                      qMin((qreal)m_tailleRectTexte.height(), m_taille.height() - m_marge*2));
    }

    return rect;
}

void AbulEduCelluleV1::abeCelluleSetRadius(int xRadius, int yRadius)
{
    m_xRadius = xRadius;
    m_yRadius = yRadius;
}

void AbulEduCelluleV1::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    m_timer->start(3000);
}

void AbulEduCelluleV1::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    m_timer->stop();
}

void AbulEduCelluleV1::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
}

int AbulEduCelluleV1::abeCelluleGetMarge()
{
    return m_marge;
}

void AbulEduCelluleV1::slotAbeCellulePieceSurvol(QGraphicsItem * item)
{
    if(item == 0 || item != this)
    {
        abeCelluleSetBrush(m_initialBrosseFond);
        update(boundingRect());
    }
    else
    {
        abeCelluleSetBrush(m_hoverBrosseFond);
        update(boundingRect());
    }
}

void AbulEduCelluleV1::slotAbeCelluleEndTimer()
{
    emit signalAbeCelluleEndTimer();
}
