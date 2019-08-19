#include "plugin.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include "eModelID.h"
#include "CModelInfo.h"
#include "CStreaming.h"
#include "CTheZones.h"
#include "CWorld.h"
#include "CTheScripts.h"


using namespace plugin;
using namespace std;

class AddSpecialCars {
public:
    static int currentSpecialModelForSiren;
    static int currentTaxiModel;
    static int currentModel;
    static unsigned int jmp_6AB360;
    static unsigned int jmp_469658;

    static void Patch_6AB349(); // Siren
    static void Patch_4912D0(); // Taxi
    static void Patch_469629(); // IsCharInModel

    static unordered_set<unsigned int> &GetCopcarlaModels() {
        static unordered_set<unsigned int> copcarlaIds;
        return copcarlaIds;
    }

    static unordered_set<unsigned int> &GetCopcarsfModels() {
        static unordered_set<unsigned int> copcarsfIds;
        return copcarsfIds;
    }

    static unordered_set<unsigned int> &GetCopcarvgModels() {
        static unordered_set<unsigned int> copcarvgIds;
        return copcarvgIds;
    }

    static unordered_set<unsigned int> &GetFbiranchModels() {
        static unordered_set<unsigned int> fbiranchIds;
        return fbiranchIds;
    }

    static unordered_set<unsigned int> &GetTaxiModels() {
        static unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static unordered_set<unsigned int> &GetAmbulanModels() {
        static unordered_set<unsigned int> ambulanIds;
        return ambulanIds;
    }

    static unordered_set<unsigned int> &GetBoxburgModels() {
        static unordered_set<unsigned int> boxburgIds;
        return boxburgIds;
    }

    static int __stdcall GetSpecialModelForSiren(unsigned int model) {
        if (model == MODEL_COPCARLA || GetCopcarlaModels().find(model) != GetCopcarlaModels().end())
            return MODEL_COPCARLA;
        else if (model == MODEL_COPCARSF || GetCopcarsfModels().find(model) != GetCopcarsfModels().end())
            return MODEL_COPCARSF;
        else if (model == MODEL_COPCARVG || GetCopcarvgModels().find(model) != GetCopcarvgModels().end())
            return MODEL_COPCARVG;
        else if (model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end())
            return MODEL_FBIRANCH;
        else if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            return MODEL_AMBULAN;
        return model;
    }

    static int __stdcall Get—urrentTaxiModel(unsigned int model) {
        if (model == MODEL_TAXI || model == MODEL_CABBIE || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        return model;
    }

    /*static int __stdcall GetCurrentModel(unsigned int model) {
        if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            return MODEL_AMBULAN;
        return model;
    }*/

    static bool __stdcall IsCharInModel(CPed *ped) {
        bool inModel = false;

        if (ped && ped->m_nPedFlags.bIsStanding) {
            if (ped && ped->m_pVehicle) {
                unsigned int model = ped->m_pVehicle->m_nModelIndex;
                if (CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) {
                    if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end()) // Paramedic
                        inModel = true;
                }
                else if (CTheScripts::ScriptParams[1].uParam == MODEL_BOXBURG) {
                    if (model == MODEL_BOXBURG || GetBoxburgModels().find(model) != GetBoxburgModels().end()) // Burglary
                        inModel = true;
                }
                else if (model == CTheScripts::ScriptParams[1].uParam)
                    inModel = true;
            }
        }
        return inModel;
    }
    
    /*static int __stdcall GetSpecialModelForOccupants(unsigned int model) {
        if (model == MODEL_COPCARLA || GetCopcarlaModels().find(model) != GetCopcarlaModels().end())
            return MODEL_COPCARLA;
        return model;
    }*/

    
    // CVehicle::IsLawEnforcementVehicle
    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
        bool result; 

        if (GetCopcarlaModels().find(_this->m_nModelIndex) != GetCopcarlaModels().end()) {
            result = true; return result;
        }
        else if (GetCopcarsfModels().find(_this->m_nModelIndex) != GetCopcarsfModels().end()) {
            result = true; return result;
        }
        else if (GetCopcarvgModels().find(_this->m_nModelIndex) != GetCopcarvgModels().end()) {
            result = true; return result;
        }
        else if (GetFbiranchModels().find(_this->m_nModelIndex) != GetFbiranchModels().end()) {
            result = true; return result;
        }
        switch (_this->m_nModelIndex) {
        case MODEL_ENFORCER:                        
        case MODEL_PREDATOR:                        
        case MODEL_RHINO:                           
        case MODEL_BARRACKS:                        
        case MODEL_FBIRANCH:                        
        case MODEL_COPBIKE:                         
        case MODEL_FBITRUCK:                        
        case MODEL_COPCARLA:                        
        case MODEL_COPCARSF:                        
        case MODEL_COPCARVG:                        
        case MODEL_COPCARRU:                        
        case MODEL_SWATVAN:
        //case MODEL_HYDRA:
            result = true;
            break;
        default:
            result = false;
            break;
        }
        return result;
    }

