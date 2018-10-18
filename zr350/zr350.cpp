#include "plugin.h"
#include <vector>
#include <string>
#include <fstream>
#include "CModelInfo.h"
#include "CAutomobile.h"
#include "common.h"
#include "CFont.h"

using namespace plugin;
using namespace std;



class ZR350 {
public:
    class ModelInfo {
    public:
        bool LightsUpDown;
        float LightsAngle;
        ModelInfo(CVehicle *vehicle) {
            LightsUpDown = false;
            LightsAngle = 0.0f;
        }
    };

    static VehicleExtendedData<ModelInfo> modelInfo;
    
    struct Zr350Info {
        unsigned int baseModelId;
        float value;
    };

    static vector<Zr350Info>& GetZr350Infos() {
        static vector<Zr350Info> zr350Infos;
        return zr350Infos;
    }

    static Zr350Info *GetZr350InfoForModel(unsigned int BaseModelId) {
        for (unsigned int i = 0; i < GetZr350Infos().size(); i++) {
            if (GetZr350Infos()[i].baseModelId == BaseModelId)
                return &GetZr350Infos()[i];
        }
        return nullptr;
    }

    static void ReadSettingsFile() {
        ifstream stream("zr350.dat");
        for (string line; getline(stream, line); ) {
            if (line[0] != ';' && line[0] != '#') {
                if (!line.compare("zr350")) {
                    while (getline(stream, line) && line.compare("end")) {
                        if (line[0] != ';' && line[0] != '#') {
                            Zr350Info entry;
                            if (sscanf(line.c_str(), "%d, %f", &entry.baseModelId, &entry.value) == 2)
                                GetZr350Infos().push_back(entry);
                        }
                    }
                }
            }
        }
    }

    ZR350() {
       ReadSettingsFile();

       Events::vehicleSetModelEvent += [](CVehicle *vehicle, int modelIndex) {
            if (vehicle) {
                Zr350Info *entryModel = GetZr350InfoForModel(vehicle->m_nModelIndex);
                if (entryModel) {
                    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                    automobile->m_damageManager.SetLightStatus(LIGHT_FRONT_LEFT, 1);
                    automobile->m_damageManager.SetLightStatus(LIGHT_FRONT_RIGHT, 1);
                }
            }
        };

        Events::gameProcessEvent += [] {
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *trafVehicle = CPools::ms_pVehiclePool->GetAt(i);
                if (trafVehicle) {
                    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(trafVehicle);
                    if (trafVehicle->m_nVehicleSubClass == VEHICLE_AUTOMOBILE && trafVehicle->m_pDriver && automobile->m_aCarNodes[CAR_MISC_A]) {
                        Zr350Info *entryModel = GetZr350InfoForModel(trafVehicle->m_nModelIndex);
                        if (entryModel) {
                            ModelInfo &info = modelInfo.Get(trafVehicle);
                            if ((trafVehicle->GetVehicleLightsStatus() && trafVehicle->m_nOverrideLights != 1) || trafVehicle->m_nOverrideLights == 2) {
                                if (info.LightsUpDown == false && trafVehicle->m_nFlags.bEngineOn) {
                                    info.LightsAngle += 0.05f;
                                    automobile->SetComponentRotation(automobile->m_aCarNodes[CAR_MISC_A], 0, info.LightsAngle, true);
                                    if (info.LightsAngle > entryModel->value) {
                                        info.LightsAngle = entryModel->value;
                                        automobile->SetComponentRotation(automobile->m_aCarNodes[CAR_MISC_A], 0, info.LightsAngle, true);
                                        automobile->m_damageManager.SetLightStatus(LIGHT_FRONT_LEFT, 0);
                                        automobile->m_damageManager.SetLightStatus(LIGHT_FRONT_RIGHT, 0);
                                        info.LightsUpDown = true;
                                    }
                                }
                            }
                            else if (info.LightsUpDown || trafVehicle->m_nOverrideLights == 1) {
                                automobile->m_damageManager.SetLightStatus(LIGHT_FRONT_LEFT, 1);
                                automobile->m_damageManager.SetLightStatus(LIGHT_FRONT_RIGHT, 1);
                                info.LightsAngle -= 0.05f;
                                automobile->SetComponentRotation(automobile->m_aCarNodes[CAR_MISC_A], 0, info.LightsAngle, true);
                                if (info.LightsAngle < 0.0f) {
                                    info.LightsAngle = 0.0f;
                                    automobile->SetComponentRotation(automobile->m_aCarNodes[CAR_MISC_A], 0, info.LightsAngle, true);
                                    info.LightsUpDown = false;
                                }
                            }
                        }
                    }
                }
            }
        };

    }
} example;

