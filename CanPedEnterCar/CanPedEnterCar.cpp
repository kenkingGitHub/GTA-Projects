#include "plugin.h"
#include "CWanted.h"
#include "CStreaming.h"
#include "CWorld.h"
//#include "CVehicleModelInfo.h"
#include "eVehicleModel.h"
#include "ePedModel.h"
//#include "CModelInfo.h"
//#include "extensions\KeyCheck.h"
//#include "CWeaponInfo.h"
//#include "ePedType.h"
//#include "CAnimManager.h"
//#include "CZoneInfo.h"
//#include "CGangInfo.h"
//#include "CPopulation.h"
//#include "CGangs.h"
#include "cMusicManager.h"
#include "cAudioManager.h"
//#include "CRunningScript.h"
#include "CTheScripts.h"

#define m_MODEL_POLICE 156

using namespace plugin;

class Test {
public:
    static unsigned int CurrentSpecialModel;
    static unsigned int CurrentPoliceModel;

    static int __stdcall GetSpecialModel(unsigned int model) {
        if (model == MODEL_POLICE || model == m_MODEL_POLICE)
            return MODEL_POLICE;
        else if (model == MODEL_TAXI || model == 102)
            return MODEL_TAXI;
        return model;
    }
    
    static int __stdcall GetPoliceModel(unsigned int model) {
        if (model == MODEL_POLICE || model == m_MODEL_POLICE)
            return MODEL_POLICE;
        return model;
    }

    static void Patch_5373D7();
    static void Patch_4F5857();

    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
        bool result; 
        
        switch (_this->m_nModelIndex) {
        case MODEL_FBICAR:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case m_MODEL_POLICE:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle) {
        if (!vehicle->m_nVehicleFlags.b25) {
            vehicle->m_nVehicleFlags.b25 = 1;// vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated = 1;
            int _random = plugin::Random(0, 2);
            switch (vehicle->m_nModelIndex) {
            case m_MODEL_POLICE:
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(_random);
                break;
            case MODEL_POLICE:
            case MODEL_RHINO:
            case MODEL_BARRACKS:
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(0);
                break;
            case MODEL_FBICAR:
            case MODEL_ENFORCER:
                vehicle->SetUpDriver();
                vehicle->SetupPassenger(0);
                vehicle->SetupPassenger(1);
                vehicle->SetupPassenger(2);
                break;
            default:
                return;
            }
        }
    }

    static int __cdecl ChoosePoliceCarModel() {
        int result;

        CPlayerPed *player = FindPlayerPed();
        if (player) {
            if (player->m_pWanted->AreSwatRequired()
                && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED)
            {
                int _random = plugin::Random(0, 10);
                /*if (_random == 0)
                result = MODEL_POLICE;*/
                if (_random < 5)
                    result = MODEL_ENFORCER;
                else {
                    unsigned char oldFlags = CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nFlags;
                    CStreaming::RequestModel(m_MODEL_POLICE, GAME_REQUIRED);
                    CStreaming::LoadAllRequestedModels(false);
                    if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED) {
                        if (!(oldFlags & GAME_REQUIRED)) {
                            CStreaming::SetModelIsDeletable(m_MODEL_POLICE);
                            CStreaming::SetModelTxdIsDeletable(m_MODEL_POLICE);
                        }
                        result = m_MODEL_POLICE;
                    }
                    else
                        result = MODEL_POLICE;
                }
            }
            else
            {
                if (player->m_pWanted->AreFbiRequired()
                    && CStreaming::ms_aInfoForModel[MODEL_FBICAR].m_nLoadState == LOADSTATE_LOADED
                    && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
                {
                    result = MODEL_FBICAR;
                }
                else {
                    if (player->m_pWanted->AreArmyRequired()
                        && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == LOADSTATE_LOADED
                        && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == LOADSTATE_LOADED
                        && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == LOADSTATE_LOADED)
                    {
                        int __random = plugin::Random(0, 10);
                        if (__random < 7)
                            result = MODEL_BARRACKS;
                        else
                            result = MODEL_RHINO;
                    }
                    else
                        result = MODEL_POLICE;
                }
            }
        }
        return result;
    }

