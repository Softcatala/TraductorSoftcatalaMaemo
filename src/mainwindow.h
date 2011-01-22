#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QtGui>
#include <QtNetwork>

#include "PLLayout.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadSettings();
    void saveSettings();
    QMap<QString, QString> langCodes();

public slots:
    void leftFromClipboard();
    void leftToClipboard();
    void rightFromClipboard();
    void rightToClipboard();
    void leftAsEmail();
    void rightAsEmail();
    void leftAsSms();
    void rightAsSms();
    void leftClear();
    void rightClear();
    void rightAsMain();
    void translateLeftToRight();
    void translateRightToLeft();
    void aboutQTraductor();
    void itemLeft(int);
    void itemRight(int);
    void itemChosen(QString, int);
    //void donate();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void closeEvent(QCloseEvent *event);

private:
    void fromClipboard(int);
    void toClipboard(int);
    void createLangCodes(QMap<QString, QString>, int, int);
    void scTranslate(QString, QString, QString, QString);
    void createMenu();
    void createComponents();
    void createPortraitLayout();
    void createLandscapeLayout();
    void createAutoLayout();
    void createConnects();
    void componentActions();
    void selfAssignLayout();
    void retranslateUi();

    QVBoxLayout *layoutPortrait;
    QHBoxLayout *layoutLandscape;
    PLLayout *autoLayout;
    QWidget *centralWidget;

    QAction *actionLeft_from_clipboard;
    QAction *actionRight_from_clipboard;
    QAction *actionLeft_to_clipboard;
    QAction *actionRight_to_clipboard;
    QAction *actionLeft_as_sms;
    QAction *actionRight_as_sms;
    QAction *actionLeft_as_e_mail;
    QAction *actionRight_as_e_mail;
    QAction *actionMark_as_Unknown;
    QAction *actionRight_as_Main;
    QAction *actionAbout_qtraductor;
    //QAction *actionDonate;

    QTextEdit *texteditLeft;
    QTextEdit *texteditRight;

    QComboBox *comboboxLang_left;
    QComboBox *comboboxLang_right;

    QPushButton *buttonLeft_clear;
    QPushButton *buttonRight_clear;

    QToolButton *buttonLeft_to_right;
    QToolButton *buttonRight_to_left;

    QNetworkAccessManager *_accessManager;
    int _toWindow;


private slots:
    void translateRequestFinished(QNetworkReply *);
};

#endif // MAINWINDOW_H
