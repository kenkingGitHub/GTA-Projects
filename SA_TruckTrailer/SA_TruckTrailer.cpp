/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include "plugin.h"
#include <vector>
#include <fstream>
#include <string>
#include "game_sa\CGeneral.h"
#include "game_sa\CStreaming.h"
#include "game_sa\CTrailer.h"
#include "game_sa\CWorld.h"
#include "game_sa\CTheScripts.h"
#include "game_sa\CModelInfo.h"
#include "game_sa\common.h"

using namespace plugin;
using namespace std;

unsigned int normalModelIds[] = { 400, 418, 419, 440, 458, 466, 467, 475, 479, 482, 483, 489, 491, 496, 500, 505, 507,
518, 526, 529, 540, 543, 545, 546, 547, 550, 554, 580, 582, 585, 589, 600, 603, 604, 605 };

unsigned int poorfamilyModelIds[] = { 401, 404, 410, 412, 436, 439, 492, 516, 517, 527, 542, 549, 567, 575, 576 };

unsigned int richfamilyModelIds[] = { 405, 421, 426, 445, 474, 477, 535, 551, 558, 559, 560, 561, 562, 565, 566, 579, 587 };

unsigned int executiveModelIds[] = { 402, 409, 411, 415, 429, 434, 451, 480, 506, 533, 534, 536, 541, 555, 602 };

unsigned int workerModelIds[] = { 408, 413, 414, 422, 423, 455, 456, 478, 498, 499, 508, 524, 530, 578, 609 };

class MyPlugin {
public:
    class ModelInfo {
    public:
        bool enabledTrailer;
        ModelInfo(CVehicle *vehicle) { enabledTrailer = true; }
    };

    static VehicleExtendedData<ModelInfo> modelInfo;

    struct MyData {
        unsigned int ModelId;
        unsigned int TrailerIdOne;
        unsigned int TrailerIdTwo;
        unsigned int TrailerIdThree;
        unsigned int TrailerIdFour;
        unsigned int TrailerColours;
        unsigned int TrailerExtras;
        unsigned int TrailerConst;
    };

    static vector<MyData>& GetDataVector() {
        static vector<MyData> vec;
        return vec;
    }

    static void ReadSettingsFile() {
        ifstream stream("trailer.dat");
        for (string line; getline(stream, line); ) {
            if (line[0] != ';' && line[0] != '#') {
                if (!line.compare("trailer")) {
                    while (getline(stream, line) && line.compare("end")) {
                        if (line[0] != ';' && line[0] != '#') {
                            MyData entry;
                            if (sscanf(line.c_str(), "%d, %d, %d, %d, %d, %d, %d, %d", &entry.ModelId, &entry.TrailerIdOne, &entry.TrailerIdTwo, &entry.TrailerIdThree, &entry.TrailerIdFour, &entry.TrailerColours, &entry.TrailerExtras, &entry.TrailerConst) == 8)
                                GetDataVector().push_back(entry);
                        }
                    }
                }
            }
        }
    }

    static MyData *GetDataInfoForModel(unsigned int BaseModelId) {
        for (unsigned int i = 0; i < GetDataVector().size(); i++) {
            if (GetDataVector()[i].ModelId == BaseModelId)
                return &GetDataVector()[i];
        }
        return nullptr;
    }

