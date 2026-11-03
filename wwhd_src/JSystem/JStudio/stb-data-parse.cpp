// STB stream records, adapted from tww stb-data-parse.cpp.
#include "JSystem/JStudio/stb-data-parse.h"
#include "JSystem/JGadget/binary.h"
namespace {
struct DataSizeTable { be<u32> sizes[8]; };
static const DataSizeTable gauDataSize_TEParagraph_data = {{0,1,2,4,8,16,32,64}};
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
const void* JStudio_parseVariableUInt(const JGadget::binary::VariableUInt*, be<u32>*, be<u32>*, be<u32>*);
void JStudio_stbParseRegisterGlobalObject(void*);

/* 0283C9C4 */
void parseSequence(be<u32>* sequence, jst_timeline::SequenceData* data) {
    data->content = 0;
    data->next = 0;
    const be<u32>* raw = reinterpret_cast<const gptr<const be<u32>>*>(sequence)->get();
    u32 head = *raw;
    u32 type = head >> 24;
    u32 param = head & 0xFFFFFF;
    data->type = type;
    data->param = param;
    if (type) {
        u32 next = u32(*sequence) + 4;
        if (type > 0x7F) {
            data->content = next;
            next += param;
        }
        data->next = next;
    }
}
/* 0283CA0C */
void parseParagraph(be<u32>* paragraph, jst_timeline::ParagraphData* data) {
    gabi::Local<be<u32>> size;
    const void* next = JStudio_parseVariableUInt(reinterpret_cast<const gptr<const JGadget::binary::VariableUInt>*>(paragraph)->get(), size.get(), &data->type, nullptr);
    u32 count = *size;
    data->size = count;
    if (!count) {
        data->next = gabi::ea(next);
        data->content = 0;
    } else {
        data->content = gabi::ea(next);
        data->next = gabi::ea(next) + ((count + 3) & ~3u);
    }
}
/* 0283CA7C */
void JStudio::stb::data::TParse_TParagraph_data::getData(TData* data) const {
    data->entryCount = 0;
    data->content = nullptr;
    data->entrySize = 0;
    data->next = nullptr;
    const be<u8>* file = raw;
    if (!file) return;
    u32 status = *file;
    data->status = status & ~8u;
    if (!status) return;
    ++file;
    u32 count = 1;
    if (status & 8) count = *file++;
    data->entryCount = count;
    data->content = reinterpret_cast<const u8*>(file);
    u32 type = status & 7;
    if (!type) return;
    u32 size = gauDataSize_TEParagraph_data.sizes[type];
    data->entrySize = size;
    u32 bytes = size * count;
    data->next = reinterpret_cast<const u8*>(file) + bytes;
}
/* 0283CAF8 */
static void __sinit_stb_data_parse_cpp() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JStudio_stbParseRegisterGlobalObject(&sStaticRegister);
}
/* WWHD glue
data gauDataSize_TEParagraph_data 101731A0
data sStaticBlock 104B60B8
data sStaticRegister 101FADA4
ext 027ED9C0 const void* JStudio_parseVariableUInt(const JGadget::binary::VariableUInt*, be<u32>*, be<u32>*, be<u32>*)
ext 028F026C void JStudio_stbParseRegisterGlobalObject(void*)
*/
