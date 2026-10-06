#pragma once
#include "gabi.h"
#include <initializer_list>
using namespace gabi;
static inline u32 U(u32 p,u32 n=0){return load<u32>(p+n);}
