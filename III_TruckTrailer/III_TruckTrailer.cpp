#include "plugin.h"
#include "CClumpModelInfo.h"
#include "CModelInfo.h"
#include "CWorld.h"
#include "extensions\KeyCheck.h"
#include "common.h"
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <string>
#include "CStreaming.h"
#include "CTheScripts.h"
#include "CCamera.h"

//#include "CMessages.h"
//#include "CFont.h"

CVector __cdecl PointOffset(CMatrix matrix, float x, float y, float z) {
    CVector pos;
    pos.x = matrix.pos.x + x * matrix.right.x + y * matrix.up.x + z * matrix.at.x;
    pos.y = matrix.pos.y + x * matrix.right.y + y * matrix.up.y + z * matrix.at.y;
    pos.z = matrix.pos.z + x * matrix.right.z + y * matrix.up.z + z * matrix.at.z;
    return pos;
}

float __cdecl Distance(CVector pos1, CVector pos2) {
    return (float)sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2) + pow(pos1.z - pos2.z, 2));
}

void MatrixSet(CMatrix *matrix, float cos_x, float sin_x, float cos_y, float sin_y, float cos_z, float sin_z) {
    matrix->right.x = cos_y * cos_z;
    matrix->right.y = cos_y * sin_z;
    matrix->right.z = -sin_y;
    matrix->up.x = sin_x * sin_y * cos_z - cos_x * sin_z;
    matrix->up.y = sin_x * sin_y * sin_z + cos_x * cos_z;
    matrix->up.z = sin_x * cos_y;
    matrix->at.x = cos_x * sin_y * cos_z + sin_x * sin_z;
    matrix->at.y = cos_x * sin_y * sin_z - sin_x * cos_z;
    matrix->at.z = cos_x * cos_y;
}

void MatrixAttach(CMatrix* matrix, CMatrix* rwMatrix, bool deleteOnDetach) {
    ((void(__thiscall *)(CMatrix*, CMatrix*, bool))0x4B8DD0)(matrix, rwMatrix, deleteOnDetach);
}

float VehicleGetSpeed(CVehicle *vehicle) {
    return 180.0f * (float)sqrt(pow(vehicle->m_vecMoveSpeed.x, 2) + pow(vehicle->m_vecMoveSpeed.y, 2) + pow(vehicle->m_vecMoveSpeed.z, 2));
}

using namespace plugin;
using namespace std;

class VehicleLodDist {
public:
    VehicleLodDist() {
        *(float *)0x600150 = 400.0f; // 70
        *(float *)0x60015C = 400.0f; // 60
    };
} vehicleLodDist;

class TruckTrailer {
public:
    static CEntity *outEntity;
    static short outCount;
    static float offset_Y;
    static unsigned int TrailerId;

    class VehicleComponents {
    public:
        RwFrame *misc, *hookup, *prop_a, *prop_b, *prop_c;  char connector; bool disableSpeed, enableTrailer, isAttached;

        VehicleComponents(CVehicle *) { 
            misc = hookup = prop_a = prop_b = prop_c = nullptr;  connector = 0;
            disableSpeed = true; enableTrailer = false, isAttached = false;
        }
    };

    static VehicleExtendedData<VehicleComponents> vehComps;

    struct MyData {
        unsigned int ModelId, TrailerIdOne, TrailerIdTwo, TrailerIdThree, TrailerIdFour, TrailerColours, TrailerExtras, TrailerConst, TrailerSecond;
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
                            if (sscanf(line.c_str(), "%d, %d, %d, %d, %d, %d, %d, %d, %d", &entry.ModelId, &entry.TrailerIdOne, &entry.TrailerIdTwo, &entry.TrailerIdThree, &entry.TrailerIdFour, &entry.TrailerColours, &entry.TrailerExtras, &entry.TrailerConst, &entry.TrailerSecond) == 9)
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

