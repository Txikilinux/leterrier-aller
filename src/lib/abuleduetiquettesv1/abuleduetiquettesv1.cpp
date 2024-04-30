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

#include "abuleduetiquettesv1.h"

AbulEduEtiquettesV1::AbulEduEtiquettesV1(const QPointF &position, QWidget *parent) :
    QGraphicsView(parent)
{
    m_localDebug = false;
    m_position = position;
    m_scene = new AbulEduSceneEtiquettesV1(this);
    m_scene->setObjectName("La QGraphicsScene");
    setScene(m_scene);

    // 20160422:ichamsirat:#4809 problème déplacement des cellules 
    setMouseTracking(false);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

#ifdef __ABULEDUTABLETTEV1__MODE__
    //! @note   Icham 19/04/2013 : La scene accepte les touchEvents, plus besoin de scrollBar !
    //!         Pensez à desactiver les scrollBar dans vos codes dans un ifdef Tablette :)
    //!  @note  #2 setDragMode nous active un curseur en forme de petite main, p-ê à désactiver !

#endif

#if QT_VERSION > 0x050000
    /* Permet au touchpad de scroller avec 2 doigts */
    setDragMode(ScrollHandDrag);
    setAttribute(Qt::WA_AcceptTouchEvents);
    QScroller::grabGesture(this, QScroller::TouchGesture);
#endif
}

AbulEduEtiquettesV1::~AbulEduEtiquettesV1()
{
    delete m_scene;
}

QMap<int, AbulEduCelluleV1 *> AbulEduEtiquettesV1::abeEtiquettesDessineTableau(const QPointF &position, int nbColonnes, int largeurColonne, int nbLignes, int hauteurLigne)
{
    QMap<int, AbulEduCelluleV1 *> listeCellules;
    listeCellules.clear();
    for(int j=0; j<nbLignes; j++){
        for(int i=0; i<nbColonnes; i++){
            AbulEduCelluleV1 *cellule = new AbulEduCelluleV1(QSizeF(largeurColonne, hauteurLigne));
            m_scene->addItem(cellule);
            cellule->setPos(i*largeurColonne + position.x(), j*hauteurLigne + position.y());
            cellule->setObjectName(QString::number(j*nbColonnes+i)); //numérotage des cellules
            listeCellules.insert(j*nbColonnes+i, cellule);
        }
    }
    return listeCellules;
}

void AbulEduEtiquettesV1::wheelEvent(QWheelEvent *event)
{
    if (m_localDebug) qDebug()<<"AbulEduEtiquettesV1::wheelEvent(QWheelEvent *event)";
    emit wheelTurn(this->verticalScrollBar()->value()-event->delta());
    event->ignore();
}

void AbulEduEtiquettesV1::abeEtiquettesSetDimensionsWidget(const QSize &taille)
{
    setFixedSize(taille);
    setSceneRect(0,0,taille.width(), taille.height());
}
