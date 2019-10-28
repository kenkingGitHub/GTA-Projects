#include "plugin.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include "eModelID.h"
#include "CModelInfo.h"
#include "CStreaming.h"
#include "CTheZones.h"
#include "CTheScripts.h"
#include "CTimer.h"
#include "CWorld.h"
#include "extensions\ScriptCommands.h"
#include "eScriptCommands.h"
#include "CCarAI.h"
#include "CFont.h"
#include "CCopPed.h"
#include "extensions\ScriptCommands.h"
#include "eScriptCommands.h"
#include "CCarCtrl.h"

//#include "CHudColours.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"

bool &/*CCarCtrl::*/bCarIsBeingCreated = *(bool *)0x9690CC;

using namespace plugin;
using namespace std;

int m_nEmergencyServices, m_nTime, m_CurrLevel;

unordered_set<unsigned int> 
/*cop vehicles*/            CopBikeLA_IDs, CopBikeSF_IDs, CopBikeVG_IDs, CopCarLA_IDs, CopCarSF_IDs, CopCarVG_IDs, 
                            CopCarRed_IDs, CopCarFlint_IDs, CopCarBone_IDs, 
                            Enforcer_IDs, Swatvan_IDs, Fbiranch_IDs, Barracks_IDs,
/*cop peds*/                CopBikerLA_IDs, CopBikerSF_IDs, CopBikerVG_IDs, CopLA_IDs, CopSF_IDs, CopVG_IDs, 
                            CopRed_IDs, CopFlint_IDs, CopBone_IDs, Swat_IDs, Fbi_IDs, Army_IDs,
/*cop weapons*/             CopWeapon_IDs, SwatWeapon_IDs, FbiWeapon_IDs, ArmyWeapon_IDs,
/*taxi cars && peds*/       TaxiLA_IDs, TaxiSF_IDs, TaxiVG_IDs, TaxiDriver_IDs,
/*emergency cars*/          AmbulanLA_IDs, AmbulanSF_IDs, AmbulanVG_IDs, FiretrukLA_IDs, FiretrukSF_IDs, FiretrukVG_IDs,
/*emergency peds*/          MedicLA_IDs, MedicSF_IDs, MedicVG_IDs, FiremanLA_IDs, FiremanSF_IDs, FiremanVG_IDs,
/*mission vehicles*/        Boxburg_IDs, Broadway_IDs, Streak_IDs, Streakc_IDs;

bool isSwat = false, isFbi = false, isArmy = false, isAmbulan = false, isFiretruck = false, isCabDriver = false,  
isEnforcer = true, isFbiranch = true, isBarracks = true, isMedic = true;

int randomFbiCar = 2, randomSwatCar = 2, randomArmyCar = 3, randomCabDriver = 5, weaponAmmo;
unsigned int randomRoadBlocksTime = 0, m_nGenerateEmergencyServicesTime = 0;

class AddSpecialCars {
public:
    static float ScreenCoord(float a) {
        return (a * (static_cast<float>(RsGlobal.maximumHeight) / 900.0f));
    }
    
    class TaxiInfo {
    public:
        bool enabledTaxiLight;
        TaxiInfo(CVehicle *vehicle) { 
            if (plugin::Random(0, 1))
                enabledTaxiLight = true;
            else
                enabledTaxiLight = false;
        }
    };

    static VehicleExtendedData<TaxiInfo> taxiInfo;

    enum eSpawnCarState { STATE_FIND, STATE_WAIT, STATE_CREATE };
    enum eSpecialModel { TYPE_AMBULAN, TYPE_MEDIC, TYPE_FIRETRUK, TYPE_FIREMAN, TYPE_COPBIKE, TYPE_COPBIKER, TYPE_COP, TYPE_COP_CAR, TYPE_SWAT, TYPE_FBI, TYPE_ARMY, TYPE_TAXI, TYPE_TAXI_DRIVER };

    static eSpawnCarState m_currentState;
    static short outCount;
    static CVector carPos;
    static float carAngle;
    static CAutoPilot pilot;
    static CBaseModelInfo *modelInfo;
    static int currentModelForSiren, currentModelCopbike, currentModelTaxi, currentModelFiretruk, currentModel,
        currentWaterJetsModel, currentTurretsModel, currentModel_Patch_41C0A6, currentModel_Patch_42BBC8, 
        currentModel_Patch_613A68, currentModel_Patch_46130F, currentModel_Patch_48DA65, currentModel_Patch_461BED,
        currentModel_Patch_43069E, currentModel_Patch_431EC0;
    static unsigned int jmp_6AB360, jmp_469658, jmp_41C0AF, jmp_42BBCE, jmp_613A71, jmp_6BD415, jmp_48DAA2;
    static eWeaponType currentWeaponType;

    static void Patch_6AB349();    static void Patch_4912D0();    static void Patch_469629();    
    static void Patch_6ACA57();    static void Patch_6B1F4F();    static void Patch_41C0A6(); 
    static void Patch_42BBC8();    static void Patch_613A68();    static void Patch_6ACA51();    
    static void Patch_6BD408();    static void Patch_46130F();    static void Patch_48DA65(); // IsCharInAnyPoliceVehicle
    static void Patch_5DDC99();    static void Patch_461BED();    static void Patch_461C0A();
    static void Patch_43069E();    static void Patch_431EC0();

    static int __stdcall GetModelForSiren(unsigned int model) {
        if (model == MODEL_COPCARLA || CopCarLA_IDs.find(model) != CopCarLA_IDs.end())
            return MODEL_COPCARLA;
        else if (model == MODEL_COPCARSF || CopCarSF_IDs.find(model) != CopCarSF_IDs.end())
            return MODEL_COPCARSF;
        else if (model == MODEL_COPCARVG || CopCarVG_IDs.find(model) != CopCarVG_IDs.end())
            return MODEL_COPCARVG;
        else if (model == MODEL_COPCARRU || CopCarRed_IDs.find(model) != CopCarRed_IDs.end() || CopCarFlint_IDs.find(model) != CopCarFlint_IDs.end() || CopCarBone_IDs.find(model) != CopCarBone_IDs.end())
            return MODEL_COPCARRU;
        else if (model == MODEL_FBIRANCH || Fbiranch_IDs.find(model) != Fbiranch_IDs.end())
            return MODEL_FBIRANCH;
        else if (model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end())
            return MODEL_ENFORCER;
        else if (model == MODEL_TAXI || TaxiLA_IDs.find(model) != TaxiLA_IDs.end() || TaxiSF_IDs.find(model) != TaxiSF_IDs.end() || TaxiVG_IDs.find(model) != TaxiVG_IDs.end())
            return MODEL_TAXI;
        else if (model == MODEL_AMBULAN || AmbulanLA_IDs.find(model) != AmbulanLA_IDs.end() || AmbulanSF_IDs.find(model) != AmbulanSF_IDs.end() || AmbulanVG_IDs.find(model) != AmbulanVG_IDs.end())
            return MODEL_AMBULAN;
        else if (model == MODEL_FIRETRUK || model == MODEL_FIRELA || FiretrukLA_IDs.find(model) != FiretrukLA_IDs.end() || FiretrukSF_IDs.find(model) != FiretrukSF_IDs.end() || FiretrukVG_IDs.find(model) != FiretrukVG_IDs.end())
            return MODEL_FIRETRUK;
        return model;
    }

    static int __stdcall GetCopbikeModel(unsigned int model) {
        if (model == MODEL_COPBIKE || CopBikeLA_IDs.find(model) != CopBikeLA_IDs.end() 
            || CopBikeSF_IDs.find(model) != CopBikeSF_IDs.end() || CopBikeVG_IDs.find(model) != CopBikeVG_IDs.end())
            return MODEL_COPBIKE;
        return model;
    }

    static int __stdcall GetTaxiModel(unsigned int model) {
        if (model == MODEL_TAXI || model == MODEL_CABBIE || TaxiLA_IDs.find(model) != TaxiLA_IDs.end() || TaxiSF_IDs.find(model) != TaxiSF_IDs.end() || TaxiVG_IDs.find(model) != TaxiVG_IDs.end())
            return MODEL_TAXI;
        return model;
    }

    static int __stdcall GetFiretrukModel(unsigned int model) {
        if (model == MODEL_FIRETRUK || FiretrukLA_IDs.find(model) != FiretrukLA_IDs.end() || FiretrukSF_IDs.find(model) != FiretrukSF_IDs.end() || FiretrukVG_IDs.find(model) != FiretrukVG_IDs.end())
            return MODEL_FIRETRUK;
        return model;
    }

