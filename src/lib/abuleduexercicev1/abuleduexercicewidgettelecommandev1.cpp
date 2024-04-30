/** Classe AbulEduExerciceWidgetTelecommandeV1
  * @author 2011-2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2012 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
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

#include "abuleduexercicewidgettelecommandev1.h"

#ifndef DEBUG_ABULEDUEXERCICEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduExerciceWidgetTelecommandeV1::AbulEduExerciceWidgetTelecommandeV1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbulEduExerciceWidgetTelecommandeV1)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    m_niveau = 0;
    m_teteEval = "attente";
    ui->setupUi(this);
    if(parent->inherits("AbulEduPageAccueilV1")) {
        connect(qobject_cast<AbulEduPageAccueilV1 *>(parent), SIGNAL(dimensionsChangees()),
                this, SLOT(setDimensionsWidget()),Qt::UniqueConnection);
    }
    raise();
    setStyleSheet("background-color: rgba(0,0,0,0);");
    setImageFond(":/abuleduexercicev1/fonds/telecommande");

    foreach(AbulEduFlatBoutonV1 *var, findChildren<AbulEduFlatBoutonV1 *>()) {
        const QString composant = var->objectName();
        var->setIconeNormale(QString(":/abuleduexercicev1/boutons/%1").arg(composant));
        #ifdef __ABULEDUTABLETTEV1__MODE__
            var->setIconePressed(QString(":/abuleduexercicev1/boutons/%1Over").arg(composant));
        #else
        var->setIconeSurvol(QString(":/abuleduexercicev1/boutons/%1Over").arg(composant));
        #endif
        var->setIconeDisabled(QString(":/abuleduexercicev1/boutons/%1Disabled").arg(composant));
        var->setTexteAlignement(Qt::AlignLeft);
    }

#ifdef __ABULEDUTABLETTEV1__MODE__
    /// 15/01/2012 Icham -> mode tablette, pas de tooltips (pas de survol en mode tactile, et puis ça faisait des trucs bizarres parfois)
    /// 15/01/2013 iCHAM -> icones survol = icones normales
    // on cherche tous les enfants, et on leur met une chaine vide en tooltips (= desactivation)
    foreach (QWidget *obj, findChildren<QWidget*>()) {
        obj->setToolTip("");
//        if(dynamic_cast<AbulEduFlatBoutonV1*>(obj)){
//            dynamic_cast<AbulEduFlatBoutonV1*>(obj)->setIconeSurvol(dynamic_cast<AbulEduFlatBoutonV1*>(obj)->getIconeNormale());
//        }
    }
#endif
    setDimensionsWidget();
}

AbulEduExerciceWidgetTelecommandeV1::~AbulEduExerciceWidgetTelecommandeV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    //On recupere les deux objets qu'on a reparenté ...
    ui->framePopupQuitter->setParent(this);
    delete ui;
}

void AbulEduExerciceWidgetTelecommandeV1::abeTelecommandeClose()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    // a faire ou pas ?
    //setAttribute(Qt::WA_DeleteOnClose);
    close();
}

void AbulEduExerciceWidgetTelecommandeV1::changeEvent(QEvent *e)
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

void AbulEduExerciceWidgetTelecommandeV1::setAbeLevel(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    float ratio = abeApp->getAbeApplicationDecorRatio();
    m_niveau = s.toInt();
    ui->lblNiveauEnCours->setPixmap(QPixmap(":/abuleduexercicev1/niveaux/"+QString::number(m_niveau+1)).scaledToWidth(498*ratio, Qt::SmoothTransformation));
    ui->lblNiveauEnCours->update();
}

void AbulEduExerciceWidgetTelecommandeV1::setMNiveau(int niveau)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_niveau = niveau;
    setDimensionsWidget();
}

void AbulEduExerciceWidgetTelecommandeV1::setMTeteEval(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_teteEval = s;
    setDimensionsWidget();
}

void AbulEduExerciceWidgetTelecommandeV1::setDimensionsWidget()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    // Redimensionnement de l'image de la télécommande
    float ratio = abeApp->getAbeApplicationDecorRatio();
    float hratio = ratio;
    QPixmap fond = m_imageFond;
    fond = fond.scaled(fond.size()*ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setFixedSize(fond.size());

    // NSE - ajout pour positionnement correct
    //On deplace a 1550 dans le cas ou l'image de fond est en 2048
    move(1550*ratio,0);

    // Dimensionne et place l'image de fond
    ui->lbImageFond->setPixmap(fond);
    ui->lbImageFond->setFixedSize(fond.size());

    //Lecture du fichier conf pour placer les boutons et les textes
    QSettings config(":/abuleduexercicev1/conf/abuleduexercicev1.conf",QSettings::IniFormat);
    if(config.value("resizeHeightFactor",0).toReal() > 0){
        hratio = ratio*config.value("resizeHeightFactor").toReal();
    }
    QString normalColor = config.value("CouleurTexteNormale").toString();
    QString hoverColor = config.value("CouleurTexteSurvol").toString();
    QString pressedColor = config.value("CouleurTextePressed").toString();
    int decalageGeneral = config.value("generalGap",0).toInt();
    ui->lblImageTop->move(decalageGeneral,0);
    ui->lblImageTop->setPixmap(QPixmap(":/abuleduexercicev1/boutons/top").scaledToWidth(498*ratio, Qt::SmoothTransformation));
    ui->lblImageTop->resize(ui->lblImageTop->pixmap()->size());

    ui->lblNiveauEnCours->setPixmap(QPixmap(":/abuleduexercicev1/niveaux/"+QString::number(m_niveau+1)).scaledToWidth(498*ratio, Qt::SmoothTransformation));
    ui->lblNiveauEnCours->resize(ui->lblNiveauEnCours->pixmap()->size());
    ABULEDU_LOG_DEBUG() << " on retaille l'image clef à "<<ui->lblNiveauEnCours->pixmap()->size();

    //Debut du calcul du positionnement vertical
    int y = ui->lblImageTop->height();
    //================================================================================

    //Le style par defaut, attention aux %1 etc. qui sont remplaces par le .arg de fin de ligne
//    QString leStyle("border-image: url(:/abuleduexercicev1/boutons/%1);image-position: left; background-repeat: no-repeat; padding-left: %2px; text-align:left;");

    //Les composants peuvent etre de 4 types: AbulEduFlatBoutonV1, QLabel, QProgressbar ou QFrame

    int size = config.beginReadArray("telecommande");
    for(int i = 0; i < size ; i++) {
        config.setArrayIndex(i);
        QString composant(config.value("name").toString());
        //Si c'est un bouton
        if(QPointer<AbulEduFlatBoutonV1> btn = findChild<AbulEduFlatBoutonV1 *>(composant))
        {
            int decalage = 0;
            if(config.value("gap").toInt() > 0){
                decalage = config.value("gap").toInt();
            }
            /* C'est pas du super code... il vaudrait mieux (cf menu Feuille ou menu graphique) mettre dans le conf des noms de propriétés et les affecter toutes par un setProperty
             * qui bouclerait sur les childKeys. Mais ça nécessite de reprendre tous les data/abuleduexercicev1/abuleduexercicev1.conf de tous les logiciels du Terrier */
            if(!normalColor.isEmpty()){
                btn->setCouleurTexteNormale(normalColor);
            }
            if(!hoverColor.isEmpty()){
                btn->setCouleurTexteSurvol(hoverColor);
                if(!pressedColor.isEmpty()){
                    btn->setCouleurTextePressed(pressedColor);
                }
                else {
                    btn->setCouleurTextePressed(hoverColor);
                }
            }
            //On place le texte a un certain point a gauche pour ne pas recouvrir le dessin (padding left)
            btn->setTextePaddingLeft(config.value("padding").toInt()*ratio);
            btn->resize(QPixmap(QString(":/abuleduexercicev1/boutons/%1").arg(composant)).size()*hratio);
            btn->move((decalageGeneral+decalage)*ratio,y);
            y += btn->height();
            ABULEDU_LOG_DEBUG() <<" " << composant<<" de taille "<<btn.data()->size()<<" en "<<btn.data()->pos();
        }
        else if (QPointer<QLabel> l = findChild<QLabel *>(composant))
        {
            /* Deux solutions pour le fond des labels
             * - une image est indiquée dans le .conf
             * - un motif (contenu dans une image png, donc potentiellement transparente ou en partie) est contenu dans un fichier d'alias lblCustom */
            if(composant.contains("custom",Qt::CaseInsensitive)){
                /* On a pour les labels un décalage vers la droite de 100 (ancienne méthode) mais qu'on peut modifier en fixant dans le .conf un "gap" */
                int decalage = 100;
                if(config.value("gap").toInt() != 0){
                    decalage = config.value("gap").toInt();
                }
                l->move((decalageGeneral+decalage)*ratio,y);
                /* Je garde le calcul ancien de largeur des QLabels à 60% de la largeur de la télécommande, mais je peux la modifier dans le .conf */
                if(config.value("width").toInt() > 0){
                    l->resize(config.value("width").toInt()*ratio,config.value("height").toInt()*hratio);
                }
                else {
                    l->resize(this->width()*0.6,config.value("height").toInt()*hratio);
                }
                if(config.value("image",QString()).toString().isEmpty()) {
                    l->setStyleSheet("background-image:url("+QString(":/abuleduexercicev1/fonds/lblCustom")+");background-repeat: repeat-x;background-position: top right;");
                }
                else {
                    int w = l->width();
                    QPixmap pix(QString(":/abuleduexercicev1/fonds/imageLblCustom%1").arg(composant.right(1)));
                    QPixmap scPix = pix.scaledToWidth(w,Qt::SmoothTransformation);
                    l->setPixmap(scPix);
                }
                QFont f = abeApp->font();
                if(config.value("pointSize",0).toInt() > 0){
                    f.setPointSize(config.value("pointSize",0).toInt()*ratio);
                }
                l->setAlignment(Qt::AlignCenter);
                l->setFont(f);
                if(!config.value("text",QString()).toString().isEmpty()){

                    l->setText(config.value("text").toString());
                }
                if(!config.value("textColor",QString()).toString().isEmpty()){
                    QPalette palette = l->palette();
                    palette.setColor(QPalette::WindowText,QColor(QString("%1").arg(config.value("textColor").toString())));
                    l->setPalette(palette);
                }
            }
            //On a que 3 labels : top / niveau et tete, on s'est deja charge de leur mettre la bonne taille au debut
            else {
                l->move(decalageGeneral,y);
            }
            y += l->height();
            ABULEDU_LOG_DEBUG() << composant<<" de taille "<<l.data()->size()<<" en "<<l.data()->pos();
        }
        else if (QPointer<QProgressBar> p = findChild<QProgressBar *>(composant))
        {
            p->move(config.value("padding").toInt()*ratio,y);
            p->resize(config.value("width").toInt()*ratio,config.value("height").toInt()*hratio);
            QString lestyle = config.value("style").toString();
            if(lestyle.contains("FONTSIZE")) {
                lestyle.replace("FONTSIZE", QString::number(round(config.value("height").toInt()*hratio/1.8)));
            }
            else {
                if(!lestyle.contains("font-size")) {
                    lestyle.append(QString(" font-size: %1px;").arg(QString::number(round(config.value("height").toInt()*hratio/1.8))));
                }
            }
            if(!lestyle.contains("text-align")) {
                lestyle.append(QString("text-align: center;"));
            }
            p->setStyleSheet(lestyle);
            y += p->height()+round(5*hratio);
            ABULEDU_LOG_DEBUG() << composant<<" de taille "<<p.data()->size()<<" en "<<p.data()->pos();

        }
        else if (QPointer<QLineEdit> le = findChild<QLineEdit *>(composant))
        {
            le->move(config.value("padding").toInt()*ratio,y);
            le->resize(config.value("width").toInt()*ratio,config.value("height").toInt()*hratio);
            QString lestyle = config.value("style").toString();
            if(lestyle.contains("FONTSIZE")) {
                lestyle.replace("FONTSIZE", QString::number(round(config.value("height").toInt()*ratio/1.8)));
            }
            else {
                if(!lestyle.contains("font-size")) {
                    lestyle.append(QString(" font-size: %1px;").arg(QString::number(round(config.value("height").toInt()*ratio/1.8))));
                }
            }
            if(!lestyle.contains("text-align")) {
                lestyle.append(QString("text-align: center;"));
            }
            le->setStyleSheet(lestyle);
            y += le->height()+round(5*ratio);
            ABULEDU_LOG_DEBUG() << composant<<" de taille "<<le.data()->size()<<" en "<<le.data()->pos();
        }
    }
    config.endArray();
    config.beginGroup("popupQuitter");
        int decalagePopUp = config.value("gap").toInt();
        config.beginGroup("btnQuitterRetourMenuPrincipal");
        foreach(const QString &prop,config.childKeys()){
            ui->btnQuitterRetourMenuPrincipal->setProperty(prop.toLatin1(), config.value(prop));
        }
        config.endGroup();
        config.beginGroup("btnQuitterAnnuler");
        foreach(QString prop,config.childKeys()){
            ui->btnQuitterAnnuler->setProperty(prop.toLatin1(), config.value(prop));
        }
        config.endGroup();
    config.endGroup();

    // Place les popup Quitter et Niveau
    //erics 20120103 : je reparente la framepopup pour qu'elle puisse déborder de la frameTelecommande sinon elle
    //s'affiche que partiellement ...

    //important de faire le raise de popupQuitter apres celui de niveau: il faut que quitter soit au dessus de niveau
    //dans le cas ou on demanderait les deux popup
    ui->framePopupQuitter->setParent(parentWidget());
    ui->framePopupQuitter->move(x()+decalagePopUp-ui->framePopupQuitter->width(),
                                ui->btnQuitter->pos().y() - ui->framePopupQuitter->height() + ui->btnQuitter->height()-25*ratio);
    ui->framePopupQuitter->raise();
}

