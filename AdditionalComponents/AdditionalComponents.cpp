#include "plugin.h"
#include "CClumpModelInfo.h"
#include "common.h"
#include "CWeather.h"
#include "extensions\KeyCheck.h"
#include "CMessages.h"
#include "CFont.h"
#include "CCoronas.h"
#include "CGeneral.h"
#include "KeySettings.h"
#include <vector>
#include <string>
#include <fstream>

using namespace plugin;
using namespace std;

class AdditionalComponents {
public:
    struct Zr350Info {
        unsigned int baseModelId;
        float valueMaxLightsAngle;
    };

    static vector<Zr350Info>& GetZr350Infos() {
        static vector<Zr350Info> zr350Infos;
        return zr350Infos;
    }

    static void ReadSettingsFile() {
        ifstream stream("AdditionalComponents.dat");
        for (string line; getline(stream, line); ) {
            if (line[0] != ';' && line[0] != '#') {
                if (!line.compare("zr350")) {
                    while (getline(stream, line) && line.compare("end")) {
                        if (line[0] != ';' && line[0] != '#') {
                            Zr350Info entry;
                            if (sscanf(line.c_str(), "%d, %f", &entry.baseModelId, &entry.valueMaxLightsAngle) == 2)
                                GetZr350Infos().push_back(entry);
                        }
                    }
                }
            }
        }
    }

    static Zr350Info *GetZr350InfoForModel(unsigned int BaseModelId) {
        for (unsigned int i = 0; i < GetZr350Infos().size(); i++) {
            if (GetZr350Infos()[i].baseModelId == BaseModelId)
                return &GetZr350Infos()[i];
        }
        return nullptr;
    }

    enum eWiperState { ONE_STATE_LEFT, ONE_STATE_RIGHT, TWO_STATE_LEFT, TWO_STATE_RIGHT };
    enum eOpenManholeState { STATE_OPENING, STATE_CLOSING, STATE_WAITING };

    static eOpenManholeState m_currentManholeState;
    static eWiperState m_currentWiperOneState, m_currentWiperTwoState;
    static float wiperOneAngle, wiperTwoLAngle, wiperTwoRAngle, cementAngle, manholeAngle;
    static CWeather *wather;

    class VehicleComponents {
    public:
        RwFrame *m_pBootSliding, *m_pSteerWheel, *m_pBootMirage, *m_pWiperOneR, *m_pWiperOneL, *m_pWiperOneM,
            *m_pWiperTwoR, *m_pWiperTwoL, *m_pWiperOneTwoR, *m_pWiperOneTwoL, *m_pBrushOneR, *m_pBrushOneL,
            *m_pDumper, *m_pCement, *m_pManhole_af, *m_pManhole_ab, *m_pManhole_bf, *m_pManhole_bb, *m_pManhole_cf,
            *m_pManhole_cb, *m_pManhole_s, *m_pLightsUp, *m_pStepVanDoorL, *m_pStepVanDoorR, *m_pMiniVanDoorL,
            *m_pMiniVanDoorR, *m_pBootLeft, *m_pBootRight, *m_pBootBottom, *m_pHubLF, *m_pHubRF, *m_pHubLM, *m_pHubRM,
            *m_pHubLB, *m_pHubRB;
        bool wiperState, cementState, manholeState, lightsUpDown;
        float dumperAngle, manholePos, currentLightsAngle, maxLightsAngle;

        VehicleComponents(CVehicle *) {
            m_pBootSliding = m_pBootMirage = m_pSteerWheel = m_pWiperOneR = m_pWiperOneL = m_pWiperOneM = m_pCement
                = m_pWiperTwoR = m_pWiperTwoL = m_pWiperOneTwoR = m_pWiperOneTwoL = m_pBrushOneR = m_pBrushOneL
                = m_pDumper = m_pManhole_af = m_pManhole_ab = m_pManhole_bf = m_pManhole_bb = m_pManhole_cf
                = m_pManhole_cb = m_pManhole_s = m_pLightsUp = m_pStepVanDoorL = m_pStepVanDoorR = m_pHubLF = m_pHubRF
                = m_pHubLM = m_pHubRM = m_pHubLB = m_pHubRB = nullptr;
            wiperState = false; cementState = true; manholeState = true; lightsUpDown = false;
            dumperAngle = manholePos = currentLightsAngle = 0.0f;

        }
    };

    static void FrameSetRotateXOnly(RwFrame *component, float angle) {
        CMatrix matrix(&component->modelling, false);
        matrix.SetRotateXOnly(angle);
        matrix.UpdateRW();
    }

    static void FrameSetRotateYOnly(RwFrame *component, float angle) {
        CMatrix matrix(&component->modelling, false);
        matrix.SetRotateYOnly(angle);
        matrix.UpdateRW();
    }

    static void FrameSetRotateZOnly(RwFrame *component, float angle) {
        CMatrix matrix(&component->modelling, false);
        matrix.SetRotateZOnly(angle);
        matrix.UpdateRW();
    }

    static void FrameSetRotateAndPositionZ(RwFrame *hub, RwFrame *wheel, float sign) {
        float angleZ = CGeneral::GetATanOfXY(sign * wheel->modelling.right.x, sign * wheel->modelling.right.y) - 3.141593f;
        CMatrix matrix(&hub->modelling, false);
        matrix.SetRotateZOnly(angleZ);
        matrix.UpdateRW();
        matrix.pos.z = wheel->modelling.pos.z;
        matrix.UpdateRW();
    }

