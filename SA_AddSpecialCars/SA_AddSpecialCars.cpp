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
#include "CTimer.h"

#include "CTaskComplexCopInCar.h"
#include "CTaskSimpleCarSetPedOut.h"
#include "CTaskComplexKillPedFromBoat.h"
#include "CPedIntelligence.h"

void CTaskSimpleCarSetPedOut__m1C(CTask *_this, CPed *ped) {
    ((void(__thiscall *)(CTask*, CPed*))0x647D10)(_this, ped);
}

using namespace plugin;
using namespace std;

class AddSpecialCars {
public:
    static int currentModelForSiren;
    static int currentModelTaxi;
    static int currentModelFiretruk;
    static int currentModel;
    static int currentModel_Patch_41C0A6;
    static int currentModel_Patch_42BBC8;
    static int currentModel_Patch_613A68;
    static unsigned int randomCopCarTime;
    static unsigned int jmp_6AB360;
    static unsigned int jmp_469658;
    static unsigned int jmp_41C0AF;
    static unsigned int jmp_42BBCE;
    static unsigned int jmp_613A71;

    static void Patch_6AB349(); // Siren
    static void Patch_4912D0(); // Taxi
    static void Patch_469629(); // IsCharInModel
    static void Patch_6ACA57(); // Firetruk
    static void Patch_6B1F4F(); // Firetruk
    static void Patch_41C0A6(); // AddPoliceCarOccupants
    static void Patch_42BBC8();
    static void Patch_613A68();

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

    static unordered_set<unsigned int> &GetCopcarruModels() {
        static unordered_set<unsigned int> copcarruIds;
        return copcarruIds;
    }

    static unordered_set<unsigned int> &GetCopbikeModels() {
        static unordered_set<unsigned int> copbikeIds;
        return copbikeIds;
    }

    static unordered_set<unsigned int> &GetFbiranchModels() {
        static unordered_set<unsigned int> fbiranchIds;
        return fbiranchIds;
    }

    static unordered_set<unsigned int> &GetEnforcerModels() {
        static unordered_set<unsigned int> enforcerIds;
        return enforcerIds;
    }

    static unordered_set<unsigned int> &GetTaxiModels() {
        static unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static unordered_set<unsigned int> &GetAmbulanModels() {
        static unordered_set<unsigned int> ambulanIds;
        return ambulanIds;
    }

    static unordered_set<unsigned int> &GetFiretrukModels() {
        static unordered_set<unsigned int> firetrukIds;
        return firetrukIds;
    }

    static unordered_set<unsigned int> &GetBoxburgModels() {
        static unordered_set<unsigned int> boxburgIds;
        return boxburgIds;
    }

    static int __stdcall GetModelForSiren(unsigned int model) {
        if (model == MODEL_COPCARLA || GetCopcarlaModels().find(model) != GetCopcarlaModels().end())
            return MODEL_COPCARLA;
        else if (model == MODEL_COPCARSF || GetCopcarsfModels().find(model) != GetCopcarsfModels().end())
            return MODEL_COPCARSF;
        else if (model == MODEL_COPCARVG || GetCopcarvgModels().find(model) != GetCopcarvgModels().end())
            return MODEL_COPCARVG;
        else if (model == MODEL_COPCARRU || GetCopcarruModels().find(model) != GetCopcarruModels().end())
            return MODEL_COPCARRU;
        else if (model == MODEL_COPBIKE || GetCopbikeModels().find(model) != GetCopbikeModels().end())
            return MODEL_COPBIKE;
        else if (model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end())
            return MODEL_FBIRANCH;
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            return MODEL_ENFORCER;
        else if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            return MODEL_AMBULAN;
        else if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end())
            return MODEL_FIRETRUK;
        return model;
    }

