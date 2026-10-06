/* hd_sinit_02031A98: 27 initialiser-only HD TUs between the sound interface and the screen-dimming TU (no GameCube
 * source). Each TU 02031A98..02032A33 consists of its 148-byte header __sinit only (no other code): probably
 * HD sound data TUs whose tables are plain .data.
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sinit_02031A98 {

static void sinit_02031A98() {
    WWHD_FUNC(0x02031A98, void);
    header_sinit(0x102004B0, 0x1018EE74, 0x10004978);
}
VERIFY(0x02031A98, sinit_02031A98);

static void sinit_02031B2C() {
    WWHD_FUNC(0x02031B2C, void);
    header_sinit(0x102004CC, 0x1018EE98, 0x10004980);
}
VERIFY(0x02031B2C, sinit_02031B2C);

static void sinit_02031BC0() {
    WWHD_FUNC(0x02031BC0, void);
    header_sinit(0x102004E8, 0x1018EEBC, 0x10004988);
}
VERIFY(0x02031BC0, sinit_02031BC0);

static void sinit_02031C54() {
    WWHD_FUNC(0x02031C54, void);
    header_sinit(0x10200504, 0x1018EEE0, 0x10004990);
}
VERIFY(0x02031C54, sinit_02031C54);

static void sinit_02031CE8() {
    WWHD_FUNC(0x02031CE8, void);
    header_sinit(0x10200520, 0x1018EF04, 0x10004998);
}
VERIFY(0x02031CE8, sinit_02031CE8);

static void sinit_02031D7C() {
    WWHD_FUNC(0x02031D7C, void);
    header_sinit(0x1020053C, 0x1018EF28, 0x100049A0);
}
VERIFY(0x02031D7C, sinit_02031D7C);

static void sinit_02031E10() {
    WWHD_FUNC(0x02031E10, void);
    header_sinit(0x10200558, 0x1018EF4C, 0x100049A8);
}
VERIFY(0x02031E10, sinit_02031E10);

static void sinit_02031EA4() {
    WWHD_FUNC(0x02031EA4, void);
    header_sinit(0x10200574, 0x1018EF70, 0x100049B0);
}
VERIFY(0x02031EA4, sinit_02031EA4);

static void sinit_02031F38() {
    WWHD_FUNC(0x02031F38, void);
    header_sinit(0x10200590, 0x1018EF94, 0x100049B8);
}
VERIFY(0x02031F38, sinit_02031F38);

static void sinit_02031FCC() {
    WWHD_FUNC(0x02031FCC, void);
    header_sinit(0x102005AC, 0x1018EFB8, 0x100049C0);
}
VERIFY(0x02031FCC, sinit_02031FCC);

static void sinit_02032060() {
    WWHD_FUNC(0x02032060, void);
    header_sinit(0x102005C8, 0x1018EFDC, 0x100049C8);
}
VERIFY(0x02032060, sinit_02032060);

static void sinit_020320F4() {
    WWHD_FUNC(0x020320F4, void);
    header_sinit(0x102005E4, 0x1018F000, 0x100049D0);
}
VERIFY(0x020320F4, sinit_020320F4);

static void sinit_02032188() {
    WWHD_FUNC(0x02032188, void);
    header_sinit(0x10200600, 0x1018F024, 0x100049D8);
}
VERIFY(0x02032188, sinit_02032188);

static void sinit_0203221C() {
    WWHD_FUNC(0x0203221C, void);
    header_sinit(0x1020061C, 0x1018F048, 0x100049E0);
}
VERIFY(0x0203221C, sinit_0203221C);

static void sinit_020322B0() {
    WWHD_FUNC(0x020322B0, void);
    header_sinit(0x10200638, 0x1018F06C, 0x100049E8);
}
VERIFY(0x020322B0, sinit_020322B0);

static void sinit_02032344() {
    WWHD_FUNC(0x02032344, void);
    header_sinit(0x10200654, 0x1018F090, 0x100049F0);
}
VERIFY(0x02032344, sinit_02032344);

static void sinit_020323D8() {
    WWHD_FUNC(0x020323D8, void);
    header_sinit(0x10200670, 0x1018F0B4, 0x100049F8);
}
VERIFY(0x020323D8, sinit_020323D8);

static void sinit_0203246C() {
    WWHD_FUNC(0x0203246C, void);
    header_sinit(0x1020068C, 0x1018F0D8, 0x10004A00);
}
VERIFY(0x0203246C, sinit_0203246C);

static void sinit_02032500() {
    WWHD_FUNC(0x02032500, void);
    header_sinit(0x102006A8, 0x1018F0FC, 0x10004A08);
}
VERIFY(0x02032500, sinit_02032500);

static void sinit_02032594() {
    WWHD_FUNC(0x02032594, void);
    header_sinit(0x102006C4, 0x1018F120, 0x10004A10);
}
VERIFY(0x02032594, sinit_02032594);

static void sinit_02032628() {
    WWHD_FUNC(0x02032628, void);
    header_sinit(0x102006E0, 0x1018F144, 0x10004A18);
}
VERIFY(0x02032628, sinit_02032628);

static void sinit_020326BC() {
    WWHD_FUNC(0x020326BC, void);
    header_sinit(0x102006FC, 0x1018F168, 0x10004A20);
}
VERIFY(0x020326BC, sinit_020326BC);

static void sinit_02032750() {
    WWHD_FUNC(0x02032750, void);
    header_sinit(0x10200718, 0x1018F18C, 0x10004A28);
}
VERIFY(0x02032750, sinit_02032750);

static void sinit_020327E4() {
    WWHD_FUNC(0x020327E4, void);
    header_sinit(0x10200734, 0x1018F1B0, 0x10004A30);
}
VERIFY(0x020327E4, sinit_020327E4);

static void sinit_02032878() {
    WWHD_FUNC(0x02032878, void);
    header_sinit(0x10200750, 0x1018F1D4, 0x10004A38);
}
VERIFY(0x02032878, sinit_02032878);

static void sinit_0203290C() {
    WWHD_FUNC(0x0203290C, void);
    header_sinit(0x1020076C, 0x1018F1F8, 0x10004A40);
}
VERIFY(0x0203290C, sinit_0203290C);

static void sinit_020329A0() {
    WWHD_FUNC(0x020329A0, void);
    header_sinit(0x10200788, 0x1018F21C, 0x10004A48);
}
VERIFY(0x020329A0, sinit_020329A0);

}  // namespace hd_sinit_02031A98
