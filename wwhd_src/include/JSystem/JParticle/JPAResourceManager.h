// Cafe resource ownership follows the GameCube JPA resource-manager classes.
#pragma once
#include "wwhd.h"
#include "d/d_com_inf_game.h"
#include "JSystem/JParticle/JPATexture.h"
namespace jpa_emitter_calc { struct JPADataBlockLinkInfo; }
struct JPAEmitterData {
    union {
        u8 mHeader[6];
        struct { gptr<gptr<jpa_emitter_calc::JPADataBlockLinkInfo>> pLinkInfoArray; be<u16> infoNum; };
    };
    be<u16> userIndex;
};
struct JPAEmitterResource {
    JPAEmitterData* getByUserIndex(u32);
    bool checkUserIndexDuplication(u32);
    void registration(JPAEmitterData*, u32);
    be<u32> registNum, maxNum;
    gptr<gptr<JPAEmitterData>> pEmtrResArray;
};
struct JPATextureResource;
struct JPAResourceManager {
    gptr<void> pHeap;
    gptr<JPAEmitterResource> pEmtrRes;
    gptr<JPATextureResource> pTexRes;
};
WWHD_OFFSET(JPAEmitterData, pLinkInfoArray, 0);
WWHD_OFFSET(JPAEmitterData, infoNum, 4);
WWHD_OFFSET(JPAEmitterData, userIndex, 6);
WWHD_SIZE(JPAEmitterData, 8);
WWHD_OFFSET(JPAEmitterResource, pEmtrResArray, 8);
WWHD_SIZE(JPAEmitterResource, 0xC);
WWHD_SIZE(JPAResourceManager, 0xC);

// HD: texture names carry Cafe binding-mode metadata alongside the canonical string.
struct JPATextureNameEntry {
    SafeString mName;
    be<u32> mMode, mFlags;
};
WWHD_SIZE(JPATextureNameEntry, 0x10);

struct JPATextureContext { u8 mPrefix[0x24]; be<s32> mTextureDataOffset; };
struct JPAStringVtable { u8 mPrefix[0x14]; be<u32> assureTermination; };
struct JPATextureResource {
    void registration(void*, SafeString*);
    JPADefaultTexture defaultTex;
    be<u32> registNum, maxNum;
    gptr<gptr<JPATextureArc>> pTexResArray;
    gptr<gptr<void>> mDescriptors;
    gptr<JPATextureNameEntry> mNames;
    gptr<void> mContext;
    be<s32> mDefaultTextureIndex;
};
WWHD_SIZE(JPATextureResource, 0x60);
WWHD_OFFSET(JPATextureResource, registNum, 0x44);
WWHD_OFFSET(JPATextureResource, mNames, 0x54);

WWHD_SIZE(JPATextureContext, 0x28);
WWHD_OFFSET(JPATextureContext, mTextureDataOffset, 0x24);
WWHD_SIZE(JPAStringVtable, 0x18);
WWHD_OFFSET(JPAStringVtable, assureTermination, 0x14);
WWHD_OFFSET(JPATextureResource, mDescriptors, 0x50);
WWHD_OFFSET(JPATextureResource, mContext, 0x58);
WWHD_OFFSET(JPATextureResource, mDefaultTextureIndex, 0x5C);
