#include "plugin.h"
#include "CClumpModelInfo.h"
#include "CParticle.h"

using namespace plugin;

class ParticleTest {
public:
    static void CreateParticleOnVehicleNode(CVehicle *vehicle, char const *nodeName) {
        RwFrame *node = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, nodeName); // получить компонент по названию
        if (node) {
            RwMatrix *m = RwFrameGetLTM(node); // получить матрицу компонента в глобальном пространстве
            CParticle::AddParticle(PARTICLE_FLYERS, CVector(m->pos.x, m->pos.y, m->pos.z), CVector(0, 0, 0), nullptr, 0.4f, 0, 0, 0, 0);
        }
    }

    ParticleTest() {
        Events::gameProcessEvent += [] {
            CPed *player = FindPlayerPed();
            if (player && player->m_bInVehicle && player->m_pVehicle && player->m_pVehicle->m_fGasPedal > 0) {
                CreateParticleOnVehicleNode(player->m_pVehicle, "wheel_lb_dummy");
                CreateParticleOnVehicleNode(player->m_pVehicle, "wheel_rb_dummy");
            }
        };
    }
} particleTest;
