/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include "plugin_III.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include "game_III\eVehicleModel.h"

using namespace plugin;

class MyTaxiModel {
public:
    static unsigned int CurrentTaxiModel;

    static std::unordered_set<unsigned int> &GetTaxiModels() {
        static std::unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static bool __stdcall CheckIfTaxiModel(unsigned int id) {
        return id == MODEL_TAXI || id == MODEL_CABBIE || id == MODEL_BORGNINE || GetTaxiModels().find(id) != GetTaxiModels().end();
    }

    static int __stdcall GetTranslatedTaxiModel(unsigned int model) {
        if (CheckIfTaxiModel(model))
            return MODEL_TAXI;
        return model;
    }

    static void Patch_5373D7();

    MyTaxiModel() {
        std::ifstream stream(PLUGIN_PATH("taxi.dat"));
        if (!stream.is_open())
            return;
        for (std::string line; getline(stream, line); ) {
            if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                GetTaxiModels().insert(std::stoi(line));
        }
        patch::SetUChar(0x446AC5, 0x53); // push ebx
        patch::RedirectCall(0x446AC6, CheckIfTaxiModel); // call
        patch::RedirectJump(0x446ACB, reinterpret_cast<void *>(0x446ADC)); // jump
        patch::RedirectJump(0x5373D7, Patch_5373D7);
    }
} myTaxiModel;

unsigned int MyTaxiModel::CurrentTaxiModel;

void __declspec(naked) MyTaxiModel::Patch_5373D7() {
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetTranslatedTaxiModel // MODEL_TAXI
        mov CurrentTaxiModel, eax
        popad
        mov eax, CurrentTaxiModel
        lea edx, [eax - 0x61]
        mov edi, 0x5373DE
        jmp edi
    }
}
