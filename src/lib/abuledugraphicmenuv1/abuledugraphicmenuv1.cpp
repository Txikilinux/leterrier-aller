/** Menu graphique affiché en haut des logiciels
  * AbulEduGraphicMenuV1
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduGraphicMenuV1
  * @see https://redmine.ryxeo.com/projects/ryxeo/wiki/Le_menu_graphique
  *
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2013 Éric Seigne <eric.seigne@ryxeo.com>
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

#include "abuledugraphicmenuv1.h"

#ifndef DEBUG_ABULEDUGRAPHICMENUV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduGraphicMenuV1::AbulEduGraphicMenuV1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbulEduGraphicMenuV1),
    m_sizeWidget(),
    m_animationStep(0),
    m_imageFond(),
    m_mapComposants(),
    m_config(new QSettings(":/abuledugraphicmenuv1/abuledugraphicmenuv1.conf",QSettings::IniFormat))
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->setupUi(this);

    m_imageFond.load(":/abuledugraphicmenuv1/fond-01");
    resize(m_imageFond.size());
    m_sizeWidget = this->geometry().size();
    QList<QWidget *> widgets = findChildren<QWidget *>();

    //================================================================================
    /* Lecture du fichier conf pour attribuer les btn. Si la liste retourne plus de 3 groupes, c'est qu'on est dans le cas d'une gestion "nouvelle", et donc on prend la place de l'ancienne liste */
    const QStringList groups = m_config->childGroups();
    if(groups.size() > 3) {
        QStringListIterator strIt(groups);
        while(strIt.hasNext()) {
            QString cur = strIt.next();
            m_config->beginGroup(cur);
            ABULEDU_LOG_DEBUG()<<cur<<m_config->value("rank",-1);
            if(m_config->value("rank",-1).toInt() >= 0){
                m_mapComposants.insert(m_config->value("rank").toInt(),cur);
                ABULEDU_LOG_DEBUG()<<" --> "<<m_mapComposants.values();
            }
            m_config->endGroup();
        }
    }

    /* Ecriture des propriétés communes */
    foreach(QWidget* w,widgets) {
        foreach(const QString commonPropertie,m_config->childKeys()) {
            w->setProperty(commonPropertie.toLatin1(), m_config->value(commonPropertie));
        }
    }

    /* Ecriture des propriétés particulières */
    foreach(const QString gr, groups) {
        ABULEDU_LOG_DEBUG() << "on cherche" << gr;
        m_config->beginGroup(gr);
        QStringList grProperties = m_config->childKeys();
        /* Sous android c'est un menu intégré à base de QActions */
#ifdef Q_OS_ANDROID
        QList<QAction *> actions = findChildren<QAction *>();
            for(int i = 0; i < actions.count(); i++) {
                if(actions.at(i)->objectName() == ("action" + gr)) {
                    QAction *btn = actions.takeAt(i);
                    ABULEDU_LOG_DEBUG() << " on a trouvé " << btn->objectName();

                    foreach(QString propertie, grProperties) {
                        ABULEDU_LOG_DEBUG() << " propriété -> " << propertie.toLatin1()<<" contient "<<m_config->value(propertie);
                        /* Dans le cas de la propriété text seulement, on gère l'Utf8 */
                        if(propertie.toLatin1() == "text")
                        {
                            QByteArray str = m_config->value(propertie).toByteArray();
                            btn->setProperty("text", QString::fromUtf8(str));
                        }
                        else
                        {
                            btn->setProperty(propertie.toLatin1(), m_config->value(propertie));
                        }
                    }
                }
            }
#endif
        /* On cherche le widget qui porte ce nom dans notre liste de pointeurs de qwidgets */
        for(int i = 0; i < widgets.count(); i++) {
            if(widgets.at(i)->objectName() == gr) {
                QWidget *btn = widgets.takeAt(i);
                ABULEDU_LOG_DEBUG() << " on a trouvé " << btn->objectName();

                foreach(const QString propertie, grProperties) {
                    ABULEDU_LOG_DEBUG() << " propriété -> " << propertie.toLatin1()<<" contient "<<m_config->value(propertie);
                    /* Dans le cas de la propriété text seulement, on gère l'Utf8 */
                    if(propertie.toLatin1() == "text")
                    {
                        QByteArray str = m_config->value(propertie).toByteArray();
                        btn->setProperty("text", QString::fromUtf8(str));
                    }
                    else
                    {
                        btn->setProperty(propertie.toLatin1(), m_config->value(propertie));
                    }
                }
            }
        }
        m_config->endGroup();
    }

    foreach(const QString elt,m_mapComposants){
        afficherCacher(elt,false);
    }