    // CVehicle::UsesSiren
    static bool __fastcall UsesSiren(CVehicle *_this) {
        bool result; 

        if (GetAmbulanModels().find(_this->m_nModelIndex) != GetAmbulanModels().end()) {
            result = true; return result;
        }
        switch (_this->m_nModelIndex) {
        case MODEL_FIRETRUK:                        
        case MODEL_AMBULAN:                         
        case MODEL_MRWHOOP:                         
            result = true;
            break;
        case MODEL_RHINO:
        //case MODEL_HYDRA:
            result = false;
            break;
        default:
            result = IsLawEnforcementVehicle(_this) != 0;
            break;
        }
        return result;
    }

    // CStreaming::GetDefaultCopCarModel
    static int __cdecl GetDefaultCopCarModel(unsigned int a1) {
        int result, v2, i; 
        //
        // CStreaming::ms_aDefaultCopCarModel[1] = 3136;

        if (!CStreaming::m_bCopBikeLoaded || a1
            || CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCopModel[4]].m_nLoadState != 1
            || (result = CStreaming::ms_DefaultCopBikeModel, CStreaming::ms_aInfoForModel[CStreaming::ms_DefaultCopBikeModel].m_nLoadState != 1))
        {
            result = CStreaming::ms_aDefaultCopCarModel[CTheZones::m_CurrLevel];
            if (CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCopModel[CTheZones::m_CurrLevel]].m_nLoadState != 1
                || CStreaming::ms_aInfoForModel[result].m_nLoadState != 1)
            {
                v2 = 5;
                if (a1)
                    v2 = 4;
                i = 0;
                if (v2 <= 0)
                {
                LABEL_13:
                    result = -1;
                }
                else
                {
                    while (CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCopModel[i]].m_nLoadState != 1
                        || CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCopCarModel[i]].m_nLoadState != 1)
                    {
                        if (++i >= v2)
                            goto LABEL_13;
                    }
                    //if (i == 1)
                        result = 3136;
                    //else
                        //result = CStreaming::ms_aDefaultCopCarModel[i];
                }
            }
        }
        return result;
    }

    // CCarCtrl::ChoosePoliceCarModel
    static int __cdecl ChoosePoliceCarModel(unsigned int a1) {
        CWanted *wanted = FindPlayerWanted(-1);

        if (wanted->AreSwatRequired() && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
            && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED) {
            if (plugin::Random(0, 3) == 2)
                return MODEL_ENFORCER;
        }
        else {
            if (wanted->AreFbiRequired() && CStreaming::ms_aInfoForModel[MODEL_FBIRANCH].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
            {
                return MODEL_FBIRANCH;
            }
            if (wanted->AreArmyRequired()
                && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == LOADSTATE_LOADED)
            {
                return (rand() < 16383) + MODEL_RHINO;
            }
        }
        //return CStreaming::GetDefaultCopCarModel(a1);
        if (LoadModel(3137))
            return 3137;
        else
            return CStreaming::GetDefaultCopCarModel(a1);
    }

    static int GetModel(unsigned int modelId) {
        for (unsigned int i : GetCopcarlaModels()) {
            if (i == modelId)
                return i;
        }
        return 0;
    }

    static bool LoadModel(int model) {
        unsigned char oldFlags = CStreaming::ms_aInfoForModel[model].m_nFlags;
        CStreaming::RequestModel(model, GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::ms_aInfoForModel[model].m_nLoadState == LOADSTATE_LOADED) {
            if (!(oldFlags & GAME_REQUIRED)) {
                CStreaming::SetModelIsDeletable(model);
                CStreaming::SetModelTxdIsDeletable(model);
            }
            return true;
        }
        return false;
    }

    
    AddSpecialCars() {
        ifstream stream(PLUGIN_PATH("SpecialCars.dat"));
        if (!stream.is_open())
            return;
        for (string line; getline(stream, line); ) {
            // copcarla
            if (!line.compare("copcarla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopcarlaModels().insert(stoi(line));
                }
            }
            // copcarsf
            if (!line.compare("copcarsf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopcarsfModels().insert(stoi(line));
                }
            }
            // copcarvg
            if (!line.compare("copcarvg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopcarvgModels().insert(stoi(line));
                }
            }
            // fbiranch
            if (!line.compare("fbiranch")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetFbiranchModels().insert(stoi(line));
                }
            }
            // taxi
            if (!line.compare("taxi")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetTaxiModels().insert(stoi(line));
                }
            }
            // ambulan
            if (!line.compare("ambulan")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetAmbulanModels().insert(stoi(line));
                }
            }
            // boxburg
            if (!line.compare("boxburg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetBoxburgModels().insert(stoi(line));
                }
            }

        }
        
        patch::RedirectJump(0x6D2370, IsLawEnforcementVehicle);
        patch::RedirectJump(0x6D8470, UsesSiren);
        //patch::RedirectJump(0x407C50, GetDefaultCopCarModel);
        //patch::RedirectJump(0x421980, ChoosePoliceCarModel);

        patch::RedirectJump(0x6AB349, Patch_6AB349);
        patch::RedirectJump(0x4912D0, Patch_4912D0); 
        patch::RedirectJump(0x469629, Patch_469629);

        Events::drawingEvent += [] {
            CVehicle *vehicle = FindPlayerVehicle(0, false);
            if (vehicle) {
                gamefont::Print({
                    Format("siren = %d", vehicle->m_nVehicleFlags.bSirenOrAlarm),
                    Format("model = %d", GetCopcarlaModels().find(vehicle->m_nModelIndex) != GetCopcarlaModels().end()),
                    Format("test = %d", GetModel(vehicle->m_nModelIndex))
                }, 10, 300, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
            }
        };

    }
} specialCars;

