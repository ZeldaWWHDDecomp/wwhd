/** Shared local views of the HD sea-chart marker screens (MAP-secret_entrance+squid+4more).
 *  The screens derive from the canonical sea-chart link-position class (d/hd_sea_chart_link_pos.h). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_layout_mgr.h"
#include "d/hd_sea_chart_link_pos.h"
/* a child parts descriptor of the marker layouts: the pane (child) name sits at +0x14 */
struct hdChartChildDescriptor_l { u8 mPrefix[0x14]; hdUiSafeString mName; };
WWHD_OFFSET(hdChartChildDescriptor_l,mName,0x14);
inline hdChartChildDescriptor_l* hdChartChildName(hdPartsDesc_c* descriptor) {
    return reinterpret_cast<hdChartChildDescriptor_l*>(descriptor);
}
template<u32 N> struct hdChartStrings_l { hdUiSafeString mValues[N]; };
template<u32 N> struct hdChartIndices_l { be<u32> mValues[N]; };
struct hdChartRegistrationList_l { be<u32> mHead,mTail,mCount,mCapacity; };
WWHD_SIZE(hdChartRegistrationList_l,0x10);
struct hdChartSave_l { u8 mPrefix[0xD4]; u8 mCollect[0x10]; u8 mMap[1]; };
WWHD_OFFSET(hdChartSave_l,mMap,0xE4);
