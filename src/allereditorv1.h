/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2012 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2014 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#ifndef ALLEREDITORV1_H
#define ALLEREDITORV1_H

#include <QSettings>
#include <QDebug>
#include <QLayout>
#include <QScrollBar>
#include <QTextCursor>
#include <QInputDialog>

#include "mainwindow.h"
#include "abuledutextv1.h"
#include "lib/abuledutexteditv1.h"
#include "abuleducorrecteurv1.h"
#include "abuledumediathequepushv1.h"
#include "abuleduboxfilemanagerv1.h"
#include "abuledumultimediav1.h"
#include "abuleduinstructionv1.h"

namespace Ui {
    class AllerEditorV1;
}

class AllerEditorV1 : public QWidget
{
    Q_OBJECT

public:
    /** Constructeur de la classe AllerEditorV1
      * @param QString nomModule est le nom d'un fichier .abe, initialisé vide
      * Si QString nomModule est vide, un allerEditorV1 vierge est créé.
      * Si QString nomModule n'est pas vide, le constructeur va lire les informations dans un fichier module.conf
      */
    explicit AllerEditorV1(QWidget *parent = 0, QString nomModule = QString());

    void abeEditorSetMainWindow(QWidget *mw);

    AbulEduTextV1 *abeEditorGetText();

    /** Destructeur de la classe AllerEditorV1 */
    ~AllerEditorV1();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent * event);

private:
    Ui::AllerEditorV1 *ui;
    AbulEduBoxFileManagerV1* m_abuleduFileManager;
    AbulEduMultiMediaV1* m_multiMedia;
    QString m_currentSoundPath;
    QSharedPointer<AbulEduFileV1> m_abuleduFile;


    /** Stocke la MainWindow parent passée au constructeur */
    QWidget* m_widgetMainwindow;

    /** Stocke le lien vers le fichier module.conf et permet son parcours pour lecture et écriture */
    QSettings* m_config;

    /** Stockent le nombre d'exercices de chaque type du module */
    int m_nombreExerciceTexteTrous;
    int m_nombreExerciceOrdreAlphabetique;
    int m_nombreExerciceMotsMelanges;
    int m_nombreExercicePhrasesMelangees;
    int m_nombreExerciceParagraphesMelanges;
    int m_nombreExercicePhrasesSansEspace;
    int m_nombreExerciceRechercheRapide;

    /** Les mots du texte avec leur numéro d'ordre dans le texte */
    QMap<int,QString> m_listeMotsIndexes;

    /** Garde au changement d'onglet la valeur de l'onglet précédent le temps de l'enregistrement du fichier module.conf
      * Mis à jour ensuite grâce à QTab::currentIndex() */
