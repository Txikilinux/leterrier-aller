/** Classe AbulEduPageAccueilV1
  *
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduPageAccueilV1
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
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

#include "abuledupageaccueilv1.h"

#ifndef DEBUG_ABULEDUPAGEACCUEILV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduPageAccueilV1::AbulEduPageAccueilV1(QMap<int, QString> *listeTexteBulles, QWidget *parent, QMainWindow *mainwindow) :
    QGraphicsView(parent),
    m_scene(new QGraphicsScene(parent)),
    m_config(new QSettings(":/abuledupageaccueilv1/settings.conf", QSettings::IniFormat)),
    m_listeTexteBulles(listeTexteBulles),
    m_imageFond(QPixmap()),
    m_tailleImageFond(QSize()),
    m_proportionImageFond(0),
    m_ratio(0),
    m_zone(0),
    m_listeZones(QList<AbulEduZoneV1 *>()),
    m_parchemin(0),
    m_btnRevenirEditeur(0),
    m_mainWindow(mainwindow)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    initAbulEduPageAccueilV1();
    installPageAccueil();
}

AbulEduPageAccueilV1::AbulEduPageAccueilV1(QWidget *parent) :
    QGraphicsView(parent),
    m_scene(new QGraphicsScene(parent)),
    m_config(new QSettings(":/abuledupageaccueilv1/settings.conf", QSettings::IniFormat)),
    m_listeTexteBulles(0),
    m_imageFond(QPixmap()),
    m_tailleImageFond(QSize()),
    m_proportionImageFond(0),
    m_ratio(0),
    m_zone(0),
    m_listeZones(QList<AbulEduZoneV1 *>()),
    m_parchemin(0),
    m_btnRevenirEditeur(0)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    initAbulEduPageAccueilV1();
    installPageAccueil();
}

void AbulEduPageAccueilV1::initAbulEduPageAccueilV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    m_config->setIniCodec("UTF-8");
    setCacheMode(QGraphicsView::CacheBackground);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(m_scene);
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

AbulEduPageAccueilV1::~AbulEduPageAccueilV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    delete m_config;
    delete m_parchemin;
    delete m_scene;
}


void AbulEduPageAccueilV1::installPageAccueil()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

/* Scorie ? */
    //QFileInfo settingsPath(m_config->fileName());

    /* Installer le fond, le contour */
    if(!m_config->value("cheminFond").toString().isEmpty())
    {
        m_imageFond.load(":/abuledupageaccueilv1/images/fond.jpg");
        m_tailleImageFond = m_imageFond.size();
        m_proportionImageFond = (float)m_tailleImageFond.width()
                                / (float)m_tailleImageFond.height();
    }

    parentWidget()->setStyleSheet(m_config->value("couleurFond").toString());
    parentWidget()->setStyleSheet(parentWidget()->styleSheet() + QString("margin: 0; border-width: 0"));

    /* Installer les zones sensibles et leur bulle */
    const int size= m_config->beginReadArray("zone");
    for(int i=0; i<size; i++)
    {
        m_config->setArrayIndex(i);
        QFont font = m_config->value("police").value<QFont>();
        int pixelSize = m_config->value("pixelSize").toInt();
        if(pixelSize > 0){
            font.setPixelSize(m_config->value("pixelSize").toInt());
        }

        /* Pour la tablette on ajoute un menu au milieu ... */

        //Pour la tablette on ajoute un menu au milieu ...
#ifdef __ABULEDUTABLETTEV1__MODE__
        m_zone = new AbulEduZoneV1(i,
                                   m_config->value("rect").toRectF(),
                                   1,
                                   ":/abuledupageaccueilv1/images/" + m_config->value("cheminBulle").toString().replace("bulle","bulleTablette"),
                                   trUtf8(m_config->value("texteBulle").toString().toUtf8()),
                                   font,
                                   m_config->value("posBulle").toPoint(),
                                   m_config->value("posFleche").toPoint(),
                                   m_config->value("type").toString(),
                                   m_config->value("offsetV").toInt(),
                                   m_config->value("actif").toBool());
#else
        m_zone = new AbulEduZoneV1(i,
                                   m_config->value("rect").toRectF(),
                                   1,
                                   ":/abuledupageaccueilv1/images/" + m_config->value("cheminBulle").toString(),
                                   trUtf8(m_config->value("texteBulle").toString().toUtf8()),
                                   font,
                                   m_config->value("posBulle").toPoint(),
                                   m_config->value("posFleche").toPoint(),
                                   m_config->value("type").toString(),
                                   m_config->value("offsetV").toInt(),
                                   m_config->value("actif").toBool());
#endif
        m_listeZones.append(m_zone);
        m_scene->addItem(m_zone);
        m_scene->addItem(m_zone->abeZoneGetBulle());
        m_scene->addItem(m_zone->abeZoneGetFleche());
        m_zone->setPos(0,0);
    }
    m_config->endArray();

    /* Créer le menu graphique */
    m_parchemin = new AbulEduGraphicMenuV1(this);
    m_parchemin->setVisible(true);
    /* Redimensionner le tout */
    m_btnRevenirEditeur = new QPushButton(trUtf8("Revenir à l'éditeur"));
    m_btnRevenirEditeur->move(0, (m_tailleImageFond.height()-m_btnRevenirEditeur->height()));
    m_scene->addWidget(m_btnRevenirEditeur);
    m_scene->addWidget(m_parchemin);
    m_btnRevenirEditeur->hide();

    setDimensionsWidgets();
}

