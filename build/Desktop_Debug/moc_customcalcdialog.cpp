/****************************************************************************
** Meta object code from reading C++ file 'customcalcdialog.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/widgets/customcalcdialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'customcalcdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN16CustomCalcDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto CustomCalcDialog::qt_create_metaobjectdata<qt_meta_tag_ZN16CustomCalcDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CustomCalcDialog",
        "onFormulaChanged",
        "",
        "text",
        "detectSymbolsFromFormula",
        "addParameterRow",
        "symbol",
        "name",
        "unit",
        "defaultValue",
        "removeParameterRow",
        "index",
        "insertSymbolIntoFormula",
        "updateLiveTest",
        "onSaveClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onFormulaChanged'
        QtMocHelpers::SlotData<void(const QString &)>(1, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'detectSymbolsFromFormula'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'addParameterRow'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &, double)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 7 }, { QMetaType::QString, 8 }, { QMetaType::Double, 9 },
        }}),
        // Slot 'addParameterRow'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &)>(5, 2, QMC::AccessPrivate | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 7 }, { QMetaType::QString, 8 },
        }}),
        // Slot 'addParameterRow'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(5, 2, QMC::AccessPrivate | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'addParameterRow'
        QtMocHelpers::SlotData<void(const QString &)>(5, 2, QMC::AccessPrivate | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Slot 'addParameterRow'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate | QMC::MethodCloned, QMetaType::Void),
        // Slot 'removeParameterRow'
        QtMocHelpers::SlotData<void(int)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 11 },
        }}),
        // Slot 'insertSymbolIntoFormula'
        QtMocHelpers::SlotData<void(const QString &)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Slot 'updateLiveTest'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveClicked'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CustomCalcDialog, qt_meta_tag_ZN16CustomCalcDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CustomCalcDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16CustomCalcDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16CustomCalcDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16CustomCalcDialogE_t>.metaTypes,
    nullptr
} };

void CustomCalcDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CustomCalcDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onFormulaChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->detectSymbolsFromFormula(); break;
        case 2: _t->addParameterRow((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[4]))); break;
        case 3: _t->addParameterRow((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->addParameterRow((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->addParameterRow((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->addParameterRow(); break;
        case 7: _t->removeParameterRow((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->insertSymbolIntoFormula((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->updateLiveTest(); break;
        case 10: _t->onSaveClicked(); break;
        default: ;
        }
    }
}

const QMetaObject *CustomCalcDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomCalcDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16CustomCalcDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CustomCalcDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
