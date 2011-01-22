#include "mainwindow.h"
#include <QMessageBox>
#include <QDesktopWidget>

bool ishexnstring(const QString& string)
{
  for (int i = 0; i < string.length(); i++) {
    if (isxdigit(string[i] == 0))
      return false;
  }
  return true;
}

QString unescapeJson( const QByteArray& ba, bool* ok )
{
  *ok = false;
  QString res;
  QByteArray seg;
  bool bs = false;
  for ( int i = 0, size = ba.size(); i < size; ++i ) {
    const char ch = ba[i];
    if ( !bs ) {
      if ( ch == '\\' )
        bs = true;
      else
        seg += ch;
    } else {
      bs = false;
      switch ( ch ) {
        case 'b':
          seg += '\b';
          break;
        case 'f':
          seg += '\f';
          break;
        case 'n':
          seg += '\n';
          break;
        case 'r':
          seg += '\r';
          break;
        case 't':
          seg += '\t';
          break;
        case 'u':
        {
          res += QString::fromUtf8( seg );
          seg.clear();

          if ( i > size - 5 ) {
            //error
            return QString();
          }

          const QString hex_digit1 = QString::fromUtf8( ba.mid( i + 1, 2 ) );
          const QString hex_digit2 = QString::fromUtf8( ba.mid( i + 3, 2 ) );
          i += 4;

          if ( !ishexnstring( hex_digit1 ) || !ishexnstring( hex_digit2 ) ) {
            qCritical() << "Not an hex string:" << hex_digit1 << hex_digit2;
            return QString();
          }
          bool hexOk;
          const ushort hex_code1 = hex_digit1.toShort( &hexOk, 16 );
          if (!hexOk) {
            qCritical() << "error converting hex value to short:" << hex_digit1;
            return QString();
          }
          const ushort hex_code2 = hex_digit2.toShort( &hexOk, 16 );
          if (!hexOk) {
            qCritical() << "error converting hex value to short:" << hex_digit2;
            return QString();
          }

          res += QChar(hex_code2, hex_code1);
          break;
        }
        case '\\':
          seg  += '\\';
          break;
        default:
          seg += ch;
          break;
      }
    }
  }
  res += QString::fromUtf8( seg );
  *ok = true;
  return res;
}

QString unescapeXML(const QString &str)
{
    QString data = str;
    data.replace( "&#39;", "'" );
    data.replace( "&quot;", "\"" );
    return data;
}

/************************ Main Window ********************************/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    _accessManager = new QNetworkAccessManager(this);
    _toWindow = 0;

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5AutoOrientation, true);
#endif

    createMenu();
    createComponents();
    createPortraitLayout();
    createLandscapeLayout();
    createAutoLayout();

    selfAssignLayout();

    createConnects();

    createLangCodes(langCodes(), 1, 1);

    retranslateUi();
    loadSettings();
}

QMap<QString, QString> MainWindow::langCodes() {

    QMap<QString, QString> langInput;

    langInput["ca"] = tr("Catalan");
    langInput["ca_valencia"] =  tr("Catalan (Valencian)");
    langInput["en"] = tr("English");
    langInput["fr"] = tr("French") ;
    langInput["pt"] = tr("Portuguese");
    langInput["es"] = tr("Spanish");

    return langInput;
}

