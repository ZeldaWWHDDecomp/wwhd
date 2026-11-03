// JStudio function-value evaluation, reconstructed against Cafe and the GameCube JStudio reference.
#include "JSystem/JStudio/functionvalue.h"
#include "JSystem/JStudio/jstudio-object.h"
using namespace JStudio;
namespace {
struct VtableStorage { u8 bytes[0x38]; };
static const VtableStorage sBaseVtable;
static const VtableStorage sCompositeVtable;
static const VtableStorage sConstantVtable;
static const VtableStorage sTransitionVtable;
static const VtableStorage sListVtable;
static const VtableStorage sParameterVtable;
static const VtableStorage sHermiteVtable;
struct ExtrapolationTable { be<u32> functions[4]; };
static const ExtrapolationTable gapfnExtrapolateParameter_;
}
void* FunctionValue_allocate(u32);
void FunctionValue_delete(void*);
void FunctionValue_register(void*);
void FunctionValue_constructRefer(TFunctionValueAttribute_refer*, const void*);
f64 FunctionValue_floor(f64);
Pair32 FunctionValue_div(s32,s32);

/* 028488A0 */
u32 TFunctionValue::toFunction_outside(s32 index) {
    u32 function=0;
    if ((u32)index<4) function=gapfnExtrapolateParameter_.functions[index];
    if (!function) function=gapfnExtrapolateParameter_.functions[0];
    return function;
}

/* 028488E8 */
TFunctionValue* TFunctionValue::construct(TFunctionValue* value) {
    if (!value) value=static_cast<TFunctionValue*>(FunctionValue_allocate(sizeof(TFunctionValue)));
    if (value) value->__vtbl=reinterpret_cast<FunctionValueVtable*>(const_cast<VtableStorage*>(&sBaseVtable));
    return value;
}

/* 02848928 */
void TFunctionValue::destroy(s32 flags) {
    if (this && (flags&1)) FunctionValue_delete(this);
}

/* 0284893C */
void TFunctionValueAttribute_range::range_prepare() {
    f64 origin=0.0, scale=-1.0;
    switch ((s8)mProgress) {
    default: scale=1.0; break;
    case 1: break;
    case 2: origin=fBegin_; break;
    case 3: origin=fEnd_; break;
    case 4: origin=(f64(fBegin_)+f64(fEnd_))*0.5; break;
    }
    // HD: retain GHS store order, which is observable for overlapping guest views.
    if ((s8)mProgress==2 || (s8)mProgress==3) { progressScale=scale; progressOrigin=origin; }
    else { progressOrigin=origin; progressScale=scale; }
}

/* 028489D8 */
void TFunctionValueAttribute_range::range_set(f64 begin,f64 end) {
    f64 difference=end-begin;
    fBegin_=begin; fEnd_=end; fDifference_=difference;
}

/* 028489EC */
f64 TFunctionValueAttribute_range::range_getParameter(f64 parameter,f64 outputBegin,f64 outputEnd) const {
    f64 origin=progressOrigin;
    f64 progress=gabi::fmadd(parameter-origin,f64(progressScale),origin);
    s8 adjust=mAdjust;
    if (adjust==1) progress=progress+f64(fBegin_);
    else if (adjust==2) progress=progress+f64(fEnd_);
    else if (adjust==3) progress=gabi::fmadd(f64(fBegin_)+f64(fEnd_),0.5,progress);
    f64 begin=fBegin_;
    f64 outside=progress-begin;
    f64 difference;
    if (outside<0.0) {
        u32 function=TFunctionValue::toFunction_outside(mBegin);
        difference=fDifference_;
        outside=gabi::call_ptr<f64>(function,outside,difference);
        begin=fBegin_;
        if (adjust==4) difference=fDifference_;
    } else {
        difference=fDifference_;
        if (!(outside<difference)) {
            u32 function=TFunctionValue::toFunction_outside(mEnd);
            outside=gabi::call_ptr<f64>(function,outside,difference);
            if (adjust==4) difference=fDifference_;
            begin=fBegin_;
        }
    }
    f64 result=outside+begin;
    if (adjust==4) return ((result-begin)*(outputEnd-outputBegin))/difference+outputBegin;
    return result;
}

