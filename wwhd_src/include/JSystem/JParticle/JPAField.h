// Remaining JPAField methods use the merged field-data and list layouts.
#pragma once
#include "JSystem/JParticle/JPA_field_b.h"
namespace jpa_field_extension {
using JPAFieldData = jpa_field_b::JPAFieldData;
using JPAFieldLink = jpa_field_b::JPAFieldLink;
using JPAFieldList = jpa_field_b::JPAFieldList;
struct JPAFieldBlockVtable {
    u8 mHeader[0x14];
    be<u32> getType, mTypeDelta, getID, mIDDelta, getVelType, mVelTypeDelta;
    be<u32> getCycle, mCycleDelta, getSttFlag, mSttFlagDelta;
    be<u32> getPos, mPosDelta, getDir, mDirDelta;
    be<u32> getMag, mMagDelta, getMagRndm, mMagRndmDelta;
    be<u32> getMaxDist, mMaxDistDelta, getVal1, mVal1Delta;
    be<u32> getVal2, mVal2Delta, getVal3, mVal3Delta;
    be<u32> getFadeIn, mFadeInDelta, getFadeOut, mFadeOutDelta;
    be<u32> getEnTime, mEnTimeDelta, getDisTime;
};
struct JPAFieldBlock { gptr<JPAFieldBlockVtable> __vtbl; };
struct JPAFieldDataObject : JPAFieldData {
    u8 mBeforeVtable[2];
    gptr<const void> __vtbl;
};
struct JPABaseField : jpa_field_b::JPABaseField {
    void loadFieldData(JPAFieldData*, JPAFieldBlock*);
};
struct JPADataBlockLinkInfo {
    u8 mPrefix[0x18];
    gptr<gptr<JPAFieldBlock>> mpFieldBlocks;
    u8 mBeforeCount[4];
    be<u8> mFieldNum;
};
struct JPAFieldManager : jpa_field_b::JPAFieldManager {
    void initField(JPADataBlockLinkInfo*, void*);
    void deleteField(JPAFieldData*);
    void deleteAllField();
};
WWHD_SIZE(JPAFieldBlockVtable, 0x98);
WWHD_OFFSET(JPAFieldBlockVtable, getMag, 0x4C);
WWHD_OFFSET(JPAFieldBlockVtable, getDisTime, 0x94);
WWHD_SIZE(JPAFieldBlock, 4);
WWHD_OFFSET(JPAFieldDataObject, __vtbl, 0x98);
WWHD_SIZE(JPAFieldDataObject, 0x9C);
WWHD_OFFSET(JPADataBlockLinkInfo, mpFieldBlocks, 0x18);
WWHD_OFFSET(JPADataBlockLinkInfo, mFieldNum, 0x20);
WWHD_SIZE(JPADataBlockLinkInfo, 0x21);
WWHD_SIZE(JPABaseField, 4);
WWHD_SIZE(JPAFieldManager, 0x10);
}