    static int __stdcall GetWaterJetsModel(unsigned int model) {
        if (model == MODEL_FIRETRUK || FiretrukLA_IDs.find(model) != FiretrukLA_IDs.end()
            || FiretrukSF_IDs.find(model) != FiretrukSF_IDs.end() || FiretrukVG_IDs.find(model) != FiretrukVG_IDs.end()
            || model == MODEL_SWATVAN || Swatvan_IDs.find(model) != Swatvan_IDs.end())
            return MODEL_FIRETRUK;
        return model;
    }

    static int __stdcall GetTurretsModel(unsigned int model) {
        if (model == MODEL_SWATVAN || Swatvan_IDs.find(model) != Swatvan_IDs.end())
            return MODEL_SWATVAN;
        return model;
    }

    static bool __stdcall IsCharInModel(CPed *ped) {
        bool inModel = false;

        if (ped && ped->m_nPedFlags.bIsStanding) {
            if (ped && ped->m_pVehicle) {
                unsigned int model = ped->m_pVehicle->m_nModelIndex;
                if (CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) {
                    if (model == MODEL_AMBULAN 
                        || AmbulanLA_IDs.find(model) != AmbulanLA_IDs.end()
                        || AmbulanSF_IDs.find(model) != AmbulanSF_IDs.end() 
                        || AmbulanVG_IDs.find(model) != AmbulanVG_IDs.end()) // Paramedic
                        inModel = true;
                }
                else if (CTheScripts::ScriptParams[1].uParam == MODEL_BOXBURG) {
                    if (model == MODEL_BOXBURG || Boxburg_IDs.find(model) != Boxburg_IDs.end()) // Burglary
                        inModel = true;
                }
                else if (CTheScripts::ScriptParams[1].uParam == MODEL_FIRETRUK) {
                    if (model == MODEL_FIRETRUK 
                        || FiretrukLA_IDs.find(model) != FiretrukLA_IDs.end()
                        || FiretrukSF_IDs.find(model) != FiretrukSF_IDs.end() 
                        || FiretrukVG_IDs.find(model) != FiretrukVG_IDs.end()) // Firefighter
                        inModel = true;
                }
                else if (CTheScripts::ScriptParams[1].uParam == MODEL_BROADWAY) {
                    if (model == MODEL_BROADWAY || Broadway_IDs.find(model) != Broadway_IDs.end()) // Pimping
                        inModel = true;
                }
                else if (CTheScripts::ScriptParams[1].uParam == MODEL_STREAK) {
                    if (model == MODEL_STREAK || Streak_IDs.find(model) != Streak_IDs.end()) // Freight Train
                        inModel = true;
                }
                else if (CTheScripts::ScriptParams[1].uParam == MODEL_STREAKC) {
                    if (model == MODEL_STREAKC || Streakc_IDs.find(model) != Streakc_IDs.end()) 
                        inModel = true;
                }
                else if (model == CTheScripts::ScriptParams[1].uParam)
                    inModel = true;
            }
        }
        return inModel;
    }
    
    static int __stdcall GetModel_Patch_41C0A6(unsigned int model) {
        if (model == MODEL_COPCARLA || model == MODEL_FBITRUCK || CopCarLA_IDs.find(model) != CopCarLA_IDs.end())
            return 169;
        else if (model == MODEL_COPCARSF || model == MODEL_SWATVAN || CopCarSF_IDs.find(model) != CopCarSF_IDs.end())
            return 170;
        else if (model == MODEL_COPCARVG || CopCarVG_IDs.find(model) != CopCarVG_IDs.end())
            return 171;
        else if (model == MODEL_COPCARRU || CopCarRed_IDs.find(model) != CopCarRed_IDs.end() || CopCarFlint_IDs.find(model) != CopCarFlint_IDs.end() || CopCarBone_IDs.find(model) != CopCarBone_IDs.end())
            return 172;
        else if (model == MODEL_COPBIKE || CopBikeLA_IDs.find(model) != CopBikeLA_IDs.end() || CopBikeSF_IDs.find(model) != CopBikeSF_IDs.end() || CopBikeVG_IDs.find(model) != CopBikeVG_IDs.end())
            return 96;
        else if (model == MODEL_FBIRANCH || model == MODEL_PATRIOT || Fbiranch_IDs.find(model) != Fbiranch_IDs.end())
            return 63;
        else if (model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end())
            return 0;
        else if (model == MODEL_BARRACKS || Barracks_IDs.find(model) != Barracks_IDs.end())
            return 6;
        return model - 427;
    }

    static int __stdcall GetModel_Patch_46130F(unsigned int model) {
        if (model == MODEL_COPCARLA || CopCarLA_IDs.find(model) != CopCarLA_IDs.end())
            return 169;
        else if (model == MODEL_COPCARSF || CopCarSF_IDs.find(model) != CopCarSF_IDs.end())
            return 170;
        else if (model == MODEL_COPCARVG || CopCarVG_IDs.find(model) != CopCarVG_IDs.end())
            return 171;
        else if (model == MODEL_COPCARRU || CopCarRed_IDs.find(model) != CopCarRed_IDs.end() || CopCarFlint_IDs.find(model) != CopCarFlint_IDs.end() || CopCarBone_IDs.find(model) != CopCarBone_IDs.end())
            return 172;
        else if (model == MODEL_COPBIKE || CopBikeLA_IDs.find(model) != CopBikeLA_IDs.end() || CopBikeSF_IDs.find(model) != CopBikeSF_IDs.end() || CopBikeVG_IDs.find(model) != CopBikeVG_IDs.end())
            return 96;
        else if (model == MODEL_FBIRANCH || model == MODEL_FBITRUCK || Fbiranch_IDs.find(model) != Fbiranch_IDs.end())
            return 63;
        else if (model == MODEL_ENFORCER || model == MODEL_SWATVAN || Enforcer_IDs.find(model) != Enforcer_IDs.end())
            return 0;
        else if (model == MODEL_BARRACKS || model == MODEL_PATRIOT || Barracks_IDs.find(model) != Barracks_IDs.end())
            return 6;
        return model - 427;
    }

    static int __stdcall GetModel_Patch_42BBC8(unsigned int model) {
        if (model == MODEL_COPCARLA || CopCarLA_IDs.find(model) != CopCarLA_IDs.end())
            return 189;
        else if (model == MODEL_COPCARSF || CopCarSF_IDs.find(model) != CopCarSF_IDs.end())
            return 190;
        else if (model == MODEL_COPCARVG || CopCarVG_IDs.find(model) != CopCarVG_IDs.end())
            return 191;
        else if (model == MODEL_COPCARRU || CopCarRed_IDs.find(model) != CopCarRed_IDs.end() || CopCarFlint_IDs.find(model) != CopCarFlint_IDs.end() || CopCarBone_IDs.find(model) != CopCarBone_IDs.end())
            return 192;
        else if (model == MODEL_COPBIKE || CopBikeLA_IDs.find(model) != CopBikeLA_IDs.end() || CopBikeSF_IDs.find(model) != CopBikeSF_IDs.end() || CopBikeVG_IDs.find(model) != CopBikeVG_IDs.end())
            return 116;
        else if (model == MODEL_ENFORCER || model == MODEL_SWATVAN || Enforcer_IDs.find(model) != Enforcer_IDs.end())
            return 20;
        else if (model == MODEL_AMBULAN || AmbulanLA_IDs.find(model) != AmbulanLA_IDs.end() || AmbulanSF_IDs.find(model) != AmbulanSF_IDs.end() || AmbulanVG_IDs.find(model) != AmbulanVG_IDs.end())
            return 9;
        else if (model == MODEL_FIRETRUK || model == MODEL_FIRELA || FiretrukLA_IDs.find(model) != FiretrukLA_IDs.end() || FiretrukSF_IDs.find(model) != FiretrukSF_IDs.end() || FiretrukVG_IDs.find(model) != FiretrukVG_IDs.end())
            return 0;
        else if (model == MODEL_FBIRANCH || model == MODEL_FBITRUCK || Fbiranch_IDs.find(model) != Fbiranch_IDs.end())
            return 83;
        else if (model == MODEL_BARRACKS || model == MODEL_PATRIOT || Barracks_IDs.find(model) != Barracks_IDs.end())
            return 26;
        else if (model == MODEL_STREAKC || Streakc_IDs.find(model) != Streakc_IDs.end())
            return 163;
        return model - 407;
    }

    static unsigned int GetRandomModel(unordered_set<unsigned int> IDS) {
        vector<unsigned int> ids;
        for (auto id : IDS)
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }
    