/* 02848C04 */
TFunctionValueAttribute_range* TFunctionValueAttribute_range::construct(TFunctionValueAttribute_range* range) {
    if (!range) range=static_cast<TFunctionValueAttribute_range*>(FunctionValue_allocate(sizeof(*range)));
    if (range) {
        f64 nan=sJStudioQuietNaN;
        range->mProgress=0; range->fEnd_=nan; range->progressOrigin=nan; range->mAdjust=0;
        range->progressScale=nan; range->mBegin=0; range->fDifference_=nan; range->mEnd=0; range->fBegin_=nan;
    }
    return range;
}

/* 02848C68 */
TFunctionValue_composite* TFunctionValue_composite::construct(TFunctionValue_composite* value) {
    auto frameScope = gabi::AutoFrame(gabi::current_function());
    if (!value) value=static_cast<TFunctionValue_composite*>(FunctionValue_allocate(sizeof(*value)));
    if (value) {
        TFunctionValue::construct(value);
        TFunctionValueAttribute_refer* refer=&value->refer;
        if (!refer) refer=static_cast<TFunctionValueAttribute_refer*>(FunctionValue_allocate(sizeof(*refer)));
        if (refer) { gabi::FrameLocal<u32> allocator(8); FunctionValue_constructRefer(refer,&*allocator); }
        value->data.uintdata=0;
        value->__vtbl=reinterpret_cast<FunctionValueVtable*>(const_cast<VtableStorage*>(&sCompositeVtable));
        value->pfn_.v=0;
    }
    return value;
}

/* 02848CE8 */
void TFunctionValue_composite::getAttributeSet(TFunctionValueAttributeSet* out,TFunctionValue_composite* value) {
    out->refer_=&value->refer; out->range_=nullptr; out->interpolate_=nullptr;
}

/* 02848D00 */
void TFunctionValue_composite::prepare() {}

/* 02848D04 */
f64 TFunctionValue_composite::getValue(f64 parameter) {
    u32 function=pfn_.get();
    return gabi::call_ptr<f64>(function,&refer,&data,parameter);
}

/* 02848D18 */
f64 TFunctionValue_composite::composite_raw(const TFunctionValueAttribute_refer* refer,const TData* data,f64 parameter) {
    gptr<TFunctionValue>* begin=refer->begin;
    u32 index=data->uintdata;
    if (!begin) return 0.0;
    gptr<TFunctionValue>* end=refer->end;
    u32 count=(s32)(u32(gabi::ea(end)-gabi::ea(begin)))>>2;
    if (index>=count) return 0.0;
    TFunctionValue* value=begin[index];
    return gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,parameter);
}

/* 02848D60 */
f64 TFunctionValue_composite::composite_index(const TFunctionValueAttribute_refer* refer,const TData* data,f64 parameter) {
    gptr<TFunctionValue>* begin=refer->begin;
    s32 count=begin?s32(u32(gabi::ea(refer->end.get())-gabi::ea(begin)))>>2:0;
    if (count<=1) return 0.0;
    TFunctionValue* first=begin[0];
    u32 function=first->__vtbl->getValue.get();
    --count;
    f64 selected=gabi::call_ptr<f64>(function,first,parameter);
    s32 index=gabi::ftoi(FunctionValue_floor(selected));
    switch (u32(data->uintdata)) {
    case 1: {
        auto result=FunctionValue_div(index,count);
        index=s32(result.r4);
        if (index<0) index+=count;
        break;
    }
    case 2:
        if (count==1) index=0;
        else {
            s32 period=count*2-2;
            auto result=FunctionValue_div(index,period);
            index=s32(result.r4);
            if (index<0) index+=period;
            if (index>=count) index=period-index;
        }
        break;
    default:
        if (index<0) index=0;
        else if (index>=count) index=count-1;
        break;
    }
    TFunctionValue* value=begin[u32(index)+1];
    return gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,parameter);
}

/* 02848F3C */
f64 TFunctionValue_composite::composite_parameter(const TFunctionValueAttribute_refer* refer,const TData* data,f64 parameter) {
    gptr<TFunctionValue>* begin=refer->begin;
    gptr<TFunctionValue>* end=refer->end;
    f64 result=parameter-f64(data->f64data);
    while (begin!=end) {
        TFunctionValue* value=*begin++;
        result=gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,result);
    }
    return result;
}

/* 02848FA0 */
f64 TFunctionValue_composite::composite_add(const TFunctionValueAttribute_refer* refer,const TData* data,f64 parameter) {
    gptr<TFunctionValue>* begin=refer->begin;
    gptr<TFunctionValue>* end=refer->end;
    f64 result=data->f64data;
    while (begin!=end) {
        TFunctionValue* value=*begin++;
        result+=gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,parameter);
    }
    return result;
}

