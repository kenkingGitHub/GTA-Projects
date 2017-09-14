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
        RwFrame *m_pSteerWheel;
        RwFrame *m_pWiperOneR;
        RwFrame *m_pWiperOneL;
        bool wiperState;

        VehicleComponents(CVehicle *) { // Конструктор этого класса будет вызван при вызове конструктора транспорта (CVehicle::CVehicle)
            m_pBootSliding = m_pSteerWheel = m_pWiperOneR = m_pWiperOneL = nullptr;  // устанавливаем все указатели в 0
            wiperState = false;

        }
    };

    AdditionalComponents() {
        static VehicleExtendedData<VehicleComponents> vehComps; // Создаем экземпляр нашего расширения. vehComps - это переменная, через которую мы будем
                                                                // обращаться к нашим данным (используя метод Get(CVehicle *транспорт) )

        Events::vehicleSetModelEvent += [](CVehicle *vehicle, int modelIndex) { // Выполняем нашу функцию, когда игра устанавливает модель транспорту
            if (vehicle->m_pRwClump) { // Находим компоненты в иерархии и записываем их в наш класс
                vehComps.Get(vehicle).m_pBootSliding = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "boot_sliding"); 
                vehComps.Get(vehicle).m_pSteerWheel = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "steerwheel"); 
                vehComps.Get(vehicle).m_pWiperOneR = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_or");
                vehComps.Get(vehicle).m_pWiperOneL = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_ol");

                
            }
            else {
                vehComps.Get(vehicle).m_pBootSliding = vehComps.Get(vehicle).m_pSteerWheel = vehComps.Get(vehicle).m_pWiperOneR = vehComps.Get(vehicle).m_pWiperOneL = nullptr;

            }
        };

        Events::drawingEvent += [] {
        //Events::gameProcessEvent += [] {
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                if (vehicle && vehicle->GetIsOnScreen() && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
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
                    // руль
                    if (vehComps.Get(vehicle).m_pSteerWheel) {
                        CMatrix matrixSteerWheel(&vehComps.Get(vehicle).m_pSteerWheel->modelling, false);
                        matrixSteerWheel.SetRotateYOnly(vehicle->m_fSteerAngle * (-7.0f));
                        matrixSteerWheel.UpdateRW();
                    }
                    // дворники тип 1
                    if (vehComps.Get(vehicle).m_pWiperOneR && vehComps.Get(vehicle).m_pWiperOneL) {
                        if (vehicle->m_pDriver && wather->NewWeatherType == 2)
                            vehComps.Get(vehicle).wiperState = true;
                        if (vehComps.Get(vehicle).wiperState == true) {
                            CMatrix matrixWiperOneR(&vehComps.Get(vehicle).m_pWiperOneR->modelling, false);
                            CMatrix matrixWiperOneL(&vehComps.Get(vehicle).m_pWiperOneL->modelling, false);
                            switch (m_currentWiperState) {
                            case STATE_LEFT:
                                matrixWiperOneR.SetRotateYOnly(wiperAngle);
                                matrixWiperOneR.UpdateRW();
                                matrixWiperOneL.SetRotateYOnly(wiperAngle);
                                matrixWiperOneL.UpdateRW();
                                wiperAngle -= 0.05f;
                                if (wiperAngle < -1.4f) {
                                    wiperAngle = -1.4f;
                                    matrixWiperOneR.SetRotateYOnly(wiperAngle);
                                    matrixWiperOneR.UpdateRW();
                                    matrixWiperOneL.SetRotateYOnly(wiperAngle);
                                    matrixWiperOneL.UpdateRW();
                                    m_currentWiperState = STATE_RIGHT;
                                }
                                break;
                            case STATE_RIGHT:
                                matrixWiperOneR.SetRotateYOnly(wiperAngle);
                                matrixWiperOneR.UpdateRW();
                                matrixWiperOneL.SetRotateYOnly(wiperAngle);
                                matrixWiperOneL.UpdateRW();
                                wiperAngle += 0.05f;
                                if (wiperAngle > 0.0f) {
                                    wiperAngle = 0.0f;
                                    matrixWiperOneR.SetRotateYOnly(wiperAngle);
                                    matrixWiperOneR.UpdateRW();
                                    matrixWiperOneL.SetRotateYOnly(wiperAngle);
                                    matrixWiperOneL.UpdateRW();
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


            //CVehicle *vehicle = FindPlayerVehicle();
            //if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                //CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
            CPed *playa = FindPlayerPed();
            if (playa) {
                // вывод текста
                CFont::SetScale(0.5f, 1.0f);
                CFont::SetColor(CRGBA(255, 255, 255, 255));
                CFont::SetJustifyOn();
                CFont::SetFontStyle(0);
                CFont::SetPropOn();
                CFont::SetWrapx(600.0f);
                wchar_t text[32];


                //swprintf(text, L"steer angle %.2f", vehicle->m_fSteerAngle);
                swprintf(text, L"weather %d", wather->NewWeatherType);
                CFont::PrintString(10.0f, 10.0f, text);

                /*swprintf(text, L"boot %d", automobile->m_carDamage.GetDoorStatus(BOOT));
                CFont::PrintString(10.0f, 10.0f, text);
                swprintf(text, L"wing %d", automobile->m_carDamage.GetPanelStatus(WING_REAR_LEFT));
                CFont::PrintString(10.0f, 30.0f, text);
                swprintf(text, L"angle %.2f", automobile->m_aDoors[1].m_fAngle);
                CFont::PrintString(10.0f, 50.0f, text);*/
                

            }

        };

    }
} AdditionalComponents;

AdditionalComponents::eWiperState AdditionalComponents::m_currentWiperState = STATE_LEFT;
float AdditionalComponents::wiperAngle = 0.0f;
