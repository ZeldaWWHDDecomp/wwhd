/* blobfile: a file inside the user's game files, optionally inside Yaz0 / SARC / Wii U BFRES containers (2026-10-27, AGL#3
 * FilterAa parameter archive request). A chain `FILE[:STEP...]` starts at game/FILE; each STEP descends into the current bytes:
 *   <yaz0>  Yaz0-decode the current bytes (an explicit step, so a chain can end on the decoded or the stored bytes);
 *   NAME    an entry of the current SARC (by name; SFAT entries without a name match by the SARC name hash) or an embedded
 *           file of the current BFRES (Wii U FRES, index group 11 "external files"); Yaz0 bytes are decoded first.
 * The Yaz0 decoder follows the format the game's own decoder reads (sead SZS: 0275ED14 -> 0275EBC0): a 16-byte header with
 * the big-endian decoded size at +4, then groups of one code byte (MSB first) where 1 = literal byte and 0 = back reference
 * (two bytes NR RR: distance RRR + 1; length N + 2, or the next byte + 0x12 when N = 0). Self-test blobfile_test.py compares
 * this output with the bytes the original 0275ED14 writes. */
#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace blobfile {

static inline uint32_t be32(const std::vector<uint8_t>& d, size_t o) {
    return (uint32_t)d[o] << 24 | (uint32_t)d[o + 1] << 16 | (uint32_t)d[o + 2] << 8 | d[o + 3];
}
static inline uint16_t be16(const std::vector<uint8_t>& d, size_t o) { return (uint16_t)(d[o] << 8 | d[o + 1]); }
static inline uint32_t le32(const std::vector<uint8_t>& d, size_t o) {
    return (uint32_t)d[o + 3] << 24 | (uint32_t)d[o + 2] << 16 | (uint32_t)d[o + 1] << 8 | d[o];
}
static inline uint16_t le16(const std::vector<uint8_t>& d, size_t o) { return (uint16_t)(d[o + 1] << 8 | d[o]); }

/* returns false (with *err) on a malformed stream */
static bool yaz0(const std::vector<uint8_t>& in, std::vector<uint8_t>* out, std::string* err) {
    if (in.size() < 16 || memcmp(in.data(), "Yaz0", 4)) { *err = "not Yaz0 data"; return false; }
    uint32_t n = be32(in, 4);
    if (n > 0x4000000u) { *err = "Yaz0 decoded size over 64 MB"; return false; }
    out->assign(n, 0);
    size_t s = 16, d = 0;
    while (d < n) {
        if (s >= in.size()) { *err = "Yaz0 stream ends early"; return false; }
        uint8_t code = in[s++];
        for (int bit = 0; bit < 8 && d < n; bit++) {
            if (code & (0x80 >> bit)) {
                if (s >= in.size()) { *err = "Yaz0 stream ends early"; return false; }
                (*out)[d++] = in[s++];
                continue;
            }
            if (s + 1 >= in.size()) { *err = "Yaz0 stream ends early"; return false; }
            uint32_t b1 = in[s], b2 = in[s + 1];
            s += 2;
            uint32_t dist = ((b1 & 0xF) << 8 | b2) + 1, len = b1 >> 4;
            if (len == 0) {
                if (s >= in.size()) { *err = "Yaz0 stream ends early"; return false; }
                len = in[s++] + 0x12u;
            } else {
                len += 2;
            }
            if (dist > d) { *err = "Yaz0 back reference before the start"; return false; }
            for (uint32_t k = 0; k < len && d < n; k++, d++) (*out)[d] = (*out)[d - dist];
        }
    }
    return true;
}

