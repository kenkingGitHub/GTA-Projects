/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include <plugin_III.h>
#include "game_III\CClumpModelInfo.h"
#include "game_III\common.h"
#include "game_III\CWeather.h"
#include "extensions\KeyCheck.h"
#include "game_III\CMessages.h"

#include "game_III\CFont.h"

using namespace plugin;

class AdditionalComponents {
public:
    enum eWiperState {
        STATE_LEFT, 
        STATE_RIGHT
    };
    
    static eWiperState m_currentWiperState;
    static CWeather *wather;
    static float wiperAngle;

    class VehicleComponents { // Класс, который представляет наши данные (можно сказать, что эти данные "прикрепляются" к структуре транспорта)
    public:
        RwFrame *m_pBootSliding;
        RwFrame *m_pBootMirage;
        RwFrame *m_pSteerWheel;
        RwFrame *m_pWiperOneR;
        RwFrame *m_pWiperOneL;
        RwFrame *m_pWiperOneM;
        bool wiperState;

        VehicleComponents(CVehicle *) { // Конструктор этого класса будет вызван при вызове конструктора транспорта (CVehicle::CVehicle)
            m_pBootSliding = m_pBootMirage = m_pSteerWheel = m_pWiperOneR = m_pWiperOneL = m_pWiperOneM = nullptr;  // устанавливаем все указатели в 0
            wiperState = false;

        }
    };

    static void MatrixSetRotateXOnly(RwFrame *component, float angle) {
        CMatrix matrix(&component->modelling, false);
        matrix.SetRotateXOnly(angle);
        matrix.UpdateRW();
    }

    static void MatrixSetRotateYOnly(RwFrame *component, float angle) {
        CMatrix matrix(&component->modelling, false);
        matrix.SetRotateYOnly(angle);
        matrix.UpdateRW();
    }

