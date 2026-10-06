# JKRHeap thread-context subset

This qualified SDK subset contains exactly two entries: thread-heap setter02756170..027561B3 and current-heap getter wrapper027EC230..027EC23B. The root-owned getter02756140 and six heap adapters are excluded; no full heap-library/TU claim is made.

The HD setter preserves incoming r4 across OSGetThreadSpecific(C0009CC8), obtains its key from module-manager pointer101F8B94+0x24, reads the old heap at thread-context+0x70, writes incoming r4 there and returns the old value. The wrapper loads101F8B4C and calls02756140. This HD thread-local context replaces assumptions about a single mutable global current heap; native bodies, not incomplete matcher names, determine the meaning.

Both functions pass10000generated inputs at seeds1and7 with2/2block coverage and zero aborts/crashes/timeouts. The initialized module-manager/key fixture distinguishes wrong-global and offset changes. No shared harness/header changes, stable-memory exclusions or indirect-call exemptions were used.

The bounded eight-case mutation sample has seven returning observation differences and one equivalent, with no invalid/open cases. On initialized ordinary RAM, changing the wrapper load101F8B4C to101F8B4D only changes the discarded incoming r3 of02756140: its native body overwrites r3 with the module TLS key at02756154 before any incoming-r3 read, preserving call effects and return. This equivalence is not inferred merely from equal generated observations and makes no claim about memory-mapped I/O or faulting addresses.

