/* Pane hit test with the original paired-single inverse and point transform. */
#pragma once
#include "nw/lyt/LayoutGpuResources.h"
namespace nw { namespace lyt {
u32 containsLayoutTransformedPoint(LayoutPane* pane,const PaneSize* point);
} }
u32 panePointContains(const nw::lyt::TextBounds* bounds,const nw::lyt::PaneSize* point);