/* 0284903C */
f64 TFunctionValue_composite::composite_subtract(const TFunctionValueAttribute_refer* refer,const TData* data,f64 parameter) {
    gptr<TFunctionValue>* begin=refer->begin;
    if (!begin) return 0.0;
    gptr<TFunctionValue>* end=refer->end;
    if ((s32(u32(gabi::ea(end)-gabi::ea(begin)))>>2)==0) return 0.0;
    TFunctionValue* value=*begin++;
    f64 result=gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,parameter);
    while (begin!=end) { value=*begin++; result-=gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,parameter); }
    return result-f64(data->f64data);
}

/* 02849150 */
f64 TFunctionValue_composite::composite_multiply(const TFunctionValueAttribute_refer* refer,const TData* data,f64 parameter) {
    gptr<TFunctionValue>* begin=refer->begin;
    gptr<TFunctionValue>* end=refer->end;
    f64 result=data->f64data;
    while (begin!=end) { TFunctionValue* value=*begin++; result*=gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,parameter); }
    return result;
}

/* 028491EC */
f64 TFunctionValue_composite::composite_divide(const TFunctionValueAttribute_refer* refer,const TData* data,f64 parameter) {
    gptr<TFunctionValue>* begin=refer->begin;
    if (!begin) return 0.0;
    gptr<TFunctionValue>* end=refer->end;
    if ((s32(u32(gabi::ea(end)-gabi::ea(begin)))>>2)==0) return 0.0;
    TFunctionValue* value=*begin++;
    f64 result=gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,parameter);
    while (begin!=end) { value=*begin++; result/=gabi::call_ptr<f64>(value->__vtbl->getValue.get(),value,parameter); }
    return result/f64(data->f64data);
}

/* 02849300 */
TFunctionValue_constant* TFunctionValue_constant::construct(TFunctionValue_constant* value) {
    if (!value) value=static_cast<TFunctionValue_constant*>(FunctionValue_allocate(sizeof(*value)));
    if (value) { TFunctionValue::construct(value); value->__vtbl=reinterpret_cast<FunctionValueVtable*>(const_cast<VtableStorage*>(&sConstantVtable)); value->fValue_=f64(sJStudioQuietNaN); }
    return value;
}

/* 02849360 */
void TFunctionValue_constant::getAttributeSet(TFunctionValueAttributeSet* out,TFunctionValue_constant*) {
    out->refer_=nullptr; out->range_=nullptr; out->interpolate_=nullptr;
}

/* 02849374 */
f64 TFunctionValue_constant::getValue(f64) { return fValue_; }

/* 0284937C */
TFunctionValue_transition* TFunctionValue_transition::construct(TFunctionValue_transition* value) {
    if (!value) value=static_cast<TFunctionValue_transition*>(FunctionValue_allocate(sizeof(*value)));
    if (value) {
        TFunctionValue::construct(value); TFunctionValueAttribute_range::construct(&value->range);
        value->interpolate=0; value->__vtbl=reinterpret_cast<FunctionValueVtable*>(const_cast<VtableStorage*>(&sTransitionVtable));
        f64 nan=sJStudioQuietNaN; value->valueEnd=nan; value->valueBegin=nan;
    }
    return value;
}

/* 028493F0 */
void TFunctionValue_transition::getAttributeSet(TFunctionValueAttributeSet* out,TFunctionValue_transition* value) {
    out->refer_=nullptr; out->range_=&value->range; out->interpolate_=&value->interpolate;
}

/* 0284940C */
void TFunctionValue_transition::prepare() { range.range_prepare(); }

namespace JStudio { namespace functionvalue {
/* 02849414 */
f64 interpolateValue_hermite(f64 parameter,f64 begin,f64 valueBegin,f64 tangentBegin,f64 end,f64 valueEnd,f64 tangentEnd) {
    f64 delta=parameter-begin;
    f64 factor=delta/(end-begin);
    f64 blend=gabi::fmadd(factor,-2.0,3.0);
    f64 inverse=factor-1.0;
    f64 square=factor*factor;
    f64 inverseSquare=inverse*inverse;
    f64 weight=blend*square;
    f64 tangent=inverseSquare*delta;
    f64 rest=1.0-weight;
    f64 last=weight*valueEnd;
    f64 cross=inverse*delta;
    f64 result=gabi::fmadd(rest,valueBegin,last);
    f64 outWeight=cross*factor;
    result=gabi::fmadd(tangent,tangentBegin,result);
    return gabi::fmadd(outWeight,tangentEnd,result);
}

}} // namespace JStudio::functionvalue
using namespace JStudio::functionvalue;


