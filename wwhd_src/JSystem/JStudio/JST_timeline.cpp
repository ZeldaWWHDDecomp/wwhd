// WWHD timeline reconstruction using the CC0 TWW JStudio template.
// Local offsets are confirmed from the HD binary.
#include "gabi.h"
namespace jst_timeline {
using namespace gabi;
static u32 objectVfn(u32 self, u32 slot) { return load<u32>(load<u32>(self + 0x30) + slot); }

void TVariableValue_update(u32 self, f64 seconds, u32 adaptor) {
    WWHD_FUNC(0x02839D38, void, self, seconds, adaptor);
    u32 update = load<u32>(self + 8);
    if (update) {
        call_ptr<void>(update, self, seconds);
        u32 output = load<u32>(self + 0x10);
        u32 fn = load<u32>(load<u32>(output) + 0xC);
        f32 value = load<f32>(self);
        call_ptr<void>(fn, output, value, adaptor);
    }
}
VERIFY(0x02839D38, TVariableValue_update);

void TVariableValue_update_immediate(u32 self, f64 seconds) {
    WWHD_FUNC(0x02839DA0, void, self, seconds);
    u32 value = load<u32>(self + 0xC);
    store<u32>(self + 8, 0);
    store<u32>(self, value);
}
VERIFY(0x02839DA0, TVariableValue_update_immediate);

void TVariableValue_update_time(u32 self, f64 seconds) {
    WWHD_FUNC(0x02839DB4, void, self, seconds);
    f64 time = static_cast<f64>(load<u32>(self + 4)) * seconds;
    f64 value = time * static_cast<f64>(load<f32>(self + 0xC));
    store<f32>(self, static_cast<f32>(value));
}
VERIFY(0x02839DB4, TVariableValue_update_time);

void TVariableValue_update_functionValue(u32 self, f64 seconds) {
    WWHD_FUNC(0x02839DF4, void, self, seconds);
    u32 age = load<u32>(self + 4);
    u32 fv = load<u32>(self + 0xC);
    u32 fn = load<u32>(load<u32>(fv) + 0x34);
    f64 value = call_ptr<f64>(fn, fv, static_cast<f64>(age) * seconds);
    store<f32>(self, static_cast<f32>(value));
}
VERIFY(0x02839DF4, TVariableValue_update_functionValue);

void TAdaptor_updateVariableValue(u32 self, u32 object, u32 frames) {
    WWHD_FUNC(0x0283A48C, void, self, object, frames);
    u32 count = load<u32>(self + 4);
    u32 value = load<u32>(self);
    u32 end = value + count * 0x14;
    u32 control = load<u32>(object + 0x10);
    f64 seconds = load<f64>(control + 0x58);
    while (value != end) {
        u32 current = value;
        u32 age = load<u32>(current + 4);
        value += 0x14;
        store<u32>(current + 4, ~age <= frames ? 0xFFFFFFFFu : age + frames);
        TVariableValue_update(current, seconds, self);
    }
}
VERIFY(0x0283A48C, TAdaptor_updateVariableValue);

void TObject_forward_value(u32 self, u32 frames) {
    WWHD_FUNC(0x0283A5BC, void, self, frames);
    u32 adaptor = load<u32>(self + 0x34);
    if (adaptor) {
        TAdaptor_updateVariableValue(adaptor, self, frames);
        u32 fn = load<u32>(load<u32>(adaptor + 8) + 0x2C);
        call_ptr<void>(fn, adaptor, self, frames);
    }
}
VERIFY(0x0283A5BC, TObject_forward_value);

void TObject_setFlag_operation(u32 self, u32 operation, s32 value) {
    WWHD_FUNC(0x0283CCBC, void, self, operation, value);
    switch (operation) {
    case 1: store<u16>(self + 0x18, load<u16>(self + 0x18) | value); break;
    case 2: store<u16>(self + 0x18, load<u16>(self + 0x18) & value); break;
    case 3: store<u16>(self + 0x18, load<u16>(self + 0x18) ^ value); break;
    }
}
VERIFY(0x0283CCBC, TObject_setFlag_operation);

void TObject_process_paragraph_reserved(u32 self, u32 type, u32 content, u32 size) {
    WWHD_FUNC(0x0283CD24, void, self, type, content, size);
    switch (type) {
    case 1: {
        u32 flags = load<u32>(content);
        TObject_setFlag_operation(self, static_cast<u8>(flags >> 16), flags & 0xFFFF);
        break;
    }
    case 2: store<u32>(self + 0x28, load<u32>(content)); break;
    case 3: {
        u32 seq = load<u32>(self + 0x20);
        store<u32>(self + 0x24, seq + load<u32>(content)); break;
    }
    case 0x80:
        tail_ptr<void>(objectVfn(self, 0x34), self, u32(0), u32(0), content, size);
        break;
    case 0x81: {
        u32 idSize = load<u16>(content + 2);
        u32 fn = objectVfn(self, 0x34);
        u32 data = content + ((idSize + 3) & ~3u) + 4;
        tail_ptr<void>(fn, self, content + 4, idSize, data, size + content - data);
        break;
    }
    }
}
VERIFY(0x0283CD24, TObject_process_paragraph_reserved);

void TObject_process_sequence(u32 self) {
    WWHD_FUNC(0x0283CDE0, void, self);
    struct Words { u8 bytes[16]; };
    Local<be<u32>> sequence;
    Local<Words> data;
    *sequence = load<u32>(self + 0x20);
    call<void>(0x0283C9C4, sequence.a, data.a);
    u32 next = load<u32>(data.a + 12);
    u8 type = load<u8>(data.a);
    store<u32>(self + 0x24, next);
    u32 content = load<u32>(data.a + 8);
    u32 param = load<u32>(data.a + 4);
    switch (type) {
    case 1: TObject_setFlag_operation(self, static_cast<u8>(param >> 16), param & 0xFFFF); break;
    case 2: store<u32>(self + 0x28, param); break;
    case 3:
        if (param & 0x800000) param |= 0xFF000000;
        store<u32>(self + 0x24, load<u32>(self + 0x20) + param); break;
    case 4:
        if (param & 0x800000) param |= 0xFF000000;
        store<u32>(self + 0x1C, load<u32>(self + 0x1C) + param); break;
    case 0x80: {
        Local<be<u32>> paragraph;
        Local<Words> pData;
        while (content < next) {
            *paragraph = content;
            call<void>(0x0283CA0C, paragraph.a, pData.a);
            u32 pType = load<u32>(pData.a);
            u32 pContent = load<u32>(pData.a + 8);
            u32 pSize = load<u32>(pData.a + 4);
            if (pType <= 0xFF)
                TObject_process_paragraph_reserved(self, pType, pContent, pSize);
            else call_ptr<void>(objectVfn(self, 0x24), self, pType, pContent, pSize);
            content = load<u32>(pData.a + 12);
        }
        break;
    }
    }
}
VERIFY(0x0283CDE0, TObject_process_sequence);

bool TObject_forward(u32 self, u32 frames) {
    WWHD_FUNC(0x0283CF90, bool, self, frames);
    bool waited = false;
    for (;;) {
        u16 flags = load<u16>(self + 0x18);
        u32 status = load<u32>(self + 0x2C);
        if (flags & 0x8000) {
            if (status != 8) {
                u8 active = load<u8>(self + 0x1A);
                store<u32>(self + 0x2C, 8);
                if (active) call_ptr<void>(objectVfn(self, 0x1C), self);
            }
            return true;
        }
        if (status == 8) {
            call_ptr<void>(objectVfn(self, 0x14), self);
            store<u32>(self + 0x2C, 2);
        }
        u32 control = load<u32>(self + 0x10);
        if ((control && load<s32>(control + 0x50) > 0) || load<s32>(self + 0x1C) > 0) {
            if (load<u8>(self + 0x1A)) {
                u32 fn = objectVfn(self, 0x2C);
                store<u32>(self + 0x2C, 4);
                call_ptr<void>(fn, self, frames);
            }
            return true;
        }
        for (;;) {
            u32 next = load<u32>(self + 0x24);
            store<u32>(self + 0x20, next);
            u8 active = load<u8>(self + 0x1A);
            if (!next) {
                if (active) {
                    if (!waited) call_ptr<void>(objectVfn(self, 0x2C), self, u32(0));
                    u32 table = load<u32>(self + 0x30);
                    store<u32>(self + 0x2C, 1);
                    store<u8>(self + 0x1A, 0);
                    call_ptr<void>(load<u32>(table + 0x1C), self);
                }
                return false;
            }
            if (!active) {
                u32 table = load<u32>(self + 0x30);
                store<u8>(self + 0x1A, 1);
                call_ptr<void>(load<u32>(table + 0x14), self);
            }
            u32 wait = load<u32>(self + 0x28);
            store<u32>(self + 0x2C, 2);
            if (!wait) {
                TObject_process_sequence(self);
                wait = load<u32>(self + 0x28);
                if (!wait) break;
            }
            waited = true;
            u32 fn = objectVfn(self, 0x2C);
            if (frames >= wait) {
                store<u32>(self + 0x28, 0);
                frames -= wait;
                call_ptr<void>(fn, self, wait);
            } else {
                store<u32>(self + 0x28, wait - frames);
                call_ptr<void>(fn, self, frames);
                return true;
            }
        }
    }
}
VERIFY(0x0283CF90, TObject_forward);

bool TControl_forward(u32 self, u32 frames) {
    WWHD_FUNC(0x0283D514, bool, self, frames);
    store<u32>(self + 0x50, load<u32>(self + 0x38));
    bool result = TObject_forward(self + 0x1C, frames);
    u32 end = self + 0x10;
    u32 node = load<u32>(self + 0x10);
    u32 all = 0xF, any = 0;
    while (node != end) {
        u32 object = node - 8;
        node = load<u32>(node);
        bool current = TObject_forward(object, frames);
        u32 status = load<u32>(object + 0x2C);
        result = current || result;
        all &= status;
        any |= status;
    }
    store<u32>(self + 0x18, all | (any << 16));
    return result;
}
VERIFY(0x0283D514, TControl_forward);
}
