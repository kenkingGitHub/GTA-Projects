#pragma once
#include "plugin.h"
#include "KeySettings.h"

KeySettings settings;

KeySettings::KeySettings() {
    plugin::config_file conf(PLUGIN_PATH("SpecialCars.dat"));

    keyBlink     = conf["KEY_BLINK"].asInt(79);
}