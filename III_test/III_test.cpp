#include "plugin_III.h"
#include "game_III\common.h"
#include "extensions\KeyCheck.h"
//#include "game_III\CMessages.h"
#include "game_III\CFont.h"

//#include "game_III\CModelInfo.h"

using namespace plugin;

class MyPlugin {
public:
    MyPlugin() {
        //CModelInfo::AddVehicleModel(4000);
        // 0x546B99;

        Events::drawingEvent += [] {
            CVehicle *playerVehicle = FindPlayerVehicle();
            if (playerVehicle && playerVehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                CAutomobile *playerAutomobile = reinterpret_cast<CAutomobile *>(playerVehicle);
                //CPed *player = FindPlayerPed();
                //if (player) 
                // вывод текста
                CFont::SetScale(0.5f, 1.0f);
                CFont::SetColor(CRGBA(255, 255, 255, 255));
                CFont::SetJustifyOn();
                CFont::SetFontStyle(0);
                CFont::SetPropOn();
                CFont::SetWrapx(600.0f);
                wchar_t text[32];
                /*int currentWather = wather->NewWeatherType;
                switch (currentWather) {
                case 0: swprintf(text, L"weather %hs", "SUNNY");  break;
                case 1: swprintf(text, L"weather %hs", "CLOUDY"); break;
                case 2: swprintf(text, L"weather %hs", "RAINY");  break;
                case 3: swprintf(text, L"weather %hs", "FOGGY");  break;
                }*/
                //CFont::PrintString(10.0f, 10.0f, text);
                swprintf(text, L"VehicleFlags %d", playerVehicle->m_nVehicleFlags);
                CFont::PrintString(10.0f, 30.0f, text);
                //swprintf(text, L"SteerRatio %.2f", playerVehicle->m_fSteerRatio);
                //swprintf(text, L"steer angle %.2f", playerVehicle->m_fSteerAngle);
                /*swprintf(text, L"dumperAngle %.2f", vehComps.Get(playerVehicle).dumperAngle);
                CFont::PrintString(10.0f, 50.0f, text);
                swprintf(text, L"manholePos %.2f", vehComps.Get(playerVehicle).manholePos);
                CFont::PrintString(10.0f, 80.0f, text);*/
                KeyCheck::Update();
                if (KeyCheck::CheckWithDelay(0x6E, 1000))
                    playerVehicle->BurstTyre(0);

                /*swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[0]);
                CFont::PrintString(10.0f, 110.0f, text);
                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[1]);
                CFont::PrintString(10.0f, 140.0f, text);
                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[2]);
                CFont::PrintString(10.0f, 170.0f, text);
                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[3]);
                CFont::PrintString(10.0f, 200.0f, text);*/

                /*swprintf(text, L"LightStatus %d", playerAutomobile->m_carDamage.GetLightStatus(LIGHT_FRONT_LEFT));
                CFont::PrintString(10.0f, 140.0f, text);
                swprintf(text, L"LightStatus %d", playerAutomobile->m_carDamage.GetLightStatus(LIGHT_FRONT_RIGHT));
                CFont::PrintString(10.0f, 170.0f, text);
                swprintf(text, L"LightStatus %d", playerAutomobile->m_carDamage.GetLightStatus(LIGHT_REAR_RIGHT));
                CFont::PrintString(10.0f, 200.0f, text);
                swprintf(text, L"LightStatus %d", playerAutomobile->m_carDamage.GetLightStatus(LIGHT_REAR_LEFT));
                CFont::PrintString(10.0f, 230.0f, text);*/
            }
        };

    }
} myPlugin;