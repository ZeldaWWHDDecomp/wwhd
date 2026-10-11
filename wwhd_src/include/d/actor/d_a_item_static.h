/* Field-item static methods: private view of the shared daItem_c layout.
 * The complete layout remains in d_a_item.h for the other item units. */
#pragma once
#include "gc_api/gc_api.h"

struct daItem_c : daItemBase_c {
    enum Status {
        STATUS_UNK0 = 0,
        STATUS_WAIT_MAIN = 2,
        STATUS_BRING_NEZUMI = 3,
        STATUS_UNK4 = 4,
        STATUS_INIT_NORMAL = 5,
        STATUS_MAIN_NORMAL = 6,
    };
    // HD: daItemBase_c is 0x750 bytes; the status follows the item-specific fields.
    u8 _750[0x35];
    be<u8> mItemStatus;
    u8 _786[0x6A];

    BOOL checkActionNow();
    BOOL checkControl();
    BOOL startControl();
    BOOL endControl();
    BOOL checkLock();
    BOOL setLock();
    BOOL releaseLock();
};
WWHD_OFFSET(daItem_c, mItemStatus, 0x785);
WWHD_SIZE(daItem_c, 0x7F0);
