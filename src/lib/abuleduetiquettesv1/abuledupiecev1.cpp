/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
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
#include "abuledupiecev1.h"

AbulEduPieceV1::AbulEduPieceV1(const QRectF &rect, QGraphicsObject *parent) :
    QGraphicsObject(parent)
{
    m_rect = rect;
    initConstructeur();
}

AbulEduPieceV1::AbulEduPieceV1(const QSizeF &rect, QGraphicsObject *parent) :
    QGraphicsObject(parent)
{
    m_rect = QRectF(0,0,rect.width(), rect.height());
    initConstructeur();
}

AbulEduPieceV1::~AbulEduPieceV1()
{
}

void AbulEduPieceV1::initConstructeur()
{
    m_lastCell = 0;
    m_isAcceptMouse = true;
    m_texte = QString("");
    m_font = QFont();
    m_penTexte = QPen(QColor(Qt::black), 1, Qt::SolidLine);
    m_xRadius = 6;
    m_yRadius = 6;
    m_retourPositionOriginale = true;
    /* J'initialise cette valeur à true parce que c'était le comportement avant que je permette de le choisir */
    m_isAcceptWordWrap = true;
    setZValue(10);
    QPixmap pix(":/abuleduetiquettesv1/handCursor");
    if(QFile(":/abuleduetiquettesv1/handCursor").exists()){
            setCursor(QCursor(pix));
    }
    else{
        setCursor(Qt::PointingHandCursor);
    }
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsMovable);
    abePiecePlaceFixe(true);

    m_marge = 6; // Pourquoi pas ?
    abePieceSetPen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
    abePieceSetBrush(Qt::white);
    m_image = QImage();
    abePieceSetImageAlignement(Qt::AlignCenter);
    abePieceSetTexteAlignement(Qt::AlignCenter);
    QGraphicsDropShadowEffect *ombre = new QGraphicsDropShadowEffect(this);

    setGraphicsEffect(ombre);
    ombre->setEnabled(false);
    ombre->setBlurRadius(24);
    ombre->setColor(QColor(63,63,63,100));
    ombre->setOffset(8);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotAbePieceEndTimer()),Qt::UniqueConnection);
}

void AbulEduPieceV1::abePieceSetPositionOriginale(const QPointF &pos)
{
    //Evite le bug du changement de bureau / minimize
    if(m_positionOriginale != pos)
    {
        m_positionOriginale = pos;
        m_lastPosition = pos;
        setPos(pos);
    }
}

void AbulEduPieceV1::abePieceSetPositionOriginale(qreal x, qreal y)
{
    QPointF point(x,y);
    abePieceSetPositionOriginale(point);
}

///////////////////////////////////////////////////////////////////
//                 Dessin et Placement de l'image
///////////////////////////////////////////////////////////////////


void AbulEduPieceV1::abePieceSetImage(const QString &cheminImage, const QPointF &positionImage, const QSize &tailleImage)
{
    m_image = QImage(cheminImage);
    abePieceSetImagePosition(positionImage);
    abePieceSetImageTaille(tailleImage);
    abePieceSetImageAlignement(m_imageAlignement);
}

void AbulEduPieceV1::abePieceSetPixmap(const QPixmap pixmap, const QPointF &positionImage, const QSize &tailleImage)
{
    m_image = QImage(pixmap.toImage());
    abePieceSetImagePosition(positionImage);
    abePieceSetImageTaille(tailleImage);
    abePieceSetImageAlignement(m_imageAlignement);
}

