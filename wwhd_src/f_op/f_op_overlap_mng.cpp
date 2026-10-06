#include "gabi.h"

int fopOvlpM_SceneIsStop() {
    WWHD_FUNC(0x025DBD24, int);
    u32 request = gmem_ld32(0x101F36CC);
    if (!request) return 0;
    u32 task = gmem_ld32(request + 0x20);
    u32 id = gmem_ld32(task + 0xCC);
    u32 scene = gabi::call<u32>(0x025DE50C, id);
    return gabi::call<int>(0x025DCA64, scene);
}
VERIFY(0x025DBD24, fopOvlpM_SceneIsStop);

int fopOvlpM_SceneIsStart() {
    WWHD_FUNC(0x025DBD74, int);
    u32 request = gmem_ld32(0x101F36CC);
    if (!request) return 0;
    u32 task = gmem_ld32(request + 0x20);
    u32 id = gmem_ld32(task + 0xCC);
    u32 scene = gabi::call<u32>(0x025DE50C, id);
    return gabi::call<int>(0x025DCAC4, scene);
}
VERIFY(0x025DBD74, fopOvlpM_SceneIsStart);

BOOL fopOvlpM_IsOutReq(u32 task) {
    WWHD_FUNC(0x025DBDC4, BOOL, task);
    return (gmem_ld8(task + 0xC8) & 0x3F) == 2;
}
VERIFY(0x025DBDC4, fopOvlpM_IsOutReq);

void fopOvlpM_Done(u32 task) {
    WWHD_FUNC(0x025DBDDC, void, task);
    gabi::call<void>(0x0201A330, task + 0xC8);
}
VERIFY(0x025DBDDC, fopOvlpM_Done);

void fopOvlpM_ToldAboutID(u32 id) {
    WWHD_FUNC(0x025DBDE4, void, id);
    u32 request = gmem_ld32(0x101F36CC);
    if (request) {
        u32 task = gmem_ld32(request + 0x20);
        gmem_st32(task + 0xCC, id);
    }
}
VERIFY(0x025DBDE4, fopOvlpM_ToldAboutID);

BOOL fopOvlpM_IsPeek() {
    WWHD_FUNC(0x025DBE00, BOOL);
    u32 request = gmem_ld32(0x101F36CC);
    return request ? gmem_ld32(request + 8) : 0;
}
VERIFY(0x025DBE00, fopOvlpM_IsPeek);

BOOL fopOvlpM_IsDone() {
    WWHD_FUNC(0x025DBE1C, BOOL);
    u32 request = gmem_ld32(0x101F36CC);
    return request ? gabi::call<BOOL>(0x0201A308, request) : 0;
}
VERIFY(0x025DBE1C, fopOvlpM_IsDone);

BOOL fopOvlpM_IsDoingReq() {
    WWHD_FUNC(0x025DBE38, BOOL);
    u32 request = gmem_ld32(0x101F36CC);
    return request && gmem_ld16(request + 4) == 1;
}
VERIFY(0x025DBE38, fopOvlpM_IsDoingReq);

BOOL fopOvlpM_ClearOfReq() {
    WWHD_FUNC(0x025DBE64, BOOL);
    u32 request = gmem_ld32(0x101F36CC);
    return request ? gabi::call<BOOL>(0x025DC4C4, request) : 0;
}
VERIFY(0x025DBE64, fopOvlpM_ClearOfReq);

u32 fopOvlpM_Request(u32 name, u32 peekTime, u32 extra) {
    WWHD_FUNC(0x025DBE80, u32, name, peekTime, extra);
    if (gmem_ld32(0x101F36CC)) return 0;
    u32 request = gabi::call<u32>(0x025DC344, 0x1048A55Cu, name, peekTime, extra);
    gmem_st32(0x101F36CC, request);
    return request;
}
VERIFY(0x025DBE80, fopOvlpM_Request);

void fopOvlpM_Management() {
    WWHD_FUNC(0x025DBEEC, void);
    u32 request = gmem_ld32(0x101F36CC);
    if (request) {
        u32 phase = gabi::call<u32>(0x025DC418, request);
        if (phase >= 3 && phase <= 5) gmem_st32(0x101F36CC, 0);
    }
}
VERIFY(0x025DBEEC, fopOvlpM_Management);

BOOL fopOvlpM_Cancel() {
    WWHD_FUNC(0x025DBF3C, BOOL);
    u32 request = gmem_ld32(0x101F36CC);
    if (!request) return 1;
    if (gabi::call<int>(0x025DC488, request) != 1) return 0;
    gmem_st32(0x101F36CC, 0);
    return 1;
}
VERIFY(0x025DBF3C, fopOvlpM_Cancel);

// Adjacent header-generated initializer, exact generating-header attribution qualified.
void fopOvlpM_StaticInit() {
    WWHD_FUNC(0x025DBFB8, void);
    gmem_st32(0x1048A554, 0);
    gmem_st32(0x1048A54C, 0);
    gmem_st32(0x1048A558, 0);
    gmem_st32(0x1048A550, 0);
    gabi::call<void>(0x028F026C, 0x101F36D0u);
    u32 lower = gmem_ld32(0x10057D1C);
    u32 upper = gmem_ld32(0x10057D20);
    gmem_stf32(0x1048A540, lower);
    gmem_stf32(0x1048A544, upper);
    gabi::call<void>(0x028ED6F8, 0x1048A548u);
    gabi::call<void>(0x028F026C, 0x101F36DCu);
    gabi::call<void>(0x028EAB2C, 0x1048A549u);
    gabi::call<void>(0x028F026C, 0x101F36E8u);
}
VERIFY(0x025DBFB8, fopOvlpM_StaticInit);

void fopOvlpM_Init() {
    WWHD_FUNC(0x025DC04C, void);
}
VERIFY(0x025DC04C, fopOvlpM_Init);
