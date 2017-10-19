#include "plugin_vc.h"
#include "extensions\KeyCheck.h"
#include "game_vc\CModelInfo.h"
#include "game_vc\CFont.h"

using namespace plugin;

class MyPlugin {
public:
    MyPlugin() {

        Events::drawingEvent += [] {
            CFont::SetScale(0.5f, 1.0f);
            CFont::SetColor(CRGBA(255, 255, 255, 255));
            CFont::SetJustifyOn();
            CFont::SetFontStyle(2);
            CFont::SetPropOn();
            CFont::SetWrapx(600.0f);
            wchar_t text[32];
            swprintf(text, L"TxdIndex %d", CModelInfo::ms_modelInfoPtrs[162]->m_nTxdIndex);
            CFont::PrintString(10.0f, 30.0f, text);
        };
        
        Events::gameProcessEvent += [] {
            KeyCheck::Update();
            if (KeyCheck::CheckWithDelay(VK_TAB, 1000)) {
                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino2");
            }
            if (KeyCheck::CheckWithDelay(110, 1000)) {
                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino");
            }
        };
    }
} myPlugin;