void AbulEduPieceV1::abePieceSetImageAlignement(const Qt::Alignment &alignement)
{
    m_imageAlignement = alignement;
    int VAlignment = (m_imageAlignement/0x20)*0x20; // Supprime les chiffres inférieurs à 0x20

    switch (VAlignment)
    {
    case Qt::AlignBottom :
        m_positionImage.setY(m_rect.height() - m_marge - m_image.height());
        break;
    case Qt::AlignVCenter :
        m_positionImage.setY((m_rect.height()-m_image.size().height())/2);

        break;
    case Qt::AlignCenter :

        m_positionImage.setY((m_rect.height()-m_image.height())/2);
        m_positionImage.setX(m_rect.size().width()-m_image.size().width()/2);
        break;
    case Qt::AlignTop :
        m_positionImage.setY(m_marge);
        break;
    default :
        m_positionImage.setY((m_rect.size().height()-m_image.size().height())/2);
    }

    // Alignement Horizontal
    int hAlignment = m_imageAlignement%8; // Garde les chiffres inférieurs à 8
    switch(hAlignment)
    {
    case Qt::AlignLeft :
        m_positionImage.setX(m_marge);
        break;
    case Qt::AlignHCenter :
        m_positionImage.setX((m_rect.width()-m_image.width())/2);
        break;
    case (Qt::AlignRight) :
        m_positionImage.setX(m_rect.width()-m_image.width()-m_marge);
        break;
    default :
        m_positionImage.setX((m_rect.width()-m_image.size().width())/2);
    }
}

void AbulEduPieceV1::abePieceSetImageTaille(const QSize &tailleImage, const Qt::AspectRatioMode &modeRatio)
{
    if(!tailleImage.isNull()){
        m_image = m_image.scaled(tailleImage, modeRatio, Qt::SmoothTransformation);
    }
    else{
        m_image = m_image.scaled(m_rect.width()-m_marge, m_rect.height()-m_marge, modeRatio, Qt::SmoothTransformation); // il faudrait aussi tenir compte de la demi épaisseur du Pen !!!
    }
}

void AbulEduPieceV1::abePieceSetImagePosition(const QPointF& positionImage)
{
    if(!positionImage.isNull()){
        abePieceSetImageAlignement(0);
        m_positionImage = positionImage;
        m_positionImage.setX(m_positionImage.x() + m_marge); // Ici, on tient compte de la marge
        m_positionImage.setY(m_positionImage.y() + m_marge);
    }
}

///////////////////////////////////////////////////////////////////
//                 Dessin et Placement du texte
///////////////////////////////////////////////////////////////////


void AbulEduPieceV1::abePieceSetTexte(const QString &texte, const QPointF &positionTexte, const QSize &tailleRectTexte)
{
    m_texte = texte;
    m_positionTexte = positionTexte;
    if(tailleRectTexte.isNull()){
        m_tailleRectTexte = QSize(m_rect.width() - m_marge*2,
                                  m_rect.height() - m_marge*2);
    }
    else{
        m_tailleRectTexte = tailleRectTexte;
    }
    abePieceSetTexteAlignement(0);
}

const QPointF& AbulEduPieceV1::abePieceGetCenter()
{
    m_center.setX(pos().x()+abePieceGetSize().width()/2);
    m_center.setY(pos().y()+abePieceGetSize().height()/2);
    return m_center;
}

QPointF AbulEduPieceV1::abePieceMoveToCell(AbulEduCelluleV1 *cellToGo, bool anim)
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__;
    if(anim)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "position");
        animation->setDuration(1000);
        animation->setStartValue(pos());
        animation->setEndValue(QPointF(cellToGo->x()+(cellToGo->abeCelluleGetSize().width() - this->abePieceGetSize().width())/2,
                               cellToGo->y()+(cellToGo->abeCelluleGetSize().height()- this->abePieceGetSize().height())/2));

        animation->start();
    }
    else
    {
    setPos(cellToGo->x()+(cellToGo->abeCelluleGetSize().width() - this->abePieceGetSize().width())/2,
           cellToGo->y()+(cellToGo->abeCelluleGetSize().height()- this->abePieceGetSize().height())/2);
    }
    return pos();
}

void AbulEduPieceV1::slotAbePieceFakeRelease()
{
    mouseReleaseEvent(new QGraphicsSceneMouseEvent());
}

void AbulEduPieceV1::abePieceSetTexteAlignement(const Qt::Alignment& alignement)
{
    m_texteAlignement = alignement;
    m_boundingTexteRect = abePieceTexteBoundingRect();
}

void AbulEduPieceV1::abePieceSetTextePosition(const QPointF& position)
{
    m_positionTexte = position;
    m_texteAlignement = 0;
}

