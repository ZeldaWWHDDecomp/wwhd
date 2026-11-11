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

## dTree_data_c (d_tree.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x001 | `field_0x001` | `mCut` | setData 025CA9F8 stores the state argument (1: placed already cut); WorkAt 025C6A58 sets 1 when the tree is cut (and refuses a second cut); WorkCo 025C65CC / WorkAt_NoCutAnim 025C6784 skip a cut tree; animation 025C6D40 swings an uncut tree and drops the top of a cut one; TreePacketState::draw 025C91FC picks the model part status by it |
| 0x003 | `field_0x003` | `mTopAlpha` | setData 025CA9F8: 0 for a tree placed cut, else 255; update 025CA6EC fades it to 0 once the top has landed; TreePacketState::draw 025C91FC passes it as the top alpha (255: the opaque path), the native draw scales the top by it |
| 0x010 | `field_0x010` | `mTopDrawMtx` | update 025CA6EC copies the animation's mTopMtx (or mFallenTopMtx once landed) into it; the native draw (tree_matrixUniform) uses it for the top part |
| 0x040 | `field_0x040` | `mTrunkDrawMtx` | update 025CA6EC copies the animation's mTrunkMtx into it; the native draw (tree_matrixUniform) uses it for the trunk; Tree_dataCtor 025C64C4 clears it |
| 0x070 | `field_0x070` | `mFallenTopMtx` | animation 025C6D40 stores the top's final matrix (with mTopOffset) once the falling top has landed (mState bit 8); update 025CA6EC then draws the top with it; Tree_dataCtor 025C64C4 clears it |


## dTree_anm_c (d_tree.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x002 | `field_0x02` | `mTrunkAngleY` | setAnm 025C79D0 / newAnm 025C6558 store the tree's Y angle; update 025CA6EC builds mTrunkMtx with Y rotation it - mCutAngleY; its top 3 bits (>> 13) are the wind animation an uncut swinging tree returns to (animation 025C6D40) or a fallen one keeps; calc 025C9948 sways the 8 wind animations around it |
| 0x004 | `field_0x04` | `mTopAngleX` | the X tilt of the top: calc 025C9948 sways it with the wind; animation 025C6D40 stores the swing angle for a hit tree and tilts a cut top to 13600; update 025CA6EC builds mTopMtx with it |
| 0x006 | `field_0x06` | `mTopAngleY` | the Y rotation of the top: calc 025C9948 sways it with the wind; setAnm 025C79D0 / newAnm 025C6558 set the tree's angle; update 025CA6EC builds mTopMtx with it - mCutAngleY |
| 0x008 | `field_0x08` | `mTrunkAngleX` | the swing of a hit tree: animation 025C6D40 runs it as a damped oscillation (with mAnimTimer) and clears it when it settles; update 025CA6EC builds mTrunkMtx with it |
| 0x00A | `field_0x0a` | `mCutAngleY` | WorkAt 025C6A58 stores Link's sword move angle; animation 025C6D40 moves the falling top along it; update 025CA6EC rotates both matrices by it; WorkCo 025C65CC / WorkAt_NoCutAnim 025C6784 store the push / hit direction |
| 0x010 | `field_0x10` | `mFallSpeedXZ` | WorkAt 025C6A58 sets the cut top's horizontal speed; animation 025C6D40 moves the top by it along mCutAngleY and clears it on landing (which ends the fall) |
| 0x014 | `field_0x14` | `mFallSpeedY` | WorkAt 025C6A58 sets the cut top's initial vertical speed; animation 025C6D40 adds gravity (down to the terminal speed) and moves the top by it |
| 0x018 | `field_0x18` | `mTopOffsetX` | animation 025C6D40 moves the falling top along x; update 025CA6EC adds it to mPos.x for mTopMtx; animation adds it for mFallenTopMtx |
| 0x01C | `field_0x1c` | `mTopOffsetY` | animation 025C6D40 moves the falling top along y (not below the ground offset); update 025CA6EC adds it to mPos.y for mTopMtx |
| 0x020 | `field_0x20` | `mTopOffsetZ` | animation 025C6D40 moves the falling top along z; update 025CA6EC adds it to mPos.z for mTopMtx |
| 0x024 | `field_0x24` | `mTopMtx` | update 025CA6EC builds the top's matrix (Y mCutAngleY, X mTopAngleX, Y mTopAngleY, translation mPos + mTopOffset) into it; animation 025C6D40 places the cut smoke with it and copies it into mFallenTopMtx |
| 0x054 | `field_0x54` | `mTrunkMtx` | update 025CA6EC builds the trunk's matrix (Y mCutAngleY, X mTrunkAngleX, Y mTrunkAngleY, translation mPos) into it and copies it into dTree_data_c::mTrunkDrawMtx |

## d_kankyo_rain_drawSibuki.h views (round 2)

Views take their class's names (a view of another class's layout is renamed with that class).

| View / offset | Old | New | Class member | Uses |
|---|---|---|---|---|
| SunPacket 0xC4 | `field_0x3c` | `mOcclusionDelayHD` | dKankyo_sun_Packet::mOcclusionDelayHD (d_kankyo_wether.h) | dKyr_drawSun 0256F1C4 raises it by 2 (below 5) |
| SunPacket 0xC5 | `field_0x3d` | `mHasVisiblePointHD` | dKankyo_sun_Packet::mHasVisiblePointHD | dKyr_drawSun 0256F1C4 sets 1 |
| HousiPacket 0x5E64 | `field_0x5ddc` | `mAlpha` | dKankyo_housi_Packet::mAlpha (d_kankyo_wether.h) | dKyr_housi_move 02567F68 eases it toward 1 / 0 by the environment's mHousiCount and fades the effects by it |

Open: HOUSI_EFF `field_0x34`, `field_0x38`, `field_0x3c`, `field_0x44`, `field_0x48`, `field_0x4c` and WAVE_EFF `field_0x30`, `field_0x32` (their classes in d_kankyo_wether.h are unnamed at these offsets too; the views follow when the classes get names).
