#ifndef ABULEDUUNITPRINTERV1_H
#define ABULEDUUNITPRINTERV1_H

#include <QStackedWidget>
#include <QSpacerItem>
#include <QGroupBox>
#include <QTextDocument>
#include <QTextEdit>
#include <QPrintDialog>

namespace Ui {
class AbulEduUnitPrinterV1;
}

class AbulEduUnitPrinterV1 : public QStackedWidget
{
    Q_OBJECT

public:
    explicit AbulEduUnitPrinterV1(QWidget *parent = 0);
    ///
    /// \brief Supprime les QGroupBox qui pourraient exister dans le cadre choosePrintGbFrame
    ///
    void abeUnitPrinterClear();

    ///
    /// \brief Passe depuis un objet extérieur un slot à connecter au bouton OK.
    /// \note Seule l'application sait ce qu'il faut faire pour construire le rendu
    /// \param obj l'objet auquel appartient le slot
    /// \param slot le slot qui construit le rendu
    ///
    void abeUnitPrintSetRenderSlot(QObject *obj, const char * slot);
    ///
    /// \brief Ajoute un QGroupBox pour un type d'exercice au cadre choosePrintGbFrame
    /// \param title le titre de mon QGroupBox
    /// \param unitPath le chemin vers le fichier .conf qui va servir à peupler mon objet
    /// \param unitKey la clef dans le fichier de .conf qui correspond à mon type d'exercice
    /// \param genericInstruction une consigne générique pour mon type d'exercice
    /// \param unitArray le clef de mon tableau d'exercice. La normalisation entre applications voudrait que ce soit partout "exercise", mais ça peut être autre chose, "exercice" par exemple
    ///
    void abeUnitPrinterAddExerciseType(const QString& title, const QString& unitPath, const QString& unitKey, const QString& abeUnitPrinterGetGenericInstruction, const QString &unitArray = "exercise");

    ///
    /// \brief abeUnitPrinterGetTextDocument
    /// \return Pointeur vers le QTextDocument de unitPrinterTeDisplay
    ///
    QTextEdit *abeUnitPrinterGetTextEdit();

    QMap<QString,QList<int> > abeUnitPrinterGetChecked();

    QHash<QString,QPair<QStringList,QString> > abeUnitPrinterGetExercises(){return m_exercises;}

    ///
    /// \brief Destructeur de ma classe
    ///
    ~AbulEduUnitPrinterV1();

    ///
    /// \return si on doit afficher une consigne
    ///
    inline bool abeUnitPrinterGetIsInstructionAllowed() const {return m_isInstructionAllowed;}

    ///
    /// \return si l'éventuelle consigne à afficher est la consigne générique
    ///
    inline bool abeUnitPrinterGetUseGenericInstruction() const {return m_useGenericInstruction;}

    QWidget *abeUnitPrinterGetParent() const;
    void abeUnitPrinterSetParent(QWidget *parent);

private slots:
    void on_unitPrinterBtnBack_clicked();
    void on_unitPrinterBtnOK_clicked();
    void on_unitPrinterBtnCancelDisplay_clicked();
    void on_unitPrinterChkAllExercise_toggled(bool checked);
    void slotSetBooleanValuesFromRadioButtons();

    void on_unitPrinterBtnPrint_clicked();

    void slotShowDisplayPage();
#ifndef QT_NO_PRINTER
    void slotUnitPrinterFilePrint(QPrinter*p);
#endif

private:
    Ui::AbulEduUnitPrinterV1 *ui;
    ///
    /// \brief permet de garder un pointeur vers le dernier QSpacerItem ajouté dans le layout vlChoosePrintgbFrame, où on insère des boutons pour la fonction d'impression
    ///
    QSpacerItem* m_lastSpacer;

    ///
    /// \brief une QMap dont les clefs sont les types d'exercice (paramètres unitKey de la méthode abeUnitPrinterAddExerciseType) avec la liste des exercices
    ///
    QHash<QString,QPair<QStringList,QString> > m_exercises;

    bool m_isInstructionAllowed;

    bool m_useGenericInstruction;

    QWidget* m_parent;

#ifndef QT_NO_PRINTER
    QPrinter *m_printer;
    QPrintDialog *m_printDialog;
#endif

    /* Pas de signal : les boutons choosePrintBtnCancel, choosePrintBtnOK et XXXXX sont appelés depuis l'application par un auto-connect */

};

#endif // ABULEDUUNITPRINTERV1_H
