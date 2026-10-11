/* Draw queue iteration. */
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_tree.h"
#include "f_pc/f_pc_base.h"

create_tag_class* fopDwIt_Begin();
create_tag_class* fopDwIt_Next(create_tag_class*);