#ifdef __ABULEDUTABLETTEV1__MODE__
    /* 13/12/2012 Icham -> mode tablette, pas de tooltips (pas de survol en mode tactile, et puis ça faisait des trucs bizarres parfois)
     * 02/01/2013 iCHAM -> icones survol = icones normales
     * on cherche tous les enfants, et on leur met une chaine vide en tooltips (= desactivation) */
    foreach (QWidget *obj, findChildren<QWidget*>()) {
        obj->setToolTip("");
        if(qobject_cast<AbulEduFlatBoutonV1*>(obj)){
            qobject_cast<AbulEduFlatBoutonV1*>(obj)->setIconeSurvol(qobject_cast<AbulEduFlatBoutonV1*>(obj)->getIconeNormale());
        }
    }
#endif

    deplierMenu();
}

AbulEduGraphicMenuV1::~AbulEduGraphicMenuV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    delete ui;
}

void AbulEduGraphicMenuV1::changeEvent(QEvent *e)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AbulEduGraphicMenuV1::setDimensionsWidget()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    move(this->parentWidget()->width() - this->width(), 0);
    float ratio = abeApp->getAbeApplicationDecorRatio();
    QPixmap imagefondScaled = m_imageFond.scaled(m_imageFond.size()*ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setFixedSize(imagefondScaled.size());
    ui->abeGraphicMenuLblBackground->setPixmap(imagefondScaled);
    ui->abeGraphicMenuLblBackground->setFixedSize(imagefondScaled.size());
    setFixedSize(m_sizeWidget * ratio);

    /* Debut du calcul du positionnement horizontal */
    int x = ui->abeGraphicMenuLblBackground->pos().x();
    QFont fonte = font();
    int specialFontSize = m_config->value("baseFont",0).toInt()*ratio;
    if(specialFontSize > 0){
        fonte.setPixelSize(specialFontSize);
    }
    else {
        fonte.setPixelSize(5+13*ratio);
    }
    setFont(fonte);
    QMapIterator<int,QString> iter(m_mapComposants);
    while(iter.hasNext()){
        QString now = iter.next().value();
        QString button = QString();
        QSettings config(":/abuledugraphicmenuv1/abuledugraphicmenuv1.conf",QSettings::IniFormat);
        config.beginGroup(now);
        if(!config.value("buttonName",QString()).toString().isEmpty()){
            button = config.value("buttonName").toString();
        }
        if(QWidget* currentWidget = findChild<QWidget*>(now)){
            currentWidget->move(x,0);
            currentWidget->resize(QPixmap(QString(":/abuledugraphicmenuv1/%1").arg(button)).size()*ratio);
            currentWidget->setFont(fonte);
            x+=currentWidget->width();
        }
    }

    /* cas particulier du bouton OuvrirMenu dont l'emplacement (position) n'est pas automatiquement calculée */
    ui->abeGraphicMenuBtnOpenMenu->move(ui->abeGraphicMenuBtnQuit->pos());
    ui->abeGraphicMenuBtnOpenMenu->resize(QPixmap(QString(":/abuledugraphicmenuv1/btnQuitter")).size()*ratio);
}

