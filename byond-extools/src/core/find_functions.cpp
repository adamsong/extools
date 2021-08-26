#include "find_functions.h"

#ifdef _WIN32
#define BYONDCORE "byondcore.dll"
#else
#define BYONDCORE "libbyond.so"
#endif

#define FIND_OR_DIE(name, sig)                                      \
	name = (name##Ptr)Pocket::Sigscan::FindPattern(BYONDCORE, sig); \
	if (!name)                                                      \
	{                                                               \
		Core::Alert("Failed to locate " #name);                     \
		failed = true;                                              \
	}
#ifdef _WIN32
#define IMPORT_OR_DIE(name, sig)                                        \
	name = (name##Ptr)GetProcAddress(GetModuleHandleA(BYONDCORE), sig); \
	if (!name)                                                          \
	{                                                                   \
		Core::Alert("Failed to locate " #name " via " #sig);            \
		return false;                                                   \
	}
#else
#define IMPORT_OR_DIE(name, sig)                             \
	name = (name##Ptr)dlsym(dlopen(BYONDCORE, 0), sig);      \
	if (!name)                                               \
	{                                                        \
		Core::Alert("Failed to locate " #name " via " #sig); \
		return false;                                        \
	}
#endif

bool Core::verify_compat()
{
#ifdef _WIN32
	IMPORT_OR_DIE(GetByondVersion, "?GetByondVersion@ByondLib@@QAEJXZ");
	IMPORT_OR_DIE(GetByondBuild, "?GetByondBuild@ByondLib@@QAEJXZ");
	IMPORT_OR_DIE(StdDefDM, "?StdDefDM@DungBuilder@@QAEPADXZ");
#else
	IMPORT_OR_DIE(GetByondVersion, "_ZN8ByondLib15GetByondVersionEv");
	IMPORT_OR_DIE(GetByondBuild, "_ZN8ByondLib13GetByondBuildEv");
	IMPORT_OR_DIE(StdDefDM, "_ZN11DungBuilder8StdDefDMEv");
#endif
	ByondVersion = GetByondVersion();
	ByondBuild = GetByondBuild();
	if (ByondVersion < MIN_COMPATIBLE_MAJOR || ByondBuild < MIN_COMPATIBLE_MINOR)
	{
		Core::Alert("Incompatible BYOND version, must be at least " + std::to_string(MIN_COMPATIBLE_MAJOR) + "." + std::to_string(MIN_COMPATIBLE_MINOR) + " (Current: " + std::to_string(ByondVersion) + "." + std::to_string(ByondBuild) + ")");
		return false;
	}
	return true;
}

bool Core::find_functions()
{
	bool failed = false;
#ifdef _WIN32
	FIND_OR_DIE(Suspend, "55 8B EC 53 56 57 8B 7D 08 57 E8 ?? ?? ?? ?? 8B 1F 8B F0 8A 4F 63 83 C4 04 8B 56 18 88 4A 63 8B 4B 20 89 4E 20 8B 43 24 89 46 24 8B 45 0C C6 47 63 00 C7 43 ?? ?? ?? ?? ?? C7 43 ?? ?? ?? ?? ?? 8B 4E 18 89 41 04 F6 43 04 10");
	FIND_OR_DIE(StartTiming, "55 8B EC 56 8B 75 ?? 56 80 4E ?? ??");
	FIND_OR_DIE(SetVariable, "55 8B EC 8B 4D 08 0F B6 C1 48 57 8B 7D 10 83 F8 53 0F ?? ?? ?? ?? ?? 0F B6 80 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? FF 75 18 FF 75 14 57 FF 75 0C E8 ?? ?? ?? ?? 83 C4 10 5F 5D C3");
	FIND_OR_DIE(GetProcArrayEntry, "55 8B EC 8B 45 08 3B 05 ?? ?? ?? ?? 72 04 33 C0 5D C3 8D 0C C0 A1 ?? ?? ?? ?? 8D 04 88 5D C3");
	FIND_OR_DIE(GetStringTableEntry, "55 8B EC 8B 4D 08 3B 0D ?? ?? ?? ?? 73 10 A1");
	FIND_OR_DIE(GetProfileInfo, "55 8B EC A1 ?? ?? ?? ?? 56 8B 75 08 3B F0 73 30 8B ?? ?? ?? ?? ?? 85 C9 75 36 8D 04 80 C1 E0 03 50 FF 15 ?? ?? ?? ?? 83 C4 04 A3 ?? ?? ?? ?? 85 C0 75 12 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 33 C0 5E 5D C3 E8 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? 8D 04 B6 8D 04 C1 5E 5D C3");
	FIND_OR_DIE(CallGlobalProc, "55 8B EC 81 EC ?? ?? ?? ?? A1 ?? ?? ?? ?? 33 C5 89 45 ?? 8B 55 ?? 8B 45 ??");
	FIND_OR_DIE(CrashProc, "55 8B EC 6A ?? 68 ?? ?? ?? ?? 64 A1 ?? ?? ?? ?? 50 A1 ?? ?? ?? ?? 33 C5 50 8D 45 ?? 64 A3 ?? ?? ?? ?? A1 ?? ?? ?? ?? A8 ?? 75 ?? 83 C8 ?? A3 ?? ?? ?? ?? B9 ?? ?? ?? ?? C7 45 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 ?? C7 45 ?? ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 45 ??")
	FIND_OR_DIE(GetVariable, "55 8B EC 8B 4D ?? 0F B6 C1 48 83 F8 ?? 0F 87 ?? ?? ?? ?? 0F B6 80 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? FF 75 ?? FF 75 ?? E8 ?? ?? ?? ??");
	FIND_OR_DIE(ProcCleanup, "55 8B EC 53 8B ?? ?? ?? ?? ?? 56 8B 75 08 57 8B 7E 44 85 FF 74 2A 8B 47 1C 89 46 44 FF 77 08 FF 37 E8 ?? ?? ?? ?? 8D 47 10 50 E8 ?? ?? ?? ?? FF 37 FF D3 57 FF D3 8B 7E 44 83 C4 14 85 FF 75 D6 FF 76 50 FF 76 48 E8");
	FIND_OR_DIE(CreateContext, "55 8B EC 83 EC 64 56 57 8B 7D 08 8B 4F 18 85 C9 0F 84 ?? ?? ?? ?? A1 ?? ?? ?? ?? 89 41 04 8B 4F 18 89 ?? ?? ?? ?? ?? 80 79 69 00");
	FIND_OR_DIE(GetTypeById, "55 8B EC 8B 45 08 3B 05 ?? ?? ?? ?? 72 04 33 C0 5D C3 6B C0 64 03 05 ?? ?? ?? ?? 5D C3");
	FIND_OR_DIE(MobTableIndexToGlobalTableIndex, "55 8B EC 8B 45 08 3B 05 ?? ?? ?? ?? 72 04 33 C0 5D C3 C1 E0 04 03 05 ?? ?? ?? ?? 5D C3");
	FIND_OR_DIE(Runtime, "55 8B EC 6A FF 68 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 50 83 EC 40 53 56 57 A1 ?? ?? ?? ?? 33 C5 50 8D 45 F4 ?? ?? ?? ?? ?? ?? 89 65 F0 A1");
	FIND_OR_DIE(GetTurf, "55 8B EC 8B 45 08 0F BF ?? ?? ?? ?? ?? 53 56 0F B7 F0 57 3B F2 7D 3A 8B 45 0C 0F BF ?? ?? ?? ?? ?? 0F B7 D8 3B DF 7D 29");
	FIND_OR_DIE(AppendToContainer, "55 8B EC 8B 4D 08 0F B6 C1 48 56 83 F8 53 0F 87 ?? ?? ?? ?? 0F B6 ?? ?? ?? ?? ?? FF 24 ?? ?? ?? ?? ?? FF 75 0C E8 ?? ?? ?? ?? 8B F0 83 C4 04 85 F6 0F 84 ?? ?? ?? ?? 8B 46 0C 40 50 56 E8 ?? ?? ?? ?? 8B 56 0C 83 C4 08 85 D2");
	FIND_OR_DIE(RemoveFromContainer, "55 8B EC 8B 4D 08 83 EC 0C 0F B6 C1 48 53 83 F8 53 0F 87 ?? ?? ?? ?? 0F B6 ?? ?? ?? ?? ?? 8B 55 10 FF 24 ?? ?? ?? ?? ?? 6A 0F FF 75 0C 51 E8 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 10 85 C0 0F 84 ?? ?? ?? ?? 8B 48 0C 8B 10 85 C9 0F 84 ?? ?? ?? ?? 8B 45 14 8B 5D 10");
	FIND_OR_DIE(GetAssocElement, "55 8B EC 51 8B 4D 08 C6 45 FF 00 80 F9 05 76 11 80 F9 21 74 10 80 F9 0D 74 0B 80 F9 0E 75 65 EB 04 84 C9 74 5F 6A 00 8D 45 FF 50 FF 75 0C 51 6A 00 6A 7B");
	FIND_OR_DIE(SetAssocElement1, "55 8B EC 83 EC 14 8B 4D 08 C6 45 FF 00 80 F9 05 76 15 80 F9 21 74 14 80 F9 0D 74 0F 80 F9 0E 0F 85 ?? ?? ?? ?? EB 04 84 C9 74 7A 6A 00 8D 45 FF 50 FF 75 0C 51 6A 00 6A 7C");
	FIND_OR_DIE(CreateList, "55 8B EC 8B ?? ?? ?? ?? ?? 56 85 C9 74 1B A1 ?? ?? ?? ?? 49 89 ?? ?? ?? ?? ?? 8B 34 88 81 FE ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? 8B F1 81 F9 ?? ?? ?? ?? 75 1B 51 68 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 0C B8 ?? ?? ?? ?? 5E 5D C3");
	FIND_OR_DIE(Length, "55 8B EC 8B 4D 08 83 EC 18 0F B6 C1 48 53 56 57 83 F8 53 0F 87 ?? ?? ?? ?? 0F B6 ?? ?? ?? ?? ?? FF 24 ?? ?? ?? ?? ?? FF 75 0C");
	if (ByondBuild <= 1506)
	{
		FIND_OR_DIE(IsInContainer, "55 8B EC 8B 45 10 83 EC 38 3C 0F 75 4D FF 75 14 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 3B 83 78 04 00 74 35 6A 0F FF 75 14 FF 75 10 E8");
	}
	else
	{
		FIND_OR_DIE(IsInContainer, "55 8B EC 8B 4D 10 83 EC 38 0F B6 C1 56 57 83 F8 53 0F 87 ?? ?? ?? ?? 0F B6 ?? ?? ?? ?? ?? FF 24 ?? ?? ?? ?? ?? 53 6A 0F FF 75 14 51 E8");
	}
	if (ByondVersion > 514)
		{
			if(ByondBuild > 1561){
				FIND_OR_DIE(ToString, "55 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 ?? ?? ?? ?? 50 83 EC 18 53 56 57 A1 ?? ?? ?? ?? 33 C5 50 8D 45 ?? 64 A3 ?? ?? ?? ?? 8B 4D ?? 0F B6 C1");
			}
		}
		else
		{
			FIND_OR_DIE(ToString, "55 8B EC 6A FF 68 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 50 83 EC 10 53 56 57 A1 ?? ?? ?? ?? 33 C5 50 8D 45 F4 ?? ?? ?? ?? ?? ?? 8B 5D 08 0F B6 C3 48 83 F8 53");
		}
	FIND_OR_DIE(TopicFloodCheck, "55 8B EC 6A FF 68 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 50 83 EC 2C 53 56 57 A1 ?? ?? ?? ?? 33 C5 50 8D 45 F4 ?? ?? ?? ?? ?? ?? FF 75 08 E8 ?? ?? ?? ?? 83 C4 04 85 C0");
	FIND_OR_DIE(PrintToDD, "55 8B EC 56 8B ?? ?? ?? ?? ?? 85 F6 74 30 8B 46 0C 8B 40 04 85 C0 74 1C 50 FF 75 08 FF 15");
	FIND_OR_DIE(DisconnectClient1, "55 8B EC 6A FF 68 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 50 81 EC ?? ?? ?? ?? A1 ?? ?? ?? ?? 33 C5 89 45 F0 56 50 8D 45 F4 ?? ?? ?? ?? ?? ?? 6A 00 8D");
	FIND_OR_DIE(DisconnectClient2, "55 8B EC 6A FF 68 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 50 51 53 56 57 A1 ?? ?? ?? ?? 33 C5 50 8D 45 F4 ?? ?? ?? ?? ?? ?? 8B 5D 08 ?? ?? ?? ?? ?? ?? ?? 0F 83");
	FIND_OR_DIE(GetSocketHandleStruct, "55 8B EC FF 75 08 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 19 0F B7 40 02 8B ?? ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 85 C0 74 05 83 C0 74 5D C3 33 C0 5D C3");
	FIND_OR_DIE(CallProcByName, "55 8B EC 83 EC 0C 53 8B 5D 10 8D 45 FF 56 8B 75 14 57 6A 01 50 FF 75 1C C6 45 FF 00 FF 75 18 6A 00 56 53");
	FIND_OR_DIE(SendMaps, "55 8B EC 6A FF 68 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 50 81 EC ?? ?? ?? ?? A1 ?? ?? ?? ?? 33 C5 89 45 F0 53 56 57 50 8D 45 F4 ?? ?? ?? ?? ?? ?? A0 ?? ?? ?? ?? 04 01 75 05 E8 ?? ?? ?? ?? E8");
	FIND_OR_DIE(GetGlobalByName, "55 8B EC 51 FF 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 85 C0 75 26 FF 75 08 E8 ?? ?? ?? ?? FF 30 68 ?? ?? ?? ?? E8");
	FIND_OR_DIE(DelDatum, "55 8b ec 8b 4d 08 3b 0d ?? ?? ?? ?? 73 55 a1 ?? ?? ?? ?? 8b 04 88 85 c0 74 49 8b 50 10 81 fa 00 00 00 70");
	//FIND_OR_DIE(GetMemoryStats, "55 8B EC 6A FF 68 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 50 83 EC 24 53 56 57 A1 ?? ?? ?? ?? 33 C5 50 8D 45 F4 ?? ?? ?? ?? ?? ?? 8D 4D D0 C7 45 ?? ?? ?? ?? ?? E8");
	FIND_OR_DIE(GetRBTreeMemoryUsage, "55 8B EC 56 8B 75 08 57 33 FF 85 F6 74 19 8B FF");
	FIND_OR_DIE(InitializeListFromContext, "55 8B EC 56 FF 75 08 E8 ?? ?? ?? ?? 8B F0 83 C4 04 85 F6 74 2A 8B ?? ?? ?? ?? ?? 0F B7 4E 0C 66 29 4A 42 8B 46 0C");
	FIND_OR_DIE(DestroyList, "55 8B EC A1 ?? ?? ?? ?? 56 57 8B 7D 08 8B 34 B8 85 F6 0F 84 ?? ?? ?? ?? 83 7E 10 00 74 1A 6A 0F 57");
	FIND_OR_DIE(DestroyDatum, "55 8B EC 8B 55 10 0F B6 C2 83 C0 FE 83 F8 1F 0F 87 ?? ?? ?? ?? 0F B6 ?? ?? ?? ?? ?? FF 24 ?? ?? ?? ?? ?? 8B 4D 14 3B ?? ?? ?? ?? ?? 0F 83 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 04 88 85 C0 0F 84");
	if (ByondVersion > 513)
	{
		FIND_OR_DIE(GetStringTableIndexUTF8, "55 8B EC 8B 45 ?? 83 EC ?? 53 56 8B 35 ?? ?? ?? ??");
	}
	if (failed) return false;
	int offset = *(int *)((char *)TopicFloodCheck + 0x2C);
	GetBSocket = (GetBSocketPtr)((char *)TopicFloodCheck + 0x2C + offset + 4); //done this way because pattern scanning would result in a different function

	name_table_id_ptr = *(unsigned int **)((char *)GetGlobalByName + 6);
	name_table = **(unsigned int ***)((char *)GetGlobalByName + 0x6D);

	offset = *(int *)((char *)GetGlobalByName + 11);
	GetTableHolderThingyById = (GetTableHolderThingyByIdPtr)((char *)GetGlobalByName + 11 + offset + 4);

	char *x_ref_count_call = (char *)Pocket::Sigscan::FindPattern(BYONDCORE, "3D ?? ?? ?? ?? 74 14 50 E8 ?? ?? ?? ?? FF 75 0C FF 75 08 E8", 20);
	DecRefCount = (DecRefCountPtr)(x_ref_count_call + *(int *)x_ref_count_call + 4); //x_ref_count_call points to the relative offset to DecRefCount from the call site
	x_ref_count_call = (char *)Pocket::Sigscan::FindPattern(BYONDCORE, "FF 75 10 E8 ?? ?? ?? ?? FF 75 0C 8B F8 FF 75 08 E8 ?? ?? ?? ?? 57", 17);
	IncRefCount = (IncRefCountPtr)(x_ref_count_call + *(int *)x_ref_count_call + 4);

	char *datum_inc_function = (char *)Pocket::Sigscan::FindPattern(BYONDCORE, "55 8b ec 8b 4d 08 3b 0d ?? ?? ?? ?? 73 11 a1 ?? ?? ?? ?? 8b 04 88 85 c0 74 05 ff 40 10 5d c3 6a 21 51 e8 ?? ?? ?? ??");
	datum_pointer_table_length = *(unsigned int **)(datum_inc_function + 8);
	datum_pointer_table = *(RawDatum ****)(datum_inc_function + 15);

	// the signature below is for a function that fetches a list via the real GetListPointerById and then checks if the list exists, runtiming otherwise
	char *get_list_ptr_ptr = (char *)Pocket::Sigscan::FindPattern(BYONDCORE, "55 8B EC FF 75 08 E8 ?? ?? ?? ?? 83 C4 04 85 C0 75 13 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 5D E9 ?? ?? ?? ?? 5D C3", 7);
	GetListPointerById = (GetListPointerByIdPtr)(get_list_ptr_ptr + *(int *)get_list_ptr_ptr + 4);

	current_execution_context_ptr = *(ExecutionContext ***)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? 8D ?? ?? ?? ?? ?? 83 C4 08 89 48 28 8D ?? ?? ?? ?? ?? 89 48 2C 83 3D ?? ?? ?? ?? ?? 74 25 8B 00 FF 30 E8 ?? ?? ?? ?? 83 C4 04 FF 30 E8 ?? ?? ?? ?? 83 C4 04 FF 30 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 66 ?? ?? ?? ?? ?? ?? A1 ?? ?? ?? ?? 75 28 A8 02 75 24 E8 ?? ?? ?? ?? 85 C0 75 09 50", 1);
	misc_entry_table = **(MiscEntry ****)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? FF 34 B8 FF D6 47 83 C4 04 3B ?? ?? ?? ?? ?? 72 EA FF 35 ?? ?? ?? ?? FF D6 33 FF 83 C4 04 39 ?? ?? ?? ?? ?? 76 1E", 1);
	some_flags_including_profile = *(unsigned int **)Pocket::Sigscan::FindPattern(BYONDCORE, "F7 05 ?? ?? ?? ?? ?? ?? ?? ?? 74 34 8B 01 FF 30 E8 ?? ?? ?? ?? 83 C4 04 8B D8 E8 ?? ?? ?? ?? 8B F0 8B FA E8 ?? ?? ?? ?? 85 DB 74 02 FF 03 8B ?? ?? ?? ?? ?? 89 71 70 89 79 74 89 41 78 89 51 7C 83 3D ?? ?? ?? ?? ?? 5B 74 37", 2);
	global_var_table = **(Value ***)Pocket::Sigscan::FindPattern(BYONDCORE, "C3 8B ?? ?? ?? ?? ?? 8B 04 FA 8B 54 FA 04 5F 5E 5B 8B E5 5D C3", 3);
	obj_table = *(TableHolder2 **)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? 33 FF C1 E0 02 89 06 89 7D 0C 39 ?? ?? ?? ?? ?? 0F 86 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 3C B8 85 FF 74 6A FF 03 83 7F 28 00", 28);
	datum_table = *(TableHolder2 **)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? 8B 04 88 85 C0 74 1E FF 03 83 78 04 00 74 09 0F B7 40 0A C1 E0 04 EB 02 33 C0", 1);
	list_table = *(TableHolder2 **)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? 8B 04 B8 89 45 0C 85 C0 74 21 FF 03 FF 70 04", 1);
	mob_table = *(TableHolder2 **)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? 8B 3C B8 85 FF 0F 84 ?? ?? ?? ?? FF 03 83 7F 28 00 74 09 0F B7 47 2E C1 E0 04", 1);
	suspended_proc_list = *(SuspendedProcList **)Pocket::Sigscan::FindPattern(BYONDCORE, "8B 35 ?? ?? ?? ?? 8B 80 88 00 00 00 57 8B 3D ?? ?? ?? ?? 8B D7 89 45 FC A1 ?? ?? ?? ?? 8B D8 89 45 F8 89 75 F4 3B DA", 2);
#else
	FIND_OR_DIE(Suspend, "55 89 E5 57 31 FF 56 89 C6 53 83 EC ?? 89 F0"); //regparm3
	FIND_OR_DIE(CrashProc, "55 89 E5 53 83 EC ?? 80 3D ?? ?? ?? ?? ?? 75 ?? C7 04 24 ?? ?? ?? ?? E8 ?? ?? ?? ?? 85 C0 75 ?? C7 04 24 ?? ?? ?? ?? 8D 5D ?? E8 ?? ?? ?? ?? 8B 45 ?? 89 5C 24 ?? C7 04 24 ?? ?? ?? ?? 89 44 24 ?? E8 ?? ?? ?? ?? C7 04 24 ?? ?? ?? ??");
	FIND_OR_DIE(StartTiming, "55 89 E5 83 EC ?? 85 C0 89 5D ?? 89 C3 89 75 ?? 89 7D ?? 74 ?? 8B 50 ??"); //regparm3
	FIND_OR_DIE(GetVariable, "55 89 E5 81 EC ?? ?? ?? ?? 8B 55 ?? 89 5D ?? 8B 5D ?? 89 75 ?? 8B 75 ??");
	FIND_OR_DIE(GetProcArrayEntry, "55 31 C0 89 E5 8B 55 ?? 39 15 ?? ?? ?? ?? 76 ?? 8D 04 D2");
	FIND_OR_DIE(GetStringTableEntry, "55 89 E5 83 EC ?? 8B 45 ?? 39 05 ?? ?? ?? ??");
	FIND_OR_DIE(GetProfileInfo, "55 31 C0 89 E5 53 83 EC ?? 8B 5D ?? 8B 15 ?? ?? ?? ??");
	FIND_OR_DIE(CallGlobalProc, "55 89 E5 81 EC ?? ?? ?? ?? 89 5D ?? 89 C3 0F B6 45");
	FIND_OR_DIE(ProcCleanup, "55 89 E5 56 89 C6 53 83 EC ?? 8B 58 ??");
	FIND_OR_DIE(CreateContext, "55 89 E5 57 56 89 C6 53 89 D3 83 EC ?? 80 3D ?? ?? ?? ?? ??");
	FIND_OR_DIE(Runtime, "55 89 E5 57 56 53 83 EC ?? A1 ?? ?? ?? ?? 85 C0 0F 84 ?? ?? ?? ?? 0F B6 48 ??");
	FIND_OR_DIE(DisconnectClient2, "55 89 E5 57 56 53 83 EC ?? 0F B7 7D ?? 66 3B 3D ?? ?? ?? ??");
	FIND_OR_DIE(CreateList, "55 89 E5 57 56 53 83 EC ?? A1 ?? ?? ?? ?? 8B 75 ?? 85 C0 0F 84 ?? ?? ?? ??");
	FIND_OR_DIE(SendMaps, "55 89 E5 57 56 53 81 EC ?? ?? ?? ?? 80 3D ?? ?? ?? ?? ?? 0F 84 ?? ?? ?? ??");
	FIND_OR_DIE(ToString, "55 89 E5 83 EC ?? 89 5D ?? 8B 5D ?? 89 75 ?? 8B 75 ?? 89 7D ?? 80 FB ??");
	FIND_OR_DIE(Length, "55 89 E5 57 56 53 83 EC ?? 8B 45 ?? 8B 5D ?? 3C ??");
	FIND_OR_DIE(PrintToDD, "55 89 E5 53 83 EC ?? 8B 1D ?? ?? ?? ?? 8B 55 ?? 85 DB");
	FIND_OR_DIE(TopicFloodCheck, "55 89 E5 57 56 53 83 EC ?? 0F B7 45 ?? 89 04 24 E8 ?? ?? ?? ?? BA ?? ?? ?? ??");
	FIND_OR_DIE(GetTurf, "55 89 E5 83 EC ?? 0F BF 0D ?? ?? ?? ??");
	FIND_OR_DIE(GetTypeById, "55 31 C0 89 E5 8B 55 ?? 39 15 ?? ?? ?? ?? 76 ?? 6B C2 ??");
	FIND_OR_DIE(MobTableIndexToGlobalTableIndex, "55 31 C0 89 E5 8B 55 ?? 39 15 ?? ?? ?? ?? 76 ?? 89 D0");
	FIND_OR_DIE(AppendToContainer, "55 89 E5 83 EC ?? 3C ?? 89 5D ?? 8B 5D ?? 89 75 ?? 8B 75 ?? 89 7D ?? 76 ??");
	FIND_OR_DIE(DisconnectClient1, "55 89 E5 81 EC ?? ?? ?? ?? 89 75 ?? 8D B5 ?? ?? ?? ?? 89 5D ?? 89 D3");
	FIND_OR_DIE(GetListPointerById, "55 89 E5 83 EC ?? 89 04 24 E8 ?? ?? ?? ??");
	if (ByondBuild >= 1523)
	{
		FIND_OR_DIE(SetAssocElement2, "55 B9 ?? ?? ?? ?? 89 E5 83 EC ?? 89 7D ??");
	}
	else
	{
		FIND_OR_DIE(SetAssocElement1, "55 B9 ?? ?? ?? ?? 89 E5 83 EC ?? 89 75 ??");
	}
	FIND_OR_DIE(CallProcByName, "55 89 E5 83 EC ?? 89 5D ?? 89 D3 8B 55 ?? 89 45 ?? 0F B6 45 ?? 89 75 ?? 89 CE 8B 4D ?? 89 7D ?? 89 14 24 8B 55 ?? 88 45 ?? 0F B6 F8 8D 45 ?? 89 44 24 ?? 89 F8 89 4C 24 ?? 31 C9 C6 45 ?? ?? C7 44 24 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 80 7D ?? ?? 0F 84 ?? ?? ?? ?? 3D ?? ?? ?? ?? 74 ?? 8B 4D ?? 8B 55 ?? 89 44 24 ?? 8D 45 ?? C7 44 24 ?? ?? ?? ?? ?? 89 4C 24 ?? 8B 4D ?? 89 54 24 ?? 8B 55 ?? 89 3C 24 89 4C 24 ?? 89 F1 89 54 24 ?? 89 DA E8 ?? ?? ?? ?? 8B 45 ?? 8B 55 ?? 8B 4D ?? 89 01 89 51 ?? 8D B6 ?? ?? ?? ?? 8B 45 ?? 8B 5D ?? 8B 75 ?? 8B 7D ?? 89 EC 5D C3 F7 C7 ?? ?? ?? ?? 74 ?? 80 7D ?? ?? 0F 84 ?? ?? ?? ?? 8B 4D ?? C7 01 ?? ?? ?? ?? C7 41 ?? ?? ?? ?? ?? 8B 45 ?? 8B 55 ?? 89 44 24 ?? 89 14 24 E8 ?? ?? ?? ?? 80 7D ?? ?? 74 ?? C7 04 24 ?? ?? ?? ?? E8 ?? ?? ?? ?? C7 44 24 ?? ?? ?? ?? ?? C7 04 24 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 45 ?? 8B 55 ?? C7 44 24 ?? ?? ?? ?? ?? C7 44 24 ?? ?? ?? ?? ?? 89 04 24 89 54 24 ?? E8 ?? ?? ?? ?? 8B 55 ?? 89 14 24 E8 ?? ?? ?? ?? 8B 18 C7 44 24 ?? ?? ?? ?? ?? C7 04 24 ?? ?? ?? ?? E8 ?? ?? ?? ?? 89 5C 24 ?? 89 04 24 E8 ?? ?? ?? ?? C7 44 24 ?? ?? ?? ?? ?? 89 04 24 E8 ?? ?? ?? ?? C7 04 24 ?? ?? ?? ?? E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 89 04 24 E8 ?? ?? ?? ??"); // OH DEAR GOD
	FIND_OR_DIE(DelDatum, "55 89 e5 53 83 ec 44 8b 45 08 3b 05 ?? ?? ?? ?? 73 2c 8b 15 ?? ?? ?? ?? 8b 0c 82 85 c9 74 1f 8b 51 10 81 fa 00 00 00 70");
	FIND_OR_DIE(DecRefCount, "55 89 E5 83 EC ?? 8B 45 08 89 5D ?? 8B 5D 0C 89 75 ?? 3C 54");
	FIND_OR_DIE(IncRefCount, "55 89 E5 83 EC ?? 8B 55 08 8b 45 0C 80 FA 54 76 07 c9 c3 90");
	FIND_OR_DIE(GetAssocElement, "55 89 E5 83 EC ?? 89 4D C4 B9 7B 00 00 00 89 5D F4 89 D3 89 75 F8 89 C6 89 D0 8B 55 C4 89 7D FC C7 44 24 04 00 00 00 00 C7 04 24 00 00 00 00 E8 ?? ?? ?? ?? 3D FF FF 00 00"); // regparm3
#warning TODO: IsInContainer
#warning TODO: GetSocketHandleStruct
#warning TODO: GetGlobalByName
#warning TODO: global_var_table
#warning TODO: GetTableHolderThingyById
#warning TODO: GetBSocket
#warning TODO: name_table
	switch (ByondVersion)
	{
	case 512:
		FIND_OR_DIE(SetVariable, "55 89 E5 81 EC ?? ?? ?? ?? 89 75 ?? 8B 55 ?? 8B 75 ??");
		FIND_OR_DIE(GetStringTableIndex, "55 89 E5 57 56 53 89 D3 83 EC ?? 85 C0"); // regparm3
		break;
	case 513:
		FIND_OR_DIE(SetVariable, "55 89 E5 81 EC ?? ?? ?? ?? 8B 55 ?? 8B 45 ??");
		FIND_OR_DIE(GetStringTableIndexUTF8, "55 89 E5 57 56 89 CE 53 89 D3 83 EC ?? 8B 55 ?? 85 C0") // regparm3
		break;
	default:
		break;
	}
	if (failed)
		return false;

	datum_pointer_table_length = *(unsigned int **)((char *)DelDatum + 12);
	datum_pointer_table = *(RawDatum ****)((char *)DelDatum + 20);

	current_execution_context_ptr = *(ExecutionContext ***)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? 8D 7D ?? 89 78 ??", 1);
	misc_entry_table = **(MiscEntry ****)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? 8B 04 98 85 C0 74 ?? 89 04 24 E8 ?? ?? ?? ?? 8B 15 ?? ?? ?? ??", 1);
	some_flags_including_profile = *(unsigned int **)Pocket::Sigscan::FindPattern(BYONDCORE, "A1 ?? ?? ?? ?? A8 ?? 0F 85 ?? ?? ?? ??", 2);
#endif
	return !failed;
}
