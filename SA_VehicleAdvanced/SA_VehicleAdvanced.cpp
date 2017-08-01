/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include "plugin.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "game_sa\CModelInfo.h"
#include "game_sa\CAutomobile.h"

using namespace plugin;
using namespace std;

#define Z_POS_FOR_MODELS_WITHOUT_NODE 1.3f
#define g_TOWTRUCK_HOIST_DOWN_LIMIT *(unsigned short *)0x8D313C

class VehicleAdvanced {
public:
    static vector<unsigned int>& GetTruckIDs() {
        static vector<unsigned int> truckIDs;
        return truckIDs;
    }

    static void ReadSettingsFile() {
        ifstream stream("vehicle_advanced.dat");
        for (string line; getline(stream, line); ) {
            if (line[0] != ';' && line[0] != '#') {
                if (!line.compare("trucks")) {
                    while (getline(stream, line) && line.compare("end")) {
                        if (line[0] != ';' && line[0] != '#') {
                            stringstream ss(line);
                            int i;
                            while (ss >> i) {
                                GetTruckIDs().push_back(i);
                                if (ss.peek() == ',')
                                    ss.ignore();
                            }
                        }
                    }
                }
            }
        }
    }

    static bool GetIsModelTruck(unsigned int modelId) {
        for (unsigned int i : GetTruckIDs()) {
            if (i == modelId)
                return true;
        }
        return false;
    }

    static bool __fastcall MyGetTowBarPos(CAutomobile *automobile, int, CVector &outPos, bool ignoreModelType, CVehicle *attachTo) {
        if (automobile->m_wModelIndex == MODEL_TOWTRUCK || automobile->m_wModelIndex == MODEL_TRACTOR) {
            float yOffset = -1.05f;
            if (automobile->m_wModelIndex == MODEL_TRACTOR) {
                if (attachTo && attachTo->m_nVehicleSubClass == VEHICLE_TRAILER && attachTo->m_wModelIndex != MODEL_FARMTR1)
                    return false;
                yOffset = -0.6f;
            }
            else if (attachTo && attachTo->m_nVehicleSubClass == VEHICLE_TRAILER)
                return false;
            outPos.x = 0.0f;
            outPos.y = yOffset + CModelInfo::ms_modelInfoPtrs[automobile->m_wModelIndex]->m_pColModel->m_boundBox.m_vecSup.y;
            outPos.z = (1.0f - static_cast<float>(automobile->m_wMiscComponentAngle) / static_cast<float>(g_TOWTRUCK_HOIST_DOWN_LIMIT)) * 0.5f + 0.5f - automobile->m_fFrontHeightAboveRoad;
            outPos = *automobile->m_matrix * outPos;
            return true;
        }
        if (GetIsModelTruck(automobile->m_wModelIndex)
            || automobile->m_wModelIndex == MODEL_UTILITY && attachTo && attachTo->m_wModelIndex == MODEL_UTILTR1
            || (automobile->m_wModelIndex == MODEL_BAGGAGE || automobile->m_wModelIndex == MODEL_TUG || automobile->m_wModelIndex == MODEL_BAGBOXA || automobile->m_wModelIndex == MODEL_BAGBOXB)
            && attachTo
            && (attachTo->m_wModelIndex == MODEL_BAGBOXA
                || attachTo->m_wModelIndex == MODEL_BAGBOXB
                || attachTo->m_wModelIndex == MODEL_TUGSTAIR))
        {
            if (automobile->m_aCarNodes[CAR_MISC_A]) {
                RwMatrix *ltm = RwFrameGetLTM(automobile->m_aCarNodes[CAR_MISC_A]);
                outPos.x = ltm->pos.x;
                outPos.y = ltm->pos.y;
                outPos.z = ltm->pos.z;
            }
            else {
                outPos.x = 0.0f;
                outPos.y = CModelInfo::ms_modelInfoPtrs[automobile->m_wModelIndex]->m_pColModel->m_boundBox.m_vecSup.y - 0.5f;
                outPos.z = Z_POS_FOR_MODELS_WITHOUT_NODE - automobile->m_fFrontHeightAboveRoad;
                outPos = *automobile->m_matrix * outPos;
            }
            return true;
        }
        if (ignoreModelType) {
            outPos.x = 0.0f;
            outPos.y = CModelInfo::ms_modelInfoPtrs[automobile->m_wModelIndex]->m_pColModel->m_boundBox.m_vecSup.y - 0.5f;
            outPos.z = 0.5f - automobile->m_fFrontHeightAboveRoad;
            outPos = *automobile->m_matrix * outPos;
            return true;
        }
        return false;
    }

    VehicleAdvanced() {
        patch::RedirectJump(0x6AF250, MyGetTowBarPos);
        ReadSettingsFile();
    }
} vehAdv;
