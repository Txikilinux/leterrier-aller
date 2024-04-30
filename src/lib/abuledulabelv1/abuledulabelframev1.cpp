/** AbulEduLabelFrameV1
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2013 Éric Seigne <eric.seigne@ryxeo.com>
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

#include "abuledulabelframev1.h"

#ifndef DEBUG_ABULEDULABELV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduLabelFrameV1::AbulEduLabelFrameV1(int orientation,QWidget *parent) :
    QFrame(parent)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;

    m_orientation = orientation;
    m_abuleduLabelSelected = -1;
    m_PosListeLabels = QPoint(10,10);
    m_marge = 10;
    m_largeurEtiquettes = 0;
    m_listeAbulEduLabels.clear();
    setFixedSize(200, 50);
    connect(parent, SIGNAL(moveSelectedLabelUp()), this, SLOT(moveSelectedAbulEduLabelUp()), Qt::UniqueConnection);
    connect(parent, SIGNAL(moveSelectedLabelDown()), this, SLOT(moveSelectedAbulEduLabelDown()), Qt::UniqueConnection);
    connect(parent, SIGNAL(moveSelectedLabelLeft()), this, SLOT(moveSelectedAbulEduLabelLeft()), Qt::UniqueConnection);
    connect(parent, SIGNAL(moveSelectedLabelRight()), this, SLOT(moveSelectedAbulEduLabelRight()), Qt::UniqueConnection);
    connect(this, SIGNAL(btnDroiteSetVisible(bool)), parent, SLOT(btnDroiteSetVisible(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(btnGaucheSetVisible(bool)), parent, SLOT(btnGaucheSetVisible(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(btnHautSetVisible(bool)), parent, SLOT(btnHautSetVisible(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(btnBasSetVisible(bool)), parent, SLOT(btnBasSetVisible(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(btnDroiteSetEnabled(bool)), parent, SLOT(btnDroiteSetEnabled(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(btnGaucheSetEnabled(bool)), parent, SLOT(btnGaucheSetEnabled(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(btnHautSetEnabled(bool)), parent, SLOT(btnHautSetEnabled(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(btnBasSetEnabled(bool)), parent, SLOT(btnBasSetEnabled(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(AbulEduLabel_moved(AbuleduLabelV1 *)), this, SLOT(on_AbulEduLabel_moved(AbuleduLabelV1 *)), Qt::UniqueConnection);
}

AbulEduLabelFrameV1::~AbulEduLabelFrameV1()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
}

void AbulEduLabelFrameV1::abelabelCreeAbulEduLabels(const QStringList &listeTextes, const QString &defaultCSS, const QString &selectedCSS, const QString &errorCSS, const QFont &fonte)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;

    QList<AbuleduLabelV1 *> etiquetteExistantes =  findChildren<AbuleduLabelV1 *>();
    QListIterator<AbuleduLabelV1*> it(etiquetteExistantes);
    while(it.hasNext())
    {
        delete it.next();
    }
    if(listeTextes.size() > 0)
    {
        // Au cas ou il y aurait des abuleduLabels qui trainent, on les efface
        QMap<int, AbuleduLabelV1 *> aEffacer = abelabelListeAbuleduLabelsByNumPlace();
        foreach(AbuleduLabelV1 *etiq, aEffacer)
        {
            etiq->deleteLater();
        }

        // erics 20130712: si la boucle for de delete ci-dessus efface des QLabel ... peut-être faut-il
        //nettoyer la liste ?
        m_listeAbulEduLabels.clear();

        //    je commente, parce qu'il me semble anormal d'appeler cette méthode avant la création des labels dessous
        //    m_listeAbulEduLabels = abelabelListeAbuleduLabelsByNumPlace();
        //Création des abuleduLabel : un par mot
        for(int i=0;i<listeTextes.size();i++)
        {
            AbuleduLabelV1* etiq = new AbuleduLabelV1(listeTextes[i],i,i,fonte,this);
            connect(etiq, SIGNAL(AbulEduLabel_moved(AbuleduLabelV1*)),this, SIGNAL(AbulEduLabel_moved(AbuleduLabelV1*)), Qt::UniqueConnection);
            etiq->setObjectName("Etiquette "+QString::number(i));
            etiq->abelabelSetDefaultStyleSheet(defaultCSS);
            etiq->abelabelSetSelectedStyleSheet(selectedCSS);
            etiq->abelabelSetErrorStyleSheet(errorCSS);
            etiq->setFrameShape(QFrame::NoFrame);
            etiq->abelabelSetMarge(m_marge);
            etiq->setAcceptDrops(true);
            etiq->abelabelSetFont(fonte);
            m_listeAbulEduLabels.insert(i, etiq);
        }
        if(m_orientation == Qt::Vertical)
        {
            emit btnDroiteSetVisible(false);
            emit btnGaucheSetVisible(false);
        }
        else
        {
            emit btnHautSetVisible(false);
            emit btnBasSetVisible(false);
        }
        abelabelDeselectionneLabels();
        abelabelResizeLabels();
        abelabelPlaceAbulEduLabelsByNumPlace();
        abelabelMontreEtiquettes(true);
    }
}

QMap<int, AbuleduLabelV1 *>AbulEduLabelFrameV1::abelabelListeAbuleduLabelsByNumPlace()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    QList<AbuleduLabelV1 *> listEtiquettes =  findChildren<AbuleduLabelV1 *>();
    QMap<int, AbuleduLabelV1 *> listAbuleduLabels;

    foreach(AbuleduLabelV1 *etiq, listEtiquettes)
    {
        listAbuleduLabels.insert(etiq->abelabelNumPlace(), etiq);
    }
    ABULEDU_LOG_DEBUG() << listAbuleduLabels;
    return listAbuleduLabels;
}

QMap<int, AbuleduLabelV1 *>AbulEduLabelFrameV1::abelabelListeAbuleduLabelsByNumOrdre()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    QList<AbuleduLabelV1 *> listEtiquettes =  findChildren<AbuleduLabelV1 *>();
    QMap<int, AbuleduLabelV1 *> listAbuleduLabels;

    foreach(AbuleduLabelV1 *etiq, listEtiquettes)
    {
        listAbuleduLabels.insert(etiq->abelabelNumOrdre(), etiq);
    }
    return listAbuleduLabels;
}

void AbulEduLabelFrameV1::abelabelPlaceAbulEduLabelsByNumPlace()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    QMap<int, AbuleduLabelV1 *> listeByNumPlace;
    listeByNumPlace.clear();
    foreach(int cle, m_listeAbulEduLabels.keys())
    {
        listeByNumPlace.insert(m_listeAbulEduLabels.value(cle)->abelabelNumPlace(), m_listeAbulEduLabels.value(cle));
    }

    int x = m_PosListeLabels.x();
    int y = m_PosListeLabels.y();

    QMapIterator<int, AbuleduLabelV1 *> i(listeByNumPlace);
    while (i.hasNext())
    {
        i.next();
        i.value()->move(x, y);
        i.value()->abelabelSetLastPos(x, y);
        if(m_orientation == Qt::Vertical)
        {
            y = y + m_marge + i.value()->height();
        }
        else
        {
            x = x + m_marge + i.value()->width();
        }
    }
}

void AbulEduLabelFrameV1::abelabelPlaceAbulEduLabelsByNumOrdre()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    QMap<int, AbuleduLabelV1 *> listeByNumOrdre = abelabelListeAbuleduLabelsByNumOrdre();

    int x = m_PosListeLabels.x();
    int y = m_PosListeLabels.y();

    QMapIterator<int, AbuleduLabelV1 *> i(listeByNumOrdre);
    while (i.hasNext())
    {
        i.next();
        i.value()->move(x, y);
        i.value()->abelabelSetLastPos(x, y);
        if(m_orientation == Qt::Vertical)
        {
            y = y + m_marge + i.value()->height();
        }
        else
        {
            x = x + m_marge + i.value()->width();
        }
    }
}

void AbulEduLabelFrameV1::abelabelMontreEtiquettes(const bool onOff)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__ << onOff;
    foreach(int cle, m_listeAbulEduLabels.keys())
    {
        (onOff) ? m_listeAbulEduLabels.value(cle)->show() :  m_listeAbulEduLabels.value(cle)->hide();
    }
}


void AbulEduLabelFrameV1::abelabelDeplaceEtiquette(const int deplacement)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    if(deplacement == -1 || deplacement == +1)
    {
        if(abelabelSwapAbuleduLabel(m_abuleduLabelSelected,m_abuleduLabelSelected-deplacement))
        {
            m_abuleduLabelSelected=m_abuleduLabelSelected-deplacement;
            m_listeAbulEduLabels=abelabelListeAbuleduLabelsByNumPlace();

            emit AbulEduLabel_moved(m_listeAbulEduLabels.value(m_abuleduLabelSelected));
            if(m_abuleduLabelSelected == 0)
            {
                emit btnHautSetEnabled(false);
                emit btnGaucheSetEnabled(false);
            }
            else
            {
                emit btnHautSetEnabled(true);
                emit btnGaucheSetEnabled(true);
            }
            if(m_abuleduLabelSelected == m_listeAbulEduLabels.size()-1)
            {
                emit btnBasSetEnabled(false);
                emit btnDroiteSetEnabled(false);
            }
            else
            {
                emit btnBasSetEnabled(true);
                emit btnDroiteSetEnabled(true);
            }
        }
    }
    traceEtiquettes();
}

bool AbulEduLabelFrameV1::abelabelSwapAbuleduLabel(int rangEtiq1, int rangEtiq2)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    QMap<int, AbuleduLabelV1 *> listeItems = this->abelabelListeAbuleduLabelsByNumPlace();
    if(rangEtiq1 >= 0
            && rangEtiq2 >= 0
            && rangEtiq1 < listeItems.size()
            && rangEtiq2 < listeItems.size())
    {
        listeItems.value(rangEtiq1)->abelabelSetNumPlace(rangEtiq2);
        listeItems.value(rangEtiq2)->abelabelSetNumPlace(rangEtiq1);
        abelabelPlaceAbulEduLabelsByNumPlace();
        return true;
    }
    else
    {
        return false; // les étiquettes n'ont pas été permutées
    }
}

void AbulEduLabelFrameV1::abelabelDeselectionneLabels()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    // Un clic en dehors d'une étiquette déselectionne les étiquettes
    QList<AbuleduLabelV1 *> listeEtiquettes = this->findChildren<AbuleduLabelV1 *>();
    foreach(AbuleduLabelV1* etiq, listeEtiquettes)
    {
        etiq->setStyleSheet(etiq->abelabelGetDefaultStyleSheet());
    }
    m_abuleduLabelSelected=-1; // -1 pas d'étiquette sélectionnée
    emit btnHautSetEnabled(false);
    emit btnBasSetEnabled(false);
    emit btnGaucheSetEnabled(false);
    emit btnDroiteSetEnabled(false);
}

void AbulEduLabelFrameV1::abelabelMelangeAbulEduLabels()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;

    QMap<int, AbuleduLabelV1 *> tmpListe = abelabelListeAbuleduLabelsByNumOrdre();
    QList<int> tmpPlace;
    for(int i = 0; i < tmpListe.size(); i++)
        tmpPlace << i;

    int hasard = 0;
    QMapIterator<int, AbuleduLabelV1 *> i(tmpListe);
    while (i.hasNext())
    {
        i.next();
        hasard = qrand()%tmpPlace.size();
        hasard = tmpPlace.takeAt(hasard);
        i.value()->abelabelSetNumPlace(hasard);
    }
    abelabelPlaceAbulEduLabelsByNumPlace();
    m_listeAbulEduLabels = abelabelListeAbuleduLabelsByNumPlace();
    traceEtiquettes();
}

void AbulEduLabelFrameV1::abelabelSetMarge(const int marge)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__ << marge;
    m_marge=marge;
}

void AbulEduLabelFrameV1::abelabelSetPosListeLabels(const QPoint &position)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    m_PosListeLabels = position;
}

void AbulEduLabelFrameV1::abelabelSetPosListeLabels(int x, int y)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    m_PosListeLabels = QPoint(x, y);
}

void AbulEduLabelFrameV1::abelabelSetLargeurEtiquettes(const int largeur)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    m_largeurEtiquettes = largeur;
}

void AbulEduLabelFrameV1::abelabelSetOrientation(const int orientation)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    m_orientation = orientation;
    abelabelPlaceAbulEduLabelsByNumPlace();
}

int AbulEduLabelFrameV1::abelabelOrientation() const
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    return m_orientation;
}

QSize AbulEduLabelFrameV1::abelabelResizeFrame()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    int haut = m_PosListeLabels.y();
    int larg = 0;
    if(m_orientation==Qt::Vertical)
    {
        foreach(int cle, m_listeAbulEduLabels.keys())
        {
            haut = haut + m_listeAbulEduLabels.value(cle)->height() + m_marge;
            larg = qMax(larg, m_listeAbulEduLabels.value(cle)->width());
        }
        larg = larg + 2*m_PosListeLabels.x();
    }
    else
    {
        // Cas de la liste horizontale (haut ne change pas)
        int ymax = 0;
        foreach(int cle, m_listeAbulEduLabels.keys())
        {
            ymax= qMax(ymax,m_listeAbulEduLabels.value(cle)->height());
            larg = larg + m_listeAbulEduLabels.value(cle)->width() +m_marge;
        }
        haut = haut + ymax + m_PosListeLabels.y();
        larg = larg + m_marge;
    }
    this->setFixedSize(larg, haut);
    return QSize(larg, haut);
}

void AbulEduLabelFrameV1::abelabelResizeLabels()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    if(m_largeurEtiquettes > 0)
    {
        foreach(int cle, m_listeAbulEduLabels.keys())
        {
            /* Comment ça peut planter là ? si je rentre dans le for, c'est que m_listeAbulEduLabels n'est pas vide,  */
            m_listeAbulEduLabels.value(cle)->setFixedWidth(m_largeurEtiquettes);
            if(m_listeAbulEduLabels.value(cle)->width() > 0)
            {
                m_listeAbulEduLabels.value(cle)->setFixedHeight(m_listeAbulEduLabels.value(cle)->abelabelMarge()*2+fontMetrics().height() *  (1+fontMetrics().width(m_listeAbulEduLabels.value(cle)->text()) / m_listeAbulEduLabels.value(cle)->width()));
            }
        }
    }
}