    static void WiperWorks(CVehicle *vehicle, float angle) {
        FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneR, wiperOneAngle);
        FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneL, wiperOneAngle);
        if (vehComps.Get(vehicle).m_pWiperOneM)
            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneM, wiperOneAngle);
        if (vehComps.Get(vehicle).m_pWiperOneTwoR)
            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneTwoR, wiperOneAngle);
        if (vehComps.Get(vehicle).m_pWiperOneTwoL)
            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneTwoL, wiperOneAngle);
        if (vehComps.Get(vehicle).m_pBrushOneR)
            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pBrushOneR, -wiperOneAngle);
        if (vehComps.Get(vehicle).m_pBrushOneL)
            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pBrushOneL, -wiperOneAngle);
    }

    static void ManholeWorks(CVehicle *vehicle, float angle) {
        if (vehComps.Get(vehicle).m_pManhole_af)
            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pManhole_af, manholeAngle);
        if (vehComps.Get(vehicle).m_pManhole_ab)
            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pManhole_ab, -manholeAngle);
        if (vehComps.Get(vehicle).m_pManhole_bf)
            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pManhole_bf, manholeAngle);
        if (vehComps.Get(vehicle).m_pManhole_bb)
            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pManhole_bb, -manholeAngle);
        if (vehComps.Get(vehicle).m_pManhole_cf)
            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pManhole_cf, manholeAngle);
        if (vehComps.Get(vehicle).m_pManhole_cb)
            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pManhole_cb, -manholeAngle);
    }

    static VehicleExtendedData<VehicleComponents> vehComps;

    AdditionalComponents() {
        ReadSettingsFile();

        Events::onMenuDrawingEvent += [] {
            CFont::SetScale(0.5f, 1.0f);
            CFont::SetColor(CRGBA(238, 173, 53, 255));
            CFont::SetJustifyOn();
            CFont::SetFontStyle(0);
            CFont::SetPropOn();
            CFont::SetWrapx(600.0f);
            wchar_t text[64];
            swprintf(text, L"Additional Components by kenking (14.03.2018)");
            CFont::PrintString(25.0f, 25.0f, text);
        };

        Events::vehicleSetModelEvent += [](CVehicle *vehicle, int modelIndex) {
            if (vehicle) {
                Zr350Info *entryModel = GetZr350InfoForModel(vehicle->m_nModelIndex);
                if (entryModel) {
                    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                    automobile->m_carDamage.SetLightStatus(LIGHT_FRONT_LEFT, 1);
                    automobile->m_carDamage.SetLightStatus(LIGHT_FRONT_RIGHT, 1);
                    vehComps.Get(vehicle).maxLightsAngle = entryModel->valueMaxLightsAngle;
                }
            }

            if (vehicle->m_pRwClump) {
                vehComps.Get(vehicle).m_pBootSliding = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "boot_sliding");
                vehComps.Get(vehicle).m_pBootMirage = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "boot_mirage");
                vehComps.Get(vehicle).m_pSteerWheel = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "steerwheel");
                vehComps.Get(vehicle).m_pWiperOneR = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_or");
                vehComps.Get(vehicle).m_pWiperOneL = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_ol");
                vehComps.Get(vehicle).m_pWiperOneM = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_om");
                vehComps.Get(vehicle).m_pWiperTwoR = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_tr");
                vehComps.Get(vehicle).m_pWiperTwoL = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_tl");
                vehComps.Get(vehicle).m_pWiperOneTwoR = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_otr");
                vehComps.Get(vehicle).m_pWiperOneTwoL = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_otl");
                vehComps.Get(vehicle).m_pBrushOneR = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "brush_or");
                vehComps.Get(vehicle).m_pBrushOneL = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "brush_ol");
                vehComps.Get(vehicle).m_pDumper = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "dumper");
                vehComps.Get(vehicle).m_pCement = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "cement");
                vehComps.Get(vehicle).m_pManhole_af = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hatch_af_hi");
                vehComps.Get(vehicle).m_pManhole_ab = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hatch_ab_hi");
                vehComps.Get(vehicle).m_pManhole_bf = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hatch_bf_hi");
                vehComps.Get(vehicle).m_pManhole_bb = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hatch_bb_hi");
                vehComps.Get(vehicle).m_pManhole_cf = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hatch_cf_hi");
                vehComps.Get(vehicle).m_pManhole_cb = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hatch_cb_hi");
                vehComps.Get(vehicle).m_pManhole_s = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hatch_s_hi");
                vehComps.Get(vehicle).m_pLightsUp = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "lights_up");
                vehComps.Get(vehicle).m_pStepVanDoorL = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_slf");
                vehComps.Get(vehicle).m_pStepVanDoorR = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_srf");
                vehComps.Get(vehicle).m_pMiniVanDoorL = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_mlr");
                vehComps.Get(vehicle).m_pMiniVanDoorR = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_mrr");
                vehComps.Get(vehicle).m_pBootLeft = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "boot_l");
                vehComps.Get(vehicle).m_pBootRight = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "boot_r");
                vehComps.Get(vehicle).m_pBootBottom = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "boot_b");
                vehComps.Get(vehicle).m_pHubLF = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hub_lf");
                vehComps.Get(vehicle).m_pHubRF = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hub_rf");
                vehComps.Get(vehicle).m_pHubLM = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hub_lm");
                vehComps.Get(vehicle).m_pHubRM = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hub_rm");
                vehComps.Get(vehicle).m_pHubLB = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hub_lb");
                vehComps.Get(vehicle).m_pHubRB = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "hub_rb");

            }
            else {
                vehComps.Get(vehicle).m_pBootSliding = vehComps.Get(vehicle).m_pSteerWheel = vehComps.Get(vehicle).m_pWiperOneR
                    = vehComps.Get(vehicle).m_pWiperOneL = vehComps.Get(vehicle).m_pBootMirage = vehComps.Get(vehicle).m_pWiperOneM
                    = vehComps.Get(vehicle).m_pWiperTwoR = vehComps.Get(vehicle).m_pWiperTwoL = vehComps.Get(vehicle).m_pWiperOneTwoR
                    = vehComps.Get(vehicle).m_pWiperOneTwoL = vehComps.Get(vehicle).m_pBrushOneR = vehComps.Get(vehicle).m_pBrushOneL
                    = vehComps.Get(vehicle).m_pDumper = vehComps.Get(vehicle).m_pCement = vehComps.Get(vehicle).m_pManhole_af
                    = vehComps.Get(vehicle).m_pManhole_ab = vehComps.Get(vehicle).m_pManhole_bf = vehComps.Get(vehicle).m_pManhole_bb
                    = vehComps.Get(vehicle).m_pManhole_cf = vehComps.Get(vehicle).m_pManhole_cb = vehComps.Get(vehicle).m_pManhole_s
                    = vehComps.Get(vehicle).m_pLightsUp = vehComps.Get(vehicle).m_pStepVanDoorL = vehComps.Get(vehicle).m_pStepVanDoorR
                    = vehComps.Get(vehicle).m_pMiniVanDoorL = vehComps.Get(vehicle).m_pMiniVanDoorR = vehComps.Get(vehicle).m_pBootLeft
                    = vehComps.Get(vehicle).m_pBootRight = vehComps.Get(vehicle).m_pBootBottom = vehComps.Get(vehicle).m_pHubLF
                    = vehComps.Get(vehicle).m_pHubRF = vehComps.Get(vehicle).m_pHubLM = vehComps.Get(vehicle).m_pHubRM
                    = vehComps.Get(vehicle).m_pHubLB = vehComps.Get(vehicle).m_pHubRB = nullptr;

            }
        };

        Events::gameProcessEvent += [] {
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                if (vehicle && vehicle->m_fHealth > 0.1f && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                    if (vehicle->GetIsOnScreen()) {
                        // trunk shutters
                        if (vehicle->m_pDriver && vehComps.Get(vehicle).m_pBootSliding) {
                            if (automobile->m_aCarNodes[CAR_BOOT] && automobile->m_aCarNodes[CAR_WING_LR]) {
                                if (automobile->m_carDamage.GetPanelStatus(WING_REAR_LEFT) == 0) {
                                    automobile->m_carDamage.SetDoorStatus(BOOT, 0);
                                    vehComps.Get(vehicle).m_pBootSliding->modelling.at.z = 1.0f + automobile->m_aDoors[1].m_fAngle;
                                }
                                else
                                    automobile->m_carDamage.SetDoorStatus(BOOT, 3);
                            }
                        }
                        // trunk with wipers
                        if (vehComps.Get(vehicle).m_pBootMirage && automobile->m_aCarNodes[CAR_BOOT]) {
                            if ((automobile->m_aDoors[1].m_fAngle < 0.0f) && (vehicle->m_nVehicleFlags & 0x20))
                                FrameSetRotateXOnly(automobile->m_aCarNodes[CAR_BOOT], automobile->m_aDoors[1].m_fAngle);
                            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pBootMirage, automobile->m_aDoors[1].m_fAngle);
                            if (automobile->m_carDamage.GetDoorStatus(BOOT) == 3) {
                                if (vehComps.Get(vehicle).m_pWiperOneM)
                                    automobile->SetComponentVisibility(vehComps.Get(vehicle).m_pWiperOneM, 1);
                            }
                        }
                        // steering wheel
                        if (vehicle->m_pDriver && vehComps.Get(vehicle).m_pSteerWheel) {
                            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pSteerWheel, (vehicle->m_fSteerAngle * (-7.0f)));
                        }
                        // wipers type one
                        if (vehComps.Get(vehicle).m_pWiperOneR && vehComps.Get(vehicle).m_pWiperOneL) {
                            if (vehicle->m_pDriver && wather->NewWeatherType == 2)
                                vehComps.Get(vehicle).wiperState = true;
                            if (vehComps.Get(vehicle).wiperState == true) {
                                if (m_currentWiperOneState == ONE_STATE_LEFT) {
                                    wiperOneAngle -= 0.05f;
                                    if (wiperOneAngle > -1.4f)
                                        WiperWorks(vehicle, wiperOneAngle);
                                    else {
                                        wiperOneAngle = -1.4f;
                                        WiperWorks(vehicle, wiperOneAngle);
                                        m_currentWiperOneState = ONE_STATE_RIGHT;
                                    }
                                }
                                else {
                                    wiperOneAngle += 0.05f;
                                    if (wiperOneAngle < 0.0f)
                                        WiperWorks(vehicle, wiperOneAngle);
                                    else {
                                        wiperOneAngle = 0.0f;
                                        WiperWorks(vehicle, wiperOneAngle);
                                        if (!vehicle->m_pDriver)
                                            vehComps.Get(vehicle).wiperState = true;
                                        else
                                            vehComps.Get(vehicle).wiperState = false;
                                        m_currentWiperOneState = ONE_STATE_LEFT;
                                    }
                                }
                            }
                        }
                        // wipers type two
                        if (vehComps.Get(vehicle).m_pWiperTwoR && vehComps.Get(vehicle).m_pWiperTwoL) {
                            if (vehicle->m_pDriver && wather->NewWeatherType == 2)
                                vehComps.Get(vehicle).wiperState = true;
                            if (vehComps.Get(vehicle).wiperState == true) {
                                if (m_currentWiperTwoState == TWO_STATE_LEFT) {
                                    wiperTwoLAngle -= 0.05f;
                                    if (wiperTwoLAngle > -1.1f)
                                        FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperTwoL, wiperTwoLAngle);
                                    if (wiperTwoLAngle < -0.25f) {
                                        wiperTwoRAngle -= 0.05f;
                                        if (wiperTwoRAngle > -1.1f)
                                            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperTwoR, wiperTwoRAngle);
                                        else {
                                            wiperTwoLAngle = wiperTwoRAngle = -1.1f;
                                            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperTwoL, wiperTwoLAngle);
                                            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperTwoR, wiperTwoRAngle);
                                            m_currentWiperTwoState = TWO_STATE_RIGHT;
                                        }
                                    }
                                }
                                else {
                                    wiperTwoRAngle += 0.05f;
                                    if (wiperTwoRAngle < 0.0f)
                                        FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperTwoR, wiperTwoRAngle);
                                    if (wiperTwoRAngle > -0.85f) {
                                        wiperTwoLAngle += 0.05f;
                                        if (wiperTwoLAngle < 0.0f)
                                            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperTwoL, wiperTwoLAngle);
                                        else {
                                            wiperTwoLAngle = wiperTwoRAngle = 0.0f;
                                            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperTwoL, wiperTwoLAngle);
                                            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pWiperTwoR, wiperTwoRAngle);
                                            if (!vehicle->m_pDriver)
                                                vehComps.Get(vehicle).wiperState = true;
                                            else
                                                vehComps.Get(vehicle).wiperState = false;
                                            m_currentWiperTwoState = TWO_STATE_LEFT;
                                        }
                                    }
                                }
                            }
                        }
                        // cement
                        if (vehComps.Get(vehicle).m_pCement && (vehComps.Get(vehicle).cementState == true) && (vehicle->m_nVehicleFlags & 0x10)) {
                            cementAngle += 0.05f;
                            FrameSetRotateYOnly(vehComps.Get(vehicle).m_pCement, cementAngle);
                        }
                        // manhole
                        if (vehicle->m_pDriver) {
                            if (wather->NewWeatherType == 0 && (vehComps.Get(vehicle).manholeState == true)) {
                                manholeAngle = 0.12f;
                                ManholeWorks(vehicle, manholeAngle);
                                vehComps.Get(vehicle).manholeState = false;
                            }
                            else if ((wather->NewWeatherType == 2 || wather->NewWeatherType == 3) && (vehComps.Get(vehicle).manholeState == false)) {
                                manholeAngle = 0.0f;
                                ManholeWorks(vehicle, manholeAngle);
                                vehComps.Get(vehicle).manholeState = true;
                            }
                        }
                        // manhole sliding
                        if (vehicle->m_pDriver && vehComps.Get(vehicle).m_pManhole_s) {
                            CPed *player = FindPlayerPed();
                            if (player && player != vehicle->m_pDriver) {
                                switch (m_currentManholeState) {
                                case STATE_OPENING:
                                    vehComps.Get(vehicle).manholePos -= 0.01f;
                                    if (vehComps.Get(vehicle).manholePos > -0.5f)
                                        vehComps.Get(vehicle).m_pManhole_s->modelling.pos.y = vehComps.Get(vehicle).manholePos;
                                    else {
                                        vehComps.Get(vehicle).manholePos = -0.5f;
                                        vehComps.Get(vehicle).m_pManhole_s->modelling.pos.y = vehComps.Get(vehicle).manholePos;
                                        m_currentManholeState = STATE_WAITING;
                                    }
                                    break;
                                case STATE_CLOSING:
                                    vehComps.Get(vehicle).manholePos += 0.01f;
                                    if (vehComps.Get(vehicle).manholePos < 0.0f)
                                        vehComps.Get(vehicle).m_pManhole_s->modelling.pos.y = vehComps.Get(vehicle).manholePos;
                                    else {
                                        vehComps.Get(vehicle).manholePos = 0.0f;
                                        vehComps.Get(vehicle).m_pManhole_s->modelling.pos.y = vehComps.Get(vehicle).manholePos;
                                        m_currentManholeState = STATE_WAITING;
                                    }
                                    break;
                                case STATE_WAITING:
                                    if (wather->NewWeatherType == 0 && vehComps.Get(vehicle).manholePos == 0.0f)
                                        m_currentManholeState = STATE_OPENING;
                                    else if ((wather->NewWeatherType == 2 || wather->NewWeatherType == 3) && vehComps.Get(vehicle).manholePos == -0.5f)
                                        m_currentManholeState = STATE_CLOSING;
                                    break;
                                }
                            }
                        }
                        // step van doors
                        if (vehComps.Get(vehicle).m_pStepVanDoorL && automobile->m_aCarNodes[CAR_DOOR_LF]) {
                            if (automobile->m_aDoors[2].m_fAngle > -0.9f)
                                vehComps.Get(vehicle).m_pStepVanDoorL->modelling.pos.y = automobile->m_aDoors[2].m_fAngle;
                            if (automobile->m_aCarNodes[CAR_WING_LR]) {
                                if (automobile->m_carDamage.GetPanelStatus(WING_REAR_LEFT) == 3) {
                                    if (automobile->m_carDamage.GetDoorStatus(DOOR_FRONT_LEFT) != 3)
                                        automobile->m_carDamage.SetDoorStatus(DOOR_FRONT_LEFT, 3);
                                }
                                else if (automobile->m_carDamage.GetDoorStatus(DOOR_FRONT_LEFT) == 3)
                                    automobile->m_carDamage.SetDoorStatus(DOOR_FRONT_LEFT, 0);
                            }
                        }
                        if (vehComps.Get(vehicle).m_pStepVanDoorR && automobile->m_aCarNodes[CAR_DOOR_RF]) {
                            if (automobile->m_aDoors[3].m_fAngle < 0.9f)
                                vehComps.Get(vehicle).m_pStepVanDoorR->modelling.pos.y = -automobile->m_aDoors[3].m_fAngle;
                            if (automobile->m_aCarNodes[CAR_WING_RR]) {
                                if (automobile->m_carDamage.GetPanelStatus(WING_REAR_RIGHT) == 3) {
                                    if (automobile->m_carDamage.GetDoorStatus(DOOR_FRONT_RIGHT) != 3)
                                        automobile->m_carDamage.SetDoorStatus(DOOR_FRONT_RIGHT, 3);
                                }
                                else if (automobile->m_carDamage.GetDoorStatus(DOOR_FRONT_RIGHT) == 3)
                                    automobile->m_carDamage.SetDoorStatus(DOOR_FRONT_RIGHT, 0);
                            }
                        }
                        // mini van doors
                        if (vehComps.Get(vehicle).m_pMiniVanDoorL && automobile->m_aCarNodes[CAR_DOOR_LR]) {
                            if (automobile->m_aDoors[4].m_fAngle > -1.1f)
                                vehComps.Get(vehicle).m_pMiniVanDoorL->modelling.pos.y = automobile->m_aDoors[4].m_fAngle;
                            if (automobile->m_aDoors[4].m_fAngle != 0.0f)
                                vehComps.Get(vehicle).m_pMiniVanDoorL->modelling.pos.x = -0.15f;
                            else
                                vehComps.Get(vehicle).m_pMiniVanDoorL->modelling.pos.x = 0.0f;
                            if (automobile->m_aCarNodes[CAR_WING_LF]) {
                                if (automobile->m_carDamage.GetPanelStatus(WING_FRONT_LEFT) == 3) {
                                    if (automobile->m_carDamage.GetDoorStatus(DOOR_REAR_LEFT) != 3)
                                        automobile->m_carDamage.SetDoorStatus(DOOR_REAR_LEFT, 3);
                                }
                                else if (automobile->m_carDamage.GetDoorStatus(DOOR_REAR_LEFT) == 3)
                                    automobile->m_carDamage.SetDoorStatus(DOOR_REAR_LEFT, 0);
                            }
                        }
                        if (vehComps.Get(vehicle).m_pMiniVanDoorR && automobile->m_aCarNodes[CAR_DOOR_RR]) {
                            if (automobile->m_aDoors[5].m_fAngle < 1.1f)
                                vehComps.Get(vehicle).m_pMiniVanDoorR->modelling.pos.y = -automobile->m_aDoors[5].m_fAngle;
                            if (automobile->m_aDoors[5].m_fAngle != 0.0f)
                                vehComps.Get(vehicle).m_pMiniVanDoorR->modelling.pos.x = 0.15f;
                            else
                                vehComps.Get(vehicle).m_pMiniVanDoorR->modelling.pos.x = 0.0f;
                            if (automobile->m_aCarNodes[CAR_WING_RF]) {
                                if (automobile->m_carDamage.GetPanelStatus(WING_FRONT_RIGHT) == 3) {
                                    if (automobile->m_carDamage.GetDoorStatus(DOOR_REAR_RIGHT) != 3)
                                        automobile->m_carDamage.SetDoorStatus(DOOR_REAR_RIGHT, 3);
                                }
                                else if (automobile->m_carDamage.GetDoorStatus(DOOR_REAR_RIGHT) == 3)
                                    automobile->m_carDamage.SetDoorStatus(DOOR_REAR_RIGHT, 0);
                            }
                        }
                        // dual trunk 
                        if (vehComps.Get(vehicle).m_pBootRight && automobile->m_aCarNodes[CAR_BOOT]) {
                            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pBootRight, 0.3f * automobile->m_aDoors[1].m_fAngle);
                        }
                        if (vehComps.Get(vehicle).m_pBootLeft && automobile->m_aCarNodes[CAR_BOOT]) {
                            FrameSetRotateZOnly(vehComps.Get(vehicle).m_pBootLeft, 1.3f * automobile->m_aDoors[1].m_fAngle);
                        }
                        if (vehComps.Get(vehicle).m_pBootBottom && automobile->m_aCarNodes[CAR_BOOT]) {
                            FrameSetRotateXOnly(vehComps.Get(vehicle).m_pBootBottom, -1.3f * automobile->m_aDoors[1].m_fAngle);
                        }
                        // hub
                        if (vehComps.Get(vehicle).m_pHubLF && automobile->m_aCarNodes[CAR_WHEEL_LF])
                            FrameSetRotateAndPositionZ(vehComps.Get(vehicle).m_pHubLF, automobile->m_aCarNodes[CAR_WHEEL_LF], 1.0f);
                        if (vehComps.Get(vehicle).m_pHubLB && automobile->m_aCarNodes[CAR_WHEEL_LB]) {
                            FrameSetRotateAndPositionZ(vehComps.Get(vehicle).m_pHubLB, automobile->m_aCarNodes[CAR_WHEEL_LB], 1.0f);
                            if (vehComps.Get(vehicle).m_pHubLM && automobile->m_aCarNodes[CAR_WHEEL_LM])
                                FrameSetRotateAndPositionZ(vehComps.Get(vehicle).m_pHubLM, automobile->m_aCarNodes[CAR_WHEEL_LM], 1.0f);
                        }
                        if (vehComps.Get(vehicle).m_pHubRF && automobile->m_aCarNodes[CAR_WHEEL_RF])
                            FrameSetRotateAndPositionZ(vehComps.Get(vehicle).m_pHubRF, automobile->m_aCarNodes[CAR_WHEEL_RF], -1.0f);
                        if (vehComps.Get(vehicle).m_pHubRB && automobile->m_aCarNodes[CAR_WHEEL_RB]) {
                            FrameSetRotateAndPositionZ(vehComps.Get(vehicle).m_pHubRB, automobile->m_aCarNodes[CAR_WHEEL_RB], -1.0f);
                            if (vehComps.Get(vehicle).m_pHubRM && automobile->m_aCarNodes[CAR_WHEEL_RM])
                                FrameSetRotateAndPositionZ(vehComps.Get(vehicle).m_pHubRM, automobile->m_aCarNodes[CAR_WHEEL_RM], -1.0f);
                        }
                        //

                    } // vehicle->GetIsOnScreen

                      // lights up/down
                    if (vehicle->m_pDriver && vehComps.Get(vehicle).m_pLightsUp) {
                        if (vehicle->m_nVehicleFlags & 0x40) {
                            if (vehComps.Get(vehicle).lightsUpDown == false) {
                                vehComps.Get(vehicle).currentLightsAngle += 0.1f;
                                if (vehComps.Get(vehicle).currentLightsAngle < vehComps.Get(vehicle).maxLightsAngle)
                                    FrameSetRotateXOnly(vehComps.Get(vehicle).m_pLightsUp, vehComps.Get(vehicle).currentLightsAngle);
                                else {
                                    vehComps.Get(vehicle).currentLightsAngle = vehComps.Get(vehicle).maxLightsAngle;
                                    FrameSetRotateXOnly(vehComps.Get(vehicle).m_pLightsUp, vehComps.Get(vehicle).currentLightsAngle);
                                    automobile->m_carDamage.SetLightStatus(LIGHT_FRONT_LEFT, 0);
                                    automobile->m_carDamage.SetLightStatus(LIGHT_FRONT_RIGHT, 0);
                                    vehComps.Get(vehicle).lightsUpDown = true;
                                }
                            }
                        }
                        else if (vehComps.Get(vehicle).lightsUpDown == true) {
                            automobile->m_carDamage.SetLightStatus(LIGHT_FRONT_LEFT, 1);
                            automobile->m_carDamage.SetLightStatus(LIGHT_FRONT_RIGHT, 1);
                            vehComps.Get(vehicle).currentLightsAngle -= 0.1f;
                            if (vehComps.Get(vehicle).currentLightsAngle > 0.0f)
                                FrameSetRotateXOnly(vehComps.Get(vehicle).m_pLightsUp, vehComps.Get(vehicle).currentLightsAngle);
                            else {
                                vehComps.Get(vehicle).currentLightsAngle = 0.0f;
                                FrameSetRotateXOnly(vehComps.Get(vehicle).m_pLightsUp, vehComps.Get(vehicle).currentLightsAngle);
                                vehComps.Get(vehicle).lightsUpDown = false;
                            }
                        }
                    }
                    //

                }
            }
        };

        Events::drawingEvent += [] {
            CVehicle *playerVehicle = FindPlayerVehicle();
            if (playerVehicle && playerVehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                CAutomobile *playerAutomobile = reinterpret_cast<CAutomobile *>(playerVehicle);
                // dumper
                if (vehComps.Get(playerVehicle).m_pDumper) {
                    if (KeyPressed(settings.keyOpenClose) && KeyPressed(settings.keyOpen)) {
                        vehComps.Get(playerVehicle).dumperAngle += 0.01f;
                        if (vehComps.Get(playerVehicle).dumperAngle < 0.7f)
                            FrameSetRotateXOnly(vehComps.Get(playerVehicle).m_pDumper, vehComps.Get(playerVehicle).dumperAngle);
                        else {
                            CMessages::AddMessageJumpQ(L"full up", 1000, 0);
                            vehComps.Get(playerVehicle).dumperAngle = 0.7f;
                            FrameSetRotateXOnly(vehComps.Get(playerVehicle).m_pDumper, vehComps.Get(playerVehicle).dumperAngle);
                        }
                    }
                    if (KeyPressed(settings.keyOpenClose) && KeyPressed(settings.keyClose)) {
                        vehComps.Get(playerVehicle).dumperAngle -= 0.01f;
                        if (vehComps.Get(playerVehicle).dumperAngle > 0.0f)
                            FrameSetRotateXOnly(vehComps.Get(playerVehicle).m_pDumper, vehComps.Get(playerVehicle).dumperAngle);
                        else {
                            CMessages::AddMessageJumpQ(L"full down", 1000, 0);
                            vehComps.Get(playerVehicle).dumperAngle = 0.0f;
                            FrameSetRotateXOnly(vehComps.Get(playerVehicle).m_pDumper, vehComps.Get(playerVehicle).dumperAngle);
                        }
                    }
                }
                // cement 
                KeyCheck::Update();
                if (KeyCheck::CheckWithDelay(settings.keyOnOff, 1000) && vehComps.Get(playerVehicle).m_pCement) {
                    if (vehComps.Get(playerVehicle).cementState == true)
                        vehComps.Get(playerVehicle).cementState = false;
                    else
                        vehComps.Get(playerVehicle).cementState = true;
                }
                // manhole sliding
                if (vehComps.Get(playerVehicle).m_pManhole_s) {
                    if (KeyPressed(settings.keyOpenClose) && KeyPressed(settings.keyOpen)) {
                        vehComps.Get(playerVehicle).manholePos -= 0.01f;
                        if (vehComps.Get(playerVehicle).manholePos > -0.5f)
                            vehComps.Get(playerVehicle).m_pManhole_s->modelling.pos.y = vehComps.Get(playerVehicle).manholePos;
                        else {
                            CMessages::AddMessageJumpQ(L"full open", 1000, 0);
                            vehComps.Get(playerVehicle).manholePos = -0.5f;
                            vehComps.Get(playerVehicle).m_pManhole_s->modelling.pos.y = vehComps.Get(playerVehicle).manholePos;
                        }
                    }
                    if (KeyPressed(settings.keyOpenClose) && KeyPressed(settings.keyClose)) {
                        vehComps.Get(playerVehicle).manholePos += 0.01f;
                        if (vehComps.Get(playerVehicle).manholePos < 0.0f)
                            vehComps.Get(playerVehicle).m_pManhole_s->modelling.pos.y = vehComps.Get(playerVehicle).manholePos;
                        else {
                            CMessages::AddMessageJumpQ(L"full closed", 1000, 0);
                            vehComps.Get(playerVehicle).manholePos = 0.0f;
                            vehComps.Get(playerVehicle).m_pManhole_s->modelling.pos.y = vehComps.Get(playerVehicle).manholePos;
                        }
                    }
                }
                // boot close
                if (KeyCheck::CheckWithDelay(settings.keyBootClose, 1000) && playerAutomobile->m_aCarNodes[CAR_BOOT])
                    playerAutomobile->OpenDoor(18, BOOT, 0.0f);
                //

            }
        };

    }
} AdditionalComponents;

