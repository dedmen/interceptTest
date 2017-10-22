#include "magGroupTest.h"
#include <client/headers/intercept.hpp>
using namespace intercept;

uint32_t weaponInitGetMagJmpBack, magazineInitMagGroupJmpBack;
class __dummyVtable {
    char pad_0x0000[0x10]; //missunderstanding crap.. might as well be empty class

};

class magType : public refcount, public __dummyVtable {
public:
    char pad_0x0000[0x24 -0x10-8 /*-refcount */]; //0x0000
    r_string displayName; //0x24

};//Size=0x0044


std::map<std::string, std::vector<ref<magType>>> groups;

void weaponGetMag(uintptr_t muzzle, uintptr_t magName) {
    r_string name(reinterpret_cast<compact_array<char>*>(magName));
    std::string nameStr = static_cast<std::string>(name);
    auto_array<ref<magType>>* magTypes = reinterpret_cast<auto_array<ref<magType>>*>(muzzle + 0x3AC);
    auto found = groups.find(nameStr);
    if (found != groups.end()) {
        for (auto& addMag : found->second) {
            if (std::find(magTypes->begin(), magTypes->end(), addMag) == magTypes->end()) {
                magTypes->push_back(addMag);
            }
        }

    }
    
    
    //muzzle + 0x3AC is array
}

void magGroupInit(uintptr_t mType, uintptr_t groupName) {
    r_string* name(reinterpret_cast<r_string*>(groupName));
    std::string nameStr = static_cast<std::string>(*name);
    auto mtype = reinterpret_cast<magType*>(mType);
    ref<magType>* mtypePtr = reinterpret_cast<ref<magType>*>(&mType);
    std::string magName = static_cast<std::string>(mtype->displayName);

    groups[nameStr].push_back(*mtypePtr);

    return;

    //muzzle + 0x3AC is array
}

void __declspec(naked) weaponInitGetMag() {
    static uintptr_t mtype = 0;
    static uintptr_t magName = 0;
    __asm
    {
        mov     ecx, eax
        //0x00D88382 - 0x1D0000

        mov     edx, [eax+8]
        mov     magName, edx

        mov     edx, [eax]
        call    dword ptr[edx + 0Ch]
        mov     mtype, ebx
        //muzzleType in EBX
        //mag name in ECX


        push ecx;

        push eax;
        push ebx;

    }
    weaponGetMag(mtype, magName);
    __asm
    {
        pop ebx;
        pop eax;
          //push    ecx

        mov     ecx, [esp + 38h]
        mov     edx, esp
        test    ecx, ecx

        jmp     weaponInitGetMagJmpBack //0x00D88392 - 0x1D0000
    }
}

void __declspec(naked) magazineInitMagGroup() {
    static uintptr_t mtype = 0;
    static uintptr_t magName = 0;
    __asm
    {
        //00D86854 - 0x1D0000
        push    ecx
        mov     ecx, eax
        mov     edx, [eax]
        call    dword ptr[edx + 0Ch]


        //0x00D8685C - 0x1D0000
        //ESP + 4 is magazineType
        //ECX is name of magGroup
        mov     magName, ecx
        mov     eax, [esp +4]
        mov     mtype, eax



        mov     eax, [esp + 78h]
        lea     ecx, [esp + 78h]
        push ecx;
        push eax;
        push ebx;

    }
    magGroupInit(mtype, magName);
    __asm
    {
        pop     ebx;
        pop     eax;
        pop     ecx

        jmp     magazineInitMagGroupJmpBack //0x00D88392 - 0x1D0000
    }
}




uintptr_t placeHookTotalOffs(uintptr_t totalOffset, uintptr_t jmpTo);

#include <Psapi.h>
#pragma comment (lib, "Psapi.lib")//GetModuleInformation

uintptr_t(_cdecl *findMagType)(int stat, const char* name);
uintptr_t findMagStat;
uintptr_t(__thiscall *magTypeConstr)(uintptr_t mt);
uintptr_t(__thiscall *magTypeInit)(uintptr_t mt, const char* name);
uintptr_t(__thiscall *addMagType)(uintptr_t stat, uintptr_t mag);
uintptr_t allocB;


void addHuuk() {
    MODULEINFO modInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
    auto engineBase = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    auto engineSize = static_cast<uintptr_t>(modInfo.SizeOfImage);


    placeHookTotalOffs(0x00D88382 - 0x1D0000 + engineBase, (uintptr_t) weaponInitGetMag);
    weaponInitGetMagJmpBack = 0x00D88392 - 0x1D0000 + engineBase;
    placeHookTotalOffs(0x00D86854 - 0x1D0000 + engineBase, (uintptr_t) magazineInitMagGroup);
    magazineInitMagGroupJmpBack = 0x00D86864 - 0x1D0000 + engineBase;

    findMagType = (uintptr_t(*)(int, const char*))(0x00E944D0 - 0xA30000 + engineBase);
    magTypeConstr = (uintptr_t(__thiscall*)(uintptr_t))(0x15DF150 - 0xA30000 + engineBase);
    magTypeInit = (uintptr_t(__thiscall*)(uintptr_t, const char*))(0x15E5AE0 - 0xA30000 + engineBase);
    addMagType = (uintptr_t(__thiscall*)(uintptr_t, uintptr_t))(0xDB1EB0 - 0xA30000 + engineBase);
    allocB = 0x25F0B40 - 0xA30000 + engineBase;
    findMagStat = 0x28A5604 - 0xA30000 + engineBase;
}


/*
00FC0000
rail height modelLocal = 0201E082 [eax+0x28]
muzzleHeight = 0201D9A5 [esp+0x70]
scopecenter modellocal = 0201D7B9 [eax+0x4]
*/

#pragma endregion railScopeHeightHack

void magGroupTest::preStart() {
    addHuuk();
    //#TODO and scope is not private
    auto cfgProps = sqf::config_properties(sqf::config_entry() >> "cfgMagazines", "isClass _x && isArray (_x>>'magazineGroup')", true);

    for (auto& entry : cfgProps) {
        std::string classname = sqf::config_name(entry);
        if (classname == "ACE_30Rnd_556x45_Stanag_M995_AP_mag")       __debugbreak();
        r_string cnameR = classname;
        uintptr_t found = findMagType(findMagStat,cnameR.data());
        if (!found) {
            
            //typedef uintptr_t(__stdcall *allloc)(signed int);
            //allloc allF = **((allloc**) (allocB + 4));    allF(632)
            uintptr_t newMag = (uintptr_t)intercept::types::__internal::rv_allocator_allocate_generic(632);
            magTypeConstr(newMag);
            magTypeInit(newMag, cnameR.data());
            addMagType(findMagStat, newMag);
            //add refcount?
        }

    }
    OutputDebugStringA("done");


}
