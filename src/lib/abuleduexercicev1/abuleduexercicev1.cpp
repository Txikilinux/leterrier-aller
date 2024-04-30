/**
  * Classe de base en version une pour les exercices AbulEdu permettant de structurer
  * nos logiciels et respecter ainsi un certain nombre de choses
  * @author 2009-2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GPL)
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

#include "abuleduexercicev1.h"

#ifndef DEBUG_ABULEDUEXERCICEV1
    #include "abuledudisableloggerv1.h"
#endif

/** constructeur ... */
AbulEduExerciceV1::AbulEduExerciceV1(QWidget *parent) : QObject(parent), AbulEduExerciceCommonV1(this)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* Initialisation du générateur de nombres aléatoires */
    #if QT_VERSION >= 0x040700
        qsrand(uint(QDateTime::currentMSecsSinceEpoch() & 0xFFFF));
    #else
        QDateTime t;
        qsrand(uint(t.toTime_t() & 0xFFFF));
    #endif

    setObjectName("AbulEduExerciceV1");
    setProperty("AbulEduType","AbulEduExerciceV1");
    /* Les themes par defaut */
    m_themeTetes = "asma";
    m_themeLevel = "chiffres";
    m_rightColor = QColor(3,151,3,179);
    m_wrongColor = QColor(234,33,0,179);

    m_uiAbulEduAireDeTravail = new AbulEduExerciceWidgetAireDeTravailV1(parent);
    m_uiAbulEduTelecommande = new AbulEduExerciceWidgetTelecommandeV1(parent);
    m_uiAbulEduMessage = new AbulEduExerciceWidgetMessageV1(parent);
    m_unitLanguage = "fre";

    /* Pas besoin de montrer les AireDeTravail et Telecommande ici (évite l'affichage multiple des images) */
    m_uiAbulEduAireDeTravail->setVisible(false);
    m_uiAbulEduTelecommande->setVisible(false);
    m_uiAbulEduMessage->setVisible(false);

    /* Objets de l'interface graphiques */
    m_uiAbulEduAireDeTravail->ui->gvPrincipale->setCacheMode(QGraphicsView::CacheBackground);
    m_uiAbulEduAireDeTravail->ui->gvPrincipale->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
}

AbulEduExerciceV1::~AbulEduExerciceV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    delete m_uiAbulEduMessage;
    delete m_uiAbulEduAireDeTravail;
    delete m_uiAbulEduTelecommande;
}

void AbulEduExerciceV1::setAbeLevel(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduExerciceCommonV1::setAbeLevel(s);
    m_uiAbulEduTelecommande->setAbeLevel(s);
}

void AbulEduExerciceV1::setAbeNbTotalQuestions(int n)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduExerciceCommonV1::setAbeNbTotalQuestions(n);
}

void AbulEduExerciceV1::setAbeTeteTelecommande(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    ABULEDU_LOG_WARN()  << " IS DEPRECATED ... No more evaluation head on the remote control";

    //Attention il faudra vérifier que s = tresbien, rien, moyen, mal, bien, attente
    QStringList vocabulaire;
    vocabulaire << "tresbien" << "rien" << "moyen" << "mal" << "bien" << "attente";
    if(s.trimmed() != "" && vocabulaire.contains(s)) {
        m_uiAbulEduTelecommande->setMTeteEval(s);
    }
    else {
        ABULEDU_LOG_WARN() << "Attention vous devez passer un mot du vocabulaire à l'appel de la fonction AbulEduExerciceV1::setAbeTeteTelecommande " << vocabulaire;
    }
}

qreal AbulEduExerciceV1::setAbeTeteForResult(int errors, int total)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    ABULEDU_LOG_WARN() << "Méthode à corriger : elle fait actuellement appel à la méthode setAbeTeteTelecommande() qui est dépréciée depuis la disparition de la tête dans la télécommmande";

    qreal percentage = -1;
    if (total == 0) {
        return percentage;
    }
    else {
        QSettings config(":/abuleduexercicev1/conf/abuleduexercicev1.conf",QSettings::IniFormat);
        int veryGoodPoint = 10;
        if (config.value("seuilTresBien").isValid())
            veryGoodPoint = config.value("seuilTresBien").toInt();
        int goodPoint = 30;
        if (config.value("seuilBien").isValid())
            goodPoint = config.value("seuilBien").toInt();
        int mediumPoint = 60;
        if (config.value("seuilMoyen").isValid())
            mediumPoint = config.value("seuilMoyen").toInt();

        qreal errorsAsReal = static_cast<qreal> (errors);
        percentage = (errorsAsReal/total) * 100;

        ABULEDU_LOG_DEBUG() << " Pourcentage d'erreurs : " << percentage;

        if (percentage <= veryGoodPoint)
        {
            setAbeTeteTelecommande("tresbien");
            setAbeExerciceEvaluation(abe::evalA);
        }
        else if (percentage <= goodPoint)
        {
            setAbeTeteTelecommande("bien");
            setAbeExerciceEvaluation(abe::evalB);
        }
        else if (percentage <= mediumPoint)
        {
            setAbeTeteTelecommande("moyen");
            setAbeExerciceEvaluation(abe::evalC);
        }
        else
        {
            setAbeTeteTelecommande("mal");
            setAbeExerciceEvaluation(abe::evalD);
        }
    }
    return percentage;
}

void AbulEduExerciceV1::setAbeThemeTeteTelecommande(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    qFatal("FATAL: DEPRECATED AbulEduExerciceV1::setAbeThemeTeteTelecommande");
    /* remplacé par les qrc qui embarquent leur 'theme' */
    m_themeTetes = s;
}

void AbulEduExerciceV1::setAbeThemeLevelTelecommande(const QString& s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    ABULEDU_LOG_FATAL() << "FATAL: DEPRECATED AbulEduExerciceV1::setAbeThemeLevelTelecommande";
    /* remplacé par les qrc qui embarquent leur 'theme' */
    m_themeLevel = s;
}

AbulEduExerciceWidgetTelecommandeV1 * AbulEduExerciceV1::getAbeExerciceTelecommandeV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_uiAbulEduTelecommande;
}

AbulEduExerciceWidgetAireDeTravailV1 * AbulEduExerciceV1::getAbeExerciceAireDeTravailV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_uiAbulEduAireDeTravail;
}

AbulEduExerciceWidgetMessageV1 *AbulEduExerciceV1::getAbeExerciceMessageV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_uiAbulEduMessage;
}

void AbulEduExerciceV1::slotAbeFermerExercice()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_uiAbulEduTelecommande->on_btnQuitterRetourMenuPrincipal_clicked();
}

void AbulEduExerciceV1::displayErrorMissingExercise(const QString &nomExercice, QWidget* place)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Exercice absent du module"),
                                                              trUtf8("Ce module ne contient aucun exercice de type <i>%1</i>").arg(nomExercice),
                                                              true,place);
    messageBox->show();
}

const QString &AbulEduExerciceV1::getAbeExerciceUnitLanguage() const
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_unitLanguage;
}

void AbulEduExerciceV1::setAbeExerciceUnitLanguage(const QString &unitLanguage)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_unitLanguage = unitLanguage;
}