QRectF AbulEduPieceV1::abePieceTexteBoundingRect()
{
    QFontMetrics metrics = QFontMetrics(m_font);
    QRectF rect;
    if(m_texteAlignement != 0){
        QRectF rectUtile = QRectF(m_rect.x() + m_marge,
                                  m_rect.y() + m_marge,
                                  m_rect.width() - m_marge*2,
                                  m_rect.height() - m_marge*2);

        if(m_isAcceptWordWrap) {
            rect = metrics.boundingRect(rectUtile.toRect(), Qt::TextWordWrap | m_texteAlignement, m_texte);
        }
        else {
            rect = metrics.boundingRect(rectUtile.toRect(), m_texteAlignement, m_texte);
        }
    }
    else
    {
        rect = QRectF(qMax(m_positionTexte.x(), m_rect.x() + m_marge),
                      qMax(m_positionTexte.y(), m_rect.y() + m_marge),
                      qMin((qreal)m_tailleRectTexte.width(), m_rect.width() - m_marge*2),
                      qMin((qreal)m_tailleRectTexte.height(), m_rect.height() - m_marge*2));
    }

    return rect;
}

void AbulEduPieceV1::abePieceSetTexteSize(const QSize &tailleTexteRect)
{
    m_tailleRectTexte = tailleTexteRect;
    m_texteAlignement = 0;
}

/////////////////////////////////////////////////////////////////////////
//                       Utilitaires
/////////////////////////////////////////////////////////////////////////


void AbulEduPieceV1::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //Les calculs des rectangles, des images mises à l'échelle se font en dehors du paint
    //Afin d'accélérer le dessin lors des refresh

    // Dessin du fond et du contour
    QPainterPath zoneDessin;
    zoneDessin.addRect(boundingRect());
    painter->setClipPath(zoneDessin); // Limite le dessin à l'"intérieur" de l'étiquette
    painter->setPen(m_penContour); // Le crayon de contour
    painter->setBrush(m_brosseFond); // La couleur du fond
    painter->drawRoundedRect(m_rect, m_xRadius, m_yRadius);

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

QRectF AbulEduPieceV1::boundingRect() const
{
    QRectF rect = QRectF((m_rect.x() - (m_penContour.width() + 1)/2),
                         (m_rect.y() - (m_penContour.width() + 1)/2),
                         (m_rect.width() + m_penContour.width() + 1),
                         (m_rect.height() + m_penContour.width() + 1));
    return rect;
}

void AbulEduPieceV1::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isAcceptMouse)
    {
        m_pressPos = event->pos();
        setZValue(11); // pour être au-dessus des autres
        graphicsEffect()->setEnabled(true);

        m_hotspot = event->scenePos() - scenePos();
        QList<QGraphicsItem *>listeItems = static_cast<QGraphicsScene *>(this->parent())->items(event->scenePos());
        listeItems.removeAt(listeItems.indexOf(this)); // On enleve cette piece de la liste
        if(listeItems.size() > 0 && listeItems[0]->type() == AbulEduCelluleV1::AbulEduCelluleType){ //La pièce est sur une cellule
            static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleSetPiece(0); // On enlève sa référence de la cellule
        }

         m_timer->start(3000);

         ABULEDU_LOG_DEBUG()<<__PRETTY_FUNCTION__<<" a la position : "<<scenePos()<<pos()<<"hotspot"<<m_hotspot;
        emit signalPiecePressed(this);
        event->accept();
    }
}

