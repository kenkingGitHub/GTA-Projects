/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include <string>
#include "plugin.h"
#include "extensions\KeyCheck.h"
#include "game_sa\common.h"
#include "game_sa\CModelInfo.h"
#include "game_sa\CCheat.h"
#include "game_sa\CTimer.h"
#include "game_sa\CFont.h"
#include "game_sa\CSprite2d.h"

using namespace plugin;

class MoreVehiclesSpawner {
public:
    static std::string typedBuffer;
    static std::string errorMessage;
    static std::string errorMessageBuffer;
    static unsigned int errorMessageTimer;
    static bool enabled;

    static void ReportAudioEvent(int audioEventId, float volume, float speed) { // с классами, связанными со звуком, в sdk пока что не очень
        CallMethod<0x506EA0>(0xB6BC90, audioEventId, volume, speed);
    }

    static void Update() {
        KeyCheck::Update(); // апдейтим состояния клавиш
        if (FindPlayerPed() && FindPlayerPed()->IsAlive()) {
            if (KeyCheck::CheckJustDown(VK_TAB)) { // Если нажата Tab - включаем или выключаем консоль
                enabled = !enabled;
                typedBuffer.clear();
                errorMessageBuffer.clear();
            }
            if (enabled) {
                errorMessage.clear();
                if (KeyCheck::CheckWithDelay(VK_BACK, 200)) { // Если нажат Backspace - убираем последний символ в строке
                    if (typedBuffer.size() > 0) {
                        typedBuffer.pop_back();
                        ReportAudioEvent(3, 0.0f, 1.0f);
                    }
                }
                else {
                    for (int i = 0; i <= 9; i++) {
                        if (KeyCheck::CheckWithDelay(i + 48, 200)) {
                            if (typedBuffer.size() == 5)
                                errorMessage = "Too many digits!";
                            else {
                                typedBuffer.push_back(i + 48); // Добавляем символ в конец строки
                                ReportAudioEvent(3, 0.0f, 1.0f);
                            }
                            break;
                        }
                    }
                }
                if (KeyCheck::CheckJustDown(VK_RETURN)) { // Если нажата Return - спавним транспорт
                    if (typedBuffer.size() > 0) {
                        unsigned int modelId = std::stoi(typedBuffer);
                        if (modelId < 19010) {
                            int modelType = CModelInfo::IsVehicleModelType(modelId);
                            if (modelType != -1) {
                                if (modelType != 6) {
                                    CCheat::VehicleCheat(modelId);
                                    ReportAudioEvent(12, 0.0f, 1.0f);
                                    errorMessageBuffer.clear(); // убираем надпись об ошибке (если она была на экране)
                                }
                                else
                                    errorMessage = "Can't spawn a train model";
                            }
                            else
                                errorMessage = "This model is not a vehicle!";
                        }
                        else
                            errorMessage = "ID is too big!";
                    }
                    else
                        errorMessage = "Please enter model Id!";
                }
            }
        }
        else
            enabled = false;
    }

    static void Render() {
        if (enabled) {
            CSprite2d::DrawRect(CRect(100.0f, 100.0f, 470.0f, 200.0f), CRGBA(0, 0, 0, 100));
            CSprite2d::DrawRect(CRect(250.0f, 140.0f, 340.0f, 142.0f), CRGBA(255, 255, 255, 255));
            CFont::SetScale(0.8f, 1.9f);
            CFont::SetColor(CRGBA(255, 255, 255, 255));
            CFont::SetAlignment(ALIGN_LEFT);
            CFont::SetOutlinePosition(0);
            CFont::SetBackground(false, false);
            CFont::SetFontStyle(FONT_SUBTITLES);
            CFont::SetProp(true);
            CFont::SetWrapx(600.0f);
            CFont::PrintString(105.0f, 105.0f, "Model ID:");
            if (typedBuffer.size() > 0)
                CFont::PrintString(250.0f, 105.0f, const_cast<char*>(typedBuffer.c_str()));
            if (errorMessage.size() > 0) {
                errorMessageBuffer = errorMessage;
                errorMessageTimer = CTimer::m_snTimeInMilliseconds;
                ReportAudioEvent(4, 0.0f, 1.0f);
            }
            if (errorMessageBuffer.size() > 0 && CTimer::m_snTimeInMilliseconds < (errorMessageTimer + 2000)) {
                CFont::SetColor(CRGBA(255, 0, 0, 255));
                CFont::PrintString(105.0f, 150.0f, const_cast<char*>(errorMessageBuffer.c_str()));
            }
        }
    }

    MoreVehiclesSpawner() {
        Events::gameProcessEvent += Update;
        Events::drawingEvent += Render;
    };
} moreVehiclesSpawner;

std::string MoreVehiclesSpawner::typedBuffer;
std::string MoreVehiclesSpawner::errorMessage;
std::string MoreVehiclesSpawner::errorMessageBuffer;
unsigned int MoreVehiclesSpawner::errorMessageTimer = 0;
bool MoreVehiclesSpawner::enabled = false;
