/**
  * Classe
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
  */
/*
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

#include "abuleduexercicewidgetairedetravailv1.h"

#ifndef DEBUG_ABULEDUEXERCICEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduExerciceWidgetAireDeTravailV1::AbulEduExerciceWidgetAireDeTravailV1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbulEduExerciceWidgetAireDeTravailV1)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    this->setStyleSheet("background-color: transparent");
    ui->setupUi(this);
    if(parent->inherits("AbulEduPageAccueilV1")) {
        connect(qobject_cast<AbulEduPageAccueilV1 *>(parent), SIGNAL(dimensionsChangees()),
                this, SLOT(setDimensionsWidget()),Qt::UniqueConnection);
    }
    setParent(parent);
    gsPrincipale = new QGraphicsScene(ui->gvPrincipale);

    gsPrincipale->setObjectName("gsPrincipale");
    ui->gvPrincipale->setScene(gsPrincipale);
    ui->gvPrincipale->setSceneRect(0,0,ui->gvPrincipale->width(),ui->gvPrincipale->height());
    ui->gvPrincipale->setStyleSheet("background-color: transparent;");
    m_fond = new QGraphicsPixmapItem();
    gsPrincipale->addItem(m_fond);
    m_fond->setZValue(-10);

    setImageFond(":/abuleduexercicev1/fonds/airedetravail");
}

AbulEduExerciceWidgetAireDeTravailV1::~AbulEduExerciceWidgetAireDeTravailV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    delete ui;
    delete gsPrincipale;
    /* pas de delete m_fond, c'est le destructeur de la scène qui s'en charge */
}

void AbulEduExerciceWidgetAireDeTravailV1::abeAireDeTravailClose()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    // a faire ou pas ?
    //setAttribute(Qt::WA_DeleteOnClose);
    close();
}

void AbulEduExerciceWidgetAireDeTravailV1::changeEvent(QEvent *e)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AbulEduExerciceWidgetAireDeTravailV1::setImageFond(const QString &chemin)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
//    QPixmap imageFond(chemin);

    m_imageFond.load(chemin);
//    m_Fond = gsPrincipale->addPixmap(m_imageFond);
    m_fond->setPixmap(m_imageFond);
    setDimensionsWidget();

}

void AbulEduExerciceWidgetAireDeTravailV1::setImageFond(const QPixmap &pixmap)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
//    QPixmap imageFond(chemin);

    m_imageFond = pixmap;
//    m_Fond = gsPrincipale->addPixmap(m_imageFond);
    m_fond->setPixmap(m_imageFond);
    setDimensionsWidget();
}

void AbulEduExerciceWidgetAireDeTravailV1::setDimensionsWidget()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QPixmap fond = m_imageFond;
    fond = fond.scaled(fond.size()*abeApp->getAbeApplicationDecorRatio(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setFixedSize(fond.width(), fond.height());

    // NSE - ajout pour positionnement correct
//    float ratio = abeApp->getAbeApplicationDecorRatio();
    // move(1560*ratio,18*ratio);

    //qDebug() << "AbulEduExerciceWidgetAireDeTravailV1::setDimW : " << fond.width() << " et ratio " << abeApp->getAbeApplicationDecorRatio();
    m_fond->setPixmap(fond);
//    gsPrincipale->setSceneRect(0,0, fond.width(),  fond.height());
    ui->gvPrincipale->setFixedSize(fond.size());
}
