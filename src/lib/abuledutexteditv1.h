#ifndef ABULEDUTEXTEDITV1_H
#define ABULEDUTEXTEDITV1_H

#include "abuleduapplicationv1.h"
#include <QTextEdit>
#include <QCursor>

class AbulEduTextEditV1 : public QTextEdit
{
    Q_OBJECT
public:
    explicit AbulEduTextEditV1(QString text, int rank, QWidget * parent = 0);
    explicit AbulEduTextEditV1(QWidget * parent = 0);
    int abeGetMSerialNumber();
    void abeSetMSerialNumber(int number);
    QString abeGetMInitialStyleSheet();
    void abeSetMInitialStyleSheet(QString styleSheet);
    inline void abeSetIgnoreStyleSheet(){m_isIgnoreStyleSheet = true;}

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
private:
    int m_serialNumber;
    QString m_initialStyleSheet;
    bool m_isIgnoreStyleSheet;

signals:
    void AbulEduTextClicked(int);

public slots:

};

#endif // ABULEDUTEXTEDITV1_H
