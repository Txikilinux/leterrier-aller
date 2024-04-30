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

#ifndef EXERCICEPHRASESANSESPACES_H
#define EXERCICEPHRASESANSESPACESH

//#include <QGraphicsItemAnimation>
#include <QGraphicsProxyWidget>
#include "abuleducommonstatesv1.h"
#include "abuleduexercicev1.h"
#include "abuleduapplicationv1.h"
#include "abuleduetiquettesv1.h"
#include "abuleduoutilsv1.h"
#include "abuledutextv1.h"
class ExercicePhraseSansEspaces : public AbulEduCommonStatesV1, public AbulEduOutilsV1
{
    Q_OBJECT
public:
    ExercicePhraseSansEspaces(QWidget *parent=0, QString repertoireBase ="");
    ~ExercicePhraseSansEspaces();
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

    /** Verifie si l'espace qui vient d'être placé est bien placé et si on a terminé de répondre à la question */
    void verifieEspace();


    /** Construit une serie (une par exercice) de listes de mots tirés au hazard dans un fichier cvs organisé mot, niveau, nature (séparateur tab)
      * Les listes sont triées dans l'ordre alphabétique
      * @param listeMots la "liste" de mots (mot, nature,niveau)
      * @param nbMots le nombre de mots à sélectionner dans la liste
      * @param niveau le niveau d'aquisition des mots Outils, CP, CE1, CE2, CM1,CM2
      * @param nature NOM nom, VER verbe, ADV adverbe, ADJ adjectif, PRO pronom, PRE préposition, DET déterminant, CON conjonction
      * @param profondeur le nombre de lettres (en début de mot) sur lesquelles porte le classement (0 ignoré)
      * @return Qmap avec comme clé le numéro de la liste et comme valeur la QStringListe contenant les mots.
      */
    QMap<int, QStringList> abeoutilsCreerListeMots(QStringList listeMots, int nbMots, AbulEduOutilsV1::NIVEAU niveau, QString nature, int profondeur);

    /** Le texte support */
    AbulEduTextV1 *m_texte;

    /** La phrase de l'exercice en cours */
    QString m_phrase;
    /** Le niveau de difficulté de l'exercice */
    int m_numLevel;

    void affichePhraseSansEspace(QString phraseExo);

    /** Affiche la phrase de départ avec ses espaces colorés en bleu */
    void affichePhraseCorrigee(QString phraseExo);

    /** le widget pour afficher les phrases */
    QTextEdit *m_AireDeJeu;

    /** liste des espaces dans la phrase */
    QList<int> m_espacePos;
    /** Taille de la chaine sans les espaces
      * avec 0 pour commencer
      * et la position du dernier caractère à la fin
      */
    int longueurChaineSans;
    /** Le curseur de texte */
    QTextCursor m_curseur;
    /** La position du curseur dans le texte */
    int m_curseurPosition;
    /** le format des espaces dans l'exercice */
    QTextCharFormat textFormatEspace;
    /** le format des mots en erreur dans l'exercice */
    QTextCharFormat textFormatErreur;
    /** La font de l'exercice */
    QFont fontExo;
    /** La Font de présentation du texte avant exo */
    QFont fontAvantExo;
    int JEVEUXCOLORIERETIQUETTESMALPLACEES;

    /** Le nombre d'essais effectués pour répondre à la question
      * On stocke le numéro de l'exercice puis le nombre de clics sur btnVerifier
      */
    QList<QPair<int, int> > m_resultats;

    int m_nbEspacesBienPlaces;
    int m_nbEspacesMalPlaces;
    int m_nbEspacesAPlacer;
    QStringList m_motsEnErreur;
    abe::ABE_EVAL m_EvaluationExercice;
    void redimensionneConsigne();
    void redimensionneBilan();
    void redimensionneAireDeJeu();

    QWidget *m_parent;
    QMap<int, QString> levels;
    QGraphicsProxyWidget *proxy;

    bool m_localDebug;

    /** Stocke le taux de réussite sous forme de pourcentage */
    qreal m_percent;

#ifdef __ABULEDUTABLETTEV1__MODE__
    void createBtnForTab();
    AbulEduFlatBoutonV1* btnBarreEspace;
    AbulEduFlatBoutonV1* btnCurseurGauche;
    AbulEduFlatBoutonV1* btnCurseurDroite;
#endif

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
    void slotVerificationQuestionEntered();
    void slotBilanExerciceEntered();
    void slotBilanSequenceEntered();
    void slotBilanSequenceExited();
    void slotQuitter();

    void setDimensionsWidgets();
    bool eventFilter(QObject *obj, QEvent *event);
    void cursorPositionChanged();

#ifdef __ABULEDUTABLETTEV1__MODE__
    void on_btnBarreEspace_clicked();
    void on_btnCurseurGauche_clicked();
    void on_btnCurseurDroite_clicked();
#endif
signals:
    void exerciceExited();

};

#endif // EXERCICEPHRASESANSESPACES_H
