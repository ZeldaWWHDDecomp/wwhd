/**
 * port_native_draw.h (WWHD)
 * Native draw packets (the port's native renderer, stage 1): a hot draw site hands the port's render thread one packet
 * description per pass and a small instance per draw, instead of the GX2 register writes plus a draw.
 *
 * Game build only, and only when the port's switch defines GABI_NATIVE_DRAW (decomp build, fast mode,
 * WWHD_NATIVE_DRAW=ON). Everywhere else PORT_NATIVE_DRAW is 0 and every site compiles exactly as without it.
 *
 * The generic entry points (one SITE value per draw site):
 * - wwhd_native_begin: after the pass's own setup (shader, display list, textures, render state), the packet of this
 *   pass (and VARIANT: a texture or blend group the site switches between). Nonzero: the port may take the draws.
 * - wwhd_native_pair_slot / wwhd_native_declare_pair / wwhd_native_declare_block: the instance blocks: the blocks the
 *   site binds per draw (or per group of draws), by a uniform pair's bind slot of the material's location table (given,
 *   or the one the pair's bind function binds), or by stage and location. A declared block the site binds itself (a
 *   per-object block) is read from the registers at each instance.
 * - wwhd_native_mesh: mesh KIND, with the arguments the site passes to GX2DrawIndexedEx (in GX2's order). INDEXADDR 0
 *   (and INDEXTYPE 0): a non-indexed mesh, the site's GX2DrawEx(PRIM, COUNT, 0, 1).
 * - wwhd_native_commit: the packet is complete. Nonzero: the draws become its instances.
 * - wwhd_native_pair / wwhd_native_block / wwhd_native_shape: where the site would call a uniform pair's bind (its
 *   vtable +0x2C), GX2Set{Vertex,Pixel,Geometry}UniformBlock or agl::VertexInputSet::bind. Nonzero: the call is the
 *   instance's (the port reads the same fields at the same point); zero: the site makes the call itself.
 * - wwhd_native_draw: where the site would call GX2DrawIndexedEx with mesh KIND. Nonzero: drawn as an instance.
 * - wwhd_native_draw_range: the same with mesh KIND's primitive and index type but an index range of its own (COUNT
 *   indices at INDEXADDR: a strip of a shared index buffer). Nonzero: drawn as an instance.
 * - wwhd_native_end: the pass's draws are done (the instance effects still pending are applied).
 * - wwhd_native_state: 0 no packet, 1 begun (the mesh and commit still to come), 2 defined: for a packet a callee
 *   completes (the 3D lines: the geometry pool's draw knows the index list).
 * - wwhd_native_declare_regs: before the commit, an instance register range of STAGE (0 vertex, 1 pixel): OFFSET and
 *   COUNT in words, as GX2Set{Vertex,Pixel}UniformReg take them (the UI's per-quad values).
 * - wwhd_native_regs: where the site would call GX2Set{Vertex,Pixel}UniformReg(OFFSET, COUNT, VALUES). Nonzero: the
 *   values (read at the call, by value) are the instance's; zero: the site makes the call itself.
 * - wwhd_native_attrib: where the site would call GX2SetAttribBuffer(INDEX, SIZE, STRIDE, DATA) alone (the fetch shader
 *   stays as it is). Nonzero: the buffer is the instance's vertex input; zero: the site makes the call itself.
 * - wwhd_native_declare_check: before the commit, state the site (or its callees) sets itself between instances: WHAT
 *   0 pixel texture unit INDEX (texture and sampler), 1 blend target INDEX, 2 attribute buffer INDEX. Such calls keep
 *   the packet; an instance replays only when that state is what the packet was lowered with.
 * - wwhd_native_j3d: the J3D material cache (the port's WWHD_NATIVE_DRAW_J3D): right before a J3D shape draw's
 *   GX2DrawIndexedEx, its key (shape resource, material binding, pass, view, program record). The site still makes
 *   every GX2 call of the draw itself; the port's render thread reuses what it resolved for the key last time while
 *   everything that resolution reads is unchanged.
 */
#pragma once

#if defined(GABI_GAME_BUILD) && defined(GABI_NATIVE_DRAW)
#define PORT_NATIVE_DRAW 1
enum PortNativeSite : unsigned {
    PORT_NATIVE_GRASS = 1, PORT_NATIVE_TREE = 2, PORT_NATIVE_WOOD = 3, PORT_NATIVE_FLOWER = 4, PORT_NATIVE_MAGMA = 5,
    PORT_NATIVE_STAR = 6, PORT_NATIVE_HOUSI = 7, PORT_NATIVE_WAVE = 8, PORT_NATIVE_MOYA = 9, PORT_NATIVE_VRKUMO = 10,
    PORT_NATIVE_RAIN = 11, PORT_NATIVE_SEA = 12, PORT_NATIVE_LINE = 13, PORT_NATIVE_JPA = 14, PORT_NATIVE_LAYOUT = 15,
    PORT_NATIVE_TEXT = 16,
    /* one draw per packet (port_native_draw_single): */
    PORT_NATIVE_POST = 17, PORT_NATIVE_SHADOW = 18, PORT_NATIVE_AGL_QUAD = 19, PORT_NATIVE_AGL_PAIR = 20,
    PORT_NATIVE_SUN = 21,
};
// PORT_NATIVE_POST's packets, one per effect
enum PortNativePostPass : unsigned {
    PORT_NATIVE_POST_BLOOM = 0, PORT_NATIVE_POST_BLOOM_VALUES = 1, PORT_NATIVE_POST_DEPTH_BLUR = 2,
    PORT_NATIVE_POST_FILTER_AA = 3, PORT_NATIVE_POST_COLOR_CORRECTION = 4, PORT_NATIVE_POST_HD_BACKEND = 5,
};
extern "C" unsigned wwhd_native_begin(unsigned site, unsigned pass, unsigned variant, unsigned material,
                                      unsigned program, unsigned texture);
