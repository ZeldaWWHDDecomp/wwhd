/* Background registry: GameCube names and unchanged HD sizes (0x14 element, 0x1404 registry).
 * Reuses the verified mesh layout. The relocation view adds the DZB tail omitted by that view. */
#pragma once
#include "SSystem/SComponent/c_bg_w.h"
#include "SSystem/SComponent/c_bg_s_gnd_chk.h"
struct cBgD_Reloc : cBgD_Mesh {
    /* 0x28 */ be<s32> m_ti_num;
    /* 0x2C */ gptr<void> m_ti_tbl;
    /* 0x30 */ be<u32> flag;
    enum : u32 { RELOCATED = 0x80000000 };
};
class cBgS_ChkElm {
  public:
    enum { USED = 1 };
    void Init();
    void Regist2(cBgWMesh *, u32, void *);
    void Release();
    /* 0x00 */ gptr<cBgWMesh> m_bgw_base_ptr;
    /* 0x04 */ be<u32> m_flags;
    /* 0x08 */ be<u32> m_actor_id;
    /* 0x0C */ gptr<void> m_actor_ptr;
    /* 0x10 */ be<u32> __vtbl;
};
class cBgS_LinChk : public cBgS_Chk, public cBgS_PolyInfo {
  public:
    enum { HIT_e = 0x10 };
    /* 0x24 */ cBgW_Line mLin;
    /* 0x40 */ cXyz mWork;
    /* 0x4C */ be<u32> mFlag;
    /* 0x50 */ be<u8> mPreWallChk, mPreGroundChk, mPreRoofChk, mFrontFlag, mBackFlag;
    u8 tail[3];
};
class cBgS {
  public:
    enum { MAX_BG = 256, INVALID_POLY = 0xFFFF };
    bool Regist(cBgWMesh *, u32, void *);
    bool Release(cBgWMesh *);
    void Ct();
    void Dt();
    bool LineCross(cBgS_LinChk *);
    f32 GroundCross(cBgS_GndChk *);
    void *GetActorPointer(u32);
    cBgWMesh *GetBgWPointer(cBgS_PolyInfo *);
    bool ChkPolySafe(cBgS_PolyInfo *);
    s32 GetTriGrp(u32, s32);
    s32 GetGrpToRoomId(u32, u32);
    cBgW_Plane *GetTriPla(u32, s32);
    void GetTriPnt(cBgS_PolyInfo *, cXyz *, cXyz *, cXyz *);
    void Move();
    s32 GetGrpInf(cBgS_PolyInfo *, s32);
    /* 0x0000 */ cBgS_ChkElm m_chk_element[MAX_BG];
    /* 0x1400 */ be<u32> __vtbl;
};
struct BgSRoomVtable {
    u8 prefix[0x14];
    be<u32> GetGrpToRoomIndex;
};
WWHD_SIZE(cBgD_Reloc, 0x34);
WWHD_SIZE(cBgS_ChkElm, 0x14);
WWHD_OFFSET(cBgS_LinChk, mFlag, 0x4C);
WWHD_OFFSET(cBgS_LinChk, mPreWallChk, 0x50);
WWHD_SIZE(cBgS_LinChk, 0x58);
WWHD_SIZE(cBgS, 0x1404);
void *operator_new(u32);
using BgSElementCtor = cBgS_ChkElm *(*)(cBgS_ChkElm *);
void constructElements(cBgS *, u32, u32, BgSElementCtor);
// HD: these callers consume the full r3, without extending the callees' u8 declarations.
s32 cBgW_ChkMemoryError(cBgWMesh *);
s32 cBgS_ChkSameActorPid(cBgS_Chk *, u32);
s32 cBgW_LineCheckGrpRp(cBgWMesh *, cBgS_LinChk *, u32, u32);
s32 cBgW_GroundCrossGrpRp(cBgWMesh *, cBgS_GndChk *, u32, u32);
void cBgW_GetTriPnt(cBgWMesh *, u32, cXyz *, cXyz *, cXyz *);
void assertion(const char *, u32, const char *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
