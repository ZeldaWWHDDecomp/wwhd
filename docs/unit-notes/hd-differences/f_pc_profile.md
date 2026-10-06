### f_pc_profile — process profile lookup

The HD unit has two entries: fpcPf_Get at 025E10E0 and the per-unit static initializer at 025E10F4..025E1184. Lookup attribution is established by fpcBs_Create at 025DD414, which calls 025E10E0 at 025DD430 before reading the returned profile's allocation sizes at +0x10 and +0x14. Initializer attribution follows TU adjacency; the previous initializer025E104C ends at025E10DC, and the next initializer starts025E1188.

GameCube fpcPf_Get loads an element through the mutable global g_fpcPf_ProfileList_p. HD indexes the embedded guest table at101F3EE0 directly, shifts the incoming integer register by two, and returns the selected32-bit profile pointer. HD does not sign-extend the incoming register within this function; normal callers supply the intended profile number. The candidate retains the raw register/address-wrap semantics and introduces no bounds checks.

HD also emits SDK registration and local math-global initialization as a static initializer. The candidate retains the exact store/call order and argument addresses. Neither entry uses indirect calls or stack objects passed to callees. Both are verified using unrestricted stock generated fixtures; external initializer calls are mocked. This is evidence for the reference function contracts, not a claim about invalid profile numbers or native gameplay integration.