void MainWindow::createMenu()
{
    actionLeft_from_clipboard = new QAction(this);
    actionLeft_from_clipboard->setObjectName(QString::fromUtf8("actionLeft_from_clipboard"));
    actionRight_from_clipboard = new QAction(this);
    actionRight_from_clipboard->setObjectName(QString::fromUtf8("actionRight_from_clipboard"));
    actionLeft_to_clipboard = new QAction(this);
    actionLeft_to_clipboard->setObjectName(QString::fromUtf8("actionLeft_to_clipboard"));
    actionRight_to_clipboard = new QAction(this);
    actionRight_to_clipboard->setObjectName(QString::fromUtf8("actionRight_to_clipboard"));
    actionLeft_as_sms = new QAction(this);
    actionLeft_as_sms->setObjectName(QString::fromUtf8("actionLeft_as_sms"));
    actionRight_as_sms = new QAction(this);
    actionRight_as_sms->setObjectName(QString::fromUtf8("actionRight_as_sms"));
    actionLeft_as_e_mail = new QAction(this);
    actionLeft_as_e_mail->setObjectName(QString::fromUtf8("actionLeft_as_e_mail"));
    actionRight_as_e_mail = new QAction(this);
    actionRight_as_e_mail->setObjectName(QString::fromUtf8("actionRight_as_e_mail"));
    actionMark_as_Unknown = new QAction(this);
    actionMark_as_Unknown->setCheckable(true);
    actionMark_as_Unknown->setObjectName(QString::fromUtf8("actionMark_as_Unknown"));
    actionRight_as_Main = new QAction(this);
    actionRight_as_Main->setCheckable(true);
    actionRight_as_Main->setObjectName(QString::fromUtf8("actionRight_as_Main"));
    actionAbout_qtraductor = new QAction(this);
    actionAbout_qtraductor->setObjectName(QString::fromUtf8("actionAbout_qtraductor"));
    //actionDonate = new QAction(this);
    //actionDonate->setObjectName(QString::fromUtf8("actionDonate"));

    QMenuBar * menuBar = new QMenuBar(this);

    menuBar->addAction(actionLeft_from_clipboard);
    menuBar->addAction(actionRight_from_clipboard);
    menuBar->addAction(actionLeft_to_clipboard);
    menuBar->addAction(actionRight_to_clipboard);
    menuBar->addAction(actionLeft_as_sms);
    menuBar->addAction(actionRight_as_sms);
    menuBar->addAction(actionLeft_as_e_mail);
    menuBar->addAction(actionRight_as_e_mail);
    menuBar->addAction(actionMark_as_Unknown);
    menuBar->addAction(actionRight_as_Main);
    menuBar->addAction(actionAbout_qtraductor);
    setMenuBar(menuBar);
    //Fremantle->addAction(actionDonate);
}

void MainWindow::createComponents()
{
    texteditLeft = new QTextEdit();
    texteditLeft->setObjectName(QString::fromUtf8("texteditLeft"));
    texteditRight = new QTextEdit();
    texteditRight->setObjectName(QString::fromUtf8("texteditRight"));

    comboboxLang_left = new QComboBox();
    comboboxLang_left->setObjectName(QString::fromUtf8("comboboxLang_left"));

    comboboxLang_right = new QComboBox();
    comboboxLang_right->setObjectName(QString::fromUtf8("comboboxLang_right"));

    buttonLeft_clear = new QPushButton();
    buttonLeft_clear->setObjectName(QString::fromUtf8("buttonLeft_clear"));
    buttonLeft_clear->setMaximumSize(QSize(100, 16777215));

    buttonRight_clear = new QPushButton();
    buttonRight_clear->setObjectName(QString::fromUtf8("buttonRight_clear"));
    buttonRight_clear->setMaximumSize(QSize(100, 16777215));

    buttonLeft_to_right = new QToolButton();
    buttonLeft_to_right->setObjectName(QString::fromUtf8("buttonLeft_to_right"));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/images/arrowright.png"), QSize(), QIcon::Normal, QIcon::Off);
    buttonLeft_to_right->setIcon(icon);

    buttonRight_to_left = new QToolButton();
    buttonRight_to_left->setObjectName(QString::fromUtf8("buttonRight_to_left"));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/images/images/arrowleft.png"), QSize(), QIcon::Normal, QIcon::Off);
    buttonRight_to_left->setIcon(icon1);
}

