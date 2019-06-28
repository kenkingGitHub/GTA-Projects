

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//#include "CWorld.h"
//#include "eWeaponModel.h"
//#include "eWeaponType.h"
//#include "CStreaming.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static int m_test;
//
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('B', 1000)) {
//                static char message[256];
//                snprintf(message, 256, "test = %d", m_test);
//                CMessages::AddMessageJumpQ(message, 1000, false);
//            }
//            if (player) {
//                if (KeyCheck::CheckWithDelay('N', 1000)) {
//                    //if (player->m_bInVehicle)
//                    if (Command<COMMAND_IS_PLAYER_IN_ANY_CAR>(CWorld::PlayerInFocus))
//                        CMessages::AddMessageJumpQ(L"in car", 1000, 0);
//                    else
//                        CMessages::AddMessageJumpQ(L"not in car", 1000, 0);
//                }
//                if (KeyCheck::CheckWithDelay('M', 1000)) {
//                    //CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 1000;
//                    Command <COMMAND_ADD_SCORE>(CWorld::PlayerInFocus, 1000);
//
//                    /*CStreaming::RequestModel(MODEL_COLT45, 2);
//                    CStreaming::LoadAllRequestedModels(false);
//                    player->GiveWeapon(WEAPONTYPE_PISTOL, 100, true);
//                    player->SetCurrentWeapon(WEAPONTYPE_PISTOL);
//                    CStreaming::SetModelIsDeletable(MODEL_COLT45);*/
//                    Command<COMMAND_REQUEST_MODEL>(MODEL_COLT45, 2);
//                    Command<COMMAND_LOAD_ALL_MODELS_NOW>(false);
//                    if (Command<COMMAND_HAS_MODEL_LOADED>(MODEL_COLT45)) {
//                        Command<COMMAND_GIVE_WEAPON_TO_PLAYER>(CWorld::PlayerInFocus, WEAPONTYPE_PISTOL, 100, true);
//                        Command<COMMAND_SET_CURRENT_PLAYER_WEAPON>(CWorld::PlayerInFocus, WEAPONTYPE_PISTOL);
//                        Command<COMMAND_MARK_MODEL_AS_NO_LONGER_NEEDED>(MODEL_COLT45);
//                    }
//                }
//            }
//        };
//    }
//} test;
//
//int Test::m_test = 3;

//#include "plugin.h"
//#include "CSprite2d.h"
//#include "CFileLoader.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static RwTexture *textureTest;
//    static RwTexDictionary *m_txd;
//
//    static float ScreenCoord(float a) {
//        return static_cast<int>(a * (static_cast<float>(RsGlobal.maximumHeight) / 900.0f));
//    }
//
//    Test() {
//        Events::initRwEvent += [] {
//            m_txd = CFileLoader::LoadTexDictionary(GAME_PATH("models\\MYTEST.TXD"));
//            textureTest = GetFirstTexture(m_txd);
//        };
//
//        Events::drawingEvent += [] {
//            if (textureTest) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, textureTest->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(120.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(200.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255));
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//        };
//
//        Events::shutdownRwEvent += [] {
//            RwTexDictionaryDestroy(m_txd);
//        };
//    }
//} test;
//
//RwTexDictionary  *Test::m_txd;
//RwTexture *Test::textureTest;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static int blip;
//    
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            KeyCheck::Update();
//            if (player && KeyCheck::CheckWithDelay('M', 1000)) 
//                Command<COMMAND_ADD_BLIP_FOR_CHAR_OLD>(CPools::GetPedRef(player), 4, 2, &blip);
//                //Command<COMMAND_ADD_BLIP_FOR_CHAR>(CPools::GetPedRef(player), &blip);
//                //CVector offset = { 0.0f, 10.0f, 0.0f };
//                //CVector point = player->m_placement * offset;
//                //Command<COMMAND_ADD_BLIP_FOR_COORD_OLD>(point.x, point.y, point.z, 1, 3, &blip);
//            if (player && KeyCheck::CheckWithDelay('N', 1000)) 
//                Command<COMMAND_REMOVE_BLIP>(blip);
//        };
//    }
//} test;
//
//int Test::blip;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CModelInfo.h"
//#include "CFont.h"
//#include "CStreaming.h"
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
//
//            unsigned char oldFlags = CStreaming::ms_aInfoForModel[162].m_nFlags;
//            swprintf(text, L"Flag %d", oldFlags);
//            CFont::PrintString(10.0f, 50.0f, text);
//
//            swprintf(text, L"LoadState %d", CStreaming::ms_aInfoForModel[162].m_nLoadState);
//            CFont::PrintString(10.0f, 70.0f, text);
//        };
//
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            //unsigned char oldLoadState;
//            if (KeyCheck::CheckWithDelay(99, 1000)) {
//                //oldLoadState = CStreaming::ms_aInfoForModel[162].m_nLoadState;
//                
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino4");
//                CStreaming::ms_aInfoForModel[162].m_nLoadState = 0;
//                CStreaming::ms_aInfoForModel[162].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[162].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(98, 1000)) {
//                //oldLoadState = CStreaming::ms_aInfoForModel[162].m_nLoadState;
//                
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino3");
//                CStreaming::ms_aInfoForModel[162].m_nLoadState = 0;
//                CStreaming::ms_aInfoForModel[162].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[162].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(97, 1000)) {
//                //oldLoadState = CStreaming::ms_aInfoForModel[162].m_nLoadState;
//                
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino2");
//                CStreaming::ms_aInfoForModel[162].m_nLoadState = 0;
//                CStreaming::ms_aInfoForModel[162].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[162].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(96, 1000)) {
//                //oldLoadState = CStreaming::ms_aInfoForModel[162].m_nLoadState;
//                
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino");
//                CStreaming::ms_aInfoForModel[162].m_nLoadState = 0;
//                CStreaming::ms_aInfoForModel[162].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[162].m_nLoadState = oldLoadState;
//            }
//        };
//    }
//} myPlugin;

//#include "plugin.h"
//#include "CPed.h"
//#include "extensions\KeyCheck.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static CPed *GetRandomPed(CVector const &posn, float radius) {
//        std::vector<CPed *> peds;
//        for (auto ped : CPools::ms_pPedPool) {
//            if (DistanceBetweenPoints(ped->GetPosition(), posn) <= radius)
//                peds.push_back(ped);
//        }
//        return peds.empty() ? nullptr : peds[plugin::Random(0, peds.size() - 1)];
//    }
//    
//    Test() {
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('M', 500)) {
//                CPed *player = FindPlayerPed();
//                if (player) {
//                    /*CVector offset = { 0.0f, 20.0f, 0.0f };
//                    CVector point = player->m_placement * offset;
//                    player->m_nPedFlags.bHasObjectiveCompleted = 0;
//                    player->SetObjective(OBJECTIVE_RUN_TO_AREA, point);*/
//                    CPed *ped = GetRandomPed(player->GetPosition(), 15.0f);
//                    if (ped) {
//                        //CVector offset = { 0.0f, 2.0f, 0.0f };
//                        //CVector point = player->m_placement * offset;
//                        ped->m_nPedFlags.bHasObjectiveCompleted = 0;
//                        //ped->SetObjective(OBJECTIVE_RUN_TO_AREA, point);
//                        //ped->SetObjective(OBJECTIVE_SOLICIT_VEHICLE, player->m_pVehicle);
//                        ped->SetObjective(OBJECTIVE_ENTER_CAR_AS_PASSENGER, player->m_pVehicle);
//                        ped->m_dwObjectiveTimer = 60000;
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CPed.h"
//#include "CWorld.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pObjectPool->m_nSize; i++) {
//                    CObject *object = CPools::ms_pObjectPool->GetAt(i);
//                    if (object && /*object->m_nModelIndex == 370 &&*/ DistanceBetweenPoints(object->GetPosition(), player->GetPosition()) < 5.0f) {
//                        static char message[256];
//                        snprintf(message, 256, "ID = %d", object->m_nModelIndex);
//                        CMessages::AddMessageJumpQ(message, 1000, false);
//                        //object->m_placement.pos.x += 2.0f;
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CPed.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (auto car : CPools::ms_pVehiclePool) {
//                    if (DistanceBetweenPoints(car->GetPosition(), player->GetPosition()) < 20.0f && car->m_nVehicleClass == VEHICLE_AUTOMOBILE && car->m_fHealth > 50.0f) {
//                        if (car->m_pDriver && car->m_pDriver != player)
//                            car->m_pDriver->SetExitCar(car, 15);
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            KeyCheck::Update();
//            if (vehicle && KeyCheck::CheckWithDelay('M', 200)) {
//                CVector pos = { 0.0f, 0.0f, 0.0f };
//                Command<COMMAND_GET_CAR_COORDINATES>(CPools::GetVehicleRef(vehicle), &pos.x, &pos.y, &pos.z);
//                static char message[256];
//                snprintf(message, 256, "x = %.2f; y = %.2f; z = %.2f; ", pos.x, pos.y, pos.z);
//                CMessages::AddMessageJumpQ(message, 3000, false);
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        static char message[256];
//                        snprintf(message, 256, "MaxSpeed = %.2f", vehicle->m_pHandlingData->m_transmissionData.m_fMaxSpeed);
//                        CMessages::AddMessageJumpQ(message, 3000, false);
//                        vehicle->m_autopilot.m_nCruiseSpeed = 30.0f;
//                        float maxSpeed = 60.0f * vehicle->m_pHandlingData->m_transmissionData.m_fMaxSpeed;
//                        if (vehicle->m_autopilot.m_nCruiseSpeed >= maxSpeed)
//                            maxSpeed = vehicle->m_autopilot.m_nCruiseSpeed;
//                        vehicle->m_autopilot.m_nCruiseSpeed = maxSpeed;
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            static int blip;
//            KeyCheck::Update();
//            if (vehicle && KeyCheck::CheckWithDelay('M', 200)) {
//                Command<COMMAND_ADD_BLIP_FOR_CAR>(CPools::GetVehicleRef(vehicle), &blip);
//            }
//            if (vehicle && KeyCheck::CheckWithDelay('N', 200)) {
//                Command<COMMAND_REMOVE_BLIP>(&blip);
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CWorld.h"
//#include "CTimer.h"
//#include "CVehicle.h"
//#include "CCarCtrl.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        CVector offset = { 0.0f, 10.0f, 0.0f };
//                        CVector point = vehicle->m_placement * offset;
//                        
//                        if (point.z <= -100.0f)
//                            point.z = CWorld::FindGroundZForCoord(point.x, point.y);
//                        point.z = vehicle->GetDistanceFromCentreOfMassToBaseOfModel() + point.z;
//                        if (CCarCtrl::JoinCarWithRoadSystemGotoCoors(vehicle, point))
//                            vehicle->m_autopilot.m_nCarMission = 9;
//                        else
//                            vehicle->m_autopilot.m_nCarMission = 8;
//                        vehicle->m_nType |= 0x18;
//                        vehicle->m_nVehicleFlags.bIsEngineOn = 1;
//                        if (vehicle->m_autopilot.m_nCruiseSpeed <= 6)
//                            vehicle->m_autopilot.m_nCruiseSpeed = 6;
//                        else
//                            vehicle->m_autopilot.m_nCruiseSpeed = vehicle->m_autopilot.m_nCruiseSpeed;
//                        vehicle->m_autopilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
//
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        CVector offset = {0.0f, 10.0f, 0.0f};
//                        CVector point = vehicle->m_placement * offset;
//                        Command<COMMAND_CAR_GOTO_COORDINATES>(CPools::GetVehicleRef(vehicle), point.x, point.y, point.z);
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CFont.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::drawingEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        CFont::SetScale(0.5f, 1.0f);
//                        CFont::SetColor(CRGBA(255, 255, 255, 255));
//                        CFont::SetJustifyOn();
//                        CFont::SetFontStyle(1);
//                        CFont::SetPropOn();
//                        CFont::SetWrapx(600.0f);
//                        wchar_t text[64];
//                        swprintf(text, L"ID vehicle %.d", vehicle->m_nModelIndex);
//                        CFont::PrintString(10.0f, 10.0f, text);
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CWorld.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            KeyCheck::Update();
//            if (player && player->m_bInVehicle && player->m_pVehicle && KeyCheck::CheckWithDelay('M', 200)) {
//                player->m_pVehicle->m_fHealth = 1000.0f;
//                player->m_pVehicle->m_nPrimaryColor = 0;
//                player->m_pVehicle->m_nSecondaryColor = 0;
//                static char message[256];
//                snprintf(message, 256, "x = %.2f; y = %.2f; z = %.2f;", player->m_pVehicle->GetPosition().x, player->m_pVehicle->GetPosition().y, player->m_pVehicle->GetPosition().z);
//                CMessages::AddMessageJumpQ(message, 3000, false);
//                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 5000;
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CWorld.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            KeyCheck::Update();
//            if (vehicle && KeyCheck::CheckWithDelay('M', 200)) {
//                vehicle->m_fHealth = 1000.0f;
//                vehicle->m_nPrimaryColor = 0;
//                vehicle->m_nSecondaryColor = 0;
//                static char message[256];
//                snprintf(message, 256, "x = %.2f; y = %.2f; z = %.2f;", vehicle->GetPosition().x, vehicle->GetPosition().y, vehicle->GetPosition().z);
//                CMessages::AddMessageJumpQ(message, 3000, false);
//                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 5000;
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CClumpModelInfo.h"
//#include "CParticle.h"
//#include "CWorld.h"
//#include "extensions\KeyCheck.h"
//
//using namespace plugin;
//
//class ParticleTest {
//public:
//    static void CreateParticleOnVehicleNode(CVehicle *vehicle, char const *nodeName) {
//        RwFrame *node = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, nodeName); // получить компонент по названию
//        if (node) {
//            RwMatrix *m = RwFrameGetLTM(node); // получить матрицу компонента в глобальном пространстве
//            CParticle::AddParticle(PARTICLE_FLYERS, CVector(m->pos.x, m->pos.y, m->pos.z), CVector(0, 0, 0), nullptr, 0.4f, 0, 0, 0, 0);
//        }
//    }
//
//    ParticleTest() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (KeyPressed('I'))
//                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 5000;
//            if (player && player->m_bInVehicle && player->m_pVehicle && player->m_pVehicle->m_fGasPedal > 0) {
//                CreateParticleOnVehicleNode(player->m_pVehicle, "wheel_lb_dummy");
//                CreateParticleOnVehicleNode(player->m_pVehicle, "wheel_rb_dummy");
//            }
//        };
//    }
//} particleTest;

//#include "plugin.h"
//#include "game_vc\common.h"
//#include <map>
//
//using namespace plugin;
//
//class VehicleTextures {
//public:
//    VehicleTextures() {
//        static CdeclEvent<AddressList<0x4C9ED1, H_CALL, 0x4C9F23, H_CALL, 0x4CA10D, H_CALL, 0x5813D1, H_CALL>,
//            PRIORITY_BEFORE, ArgPickN<CEntity*, 0>, void(CEntity*)> myOnRenderOneNonRoad;
//
//        static std::map<RpMaterial*, RwTexture *> originalTextures;
//
//        myOnRenderOneNonRoad.before += [](CEntity *entity) {
//            if (KeyPressed('Z') && entity == FindPlayerVehicle()) {
//                CVehicle *vehicle = reinterpret_cast<CVehicle *>(entity);
//                RpClumpForAllAtomics(vehicle->m_pRwClump, [](RpAtomic *atomic, void *data) {
//                    RpGeometryForAllMaterials(atomic->geometry, [](RpMaterial *material, void *data) {
//                        if (originalTextures.find(material) == originalTextures.end())
//                            originalTextures[material] = material->texture;
//                        material->texture = *reinterpret_cast<RwTexture **>(0x77FA58);
//                        return material;
//                    }, nullptr);
//                    return atomic;
//                }, nullptr);
//            }
//        };
//
//        myOnRenderOneNonRoad.after += [](CEntity *) {
//            if (originalTextures.size() > 0) {
//                for (auto &i : originalTextures)
//                    i.first->texture = i.second;
//                originalTextures.clear();
//            }
//        };
//    }
//} vehicleTextures;

