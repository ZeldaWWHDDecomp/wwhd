// JStudio value and sequence playback, using the tww JStudio template.
// Verified HD layout and dispatch order.
#include "JSystem/JStudio/JST_timeline.h"
using namespace gabi;
using namespace jst_timeline;

/* 02839D38 */
void jst_timeline::TVariableValue::update(f64 seconds, TAdaptor* adaptor) {
    u32 updateFunction = field_0x8.get();
    if (updateFunction) {
        invoke<void>(updateFunction, this, seconds);
        u32 output = gabi::ea(pOutput_.get());
        u32 fn = at<TOutput>(output)->__vtbl.get()->output.get();
        f32 value = f32(mValue);
        invoke<void>(fn, output, value, adaptor);
    }
}

/* 02839DA0 */
void TVariableValue_update_immediate_(TVariableValue* self, f64 seconds) {
    // HD: immediate assignment copies the word, preserving signalling NaN bits.
    u32 value = u32(self->field_0xc.bits);
    self->field_0x8.v = 0;
    self->mValueBits = value;
}

/* 02839DB4 */
void TVariableValue_update_time_(TVariableValue* self, f64 seconds) {
    // HD: retain double multiplication and its operand order until the final store.
    f64 time = static_cast<f64>(u32(self->field_0x4)) * seconds;
    f64 value = time * static_cast<f64>(f32(self->field_0xc.val));
    self->mValue = static_cast<f32>(value);
}

/* 02839DF4 */
void TVariableValue_update_functionValue_(TVariableValue* self, f64 seconds) {
    u32 age = u32(self->field_0x4);
    u32 fv = u32(self->field_0xc.bits);
    u32 fn = at<TFunctionValue>(fv)->__vtbl.get()->getValue.get();
    f64 value = invoke<f64>(fn, fv, static_cast<f64>(age) * seconds);
    self->mValue = static_cast<f32>(value);
}

/* 0283A48C */
void jst_timeline::TAdaptor::adaptor_updateVariableValue(TObject* object, u32 frames) {
    u32 count = u32(mCount);
    u32 value = ea(mVariableValues.get());
    // HD: array bounds use wrapping 32-bit guest addresses.
    u32 end = value + count * sizeof(TVariableValue);
    u32 control = ea(object->pControl.get());
    f64 seconds = f64(at<TControl>(control)->mSecondPerFrame);
    while (value != end) {
        u32 current = value;
        u32 age = u32(at<TVariableValue>(current)->field_0x4);
        value += sizeof(TVariableValue);
        at<TVariableValue>(current)->field_0x4 = ~age <= frames ? 0xFFFFFFFFu : age + frames;
        at<TVariableValue>(current)->update(seconds, this);
    }
}

/* 0283A5BC */
void jst_timeline::TObject::forward_value(u32 frames) {
    u32 adaptor = ea(mpAdaptor.get());
    if (adaptor) {
        at<TAdaptor>(adaptor)->adaptor_updateVariableValue(this, frames);
        u32 fn = at<TAdaptor>(adaptor)->__vtbl.get()->adaptor_do_update.get();
        invoke<void>(fn, adaptor, this, frames);
    }
}

/* 0283CCBC */
// HD: operation occupies the full argument register, unlike the GC u8 declaration.
void jst_timeline::stb::TObject::setFlag_operation(u32 operation, s32 value) {
    switch (operation) {
    case FLAG_OR:
        mFlag = u16(mFlag) | value;
        break;
    case FLAG_AND:
        mFlag = u16(mFlag) & value;
        break;
    case FLAG_XOR:
        mFlag = u16(mFlag) ^ value;
        break;
    }
}

/* 0283CD24 */
void jst_timeline::stb::TObject::process_paragraph_reserved_(u32 type, const void* pContent,
                                                             u32 size) {
    u32 content = ea(pContent);
    switch (type) {
    case CMD_FLAG: {
        u32 flags = u32(at<ParagraphContent>(content)->word);
        setFlag_operation(static_cast<u8>(flags >> 16), flags & 0xFFFF);
        break;
    }
    case CMD_WAIT:
        u32Wait_ = u32(at<ParagraphContent>(content)->word);
        break;
    case CMD_JUMP: {
        u32 seq = ea(pSequence.get());
        pSequence_next = at<const u8>(seq + u32(at<ParagraphContent>(content)->word));
        break;
    }
    case CMD_PARAGRAPH:
        transfer(__vtbl.get()->do_data.get(), this, u32(0), u32(0), content, size);
        break;
    case CMD_DATA_ID: {
        u32 idSize = u16(at<ParagraphContent>(content)->id.idSize);
        u32 fn = __vtbl.get()->do_data.get();
        u32 data = content + ((idSize + 3) & ~3u) + 4;
        transfer(fn, this, content + 4, idSize, data, size + content - data);
        break;
    }
    }
}

