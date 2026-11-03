/* Window pane of nw::lyt: a 0xD8-byte pane with a content quad and one, two, four or eight frame
 * materials drawn around it. Layout from the constructor and its method table at 10180874; resource
 * offsets are the BFLYT wnd1 block (format facts only). */
#pragma once
#include "nw/lyt/PaneText.h"
#include "nw/lyt/PictureGeometry.h"
namespace nw { namespace lyt {

/* BFLYT wnd1 block after the 0x54-byte pane header, its content record and its frame records. */
struct WindowResource {
    u8 pane[0x54];
    be<s16> inflation[4];       // left, right, top, bottom in 1/16 units
    be<u16> frameSizes[4];      // left, right, top, bottom
    be<u8> frameCount, flags;
    u8 reserved[2];
    be<u32> contentOffset, frameTableOffset;
};
struct WindowContentResource {
    ColorWord colors[4];
    be<u16> materialIndex;
    be<u8> coordinateCount, reserved;
    LayoutTextureCoordinates coordinates[1];
};
struct WindowFrameResource { be<u16> materialIndex; be<u8> textureFlip, reserved; };
enum WindowResourceFlag : u32 {
    WindowResource_OneMaterialForAll = 1, WindowResource_VertexColorForAll = 2,
    WindowResource_KindShift = 2, WindowResource_HideContent = 0x10,
};
WWHD_OFFSET(WindowResource, frameSizes, 0x5C);
WWHD_OFFSET(WindowResource, frameCount, 0x64);
WWHD_OFFSET(WindowResource, contentOffset, 0x68);
WWHD_OFFSET(WindowContentResource, coordinates, 0x14);
WWHD_SIZE(WindowFrameResource, 4);

enum WindowKind : u32 { Window_Around = 0, Window_Horizontal = 1, Window_HorizontalNoContent = 2 };
enum WindowFlag : u32 {
    Window_VertexColorForAll = 1, Window_OneMaterialForAll = 2, Window_HideContent = 4, Window_Inflated = 8,
};

struct WindowFrame {
    be<u8> textureFlip;
    u8 padding[3];
    gptr<LayoutMaterial> material;
    void destroy(u32 flags);
};
WWHD_SIZE(WindowFrame, 8);
struct WindowInsets { be<f32> left, right, top, bottom; };
WWHD_SIZE(WindowInsets, 0x10);

struct WindowPane : LayoutPane {
    be<s16> inflation[4];
    be<u16> frameSizes[4];
    ColorWord colors[4];
    TextureCoordinateStorage coordinates;
    be<u8> kind, frameCount, windowFlags, reserved;
    gptr<WindowFrame> frames;
    gptr<LayoutMaterial> material;

    static WindowPane* createFromResource(WindowPane* pane, WindowResource* resource, WindowResource* overrideResource, PaneBuildContext* context);
    void destroy(u32 flags);
    void reserveCoordinates(u32 count);
    void reserveFrames(u32 count);
    void setColorElement(u32 index, u32 value);
    u32 materialCount();
    LayoutMaterial* materialAt(u32 index);
    LayoutMaterial* findMaterial(const be<u8>* name, u32 recursive);
    void drawAround(LayoutDrawContext* context);
    void drawHorizontal(LayoutDrawContext* context);
    void drawHorizontalNoContent(LayoutDrawContext* context);
    void drawContent(LayoutDrawContext* context, const PaneSize* origin, const WindowInsets* insets, u32 alpha);
    void drawAroundFrameOne(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha);
    void drawAroundFrameFour(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha);
    void drawAroundFrameEight(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha);
    void drawHorizontalFrameOne(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha);
    void drawHorizontalFrameTwo(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha);
    void drawHorizontalNoContentFrameOne(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha);
    void drawHorizontalNoContentFrameTwo(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha);
};
WWHD_OFFSET(WindowPane, inflation, 0xA4);
WWHD_OFFSET(WindowPane, colors, 0xB4);
WWHD_OFFSET(WindowPane, coordinates, 0xC4);
WWHD_OFFSET(WindowPane, frameCount, 0xCD);
WWHD_OFFSET(WindowPane, frames, 0xD0);
WWHD_OFFSET(WindowPane, material, 0xD4);
WWHD_SIZE(WindowPane, 0xD8);
} }

void windowCreateCoordinates(nw::lyt::TextureCoordinateStorage* coordinates);
void windowClearCoordinates(nw::lyt::TextureCoordinateStorage* coordinates);
void windowReserveCoordinates(nw::lyt::TextureCoordinateStorage* coordinates, u32 count);
void windowSetCoordinates(nw::lyt::TextureCoordinateStorage* coordinates, const nw::lyt::LayoutTextureCoordinates* source, u32 count);
void windowDrawQuad(nw::lyt::LayoutDrawContext* context, const nw::lyt::PaneSize* position, const nw::lyt::PaneSize* size, u32 coordinateCount, nw::lyt::LayoutTextureCoordinates* coordinates);
void windowDrawRectangle(nw::lyt::LayoutDrawContext* context, const nw::lyt::PaneSize* position, const nw::lyt::PaneSize* size);
void windowDrawNextRectangle(nw::lyt::LayoutDrawContext* context, const nw::lyt::PaneSize* position, const nw::lyt::PaneSize* size);
void windowSetVertexUniforms(u32 location, u32 count, const void* values);
void windowBindMaterial(nw::lyt::LayoutMaterial* material, nw::lyt::LayoutDrawContext* context);
void windowSetMaterialTranslation(nw::lyt::LayoutMaterial* material, nw::lyt::LayoutDrawContext* context, const nw::lyt::PaneSize* translation);
nw::lyt::ColorWord* windowCreateColor(nw::lyt::ColorWord* color);
nw::lyt::PaneSize* windowCreatePoint(nw::lyt::PaneSize* point);
