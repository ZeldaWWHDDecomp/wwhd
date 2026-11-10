# Field names: non-actor draw systems (d_tree, d_magma, d_kankyo_wether)

Field naming pass (2026-11-10). Names come from how the original code uses a field (address of
the use), from names already in this decomp, or from the CC0 zeldaret decomps. Renames only: offsets, types and code are
unchanged. The source keeps the old name (`was ...`) with the evidence next to each declaration.

## GX2 import declarations

The `ext` glue lines of these three sources declared the GX2 import slots below under made-up names; two of them
("UniformReg") are the names of the neighbouring slots C0006908 GX2SetPixelUniformReg and C0006A40
GX2SetVertexUniformReg. The names now follow `build/gen/imports.json` (slot -> library and export name); the
recompiled original calls the same slots as `imp_gx2_<name>` (e.g. 02588F50, 02588F68, 02588F80, 0258924C in
d_magma, 025C7934 in d_tree).

| Slot | GX2 export (imports.json) | Old name in d_tree.cpp | Old name in d_magma.cpp | Old name in d_kankyo_wether.cpp |
|---|---|---|---|---|
| C0006178 | `GX2DrawIndexedEx` | `GX2DrawIndexed` | `magmaDrawIndexed` | `weatherDrawIndexed` |
| C00068A8 | `GX2SetGeometryUniformBlock` | `GX2SetGeometryUniformReg` | `magmaBindGeometryBuffer` | `weatherSetGeometryUniforms` |
| C0006900 | `GX2SetPixelUniformBlock` | `GX2SetPixelUniformReg` | `magmaBindVertexBuffer` | `weatherSetPixelUniforms` |
| C0006A38 | `GX2SetVertexUniformBlock` | `GX2SetVertexUniformReg` | `magmaBindIndexBuffer` | `weatherSetVertexUniforms` |

The forward declarations in include/d/d_tree.h, include/d/d_kankyo_wether.h and d_magma.cpp follow.

## Fields

GX2 argument order used as evidence: `GX2DrawIndexedEx(mode r3, count r4, indexType r5, indices r6, baseVertex r7,
numInstances r8)` and `GX2Set{Pixel,Vertex,Geometry}UniformBlock(location r3, size r4, data r5)`, as in our
include/agl/EffectShaderViews.h and include/JSystem/J3DGraph/RenderUniformBindings.h.

| Struct | Offset | Old | New | Evidence |
|---|---|---|---|---|
| TreeIndexBuffer (d_tree.h) | 0x0 | `mPrimitive` | `mIndexType` | GX2DrawIndexedEx indexType (r5) at 025C7934 (lwz 0x0(r27)); agl::IndexBufferLayout::mFormat |
| TreeIndexBuffer (d_tree.h) | 0x4 | `mIndexType` | `mPrimitive` | GX2DrawIndexedEx mode (r3) at 025C7934 (lwz 0x4(r27)); agl::IndexBufferLayout::mPrimitiveMode |
| dMagma::HdIndexBuffer (d_magma.h) | 0x0 | `mIndexCount` | `mFormat` | GX2DrawIndexedEx indexType (r5) at 0258924C / 0258AC80; agl::IndexBufferLayout::mFormat (the struct is a view of agl::IndexBufferLayout, constructed by 027B5430) |
| dMagma::HdIndexBuffer | 0x8 | `mIndexOffset` | `mStorage` | GX2DrawIndexedEx indices (r6) at 0258AC80, base of the per-strip pointer at 0258924C; agl::IndexBufferLayout::mStorage |
| dMagma::HdIndexBuffer | 0xC | `mpIndices` | `mIndexCount` | GX2DrawIndexedEx count (r4) at 0258AC80, tested for 0 first; agl::IndexBufferLayout::mIndexCount |
| dMagma::HdIndexBuffer | 0x10 | `mIndexStride` | `mIndexBytes` | scales the strip's first index into a byte offset from mStorage at 0258924C; agl::IndexBufferLayout::mIndexBytes |
| dMagma::HdRenderBuffer (d_magma.h) | 0x4 | `mSize` | `mCpuStorage` | GX2Set*UniformBlock data (r5) at 02588F50 / 02588F68 / 02588F80; agl::RenderUniformBuffer_l::mCpuStorage |
| dMagma::HdRenderBuffer | 0xC | `mpData` | `mStride` | GX2Set*UniformBlock size (r4) at the same calls; agl::RenderUniformBuffer_l::mStride |
| MagmaBindingDescriptor (d_magma.h) | 0xC | `mIndexBuffer` | `mVertexLocation` | location (r3) of GX2SetVertexUniformBlock at 02588F68; j3d::RenderUniformBinding_l::mVertexLocation |
| MagmaBindingDescriptor | 0xE | `mVertexBuffer` | `mPixelLocation` | location (r3) of GX2SetPixelUniformBlock at 02588F50; j3d::RenderUniformBinding_l::mPixelLocation |
| MagmaBindingDescriptor | 0x10 | `mGeometryBuffer` | `mGeometryLocation` | location (r3) of GX2SetGeometryUniformBlock at 02588F80; j3d::RenderUniformBinding_l::mGeometryLocation |
| MagmaBallStripTable (d_magma.h) | 0x0 | `indices[6]` | `mIndexCounts[6]` | entry i is the GX2DrawIndexedEx count (r4) of ball strip i at 0258924C (lwzx from 101E9C84), skipped when 0 |

