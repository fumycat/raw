#include "mainwindow.h"
#include "raw_global.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(translations);

    QApplication a(argc, argv);

#ifdef SET_LANG_RU
    QTranslator qtLanguageTranslator;
    qDebug() << qtLanguageTranslator.load("QtLanguage_ru_RU", ":/translations", "_");
    qApp->installTranslator(&qtLanguageTranslator);
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