/* 02849470 */
f64 TFunctionValue_transition::getValue(f64 parameter) {
    f64 origin=range.progressOrigin;
    f64 progress=gabi::fmadd(parameter-origin,f64(range.progressScale),origin);
    f64 begin=range.fBegin_;
    f64 outside=progress-begin;
    if (outside<0.0) {
        u32 function=TFunctionValue::toFunction_outside(range.mBegin);
        outside=gabi::call_ptr<f64>(function,outside,f64(range.fDifference_));
        begin=range.fBegin_;
    } else {
        f64 difference=range.fDifference_;
        if (!(outside<difference)) {
            u32 function=TFunctionValue::toFunction_outside(range.mEnd);
            outside=gabi::call_ptr<f64>(function,outside,difference);
            begin=range.fBegin_;
        }
    }
    s8 adjust=range.mAdjust;
    f64 value=outside+begin;
    switch (adjust) {
    default: return value<begin?f64(valueBegin):f64(valueEnd);
    case 2: return value<f64(range.fEnd_)?f64(valueBegin):f64(valueEnd);
    case 3: return value<(begin+f64(range.fEnd_))*0.5?f64(valueBegin):f64(valueEnd);
    case 4:
        if (value<begin) return valueBegin;
        f64 end=range.fEnd_;
        if (!(value<end)) return valueEnd;
        switch (u32(interpolate)) {
        default: return value<(begin+end)*0.5?f64(valueBegin):f64(valueEnd);
        case 1: case 3: {
            f64 low=valueBegin, high=valueEnd;
            return (value-begin)*(high-low)/f64(range.fDifference_)+low;
        }
        case 2: return functionvalue::interpolateValue_hermite(value,begin,valueBegin,0.0,end,valueEnd,0.0);
        }
    }
}

/* 0284966C */
TFunctionValue_list* TFunctionValue_list::construct(TFunctionValue_list* value) {
    if (!value) value=static_cast<TFunctionValue_list*>(FunctionValue_allocate(sizeof(*value)));
    if (value) {
        TFunctionValue::construct(value); TFunctionValueAttribute_range::construct(&value->range);
        value->pfData_=nullptr; value->uData_=0;
        value->__vtbl=reinterpret_cast<FunctionValueVtable*>(const_cast<VtableStorage*>(&sListVtable));
        value->interpolate=0; f64 nan=sJStudioQuietNaN; value->pfnUpdate_.v=0; value->interval=nan;
    }
    return value;
}

/* 028496E8 */
void TFunctionValue_list::getAttributeSet(TFunctionValueAttributeSet* out,TFunctionValue_list* value) {
    out->refer_=nullptr; out->range_=&value->range; out->interpolate_=&value->interpolate;
}

/* 02849704 */
f64 TFunctionValue_list::update_INTERPOLATE_NONE_(const TFunctionValue_list* value,const TIndexData_* index) {
    u32 item=index->index; const be<f32>* data=value->pfData_; return data[item];
}

/* 02849718 */
f64 TFunctionValue_list::update_INTERPOLATE_LINEAR_(const TFunctionValue_list* value,const TIndexData_* index) {
    u32 item=index->index; const be<f32>* data=value->pfData_;
    f64 parameter=index->parameter; f64 begin=data[item]; f64 end=data[item+1];
    f64 indexParameter=index->indexParameter;
    return gabi::fmadd(end-begin,parameter-indexParameter,begin);
}

/* 02849748 */
f64 TFunctionValue_list::update_INTERPOLATE_PLATEAU_(const TFunctionValue_list* value,const TIndexData_* index) {
    u32 item=index->index; const be<f32>* data=value->pfData_;
    f64 begin=data[item]; f64 indexParameter=index->indexParameter;
    return functionvalue::interpolateValue_hermite(index->parameter,indexParameter,begin,0.0,indexParameter+1.0,data[item+1],0.0);
}

namespace JStudio { namespace functionvalue {
/* 02849784 */
f64 interpolateValue_BSpline_uniform(f64 factor,f64 point2,f64 point3,f64 point4,f64 point5) {
    f64 square=factor*factor;
    f64 inverse=1.0-factor;
    f64 cube=square*factor;
    f64 sum=factor+square;
    f64 blend=gabi::fmsub(cube,0.5,square);
    f64 inverseSquare=inverse*inverse;
    f64 blendThird=blend+2.0/3.0;
    f64 inverseCube=inverseSquare*inverse;
    f64 difference=sum-cube;
    f64 last=cube*point5;
    f64 result=blendThird*point3;
    f64 weighted=gabi::fmadd(inverseCube,point2,last);
    f64 blendFourth=gabi::fmadd(difference,0.5,1.0/6.0);
    result=gabi::fmadd(weighted,1.0/6.0,result);
    return gabi::fmadd(blendFourth,point4,result);
}
}}

