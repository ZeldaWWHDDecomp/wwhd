/* jerror.c (decompiled WWHD build)
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Rewritten for guest error fields and Cafe diagnostic output.
 */
#include "libjpeg/jerror.h"
// View of the first entry; the message strings remain in the guest image.
static jpeg_message_ptr jpeg_std_message_table;
/* 028580B0 */
static void error_exit(j_common_ptr cinfo) {
    // HD: the built-in fatal-error hook returns without printing or terminating.
}
/* 028580B4 */
static void output_message(j_common_ptr cinfo) {
    jpeg_message_buffer buffer;
    cinfo->err->format_message(cinfo,reinterpret_cast<char*>(buffer.characters));
    // HD: Cafe console output replaces the reference's stderr stream.
    jpeg_console_message("%s\n",reinterpret_cast<char*>(buffer.characters));
}
/* 028580F8 */
static void emit_message(j_common_ptr cinfo,s32 msg_level) {
    // HD: the built-in warning/trace hook discards messages and warning counts.
}
/* 028580FC */
static void format_message(j_common_ptr cinfo,char* buffer) {
    jpeg_error_mgr* err=cinfo->err.get();
    s32 msg_code=err->msg_code;
    const be<char>* msgtext=nullptr;
    if (msg_code>0 && msg_code<=err->last_jpeg_message)
        msgtext=reinterpret_cast<const be<char>*>(err->jpeg_message_table[msg_code].get());
    else if (err->addon_message_table && msg_code>=err->first_addon_message && msg_code<=err->last_addon_message)
        msgtext=reinterpret_cast<const be<char>*>(err->addon_message_table[msg_code-err->first_addon_message].get());
    if (!msgtext) {
        msgtext=reinterpret_cast<const be<char>*>(err->jpeg_message_table[0].get());
        err->msg_parm[0]=msg_code;
    }
    bool isstring=false;
    const be<char>* msgptr=msgtext;
    char ch;
    while ((ch=*msgptr++)!='\0') {
        if (ch=='%') { isstring=(*msgptr=='s'); break; }
    }
    if (isstring)
        format_string_message(buffer,reinterpret_cast<const char*>(msgtext),reinterpret_cast<char*>(err->msg_parm));
    else
        format_integer_message(buffer,reinterpret_cast<const char*>(msgtext),err->msg_parm[0],err->msg_parm[1],err->msg_parm[2],err->msg_parm[3],err->msg_parm[4],err->msg_parm[5],err->msg_parm[6],err->msg_parm[7]);
}
/* 02858224 */
static void reset_error_mgr(j_common_ptr cinfo) {
    jpeg_error_mgr* err=cinfo->err.get();
    err->num_warnings=0;
    err->msg_code=0;
}
/* 02858238 */
jpeg_error_mgr* jpeg_std_error(jpeg_error_mgr* err) {
    err->error_exit=error_exit;
    err->emit_message=emit_message;
    err->output_message=output_message;
    err->format_message=format_message;
    err->reset_error_mgr=reset_error_mgr;
    err->trace_level=0;
    err->num_warnings=0;
    err->msg_code=0;
    err->jpeg_message_table=&jpeg_std_message_table;
    err->last_jpeg_message=JWRN_TOO_MUCH_DATA;
    err->addon_message_table=nullptr;
    err->first_addon_message=0;
    err->last_addon_message=0;
    return err;
}
/* WWHD glue
str 1017C078 "%s\n"
data jpeg_std_message_table 1017D38C
ext C0009EE8 void jpeg_console_message(const char* format,const char* buffer)
ext 028F186C s32 format_string_message(char* buffer,const char* format,const char* text)
ext 028F186C s32 format_integer_message(char* buffer,const char* format,s32 p0,s32 p1,s32 p2,s32 p3,s32 p4,s32 p5,s32 p6,s32 p7)
*/
