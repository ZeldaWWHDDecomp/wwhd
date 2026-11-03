/* Mutant schemata (mutate.py --schemata, 2026-10-26): force-included (-include) into a schema TU, the unit source
 * with every schema-able mutant of it compiled in at once. Mutant K is live when wwhd_mut_active == K (harness:
 * env WWHD_MUT_ACTIVE, or the fork server); every other mutant runs the original code.
 *
 * Sites (tools/verify/mutschema.py writes them):
 *   literal      (WWHD_MUT(K) ? NEW : OLD)
 *   a OP b       a OP wwhd_mut::Op<K, OPID>{} OP b   (==, !=, <, <=, >, >=, +): the compiler parses a and b exactly as
 *                in the original expression ((a OP x) OP b, same precedence, left associative); the proxy keeps a's
 *                value (be<T>/gptr<T> guest fields: their get(), what the builtin operator would convert to) and
 *                evaluates `a OP b` or `a OP' b`
 *   && / || / !  (WWHD_MUT(K) ? (mutant) : (original)) with the operands spelled out; for && / || the operands are
 *                the whole logical-and-expression around the operator (to the closest || on each side), so the
 *                mutant groups as its textual form does (2026-10-28: `a && b && c` -> `a || (b && c)`, not
 *                `(a || b) && c`)
 *   statements   { if (!WWHD_MUT(K)) { S } }  and  { if (WWHD_MUT(K)) { B A } else { A B } }
 * WWHD_MUT also records which sites ran when wwhd_mut_recording is set (reachability, mutate.py --reach-skip). */
#pragma once
#include <type_traits>
#include <utility>

extern "C" int wwhd_mut_active;
extern "C" int wwhd_mut_recording;
extern "C" void wwhd_mut_hit(int k);

static inline bool WWHD_MUT(int k) {
    if (__builtin_expect(wwhd_mut_recording, 0)) wwhd_mut_hit(k);
    return wwhd_mut_active == k;
}

namespace wwhd_mut {
enum { EQ, NE, LT, LE, GT, GE, ADD };
template <int K, int O> struct Op {};
template <class V, int K, int O> struct L { V a; };
struct NullTok {};    /* right operand spelled NULL */
struct ZeroTok {};    /* right operand spelled 0 */
struct NullptrTok {}; /* right operand spelled nullptr */

/* the value the builtin operator works on: a copy, an array/function decayed, a guest field's get() */
template <class T> auto val(T&& a) {
    using D = std::decay_t<T>;
    if constexpr (std::is_copy_constructible_v<D>) return D(std::forward<T>(a));
    else return a.get();
}

#define WWHD_MUT_OP(OPID, OP, MOP)                                                                                \
    template <class T, int K> auto operator OP(T&& a, Op<K, OPID>) {                                             \
        using V = decltype(val(std::forward<T>(a)));                                                            \
        return L<V, K, OPID>{val(std::forward<T>(a))};                                                          \
    }                                                                                                            \
    template <class V, int K, class U> auto operator OP(L<V, K, OPID> l, U&& b)->decltype(l.a OP std::forward<U>(b)) { \
        if (WWHD_MUT(K)) return l.a MOP std::forward<U>(b);                                                     \
        return l.a OP std::forward<U>(b);                                                                       \
    }                                                                                                            \
    template <class V, int K> auto operator OP(L<V, K, OPID> l, NullTok)->decltype(l.a OP __null) {             \
        if (WWHD_MUT(K)) return l.a MOP __null;                                                                 \
        return l.a OP __null;                                                                                   \
    }                                                                                                            \
    template <class V, int K> auto operator OP(L<V, K, OPID> l, ZeroTok)->decltype(l.a OP 0) {                  \
        if (WWHD_MUT(K)) return l.a MOP 0;                                                                      \
        return l.a OP 0;                                                                                        \
    }                                                                                                            \
    template <class V, int K> auto operator OP(L<V, K, OPID> l, NullptrTok)->decltype(l.a OP nullptr) {         \
        if (WWHD_MUT(K)) return l.a MOP nullptr;                                                                \
        return l.a OP nullptr;                                                                                  \
    }

WWHD_MUT_OP(EQ, ==, !=)
WWHD_MUT_OP(NE, !=, ==)
WWHD_MUT_OP(LT, <, <=)
WWHD_MUT_OP(LE, <=, <)
WWHD_MUT_OP(GT, >, >=)
WWHD_MUT_OP(GE, >=, >)
WWHD_MUT_OP(ADD, +, -)
#undef WWHD_MUT_OP
}  // namespace wwhd_mut
