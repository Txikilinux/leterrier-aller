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

#include "abuleduinstructionv1.h"
#include "ui_abuleduinstructionv1.h"

#ifndef DEBUG_ABULEDUINSTRUCTIONV1
    #include "abuledudisableloggerv1.h"
#endif

#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
AbulEduInstructionV1::AbulEduInstructionV1(QWidget *parent, AbulEduMultiMediaV1* multimedia) :
    QGroupBox(parent),
    ui(new Ui::AbulEduInstructionV1),
    m_isSingle(false),
    m_defaultInstruction(QString()),
    m_defaultInstructionPath(QString()),
    m_currentInstructionPath(QString()),
    m_instructionsDirectory(QString()),
    m_multimedia(multimedia)
{
    ui->setupUi(this);
    installEventFilters();
    ui->abeInstructionBtnShowControlAudio->setEnabled(false);
    ui->abeInstructionBtnDelete->setEnabled(false);
    ui->abeInstructionBtnInitialize->setEnabled(false);
    ui->abeInstructionCbSingleInstruction->setVisible(false);
    connect(ui->abeInstructionText, SIGNAL(textChanged()),this,SIGNAL(signalAbeInstructionSomethingChanged()),Qt::UniqueConnection);
}

void AbulEduInstructionV1::abeInstructionSetMultimedia(AbulEduMultiMediaV1 *multimedia)
{
    ABULEDU_LOG_TRACE()<<__FILE__ <<  __LINE__ << __FUNCTION__;
    m_multimedia = multimedia;
    ui->abeInstructionBtnShowControlAudio->setEnabled(true);
    ui->abeInstructionBtnDelete->setEnabled(true);
}

#else
AbulEduInstructionV1::AbulEduInstructionV1(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::AbulEduInstructionV1),
    m_isSingle(false)
{
    ui->setupUi(this);
    ui->abeInstructionFrmButtons->setVisible(false);
    ui->abeInstructionFrmMultiMedia->setVisible(false);
    installEventFilters();
}
#endif

AbulEduInstructionV1::~AbulEduInstructionV1()
{
    delete ui;
}

void AbulEduInstructionV1::abeInstructionSetDefaultInstructionDisabled(bool trueFalse)
{
    ABULEDU_LOG_WARN()  << __FILE__ <<  __LINE__ << __FUNCTION__ << " IS DEPRECATED please use AbulEduInstructionV1::abeInstructionSetDefaultInstructionEnabled()";
    ui->abeInstructionBtnInitialize->setDisabled(trueFalse);
}

void AbulEduInstructionV1::abeInstructionSetDefaultInstructionEnabled(bool trueFalse)
{
    ui->abeInstructionBtnInitialize->setEnabled(trueFalse);
}

bool AbulEduInstructionV1::abeInstructionGetDefaultInstructionDisabled()
{
    ABULEDU_LOG_WARN()  << __FILE__ <<  __LINE__ << __FUNCTION__ << " IS DEPRECATED please use AbulEduInstructionV1::abeInstructionGetDefaultInstructionEnabled()";
    return !ui->abeInstructionBtnInitialize->isEnabled();
}

bool AbulEduInstructionV1::abeInstructionGetDefaultInstructionEnabled()
{
    return ui->abeInstructionBtnInitialize->isEnabled();
}

void AbulEduInstructionV1::abeInstructionSetText(const QString &text)
{
    ABULEDU_LOG_WARN()  << __FILE__ <<  __LINE__ << __FUNCTION__ << " IS DEPRECATED please use AbulEduInstructionV1::abeInstructionSetDefaultInstruction(const QString &instruction)";
    ui->abeInstructionText->setText(text);
    emit signalAbeInstructionSomethingChanged();
}

bool AbulEduInstructionV1::abeInstructionSetDefaultInstruction(const QString &instruction, const QString &instructionPath, const QString& instructionFile, const QString& instructionFileName)
{
    ABULEDU_LOG_TRACE()<<__FILE__ <<  __LINE__ << __FUNCTION__;
    QString correctPath = instructionPath;
    if(!correctPath.endsWith("/")){
        correctPath.append("/");
    }
    m_defaultInstruction = instruction;
    m_defaultInstructionPath = instructionFile;
    ui->abeInstructionText->setText(instruction);
    bool  success = false;
    if(m_multimedia){
        if(QFile(instructionPath).exists()){
            QFile file(instructionFile);
            /* Vérification de l'existance de la consigne par défaut
            if(file.exists())
                ABULEDU_LOG_DEBUG()<<"consigne par défaut OK";
            else
                ABULEDU_LOG_DEBUG()<<"consigne par défaut KO"<<instructionFileName;*/
            QString instructionFilePath;
            if(instructionFileName.isNull()){
                instructionFilePath = correctPath+QString(this->objectName().remove("abeInstruction",Qt::CaseSensitive).toLower()+QString("ConsigneSequence.ogg"));
            }
            else {
                instructionFilePath = correctPath + instructionFileName;
            }
            success =  file.copy(instructionFilePath);
        #ifdef Q_OS_WIN
            QFile(instructionFilePath).setPermissions(QFileDevice::WriteUser);
        #endif
            /* Vérification de la création de la consigne par défaut
            if(success){
                ABULEDU_LOG_DEBUG()<<"consigne par défaut "<<instructionFilePath<<" copiée pour "<<this->objectName();
            }
            else {
                ABULEDU_LOG_DEBUG()<<"Echec de la copie de l'instruction par défaut pour "<<this->objectName();
            }*/
            m_currentInstructionPath = instructionFilePath;
            ui->abeInstructionBtnInitialize->setEnabled(true);
        }
    }
    return success;
}

