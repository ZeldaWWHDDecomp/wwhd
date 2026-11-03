// Function-value bank paragraph boundary parser.
#include "JSystem/JStudio/fvb-data-parse.h"
const u8* JStudioFvb_parseVariable(const void*,be<u32>*,be<u32>*,void*);
/* 0284A854 */
void JStudio::fvb::data::TParse_TParagraph::getData(TParagraph* data) const {
    const u8* content=JStudioFvb_parseVariable(raw,&data->size,&data->type,nullptr);
    u32 size=data->size;
    if (!size) { data->next=content; data->content=nullptr; }
    else { data->content=content; data->next=content+((size+3)&~3u); }
}
void FunctionValue_register(void*);
namespace {
struct fvb_data_parse_StaticBlock { be<u32> words[4]; };
static fvb_data_parse_StaticBlock sfvb_data_parseStaticBlock;
static u32 sfvb_data_parseStaticRegister;
}

/* 0284A90C */
static void __sinit_fvb_data_parse_cpp() {
    sfvb_data_parseStaticBlock.words[2]=0; sfvb_data_parseStaticBlock.words[0]=0; sfvb_data_parseStaticBlock.words[3]=0; sfvb_data_parseStaticBlock.words[1]=0;
    FunctionValue_register(&sfvb_data_parseStaticRegister);
}
/* WWHD glue:
data sfvb_data_parseStaticBlock 104B6714
data sfvb_data_parseStaticRegister 101FC79C
ext 027ED9C0 const u8* JStudioFvb_parseVariable(const void*,be<u32>*,be<u32>*,void*)
*/
