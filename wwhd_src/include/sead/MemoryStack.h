/* WWHD sead memory helper: whether an address lies on the stack of a thread the thread manager knows.
 * From this game's instructions. */
#pragma once
#include "sead/Thread.h"

namespace sead {
namespace memory {
bool isStackAddress(const void* address);
}
}

