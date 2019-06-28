#include "plugin.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include "eVehicleModel.h"
#include "CTheScripts.h"
#include "CWorld.h"

using namespace plugin;

class MyTaxiModel {
public:
    static unsigned int CurrentTaxiModel;

    static std::unordered_set<unsigned int> &GetTaxiModels() {
        static std::unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static int __stdcall GetTaxiModel(unsigned int model) {
        if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        return model;
    }

    static void Patch_58BE1F();

    static void __fastcall OpcodePlayerDrivingTaxiVehicle(CRunningScript *script) {
        script->CollectParameters(&script->m_nIp, 1);
        bool isTaxiModel = false;

        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
        if (player->m_bInVehicle) {
            unsigned int model = player->m_pVehicle->m_nModelIndex;
            if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_ZEBRA || model == MODEL_KAUFMAN || GetTaxiModels().find(model) != GetTaxiModels().end())
                isTaxiModel = true;
        }
        script->UpdateCompareFlag(isTaxiModel);
    }

    MyTaxiModel() {
        std::ifstream stream(PLUGIN_PATH("taxi.dat"));
        if (!stream.is_open())
            return;
        for (std::string line; getline(stream, line); ) {
            if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                GetTaxiModels().insert(std::stoi(line));
        }

        patch::RedirectCall(0x45600E, OpcodePlayerDrivingTaxiVehicle);
        patch::Nop(0x456013, 0x5B); // или сделать jump на 0x45606E
        patch::RedirectJump(0x58BE1F, Patch_58BE1F);
    }
} myTaxiModel;

unsigned int MyTaxiModel::CurrentTaxiModel;

void __declspec(naked) MyTaxiModel::Patch_58BE1F() {
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetTaxiModel
        mov CurrentTaxiModel, eax
        popad
        mov eax, CurrentTaxiModel
        mov ecx, eax
        mov edx, 0x58BE25
        jmp edx
    }
}