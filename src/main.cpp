#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QPalette>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("RigPlan Integrator");
    app.setOrganizationName("RigPlan Systems");
    app.setApplicationVersion("1.0.0");

    // Load stylesheet from QRC resource
    QFile styleFile(":/styles/theme.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleStr = QString::fromUtf8(styleFile.readAll());
        app.setStyleSheet(styleStr);
        styleFile.close();
    } else {
        qWarning() << "Não foi possível carregar o tema visual QSS de :/styles/theme.qss";
    }

    MainWindow w;
    w.showMaximized();

    return app.exec();
}
