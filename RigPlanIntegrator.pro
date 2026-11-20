QT += core gui widgets printsupport

CONFIG += c++17

TEMPLATE = app
TARGET = RigPlanIntegrator

INCLUDEPATH += $$PWD/src

# Encapsulate all compiler artifacts inside build/
DESTDIR = $$PWD/build/bin
OBJECTS_DIR = $$PWD/build/.obj
MOC_DIR = $$PWD/build/.moc
RCC_DIR = $$PWD/build/.rcc
UI_DIR = $$PWD/build/.ui

win32 {
    DESTDIR = $$OUT_PWD/bin
    OBJECTS_DIR = $$OUT_PWD/obj
    MOC_DIR = $$OUT_PWD/moc
    RCC_DIR = $$OUT_PWD/rcc
    UI_DIR = $$OUT_PWD/ui
}

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/utils/datavalidator.cpp \
    src/utils/expressionevaluator.cpp \
    src/models/riggingcalculation.cpp \
    src/models/formatpreset.cpp \
    src/models/calculationpreset.cpp \
    src/models/presetmanager.cpp \
    src/core/calculationengine.cpp \
    src/core/documentgenerator.cpp \
    src/core/pdfexporter.cpp \
    src/core/docxexporter.cpp \
    src/widgets/headerbar.cpp \
    src/widgets/calculationblock.cpp \
    src/widgets/calculationpanel.cpp \
    src/widgets/documentpreview.cpp \
    src/widgets/presetcard.cpp \
    src/widgets/presetgallery.cpp \
    src/widgets/preseteditor.cpp \
    src/widgets/customcalcdialog.cpp \
    src/widgets/calculationpresetdialog.cpp \
    src/models/riggingplandata.cpp \
    src/widgets/riggingplandatadialog.cpp \
    src/models/documentsection.cpp \
    src/utils/aivalidator.cpp \
    src/widgets/pageorderdialog.cpp \
    src/widgets/exportdialog.cpp \
    src/widgets/aivalidatordialog.cpp \
    src/models/craneprofile.cpp \
    src/models/cranemanager.cpp \
    src/widgets/craneselectiondialog.cpp

HEADERS += \
    src/mainwindow.h \
    src/utils/constants.h \
    src/utils/datavalidator.h \
    src/utils/expressionevaluator.h \
    src/models/riggingcalculation.h \
    src/models/formatpreset.h \
    src/models/calculationpreset.h \
    src/models/presetmanager.h \
    src/core/calculationengine.h \
    src/core/documentgenerator.h \
    src/core/pdfexporter.h \
    src/core/docxexporter.h \
    src/widgets/headerbar.h \
    src/widgets/calculationblock.h \
    src/widgets/calculationpanel.h \
    src/widgets/documentpreview.h \
    src/widgets/presetcard.h \
    src/widgets/presetgallery.h \
    src/widgets/preseteditor.h \
    src/widgets/customcalcdialog.h \
    src/widgets/calculationpresetdialog.h \
    src/models/riggingplandata.h \
    src/widgets/riggingplandatadialog.h \
    src/models/documentsection.h \
    src/utils/aivalidator.h \
    src/widgets/pageorderdialog.h \
    src/widgets/exportdialog.h \
    src/widgets/aivalidatordialog.h \
    src/models/craneprofile.h \
    src/models/cranemanager.h \
    src/widgets/craneselectiondialog.h

RESOURCES += \
    resources/resources.qrc

FORMS += \
    src/mainwindow.ui