    static void AddTrailer(CVehicle *vehicle, CVehicle *trailer, unsigned int colour) {
        offset_Y = -4.0f;
        if (vehComps.Get(vehicle).misc && vehComps.Get(trailer).hookup)
            offset_Y = -((vehComps.Get(vehicle).misc->modelling.pos.y * (-1.0f)) + vehComps.Get(trailer).hookup->modelling.pos.y);
        trailer->SetPosition(vehicle->TransformFromObjectSpace(CVector(0.0f, offset_Y, 0.0f)));
        trailer->SetHeading(vehicle->GetHeading() / 57.295776f);
        trailer->m_nState = 4;
        trailer->m_nDoorLock = CARLOCK_LOCKED;
        CWorld::Add(trailer);
        CTheScripts::ClearSpaceForMissionEntity(trailer->GetPosition(), trailer);
        reinterpret_cast<CAutomobile *>(trailer)->PlaceOnRoadProperly();
        vehComps.Get(trailer).isAttached = true;
        if (colour) {
            trailer->m_nPrimaryColor = vehicle->m_nPrimaryColor;
            trailer->m_nSecondaryColor = vehicle->m_nSecondaryColor;
        }
    }

    static void SetTrailer(CVehicle *vehicle, unsigned int modelTrailer, unsigned int colour, unsigned int extra, unsigned int enableSecondTrailer) {
        unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelTrailer].m_nFlags;
        CStreaming::RequestModel(modelTrailer, GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::ms_aInfoForModel[modelTrailer].m_nLoadState == LOADSTATE_LOADED && vehicle) {
            if (extra) {
                CVehicleModelInfo::ms_compsToUse[0] = vehicle->m_anExtras[0];
                CVehicleModelInfo::ms_compsToUse[1] = vehicle->m_anExtras[1];
            }
            CVehicle *trailer = nullptr;
            trailer = new CAutomobile(modelTrailer, 1);
            if (trailer) {
                AddTrailer(vehicle, trailer, colour);
                // второй прицеп
                int enable = plugin::Random(0, 2);
                if (enableSecondTrailer && vehComps.Get(trailer).misc && enable != 0) {
                    if (EnableSpawnTrailer(trailer, trailer->m_nModelIndex)) {
                        CVehicle *secondTrailer = nullptr;
                        secondTrailer = new CAutomobile(modelTrailer, 1);
                        if (secondTrailer) 
                            AddTrailer(trailer, secondTrailer, colour);
                    }
                }
            }
            if (!(oldFlags & GAME_REQUIRED)) {
                CStreaming::SetModelIsDeletable(modelTrailer);
                CStreaming::SetModelTxdIsDeletable(modelTrailer);
            }
        }
    }

