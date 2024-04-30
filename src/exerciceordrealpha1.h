/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#ifndef EXERCICEORDREALPHA1_H
#define EXERCICEORDREALPHA1 _H

#include <QScrollBar>
#include "abuleducommonstatesv1.h"
#include "abuleduexercicev1.h"
#include "abuleduapplicationv1.h"

#include "abuleduetiquettesv1.h"

#include "abuleduoutilsv1.h"
#include "abuledutextv1.h"
//#include "ui_abuleduexercicev1.h"
#include <QGraphicsProxyWidget>


class ExerciceOrdreAlpha1 : public AbulEduCommonStatesV1, public AbulEduOutilsV1
{
    Q_OBJECT
public:
    ExerciceOrdreAlpha1(QWidget *parent=0, QString repertoireBase ="");
    ~ExerciceOrdreAlpha1();

    static QStringList takeWords(const QString &unitPath, int exerciseNumber, int wordsNumber, AbulEduTextV1* text);
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

    /** AbulEduEtiquettesV1 est dérivée de qGraphicsview */
   AbulEduEtiquettesV1 *gv_AireDeJeu;

    /** Construit une serie (une par exercice) de listes de mots tirés au hazard dans un fichier cvs organisé mot, niveau, nature (séparateur tab)
      * Les listes sont triées dans l'ordre alphabétique
      * @param listeMots la "liste" de mots (mot, nature,niveau)
      * @param nbMots le nombre de mots à sélectionner dans la liste
      * @param niveau le niveau d'aquisition des mots Outils, CP, CE1, CE2, CM1,CM2
      * @param nature NOM nom, VER verbe, ADV adverbe, ADJ adjectif, PRO pronom, PRE préposition, DET déterminant, CON conjonction
      * @param profondeur le nombre de lettres (en début de mot) sur lesquelles porte le classement (0 ignoré)
      * @return Qmap avec comme clé le numéro de la liste et comme valeur la QStringListe contenant les mots.
      */
    QMap<int, QStringList> abeoutilsCreerListeMots(QStringList listeMots, int nbMots, abe::ABE_LEVEL niveau, QString nature, int profondeur);

    /** Le texte support */
    AbulEduTextV1 *m_texte;

    /** Le niveau de difficulté de l'exercice */
    int m_numLevel;

    /** Liste de tous les mots avec leurs attributs */
    QStringList m_listeTousLesMots;

    /** La liste des mots à classer, présentée dans le bon ordre */
    QStringList m_listeMots;

    /** Nombre de mots dans l'exercice */
    int m_nbMotsARanger;

    /** Liste des pieces */
    QList<AbulEduPieceV1 *> m_listeEtiquettes;

    /** Liste des Cellules */
    QList<AbulEduCelluleV1 *> m_listeCellules;

    /** QMap des listes de mots (une liste par exo dans la séquence) */
    QMap<int, QStringList> m_listeExo;

    /** Affiche les mots à classer dans le désordre
      * et crée les Pieces correspondantes
      * les mots à afficher sont dans m_listeMots
      */
    void afficheMots();

    /** Place les mots sur leur zone */
    void placeMots();

    /** Place les Cellules sur leur Zone */
    void placeCellules();

    /** Le nombre d'essais effectués pour répondre à la question
      * On stocke le numéro de l'exercice puis le nombre de clics sur btnVerifier
      */
    QList<QPair<int, int> > m_resultats;

    /** Nettoie la scene de tous les items
      * Remets les listes à zéro
      */
    void clearScene();

    int maxLargeurEtiquette;

    QWidget *m_parent;
    QGraphicsPixmapItem *tete;
    QGraphicsProxyWidget *proxy;
    bool reponseJuste;
    QLinearGradient gradient;
    QLinearGradient gradientOk;
    QLinearGradient gradientFaux;
    int JEVEUXCOLORIERETIQUETTESMALPLACEES;
    void redimensionneConsigne();

    bool m_localDebug;

    /** Liste des évaluations reçues par l'exercice, évaluations émises par les cellules */
    QList<abe::ABE_EVAL> m_receivedDropResults;

    /** Stocke le taux de réussite sous forme de pourcentage */
    qreal m_percent;

private slots:
    void slotSequenceEntered();
    void slotPresenteSequenceEntered();
    void slotExercicesEntered();
    void slotPresentationExerciceEntered();
    void slotRealisationExerciceEntered();
    void slotInitQuestionEntered();
    void slotQuestionEntered();
    void slotAfficheCorrectionQuestionEntered();
    void slotFinCorrectionQuestionEntered();
    void slotAfficheVerificationQuestionEntered();
    void slotFinVerificationQuestionEntered();
    void slotFinQuestionEntered();
    void slotBilanExerciceEntered();
    void redimensionneBilan();
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

#endif // EXERCICEORDREALPHA1_H