VehicleExtendedData<ZR350::ModelInfo> ZR350::modelInfo;

////------------------------------------------------------------------------------------------------------------
//#include "plugin.h"
//#include <vector>
//#include <fstream>
//#include <string>
//#include <sstream>
//#include "game_sa\common.h"
//#include "game_sa\CFont.h"
//
//using namespace plugin;
//using namespace std;
//
//class ZR350 {
//public:
//    struct Zr350Info {
//        unsigned int baseModelId;
//        float value;
//    };
//
//    static vector<Zr350Info>& GetZr350Infos() {
//        static vector<Zr350Info> zr350Infos;
//        return zr350Infos;
//    }
//
//    static vector<unsigned int>& GetTruckIDs() {
//        static vector<unsigned int> truckIDs;
//        return truckIDs;
//    }
//
//    static void ReadSettingsFile() {
//        ifstream stream("vehicle_advanced.dat");
//        for (string line; getline(stream, line); ) {
//            if (line[0] != ';' && line[0] != '#') {
//                if (!line.compare("zr350")) {
//                    while (getline(stream, line) && line.compare("end")) {
//                        if (line[0] != ';' && line[0] != '#') {
//                            Zr350Info entry;
//                            if (sscanf(line.c_str(), "%d, %f", &entry.baseModelId, &entry.value) == 2)
//                                GetZr350Infos().push_back(entry);
//                        }
//                    }
//                }
//                else if (!line.compare("trucks")) {
//                    while (getline(stream, line) && line.compare("end")) {
//                        if (line[0] != ';' && line[0] != '#') {
//                            std::stringstream ss(line);
//                            int i;
//                            while (ss >> i) {
//                                GetTruckIDs().push_back(i);
//                                if (ss.peek() == ',')
//                                    ss.ignore();
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    static Zr350Info *GetZr350InfoForModel(unsigned int BaseModelId) {
//        for (unsigned int i = 0; i < GetZr350Infos().size(); i++) {
//            if (GetZr350Infos()[i].baseModelId == BaseModelId)
//                return &GetZr350Infos()[i];
//        }
//        return nullptr;
//    }
//
//    static bool GetIsModelTruck(unsigned int modelId) {
//        for (unsigned int i : GetTruckIDs()) {
//            if (i == modelId)
//                return true;
//        }
//        return false;
//    }
//
//    static void Test() {
//        CVehicle *vehicle = FindPlayerVehicle(-1, false);
//        if (vehicle) {
//            CFont::SetScale(0.5f, 1.0f);
//            CFont::SetColor(CRGBA(255, 255, 255, 255));
//            CFont::SetAlignment(ALIGN_LEFT);
//            CFont::SetOutlinePosition(1);
//            CFont::SetDropColor(CRGBA(0, 0, 0, 255));
//            CFont::SetBackground(false, false);
//            CFont::SetFontStyle(FONT_SUBTITLES);
//            CFont::SetProp(true);
//            CFont::SetWrapx(600.0f);
//            static char str[32];
//            Zr350Info *entryModel = GetZr350InfoForModel(vehicle->m_nModelIndex);
//            if (entryModel) {
//                sprintf(str, "yes %d %.2f", entryModel->baseModelId, entryModel->value);
//                CFont::PrintString(5.0f, 445.0f, str);
//            }
//            else {
//                sprintf(str, "no %d", vehicle->m_nModelIndex);
//                CFont::PrintString(5.0f, 445.0f, str);
//            }
//            if (GetIsModelTruck(vehicle->m_nModelIndex)) {
//                sprintf(str, "yes truck %d", vehicle->m_nModelIndex);
//                CFont::PrintString(205.0f, 445.0f, str);
//            }
//            else {
//                sprintf(str, "no truck %d", vehicle->m_nModelIndex);
//                CFont::PrintString(205.0f, 445.0f, str);
//            }
//        }
//    }
//
//    ZR350() {
//        ReadSettingsFile();
//        Events::drawingEvent += Test;
//    }
//} test;
