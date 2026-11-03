/* Port-mirroring answers (`osmem ... portconst`). Each import
 * answers as the port's HLE at f5554821 does, r3 only; portans_equiv.py compiles those lines and checks this table.
 * An import with no HLE in the port takes core.cpp hle_unimplemented: r3 = 0. A stub is slot - the library's delta
 * (measured as strictmocks_test.py 6 does; portans_equiv.py re-measures it). */
#pragma once
#include <stdint.h>

namespace portans {

struct Constant { uint32_t slot, stub_delta, r3; const char* name; };
/* PORTANS_TABLE_BEGIN */
static const Constant kConstants[] = {
    {0xC0006168u, 0xBD70BC88u, 1u, "gx2 GX2DrawDone"},                       /* gx2/gx2_core.cpp:1363-1368; waits for the host GPU: no guest time */
    {0xC0007CA0u, 0xBD70BD28u, 0xA0000000u, "nn_ac Connect"},                /* hle/system_stubs.cpp:33 kResultFail */
    {0xC0007E48u, 0xBD70BD28u, 0u, "nn_ac Initialize"},                      /* hle/system_stubs.cpp:31 */
    {0xC0004C98u, 0xBD70BBF0u, 0xA0000000u, "nn_olv Initialize"},            /* hle/system_stubs.cpp:54 kResultFail */
    {0xC0005A30u, 0xBD70BC50u, 0u, "nlibcurl curl_global_init_mem"},         /* hle/system_stubs.cpp:73 */
    {0xC0005BD8u, 0xBD70BC50u, 0u, "nsysnet NSSLInit"},                      /* hle/system_stubs.cpp:71 */
    {0xC0006028u, 0xBD70BC50u, 0u, "nsysnet socket_lib_init"},               /* hle/system_stubs.cpp:69 */
    {0xC0009010u, 0xBD70BD88u, 0u, "coreinit IMDisableDim"},                 /* hle/coreinit_misc.cpp:334 */
    {0xC0009B18u, 0xBD70BD88u, 1u, "coreinit OSEnableHomeButtonMenu"},       /* hle/coreinit_misc.cpp:332 */
    {0xC0009C58u, 0xBD70BD88u, 0u, "coreinit OSGetSharedData"},              /* hle/coreinit_misc.cpp:329; no memory effect */
    {0xC0004E10u, 0xBD70BBF0u, 0u, "nn_olv SetReportTypes"},                 /* no HLE: hle_unimplemented */
    {0xC0004E90u, 0xBD70BBF0u, 0u, "nn_olv InitializeParam::SetWork"},       /* no HLE: hle_unimplemented */
    {0xC0004F00u, 0xBD70BBF0u, 0u, "nn_olv InitializeParam ctor"},           /* no HLE: hle_unimplemented */
};
/* PORTANS_TABLE_END */
/* `osmem ... portmem` (needs `threads`: the clock): argument-derived answers and memory effects at f5554821, r3 and guest memory
 * only. GX2GPUTimeToCPUTime keeps r3/r4 (gx2/gx2_core.cpp:1599 ret64(arg64(c, 3))); GX2SampleTop/BottomGPUCycle store the guest
 * clock at r3 when nonzero (:1600-1601, timebase::guest_now, here the thread model's clock); IMIsDimEnabled stores 0 at r3 when
 * nonzero, r3 = 0 (hle/coreinit_misc.cpp:336); GX2SetSurfaceSwizzle (gx2/gx2_resources.cpp:24-27): +0x34 byte 2 from r4;
 * GX2SetClearDepthStencil (gx2/gx2_core.cpp:1324-1328): (float)f1 at +0x88, r4 & 0xFF at +0x8C; GX2InitDepthBufferHiZEnable no
 * effect (gx2/gx2_resources.cpp:31); SAVEOpenFile (hle/fs.cpp:452-454, open_file :235-237): a read of a missing save: FS_NOT_FOUND,
 * nothing stored; a write or update mode is a model rejection (the port would create the file; no /vol/save input exists).
 * E: uint8_t ld8(a); uint32_t ld32(a); void st32(a, v); void st64(a, v); uint64_t now(); [[noreturn]] void reject(why). */
enum : uint32_t {
    GX2GPUTimeToCPUTime = 0xC00061B8u, GX2SampleBottomGPUCycle = 0xC0006748u, GX2SampleTopGPUCycle = 0xC0006760u,
    GX2SetSurfaceSwizzle = 0xC00069B8u, GX2SetClearDepthStencil = 0xC00067C0u, GX2InitDepthBufferHiZEnable = 0xC00063E8u,
    IMIsDimEnabled = 0xC0009060u, SAVEOpenFile = 0xC0007490u,
};
static const uint32_t kDeltaGx2 = 0xBD70BC88u, kDeltaCoreinit = 0xBD70BD88u, kDeltaSave = 0xBD70BCB0u;
static const int32_t kFsNotFound = -6; /* hle/fs.cpp FS_NOT_FOUND */
inline uint32_t mem_slot(uint32_t t) {
    for (uint32_t s : {GX2GPUTimeToCPUTime, GX2SampleBottomGPUCycle, GX2SampleTopGPUCycle, GX2SetSurfaceSwizzle, GX2SetClearDepthStencil, GX2InitDepthBufferHiZEnable})
        if (t == s || t == s - kDeltaGx2) return s;
    if (t == IMIsDimEnabled || t == IMIsDimEnabled - kDeltaCoreinit) return IMIsDimEnabled;
    if (t == SAVEOpenFile || t == SAVEOpenFile - kDeltaSave) return SAVEOpenFile;
    return 0;
}
/* one call: 1 = r3 is *r, 0 = r3 (and r4) kept. a = r0..r10; f1 = the first float argument */
template <class E> int mem_call(E& e, uint32_t slot, const uint32_t* a, double f1, uint32_t* r) {
    switch (slot) {
    case GX2SampleTopGPUCycle: case GX2SampleBottomGPUCycle: if (a[3]) e.st64(a[3], e.now()); return 0;
    case IMIsDimEnabled: if (a[3]) e.st32(a[3], 0); *r = 0; return 1;
    case GX2SetSurfaceSwizzle: e.st32(a[3] + 0x34, (e.ld32(a[3] + 0x34) & 0xFFFF00FFu) | ((a[4] & 0xFFu) << 8)); return 0;
    case GX2SetClearDepthStencil: {
        const float d = (float)f1;
        uint32_t bits;
        __builtin_memcpy(&bits, &d, 4);
        e.st32(a[3] + 0x88, bits);
        e.st32(a[3] + 0x8C, a[4] & 0xFFu);
        return 0;
    }
    case SAVEOpenFile: {
        if (a[7]) for (uint32_t i = 0;; i++) {
            if (i >= 256) e.reject("osmem portmem: SAVEOpenFile mode longer than 256 bytes");
            const uint8_t ch = e.ld8(a[7] + i);
            if (!ch) break;
            if (ch == 'w' || ch == 'a' || ch == '+') e.reject("osmem portmem: SAVEOpenFile write or update mode (no save input: not modelled)");
        }
        *r = (uint32_t)kFsNotFound;
        return 1;
    }
    default: return 0; /* GX2GPUTimeToCPUTime, GX2InitDepthBufferHiZEnable */
    }
}
inline const Constant* constant(uint32_t t) {
    for (const Constant& k : kConstants) if (t == k.slot || t == k.slot - k.stub_delta) return &k;
    return nullptr;
}

}  // namespace portans