/* 028497E4 */
f64 TFunctionValue_list::update_INTERPOLATE_BSPLINE_dataMore3_(const TFunctionValue_list* value,const TIndexData_* index) {
    u32 item=index->index; const be<f32>* data=value->pfData_;
    f64 point3=data[item],point4=data[item+1];
    f64 point2,point5;
    if (item==0) { point2=(point3+point3)-point4; point5=data[2]; }
    else { point2=data[item-1]; if (item==u32(value->uData_)-2) point5=(point4+point4)-point3; else point5=data[item+2]; }
    return functionvalue::interpolateValue_BSpline_uniform(f64(index->parameter)-f64(index->indexParameter),point2,point3,point4,point5);
}

/* 02849860 */
void TFunctionValue_list::prepare() {
    range.range_prepare();
    switch (u32(interpolate)) {
    default: pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list::update_INTERPOLATE_NONE_)); break;
    case 1: pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list::update_INTERPOLATE_LINEAR_)); break;
    case 2: pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list::update_INTERPOLATE_PLATEAU_)); break;
    case 3:
        pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list::update_INTERPOLATE_BSPLINE_dataMore3_));
        if (uData_==2) pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list::update_INTERPOLATE_LINEAR_));
        break;
    }
}

/* 028498E8 */
f64 TFunctionValue_list::getValue(f64 parameter) {
    auto frameScope = gabi::AutoFrame(gabi::current_function());
    u32 last=u32(uData_)-1;
    // HD: interval is reloaded after callbacks; GHS inlined the range conversion here.
    f64 origin=range.progressOrigin;
    f64 progress=gabi::fmadd(parameter-origin,f64(range.progressScale),origin);
    s8 adjust=range.mAdjust;
    if (adjust==1) progress+=f64(range.fBegin_);
    else if (adjust==2) progress+=f64(range.fEnd_);
    else if (adjust==3) progress=gabi::fmadd(f64(range.fBegin_)+f64(range.fEnd_),0.5,progress);
    f64 begin=range.fBegin_;
    f64 outside=progress-begin;
    f64 difference;
    if (outside<0.0) {
        u32 function=TFunctionValue::toFunction_outside(range.mBegin);
        outside=gabi::call_ptr<f64>(function,outside,f64(range.fDifference_));
        begin=range.fBegin_;
        if (adjust==4) difference=range.fDifference_;
    } else {
        difference=range.fDifference_;
        if (!(outside<difference)) {
            u32 function=TFunctionValue::toFunction_outside(range.mEnd);
            outside=gabi::call_ptr<f64>(function,outside,difference);
            if (adjust==4) difference=range.fDifference_;
            begin=range.fBegin_;
        }
    }
    f64 coordinate=outside+begin;
    coordinate=adjust==4?(coordinate-begin)*f64(last)/difference:coordinate/f64(interval);
    if (coordinate<0.0) return pfData_.get()[0];
    if (!(coordinate<f64(last))) return pfData_.get()[last];
    f64 integral=FunctionValue_floor(coordinate);
    u32 index=integral<2147483648.0?u32(gabi::ftoi(integral)):u32(gabi::ftoi(integral-2147483648.0))+0x80000000u;
    u32 function=pfnUpdate_.get();
    gabi::FrameLocal<TIndexData_> data(0x10);
    data->parameter=coordinate; data->indexParameter=integral; data->index=index;
    return gabi::call_ptr<f64>(function,this,&*data);
}

/* 02849C04 */
TFunctionValue_list_parameter* TFunctionValue_list_parameter::construct(TFunctionValue_list_parameter* value) {
    if (!value) value=static_cast<TFunctionValue_list_parameter*>(FunctionValue_allocate(sizeof(*value)));
    if (value) {
        TFunctionValue::construct(value); TFunctionValueAttribute_range::construct(&value->range);
        value->dat2=nullptr; value->__vtbl=reinterpret_cast<FunctionValueVtable*>(const_cast<VtableStorage*>(&sParameterVtable));
        value->pfnUpdate_.v=0; value->interpolate=0; value->pfData_=nullptr; value->dat3=nullptr; value->dat1=nullptr; value->uData_=0;
    }
    return value;
}

