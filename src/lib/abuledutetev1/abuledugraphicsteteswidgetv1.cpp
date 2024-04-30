/**
  * Outils(le terrier d'AbulEdu)
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduTeteV1
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011-2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GPL)

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

#include "abuledugraphicsteteswidgetv1.h"

#ifndef DEBUG_ABULEDUTETEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduGraphicsTetesWidgetV1::AbulEduGraphicsTetesWidgetV1(int nbTete, int taille,QGraphicsWidget *parent ) :
    QGraphicsWidget(parent)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    Q_UNUSED(parent)
    m_taille =QSize(taille, taille);
    m_listeTetes.clear();
    suffixe = new QMap<int, QString>();
    suffixe->insert(abe::evalZ, "rien");
    suffixe->insert(abe::evalY, "attente");
    suffixe->insert(abe::evalA, "tresbien");
    suffixe->insert(abe::evalB, "bien");
    suffixe->insert(abe::evalC, "moyen");
    suffixe->insert(abe::evalD, "mal");
    margeH = 15;
    margeV = 15;
    m_currentTete = -1;
    m_isAppraisal = true;
    m_numQuestion = -1;
    m_contour=QColor(Qt::transparent); // On peut passer un QPen plus complexe
    m_background = QColor(Qt::transparent); //Pareil voir la doc sur les QPen et QBrush
    //qt 4.7 ou +
#if QT_VERSION >= 0x040700
    setAutoFillBackground(false);
#endif
    resetTetes(nbTete, parent);

}

AbulEduGraphicsTetesWidgetV1::~AbulEduGraphicsTetesWidgetV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    delete suffixe;
}

void AbulEduGraphicsTetesWidgetV1::resetTetes(int nbTete, QGraphicsWidget *parent)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    Q_UNUSED(parent)
    // On efface toutes les têtes
    foreach(QGraphicsPixmapItem *tmpItem, listePixmapTetes)
    {
        delete tmpItem;
    }

    listePixmapTetes.clear();
    m_listeTetes.clear();

    //20120302: modification structurelle (eric) pour converger vers la meme automatisation des
    //themes que la abuleduexercicev1 ... le fichier qrc a pour prefixe abuledutetev1/

    int x=margeH;
    int y = margeV;
    for(int i=0; i<nbTete; i++)
    {
        QGraphicsPixmapItem *pixTete = new QGraphicsPixmapItem();
        pixTete->setParentItem(this);
        pixTete->setPixmap(QPixmap(QString(":/evaluation/attente")).scaled(m_taille,Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ABULEDU_LOG_DEBUG() << QString(":/evaluation/attente");
        pixTete->setPos(x, y);
        x=x+m_taille.width()+margeH;

        listePixmapTetes.append(pixTete);
        m_listeTetes.append(abe::evalY); // Tete attente

        setGeometry(pos().x(), pos().y(),(m_taille.width() + margeH)*nbTete + margeH, m_taille.height() + margeV*2 );
    }
    m_currentTete = -1;
}

void AbulEduGraphicsTetesWidgetV1::setNbTetes(int nbTete)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    resetTetes(nbTete,parentWidget());
}

void AbulEduGraphicsTetesWidgetV1::setEtatTete(int numTete, abe::ABE_EVAL tete, bool isAppraisal, int numQuestion)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__
                        << m_currentTete<<" -> "<<numTete << " code " << tete;

    m_isAppraisal = isAppraisal;
    m_numQuestion = numQuestion;
    QString context;
    if(isAppraisal)
    {
        context = ":/evaluation/";
    }
    else
    {
        context = ":/current/";
    }
    if(numTete < listePixmapTetes.size())
    {
        QGraphicsPixmapItem *pixTete = listePixmapTetes[numTete];
        QFileInfo fi(context+suffixe->value(tete));
        if(fi.exists()) {
            if(isAppraisal)
            {
                pixTete->setPixmap(QPixmap(fi.absoluteFilePath()).scaled(m_taille, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                foreach(QGraphicsItem* it,pixTete->childItems())
                {
                    delete it;
                }
            }
            else
            {
                pixTete->setPixmap(QPixmap(fi.absoluteFilePath()).scaled(m_taille*1.2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                if(numQuestion > -1)
                {
                    foreach(QGraphicsItem* it,pixTete->childItems())
                    {
                        delete it;
                    }

                    AbulEduGraphicsEllipseItem* elp = new AbulEduGraphicsEllipseItem(numQuestion,0,0,m_taille.width()/3,pixTete);
                    elp->setPen(QPen(QBrush(QColor(104,33,21)),2));
                    elp->setBrush(QBrush(QColor(253,250,241)));
                }

                if(m_currentTete != numTete) pixTete->moveBy(-1*m_taille.width()*0.1,-1*m_taille.height()*0.1);
            }
        }
        else {
            ABULEDU_LOG_WARN() << "AbulEduGraphicsTetesWidgetV1::setEtatTete fichier manquant : " << fi.absoluteFilePath();
        }
        m_listeTetes[numTete]=tete;

    }
    ABULEDU_LOG_DEBUG() << tete << " -> " << m_listeTetes << ", liste de taille " << m_listeTetes.count();
    m_currentTete = numTete;
}

void AbulEduGraphicsTetesWidgetV1::resizeTetes(int taille)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_taille = QSize(taille, taille);
    int x = margeH;
    int y = margeV;
    for(int i = 0; i < listePixmapTetes.size(); i++)
    {
        listePixmapTetes[i]->setPos(x,y);
        x=x+m_taille.width()+margeH;

        setEtatTete(i, m_listeTetes[i],m_isAppraisal,m_numQuestion); // Redessine la tête dans son état antérieur et à la nouvelle taille
    }
    setGeometry(pos().x(), pos().y(),(m_taille.width() + margeH)*listePixmapTetes.size() + margeH, m_taille.height() + margeV*2 );
    m_currentTete = -1;
}

int AbulEduGraphicsTetesWidgetV1::getSizeTetes()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_taille.width();
}

abe::ABE_EVAL AbulEduGraphicsTetesWidgetV1::getEtatTete(int numTete)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(numTete < m_listeTetes.count())
        return m_listeTetes[numTete];
    else
        return abe::evalZ;
}

QList<abe::ABE_EVAL> AbulEduGraphicsTetesWidgetV1::getEtatTetes()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_listeTetes;
}

void AbulEduGraphicsTetesWidgetV1::setCouleurs(QPen contour, QBrush brosse)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_contour = contour;
    m_background = brosse;
}

void AbulEduGraphicsTetesWidgetV1::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setPen(m_contour);
    painter->setBrush(m_background);
    painter->drawRoundedRect(boundingRect(), margeH, margeV);
}

QRectF AbulEduGraphicsTetesWidgetV1::boundingRect()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return QRectF(0,0,geometry().width(), geometry().height());
}
