/**
  * AbulEduLabelV1
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011-2019 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2019 Eric Seigne <eric.seigne@abuledu.org>
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

#include "abuledulabelv1.h"
#include "abuledulabelframev1.h"

#ifndef DEBUG_ABULEDULABELV1
    #include "abuledudisableloggerv1.h"
#endif

AbuleduLabelV1::AbuleduLabelV1(QWidget *parent) :
        QLabel(parent)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;

    m_defaultStyleSheet = "";
    m_selectedStyleSheet = abe::STYLESELECTION;
    m_errorStyleSheet = abe::STYLEERREUR;
}

AbuleduLabelV1::AbuleduLabelV1(const QString &texte, int numOrdre, int numPlace, const QFont &font, QWidget *parent) :
        QLabel(parent)
{
    Q_UNUSED(font)
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;

    m_mimecode="abuledulabel/x-abuledu";
    m_defaultStyleSheet = "";
    m_selectedStyleSheet = abe::STYLESELECTION;
    m_errorStyleSheet = abe::STYLEERREUR;
    setObjectName("AbuleduLabel");
    setText(texte);
    abelabelSetNumOrdre(numOrdre);
    abelabelSetNumPlace(numPlace);
    abelabelSetLastPos(QPoint(0,0));
    abelabelSetMarge(10);
    setFrameShape(QFrame::Box);
    setCursor(Qt::PointingHandCursor);
    setAcceptDrops(false);
    setWordWrap(true);

    if (parent)
    {
        setMaximumWidth(parent->width());
    }
    setWordWrap(true);
    connect(this, SIGNAL(abuleduLabelSelected(int)), parentWidget(), SLOT(on_abuleduLabelSelected(int)),Qt::UniqueConnection);
    connect(this, SIGNAL(AbulEduLabel_moved(AbuleduLabelV1 *)), parentWidget(), SLOT(on_AbulEduLabel_moved(AbuleduLabelV1 *)), Qt::UniqueConnection);
}

AbuleduLabelV1::~AbuleduLabelV1()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
}

void AbuleduLabelV1::abelabelSetNumOrdre(int intOrdre)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    m_intOrdre=intOrdre;
}

int AbuleduLabelV1::abelabelNumOrdre()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    return m_intOrdre;
}

void AbuleduLabelV1::abelabelSetNumPlace(int intPlace)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    m_intPlace=intPlace;
}

//-----------------------------------------------------
//
//             Gestion du Drag & Drop
//
//-----------------------------------------------------

void AbuleduLabelV1::mousePressEvent(QMouseEvent *event)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    if(!acceptDrops())
    {
        return; // Ne fait rien si l'exo n'est pas commencé
    }
    else
    {
        m_lastMousePos=event->pos();

        // Déselectionner les étiquettes
        QList<AbuleduLabelV1 *> listeEtiquettes = this->parentWidget()->findChildren<AbuleduLabelV1 *>();
        foreach(AbuleduLabelV1* etiq, listeEtiquettes)
        {
            etiq->setStyleSheet(m_defaultStyleSheet);
        }
        this->setStyleSheet(m_selectedStyleSheet);
        emit abuleduLabelSelected(this->abelabelNumPlace());
        qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__<<abelabelNumPlace();
    }
}

void AbuleduLabelV1::mouseMoveEvent(QMouseEvent *event)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    // Démarre-t-on un drag ?
    if (!acceptDrops())
        return;
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_lastMousePos).manhattanLength()
        < QApplication::startDragDistance())
        return;

    // On crée un drag minimum car il ne servira pas, à moins qu'on décide autrement !

    QPoint hotSpot = event->pos();
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData; // Faudra peut-être un mimetype abuledu
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << m_mimecode << styleSheet() ;

    mimeData->setData(m_mimecode, itemData);
    mimeData->setText(this->text());
    drag->setHotSpot(hotSpot);

    drag->setMimeData(mimeData);
    QPixmap pix(this->size());
    //J'essaye de créer une région qui serait un roundedRect avec 10 comme radius, et de l'utiliser dans render
//    QRegion region(0, 0, width(), height(), QRegion::Rectangle);
//    QRegion round (0, 0, 2*10, 2*10, QRegion::Ellipse);
//    QRegion corner(0, 0, 10, 10, QRegion::Rectangle);
//    region = region.subtracted(corner.subtracted(round));
//    round = QRegion(width()-2*10, 0, 2*10, 2*10, QRegion::Ellipse);
//    corner = QRegion(width()-10, 0, 10, 10, QRegion::Rectangle);
//    region = region.subtracted(corner.subtracted(round));
//    round = QRegion(width()-2*10, height()-2*10, 2*10, 2*10, QRegion::Ellipse);
//    corner = QRegion(width()-10, height()-10, 10, 10, QRegion::Rectangle);
//    region = region.subtracted(corner.subtracted(round));
//    round = QRegion(0, height()-2*10, 2*10, 2*10, QRegion::Ellipse);
//    corner = QRegion(0, height()-10, 10, 10, QRegion::Rectangle);
//    region = region.subtracted(corner.subtracted(round));
//    this->render(&pix,QPoint(),region, !QWidget::IgnoreMask);

#if QT_VERSION < 0x050900
    this->render(&pix,QPoint(),QRect(), !QWidget::DrawWindowBackground);
#else
    //Solution JLF 20190804
    QFlags<QWidget::RenderFlag> option;
    option.setFlag(QWidget::DrawWindowBackground, false);
    this->render(&pix,QPoint(),QRect(), option);
#endif

    drag->setPixmap(pix);
    this->hide();
    drag->exec(Qt::CopyAction | Qt::CopyAction | Qt::MoveAction);

    this->move(this->abelabelLastPos());
    this->show(); // Si l'étiquette est dropée ailleurs, on la réaffiche
    emit abuleduLabelSelected(this->abelabelNumPlace());
    qDebug()<<"AbulEduLabel_moved(static_cast<AbuleduLabel *>(event->source())"<<this;
    emit AbulEduLabel_moved(this);
    event->accept();
}

void AbuleduLabelV1::dragEnterEvent(QDragEnterEvent * event)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;

    /// @todo Les deux lignes ci-dessous créent une fuite mémoire
    /// suggestion: on les supprime donc en fin de fonction ?
    AbuleduLabelV1* etiquetteDessous = new AbuleduLabelV1();
    AbuleduLabelV1* etiquetteDessus = new AbuleduLabelV1();
    // On vérifie que l'on a bien une étiquette
    if(!event->mimeData()->formats().contains(m_mimecode))
    {
        event->ignore();
    }
    else // On déplace l'étiquette (ou pas)
    {
        if(event->source()->parent() != this->parentWidget())
        {
            event->ignore();
        }
        else
        {
            AbuleduLabelV1 *etiqCastee = static_cast<AbuleduLabelV1 *>(event->source());

            int place=0;

            //Pour chaque étiquette comprise entre les deux, récupérer l étiquette qui a le numPlace au dessus et les échanger, jusqu a ce que l étiquette au dessus soit l etiquette survolee

            if (etiqCastee->abelabelNumPlace() > this->abelabelNumPlace()) {
                QList<AbuleduLabelV1 *> listeEtiquettes = this->parentWidget()->findChildren<AbuleduLabelV1 *>();
//                AbuleduLabelV1* etiquetteDessus = new AbuleduLabelV1();
                /** @warning ce while n'a pas de condition de sortie et donc peut nous péter dans les doigts en cas de truc mal fait en amon */
                while (etiquetteDessus != this) {
                    foreach(AbuleduLabelV1* etiq, listeEtiquettes)
                    {
                        if (etiq->abelabelNumPlace() == etiqCastee->abelabelNumPlace() - 1)
                        {
                            etiquetteDessus = etiq;

                        }
                    }
                    if(static_cast<AbulEduLabelFrameV1 *>(this->parentWidget())->abelabelOrientation() == Qt::Vertical)
                    {
                        etiquetteDessus->move(etiqCastee->abelabelLastPos().x() - etiquetteDessus->width() + etiqCastee->width(), etiqCastee->abelabelLastPos().y() - etiquetteDessus->height() + etiqCastee->height());
                    }
                    else // liste horizontale
                    {
                        etiquetteDessus->move(etiqCastee->abelabelLastPos().x() - etiquetteDessus->width() + etiqCastee->width(), etiqCastee->abelabelLastPos().y());
                    }
                    place = etiquetteDessus->abelabelNumPlace();
                    etiquetteDessus->abelabelSetNumPlace(etiqCastee->abelabelNumPlace());
                    etiqCastee->abelabelSetNumPlace(place);
                    etiqCastee->abelabelSetLastPos(etiquetteDessus->abelabelLastPos()); // On affecte les nouvelles positions
                    etiquetteDessus->abelabelSetLastPos(etiquetteDessus->pos());
                }
            }
            else {
                QList<AbuleduLabelV1 *> listeEtiquettes = this->parentWidget()->findChildren<AbuleduLabelV1 *>();
//                AbuleduLabelV1* etiquetteDessous = new AbuleduLabelV1();
                while (etiquetteDessous != this) {
                    foreach(AbuleduLabelV1* etiq, listeEtiquettes)
                    {
                        if (etiq->abelabelNumPlace() == etiqCastee->abelabelNumPlace() + 1)
                        {
                            etiquetteDessous = etiq;
                        }
                    }
                    etiquetteDessous->move(etiqCastee->abelabelLastPos());
                    place = etiquetteDessous->abelabelNumPlace();
                    etiquetteDessous->abelabelSetNumPlace(etiqCastee->abelabelNumPlace());
                    etiqCastee->abelabelSetNumPlace(place);
                    if(static_cast<AbulEduLabelFrameV1 *>(this->parentWidget())->abelabelOrientation() == Qt::Vertical)
                    {
                        etiqCastee->abelabelSetLastPos(etiquetteDessous->abelabelLastPos().x() + etiquetteDessous->width() - etiqCastee->width(),etiquetteDessous->abelabelLastPos().y() + etiquetteDessous->height() - etiqCastee->height()); // On affecte les nouvelles positions
                    }
                    else //liste horizontale
                    {
                        etiqCastee->abelabelSetLastPos(etiquetteDessous->abelabelLastPos().x() + etiquetteDessous->width() - etiqCastee->width(),etiquetteDessous->abelabelLastPos().y() );
                    }
                    etiquetteDessous->abelabelSetLastPos(etiquetteDessous->pos());

                }
            }
        }
    }
}

/** Utilisé uniquement pour faire un signal clic */
void AbuleduLabelV1::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    emit clicked();
}
