/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2011 André Connes <andre.connes@wanadoo.fr>
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
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

#ifndef EXERCICECLOSURE_H
#define EXERCICECLOSURE_H
#include <QGraphicsProxyWidget>
#include <QFile>
#include <QSettings>
#include <QScrollBar>

#include "abuledutextv1.h"
#include "abuleduoutilsv1.h"
#include "abuleduexercicev1.h"
#include "abuleduetiquettesv1.h"
#include "abuleduapplicationv1.h"
#include "abuleducommonstatesv1.h"
#include "abuledupiecebuttonv1.h"

class ExerciceClosure : public AbulEduCommonStatesV1, public AbulEduOutilsV1
{
    Q_OBJECT

public:
    ExerciceClosure(QWidget *parent = 0, QString repertoireBase = "");
    ~ExerciceClosure();

    static QList<QPair<int, int> > returnHolesFromAutomatic(QMap<int, QPair<QString, QPair<int, int> > > superMots, int holesNumber, int beginning, int end);
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
    //Affiche le texte et les etiquettes
    void afficheTexteEtEtiquettes();
    /** Le fichier de configuration de la séquence */
    QSettings *m_config;

    /** Une AbulEduMessageBoxV1 déclarée en attribut, et réutilisée plutôt que d'en créer une à chaque fois et la détruire à la fermeture */
    AbulEduMessageBoxV1* m_messageBox;

    bool reponseJuste;
    int JEVEUXCOLORIERETIQUETTESMALPLACEES;

    /** Pour stocker AbulEduLabelWidget */
    QGraphicsProxyWidget *proxy;

    /** AbulEduEtiquette pour placer les pièces et cellules mots */
    AbulEduEtiquettesV1 *m_aireDeJeu;

    //    int m_nbMotsARanger;
    AbulEduTextV1 *m_texte;

    /** Structure permettant de stocker les mots du texte, chaque mot étant (rang, (mot,(index début,index fin))) */
    QMap <int,QPair<QString,QPair<int,int> > > m_listeSuperMots;

    /** Le niveau de difficulté de l'exercice */
    int m_numLevel;

    /** Le nombre d'essais effectués pour répondre à la question
      * On stocke le numéro de l'exercice puis le nombre de clics sur btnVerifier
      */
    QList<QPair<int, int> > m_resultats;

    QGraphicsPixmapItem *tete;

    QMap<int, QStringList> abeoutilsCreerListeMots(QStringList listeMots, int nbMots, abe::ABE_LEVEL niveau, QString nature, int profondeur);

    /** Place les mots sur leur zone */
    void placeMots();

    /** Place les cellules qui reconstituent le texte, que leur contenu soit plein ou vide */
    void placeCellules();

    QWidget *m_parent;

    /** Liste des Cellules acceptant des pieces */
    QList<AbulEduCelluleV1 *> m_listeCellulesVides;

    /** Liste des pièces */
    QList<AbulEduPieceV1* > m_listePieces;

    /** Liste des Cellules n'acceptant pas de pieces. */
    QList<AbulEduCelluleV1 *> m_listeCellulesPleines;

    /** Liste de toutes les cellules du texte, qu'elles acceptent des pièces ou non */
    QList<AbulEduCelluleV1 *> m_listeCellules;

    /** Liste des pièces placées dans une cellule */
    QList<AbulEduPieceV1* > m_listePiecesPlacees;

    /** Liste des évaluations reçues par l'exercice, évaluations émises par les cellules */
    QList<abe::ABE_EVAL> m_receivedDropResults;

    /** Enregistrement de l'ordre dans lequel les étiquettes sont présentées en début d'exercice
      * Utilisée pour les logs */
    QStringList m_ordrePresentation;

    /** Nettoyage de la scene */
    void clearScene();

    void redimensionneConsigne();
    void redimensionneBilan();

    /** fonction qui retourne le rang dans le texte du mot dans lequel se trouve le curseur à index
      * @param index est la position du curseur pour laquelle on veut savoir si c'est à l'intérieur d'un mot
      * @param est le rang dans m_listeSuperMots à partir duquel on recherche
      * @return si la position ne correspond pas à un mot, la fonction retourne -1 */
    int findWordWhereIndex(int index, int indexDepart=0);

    /** fonction qui retourne le rang dans le texte du trou dans lequel se trouve le curseur à index
      * @param index est la position du curseur pour laquelle on veut savoir si c'est à l'intérieur d'un trou
      * @param est le rang dans m_listeSuperMots à partir duquel on recherche
      * @return si la position ne correspond pas à un mot, la fonction retourne -1 */
    int findHoleWhereIndex(int index, QList< QPair<int,int> > listeTrous);

    /** crée selon un mode (manuel, intelligent ou automatique) lu dans le fichier module.conf la liste des trous à faire dans l'extrait de texte */
    QList< QPair<int,int> > createHoleInText();

    bool m_localDebug;

    int m_maxPieceWidth;
    int m_maxCelluleWidth;
    QList<QPair<QPointF,AbulEduPieceV1*> > m_placesPourPieces;

    int m_precedentDebutPlaces;

    /** Stocke le taux de réussite sous forme de pourcentage */
    qreal m_percent;

    AbulEduPieceButtonV1* m_upButton;
    AbulEduPieceButtonV1* m_downButton;

    float m_pointSize;

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
    void slotBilanSequenceEntered();
    void slotBilanSequenceExited();
    void slotQuitter();
    void slotAide();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void setDimensionsWidgets();
    void updateDropResultReceived(abe::ABE_EVAL eval);
    void movePieceOrdinate(int y);
    void changeOriginalPos(QString nom, int numCellule);
    /** Déplace les pièces affichées vers le haut ou le bas, selon que movement vaut -1 ou 1 */
    void slotMoveDisplayPieces(AbulEduPieceV1 *piece);
    void slotPaintHoverPiece(AbulEduPieceV1* p);
    void slotUnPaintHoverPiece(AbulEduPieceV1* p);
    void slotPaintPressedPiece(AbulEduPieceV1* p);
    void slotUnPaintPressedPiece(AbulEduPieceV1* p);
    void slotPaintDisabledPiece(AbulEduPieceV1* p);
    void slotUnPaintDisabledPiece(AbulEduPieceV1* p);

    void slotLimitePieceX();
    void slotLimitePieceY();

signals:
    void exerciceExited();
};

#endif // EXERCICECLOSURE_H
