#include "common/IDebugLog.h"  // IDebugLog
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // SKSEInterface, PluginInfo

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "version.h"  // VERSION_VERSTRING, VERSION_MAJOR

#include "SLTOptional.h"
#include "skse64/PapyrusVM.h"

// Add this global
SKSEPapyrusInterface* g_papyrus = nullptr;


extern "C" {
	void RegisterSKSEOptional()
	{
		// Register the class with the SKSE object registry
		SKSEObjectRegistryInstance().RegisterClass<SLTOptional>();

		_MESSAGE("SKSEOptional class registered successfully");
	}

	bool RegisterPapyrusOptional(VMClassRegistry* registry)
	{
		// Register Papyrus functions
		papyrusOptional::RegisterFuncs(registry);

		_MESSAGE("SKSEOptional Papyrus functions registered successfully");

		return true;
	}

	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\hexscriptex.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("hexscriptex v%s", MYFP_VERSION_VERSTRING);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "hexscriptex";
		a_info->version = MYFP_VERSION_MAJOR;

		if (a_skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		}

		_MESSAGE("Runtime version: %08X", a_skse->runtimeVersion);

		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		_MESSAGE("[MESSAGE] hexscriptex loaded");

		// Get the papyrus interface
		g_papyrus = (SKSEPapyrusInterface*)a_skse->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus) {
			_ERROR("Failed to get papyrus interface");
			return false;
		}

		// Register the SLTOptional class with SKSE object registry
		RegisterSKSEOptional();

		// Register Papyrus functions
		g_papyrus->Register(RegisterPapyrusOptional);

		return true;
	}
};
