/* WWHD node requests, HD TU025E0330..025E0B1F (16 entries).
 * GC queue admission/removal and three request constructors are inlined in HD.
 * Constant delete-timing phase emitted after the TU initializer. */
#include "bindings.h"
namespace f_pc_node_req_cpp {
static s32 phase_IsCreated(u32 req) {
    WWHD_FUNC(0x025E0330, s32, req);
    if (gabi::call<s32>(0x025DD868, gabi::load<u32>(req + 0x54)) == 1) return 0;
    return gabi::call<s32>(0x025DE564, gabi::load<u32>(req + 0x54)) == 1 ? 2 : 3;
}
VERIFY(0x025E0330, phase_IsCreated);
static s32 phase_Create(u32 req) {
    WWHD_FUNC(0x025E039C, s32, req);
    u32 methods = gabi::load<u32>(req + 0x3C);
    s32 id = gabi::call<s32>(0x025E14A8, gabi::load<u32>(req + 0x50), gabi::load<s16>(req + 0x58), gabi::load<u32>(methods + 0xC), req, gabi::load<u32>(req + 0x5C));
    gabi::store<u32>(req + 0x54, id);
    return id == -1 ? 3 : 2;
}
VERIFY(0x025E039C, phase_Create);
static s32 phase_IsDeleted(u32 req) {
    WWHD_FUNC(0x025E03F8, s32, req);
    return gabi::call<s32>(0x025DDFA4) == 0 ? 0 : 2;
}
VERIFY(0x025E03F8, phase_IsDeleted);
static s32 phase_Delete(u32 req) {
    WWHD_FUNC(0x025E0424, s32, req);
    u32 node = gabi::load<u32>(req + 0x48);
    if (node != 0) {
        if (gabi::call<s32>(0x025DE1B8, node) == 0) return 0;
        gabi::store<u32>(req + 0x48, 0);
    }
    return 2;
}
VERIFY(0x025E0424, phase_Delete);
static s32 DoPhase(u32 req) {
    WWHD_FUNC(0x025E0488, s32, req);
    s32 result;
    do {
        result = gabi::call<s32>(0x0201A0AC, req + 0x30, gabi::load<u32>(req + 0x38), req);
    } while (result == 2);
    return result;
}
VERIFY(0x025E0488, DoPhase);
static u32 Execute(u32 req) {
    WWHD_FUNC(0x025E04D0, u32, req);
    u32 result = gabi::call<u32>(0x025E0488, req);
    if (result <= 1) return 0;
    if (result >= 3 && result <= 5) return gabi::load<u8>(0x10058401 + result);
    return result;
}
VERIFY(0x025E04D0, Execute);
static s32 Delete(u32 req) {
    WWHD_FUNC(0x025E0538, s32, req);
    gabi::call<void>(0x025DEA90, gabi::load<u32>(req + 0x50));
    gabi::call<void>(0x025DE9E0, req + 0x14);
    gabi::call<void>(0x0201A868, req);
    u32 methods = gabi::load<u32>(req + 0x3C);
    if (methods != 0) {
        u32 method = gabi::load<u32>(methods + 8);
        if (method != 0 && gabi::call<s32>(0x025DFCAC, method, req) == 0) return 0;
    }
    gabi::call<void>(0x0201945C, req);
    return 1;
}
VERIFY(0x025E0538, Delete);
static s32 Cancel(u32 req) {
    WWHD_FUNC(0x025E05C4, s32, req);
    u32 methods = gabi::load<u32>(req + 0x3C);
    if (methods != 0 && gabi::call<s32>(0x025DFCAC, gabi::load<u32>(methods + 4), req) == 0) return 0;
    return gabi::call<s32>(0x025E0538, req);
}
VERIFY(0x025E05C4, Cancel);
static s32 Handler() {
    WWHD_FUNC(0x025E062C, s32, (u32)0);
    u32 node = gabi::load<u32>(0x101F3DA4);
    while (node != 0) {
        u32 req = gabi::load<u32>(node + 0xC);
        u32 methods = gabi::load<u32>(req + 0x3C);
        u32 result = gabi::call_ptr<u32>(gabi::load<u32>(methods), req);
        node = gabi::load<u32>(node + 8);
        if (result == 3 || result == 5) {
            if (gabi::call<s32>(0x025E05C4, req) == 0) return 0;
        } else if (result == 4) {
            if (gabi::call<s32>(0x025E0538, req) == 0) return 0;
        }
    }
    return 1;
}
VERIFY(0x025E062C, Handler);
static s32 IsPossibleTarget(u32 proc) {
    WWHD_FUNC(0x025E06EC, s32, proc);
    u32 id = gabi::load<u32>(proc + 4);
    u32 node = gabi::load<u32>(0x101F3DA4);
    while (node != 0) {
        u32 req = gabi::load<u32>(node + 0xC);
        s32 type = gabi::load<s32>(req + 0x40);
        if ((type == 2 || type == 4 || type == 1) && gabi::load<u32>(req + 0x4C) == id) return 0;
        node = gabi::load<u32>(node + 8);
    }
    return 1;
}
VERIFY(0x025E06EC, IsPossibleTarget);
static s32 IsIng(u32 proc) {
    WWHD_FUNC(0x025E0750, s32, proc);
    u32 id = gabi::load<u32>(proc + 4);
    u32 node = gabi::load<u32>(0x101F3DA4);
    while (node != 0) {
        u32 req = gabi::load<u32>(node + 0xC);
        if (gabi::load<u32>(req + 0x54) == id) return 1;
        node = gabi::load<u32>(node + 8);
    }
    return 0;
}
VERIFY(0x025E0750, IsIng);
static u32 Create(u32 size) {
    WWHD_FUNC(0x025E0798, u32, size);
    u32 req = gabi::call<u32>(0x02019430, -4, size);
    if (req != 0) {
        gabi::call<void>(0x0201B680, req, size);
        for (u32 i = 0; i < 25; i++) gabi::store<u32>(req + i * 4, gabi::load<u32>(0x101F3DC0 + i * 4));
        gabi::call<void>(0x0201A918, req, req);
        gabi::call<void>(0x025DFE2C, req + 0x14, 0x025E05C4u, req);
        u32 id = gabi::load<u32>(0x101F3DBC);
        gabi::store<u32>(0x101F3DBC, id + 1);
        gabi::store<u32>(req + 0x44, id);
    }
    return req;
}
VERIFY(0x025E0798, Create);
static u32 Request(u32 size, u32 type, u32 proc, s16 name, u32 data, u32 methods) {
    WWHD_FUNC(0x025E0840, u32, size, type, proc, name, data, methods);
    u32 req = 0;
    if (type == 0) {
        u32 layer = gabi::call<u32>(0x025DED64);
        if (gabi::load<u32>(layer + 0xC) != 0 && gabi::call<s32>(0x025E06EC, gabi::load<u32>(layer + 0x18)) == 0) return 0;
        req = gabi::call<u32>(0x025E0798, size);
        if (req != 0) {
            gabi::store<u32>(req + 0x38, 0x101F3DB0);
            if (gabi::load<u32>(layer + 0xC) != 0) {
                gabi::store<u32>(req + 0x48, gabi::load<u32>(layer + 0x18));
                gabi::store<u32>(req + 0x4C, gabi::load<u32>(gabi::load<u32>(layer + 0x18) + 4));
            }
            gabi::store<s16>(req + 0x58, name);
            gabi::store<u32>(req + 0x50, layer);
            gabi::store<u32>(req + 0x5C, data);
        }
    } else if (type == 1 || type == 2) {
        if (gabi::call<s32>(0x025E06EC, proc) != 1) return 0;
        if (gabi::call<s32>(0x025E0750, proc) != 0) return 0;
        req = gabi::call<u32>(0x025E0798, size);
        if (req != 0) {
            gabi::store<u32>(req + 0x48, proc);
            gabi::store<u32>(req + 0x38, type == 1 ? 0x101F3D94u : 0x101F3D7Cu);
            gabi::store<u32>(req + 0x4C, gabi::load<u32>(proc + 4));
            u32 layer = gabi::load<u32>(proc + 0x2C);
            if (type == 2) gabi::store<s16>(req + 0x58, name);
            gabi::store<u32>(req + 0x50, layer);
            if (type == 2) gabi::store<u32>(req + 0x5C, data);
        }
    } else {
        gabi::call<void>(0x0273AA24, 0x1005840Cu, 825, 0x10058407u);
        return 0;
    }
    if (req != 0) {
        gabi::store<u32>(req + 0x40, type);
        gabi::store<u32>(req + 0x3C, methods);
        gabi::call<void>(0x0201A8B4, 0x101F3DA4u, req);
        gabi::call<void>(0x025DE9E4, gabi::load<u32>(req + 0x50), req + 0x14);
        gabi::call<void>(0x025DEA80, gabi::load<u32>(req + 0x50));
    }
    return req;
}
VERIFY(0x025E0840, Request);
static s32 ReChangeNode(u32 id, s16 name, u32 data) {
    WWHD_FUNC(0x025E0A20, s32, id, name, data);
    u32 node = gabi::load<u32>(0x101F3DA4);
    while (node != 0) {
        u32 req = gabi::load<u32>(node + 0xC);
        if (gabi::load<s32>(req + 0x40) == 2 && gabi::load<u32>(req + 0x44) == id) {
            if (gabi::load<s32>(req + 0x54) != -2) return 0;
            gabi::store<s16>(req + 0x58, name);
            gabi::store<u32>(req + 0x5C, data);
            return 1;
        }
        node = gabi::load<u32>(node + 8);
    }
    return 0;
}
VERIFY(0x025E0A20, ReChangeNode);
static void __sinit_node_req() {
    WWHD_FUNC(0x025E0A84, void, (u32)0);
    sinit_header_statics(0x1048AAEC, 0x101F3E24);
}
VERIFY(0x025E0A84, __sinit_node_req);
static s32 phase_IsDeleteTiming(u32 req) {
    WWHD_FUNC(0x025E0B18, s32, req);
    return 2;
}
VERIFY(0x025E0B18, phase_IsDeleteTiming);
}
