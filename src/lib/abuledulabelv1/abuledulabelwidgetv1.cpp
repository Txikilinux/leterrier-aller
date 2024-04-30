/** AbulEduLabelWidgetV1
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2013 Éric Seigne <eric.seigne@ryxeo.com>
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

#include "abuledulabelwidgetv1.h"

#ifndef DEBUG_ABULEDULABELV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduLabelWidgetV1::AbulEduLabelWidgetV1(QWidget *parent) :
    QFrame(parent)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;

    setFrameShape(QFrame::Box);
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11, 11, 11, 11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    horizontalSpacer = new QSpacerItem(68, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer, 0, 0, 1, 1);

    btnGauche = new AbulEduFlatBoutonV1(this);
    btnGauche->setMinimumSize(40,40);
    btnGauche->setIcones(":/abuledulabelv1/buttons/left",":/abuledulabelv1/buttons/leftHover",":/abuledulabelv1/buttons/leftPressed",":/abuledulabelv1/buttons/leftDisabled");
    btnGauche->setObjectName(QString::fromUtf8("btnGauche"));
    gridLayout->addWidget(btnGauche, 0, 1, 1, 1);

    btnDroite = new AbulEduFlatBoutonV1(this);
    btnDroite->setMinimumSize(40,40);
    btnDroite->setIcones(":/abuledulabelv1/buttons/right",":/abuledulabelv1/buttons/rightHover",":/abuledulabelv1/buttons/rightPressed",":/abuledulabelv1/buttons/rightDisabled");
    btnDroite->setObjectName(QString::fromUtf8("btnDroite"));
    gridLayout->addWidget(btnDroite, 0, 2, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(68, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_2, 0, 3, 1, 1);

    verticalSpacer = new QSpacerItem(20, 78, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 1, 4, 1, 1);

    labelsScrollArea = new QScrollArea(this);
    labelsScrollArea->setObjectName(QString::fromUtf8("LabelsScrollArea"));
    labelsScrollArea->setWidgetResizable(true);
    labelsScrollArea->setAlignment(Qt::AlignCenter);
    labelsScrollArea->setFrameShape(QFrame::NoFrame);
    scrollAreaWidgetContents = new QWidget(this);
    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 328, 193));
    labelsScrollArea->setWidget(scrollAreaWidgetContents);

    gridLayout->addWidget(labelsScrollArea, 1, 0, 4, 4);

    btnHaut = new AbulEduFlatBoutonV1(this);
    btnHaut->setMinimumSize(40,40);
    btnHaut->setIcones(":/abuledulabelv1/buttons/up",":/abuledulabelv1/buttons/upHover",":/abuledulabelv1/buttons/upPressed",":/abuledulabelv1/buttons/upDisabled");
    btnHaut->setObjectName(QString::fromUtf8("btnHaut"));

    gridLayout->addWidget(btnHaut, 2, 4, 1, 1);

    btnBas = new AbulEduFlatBoutonV1(this);
    btnBas->setMinimumSize(40,40);
    btnBas->setIcones(":/abuledulabelv1/buttons/down",":/abuledulabelv1/buttons/downHover",":/abuledulabelv1/buttons/downPressed",":/abuledulabelv1/buttons/downDisabled");
    btnBas->setObjectName(QString::fromUtf8("btnBas"));

    gridLayout->addWidget(btnBas, 3, 4, 1, 1);

    verticalSpacer_2 = new QSpacerItem(20, 71, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer_2, 4, 4, 1, 1);

    QMetaObject::connectSlotsByName(this);

#if QT_VERSION > 0x050000
    QScroller::grabGesture(labelsScrollArea, QScroller::TouchGesture);
#endif
}

AbulEduLabelWidgetV1::~AbulEduLabelWidgetV1()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
}

void AbulEduLabelWidgetV1::on_btnHaut_clicked()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    emit moveSelectedLabelUp();
}
void AbulEduLabelWidgetV1::on_btnBas_clicked()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    emit moveSelectedLabelDown();
}

void AbulEduLabelWidgetV1::on_btnGauche_clicked()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    emit moveSelectedLabelLeft();
}

void AbulEduLabelWidgetV1::on_btnDroite_clicked()
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    emit moveSelectedLabelRight();
}

void AbulEduLabelWidgetV1::btnDroiteSetVisible(bool on)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    btnDroite->setVisible(on);
}

void AbulEduLabelWidgetV1::btnGaucheSetVisible(bool on)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    btnGauche->setVisible(on);
}

void AbulEduLabelWidgetV1::btnBasSetVisible(bool on)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    btnBas->setVisible(on);
}

void AbulEduLabelWidgetV1::btnHautSetVisible(bool on)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    btnHaut->setVisible(on);
}

void AbulEduLabelWidgetV1::btnDroiteSetEnabled(bool on)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    btnDroite->setEnabled(on);
}

void AbulEduLabelWidgetV1::btnGaucheSetEnabled(bool on)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    btnGauche->setEnabled(on);
}

void AbulEduLabelWidgetV1::btnBasSetEnabled(bool on)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    btnBas->setEnabled(on);
}

void AbulEduLabelWidgetV1::btnHautSetEnabled(bool on)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    btnHaut->setEnabled(on);
}

void AbulEduLabelWidgetV1::mousePressEvent(QMouseEvent *)
{
    ABULEDU_LOG_TRACE() <<  __PRETTY_FUNCTION__;
    AbulEduLabelFrameV1 *aire = findChild<AbulEduLabelFrameV1 *>();
    aire->abelabelDeselectionneLabels(); // Si on clique en dehors des abuledulabels, on désélectionne tout
}