    static int __stdcall GetTaxiModel(unsigned int model) {
        if (model == MODEL_TAXI || model == MODEL_CABBIE || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        return model;
    }

    static int __stdcall GetFiretrukModel(unsigned int model) {
        if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end())
            return MODEL_FIRETRUK;
        return model;
    }

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
                else if (CTheScripts::ScriptParams[1].uParam == MODEL_FIRETRUK) {
                    if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end()) // Firefighter
                        inModel = true;
                }
                else if (model == CTheScripts::ScriptParams[1].uParam)
                    inModel = true;
            }
        }
        return inModel;
    }
    
    static int __stdcall GetModel_Patch_41C0A6(unsigned int model) {
        if (model == MODEL_COPCARLA || GetCopcarlaModels().find(model) != GetCopcarlaModels().end())
            return 169;
        else if (model == MODEL_COPCARSF || GetCopcarsfModels().find(model) != GetCopcarsfModels().end())
            return 170;
        else if (model == MODEL_COPCARVG || GetCopcarvgModels().find(model) != GetCopcarvgModels().end())
            return 171;
        else if (model == MODEL_COPCARRU || GetCopcarruModels().find(model) != GetCopcarruModels().end())
            return 172;
        else if (model == MODEL_COPBIKE || GetCopbikeModels().find(model) != GetCopbikeModels().end())
            return 96;
        else if (model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end())
            return 63;
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            return 0;
        return model - 427;
    }

    static int __stdcall GetModel_Patch_42BBC8(unsigned int model) {
        if (model == MODEL_COPCARLA || GetCopcarlaModels().find(model) != GetCopcarlaModels().end())
            return 189;
        else if (model == MODEL_COPCARSF || GetCopcarsfModels().find(model) != GetCopcarsfModels().end())
            return 190;
        else if (model == MODEL_COPCARVG || GetCopcarvgModels().find(model) != GetCopcarvgModels().end())
            return 191;
        else if (model == MODEL_COPCARRU || GetCopcarruModels().find(model) != GetCopcarruModels().end())
            return 192;
        else if (model == MODEL_COPBIKE || GetCopbikeModels().find(model) != GetCopbikeModels().end())
            return 116;
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            return 20;
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            return 9;
        else if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end())
            return 0;
        return model - 407;
    }

    static unsigned int GetRandomCopcarla() {
        vector<unsigned int> ids;
        for (auto id : GetCopcarlaModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomCopcarsf() {
        vector<unsigned int> ids;
        for (auto id : GetCopcarsfModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomCopcarvg() {
        vector<unsigned int> ids;
        for (auto id : GetCopcarvgModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomCopcarru() {
        vector<unsigned int> ids;
        for (auto id : GetCopcarruModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomCopbike() {
        vector<unsigned int> ids;
        for (auto id : GetCopbikeModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }
    
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
        else if (GetCopcarruModels().find(_this->m_nModelIndex) != GetCopcarruModels().end()) {
            result = true; return result;
        }
        else if (GetCopbikeModels().find(_this->m_nModelIndex) != GetCopbikeModels().end()) {
            result = true; return result;
        }
        else if (GetFbiranchModels().find(_this->m_nModelIndex) != GetFbiranchModels().end()) {
            result = true; return result;
        }
        else if (GetEnforcerModels().find(_this->m_nModelIndex) != GetEnforcerModels().end()) {
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
        if (GetFiretrukModels().find(_this->m_nModelIndex) != GetFiretrukModels().end()) {
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
        
        if (CTimer::m_snTimeInMilliseconds > (randomCopCarTime + 30000)) {
            randomCopCarTime = CTimer::m_snTimeInMilliseconds;
            unsigned int copbikeId = GetRandomCopbike();
            if (copbikeId != 0) {
                if (CStreaming::ms_DefaultCopBikeModel == MODEL_COPBIKE)
                    CStreaming::ms_DefaultCopBikeModel = copbikeId;
                else
                    CStreaming::ms_DefaultCopBikeModel = MODEL_COPBIKE;
            }
            unsigned int copcarruId, copcarlaId, copcarsfId, copcarvgId;
            switch (CTheZones::m_CurrLevel) {
            case 0:
                copcarruId = GetRandomCopcarru();
                if (copcarruId != 0) {
                    if (CStreaming::ms_aDefaultCopCarModel[0] == MODEL_COPCARRU)
                        CStreaming::ms_aDefaultCopCarModel[0] = copcarruId;
                    else
                        CStreaming::ms_aDefaultCopCarModel[0] = MODEL_COPCARRU;
                }
                break;
            case 1:
                copcarlaId = GetRandomCopcarla();
                if (copcarlaId != 0) {
                    if (CStreaming::ms_aDefaultCopCarModel[1] == MODEL_COPCARLA)
                        CStreaming::ms_aDefaultCopCarModel[1] = copcarlaId;
                    else
                        CStreaming::ms_aDefaultCopCarModel[1] = MODEL_COPCARLA;
                }
                break;
            case 2:
                copcarsfId = GetRandomCopcarsf();
                if (copcarsfId != 0) {
                    if (CStreaming::ms_aDefaultCopCarModel[2] == MODEL_COPCARSF)
                        CStreaming::ms_aDefaultCopCarModel[2] = copcarsfId;
                    else
                        CStreaming::ms_aDefaultCopCarModel[2] = MODEL_COPCARSF;
                }
                break;
            case 3:
                copcarvgId = GetRandomCopcarvg();
                if (copcarvgId != 0) {
                    if (CStreaming::ms_aDefaultCopCarModel[3] == MODEL_COPCARVG)
                        CStreaming::ms_aDefaultCopCarModel[3] = copcarvgId;
                    else
                        CStreaming::ms_aDefaultCopCarModel[3] = MODEL_COPCARVG;
                }
                break;
            }
        }

        if (!CStreaming::m_bCopBikeLoaded || a1 || CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCopModel[4]].m_nLoadState != 1 || (result = CStreaming::ms_DefaultCopBikeModel, CStreaming::ms_aInfoForModel[CStreaming::ms_DefaultCopBikeModel].m_nLoadState != 1))
        {
            result = CStreaming::ms_aDefaultCopCarModel[CTheZones::m_CurrLevel];
            if (CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCopModel[CTheZones::m_CurrLevel]].m_nLoadState != 1 || CStreaming::ms_aInfoForModel[result].m_nLoadState != 1)
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
                    while (CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCopModel[i]].m_nLoadState != 1 || CStreaming::ms_aInfoForModel[CStreaming::ms_aDefaultCopCarModel[i]].m_nLoadState != 1)
                    {
                        if (++i >= v2)
                            goto LABEL_13;
                    }
                    result = CStreaming::ms_aDefaultCopCarModel[i];
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
        return CStreaming::GetDefaultCopCarModel(a1);
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

    

    // CCarAI::AddPoliceCarOccupants
    //static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle, bool a3)  {
    //    if (!vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated) {
    //        vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated = 1;
    //        CPlayerPed *player = FindPlayerPed(-1);
    //        CPed *driver, *passenger;
    //        switch (vehicle->m_nModelIndex) {
    //        case MODEL_BARRACKS:
    //        case MODEL_COPCARLA:
    //        case MODEL_COPCARSF:
    //        case MODEL_COPCARVG:
    //        case MODEL_COPCARRU:
    //        case 3136:
    //        case 3137:
    //        case 3140:
    //        case 3142:
    //        case 3143:
    //            driver = vehicle->SetUpDriver(-1, 0, 0);
    //            if (player->GetWantedLevel() > 1) {
    //                passenger = vehicle->SetupPassenger(0, -1, 0, 0);
    //                if (player->GetWantedLevel() > 2) {
    //                    if (plugin::Random(0, 3) < 1) {
    //                        driver->GiveDelayedWeapon(WEAPON_SHOTGUN, 1000);
    //                        goto LABEL_13;
    //                    }
    //                    if (plugin::Random(0, 3) < 1)
    //                        passenger->GiveDelayedWeapon(WEAPON_SHOTGUN, 1000);
    //                }
    //            LABEL_13:
    //                driver->m_pIntelligence->ClearTasks(1, 1);
    //                CTask *task_driver = nullptr;
    //                task_driver = new CTaskComplexCopInCar(vehicle, driver, player, 1);
    //                driver->m_pIntelligence->m_TaskMgr.SetTask(task_driver, 3, 1);
    //                passenger->m_pIntelligence->ClearTasks(1, 1);
    //                CTask *task_passenger = nullptr;
    //                task_passenger = new CTaskComplexCopInCar(vehicle, passenger, player, 0);
    //                passenger->m_pIntelligence->m_TaskMgr.SetTask(task_passenger, 3, 1);
    //                return;
    //            }
    //            if (a3 || plugin::Random(0, 100) < 50)
    //                vehicle->SetupPassenger(0, -1, 0, 0);
    //            break;
    //        case MODEL_ENFORCER:
    //        case MODEL_FBIRANCH:
    //        case 3146:
    //        case 3141:
    //            vehicle->SetUpDriver(-1, 0, 0);
    //            vehicle->SetupPassenger(0, -1, 0, 0);
    //            vehicle->SetupPassenger(1, -1, 0, 0);
    //            vehicle->SetupPassenger(2, -1, 0, 0);
    //            return;
    //        case MODEL_PREDATOR:
    //            if (player->GetWantedLevel() > 1) {
    //                driver = vehicle->SetUpDriver(-1, 0, 0);
    //                CTask *task_driver = nullptr;
    //                task_driver = new CTaskSimpleCarSetPedOut(vehicle, 10, 1);
    //                CTaskSimpleCarSetPedOut__m1C(task_driver, driver); // ?
    //                CVector pos = { 0.0f, 3.0f, 2.0f };
    //                driver->AttachPedToEntity(vehicle, pos, 0, 6.2831855f, WEAPON_PISTOL);
    //                driver->m_nPedFlags.bStayInSamePlace = 1;
    //                CTask *task = nullptr;
    //                task = new CTaskComplexKillPedFromBoat(player);
    //                driver->m_pIntelligence->m_TaskMgr.SetTask(task, 3, 0);
    //                CTaskSimpleCarSetPedOut__dtor((int)&a1);
    //            }
    //            goto LABEL_28;
    //        case MODEL_RHINO:
    //        case MODEL_COPBIKE:
    //        case 3144:
    //        LABEL_28:
    //            vehicle->SetUpDriver(-1, 0, 0);
    //            return;
    //        default:
    //            return;
    //        }
    //    }
    //}

    
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
            if (!line.compare("copcarsf")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopcarsfModels().insert(stoi(line));
                }
            }
            if (!line.compare("copcarvg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopcarvgModels().insert(stoi(line));
                }
            }
            if (!line.compare("copcarru")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopcarruModels().insert(stoi(line));
                }
            }
            if (!line.compare("copbike")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopbikeModels().insert(stoi(line));
                }
            }
            if (!line.compare("fbiranch")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetFbiranchModels().insert(stoi(line));
                }
            }
            if (!line.compare("enforcer")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetEnforcerModels().insert(stoi(line));
                }
            }
            if (!line.compare("taxi")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetTaxiModels().insert(stoi(line));
                }
            }
            if (!line.compare("ambulan")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetAmbulanModels().insert(stoi(line));
                }
            }
            if (!line.compare("firetruk")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetFiretrukModels().insert(stoi(line));
                }
            }
            if (!line.compare("boxburg")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetBoxburgModels().insert(stoi(line));
                }
            }

        }
        
        patch::RedirectJump(0x6D2370, IsLawEnforcementVehicle);
        patch::RedirectJump(0x6D8470, UsesSiren);
        patch::RedirectJump(0x407C50, GetDefaultCopCarModel);
        //patch::RedirectJump(0x421980, ChoosePoliceCarModel);
        //patch::RedirectJump(0x41C070, AddPoliceCarOccupants);

        patch::RedirectJump(0x6AB349, Patch_6AB349);
        patch::RedirectJump(0x4912D0, Patch_4912D0); 
        patch::RedirectJump(0x469629, Patch_469629);
        patch::RedirectJump(0x6ACA57, Patch_6ACA57);
        patch::RedirectJump(0x6B1F4F, Patch_6B1F4F);
        
        patch::RedirectJump(0x41C0A6, Patch_41C0A6);
        patch::RedirectJump(0x42BBC8, Patch_42BBC8);
        patch::RedirectJump(0x613A68, Patch_613A68);

        Events::drawingEvent += [] {
            //CVehicle *vehicle = FindPlayerVehicle(0, false);
            //if (vehicle) {
                //unsigned int size = GetCopcarlaModels().size();
                gamefont::Print({
                    //Format("siren = %d", vehicle->m_nVehicleFlags.bSirenOrAlarm),
                    Format("wanted = %d", FindPlayerWanted(-1)->m_nWantedLevel),
                    Format("level = %d", CTheZones::m_CurrLevel),
                    Format("copcar ru = %d", CStreaming::ms_aDefaultCopCarModel[0]),
                    Format("copcar la = %d", CStreaming::ms_aDefaultCopCarModel[1]),
                    Format("copcar sf = %d", CStreaming::ms_aDefaultCopCarModel[2]),
                    Format("copcar vg = %d", CStreaming::ms_aDefaultCopCarModel[3]),
                    Format("copbike = %d", CStreaming::ms_DefaultCopBikeModel)
                }, 10, 300, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
            //}
        };

    }
} specialCars;

