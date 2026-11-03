/* Audio command arena and producer queues. Reconstructed from the supplied WWHD binary. */
#include "nw/snd/DriverCommandQueue.h"
using namespace nw::snd;

/* 02886D68 */
void commandIdle() {
    gabi::AutoFrame frame{0x02886D68};
    void* thread = soundThreadInstance();
    processSoundThreadCommands(thread);
}

/* 02886D8C */
void DriverCommandManager::prepare(DriverCommand* buffer, u32 size) {
    gabi::AutoFrame frame{0x02886D8C};
    initialize(buffer, size, gabi::guest_fn((const void*)dispatchAudioCommandList));
    idleCallback = gabi::guest_fn((const void*)commandIdle);
}

/* 02886DCC */
void DriverCommandManager::destroyWrapper(u32 flags) {
    gabi::AutoFrame frame{0x02886DCC};
    if (!this) return;
    destroy(0);
    if (flags & 1) sndDelete(this);
}

/* 02886E20 */
CommandArena* CommandArena::create(CommandArena* arena) {
    gabi::AutoFrame frame{0x02886E20};
    if (!arena) {
        arena = (CommandArena*)sndAllocate(sizeof(CommandArena));
        if (!arena) return arena;
    }
    arena->readWord = 0;
    arena->full = 1;
    arena->buffer = nullptr;
    arena->writeWord = 0;
    arena->capacityWords = 0;
    return arena;
}

/* 02886E70 */
void CommandArena::destroy(u32 flags) {
    gabi::AutoFrame frame{0x02886E70};
    if (this && (flags & 1)) sndDelete(this);
}

/* 02886E84 */
void CommandArena::initialize(DriverCommand* storage, u32 bytes) {
    gabi::AutoFrame frame{0x02886E84};
    buffer = storage;
    full = 0;
    capacityWords = bytes >> 2;
    writeWord = 0;
    readWord = 0;
}

/* 02886EA4 */
DriverCommand* CommandArena::allocate(u32 bytes) {
    gabi::AutoFrame frame{0x02886EA4};
    if (full) return nullptr;
    u32 write = writeWord;
    u32 read = readWord;
    u32 words = (bytes + 3) >> 2;
    u32 end = write + words;
    DriverCommand* result;
    if (read > write) {
        if (end > read) return nullptr;
        result = gabi::at<DriverCommand>(gabi::ea(buffer.get()) + (write << 2));
        writeWord = end;
    } else if (end <= capacityWords) {
        result = gabi::at<DriverCommand>(gabi::ea(buffer.get()) + (write << 2));
        writeWord = end;
    } else {
        if (words > read) return nullptr;
        result = buffer;
        end = words;
        writeWord = end;
    }
    if (!result) return nullptr;
    if (end == read) {
        end = writeWord;
        full = 1;
    }
    result->arenaEnd = end;
    return result;
}

/* 02886F44 */
void CommandArena::release(DriverCommand* last) {
    gabi::AutoFrame frame{0x02886F44};
    u32 end = last->arenaEnd;
    full = 0;
    readWord = end;
}

/* 02886F58 */
DriverCommandManager* DriverCommandManager::construct(DriverCommandManager* manager) {
    gabi::AutoFrame frame{0x02886F58};
    if (!manager) {
        manager = (DriverCommandManager*)sndAllocate(sizeof(DriverCommandManager));
        if (!manager) return manager;
    }
    manager->idleCallback = 0;
    manager->initialized = 0;
    manager->dispatch = 0;
    nw::ut::MessageQueue::create(&manager->pending);
    nw::ut::MessageQueue::create(&manager->retired);
    CommandArena::create(&manager->arena);
    return manager;
}

/* 02886FC0 */
void DriverCommandManager::finalize() {
    gabi::AutoFrame frame{0x02886FC0};
    clearCommandQueue(&pending);
    clearCommandQueue(&retired);
    initialized = 0;
}

/* 02887000 */
void DriverCommandManager::destroy(u32 flags) {
    gabi::AutoFrame frame{0x02887000};
    if (!this) return;
    finalize();
    arena.destroy(2);
    retired.destroy(2);
    pending.destroy(2);
    if (flags & 1) sndDelete(this);
}

/* 02887074 */
void DriverCommandManager::initialize(DriverCommand* buffer, u32 size, u32 dispatchFunction) {
    gabi::AutoFrame frame{0x02887074};
    dispatch = dispatchFunction;
    idleCallback = 0;
    arena.initialize(buffer, size);
    first = nullptr;
    last = nullptr;
    nextSerial = 0;
    pending.initialize(pendingMessages, 32);
    retired.initialize(retiredMessages, 33);
    pendingCount = 0;
    initialized = 1;
}

