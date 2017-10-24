#include "plugin.h"
#include "game_vc\common.h"
#include <map>

using namespace plugin;

class VehicleTextures {
public:
    VehicleTextures() {
        static CdeclEvent<AddressList<0x4C9ED1, H_CALL, 0x4C9F23, H_CALL, 0x4CA10D, H_CALL, 0x5813D1, H_CALL>,
            PRIORITY_BEFORE, ArgPickN<CEntity*, 0>, void(CEntity*)> myOnRenderOneNonRoad;

        static std::map<RpMaterial*, RwTexture *> originalTextures;

        myOnRenderOneNonRoad.before += [](CEntity *entity) {
            if (KeyPressed('Z') && entity == FindPlayerVehicle()) {
                CVehicle *vehicle = reinterpret_cast<CVehicle *>(entity);
                RpClumpForAllAtomics(vehicle->m_pRwClump, [](RpAtomic *atomic, void *data) {
                    RpGeometryForAllMaterials(atomic->geometry, [](RpMaterial *material, void *data) {
                        if (originalTextures.find(material) == originalTextures.end())
                            originalTextures[material] = material->texture;
                        material->texture = *reinterpret_cast<RwTexture **>(0x77FA58);
                        return material;
                    }, nullptr);
                    return atomic;
                }, nullptr);
            }
        };

        myOnRenderOneNonRoad.after += [](CEntity *) {
            if (originalTextures.size() > 0) {
                for (auto &i : originalTextures)
                    i.first->texture = i.second;
                originalTextures.clear();
            }
        };
    }
} vehicleTextures;



//#include "plugin_vc.h"
//#include "extensions\KeyCheck.h"
//#include "game_vc\CModelInfo.h"
//#include "game_vc\CFont.h"
//
//using namespace plugin;
//
//class MyPlugin {
//public:
//    MyPlugin() {
//
//        Events::drawingEvent += [] {
//            CFont::SetScale(0.5f, 1.0f);
//            CFont::SetColor(CRGBA(255, 255, 255, 255));
//            CFont::SetJustifyOn();
//            CFont::SetFontStyle(2);
//            CFont::SetPropOn();
//            CFont::SetWrapx(600.0f);
//            wchar_t text[32];
//            swprintf(text, L"TxdIndex %d", CModelInfo::ms_modelInfoPtrs[162]->m_nTxdIndex);
//            CFont::PrintString(10.0f, 30.0f, text);
//        };
//        
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay(VK_TAB, 1000)) {
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino2");
//            }
//            if (KeyCheck::CheckWithDelay(110, 1000)) {
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino");
//            }
//        };
//    }
//} myPlugin;