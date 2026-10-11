/** HD UI event records and input-state publication. No GameCube counterpart.
 * Private decomp source. */
#include "bindings.h"
#include "hd/hd_ui_02001084.h"

static void* sInputState;
enum : u32 {
    EVENT_BOX_VTABLE = 0x10000170,
    INPUT_STATE_HOLDER_VTABLE = 0x10000180,
};

// HD: member address arithmetic wraps at 32 bits, including other == 0xFFFFFFF8.
static inline void* eventPayload(hdUiEvent* event) {
    u32 address = gabi::ea(event) + offsetof(hdUiEvent, mPayload);
    return gabi::at<void>(address);
}
static hdUiEvent* constructUiEvent(hdUiEvent* i_this);

/* 02001084 */
static hdUiEventBox* constructUiEventBox(hdUiEventBox* i_this) {
    if (i_this == NULL) {
        i_this = (hdUiEventBox*)operator_new(sizeof(hdUiEventBox));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->field_0x0 = 0;
    i_this->field_0x4 = 0;
    constructUiEvent(&i_this->mEvent);
    i_this->field_0x30 = 0;
    i_this->mVtable = EVENT_BOX_VTABLE;
    return i_this;
}

/* 02001110 */
static void destroyUiEventBox(void* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02001124 */
static void __sinit_hd_ui_event_box_cpp() {
    sinit_header_statics_rodata(0x101FF07C, 0x1018C0E4, 0x101FF088, 0x10000168);
}

/* 020011B8 */
static void destroyInputStateHolder(void* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 020011CC */
static hdUiInputStateHolder* constructInputStateHolder(hdUiInputStateHolder* i_this, void* state) {
    if (i_this == NULL) {
        i_this = (hdUiInputStateHolder*)operator_new(sizeof(hdUiInputStateHolder));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->field_0x0 = 0;
    i_this->mVtable = INPUT_STATE_HOLDER_VTABLE;
    sInputState = state;
    return i_this;
}

/* 02001228 */
static void __sinit_hd_ui_input_state_cpp() {
    sinit_header_statics_rodata(0x101FF098, 0x1018C108, 0x101FF0A4, 0x10000190);
}

/* 020012BC */
static u32 clearEventPayload(hdUiEvent* i_this) {
    return hdUiBlockSet(eventPayload(i_this), 0, sizeof(i_this->mPayload));
}

/* 020012CC */
static hdUiEvent* constructUiEvent(hdUiEvent* i_this) {
    if (i_this == NULL) {
        i_this = (hdUiEvent*)operator_new(sizeof(hdUiEvent));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mKind = 0;
    i_this->mId = 0;
    clearEventPayload(i_this);
    return i_this;
}

/* 02001320 */
static void setEventPayload(hdUiEvent* i_this, const void* source) {
    if (source == NULL) {
        return;
    }
    hdUiBlockMove(eventPayload(i_this), source, sizeof(i_this->mPayload), 0);
}

/* 02001338 */
static hdUiEvent* constructUiEventWithPayload(hdUiEvent* i_this, s32 kind, s32 id, const void* payload) {
    if (i_this == NULL) {
        i_this = (hdUiEvent*)operator_new(sizeof(hdUiEvent));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mKind = kind;
    i_this->mId = id;
    if (payload != NULL) {
        setEventPayload(i_this, payload);
    } else {
        clearEventPayload(i_this);
    }
    return i_this;
}

/* 020013C0 */
static hdUiEvent* copyConstructUiEvent(hdUiEvent* i_this, hdUiEvent* other) {
    if (i_this == NULL) {
        i_this = (hdUiEvent*)operator_new(sizeof(hdUiEvent));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mKind = other->mKind;
    i_this->mId = other->mId;
    const void* payload = eventPayload(other);
    if (payload != NULL) {
        setEventPayload(i_this, payload);
    } else {
        clearEventPayload(i_this);
    }
    return i_this;
}

/* 0200143C */
static hdUiEvent* assignUiEvent(hdUiEvent* i_this, hdUiEvent* other) {
    i_this->mKind = other->mKind;
    i_this->mId = other->mId;
    setEventPayload(i_this, eventPayload(other));
    return i_this;
}

/* 02001480 */
static u32 clearUiEvent(hdUiEvent* i_this) {
    i_this->mKind = 0;
    i_this->mId = 0;
    return clearEventPayload(i_this);
}

/* 02001490 */
static void spinDelay() {
    // HD: the original 32-iteration empty loop has no observable effect.
}

/* 020014A0 */
static void __sinit_hd_ui_event_cpp() {
    sinit_header_statics_rodata(0x101FF0B4, 0x1018C130, 0x101FF0C0, 0x10000198);
}

/* 02001534 */
static void __sinit_hd_ui_event_adjacent_cpp() {
    // HD: adjacent header-statics-only translation unit; original filename unknown.
    sinit_header_statics_rodata(0x101FF0D0, 0x1018C154, 0x101FF0DC, 0x100001A0);
}

/* WWHD glue (tools/verify/autoglue.py)
 data sInputState 1018C12C
 ext C0009990 u32 hdUiBlockSet(void*, u32, u32)
 ext C0009988 void hdUiBlockMove(void*, const void*, u32, u32)
*/