/* 028870F0 */
u32 DriverCommandManager::pushCommand(DriverCommand* command) {
    gabi::AutoFrame frame{0x028870F0};
    DriverCommand* tail = last;
    if (!tail)
        first = command;
    else
        tail->next = command;
    last = command;
    command->next = nullptr;
    return nextSerial;
}

/* 0288712C */
DriverCommand* DriverCommandManager::reserve(u32 size) {
    gabi::AutoFrame frame{0x0288712C};
    return arena.allocate(size);
}

/* 02887134 */
void DriverCommandManager::releaseList(DriverCommand* command) {
    gabi::AutoFrame frame{0x02887134};
    releasedSerial = command->serial;
    for (;;) {
        DriverCommand* next = command->next;
        if (!next) break;
        command = next;
    }
    arena.release(command);
}

/* 02887158 */
void DriverCommandManager::collect() {
    gabi::AutoFrame frame{0x02887158};
    gabi::FrameLocal<be<u32>> message(8);
    while (retired.receive(message.get(), 0)) releaseList(gabi::at<DriverCommand>((u32)*message));
}

/* 028871BC */
void DriverCommandManager::waitFor(u32 serial) {
    gabi::AutoFrame frame{0x028871BC};
    gabi::FrameLocal<be<u32>> message(8);
    for (;;) {
        if (!retired.receive(message.get(), 0)) {
            u32 callback = idleCallback;
            if (callback) gabi::call_ptr<void>(callback);
            retired.receive(message.get(), 1);
        }
        DriverCommand* command = gabi::at<DriverCommand>((u32)*message);
        releaseList(command);
        if (command->serial == serial) return;
    }
}

/* 02887248 */
DriverCommand* DriverCommandManager::allocCommand(u32 size) {
    gabi::AutoFrame frame{0x02887248};
    DriverCommand* command = reserve(size);
    if (command) return command;
    collect();
    command = reserve(size);
    while (!command) {
        if (first) {
            u32 serial = submit(1);
            waitFor(serial);
        } else {
            u32 callback = idleCallback;
            if (callback) gabi::call_ptr<void>(callback);
            collect();
        }
        command = reserve(size);
    }
    return command;
}

/* 02887314 */
u32 DriverCommandManager::submit(u32 blocking) {
    gabi::AutoFrame frame{0x02887314};
    DriverCommand* commands = first;
    if (!commands) {
        if (!blocking) return 0;
        DriverCommand* marker = allocCommand(sizeof(DriverCommand));
        marker->id = 0xFFFFFFFF;
        pushCommand(marker);
        commands = first;
    }
    u32 serial = nextSerial;
    OSMemoryBarrier();
    pendingCount = (u32)pendingCount + 1;
    if (!pending.send(gabi::ea(commands), 0)) {
        if (!blocking) return 0;
        u32 callback = idleCallback;
        if (callback) gabi::call_ptr<void>(callback);
        pending.send(gabi::ea(commands), 1);
    }
    DriverCommand* current = first;
    current->serial = serial;
    u32 next = nextSerial;
    first = nullptr;
    last = nullptr;
    nextSerial = next + 1;
    return serial;
}

/* 02887464 */
u32 DriverCommandManager::executePending() {
    gabi::AutoFrame frame{0x02887464};
    gabi::FrameLocal<be<u32>> message(8);
    if (!pending.receive(message.get(), 0)) return 0;
    pendingCount = (u32)pendingCount - 1;
    DriverCommand* command = gabi::at<DriverCommand>((u32)*message);
    if (command->id != 0xFFFFFFFF) {
        gabi::call_ptr<void>(dispatch, command);
        OSMemoryBarrier();
        command = gabi::at<DriverCommand>((u32)*message);
    }
    retired.send(gabi::ea(command), 1);
    return 1;
}

/* WWHD glue
ext 0273AD10 void* sndAllocate(u32)
ext 028F6BEC nw::ut::MessageQueue* nw::ut::MessageQueue::create(nw::ut::MessageQueue*)
ext 028F6C18 void nw::ut::MessageQueue::destroy(u32)
ext 028F6C2C void nw::ut::MessageQueue::initialize(void*,s32)
ext 028F6C94 u32 nw::ut::MessageQueue::receive(be<u32>*,u32)
ext 0273AF40 void sndDelete(void*)
ext 028986B0 void* soundThreadInstance()
ext 02898EB0 void processSoundThreadCommands(void*)
ext 028F6CD4 void clearCommandQueue(nw::ut::MessageQueue*)
ext C0009E90 void OSMemoryBarrier()
ext 028F6C30 u32 nw::ut::MessageQueue::send(u32,u32)
*/