void AbulEduPieceV1::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
    //    if ((event->pos() - m_pressPos).manhattanLength() < QApplication::startDragDistance())
    //    {
    //        return;
    //    }
    m_timer->stop();
    if (m_isAcceptMouse)
    {
        if (this->flags().testFlag(QGraphicsItem::ItemIsMovable))
            setPos(event->scenePos() - m_hotspot);
    }
    QList<QGraphicsItem *>listeItems = static_cast<QGraphicsScene *>(this->parent())->items(event->scenePos());
    listeItems.removeAt(listeItems.indexOf(this)); // On enleve cette piece de la liste
    if(listeItems.size() != 0)
    {
        foreach(QGraphicsItem* it,listeItems)
        {
            emit signalAbePieceSurvole(it);
            if(static_cast<QGraphicsObject *>(it)->type() == AbulEduCelluleV1::AbulEduCelluleType)
            {
                if(static_cast<AbulEduCelluleV1 *>(it)->abeCelluleIsAcceptePiece())
                {
                    emit signalAbePieceSurvolCellule(m_lastCell, static_cast<AbulEduCelluleV1 *>(it));
                    m_lastCell = static_cast<AbulEduCelluleV1 *>(it);
                    ABULEDU_LOG_DEBUG()<<"cellule survolée"<<static_cast<AbulEduCelluleV1 *>(it)->objectName();
                }
                else
                {
                    emit signalAbePieceSurvolCellule(m_lastCell, 0);
                    m_lastCell = 0;
                }
            }
        }
    }
    else
    {
        emit signalAbePieceSurvolCellule(m_lastCell, 0);
        m_lastCell = 0;
        emit signalAbePieceSurvole(0);
    }
}

void AbulEduPieceV1::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_timer)
        m_timer->stop();
    if (m_isAcceptMouse)
    {
        if (this->flags().testFlag(QGraphicsItem::ItemIsMovable))
        {
            graphicsEffect()->setEnabled(false);
            QList<QGraphicsItem *>listeItems = static_cast<QGraphicsScene *>(this->parent())->items(event->scenePos());
            listeItems.removeAt(listeItems.indexOf(this)); // On enleve cette piece de la liste
            if(listeItems.size() == 0){ // On essaie de déposer la pièce en dehors d'une cellule ou d'un autre objet(autre pièce)
                if(m_retourPositionOriginale){
                    setPos(abePieceGetPositionOriginale());
                }
                else{
                    if(this->collidingItems().size() > 0){
                        setPos(m_lastPosition);
                    }
                    else{
                        m_lastPosition = pos();
                    }
                }
                emit signalAbePieceDeposee(objectName(), -1);
            }
            if(listeItems.size() == 1){
                if(listeItems[0]->type() == AbulEduCelluleV1::AbulEduCelluleType){

                    if(!static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleHasPiece())
                    {//On est sur une cellule vide, on place la piece au mileu
                        if(static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleIsAcceptePiece())
                        {
                            // La cellule accepte le drop de piece
                            setPos(listeItems[0]->x()+(static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleGetSize().width() - this->abePieceGetSize().width())/2,
                                    listeItems[0]->y()+(static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleGetSize().height()-this->abePieceGetSize().height())/2);
                            static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleSetPiece(this);
                            emit signalAbePieceDeposee(objectName(), static_cast<AbulEduCelluleV1 *>(listeItems[0])->objectName().toInt());
                        }
                        else
                        {
                            // La cellule n'accepte pas de piece
                            if(m_retourPositionOriginale){// Cette pièce reprend sa position d'origine
                                setPos(abePieceGetPositionOriginale());
                            }
                            else{
                                setPos(m_lastPosition);
                            }
                            emit signalAbePieceDeposee(objectName(),-1);
                        }
                    }
                    else {// Il y a déjà une pièce, mais on a cliqué entre le bord de la cellule et le bord de la pièce
                        setPos(listeItems[0]->x()+(listeItems[0]->boundingRect().width() - this->boundingRect().width())/2,
                                listeItems[0]->y()+(listeItems[0]->boundingRect().height()-this->boundingRect().height())/2);
                        //C'est le code des lignes ci-dessous qui provoquait les deux gros bugs d'Aller
                        //                    static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleHasPiece()->setPos(static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleHasPiece()->abePieceGetPositionOriginale());
                        //                    emit abePieceDeposee(objectName(), static_cast<AbulEduCelluleV1 *>(listeItems[0])->objectName().toInt());
                        //                    static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleSetPiece(this);
                        //                    emit abePieceDeposee(static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleHasPiece()->objectName(), -1);
                    }
                }
                else if(listeItems[0]->type() == AbulEduPieceV1::AbulEduPieceType){ // On est sur une pièce qui n'est pas dans une cellule
                    if(m_retourPositionOriginale){// Cette pièce reprend sa position d'origine
                        setPos(abePieceGetPositionOriginale());
                    }
                    else{
                        if(this->collidingItems().size() > 0){
                            setPos(m_lastPosition);
                        }
                        else{
                            m_lastPosition = pos();
                        }
                    }
                    emit signalAbePieceDeposee(objectName(),-1);
                }
            }

            if(listeItems.size() == 2)
            {//il y a déjà une piece dans la cellule et on a déposé sur cette pièce
                //            if(listeItems[0]->type() == AbulEduPieceV1::AbulEduPieceType)
                //            { // Le premier item est une pièce : on la remet à sa positon originale
                //                listeItems[0]->setPos(static_cast<AbulEduPieceV1 *>(listeItems[0])->abePieceGetPositionOriginale());
                //                emit abePieceDeposee(static_cast<AbulEduPieceV1 *>(listeItems[0])->objectName(), -1);
                //                listeItems.removeAt(0);
                //            }
                //            else{
                //                listeItems[1]->setPos(static_cast<AbulEduPieceV1 *>(listeItems[1])->abePieceGetPositionOriginale());
                //                emit abePieceDeposee(static_cast<AbulEduPieceV1 *>(listeItems[1])->objectName(), -1);
                //            }
                //            setPos(listeItems[0]->x()+(listeItems[0]->boundingRect().width() - this->boundingRect().width())/2,
                //                   listeItems[0]->y()+(listeItems[0]->boundingRect().height()-this->boundingRect().height())/2);
                //            emit abePieceDeposee(objectName(), static_cast<AbulEduCelluleV1 *>(listeItems[0])->objectName().toInt());
                //            static_cast<AbulEduCelluleV1 *>(listeItems[0])->abeCelluleSetPiece(this);
                if(m_retourPositionOriginale){
                    setPos(abePieceGetPositionOriginale());
                }
                else{
                    if(this->collidingItems().size() > 0){
                        setPos(m_lastPosition);
                    }
                    else{
                        m_lastPosition = pos();
                    }
                }
                emit signalAbePieceDeposee(objectName(), -1);
            }

            setZValue(10); // On la remet à son niveau initial
        }
        emit signalAbePieceCliquee(this);
        emit signalPieceReleased(this);
    }
}

