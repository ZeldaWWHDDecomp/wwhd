/** HD message tag processor and text-to-pane helpers. */
#include "d/hd_text_writer.h"

static hdMessageData_c l_writerVtable;
static hdMessageHeaderStatics_c l_writerHeaderStatics;
static hdMessageStaticRecords_c l_writerRecords;
static f32 l_writerInitialFirst;
static f32 l_writerInitialSecond;
static u16 l_writerTerminator = 0;
static u32 l_writerTypeGuard;
static u32 l_writerBaseTypeGuard;
static u32 l_writerTypeInfo;
static u32 l_writerBaseTypeInfo;

// HD: These entry points retain their verified full-register argument types.
/* 026025CC */
u32 hdMessageTagProcessor_construct(u32 self) {
    if (!self) {
        self = gabi::ea(hdMessage_alloc(sizeof(hdMessageTagProcessor_c)));
        if (!self) return 0;
    }
    auto* processor = gabi::at<hdMessageTagProcessor_c>(self);
    hdMessage_baseConstruct(processor);
    processor->__vtbl = gabi::ea(&l_writerVtable);
    hdMessage_builderConstruct(&processor->mBuilder);
    hdMessage_rubyConstruct(&processor->mRubyState);
    hdMessage_tagConstruct(&processor->mTagState);
    processor->mMessageArgument = 0;
    return self;
}

static u32 findTag(u32 text, u32& next) {
    u32 tag = text - 2;
    u32 code = hdMessage_character(tag);
    if (code == hdMessageTag_c::BEGIN_TAG) {
        next = tag + hdMessage_tag(tag)->mParamSize + sizeof(hdMessageTag_c);
        return tag;
    }
    if (code == hdMessageTag_c::END_TAG) {
        next = tag + 6;
        return tag;
    }
    return 0;
}

/* 02602640 */
u32 hdMessageTagProcessor_Process(u32 self, u32 code, u32 context) {
    auto* processor = gabi::at<hdMessageTagProcessor_c>(self);
    auto* printContext = gabi::at<hdMessageContext_c>(context);
    u32 next = 0;
    u32 tag = findTag(gabi::ea(printContext->mpText.get()), next);
    if (!tag) return hdMessage_baseProcess(processor, code, printContext);
    printContext->mpText = gabi::at<be<u16>>(next);
    // HD: rlwimi rebuilds r8 for begin tags; it is a verifier live-in, not a call argument.
    if (hdMessage_character(tag) == hdMessageTag_c::BEGIN_TAG) gabi::cpu->r[8] = next - 8;
    return hdMessage_rubyProcess(&processor->mRubyState, printContext, tag, next);
}

/* 02602694 */
u32 hdMessageTagProcessor_CalcRect(u32 self, u32 rect, u32 code, u32 context) {
    auto* processor = gabi::at<hdMessageTagProcessor_c>(self);
    auto* printContext = gabi::at<hdMessageContext_c>(context);
    u32 next = 0;
    u32 tag = findTag(gabi::ea(printContext->mpText.get()), next);
    if (!tag) return hdMessage_baseCalcRect(processor, rect, code, printContext);
    printContext->mpText = gabi::at<be<u16>>(next);
    // HD: The corresponding rectangle path leaves the reconstructed value in r9.
    if (hdMessage_character(tag) == hdMessageTag_c::BEGIN_TAG) gabi::cpu->r[9] = next - 8;
    return hdMessage_tagCalcRect(&processor->mTagState, rect, printContext, tag, next);
}

/* 026026E8 */
void hdMessageTagProcessor_reset(u32 self) {
    auto* processor = gabi::at<hdMessageTagProcessor_c>(self);
    hdMessage_rubyReset(&processor->mRubyState);
    hdMessage_tagReset(&processor->mTagState);
}

/* 02602720 */
u32 hdMessageTagProcessor_begin(u32 self, u32 a, u32 b, u32 c, u32 d) {
    auto* processor = gabi::at<hdMessageTagProcessor_c>(self);
    processor->mMessageArgument = c;
    hdMessageTagProcessor_reset(self);
    return hdMessage_setup(&processor->mBuilder, a, b, c, d);
}

/* 02602778 */
void hdMessageTagProcessor_countText(u32 self, u32 chars, u32 lines, u32 text, s32 bytes) {
    s32 length = bytes / 2;
    u32 cursor = text;
    s32 lineCount = 1, charCount = 0;
    while ((s32)(cursor - text) >> 1 < length) {
        u32 code = hdMessage_character(cursor);
        if (code == hdMessageTag_c::BEGIN_TAG || code == hdMessageTag_c::END_TAG) {
            u32 tag = cursor;
            cursor = code == hdMessageTag_c::BEGIN_TAG ?
                cursor + hdMessage_tag(cursor)->mParamSize + sizeof(hdMessageTag_c) : cursor + 6;
            if (tag) continue;
        }
        if (hdMessage_character(cursor) == hdMessageTag_c::NEW_LINE) {
            cursor += 2;
            lineCount++;
            continue;
        }
        cursor += 2;
        charCount++;
    }
    if (chars) *gabi::at<be<u32>>(chars) = (u32)charCount;
    if (lines) *gabi::at<be<u32>>(lines) = (u32)lineCount;
}

