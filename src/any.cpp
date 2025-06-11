#include <skse64/PapyrusObjects.h>
#ifdef asdfasdfasdf
class Any : ISKSEObject {
public:
	Any(SerializationTag) = default;

	// Inherited via ISKSEObject
	const char* ClassName() const override
	{
		return "Any";
	}
	UInt32 ClassVersion() const override
	{
		return 1;
	}
	bool Save(SKSESerializationInterface* intfc) override
	{
		return false;
	}
	bool Load(SKSESerializationInterface* intfc, UInt32 version) override
	{
		return false;
	}
};
#endif