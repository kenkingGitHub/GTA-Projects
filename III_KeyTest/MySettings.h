#pragma once

class MySettings {
public:
    MySettings();

    int keyOpenClose, keyOpen, keyClose, keyOnOff;
};

extern MySettings settings;