/* 02849C80 */
void TFunctionValue_list_parameter::getAttributeSet(TFunctionValueAttributeSet* out,TFunctionValue_list_parameter* value) {
    out->refer_=nullptr; out->range_=&value->range; out->interpolate_=&value->interpolate;
}

/* 02849C9C */
void TFunctionValue_list_parameter::data_set(const be<f32>* data,u32 count) {
    pfData_=data; uData_=count; dat1=data; dat2=data+u32(count*2); dat3=data;
}

/* 02849CBC */
f64 TFunctionValue_list_parameter::update_INTERPOLATE_NONE_(const TFunctionValue_list_parameter* value,f64) { return value->dat3.get()[-1]; }

/* 02849CC8 */
f64 TFunctionValue_list_parameter::update_INTERPOLATE_LINEAR_(const TFunctionValue_list_parameter* value,f64 parameter) {
    const be<f32>* data=value->dat3;
    f64 high=data[1],low=data[-1],begin=data[-2],end=data[0];
    return low+(high-low)*(parameter-begin)/(end-begin);
}

/* 02849CF8 */
f64 TFunctionValue_list_parameter::update_INTERPOLATE_PLATEAU_(const TFunctionValue_list_parameter* value,f64 parameter) {
    const be<f32>* data=value->dat3;
    f64 begin=data[-2],end=data[0],high=data[1],low=data[-1];
    return functionvalue::interpolateValue_hermite(parameter,begin,low,0.0,end,high,0.0);
}

namespace JStudio { namespace functionvalue {
/* 02849D1C */
f64 interpolateValue_BSpline_nonuniform(const SplinePoints* points,const SplineKnots* knots,f64 parameter) {
    f64 knot4=knots->values[4],knot3=knots->values[3],knot2=knots->values[2],knot1=knots->values[1];
    f64 diff4=knot4-parameter, diff2=parameter-knot2;
    f64 inverse=1.0/(knot3-knot2);
    f64 knot5=knots->values[5];
    f64 factor2=(diff2*inverse)/(knot4-knot2);
    f64 diff3=knot3-parameter;
    f64 factor3=(diff3*inverse)/(knot3-knot1);
    f64 middle=gabi::fmadd(parameter-knot1,factor3,diff4*factor2);
    f64 knot0=knots->values[0];
    f64 begin=diff3*factor3;
    f64 factor4=middle/(knot4-knot1);
    f64 blend4=diff4*factor4;
    f64 factor1=begin/(knot3-knot0);
    f64 term2=gabi::fmadd(parameter-knot0,factor1,blend4);
    f64 factor5=(diff2*factor2)/(knot5-knot2);
    f64 term3=gabi::fmadd(parameter-knot1,factor4,(knot5-parameter)*factor5);
    f64 result=term2*f64(points->values[1]);
    result=gabi::fmadd(diff3*factor1,f64(points->values[0]),result);
    result=gabi::fmadd(term3,f64(points->values[2]),result);
    return gabi::fmadd(diff2*factor5,f64(points->values[3]),result);
}
}}

/* 02849E20 */
f64 TFunctionValue_list_parameter::update_INTERPOLATE_BSPLINE_dataMore3_(const TFunctionValue_list_parameter* value,f64 parameter) {
    auto frameScope = gabi::AutoFrame(gabi::current_function());
    const be<f32>* data=value->dat3;
    const be<f32>* first=value->dat1;
    const be<f32>* end=value->dat2;
    u32 before=u32(gabi::ea(data)-gabi::ea(first))>>2;
    u32 after=u32(gabi::ea(end)-gabi::ea(data))>>2;
    gabi::FrameLocal<functionvalue::SplinePoints> points(8);
    gabi::FrameLocal<functionvalue::SplineKnots> knots(0x28);
    f64 low=data[-1]; points->values[1]=low;
    f64 high=data[1]; points->values[2]=high;
    f64 begin=data[-2]; knots->values[2]=begin;
    f64 finish=data[0]; knots->values[3]=finish;
    if (before==2) {
        points->values[0]=(low+low)-high;
        f64 nextHigh=data[3]; points->values[3]=nextHigh;
        f64 twice=begin+begin;
        knots->values[1]=twice-finish;
        f64 next=data[2]; knots->values[4]=next;
        knots->values[0]=twice-next;
        if (after==2 || after==4) knots->values[5]=(next+next)-finish;
        else knots->values[5]=f64(data[4]);
    } else {
        points->values[0]=f64(data[-3]);
        f64 previous=data[-4]; knots->values[1]=previous;
        if (before==4) knots->values[0]=(previous+previous)-begin;
        else knots->values[0]=f64(data[-6]);
        if (after==2) {
            points->values[3]=(high+high)-low;
            f64 twice=finish+finish;
            knots->values[4]=twice-begin; knots->values[5]=twice-previous;
        } else {
            points->values[3]=f64(data[3]);
            f64 next=data[2]; knots->values[4]=next;
            if (after==4) knots->values[5]=(next+next)-finish;
            else knots->values[5]=f64(data[4]);
        }
    }
    return functionvalue::interpolateValue_BSpline_nonuniform(&*points,&*knots,parameter);
}

