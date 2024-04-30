/** Classe AbulEduInstructionV1 : Gestion des consignes de séquence
  * @author 2014-2015 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
  * @author 2014 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @see The GNU Public License (GNU/GPL) v3
  *
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

#ifndef ABULEDUINSTRUCTIONV1_H
#define ABULEDUINSTRUCTIONV1_H

#include <QGroupBox>
#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
#include "abuledumultimediav1.h"
#endif

namespace Ui {
class AbulEduInstructionV1;
}

class AbulEduInstructionV1 : public QGroupBox
{
    Q_OBJECT
    Q_PROPERTY(QString Title  READ abeInstructionGetTitle  WRITE abeInstructionSetTitle  USER true)
    Q_PROPERTY(bool DefaultInstructionDisabled  READ abeInstructionGetDefaultInstructionDisabled  WRITE abeInstructionSetDefaultInstructionDisabled  USER true)

public:
#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
    /** Constructeur appelé si la lib AbulEduMultiMediaV1 existe dans le projet */
    explicit AbulEduInstructionV1(QWidget *parent = 0, AbulEduMultiMediaV1 *multimedia = 0);
    /** Donne un AbulEduMultiMediaV1 à l'AbulEduInstructionV1
     * @note Utile quand on crée l'AbulEduInstructionV1 par promotion dans l'interface graphique
     * @param multimedia est le multimedia idéalement unique de l'éditeur */
    void abeInstructionSetMultimedia(AbulEduMultiMediaV1* multimedia);

#else
    /** Constructeur appelé si la lib AbulEduMultiMediaV1 n'existe pas dans le projet */
    explicit AbulEduInstructionV1(QWidget *parent = 0);
#endif

    ~AbulEduInstructionV1();

    /** Met à jour le titre du QGroupBox */
    inline void abeInstructionSetTitle(const QString& title) {setTitle(title);}

    /** Retourne le titre du QGroupBox */
    inline const QString& abeInstructionGetTitle() {return title();}

    /** Met à jour le texte de consigne */
    void abeInstructionSetText(const QString& text);


    /** Retourne la consigne par défaut */
    inline const QString& abeInstructionGetDefaultInstruction(){return m_defaultInstruction;}

#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
    /** @deprecated */
    void abeInstructionSetDefaultInstructionDisabled(bool trueFalse);

    /** Permet de jouer sur l'accessibilité du bouton Initialise */
    void abeInstructionSetDefaultInstructionEnabled(bool trueFalse);

    /** @deprecated */
    bool abeInstructionGetDefaultInstructionDisabled();

    /** Permet de savoir si le bouton Initialise est accessible */
    bool abeInstructionGetDefaultInstructionEnabled();

    /** Retourne la consigne audio par défaut */
    inline const QString& abeInstructionGetDefaultInstructionPath(){return m_defaultInstructionPath;}

    /** Enregistre la consigne courante */
    void abeInstructionSetInstructionPath(const QString& instructionPath);

    /** Retourne le chemin de la consigne audio courante */
    inline const QString& abeInstructionGetInstructionPath(){return m_currentInstructionPath;}

    /** Cache la QFrame multimedia */
    void abeInstructionHideMultiMedia();

    ///
    /// \brief Fixe la consigne par défaut
    /// \param instruction est la consigne écrite par défaut
    /// \param instructionPath est le répertoire (souvent temporaire) où doivent se copier les consignes sonores
    /// \param instructionFile est le fichier (ressource, en principe) qui correspond à l'enregistrement de la consigne écrite passée en premier paramètre
    /// \param instructionFileName est le nom que vous souhaitez que porte votre instruction par défaut si vous ne voulez pas du nom par défaut (nom par défaut : objectName() de l'AbulEduInstructionV1 débarrassé de "abeInstruction" + "ConsigneSequence.ogg")
    /// \return
    ///
    bool abeInstructionSetDefaultInstruction(const QString& instruction, const QString& instructionPath, const QString &instructionFile, const QString& instructionFileName = QString());

    /** @deprecated */
    void abeInstructionSetDefaultInstruction(const QString& instruction, const QString& instructionPath);

    /** @deprecated */
    void abeInstructionSetDefaultInstruction(const QString& instruction);

    /** Fixe la consigne par défaut */
    void abeInstructionSetInstruction(const QString& instruction, const QString& instructionPath);
#else
    /** Fixe la consigne par défaut */
    void abeInstructionSetDefaultInstruction(const QString& instruction);
#endif

    /** Retourne le texte de consigne format HTML */
    QString abeInstructionGetText();

    /** Retourne le texte de consigne format "plainText" */
    QString abeInstructionGetPlainText();

    /** Permet de cocher en programmation le QGroupBox */
    void setChecked(bool checked);

    ///
    /// \brief abeInstructionSetSingle
    /// \param trueFalse vrai pour cocher la case qui indique que la consigne doit être unique dans la séquence
    ///
    void abeInstructionSetSingle(bool trueFalse);

    ///
    /// \brief abeInstructionIsSingle
    /// \return vrai si la case qui indique que la consigne doit être unique dans la séquence est cochée
    ///
    bool abeInstructionIsSingle();

    ///
    /// \brief Rend visible la case à cocher qui permet d'indiquer que la consigne doit être unique dans la séquence
    ///
    void abeInstructionAllowSingle();
private:
    Ui::AbulEduInstructionV1 *ui;
    bool m_isSingle;
    QString m_defaultInstruction;
#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
    QString m_defaultInstructionPath;
    QString m_currentInstructionPath;
    QString m_instructionsDirectory;
    AbulEduMultiMediaV1* m_multimedia;
#endif
    void installEventFilters();

private slots:
    void on_abeInstructionBtnInitialize_clicked();

    void on_abeInstructionBtnDelete_clicked();

    void on_abeInstructionBtnShowControlAudio_clicked();

    bool eventFilter(QObject *obj, QEvent *event);

    void on_AbulEduInstructionV1_toggled(bool trueFalse);

#ifdef Q_OS_WIN
    void slotCopydefaultfile();
#endif

signals:
    void signalAbeInstructionShowControlAudio(bool);
    void signalAbeInstructionInitializeSound();
    void signalAbeInstructionObjectHovered(QString);
    void signalAbeInstructionObjectLeaved(QString);
    void signalAbeInstructionSomethingChanged();
};

#endif // ABULEDUINSTRUCTIONV1_H
