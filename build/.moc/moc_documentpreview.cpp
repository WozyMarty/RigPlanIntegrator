/****************************************************************************
** Meta object code from reading C++ file 'documentpreview.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/widgets/documentpreview.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'documentpreview.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15DocumentPreviewE_t {};
} // unnamed namespace

template <> constexpr inline auto DocumentPreview::qt_create_metaobjectdata<qt_meta_tag_ZN15DocumentPreviewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DocumentPreview",
        "openPresetGalleryRequested",
        "",
        "aiAssistantRequested",
        "zoomIn",
        "zoomOut",
        "zoomReset",
        "onPresetSelected",
        "FormatPreset",
        "preset",
        "onImportCrea",
        "onDeleteCrea",
        "onImportSolidworks",
        "onDeleteSolidworks",
        "onAddStyleRequested",
        "onOpenPageOrder",
        "onOpenAiValidator"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'openPresetGalleryRequested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'aiAssistantRequested'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'zoomIn'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'zoomOut'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'zoomReset'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPresetSelected'
        QtMocHelpers::SlotData<void(const FormatPreset &)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Slot 'onImportCrea'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteCrea'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImportSolidworks'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteSolidworks'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddStyleRequested'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenPageOrder'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenAiValidator'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DocumentPreview, qt_meta_tag_ZN15DocumentPreviewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DocumentPreview::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DocumentPreviewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DocumentPreviewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15DocumentPreviewE_t>.metaTypes,
    nullptr
} };

void DocumentPreview::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DocumentPreview *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->openPresetGalleryRequested(); break;
        case 1: _t->aiAssistantRequested(); break;
        case 2: _t->zoomIn(); break;
        case 3: _t->zoomOut(); break;
        case 4: _t->zoomReset(); break;
        case 5: _t->onPresetSelected((*reinterpret_cast<std::add_pointer_t<FormatPreset>>(_a[1]))); break;
        case 6: _t->onImportCrea(); break;
        case 7: _t->onDeleteCrea(); break;
        case 8: _t->onImportSolidworks(); break;
        case 9: _t->onDeleteSolidworks(); break;
        case 10: _t->onAddStyleRequested(); break;
        case 11: _t->onOpenPageOrder(); break;
        case 12: _t->onOpenAiValidator(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DocumentPreview::*)()>(_a, &DocumentPreview::openPresetGalleryRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentPreview::*)()>(_a, &DocumentPreview::aiAssistantRequested, 1))
            return;
    }
}

const QMetaObject *DocumentPreview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocumentPreview::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DocumentPreviewE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DocumentPreview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void DocumentPreview::openPresetGalleryRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DocumentPreview::aiAssistantRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