/* 02849FD8 */
void TFunctionValue_list_parameter::prepare() {
    range.range_prepare();
    switch (u32(interpolate)) {
    default: pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list_parameter::update_INTERPOLATE_NONE_)); break;
    case 1: pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list_parameter::update_INTERPOLATE_LINEAR_)); break;
    case 2: pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list_parameter::update_INTERPOLATE_PLATEAU_)); break;
    case 3:
        pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list_parameter::update_INTERPOLATE_BSPLINE_dataMore3_));
        if (uData_==2) pfnUpdate_.v=gabi::guest_fn(reinterpret_cast<const void*>(&TFunctionValue_list_parameter::update_INTERPOLATE_LINEAR_));
        break;
    }
}

// JGadget's upper-bound search expands by eight around the cached iterator before binary search.
static const be<f32>* upperBoundAll(const be<f32>* first,const be<f32>* last,u32 stride,f64 parameter) {
    s32 count=s32(u32(gabi::ea(last)-gabi::ea(first))) / s32(stride*4);
    while (count>0) {
        s32 half=count/2;
        const be<f32>* middle=first+u32(half)*stride;
        if (parameter<f64(*middle)) count=half;
        else { first=middle+stride; count-=half+1; }
    }
    return first;
}
static const be<f32>* upperBoundCurrent(const be<f32>* first,const be<f32>* last,const be<f32>* current,u32 stride,f64 parameter) {
    if (current==last || parameter<f64(*current)) {
        if (first==current) return current;
        const be<f32>* end=current-stride;
        u32 count=u32(gabi::ea(end)-gabi::ea(first))/(stride*4);
        u32 step=1;
        const be<f32>* item=end;
        while (true) {
            if (!(parameter<f64(*item))) { if (step==1) return item+stride; break; }
            end=item;
            s32 remaining=s32(count-step);
            if (remaining<=0) { item=first; break; }
            count=u32(remaining); item-=step*stride; step<<=3;
        }
        return upperBoundAll(item,end+stride,stride,parameter);
    }
    if (current==last) return last;
    u32 count=u32(gabi::ea(last)-gabi::ea(current))/(stride*4),step=1;
    const be<f32>* item=current;
    while (true) {
        if (parameter<f64(*item)) { if (step==1) return item; break; }
        current=item;
        s32 remaining=s32(count-step);
        if (remaining<=0) { item=last; break; }
        count=u32(remaining); item+=step*stride; step<<=3;
    }
    return upperBoundAll(current,item,stride,parameter);
}

/* 0284A060 */
f64 TFunctionValue_list_parameter::getValue(f64 parameter) {
    u32 count=uData_;
    const be<f32>* data=pfData_;
    f64 begin=data[0],end=data[u32(count*2)-2];
    parameter=range.range_getParameter(parameter,begin,end);
    const be<f32>* finish=dat2;
    const be<f32>* current=dat3;
    const be<f32>* first=dat1;
    current=upperBoundCurrent(first,finish,current,2,parameter);
    dat3=current;
    if (current==dat1.get()) return current[1];
    if (current==dat2.get()) { current-=2; dat3=current; return current[1]; }
    return gabi::call_ptr<f64>(pfnUpdate_.get(),this,parameter);
}

/* 0284A2C4 */
TFunctionValue_hermite* TFunctionValue_hermite::construct(TFunctionValue_hermite* value) {
    if (!value) value=static_cast<TFunctionValue_hermite*>(FunctionValue_allocate(sizeof(*value)));
    if (value) {
        TFunctionValue::construct(value); TFunctionValueAttribute_range::construct(&value->range);
        value->dat1.pf_=nullptr; value->__vtbl=reinterpret_cast<FunctionValueVtable*>(const_cast<VtableStorage*>(&sHermiteVtable));
        value->dat3.pf_=nullptr; value->u_=0; value->dat3.uSize_=0; value->pfData_=nullptr;
        value->dat2.pf_=nullptr; value->dat2.uSize_=0; value->uSize_=0; value->dat1.uSize_=0;
    }
    return value;
}