int AddSpecialCars::currentModelForSiren;
int AddSpecialCars::currentModelTaxi;
int AddSpecialCars::currentModelFiretruk;
int AddSpecialCars::currentModel;
int AddSpecialCars::currentModel_Patch_41C0A6;
int AddSpecialCars::currentModel_Patch_42BBC8;
int AddSpecialCars::currentModel_Patch_613A68;
unsigned int AddSpecialCars::randomCopCarTime = 0;
unsigned int AddSpecialCars::jmp_6AB360;
unsigned int AddSpecialCars::jmp_469658;
unsigned int AddSpecialCars::jmp_41C0AF;
unsigned int AddSpecialCars::jmp_42BBCE;
unsigned int AddSpecialCars::jmp_613A71;

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

void __declspec(naked) AddSpecialCars::Patch_6B1F4F() { // Firetruk
    __asm {
        CWDE
        mov ecx, 407
        pushad
        push eax
        call GetFiretrukModel
        mov currentModelFiretruk, eax
        popad
        cmp ecx, currentModelFiretruk
        jz SET_TRUE
        jmp END_CHECK
        SET_TRUE :
        mov ecx, 0x6B1F5B
        jmp ecx
            END_CHECK :
            mov ecx, 0x6B1F55
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

void __declspec(naked) AddSpecialCars::Patch_613A68() {
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
