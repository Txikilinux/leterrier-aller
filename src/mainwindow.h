/**
  * Aller (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2015 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QFontDatabase>
#include "lib/abuleduboutonexo.h"
#include "abuledupageaccueilv1.h"
#include "abuledufilev1.h"
#include "exercicemotsmelanges.h"
#include "exercicephrasesmelangees.h"
#include "abuleduboxfilemanagerv1.h"
#include "abuledumessageboxv1.h"
#include "activityfilter.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /** Retourne l'AbulEduFileV1 vers lequel pointe m_abuleduFile */
    inline QSharedPointer<AbulEduFileV1> abeGetMyAbulEduFile() {return m_abuleduFile;}

    /** Donne la valeur file au pointeur m_abuleduFile */
    inline void abeSetMyAbulEduFile(QSharedPointer<AbulEduFileV1> file) {m_abuleduFile = file;}

    /** Permet d'accéder à page d'accueil */
    inline AbulEduPageAccueilV1* abeGetMyAbulEduAccueil(){return gv_Accueil;}

    inline AbulEduBoxFileManagerV1* abeGetMyAbulEduBoxFileManager(){return m_abuleduFileManager;}

    /** Retourne un pointeur vers le dossier temporaire créé dans le constructeur de la MainWindow
      * Fonction peut être à supprimer quand la décision définitive sera prise quant à la gestion des fichiers .abe */
    QDir abeGetMyTempDir();

    /** Vide le répertoire temporaire créé dans le constructeur de la MainWindow */
    bool abeCleanMyTempDir();
    /** Permet l'accès aux exercices en fonction de la clef exerciceActive du fichier de conf */
    void setExercisesEnabled();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void showEvent(QShowEvent *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QAction *actionQuitter;


private:
    Ui::MainWindow *ui;
        QMap<int, QString>m_texteBulles;
    QSettings *m_config;
    AbulEduPageAccueilV1 *gv_Accueil;
    void setDimensionsWidgets();
    QSharedPointer<AbulEduFileV1> m_abuleduFile;
    bool m_isExerciceEnCours;
    QDir* m_tempDir;
    bool cleanDirectory(QString dirPath, QString dirInitial);
    bool m_localDebug;
    bool m_isDemoAvailable;
    AbulEduBoxFileManagerV1* m_abuleduFileManager;
    AbulEduMessageBoxV1* m_abuleduMessageBox;
    QString m_messageAide;

    int m_numberExoCalled;
    int m_lastNumberExoCalled;

    ///
    /// \brief permet de garder un pointeur vers le dernier QSpacerItem ajouté dans le layout vlChoosePrintgbFrame, où on insère des boutons pour la fonction d'impression
    ///
    QSpacerItem* m_lastSpacer;

    void abeAiguillage();

    ///
    /// \brief modifie le texte à trous passé en paramètres pour que tous les trous aient la même longueur : celle du plus long des trous
    /// \param le texte à trous généré par l'élément second de la méthode matchHolesWords d'AllerEditorV1
    /// \return le texte à trous avec des trous de même longueur
    QString balanceTextHoles(QString text);

    QString m_locale;
    /** QTimeLine destinée à gérer le temps d'affichage du slot démo
      * Signal finished()) connecté au slotFinDemo() */
    QTimeLine* m_demoTimeLine;

    /** Détecteur d'activité */
    ActivityFilter *m_activityFilter;

public slots:
    void btnAideClicked();
    void slotSessionAuthenticated(bool enable);
    void slotDemo();
    void slotAfficheBoxFileManagerSave(QSharedPointer<AbulEduFileV1> file);

//    bool event(QEvent *event);
protected slots:
    /** lance l'exo numero x correspondant à la zone sensible cliquée */
    void abeLanceExo(int numero);
    void btnQuitterClicked();
    void btnOuvrirClicked();
    void btnBoxClicked();
    void btnFermerClicked();
    void slotExerciceExited();
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    /** Décompresse dans un fichier temporaire et ouvre un module depuis un fichier .abe */
    void on_action_Ouvrir_triggered();

    /** Lance l'éditeur */
    void on_action_Editeur_Exercice_triggered();

    /** Quitte le logiciel */
    void on_action_Quitter_triggered();

    /** Slot attaché sur le signal emis par la boite abuleduboxfilemanagerv1 */
    void slotOpenFile(QSharedPointer<AbulEduFileV1> abeFile);

    void on_actionTexte_trous_triggered();

    void on_actionOrdre_alphab_tique_triggered();

    void on_actionMots_m_lang_s_triggered();

    void on_actionPhrases_m_lang_es_triggered();

    void on_actionParagraphes_m_lang_s_triggered();

    void on_actionPhrase_sans_espaces_triggered();

    void on_actionRecherche_rapide_triggered();

    void on_actionImprimer_triggered();

    void on_actionMode_d_mo_triggered(bool checked);

    void on_action_Journal_de_mes_activit_s_triggered();

    void on_action_Changer_d_utilisateur_triggered();

    void on_stCentral_currentChanged(int arg1);

    void setTitle(int authStatus);

    void slotFinDemo();

    void slotShowMainPage();

    void slotShowEditorPage();

    ///
    /// \brief Affichage la page d'impression d'exercice
    /// \note le sender est un AbulEduFlatBoutonV1 à qui j'ai passé le type d'exercice (tel qu'écrit dans le module) en whatsThis() et le numéro d'exercice dans la séquence en objectName()
    ///
    void slotBuildRenderPage();

    void slotSwitchExitBoxFileManager(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool);

    void debutTestParametres();

    void slotAfficheBoxFileManagerOpen();

    ///
    /// \brief AutoConnect du bouton Annuler de la page de sélection de l'AbulEduUnitPrinterV1
    /// \note Retour sur la page principale de l'application
    ///
    void on_unitPrinterBtnCancelSelect_clicked();

    void slotGoToPrint();
    void slotDontGoToPrint();
};

#endif // MAINWINDOW_H
