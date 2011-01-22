#include <QtGui/QApplication>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load(":/translation/qtraductor_" + QLocale::system().name());
    //qDebug() << "qtranslate_" << QLocale::system().name();
    a.installTranslator(&myappTranslator);

    MainWindow w;
#ifdef Q_OS_SYMBIAN
    w.showMaximized();
#else
    w.show();
#endif
    return a.exec();
}