//    int m_previousIndex;
    QWidget* m_previousWidget;

    /** Garde au changement d'index la valeur de l'index de la comboBox "Exercice" le temps de l'enregistrement du fichier module.conf
      * Cet attribut est utilisé pour tous les onglets */
    int m_previousExercice;

    int m_previousStart;

    QString m_previousSelection;

    /** Indique si le fichier de configuration existe déjà, c'est à dire si on est sur un nouveau module ou pas */
    bool m_isFileSettingsFound;

    /** Stocke le texte de l'exercice en cours dans un AbuleduTextV1 */
    AbulEduTextV1 *m_text;

    /** Nombres d'objets contenus dans le texte */
    int m_numberWords;
    int m_numberSentences;
    int m_numberParagraphs;

    /** QTextEdit contenant le texte où on doit éditer des trous */
    AbulEduTextEditV1* m_textEditForHoles;

    /** Stocke la liste des paragraphes sélectionnés pour construire Paragraphes mélangés,
      * ou des phrases sélectionnées pour Phrases mélangées, ou des mots sélectionnés pour Mots mélangés ou Ordre Alphabétique*/
    QStringList m_selectedObjectsList;

    /** Structure destinée à stocker les trous pour Texte à trous sous forme de deux positions du curseur : le début et la fin */
    QList< QPair<int,int> > m_holeArray;

    QPair<int, int> m_lastSelection;

    /** Permet de conserver la détection de changement dans le TEteTexte */
    bool m_isTextModify;

    /** Stocke les objets sélectionnés pour être utilisés (paragraphes, phrases ou mots) avec le numéro de la place qu'ils ont dans le texte */
    QMap<int,AbulEduTextEditV1*> m_displayedObjectsMap;

    QMap<QString,QString> m_nameTabs;

    /** Pointeur vers un correcteur orthographique */
    AbulEduCorrecteurV1* m_correcteur;
    /** Un curseur pour le texte à vérifier */
    QTextCursor m_curseur;
    /** Le menu contextuel qui sera affiché */
    QMenu* m_menuContextuel;

    QTextCharFormat m_textFormatSelected;
    QTextCharFormat m_textFormatUnSelected;

    /** Connecte les éléments interactifs à la fonction setItHasBeenModified() */
    void connectToHasChanged();

    /** Retourne la liste des chemins absolus des fichiers contenus dans le directoryName et ses sous-dossiers */
    QStringList recursiveRide(QString directoryName);

    /** Place les mots du texte dans des AbuleduTextEditV1 dont le placement dépend de la fenêtre qui les reçoit
      * @param indexed est la QMap des objets à afficher (mots, phrases ou paragraphes) indexés
      * @param begining est l'index du premier à afficher
      * @param end est l'index du dernier à afficher
      * @param direction est l'orientation : mots les uns à côté des autres (Qt::Horizontal) ou phrases et paragraphes les uns sous les autres (Qt::Vertical), choix par défaut */
    void displayToChoice(QMap<int,QString> indexed, int begining, int end, int direction = Qt::Vertical);

    /** Pointeur vers une scrollArea, utile pour pouvoir la fermer dans un slot différent de la fonction dans laquelle on l'a créé */

    /** Filtre d'évènements */
    bool eventFilter(QObject *obj, QEvent *event);

    /** Enregistre le fichier .abe
      * @param si "nouveau" vaut "new", un nouveau fichier sera créé. */
    bool prepareSauvegarde(QString nouveau = "");

    /** Supprime l'exercice présentement affiché dans l'éditeur */
    void deleteCurrentExercise(int num,QString typ);

    /** Charge depuis le fichier module.conf les paramètres de l'exercice correspondant à l'onglet indexOngletExercice
      * @param indexOnglet représente l'onglet du twEditor dans lequel on se trouve
      * @param indexExercice représente la position de la comboBox Exercice*/
    void loadSettings(QWidget* onglet, int indexExercice);

    /** Enregistre dans le fichier module.conf les paramètres de l'exercice correspondant à l'onglet indexOngletExercice
      * @param indexOnglet représente l'onglet du twEditor dans lequel on se trouve
      * @param indexExercice représente la position de la comboBox Exercice*/
    void saveSettings(QWidget* onglet, int indexExercice);

    ///
    /// \brief Charge dans l'éditeur les paramètres d'un module
    /// \param unitName le nom du module. Il est initialisé à QString() pour le cas où on veut créer un nouveau module
    ///
    void setNewUnitSettings(QString unitName = QString());

    /** Factorise la mise en place des onglets et du chemin pour le constructeur */
    void setTabSettings();

    /** Factorise le paramétrage initial de l'onglet Texte */
    void setTabTextSettings();

    /** Factorise le paramétrage initial de l'onglet Texte à trous */
    void setTabTTRSettings();

    /** Factorise le paramétrage initial de l'onglet Ordre alphabétique */
    void setTabOASettings();

    /** Factorise le paramétrage initial de l'onglet Mots mélangés */
    void setTabMOMSettings();

    /** Factorise le paramétrage initial de l'onglet Phrases mélangées */
    void setTabPHMSettings();

    /** Factorise le paramétrage initial de l'onglet Paragraphes mélangés */
    void setTabPAMSettings();

    /** Factorise le paramétrage initial de l'onglet Phrase sans espaces */
    void setTabPSESettings();

    /** Factorise le paramétrage initial de l'onglet Recherche rapide */
    void setTabRRSettings();

    ///
    /// \brief création d'une consigne par défaut pour un AbulEduInstructionV1
    /// \param abeInstruction l'AbulEduInstructionV1
    /// \param instructionFileName le fichier ressource d'instruction par défaut
    /// \return true si la copie s'est faite
    ///
    bool createDefaultInstruction(AbulEduInstructionV1 *abeInstruction, const QString &instruction, const QString& instructionResourceName);

    void allerEditorCleanProject();

