/* Tables emitted by mkunit.py for one verification unit (unit_<name>.c). */
#pragma once
#include <stdint.h>

#include "ppc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CalleeInfo {
    uint32_t addr;
    const char* name;
    uint32_t imask;   /* argument GPRs compared at the call (bit n = rn) */
    uint32_t fmask;   /* argument FPRs compared (bit n = fn) */
    uint32_t defr;    /* volatile GPRs the callee may write (bit n = rn); ~0 unknown */
    uint32_t deff;    /* volatile FPRs the callee may write */
    uint16_t ptrsz[11]; /* per GPR: pointee size of a pointer argument (0 unknown, 255 constructor storage: not compared, 0xFFFE explicit scalar `rN=0`) */
    uint8_t outp[11];  /* per GPR: 1 if a non-const pointer (the callee may write through it) */
    void (*real)(Cpu*); /* linked real code (unit option `real`), else NULL */
    uint8_t declared;   /* compare the argument registers the candidate declares (imports) */
    uint8_t retkind;    /* shape of the r3 result: 0 any, 1 zero-extended, 2 sign-extended, 3 constant */
    uint8_t retbits;
    uint32_t retconst;
    uint8_t nstack;     /* argument words passed on the stack (sp+8...) beyond r3-r10 */
    uint32_t outonly;   /* bit n: rN points to output-only storage (contents before the call not compared; mocks fill ptrsz bytes) */
    uint16_t ext[11];   /* per GPR: static extent of the callee's accesses through rN (extent_lint.py; 0 unknown): the undersized-Local check uses it like ptrsz */
    uint32_t fill;      /* bit n: generated mocks fill ptrsz bytes at rN after the call (rN=outSIZE and rN=inoutSIZE) */
} CalleeInfo;

typedef struct OrigFunc {
    uint32_t addr;
    const char* name;      /* WWHD name (names.tsv) */
    const char* gcsym;     /* GameCube mangled symbol, "" if none */
    void (*fn)(Cpu*);
    uint32_t nblocks;      /* coverage points */
    char argtype[11];      /* per GPR r3..r10 from the signature: p ptr, i int, h s16, H u16, c s8, b u8, x unknown */
    uint8_t nflt;          /* float arguments */
    uint32_t livein_r, livein_f; /* argument registers the code reads (dataflow) */
    uint8_t retregs;       /* ret_values.tsv: bit0 r3, bit1 f1, bit2 r4 (pair) is used by original callers */
    uint8_t retbits;       /* width of the r3 result (8/16/32) */
} OrigFunc;

extern const CalleeInfo unit_callees[];
extern const unsigned unit_ncallees;
extern const OrigFunc unit_funcs[];
extern const unsigned unit_nfuncs;
extern const char unit_name[];

#ifdef __cplusplus
}
#endif