void AbulEduLabelFrameV1::on_abuleduLabelSelected(int rang)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__ << rang;

    m_abuleduLabelSelected = rang;
    if(m_abuleduLabelSelected == 0)
    {
        emit btnHautSetEnabled(false);
        emit btnGaucheSetEnabled(false);
    }
    else
    {
        emit btnHautSetEnabled(true);
        emit btnGaucheSetEnabled(true);
    }
    if(m_abuleduLabelSelected == m_listeAbulEduLabels.size()-1)
    {
        emit btnBasSetEnabled(false);
        emit btnDroiteSetEnabled(false);
    }
    else
    {
        emit btnBasSetEnabled(true);
        emit btnDroiteSetEnabled(true);
    }
}

void AbulEduLabelFrameV1::moveSelectedAbulEduLabelUp()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    abelabelDeplaceEtiquette(+1);
}

void AbulEduLabelFrameV1::moveSelectedAbulEduLabelDown()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    abelabelDeplaceEtiquette(-1);
}
void AbulEduLabelFrameV1::moveSelectedAbulEduLabelLeft()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    abelabelDeplaceEtiquette(+1);
}
void AbulEduLabelFrameV1::moveSelectedAbulEduLabelRight()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    abelabelDeplaceEtiquette(-1);
}

void AbulEduLabelFrameV1::on_AbulEduLabel_moved(AbuleduLabelV1 *etiq)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    if(m_orientation == Qt::Vertical)
    {
        static_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureWidgetVisible(etiq,80,80);
    }
    else
    {
        static_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureWidgetVisible(etiq,80,80);
    }
}

