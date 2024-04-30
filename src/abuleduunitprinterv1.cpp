#include "abuleduunitprinterv1.h"
#include "ui_abuleduunitprinterv1.h"
#include "abuledumessageboxv1.h"
#include <QSettings>
#include <QCheckBox>
#include <QDebug>
#include <QPrinter>

AbulEduUnitPrinterV1::AbulEduUnitPrinterV1(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::AbulEduUnitPrinterV1),
    m_exercises(QHash<QString,QPair<QStringList,QString> >()),
    m_isInstructionAllowed(true),
    m_useGenericInstruction(true),
    m_parent(parent),
#ifndef QT_NO_PRINTER
    m_printer(new QPrinter(QPrinter::HighResolution)),
    m_printDialog(new QPrintDialog(m_printer /*, this*/))
#endif
{
    ui->setupUi(this);
#ifndef QT_NO_PRINTER
    /* Gestion Impression */
    m_printer->setPageSize(QPrinter::A4);
    m_printDialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    connect(m_printDialog, SIGNAL(rejected()), this, SLOT(slotShowDisplayPage()), Qt::UniqueConnection);
    connect(m_printDialog, SIGNAL(accepted(QPrinter*)), this, SLOT(slotUnitPrinterFilePrint(QPrinter*)), Qt::UniqueConnection);
    /* #4077 -> impossible d'integrer QPrintDialog dans un widget sous windows */
    #ifndef Q_OS_WIN
        ui->glPagePrint->addWidget(m_printDialog);
        connect(m_printDialog, SIGNAL(rejected()), m_printDialog, SLOT(show()), Qt::UniqueConnection);
        connect(m_printDialog, SIGNAL(accepted()), m_printDialog, SLOT(show()), Qt::UniqueConnection);
    #endif
#endif
    setCurrentWidget(ui->pageSelect);
    foreach(QRadioButton* rb,ui->unitPrinterFrmInstructions->findChildren<QRadioButton*>()){
        connect(rb, SIGNAL(clicked()),this, SLOT(slotSetBooleanValuesFromRadioButtons()),Qt::UniqueConnection);
    }
    /** Code jetable pour test */
    ui->unitPrinterChkAllExercise->setChecked(true);
    /** Fin du code jetable */
}

void AbulEduUnitPrinterV1::abeUnitPrinterClear()
{
    foreach(QGroupBox* childGroupBox,ui->unitPrinterGbFrame->findChildren<QGroupBox*>()){
        childGroupBox->deleteLater();
    }
    foreach(QLayout* childLayout,ui->unitPrinterGbFrame->findChildren<QLayout*>()){
        if(childLayout != ui->vlChoosePrintGbFrame)
        childLayout->deleteLater();
    }
    ui->unitPrinterCbAllExercise->clear();
    ui->unitPrinterCbAllExercise->addItem("1");;
    m_exercises.clear();
}

void AbulEduUnitPrinterV1::abeUnitPrintSetRenderSlot(QObject* obj,const char *slot){
    connect(ui->unitPrinterBtnOK, SIGNAL(clicked()),obj, slot, Qt::UniqueConnection);
}

void AbulEduUnitPrinterV1::abeUnitPrinterAddExerciseType(const QString &title, const QString& unitPath,const QString &unitKey, const QString& genericInstruction, const QString& unitArray)
{
    ui->vlChoosePrintGbFrame->removeItem(m_lastSpacer);
    QSettings* settings = new QSettings(unitPath,QSettings::IniFormat);
    QGroupBox* gb = new QGroupBox(ui->unitPrinterGbFrame);
    gb->setObjectName(QString("groupbox%1").arg(unitKey));
    gb->setTitle(title);
    QGridLayout* gl = new QGridLayout(gb);
    gb->setLayout(gl);
    settings->beginGroup(unitKey);
    int nombreExercices = settings->beginReadArray(unitArray);
    settings->endArray();
    settings->endGroup();
    QStringList exercises;
    int elementNumber = 0;
    for(int i = 0;i < nombreExercices;i++){
        QCheckBox* chk = new QCheckBox(gb);
        exercises << QString::number(i);
        chk->setObjectName(QString("%1;%2").arg(unitKey).arg(i+1));
        chk->setText(trUtf8("Exercice %1").arg(i+1));
        /** Code jetable pour test */
//        if(unitKey == "texteATrous")
//            if(i == 1) chk->setChecked(true);
//        if(unitKey == "ordreAlphabetique")
//            if(i == 1 || i ==2)chk->setChecked(true);
//        if(unitKey == "motsMelanges")
//            if(i == 0 || i ==1)chk->setChecked(true);
//        if(unitKey == "phrasesMelangees")
//            if(i == 0 || i ==1)chk->setChecked(true);
//        if(unitKey == "paragraphesMelanges")
//            if(i == 0 || i ==1)chk->setChecked(true);
//        if(unitKey == "phraseSansEspace")
//            if(i == 0 || i ==1)chk->setChecked(true);
//        if(unitKey == "rechercheRapide")
//            if(i == 0 || i ==1)chk->setChecked(true);
        /** Fin du code jetable */
        gl->addWidget(chk,0,i);
        QSpacerItem* sp = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
        gl->addItem(sp,0,i+1);
        elementNumber+=2;
    }
    QSpacerItem* spFin = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gl->addItem(spFin,0,elementNumber);
    QStringList cbItems;
    QStringListIterator listIterator(exercises);
    while(listIterator.hasNext()){
        cbItems << QString::number(listIterator.next().toInt() + 1);
    }
    if(exercises.size() > ui->unitPrinterCbAllExercise->count()){
        ui->unitPrinterCbAllExercise->clear();
        ui->unitPrinterCbAllExercise->addItems(cbItems);
    }
    m_exercises.insert(unitKey,qMakePair(exercises,genericInstruction));
    ui->vlChoosePrintGbFrame->addWidget(gb);
    QSpacerItem* sp = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->vlChoosePrintGbFrame->addSpacerItem(sp);
    m_lastSpacer = sp;
}