int AddSpecialCars::currentSpecialModelForSiren;
int AddSpecialCars::currentTaxiModel;
int AddSpecialCars::currentModel;
unsigned int AddSpecialCars::jmp_6AB360;
unsigned int AddSpecialCars::jmp_469658;

void __declspec(naked) AddSpecialCars::Patch_6AB349() { // Siren
    __asm {
        mov     ecx, esi
        call    CVehicle::AddDamagedVehicleParticles
        movsx   eax, word ptr[esi + 0x22]
        pushad
        push eax
        call GetSpecialModelForSiren
        mov currentSpecialModelForSiren, eax
        popad
        mov eax, currentSpecialModelForSiren
        lea edi, [eax - 0x197]
        cmp edi, 192
        mov jmp_6AB360, 0x6AB360
        jmp jmp_6AB360
    }
}

void __declspec(naked) AddSpecialCars::Patch_4912D0() { // Taxi
    __asm {
        CWDE
        mov ecx, 420
        pushad
        push eax
        call Get—urrentTaxiModel
        mov currentTaxiModel, eax
        popad
        cmp ecx, currentTaxiModel
        jz SET_TRUE
        jmp END_CHECK
        SET_TRUE :
        mov byte ptr[esp + 20], 1
            END_CHECK :
            mov ecx, 0x4912E1
            jmp ecx
    }
}

void __declspec(naked) AddSpecialCars::Patch_469629() { // IsCharInModel
    __asm {
        pushad
        push eax
        call IsCharInModel
        mov currentModel, eax
        popad
        mov eax, currentModel
        mov jmp_469658, 0x469658
        jmp jmp_469658
    }
}
