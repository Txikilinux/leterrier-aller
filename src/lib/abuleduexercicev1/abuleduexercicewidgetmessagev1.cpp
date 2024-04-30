/**
  * Classe
  * @author 2011 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2012 Icham Sirat <icham.sirat@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
  *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distrAireDeTravailibuted in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "abuleduexercicewidgetmessagev1.h"

#ifndef DEBUG_ABULEDUEXERCICEV1
    #include "abuledudisableloggerv1.h"
#endif

AbulEduExerciceWidgetMessageV1::AbulEduExerciceWidgetMessageV1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbulEduExerciceWidgetMessageV1),
    m_fond(new QGraphicsPixmapItem()),
    m_imageFond(QPixmap()),
    frPrincipale(new QFrame()),
    m_vLayout(new QVBoxLayout(frPrincipale)),
    lbTitre(new QLabel(frPrincipale)),
    lbSeparationTitre(new QLabel(frPrincipale)),
    teConsigne(new QTextEdit(frPrincipale)),
    m_frmMultimedia(new QFrame()),
    lbSeparationConsigne(new QLabel(frPrincipale)),
    m_frmCustom(new QFrame()),
    m_frmCustomLayout(new QBoxLayout(QBoxLayout::LeftToRight,m_frmCustom)),
    teTextExercice(new QTextEdit(frPrincipale)),
    spacer(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding)),
    m_multimedia(new AbulEduMultiMediaV1(AbulEduMultiMediaV1::Sound)),
    m_previousRatio(abeApp->getAbeApplicationDecorRatio()),
    m_isDisplayError(true),
    m_hasImageFond(true),
    m_parent(parent),
    m_defaultStyleSheet(QString())
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->setupUi(this);
    gsPrincipale = new QGraphicsScene(ui->gvPrincipale);

    if(parent->inherits("AbulEduPageAccueilV1")) {
        connect(qobject_cast<AbulEduPageAccueilV1 *>(parent), SIGNAL(dimensionsChangees()),
                this, SLOT(setDimensionsWidget()),Qt::UniqueConnection);
    }
    setParent(parent);
    setStyleSheet("background-color: transparent;");
    setObjectName("message");
    ui->gvPrincipale->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->gvPrincipale->setFrameShape(QFrame::NoFrame);
    ui->gvPrincipale->setScene(gsPrincipale);
    ui->gvPrincipale->setSceneRect(0,0,ui->gvPrincipale->width(),ui->gvPrincipale->height());
    ui->gvPrincipale->setStyleSheet("background-color: transparent;");

    gsPrincipale->setObjectName("gsPrincipale");
    gsPrincipale->addItem(m_fond);

    m_fond->setZValue(-10);

    frPrincipale->setStyleSheet("background-color: transparent;");
    frPrincipale->setObjectName(QString::fromUtf8("frPrincipale"));
    frPrincipale->setFrameShape(QFrame::NoFrame);

    m_vLayout->setSpacing(0);
    m_vLayout->setObjectName(QString::fromUtf8("m_vLayout"));
    m_vLayout->setContentsMargins(0, 0, 0, 0);

    lbTitre->setObjectName(QString::fromUtf8("lbTitre"));
    lbTitre->setAlignment(Qt::AlignCenter);
    lbTitre->setFont(QFont(abeApp->font().family(),18,75));
    lbTitre->setText(trUtf8("Mon Joli Titre"));
    lbTitre->setTextInteractionFlags(Qt::NoTextInteraction);
    m_vLayout->addWidget(lbTitre);

    lbSeparationTitre->setObjectName(QString::fromUtf8("lbSeparationTitre"));
    QPixmap trait;
    trait.load(":/abuleduexercicev1/fonds/traitseparation");
    lbSeparationTitre->setPixmap(trait);
    m_vLayout->addWidget(lbSeparationTitre);

    teConsigne->setObjectName(QString::fromUtf8("teConsigne"));
    teConsigne->setFrameShape(QFrame::NoFrame);
    teConsigne->setTextInteractionFlags(Qt::NoTextInteraction);
    m_vLayout->addWidget(teConsigne);

    QHBoxLayout* hlay = new QHBoxLayout(m_frmMultimedia);
    QSpacerItem* sp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlay->addSpacerItem(sp);
    connect(m_multimedia, SIGNAL(signalAbeMultiMediaStateChanged(AbulEduMultiMediaV1::enumPlayerState)),this, SIGNAL(signalAbeExerciceWidgetMessageMultimediaEvent()),Qt::UniqueConnection);
#if !defined (Q_OS_WIN) || defined(Q_OS_ANDROID)
    connect(m_multimedia->abeMultiMediaGetAudioControlWidget(), SIGNAL(signalAbeControlAudioToggleOutput(bool)),this, SIGNAL(signalAbeExerciceWidgetMessageMultimediaEvent()),Qt::UniqueConnection);
#endif
    connect(this, SIGNAL(signalAbeExerciceWidgetMessageMultimediaEvent()),this, SLOT(slotRepaintParent()),Qt::UniqueConnection);
    m_multimedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioSetType(AbulEduControlAudioV1::PlayerType);
    m_multimedia->abeMultiMediaSetButtonVisible(AbulEduMultiMediaV1::BtnMagnifyingGlass | AbulEduMultiMediaV1::BtnPrevious | AbulEduMultiMediaV1::BtnNext | AbulEduMultiMediaV1::BtnHide,false);
    hlay->addWidget(m_multimedia->abeMultiMediaGetAudioControlWidget());
//    m_multimedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),QDir::homePath()+"/essaiOgg.ogg"));
    m_vLayout->addWidget(m_frmMultimedia);

    lbSeparationConsigne->setObjectName(QString::fromUtf8("lbSeparationConsigne"));
    lbSeparationConsigne->setPixmap(trait);
    m_vLayout->addWidget(lbSeparationConsigne);

    m_vLayout->addWidget(m_frmCustom);

    teTextExercice->setObjectName(QString::fromUtf8("teTextExercice"));
    teTextExercice->setTextInteractionFlags(Qt::NoTextInteraction);
    teTextExercice->setFrameShape(QFrame::NoFrame);
    m_vLayout->addWidget(teTextExercice);

    m_vLayout->addSpacerItem(spacer);

    gsPrincipale->addWidget(frPrincipale);

#if QT_VERSION > 0x050000
    QScroller::grabGesture(teConsigne, QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(teTextExercice, QScroller::LeftMouseButtonGesture);
#endif

    QSettings config(":/abuleduexercicev1/conf/abuleduexercicev1.conf",QSettings::IniFormat);
    config.beginGroup("message");
    m_defaultStyleSheet = config.value("styleSheet","background-color: #F8FCF4").toString();
    config.endGroup();
}

AbulEduExerciceWidgetMessageV1::~AbulEduExerciceWidgetMessageV1()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(m_multimedia){
        m_multimedia->abeMultiMediaForceStop();
    }
    delete ui;
    delete gsPrincipale;
    /* Je pense que les destructeurs sont inutiles : m_fond et frPrincipale sont détruits par le destructeur de la scène, par contre la destruction
        du layout ne détruit pas les objets. Les autres objets instanciés dans le constructeur sont fils de frPrincipale et détruits avec elle */
    //delete m_fond;
    //delete frPrincipale;
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageClose()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    // a faire ou pas ?
    //setAttribute(Qt::WA_DeleteOnClose);
    close();
}

