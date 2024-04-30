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

 * @file abuleduexercicewidgetmessagev1.h
 * @version 1
 */

#ifndef ABULEDUEXERCICEWIDGETMESSAGEV1_H
#define ABULEDUEXERCICEWIDGETMESSAGEV1_H

#include <QGraphicsProxyWidget>
#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QTextEdit>
#include <QLabel>
#include "ui_abuleduexercicewidgetmessagev1.h"
#include "abuleduapplicationv1.h"
#include "abuledupageaccueilv1.h"
#include "abuleduflatboutonv1.h"
#include "abuledumultimediav1.h"
#include "abuleduloggerv1.h"

#if QT_VERSION > 0x050000
    #include <QScroller>
#endif

namespace Ui {
    class AbulEduExerciceWidgetMessageV1;
}

class AbulEduExerciceWidgetMessageV1 : public QWidget
{
    Q_OBJECT

public:

    /** Constructeur de la classe
      * @param QWidget *parent, un pointeur sur le widget parent de l'objet, initialise par defaut a 0
      */
    explicit AbulEduExerciceWidgetMessageV1(QWidget *parent = 0);

    /** Destructeur par defaut de la classe
      */
    ~AbulEduExerciceWidgetMessageV1();

    /**
      * Pour fermer le widget message et appeller le destructeur
      */
    void abeWidgetMessageClose();

    Ui::AbulEduExerciceWidgetMessageV1 *ui;

    /** Permet de definir l'image de fond du widget
      * @param chemin, chaine de caractere, le chemin de l'emplacement de l'image
      * @return void
      */
    void abeWidgetMessageSetImageFond(const QString&);

    void abeWidgetMessageShowImageFond(bool trueFalse);

    bool abeWidgetMessageHasImageFond();

    /** Permet de definir le titre
      * @param titre, chaine de caractere, le titre du message
      * @return void
      */
    void abeWidgetMessageSetTitre(const QString &);

    /** Permet de definir la consigne
      * @param consigne, chaine de caractere, la consigne que l'on veut appliquer
      * @return void
      */
    void abeWidgetMessageSetConsigne(const QString&);

    /** Permet de definir le texte de l'exercice
      * @param textExercice, chaine de caractere, le texte de l'exercice
      * @return void
      */
    void abeWidgetMessageSetTexteExercice(const QString&);

    /** Permet de redimensionner le widgetMessage
      * @brief appel de la fonction AbulEduExerciceWidgetMessageV1::setDimensionWidget()
      * @see setDimensionsWidget()
      * @return void
      */
    void abeWidgetMessageResize();

    /** Permet de rendre visible ou non la zone de texte
      * @param yesNo, booleen, true si on veut l'afficher, sinon false
      * @return void
      */
    void abeWidgetMessageSetZoneTexteVisible(bool yesNo);

    /** Met le booléen m_isDisplayError à vrai */
    void abeWidgetMessageSetIsDisplayError();

    /** Rend le bouton Fermer visible ou pas */
    void abeWidgetSetBtnFermerVisible(bool yesNo);

    /** Donne un AbulEduMediaMedias à lire à l'AbulEduMultiMediaV1 */
    void abeWidgetMessageSetMediaMedias(const AbulEduMediaMedias &);

    ///
    /// \brief Retourne un pointeur vers le layout de la frame Custom
    /// \note Initialisé à QBoxLayout::LeftToRight
    ///
    inline QBoxLayout* abeWidgetMessageGetFrmCustomLayout(){return m_frmCustomLayout;}

protected:
    /** Permet de changer d'evenement
      * @param *e, QEvent, un pointeur sur un evenement
      * @link http://qt-project.org/doc/qt-4.8/QEvent.html
      */
    void changeEvent(QEvent *e);

    void showEvent(QShowEvent *);

private:
    ///
    /// \brief La scene attachée à la view ui->gvPrincipale
    ///
    QGraphicsScene        *gsPrincipale;

    ///
    /// \brief Permet d'afficher l'image de fond dans la scene
    ///
    QGraphicsPixmapItem   *m_fond;

    ///
    /// \brief L'image de fond
    ///
    QPixmap                m_imageFond;

    ///
    /// \brief QFrame hébergeant les widgets de texte et autres
    ///
    QFrame *frPrincipale;

    ///
    /// \brief Layout vertical dans lequel on vient ranger les différents objets
    ///
    QVBoxLayout *m_vLayout;

    ///
    /// \brief Le titre de la fenêtre de consigne ou de bilan
    ///
    QLabel *lbTitre;

    ///
    /// \brief Trait de séparation entre le titre et le corps de la consigne
    ///
    QLabel *lbSeparationTitre;

    ///
    /// \brief Le texte de la consigne ou le message d'évaluation
    ///
    QTextEdit *teConsigne;

    ///
    /// \brief Une QFrame destinée à afficher les boutons de l'AbulEduMultiMediaV1
    ///
    QFrame* m_frmMultimedia;

    ///
    /// \brief Trait de séparation sous le message de consigne
    ///
    QLabel *lbSeparationConsigne;

    ///
    /// \brief Une QFrame destinée à ajouter des éléments, personnalisée selon les projets
    ///
    QFrame * m_frmCustom;

    ///
    /// \brief Le QBoxLayout de la QFrame m_frmCustom
    ///
    QBoxLayout* m_frmCustomLayout;

    ///
    /// \brief Le champ pour le texte de l'exercice
    ///
    QTextEdit *teTextExercice;

    ///
    /// \brief Pour tasser le tout en haut
    ///
    QSpacerItem *spacer;

    ///
    /// \brief AbulEduMultiMediaV1 qui permet la lecture vocale des consignes
    ///
    AbulEduMultiMediaV1* m_multimedia;

    ///
    /// \brief Le ratio avant redimensionnement, qui permet des calculs de recalage
    ///
    float m_previousRatio;

    ///
    /// \brief m_isDisplayError
    ///
    bool m_isDisplayError;

    ///
    /// \brief m_hasImageFond
    ///
    bool m_hasImageFond;

    ///
    /// \brief Pointeur vers le QWidget parent
    ///
    QWidget* m_parent;

    ///
    /// \brief Style CSS, lu dans le fichier abuleduexercicev1.conf, qui sera appliqué s'il n'y a pas d'image de fond
    ///
    QString m_defaultStyleSheet;

private slots :

    /** Permet de redimensionner le widget à la bonne taille
      * @return void
      */
    void setDimensionsWidget();

    /** Gere l'evenement appuie sur la touche
      */
    void keyPressEvent(QKeyEvent *event);

    /** Gere l'evenement relachement de la touche
      */
    void keyReleaseEvent(QKeyEvent *event);

    /** Gere l'événement relachement de la souris
      * Cache cet AbulEduExerciceWidgetMessageV1 si c'est un "message d'erreur" */
    void mouseReleaseEvent(QMouseEvent * event);

    QString scaleImgWidthHtml(QString html,float ratio);
    QString scaleImgHeightHtml(QString html,float ratio);

    void btnFermer_clicked();
    void slotRepaintParent();

signals:
    void signalAbeExerciceWidgetMessageMultimediaEvent();

};

#endif // ABULEDUEXERCICEWIDGETMESSAGEV1_H
