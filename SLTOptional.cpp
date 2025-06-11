#include "hexscriptex/SLTOptional.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/PapyrusArgs.h"
#include "skse64/GameForms.h"

// ===== SLTOptional Implementation =====

const char* SLTOptional::SCRIPT_NAME = "SLTOptional";

SLTOptional::SLTOptional()
    : m_type(kType_None)
    , m_intValue(0)
    , m_stringValue()
    , m_formValue(nullptr)
{
}

SLTOptional::SLTOptional(SerializationTag tag)
    : m_type(kType_None)
    , m_intValue(0)
    , m_stringValue()
    , m_formValue(nullptr)
{
}

SLTOptional::~SLTOptional()
{
    ClearValue();
}

bool SLTOptional::Save(SKSESerializationInterface* intfc)
{
    using namespace Serialization;

    if (!WriteData(intfc, &m_type))
        return false;

    switch (m_type)
    {
    case kType_Int:
        return WriteData(intfc, &m_intValue);

    case kType_Float:
        return WriteData(intfc, &m_floatValue);

    case kType_Bool:
        return WriteData(intfc, &m_boolValue);

    case kType_String:
        return WriteData(intfc, &m_stringValue);

    case kType_Form:
    {
        UInt32 formId = m_formValue ? m_formValue->formID : 0;
        return WriteData(intfc, &formId);
    }

    case kType_None:
    default:
        return true;
    }
}

bool SLTOptional::Load(SKSESerializationInterface* intfc, UInt32 version)
{
    using namespace Serialization;

    if (version != kSaveVersion)
        return false;

    ClearValue();

    if (!ReadData(intfc, &m_type))
        return false;

    switch (m_type)
    {
    case kType_Int:
        return ReadData(intfc, &m_intValue);

    case kType_Float:
        return ReadData(intfc, &m_floatValue);

    case kType_Bool:
        return ReadData(intfc, &m_boolValue);

    case kType_String:
        return ReadData(intfc, &m_stringValue);

    case kType_Form:
    {
        UInt32 formId;
        if (!ReadData(intfc, &formId))
            return false;

        if (formId != 0)
        {
            UInt32 resolvedFormId;
            if (intfc->ResolveFormId(formId, &resolvedFormId))
            {
                m_formValue = LookupFormByID(resolvedFormId);
            }
            else
            {
                m_formValue = nullptr;
            }
        }
        else
        {
            m_formValue = nullptr;
        }
        return true;
    }

    case kType_None:
    default:
        return true;
    }
}

void SLTOptional::Reset()
{
    ClearValue();
    m_type = kType_None;
}

void SLTOptional::SetInt(SInt32 value)
{
    ClearValue();
    m_type = kType_Int;
    m_intValue = value;
}

void SLTOptional::SetFloat(float value)
{
    ClearValue();
    m_type = kType_Float;
    m_floatValue = value;
}

void SLTOptional::SetBool(bool value)
{
    ClearValue();
    m_type = kType_Bool;
    m_boolValue = value;
}

void SLTOptional::SetString(const BSFixedString& value)
{
    ClearValue();
    m_type = kType_String;
    m_stringValue = value;
}

void SLTOptional::SetForm(TESForm* value)
{
    ClearValue();
    m_type = kType_Form;
    m_formValue = value;
}

SInt32 SLTOptional::GetInt(SInt32 defaultValue) const
{
    return (m_type == kType_Int) ? m_intValue : defaultValue;
}

float SLTOptional::GetFloat(float defaultValue) const
{
    return (m_type == kType_Float) ? m_floatValue : defaultValue;
}

bool SLTOptional::GetBool(bool defaultValue) const
{
    return (m_type == kType_Bool) ? m_boolValue : defaultValue;
}

BSFixedString SLTOptional::GetString(const BSFixedString& defaultValue) const
{
    return (m_type == kType_String) ? m_stringValue : defaultValue;
}

TESForm* SLTOptional::GetForm(TESForm* defaultValue) const
{
    return (m_type == kType_Form) ? m_formValue : defaultValue;
}

void SLTOptional::ClearValue()
{
    switch (m_type)
    {
    case kType_String:
        m_stringValue = BSFixedString();
        break;

    case kType_Form:
        m_formValue = nullptr;
        break;

    default:
        // Value types don't need special cleanup
        break;
    }
}

// ===== Handle Management =====

SInt32 StoreOptional(SLTOptional* optional, UInt32 stackId)
{
    if (!optional)
        return 0;

    return SKSEObjectStorageInstance().StoreObject(optional, stackId);
}

SLTOptional* GetOptional(SInt32 handle)
{
    if (handle == 0)
        return nullptr;

    return SKSEObjectStorageInstance().AccessObject<SLTOptional>(handle);
}

SLTOptional* TakeOptional(SInt32 handle)
{
    if (handle == 0)
        return nullptr;

    return SKSEObjectStorageInstance().TakeObject<SLTOptional>(handle);
}

// ===== Papyrus Interface =====

namespace papyrusOptional
{
    SLTOptional* CreateInt(StaticFunctionTag* sft, SInt32 value)
    {
        SLTOptional* optional = new SLTOptional();
        optional->SetInt(value);
        return optional;
    }