void AbulEduExerciceWidgetMessageV1::changeEvent(QEvent *e)
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

void AbulEduExerciceWidgetMessageV1::showEvent(QShowEvent *)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    bool frameVisible = m_frmMultimedia->isVisible();
    bool mediaEmpty = m_multimedia->abeMultiMediaGetCurrentMediaMedia().isEmpty();
    if(mediaEmpty && frameVisible){
        m_frmMultimedia->setVisible(false);
        setDimensionsWidget();
    }
    else if(!mediaEmpty && !frameVisible){
        m_frmMultimedia->setVisible(true);
        setDimensionsWidget();
    }
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageSetImageFond(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_imageFond.load(s);
    m_fond->setPixmap(m_imageFond);
    m_fond->setPos(0,0);
    setDimensionsWidget();
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageShowImageFond(bool trueFalse)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_hasImageFond = trueFalse;
    if (m_hasImageFond == true)
    {
        setStyleSheet("background-color: transparent;");
    }
    else
    {
        setStyleSheet(m_defaultStyleSheet);
    }
}

bool AbulEduExerciceWidgetMessageV1::abeWidgetMessageHasImageFond()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    return m_hasImageFond;
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageSetTitre(const QString& s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    lbTitre->setText(s);
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageSetConsigne(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    teConsigne->setHtml(s);
    teConsigne->setVisible(!s.isEmpty());
    lbSeparationTitre->setVisible(teConsigne->isVisible());
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageSetTexteExercice(const QString &s)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    teTextExercice->setHtml(s);
    teTextExercice->setVisible(!s.isEmpty());
    teTextExercice->setMinimumSize(teTextExercice->document()->size().toSize());
    lbSeparationConsigne->setVisible(teTextExercice->isVisible());
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageSetZoneTexteVisible(bool yesNo)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    lbSeparationConsigne->setVisible(yesNo);
    teTextExercice->setVisible(yesNo);
    if (m_hasImageFond == true)
    {
        if (yesNo)
        {
            abeWidgetMessageSetImageFond(":/abuleduexercicev1/fonds/fondmessagegrand");
        }
        else
        {
            abeWidgetMessageSetImageFond(":/abuleduexercicev1/fonds/fondmessagepetit");

        }
    }
    setDimensionsWidget();
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageSetIsDisplayError()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_isDisplayError = true;
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageSetMediaMedias(const AbulEduMediaMedias &media)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    bool display = isVisible();
    setVisible(false);
    m_multimedia->abeMultimediaSetTTS(AbulEduPicottsV1::None);
    m_multimedia->abeMultiMediaSetNewMedia(media);
    setVisible(display);
}

void AbulEduExerciceWidgetMessageV1::abeWidgetMessageResize()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    setDimensionsWidget();
}

