#pragma once
#include "gc_api/gc_api.h"
class daObjEayogn_c : public fopAc_ac_c {
public:
    // HD: actor construction also writes this TU's virtual-destructor table.
    daObjEayogn_c() { fopAc_ac_c_ct(this); __vtbl = 0x100278F4; }
    BOOL check_ev_bit() const;
    cPhs_State _create();
    bool _draw();
    bool _delete();
    BOOL create_heap();
    void init_mtx();
    static const char M_arcname[7];
    gptr<J3DModel> mpModel;
    request_of_phase_process_class mPhs;
    gptr<dBgW> mpBgW;
};
WWHD_OFFSET(daObjEayogn_c, mpModel, 0x3AC);
WWHD_OFFSET(daObjEayogn_c, mPhs, 0x3B0);
WWHD_OFFSET(daObjEayogn_c, mpBgW, 0x3B8);
WWHD_SIZE(daObjEayogn_c, 0x3BC);
enum { dRes_INDEX_EAYOGN_BDL_EAYOGN_e = 4, dRes_INDEX_EAYOGN_DZB_EAYOGN_e = 7, dSymbol_DIN_e = 1 };
inline BOOL daObjEayogn_c::check_ev_bit() const { return dComIfGs_isSymbol(dSymbol_DIN_e); }