void AbulEduPageAccueilV1::resizeEvent(QResizeEvent *)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    if(isVisible()){
        setDimensionsWidgets();
    }
}

void AbulEduPageAccueilV1::showEvent(QShowEvent *)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    setDimensionsWidgets();
}

void AbulEduPageAccueilV1::setDimensionsWidgets()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    float largeurScene = (float)parentWidget()->width();
    float hauteurScene = (float)parentWidget()->height();
    float proportionFondAccueil = largeurScene / hauteurScene;
    if(m_proportionImageFond > proportionFondAccueil)
    {
        /* Redimensionnement horizontal */
        m_ratio = largeurScene / (float)m_tailleImageFond.width();
        setFixedSize(QSize(largeurScene, (largeurScene / m_proportionImageFond)));
    }
    else
    {
        /* Redimensionnement vertical */
        m_ratio = hauteurScene / (float)m_tailleImageFond.height();
        setFixedSize(QSize((hauteurScene * m_proportionImageFond),hauteurScene));
    }

    /* Placement de la page d'accueil (centrée) */
    move((largeurScene - this->width()) /2, (hauteurScene - this->height()) /2);
    setSceneRect(0,0,this->width(), this->height());
    setBackgroundBrush(m_imageFond.scaled(QSize(this->width(),this->height()),Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    /* Redimensionnement des Zones sensibles et des bulles */
    foreach (AbulEduZoneV1 *p_abez, m_listeZones) {
        p_abez->abeZoneResizeRect(m_ratio);
    }

    abeApp->setAbeApplicationDecorRatio(m_ratio);
    m_btnRevenirEditeur->move(0, height()-m_btnRevenirEditeur->height());
    m_parchemin->setDimensionsWidget();

    emit dimensionsChangees();
}

void AbulEduPageAccueilV1::mousePressEvent(QMouseEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    QGraphicsView::mousePressEvent(event);
    if(event->button() == Qt::RightButton)
    {
        /* On trouve les bulles */
        abePageAccueilMontreBulles(true,true);
    }
    if((items(event->pos())).size() > 0) /* Il y a quelquechose sous le curseur */
    {
        event->ignore();
    }
    else
    {
        emit signalAbePageAccueilMousePressed();
    }
}

void AbulEduPageAccueilV1::mouseReleaseEvent(QMouseEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    QGraphicsView::mouseReleaseEvent(event);
    if(event->button() == Qt::RightButton)
    {
        abePageAccueilMontreBulles(false);
    }
    else if(event->button() == Qt::LeftButton)
    {
        if((items(event->pos())).size() > 0) /* Il y a quelquechose sous le curseur */
        {
            const QList<QGraphicsItem *>listeItems = items(event->pos()); /* On récupère la liste des items sous la souris */
            foreach (QGraphicsItem *p_gi, listeItems) {
                if(p_gi->type() == 3) /* C'est un QGraphicsRectItem, donc une zone sensible (normalement ! ) */
                {
                    AbulEduZoneV1 *zone = static_cast<AbulEduZoneV1 *>(p_gi);
                    if(zone->abeZoneIsActif())
                    {
                        emit boutonPressed(zone->abeZoneGetNumero(), zone->abeZoneGetBulle()->abeBulleGetText());

                        /// deprecated
                        emit boutonPressed(zone->abeZoneGetNumero());
                    }
                }

            }
        }
        else
        {
            event->ignore();
        }
    }
    event->ignore();
}

void AbulEduPageAccueilV1::keyPressEvent(QKeyEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    if( event->key() == Qt::Key_Space )
    {
        /* On trouve les bulles */
        if(!event->isAutoRepeat()){
            abePageAccueilMontreBulles(true,true);
        }
    }
    QGraphicsView::keyPressEvent(event);
}

void AbulEduPageAccueilV1::keyReleaseEvent(QKeyEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    if( event->key() == Qt::Key_Space )
    {
        if(!event->isAutoRepeat()){
            abePageAccueilMontreBulles(false);
        }
    }
    QGraphicsView::keyReleaseEvent(event);
}

void AbulEduPageAccueilV1::abePageAccueilMontreBulles(bool vraiFaux, bool isDemo)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    foreach(AbulEduZoneV1 *zone, m_listeZones)
    {
        if(vraiFaux){
            if(isDemo){
                /* En mode démo elles sont toutes visibles */
            #ifndef __ABULEDUTABLETTEV1__MODE__
                zone->abeZoneGetBulle()->setVisible(true); //vraifaux
            #endif
            }
            else{
                if(zone->abeZoneIsActif()){
                    /* Quand on est sur tablette on ne cache pas les bulles */
                    #ifndef __ABULEDUTABLETTEV1__MODE__
                    zone->abeZoneGetBulle()->setVisible(true);
                    #endif
                }
                else{
                    #ifndef __ABULEDUTABLETTEV1__MODE__
                    zone->abeZoneGetBulle()->setVisible(false);
                    #endif
                }
            }
        }
        else{
            #ifndef __ABULEDUTABLETTEV1__MODE__
            zone->abeZoneGetBulle()->setVisible(false);// !vraifaux
            #endif
        }
    }
}