void AbulEduExerciceWidgetTelecommandeV1::setImageFond(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_imageFond.load(s);
    setDimensionsWidget();
}

void AbulEduExerciceWidgetTelecommandeV1::abeTelecommandeResize()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    setDimensionsWidget();
}

void AbulEduExerciceWidgetTelecommandeV1::keyPressEvent(QKeyEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QWidget::keyPressEvent(event);
}

void AbulEduExerciceWidgetTelecommandeV1::keyReleaseEvent(QKeyEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QWidget::keyReleaseEvent(event);
}

void AbulEduExerciceWidgetTelecommandeV1::on_btnQuitter_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* desactivat° Aire Travail */
    foreach(QWidget *var, parent()->findChildren<QWidget *>("AbulEduExerciceWidgetAireDeTravailV1")){
        var->setEnabled(false);
    }
    /* desactivat°  telecommande */
    parent()->findChildren<QWidget *>("AbulEduExerciceWidgetAireDeTravailV1").last()->setEnabled(false);
    this->setEnabled(false);
    ui->framePopupQuitter->setVisible(true);
    emit btnTelecommandeQuitterClicked();
}

void AbulEduExerciceWidgetTelecommandeV1::on_btnQuitterAnnuler_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* activat° Aire Travail */
    foreach(QWidget *var, parentWidget()->findChildren<QWidget *>("AbulEduExerciceWidgetAireDeTravailV1")){
        var->setEnabled(true);
    }
    /* activat°  telecommande */
    this->setEnabled(true);
    ui->framePopupQuitter->setVisible(false);
    emit btnTelecommandeQuitterAnnulerClicked();
}

void AbulEduExerciceWidgetTelecommandeV1::on_btnQuitterRetourMenuPrincipal_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    ui->framePopupQuitter->setVisible(false);
    emit btnTelecommandeQuitterFermerClicked();
}

void AbulEduExerciceWidgetTelecommandeV1::on_btnAide_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    emit btnTelecommandeAideClicked();
}