void AbulEduLabelFrameV1::mousePressEvent(QMouseEvent *event)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    abelabelDeselectionneLabels();
    event->accept();
}

void AbulEduLabelFrameV1::childEvent(QChildEvent *event)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    AbuleduLabelV1 *etiq = static_cast<AbuleduLabelV1 *>(event->child());
    if(etiq)
    {
        switch(event->type())
        {
        case QChildEvent::ChildAdded:
            break;
        case QChildEvent::ChildPolished:
            abelabelResizeFrame();
            break;
        default:
            break;
        }
    }
}

void AbulEduLabelFrameV1::traceEtiquettes()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    foreach(int cle, m_listeAbulEduLabels.keys())
    {
        ABULEDU_LOG_DEBUG() << m_listeAbulEduLabels.value(cle)->objectName()
                            <<"numOrdre"<<m_listeAbulEduLabels.value(cle)->abelabelNumOrdre()
                            <<"numPlace"<<m_listeAbulEduLabels.value(cle)->abelabelNumPlace()
                            <<"lastPos"<<m_listeAbulEduLabels.value(cle)->abelabelLastPos();
    }
    ABULEDU_LOG_DEBUG()<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<liste par ordre<<<<<<<<<<<<<<<<<<<<<<<<";
    QMap<int, AbuleduLabelV1 *> tmpListe= abelabelListeAbuleduLabelsByNumOrdre();
    foreach(int cle, m_listeAbulEduLabels.keys())
    {
        ABULEDU_LOG_DEBUG()<<tmpListe.value(cle);
    }
    ABULEDU_LOG_DEBUG()<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<liste par place<<<<<<<<<<<<<<<<<<<<<<<<<";
    tmpListe= abelabelListeAbuleduLabelsByNumPlace();
    foreach(int clef,m_listeAbulEduLabels.keys())
    {
        ABULEDU_LOG_DEBUG()<<m_listeAbulEduLabels.value(clef);
    }
}