void MainWindow::createPortraitLayout()
{
    layoutPortrait = new QVBoxLayout;

    QVBoxLayout * verticalLayout_left = new QVBoxLayout();
    verticalLayout_left->addWidget(texteditLeft);
    QHBoxLayout * horizontalLayout_left = new QHBoxLayout();
    horizontalLayout_left->addWidget(comboboxLang_left);
    horizontalLayout_left->addWidget(buttonLeft_clear);
    verticalLayout_left->addLayout(horizontalLayout_left);

    layoutPortrait->addLayout(verticalLayout_left);

    QHBoxLayout * horizontalLayout_center = new QHBoxLayout();
    horizontalLayout_center->addWidget(buttonLeft_to_right);
    horizontalLayout_center->addWidget(buttonRight_to_left);

    layoutPortrait->addLayout(horizontalLayout_center);

    QVBoxLayout * verticalLayout_right = new QVBoxLayout();
    verticalLayout_right->addWidget(texteditRight);
    QHBoxLayout * horizontalLayout_right = new QHBoxLayout();
    horizontalLayout_right->addWidget(comboboxLang_right);
    horizontalLayout_right->addWidget(buttonRight_clear);
    verticalLayout_right->addLayout(horizontalLayout_right);

    layoutPortrait->addLayout(verticalLayout_right);
}

void MainWindow::createLandscapeLayout()
{
    layoutLandscape = new QHBoxLayout;

    QVBoxLayout * verticalLayout_left = new QVBoxLayout();
    verticalLayout_left->addWidget(texteditLeft);
    QHBoxLayout * horizontalLayout_left = new QHBoxLayout();
    horizontalLayout_left->addWidget(comboboxLang_left);
    horizontalLayout_left->addWidget(buttonLeft_clear);
    verticalLayout_left->addLayout(horizontalLayout_left);

    layoutLandscape->addLayout(verticalLayout_left);

    QVBoxLayout * verticalLayout_center = new QVBoxLayout();
    verticalLayout_center->addWidget(buttonLeft_to_right);
    verticalLayout_center->addWidget(buttonRight_to_left);

    layoutLandscape->addLayout(verticalLayout_center);

    QVBoxLayout * verticalLayout_right = new QVBoxLayout();
    verticalLayout_right->addWidget(texteditRight);
    QHBoxLayout * horizontalLayout_right = new QHBoxLayout();
    horizontalLayout_right->addWidget(buttonRight_clear);
    horizontalLayout_right->addWidget(comboboxLang_right);
    verticalLayout_right->addLayout(horizontalLayout_right);

    layoutLandscape->addLayout(verticalLayout_right);
}

void MainWindow::createAutoLayout()
{
    autoLayout = new PLLayout();
    autoLayout->setPLayout(layoutPortrait);
    autoLayout->setLLayout(layoutLandscape);
}

void MainWindow::selfAssignLayout()
{
    centralWidget = new QWidget();
    centralWidget->setLayout(autoLayout);
    this->setCentralWidget(centralWidget);
}

void MainWindow::createConnects()
{
    connect(comboboxLang_left, SIGNAL(activated(int)), this, SLOT(itemLeft(int)));
    connect(comboboxLang_right, SIGNAL(activated(int)), this, SLOT(itemRight(int)));
    connect(actionLeft_from_clipboard, SIGNAL(triggered()), this, SLOT(leftFromClipboard()));
    connect(actionRight_from_clipboard, SIGNAL(triggered()), this, SLOT(rightFromClipboard()));
    connect(actionLeft_to_clipboard, SIGNAL(triggered()), this, SLOT(leftToClipboard()));
    connect(actionRight_to_clipboard, SIGNAL(triggered()), this, SLOT(rightToClipboard()));
    connect(buttonLeft_to_right, SIGNAL(clicked()), this, SLOT(translateLeftToRight()));
    connect(buttonRight_to_left, SIGNAL(clicked()), this, SLOT(translateRightToLeft()));
    connect(actionLeft_as_e_mail, SIGNAL(triggered()), this, SLOT(leftAsEmail()));
    connect(actionRight_as_e_mail, SIGNAL(triggered()), this, SLOT(rightAsEmail()));
    connect(actionLeft_as_sms, SIGNAL(triggered()), this, SLOT(leftAsSms()));
    connect(actionRight_as_sms, SIGNAL(triggered()), this, SLOT(rightAsSms()));
    connect(actionRight_as_Main, SIGNAL(triggered()), this, SLOT(rightAsMain()));
    connect(buttonLeft_clear, SIGNAL(clicked()), this, SLOT(leftClear()));
    connect(buttonRight_clear, SIGNAL(clicked()), this, SLOT(rightClear()));
    connect(actionAbout_qtraductor, SIGNAL(triggered()), this, SLOT(aboutQTraductor()));
    //connect(actionDonate, SIGNAL(triggered()), this, SLOT(donate()));

    connect(_accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(translateRequestFinished(QNetworkReply*)));
}

