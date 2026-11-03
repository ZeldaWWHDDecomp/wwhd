// Storage of the singleton and disposer words of agl::detail::ShaderHolder (AGL#5) and of AGL#6's timing
// history (data only; one definition of each word).
#include "agl/TimingHistory_l.h"
#include "agl/ShaderHolder.h"
static agl::detail::ShaderHolder* sShaderHolder;
static sead::Disposer* sShaderHolderDisposer;
namespace agl_history_l {
static TimingHistory* sTimingOwner;
static sead::Disposer* sTimingDisposer;
}
/* WWHD glue
data sShaderHolder 101F8D6C
data sShaderHolderDisposer 101F8D70
data sTimingOwner 101F8D74
data sTimingDisposer 101F8D78
*/
