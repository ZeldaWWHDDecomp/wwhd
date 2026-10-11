/* HostIO registration API. HD disables registration; no object fields are accessed.
 * GameCube names follow m_Do_hostIO.h. */
#pragma once
#include "wwhd.h"

class JORReflexible;
class mDoHIO_subRoot_c {
public:
    s8 createChild(const char* i_name, JORReflexible* i_reflexible);
    void deleteChild(s8 i_childID);
};
