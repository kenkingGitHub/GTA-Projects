#include "plugin_III.h"
#include "game_III\common.h"
#include "game_III\CStreaming.h"
#include "game_III\CModelInfo.h"
#include "game_III\CBoat.h"
#include "game_III\CWorld.h"
#include "game_III\CTheScripts.h"
#include "game_III\CTimer.h"
#include "game_III\CMessages.h"

using namespace plugin;

class CreateCarExample {
public:
    static unsigned int m_nLastSpawnedTime; 

    static void SetPosition(CVehicle * vehicle, CVector &pos) {
        vehicle->m_matrix.pos = pos;
    }

    static void SetOrientation(CVehicle * vehicle, float x, float y, float z) {
        CVector pos = vehicle->m_matrix.pos;
        vehicle->m_matrix.SetRotate(x, y, z);
        vehicle->m_matrix.pos = pos;
    }

    static CVehicle *SpawnVehicle(unsigned int modelIndex, CVector position, float orientation) {
        unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelIndex].m_nFlags;
        CStreaming::RequestModel(modelIndex, GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::ms_aInfoForModel[modelIndex].m_nLoadState == LOADSTATE_LOADED) {
            if (!(oldFlags & GAME_REQUIRED)) {
                CStreaming::SetModelIsDeletable(modelIndex);
                CStreaming::SetModelTxdIsDeletable(modelIndex);
            }
            CVehicle *vehicle = nullptr;
            switch (reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleType) {
            case VEHICLE_BOAT:
                vehicle = new CBoat(modelIndex, 1);
                break;
            default:
                vehicle = new CAutomobile(modelIndex, 1);
                break;
            }
            if (vehicle) {
                SetPosition(vehicle, position);
                SetOrientation(vehicle, 0.0f, 0.0f, orientation);
                vehicle->m_nState = 4;
                vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
                CWorld::Add(vehicle);
                CTheScripts::ClearSpaceForMissionEntity(position, vehicle); 
                if (vehicle->m_nVehicleClass != VEHICLE_BOAT)
                    reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
                return vehicle;
            }
        }
        return nullptr;
    }

    static void ProcessSpawn() {
        if (KeyPressed(VK_TAB) && CTimer::m_snTimeInMilliseconds > (m_nLastSpawnedTime + 1000)) { 
            CPed *player = FindPlayerPed(); 
            if (player) {
                //CVector position = FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 5.0f, 0.0f)); 
                CVector offset = { 0.0f, 5.0f, 0.0f };
                CVector position = player->m_matrix * offset;
                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(SpawnVehicle(91, position, FindPlayerPed()->m_fRotationCur + 1.5707964f));
                if (automobile) {
                    m_nLastSpawnedTime = CTimer::m_snTimeInMilliseconds;
                    CMessages::AddMessageJumpQ(L"vehicle create", 1000, 0);
                }
            }
        }
    }

    CreateCarExample() {
        Events::gameProcessEvent.Add(ProcessSpawn); 
    }
} example;

unsigned int CreateCarExample::m_nLastSpawnedTime = 0;