    static void SetTrailer(CVehicle *vehicle, unsigned int modelTrailer, unsigned int colour, unsigned int extra) {
        CStreaming::RequestModel(modelTrailer, 0);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::ms_aInfoForModel[modelTrailer].m_loadState == LOADSTATE_LOADED) {
            if (extra && vehicle) {
                CVehicleModelInfo::ms_compsToUse[0] = vehicle->m_anExtras[0];
                CVehicleModelInfo::ms_compsToUse[1] = vehicle->m_anExtras[1];
            }
            CVehicle *trailer = nullptr;
            if (CModelInfo::IsVehicleModelType(modelTrailer) == 11)
                trailer = new CTrailer(modelTrailer, 1);
            else
                trailer = new CAutomobile(modelTrailer, 1, true);
            if (trailer) {
                trailer->SetPosn(0.0f, 0.0f, 0.0f);
                trailer->m_nStatus = 4;
                CWorld::Add(trailer);
                if (vehicle) {
                    trailer->SetTowLink(vehicle, true);
                    if (colour) {
                        trailer->m_nPrimaryColor = vehicle->m_nPrimaryColor;
                        trailer->m_nSecondaryColor = vehicle->m_nSecondaryColor;
                        trailer->m_nTertiaryColor = vehicle->m_nTertiaryColor;
                        trailer->m_nQuaternaryColor = vehicle->m_nQuaternaryColor;
                    }
                }
                CTheScripts::ClearSpaceForMissionEntity(trailer->GetPosition(), trailer);
                if (CModelInfo::IsVehicleModelType(modelTrailer) == 11) {
                    trailer->m_nFlags.bEngineOn = 1;
                    trailer->m_nFlags.bIsLocked = 1;
                }
                else {
                    trailer->m_nFlags.bEngineOn = 0;
                    trailer->m_nDoorLock = CARLOCK_LOCKED;
                    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(trailer);
                    unsigned int perRandomDamage = CGeneral::GetRandomNumberInRange(0, 3);
                    if (perRandomDamage == 2)
                        automobile->SetTotalDamage(1);
                    else if (perRandomDamage == 1)
                        automobile->SetRandomDamage(1);
                }
            }
        }
    }

    MyPlugin() {
        ReadSettingsFile();
        static unsigned int Id;
        static unsigned int TrailerId;
        static unsigned int currentVariant = 0;

        Events::gameProcessEvent += [] {
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                if (vehicle) {
                    if (vehicle->m_nFlags.bIsLocked == 1) {
                        if (FindPlayerPed()) {
                            if ((DistanceBetweenPoints(FindPlayerCoors(0), vehicle->GetPosition()) > 200.0f)) {
                                if (vehicle->m_pTrailer) {
                                    vehicle->m_pTrailer->m_nFlags.bIsLocked = 0;
                                    vehicle->m_pTrailer->CanBeDeleted();
                                }
                                vehicle->m_nFlags.bIsLocked = 0;
                                vehicle->CanBeDeleted();
                            }
                            else if (FindPlayerPed()->m_pVehicle == vehicle) {
                                if (vehicle->m_pTrailer)
                                    vehicle->m_pTrailer->m_nFlags.bIsLocked = 0;
                                vehicle->m_nFlags.bIsLocked = 0;
                            }
                        }
                    }
                    MyData *entryModel = GetDataInfoForModel(vehicle->m_wModelIndex);
                    ModelInfo &info = modelInfo.Get(vehicle);
                    if (entryModel && info.enabledTrailer) {
                        if (!entryModel->TrailerConst) {
                            if (currentVariant < 2)
                                currentVariant += 1;
                            else
                                currentVariant = 0;
                            if (currentVariant == 2)
                                info.enabledTrailer = false;
                        }
                        switch (CGeneral::GetRandomNumberInRange(0, 4)) {
                        case 0: Id = entryModel->TrailerIdOne; break;
                        case 1: Id = entryModel->TrailerIdTwo; break;
                        case 2: Id = entryModel->TrailerIdThree; break;
                        case 3: Id = entryModel->TrailerIdFour; break;
                        }
                        switch (Id) {
                            CVehicleModelInfo *vehModel;
                            bool enabledExit;
                        case 0: {
                            enabledExit = false;
                            do {
                                TrailerId = normalModelIds[CGeneral::GetRandomNumberInRange(0, 35)];
                                vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[TrailerId]);
                                if (CModelInfo::IsVehicleModelType(TrailerId) == 0 && vehModel->m_nVehicleClass == 0)
                                    enabledExit = true;
                            } while (!enabledExit);
                            break;
                        }
                        case 1: {
                            enabledExit = false;
                            do {
                                TrailerId = poorfamilyModelIds[CGeneral::GetRandomNumberInRange(0, 15)];
                                vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[TrailerId]);
                                if (CModelInfo::IsVehicleModelType(TrailerId) == 0 && vehModel->m_nVehicleClass == 1)
                                    enabledExit = true;
                            } while (!enabledExit);
                            break;
                        }
                        case 2: {
                            enabledExit = false;
                            do {
                                TrailerId = richfamilyModelIds[CGeneral::GetRandomNumberInRange(0, 17)];
                                vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[TrailerId]);
                                if (CModelInfo::IsVehicleModelType(TrailerId) == 0 && vehModel->m_nVehicleClass == 2)
                                    enabledExit = true;
                            } while (!enabledExit);
                            break;
                        }
                        case 3: {
                            enabledExit = false;
                            do {
                                TrailerId = executiveModelIds[CGeneral::GetRandomNumberInRange(0, 15)];
                                vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[TrailerId]);
                                if (CModelInfo::IsVehicleModelType(TrailerId) == 0 && vehModel->m_nVehicleClass == 3)
                                    enabledExit = true;
                            } while (!enabledExit);
                            break;
                        }
                        case 4: {
                            enabledExit = false;
                            do {
                                TrailerId = workerModelIds[CGeneral::GetRandomNumberInRange(0, 15)];
                                vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[TrailerId]);
                                if (CModelInfo::IsVehicleModelType(TrailerId) == 0 && vehModel->m_nVehicleClass == 4)
                                    enabledExit = true;
                            } while (!enabledExit);
                            break;
                        }
                        default: TrailerId = Id; break;
                        }
                        if (info.enabledTrailer && vehicle->m_pDriver && !vehicle->m_pTrailer
                            && (CModelInfo::IsVehicleModelType(TrailerId) == 11 || CModelInfo::IsVehicleModelType(TrailerId) == 0)) {
                            vehicle->m_nFlags.bMadDriver = 0;
                            vehicle->m_nFlags.bIsLocked = 1;
                            SetTrailer(vehicle, TrailerId, entryModel->TrailerColours, entryModel->TrailerExtras);
                        }
                    }
                    info.enabledTrailer = false;
                }
            }
        };

    }
} myPlugin;

VehicleExtendedData<MyPlugin::ModelInfo> MyPlugin::modelInfo;