    static bool __fastcall IsLawEnforcementVehicleCheck(CVehicle *_this) {
        bool result = false; 

        if (_this->IsLawEnforcementVehicle() || _this->m_nModelIndex == MODEL_PATRIOT
            || CopCarLA_IDs.find(_this->m_nModelIndex) != CopCarLA_IDs.end()
            || CopCarSF_IDs.find(_this->m_nModelIndex) != CopCarSF_IDs.end()
            || CopCarVG_IDs.find(_this->m_nModelIndex) != CopCarVG_IDs.end()
            || CopCarRed_IDs.find(_this->m_nModelIndex) != CopCarRed_IDs.end()
            || CopCarFlint_IDs.find(_this->m_nModelIndex) != CopCarFlint_IDs.end()
            || CopCarBone_IDs.find(_this->m_nModelIndex) != CopCarBone_IDs.end()
            || CopBikeLA_IDs.find(_this->m_nModelIndex) != CopBikeLA_IDs.end()
            || CopBikeSF_IDs.find(_this->m_nModelIndex) != CopBikeSF_IDs.end()
            || CopBikeVG_IDs.find(_this->m_nModelIndex) != CopBikeVG_IDs.end()
            || Fbiranch_IDs.find(_this->m_nModelIndex) != Fbiranch_IDs.end()
            || Enforcer_IDs.find(_this->m_nModelIndex) != Enforcer_IDs.end()
            || Barracks_IDs.find(_this->m_nModelIndex) != Barracks_IDs.end()
            || Swatvan_IDs.find(_this->m_nModelIndex) != Swatvan_IDs.end())
            result = true;
        return result;
    }

    // CVehicle::UsesSiren
    static bool __fastcall UsesSiren(CVehicle *_this) {
        bool result; 

        if (AmbulanLA_IDs.find(_this->m_nModelIndex) != AmbulanLA_IDs.end()
            || AmbulanSF_IDs.find(_this->m_nModelIndex) != AmbulanSF_IDs.end()
            || AmbulanVG_IDs.find(_this->m_nModelIndex) != AmbulanVG_IDs.end()
            || FiretrukLA_IDs.find(_this->m_nModelIndex) != FiretrukLA_IDs.end()
            || FiretrukSF_IDs.find(_this->m_nModelIndex) != FiretrukSF_IDs.end() 
            || FiretrukVG_IDs.find(_this->m_nModelIndex) != FiretrukVG_IDs.end()) {
            result = true; return result;
        }
        switch (_this->m_nModelIndex) {
        case MODEL_FIRELA:
        case MODEL_FIRETRUK:                        
        case MODEL_AMBULAN:                         
        case MODEL_MRWHOOP:                         
            result = true;
            break;
        case MODEL_RHINO:
            result = false;
            break;
        default:
            result = IsLawEnforcementVehicleCheck(_this) != 0;
            break;
        }
        return result;
    }

    static int __cdecl GetDefaultCopCarModel() {
        switch (m_CurrLevel) {
        case 1:
            if (CStreaming::m_bCopBikeLoaded)
                return GetCurrentVehicleModel(CopBikeLA_IDs, TYPE_COPBIKE);
            else
                return GetCurrentVehicleModel(CopCarLA_IDs, TYPE_COP_CAR);
        case 2:
            if (CStreaming::m_bCopBikeLoaded)
                return GetCurrentVehicleModel(CopBikeSF_IDs, TYPE_COPBIKE);
            else
                return GetCurrentVehicleModel(CopCarSF_IDs, TYPE_COP_CAR);
        case 3:
            if (CStreaming::m_bCopBikeLoaded)
                return GetCurrentVehicleModel(CopBikeVG_IDs, TYPE_COPBIKE);
            else
                return GetCurrentVehicleModel(CopCarVG_IDs, TYPE_COP_CAR);
        case 4: return GetCurrentVehicleModel(CopCarRed_IDs, TYPE_COP_CAR);
        case 5: return GetCurrentVehicleModel(CopCarFlint_IDs, TYPE_COP_CAR);
        case 6: return GetCurrentVehicleModel(CopCarBone_IDs, TYPE_COP_CAR);
        default: return CStreaming::GetDefaultCopCarModel(0);
        }
    }

