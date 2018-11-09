#include "plugin.h"
#include "CClumpModelInfo.h"
#include "CModelInfo.h"
#include "CWorld.h"
#include "extensions\KeyCheck.h"
#include "common.h"
#include <stdlib.h>

#include "CMessages.h"

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

class TruckTrailer {
public:
    class VehicleComponents {
    public:
        RwFrame *drag, *trailer;
        char connector;

        VehicleComponents(CVehicle *) {
            drag = trailer = nullptr;  connector = 0; 
        }
    };
    
    static VehicleExtendedData<VehicleComponents> vehComps;
    
    TruckTrailer() {
        
        Events::vehicleSetModelEvent += [](CVehicle *vehicle, int modelIndex) {
            if (vehicle->m_pRwClump) {
                vehComps.Get(vehicle).connector = 0;
                vehComps.Get(vehicle).drag = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "drag1_dummy");		if (vehComps.Get(vehicle).drag)	   vehComps.Get(vehicle).connector = 1;
                else {
                    vehComps.Get(vehicle).drag = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "drag2_dummy");		if (vehComps.Get(vehicle).drag) 	vehComps.Get(vehicle).connector = 2;
                    else {
                        vehComps.Get(vehicle).drag = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "drag3_dummy");		if (vehComps.Get(vehicle).drag) 	vehComps.Get(vehicle).connector = 3;
                        else {
                            vehComps.Get(vehicle).drag = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "drag4_dummy");		if (vehComps.Get(vehicle).drag) 	vehComps.Get(vehicle).connector = 4;
                            else {
                                vehComps.Get(vehicle).drag = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "drag5_dummy");		if (vehComps.Get(vehicle).drag) 	vehComps.Get(vehicle).connector = 5;
                            }
                        }
                    }
                }
                vehComps.Get(vehicle).trailer = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "trailer1_dummy");	if (vehComps.Get(vehicle).trailer)		vehComps.Get(vehicle).connector = 1;
                else {
                    vehComps.Get(vehicle).trailer = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "trailer2_dummy");	if (vehComps.Get(vehicle).trailer)		vehComps.Get(vehicle).connector = 2;
                    else {
                        vehComps.Get(vehicle).trailer = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "trailer3_dummy");	if (vehComps.Get(vehicle).trailer)		vehComps.Get(vehicle).connector = 3;
                        else {
                            vehComps.Get(vehicle).trailer = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "trailer4_dummy");	if (vehComps.Get(vehicle).trailer)		vehComps.Get(vehicle).connector = 4;
                            else {
                                vehComps.Get(vehicle).trailer = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "trailer5_dummy");	if (vehComps.Get(vehicle).trailer)		vehComps.Get(vehicle).connector = 5;
                            }
                        }
                    }
                }
            }
            else {
                vehComps.Get(vehicle).drag = vehComps.Get(vehicle).trailer = nullptr;
            }
        };
        
        Events::gameProcessEvent += [] {
            //patch::SetFloat(0x6FAE24, 1.0f, true);  // camera
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                if (vehicle && vehicle->m_fHealth > 0.1f && vehComps.Get(vehicle).drag) {
                    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                    for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                        CVehicle *trailer = CPools::ms_pVehiclePool->GetAt(i);
                        if (trailer && trailer->m_fHealth > 0.1f && vehComps.Get(trailer).trailer) {
                            CAutomobile *trail = reinterpret_cast<CAutomobile *>(trailer);
                            if (vehicle && trailer && (vehComps.Get(vehicle).connector == vehComps.Get(trailer).connector) && (Distance(PointOffset(vehicle->m_matrix, 0, vehComps.Get(vehicle).drag->modelling.pos.y, vehComps.Get(vehicle).drag->modelling.pos.z), PointOffset(trailer->m_matrix, 0, vehComps.Get(trailer).trailer->modelling.pos.y, vehComps.Get(trailer).trailer->modelling.pos.z)) < 2.0f)) {
                                CVehicle *playerVehicle = FindPlayerVehicle();
                                if (playerVehicle && (playerVehicle == vehicle)) {
                                    //patch::SetFloat(0x6FAE24, 1.0f + 2 * CModelInfo::ms_modelInfoPtrs[trail->m_nModelIndex]->m_pColModel->m_boundBox.m_vecMax.y, true);	// camera
                                }
                                KeyCheck::Update();
                                if ((KeyCheck::CheckWithDelay(VK_BACK, 200)) && playerVehicle && (playerVehicle == vehicle))
                                    trailer->m_matrix.pos = PointOffset(trailer->m_matrix, 0, -2.1f, 0);
                                else {
                                    bool  find;
                                    float LinkDifferenceZ = trailer->GetDistanceFromCentreOfMassToBaseOfModel() - vehicle->GetDistanceFromCentreOfMassToBaseOfModel();
                                    float TrailerOnGroundZ = min(0, CWorld::FindGroundZFor3DCoord(trailer->m_matrix.pos.x, trailer->m_matrix.pos.y, trailer->m_matrix.pos.z, &find) +
                                        trailer->GetDistanceFromCentreOfMassToBaseOfModel() - trailer->m_matrix.pos.z);
                                    if ((TrailerOnGroundZ < -1.0f) || (!VehicleGetSpeed(vehicle)))
                                        continue;
                                    CVector a = PointOffset(vehicle->m_matrix, 0, vehComps.Get(vehicle).drag->modelling.pos.y, LinkDifferenceZ);
                                    CVector b = PointOffset(trailer->m_matrix, 0, (trail)->m_aCarNodes[CAR_WHEEL_LB]->modelling.pos.y, TrailerOnGroundZ);
                                    float R = Distance(a, b);
                                    float cos_x = float(sqrt(pow(a.y - b.y, 2) + pow(b.x - a.x, 2)) / R);
                                    float sin_x = (a.z - b.z) / R;
                                    float cos_y = float(sqrt(pow(vehicle->m_matrix.right.x, 2) + pow(vehicle->m_matrix.right.y, 2)));
                                    float sin_y = -vehicle->m_matrix.right.z;
                                    float cos_z = (a.y - b.y) / (R * cos_x);
                                    float sin_z = (b.x - a.x) / (R * cos_x);
                                    CMatrix matrix;
                                    matrix.m_bOwnsAttachedMatrix = 0;
                                    matrix.m_pAttachMatrix = 0;
                                    MatrixAttach(&matrix, &trailer->m_matrix, 0);
                                    MatrixSet(&matrix, cos_x, sin_x, cos_y, sin_y, cos_z, sin_z);
                                    matrix.pos = a;
                                    matrix.pos = PointOffset(matrix, 0, -vehComps.Get(trailer).trailer->modelling.pos.y, 0);
                                    matrix.UpdateRW();
                                    matrix.~CMatrix();
                                    for (int w = 0; w < 4; w++)
                                        trail->m_fWheelTotalRot[w] = automobile->m_fWheelTotalRot[w];
                                    float vR = float(sqrt(pow(vehicle->m_vecMoveSpeed.x, 2) + pow(vehicle->m_vecMoveSpeed.y, 2)));
                                    float vx = -vR * sin_z;
                                    float vy = vR * cos_z;
                                    float vz = vehicle->m_vecMoveSpeed.z;
                                    trailer->m_vecMoveSpeed.x = vx;
                                    trailer->m_vecMoveSpeed.y = vy;
                                    trailer->m_vecMoveSpeed.z = vz;
                                    trailer->m_nVehicleFlags = vehicle->m_nVehicleFlags;
                                }
                            }
                        }
                    }
                }
            }
        };
    }
} truckTrailer;

VehicleExtendedData<TruckTrailer::VehicleComponents> TruckTrailer::vehComps;
