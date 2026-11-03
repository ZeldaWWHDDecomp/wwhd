// Remaining field loading and list operations, adapted from tww JPAField.cpp.
#include "JSystem/JParticle/JPAField.h"
using namespace jpa_field_extension;
void JPAFieldListRemove(JPAFieldList*, JPAFieldLink*);
void JPAFieldListPrepend(JPAFieldList*, JPAFieldLink*);
void JPAFieldListAppend(JPAFieldList*, JPAFieldLink*);
void JPAFieldRegister(void*);
void* JPAFieldNew(u32);
void JPAFieldLinkConstruct(JPAFieldLink*, void*);
void JPAFieldListInitiate(JPAFieldList*);
namespace {
struct FieldContainer { jpa_field_b::JPABaseField fields[9]; };
struct StaticBlock { be<u32> words[4]; };
static FieldContainer sFieldContainer;
static StaticBlock sStaticBlock;
static u32 sStaticRegistration;
static const u32 sFieldDataVtable;
static const jpa_field_b::JPAFieldVtable sGravityVtable;
static const jpa_field_b::JPAFieldVtable sAirVtable;
static const jpa_field_b::JPAFieldVtable sMagnetVtable;
static const jpa_field_b::JPAFieldVtable sNewtonVtable;
static const jpa_field_b::JPAFieldVtable sVortexVtable;
static const jpa_field_b::JPAFieldVtable sRandomVtable;
static const jpa_field_b::JPAFieldVtable sDragVtable;
static const jpa_field_b::JPAFieldVtable sConvectionVtable;
static const jpa_field_b::JPAFieldVtable sSpinVtable;
}


/* 0282275C */
JPAFieldDataObject* JPAFieldDataConstruct(JPAFieldDataObject* self) {
    // HD: the vtable follows the field-data prefix instead of preceding it.
    if (!self) {
        self = static_cast<JPAFieldDataObject*>(JPAFieldNew(sizeof(JPAFieldDataObject)));
        if (!self) return nullptr;
    }
    self->__vtbl = &sFieldDataVtable;
    auto link = reinterpret_cast<JPAFieldLink*>(self->mLink);
    if (gabi::ea(link) == 0) link = static_cast<JPAFieldLink*>(JPAFieldNew(sizeof(JPAFieldLink)));
    if (link) JPAFieldLinkConstruct(link, self);
    // HD: retain null-preserving inline vector-constructor allocation branches.
    if (gabi::ea(&self->mVel) == 0) JPAFieldNew(sizeof(cXyz));
    if (gabi::ea(&self->mWork0) == 0) JPAFieldNew(sizeof(cXyz));
    if (gabi::ea(&self->mWork1) == 0) JPAFieldNew(sizeof(cXyz));
    if (gabi::ea(&self->mWork2) == 0) JPAFieldNew(sizeof(cXyz));
    if (gabi::ea(&self->mPos) == 0) JPAFieldNew(sizeof(cXyz));
    if (gabi::ea(&self->mDir) == 0) JPAFieldNew(sizeof(cXyz));
    return self;
}

/* 02822828 */
JPAFieldList* JPAFieldListConstruct(JPAFieldList* self) {
    if (!self) {
        self = static_cast<JPAFieldList*>(JPAFieldNew(sizeof(JPAFieldList)));
        if (!self) return nullptr;
    }
    JPAFieldListInitiate(self);
    return self;
}

/* 028228C8 */
void jpa_field_extension::JPABaseField::loadFieldData(JPAFieldData* data, JPAFieldBlock* block) {
    data->mType = gabi::call_ptr<u32>(block->__vtbl->getType, block);
    data->mID = gabi::call_ptr<u32>(block->__vtbl->getID, block);
    data->mVelType = gabi::call_ptr<u32>(block->__vtbl->getVelType, block);
    data->mCycle = gabi::call_ptr<u32>(block->__vtbl->getCycle, block);
    data->mSttFlag = gabi::call_ptr<u32>(block->__vtbl->getSttFlag, block);
    data->mMag = gabi::call_ptr<f32>(block->__vtbl->getMag, block);
    data->mMagRndm = gabi::call_ptr<f32>(block->__vtbl->getMagRndm, block);
    data->mMaxDist = gabi::call_ptr<f32>(block->__vtbl->getMaxDist, block);
    gabi::call_ptr<void>(block->__vtbl->getPos, block, &data->mPos);
    gabi::call_ptr<void>(block->__vtbl->getDir, block, &data->mDir);
    data->mVal1 = gabi::call_ptr<f32>(block->__vtbl->getVal1, block);
    data->mVal2 = gabi::call_ptr<f32>(block->__vtbl->getVal2, block);
    data->mVal3 = gabi::call_ptr<f32>(block->__vtbl->getVal3, block);
    data->mFadeIn = gabi::call_ptr<f32>(block->__vtbl->getFadeIn, block);
    data->mFadeOut = gabi::call_ptr<f32>(block->__vtbl->getFadeOut, block);
    data->mEnTime = gabi::call_ptr<f32>(block->__vtbl->getEnTime, block);
    f32 disTime = gabi::call_ptr<f32>(block->__vtbl->getDisTime, block);
    u32 flags = data->mSttFlag;
    data->mDisTime = disTime;
    if (!(flags & 0x10)) data->mDisTime = 1.0f;
    data->mVel.z = 0.0f;
    data->mVel.y = 0.0f;
    data->mVel.x = 0.0f;
}