    // CCarCtrl::ChoosePoliceCarModel
    static int __cdecl ChoosePoliceCarModel(unsigned int a1) {
        CWanted *wanted = FindPlayerWanted(-1);

        if (wanted->AreSwatRequired() 
            && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED 
            && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED) 
        {
            unsigned int enforcerId;
            if (randomSwatCar < 2)
                randomSwatCar++;
            else
                randomSwatCar = 0;
            switch (randomSwatCar) {
            case 0:
                enforcerId = GetRandomModel(Enforcer_IDs);
                if (CModelInfo::IsCarModel(enforcerId) && LoadModel(enforcerId))
                    return enforcerId;
                else
                    return MODEL_ENFORCER;
            case 1: return MODEL_ENFORCER;
            case 2:
                if (LoadModel(MODEL_SWATVAN))
                    return MODEL_SWATVAN;
                else
                    return MODEL_ENFORCER;
            default: return MODEL_ENFORCER;
            }
        }
        else {
            if (wanted->AreFbiRequired() 
                && CStreaming::ms_aInfoForModel[MODEL_FBIRANCH].m_nLoadState == LOADSTATE_LOADED 
                && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
            {
                unsigned int fbiranchId;
                if (randomFbiCar < 2)
                    randomFbiCar++;
                else
                    randomFbiCar = 0;
                switch (randomFbiCar) {
                case 0:
                    fbiranchId = GetRandomModel(Fbiranch_IDs);
                    if (CModelInfo::IsCarModel(fbiranchId) && LoadModel(fbiranchId)) 
                        return fbiranchId;
                    else
                        return MODEL_FBIRANCH;
                case 1: return MODEL_FBIRANCH;
                case 2: 
                    if (LoadModel(MODEL_FBITRUCK))
                        return MODEL_FBITRUCK;
                    else
                        return MODEL_FBIRANCH;
                default: return MODEL_FBIRANCH;
                }
            }
            if (wanted->AreArmyRequired()
                && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == LOADSTATE_LOADED)
            {
                unsigned int barracksId;
                if (randomArmyCar < 3)
                    randomArmyCar++;
                else
                    randomArmyCar = 0;
                switch (randomArmyCar) {
                case 0:
                    barracksId = GetRandomModel(Barracks_IDs);
                    if (CModelInfo::IsCarModel(barracksId) && LoadModel(barracksId)) 
                        return barracksId;
                    else
                        return MODEL_BARRACKS;
                case 1: return MODEL_BARRACKS;
                case 2: return MODEL_RHINO;
                case 3: 
                    if (LoadModel(MODEL_PATRIOT))
                        return MODEL_PATRIOT;
                    else
                        return MODEL_BARRACKS;
                default: return MODEL_BARRACKS;
                }
            }
        }
        return GetDefaultCopCarModel();
    }

    // CPopulation::LoadSpecificDriverModelsForCar
    static void __cdecl LoadSpecificDriverModelsForCar(int model) {
        if (model == MODEL_TAXI || model == MODEL_CABBIE || TaxiLA_IDs.find(model) != TaxiLA_IDs.end()
            || TaxiSF_IDs.find(model) != TaxiSF_IDs.end() || TaxiVG_IDs.find(model) != TaxiVG_IDs.end()) {
            int modelDriver = /*CStreaming::*/GetDefaultCabDriverModel();
            CStreaming::RequestModel(modelDriver, 10);
            return;
        }
        switch (model) {
        case MODEL_BMX:
            CStreaming::RequestModel(MODEL_WMYBMX, 10);
            break;
        case MODEL_PIZZABOY:
            CStreaming::RequestModel(MODEL_WMYPIZZ, 10);
            break;
        case MODEL_FREEWAY:
            CStreaming::RequestModel(MODEL_BIKERA, 10);
            CStreaming::RequestModel(MODEL_BIKERB, 10);
            break;
        case MODEL_STRETCH:
            CStreaming::RequestModel(MODEL_WMYCH, 10);
            break;
        case MODEL_MRWHOOP:
            CStreaming::RequestModel(MODEL_WMOICE, 10);
            break;
        case MODEL_SECURICA:
            CStreaming::RequestModel(MODEL_WMYSGRD, 10);
            break;
        default:
            return;
        }
    }

    // CPopulation::RemoveSpecificDriverModelsForCar
    static void __cdecl RemoveSpecificDriverModelsForCar(int model) {
        if (model == MODEL_TAXI || model == MODEL_CABBIE || TaxiLA_IDs.find(model) != TaxiLA_IDs.end()
            || TaxiSF_IDs.find(model) != TaxiSF_IDs.end() || TaxiVG_IDs.find(model) != TaxiVG_IDs.end()) {
            int modelDriver = /*CStreaming::*/GetDefaultCabDriverModel();
            CStreaming::SetModelIsDeletable(modelDriver);
            CStreaming::SetModelTxdIsDeletable(modelDriver);
            return;
        }

        switch (model) {
        case MODEL_BMX:
            CStreaming::SetModelIsDeletable(MODEL_WMYBMX);
            CStreaming::SetModelTxdIsDeletable(MODEL_WMYBMX);
            break;
        case MODEL_PIZZABOY:
            CStreaming::SetModelIsDeletable(MODEL_WMYPIZZ);
            CStreaming::SetModelTxdIsDeletable(MODEL_WMYPIZZ);
            break;
        default:
            return;
        case MODEL_FREEWAY:
            CStreaming::SetModelIsDeletable(MODEL_BIKERA);
            CStreaming::SetModelTxdIsDeletable(MODEL_BIKERA);
            CStreaming::SetModelIsDeletable(MODEL_BIKERB);
            CStreaming::SetModelTxdIsDeletable(MODEL_BIKERB);
            break;
        case MODEL_STRETCH:
            CStreaming::SetModelIsDeletable(MODEL_WMYCH);
            CStreaming::SetModelTxdIsDeletable(MODEL_WMYCH);
            break;
        case MODEL_MRWHOOP:
            CStreaming::SetModelIsDeletable(MODEL_WMOICE);
            CStreaming::SetModelTxdIsDeletable(MODEL_WMOICE);
            break;
        case MODEL_SECURICA:
            CStreaming::SetModelIsDeletable(MODEL_WMYSGRD);
            CStreaming::SetModelTxdIsDeletable(MODEL_WMYSGRD);
            break;
        }
    }

    // CPopulation::FindSpecificDriverModelForCar_ToUse
    static int __cdecl FindSpecificDriverModelForCar_ToUse(int model) {
        int result; int randomBiker; 
        if (model == MODEL_TAXI || model == MODEL_CABBIE || TaxiLA_IDs.find(model) != TaxiLA_IDs.end()
            || TaxiSF_IDs.find(model) != TaxiSF_IDs.end() || TaxiVG_IDs.find(model) != TaxiVG_IDs.end()) {
            result = /*CStreaming::*/GetDefaultCabDriverModel(); return result;
        }
        switch (model) {
        case MODEL_BMX:
            result = MODEL_WMYBMX;
            break;
        case MODEL_PIZZABOY:
            result = MODEL_WMYPIZZ;
            break;
        case MODEL_FREEWAY:
            randomBiker = plugin::Random(0, 3);
            if (randomBiker) {
                if (randomBiker != 1)
                    goto LABEL_12;
                result = MODEL_BIKERB;
            }
            else
                result = MODEL_BIKERA;
            break;
        case MODEL_STRETCH:
            result = MODEL_WMYCH;
            break;
        case MODEL_MRWHOOP:
            result = MODEL_WMOICE;
            break;
        case MODEL_SECURICA:
            result = MODEL_WMYSGRD;
            break;
        default:
        LABEL_12:
            result = -1;
            break;
        }
        return result;
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

    static bool __stdcall IsCharInAnyPoliceVehicle(CPed *ped) {
        bool inModel = false;

        if (ped && ped->m_nPedFlags.bIsStanding) {
            if (ped && ped->m_pVehicle) {
                if (IsLawEnforcementVehicleCheck(ped->m_pVehicle)) {
                    if (ped->m_pVehicle->m_nModelIndex != MODEL_PREDATOR)
                        inModel = true;
                }
            }
        }
        return inModel;
    }

    // CGarages::IsCarSprayable
    static bool __cdecl IsCarSprayable(CVehicle *vehicle) {
        bool result = true;

        if (IsLawEnforcementVehicleCheck(vehicle) || vehicle->m_nVehicleSubClass == VEHICLE_BMX 
            || vehicle->m_nModelIndex == MODEL_BUS || vehicle->m_nModelIndex == MODEL_COACH 
            || vehicle->m_nModelIndex == MODEL_FIRELA || vehicle->m_nModelIndex == MODEL_AMBULAN 
            || AmbulanLA_IDs.find(vehicle->m_nModelIndex) != AmbulanLA_IDs.end()
            || AmbulanSF_IDs.find(vehicle->m_nModelIndex) != AmbulanSF_IDs.end() 
            || AmbulanVG_IDs.find(vehicle->m_nModelIndex) != AmbulanVG_IDs.end()
            || vehicle->m_nModelIndex == MODEL_FIRETRUK 
            || FiretrukLA_IDs.find(vehicle->m_nModelIndex) != FiretrukLA_IDs.end()
            || FiretrukSF_IDs.find(vehicle->m_nModelIndex) != FiretrukSF_IDs.end() 
            || FiretrukVG_IDs.find(vehicle->m_nModelIndex) != FiretrukVG_IDs.end())
            result = false;
        return result;
    }

    static eWeaponType __stdcall GetCurrentWeaponType(int type) {
        eWeaponType result; unsigned int model;
        switch (type) {
        case 0: model = GetRandomModel(CopWeapon_IDs);  result = WEAPON_PISTOL; break;
        case 1: model = GetRandomModel(SwatWeapon_IDs); result = WEAPON_MICRO_UZI; break;
        case 2: model = GetRandomModel(FbiWeapon_IDs);  result = WEAPON_MP5; break;
        case 3: model = GetRandomModel(ArmyWeapon_IDs); result = WEAPON_M4; break;
        }
        switch (model) {
        case MODEL_GRENADE:
            if (LoadModel(MODEL_GRENADE))
                result = WEAPON_GRENADE;
            break;
        case MODEL_COLT45:
            if (LoadModel(MODEL_COLT45))
                result = WEAPON_PISTOL;
            break;
        case MODEL_SILENCED:
            if (LoadModel(MODEL_SILENCED))
                result = WEAPON_PISTOL_SILENCED;
            break;
        case MODEL_DESERT_EAGLE:
            if (LoadModel(MODEL_DESERT_EAGLE))
                result = WEAPON_DESERT_EAGLE;
            break;
        case MODEL_CHROMEGUN:
            if (LoadModel(MODEL_CHROMEGUN))
                result = WEAPON_SHOTGUN;
            break;
        case MODEL_SAWNOFF:
            if (LoadModel(MODEL_SAWNOFF))
                result = WEAPON_SAWNOFF;
            break;
        case MODEL_SHOTGSPA:
            if (LoadModel(MODEL_SHOTGSPA))
                result = WEAPON_SPAS12;
            break;
        case MODEL_MICRO_UZI:
            if (LoadModel(MODEL_MICRO_UZI))
                result = WEAPON_MICRO_UZI;
            break;
        case MODEL_MP5LNG:
            if (LoadModel(MODEL_MP5LNG))
                result = WEAPON_MP5;
            break;
        case MODEL_AK47:
            if (LoadModel(MODEL_AK47))
                result = WEAPON_AK47;
            break;
        case MODEL_M4:
            if (LoadModel(MODEL_M4))
                result = WEAPON_M4;
            break;
        case MODEL_TEC9:
            if (LoadModel(MODEL_TEC9))
                result = WEAPON_TEC9;
            break;
        case MODEL_CUNTGUN:
            if (LoadModel(MODEL_CUNTGUN))
                result = WEAPON_COUNTRYRIFLE;
            break;
        case MODEL_SNIPER:
            if (LoadModel(MODEL_SNIPER))
                result = WEAPON_SNIPERRIFLE;
            break;
        case MODEL_ROCKETLA:
            if (LoadModel(MODEL_ROCKETLA))
                result = WEAPON_RLAUNCHER;
            break;
        
        case MODEL_MINIGUN:
            if (LoadModel(MODEL_MINIGUN))
                result = WEAPON_MINIGUN;
            break;
        default:
            switch (type) {
            case 0: result = WEAPON_PISTOL; break;
            case 1: result = WEAPON_MICRO_UZI; break;
            case 2: result = WEAPON_MP5;  break;
            case 3: result = WEAPON_M4; break;
            }
        }
        return result;
    }

    static int __stdcall GetWeaponAmmo(int type) {
        int result;
        switch (type) {
        case 16:
        case 35:
            result = 3; 
            break;
        case 22:
        case 23:
        case 24:
            result = 7;
            break;
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
            result = 1000;
            break;
        case 33:
        case 34:
            result = 10;
            break;
        case 38:
            result = 100;
            break;
        default:
            result = 1000; 
            break;
        }
        return result;
    }

    static int __stdcall GetEmergencyServicesModel(int model) {
        int modelReplacement;
        if (CTheZones::m_CurrLevel) {
            if (model == MODEL_TAXI || model == MODEL_CABBIE) {
                if (m_nEmergencyServices && CTimer::m_snTimeInMilliseconds > (m_nGenerateEmergencyServicesTime + m_nTime * 1000)) {
                    m_nGenerateEmergencyServicesTime = CTimer::m_snTimeInMilliseconds;
                    if (isMedic) {
                        isMedic = false;
                        modelReplacement = GetDefaultAmbulanceModel();
                        if (CStreaming::ms_aInfoForModel[modelReplacement].m_nLoadState == LOADSTATE_LOADED)
                            model = modelReplacement;
                    }
                    else {
                        isMedic = true;
                        modelReplacement = GetDefaultFireEngineModel();
                        if (CStreaming::ms_aInfoForModel[modelReplacement].m_nLoadState == LOADSTATE_LOADED)
                            model = modelReplacement;
                    }
                }
                else {
                    modelReplacement = GetDefaultTaxiModel();
                    if (CStreaming::ms_aInfoForModel[modelReplacement].m_nLoadState == LOADSTATE_LOADED)
                        model = modelReplacement;
                }
            }
        }
        return model;
    }

    static int __stdcall GetSpecialModel(eSpecialModel type) {
        int result;
        switch (type) {
        case TYPE_AMBULAN:   result = CStreaming::ms_aDefaultAmbulanceModel[CTheZones::m_CurrLevel];   break;
        case TYPE_MEDIC:     result = CStreaming::ms_aDefaultMedicModel[CTheZones::m_CurrLevel];       break;
        case TYPE_FIRETRUK:  result = CStreaming::ms_aDefaultFireEngineModel[CTheZones::m_CurrLevel];  break;
        case TYPE_FIREMAN:   result = CStreaming::ms_aDefaultFiremanModel[CTheZones::m_CurrLevel];     break;
        case TYPE_COPBIKE:   result = CStreaming::ms_DefaultCopBikeModel;                              break;
        case TYPE_COPBIKER:  result = CStreaming::ms_DefaultCopBikerModel;                             break;
        case TYPE_COP_CAR:   result = CStreaming::ms_aDefaultCopCarModel[CTheZones::m_CurrLevel];      break;
        case TYPE_COP:       result = CStreaming::ms_aDefaultCopModel[CTheZones::m_CurrLevel];         break;
        case TYPE_SWAT:      result = MODEL_SWAT;                                                      break;
        case TYPE_FBI:       result = MODEL_FBI;                                                       break;
        case TYPE_ARMY:      result = MODEL_ARMY;                                                      break;
        case TYPE_TAXI:      result = MODEL_TAXI;                                                      break;
        case TYPE_TAXI_DRIVER:
            if (randomCabDriver < 5)
                randomCabDriver++;
            else
                randomCabDriver = 0;
            if (CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCabDriverModel[randomCabDriver]].m_nLoadState != LOADSTATE_LOADED)
                LoadModel(CStreaming::ms_aDefaultCabDriverModel[randomCabDriver]);
            result = CStreaming::ms_aDefaultCabDriverModel[randomCabDriver];
            break;
        }
        return result;
    }

    static int __stdcall GetCurrentPedModel(unordered_set<unsigned int> IDs, eSpecialModel type) {
        if (IDs.size()) {
            int pedId = GetRandomModel(IDs);
            modelInfo = CModelInfo::ms_modelInfoPtrs[pedId];
            if (modelInfo && modelInfo->GetModelType() == MODEL_INFO_PED) {
                if (CStreaming::ms_aInfoForModel[pedId].m_nLoadState == LOADSTATE_LOADED)
                    return pedId;
                else {
                    if (LoadModel(pedId))
                        return pedId;
                    else
                        return GetSpecialModel(type);
                }
            }
            else
                return GetSpecialModel(type);
        }
        else
            return GetSpecialModel(type);
    }

    static int __stdcall GetCurrentVehicleModel(unordered_set<unsigned int> IDs, eSpecialModel type) {
        int vehicleId = GetRandomModel(IDs);
        if (CModelInfo::IsCarModel(vehicleId) || CModelInfo::IsBikeModel(vehicleId)) {
            if (CStreaming::ms_aInfoForModel[vehicleId].m_nLoadState == LOADSTATE_LOADED)
                return vehicleId;
            else {
                if (LoadModel(vehicleId))
                    return vehicleId;
                else
                    return GetSpecialModel(type);
            }
        }
        else
            return GetSpecialModel(type);
    }

    static int __stdcall GetDefaultCopModel() {
        switch (m_CurrLevel) {
        case 1: return GetCurrentPedModel(CopLA_IDs, TYPE_COP);
        case 2: return GetCurrentPedModel(CopSF_IDs, TYPE_COP);
        case 3: return GetCurrentPedModel(CopVG_IDs, TYPE_COP);
        case 4: return GetCurrentPedModel(CopRed_IDs, TYPE_COP);
        case 5: return GetCurrentPedModel(CopFlint_IDs, TYPE_COP);
        case 6: return GetCurrentPedModel(CopBone_IDs, TYPE_COP);
        default: return CStreaming::GetDefaultCopModel();
        }
    }

    static int __stdcall GetDefaultCopBikerModel() {
        switch (CTheZones::m_CurrLevel) {
        default:
        case 0:  return CStreaming::ms_DefaultCopBikerModel;
        case 1:  return GetCurrentPedModel(CopBikerLA_IDs, TYPE_COPBIKER);
        case 2:  return GetCurrentPedModel(CopBikerSF_IDs, TYPE_COPBIKER);
        case 3:  return GetCurrentPedModel(CopBikerVG_IDs, TYPE_COPBIKER);
        }
    }

    // CStreaming::GetDefaultMedicModel
    static int __stdcall GetDefaultMedicModel() {
        switch (CTheZones::m_CurrLevel) {
        default:
        case 0:  return CStreaming::ms_aDefaultMedicModel[CTheZones::m_CurrLevel];
        case 1:  return GetCurrentPedModel(MedicLA_IDs, TYPE_MEDIC);
        case 2:  return GetCurrentPedModel(MedicSF_IDs, TYPE_MEDIC);
        case 3:  return GetCurrentPedModel(MedicVG_IDs, TYPE_MEDIC);
        }
    }
    
    // CStreaming::GetDefaultFiremanModel
    static int __stdcall GetDefaultFiremanModel() {
        switch (CTheZones::m_CurrLevel) {
        default:
        case 0:  return CStreaming::ms_aDefaultFiremanModel[CTheZones::m_CurrLevel];
        case 1:  return GetCurrentPedModel(FiremanLA_IDs, TYPE_FIREMAN);
        case 2:  return GetCurrentPedModel(FiremanSF_IDs, TYPE_FIREMAN);
        case 3:  return GetCurrentPedModel(FiremanVG_IDs, TYPE_FIREMAN);
        }
    }

    // CStreaming::GetDefaultFireEngineModel
    static int __stdcall GetDefaultFireEngineModel() {
        switch (CTheZones::m_CurrLevel) {
        default:
        case 0:  return CStreaming::ms_aDefaultFireEngineModel[CTheZones::m_CurrLevel];
        case 1:  return GetCurrentVehicleModel(FiretrukLA_IDs, TYPE_FIRETRUK);
        case 2:  return GetCurrentVehicleModel(FiretrukSF_IDs, TYPE_FIRETRUK);
        case 3:  return GetCurrentVehicleModel(FiretrukVG_IDs, TYPE_FIRETRUK);
        }
    }

    // CStreaming::GetDefaultAmbulanceModel
    static int __stdcall GetDefaultAmbulanceModel() {
        switch (CTheZones::m_CurrLevel) {
        default:
        case 0:  return CStreaming::ms_aDefaultAmbulanceModel[CTheZones::m_CurrLevel];
        case 1:  return GetCurrentVehicleModel(AmbulanLA_IDs, TYPE_AMBULAN);
        case 2:  return GetCurrentVehicleModel(AmbulanSF_IDs, TYPE_AMBULAN);
        case 3:  return GetCurrentVehicleModel(AmbulanVG_IDs, TYPE_AMBULAN);
        }
    }

    // CStreaming::GetDefaultCabDriverModel
    static int __cdecl GetDefaultCabDriverModel() {
        int cabDriverModel;
        if (isCabDriver) {
            cabDriverModel = GetSpecialModel(TYPE_TAXI_DRIVER); isCabDriver = false;
        }
        else {
            cabDriverModel = GetCurrentPedModel(TaxiDriver_IDs, TYPE_TAXI_DRIVER); isCabDriver = true;
        }
        return cabDriverModel;
    }

    static int __stdcall GetDefaultTaxiModel() {
        switch (CTheZones::m_CurrLevel) {
        default:
        case 0:  return MODEL_TAXI;
        case 1:  return GetCurrentVehicleModel(TaxiLA_IDs, TYPE_TAXI);
        case 2:  return GetCurrentVehicleModel(TaxiSF_IDs, TYPE_TAXI);
        case 3:  return GetCurrentVehicleModel(TaxiVG_IDs, TYPE_TAXI);
        }
    }

    static void __stdcall ConstructorCopPed(CCopPed *cop, eCopType copType) {
        int copModel; CWanted *wanted = FindPlayerWanted(-1);
        cop->m_copType = copType;
        switch (copType) {
        default:
        case COP_TYPE_CITYCOP:
            copModel = GetDefaultCopModel();
            goto LABEL_COP;
        case COP_TYPE_CSHER:
            copModel = MODEL_CSHER;
        LABEL_COP:
            cop->SetModelIndex(copModel);
            currentWeaponType = GetCurrentWeaponType(0);
            weaponAmmo = GetWeaponAmmo(currentWeaponType);
            cop->GiveWeapon(currentWeaponType, weaponAmmo, true);
            cop->GiveWeapon(WEAPON_NIGHTSTICK, 1000, true);
            cop->GiveWeapon(WEAPON_PISTOL, 1000, true);
            if (wanted->m_nWantedLevel > 1)
                cop->SetCurrentWeapon(currentWeaponType);
            else
                cop->m_nActiveWeaponSlot = 0;
            cop->m_fArmour = 0.0f;
            cop->m_nWeaponShootingRate = 30;
            cop->m_nWeaponAccuracy = 60;
            break;
        case COP_TYPE_LAPDM1:
            copModel = GetDefaultCopBikerModel();
            cop->SetModelIndex(copModel);
            cop->GiveWeapon(WEAPON_NIGHTSTICK, 1000, 1);
            cop->GiveDelayedWeapon(WEAPON_PISTOL, 1000);
            cop->m_nActiveWeaponSlot = 0;
            cop->m_fArmour = 0.0f;
            cop->m_nWeaponShootingRate = 30;
            cop->m_nWeaponAccuracy = 60;
            break;
        case COP_TYPE_SWAT1:
        case COP_TYPE_SWAT2:
            if (isSwat) {
                copModel = MODEL_SWAT; isSwat = false;
            }
            else {
                copModel = GetCurrentPedModel(Swat_IDs, TYPE_SWAT); isSwat = true;
            }
            cop->SetModelIndex(copModel);
            currentWeaponType = GetCurrentWeaponType(1);
            weaponAmmo = GetWeaponAmmo(currentWeaponType);
            cop->GiveWeapon(currentWeaponType, weaponAmmo, true);
            cop->GiveWeapon(WEAPON_MICRO_UZI, 1000, true);
            cop->SetCurrentWeapon(currentWeaponType);
            cop->m_fArmour = 50.0f;
            cop->m_nWeaponShootingRate = 70;
            cop->m_nWeaponAccuracy = 68;
            break;
        case COP_TYPE_FBI:
            if (isFbi) {
                copModel = MODEL_FBI; isFbi = false;
            }
            else {
                copModel = GetCurrentPedModel(Fbi_IDs, TYPE_FBI); isFbi = true;
            }
            cop->SetModelIndex(copModel);
            currentWeaponType = GetCurrentWeaponType(2);
            weaponAmmo = GetWeaponAmmo(currentWeaponType);
            cop->GiveWeapon(currentWeaponType, weaponAmmo, true);
            cop->GiveWeapon(WEAPON_MP5, 1000, true);
            cop->SetCurrentWeapon(currentWeaponType);
            cop->m_fArmour = 100.0f;
            cop->m_nWeaponShootingRate = 60;
            cop->m_nWeaponAccuracy = 76;
            break;
        case COP_TYPE_ARMY:
            if (isArmy) {
                copModel = MODEL_ARMY; isArmy = false;
            }
            else {
                copModel = GetCurrentPedModel(Army_IDs, TYPE_ARMY); isArmy = true;
            }
            cop->SetModelIndex(copModel);
            currentWeaponType = GetCurrentWeaponType(3);
            weaponAmmo = GetWeaponAmmo(currentWeaponType);
            cop->GiveWeapon(currentWeaponType, weaponAmmo, true);
            cop->GiveWeapon(WEAPON_M4, 1000, true);
            cop->SetCurrentWeapon(currentWeaponType);
            cop->m_fArmour = 100.0f;
            cop->m_nWeaponShootingRate = 80;
            cop->m_nWeaponAccuracy = 84;
            break;
        }
    }

    static void SetSiren(CVehicle *vehicle) {
        if (plugin::Random(0, 1)) {
            vehicle->m_nVehicleFlags.bSirenOrAlarm = true;
            vehicle->m_autoPilot.m_nCarDrivingStyle = DRIVINGSTYLE_AVOID_CARS;
            vehicle->m_autoPilot.m_nCruiseSpeed = 25;
        }
        else
            vehicle->m_nVehicleFlags.bSirenOrAlarm = false;
    }

    static int __stdcall GenerateEmergencyCar(CVehicle *vehicle) {
        int modelIndex = vehicle->m_nModelIndex;
        if (AmbulanLA_IDs.find(modelIndex) != AmbulanLA_IDs.end()
            || AmbulanSF_IDs.find(modelIndex) != AmbulanSF_IDs.end()
            || AmbulanVG_IDs.find(modelIndex) != AmbulanVG_IDs.end()) {
            CCarAI::AddAmbulanceOccupants(vehicle);
            SetSiren(vehicle);
            return 416;
        }
        else if (FiretrukLA_IDs.find(modelIndex) != FiretrukLA_IDs.end()
            || FiretrukSF_IDs.find(modelIndex) != FiretrukSF_IDs.end()
            || FiretrukVG_IDs.find(modelIndex) != FiretrukVG_IDs.end()) {
            CCarAI::AddFiretruckOccupants(vehicle);
            SetSiren(vehicle);
            return 416;
        }
        return modelIndex;
    }

    AddSpecialCars() {
        ifstream stream(PLUGIN_PATH("SpecialCars.dat"));
        if (!stream.is_open())
            return;
        for (string line; getline(stream, line); ) {
            if (!line.compare("copcarla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopCarLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copcarsf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopCarSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copcarvg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopCarVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copcarred")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopCarRed_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copcarflint")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopCarFlint_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copcarbone")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopCarBone_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copbikela")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopBikeLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copbikesf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopBikeSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copbikevg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopBikeVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("fbiranch")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Fbiranch_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("enforcer")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Enforcer_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("taxila")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        TaxiLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("taxisf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        TaxiSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("taxivg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        TaxiVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("taxidriver")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        TaxiDriver_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("ambulanla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        AmbulanLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("ambulansf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        AmbulanSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("ambulanvg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        AmbulanVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("firetrukla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        FiretrukLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("firetruksf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        FiretrukSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("firetrukvg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        FiretrukVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("boxburg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Boxburg_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("barracks")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Barracks_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("swatvan")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Swatvan_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("broadway")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Broadway_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("streak")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Streak_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("streakc")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Streakc_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copsf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copvg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copred")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopRed_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copflint")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopFlint_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copbone")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopBone_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copbikerla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopBikerLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copbikersf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopBikerSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copbikervg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopBikerVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("swat")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Swat_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("fbi")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Fbi_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("army")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Army_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("copweapon")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        CopWeapon_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("swatweapon")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        SwatWeapon_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("fbiweapon")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        FbiWeapon_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("armyweapon")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        ArmyWeapon_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("medicla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        MedicLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("medicsf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        MedicSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("medicvg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        MedicVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("firemanla")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        FiremanLA_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("firemansf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        FiremanSF_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("firemanvg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        FiremanVG_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("emergency")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        m_nEmergencyServices = stoi(line);
                }
            }
            if (!line.compare("time")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        m_nTime = stoi(line);
                }
            }
        }
        
        if (!CopCarLA_IDs.size() || !CopCarSF_IDs.size() || !CopCarVG_IDs.size() 
            || !CopCarRed_IDs.size() || !CopCarFlint_IDs.size() || !CopCarBone_IDs.size() 
            || !CopBikeLA_IDs.size() || !CopBikeSF_IDs.size() || !CopBikeVG_IDs.size() 
            || !AmbulanLA_IDs.size() || !AmbulanSF_IDs.size() || !AmbulanVG_IDs.size()
            || !FiretrukLA_IDs.size() || !FiretrukSF_IDs.size() || !FiretrukVG_IDs.size() 
            || !CopLA_IDs.size() || !CopSF_IDs.size() || !CopVG_IDs.size()
            || !CopRed_IDs.size() || !CopFlint_IDs.size() || !CopBone_IDs.size()
            || !CopBikerLA_IDs.size() || !CopBikerSF_IDs.size() || !CopBikerVG_IDs.size()
            || !MedicLA_IDs.size() || !MedicSF_IDs.size() || !MedicVG_IDs.size()
            || !FiremanLA_IDs.size() || !FiremanSF_IDs.size() || !FiremanVG_IDs.size()
            || !TaxiLA_IDs.size() || !TaxiSF_IDs.size() || !TaxiVG_IDs.size())
            return;

        patch::RedirectJump(0x6D8470, UsesSiren);
        patch::RedirectJump(0x6117F0, LoadSpecificDriverModelsForCar);
        patch::RedirectJump(0x6119D0, RemoveSpecificDriverModelsForCar);
        patch::RedirectJump(0x611900, FindSpecificDriverModelForCar_ToUse);
        patch::RedirectJump(0x421980, ChoosePoliceCarModel);
        patch::RedirectJump(0x4479A0, IsCarSprayable);
        patch::RedirectJump(0x407D20, GetDefaultMedicModel);
        patch::RedirectJump(0x407D40, GetDefaultFiremanModel);
        patch::RedirectJump(0x407D30, GetDefaultAmbulanceModel);
        patch::RedirectJump(0x407DC0, GetDefaultFireEngineModel);
        patch::RedirectJump(0x407D50, GetDefaultCabDriverModel);

        patch::RedirectJump(0x6AB349, Patch_6AB349);
        patch::RedirectJump(0x4912D0, Patch_4912D0); 
        patch::RedirectJump(0x469629, Patch_469629);
        patch::RedirectJump(0x6ACA57, Patch_6ACA57);
        patch::RedirectJump(0x6B1F4F, Patch_6B1F4F);
        patch::RedirectJump(0x6ACA51, Patch_6ACA51);
        patch::RedirectJump(0x41C0A6, Patch_41C0A6);
        patch::RedirectJump(0x42BBC8, Patch_42BBC8);
        patch::RedirectJump(0x613A68, Patch_613A68);
        patch::RedirectJump(0x6BD408, Patch_6BD408);
        patch::RedirectJump(0x46130F, Patch_46130F);
        patch::RedirectJump(0x48DA65, Patch_48DA65);
        patch::RedirectJump(0x5DDC99, Patch_5DDC99);
        patch::RedirectJump(0x461BED, Patch_461BED);
        patch::RedirectJump(0x461C0A, Patch_461C0A);
        patch::RedirectJump(0x43069E, Patch_43069E); 
        patch::RedirectJump(0x431EC0, Patch_431EC0);

        patch::RedirectCall(0x42CDDD, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x42DC19, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x42DD23, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x43DFCA, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x478635, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x479A28, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x4862B8, IsLawEnforcementVehicleCheck);
        //patch::RedirectCall(0x48DA81, IsLawEnforcementVehicleCheck); // opcode_056C actor %1d% driving_police_car
        patch::RedirectCall(0x562D97, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x63E6BA, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x6445FC, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x647E48, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x64BD61, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x64C29F, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x651145, IsLawEnforcementVehicleCheck);
        patch::RedirectCall(0x6B11C2, IsLawEnforcementVehicleCheck);
        
        //patch::SetChar(0x42F9FB, 6, true);

        Events::gameProcessEvent += [] {
            /*KeyCheck::Update();
            if (KeyCheck::CheckWithDelay('P', 1000)) {
                if (CTimer::m_UserPause)
                    CTimer::m_UserPause = false;
                else
                    CTimer::m_UserPause = true;
            }*/

            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                    if (vehicle->m_nModelIndex == MODEL_TAXI || vehicle->m_nModelIndex == MODEL_CABBIE || TaxiLA_IDs.find(vehicle->m_nModelIndex) != TaxiLA_IDs.end() || TaxiSF_IDs.find(vehicle->m_nModelIndex) != TaxiSF_IDs.end() || TaxiVG_IDs.find(vehicle->m_nModelIndex) != TaxiVG_IDs.end()) {
                        CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                        if (FindPlayerPed()->m_pVehicle == vehicle) {
                            if (!CTheScripts::IsPlayerOnAMission())
                                automobile->SetTaxiLight(false);
                        }
                        else {
                            TaxiInfo &info = taxiInfo.Get(vehicle);
                            if (!vehicle->m_nNumPassengers && info.enabledTaxiLight) {
                                if (vehicle->m_pDriver)
                                    automobile->SetTaxiLight(true);
                            }
                            if (automobile->taxiAvaliable && (vehicle->m_nNumPassengers || vehicle->m_fHealth < 0.1f))
                                automobile->SetTaxiLight(false);
                        }
                    }
                }
            }

            CPlayerPed *player = FindPlayerPed(-1);
            if (player) {
                if (Command<COMMAND_IS_PLAYER_IN_INFO_ZONE>(CWorld::PlayerInFocus, "LA"))
                    m_CurrLevel = 1;
                if (Command<COMMAND_IS_PLAYER_IN_INFO_ZONE>(CWorld::PlayerInFocus, "SF"))
                    m_CurrLevel = 2;
                if (Command<COMMAND_IS_PLAYER_IN_INFO_ZONE>(CWorld::PlayerInFocus, "VE"))
                    m_CurrLevel = 3;
                if (Command<COMMAND_IS_PLAYER_IN_INFO_ZONE>(CWorld::PlayerInFocus, "RED"))
                    m_CurrLevel = 4;
                if (Command<COMMAND_IS_PLAYER_IN_INFO_ZONE>(CWorld::PlayerInFocus, "FLINTC") 
                    || Command<COMMAND_IS_PLAYER_IN_INFO_ZONE>(CWorld::PlayerInFocus, "WHET")
                    || Command<COMMAND_IS_PLAYER_IN_INFO_ZONE>(CWorld::PlayerInFocus, "ROBAD"))
                    m_CurrLevel = 5;
                if (Command<COMMAND_IS_PLAYER_IN_INFO_ZONE>(CWorld::PlayerInFocus, "BONE"))
                    m_CurrLevel = 6;

                CWanted *wanted = FindPlayerWanted(-1);
                // RoadBlocks
                if (CTimer::m_snTimeInMilliseconds > (randomRoadBlocksTime + 30000)) {
                    randomRoadBlocksTime = CTimer::m_snTimeInMilliseconds;
                    if (wanted->AreSwatRequired()) {
                        unsigned int enforcerId = GetRandomModel(Enforcer_IDs);
                        if (CModelInfo::IsCarModel(enforcerId)) {
                            if (patch::GetShort(0x461BE7) == MODEL_ENFORCER || patch::GetShort(0x461BE7) == MODEL_SWATVAN)
                                patch::SetShort(0x461BE7, enforcerId, true);
                            else {
                                if (isEnforcer) {
                                    patch::SetShort(0x461BE7, MODEL_ENFORCER, true); isEnforcer = false;
                                }
                                else {
                                    patch::SetShort(0x461BE7, MODEL_SWATVAN, true); isEnforcer = true;
                                }
                            }
                        }
                    }
                    if (wanted->AreFbiRequired()) {
                        unsigned int fbiranchId = GetRandomModel(Fbiranch_IDs);
                        if (CModelInfo::IsCarModel(fbiranchId)) {
                            if (patch::GetShort(0x461BCC) == MODEL_FBIRANCH || patch::GetShort(0x461BCC) == MODEL_FBITRUCK)
                                patch::SetShort(0x461BCC, fbiranchId, true);
                            else {
                                if (isFbiranch) {
                                    patch::SetShort(0x461BCC, MODEL_FBIRANCH, true); isFbiranch = false;
                                }
                                else {
                                    patch::SetShort(0x461BCC, MODEL_FBITRUCK, true); isFbiranch = true;
                                }
                            }
                        }
                    }
                    if (wanted->AreArmyRequired()) {
                        unsigned int barracksId = GetRandomModel(Barracks_IDs);
                        if (CModelInfo::IsCarModel(barracksId)) {
                            if (patch::GetShort(0x461BB1) == MODEL_BARRACKS || patch::GetShort(0x461BB1) == MODEL_PATRIOT)
                                patch::SetShort(0x461BB1, barracksId, true);
                            else {
                                if (isBarracks) {
                                    patch::SetShort(0x461BB1, MODEL_BARRACKS, true); isBarracks = false;
                                }
                                else {
                                    patch::SetShort(0x461BB1, MODEL_PATRIOT, true); isBarracks = true;
                                }
                            }
                        }
                    }
                }
            }
        };

        //Events::drawingEvent += [] {
        //    gamefont::Print({
        //        /*Format("CopBike load = %d", CStreaming::m_bCopBikeLoaded),*/
        //        Format("level = %d", CTheZones::m_CurrLevel),
        //        Format("my level = %d", m_CurrLevel)
        //        //Format("swatCarBlok = %d", patch::GetShort(0x461BE7)),
        //        //Format("fbiCarBlok = %d", patch::GetShort(0x461BCC)),
        //        //Format("test cab = %d", FindSpecificDriverModelForCar_ToUse(MODEL_TAXI)),
        //        //Format("color = %d, %d, %d", HudColour.m_aColours[12].red, HudColour.m_aColours[12].green, HudColour.m_aColours[12].blue),
        //    }, 10, 300, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
        //};

        Events::drawMenuBackgroundEvent += [] {
            CFont::SetScale(ScreenCoord(0.75f), ScreenCoord(1.5f));
            CFont::SetColor(CRGBA(172, 203, 241, 255));
            CFont::SetOrientation(ALIGN_LEFT);
            //CFont::SetEdge(1);
            //CFont::SetDropColor(CRGBA(0, 0, 255, 255));
            CFont::SetBackground(false, false);
            CFont::SetFontStyle(FONT_GOTHIC);
            CFont::SetProportional(true);
            CFont::SetWrapx(600.0f);
            static char text[64];
            sprintf(text, "special service by kenking");
            CFont::PrintString(ScreenCoord(5.0f), ScreenCoord(5.0f), text);
        };

    }
} specialCars;

VehicleExtendedData<AddSpecialCars::TaxiInfo> AddSpecialCars::taxiInfo;
AddSpecialCars::eSpawnCarState AddSpecialCars::m_currentState = STATE_FIND;
short AddSpecialCars::outCount = 0;
CVector AddSpecialCars::carPos = { 0.0f, 0.0f, 0.0f };
float AddSpecialCars::carAngle = 0.0f;
CAutoPilot AddSpecialCars::pilot;
CBaseModelInfo* AddSpecialCars::modelInfo;
int AddSpecialCars::currentModelForSiren;
int AddSpecialCars::currentModelCopbike;
int AddSpecialCars::currentModelTaxi;
int AddSpecialCars::currentModelFiretruk;
int AddSpecialCars::currentWaterJetsModel;
int AddSpecialCars::currentTurretsModel;
int AddSpecialCars::currentModel;
int AddSpecialCars::currentModel_Patch_41C0A6;
int AddSpecialCars::currentModel_Patch_42BBC8;
int AddSpecialCars::currentModel_Patch_613A68;
int AddSpecialCars::currentModel_Patch_46130F;
int AddSpecialCars::currentModel_Patch_48DA65;
int AddSpecialCars::currentModel_Patch_461BED;
int AddSpecialCars::currentModel_Patch_43069E;
int AddSpecialCars::currentModel_Patch_431EC0;
unsigned int AddSpecialCars::jmp_6AB360;
unsigned int AddSpecialCars::jmp_469658;
unsigned int AddSpecialCars::jmp_41C0AF;
unsigned int AddSpecialCars::jmp_42BBCE;
unsigned int AddSpecialCars::jmp_613A71;
unsigned int AddSpecialCars::jmp_6BD415;
unsigned int AddSpecialCars::jmp_48DAA2;
eWeaponType AddSpecialCars::currentWeaponType;

void __declspec(naked) AddSpecialCars::Patch_6AB349() { // Siren
    __asm {
        mov     ecx, esi
        call    CVehicle::AddDamagedVehicleParticles
        movsx   eax, word ptr[esi + 0x22]
        pushad
        push eax
        call GetModelForSiren
        mov currentModelForSiren, eax
        popad
        mov eax, currentModelForSiren
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
        call GetTaxiModel
        mov currentModelTaxi, eax
        popad
        cmp ecx, currentModelTaxi
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

void __declspec(naked) AddSpecialCars::Patch_6ACA57() { // Firetruk
    __asm {
        CWDE
        mov ecx, 407
        pushad
        push eax
        call GetFiretrukModel
        mov currentModelFiretruk, eax
        popad
        cmp ecx, currentModelFiretruk
        jnz SET_FALSE
        mov ecx, 0x6ACA5D
        jmp ecx
            SET_FALSE :
            mov ecx, 0x6ACA8D
            jmp ecx
    }
}

void __declspec(naked) AddSpecialCars::Patch_6B1F4F() { // firetruk and svatvan water jets
    __asm {
        CWDE
        mov ecx, 407
        pushad
        push eax
        call GetWaterJetsModel
        mov currentWaterJetsModel, eax
        popad
        cmp ecx, currentWaterJetsModel
        jz SET_TRUE
        jmp END_CHECK
        SET_TRUE :
        mov ecx, 0x6B1F5B
        jmp ecx
            END_CHECK :
            mov ecx, 0x6B1F77
            jmp ecx
    }
}

void __declspec(naked) AddSpecialCars::Patch_6ACA51() { // swatvan turrets
    __asm {
        CWDE
        mov ecx, 601
        pushad
        push eax
        call GetTurretsModel
        mov currentTurretsModel, eax
        popad
        cmp ecx, currentTurretsModel
        jz SET_TRUE
        jmp END_CHECK
        SET_TRUE :
        mov ecx, 0x6ACAAE
            jmp ecx
            END_CHECK :
        mov ecx, 0x6ACA57
            jmp ecx
    }
}

void __declspec(naked) AddSpecialCars::Patch_41C0A6() { // AddPoliceCarOccupants
    __asm {
        movsx eax, word ptr[esi + 22h]
        pushad
        push eax
        call GetModel_Patch_41C0A6
        mov currentModel_Patch_41C0A6, eax
        popad
        mov eax, currentModel_Patch_41C0A6
        mov jmp_41C0AF, 0x41C0AF
        jmp jmp_41C0AF
    }
}

void __declspec(naked) AddSpecialCars::Patch_42BBC8() { 
    __asm {
        mov eax, ebx
        pushad
        push eax
        call GetModel_Patch_42BBC8
        mov currentModel_Patch_42BBC8, eax
        popad
        mov eax, currentModel_Patch_42BBC8
        mov jmp_42BBCE, 0x42BBCE
        jmp jmp_42BBCE
    }
}

void __declspec(naked) AddSpecialCars::Patch_613A68() { // AddPedInCar
    __asm {
        movsx eax, word ptr[edi + 22h]
        pushad
        push eax
        call GetModel_Patch_42BBC8
        mov currentModel_Patch_613A68, eax
        popad
        mov eax, currentModel_Patch_613A68
        mov jmp_613A71, 0x613A71
        jmp jmp_613A71
    }
}

void __declspec(naked) AddSpecialCars::Patch_6BD408() { // Siren copbike
    __asm {
        mov  ecx, esi
        call  CVehicle::AddDamagedVehicleParticles
        movsx eax, word ptr[esi + 22h]
        pushad
        push eax
        call GetCopbikeModel
        mov currentModelCopbike, eax
        popad
        cmp currentModelCopbike, 523
        mov jmp_6BD415, 0x6BD415
        jmp jmp_6BD415
    }
}

void __declspec(naked) AddSpecialCars::Patch_46130F() { // GenerateRoadBlockCopsForCar
    __asm {
        movsx   eax, word ptr [edi+22h]
        pushad
        push eax
        call GetModel_Patch_46130F
        mov currentModel_Patch_46130F, eax
        popad
        mov eax, currentModel_Patch_46130F
        mov ecx, 0x461318
        jmp ecx
    }
}

void __declspec(naked) AddSpecialCars::Patch_48DA65() { // IsCharInAnyPoliceVehicle
    __asm {
        pushad
        push eax
        call IsCharInAnyPoliceVehicle
        mov currentModel_Patch_48DA65, eax
        popad
        push currentModel_Patch_48DA65
        mov jmp_48DAA2, 0x48DAA2
        jmp jmp_48DAA2
    }
}

void __declspec(naked) AddSpecialCars::Patch_5DDC99() { // CCopPed::CCopPed
    __asm {
        pushad
        push eax
        push esi
        call ConstructorCopPed
        popad
        mov edx, 0x5DDCED
        jmp edx
    }
}

void __declspec(naked) AddSpecialCars::Patch_461BED() {
    __asm {
        pushad
        call GetDefaultCopCarModel
        mov currentModel_Patch_461BED, eax
        popad
        mov eax, currentModel_Patch_461BED
        mov ecx, 0x461BF6
        jmp ecx
    }
}

void __declspec(naked) AddSpecialCars::Patch_461C0A() {
    __asm {
        pushad
        call GetDefaultCopCarModel
        mov currentModel_Patch_461BED, eax
        popad
        mov eax, currentModel_Patch_461BED
        mov ebp, eax
        mov ecx, 0x461C15
        jmp ecx
    }
}

void __declspec(naked) AddSpecialCars::Patch_43069E() {
    __asm {
        mov  eax, edi
        pushad
        push eax
        call GetEmergencyServicesModel
        mov currentModel_Patch_43069E, eax
        popad
        mov eax, currentModel_Patch_43069E
        push 1
        push eax
        call CCarCtrl::GetNewVehicleDependingOnCarModel
        mov ecx, 0x4306A6
        jmp ecx
    }
}

void __declspec(naked) AddSpecialCars::Patch_431EC0() {
    __asm {
        mov ecx, 416
        pushad
        push esi
        call GenerateEmergencyCar
        mov currentModel_Patch_431EC0, eax
        popad
        cmp ecx, currentModel_Patch_431EC0
        jz SET_TRUE
        push    99
        push    0
        push    0
        push    0
        mov ecx, 0x431EC8
        jmp ecx
        SET_TRUE :
            mov ecx, 0x431EE0
            jmp ecx
    }
}
