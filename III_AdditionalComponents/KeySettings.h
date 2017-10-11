#pragma once

class KeySettings {
public:
    KeySettings();

    int keyOpenClose, keyOpen, keyClose, keyOnOff;
};

extern KeySettings settings;