    static void TrailerLightControl(CAutomobile *trailer) {
        CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[trailer->m_nModelIndex]);
        if ((vehModel->m_avDummyPos[0].x == 0.0f) && (vehModel->m_avDummyPos[0].y == 0.0f) && (vehModel->m_avDummyPos[0].z == 0.0f)) {
            if (trailer->m_carDamage.GetLightStatus(LIGHT_FRONT_LEFT) == 0)
                trailer->m_carDamage.SetLightStatus(LIGHT_FRONT_LEFT, 1);
            if (trailer->m_carDamage.GetLightStatus(LIGHT_FRONT_RIGHT) == 0)
                trailer->m_carDamage.SetLightStatus(LIGHT_FRONT_RIGHT, 1);
        }
        if ((vehModel->m_avDummyPos[1].x == 0.0f) && (vehModel->m_avDummyPos[1].y == 0.0f) && (vehModel->m_avDummyPos[1].z == 0.0f)) {
            if (trailer->m_carDamage.GetLightStatus(LIGHT_REAR_LEFT) == 0)
                trailer->m_carDamage.SetLightStatus(LIGHT_REAR_LEFT, 1);
            if (trailer->m_carDamage.GetLightStatus(LIGHT_REAR_RIGHT) == 0)
                trailer->m_carDamage.SetLightStatus(LIGHT_REAR_RIGHT, 1);
        }
    }

    static bool EnableSpawnTrailer(CVehicle *vehicle, unsigned int TrailerId) {
        bool result = FALSE;
        offset_Y = (-1.0f * (CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]->m_pColModel->m_boundBox.m_vecMin.y)) + CModelInfo::ms_modelInfoPtrs[TrailerId]->m_pColModel->m_boundBox.m_vecMax.y;
        outCount = 0;
        CWorld::FindObjectsInRange(vehicle->TransformFromObjectSpace(CVector(0.0f, -offset_Y, 0.0f)), 2.0, 1, &outCount, 2, &outEntity, 0, 1, 0, 0, 0);
        if (outCount == 0)
            result = TRUE;
        return result;
    }

    static void FrameSetRotateXOnly(RwFrame *component, float angle) {
        CMatrix matrixFrame(&component->modelling, false);
        matrixFrame.SetRotateXOnly(angle);
        matrixFrame.UpdateRW();
    }

    static CVehicle* FindTrailer(CVehicle *tractor) {
        CVehicle* result = nullptr;
        CEntity *outEntity[5] = { nullptr };
        short outCount = 0;
        int index = 0;
        CWorld::FindObjectsInRange(RwFrameGetLTM(vehComps.Get(tractor).misc)->pos, 10.0, 1, &outCount, 5, outEntity, 0, 1, 0, 0, 0);
        if (outCount > 0) {
            do
            {
                CVehicle *trailer = (CVehicle *)outEntity[index];
                if (trailer && (trailer != tractor) && vehComps.Get(trailer).hookup && (vehComps.Get(tractor).connector == vehComps.Get(trailer).connector)) {
                    if (DistanceBetweenPoints(RwFrameGetLTM(vehComps.Get(tractor).misc)->pos, RwFrameGetLTM(vehComps.Get(trailer).hookup)->pos) < 1.5f) {
                        result = trailer; break;
                    }
                }
                ++index;
            } while (index < outCount);
        }
        return result;
    }

    static CVehicle* FindTractor(CVehicle *trailer) {
        CVehicle* result = nullptr;
        CEntity *outEntity[4] = { nullptr };
        short outCount = 0;
        int index = 0;
        CWorld::FindObjectsInRange(RwFrameGetLTM(vehComps.Get(trailer).hookup)->pos, 7.0, 1, &outCount, 4, outEntity, 0, 1, 0, 0, 0);
        if (outCount > 0) {
            do
            {
                CVehicle *tractor = (CVehicle *)outEntity[index];
                if (tractor && (tractor != trailer) && vehComps.Get(tractor).misc && (vehComps.Get(trailer).connector == vehComps.Get(tractor).connector)) {
                    if (Distance(PointOffset(tractor->m_matrix, 0, vehComps.Get(tractor).misc->modelling.pos.y, vehComps.Get(tractor).misc->modelling.pos.z), PointOffset(trailer->m_matrix, 0, vehComps.Get(trailer).hookup->modelling.pos.y, vehComps.Get(trailer).hookup->modelling.pos.z)) < 1.5f) {
                        result = tractor; break;
                    }
                }
                ++index;
            } while (index < outCount);
        }
        return result;
    }

    static void ProcessControlTrailers(CVehicle *tractor, CVehicle *trailer) {
        if (vehComps.Get(trailer).prop_a)
            vehComps.Get(trailer).prop_a->modelling.pos.z = 0.0f;
        if (vehComps.Get(trailer).prop_b)
            vehComps.Get(trailer).prop_b->modelling.pos.z = 0.0f;
        if (vehComps.Get(trailer).prop_c)
            FrameSetRotateXOnly(vehComps.Get(trailer).prop_c, 0.0f);
        trailer->m_nVehicleFlags = tractor->m_nVehicleFlags;
        //trailer->m_fBreakPedal = tractor->m_fBreakPedal;
        //trailer->m_fGasPedal = tractor->m_fGasPedal;
        TrailerLightControl(reinterpret_cast<CAutomobile *>(trailer));
        bool  find;
        float LinkDifferenceZ = trailer->GetDistanceFromCentreOfMassToBaseOfModel() - tractor->GetDistanceFromCentreOfMassToBaseOfModel();
        float TrailerOnGroundZ = min(0, CWorld::FindGroundZFor3DCoord(trailer->m_matrix.pos.x, trailer->m_matrix.pos.y, trailer->m_matrix.pos.z, &find) +
            trailer->GetDistanceFromCentreOfMassToBaseOfModel() - trailer->m_matrix.pos.z);
        if ((TrailerOnGroundZ < -1.0f) || (!VehicleGetSpeed(tractor)))
            return;
        CVector a = PointOffset(tractor->m_matrix, 0, vehComps.Get(tractor).misc->modelling.pos.y, LinkDifferenceZ);
        CVector b = PointOffset(trailer->m_matrix, 0, (reinterpret_cast<CAutomobile *>(trailer))->m_aCarNodes[CAR_WHEEL_LB]->modelling.pos.y, TrailerOnGroundZ);
        float R = Distance(a, b);
        float cos_x = float(sqrt(pow(a.y - b.y, 2) + pow(b.x - a.x, 2)) / R);
        float sin_x = (a.z - b.z) / R;
        float cos_y = float(sqrt(pow(tractor->m_matrix.right.x, 2) + pow(tractor->m_matrix.right.y, 2)));
        float sin_y = -tractor->m_matrix.right.z;
        float cos_z = (a.y - b.y) / (R * cos_x);
        float sin_z = (b.x - a.x) / (R * cos_x);
        CMatrix matrix;
        matrix.m_bOwnsAttachedMatrix = 0;
        matrix.m_pAttachMatrix = 0;
        MatrixAttach(&matrix, &trailer->m_matrix, 0);
        MatrixSet(&matrix, cos_x, sin_x, cos_y, sin_y, cos_z, sin_z);
        matrix.pos = a;
        matrix.pos = PointOffset(matrix, 0, -vehComps.Get(trailer).hookup->modelling.pos.y, 0);
        matrix.UpdateRW();
        matrix.~CMatrix();
        for (int w = 0; w < 4; w++)
            reinterpret_cast<CAutomobile *>(trailer)->m_fWheelTotalRot[w] = reinterpret_cast<CAutomobile *>(tractor)->m_fWheelTotalRot[w];
        float vR = float(sqrt(pow(tractor->m_vecMoveSpeed.x, 2) + pow(tractor->m_vecMoveSpeed.y, 2)));
        float vx = -vR * sin_z;
        float vy = vR * cos_z;
        float vz = tractor->m_vecMoveSpeed.z;
        trailer->m_vecMoveSpeed.x = vx;
        trailer->m_vecMoveSpeed.y = vy;
        trailer->m_vecMoveSpeed.z = vz;
    }

    
    TruckTrailer() {
        ReadSettingsFile();
                
        Events::vehicleSetModelEvent += [](CVehicle *vehicle, int modelIndex) {
            if (vehicle->m_pRwClump) {
                vehComps.Get(vehicle).prop_a = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "prop_a");
                vehComps.Get(vehicle).prop_b = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "prop_b");
                vehComps.Get(vehicle).prop_c = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "prop_c");
                vehComps.Get(vehicle).connector = 0;
                vehComps.Get(vehicle).misc = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "misc_a");		if (vehComps.Get(vehicle).misc)	   vehComps.Get(vehicle).connector = 1;
                else {
                    vehComps.Get(vehicle).misc = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "misc_b");		if (vehComps.Get(vehicle).misc) 	vehComps.Get(vehicle).connector = 2;
                    else {
                        vehComps.Get(vehicle).misc = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "misc_c");		if (vehComps.Get(vehicle).misc) 	vehComps.Get(vehicle).connector = 3;
                        else {
                            vehComps.Get(vehicle).misc = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "misc_d");		if (vehComps.Get(vehicle).misc) 	vehComps.Get(vehicle).connector = 4;
                            else {
                                vehComps.Get(vehicle).misc = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "misc_e");		if (vehComps.Get(vehicle).misc) 	vehComps.Get(vehicle).connector = 5;
                                else {
                                    vehComps.Get(vehicle).misc = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "misc_f");		if (vehComps.Get(vehicle).misc) 	vehComps.Get(vehicle).connector = 6;
                                }
                            }
                        }
                    }
                }
                vehComps.Get(vehicle).hookup = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hookup_a");	if (vehComps.Get(vehicle).hookup)		vehComps.Get(vehicle).connector = 1;
                else {
                    vehComps.Get(vehicle).hookup = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hookup_b");	if (vehComps.Get(vehicle).hookup)		vehComps.Get(vehicle).connector = 2;
                    else {
                        vehComps.Get(vehicle).hookup = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hookup_c");	if (vehComps.Get(vehicle).hookup)		vehComps.Get(vehicle).connector = 3;
                        else {
                            vehComps.Get(vehicle).hookup = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hookup_d");	if (vehComps.Get(vehicle).hookup)		vehComps.Get(vehicle).connector = 4;
                            else {
                                vehComps.Get(vehicle).hookup = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hookup_e");	if (vehComps.Get(vehicle).hookup)		vehComps.Get(vehicle).connector = 5;
                                else {
                                    vehComps.Get(vehicle).hookup = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hookup_f");	if (vehComps.Get(vehicle).hookup)		vehComps.Get(vehicle).connector = 6;
                                }
                            }
                        }
                    }
                }
                if (vehComps.Get(vehicle).connector != 0)
                    vehComps.Get(vehicle).enableTrailer = true;
            }
            else {
                vehComps.Get(vehicle).misc = vehComps.Get(vehicle).hookup = vehComps.Get(vehicle).prop_a = vehComps.Get(vehicle).prop_b = vehComps.Get(vehicle).prop_c = nullptr;
            }
        };

        Events::drawingEvent += [] {
            CPed *player = FindPlayerPed();
            if (player) {
                if (player->m_pVehicle && player->m_pVehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE && vehComps.Get(player->m_pVehicle).misc) {
                    /*CVehicle *trailer = FindTrailer(player->m_pVehicle);
                    gamefont::Print({
                        Format("disableSpeed %d", vehComps.Get(player->m_pVehicle).disableSpeed),
                        Format("MaxVelocity %.2f", player->m_pVehicle->m_pHandlingData->m_transmissionData.m_fMaxForwardsVelocity),
                        Format("ID trailer %d", trailer ? trailer->m_nModelIndex : -1)
                    }, 10, 10, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);*/
                    
                    if (player->m_bInVehicle) {
                        CVehicle *trailer = FindTrailer(player->m_pVehicle);
                        if (trailer) {
                            //TheCamera.m_fCarZoomValueScript = TheCamera.m_fCarZoomValue + CModelInfo::ms_modelInfoPtrs[trailer->m_nModelIndex]->m_pColModel->m_boundBox.m_vecMax.y;
                            TheCamera.m_fCarZoomValueScript = TheCamera.m_fCarZoomValue + (-1.0f * CModelInfo::ms_modelInfoPtrs[player->m_pVehicle->m_nModelIndex]->m_pColModel->m_boundBox.m_vecMin.y);
                            TheCamera.m_bUseScriptZoomValueCar = 1;
                            KeyCheck::Update();
                            if (KeyCheck::CheckWithDelay(VK_BACK, 200)) {
                                trailer->m_matrix.pos = PointOffset(trailer->m_matrix, 0, -1.6f, 0);
                                trailer->m_nVehicleFlags.bEngineOn = 0;
                                vehComps.Get(trailer).isAttached = false;
                            }
                            if (vehComps.Get(player->m_pVehicle).disableSpeed && !CTheScripts::IsPlayerOnAMission()) {
                                player->m_pVehicle->m_pHandlingData->m_transmissionData.m_fMaxForwardsVelocity *= 0.5f;
                                vehComps.Get(player->m_pVehicle).disableSpeed = false;
                            }
                        }
                        else {
                            if (TheCamera.m_bUseScriptZoomValueCar) {
                                TheCamera.m_fCarZoomValueScript = 0.0f;
                                TheCamera.m_bUseScriptZoomValueCar = 0;
                            }
                            if (!vehComps.Get(player->m_pVehicle).disableSpeed) {
                                player->m_pVehicle->m_pHandlingData->m_transmissionData.m_fMaxForwardsVelocity *= 2.0f;
                                vehComps.Get(player->m_pVehicle).disableSpeed = true;
                            }
                        }
                    }
                    else {
                        if (TheCamera.m_bUseScriptZoomValueCar) {
                            TheCamera.m_fCarZoomValueScript = 0.0f;
                            TheCamera.m_bUseScriptZoomValueCar = 0;
                            TheCamera.m_fCarZoomValueSmooth = TheCamera.m_fCarZoomValue;
                        }
                        if (!vehComps.Get(player->m_pVehicle).disableSpeed) {
                            player->m_pVehicle->m_pHandlingData->m_transmissionData.m_fMaxForwardsVelocity *= 2.0f;
                            vehComps.Get(player->m_pVehicle).disableSpeed = true;
                        }
                    }
                }
            }
        };

        Events::vehicleRenderEvent.before += [](CVehicle *vehicle) {
            CPed *player = FindPlayerPed();
            if (player) {
                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                    if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE && vehicle->m_fHealth > 0.1f) {
                        if (vehComps.Get(vehicle).hookup) {
                            vehicle->m_nVehicleFlags.bEngineOn = 0;
                            if (vehComps.Get(vehicle).prop_a)
                                vehComps.Get(vehicle).prop_a->modelling.pos.z = -0.4f;
                            if (vehComps.Get(vehicle).prop_b)
                                vehComps.Get(vehicle).prop_b->modelling.pos.z = -0.2f;
                            if (vehComps.Get(vehicle).prop_c)
                                FrameSetRotateXOnly(vehComps.Get(vehicle).prop_c, 1.65f);
                            CVehicle *tractor = FindTractor(vehicle);
                            if (tractor)
                                ProcessControlTrailers(tractor, vehicle);
                        }
                        // спавн прицепа
                        else if (!CTheScripts::IsPlayerOnAMission() && vehicle->m_pDriver && vehComps.Get(vehicle).enableTrailer && player->m_pVehicle != vehicle) {
                            MyData *entryModel = GetDataInfoForModel(vehicle->m_nModelIndex);
                            if (entryModel) {
                                if (!entryModel->TrailerConst) {
                                    switch (plugin::Random(0, 2)) {
                                    case 0:  vehComps.Get(vehicle).enableTrailer = true;   break;
                                    case 1:  vehComps.Get(vehicle).enableTrailer = false;  break;
                                    case 2:  vehComps.Get(vehicle).enableTrailer = true;   break;
                                    }
                                }
                                switch (plugin::Random(0, 3)) {
                                case 0:  TrailerId = entryModel->TrailerIdOne;   break;
                                case 1:  TrailerId = entryModel->TrailerIdTwo;   break;
                                case 2:  TrailerId = entryModel->TrailerIdThree; break;
                                case 3:  TrailerId = entryModel->TrailerIdFour;  break;
                                }
                                if (vehComps.Get(vehicle).enableTrailer && (CModelInfo::IsVehicleModelType(TrailerId) == 0)) {
                                    if (EnableSpawnTrailer(vehicle, TrailerId)) {
                                        vehComps.Get(vehicle).enableTrailer = false;
                                        if (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) > 50.0f)
                                            SetTrailer(vehicle, TrailerId, entryModel->TrailerColours, entryModel->TrailerExtras, entryModel->TrailerSecond);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        };

        Events::gameProcessEvent += [] {
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                    if (vehComps.Get(vehicle).hookup) {
                        CVehicle *tractor = FindTractor(vehicle);
                        if (!tractor && vehComps.Get(vehicle).isAttached)
                            CWorld::Remove(vehicle);
                    }
                }
            }
        };

    }
} truckTrailer;

VehicleExtendedData<TruckTrailer::VehicleComponents> TruckTrailer::vehComps;
CEntity* TruckTrailer::outEntity = nullptr;
short TruckTrailer::outCount = 0;
float TruckTrailer::offset_Y = 0.0f;
unsigned int TruckTrailer::TrailerId = 0;
