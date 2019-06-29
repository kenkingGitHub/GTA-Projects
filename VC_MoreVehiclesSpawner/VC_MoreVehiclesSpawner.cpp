#include <string>
#include "plugin.h"
#include "extensions\KeyCheck.h"
#include "common.h"
#include "CModelInfo.h"
#include "CTimer.h"
#include "CFont.h"
#include "CSprite2d.h"
#include "CStreaming.h"
#include "CBoat.h"
#include "CHeli.h"
#include "CBike.h"
#include "CWorld.h"
#include "CTheScripts.h"
#include "eVehicleModel.h"

using namespace plugin;

class MoreVehiclesSpawner {
public:
    static std::string typedBuffer;
    static std::string errorMessage;
    static std::string errorMessageBuffer;
    static unsigned int errorMessageTimer;
    static bool enabled;

    static void SpawnVehicle(unsigned int modelIndex, CVector position, float orientation) {
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
            /*case VEHICLE_HELI:
                vehicle = new CHeli(modelIndex, 1);
                break;*/
            case VEHICLE_BIKE:
                vehicle = new CBike(modelIndex, 1);
                reinterpret_cast<CBike *>(vehicle)->m_nDamageFlags |= 0x10;
                break;
            case VEHICLE_BOAT:
                vehicle = new CBoat(modelIndex, 1);
                break;
            default:
                vehicle = new CAutomobile(modelIndex, 1);
                break;
            }
            if (vehicle) {
                // Размещаем транспорт в игровом мире
                vehicle->SetPosition(position);
                vehicle->m_placement.SetOrientation(0.0f, 0.0f, orientation);
                vehicle->m_nState = 4;
                if (modelIndex == MODEL_RCBANDIT || modelIndex == MODEL_RCBARON || modelIndex == MODEL_RCRAIDER || modelIndex == MODEL_RCGOBLIN)
                    vehicle->m_nLockStatus = CARLOCK_LOCKED;
                else
                    vehicle->m_nLockStatus = CARLOCK_UNLOCKED;
                CWorld::Add(vehicle);
                CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
                if (vehicle->m_nVehicleClass != VEHICLE_BOAT)
                    reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
            }
        }
    }

    static void Update() {
        KeyCheck::Update(); 
        if (FindPlayerPed()) {
            if (KeyCheck::CheckJustDown(VK_TAB)) { 
                enabled = !enabled;
                typedBuffer.clear();
                errorMessageBuffer.clear();
            }
            if (enabled) {
                errorMessage.clear();
                if (KeyCheck::CheckWithDelay(VK_BACK, 200)) { 
                    if (typedBuffer.size() > 0) {
                        typedBuffer.pop_back();
                    }
                }
                else {
                    for (int i = 0; i <= 9; i++) {
                        if (KeyCheck::CheckWithDelay(i + 48, 200)) {
                            if (typedBuffer.size() == 4)
                                errorMessage = "Too many digits!";
                            else {
                                typedBuffer.push_back(i + 48); 
                            }
                            break;
                        }
                    }
                }
                if (KeyCheck::CheckJustDown(45)) { 
                    if (typedBuffer.size() > 0) {
                        unsigned int modelId = std::stoi(typedBuffer);
                        if (modelId < 6500) {
                            int modelType = CModelInfo::IsVehicleModelType(modelId);
                            if (modelType != -1) {
                                if (modelType == VEHICLE_AUTOMOBILE || modelType == VEHICLE_BOAT || modelType == VEHICLE_BIKE) {
                                    SpawnVehicle(modelId, FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 4.0f, 0.0f)), FindPlayerPed()->m_fRotationCur + 1.5707964f);
                                    errorMessageBuffer.clear(); 
                                }
                                else
                                    errorMessage = "Can't spawn a heli and plane";
                            }
                            else
                                errorMessage = "This model is not a vehicle!";
                        }
                        else
                            errorMessage = "ID is too big!";
                    }
                    else
                        errorMessage = "Please enter model Id!";
                }
            }
        }
        else
            enabled = false;
    }

    static void Render() {
        if (enabled) {
            CSprite2d::DrawRect(CRect(10.0f, 10.0f, 300.0f, 130.0f), CRGBA(0, 0, 0, 100));
            CSprite2d::DrawRect(CRect(150.0f, 50.0f, 230.0f, 52.0f), CRGBA(255, 255, 255, 255));

            CFont::SetScale(0.8f, 1.9f);
            CFont::SetColor(CRGBA(255, 255, 255, 255));
            CFont::SetJustifyOn();
            CFont::SetFontStyle(1);
            CFont::SetPropOn();
            CFont::SetWrapx(300.0f);
            CFont::PrintString(15.0f, 15.0f, "Model ID:");
            if (typedBuffer.size() > 0)
                CFont::PrintString(160.0f, 15.0f, const_cast<char*>(typedBuffer.c_str()));
            if (errorMessage.size() > 0) {
                errorMessageBuffer = errorMessage;
                errorMessageTimer = CTimer::m_snTimeInMilliseconds;
            }
            if (errorMessageBuffer.size() > 0 && CTimer::m_snTimeInMilliseconds < (errorMessageTimer + 2000)) {
                CFont::SetColor(CRGBA(255, 0, 0, 255));
                CFont::PrintString(15.0f, 55.0f, const_cast<char*>(errorMessageBuffer.c_str()));
            }
        }
    }

    MoreVehiclesSpawner() {
        Events::gameProcessEvent += Update;
        Events::drawingEvent += Render;
    };
} moreVehiclesSpawner;

std::string MoreVehiclesSpawner::typedBuffer;
std::string MoreVehiclesSpawner::errorMessage;
std::string MoreVehiclesSpawner::errorMessageBuffer;
unsigned int MoreVehiclesSpawner::errorMessageTimer = 0;
bool MoreVehiclesSpawner::enabled = false;