VehicleExtendedData<AdditionalComponents::VehicleComponents> AdditionalComponents::vehComps;
AdditionalComponents::eWiperState AdditionalComponents::m_currentWiperOneState = ONE_STATE_LEFT;
AdditionalComponents::eWiperState AdditionalComponents::m_currentWiperTwoState = TWO_STATE_LEFT;
AdditionalComponents::eOpenManholeState AdditionalComponents::m_currentManholeState = STATE_WAITING;
float AdditionalComponents::wiperOneAngle = 0.0f;
float AdditionalComponents::wiperTwoLAngle = 0.0f;
float AdditionalComponents::wiperTwoRAngle = 0.0f;
float AdditionalComponents::cementAngle = 0.0f;
float AdditionalComponents::manholeAngle = 0.0f;



class Turnlights {
public:
    enum eLightsStatus { LIGHTS_OFF, LIGHTS_LEFT, LIGHTS_RIGHT, LIGHTS_BOTH };
    enum eBlinksStatus { BLINKS_ENABLE, BLINKS_DISABLE, BLINKS_IGNORE };
    static unsigned int i, j;
    static const int arrayNumber = 36;
    static CWeather *wather;

    class VehicleTurnlightsData {
    public:
        eLightsStatus lightsStatus;
        eBlinksStatus blinksStatus;
        bool turnIgnore, fogEnable, fogExtraEnable;
        RwFrame *m_pTurn[arrayNumber];

