#include "plugin.h"

int __cdecl nCountCompsInRule(signed int compRulesBits) {
    return plugin::CallAndReturn<int, 0x4C7F80, signed int>(compRulesBits);
}

int __cdecl nGetRandomNumberInRange(int min, int max) {
    double random = (double)(unsigned int)rand() * 0.000030517578;
    return min + (int)((double)(max - min) * random);
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
        patch::RedirectCall(0x4C80A6, Choose);
        patch::RedirectCall(0x4C8187, Choose);
    };
} NewChooseComponents;
