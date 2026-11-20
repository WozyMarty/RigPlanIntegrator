/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "widgets/calculationpanel.h"
#include "widgets/documentpreview.h"
#include "widgets/headerbar.h"
#include "widgets/preseteditor.h"
#include "widgets/presetgallery.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *rootLayout;
    HeaderBar *headerBar;
    QStackedWidget *stackedWidget;
    QWidget *pageMain;
    QHBoxLayout *mainLayout;
    QSplitter *mainSplitter;
    DocumentPreview *docPreview;
    CalculationPanel *calcPanel;
    QWidget *pageGallery;
    QHBoxLayout *galleryLayout;
    PresetGallery *presetGallery;
    QWidget *pageEditor;
    QHBoxLayout *editorLayout;
    PresetEditor *presetEditor;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 800);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow { background-color: #0D1117; }"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        rootLayout = new QVBoxLayout(centralWidget);
        rootLayout->setSpacing(0);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(0, 0, 0, 0);
        headerBar = new HeaderBar(centralWidget);
        headerBar->setObjectName("headerBar");
        headerBar->setMinimumSize(QSize(0, 56));

        rootLayout->addWidget(headerBar);

        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName("stackedWidget");
        pageMain = new QWidget();
        pageMain->setObjectName("pageMain");
        mainLayout = new QHBoxLayout(pageMain);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainSplitter = new QSplitter(pageMain);
        mainSplitter->setObjectName("mainSplitter");
        mainSplitter->setOrientation(Qt::Horizontal);
        mainSplitter->setStyleSheet(QString::fromUtf8("QSplitter::handle { background-color: #21262D; width: 2px; }"));
        docPreview = new DocumentPreview(mainSplitter);
        docPreview->setObjectName("docPreview");
        docPreview->setMinimumSize(QSize(500, 400));
        mainSplitter->addWidget(docPreview);
        calcPanel = new CalculationPanel(mainSplitter);
        calcPanel->setObjectName("calcPanel");
        calcPanel->setMinimumSize(QSize(340, 400));
        mainSplitter->addWidget(calcPanel);

        mainLayout->addWidget(mainSplitter);

        stackedWidget->addWidget(pageMain);
        pageGallery = new QWidget();
        pageGallery->setObjectName("pageGallery");
        galleryLayout = new QHBoxLayout(pageGallery);
        galleryLayout->setSpacing(0);
        galleryLayout->setObjectName("galleryLayout");
        galleryLayout->setContentsMargins(0, 0, 0, 0);
        presetGallery = new PresetGallery(pageGallery);
        presetGallery->setObjectName("presetGallery");

        galleryLayout->addWidget(presetGallery);

        stackedWidget->addWidget(pageGallery);
        pageEditor = new QWidget();
        pageEditor->setObjectName("pageEditor");
        editorLayout = new QHBoxLayout(pageEditor);
        editorLayout->setSpacing(0);
        editorLayout->setObjectName("editorLayout");
        editorLayout->setContentsMargins(0, 0, 0, 0);
        presetEditor = new PresetEditor(pageEditor);
        presetEditor->setObjectName("presetEditor");

        editorLayout->addWidget(presetEditor);

        stackedWidget->addWidget(pageEditor);

        rootLayout->addWidget(stackedWidget);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        statusBar->setStyleSheet(QString::fromUtf8("QStatusBar { background-color: #161B22; color: #8B949E; }"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "RigPlan Integrator - Convers\303\243o & Integra\303\247\303\243o de Planos de Rigging", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