void MainWindow::retranslateUi()
{
    setWindowTitle(QApplication::translate("MainWindow", "qTraductor", 0, QApplication::UnicodeUTF8));
    actionLeft_from_clipboard->setText(QApplication::translate("MainWindow", "Left from clipboard", 0, QApplication::UnicodeUTF8));
    actionRight_from_clipboard->setText(QApplication::translate("MainWindow", "Right from clipboard", 0, QApplication::UnicodeUTF8));
    actionLeft_to_clipboard->setText(QApplication::translate("MainWindow", "Left to clipboard", 0, QApplication::UnicodeUTF8));
    actionRight_to_clipboard->setText(QApplication::translate("MainWindow", "Right to clipboard", 0, QApplication::UnicodeUTF8));
    actionLeft_as_sms->setText(QApplication::translate("MainWindow", "Left as SMS", 0, QApplication::UnicodeUTF8));
    actionRight_as_sms->setText(QApplication::translate("MainWindow", "Right as SMS", 0, QApplication::UnicodeUTF8));
    actionLeft_as_e_mail->setText(QApplication::translate("MainWindow", "Left as e-mail", 0, QApplication::UnicodeUTF8));
    actionRight_as_e_mail->setText(QApplication::translate("MainWindow", "Right as e-mail", 0, QApplication::UnicodeUTF8));
    actionMark_as_Unknown->setText(QApplication::translate("MainWindow", "Mark Unknown", 0, QApplication::UnicodeUTF8));
    actionRight_as_Main->setText(QApplication::translate("MainWindow", "Right as Main", 0, QApplication::UnicodeUTF8));
    actionAbout_qtraductor->setText(QApplication::translate("MainWindow", "About qTraductor", 0, QApplication::UnicodeUTF8));
    //actionDonate->setText(QApplication::translate("MainWindow", "Donate", 0, QApplication::UnicodeUTF8));
    buttonLeft_clear->setText(QApplication::translate("MainWindow", "Clear", 0, QApplication::UnicodeUTF8));
    buttonRight_clear->setText(QApplication::translate("MainWindow", "Clear", 0, QApplication::UnicodeUTF8));
}

void MainWindow::saveSettings() {
    QSettings settings("Toni Hermoso Pulido", "qTraductor");
    QString currentLeftLang = comboboxLang_left->itemText(comboboxLang_left->currentIndex());
    QString currentRightLang = comboboxLang_right->itemText(comboboxLang_right->currentIndex());
    settings.setValue("leftLanguage", currentLeftLang);
    settings.setValue("rightLanguage", currentRightLang);
}

void MainWindow::loadSettings() {
    QSettings settings("Toni Hermoso Pulido", "qTraductor");
    QString currentLeftLang = settings.value("leftLanguage", "es").toString();
    QString currentRightLang = settings.value("rightLanguage", "ca").toString();
    comboboxLang_left->setCurrentIndex(comboboxLang_left->findText(currentLeftLang));
    comboboxLang_right->setCurrentIndex(comboboxLang_right->findText(currentRightLang));
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
    saveSettings();
    event->accept();
 }


void MainWindow::aboutQTraductor()
{
    QMessageBox::information(this, tr("About qTraductor"),tr("qTraductor - A translation software for Maemo, Symbian and MeeGo using <a href=\"http://www.softcatala.org/traductor\">Softcatal&agrave; translation service</a>.<br>") + "v0.1<br>" + tr("Adapted from the code of <a href=\"http://mobileqt.de/apps/qtranslate\">qTranslate</a> (for Google Translate)<br>Find more information and support on <a href=\"http://www.softcatala.org/qtraductor\">Softcatal&agrave; website</a>."));
}

