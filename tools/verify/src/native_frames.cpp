/* Loader of build/verify/frames.tsv (tools/verify/frames_all.py) for gabi's automatic native
 * frames: gmem_frame_info(addr). Linked into the verification harness (included by harness.cpp)
 * and into a native/game-test build. The table is read on first use from $WWHD_FRAMES or
 * build/verify/frames.tsv (relative to the working directory); WWHD_NATIVE_FRAMES=0 disables the
 * automatic frames (gmem_frame_info returns null, the candidates behave as before 2026-10-05). */
#include "native_frames.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
struct Table {
    std::unordered_map<uint32_t, std::vector<GabiFrameItem>> items; /* node-stable: data() stays valid */
    std::unordered_map<uint32_t, GabiFrameInfo> info;
};
const char* table_path() {
    const char* path = getenv("WWHD_FRAMES");
    return path && *path ? path : "build/verify/frames.tsv";
}
Table* load() {
    Table* t = new Table;
    const char* off = getenv("WWHD_NATIVE_FRAMES");
    if (off && !strcmp(off, "0")) return t;
    FILE* f = fopen(table_path(), "r");
    if (!f) {
        fprintf(stderr, "[gabi] no frames table (%s): automatic native frames off\n", table_path());
        return t;
    }
    std::unordered_map<uint32_t, uint32_t> sizes, flags, objlo;
    char line[8192];
    while (fgets(line, sizeof line, f)) {
        unsigned addr, size;
        int pos = 0;
        if (sscanf(line, "%x %u%n", &addr, &size, &pos) < 2) continue;
        if (!size && !strstr(line + pos, "tail")) continue;
        std::vector<GabiFrameItem>& items = t->items[addr];
        for (char* tok = strtok(line + pos, " \t\n"); tok; tok = strtok(nullptr, " \t\n")) {
            GabiFrameItem it{};
            unsigned reg = 0, o = 0;
            if (!strcmp(tok, "b")) it.kind = GABI_FI_BACKCHAIN;
            else if (sscanf(tok, "l:%x", &o) == 1) it.kind = GABI_FI_LR;
            else if (sscanf(tok, "g:%u:%x", &reg, &o) == 2) it.kind = GABI_FI_GPR;
            else if (sscanf(tok, "d:%u:%x", &reg, &o) == 2) it.kind = GABI_FI_STFD;
            else if (sscanf(tok, "s0:%u:%x", &reg, &o) == 2) it.kind = GABI_FI_STFS0;
            else if (sscanf(tok, "s1:%u:%x", &reg, &o) == 2) it.kind = GABI_FI_STFS1;
            else if (sscanf(tok, "p:%u:%x", &reg, &o) == 2) it.kind = GABI_FI_PSQ;
            else if (sscanf(tok, "o:%x", &o) == 1) { objlo[addr] = o; continue; }
            else { if (!strcmp(tok, "tail")) flags[addr] |= GABI_FF_TAIL; continue; }
            it.reg = (uint8_t)reg;
            it.off = (uint16_t)o;
            items.push_back(it);
        }
        sizes[addr] = size;
    }
    fclose(f);
    for (auto& kv : t->items) t->info[kv.first] = GabiFrameInfo{sizes[kv.first], (uint32_t)kv.second.size(), kv.second.data(), flags[kv.first], objlo[kv.first]};
    return t;
}
Table* table() {
    static std::once_flag once;
    static Table* t;
    std::call_once(once, [] { t = load(); });
    return t;
}
}  // namespace

extern "C" const GabiFrameInfo* gmem_frame_info(uint32_t addr) {
    Table* t = table();
    auto it = t->info.find(addr);
    return it == t->info.end() ? nullptr : &it->second;
}
