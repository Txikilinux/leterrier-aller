#include "abuledutexteditv1.h"
#include "abuleduconstantesv1.h"

AbulEduTextEditV1::AbulEduTextEditV1(QString text, int rank, QWidget* parent) :
    QTextEdit(text,parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_serialNumber = rank;
    m_isIgnoreStyleSheet = false;
}

AbulEduTextEditV1::AbulEduTextEditV1(QWidget *parent):
       QTextEdit(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_serialNumber = 0;
    m_isIgnoreStyleSheet = false;
}

void AbulEduTextEditV1::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(!m_isIgnoreStyleSheet){
        if (styleSheet()== m_initialStyleSheet)
        {
            setStyleSheet(abe::STYLESELECTION);
        }
        else
        {
            setStyleSheet(m_initialStyleSheet);
        }
    }
    emit AbulEduTextClicked(m_serialNumber);
}

int AbulEduTextEditV1::abeGetMSerialNumber()
{
    return m_serialNumber;
}

void AbulEduTextEditV1::abeSetMSerialNumber(int number)
{
    m_serialNumber = number;
}

QString AbulEduTextEditV1::abeGetMInitialStyleSheet()
{
    return m_initialStyleSheet;
}

void AbulEduTextEditV1::abeSetMInitialStyleSheet(QString styleSheet)
{
    setStyleSheet(styleSheet);
    m_initialStyleSheet = styleSheet;
}

void AbulEduTextEditV1::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    abeApp->setOverrideCursor(Qt::PointingHandCursor);
}

void AbulEduTextEditV1::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    abeApp->restoreOverrideCursor();
}
