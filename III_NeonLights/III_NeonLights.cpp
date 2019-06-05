#include "plugin.h"
#include "extensions\KeyCheck.h"
#include "common.h"
#include "CClock.h"
#include "CShadows.h"
#include "CModelInfo.h"
#include "CTimer.h"

#define TURN_ON_OFF_DELAY 500

using namespace plugin;
using namespace std;

RwTexture *&pWhiteTex = *(RwTexture **)0x64C4F8;

class NeonLights {
public:
    enum eNeonColor {
        NEON_YELLOW, NEON_GREEN, NEON_RED, NEON_BLUE, NEON_PURPLE
    };

    class Neon {
    public:
        unsigned char color;
        bool activated;
        bool processed;

        Neon(CVehicle *) { activated = processed = false; }

        void Enable(eNeonColor Color) {
            color = Color;
            activated = true;
        }

        void Disable() { activated = false; }
    };

    static VehicleExtendedData<Neon> VehNeon;

    static bool CanEnableNeonOnThisVehicle(CVehicle *vehicle) {
        CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
        return vehicle->m_pDriver
            && (vehModel->m_nVehicleClass == 1 || vehModel->m_nVehicleClass == 2)
            && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE;
    }

    static void ProcessNpcVehicle(CVehicle *vehicle) {
        if (CanEnableNeonOnThisVehicle(vehicle) && !VehNeon.Get(vehicle).processed) {
            VehNeon.Get(vehicle).processed = true;
            if (rand() % 3 == 1) {
                VehNeon.Get(vehicle).activated = true;
                VehNeon.Get(vehicle).color = rand() % 5;
            }
        }
    }

    static void ProcessVehicles() {
        KeyCheck::Update();
        CVehicle *playaVeh = FindPlayerVehicle();
        if (playaVeh && playaVeh->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
            if (KeyCheck::Check(VK_SHIFT)) {
                if (KeyCheck::CheckJustDown('1'))
                    VehNeon.Get(playaVeh).Enable(NEON_YELLOW);
                else if (KeyCheck::CheckJustDown('2'))
                    VehNeon.Get(playaVeh).Enable(NEON_GREEN);
                else if (KeyCheck::CheckJustDown('3'))
                    VehNeon.Get(playaVeh).Enable(NEON_RED);
                else if (KeyCheck::CheckJustDown('4'))
                    VehNeon.Get(playaVeh).Enable(NEON_BLUE);
                else if (KeyCheck::CheckJustDown('5'))
                    VehNeon.Get(playaVeh).Enable(NEON_PURPLE);
                else if (KeyCheck::CheckJustDown('0'))
                    VehNeon.Get(playaVeh).Disable();
            }
        }
        for (auto vehicle : CPools::ms_pVehiclePool) {
            if (vehicle != playaVeh)
                ProcessNpcVehicle(vehicle);
        }
    }

    static void RenderNeonForVehicle(CVehicle *vehicle) {
        if (vehicle->m_fHealth < 1.0f || vehicle->IsUpsideDown())
            VehNeon.Get(vehicle).Disable();
        if (VehNeon.Get(vehicle).activated && CClock::GetIsTimeInRange(21, 6)) {
            unsigned char r, g, b;
            switch (VehNeon.Get(vehicle).color) {
            case NEON_YELLOW: r = 255; g = 200; b = 0;    break;
            case NEON_GREEN:  r = 0;   g = 255; b = 0;    break;
            case NEON_RED:    r = 255; g = 0;   b = 0;    break;
            case NEON_BLUE:   r = 0;   g = 0;   b = 255;  break;
            case NEON_PURPLE: r = 255; g = 0;   b = 255;  break;
            }
            if (CTimer::m_snTimeInMilliseconds % (TURN_ON_OFF_DELAY + 250) < TURN_ON_OFF_DELAY) {
                CVector Pos = CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]->m_pColModel->m_boundBox.m_vecMin;
                CVector center = vehicle->TransformFromObjectSpace(CVector(0.0f, 0.0f, 0.0f));
                CVector up = vehicle->TransformFromObjectSpace(CVector(0.0f, -Pos.y - 0.5f, 0.0f)) - center;
                CVector right = vehicle->TransformFromObjectSpace(CVector(Pos.x + 0.2f, 0.0f, 0.0f)) - center;
                CShadows::StoreShadowToBeRendered(1, pWhiteTex, &center, up.x, up.y, right.x, right.y, 255, r, g, b, 4.5f, false, 1.0f);
            }
        }
    }

    NeonLights() {
        Events::gameProcessEvent += ProcessVehicles;
        Events::vehicleRenderEvent += RenderNeonForVehicle;
    }
} neon;

VehicleExtendedData<NeonLights::Neon> NeonLights::VehNeon;