extern "C" unsigned wwhd_native_pair_slot(unsigned site, unsigned locations, unsigned slot);
extern "C" unsigned wwhd_native_declare_pair(unsigned site, unsigned pair, unsigned locations);
extern "C" unsigned wwhd_native_declare_block(unsigned site, unsigned stage, unsigned location);
extern "C" void wwhd_native_mesh(unsigned site, unsigned kind, unsigned prim, unsigned count, unsigned indexType,
                                 unsigned indexAddr);
extern "C" unsigned wwhd_native_commit(unsigned site);
extern "C" unsigned wwhd_native_pair(unsigned site, unsigned pair, unsigned locations);
extern "C" unsigned wwhd_native_block(unsigned site, unsigned stage, unsigned location, unsigned size, unsigned data);
extern "C" unsigned wwhd_native_shape(unsigned site, unsigned shape);
extern "C" unsigned wwhd_native_draw(unsigned site, unsigned kind);
extern "C" unsigned wwhd_native_draw_range(unsigned site, unsigned kind, unsigned count, unsigned indexAddr);
extern "C" void wwhd_native_end(unsigned site);
extern "C" unsigned wwhd_native_state(unsigned site);
extern "C" unsigned wwhd_native_declare_regs(unsigned site, unsigned stage, unsigned offset, unsigned count);
extern "C" unsigned wwhd_native_regs(unsigned site, unsigned stage, unsigned offset, unsigned count, unsigned values);
extern "C" unsigned wwhd_native_attrib(unsigned site, unsigned index, unsigned size, unsigned stride, unsigned data);
extern "C" unsigned wwhd_native_declare_check(unsigned site, unsigned what, unsigned index);
extern "C" unsigned wwhd_native_j3d(unsigned shape, unsigned binding, unsigned pass, unsigned view, unsigned program);
// a site's experiment switch (1: WWHD_NATIVE_DRAW_<SITE>_SPLIT, draws regrouped by mesh kind); 0 unless set
extern "C" unsigned wwhd_native_option(unsigned site, unsigned option);
// pass and copy descriptions (the port's native_pass.h; port switch WWHD_NATIVE_PASS=1): engine-level descriptions
// next to the site's own GX2 calls, which stay as they are.
// - wwhd_native_pass_color / _depth: an attachment of the pass the site is binding (the GX2ColorBuffer /
//   GX2DepthBuffer it binds, as it binds it); wwhd_native_pass_begin: the attachments form the pass begun now.
// - wwhd_native_pass_load: the site clears attachments of the open pass (bits: 1 colour INDEX, 2 depth, 4 stencil).
// - wwhd_native_pass_copy: a copy of KIND (1 scan-out to DEST 0 TV / 1 GamePad, 2 depth expand, 3 colour expand) of
//   the GX2ColorBuffer (or, IS_DEPTH, GX2DepthBuffer) SRC.
enum PortNativeCopy : unsigned { PORT_NATIVE_COPY_SCAN = 1, PORT_NATIVE_COPY_EXPAND_DEPTH = 2, PORT_NATIVE_COPY_EXPAND_COLOR = 3 };
extern "C" unsigned wwhd_native_pass_color(unsigned index, unsigned colorBuffer);
extern "C" unsigned wwhd_native_pass_depth(unsigned depthBuffer);
extern "C" unsigned wwhd_native_pass_begin();
extern "C" void wwhd_native_pass_load(unsigned bits, unsigned index, float r, float g, float b, float a, double depth,
                                      unsigned stencil);
extern "C" void wwhd_native_pass_copy(unsigned kind, unsigned dest, unsigned src, unsigned isDepth);
// - wwhd_native_pass_end: the open pass ends here (the site knows its end; its attachments are stored).
// - wwhd_native_pass_viewport / _scissor: the site sets the open pass's viewport / scissor (its GX2 call follows).
extern "C" void wwhd_native_pass_end();
extern "C" void wwhd_native_pass_viewport(float x, float y, float w, float h, float n, float f);
extern "C" void wwhd_native_pass_scissor(unsigned x, unsigned y, unsigned w, unsigned h);
// one draw as a packet of its own (a post effect, the shadow preparation, an agl quad: one program and target per draw;
// the program, uniforms, textures and vertex input stay the site's own calls): where the site would call
// GX2DrawIndexedEx(PRIM, COUNT, INDEXTYPE, INDEXADDR, BASEVERTEX, INSTANCES), or (INDEXTYPE and INDEXADDR 0) where it
// would call GX2DrawEx(PRIM, COUNT, BASEVERTEX, INSTANCES). Nonzero: drawn as the packet's instance; zero: the site makes
// the call itself (packets off, the site off, or another base / first vertex or instance count).
static inline unsigned port_native_draw_single(unsigned site, unsigned pass, unsigned material, unsigned program,
                                               unsigned texture, unsigned prim, unsigned count, unsigned indexType,
                                               unsigned indexAddr, unsigned baseVertex, unsigned instances) {
    if (baseVertex != 0 || instances != 1 || !wwhd_native_begin(site, pass, 0, material, program, texture)) return 0;
    wwhd_native_mesh(site, 0, prim, count, indexType, indexAddr);
    const unsigned drawn = wwhd_native_commit(site) && wwhd_native_draw(site, 0);
    wwhd_native_end(site);
    return drawn;
}
#else
#define PORT_NATIVE_DRAW 0
#endif
