#include "plugin.h"
#include "eVehicleModel.h"
#include "RenderWare.h"
#include "common.h"
#include "CTimer.h"
#include "CVehicleModelInfo.h"

#include "CMessages.h"

RwFrame* nRwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, int data) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwObjectCallBack, int))0x5A2340)(frame, callBack, data);
}

void SetWindowOpenFlag(CVehicle *vehicle, unsigned int component) {
    RwFrame *frame; 
    frame = CClumpModelInfo::GetFrameFromId(vehicle->m_pRwClump, component);
    if (frame)
        frame = nRwFrameForAllObjects(frame, CVehicleModelInfo::SetAtomicFlagCB, 0x1000);
}

void ClearWindowOpenFlag(CVehicle *vehicle, unsigned int component) {
    RwFrame *frame; 
    frame = CClumpModelInfo::GetFrameFromId(vehicle->m_pRwClump, component);
    if (frame)
        frame = nRwFrameForAllObjects(frame, CVehicleModelInfo::ClearAtomicFlagCB, 0x1000);
}



using namespace plugin;

class Test {
public:
    Test() {
        static int keyPressTime = 0;
        Events::gameProcessEvent += [] {
            CVehicle *vehicle = FindPlayerVehicle();
            if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                if (KeyPressed(51) && CTimer::m_snTimeInMilliseconds - keyPressTime > 500) {
                    keyPressTime = CTimer::m_snTimeInMilliseconds;
                    ClearWindowOpenFlag(vehicle, 12);
                    static char mess[256];
                    snprintf(mess, 256, "on: %d", vehicle->m_nModelIndex);
                    CMessages::AddMessageJumpQ(mess, 5000, false);
                }
                if (KeyPressed(50) && CTimer::m_snTimeInMilliseconds - keyPressTime > 500) {
                    keyPressTime = CTimer::m_snTimeInMilliseconds;
                    SetWindowOpenFlag(vehicle, 12);
                    static char message[256];
                    snprintf(message, 256, "off: %d", vehicle->m_nModelIndex);
                    CMessages::AddMessageJumpQ(message, 5000, false);
                }
            }
        };
    }
} test;

//class CanPedEnterCar {
//public:
//    //static bool Can(CVehicle *vehicle) {
//    //    float x; 
//    //    float y; 
//    //    double z; 
//    //    bool result; 
//
//    //    result = FALSE;
//    //    if (vehicle->m_nModelIndex == 90)
//    //        result = TRUE;
//    //    else
//    //        result = FALSE;
//    //    //x = vehicle->m_matrix.at.x;
//    //    //y = vehicle->m_matrix.at.y;
//    //    //z = vehicle->m_matrix.at.z;
//    //    ////if (z > 0.1 || z < -0.1) {
//    //    //    if (vehicle->m_vecMoveSpeed.y * vehicle->m_vecMoveSpeed.y
//    //    //        + vehicle->m_vecMoveSpeed.x * vehicle->m_vecMoveSpeed.x
//    //    //        + vehicle->m_vecMoveSpeed.z * vehicle->m_vecMoveSpeed.z <= 0.04) {
//    //    //        if (vehicle->m_nModelIndex == 90)
//    //    //            result = TRUE;
//    //    //        else
//    //    //            result = FALSE;
//    //    //        /*result = vehicle->m_vecTurnSpeed.y * vehicle->m_vecTurnSpeed.y
//    //    //            + vehicle->m_vecTurnSpeed.x * vehicle->m_vecTurnSpeed.x
//    //    //            + vehicle->m_vecTurnSpeed.z * vehicle->m_vecTurnSpeed.z <= 0.04;*/
//    //    //    }
//    //    //    else
//    //    //        result = TRUE;
//    //    ////}
//    //    ////else 
//    //    //    //result = FALSE;
//    //    return result;
//    //}
//    
//    //static bool __stdcall Siren(int vehicleModel)
//    //{
//    //    bool result; // al@2
//
//    //    switch (vehicleModel)
//    //    {
//    //    case MODEL_FIRETRUK:
//    //    case MODEL_AMBULAN:
//    //    case MODEL_FBICAR:
//    //    case MODEL_MRWHOOP:
//    //    case MODEL_POLICE:
//    //    case MODEL_ENFORCER:
//    //    case MODEL_PREDATOR:
//    //    case MODEL_LANDSTAL:
//    //        result = TRUE;
//    //        break;
//    //    default:
//    //        result = FALSE;
//    //        break;
//    //    }
//    //    return result;
//    //}
//
//    CanPedEnterCar() {
//        //patch::RedirectJump(0x5522F0, Can);
//        //patch::RedirectJump(0x552200, Siren);
//    }
//} canPedEnterCar;
