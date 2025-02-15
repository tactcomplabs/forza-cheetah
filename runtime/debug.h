#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdarg.h>

#include "rts-config.h"

// forward declaration for using struct global_stat
struct global_state;
struct __cilkrts_worker;

#define CILK_CHECK(g, cond, complain, ...)                                     \
    ((cond) ? (void)0 : cilk_die_internal(g, complain, __VA_ARGS__))

#ifndef ALERT_LVL
#define ALERT_LVL 0x3d03
#endif
#define ALERT_NONE             0x0000
#define ALERT_FIBER            0x0001
#define ALERT_FIBER_SUMMARY    0x0002
#define ALERT_MEMORY           0x0004
#define ALERT_SYNC             0x0010
#define ALERT_SCHED            0x0020
#define ALERT_STEAL            0x0040
#define ALERT_RETURN           0x0080
#define ALERT_EXCEPT           0x0100
#define ALERT_CFRAME           0x0200
#define ALERT_REDUCE           0x0400
#define ALERT_REDUCE_ID        0x0800
#define ALERT_BOOT             0x1000
#define ALERT_START            0x2000
#define ALERT_CLOSURE          0x4000
#define ALERT_NOBUF            0x80000000

#if ALERT_LVL & (ALERT_CFRAME|ALERT_RETURN)
extern unsigned int alert_level;
#else
extern CHEETAH_INTERNAL unsigned int alert_level;
#endif
#define ALERT_ENABLED(flag) (alert_level & (ALERT_LVL & ALERT_##flag))

#ifndef DEBUG_LVL
#define DEBUG_LVL 0xff
#endif

#define DEBUG_MEMORY 0x01
#define DEBUG_MEMORY_SLOW 0x02
#define DEBUG_FIBER 0x04
#define DEBUG_REDUCER 0x08
extern CHEETAH_INTERNAL unsigned int debug_level;
#define DEBUG_ENABLED(flag) (debug_level & (DEBUG_LVL & DEBUG_##flag))
#define DEBUG_ENABLED_STATIC(flag) (DEBUG_LVL & DEBUG_##flag)

// Unused: compiler inlines the stack frame creation
// #define CILK_STACKFRAME_MAGIC 0xCAFEBABE

CHEETAH_INTERNAL void set_alert_level_from_str(const char *const);
CHEETAH_INTERNAL void set_alert_level(unsigned int);
CHEETAH_INTERNAL void set_debug_level(unsigned int);
CHEETAH_INTERNAL void flush_alert_log(void);

__attribute__((__format__(__printf__, 1, 2))) CHEETAH_INTERNAL_NORETURN void
cilkrts_bug(const char *fmt, ...);
CHEETAH_INTERNAL_NORETURN
void cilk_die_internal(struct global_state *const g, const char *fmt, ...);

#if ALERT_LVL != 0
__attribute__((__format__(__printf__, 2, 3))) void
cilkrts_alert(int lvl, const char *fmt, ...);
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#define cilkrts_alert(CODE, FMT, ...)                                          \
    (alert_level & ((ALERT_##CODE) & ALERT_LVL))                               \
        ? cilkrts_alert(ALERT_##CODE, FMT, ##__VA_ARGS__)                      \
        : (void)0
#else
#define cilkrts_alert(lvl, fmt, ...)
#endif

#if CILK_DEBUG

#define WHEN_CILK_DEBUG(ex) ex

/** Standard text for failed assertion */
CHEETAH_INTERNAL extern const char *const __cilkrts_assertion_failed;

#define CILK_ASSERT(ex)                                                        \
    (__builtin_expect((ex) != 0, 1)                                            \
         ? (void)0                                                             \
         : cilkrts_bug(__cilkrts_assertion_failed, __FILE__, __LINE__, #ex))

#define CILK_ASSERT_NULL(P)                                                    \
    ({ void *_t = (P); __builtin_expect(!_t, 1)                                \
         ? (void)0                                                             \
         : cilkrts_bug("%s: %d: cilk_assertion failed: %s (%p) == NULL",       \
                       __FILE__, __LINE__, #P, _t);})

#define CILK_ASSERT_POINTER_EQUAL(P1, P2)                                      \
    ({ void *_t1 = (P1), *_t2 = (P2); __builtin_expect(_t1 == _t2, 1)          \
         ? (void)0                                                             \
         : cilkrts_bug("%s: %d: cilk_assertion failed: %s (%p) == %s (%p)",    \
                       __FILE__, __LINE__, #P1, _t1, #P2, _t2);})

#define CILK_ASSERT_INDEX_ZERO(LEFT, I, RIGHT, FMT)                            \
    (__builtin_expect(!(LEFT[I] RIGHT), 1)                                     \
         ? (void)0                                                             \
         : cilkrts_bug("%s: %d: cilk_assertion failed: %s[%u]%s = " FMT        \
                       " should be 0",                                         \
                       __FILE__, __LINE__, #LEFT, I, #RIGHT, LEFT[I] RIGHT))

#define CILK_ASSERT_LE(A, B, FMT)                                            \
    (__builtin_expect(((A) <= (B)) != 0, 1)                                    \
         ? (void)0                                                             \
         : cilkrts_bug("%s: %d: cilk assertion failed: %s (" FMT               \
                       ") <= %s " FMT ")",                                     \
                       __FILE__, __LINE__, #A, A, #B, B))

#define CILK_ABORT(msg)                                                     \
    cilkrts_bug(__cilkrts_assertion_failed, __FILE__, __LINE__, msg)

#define CILK_ABORT_G(msg)                                                      \
    cilkrts_bug(__cilkrts_assertion_failed_g, __FILE__, __LINE__, msg)

#else
#define CILK_ASSERT(ex)
#define CILK_ASSERT_NULL(ex)
#define CILK_ASSERT_POINTER_EQUAL(P1, P2)
#define CILK_ASSERT_INDEX_ZERO(LEFT, I, RIGHT, FMT)
#define CILK_ASSERT_LE(A, B, FMT)
#define CILK_ABORT(msg)
#define CILK_ABORT_G(msg)
#define WHEN_CILK_DEBUG(ex)
#endif // CILK_DEBUG

// to silence compiler warning for vars only used during debugging
#define USE_UNUSED(var) (void)(var)
#endif
