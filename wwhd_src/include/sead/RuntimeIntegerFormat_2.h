/* WWHD sead formatter (companion of sead/RuntimeIntegerFormat.cpp, SEAD#15): the %s and %ls field writers
 * and the formatter translation unit's copies of the text-class virtual functions.
 * Reconstructed from this game's instructions. */
#pragma once
#include "sead/RuntimeFormatSupport.h"
#include "sead/FileDevice.h"

namespace sead {
namespace format {
/* one string conversion: pads to the field width and stops at the precision when one is given */
void writeByteTextField(const FormatState* state, DigitBuffer* output, const be<u8>* text);
void writeWideTextField(const FormatState* state, DigitBuffer* output, const be<u16>* text);
}

/* per-TU copies of the text classes of the formatter translation unit (vtables in the order the
 * image stores them; the capacities of the four fixed buffers are not visible in their bodies) */
namespace format_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextD : SafeTextBuffer { void destroy(u32 flags); };
}
}
