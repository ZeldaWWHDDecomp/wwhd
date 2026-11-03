// JStudio function-value layouts; Cafe double attributes retain explicit eight-byte alignment gaps.
#pragma once
#include "wwhd.h"
namespace JStudio {
struct TFunctionValue;
struct TFunctionValueAttribute_refer {
    be<u32> allocator;
    gptr<gptr<TFunctionValue>> begin, end, capacity;
    be<u32> growth;
};
struct TFunctionValueAttribute_range {
    be<f64> fBegin_, fEnd_, fDifference_;
    be<s8> mProgress, mAdjust;
    u8 reserved[6];
    be<f64> progressOrigin, progressScale;
    be<u32> mBegin, mEnd;
    static TFunctionValueAttribute_range* construct(TFunctionValueAttribute_range*);
    void range_prepare();
    void range_set(f64, f64);
    f64 range_getParameter(f64, f64, f64) const;
};
struct TFunctionValueAttributeSet {
    gptr<TFunctionValueAttribute_refer> refer_;
    gptr<TFunctionValueAttribute_range> range_;
    gptr<be<u32>> interpolate_;
};
struct FunctionValueVtable { u8 prefix[0x34]; gfn getValue; };
struct TFunctionValue {
    gptr<FunctionValueVtable> __vtbl;
    static TFunctionValue* construct(TFunctionValue*);
    static u32 toFunction_outside(s32);
    void destroy(s32);
};
struct TFunctionValue_composite : TFunctionValue {
    TFunctionValueAttribute_refer refer;
    gfn pfn_;
    u8 alignment[4];
    union TData { be<u32> uintdata; be<f64> f64data; } data;
    static TFunctionValue_composite* construct(TFunctionValue_composite*);
    static void getAttributeSet(TFunctionValueAttributeSet*, TFunctionValue_composite*);
    void prepare();
    f64 getValue(f64);
    static f64 composite_raw(const TFunctionValueAttribute_refer*, const TData*, f64);
    static f64 composite_index(const TFunctionValueAttribute_refer*, const TData*, f64);
    static f64 composite_parameter(const TFunctionValueAttribute_refer*, const TData*, f64);
    static f64 composite_add(const TFunctionValueAttribute_refer*, const TData*, f64);
    static f64 composite_subtract(const TFunctionValueAttribute_refer*, const TData*, f64);
    static f64 composite_multiply(const TFunctionValueAttribute_refer*, const TData*, f64);
    static f64 composite_divide(const TFunctionValueAttribute_refer*, const TData*, f64);
};
struct TFunctionValue_constant : TFunctionValue {
    u8 alignment[4]; be<f64> fValue_;
    static TFunctionValue_constant* construct(TFunctionValue_constant*);
    static void getAttributeSet(TFunctionValueAttributeSet*, TFunctionValue_constant*);
    void prepare();
    f64 getValue(f64);
};
struct TFunctionValue_transition : TFunctionValue {
    u8 alignment[4]; TFunctionValueAttribute_range range; be<u32> interpolate; u8 tailAlignment[4];
    be<f64> valueBegin, valueEnd;
    static TFunctionValue_transition* construct(TFunctionValue_transition*);
    static void getAttributeSet(TFunctionValueAttributeSet*, TFunctionValue_transition*);
    void prepare(); f64 getValue(f64);
};
struct TFunctionValue_list : TFunctionValue {
    u8 alignment[4]; TFunctionValueAttribute_range range; be<u32> interpolate;
    gptr<const be<f32>> pfData_; be<u32> uData_; u8 intervalAlignment[4]; be<f64> interval; gfn pfnUpdate_; u8 tail[4];
    struct TIndexData_ { be<f64> parameter, indexParameter; be<u32> index; };
    static TFunctionValue_list* construct(TFunctionValue_list*);
    static void getAttributeSet(TFunctionValueAttributeSet*, TFunctionValue_list*);
    static f64 update_INTERPOLATE_NONE_(const TFunctionValue_list*, const TIndexData_*);
    static f64 update_INTERPOLATE_LINEAR_(const TFunctionValue_list*, const TIndexData_*);
    static f64 update_INTERPOLATE_PLATEAU_(const TFunctionValue_list*, const TIndexData_*);
    static f64 update_INTERPOLATE_BSPLINE_dataMore3_(const TFunctionValue_list*, const TIndexData_*);
    void prepare(); f64 getValue(f64);
};
struct TFunctionValue_list_parameter : TFunctionValue {
    u8 alignment[4]; TFunctionValueAttribute_range range; be<u32> interpolate;
    gptr<const be<f32>> pfData_; be<u32> uData_;
    gptr<const be<f32>> dat1, dat2, dat3;
    gfn pfnUpdate_; u8 tail[4];
    static TFunctionValue_list_parameter* construct(TFunctionValue_list_parameter*);
    static void getAttributeSet(TFunctionValueAttributeSet*, TFunctionValue_list_parameter*);
    void data_set(const be<f32>*,u32);
    void prepare(); f64 getValue(f64);
    static f64 update_INTERPOLATE_NONE_(const TFunctionValue_list_parameter*, f64);
    static f64 update_INTERPOLATE_LINEAR_(const TFunctionValue_list_parameter*, f64);
    static f64 update_INTERPOLATE_PLATEAU_(const TFunctionValue_list_parameter*, f64);
    static f64 update_INTERPOLATE_BSPLINE_dataMore3_(const TFunctionValue_list_parameter*, f64);
};
struct TFunctionValue_hermite : TFunctionValue {
    u8 alignment[4]; TFunctionValueAttribute_range range;
    gptr<const be<f32>> pfData_; be<u32> u_,uSize_;
    struct TIterator_data_ { gptr<const be<f32>> pf_; be<u32> uSize_; };
    TIterator_data_ dat1,dat2,dat3; u8 tail[4];
    static TFunctionValue_hermite* construct(TFunctionValue_hermite*);
    static void getAttributeSet(TFunctionValueAttributeSet*, TFunctionValue_hermite*);
    void data_set(const be<f32>*,u32,u32);
    void prepare(); f64 getValue(f64);
};
WWHD_SIZE(TFunctionValue_list_parameter,0x60); WWHD_OFFSET(TFunctionValue_list_parameter,dat1,0x4C); WWHD_OFFSET(TFunctionValue_list_parameter,pfnUpdate_,0x58);
WWHD_SIZE(TFunctionValue_hermite,0x68); WWHD_OFFSET(TFunctionValue_hermite,pfData_,0x40); WWHD_OFFSET(TFunctionValue_hermite,dat1,0x4C); WWHD_OFFSET(TFunctionValue_hermite,dat3,0x5C);
namespace functionvalue { f64 interpolateValue_hermite(f64,f64,f64,f64,f64,f64,f64); f64 interpolateValue_BSpline_uniform(f64,f64,f64,f64,f64);
struct SplinePoints { be<f64> values[4]; }; struct SplineKnots { be<f64> values[6]; };
f64 interpolateValue_BSpline_nonuniform(const SplinePoints*,const SplineKnots*,f64);
f64 extrapolateParameter_raw(f64,f64); f64 extrapolateParameter_repeat(f64,f64); f64 extrapolateParameter_turn(f64,f64); f64 extrapolateParameter_clamp(f64,f64); }
WWHD_SIZE(TFunctionValue,4); WWHD_OFFSET(TFunctionValue,__vtbl,0);
WWHD_SIZE(TFunctionValueAttribute_refer,0x14); WWHD_OFFSET(TFunctionValueAttribute_refer,begin,4); WWHD_OFFSET(TFunctionValueAttribute_refer,end,8);
WWHD_SIZE(TFunctionValueAttribute_range,0x38); WWHD_OFFSET(TFunctionValueAttribute_range,mProgress,0x18); WWHD_OFFSET(TFunctionValueAttribute_range,progressOrigin,0x20); WWHD_OFFSET(TFunctionValueAttribute_range,mBegin,0x30);
WWHD_SIZE(TFunctionValueAttributeSet,12);
WWHD_SIZE(TFunctionValue_composite,0x28); WWHD_OFFSET(TFunctionValue_composite,refer,4); WWHD_OFFSET(TFunctionValue_composite,pfn_,0x18); WWHD_OFFSET(TFunctionValue_composite,data,0x20);
WWHD_SIZE(TFunctionValue_constant,0x10); WWHD_OFFSET(TFunctionValue_constant,fValue_,8);
WWHD_SIZE(TFunctionValue_transition,0x58); WWHD_OFFSET(TFunctionValue_transition,range,8); WWHD_OFFSET(TFunctionValue_transition,interpolate,0x40); WWHD_OFFSET(TFunctionValue_transition,valueBegin,0x48);
WWHD_SIZE(TFunctionValue_list,0x60); WWHD_OFFSET(TFunctionValue_list,pfData_,0x44); WWHD_OFFSET(TFunctionValue_list,uData_,0x48); WWHD_OFFSET(TFunctionValue_list,interval,0x50); WWHD_OFFSET(TFunctionValue_list,pfnUpdate_,0x58);
}