The code was already right: only the names were wrong (d_magma's locals `size`/`data` and `indices` follow the
fields). d_kankyo_wether's own index-buffer statics (weatherTriangleIndexType/Primitive/Indices at 104B4A84..,
weatherQuadIndexType/Primitive/Indices at 104B4CFC..) were already named right.

Types (separate commit): dMagma::HdIndexBuffer::mStorage and HdRenderBuffer::mCpuStorage are `gptr<void>`,
HdIndexBuffer::mIndexCount, HdRenderBuffer::mStride and MagmaBallStripTable::mIndexCounts are `be<u32>`; d_magma's
GX2DrawIndexedEx glue declaration keeps its `void*` count parameter (object-identical; the count goes
through an explicit `at<void>()`, the uniform size through an explicit `ea(at<void>())`, as the object code always did).
Correcting the glue signature is left to the later change with re-verification ("glue signatures/round trips that
change object code").

## d_wood, d_flower

The same mix-up as in d_tree: the index-buffer views name the GX2DrawIndexedEx arguments the wrong way round
(the code was right). Both structs have the layout of agl::IndexBufferLayout and now use its names; the ext
lines `woodGX2DrawIndexed` / `flowerGX2DrawIndexed` (slot C0006178) are `GX2DrawIndexedEx` (imports.json).

| Struct | Offset | Old | New | Evidence |
|---|---|---|---|---|
| IndexBuffer (d_wood.h) | 0x0 | `mPrimitive` | `mFormat` | GX2DrawIndexedEx indexType (r5) at 025CF1D8 / 025D1CA8 / 025D1D38 |
| IndexBuffer (d_wood.h) | 0x4 | `mpIndices` | `mPrimitiveMode` | GX2DrawIndexedEx mode (r3) at the same calls; 025CF20C sets 4 (triangles) |
| IndexBuffer (d_wood.h) | 0x8 | `mIndexType` | `mStorage` | GX2DrawIndexedEx indices (r6) at the same calls |
| flower::IndexBuffer (d_flower.h) | 0x0 | `mPrimitive` | `mFormat` | GX2DrawIndexedEx indexType (r5) at 02548164 / 025481EC / 025482AC / 02548338 |
| flower::IndexBuffer (d_flower.h) | 0x4 | `mIndexFormat` | `mPrimitiveMode` | GX2DrawIndexedEx mode (r3) at the same calls; 02545A2C sets 4 (triangles) |
| flower::IndexBuffer (d_flower.h) | 0x8 | `mIndexType` | `mStorage` | GX2DrawIndexedEx indices (r6) at the same calls |

Types (separate commits, objcmp identical): d_wood's mPrimitiveMode is now `be<u32>` (was `gptr<void>`, holds the
mode), mStorage `gptr<void>` (was `be<u32>`, holds the index pointer); d_flower's mStorage is now `gptr<void>` (was
`be<u32>`). The GX2DrawIndexedEx glue keeps its u32 parameters; d_wood's draw keeps the old mode round trip explicit. The `mOffset`
member at 0x10 of both (agl's mIndexBytes) has no use in these sources and keeps its name.