/* 0284A348 */
void TFunctionValue_hermite::getAttributeSet(TFunctionValueAttributeSet* out,TFunctionValue_hermite* value) {
    out->refer_=nullptr; out->range_=&value->range; out->interpolate_=nullptr;
}

/* 0284A360 */
void TFunctionValue_hermite::data_set(const be<f32>* data,u32 count,u32 size) {
    pfData_=data; dat2.uSize_=size; dat1.uSize_=size; dat3.pf_=data; dat1.pf_=data;
    dat3.uSize_=size; dat2.pf_=data+u32(count*size); u_=count; uSize_=size;
}

/* 0284A394 */
void TFunctionValue_hermite::prepare() { range.range_prepare(); }

/* 0284A39C */
f64 TFunctionValue_hermite::getValue(f64 parameter) {
    u32 last=u32(u_)-1; u32 size=uSize_;
    const be<f32>* data=pfData_;
    f64 begin=data[0],end=data[u32(last*size)];
    parameter=range.range_getParameter(parameter,begin,end);
    u32 currentStride=dat3.uSize_,lastStride=dat2.uSize_;
    const be<f32>* finish=dat2.pf_;
    const be<f32>* current=dat3.pf_;
    u32 firstStride=dat1.uSize_;
    const be<f32>* first=dat1.pf_;
    // HD: valid iterators carry the same stride; GHS keeps the selected iterator's stride.
    bool backwards=current==finish || parameter<f64(*current);
    u32 stride=backwards?currentStride:lastStride;
    const be<f32>* selected=upperBoundCurrent(first,finish,current,stride,parameter);
    if (selected!=current && backwards) stride=firstStride;
    dat3.pf_=selected; dat3.uSize_=stride;
    if (selected==dat1.pf_.get()) return selected[1];
    if (selected==dat2.pf_.get()) { selected-=stride; dat3.pf_=selected; return selected[1]; }
    u32 actualStride=uSize_;
    const be<f32>* previous=selected-actualStride;
    f64 high=selected[1],lowTangent=selected[-1],highTangent=selected[2];
    f64 left=previous[0],low=previous[1],right=selected[0];
    return functionvalue::interpolateValue_hermite(parameter,left,low,lowTangent,right,high,highTangent);
}

f64 FunctionValue_fmod(f64,f64);
namespace JStudio { namespace functionvalue {
/* 0284A73C */
f64 extrapolateParameter_turn(f64 value,f64 maximum) {
    f64 period=maximum+maximum;
    f64 result=FunctionValue_fmod(value,period);
    if (result<0.0) result+=period;
    if (!(result<maximum)) result=period-result;
    return result;
}




}}

namespace {
struct functionvalue_StaticBlock { be<u32> words[4]; };
static functionvalue_StaticBlock sfunctionvalueStaticBlock;
static u32 sfunctionvalueStaticRegister;
}

/* 0284A7B4 */
static void __sinit_functionvalue_cpp() {
    sfunctionvalueStaticBlock.words[2]=0; sfunctionvalueStaticBlock.words[0]=0; sfunctionvalueStaticBlock.words[3]=0; sfunctionvalueStaticBlock.words[1]=0;
    FunctionValue_register(&sfunctionvalueStaticRegister);
}






/* WWHD glue:
data sBaseVtable 1017B10C
data sCompositeVtable 1017B1C4
data sConstantVtable 1017B1FC
data sTransitionVtable 1017B234
data sListVtable 1017B26C
data sParameterVtable 1017B2A4
data sHermiteVtable 1017B2DC
data gapfnExtrapolateParameter_ 101FC774
ext 0273AD10 void* FunctionValue_allocate(u32)
ext 0273AF40 void FunctionValue_delete(void*)
ext 028434D0 void FunctionValue_constructRefer(TFunctionValueAttribute_refer*,const void*)
ext 028F3B98 f64 FunctionValue_floor(f64)
ext 028F35A8 Pair32 FunctionValue_div(s32,s32)
data sfunctionvalueStaticBlock 104B66E4
data sfunctionvalueStaticRegister 101FC768
ext 028F3AB8 f64 FunctionValue_fmod(f64,f64)
ext 028F026C void FunctionValue_register(void*)
*/
