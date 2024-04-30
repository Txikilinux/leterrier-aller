/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011-2014 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#ifndef EXERCICEMOTSMELANGES_H
#define EXERCICEMOTSMELANGES_H
#include <QGraphicsProxyWidget>
#include "abuledutextv1.h"
#include "abuledulabelv1.h"
#include "abuleduoutilsv1.h"
#include "abuleduexercicev1.h"
#include "abuledulabelframev1.h"
#include "abuleduetiquettesv1.h"
#include "abuledulabelwidgetv1.h"
#include "abuleduapplicationv1.h"
#include "abuleducommonstatesv1.h"
#include "abuledupageaccueilv1.h"


class ExerciceMotsMelanges : public AbulEduCommonStatesV1, public AbulEduOutilsV1
{
    Q_OBJECT

public:
    ExerciceMotsMelanges(QWidget *parent = 0, QString repertoireBase = "");
    ~ExerciceMotsMelanges();

    /** Lis dans le fichier module.conf ou tire au sort la phrase qui va être utilisée dans l'exercice */
    static QString takeSentence(const QString &unitPath, int exerciseNumber, AbulEduTextV1 *text);

protected:
    /** Vérifie la réponse proposée
      *@return le nombre d'erreurs
      */
    int verifieReponse();

    /** Donne la bonne réponse à l'utilisateur
      * Renvoie une évaluation abe::evalZ à la question
      * @return par convention -1 correspond à "abandon" */
    int donneReponse();

private:
    /** Le fichier de configuration de la séquence */
    QSettings *m_config;

    /** Une AbulEduMessageBoxV1 déclarée en attribut, et réutilisée plutôt que d'en créer une à chaque fois et la détruire à la fermeture */
    AbulEduMessageBoxV1* m_messageBox;

    int JEVEUXCOLORIERETIQUETTESMALPLACEES;

    /** AbulEduEtiquettesV1 est dérivée de qGraphicsview */
   AbulEduEtiquettesV1 *gv_AireDeJeu;

    /** Pour stocker AbulEduLabelWidget */
    QGraphicsProxyWidget *proxy;

    /** L'objet texte support de l'exercice */
    AbulEduTextV1 *m_texte;

    /** La phrase à utiliser */
    QString m_phrase;

    /** QStringList des mots d'un exercice */
    QStringList m_listeMots;

    /** Le nombre d'essais effectués pour répondre à la question
      * On stocke le numéro de l'exercice puis le nombre de clics sur btnVerifier
      */
    QList<QPair<int, int> > m_resultats;

    /** Liste des pieces */
    QList<AbulEduPieceV1 *> m_listeEtiquettes;

    /** Liste des Cellules */
    QList<AbulEduCelluleV1 *> m_listeCellules;

    void afficheMots();
    void placeMots();
    void placeCellules();
    void clearScene();
    void redimensionneConsigne();
    void redimensionneBilan();

    int m_numLevel;
    QWidget *m_parent;
    int maxLargeurEtiquette;
    QLinearGradient gradient;
    QLinearGradient gradientOk;
    QLinearGradient gradientFaux;
    QGraphicsPixmapItem *tete;

    bool m_localDebug;

    /** Liste des évaluations reçues par l'exercice, évaluations émises par les cellules */
    QList<abe::ABE_EVAL> m_receivedDropResults;

    /** Stocke le taux de réussite sous forme de pourcentage */
    qreal m_percent;

    bool m_showSentenceAsHelpingHand;

private slots:
    void slotSequenceEntered();
    void slotPresenteSequenceEntered();
    void slotExercicesEntered();
    void slotPresentationExerciceEntered();
    void slotRealisationExerciceEntered();
    void slotInitQuestionEntered();
    void slotQuestionEntered();
    void slotAfficheVerificationQuestionEntered();
    void slotFinVerificationQuestionEntered();
    void slotAfficheCorrectionQuestionEntered();
    void slotFinCorrectionQuestionEntered();
    void slotBilanExerciceEntered();
    void slotBilanSequenceEntered();
    void slotBilanSequenceExited();
    void slotQuitter();
    void slotAide();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void setDimensionsWidgets();
    void updateDropResultReceived(abe::ABE_EVAL eval);

    void slotLimitePieceX();
    void slotLimitePieceY();

signals:
    void exerciceExited();
};

#endif // EXERCICEMOTSMELANGES_H