void AbulEduExerciceWidgetMessageV1::setDimensionsWidget()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    float ratio = abeApp->getAbeApplicationDecorRatio();
    ABULEDU_LOG_DEBUG() <<  "  ratio "<<ratio;

    QPixmap fond = m_imageFond;
    int largeur = fond.width()*ratio;
    if (m_hasImageFond == false)
    {
        largeur = m_parent->width()*2/3;
    }

    ABULEDU_LOG_DEBUG() << "  Taille image de fond : "<<largeur<<" x "<<fond.height()*ratio;

    frPrincipale->setFixedWidth(largeur - 100*ratio);
    lbTitre->setFixedWidth(largeur - 110*ratio);
    lbSeparationTitre->setFixedWidth(largeur - 110*ratio);
    lbSeparationConsigne->setFixedWidth(largeur - 110*ratio);
    teConsigne->setFixedWidth(largeur - 110*ratio);
    teTextExercice->setFixedWidth(largeur - 110*ratio);
    teTextExercice->document()->setTextWidth(largeur - 110*ratio -30);

    // Calcul de la hauteur nécessaire (et suffisante)
    int hauteur = 100*ratio;
    ABULEDU_LOG_DEBUG() <<"  Hauteur initiale " << hauteur;

    lbTitre->setFixedHeight(lbTitre->fontMetrics().height() +10);
    ABULEDU_LOG_DEBUG() << "  Hauteur titre " << lbTitre->height();
    lbSeparationTitre->setFixedHeight(lbSeparationTitre->pixmap()->height());
    ABULEDU_LOG_DEBUG() << "  Hauteur separation titre " << lbSeparationTitre->height();

    if (m_previousRatio != 0.0 && m_previousRatio !=ratio)
    {
        if (teConsigne->toHtml().contains("img"))
        {
            teConsigne->setHtml(scaleImgWidthHtml(teConsigne->toHtml(),ratio/m_previousRatio));
            teConsigne->setHtml(scaleImgHeightHtml(teConsigne->toHtml(),ratio/m_previousRatio));
        }
        m_previousRatio = ratio;
    }
    teConsigne->setFixedHeight(teConsigne->document()->size().height());
    ABULEDU_LOG_DEBUG() << "  Hauteur consigne " << teConsigne->height();

    hauteur = hauteur
              + lbTitre->height()
              + lbSeparationTitre->height()
              + teConsigne->height();
    if(m_frmMultimedia->isVisible()){
        hauteur += m_frmMultimedia->height();
    }
    hauteur += m_frmCustom->height();
    ABULEDU_LOG_DEBUG() << "  Hauteur calcul1 " << hauteur;

    lbSeparationConsigne->setFixedHeight(lbSeparationConsigne->pixmap()->height());

    if (m_hasImageFond == true)
    {
        teTextExercice->setFixedHeight(qMin((teTextExercice->document()->size().height()+10),qreal(frPrincipale->height()/2)-30));
    }
    else
    {
        teTextExercice->setFixedHeight(qMin((teTextExercice->document()->size().height()+10),qreal(m_parent->height()*0.5)));
    }

    if(teTextExercice->isVisible())
    {
        hauteur = hauteur
                  + lbSeparationConsigne->height()
                  + teTextExercice->height();
        ABULEDU_LOG_DEBUG() <<"  Hauteur separation consigne  " << lbSeparationTitre->height()
                           << "  Hauteur texte exercice " << teTextExercice->height()
                           << "  Hauteur calcul2 " << hauteur;

    }

    fond = fond.scaled(largeur,hauteur, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ABULEDU_LOG_DEBUG() << "  Image de fond : " << largeur << " x " << hauteur;
    m_fond->setPixmap(fond);
    m_fond->setPos(0,0);
    if (m_hasImageFond == true)
    {
        ui->gvPrincipale->setFixedSize(largeur, hauteur);
        frPrincipale->setFixedHeight(hauteur);
        frPrincipale->move(50*ratio, 50*ratio);
        this->setFixedWidth(largeur);
        this->setFixedHeight(hauteur);
    }
    else
    {
        ui->gvPrincipale->setFixedSize(largeur, m_parent->height()*0.85);
        frPrincipale->setFixedHeight(m_parent->height()*0.85);
        frPrincipale->move(50*ratio, 50*ratio);
        this->setFixedWidth(largeur);
        this->setFixedHeight(m_parent->height()*0.9);
    }
    if(hauteur > this->height()){
        teTextExercice->setFixedHeight(teTextExercice->height()-(hauteur - this->height()+12*ratio));
    }
}