    static bool __fastcall UsesPoliceRadio(cMusicManager *_this, int, CVehicle *vehicle) {
        bool result; 

        switch (vehicle->m_nModelIndex) {
        case MODEL_FBICAR:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case m_MODEL_POLICE:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    static bool __fastcall UsesSiren(CVehicle *_this, int, int vehicleModel) {
        bool result; 

        switch (vehicleModel) {
        case MODEL_FIRETRUK:
        case MODEL_AMBULAN:
        case MODEL_FBICAR:
        case MODEL_MRWHOOP:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case m_MODEL_POLICE:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    static bool __fastcall PlayerInCar(cMusicManager *_this) {
        int action; 
        bool result; 

        if (FindPlayerVehicle()) {
            action = FindPlayerPed()->m_nState;
            if (action != 51 && action != 54 && action != 56) {
                CVehicle *vehicle = FindPlayerVehicle();
                if (vehicle) {
                    if (vehicle->m_nState == 5)
                        result = FALSE;
                    else {
                        switch (vehicle->m_nModelIndex) {
                        case MODEL_FIRETRUK:
                        case MODEL_AMBULAN:
                        case MODEL_MRWHOOP:
                        case MODEL_PREDATOR:
                        case MODEL_TRAIN:
                        case MODEL_SPEEDER:
                        case MODEL_REEFER:
                        case MODEL_GHOST:
                        case m_MODEL_POLICE:
                            result = FALSE;
                            break;
                        default:
                            goto LABEL_11;
                        }
                    }
                }
                else {
                LABEL_11:
                    result = TRUE;
                }
            }
            else
                result = FALSE;
        }
        else
            result = FALSE;
        
        return result;
    }
    
    static bool __fastcall UsesSirenAudio(cAudioManager *_this, int, int index)  {
        bool result; 

        switch (index) { // eVehicleIndex
        case 7:
        case 16:
        case 17:
        case 26:
        case 27:
        case 30:
        case 66:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    static bool __fastcall UsesSirenSwitching(cAudioManager *_this, int, int index) {
        bool result; 

        switch (index) { // eVehicleIndex
        case 16:
        case 26:
        case 27:
        case 30:
        case 66:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    static bool __cdecl IsCarSprayable(CAutomobile *car)  {
        bool result; 

        switch (car->m_nModelIndex)  {
        case MODEL_FIRETRUK:
        case MODEL_AMBULAN:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_BUS:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_DODO:
        case MODEL_COACH:
        case m_MODEL_POLICE:
            result = FALSE;
            break;
        default:
            result = TRUE;
            break;
        }
        return result;
    }

    static void __fastcall OpcodePlayerDrivingTaxiVehicle(CRunningScript *script) {
        script->CollectParameters(&script->m_nIp, 1);
        bool isTaxiModel = false;
        
        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
        if (player->m_bInVehicle) {
            unsigned int model = player->m_pVehicle->m_nModelIndex;
            if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_BORGNINE || model == 102)
                isTaxiModel = true;
        }
        script->UpdateCompareFlag(isTaxiModel);
    }

   
    static void __fastcall OpcodeIsPlayerInModel(CRunningScript *script) {
        script->CollectParameters(&script->m_nIp, 2);
        bool inModel = false;

        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
        if (player->m_bInVehicle) {
            unsigned int model = player->m_pVehicle->m_nModelIndex;
            if (model == CTheScripts::ScriptParams[1].uParam)
                inModel = true;
            else if (model == m_MODEL_POLICE && CTheScripts::ScriptParams[1].uParam == MODEL_POLICE) // Vigilante
                inModel = true;
            //else if (model == MODEL_PEREN && CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) // Paramedic
                //inModel = true;
        }
        script->UpdateCompareFlag(inModel);
    }
     

    Test() {
        patch::RedirectJump(0x552880, IsLawEnforcementVehicle);
        patch::RedirectJump(0x415C60, AddPoliceCarOccupants);
        patch::RedirectJump(0x4181F0, ChoosePoliceCarModel);
        patch::RedirectJump(0x57E6A0, UsesPoliceRadio);
        patch::RedirectJump(0x552200, UsesSiren);
        patch::RedirectJump(0x57E4B0, PlayerInCar);
        patch::RedirectJump(0x56C3C0, UsesSirenAudio);
        patch::RedirectJump(0x56C3F0, UsesSirenSwitching);
        patch::RedirectJump(0x426700, IsCarSprayable);

        patch::RedirectCall(0x446A93, OpcodePlayerDrivingTaxiVehicle);
        patch::Nop(0x446A98, 0x4C); // или сделать jump на 0x446AE4

        patch::RedirectCall(0x43DA19, OpcodeIsPlayerInModel);
        patch::Nop(0x43DA1E, 0x40); // или сделать jump на 0x43DA5E
        
        patch::RedirectJump(0x5373D7, Patch_5373D7);
        patch::RedirectJump(0x4F5857, Patch_4F5857);
    }
}test;

unsigned int Test::CurrentSpecialModel;
unsigned int Test::CurrentPoliceModel;

void __declspec(naked) Test::Patch_5373D7() {
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetSpecialModel
        mov CurrentSpecialModel, eax
        popad
        mov eax, CurrentSpecialModel
        lea edx, [eax - 0x61]
        mov edi, 0x5373DE
        jmp edi
    }
}

void __declspec(naked) Test::Patch_4F5857() {
    __asm {
        movsx   eax, word ptr[ebx + 5Ch]
        pushad
        push eax
        call GetPoliceModel
        mov CurrentPoliceModel, eax
        popad
        mov eax, CurrentPoliceModel
        pop     ecx
        pop     ecx
        sub     eax, 61h
        mov edx, 0x4F5860
        jmp edx
    }
}



//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//#include "TxdDef.h"
//#include "CFileLoader.h"
//#include "CTxdStore.h"
//#include "CModelInfo.h"
//#include "CGameLogic.h"
//#include "CDirectory.h"
//#include "CStreaming.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static TxdDef *m_txd;
//    static RwTexDictionary *m_dictionary;
//
//    Test() {
//        Events::drawingEvent += [] {
//            if (CGameLogic::ActivePlayers) {
//                if (CTxdStore::ms_pTxdPool->m_byteMap[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex].bEmpty)
//                    m_txd = nullptr;
//                else 
//                    m_txd = &CTxdStore::ms_pTxdPool->m_pObjects[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex];
//                    
//                gamefont::Print({
//                    Format("116 ID %d", CTxdStore::ms_pTxdPool->m_byteMap[CModelInfo::ms_modelInfoPtrs[116]->m_nType]),
//                    Format("m_MODEL_POLICE ID %d", CTxdStore::ms_pTxdPool->m_pObjects[CModelInfo::ms_modelInfoPtrs[m_MODEL_POLICE]->m_nObjectDataIndex])
//                }, 10, 250, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
//
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('B', 1000)) {
//                    if (CTxdStore::ms_pTxdPool->m_byteMap[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex].bEmpty)
//                        m_txd = nullptr;
//                    else {
//                        m_txd = &CTxdStore::ms_pTxdPool->m_pObjects[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex];
//                        
//                        //CModelInfo::ms_modelInfoPtrs[116]->ClearTexDictionary();
//                        //CModelInfo::ms_modelInfoPtrs[116]->SetTexDictionary("police4");
//                        //CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                        
//                        //CMessages::AddMessageJumpQ(L"OK", 2000, 0);
//
//                        //m_dictionary = CFileLoader::LoadTexDictionary(GAME_PATH("txd\\mpolice.txd"));
//                        //if (m_dictionary) {
//                        //    CModelInfo::ms_modelInfoPtrs[116]->ClearTexDictionary();
//                        //    CModelInfo::ms_modelInfoPtrs[116]->SetTexDictionary("police4");
//                        //    CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                        //    //RwTexDictionarySetCurrent(m_txd->m_pRwDictionary);
//                        //    //CFileLoader::AddTexDictionaries(m_txd->m_pRwDictionary, m_dictionary);
//                        //    //RwTexDictionaryDestroy(m_dictionary);
//                        //    //m_txd->m_pRwDictionary = m_dictionary;
//                        //    CMessages::AddMessageJumpQ(L"OK", 2000, 0);
//                        //}
//                    }
//                }
//            }
//        };
//    }
//} test;
//
//TxdDef *Test::m_txd = nullptr;
//RwTexDictionary *Test::m_dictionary = nullptr;

//#include <plugin.h>
//#include "common.h"
//#include "CModelInfo.h"
//#include "CTxdStore.h"
//#include "eVehicleModel.h"
//#include "CSprite2d.h"
//#include "extensions\KeyCheck.h"
//#include "CFileLoader.h"
//
//char *gString = (char *)0x711B40;
//
//using namespace plugin;
//
//class VehicleTexture {
//public:
//    //static RwTexture *pMyTexture;
//    static RwTexture *myTexture;
//    static RwTexture *texture;
//
//    static RwTexDictionary *m_txd;
//    static RwTexture *m_textureLogo;
//    static RwTexture *m_textureTest;
//
//    static float ScreenCoord(float a) {
//        return static_cast<int>(a * (static_cast<float>(RsGlobal.maximumHeight) / 900.0f));
//    }
//
//    //static void Draw() {
//
//    //    static TxdDef *myTxd;
//    //    
//    //    KeyCheck::Update();
//    //    //if (KeyCheck::CheckWithDelay('J', 1000)) {
//    //    //    if (CTxdStore::ms_pTxdPool->m_byteMap[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex].bEmpty)
//    //    //        myTxd = nullptr;
//    //    //    else {
//    //    //        myTxd = &CTxdStore::ms_pTxdPool->m_pObjects[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex];
//    //    //        RwTexture *texture = RwTexDictionaryFindNamedTexture(myTxd->m_pRwDictionary, "remap");
//    //    //        if (texture) {
//    //    //            if (texture->dict) {
//    //    //                texture->dict = 0;
//    //    //                texture->lInDictionary.prev = texture->lInDictionary.next;
//    //    //                texture->lInDictionary.next = texture->lInDictionary.prev;
//    //    //            }
//    //    //            //texture = RwTextureRead("test", 0);
//    //    //            //RwTextureDestroy(texture);
//    //    //            //RwTexDictionaryAddTexture(myTxd->m_pRwDictionary, myTexture);
//    //    //        }
//    //    //    }
//    //    //}
//
//    //    if (KeyCheck::CheckWithDelay('N', 1000)) {
//    //        //CVehicle *vehicle = FindPlayerVehicle();
//    //        //if (vehicle) {
//    //            CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[91]);
//    //            if (myTexture) {
//    //                RpMaterial **materialPrim;
//    //                materialPrim = &vehModel->m_apMaterialsPrimary[13];
//    //                RpMaterial *material;
//    //                //int i = 13;
//    //                while (true) {
//    //                    material = *materialPrim;
//    //                    if (!*materialPrim)
//    //                        break;
//    //                    if (myTexture)
//    //                        RpMaterialSetTexture(material, myTexture);
//    //                    ++materialPrim; //++i;
//    //                }
//    //            }
//    //        //}
//    //    }
//    //    
//    //    if (KeyCheck::CheckWithDelay('B', 1000)) {
//    //        //CVehicle *vehicle = FindPlayerVehicle();
//    //        //if (vehicle) {
//    //            CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[91]);
//    //            RpMaterial **materialPrim;
//    //            materialPrim = &vehModel->m_apMaterialsPrimary[0];
//    //            RpMaterial *material;
//    //            TxdDef *txd = CTxdStore::ms_pTxdPool->GetAt(CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex);
//    //            if (txd) {
//    //                RwTexture *remaptexture = RwTexDictionaryFindNamedTexture(txd->m_pRwDictionary, "remap");
//    //                if (remaptexture) {
//    //                    int i = 0;
//    //                    while (i < 13) {
//    //                        material = *materialPrim;
//    //                        if (!*materialPrim)
//    //                            break;
//    //                        RpMaterialSetTexture(material, remaptexture);
//    //                        ++materialPrim; ++i;
//    //                    }
//    //                }
//    //            }
//    //        //}
//    //    }
//
//
//    //    CPed *player = FindPlayerPed();
//    //    //CVehicle *veh = FindPlayerVehicle();
//    //    //if (veh && veh->m_nModelIndex == MODEL_IDAHO) {
//    //    if (player) {
//    //        //if (!myTexture) {
//    //        //    //sprintf(gString, "image\\%s.bmp", "mytest");
//    //        //    //RwImage *image = RtBMPImageRead(gString);
//    //        //    RwImage *image = RtBMPImageRead("image\\mytest.bmp");
//    //        //    if (image) {
//    //        //        RwInt32 width, height, depth, flags;
//    //        //        RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
//    //        //        RwRaster *raster = RwRasterCreate(width, height, depth, flags);
//    //        //        RwRasterSetFromImage(raster, image);
//    //        //        myTexture = RwTextureCreate(raster);
//    //        //        RwTextureSetName(myTexture, "mytest");
//    //        //        RwImageDestroy(image);
//    //        //    }
//    //        //}
//
//    //        //TxdDef *txd = CTxdStore::ms_pTxdPool->GetAt(CModelInfo::ms_modelInfoPtrs[veh->m_nModelIndex]->m_nTxdIndex);
//    //        if (!texture) {
//    //            TxdDef *txd = CTxdStore::ms_pTxdPool->GetAt(CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex);
//    //            if (txd) {
//    //                texture = RwTexDictionaryFindNamedTexture(txd->m_pRwDictionary, "test");
//    //            }
//    //        }
//    //        
//    //        if (texture) {
//    //            RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, texture->raster);
//    //            //CSprite2d::DrawRect(CRect(RsGlobal.maximumWidth / 2 - 128, 200.0f, RsGlobal.maximumWidth / 2 + 128, 264.0f), CRGBA(255, 255, 255, 255));
//    //            //CSprite2d::SetVertices(CRect(RsGlobal.maximumWidth - (RsGlobal.maximumWidth - 130), RsGlobal.maximumHeight - 122, RsGlobal.maximumWidth - (RsGlobal.maximumWidth - 210), RsGlobal.maximumHeight - 42), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//    //            CSprite2d::SetVertices(CRect(ScreenCoord(120.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(200.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//    //            RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//    //        }
//    //        
//    //        /*if (myTexture) {
//    //            RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, myTexture->raster);
//    //            CSprite2d::SetVertices(CRect(ScreenCoord(220.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(300.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//    //            RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//    //        }*/
//
//    //    }
//    //}
//
//    /*static void DrawLogo() {
//        CPed *player = FindPlayerPed();
//        if (player) {
//            if (m_textureTest) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureTest->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(120.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(200.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//            if (m_textureLogo) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureLogo->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(220.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(640.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//        }
//    }*/
//
//    VehicleTexture() {
//        Events::initRwEvent += [] {
//            m_txd = CFileLoader::LoadTexDictionary(GAME_PATH("txd\\mpolice.txd"));
//            m_textureLogo = GetFirstTexture(m_txd);
//            m_textureTest = RwTexDictionaryFindNamedTexture(m_txd, "test");
//        };
//
//        //Events::drawingEvent += Draw;
//        //Events::menuDrawingEvent += Draw;
//        //Events::menuDrawingEvent += DrawLogo;
//        //Events::drawHudEvent += DrawLogo;
//
//        Events::drawingEvent += [] {
//            if (m_textureTest) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureTest->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(120.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(200.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//            if (m_textureLogo) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureLogo->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(220.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(640.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//        };
//
//        Events::shutdownRwEvent += [] {
//            RwTexDictionaryDestroy(m_txd);
//        };
//    }
//} vehTexture;
//
////RwTexture *VehicleTexture::pMyTexture;
//RwTexture *VehicleTexture::myTexture = nullptr;
//RwTexture *VehicleTexture::texture = nullptr;
//
//RwTexDictionary *VehicleTexture::m_txd;
//RwTexture *VehicleTexture::m_textureLogo;
//RwTexture *VehicleTexture::m_textureTest;


//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "common.h"
//#include "CWorld.h"
//#include "CObject.h"
//#include "CFont.h"
//#include "CTimer.h"
//
////RwFrame* nRwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, int data) {
////    return ((RwFrame*(__cdecl *)(RwFrame*, RwObjectCallBack, int))0x5A2340)(frame, callBack, data);
////}
////RwObject* GetCurrentAtomicObjectCB(RwObject *rwObject, void *data) {
////    return ((RwObject*(__cdecl *)(RwObject*, void*))0x52C690)(rwObject, data);
////}
//
//using namespace plugin;
//
//class Test {
//public:
//    /*static CVehicle* GetVehicle(CPed *player) {
//        CVehicle* result = nullptr;
//        CEntity *outEntity[10] = { nullptr };
//        short outCount = 0;
//        int index = 0;
//        float radius = 20.0f;
//        float distance = 0.0f;
//        CWorld::FindObjectsInRange(player->GetPosition(), 20.0, 1, &outCount, 10, outEntity, 0, 1, 0, 0, 0);
//        if (outCount > 0) {
//            do
//            {
//                CVehicle *vehicle = (CVehicle *)outEntity[index];
//                if (vehicle && player->m_pVehicle) {
//                    distance = DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition());
//                    if (distance < radius) {
//                        radius = distance; result = vehicle;
//                    }
//                }
//                ++index;
//            } while (index < outCount);
//        }
//        return result;
//    }*/
//
//    Test() {
//        static float &UserPause = *((float *)0x5F76B8 + 2);
//        static bool &Mode = *(bool *)0x95CD5B;
//
//        //Events::gameProcessEvent += [] {
//        Events::drawingEvent += [] {
//            CFont::SetScale(0.5f, 1.0f);
//            CFont::SetColor(CRGBA(255, 255, 255, 255));
//            CFont::SetJustifyOn();
//            CFont::SetFontStyle(0);
//            CFont::SetPropOn();
//            CFont::SetWrapx(600.0f);
//            wchar_t text[64];
//            swprintf(text, L"Mode %d", Mode);
//            CFont::PrintString(10.0f, 10.0f, text);
//
//            CPed *player = FindPlayerPed();
//            if (player) {
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('H', 1000)) {
//                    //UserPause = CTimer::m_UserPause;
//                    //CTimer::m_CodePause = TRUE;
//                    //UserPause = 0.0f;
//                    Mode = TRUE;
//                }
//                    
//                if (KeyCheck::CheckWithDelay('J', 1000))
//                    Mode = FALSE;
//                    //UserPause = 0.001f;
//                    //CTimer::m_CodePause = FALSE;
//            }
//            
//            //CVehicle *vehicle = FindPlayerVehicle();
//            //if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
//            //    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
//            //    KeyCheck::Update();
//            //    if (KeyCheck::CheckWithDelay('H', 1000)) {
//            //        //CPed *player = FindPlayerPed();
//            //        //if (player) {
//            //            //CVehicle* car = GetVehicle(player);
//            //            //if (car) {
//            //                RpAtomic *rwObject = nullptr;
//            //                RwFrameForAllObjects(automobile->m_aCarNodes[CAR_BOOT], GetCurrentAtomicObjectCB, &rwObject);
//            //                if (rwObject) {
//            //                    CObject *object = nullptr;
//            //                    object = new CObject();
//            //                    if (object) {
//            //                        object->SetModelIndexNoCreate(193);
//            //                        object->RefModelInfo(automobile->m_nModelIndex);
//            //                        RwMatrix *matrix = RwFrameGetLTM(automobile->m_aCarNodes[CAR_BOOT]);
//            //                        RwFrame *frame = RwFrameCreate();
//            //                        rwObject = RpAtomicClone(rwObject);
//            //                        frame->modelling = *matrix;
//            //                        RpAtomicSetFrame(rwObject, frame);
//
//            //                        object->AttachToRwObject(&rwObject->object.object);
//            //                        object->m_fMass = 0x41200000;
//            //                        object->m_fTurnMass = 0x41C80000;
//            //                        object->m_fAirResistance = 0x3F7851EC;
//            //                        object->m_fElasticity = 0x3DCCCCCD;
//            //                        object->m_vecMoveSpeed.x = automobile->m_vecMoveSpeed.x;
//            //                        object->m_vecMoveSpeed.y = automobile->m_vecMoveSpeed.y;
//            //                        object->m_vecMoveSpeed.z = automobile->m_vecMoveSpeed.z;
//
//            //                        object->m_matrix.pos.x = automobile->m_matrix.pos.x + 2.0f;
//            //                        object->m_matrix.pos.y = automobile->m_matrix.pos.y + 2.0f;
//            //                        object->m_matrix.pos.z = automobile->m_matrix.pos.z;
//
//            //                        CWorld::Add(object);
//            //                    }
//            //                    //car->AttachToRwObject(&rwObject->object.object);
//            //                }
//            //            //}
//            //        //}
//            //    }
//            //}
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CModelInfo.h"
//#include "RenderWare.h"
//#include "common.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static void __stdcall AddWeapon(int modelIndex)
//    {
//        CPed *ped = FindPlayerPed();
//        RpAtomic *atomic; 
//        CBaseModelInfo *baseInfo;
//
//        if (modelIndex != -1)
//        {
//            baseInfo = CModelInfo::ms_modelInfoPtrs[modelIndex];
//            atomic = (RpAtomic *)baseInfo->CreateInstance();
//            RwFrameDestroy((RwFrame *)atomic->object.object.parent);
//            RpAtomicSetFrame(atomic, ped->m_apFrames[HEAD]->m_pIFrame);
//            RpClumpAddAtomic(ped->m_pRwClump, atomic);
//            ped->m_nWepModelID = modelIndex;
//        }
//    }
//    
//    Test() {
//        patch::RedirectJump(0x4CF8F0, AddWeapon);
//    }
//} test;

//#include "plugin.h"
//#include "eVehicleModel.h"
//#include "RenderWare.h"
//#include "common.h"
//#include "CTimer.h"
//#include "CVehicleModelInfo.h"
//
//#include "CMessages.h"
//
//RwFrame* nRwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, int data) {
//    return ((RwFrame*(__cdecl *)(RwFrame*, RwObjectCallBack, int))0x5A2340)(frame, callBack, data);
//}
//
//void SetWindowOpenFlag(CVehicle *vehicle, unsigned int component) {
//    RwFrame *frame; 
//    frame = CClumpModelInfo::GetFrameFromId(vehicle->m_pRwClump, component);
//    if (frame)
//        frame = nRwFrameForAllObjects(frame, CVehicleModelInfo::SetAtomicFlagCB, 0x1000);
//}
//
//void ClearWindowOpenFlag(CVehicle *vehicle, unsigned int component) {
//    RwFrame *frame; 
//    frame = CClumpModelInfo::GetFrameFromId(vehicle->m_pRwClump, component);
//    if (frame)
//        frame = nRwFrameForAllObjects(frame, CVehicleModelInfo::ClearAtomicFlagCB, 0x1000);
//}
//
//
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        static int keyPressTime = 0;
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
//                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
//                if (KeyPressed(51) && CTimer::m_snTimeInMilliseconds - keyPressTime > 500) {
//                    keyPressTime = CTimer::m_snTimeInMilliseconds;
//                    ClearWindowOpenFlag(vehicle, 12);
//                    static char mess[256];
//                    snprintf(mess, 256, "on: %d", vehicle->m_nModelIndex);
//                    CMessages::AddMessageJumpQ(mess, 5000, false);
//                }
//                if (KeyPressed(50) && CTimer::m_snTimeInMilliseconds - keyPressTime > 500) {
//                    keyPressTime = CTimer::m_snTimeInMilliseconds;
//                    SetWindowOpenFlag(vehicle, 12);
//                    static char message[256];
//                    snprintf(message, 256, "off: %d", vehicle->m_nModelIndex);
//                    CMessages::AddMessageJumpQ(message, 5000, false);
//                }
//            }
//        };
//    }
//} test;

//class CanPedEnterCar {
//public:
//    //static bool Can(CVehicle *vehicle) {
//    //    float x; 
//    //    float y; 
//    //    double z; 
//    //    bool result; 
//
//    //    result = FALSE;
//    //    if (vehicle->m_nModelIndex == 90)
//    //        result = TRUE;
//    //    else
//    //        result = FALSE;
//    //    //x = vehicle->m_matrix.at.x;
//    //    //y = vehicle->m_matrix.at.y;
//    //    //z = vehicle->m_matrix.at.z;
//    //    ////if (z > 0.1 || z < -0.1) {
//    //    //    if (vehicle->m_vecMoveSpeed.y * vehicle->m_vecMoveSpeed.y
//    //    //        + vehicle->m_vecMoveSpeed.x * vehicle->m_vecMoveSpeed.x
//    //    //        + vehicle->m_vecMoveSpeed.z * vehicle->m_vecMoveSpeed.z <= 0.04) {
//    //    //        if (vehicle->m_nModelIndex == 90)
//    //    //            result = TRUE;
//    //    //        else
//    //    //            result = FALSE;
//    //    //        /*result = vehicle->m_vecTurnSpeed.y * vehicle->m_vecTurnSpeed.y
//    //    //            + vehicle->m_vecTurnSpeed.x * vehicle->m_vecTurnSpeed.x
//    //    //            + vehicle->m_vecTurnSpeed.z * vehicle->m_vecTurnSpeed.z <= 0.04;*/
//    //    //    }
//    //    //    else
//    //    //        result = TRUE;
//    //    ////}
//    //    ////else 
//    //    //    //result = FALSE;
//    //    return result;
//    //}
//    
//    //static bool __stdcall Siren(int vehicleModel)
//    //{
//    //    bool result; // al@2
//
//    //    switch (vehicleModel)
//    //    {
//    //    case MODEL_FIRETRUK:
//    //    case MODEL_AMBULAN:
//    //    case MODEL_FBICAR:
//    //    case MODEL_MRWHOOP:
//    //    case MODEL_POLICE:
//    //    case MODEL_ENFORCER:
//    //    case MODEL_PREDATOR:
//    //    case MODEL_LANDSTAL:
//    //        result = TRUE;
//    //        break;
//    //    default:
//    //        result = FALSE;
//    //        break;
//    //    }
//    //    return result;
//    //}
//
//    CanPedEnterCar() {
//        //patch::RedirectJump(0x5522F0, Can);
//        //patch::RedirectJump(0x552200, Siren);
//    }
//} canPedEnterCar;