/* 02602834 */
u32 hdMessageTagProcessor_splitText(u32 self, u32 list, u32 text, s32 bytes) {
    auto* panes = gabi::at<hdMessagePaneList_c>(list);
    u32 count = 0;
    u32 iterator = gabi::ea(panes->mpPanes.get());
    u32 end = iterator + 4 * panes->mCount;
    u32 cursor = text;
    s32 length = bytes / 2;
    if (iterator == end) return 0;
    while ((s32)(cursor - text) >> 1 < length) {
        u32 code = hdMessage_character(cursor);
        u32 tag = 0;
        bool isTag = false;
        if (code == hdMessageTag_c::BEGIN_TAG || code == hdMessageTag_c::END_TAG) {
            tag = cursor;
            cursor = code == hdMessageTag_c::BEGIN_TAG ?
                cursor + hdMessage_tag(cursor)->mParamSize + sizeof(hdMessageTag_c) : cursor + 6;
            isTag = tag != 0;
        }
        u32 paneLength;
        if (isTag) {
            u32 group = hdMessage_tag(tag)->mGroup;
            // Layout/control groups advance the text without consuming a pane.
            if ((group >= 1 && group <= 2) || (group >= 4 && group <= 6)) {
                if (iterator == end) return count;
                continue;
            }
            auto* pane = hdMessage_pane(iterator);
            paneLength = (u32)((s32)(hdMessage_tag(tag)->mParamSize + 8) / 2);
            pane->copyString(tag, 0, paneLength & 0xFFFF);
            if ((s32)paneLength <= 0) {
                if (iterator == end) return count;
                continue;
            }
        } else {
            if (hdMessage_character(cursor) == hdMessageTag_c::NEW_LINE) {
                cursor += 2;
                if (iterator == end) return count;
                continue;
            }
            auto* pane = hdMessage_pane(iterator);
            pane->copyString(cursor, 0, 1);
            cursor += 2;
            paneLength = 1;
        }
        // HD: Virtual copies can replace the current pane; reload it after each call.
        auto* pane = hdMessage_pane(iterator);
        pane->copyString(gabi::ea(&l_writerTerminator), paneLength & 0xFFFF, 1);
        auto* output = hdMessage_pane(iterator);
        output->mFlags = (u8)((output->mFlags & ~hdMessagePane_c::VISIBLE) + hdMessagePane_c::VISIBLE);
        iterator += 4;
        count++;
        if (iterator == end) return count;
    }
    do {
        auto* pane = hdMessage_pane(iterator);
        iterator += 4;
        pane->mFlags = pane->mFlags & ~hdMessagePane_c::VISIBLE;
    } while (iterator != end);
    return count;
}

// HD: Compiler-generated header statics and runtime type information.
/* 026029F0 */
void hdMessageTagProcessor_staticInit() {
    l_writerHeaderStatics.mZeroWords[2] = 0;
    l_writerHeaderStatics.mZeroWords[0] = 0;
    l_writerHeaderStatics.mZeroWords[3] = 0;
    l_writerHeaderStatics.mZeroWords[1] = 0;
    hdMessage_registerStatic(&l_writerRecords.mFirst);
    f32 first = l_writerInitialFirst, second = l_writerInitialSecond;
    l_writerHeaderStatics.mFloatPair[0] = first;
    l_writerHeaderStatics.mFloatPair[1] = second;
    hdMessage_initFlag(&l_writerHeaderStatics.mFlag);
    hdMessage_registerStatic(&l_writerRecords.mSecond);
    hdMessage_initByte(&l_writerHeaderStatics.mByte);
    hdMessage_registerStatic(&l_writerRecords.mThird);
}

/* 02602A84 */
u32 hdMessageTagProcessor_GetRuntimeTypeInfo() {
    if (l_writerTypeGuard) return gabi::ea(&l_writerTypeInfo);
    u32 baseGuard = l_writerBaseTypeGuard;
    l_writerTypeGuard = 1;
    if (!baseGuard) {
        l_writerBaseTypeGuard = 1;
        l_writerBaseTypeInfo = 0;
    }
    l_writerTypeInfo = gabi::ea(&l_writerBaseTypeInfo);
    return gabi::ea(&l_writerTypeInfo);
}

/* WWHD glue
ext 0273AD10 void* hdMessage_alloc(u32)
ext 0286F9F0 void hdMessage_baseConstruct(hdMessageTagProcessor_c*)
ext 025FA8FC void hdMessage_builderConstruct(hdMessageBuilder_c*)
ext 02601B60 void hdMessage_rubyConstruct(hdTextTagState_c*)
ext 025FA0F0 void hdMessage_tagConstruct(hdTextTagState_c*)
ext 0286FBDC u32 hdMessage_baseProcess(hdMessageTagProcessor_c*, u32, hdMessageContext_c*)
ext 0286FC44 u32 hdMessage_baseCalcRect(hdMessageTagProcessor_c*, u32, u32, hdMessageContext_c*)
ext 026024C8 u32 hdMessage_rubyProcess(hdTextTagState_c*, hdMessageContext_c*, u32, u32)
ext 025FA4CC u32 hdMessage_tagCalcRect(hdTextTagState_c*, u32, hdMessageContext_c*, u32, u32)
ext 026024F8 void hdMessage_rubyReset(hdTextTagState_c*)
ext 025FA50C void hdMessage_tagReset(hdTextTagState_c*)
ext 026017F0 u32 hdMessage_setup(hdMessageBuilder_c*, u32, u32, u32, u32)
ext 028F026C void hdMessage_registerStatic(void*)
ext 028ED6F8 void hdMessage_initFlag(void*)
ext 028EAB2C void hdMessage_initByte(void*)
data l_writerVtable 100E1820
data l_writerHeaderStatics 1048DCDC
data l_writerRecords 101F4EBC
data l_writerInitialFirst 100E1818
data l_writerInitialSecond 100E181C
data l_writerTerminator 10000162
data l_writerTypeGuard 101FDD54
data l_writerBaseTypeGuard 101FDD5C
data l_writerTypeInfo 101FDD58
data l_writerBaseTypeInfo 101FDD60
*/
