#pragma once
#include "plugin.h"
#include "KeySettings.h"

KeySettings settings;

KeySettings::KeySettings() {
    plugin::config_file conf(PLUGIN_PATH("AdditionalComponents.dat"));

    keyOpenClose = conf["KEY_OPEN_CLOSE"].asInt(110);
    keyOpen = conf["KEY_OPEN"].asInt(104);
    keyClose = conf["KEY_CLOSE"].asInt(98);
    keyOnOff = conf["KEY_ON_OFF"].asInt(46);
    keyTurnL = conf["KEY_TURN_L"].asInt(90);
    keyTurnR = conf["KEY_TURN_R"].asInt(67);
    keyTurnLR = conf["KEY_TURN_LR"].asInt(88);
    keyTurnOff = conf["KEY_TURN_OFF"].asInt(16);
    keyBootClose = conf["KEY_BOOT_CLOSE"].asInt(80);
    keyBlink = conf["KEY_BLINK"].asInt(79);
}