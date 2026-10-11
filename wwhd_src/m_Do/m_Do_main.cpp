/** Main initialization and retained HD frame entry. Private decomp source. */
#include "bindings.h"
#include "m_Do/m_Do_main.h"

static mDoMain_StartupRecord sStartupRecord;
static mDoMain_StartupRecord* sCurrentStartupRecord;
static s8 sDevelopmentMode;
static void* sHeapManager;
static u8 mDisplayHeapSize;
static u32 frameCount;
static u8 fillcheck_check_frame;
static mDoMain_HeaderZeroRecord sHeaderZeroRecord;
static f32 sHeaderPiLower;
static f32 sHeaderPiUpper;
static f32 sHeaderPiLowerSource;
static f32 sHeaderPiUpperSource;
static u8 sHeaderFirstByte;
static u8 sHeaderSecondByte;

/* 025F1658 */
static s32 LOAD_COPYDATE(void*) {
    // HD: COPYDATE loading is disabled and always reports success.
    return 1;
}

/* 025F1660 */
void main01() {
    // HD: initialize the startup record before constructing the game information.
    sStartupRecord.field_0x0 = 0;
    sStartupRecord.field_0xc = 0xFFFFFFFF;
    sStartupRecord.field_0x8 = 0;
    sCurrentStartupRecord = &sStartupRecord;
    sStartupRecord.field_0x4 = 0;
    dComIfG_inf_c_ct(dComIfGp_get());
    sDevelopmentMode = 1;
    mDoMch_Create();
    void* gameHeap = mDoExt_getGameHeap();
    u32 previousHeap = 1;
    if (gameHeap != NULL) {
        previousHeap = mDoMch_setThreadHeap(sHeapManager, gabi::ea(gameHeap));
    }
    mDoHIO_disabledEntry();
    mDoGph_Create();
    // HD: the retained m_Re controller companion is empty.
    mReCPd_disabledCreate();
    mDoDvdThd_callback_c::create(LOAD_COPYDATE, NULL);
    fapGm_Create();
    mDisplayHeapSize = FALSE;
    if (previousHeap != 1) {
        mDoMch_setThreadHeap(sHeapManager, previousHeap);
    }
}

/* 025F172C */
// HD: the GameCube main loop is a single-frame entry called by GameTask.
static void frame() {
    u32 count = frameCount + 1;
    u8 period = fillcheck_check_frame;
    frameCount = count;
    if (period != 0 && count % period == 0) {
        mDoMch_HeapCheckAll();
    }
    mReCPd_disabledRead();
    mDoAud_Execute();
    fapGm_Execute();
}

/* 025F1788 */
static void __sinit_m_Do_main_cpp() {
    // HD: byte objects follow mDisplayHeapSize, so the common header helper does not apply.
    sHeaderZeroRecord.mWords[2] = 0;
    sHeaderZeroRecord.mWords[0] = 0;
    sHeaderZeroRecord.mWords[3] = 0;
    sHeaderZeroRecord.mWords[1] = 0;
    __register_global_object(0x101F489C);
    f32 lower = sHeaderPiLowerSource;
    f32 upper = sHeaderPiUpperSource;
    sHeaderPiLower = lower;
    sHeaderPiUpper = upper;
    mDoMain_headerFirstByteConstruct(&sHeaderFirstByte);
    __register_global_object(0x101F48A8);
    mDoMain_headerSecondByteConstruct(&sHeaderSecondByte);
    __register_global_object(0x101F48B4);
}

/* WWHD glue (tools/verify/autoglue.py): private main state and calls
 data sStartupRecord 1048D090
 data sCurrentStartupRecord 101F4974
 data sDevelopmentMode 101F48C0
 data sHeapManager 101F8B4C
 data mDisplayHeapSize 1048D07C
 data frameCount 1048D0A8
 data fillcheck_check_frame 1048D0AC
 data sHeaderZeroRecord 1048D080
 data sHeaderPiLower 1048D074
 data sHeaderPiUpper 1048D078
 data sHeaderPiLowerSource 10059038
 data sHeaderPiUpperSource 1005903C
 data sHeaderFirstByte 1048D07D
 data sHeaderSecondByte 1048D07E
 ext 02520020 void dComIfG_inf_c_ct(u8*)
 ext 025F12F8 bool mDoMch_Create()
 ext 025E2FBC void* mDoExt_getGameHeap()
 ext 02756170 u32 mDoMch_setThreadHeap(void*, u32)
 ext 025F0A08 void mDoHIO_disabledEntry()
 ext 025F0550 void mDoGph_Create()
 ext 025F2D70 void mReCPd_disabledCreate()
 ext 025F2D74 void mReCPd_disabledRead()
 ext 025E2384 void mDoDvdThd_createCallback(mDoDvdThd_callback_func, void*)
 ext 025D4320 void fapGm_Create()
 ext 025D42EC void fapGm_Execute()
 ext 025E15E0 void mDoAud_Execute()
 ext 025F1654 void mDoMch_HeapCheckAll()
 ext 028ED6F8 void mDoMain_headerFirstByteConstruct(be<u8>*)
 ext 028EAB2C void mDoMain_headerSecondByteConstruct(be<u8>*)
*/
