/* Letter status API. */
#pragma once
#include "bindings.h"

// HD: these functions retain full register argument and return widths.
s32 dLetter_isNoSend(u32);
void dLetter_send(u32);
s32 dLetter_isSend(u32);
void dLetter_stock(u32);
s32 dLetter_isStock(u32);
void dLetter_read(u32);
s32 dLetter_isRead(u32);
void dLetter_delivery(u32);
void dLetter_autoStock(u32);
u32 dLetter_isDelivery(u32);