    SLTOptional* CreateFloat(StaticFunctionTag*, float value)
    {
        SLTOptional* optional = new SLTOptional();
        optional->SetFloat(value);
        return optional;
    }

    SLTOptional* CreateBool(StaticFunctionTag*, bool value)
    {
        SLTOptional* optional = new SLTOptional();
        optional->SetBool(value);
        return optional;
    }

    SLTOptional* CreateString(StaticFunctionTag*, BSFixedString value)
    {
        SLTOptional* optional = new SLTOptional();
        optional->SetString(value);
        return optional;
    }

    SLTOptional* CreateForm(StaticFunctionTag*, TESForm* value)
    {
        SLTOptional* optional = new SLTOptional();
        optional->SetForm(value);
        return optional;
    }

    SLTOptional* CreateEmpty(StaticFunctionTag*)
    {
        return new SLTOptional();
    }

    bool HasValue(SLTOptional* optional)
    {
        return optional ? optional->HasValue() : false;
    }

    UInt32 GetType(SLTOptional* optional)
    {
        return optional ? (UInt32)optional->GetType() : (UInt32)SLTOptional::kType_None;
    }

    void Reset(SLTOptional* optional)
    {
        if (optional)
            optional->Reset();
    }

    SInt32 GetInt(SLTOptional* optional, SInt32 defaultValue)
    {
        return optional ? optional->GetInt(defaultValue) : defaultValue;
    }

    float GetFloat(SLTOptional* optional, float defaultValue)
    {
        return optional ? optional->GetFloat(defaultValue) : defaultValue;
    }

    bool GetBool(SLTOptional* optional, bool defaultValue)
    {
        return optional ? optional->GetBool(defaultValue) : defaultValue;
    }

    BSFixedString GetString(SLTOptional* optional, BSFixedString defaultValue)
    {
        return optional ? optional->GetString(defaultValue) : defaultValue;
    }

    TESForm* GetForm(SLTOptional* optional, TESForm* defaultValue)
    {
        return optional ? optional->GetForm(defaultValue) : defaultValue;
    }

    void SetInt(SLTOptional* optional, SInt32 value)
    {
        if (optional)
            optional->SetInt(value);
    }

    void SetFloat(SLTOptional* optional, float value)
    {
        if (optional)
            optional->SetFloat(value);
    }

    void SetBool(SLTOptional* optional, bool value)
    {
        if (optional)
            optional->SetBool(value);
    }

    void SetString(SLTOptional* optional, BSFixedString value)
    {
        if (optional)
            optional->SetString(value);
    }

    void SetForm(SLTOptional* optional, TESForm* value)
    {
        if (optional)
            optional->SetForm(value);
    }

    void RegisterFuncs(VMClassRegistry* registry)
    {
        // Factory functions
        registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SLTOptional*, SInt32>("CreateInt", "SLTOptional", CreateInt, registry));

        registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SLTOptional*, float>("CreateFloat", "SLTOptional", CreateFloat, registry));

        registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SLTOptional*, bool>("CreateBool", "SLTOptional", CreateBool, registry));

        registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SLTOptional*, BSFixedString>("CreateString", "SLTOptional", CreateString, registry));

        registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SLTOptional*, TESForm*>("CreateForm", "SLTOptional", CreateForm, registry));

        registry->RegisterFunction(
            new NativeFunction0<StaticFunctionTag, SLTOptional*>("CreateEmpty", "SLTOptional", CreateEmpty, registry));

        // Core functions
        registry->RegisterFunction(
            new NativeFunction0<SLTOptional, bool>("HasValue", "SLTOptional", HasValue, registry));

        registry->RegisterFunction(
            new NativeFunction0<SLTOptional, UInt32>("GetType", "SLTOptional", GetType, registry));

        registry->RegisterFunction(
            new NativeFunction0<SLTOptional, void>("Reset", "SLTOptional", Reset, registry));

        // Getters
        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, SInt32, SInt32>("GetInt", "SLTOptional", GetInt, registry));

        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, float, float>("GetFloat", "SLTOptional", GetFloat, registry));

        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, bool, bool>("GetBool", "SLTOptional", GetBool, registry));

        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, BSFixedString, BSFixedString>("GetString", "SLTOptional", GetString, registry));

        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, TESForm*, TESForm*>("GetForm", "SLTOptional", GetForm, registry));

        // Setters
        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, void, SInt32>("SetInt", "SLTOptional", SetInt, registry));

        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, void, float>("SetFloat", "SLTOptional", SetFloat, registry));

        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, void, bool>("SetBool", "SLTOptional", SetBool, registry));

        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, void, BSFixedString>("SetString", "SLTOptional", SetString, registry));

        registry->RegisterFunction(
            new NativeFunction1<SLTOptional, void, TESForm*>("SetForm", "SLTOptional", SetForm, registry));

        // Mark all functions as NoWait for performance
        registry->SetFunctionFlags("SLTOptional", "HasValue", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "GetType", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "Reset", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "GetInt", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "GetFloat", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "GetBool", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "GetString", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "GetForm", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "SetInt", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "SetFloat", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "SetBool", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "SetString", VMClassRegistry::kFunctionFlag_NoWait);
        registry->SetFunctionFlags("SLTOptional", "SetForm", VMClassRegistry::kFunctionFlag_NoWait);
    }
}