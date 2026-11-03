// Method-only view over the canonical render-matrix layout.
#pragma once
#include "sead/RenderMath.h"
namespace agl { namespace env {
struct ProjectionProduct : sead::RenderMatrix44 {
    void setProduct(const sead::RenderMatrix44* projection,const sead::RenderMatrix34* view);
};
}}
WWHD_SIZE(agl::env::ProjectionProduct,64);
