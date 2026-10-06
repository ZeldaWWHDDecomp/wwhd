# f_pc_stdcreate_req HD notes

The nine phase/request bodies span 025E12E0–025E1537. The immediately following common SDK initializer at 025E1538 is included by translation-unit adjacency (inferred attribution). The preceding 025E124C initializer belongs to the search unit; the next named unit begins at 025E162C.

HD removes the GameCube Load phase. CreateProcess no longer frees the module after a failed process allocation. Request allocation remains 0x60 bytes; HD offsets are layer+44, phase+48, signed16 process name+50, append+54, callback+58, and callback data+5C. The request entry compares the full incoming signed register with32767 before truncating the name on storage.

The handler uses an iterative phase loop in HD rather than the GameCube recursive call. Subcreate and callback results are preserved exactly; the completion test requires both queried booleans to equal1. SDK initialization is verified as calls and global writes, with constructors mocked at the unit boundary.