void MainWindow::leftClear()
{
    texteditLeft->clear();
}

void MainWindow::rightClear()
{
    texteditRight->clear();
}

void MainWindow::leftAsSms()
{
    QString text = texteditLeft->toPlainText();
    QDesktopServices::openUrl("sms:?body="+text);
}

void MainWindow::rightAsSms()
{
    QString text = texteditRight->toPlainText();
    QDesktopServices::openUrl("sms:?body="+text);
}

void MainWindow::leftAsEmail()
{
    QString text = texteditLeft->toPlainText();
    QDesktopServices::openUrl("mailto:?body="+text);
}

void MainWindow::rightAsEmail()
{
    QString text = texteditRight->toPlainText();
    QDesktopServices::openUrl("mailto:?body="+text);
}

void MainWindow::rightAsMain()
{
    // Ensure all codes are there
    MainWindow::createLangCodes(langCodes(), 1, 1);
}

void MainWindow::translateLeftToRight()
{
    QString text = texteditLeft->toPlainText();
    int iLangFrom = comboboxLang_left->currentIndex();
    QString langFrom = comboboxLang_left->itemData(iLangFrom).toString();
    int iLangTo = comboboxLang_right->currentIndex();
    QString langTo = comboboxLang_right->itemData(iLangTo).toString();

    QString MarkUnknown = "no";

    if (actionMark_as_Unknown->isChecked()) {
        MarkUnknown = "yes";
    }

    _toWindow = 1;
    scTranslate(text, langFrom, langTo, MarkUnknown);


}

void MainWindow::translateRightToLeft()
{
    QString text = texteditRight->toPlainText();
    int iLangFrom = comboboxLang_right->currentIndex();
    QString langFrom = comboboxLang_right->itemData(iLangFrom).toString();
    int iLangTo = comboboxLang_left->currentIndex();
    QString langTo = comboboxLang_left->itemData(iLangTo).toString();

    QString MarkUnknown = "no";

    if (actionMark_as_Unknown->isChecked()) {
        MarkUnknown = "yes";
    }

    _toWindow = 0;
    scTranslate(text, langFrom, langTo, MarkUnknown);

}

void MainWindow::scTranslate(QString text, QString langFrom, QString langTo, QString MarkUnknown)
{

    // Hack for ca_valencia
    if (langFrom == "ca_valencia") {
        langFrom = "ca";
    }

    QNetworkRequest request;
    request.setUrl( QUrl("http://www.softcatala.org/apertium/json/translate?q="+text+"&langpair="+langFrom+"|"+langTo+"&key=NWI0MjQwMzEzMDMzMzQ2MjYyMzU"+"&markUnknown="+MarkUnknown));
    request.setRawHeader("Host", "www.softcatala.org");
    request.setRawHeader("User-Agent", "qTraductor");
    request.setRawHeader("Accept-Charset", "utf-8;q=0.7,*;q=0.7");
    request.setRawHeader("Referer", "qTraductor");
    _accessManager->get(request);

}

void MainWindow::leftFromClipboard()
{
    fromClipboard(0);
}

void MainWindow::rightFromClipboard()
{
    fromClipboard(1);
}

void MainWindow::leftToClipboard()
{
    toClipboard(0);
}

void MainWindow::rightToClipboard()
{
    toClipboard(1);
}

void MainWindow::fromClipboard(int side)
{
    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardText = clipboard->text();
    if (side == 0)
        texteditLeft->setText(clipboardText);
    else
        texteditRight->setText(clipboardText);
}

void MainWindow::toClipboard(int side)
{
    QClipboard *clipboard = QApplication::clipboard();
    QString text;
    if (side == 0)
        text = texteditLeft->toPlainText();
    else
        text = texteditRight->toPlainText();
    clipboard->setText(text);
}

