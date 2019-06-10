#include "plugin.h"
#include "CSprite2d.h"
#include "CFileLoader.h"
#include "CGameLogic.h"

//int &CurrentMenuScreen = *(int *)0x8F5F20;

using namespace plugin;

class Test {
public:
    static RwTexture *m_textureTest, *m_textureLogo;
    static RwTexDictionary *m_txd;
    static float m_startLeft, m_startRight, m_currentLeft, m_currentRight;

    static float ScreenCoord(float a) {
        return (a * (static_cast<float>(RsGlobal.maximumHeight) / 900.0f));
    }

    Test() {
        Events::menuDrawingEvent += [] {
            if (CGameLogic::ActivePlayers) {
                /*gamefont::Print({
                    Format("currentLeft %.1f", m_currentLeft),
                    Format("currentRight %.1f", m_currentRight),
                    Format("maximumWidth %d", RsGlobal.maximumWidth),
                    Format("ActivePlayers %d", CGameLogic::ActivePlayers),
                    Format("CurrentMenuScreen %d", CurrentMenuScreen)
                }, 10, 250, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);*/

                if (!m_txd) {
                    m_txd = CFileLoader::LoadTexDictionary(GAME_PATH("txd\\testlogo.txd"));
                    m_textureLogo = RwTexDictionaryFindNamedTexture(m_txd, "logokamaz");
                    m_textureTest = RwTexDictionaryFindNamedTexture(m_txd, "test");
                }
                if (m_textureTest) {
                    // Setup texture
                    RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureTest->raster);
                    CSprite2d::SetVertices(CRect(ScreenCoord(160.0f), RsGlobal.maximumHeight - ScreenCoord(110.0f), ScreenCoord(235.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
                    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
                    // Reset texture
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
                }
                if (m_textureLogo) {
                    if (ScreenCoord(m_currentLeft) < RsGlobal.maximumWidth) {
                        m_currentLeft += 2.0f; m_currentRight += 2.0f;
                    }
                    else {
                        m_currentLeft = m_startLeft; m_currentRight = m_startRight;
                    }
                    // Setup texture
                    RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureLogo->raster);
                    CSprite2d::SetVertices(CRect(ScreenCoord(m_currentLeft), RsGlobal.maximumHeight - ScreenCoord(190.0f), ScreenCoord(m_currentRight), RsGlobal.maximumHeight - ScreenCoord(120.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
                    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
                    // Reset texture
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
                }
            }
        };
        
    }
} test;

RwTexDictionary  *Test::m_txd = nullptr;
RwTexture *Test::m_textureTest = nullptr;
RwTexture *Test::m_textureLogo = nullptr;
float Test::m_startLeft = -225.0f;
float Test::m_startRight = 55.0f;
float Test::m_currentLeft = -225.0f;
float Test::m_currentRight = 55.0f;
