/** HD save-data holder; local decomp */
#include "d/hd_save_info_holder.h"
static u32 sSaveInfoHolderHeaderStatics;
static u32 sSaveInfoHolderHeaderDescriptors;
static u32 sSaveInfoHolderHeaderZero;
static u32 sSaveInfoHolderPiConstants;

static hdSaveInfoHolder_c* sSaveInfoHolder_hd_save_info_holder;
static void* sSaveThread_hd_save_info_holder;
static u32 sSaveInfoVtable_hd_save_info_holder;
static u32 sSaveThreadVtable_hd_save_info_holder;
static void constructSaveArray(void*,u32,u32,u32);
static void* allocateSaveMemory(u32,void*,s32);
static void constructSaveThread(void*,void*,u32);
static void constructSaveHost(void*);
static void constructSaveInfo(void*,u32);
dSv_memory_c* createSaveMemory(dSv_memory_c*);
static void* createSaveDungeonMemory(void*);
static void initSaveMemory(void*);
static void* createSavePictureManager(void*);
static void initSaveInfo(void*);
/* 02720400 */
static hdSaveInfoHolder_c* createSaveInfoHolder(void* heap) {
    hdSaveInfoHolder_c* holder = sSaveInfoHolder_hd_save_info_holder;
    if (holder != NULL) return holder;
    holder = (hdSaveInfoHolder_c*)allocateSaveMemory(sizeof(hdSaveInfoHolder_c), heap, 4);
    void* thread = holder->mThreadBase;
    if (gabi::ea(thread) != 0) {
        constructSaveThread(thread, heap, 3);
        holder->mThreadVtable = gabi::ea(&sSaveThreadVtable_hd_save_info_holder);
    }
    sSaveThread_hd_save_info_holder = thread;
    if (holder != NULL) {
        constructSaveHost(holder);
        holder->mVtable = gabi::ea(&sSaveInfoVtable_hd_save_info_holder);
        constructSaveInfo(&holder->mSaveInfo, sizeof(holder->mSaveInfo));
        constructSaveArray(holder->mSaveInfo.mSavedata.mMemory, dSv_save_c::STAGE_MAX, sizeof(dSv_memory_c), gabi::guest_fn((const void*)createSaveMemory));
        initSaveMemory(&holder->mSaveInfo.mMemory);
        constructSaveArray(holder->mSaveInfo.mZone, dSv_info_c::ZONE_MAX, sizeof(dSv_zone_c), gabi::guest_fn((const void*)createSaveDungeonMemory));
        createSaveHolders(&holder->mHolders);
        createSavePictureManager(holder->mPictureManager);
    }
    sSaveInfoHolder_hd_save_info_holder = holder;
    return holder;
}

/* 02720510 */
void hdSaveInfoHolder_c::init() {
    initSaveInfo(&mSaveInfo); mHolders.clear();
}

/* 027205F0 */
static void __sinit_hd_save_info_holder_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sSaveInfoHolderHeaderStatics), gabi::ea(&sSaveInfoHolderHeaderDescriptors), gabi::ea(&sSaveInfoHolderHeaderZero), gabi::ea(&sSaveInfoHolderPiConstants));
}

/* WWHD glue
data sSaveInfoHolderHeaderStatics 1049F768 nocheck
data sSaveInfoHolderHeaderDescriptors 101F84B8 nocheck
data sSaveInfoHolderHeaderZero 1049F774 nocheck
data sSaveInfoHolderPiConstants 10140768 nocheck

 data sSaveInfoHolder_hd_save_info_holder 101F84DC
 data sSaveThread_hd_save_info_holder 101F84E0
 data sSaveInfoVtable_hd_save_info_holder 10140758
 data sSaveThreadVtable_hd_save_info_holder 10140770
 ext 028EFFD0 void constructSaveArray(void*,u32,u32,u32)
 ext 0273B0D4 void* allocateSaveMemory(u32,void*,s32)
 ext 02752B0C void constructSaveThread(void*,void*,u32)
 ext 02752A84 void constructSaveHost(void*)
 ext 028F521C void constructSaveInfo(void*,u32)
 ext 025B9774 void* createSaveDungeonMemory(void*) callback
 ext 025B9170 void initSaveMemory(void*)
 ext 02725518 void* createSavePictureManager(void*)
 ext 025B9A18 void initSaveInfo(void*)
*/
