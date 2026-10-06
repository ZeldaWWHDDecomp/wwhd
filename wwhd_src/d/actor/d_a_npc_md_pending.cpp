/* d_a_npc_md: every method of daNpc_Md_c as a plain guest call, weak: the decompiled
 * definition in one of the d_a_npc_md*.cpp files replaces it when linked into the same unit.
 */
#include "d/actor/d_a_npc_md.h"

__attribute__((weak)) s16 daNpc_Md_c::XyCheckCB(int a0) { return gabi::call<s16>(0x02283480, this, a0); }
__attribute__((weak)) s16 daNpc_Md_c::XyEventCB(int a0) { return gabi::call<s16>(0x02283588, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::initLightBtkAnm(u32 a0) { return gabi::call<BOOL>(0x0228461C, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::initTexPatternAnm(u8 a0, u32 a1) { return gabi::call<BOOL>(0x022846EC, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Md_c::createHeap() { return gabi::call<BOOL>(0x022847F8, this); }
__attribute__((weak)) BOOL daNpc_Md_c::setAction(ProcFunc_l* a0, ProcFunc_l* a1, void* a2) { return gabi::call<BOOL>(0x02285384, this, a0, a1, a2); }
__attribute__((weak)) void daNpc_Md_c::setNpcAction(ProcFunc_l* a0, void* a1) { gabi::call(0x022854CC, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Md_c::shipRideCheck() { return gabi::call<BOOL>(0x02285520, this); }
__attribute__((weak)) BOOL daNpc_Md_c::init() { return gabi::call<BOOL>(0x02285580, this); }
__attribute__((weak)) void daNpc_Md_c::setAttention(bool a0) { gabi::call(0x02285984, this, a0); }
__attribute__((weak)) void daNpc_Md_c::setBaseMtx() { gabi::call(0x02285A6C, this); }
__attribute__((weak)) cPhs_State daNpc_Md_c::create() { return gabi::call<cPhs_State>(0x02286084, this); }
__attribute__((weak)) void daNpc_Md_c::checkPlayerRoom() { gabi::call(0x02286B5C, this); }
__attribute__((weak)) bool daNpc_Md_c::dNpc_Md_setAnm(mDoExt_McaMorf2* a0, f32 a1, int a2, f32 a3, f32 a4, const char* a5, const char* a6, const char* a7) { return gabi::call<bool>(0x02286BC0, this, a0, a1, a2, a3, a4, a5, a6, a7); }
__attribute__((weak)) bool daNpc_Md_c::dNpc_Md_setAnm(mDoExt_McaMorf* a0, int a1, f32 a2, f32 a3, const char* a4, const char* a5) { return gabi::call<bool>(0x02286D04, this, a0, a1, a2, a3, a4, a5); }
__attribute__((weak)) void daNpc_Md_c::deletePiyoPiyo() { gabi::call(0x02286D18, this); }
__attribute__((weak)) BOOL daNpc_Md_c::setAnm(int a0) { return gabi::call<BOOL>(0x02286D7C, this, a0); }
__attribute__((weak)) void daNpc_Md_c::playTexPatternAnm() { gabi::call(0x0228729C, this); }
__attribute__((weak)) u32 daNpc_Md_c::playLightBtkAnm() { return gabi::call<u32>(0x0228735C, this); }
__attribute__((weak)) void daNpc_Md_c::animationPlay() { gabi::call(0x02287364, this); }
__attribute__((weak)) BOOL daNpc_Md_c::isFallAction() { return gabi::call<BOOL>(0x02287684, this); }
__attribute__((weak)) BOOL daNpc_Md_c::lightHitCheck() { return gabi::call<BOOL>(0x02287754, this); }
__attribute__((weak)) void daNpc_Md_c::setCollision() { gabi::call(0x02287A74, this); }
__attribute__((weak)) BOOL daNpc_Md_c::checkCommandTalk() { return gabi::call<BOOL>(0x02287DF0, this); }
__attribute__((weak)) void daNpc_Md_c::returnLinkPlayer() { gabi::call(0x02287EAC, this); }
__attribute__((weak)) BOOL daNpc_Md_c::eventProc() { return gabi::call<BOOL>(0x02287EFC, this); }
__attribute__((weak)) void daNpc_Md_c::setPlayerAction(ProcFunc_l* a0, void* a1) { gabi::call(0x02288240, this, a0, a1); }
__attribute__((weak)) void daNpc_Md_c::playerAction(void* a0) { gabi::call(0x02288290, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::returnLinkCheck() { return gabi::call<BOOL>(0x022884D4, this); }
__attribute__((weak)) BOOL daNpc_Md_c::checkCollision(int a0) { return gabi::call<BOOL>(0x02288544, this, a0); }
__attribute__((weak)) void daNpc_Md_c::carryCheck() { gabi::call(0x022886B4, this); }
__attribute__((weak)) void daNpc_Md_c::checkOrder() { gabi::call(0x022886FC, this); }
__attribute__((weak)) void daNpc_Md_c::npcAction(void* a0) { gabi::call(0x022887DC, this, a0); }
__attribute__((weak)) void daNpc_Md_c::eventOrder() { gabi::call(0x022888C4, this); }
__attribute__((weak)) BOOL daNpc_Md_c::execute() { return gabi::call<BOOL>(0x02288B08, this); }
__attribute__((weak)) BOOL daNpc_Md_c::draw() { return gabi::call<BOOL>(0x02289E2C, this); }
__attribute__((weak)) void daNpc_Md_c::emitterTrace(JPABaseEmitter* a0, Mtx34* a1, csXyz* a2) { gabi::call(0x02288AB4, this, a0, a1, a2); }
__attribute__((weak)) void daNpc_Md_c::emitterDelete(be<u32>* a0) { gabi::call(0x0228A1C0, this, a0); }
__attribute__((weak)) void daNpc_Md_c::deleteHane02Emitter() { gabi::call(0x0228A1FC, this); }
__attribute__((weak)) void daNpc_Md_c::deleteHane03Emitter() { gabi::call(0x0228A228, this); }
__attribute__((weak)) void daNpc_Md_c::changeCaught02() { gabi::call(0x0228A4E4, this); }
__attribute__((weak)) void daNpc_Md_c::initialDefault(int a0) { gabi::call(0x0228A520, this, a0); } /* restartPoint: vtable slot 028F036C */
__attribute__((weak)) void daNpc_Md_c::lookBack(int a0, int a1, int a2) { gabi::call(0x0228A524, this, a0, a1, a2); }
__attribute__((weak)) BOOL daNpc_Md_c::actionDefault(int a0) { return gabi::call<BOOL>(0x0228A67C, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialWaitEvent(int a0) { gabi::call(0x0228A6AC, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionWaitEvent(int a0) { return gabi::call<BOOL>(0x0228A7D4, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialLetterEvent(int a0) { gabi::call(0x0228A800, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialMsgSetEvent(int a0) { gabi::call(0x0228A928, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::talk_init() { return gabi::call<BOOL>(0x0228AA7C, this); }
__attribute__((weak)) BOOL daNpc_Md_c::actionMsgSetEvent(int a0) { return gabi::call<BOOL>(0x0228AAF4, this, a0); }
__attribute__((weak)) int daNpc_Md_c::getAnmType(u8 a0) { return gabi::call<int>(0x0228AB40, this, a0); }
__attribute__((weak)) u16 daNpc_Md_c::next_msgStatus(be<u32>* a0) { return gabi::call<u16>(0x0228AB60, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::talk(int a0) { return gabi::call<BOOL>(0x0228B1B8, this, a0); }
__attribute__((weak)) void daNpc_Md_c::setHarpPlayNum(int a0) { gabi::call(0x0228B3CC, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialEndEvent(int a0) { gabi::call(0x0228B418, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionMsgEndEvent(int a0) { return gabi::call<BOOL>(0x0228B610, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialMovePosEvent(int a0) { gabi::call(0x0228B68C, this, a0); }
__attribute__((weak)) void daNpc_Md_c::particle_set(be<u32>* a0, u16 a1) { gabi::call(0x0228C104, this, a0, a1); }
__attribute__((weak)) void daNpc_Md_c::setWingEmitter() { gabi::call(0x0228C1B4, this); }
__attribute__((weak)) void daNpc_Md_c::initialFlyEvent(int a0) { gabi::call(0x0228C1C4, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionFlyEvent(int a0) { return gabi::call<BOOL>(0x0228C234, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialGlidingEvent(int a0) { gabi::call(0x0228C338, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionGlidingEvent(int a0) { return gabi::call<BOOL>(0x0228C354, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialLandingEvent(int a0) { gabi::call(0x0228C3E8, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionLandingEvent(int a0) { return gabi::call<BOOL>(0x0228C454, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialWalkEvent(int a0) { gabi::call(0x0228C4A4, this, a0); }
__attribute__((weak)) int daNpc_Md_c::wallHitCheck() { return gabi::call<int>(0x0228C4D0, this); }
__attribute__((weak)) BOOL daNpc_Md_c::actionWalkEvent(int a0) { return gabi::call<BOOL>(0x0228C50C, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionDashEvent(int a0) { return gabi::call<BOOL>(0x0228C898, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionTactEvent(int a0) { return gabi::call<BOOL>(0x0228CB88, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialTakeOffEvent(int a0) { gabi::call(0x0228CCC0, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionTakeOffEvent(int a0) { return gabi::call<BOOL>(0x0228CCF8, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialOnetimeEvent(int a0) { gabi::call(0x0228CD40, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionOnetimeEvent(int a0) { return gabi::call<BOOL>(0x0228CDFC, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialQuake(int a0) { gabi::call(0x0228CE84, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialHarpPlayEvent(int a0) { gabi::call(0x0228D0A4, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionHarpPlayEvent(int a0) { return gabi::call<BOOL>(0x0228D0AC, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialOffLinkEvent(int a0) { gabi::call(0x0228D0F0, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialOnLinkEvent(int a0) { gabi::call(0x0228D120, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialTurnEvent(int a0) { gabi::call(0x0228D150, this, a0); }
__attribute__((weak)) void daNpc_Md_c::lookBack(cXyz* a0, int a1, int a2) { gabi::call(0x0228D208, this, a0, a1, a2); }
__attribute__((weak)) BOOL daNpc_Md_c::actionTurnEvent(int a0) { return gabi::call<BOOL>(0x0228D32C, this, a0); }
__attribute__((weak)) void daNpc_Md_c::initialSetAnmEvent(int a0) { gabi::call(0x0228D564, this, a0); }
__attribute__((weak)) u32 daNpc_Md_c::initialLookDown(int a0) { return gabi::call<u32>(0x0228D840, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::actionLookDown(int a0) { return gabi::call<BOOL>(0x0228D848, this, a0); }
__attribute__((weak)) u32 daNpc_Md_c::initialLookUp(int a0) { return gabi::call<u32>(0x0228D948, this, a0); }
__attribute__((weak)) void daNpc_Md_c::setMessageAnimation(u8 a0) { gabi::call(0x0228DA10, this, a0); }
__attribute__((weak)) bool daNpc_Md_c::chkAttention(cXyz* a0, s16 a1, int a2) { return gabi::call<bool>(0x0228DAF4, this, a0, a1, a2); }
__attribute__((weak)) bool daNpc_Md_c::chkArea(cXyz* a0) { return gabi::call<bool>(0x0228DD60, this, a0); }
__attribute__((weak)) u32 daNpc_Md_c::getMsg() { return gabi::call<u32>(0x0228DDEC, this); }
__attribute__((weak)) s32 daNpc_Md_c::lookBackWaist(s16 a0, f32 a1) { return gabi::call<s32>(0x0228DFF8, this, a0, a1); }
__attribute__((weak)) void daNpc_Md_c::setHane02Emitter() { gabi::call(0x0228E2D4, this); }
__attribute__((weak)) void daNpc_Md_c::setHane03Emitter() { gabi::call(0x0228E328, this); }
__attribute__((weak)) void daNpc_Md_c::setNormalSpeedF(f32 a0, f32 a1, f32 a2, f32 a3, f32 a4) { gabi::call(0x0228E37C, this, a0, a1, a2, a3, a4); }
__attribute__((weak)) void daNpc_Md_c::setSpeedAndAngleNormal(f32 a0, s16 a1) { gabi::call(0x0228E4C0, this, a0, a1); }
__attribute__((weak)) void daNpc_Md_c::walkProc(f32 a0, s16 a1) { gabi::call(0x0228E5CC, this, a0, a1); }
__attribute__((weak)) s16 daNpc_Md_c::getStickAngY(int a0) { return gabi::call<s16>(0x0228E658, this, a0); }
__attribute__((weak)) int daNpc_Md_c::calcStickPos(s16 a0, cXyz* a1) { return gabi::call<int>(0x0228E6CC, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Md_c::flyCheck() { return gabi::call<BOOL>(0x0228E804, this); }
__attribute__((weak)) BOOL daNpc_Md_c::mirrorCancelCheck() { return gabi::call<BOOL>(0x0228E8AC, this); }
__attribute__((weak)) void daNpc_Md_c::NpcCall(be<s32>* a0) { gabi::call(0x0228E8D8, this, a0); }
__attribute__((weak)) void daNpc_Md_c::waitGroundCheck() { gabi::call(0x0228E9C0, this); }
__attribute__((weak)) BOOL daNpc_Md_c::chkAdanmaeDemoOrder() { return gabi::call<BOOL>(0x0228EACC, this); }
__attribute__((weak)) BOOL daNpc_Md_c::XYTalkCheck() { return gabi::call<BOOL>(0x0228EB50, this); }
__attribute__((weak)) BOOL daNpc_Md_c::waitNpcAction(void* a0) { return gabi::call<BOOL>(0x0228EB88, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::harpWaitNpcAction(void* a0) { return gabi::call<BOOL>(0x0228F424, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::talkNpcAction(void* a0) { return gabi::call<BOOL>(0x0228F610, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::shipTalkNpcAction(void* a0) { return gabi::call<BOOL>(0x0228FA44, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::kyohiNpcAction(void* a0) { return gabi::call<BOOL>(0x0228FC24, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::shipNpcAction(void* a0) { return gabi::call<BOOL>(0x0228FD20, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::mwaitNpcAction(void* a0) { return gabi::call<BOOL>(0x0228FF5C, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::squatdownNpcAction(void* a0) { return gabi::call<BOOL>(0x02290234, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::sqwait01NpcAction(void* a0) { return gabi::call<BOOL>(0x02290358, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::carryNpcAction(void* a0) { return gabi::call<BOOL>(0x022904DC, this, a0); }
__attribute__((weak)) s16 daNpc_Md_c::windProc() { return gabi::call<s16>(0x02290CF8, this); }
__attribute__((weak)) BOOL daNpc_Md_c::throwNpcAction(void* a0) { return gabi::call<BOOL>(0x02291094, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::glidingNpcAction(void* a0) { return gabi::call<BOOL>(0x02291304, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::fallNpcAction(void* a0) { return gabi::call<BOOL>(0x02291644, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::fall02NpcAction(void* a0) { return gabi::call<BOOL>(0x02291764, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::wallHitNpcAction(void* a0) { return gabi::call<BOOL>(0x022919D4, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::land01NpcAction(void* a0) { return gabi::call<BOOL>(0x02291BE4, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::land02NpcAction(void* a0) { return gabi::call<BOOL>(0x02291D50, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::land03NpcAction(void* a0) { return gabi::call<BOOL>(0x02291E50, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::piyo2NpcAction(void* a0) { return gabi::call<BOOL>(0x02291F50, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::deleteNpcAction(void* a0) { return gabi::call<BOOL>(0x0229216C, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::demoFlyNpcAction(void* a0) { return gabi::call<BOOL>(0x022921B4, this, a0); }
__attribute__((weak)) f32 daNpc_Md_c::checkForwardGroundY(s16 a0) { return gabi::call<f32>(0x0229249C, this, a0); }
__attribute__((weak)) f32 daNpc_Md_c::checkWallJump(s16 a0) { return gabi::call<f32>(0x02292650, this, a0); }
__attribute__((weak)) void daNpc_Md_c::routeAngCheck(cXyz* a0, be<s16>* a1) { gabi::call(0x022926D0, this, a0, a1); }
__attribute__((weak)) void daNpc_Md_c::routeWallCheck(cXyz* a0, cXyz* a1, be<s16>* a2) { gabi::call(0x0229278C, this, a0, a1, a2); }
__attribute__((weak)) BOOL daNpc_Md_c::routeCheck(f32 a0, be<s16>* a1) { return gabi::call<BOOL>(0x022928A8, this, a0, a1); }
__attribute__((weak)) BOOL daNpc_Md_c::searchNpcAction(void* a0) { return gabi::call<BOOL>(0x02292C48, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::npcAction_0229310C(void* a0) { return gabi::call<BOOL>(0x0229310C, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::hitNpcAction(void* a0) { return gabi::call<BOOL>(0x0229325C, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::jumpNpcAction(void* a0) { return gabi::call<BOOL>(0x022933A0, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::escapeNpcAction(void* a0) { return gabi::call<BOOL>(0x02293500, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::waitPlayerAction(void* a0) { return gabi::call<BOOL>(0x02293840, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::walkPlayerAction(void* a0) { return gabi::call<BOOL>(0x02293B5C, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::hitPlayerAction(void* a0) { return gabi::call<BOOL>(0x02293DDC, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::jumpPlayerAction(void* a0) { return gabi::call<BOOL>(0x02293FA8, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::flyPlayerAction(void* a0) { return gabi::call<BOOL>(0x02294160, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::landPlayerAction(void* a0) { return gabi::call<BOOL>(0x02294838, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::mkamaePlayerAction(void* a0) { return gabi::call<BOOL>(0x02294984, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::carryPlayerAction(void* a0) { return gabi::call<BOOL>(0x02294C00, this, a0); }
__attribute__((weak)) BOOL daNpc_Md_c::isTagCheckOK() { return gabi::call<BOOL>(0x0228D950, this); }
