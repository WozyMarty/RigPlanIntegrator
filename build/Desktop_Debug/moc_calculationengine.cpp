/****************************************************************************
** Meta object code from reading C++ file 'calculationengine.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/core/calculationengine.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calculationengine.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17CalculationEngineE_t {};
} // unnamed namespace

template <> constexpr inline auto CalculationEngine::qt_create_metaobjectdata<qt_meta_tag_ZN17CalculationEngineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CalculationEngine",
        "calculationUpdated",
        "",
        "id",
        "result",
        "progressChanged",
        "completed",
        "total",
        "allCalculationsReady",
        "validationError",
        "message",
        "activeCalculationsChanged",
        "calculationPresetsChanged",
        "presetApplied",
        "CalculationPreset",
        "preset",
        "customCalculationAdded",
        "RiggingCalculation*",
        "calc",
        "customCalculationRemoved",
        "onResultChanged",
        "calcId"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'calculationUpdated'
        QtMocHelpers::SignalData<void(int, double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Double, 4 },
        }}),
        // Signal 'progressChanged'
        QtMocHelpers::SignalData<void(int, int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { QMetaType::Int, 7 },
        }}),
        // Signal 'allCalculationsReady'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'validationError'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'activeCalculationsChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'calculationPresetsChanged'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'presetApplied'
        QtMocHelpers::SignalData<void(const CalculationPreset &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Signal 'customCalculationAdded'
        QtMocHelpers::SignalData<void(RiggingCalculation *)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 18 },
        }}),
        // Signal 'customCalculationRemoved'
        QtMocHelpers::SignalData<void(int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'onResultChanged'
        QtMocHelpers::SlotData<void(int, double)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 21 }, { QMetaType::Double, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CalculationEngine, qt_meta_tag_ZN17CalculationEngineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CalculationEngine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CalculationEngineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CalculationEngineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17CalculationEngineE_t>.metaTypes,
    nullptr
} };

void CalculationEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CalculationEngine *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->calculationUpdated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 1: _t->progressChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->allCalculationsReady(); break;
        case 3: _t->validationError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->activeCalculationsChanged(); break;
        case 5: _t->calculationPresetsChanged(); break;
        case 6: _t->presetApplied((*reinterpret_cast<std::add_pointer_t<CalculationPreset>>(_a[1]))); break;
        case 7: _t->customCalculationAdded((*reinterpret_cast<std::add_pointer_t<RiggingCalculation*>>(_a[1]))); break;
        case 8: _t->customCalculationRemoved((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onResultChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< RiggingCalculation* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)(int , double )>(_a, &CalculationEngine::calculationUpdated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)(int , int )>(_a, &CalculationEngine::progressChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)()>(_a, &CalculationEngine::allCalculationsReady, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)(const QString & )>(_a, &CalculationEngine::validationError, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)()>(_a, &CalculationEngine::activeCalculationsChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)()>(_a, &CalculationEngine::calculationPresetsChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)(const CalculationPreset & )>(_a, &CalculationEngine::presetApplied, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)(RiggingCalculation * )>(_a, &CalculationEngine::customCalculationAdded, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalculationEngine::*)(int )>(_a, &CalculationEngine::customCalculationRemoved, 8))
            return;
    }
}

const QMetaObject *CalculationEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalculationEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CalculationEngineE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CalculationEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void CalculationEngine::calculationUpdated(int _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void CalculationEngine::progressChanged(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void CalculationEngine::allCalculationsReady()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CalculationEngine::validationError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void CalculationEngine::activeCalculationsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CalculationEngine::calculationPresetsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CalculationEngine::presetApplied(const CalculationPreset & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void CalculationEngine::customCalculationAdded(RiggingCalculation * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void CalculationEngine::customCalculationRemoved(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}
QT_WARNING_POP
