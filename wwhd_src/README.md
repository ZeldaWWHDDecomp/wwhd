# wwhd_src: verified decompilation of The Legend of Zelda: The Wind Waker HD (cking.rpx)

This directory holds the decompiled source: C++ written from the WWHD executable's machine code,
using the GameCube decompilation (zeldaret/tww) as a reference for names and structure where the two
agree. It contains no original machine code, assets or game data; it runs only together with the recompiled game
built from your own copy (see the top-level README).

Every function here carries `VERIFY(0xADDR, name)`. The harness in `tools/verify/` has run it
against the recompiled original of that WWHD function and found the same return value, memory
writes and call sequence on all inputs tried: generated inputs, and calls recorded in the running
game where available. See `tools/verify/README.md` for the method, the rules the source follows
and the pilot results. Units (which files belong together, input steering) are in
`tools/verify/units/`.

Layout:

- `include/`: WWHD structure layouts (`be<T>` fields at WWHD offsets) and bindings to the WWHD
  functions the source calls (`gabi::call` with the WWHD address).
- `d/actor/`: actors, one file per GameCube translation unit (`d_a_kamome_exec.cpp` and
  `d_a_kamome_auto.cpp` hold kamome's two largest functions; they call the other kamome
  functions by address, as every function does).

The source is written against `tools/verify/include/gabi.h`: guest structures are accessed
through big-endian field types and every call to another WWHD function goes through its binding.