    AdditionalComponents() {
        static VehicleExtendedData<VehicleComponents> vehComps; // Создаем экземпляр нашего расширения. vehComps - это переменная, через которую мы будем
                                                                // обращаться к нашим данным (используя метод Get(CVehicle *транспорт) )

        Events::vehicleSetModelEvent += [](CVehicle *vehicle, int modelIndex) { // Выполняем нашу функцию, когда игра устанавливает модель транспорту
            if (vehicle->m_pRwClump) { // Находим компоненты в иерархии и записываем их в наш класс
                vehComps.Get(vehicle).m_pBootSliding = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "boot_sliding");
                vehComps.Get(vehicle).m_pBootMirage = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "boot_mirage");
                vehComps.Get(vehicle).m_pSteerWheel = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "steerwheel"); 
                vehComps.Get(vehicle).m_pWiperOneR = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_or");
                vehComps.Get(vehicle).m_pWiperOneL = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_ol");
                vehComps.Get(vehicle).m_pWiperOneM = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_om");
                
            }
            else {
                vehComps.Get(vehicle).m_pBootSliding = vehComps.Get(vehicle).m_pSteerWheel = vehComps.Get(vehicle).m_pWiperOneR = vehComps.Get(vehicle).m_pWiperOneL = nullptr;
                vehComps.Get(vehicle).m_pBootMirage = vehComps.Get(vehicle).m_pWiperOneM = nullptr;

            }
        };

        Events::drawingEvent += [] {
        //Events::gameProcessEvent += [] {
            //KeyCheck::Update();
            //if (KeyCheck::CheckWithDelay(0xBE, 500)) {
            //    if (m_wiperOnOff == true) {
            //        m_wiperOnOff = false;
            //        CMessages::AddMessageJumpQ(L"state: wiperOff", 2000, 0);
            //    }
            //    else {
            //        m_wiperOnOff = true;
            //        CMessages::AddMessageJumpQ(L"state: wiperOn", 2000, 0);
            //    }
            //}
            
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                if (vehicle && vehicle->GetIsOnScreen() && vehicle->m_fHealth > 0.0f && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                    // багажник роллставни
                    if (vehComps.Get(vehicle).m_pBootSliding) {
                        if (automobile->m_aCarNodes[CAR_BOOT] && automobile->m_aCarNodes[CAR_WING_LR]) {
                            if (automobile->m_carDamage.GetPanelStatus(WING_REAR_LEFT) == 0) {
                                automobile->m_carDamage.SetDoorStatus(BOOT, 0);
                                vehComps.Get(vehicle).m_pBootSliding->modelling.at.z = 1.0f + automobile->m_aDoors[1].m_fAngle;
                            }
                            else 
                                automobile->m_carDamage.SetDoorStatus(BOOT, 3);
                        }
                    }
                    // багажник с дворником
                    if (vehComps.Get(vehicle).m_pBootMirage && automobile->m_aCarNodes[CAR_BOOT]) {
                        if ((automobile->m_aDoors[1].m_fAngle < 0.0f) && (vehicle->m_nVehicleFlags & 0x20)) 
                            MatrixSetRotateXOnly(automobile->m_aCarNodes[CAR_BOOT], automobile->m_aDoors[1].m_fAngle);
                        MatrixSetRotateXOnly(vehComps.Get(vehicle).m_pBootMirage, automobile->m_aDoors[1].m_fAngle);
                        RwV3d scaleBootMirage;
                        if (automobile->m_carDamage.GetDoorStatus(BOOT) == 3) 
                            scaleBootMirage = { 0.0f, 0.0f, 0.0f };
                        else 
                            scaleBootMirage = { 1.0f, 1.0f, 1.0f };
                        RwFrameScale(vehComps.Get(vehicle).m_pBootMirage, &scaleBootMirage, rwCOMBINEPRECONCAT);
                    }
                    // руль
                    if (vehComps.Get(vehicle).m_pSteerWheel) {
                        MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pSteerWheel, (vehicle->m_fSteerAngle * (-7.0f)));
                    }
                    // дворники тип 1
                    if (vehComps.Get(vehicle).m_pWiperOneR && vehComps.Get(vehicle).m_pWiperOneL) {
                        if (vehicle->m_pDriver && wather->NewWeatherType == 2) 
                            vehComps.Get(vehicle).wiperState = true;
                        if (vehComps.Get(vehicle).wiperState == true) {
                            switch (m_currentWiperState) {
                            case STATE_LEFT:
                                wiperAngle -= 0.05f;
                                MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneR, wiperAngle);
                                MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneL, wiperAngle);
                                if (vehComps.Get(vehicle).m_pWiperOneM) {
                                    MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneM, wiperAngle);
                                }
                                if (wiperAngle < -1.4f) {
                                    wiperAngle = -1.4f;
                                    MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneR, wiperAngle);
                                    MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneL, wiperAngle);
                                    if (vehComps.Get(vehicle).m_pWiperOneM) {
                                        CMatrix matrixWiperOneM(&vehComps.Get(vehicle).m_pWiperOneM->modelling, false);
                                        MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneM, wiperAngle);
                                    }
                                    m_currentWiperState = STATE_RIGHT;
                                }
                                break;
                            case STATE_RIGHT:
                                wiperAngle += 0.05f;
                                MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneR, wiperAngle);
                                MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneL, wiperAngle);
                                if (vehComps.Get(vehicle).m_pWiperOneM) {
                                    MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneM, wiperAngle);
                                }
                                if (wiperAngle > 0.0f) {
                                    wiperAngle = 0.0f;
                                    MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneR, wiperAngle);
                                    MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneL, wiperAngle);
                                    if (vehComps.Get(vehicle).m_pWiperOneM) {
                                        MatrixSetRotateYOnly(vehComps.Get(vehicle).m_pWiperOneM, wiperAngle);
                                    }
                                    if (!vehicle->m_pDriver)
                                        vehComps.Get(vehicle).wiperState = true;
                                    else
                                        vehComps.Get(vehicle).wiperState = false;
                                    m_currentWiperState = STATE_LEFT;
                                }
                                break;
                            }
                        }
                    }
                    //

                }
            }

            CVehicle *vehicle = FindPlayerVehicle();
            if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                //KeyCheck::Update();
                //if (KeyCheck::CheckWithDelay(0xBE, 500)) {
                //    if (automobile->m_aDoors[1].IsClosed()) {
                //        //vehicle->m_nVehicleFlags &= 0xEF;
                //        //RwV3d axis = { 1.0f, 0.0f, 0.0f };
                //        //RwFrameRotate(automobile->m_aCarNodes[CAR_BOOT], &axis, 1.0f, rwCOMBINEPRECONCAT);
                //        //RwMatrixRotate(&automobile->m_aCarNodes[CAR_BOOT]->modelling, &axis, 1.0f, rwCOMBINEPRECONCAT);
                //        CMessages::AddMessageJumpQ(L"IsClosed", 2000, 0);
                //    }
                //    else {
                //        //vehicle->m_nVehicleFlags |= 0x10;
                //        //RwV3d axis = { 1.0f, 0.0f, 0.0f };
                //        //RwFrameRotate(automobile->m_aCarNodes[CAR_BOOT], &axis, 0.0f, rwCOMBINEPRECONCAT);
                //        //RwMatrixRotate(&automobile->m_aCarNodes[CAR_BOOT]->modelling, &axis, 0.0f, rwCOMBINEPRECONCAT);
                //        CMessages::AddMessageJumpQ(L"Open", 2000, 0);
                //    }
                //}

            //CPed *player2 = FindPlayerPed();
            //if (player2) {
                // вывод текста
                CFont::SetScale(0.5f, 1.0f);
                CFont::SetColor(CRGBA(255, 255, 255, 255));
                CFont::SetJustifyOn();
                CFont::SetFontStyle(0);
                CFont::SetPropOn();
                CFont::SetWrapx(600.0f);
                wchar_t text[32];
                //swprintf(text, L"steer angle %.2f", vehicle->m_fSteerAngle);
                int currentWather = wather->NewWeatherType;
                switch (currentWather) {
                case 0:
                    swprintf(text, L"weather %hs", "SUNNY");
                    break;
                case 1:
                    swprintf(text, L"weather %hs", "CLOUDY");
                    break;
                case 2:
                    swprintf(text, L"weather %hs", "RAINY");
                    break;
                case 3:
                    swprintf(text, L"weather %hs", "FOGGY");
                    break;
                }
                //swprintf(text, L"weather %d", wather->NewWeatherType);
                CFont::PrintString(10.0f, 10.0f, text);
                //swprintf(text, L"dvig %d", vehicle->m_nVehicleFlags);
                //CFont::PrintString(10.0f, 30.0f, text);
                //swprintf(text, L"OpenRatio %1f", automobile->m_aDoors[1].GetAngleOpenRatio());
                //CFont::PrintString(10.0f, 50.0f, text);
                //swprintf(text, L"Angle %.2f", automobile->m_aDoors[1].m_fAngle);
                //CFont::PrintString(10.0f, 80.0f, text);
                //swprintf(text, L"PrevAngle %.2f", automobile->m_aDoors[1].m_fPrevAngle);
                //CFont::PrintString(10.0f, 110.0f, text);
            }

        };

    }
} AdditionalComponents;

AdditionalComponents::eWiperState AdditionalComponents::m_currentWiperState = STATE_LEFT;
float AdditionalComponents::wiperAngle = 0.0f;