/* 0283CDE0 */
void jst_timeline::stb::TObject::process_sequence_() {
    // HD: parser cursors and records retain the original guest stack allocation order.
    AutoFrame frame{current_function()};
    Local<be<u32>> sequence;
    Local<SequenceData> data;
    *sequence = ea(pSequence.get());
    parseSequence(sequence.get(), data.get());
    u32 next = u32(data->next);
    u8 type = u8(data->type);
    pSequence_next = at<const u8>(next);
    u32 content = u32(data->content);
    u32 param = u32(data->param);
    switch (type) {
    case CMD_FLAG:
        setFlag_operation(static_cast<u8>(param >> 16), param & 0xFFFF);
        break;
    case CMD_WAIT:
        u32Wait_ = param;
        break;
    case CMD_JUMP:
        if (param & 0x800000)
            param |= 0xFF000000;
        pSequence_next = at<const u8>(ea(pSequence.get()) + param);
        break;
    case CMD_SUSPEND:
        if (param & 0x800000)
            param |= 0xFF000000;
        _20 = u32(_20) + param;
        break;
    case CMD_PARAGRAPH: {
        Local<be<u32>> paragraph;
        Local<ParagraphData> pData;
        while (content < next) {
            *paragraph = content;
            parseParagraph(paragraph.get(), pData.get());
            u32 pType = u32(pData->type);
            u32 pContent = u32(pData->content);
            u32 pSize = u32(pData->size);
            if (pType <= 0xFF)
                process_paragraph_reserved_(pType, at<const void>(pContent), pSize);
            else
                invoke<void>(__vtbl.get()->do_paragraph.get(), this, pType, pContent, pSize);
            content = u32(pData->next);
        }
        break;
    }
    }
}

/* 0283CF90 */
// HD: callback targets and state stores retain GHS ordering; GC debug assertions are absent.
bool jst_timeline::stb::TObject::forward(u32 frames) {
    bool waited = false;
    for (;;) {
        u16 flags = u16(mFlag);
        u32 status = u32(mStatus);
        if (flags & FLAG_INACTIVE) {
            if (status != STATUS_INACTIVE) {
                u8 active = u8(bSequence_);
                mStatus = STATUS_INACTIVE;
                if (active)
                    invoke<void>(__vtbl.get()->do_end.get(), this);
            }
            return true;
        }
        if (status == STATUS_INACTIVE) {
            invoke<void>(__vtbl.get()->do_begin.get(), this);
            mStatus = STATUS_WAIT;
        }
        u32 control = ea(pControl.get());
        if ((control && s32(at<TControl>(control)->_54) > 0) || s32(_20) > 0) {
            if (u8(bSequence_)) {
                u32 fn = __vtbl.get()->do_wait.get();
                mStatus = STATUS_SUSPEND;
                invoke<void>(fn, this, frames);
            }
            return true;
        }
        for (;;) {
            u32 next = ea(pSequence_next.get());
            pSequence = at<const u8>(next);
            u8 active = u8(bSequence_);
            if (!next) {
                if (active) {
                    if (!waited)
                        invoke<void>(__vtbl.get()->do_wait.get(), this, u32(0));
                    u32 table = ea(__vtbl.get());
                    mStatus = STATUS_END;
                    bSequence_ = 0;
                    invoke<void>(at<ObjectVTable>(table)->do_end.get(), this);
                }
                return false;
            }
            if (!active) {
                u32 table = ea(__vtbl.get());
                bSequence_ = 1;
                invoke<void>(at<ObjectVTable>(table)->do_begin.get(), this);
            }
            u32 wait = u32(u32Wait_);
            mStatus = STATUS_WAIT;
            if (!wait) {
                process_sequence_();
                wait = u32(u32Wait_);
                if (!wait)
                    break;
            }
            waited = true;
            u32 fn = __vtbl.get()->do_wait.get();
            if (frames >= wait) {
                u32Wait_ = 0;
                frames -= wait;
                invoke<void>(fn, this, wait);
            } else {
                u32Wait_ = wait - frames;
                invoke<void>(fn, this, frames);
                return true;
            }
        }
    }
}

/* 0283D514 */
bool jst_timeline::TControl::forward(u32 frames) {
    _54 = u32(mObject_control._20);
    bool result = mObject_control.forward(frames);
    u32 end = ea(&mObjectContainer);
    u32 node = ea(mObjectContainer.next.get());
    u32 all = 0xF, any = 0;
    while (node != end) {
        u32 object = node - 8;
        node = ea(at<TLinkListNode>(node)->next.get());
        bool current = at<stb::TObject>(object)->forward(frames);
        u32 status = u32(at<stb::TObject>(object)->mStatus);
        result = current || result;
        all &= status;
        any |= status;
    }
    mStatus = all | (any << 16);
    return result;
}

/* WWHD glue
ext 0283C9C4 void parseSequence(be<u32>*, jst_timeline::SequenceData*)
ext 0283CA0C void parseParagraph(be<u32>*, jst_timeline::ParagraphData*)
*/