        VehicleTurnlightsData(CVehicle *) {
            for (i = 0; i < arrayNumber; i++) {
                m_pTurn[i] = nullptr;
            }
            lightsStatus = LIGHTS_OFF; blinksStatus = BLINKS_DISABLE;
            turnIgnore = false; fogEnable = false; fogExtraEnable = false;
        }
    };

    static VehicleExtendedData<VehicleTurnlightsData> turnlightsData;

    Turnlights() {

        Events::vehicleSetModelEvent += [](CVehicle *vehicle, int modelIndex) {
            if (vehicle->m_pRwClump) {
                turnlightsData.Get(vehicle).m_pTurn[0] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_lf");
                turnlightsData.Get(vehicle).m_pTurn[1] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_lm");
                turnlightsData.Get(vehicle).m_pTurn[2] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_ln");
                turnlightsData.Get(vehicle).m_pTurn[3] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_lb");
                turnlightsData.Get(vehicle).m_pTurn[4] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_rf");
                turnlightsData.Get(vehicle).m_pTurn[5] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_rm");
                turnlightsData.Get(vehicle).m_pTurn[6] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_rn");
                turnlightsData.Get(vehicle).m_pTurn[7] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_rb");
                turnlightsData.Get(vehicle).m_pTurn[8] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_wlf");
                turnlightsData.Get(vehicle).m_pTurn[9] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_wlr");
                turnlightsData.Get(vehicle).m_pTurn[10] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_wrf");
                turnlightsData.Get(vehicle).m_pTurn[11] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_wrr");
                turnlightsData.Get(vehicle).m_pTurn[12] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_wlm");
                turnlightsData.Get(vehicle).m_pTurn[13] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_wrm");
                turnlightsData.Get(vehicle).m_pTurn[14] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turnr_lb");
                turnlightsData.Get(vehicle).m_pTurn[15] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turnr_rb");
                turnlightsData.Get(vehicle).m_pTurn[16] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turnr_wlr");
                turnlightsData.Get(vehicle).m_pTurn[17] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turnr_wrr");
                turnlightsData.Get(vehicle).m_pTurn[18] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_blf");
                turnlightsData.Get(vehicle).m_pTurn[19] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "turn_brf");

                turnlightsData.Get(vehicle).m_pTurn[20] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "fog_l");
                turnlightsData.Get(vehicle).m_pTurn[21] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "fog_r");
                turnlightsData.Get(vehicle).m_pTurn[22] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "fog_lb");
                turnlightsData.Get(vehicle).m_pTurn[23] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "fog_rb");
                turnlightsData.Get(vehicle).m_pTurn[24] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "fogex_l");
                turnlightsData.Get(vehicle).m_pTurn[25] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "fogex_r");
                turnlightsData.Get(vehicle).m_pTurn[26] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "fogex_lm");
                turnlightsData.Get(vehicle).m_pTurn[27] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "fogex_rm");

                turnlightsData.Get(vehicle).m_pTurn[28] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "day_l");
                turnlightsData.Get(vehicle).m_pTurn[29] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "day_r");

                turnlightsData.Get(vehicle).m_pTurn[30] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "blink_l");
                turnlightsData.Get(vehicle).m_pTurn[31] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "blink_r");
                turnlightsData.Get(vehicle).m_pTurn[32] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "blink_lm");
                turnlightsData.Get(vehicle).m_pTurn[33] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "blink_rm");

                turnlightsData.Get(vehicle).m_pTurn[34] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "break_l");
                turnlightsData.Get(vehicle).m_pTurn[35] = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "break_r");
            }
            else {
                for (i = 0; i < arrayNumber; i++) {
                    turnlightsData.Get(vehicle).m_pTurn[i] = nullptr;
                }
            }
        };

        Events::vehicleRenderEvent.before += [](CVehicle *vehicle) {

            if (vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE && (vehicle->m_nVehicleFlags & 0x10) && vehicle->m_fHealth > 0.1f) {
                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                eLightsStatus &lightsStatus = turnlightsData.Get(vehicle).lightsStatus;
                eBlinksStatus &blinksStatus = turnlightsData.Get(vehicle).blinksStatus;

                if (vehicle->m_pDriver) {
                    CPed *playa = FindPlayerPed();
                    if (playa && playa->m_pVehicle == vehicle) {
                        turnlightsData.Get(vehicle).turnIgnore = true;
                        if (KeyPressed(settings.keyTurnL)) { // Z
                            UpdateLightStatus(vehicle); lightsStatus = LIGHTS_LEFT;
                        }
                        else if (KeyPressed(settings.keyTurnLR)) { // X
                            UpdateLightStatus(vehicle); lightsStatus = LIGHTS_BOTH;
                        }
                        else if (KeyPressed(settings.keyTurnR)) { // C
                            UpdateLightStatus(vehicle); lightsStatus = LIGHTS_RIGHT;
                        }
                        else if (KeyPressed(settings.keyTurnOff)) {
                            UpdateLightStatus(vehicle); lightsStatus = LIGHTS_OFF;
                        }
                        // blink
                        KeyCheck::Update();
                        if (KeyCheck::CheckWithDelay(settings.keyBlink, 1000)) {
                            if (blinksStatus == BLINKS_DISABLE || blinksStatus == BLINKS_IGNORE)
                                blinksStatus = BLINKS_ENABLE;
                            else
                                blinksStatus = BLINKS_DISABLE;
                        }
                    }

                    // traffic
                    else {
                        // blink
                        if (blinksStatus == BLINKS_DISABLE) {
                            if (rand() % 3 == 1)
                                blinksStatus = BLINKS_IGNORE;
                            else
                                blinksStatus = BLINKS_ENABLE;
                        }
                        // turn
                        UpdateLightStatus(vehicle); lightsStatus = LIGHTS_OFF;

                        if (lightsStatus == LIGHTS_OFF) {
                            /*if (vehicle->m_fSteerAngle < -0.2f)
                            lightsStatus = LIGHTS_RIGHT;
                            else if (vehicle->m_fSteerAngle > 0.2f)
                            lightsStatus = LIGHTS_LEFT;*/

                            if (vehicle->m_autoPilot.m_nCurrentLane == 0 && vehicle->m_autoPilot.m_nNextLane == 1)
                                lightsStatus = LIGHTS_RIGHT;
                            else if (vehicle->m_autoPilot.m_nCurrentLane == 1 && vehicle->m_autoPilot.m_nNextLane == 0)
                                lightsStatus = LIGHTS_LEFT;
                        }
                    }
                    // traffic end

                    // break
                    for (i = 34, j = 134; i < 36; i++, j++) {
                        if (turnlightsData.Get(vehicle).m_pTurn[i]) {
                            if (vehicle->m_fBreakPedal > 0.0f) {
                                CRGBA color = { 255, 0, 0, 255 };
                                DrawLight(vehicle, j, color, turnlightsData.Get(vehicle).m_pTurn[i]);
                            }
                            else
                                UpdateLight(vehicle, j, turnlightsData.Get(vehicle).m_pTurn[i]);
                        }
                    }

                    // day light
                    if (turnlightsData.Get(vehicle).m_pTurn[28] && turnlightsData.Get(vehicle).m_pTurn[29]) {
                        if (vehicle->m_nVehicleFlags & 0x40) {
                            UpdateLight(vehicle, 128, turnlightsData.Get(vehicle).m_pTurn[28]);
                            UpdateLight(vehicle, 129, turnlightsData.Get(vehicle).m_pTurn[29]);
                        }
                        else {
                            CRGBA color;
                            color = { 255, 255, 255, 200 };
                            DrawLight(vehicle, 128, color, turnlightsData.Get(vehicle).m_pTurn[28]);
                            DrawLight(vehicle, 129, color, turnlightsData.Get(vehicle).m_pTurn[29]);
                        }
                    }

                    if ((vehicle->m_nVehicleFlags & 0x10) && (vehicle->field_1F7 & 0x2) && turnlightsData.Get(vehicle).turnIgnore == false)
                        turnlightsData.Get(vehicle).turnIgnore = true;
                }
                else if ((vehicle->m_nVehicleFlags & 0x10) && (vehicle->field_1F7 & 0x2) && lightsStatus == LIGHTS_OFF && turnlightsData.Get(vehicle).turnIgnore == true)
                    lightsStatus = LIGHTS_BOTH;

                if (CTimer::m_snTimeInMilliseconds & 0x200)
                    DrawVehicleTurnlights(vehicle, lightsStatus);
                else
                    UpdateLightStatus(vehicle);

                // fog
                if (wather->NewWeatherType == 3 && vehicle->m_pDriver && turnlightsData.Get(vehicle).fogEnable == false)
                    turnlightsData.Get(vehicle).fogEnable = true;
                else if (wather->NewWeatherType != 3 && vehicle->m_pDriver && turnlightsData.Get(vehicle).fogEnable == true)
                    turnlightsData.Get(vehicle).fogEnable = false;

                if (turnlightsData.Get(vehicle).fogEnable == true) {
                    CRGBA color = { 255, 186, 0, 200 };
                    if (turnlightsData.Get(vehicle).m_pTurn[20])
                        DrawLight(vehicle, 120, color, turnlightsData.Get(vehicle).m_pTurn[20]);
                    if (turnlightsData.Get(vehicle).m_pTurn[21])
                        DrawLight(vehicle, 121, color, turnlightsData.Get(vehicle).m_pTurn[21]);
                    if (turnlightsData.Get(vehicle).m_pTurn[22] && turnlightsData.Get(vehicle).m_pTurn[23] && vehicle->IsComponentPresent(7)) {
                        if (3 > automobile->m_carDamage.GetPanelStatus(BUMP_FRONT)) {
                            DrawLight(vehicle, 122, color, turnlightsData.Get(vehicle).m_pTurn[22]);
                            DrawLight(vehicle, 123, color, turnlightsData.Get(vehicle).m_pTurn[23]);
                        }
                        else {
                            UpdateLight(vehicle, 122, turnlightsData.Get(vehicle).m_pTurn[22]);
                            UpdateLight(vehicle, 123, turnlightsData.Get(vehicle).m_pTurn[23]);
                        }
                    }
                }
                else {
                    for (i = 20, j = 120; i < 24; i++, j++) {
                        if (turnlightsData.Get(vehicle).m_pTurn[i])
                            UpdateLight(vehicle, j, turnlightsData.Get(vehicle).m_pTurn[i]);
                    }
                }

                // fog extra
                if ((vehicle->m_nExtra[1] == 4 || vehicle->m_nExtra[1] == 5) && (wather->NewWeatherType == 3 || wather->NewWeatherType == 2) && vehicle->m_pDriver && turnlightsData.Get(vehicle).fogExtraEnable == false)
                    turnlightsData.Get(vehicle).fogExtraEnable = true;
                else if (wather->NewWeatherType != 3 && wather->NewWeatherType != 2 && vehicle->m_pDriver && turnlightsData.Get(vehicle).fogExtraEnable == true)
                    turnlightsData.Get(vehicle).fogExtraEnable = false;

                for (i = 24, j = 124; i < 28; i++, j++) {
                    if (turnlightsData.Get(vehicle).m_pTurn[i]) {
                        if (turnlightsData.Get(vehicle).fogExtraEnable == true) {
                            CRGBA color = { 255, 255, 255, 200 };
                            DrawLight(vehicle, j, color, turnlightsData.Get(vehicle).m_pTurn[i]);
                        }
                        else
                            UpdateLight(vehicle, j, turnlightsData.Get(vehicle).m_pTurn[i]);
                    }
                }

                // blink
                if (blinksStatus == BLINKS_ENABLE) {
                    CRGBA color = { 255, 128, 0, 255 };
                    for (i = 30, j = 130; i < 32; i++, j++) {
                        if (turnlightsData.Get(vehicle).m_pTurn[i]) {
                            if (CTimer::m_snTimeInMilliseconds & 0x100)
                                DrawLight(vehicle, j, color, turnlightsData.Get(vehicle).m_pTurn[i]);
                            else
                                UpdateLight(vehicle, j, turnlightsData.Get(vehicle).m_pTurn[i]);
                        }
                    }
                    for (i = 32, j = 132; i < 34; i++, j++) {
                        if (turnlightsData.Get(vehicle).m_pTurn[i]) {
                            if (CTimer::m_snTimeInMilliseconds & 0x200)
                                DrawLight(vehicle, j, color, turnlightsData.Get(vehicle).m_pTurn[i]);
                            else
                                UpdateLight(vehicle, j, turnlightsData.Get(vehicle).m_pTurn[i]);
                        }
                    }
                }
                else {
                    for (i = 30, j = 130; i < 34; i++, j++) {
                        if (turnlightsData.Get(vehicle).m_pTurn[i])
                            UpdateLight(vehicle, j, turnlightsData.Get(vehicle).m_pTurn[i]);
                    }
                }

            }
        };
    }


    static void UpdateLightStatus(CVehicle *vehicle) {
        for (i = 0, j = 100; i < 20; i++, j++) {
            if (turnlightsData.Get(vehicle).m_pTurn[i])
                UpdateLight(vehicle, j, turnlightsData.Get(vehicle).m_pTurn[i]);
        }
    }

    static CVector GetFramePosn(RwFrame *turn) {
        CVector posnCorona;
        RwV3d posn = RwFrameGetLTM(turn)->pos;
        posnCorona.x = posn.x;
        posnCorona.y = posn.y;
        posnCorona.z = posn.z;
        return posnCorona;
    }

    static void DrawLight(CVehicle *vehicle, unsigned int coronaId, CRGBA color, RwFrame *turn) {
        CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(vehicle) + coronaId, color.red, color.green, color.blue, color.alpha, GetFramePosn(turn), 0.3f, 100.0f, 1, 0, 0, 0, 0, 0.0f);
    }

    static void UpdateLight(CVehicle *vehicle, unsigned int coronaId, RwFrame *turn) {
        CCoronas::UpdateCoronaCoors(reinterpret_cast<unsigned int>(vehicle) + coronaId, GetFramePosn(turn), 100.0f, 0.0f);
    }

    static void DrawVehicleTurnlights(CVehicle *vehicle, eLightsStatus lightsStatus) {
        CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
        CRGBA color;
        if (lightsStatus == LIGHTS_BOTH || lightsStatus == LIGHTS_LEFT) {
            color = { 255, 128, 0, 255 };
            for (i = 0, j = 100; i < 4; i++, j++) {
                if (turnlightsData.Get(vehicle).m_pTurn[i])
                    DrawLight(vehicle, j, color, turnlightsData.Get(vehicle).m_pTurn[i]);
            }
            if (turnlightsData.Get(vehicle).m_pTurn[8] && vehicle->IsComponentPresent(13) && (3 > automobile->m_carDamage.GetPanelStatus(WING_FRONT_LEFT)))
                DrawLight(vehicle, 108, color, turnlightsData.Get(vehicle).m_pTurn[8]); // turn_wlf
            if (turnlightsData.Get(vehicle).m_pTurn[9] && vehicle->IsComponentPresent(14) && (3 > automobile->m_carDamage.GetPanelStatus(WING_REAR_LEFT)))
                DrawLight(vehicle, 109, color, turnlightsData.Get(vehicle).m_pTurn[9]); // turn_wlr
            if (turnlightsData.Get(vehicle).m_pTurn[12] && vehicle->IsComponentPresent(13) && (3 > automobile->m_carDamage.GetPanelStatus(WING_FRONT_LEFT)))
                DrawLight(vehicle, 112, color, turnlightsData.Get(vehicle).m_pTurn[12]); // turn_wlm
            if (turnlightsData.Get(vehicle).m_pTurn[18] && vehicle->IsComponentPresent(7) && (3 > automobile->m_carDamage.GetPanelStatus(BUMP_FRONT)))
                DrawLight(vehicle, 118, color, turnlightsData.Get(vehicle).m_pTurn[18]); // turn_blf
            // red
            color = { 255, 0, 0, 255 };
            if (turnlightsData.Get(vehicle).m_pTurn[14])
                DrawLight(vehicle, 114, color, turnlightsData.Get(vehicle).m_pTurn[14]); // turnr_lb
            if (turnlightsData.Get(vehicle).m_pTurn[16] && vehicle->IsComponentPresent(14) && (3 > automobile->m_carDamage.GetPanelStatus(WING_REAR_LEFT)))
                DrawLight(vehicle, 116, color, turnlightsData.Get(vehicle).m_pTurn[16]); // turnr_wlr
        }
        if (lightsStatus == LIGHTS_BOTH || lightsStatus == LIGHTS_RIGHT) {
            color = { 255, 128, 0, 255 };
            for (i = 4, j = 104; i < 8; i++, j++) {
                if (turnlightsData.Get(vehicle).m_pTurn[i])
                    DrawLight(vehicle, j, color, turnlightsData.Get(vehicle).m_pTurn[i]);
            }
            if (turnlightsData.Get(vehicle).m_pTurn[10] && vehicle->IsComponentPresent(9) && (3 > automobile->m_carDamage.GetPanelStatus(WING_FRONT_RIGHT)))
                DrawLight(vehicle, 110, color, turnlightsData.Get(vehicle).m_pTurn[10]); // turn_wrf
            if (turnlightsData.Get(vehicle).m_pTurn[11] && vehicle->IsComponentPresent(10) && (3 > automobile->m_carDamage.GetPanelStatus(WING_REAR_RIGHT)))
                DrawLight(vehicle, 111, color, turnlightsData.Get(vehicle).m_pTurn[11]); // turn_wrr
            if (turnlightsData.Get(vehicle).m_pTurn[13] && vehicle->IsComponentPresent(9) && (3 > automobile->m_carDamage.GetPanelStatus(WING_FRONT_RIGHT)))
                DrawLight(vehicle, 113, color, turnlightsData.Get(vehicle).m_pTurn[13]); // turn_wrm
            if (turnlightsData.Get(vehicle).m_pTurn[19] && vehicle->IsComponentPresent(7) && (3 > automobile->m_carDamage.GetPanelStatus(BUMP_FRONT)))
                DrawLight(vehicle, 119, color, turnlightsData.Get(vehicle).m_pTurn[19]); // turn_brf
            // red
            color = { 255, 0, 0, 255 };
            if (turnlightsData.Get(vehicle).m_pTurn[15])
                DrawLight(vehicle, 115, color, turnlightsData.Get(vehicle).m_pTurn[15]); // turnr_rb
            if (turnlightsData.Get(vehicle).m_pTurn[17] && vehicle->IsComponentPresent(10) && (3 > automobile->m_carDamage.GetPanelStatus(WING_REAR_RIGHT)))
                DrawLight(vehicle, 117, color, turnlightsData.Get(vehicle).m_pTurn[17]); // turnr_wrr
        }
    }

} turnlights;

VehicleExtendedData<Turnlights::VehicleTurnlightsData> Turnlights::turnlightsData;
unsigned int Turnlights::i;
unsigned int Turnlights::j;