void AbulEduPieceV1::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    emit signalAbePieceDoubleCliquee();
}

void AbulEduPieceV1::slotAbePieceEndTimer()
{
    emit signalAbePieceEndTimer();
}

QVariant AbulEduPieceV1::itemChange(const GraphicsItemChange& change, const QVariant &value)
{
    /* 20150601 : Philippe : Tentative de résolution des étiquettes qui se barrent comme dans les exemples Qt récupérés par JL pour Images sequentielles mais qui ne marche pas ici
       Je le commite tout de même, il y a sûrement un problème de condition qu'on trouvera bien un jour
       En attendant pour ALLER j'ai utilisé un bricolage */
//    qDebug()<<" ########################################";
//    qDebug()<<"ItemChange pour "<<objectName();
//    qDebug()<<" ########################################";

    /* Pourrait être fonction d'une condition ? */
    bool contrainte = true;
    if (change == ItemPositionChange && scene()) {
        /* value est la nouvelle position. */
        QPointF newPos = value.toPointF();
        if(contrainte)
        {
            QRectF rect = scene()->sceneRect();
            /* Le coin haut gauche de l'étiquette doit rester dans un rectangle plus petit que la zone */
            rect.setWidth(rect.width()-abePieceGetSize().width());
            rect.setHeight(rect.height()-abePieceGetSize().height());
            if (!rect.contains(newPos))
            {
                /* Garde l'étiquette dans la zone */
                newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
                newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            }
//            emit etiquetteChange(change, boundingRect().center().toPoint());
            return newPos; /* C'est bien un QVariant ! */
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void AbulEduPieceV1::abePieceSetRadius(int xRadius, int yRadius)
{
    m_xRadius = xRadius;
    m_yRadius = yRadius;
}
