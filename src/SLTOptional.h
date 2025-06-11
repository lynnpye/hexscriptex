#pragma once
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/PapyrusObjects.h"
#include "skse64/GameTypes.h"
#include "skse64/GameForms.h"
#include "skse64/Serialization.h"
#include "common/common/ITypes.h"

class VMClassRegistry;

// SKSE Optional class - similar to std::optional but for Papyrus types
class SLTOptional : public ISKSEObject
{
public:
    enum OptionalType : UInt32
    {
        kType_None = 0,
        kType_Int,
        kType_Float,
        kType_Bool,
        kType_String,
        kType_Form
    };

    enum { kTypeID = 0x1000 };

    enum { kSaveVersion = 1 };

    // Standard constructor
    SLTOptional();

    // Serialization constructor
    SLTOptional(SerializationTag tag);

    virtual ~SLTOptional();

    static const char* SCRIPT_NAME;

    // ISKSEObject interface
    virtual const char* ClassName() const override { return SCRIPT_NAME; }
    virtual UInt32 ClassVersion() const override { return kSaveVersion; }
    virtual bool Save(SKSESerializationInterface* intfc) override;
    virtual bool Load(SKSESerializationInterface* intfc, UInt32 version) override;

    // Core Optional functionality
    bool HasValue() const { return m_type != kType_None; }
    OptionalType GetType() const { return m_type; }
    void Reset();

    // Type-specific setters
    void SetInt(SInt32 value);
    void SetFloat(float value);
    void SetBool(bool value);
    void SetString(const BSFixedString& value);
    void SetForm(TESForm* value);

    // Type-specific getters (with default values)
    SInt32 GetInt(SInt32 defaultValue = 0) const;
    float GetFloat(float defaultValue = 0.0f) const;
    bool GetBool(bool defaultValue = false) const;
    BSFixedString GetString(const BSFixedString& defaultValue = BSFixedString()) const;
    TESForm* GetForm(TESForm* defaultValue = nullptr) const;

    // Unsafe getters (will return garbage if wrong type)
    SInt32 GetIntUnsafe() const { return m_intValue; }
    float GetFloatUnsafe() const { return m_floatValue; }
    bool GetBoolUnsafe() const { return m_boolValue; }
    const BSFixedString& GetStringUnsafe() const { return m_stringValue; }
    TESForm* GetFormUnsafe() const { return m_formValue; }

private:
    void ClearValue();

    OptionalType m_type;

    // Union for value types
    union {
        SInt32 m_intValue;
        float m_floatValue;
        bool m_boolValue;
    };

    // Reference types stored separately
    BSFixedString m_stringValue;
    TESForm* m_formValue;
};

// Factory for creating SKSEOptional objects
class SLTOptionalFactory : public ConcreteSKSEObjectFactory<SLTOptional>
{
public:
    virtual const char* ClassName() const override { return SLTOptional::SCRIPT_NAME; }
};

// Papyrus interface functions
namespace papyrusOptional
{
    void RegisterFuncs(VMClassRegistry* registry);

    // Factory functions
    SLTOptional* CreateInt(StaticFunctionTag*, SInt32 value);
    SLTOptional* CreateFloat(StaticFunctionTag*, float value);
    SLTOptional* CreateBool(StaticFunctionTag*, bool value);
    SLTOptional* CreateString(StaticFunctionTag*, BSFixedString value);
    SLTOptional* CreateForm(StaticFunctionTag*, TESForm* value);
    SLTOptional* CreateEmpty(StaticFunctionTag*);

    // Core functions
    bool HasValue(SLTOptional* optional);
    UInt32 GetType(SLTOptional* optional);
    void Reset(SLTOptional* optional);

    // Getters with default values
    SInt32 GetInt(SLTOptional* optional, SInt32 defaultValue);
    float GetFloat(SLTOptional* optional, float defaultValue);
    bool GetBool(SLTOptional* optional, bool defaultValue);
    BSFixedString GetString(SLTOptional* optional, BSFixedString defaultValue);
    TESForm* GetForm(SLTOptional* optional, TESForm* defaultValue);

    // Setters
    void SetInt(SLTOptional* optional, SInt32 value);
    void SetFloat(SLTOptional* optional, float value);
    void SetBool(SLTOptional* optional, bool value);
    void SetString(SLTOptional* optional, BSFixedString value);
    void SetForm(SLTOptional* optional, TESForm* value);
}

// Handle management for Papyrus
SInt32 StoreOptional(SLTOptional* optional, UInt32 stackId);
SLTOptional* GetOptional(SInt32 handle);
SLTOptional* TakeOptional(SInt32 handle);