/* 028240DC */
void jpa_field_extension::JPAFieldManager::initField(JPADataBlockLinkInfo* dataInfo, void* emtrInfo) {
    u32 count = dataInfo->mFieldNum;
    auto blocks = dataInfo->mpFieldBlocks.get();
    for (u32 i = 0; i < count; ++i) {
        JPAFieldList* vacant = mVacList;
        if (u32(vacant->mLength) == 0) continue;
        JPAFieldData* field = vacant->mHead->mObject;
        auto link = reinterpret_cast<JPAFieldLink*>(field->mLink);
        JPAFieldListRemove(vacant, link);
        JPAFieldListAppend(&mList, link);
        JPAFieldBlock* block = blocks[i];
        u32 type = gabi::call_ptr<u32>(block->__vtbl->getType, block);
        if (type <= 8) field->mpBaseField = &sFieldContainer.fields[type];
        l_emitterInfo.mpInfo = static_cast<jpa_field_b::JPAFieldEmitterInfo*>(emtrInfo);
        auto base = static_cast<jpa_field_extension::JPABaseField*>(field->mpBaseField.get());
        base->loadFieldData(field, blocks[i]);
    }
}

/* 028243B4 */
void jpa_field_extension::JPAFieldManager::deleteField(JPAFieldData* data) {
    auto link = reinterpret_cast<JPAFieldLink*>(data->mLink);
    JPAFieldListRemove(&mList, link);
    JPAFieldListPrepend(mVacList, link);
}

/* 028243FC */
void jpa_field_extension::JPAFieldManager::deleteAllField() {
    JPAFieldLink* link = mList.mHead;
    while (link) {
        JPAFieldLink* next = link->mNext;
        JPAFieldData* data = link->mObject;
        deleteField(data);
        link = next;
    }
}

// HD: compiler-generated field singleton initialization.
/* 02824450 */
void __sinit_JPAField_cpp() {
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[1] = 0;
    sStaticBlock.words[0] = 0;
    JPAFieldRegister(&sStaticRegistration);
    sFieldContainer.fields[0].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sGravityVtable);
    sFieldContainer.fields[1].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sAirVtable);
    sFieldContainer.fields[2].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sMagnetVtable);
    sFieldContainer.fields[3].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sNewtonVtable);
    sFieldContainer.fields[4].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sVortexVtable);
    sFieldContainer.fields[5].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sRandomVtable);
    sFieldContainer.fields[6].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sDragVtable);
    sFieldContainer.fields[7].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sConvectionVtable);
    sFieldContainer.fields[8].__vtbl = const_cast<jpa_field_b::JPAFieldVtable*>(&sSpinVtable);
}

// Native typed accessors keep the canonical field layout in its owning translation unit.
void* JPAFieldDataLinkAt(void* fields,u32 index) {
    return &static_cast<jpa_field_extension::JPAFieldDataObject*>(fields)[index].mLink;
}
void JPASetFieldVacantList(void* manager,void* vacant) {
    static_cast<jpa_field_b::JPAFieldManager*>(manager)->mVacList = static_cast<jpa_field_b::JPAFieldList*>(vacant);
}

/* WWHD glue
ext 0273AD10 void* JPAFieldNew(u32)
ext 027EC50C void JPAFieldLinkConstruct(JPAFieldLink*,void*)
ext 027EC678 void JPAFieldListInitiate(JPAFieldList*)
data sFieldDataVtable 10170DE0
ext 027EC560 void JPAFieldListRemove(JPAFieldList*,JPAFieldLink*)
ext 027EC6F4 void JPAFieldListAppend(JPAFieldList*,JPAFieldLink*)
ext 028F026C void JPAFieldRegister(void*)
data sFieldContainer 104B594C
data sStaticBlock 104B5938
data sStaticRegistration 101FA070
data sGravityVtable 10171094
data sAirVtable 101710C4
data sMagnetVtable 101710F4
data sNewtonVtable 10171124
data sVortexVtable 10171154
data sRandomVtable 101711B4
data sDragVtable 101711E4
data sConvectionVtable 10171184
data sSpinVtable 10171214
ext 027EC778 void JPAFieldListPrepend(JPAFieldList*,JPAFieldLink*)
*/
