#include "plugin_III.h"
#include "MySettings.h"
#include "game_III\CMessages.h"

using namespace plugin;

class Test {
public:
    Test() {
        Events::gameProcessEvent += [] { 
            if (KeyPressed(settings.keyOpenClose))
                CMessages::AddMessageJumpQ(L"KEY_OPEN_CLOSE pressed", 2000, 0);
        };
    }
} test;