void AbulEduInstructionV1::abeInstructionSetDefaultInstruction(const QString &instruction, const QString &instructionPath)
{
    ABULEDU_LOG_WARN()  << __FILE__ <<  __LINE__ << __FUNCTION__ << " IS DEPRECATED please use AbulEduInstructionV1::abeInstructionSetDefaultInstruction(const QString &instruction, const QString &instructionPath, const QString& instructionFile)";
    m_defaultInstruction = instruction;
    m_defaultInstructionPath = instructionPath;
    ui->abeInstructionText->setText(instruction);
    if(m_multimedia){
        if(QFile(instructionPath).exists()){
            m_multimedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),m_defaultInstructionPath));
        }
    }
}

void AbulEduInstructionV1::abeInstructionSetDefaultInstruction(const QString &instruction)
{
    ABULEDU_LOG_WARN()  << __FILE__ <<  __LINE__ << __FUNCTION__ << " IS DEPRECATED please use AbulEduInstructionV1::abeInstructionSetDefaultInstruction(const QString &instruction, const QString &instructionPath, const QString& instructionFile)";
    m_defaultInstruction = instruction;
    ui->abeInstructionText->setText(instruction);
}

void AbulEduInstructionV1::abeInstructionSetInstruction(const QString &instruction, const QString &instructionPath)
{
    ui->abeInstructionText->setText(instruction);
    if(m_multimedia){
        m_currentInstructionPath = instructionPath;
        m_multimedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),m_currentInstructionPath));
    }
}

void AbulEduInstructionV1::abeInstructionSetInstructionPath(const QString &instructionPath)
{
    m_currentInstructionPath = instructionPath;
}

QString AbulEduInstructionV1::abeInstructionGetText()
{
    return ui->abeInstructionText->toHtml();
}

QString AbulEduInstructionV1::abeInstructionGetPlainText()
{
    return ui->abeInstructionText->toPlainText();
}

void AbulEduInstructionV1::setChecked(bool checked)
{
    QGroupBox::setChecked(checked);
    emit signalAbeInstructionSomethingChanged();
}

void AbulEduInstructionV1::abeInstructionHideMultiMedia()
{
    m_multimedia->abeMultiMediaGetAudioControlWidget()->setVisible(false);
}

void AbulEduInstructionV1::abeInstructionSetSingle(bool trueFalse)
{
    m_isSingle = trueFalse;
    ui->abeInstructionCbSingleInstruction->setChecked(trueFalse);
}

bool AbulEduInstructionV1::abeInstructionIsSingle()
{
    return m_isSingle;
}

void AbulEduInstructionV1::abeInstructionAllowSingle()
{
    ui->abeInstructionCbSingleInstruction->setVisible(true);
}

void AbulEduInstructionV1::installEventFilters()
{
#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
    ui->abeInstructionBtnShowControlAudio->installEventFilter(this);
    ui->abeInstructionBtnInitialize->installEventFilter(this);
    ui->abeInstructionBtnDelete->installEventFilter(this);
    ui->abeInstructionFrmMultiMedia->installEventFilter(this);
#endif
    ui->abeInstructionText->installEventFilter(this);
}

void AbulEduInstructionV1::on_abeInstructionBtnInitialize_clicked()
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__<<m_defaultInstruction<<m_defaultInstructionPath<<m_currentInstructionPath;
    if(m_multimedia){
        if(QFile(m_defaultInstructionPath).exists()){
#ifndef Q_OS_WIN
            bool success = false;
            QFile file(m_defaultInstructionPath);
            /* Vérification de l'existance de la consigne par défaut
            if(file.exists())
                ABULEDU_LOG_DEBUG()<<"consigne par défaut OK";
            else
                ABULEDU_LOG_DEBUG()<<"consigne par défaut KO"<<instructionFileName;*/
            QFile(m_currentInstructionPath).setPermissions(QFileDevice::WriteUser);
            if(QFile(m_currentInstructionPath).remove()){
                ABULEDU_LOG_DEBUG()<<"Fichier précédent supprimé";
            }
            else{
                ABULEDU_LOG_DEBUG()<<"Impossible de supprimer le fichier précédent";
            }
            success =  file.copy(m_currentInstructionPath);

            /* Vérification de la création de la consigne par défaut*/
            if(success){
                ABULEDU_LOG_DEBUG()<<"consigne par défaut audio rétablie";
            }
            else {
                ABULEDU_LOG_DEBUG()<<"Echec du rétablissement de l'instruction par défaut ";
            }
            ui->abeInstructionBtnInitialize->setEnabled(true);
#else
            if(QFile(m_currentInstructionPath).remove()){
                ABULEDU_LOG_DEBUG()<<"Fichier précédent supprimé";
            }
            else{
                ABULEDU_LOG_DEBUG()<<"Impossible de supprimer le fichier précédent";
            }
            QTimer::singleShot(50,this,SLOT(slotCopydefaultfile()));
            ui->abeInstructionBtnInitialize->setEnabled(true);
#endif
        }
    }
    abeInstructionSetInstruction(m_defaultInstruction,m_currentInstructionPath);
    emit signalAbeInstructionInitializeSound();
    emit signalAbeInstructionSomethingChanged();
}