/* SARC (either byte order): entry NAME -> bytes */
static bool sarc(const std::vector<uint8_t>& s, const std::string& name, std::vector<uint8_t>* out, std::string* err) {
    if (s.size() < 0x20 || memcmp(s.data(), "SARC", 4)) { *err = "not a SARC archive"; return false; }
    bool big = s[6] == 0xFE && s[7] == 0xFF;
    auto r32 = [&](size_t o) { return big ? be32(s, o) : le32(s, o); };
    auto r16 = [&](size_t o) { return big ? be16(s, o) : le16(s, o); };
    uint32_t hdr = r16(4), data = r32(0xC);
    size_t sfat = hdr;
    if (sfat + 0xC > s.size() || memcmp(s.data() + sfat, "SFAT", 4)) { *err = "SARC without SFAT"; return false; }
    uint32_t count = r16(sfat + 6), key = r32(sfat + 8);
    size_t nodes = sfat + 0xC, sfnt = nodes + (size_t)count * 16;
    if (sfnt + 8 > s.size() || memcmp(s.data() + sfnt, "SFNT", 4)) { *err = "SARC without SFNT"; return false; }
    size_t names = sfnt + r16(sfnt + 4);
    uint32_t hash = 0;
    for (unsigned char ch : name) hash = hash * key + ch;
    for (uint32_t i = 0; i < count; i++) {
        size_t e = nodes + (size_t)i * 16;
        uint32_t h = r32(e), attr = r32(e + 4), b = r32(e + 8), en = r32(e + 12);
        bool match;
        if (attr >> 24) {
            size_t no = names + (size_t)(attr & 0xFFFF) * 4, z = no;
            while (z < s.size() && s[z]) z++;
            match = z < s.size() && std::string((const char*)s.data() + no, z - no) == name;
        } else {
            match = h == hash;
        }
        if (!match) continue;
        if (en < b || (size_t)data + en > s.size()) { *err = "SARC entry outside the archive"; return false; }
        out->assign(s.begin() + data + b, s.begin() + data + en);
        return true;
    }
    *err = "no SARC entry " + name;
    return false;
}

/* Wii U BFRES (FRES, big endian, self-relative s32 offsets): embedded file NAME (index group 11) -> bytes */
static bool fres(const std::vector<uint8_t>& b, const std::string& name, std::vector<uint8_t>* out, std::string* err) {
    if (b.size() < 0x68 || memcmp(b.data(), "FRES", 4) || b[8] != 0xFE || b[9] != 0xFF) { *err = "not a Wii U BFRES"; return false; }
    int32_t go = (int32_t)be32(b, 0x20 + 11 * 4);
    if (!go) { *err = "BFRES without embedded files"; return false; }
    size_t g = (size_t)(0x20 + 11 * 4 + (int64_t)go);
    if (g + 8 > b.size()) { *err = "BFRES index group outside the file"; return false; }
    int32_t n = (int32_t)be32(b, g + 4);
    for (int32_t i = 1; i <= n; i++) { /* entry 0 is the root of the Patricia tree */
        size_t e = g + 8 + (size_t)i * 16;
        if (e + 16 > b.size()) { *err = "BFRES index entry outside the file"; return false; }
        size_t np = (size_t)(e + 8 + (int64_t)(int32_t)be32(b, e + 8)), z = np;
        while (z < b.size() && b[z]) z++;
        if (z >= b.size() || std::string((const char*)b.data() + np, z - np) != name) continue;
        size_t dp = (size_t)(e + 12 + (int64_t)(int32_t)be32(b, e + 12));
        if (dp + 8 > b.size()) { *err = "BFRES file record outside the file"; return false; }
        size_t fo = (size_t)(dp + (int64_t)(int32_t)be32(b, dp));
        uint32_t fs = be32(b, dp + 4);
        if (fo + fs > b.size()) { *err = "BFRES embedded file outside the file"; return false; }
        out->assign(b.begin() + fo, b.begin() + fo + fs);
        return true;
    }
    *err = "no BFRES embedded file " + name;
    return false;
}

/* one STEP of a chain */
static bool step(std::vector<uint8_t>* cur, const std::string& st, std::string* err) {
    std::vector<uint8_t> next;
    if (st == "<yaz0>") {
        if (!yaz0(*cur, &next, err)) return false;
        *cur = std::move(next);
        return true;
    }
    if (cur->size() >= 4 && !memcmp(cur->data(), "Yaz0", 4)) {
        if (!yaz0(*cur, &next, err)) return false;
        *cur = std::move(next);
    }
    if (cur->size() >= 4 && !memcmp(cur->data(), "SARC", 4)) {
        if (!sarc(*cur, st, &next, err)) return false;
    } else if (cur->size() >= 4 && !memcmp(cur->data(), "FRES", 4)) {
        if (!fres(*cur, st, &next, err)) return false;
    } else {
        *err = "step " + st + ": the current bytes are not Yaz0, SARC or BFRES";
        return false;
    }
    *cur = std::move(next);
    return true;
}

}  // namespace blobfile