MainWindow::~MainWindow()
{
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    // Call the autoLayout convenience function for the event handling
    autoLayout->resizeEvent(event);
    // Call base class resize event handler
    QMainWindow::resizeEvent(event);

#ifdef Q_OS_SYMBIAN
    showMaximized();
#endif

    QRect qs=QApplication::desktop()->availableGeometry();
    if (event->size().height() > event->size().width()) {
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/images/arrowdown.png"), QSize(), QIcon::Normal, QIcon::Off);
        buttonLeft_to_right->setIcon(icon);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/images/arrowup.png"), QSize(), QIcon::Normal, QIcon::Off);
        buttonRight_to_left->setIcon(icon2);
    } else {
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/images/arrowright.png"), QSize(), QIcon::Normal, QIcon::Off);
        buttonLeft_to_right->setIcon(icon);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/images/arrowleft.png"), QSize(), QIcon::Normal, QIcon::Off);
        buttonRight_to_left->setIcon(icon2);
    }
}

void MainWindow::createLangCodes(QMap<QString, QString> langCodes, int left, int right)
{

    if (left) {
        comboboxLang_left->clear();
    }
    if (right) {
        comboboxLang_right->clear();
    }

    QMapIterator<QString, QString> i(langCodes);
    while (i.hasNext()) {

        i.next();

         if (left) {
            comboboxLang_left->addItem(i.value(), i.key());
        }
         if (right) {
         comboboxLang_right->addItem(i.value(), i.key());
        }
     }
}


void MainWindow::itemLeft(int CChosen) {

    if (!actionRight_as_Main->isChecked()) {
        QString LChosen;
        LChosen = comboboxLang_left->itemData(CChosen).toString();
        MainWindow::itemChosen(LChosen, 0);
    }
}

void MainWindow::itemRight(int CChosen) {

    if (actionRight_as_Main->isChecked()) {

        QString LChosen;
        LChosen = comboboxLang_right->itemData(CChosen).toString();
        MainWindow::itemChosen(LChosen, 1);
    }
}

void MainWindow::itemChosen(QString LChosen, int fromRight)
 {

    QHash<QString, QList<QString> > langCorresp;
    QMap<QString, QString> langCodes = MainWindow::langCodes();

    langCorresp["ca"].append("es");
    langCorresp["ca"].append("en");
    langCorresp["ca"].append("fr");
    langCorresp["ca"].append("pt");
    langCorresp["ca_valencia"].append("es");
    langCorresp["es"].append("ca");
    langCorresp["es"].append("ca_valencia");
    langCorresp["en"].append("ca");
    langCorresp["fr"].append("ca");
    langCorresp["pt"].append("ca");

    //qDebug() << langCorresp[LChosen];

    QMap<QString, QString> langIn;

    QListIterator<QString> q(langCorresp[LChosen]);

    while (q.hasNext())
     {

        langIn[q.peekNext()] =  langCodes[q.peekNext()] ;

        q.next();
     }

    if (fromRight) {
        MainWindow::createLangCodes(langIn, 1, 0);
    }

    else {
        MainWindow::createLangCodes(langIn, 0, 1);
    }

 }

void MainWindow::translateRequestFinished(QNetworkReply * reply)
 {
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::information(this, tr("HTTP"),
                                  tr("Could not connect to the translation server. Please check that offline mode is disabled and device is able to connect to a network. (%1)")
                                  .arg(reply->errorString()));
    } else {
        bool ok = false;
        QString jsonTranslation = unescapeJson(reply->readAll(), &ok).trimmed();
        //qDebug() << jsonTranslation;
        QString translation = QString("");
        QRegExp regexp = QRegExp("\"translatedText\":\"(.*)\"");
        regexp.setPatternSyntax(QRegExp::RegExp2);
        regexp.setMinimal(true);
        if (jsonTranslation.contains(regexp))
        {
            translation = regexp.cap(1);
            translation = unescapeXML(translation);
            //qDebug() << translation;
            //translation = translation.fromUtf8(translation);
        }
        else
        {
            //qDebug("not found");
        }

        if (_toWindow == 0)
            texteditLeft->setText(translation);
        else
            texteditRight->setText(translation);
    }
    reply->deleteLater();
}