void AbulEduInstructionV1::on_abeInstructionBtnDelete_clicked()
{
    if(m_multimedia) {
        QFile(m_multimedia->abeMultiMediaGetCurrentMediaMedia().abeMediaMediaGetSound()).remove();
        m_multimedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioSetState(m_multimedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioGetState());
    }
    emit signalAbeInstructionSomethingChanged();
}

void AbulEduInstructionV1::on_abeInstructionBtnShowControlAudio_clicked()
{
    if(m_multimedia) {
        m_multimedia->abeMultimediaSetTTS(AbulEduPicottsV1::None);
        if(m_multimedia->abeMultiMediaGetAudioControlWidget()->parentWidget() == ui->abeInstructionFrmMultiMedia) {
            ui->abeInstructionHlFrmMultimedia->removeWidget(m_multimedia->abeMultiMediaGetAudioControlWidget());
            m_multimedia->abeMultiMediaGetAudioControlWidget()->setParent(0);
            m_multimedia->abeMultiMediaGetAudioControlWidget()->setVisible(false);
            emit signalAbeInstructionShowControlAudio(false);
        }
        else{
            m_multimedia->abeMultiMediaGetAudioControlWidget()->setParent(ui->abeInstructionFrmMultiMedia);
            m_multimedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),m_currentInstructionPath));

            ui->abeInstructionHlFrmMultimedia->addWidget(m_multimedia->abeMultiMediaGetAudioControlWidget());
            m_multimedia->abeMultiMediaGetAudioControlWidget()->setVisible(true);
            ui->abeInstructionFrmMultiMedia->setVisible(true);
            emit signalAbeInstructionShowControlAudio(true);
        }
    }
    repaint();
}

bool AbulEduInstructionV1::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Enter){
        if(obj == ui->abeInstructionFrmMultiMedia){
            if(m_multimedia){
                if(m_multimedia->abeMultiMediaGetAudioControlWidget()->isVisible()){
                    emit signalAbeInstructionObjectHovered(obj->objectName());
                }
            }
        }
        else {
            emit signalAbeInstructionObjectHovered(obj->objectName());
        }
        return true;
    }
    else if(event->type() == QEvent::Leave){
        emit signalAbeInstructionObjectLeaved(obj->objectName());
        return true;
    }
    return QObject::eventFilter(obj,event);
}

void AbulEduInstructionV1::on_AbulEduInstructionV1_toggled(bool trueFalse)
{
    ABULEDU_LOG_TRACE()<<__PRETTY_FUNCTION__<<m_defaultInstruction<<m_defaultInstructionPath<<m_currentInstructionPath;
    ui->abeInstructionText->setVisible(trueFalse);
    if(!trueFalse){
        ui->abeInstructionCbSingleInstruction->setVisible(false);
    }
    else{
        ui->abeInstructionCbSingleInstruction->setVisible(m_isSingle);
    }
#ifdef __ABULEDU_MULTIMEDIA_AVAILABLE__
    ui->abeInstructionFrmButtons->setVisible(trueFalse);
    if(!trueFalse){
        if(m_multimedia){
            m_multimedia->abeMultiMediaGetAudioControlWidget()->setParent(0);
        }
    }
#endif
}

#ifdef Q_OS_WIN
void AbulEduInstructionV1::slotCopydefaultfile()
{
    QFile file(m_defaultInstructionPath);
    bool success =  file.copy(m_currentInstructionPath);
    QFile(m_currentInstructionPath).setPermissions(QFileDevice::WriteUser);
    /* Vérification de la création de la consigne par défaut*/
    if(success){
        ABULEDU_LOG_DEBUG()<<"consigne par défaut audio rétablie";
        if(m_multimedia){
            m_multimedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),m_currentInstructionPath));
        }
    }
    else {
        ABULEDU_LOG_DEBUG()<<"Echec du rétablissement de l'instruction par défaut ";
    }
}
#endif
