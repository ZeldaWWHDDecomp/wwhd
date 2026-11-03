// Text operations retained in WWHD's shadow pre-pass translation unit.
#include "agl/ShadowTextInstances.h"
#include "bindings.h"
using namespace agl::shadow_prepass_text;
static sead::SafeTextVtable sTextTable;

/* 027919BC */
void Text::destroy(u32 flags) {
    gabi::AutoFrame frame{0x027919BCu};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}

/* 027919FC */
Text* Text::create(Text* text, u32 contentAddress) {
    gabi::AutoFrame frame{0x027919FCu};
    if (text == nullptr) text = static_cast<Text*>(operator_new(sizeof(Text)));
    if (text != nullptr) {
        text->mText.v = contentAddress;
        text->mVtable = &sTextTable;
    }
    return text;
}

/* 02791A4C */
void Text::assureTermination() {}

/* WWHD glue
data sTextTable 101658B4
*/
