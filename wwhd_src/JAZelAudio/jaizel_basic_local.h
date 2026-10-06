/* JAIZelBasic: layout notes and helpers shared by the parts of the unit (JAIZelBasic*.cpp). WWHD.
 * Included inside each part's namespace after jaizel_local.h.
 *
 * HD layout of JAIZelBasic (from the code; GameCube offset in parentheses):
 *   +0x00 audio camera array pointer, +0x04 heap (GC 0x08)
 *   +0x30 (0x20) menu flag, +0x34 (0x24) event bits pointer, +0x38 (0x28)
 *   +0x55/+0x56/+0x57 (0x45/0x46/0x47), +0x64 (0x54) Vec, +0x73..+0x76 (0x63..0x66)
 *   +0x78 mpMainBgmSound (0x68), +0x7C mpSubBgmSound (0x6C), +0x80 mpStreamBgmSound (0x70)
 *   +0x84 mSubBgmNum (0x74), +0x88 mMainBgmNum (0x78), +0x8C mStreamBgmNum (0x7C)
 *   +0x90..+0xBC the bgm volume factors (0x80..0xAC): main = 90*94*98*9C*A0*A4*A8*AC*BC, sub = B0*B4*B8
 *   +0xC0 (0xB0), +0xC4..+0xDE flags (0xB4..0xCE)
 *   +0xE4 mpSeSound[32] (0xD4 [24]), +0x164 mSeNum[32] (0x134), +0x1E4 [32] (0x194)
 *   +0x268.. (0x1F8..) state bytes: +0x271 (0x201), +0x276 (0x206), +0x277 (0x207), +0x294 (0x224) scene ...
 *   +0x20AC kurobo motion counter, +0x20B0 kurobo handles [4]
 *   +0x20E8 HD allocation records [16] {count, JAIZelSound*}, +0x2168 their index */

static inline f32 mul(f32 a, f32 b) { return gabi::fmuls_ppc(a, b); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JAIBasic_startSoundVec_l(u32 b, u32 id, u32 handle, u32 pos, u32 a, u32 c, u32 d) { gabi::call(0x02802848, b, id, handle, pos, a, c, d); }
static inline void JAIBasic_setSeCategoryVolume_l(u32 b, u32 cat, u32 vol) { gabi::call(0x02803138, b, cat, vol); }
static inline void JAISound_setSeqInterVolume_l(u32 s, u32 a, f32 v, u32 fade) { gabi::call(0x0280B2AC, s, a, fade, v); }
static inline void JAISound_setTrackVolume_l(u32 s, u32 track, f32 v, u32 fade) { gabi::call(0x0280C9E4, s, track, fade, v); }
static inline u32 JAISound_getSeqParameter_l(u32 s) { return gabi::call<u32>(0x0280B05C, s); }
static inline void TTrack_writePortApp_l(u32 t, u32 a, u32 v) { gabi::call(0x02818C20, t, a, v); }
static inline void PSVECSubtract_l(u32 a, u32 b, u32 out) { gabi::call(0x028E8DAC, a, b, out); }
static inline f32 PSVECMag_l(u32 v) { return gabi::call<f32>(0x028E8E10, v); }
/* HD sound manager (*1018EC64): 0202BF78 returns its stream player, 0202FD40 asks it about a stream id */
static inline u32 hdsnd_player_l(u32 mgr) { return gabi::call<u32>(0x0202BF78, mgr); }
static inline s32 hdsnd_check_l(u32 p, u32 id, u32 a, u32 b, u32 c) { return gabi::call<s32>(0x0202FD40, p, id, a, b, c); }
static inline u32 JAIZelSound_ct_l(u32 p) { return gabi::call<u32>(0x0202ACCC, p); }
/* the default heap getter (sead) used when no heap pointer is set */
static inline u32 heap_get_l(u32 a) { return gabi::call<u32>(0x02756140, a); }
