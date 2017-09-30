/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include "plugin_vc.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include "game_vc\eVehicleModel.h"

using namespace plugin;

class MyTaxiModel {
public:
    static unsigned int CurrentTaxiModel;

    static std::unordered_set<unsigned int> &GetTaxiModels() {
        static std::unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static bool __stdcall CheckIfTaxiModel(unsigned int id) {
        return id == MODEL_TAXI || id == MODEL_CABBIE || id == MODEL_ZEBRA || id == MODEL_KAUFMAN || GetTaxiModels().find(id) != GetTaxiModels().end();
    }

    static int __stdcall GetTranslatedTaxiModel(unsigned int model) {
        if (CheckIfTaxiModel(model))
            return MODEL_TAXI;
        return model;
    }

    static void Patch_58BE1F();

    MyTaxiModel() {
        std::ifstream stream(PLUGIN_PATH("taxi.dat"));
        if (!stream.is_open())
            return;
        for (std::string line; getline(stream, line); ) {
            if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                GetTaxiModels().insert(std::stoi(line));
        }
        patch::SetUChar(0x456044, 0x51); // push edi
        patch::RedirectCall(0x456045, CheckIfTaxiModel); // call
        patch::RedirectJump(0x45604D, reinterpret_cast<void *>(0x456066)); // jump
        patch::RedirectJump(0x58BE1F, Patch_58BE1F);
    }
} myTaxiModel;

unsigned int MyTaxiModel::CurrentTaxiModel;

void __declspec(naked) MyTaxiModel::Patch_58BE1F() {
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetTranslatedTaxiModel // MODEL_TAXI
        mov CurrentTaxiModel, eax
        popad
        mov eax, CurrentTaxiModel
        mov edi, eax //mov ecx, eax
        mov edx, 0x58BE25
        jmp edx
    }
}
