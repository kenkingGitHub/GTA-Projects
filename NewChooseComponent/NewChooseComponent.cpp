#include "plugin.h"

int __cdecl nCountCompsInRule(signed int compRulesBits) {
    return plugin::CallAndReturn<int, 0x520990, signed int>(compRulesBits);
}

int __cdecl nGetRandomNumberInRange(int min, int max) {
    return plugin::CallAndReturn<int, 0x54A4C0, int, int>(min, max);
}

using namespace plugin;

int randomId, numberId, result;

class NewChooseComponent {
public:
    static int Choose(int type, signed int compRulesBits) {
        switch (type) {
        case 1:
        case 2:
            numberId = nCountCompsInRule(compRulesBits);
            result = (compRulesBits >> 4 * nGetRandomNumberInRange(0, numberId)) & 0xF;
            break;
        case 3:
            numberId = nCountCompsInRule(compRulesBits);
            randomId = nGetRandomNumberInRange(-1, numberId);
            if (randomId == -1) {
                result = -1;
                break;
            }
            result = (compRulesBits >> 4 * randomId) & 0xF;
            break;
        case 4:
            result = nGetRandomNumberInRange(0, 4);
            break;
        case 5:
            result = nGetRandomNumberInRange(0, 5);
            break;
        case 6:
            result = nGetRandomNumberInRange(0, 6);
            break;
        default:
            result = -1;
            break;
        }
        return result;
    }

    NewChooseComponent() {
        patch::RedirectJump(0x5209C0, Choose);
    };
} NewChooseComponents;