public slots:
    void slotAbeEditorShowEditorMainPage();
    void slotAbeEditorShowEditorTextPage();
    void resizeEvent(QResizeEvent *e);
    /** Créé une fenêtre dans laquelle des éléments pourront être choisis par l'utilisateur en vue de la création d'un
      * exercice en mode "Manuel" */
    QPair<QStringList,QString> matchHolesWords(QString text, QString holes, int beginning = -1, int end = -1);

private slots:
    void slotLoadTextFromData(QSharedPointer<AbulEduFileV1> abeFile, int code);

    /** Ouvre une fenêtre d'éditeur vide pour créer un nouveau projet */
    void on_action_Nouveau_triggered();

    /** Ouvre une fenêtre d'éditeur pour un projet existant
      * QFileDialog pour trouver le projet
      * Lecture du contenu des champs dans le fichier module.conf et le(s) fichier(s) XML */
    void on_actionEditer_un_projet_triggered();

    /** Enregistre le projet en cours dans l'éditeur
      * Crée l'arborescence si elle n'existe pas dans le dossier ~/leterrier/leterrier-aller/nomProjet */
    void on_actionEnregistrer_le_projet_en_cours_triggered();

    /** Publie un module sous forme de fichier .abe sur AbulÉduMédiathèque */
    void on_actionPublier_sur_AbuleduMediatheque_triggered();

    /** Quitte l'éditeur */
    void on_action_Quitter_triggered();

    /** Ajoute un nouvel index au comboBox Exercice de l'onglet correspondant,
      * champs initialisés à la valeur automatique si elle existe (choix de paresseux ;-) */
    void on_TTRbtnAjouterExercice_clicked();
    void on_OAbtnAjouterExercice_clicked();
    void on_MOMbtnAjouterExercice_clicked();
    void on_PHMbtnAjouterExercice_clicked();
    void on_PAMbtnAjouterExercice_clicked();
    void on_PSEbtnAjouterExercice_clicked();
    void on_RRbtnAjouterExercice_clicked();

    /** Destiné à être connecté au signal QTabWidget::currentChanged ( int index )
      * Enregistre dans le fichier module.conf les valeurs de l'onglet quitté
      * Charge les valeurs du fichier module.conf dans le nouvel onglet */
    void onSwitchOnglet(int nouvelIndex);

    /** Déclenche l'enregistrement puis le re-chargement des valeurs au changement des comboBox exercice dans les onglets
      * @param index est l'index affiché après changement */
    void on_TTRcbNumeroExercice_currentIndexChanged(int index);
    void on_OAcbNumeroExercice_currentIndexChanged(int index);
    void on_MOMcbNumeroExercice_currentIndexChanged(int index);
    void on_PHMcbNumeroExercice_currentIndexChanged(int index);
    void on_PAMcbNumeroExercice_currentIndexChanged(int index);
    void on_PSEcbNumeroExercice_currentIndexChanged(int index);
    void on_RRcbNumeroExercice_currentIndexChanged(int index);

    /** Change la couleur d'une étiquette sélectionnée dans les fenêtres de choix manuel.
      * Met à jour les QLineEdit "PAMleParagraphesChoisis", "OAteMotsModeManuel", "MOMteMotsModeManuel" ou "RRteMotsModeManuel" */
    void onAbuleduLabelSelected(int rang);

    /** Permet d'accéder au texte du QTextEdit de l'onglet Texte (en lecture seule sinon)
      * Enregistre le texte si sur le bouton est écrit "Enregistrer" */
    void on_TEbtnModifier_clicked();

    /** Met le booléen m_isTextModify à true */
    void onTEtextModify();

    void onTTRtextSelected();

    /** Enregistre le texte contenu dans m_text dans les fichiers de l'archive .abe */
    void saveTextAsFile();

    /** Appelle la fonction "abetextNettoieText()" d'AbulEduTexte */
    void on_TEbtnNettoyer_clicked();





    void on_TTRbtnChoixMots_clicked();
    void on_OAbtnChoixMots_clicked();
    void on_MOMbtnChoixMots_clicked();
    void on_PHMbtnChoixPhrasesManuel_clicked();
    void on_PAMbtnChoixParagraphes_clicked();
    void on_PSEbtnChoixPhrase_clicked();
    void on_RRbtnChoixMots_clicked();

    /** Établissent les choix par défaut d'accessibilité des objets */
    void setDefaultSettingsTexteTrous();
    void setDefaultSettingsOrdreAlphabetique();
    void setDefaultSettingsMotsMelanges();
    void setDefaultSettingsPhrasesMelangees();
    void setDefaultSettingsParagraphesMelanges();
    void setDefaultSettingsPhrasesSansEspace();
    void setDefaultSettingsRechercheRapide();

    /** Slots des boutons généraux de l'éditeur */
    void on_btnAnnulerTout_clicked();

    /** Slots liés à la détection de changements dans des paramètres */
    void on_MOMteMotsModeManuel_textChanged();
    void on_PSEtePhraseModeManuel_textChanged();
    void on_PSEsbNumeroPhrase_valueChanged(int arg1);
    void on_MOMsbNumeroPhrase_valueChanged(int arg1);
    void on_OAteMotsModeManuel_textChanged();
    void on_PHMtePhrasesModeManuel_textChanged();
    void on_RRteMotsModeManuel_textChanged();
    void on_RRsbMotDebut_valueChanged(int arg1);
    void on_RRsbMotFin_valueChanged(int arg1);
    void adjustPHMsbValues(int inutilise);


    /** Remplace le mot sélectionné par le mot passé en paramètre
      * utilisé par le correcteur orthographique
      * @param proposition le nouveau mot
      */
    void remplaceMot(QString proposition);

    void deselectionne();

    void setItHasBeenModified();

    void slotOpenFile(QSharedPointer<AbulEduFileV1> abeFile);

    void on_btnWigdetSuivant_clicked();
    void on_btnWidgetPrecedent_clicked();

    void updateLabelWidgetName(int index);

    /** Slots accrochés au clic sur les entrées de menu */
    void allerEditorShowTutorial(QUrl url);
    void on_actionEnregistrer_le_projet_sous_triggered();

    void displayWidget(bool yesNo, QCheckBox* from = 0);

    void on_TTRsbMarqueurFinTexte_valueChanged(int arg1);

    void on_TTRsbMarqueurDebutTexte_valueChanged(int arg1);

    void displaySaveLocation(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation loc, QString name,bool success);

    void on_btnPublier_clicked();

    void on_btnEnregistrer_clicked();

    void on_btnEssayer_clicked();

    void on_actionEssayer_ces_param_tres_triggered();

    void on_TTRbtnDeleteCurrentExercise_clicked();

    void on_OAbtnDeleteCurrentExercise_clicked();

    void on_MOMbtnDeleteCurrentExercise_clicked();

    void on_PHMbtnDeleteCurrentExercise_clicked();

    void on_PAMbtnDeleteCurrentExercise_clicked();

    void on_PSEbtnDeleteCurrentExercise_clicked();

    void on_RRbtnDeleteCurrentExercise_clicked();

    void on_TEbtnOuvrirTexte_clicked();

    void slotLocalFileDialogSelected(QString);

    void slotRecordingStarted();

    void slotRecordingStopped();

    void onCheckBoxRecordToogled(bool trueFalse);

    void on_twEditor_currentChanged(int arg1);

    void on_allerEditorBtnCurrent_clicked();

    void on_allerEditorBtnNew_clicked();

    void on_allerEditorBtnOpen_clicked();

    void on_AAbtnAnnuler_clicked();

    void on_AAbtnReinitialiser_clicked();

    void on_AbtnValider_clicked();

    void on_allerEditorBtnTutorials_clicked();

    void on_allerEditorBtnTutoGetTexte_clicked();

    void on_allerEditorBtnTutoAutoSettings_clicked();

    void on_allerEditorBtnTutoChangeInstruction_clicked();

    void on_allerEditorBtnTutoManualSettings_clicked();

    void on_allerEditorBtnTutoSaveUnit_clicked();

    void on_allerEditorBtnCloseTutorials_clicked();

signals:
    void hasChanged();
    void editorLeft();
    void editorTest();
    void editorChoose();
    void editorChooseImport();
    void editorNew();
    void editorSave(QSharedPointer<AbulEduFileV1>);
};

#endif // ALLEREDITORV1_H