/* @note utilisation de la liste pour qu'on puisse specifier un ordre
* d'icones différent dans le fichier conf ... par contre la numérotation est inversée
* @see https://redmine.ryxeo.com/projects/ryxeo/wiki/Le_menu_graphique
*/
void AbulEduGraphicMenuV1::plierDeplierMenu(bool deplier)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    switch(m_animationStep) {
    case 11:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-04");
        afficherCacher(m_mapComposants.value(1),deplier);
        break;
    case 10:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-04");
        afficherCacher(m_mapComposants.value(2),deplier);
        break;
    case 9:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-04");
        afficherCacher(m_mapComposants.value(3),deplier);
        break;
    case 8:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-04");
        afficherCacher(m_mapComposants.value(4),deplier);
        break;
    case 7:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-04");
        if(m_mapComposants.size()>5)
            afficherCacher(m_mapComposants.value(5),deplier);
        break;
    case 6:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-04");
        if(m_mapComposants.size()>6)
            afficherCacher(m_mapComposants.value(6),deplier);
        break;
    case 5:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-04");
        if(m_mapComposants.size()>7)
            afficherCacher(m_mapComposants.value(7),deplier);
        break;
    case 4:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-03");
        break;
    case 3:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-02");
        break;
    case 2:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-01");
        break;
    case 1:
        m_imageFond.load(":/abuledugraphicmenuv1/fond-01");
        ui->abeGraphicMenuBtnOpenMenu->hide();
        break;
    case 0:
        ui->abeGraphicMenuBtnOpenMenu->show();
        m_imageFond.load(":/abuledugraphicmenuv1/fond-00");
        emit btnFermerTriggered();
        break;
    default:
        break;
    }
    /* Si on n'a pas terminé de plier le menu on tourne un tour de plus */
    if(!deplier && m_animationStep > 0) {
        QTimer::singleShot(150,this,SLOT(plierMenu()));
        m_animationStep--;
    }
    /* Si on n'a pas terminé de déplier le menu on tourne un tour de plus */
    if(deplier && m_animationStep < 11) {
        QTimer::singleShot(150,this,SLOT(deplierMenu()));
        m_animationStep++;
    }
    /* Si on doit bouger encore */
    if(m_animationStep > 0 || m_animationStep < 11) {
        setDimensionsWidget();
    }
    updateGeometry();
}

void AbulEduGraphicMenuV1::plierMenu()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    plierDeplierMenu(false);
}

void AbulEduGraphicMenuV1::deplierMenu()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    plierDeplierMenu(true);
}

/* Permet d'afficher ou de cacher un objet en utilisant son nom */
void AbulEduGraphicMenuV1::afficherCacher(const QString& composant, bool deplier)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << composant << deplier;

    if(QPointer<AbulEduFlatBoutonV1> btn = findChild<AbulEduFlatBoutonV1 *>(composant)) {
        btn.data()->setVisible(deplier);
    }
    else if (QPointer<QLabel> l = findChild<QLabel *>(composant)) {
        l.data()->setVisible(deplier);
    }
}

void AbulEduGraphicMenuV1::on_abeGraphicMenuBtnBox_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    emit btnBoxTriggered();
}

void AbulEduGraphicMenuV1::on_abeGraphicMenuBtnQuit_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    emit btnQuitterTriggered();
}

void AbulEduGraphicMenuV1::on_abeGraphicMenuBtnClose_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    /* Pseudo animation pour la fermeture du menu */
    ui->abeGraphicMenuBtnClose->hide();
    plierMenu();
}

void AbulEduGraphicMenuV1::on_abeGraphicMenuBtnHelp_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    emit btnAideTriggered();
}

void AbulEduGraphicMenuV1::on_abeGraphicMenuBtnReport_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    emit btnLivretTriggered();
}

void AbulEduGraphicMenuV1::on_abeGraphicMenuBtnSettings_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    emit btnParametresTriggered();
}

void AbulEduGraphicMenuV1::on_abeGraphicMenuBtnOpenMenu_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    plierDeplierMenu(true);
}