void AbulEduPageAccueilV1::abePageAccueilDesactiveZones(bool vraiFaux)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    if(vraiFaux){
        foreach(AbulEduZoneV1 *zone, m_listeZones)
        {
            zone->abeZoneSetActif(false);
        }
    }
    else{
        /* Réactiver les zones sensibles en fonction de la config */
        const int size= m_config->beginReadArray("zone");
        for(int i=0; i<size; i++)
        {
            m_config->setArrayIndex(i);
            m_listeZones[i]->abeZoneSetActif( m_config->value("actif").toBool());
        }
        m_config->endArray();
    }
}

#ifdef __ABULEDUTABLETTEV1__MODE__
void AbulEduPageAccueilV1::abePageAccueilRetablisBullesTablette()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    foreach(AbulEduZoneV1 *zone, m_listeZones)
    {
        zone->abeZoneSetImageBulleTablette();
    }
}
#endif

void AbulEduPageAccueilV1::abePageAccueilRetranslate()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    const int size= m_config->beginReadArray("zone");
    for(int i=0; i<size; i++)
    {
        m_config->setArrayIndex(i);
        m_listeZones[i]->abeZoneGetBulle()->abeBulleSetTexte(trUtf8(m_config->value("texteBulle").toString().toUtf8()));
    }
    m_config->endArray();
}

QMainWindow *AbulEduPageAccueilV1::abePageAccueilGetMainWindow() const
{
    if(m_mainWindow == 0){
        ABULEDU_LOG_WARN()<<"Attention : vous avez fait appel à la méthode "<<__PRETTY_FUNCTION__<<" mais elle va vous retourner 0";
    }
    return m_mainWindow;
}