QTextEdit *AbulEduUnitPrinterV1::abeUnitPrinterGetTextEdit()
{
    return ui->unitPrinterTeDisplay;
}

QMap<QString, QList<int> > AbulEduUnitPrinterV1::abeUnitPrinterGetChecked()
{
    QMap<QString,QList<int> > map;
    QListIterator<QGroupBox*> i(ui->unitPrinterGbFrame->findChildren<QGroupBox*>());
    while (i.hasNext()){
        QGroupBox* g = i.next();
        QList<int> list;
        QListIterator<QCheckBox*> it(g->findChildren<QCheckBox*>());
        while (it.hasNext()){
            QCheckBox* c = it.next();
            if(ui->unitPrinterChkAllExercise->isChecked()){
                if(c->objectName().split(";", QString::SkipEmptyParts).last() == ui->unitPrinterCbAllExercise->currentText()){
                    list.append(c->objectName().split(";", QString::SkipEmptyParts).last().toInt());
                }
            }
            else{
                if(c->isChecked()){
                    list.append(c->objectName().split(";", QString::SkipEmptyParts).last().toInt());
                }
            }
        }
        if(!list.isEmpty()){
            map.insert(g->objectName().remove("groupbox"),list);
        }
    }
    return map;
}

AbulEduUnitPrinterV1::~AbulEduUnitPrinterV1()
{
    delete ui;
}

void AbulEduUnitPrinterV1::on_unitPrinterBtnBack_clicked()
{
    setCurrentWidget(ui->pageSelect);
}

void AbulEduUnitPrinterV1::on_unitPrinterBtnOK_clicked()
{
    setCurrentWidget(ui->pageDisplay);
}

void AbulEduUnitPrinterV1::on_unitPrinterBtnCancelDisplay_clicked()
{
    ui->unitPrinterBtnCancelSelect->click();
}

void AbulEduUnitPrinterV1::on_unitPrinterChkAllExercise_toggled(bool checked)
{
    ui->unitPrinterGbFrame->setEnabled(!checked);
    ui->unitPrinterCbAllExercise->setEnabled(checked);
}

void AbulEduUnitPrinterV1::slotSetBooleanValuesFromRadioButtons()
{
    if(ui->unitPrinterRbNoInstructions->isChecked()){
        m_isInstructionAllowed = false;
    }
    else{
        m_isInstructionAllowed = true;
        m_useGenericInstruction = ui->unitPrinterRbGenericInstructions->isChecked();
    }
}

void AbulEduUnitPrinterV1::on_unitPrinterBtnPrint_clicked()
{
    setCurrentWidget(ui->pagePrint);
#ifdef Q_OS_WIN
    m_printDialog->move(300,300);
    m_printDialog->show();
//    abeApp->abeCenterWindow(m_printDialog);
#endif

}

void AbulEduUnitPrinterV1::slotShowDisplayPage()
{
    setCurrentWidget(ui->pageDisplay);
}

void AbulEduUnitPrinterV1::slotUnitPrinterFilePrint(QPrinter *p)
{
    /* On imprime */
    ui->unitPrinterTeDisplay->print(p);
    /* On affiche un message */
    QString message("Impression en cours");
    AbulEduMessageBoxV1* msgImpression = new AbulEduMessageBoxV1(trUtf8("Impression"), message,true,ui->pagePrint);
    msgImpression->abeMessageBoxSetMultimedia();
    msgImpression->setWink();
    msgImpression->show();
    connect(msgImpression, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotShowDisplayPage()), Qt::UniqueConnection);
}
QWidget *AbulEduUnitPrinterV1::abeUnitPrinterGetParent() const
{
    return m_parent;
}

void AbulEduUnitPrinterV1::abeUnitPrinterSetParent(QWidget *parent)
{
    m_parent = parent;
}

