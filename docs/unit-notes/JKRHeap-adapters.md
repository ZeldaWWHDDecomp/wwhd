# Heap entry adapter

Six entries only:0273AEC4,0273AF40,0273AFC8,02756140,027EC0B0,027EC220. This is a qualified library adapter subset, not the complete SDK heap translation unit. Metadata027F3F94 is excluded: its actual body computes a resource-relative offset, not operator new. Native02756140 retrieves the thread-specific current heap despite its old allocation label.

HD allocation first resolves the current heap through the module context, then invokes its allocation virtual method with size and alignment. Scalar and array deletion resolve the containing heap, call its free method, or use the imported fallback when no root exists. Current-heap access uses the SDK thread-specific context. Native argument order is retained; no local objects or omitted indirect arguments.

All six pass10000 generated inputs at seeds1/7;18/18 blocks both. Bounded31 sample:9 returning differences,1 source-equivalent,19compile-invalid,2unaligned invented directcall targets excluded. Initial moduleglobals0 alias masked19/26; initialized module/context and varying threadkey steering detects19 at10000. Mutant26 remains equivalent:02756170 discards incomingr3 before any read and uses only the unchanged savedr4 heap plus SDK module/thread context, so loading the alternate mapped global for the unusedr3 argument cannot affect its calls, stores or returned old heap. This assumes ordinary nonfaulting moduledata loads; no transitive lifecycle or corruption proof is claimed. Uninitialized and initialized cases remain in final fixture.

The full heap library and allocator internals remain outside this subset; optional expansion deferred.
