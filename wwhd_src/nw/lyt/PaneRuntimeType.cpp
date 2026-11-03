// The pane base class's runtime-type node and its guard, shared by every nw::lyt pane kind (their constructors
// initialise it inline), and the text box pane's node and guard; declared in nw/lyt/ResourceProvider.h.
#include "nw/lyt/ResourceProvider.h"
static nw::lyt::RuntimeTypeGuard sPaneTypeGuard;
static nw::lyt::RuntimeTypeNode sPaneType;
static nw::lyt::RuntimeTypeGuard sTextPaneTypeGuard;
static nw::lyt::RuntimeTypeNode sTextPaneType;
/* WWHD glue
data sPaneTypeGuard 101FD888
data sPaneType 101FD8FC
data sTextPaneTypeGuard 101FD89C
data sTextPaneType 101FD8F8
*/