void AbulEduExerciceWidgetMessageV1::keyPressEvent(QKeyEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QWidget::keyPressEvent(event);
}

void AbulEduExerciceWidgetMessageV1::keyReleaseEvent(QKeyEvent *event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if (m_isDisplayError)
    {
        hide();
    }
    else
    {
        QWidget::keyReleaseEvent(event);
    }
}

void AbulEduExerciceWidgetMessageV1::mouseReleaseEvent(QMouseEvent* event)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if (m_isDisplayError == true)
    {
        hide();
        m_isDisplayError = false;
    }
    else
    {
        QWidget::mouseReleaseEvent(event);
    }
}

QString AbulEduExerciceWidgetMessageV1::scaleImgWidthHtml(QString html, float ratio)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    int imgOpenTag = html.indexOf("<img");
    int imgCloseTag = html.indexOf(">",imgOpenTag);
    QString contentTag = html.mid(imgOpenTag, imgCloseTag - imgOpenTag +1);
    QString previousContentTag = contentTag;
    ABULEDU_LOG_DEBUG() <<"  Balise image : " << contentTag;
    int keyWidth = contentTag.indexOf("width=");
    if (keyWidth == -1)
    {
        int keySrc = contentTag.indexOf("src=");
        int beginSrc = contentTag.indexOf("\"",keySrc);
        int endSrc = contentTag.indexOf("\"",beginSrc+1);
        QString contentKeySrc = contentTag.mid(beginSrc+1,endSrc-beginSrc-1);
        QPixmap copiePourTaille(contentKeySrc);
        contentTag.insert(endSrc+1," width=\""+QString::number(copiePourTaille.width()*ratio)+"\"");
        html.replace(previousContentTag,contentTag);
    }
    else
    {
        int beginWidth = contentTag.indexOf("\"",keyWidth);
        int endWidth = contentTag.indexOf("\"",beginWidth+1);
        QString contentKeyWidth = contentTag.mid(beginWidth+1,endWidth-beginWidth-1);
        bool widthConversion;
        float readWidth = contentKeyWidth.toFloat(&widthConversion);
        contentTag.replace(contentKeyWidth,QString::number(readWidth*ratio));
        ABULEDU_LOG_DEBUG() <<"Balise image modifiee : " << contentTag;
        html.replace(previousContentTag,contentTag);
    }
    ABULEDU_LOG_DEBUG() <<"  Message modifie" << html;
    return html;
}

QString AbulEduExerciceWidgetMessageV1::scaleImgHeightHtml(QString html, float ratio)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    int imgOpenTag = html.indexOf("<img");
    int imgCloseTag = html.indexOf(">",imgOpenTag);
    QString contentTag = html.mid(imgOpenTag, imgCloseTag - imgOpenTag +1);
    QString previousContentTag = contentTag;
    ABULEDU_LOG_DEBUG() << "Balise image : " << contentTag;
    int keyHeight = contentTag.indexOf("height=");
    if (keyHeight == -1)
    {
        int keySrc = contentTag.indexOf("src=");
        int beginSrc = contentTag.indexOf("\"",keySrc);
        int endSrc = contentTag.indexOf("\"",beginSrc+1);
        QString contentKeySrc = contentTag.mid(beginSrc+1,endSrc-beginSrc-1);
        QPixmap copiePourTaille(contentKeySrc);
        contentTag.insert(endSrc+1," height=\""+QString::number(copiePourTaille.height()*ratio)+"\"");
        html.replace(previousContentTag,contentTag);
    }
    else
    {
        int beginHeight = contentTag.indexOf("\"",keyHeight);
        int endHeight = contentTag.indexOf("\"",beginHeight+1);
        QString contentKeyHeight = contentTag.mid(beginHeight+1,endHeight-beginHeight-1);
        bool heightConversion;
        float readHeight = contentKeyHeight.toFloat(&heightConversion);
        contentTag.replace(contentKeyHeight,QString::number(readHeight*ratio));
        ABULEDU_LOG_DEBUG() <<"Balise image modifiee : " << contentTag;
        html.replace(previousContentTag,contentTag);
    }
    ABULEDU_LOG_DEBUG() <<"  Message modifie " <<html;
    return html;
}

void AbulEduExerciceWidgetMessageV1::btnFermer_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    this->hide();
}

void AbulEduExerciceWidgetMessageV1::slotRepaintParent()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    update();
}
