#include "plugin.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include "eModelID.h"
#include "CModelInfo.h"
#include "CTheScripts.h"


using namespace plugin;
using namespace std;

class AddSpecialCars {
public:
    static unsigned int currentSpecialModelForSiren;
    
    static unsigned int jmp_6AB360;

    static unordered_set<unsigned int> &GetCopcarlaModels() {
        static unordered_set<unsigned int> copcarlaIds;
        return copcarlaIds;
    }

    static int __stdcall GetSpecialModelForSiren(unsigned int model) {
        if (model == MODEL_COPCARLA || GetCopcarlaModels().find(model) != GetCopcarlaModels().end())
            return MODEL_COPCARLA;
        return model;
    }


    static void Patch_6AB350(); // Siren


    // CVehicle::IsLawEnforcementVehicle
    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
        bool result; 

        if (GetCopcarlaModels().find(_this->m_nModelIndex) != GetCopcarlaModels().end()) {
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

        switch (_this->m_nModelIndex) {
        case MODEL_FIRETRUK:                        
        case MODEL_AMBULAN:                         
        case MODEL_MRWHOOP:                         
            result = true;
            break;
        case MODEL_RHINO:                           
            result = false;
            break;
        default:
            result = IsLawEnforcementVehicle(_this) != 0;
            break;
        }
        return result;
    }


    static void __fastcall OpcodeIsCharInAnyPoliceVehicle(CRunningScript *script) {
        script->CollectParameters(1);
        bool inModel = false;

        CPed *ped = CPools::ms_pPedPool->GetAt(CTheScripts::ScriptParams[0].uParam);
        if (ped->m_nPedFlags.bIsStanding) {
            if (ped->m_pVehicle) {
                if (IsLawEnforcementVehicle(ped->m_pVehicle)) {
                    if (ped->m_pVehicle->m_nModelIndex != MODEL_PREDATOR)
                        inModel = true;
                }
            }
        }
        script->UpdateCompareFlag(inModel);
    }

    AddSpecialCars() {
        ifstream stream(PLUGIN_PATH("SpecialCars.dat"));
        if (!stream.is_open())
            return;
        for (string line; getline(stream, line); ) {
            if (!line.compare("copcarla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopcarlaModels().insert(stoi(line));
                }
            }
            
        }
        
        patch::RedirectJump(0x6D2370, IsLawEnforcementVehicle);
        patch::RedirectJump(0x6D8470, UsesSiren);

        patch::RedirectJump(0x6AB350, Patch_6AB350);


        Events::drawingEvent += [] {
            CVehicle *vehicle = FindPlayerVehicle(0, false);
            if (vehicle) {
                gamefont::Print({
                    Format("siren = %d", vehicle->m_nVehicleFlags.bSirenOrAlarm)
                }, 10, 300, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
            }
        };

    }
} specialCars;

unsigned int AddSpecialCars::currentSpecialModelForSiren;

unsigned int AddSpecialCars::jmp_6AB360;

void __declspec(naked) AddSpecialCars::Patch_6AB350() { // Siren
    __asm {
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
