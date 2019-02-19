#include "plugin.h"
#include "common.h"
#include "CModelInfo.h"
#include "CTxdStore.h"
#include "CKeyGen.h"
#include "extensions\KeyCheck.h"

using namespace plugin;

class VehicleTexture {
public:
    static int index;

    static void Remap() {
        KeyCheck::Update();
        if (KeyCheck::CheckWithDelay('P', 1000)) {
            CPed *player = FindPlayerPed(-1);
            if (player) {
                if (player->m_pVehicle && player->m_nPedFlags.bInVehicle) {
                    TxdDef *txd = CTxdStore::ms_pTxdPool->GetAt(CModelInfo::ms_modelInfoPtrs[player->m_pVehicle->m_nModelIndex]->m_nTxdIndex);
                    if (txd) {
                        RwTexture *texture = nullptr;

                        switch (index) {
                        case 0:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint1"));     break;
                        case 1:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint2"));     break;
                        case 2:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint3"));     break;
                        case 3:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint4"));     break;
                        case 4:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint5"));     break;
                        case 5:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint6"));     break;
                        case 6:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint7"));     break;
                        case 7:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint8"));     break;
                        case 8:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint9"));     break;
                        case 9:  texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint10"));    break;
                        case 10: texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint11"));    break;
                        case 11: texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::GetUppercaseKey("paint12"));    break;
                        case 12: player->m_pVehicle->m_pRemapTexture = nullptr; break;
                        }

                        if (texture) {
                            player->m_pVehicle->m_pRemapTexture = texture; 
                            if (index <= 11)
                                ++index;
                            else
                                index = 0;
                        }
                        else {
                            player->m_pVehicle->m_pRemapTexture = nullptr;
                            index = 0;
                        }
                    }
                }
            }
        }
    }

    VehicleTexture() {
        Events::gameProcessEvent += Remap;
    }
} vehTexture;

int VehicleTexture::index = 0;
