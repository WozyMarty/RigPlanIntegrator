/****************************************************************************
** Meta object code from reading C++ file 'pageorderdialog.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/widgets/pageorderdialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pageorderdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.12.0. It"
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
struct qt_meta_tag_ZN15PageOrderDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto PageOrderDialog::qt_create_metaobjectdata<qt_meta_tag_ZN15PageOrderDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PageOrderDialog",
        "sectionsChanged",
        "",
        "QList<DocumentSection>",
        "sections",
        "requestImportCrea",
        "requestImportSolidworks",
        "moveUp",
        "moveDown",
        "addStylePreset",
        "removeSelected",
        "onImportCreaClicked",
        "onImportSolidworksClicked",
        "updateTable"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sectionsChanged'
        QtMocHelpers::SignalData<void(const QVector<DocumentSection> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'requestImportCrea'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestImportSolidworks'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'moveUp'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'moveDown'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'addStylePreset'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'removeSelected'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImportCreaClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImportSolidworksClicked'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateTable'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PageOrderDialog, qt_meta_tag_ZN15PageOrderDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PageOrderDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PageOrderDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PageOrderDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15PageOrderDialogE_t>.metaTypes,
    nullptr
} };

void PageOrderDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PageOrderDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sectionsChanged((*reinterpret_cast<std::add_pointer_t<QList<DocumentSection>>>(_a[1]))); break;
        case 1: _t->requestImportCrea(); break;
        case 2: _t->requestImportSolidworks(); break;
        case 3: _t->moveUp(); break;
        case 4: _t->moveDown(); break;
        case 5: _t->addStylePreset(); break;
        case 6: _t->removeSelected(); break;
        case 7: _t->onImportCreaClicked(); break;
        case 8: _t->onImportSolidworksClicked(); break;
        case 9: _t->updateTable(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PageOrderDialog::*)(const QVector<DocumentSection> & )>(_a, &PageOrderDialog::sectionsChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PageOrderDialog::*)()>(_a, &PageOrderDialog::requestImportCrea, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (PageOrderDialog::*)()>(_a, &PageOrderDialog::requestImportSolidworks, 2))
            return;
    }
}

const QMetaObject *PageOrderDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PageOrderDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PageOrderDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PageOrderDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void PageOrderDialog::sectionsChanged(const QVector<DocumentSection> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void PageOrderDialog::requestImportCrea()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PageOrderDialog::requestImportSolidworks()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
