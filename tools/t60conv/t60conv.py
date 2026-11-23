#!/usr/bin/env python3
"""t60conv.py: the true60 conversion tool for simple actors (decomp build only; 2026-11-17).

Reads one actor source of the readable decomp, finds the per-frame sites of its execute path and rewrites the
mechanical ones into the established step forms (dt = 1/N), using the shared helpers of
wwhd_src/include/d/t60conv.h. Everything that needs judgement is left as it is and listed for review.

  t60conv.py convert UNIT [--src PATH] [--dry]   rewrite the source, write <actor>_t60.h, the T-02 spec, report.json
  t60conv.py check UNIT [--n N] [--seeds 1,7]    T-01 (s1/s7) + T-02 (N 2/4 x s1/s7) + negative control, verdict
  t60conv.py run UNIT                            convert + check
  t60conv.py draw UNIT... [--dry]                the draw-path pass alone (t60_draw.h) on already-converted units + T-01
  t60conv.py drawwrites UNIT... [--dry]           the draw path's state writes for review (--dry: render / guard ones too) and random draws
  t60conv.py marks SRC...|all                    t60c::mark check: every mark cleared by a first-step unmark (check runs it too)

Outputs (per unit, in the clone): the rewritten source, wwhd_src/include/d/actor/<actor>_t60.h,
tools/verify/units/_t60/<unit>.txt; tool records in $T60CONV_OUT/<unit>/ (report.json, REPORT.md, logs).
"""
import argparse, json, os, re, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
UNITS = os.path.join(ROOT, 'tools', 'verify', 'units')
OUT = os.environ.get('T60CONV_OUT', os.path.join(ROOT, 'build', 't60conv', 'runs'))
SCRATCH = os.environ.get('WWHD_VERIFY_SCRATCH', os.path.join(ROOT, 'build', 'verify'))

# ------------------------------------------------------------------ source parsing
ADDR = re.compile(r'/\*\s*([0-9A-Fa-f]{8})\s*\*/')
INLINED = re.compile(r'inlined into\s+([~\w:&]+)')
FNHEAD = re.compile(r'^(?!\s)(?!return\b|else\b|if\b|for\b|while\b|switch\b|case\b|#|typedef\b|struct\b|class\b|'
                    r'namespace\b|enum\b|union\b|ext\b|str\b|data\b)[^;=]*?\b(~?[A-Za-z_][\w:~]*)\s*\(')
STR = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'')
IDENT = re.compile(r'\b([A-Za-z_]\w*)\s*\(')


def strip_code(lines):
    out, inblk = [], False
    for raw in lines:
        s, res, i = raw, [], 0
        while i < len(s):
            if inblk:
                j = s.find('*/', i)
                if j < 0:
                    i = len(s)
                    break
                inblk = False
                i = j + 2
                continue
            j1, j2 = s.find('/*', i), s.find('//', i)
            js = [j for j in (j1, j2) if j >= 0]
            if not js:
                res.append(s[i:])
                break
            j = min(js)
            res.append(s[i:j])
            if j == j2:
                break
            inblk = True
            i = j + 2
        out.append(STR.sub('""', ''.join(res)))
    return out


class Func:
    def __init__(self, name, start, body, end):
        self.name, self.start, self.body, self.end = name, start, body, end
        self.addr = None
        self.inlined_into = None
        self.calls = set()

    @property
    def short(self):
        return self.name.split('::')[-1]


def parse_functions(raw, code):
    """functions at column 0 with a brace body; address from the comment lines above"""
    funcs, i, n = [], 0, len(code)
    while i < n:
        m = FNHEAD.match(code[i])
        if not m or code[i].rstrip().endswith(';'):
            i += 1
            continue
        # find the opening brace (signature may span lines), stop at ';'
        j, depth, opened = i, 0, None
        while j < n and j < i + 12:
            if ';' in code[j] and '{' not in code[j] and depth == 0 and not code[j].rstrip().endswith(','):
                if code[j].strip().endswith(';'):
                    break
            if '{' in code[j]:
                opened = j
                break
            j += 1
        if opened is None:
            i += 1
            continue
        depth, k = 0, opened
        end = None
        while k < n:
            depth += code[k].count('{') - code[k].count('}')
            if depth <= 0 and ('}' in code[k]):
                end = k
                break
            k += 1
        if end is None:
            break
        f = Func(m.group(1), i, opened, end)
        for b in range(i - 1, max(-1, i - 4), -1):
            a = ADDR.search(raw[b])
            if a and not f.addr:
                f.addr = a.group(1).upper()
            il = INLINED.search(raw[b])
            if il:
                f.inlined_into = il.group(1).lstrip('&')
            if code[b].strip():
                break
        a = ADDR.search(raw[i])
        if a and not f.addr:
            f.addr = a.group(1).upper()
        for k in range(opened, end + 1):
            for c in IDENT.findall(code[k]):
                f.calls.add(c)
            for c in re.findall(r'&\s*(?:\w+::)*(\w+)\b(?!\s*[\(\[.-])', code[k]):
                f.calls.add(c)
        funcs.append(f)
        i = end + 1
    return funcs


# ------------------------------------------------------------------ statements split over several lines (tool fix 2026-11-17)
CONT_END = re.compile(r'(&&|\|\||[=+\-*/%&|^<>?,(]|\bnew|\breturn)$')
CONT_START = re.compile(r'^(&&|\|\||\?|:(?!:)|[+*/%|^]|-(?![-=>])|==|!=|<=|>=|\.(?!\.)|->)')
CTRL_HEAD = re.compile(r'^(?:\}\s*)?(?:else\s+)?(?:if|for|while|switch)\b.*\)$|^(?:\}\s*)?else$|^do$')


def line_comment_split(raw):
    """(code part, // comment text or '') of a raw line; strings and block comments respected"""
    i, n, q = 0, len(raw), None
    while i < n:
        ch = raw[i]
        if q:
            if ch == '\\':
                i += 2
                continue
            if ch == q:
                q = None
        elif ch in '"\'':
            q = ch
        elif raw.startswith('/*', i):
            j = raw.find('*/', i + 2)
            if j < 0:
                return raw, ''
            i = j + 2
            continue
        elif raw.startswith('//', i):
            return raw[:i].rstrip(), raw[i + 2:].strip()
        i += 1
    return raw.rstrip(), ''


def join_statements(raw, code, funcs):
    """statements (and control headers) of function bodies that span several lines become one logical line: the joined
    code / raw text sits at the first line, the continuation lines are blank in both views. Returns
    {first line: (last line, joined raw, original raw lines)}; write_source() puts the original lines back for every
    statement no rule changed (a changed one is written as the one joined line)."""
    joined = {}
    for f in funcs:
        k = f.body + 1
        while k < f.end:
            s = code[k].strip()
            if not s or s.startswith('#') or (CTRL_HEAD.match(s) and s.count('(') == s.count(')')) or \
                    re.match(r'^(case\b.*|default\s*|[A-Za-z_]\w*\s*|public|private|protected)\s*:$', s):
                k += 1
                continue
            j, depth = k, 0
            while True:
                c = code[j].strip()
                depth += c.count('(') - c.count(')')
                nxt = code[j + 1].strip() if j + 1 < f.end else ''
                more = depth > 0 or (CONT_END.search(c) and not re.search(r'\b(case\b.*|default\s*):$', c)) or \
                    (c and not re.search(r'[;{}]$', c) and nxt and CONT_START.match(nxt))
                if not more or j + 1 >= f.end or j - k >= 10:
                    break
                j += 1
            if j > k and depth == 0 and re.search(r'[;{]$', code[j].strip()) and \
                    not any(re.search(r'[{}]', code[x]) for x in range(k, j)) and code[j].count('}') == 0:
                parts, cmts = [], []
                for x in range(k, j + 1):
                    c, cm = line_comment_split(raw[x])
                    parts.append(c.rstrip() if x == k else c.strip())
                    if cm:
                        cmts.append(cm.replace('*/', '* /'))
                text = ' '.join(p for p in parts if p)
                text = re.sub(r'\(\s+', '(', text)
                text = re.sub(r'\s+\)', ')', text)
                text = re.sub(r'\s+\.(?=\w)', '.', text)
                if cmts:
                    text += ' /* ' + '; '.join(cmts) + ' */'
                joined[k] = (j, text, raw[k:j + 1])
                ctext = ' '.join(code[x].strip() for x in range(k, j + 1))
                ctext = re.sub(r'\(\s+', '(', ctext)
                ctext = re.sub(r'\s+\)', ')', ctext)
                ctext = re.sub(r'\s+\.(?=\w)', '.', ctext)
                code[k] = re.match(r'\s*', code[k]).group(0) + ctext
                raw[k] = text
                for x in range(k + 1, j + 1):
                    code[x] = ''
                    raw[x] = ''
                k = j + 1
                continue
            k = max(j, k) + 1
    return joined


def write_source(conv, new):
    """the output lines: a joined statement that no rule changed gets its original lines back"""
    out = []
    for i, line in enumerate(new):
        if i in conv.cont:
            continue
        if i in conv.joined:
            j, text, orig = conv.joined[i]
            out.extend(orig if line == text else [line])
            continue
        out.append(line)
    return out


# ------------------------------------------------------------------ guest-table dispatch (tool fix 2026-11-17)
DISPATCH = re.compile(r'\bdispatch\s*\(|\bcall_ptr\b|\btail_ptr\b|(->|\.)\*\s*\(?\s*\w')
RPX = os.path.join(ROOT, 'game', 'code', 'cking.rpx')
_rpx_secs = None
CKW = set('this return if else for while switch case u32 s32 u16 s16 u8 s8 int void const auto gabi ea at object_at ptmf '
          'call_ptr tail_ptr dispatch static_cast reinterpret_cast be true false TRUE FALSE'.split())


def rpx_read(addr, n):
    """n bytes of the original game image at guest address addr (decompressed RPX sections; None outside them)"""
    global _rpx_secs
    if _rpx_secs is None:
        import struct as _st, zlib as _z
        _rpx_secs = []
        try:
            d = open(RPX, 'rb').read()
        except OSError:
            return None
        shoff = _st.unpack('>I', d[0x20:0x24])[0]
        shnum = _st.unpack('>H', d[0x30:0x32])[0]
        for i in range(shnum):
            name, typ, flags, a, off, size = _st.unpack('>IIIIII', d[shoff + i * 40:shoff + i * 40 + 24])
            if typ == 8 or size == 0 or a == 0:
                continue
            raw = d[off:off + size]
            if flags & 0x08000000:
                raw = _z.decompress(raw[4:])
            _rpx_secs.append((a, raw))
    for a, raw in _rpx_secs:
        if a <= addr and addr + n <= a + len(raw):
            return raw[addr - a:addr - a + n]
    return None


def braced(text, i):
    """the text of the brace block that opens at text[i] ('{')"""
    depth = 0
    for j in range(i, len(text)):
        depth += (text[j] == '{') - (text[j] == '}')
        if depth == 0:
            return text[i:j + 1]
    return text[i:]


def resolve_dispatch(raw, code, funcs, header_text=''):
    """name the same-unit functions behind indirect calls through guest tables: a static table initialised in the
    source (&Class::fn or code addresses), a guest table named by a `data NAME ADDR` glue line or a `NAME = 0x1...`
    constant (read from the game image: code addresses of this unit's functions, member-pointer headers skipped), or a
    member pointer assigned from such a table elsewhere in the unit. The targets become callees of the dispatching
    function (execute path, T-02 real + t60cand); f.dispatch[line] lists them."""
    import struct as _st
    by_addr = {f.addr.upper(): f for f in funcs if f.addr}
    shorts = {f.short for f in funcs}
    text = '\n'.join(code)
    rawtext = '\n'.join(raw) + '\n' + header_text
    memo = {}

    def targets(name, depth=0):
        if (name, depth) in memo:
            return memo[(name, depth)]
        memo[(name, depth)] = set()
        out = set()
        m = re.search(r'\b' + re.escape(name) + r'\s*(?:\[[^\]]*\])+\s*=\s*\{', text)
        if m:
            body = braced(text, m.end() - 1)
            out |= {x for x in re.findall(r'&\s*(?:\w+::)*(\w+)', body) if x in shorts}
            for h in re.findall(r'0x0*(2[0-9A-Fa-f]{6})\b', body):
                g = by_addr.get(('0' + h).upper())
                if g:
                    out.add(g.short)
        else:
            a = re.match(r'0x(1[0-9A-Fa-f]{7})$', name) or \
                re.search(r'^\s*data\s+' + re.escape(name) + r'\s+([0-9A-Fa-f]{8})\b', rawtext, re.M) or \
                re.search(r'\b' + re.escape(name) + r'\s*=\s*0x(1[0-9A-Fa-f]{7})\b', rawtext)
            if a:
                base = int(a.group(1), 16)
                blob = rpx_read(base, 4 * 96) or b''
                for k in range(len(blob) // 4):
                    w = _st.unpack('>I', blob[4 * k:4 * k + 4])[0]
                    g = by_addr.get('%08X' % w)
                    if g:
                        out.add(g.short)
                    elif w in (0, 0xFFFF, 0xFFFFFFFF) or w < 0x10000 or (w >> 16) == 0xFFFF:
                        continue
                    else:
                        break
        if depth == 0 and not out:
            # a member pointer: tables indexed in the functions that assign it, or &fn assigned to it
            for f in funcs:
                body = code[f.body:f.end + 1]
                if not any(re.search(r'\b' + re.escape(name) + r'(?:\.\w+)?\s*=[^=]', l) for l in body):
                    continue
                for l in body:
                    for x in re.findall(r'\b' + re.escape(name) + r'(?:\.\w+)?\s*=\s*&\s*(?:\w+::)*(\w+)', l):
                        if x in shorts:
                            out.add(x)
                    for t in re.findall(r'\b([A-Za-z_]\w*)\s*\[', l):
                        if t != name:
                            out |= targets(t, 1)
        memo[(name, depth)] = out
        return out

    # dispatch helpers (dai_item invokeMember(actor, &TABLE[mode]): the call_ptr sits in the helper, the guest table at
    # its call sites): a call of one is a dispatch line of the caller (2026-11-18)
    helpers = {g.short for g in funcs if any(DISPATCH.search(code[k]) for k in range(g.body, g.end + 1))}
    for f in funcs:
        f.dispatch = {}
        for k in range(f.body, f.end + 1):
            s = code[k]
            if not DISPATCH.search(s) and not any(h != f.short and re.search(r'\b' + re.escape(h) + r'\s*\(', s) for h in helpers):
                continue
            tg = set()
            ids = set(re.findall(r'\b([A-Za-z_]\w*)\b', s)) - CKW
            ids |= set(re.findall(r'\b(0x1[0-9A-Fa-f]{7})\b', s))
            # locals of the call: their definition / assignment lines earlier in the function name the table
            for i in list(ids):
                for j in range(k - 1, f.body - 1, -1):
                    dl = code[j]
                    if re.search(r'\b' + re.escape(i) + r'\s*=[^=]', dl):
                        rhs = dl.split('=', 1)[1]
                        j2 = j
                        while not rhs.rstrip().endswith(';') and j2 + 1 < k:
                            j2 += 1
                            rhs += code[j2]
                        ids |= set(re.findall(r'\b([A-Za-z_]\w*)\b', rhs)) - CKW
                        ids |= set(re.findall(r'\b(0x1[0-9A-Fa-f]{7})\b', rhs))
                        break
            for i in ids:
                if i in shorts:
                    continue
                tg |= targets(i)
            # a local dispatch helper (coming2: dispatch(this, TABLE + i * 8)): the table names are its arguments
            tg.discard(f.short)
            if tg:
                f.dispatch[k] = tg
                f.calls |= tg


EXEC_NAME = re.compile(r'(?i)(^|_)execute$|Execute$|^_?execute\w*$|_execute$|exeProc|Execute_')
NONEXEC = re.compile(r'(?i)draw|create|delete|heap|ctor|dtor|sinit|init$|isdelete')


def execute_path(funcs):
    by_short = {}
    for f in funcs:
        by_short.setdefault(f.short, []).append(f)
    roots = [f for f in funcs if EXEC_NAME.search(f.short) and not NONEXEC.search(f.short)]
    seen, todo = set(), list(roots)
    while todo:
        f = todo.pop()
        if id(f) in seen:
            continue
        seen.add(id(f))
        for c in f.calls:
            for g in by_short.get(c, []):
                if NONEXEC.search(g.short) and not EXEC_NAME.search(g.short):
                    continue
                todo.append(g)
    return [f for f in funcs if id(f) in seen], roots


def address_of(f, funcs):
    """the verified address that holds f's code (its own, or the function it is inlined into, transitively)"""
    hops, g = 0, f
    while g and not g.addr and g.inlined_into and hops < 6:
        target = g.inlined_into.split('::')[-1]
        nxt = [h for h in funcs if h.short == target or h.name == g.inlined_into]
        g = nxt[0] if nxt else None
        hops += 1
    return g.addr if g else None


# ------------------------------------------------------------------ block context
COND_KW = re.compile(r'^\s*(?:\}\s*)?(if|else if|else|for|while|switch|case|default|do)\b')


def block_context(code, f):
    """per line of f: list of enclosing block kinds (outermost first) with their condition text"""
    ctx, stack = {}, []
    pending = None  # (kind, cond) of an if/for/... whose brace is on a later line
    single = None   # a braceless if/else body: applies to the next statement line
    for ln in range(f.body, f.end + 1):
        s = code[ln]
        st = s.strip()
        kinds = list(stack)
        if single is not None and st and not st.startswith('{'):
            kinds = kinds + [single]
            if st.endswith(';') or st.endswith('}'):
                single = None
        ctx[ln] = kinds
        m = COND_KW.match(s)
        if m:
            kind = m.group(1)
            cond = st
            if '{' in s:
                pending = None
            elif st.endswith(')') or kind in ('else', 'do'):
                single = (kind, cond)
                pending = None
            else:
                # one-line `if (c) stmt;` : the line itself is conditional
                if kind in ('if', 'else if', 'else') and st.endswith(';'):
                    ctx[ln] = kinds + [(kind, cond)]
                pending = None
        for ch in s:
            if ch == '{':
                if m:
                    stack.append((m.group(1), st))
                    m = None
                else:
                    stack.append(('block', st))
            elif ch == '}':
                if stack:
                    stack.pop()
    return ctx


def conditional(kinds):
    return any(k in ('if', 'else if', 'else', 'switch', 'case', 'default', 'while', 'event') for k, _ in kinds)


# ------------------------------------------------------------------ site rules
SPEEDISH = re.compile(r'(?i)speed|vel|spd|move|accel')
ANGLEISH = re.compile(r'(?i)angle|ang\b|phase|wave|rot|theta|swing|spin|yaw|pitch|roll|\.[xyz]$|wobble|sway')
SND = re.compile(r'\b(\w*seStart\w*|\w*SeStart\w*|mDoAud_\w+)\s*\(')
PTC = re.compile(r'\b(dComIfGp_particle_\w+|dPa_control\w*|\w*particle_set\w*)\s*\(')
RND = re.compile(r'\bcM_rnd(F|FX)?\s*\(([^()]*)\)')
EVT_ORDER = re.compile(r'\bfopAcM_order\w*Event\w*|\borderEvent\b|\bdComIfGp_event_order\w*|\bfopAcM_orderChangeEvent\w*')
EVT = re.compile(r'\bdComIfGp_evmng_\w+|\bdComIfGp_event_\w+|\beventInfo\b|\bdEvent_\w+|\bmEvtInfo\b|\bdComIfGp_getEvent\w*|\bdComIfGp_evmng\w*|\bdemoProc\w*')
MSG = re.compile(r'\bfopMsgM_\w+|\bdComIfGp_\w*Mesg\w*|\bdMsg_\w+|\bmMsgNo\b')
INP = re.compile(r'\bCPad_\w+|\b_?mDoCPd_\w+|\bmItemTrigger\b|\bgetStickAng\w*|\bJUTGamePad\b')
HITREAD = re.compile(r'((?:[\w\[\]]+(?:->|\.))*[\w\[\]]+)(->|\.)(ChkTgHit|ChkCoHit|ChkAtHit|ChkAtShieldHit|ChkTgShieldHit)\s*\(\s*\)')
ISSTOP = re.compile(r'((?:[\w\[\]]+(?:->|\.))*[\w\[\]]+)(->|\.)isStop\s*\(\s*\)')
CHECKPASS = re.compile(r'\b(checkPass|checkFrame|checkState)\s*\(')
SETAT = re.compile(r'\bAtSet|\bmObjAt\b|OnAtSPrmBit|OnAtSetBit|\bAtHit|\bAtSPrm|\bSetAtVec|\bsetAtVec|\bOnAtHit')
# the shared-world-state rule (14s) (2026-11-20, pair): shared world state (switches, event bits / the event order, save
# flags): frame-level logic touching it runs on the tick30 step at the actor's line place, where the unconverted and
# whole-form partners run in A's order (a first-step read saw an earlier writer one frame late; a first-step write
# reached earlier readers one frame early)
WORLDSTATE = re.compile(r'\b(?:dComIfGs_\w+|dSv_\w+|\w*(?:is|on|off|chk|check|Is|On|Off)(?:Switch|EventBit|TmpBit|ItemBit|Tbox|Collect)\w*|'
                        r'fopAcM_order\w*Event\w*|\w*orderOtherEvent\w*|\w*orderPotentialEvent\w*|eventInfo\s*\.\s*\w+|'
                        r'\w*[Ee]vt\w*_(?:onCondition|check\w*)|\w*getEventReg\w*|\w*setEventReg\w*)\s*\(')
CCSET = re.compile(r'\b(?:dComIfG_Ccsp\(\)|Ccsp\(\)|\w+Ccsp\w*\(\))\s*->\s*Set\s*\(|\bdComIfG_Ccsp_Set\w*\s*\(|\bcc_set\w*\s*\(')
BGMOVE = re.compile(r'(->|\.)Move\s*\(\s*\)|dComIfG_Bgsp\(\)\s*->\s*Regist|\bride_call_back\b|SetRideCallback|\bmoveBG\b')
PATH = re.compile(r'\bdPath_\w+|\bmPath\w*\b|\bspline\b')
CENTRAL = re.compile(r'\bcLib_(addCalc|chase)\w*|\bfopAcM_(posMove|calcSpeed)\w*|(->|\.)(play|playMorf)\s*\(|\bmDoExt_\w*_play\b|\bmDoExt_baseAnm_play\b')
LIT_INT = r'(?:0x[0-9A-Fa-f]+|\d+)'
LIT_FLT = r'(?:\d+\.\d*(?:[eE][-+]?\d+)?f?|\.\d+f?)'
LV = r'((?:this->)?(?:[A-Za-z_]\w*(?:\[[^\]]+\])?)(?:(?:->|\.)[A-Za-z_]\w*(?:\[[^\]]+\])?)*)'


class Site:
    def __init__(self, ln, func, cat, cls, rule, reason, before, after=None, addr=None):
        self.ln, self.func, self.cat, self.cls, self.rule, self.reason = ln, func, cat, cls, rule, reason
        self.before, self.after, self.addr = before, after, addr

    def js(self):
        return dict(line=self.ln + 1, func=self.func, addr=self.addr, cat=self.cat, cls=self.cls, rule=self.rule,
                    reason=self.reason, before=self.before.strip(), after=(self.after or '').strip())


def field_reads_decide(name, code_all):
    """the field is read by a decision somewhere in the unit (condition, switch, comparison, return)"""
    n = re.escape(name)
    pats = [r'\bif\s*\([^;]*' + n + r'\b', n + r'\s*(==|!=|<=|>=|<|>)', r'(==|!=|<=|>=|<|>)\s*' + n + r'\b',
            r'!\s*' + n + r'\b', r'switch\s*\(\s*' + n, r'return\s+' + n + r'\s*;', r'\?\s*', r'while\s*\([^;]*' + n]
    for line in code_all:
        if name not in line:
            continue
        if re.search(r'^\s*' + n + r'\s*(\+\+|--|[-+*/]?=)', line) and not re.search(r'\bif\b|==|!=|<|>', line):
            continue
        for p in pats[:6] + pats[7:]:
            if re.search(p, line):
                return True
    return False


CAST = re.compile(r'\(\s*(?:s8|u8|s16|u16|s32|u32|int|f32|unsigned|signed)\s*\)')


def uncast(e):
    """an expression without integer casts and redundant parentheses: (s16)((s16)mX + 64) -> mX + 64"""
    e = CAST.sub('', e).strip()
    for _ in range(6):
        e2 = re.sub(r'\(\s*([\w.\->\[\]]+)\s*\)', r'\1', e).strip()
        if e2.startswith('(') and e2.endswith(')'):
            depth, ok = 0, True
            for i, ch in enumerate(e2):
                depth += (ch == '(') - (ch == ')')
                if depth == 0 and i < len(e2) - 1:
                    ok = False
                    break
            if ok:
                e2 = e2[1:-1].strip()
        if e2 == e:
            break
        e = e2
    return e


def is_member(lv):
    return bool(re.search(r'->|\.', lv) or re.match(r'^(?:this->)?(m[A-Z0-9_]\w*|field_\w+|unk\w+)', lv))


def base_field(lv):
    """the member name of an lvalue (array index dropped): this->mTimer[i] -> mTimer"""
    s = re.sub(r'\[[^\]]*\]', '', lv)
    return re.split(r'->|\.', s)[-1]


# step-work primitives by their canonical name (fn_names.h): an `ext ADDR sig NAME(...)` glue line gives such a primitive
# a local name (dai_item standItemAddCalc = cLib_addCalc 0200ECD4, ib playAnimation = mDoExt_baseAnm::play 025E742C,
# movePosition = fopAcM_posMoveF 025D6870); the whole-function first-step gate must see them (2026-11-18)
STEPNAME = re.compile(r'^(cLib_(addCalc|chase)\w*|fopAcM_(posMove|calcSpeed|addAngle)\w*|(\w+::)+(animPlay|anmPlay|playAnim\w*)|mDoMtx_quatSlerp\w*|cM_s(sin|cos)\w*|'
                      r'(\w+::)+(play|playMorf|Move|CrrPos)|mDoExt_\w+_play\w*|\w+::calc[A-Z]?\w*Speed\w*)$')
_fn_names = None


def fn_name_of(addr):
    """canonical name of a guest function address (fn_names.h's `gabi::call<R>(0xADDR, ...)  // NAME` lines)"""
    global _fn_names
    if _fn_names is None:
        _fn_names = {}
        try:
            for l in open(os.path.join(ROOT, 'wwhd_src', 'include', 'fn_names.h')):
                m = re.search(r'gabi::call<R>\(0x([0-9A-Fa-f]{8}),.*//\s*(\S+)\s*$', l)
                if m:
                    _fn_names.setdefault(m.group(1).upper(), m.group(2))
        except OSError:
            pass
    return _fn_names.get(addr.upper())


def src_at(raw):
    """a collider source struct read from the game image (`gabi::at<dCcD_Src...>(0x1...)`, hitobj's cc_sph_src at
    0x101B7020) with its AT part set (cCcD_SrcObj mAtSPrm +0xC bit 0, cCcD_AtSPrm_Set_e): the unit has an AT collider"""
    import struct as _st
    for l in raw:
        for a in re.findall(r'gabi::at<\s*dCcD_Src\w*\s*>\s*\(\s*0x(1[0-9A-Fa-f]{7})\s*\)', l):
            b = rpx_read(int(a, 16) + 0xC, 4)
            if b and _st.unpack('>I', b)[0] & 1:
                return True
    return False


_sound_ext = None


def sound_ext_addrs():
    """guest addresses that some unit's `ext` glue line names as a sound start (SND: *seStart*, mDoAud_*): a sound start
    under a local glue name elsewhere (obj_light ObjLight_sound = 025E19CC mDoAud_seStart(u32, cXyz*); in pair
    wfP1 0x303B twice per frame went unseen by the sound forms and by t60calls, 2026-11-20)"""
    global _sound_ext
    if _sound_ext is None:
        votes = {}
        for dp, _, fs in os.walk(os.path.join(ROOT, 'wwhd_src')):
            for fn in fs:
                if not fn.endswith(('.cpp', '.h')):
                    continue
                try:
                    txt = open(os.path.join(dp, fn), errors='replace').read()
                except OSError:
                    continue
                for m in EXTGLUE.finditer(txt):
                    votes.setdefault(m.group(1).upper(), set()).add(m.group(2))
        _sound_ext = {a for a, ns in votes.items()
                      if any(SND.match(n + '(') and not re.match(r'(?:mDoAud_)?(?:check|is|get|Check|Is|Get)', n.split('_', 1)[-1] if n.startswith('mDoAud_') else n) for n in ns)}
    return _sound_ext


def ext_sound_names(raw):
    """local names of `ext` glue lines whose guest function is a sound start by another unit's name for it"""
    out = set()
    snd = sound_ext_addrs()
    for l in raw:
        m = re.match(r'^\s*ext\s+([0-9A-Fa-f]{8})\s+.*?\b(\w+)\s*\(', l)
        if m and m.group(1).upper() in snd and not SND.match(m.group(2) + '('):
            out.add(m.group(2))
    return out


def ext_step_names(raw):
    """local names of `ext` glue lines whose guest function is a step-work primitive (STEPNAME)"""
    out = set()
    for l in raw:
        m = re.match(r'^\s*ext\s+([0-9A-Fa-f]{8})\s+.*?\b(\w+)\s*\(', l)
        if m:
            nm = fn_name_of(m.group(1))
            if nm and STEPNAME.match(nm):
                out.add(m.group(2))
    return out


class Converter:
    def __init__(self, unit, src):
        self.unit, self.src = unit, src
        self.raw = open(os.path.join(ROOT, src)).read().split('\n')
        self.code = strip_code(self.raw)
        self.funcs = parse_functions(self.raw, self.code)
        self.joined = join_statements(self.raw, self.code, self.funcs)
        self.cont = {x for k, v in self.joined.items() for x in range(k + 1, v[0] + 1)}
        hdr =os.path.join(ROOT, 'wwhd_src', 'include', os.path.dirname(src).replace('wwhd_src/', '', 1), os.path.basename(src)[:-4] + '.h')
        resolve_dispatch(self.raw, self.code, self.funcs, open(hdr).read() if os.path.isfile(hdr) else '')
        self.exe, self.roots = execute_path(self.funcs)
        self.sites = []
        self.new = list(self.raw)
        self.site_no = 0
        self.has_at = any(SETAT.search(l) for l in self.code) or src_at(self.raw)
        self.helpers = set()
        self.ext_step = ext_step_names(self.raw)
        self.ext_sound = ext_sound_names(self.raw)

    def helper_resolved(self, f):
        """f is a dispatch helper (coming2 dispatch(actor, descriptor)): every call of it is a resolved dispatch line"""
        sites = [(g, k) for g in self.funcs if g is not f for k in range(g.body, g.end + 1)
                 if re.search(r'\b' + re.escape(f.short) + r'\s*\(', self.code[k])]
        return bool(sites) and all(k in getattr(g, 'dispatch', {}) for g, k in sites)

    def add(self, ln, f, cat, cls, rule, reason, after=None):
        s = Site(ln, f.name, cat, cls, rule, reason, self.raw[ln], after, address_of(f, self.funcs))
        self.sites.append(s)
        return s

    def next_site(self):
        self.site_no += 1
        return self.site_no

    def keyexpr(self, lv):
        """a guest-address key expression for an lvalue (its address)"""
        return '&' + lv if not lv.startswith('&') else lv

    MPCALL = re.compile(r'^\s*\(\s*this\s*->\s*\*\s*\(?\s*([^()]+?)\s*\)?\s*\)\s*\([^;]*\)\s*;\s*$')
    TABIDX = re.compile(r'\[\s*(?:\(\s*\w+\s*\)\s*)?((?:\w+\s*->\s*)?m\w+)\s*\]|[+*]\s*\(?\s*(?:\(\s*\w+\s*\)\s*)?((?:\w+\s*->\s*)?m\w+)\s*\)?\s*\*')

    def r_procdispatch(self, f, ln, s, kinds):
        """the 30 Hz visual-motion rule (14b(2)) (2026-11-18): a procedure dispatch through a member pointer ((this->*mProc)) or a
        resolved guest table indexed by a member (call_ptr(TABLE[mMode]) / dispatch(this, TABLE + mMode * 8)): wrapped in
        t60c::procBegin / procEnd (a procedure set inside the dispatch starts its body next frame, one set before it on a
        later step runs on the tick30 step). A procedure setter that calls the new body itself, or a dispatch that is not
        a whole statement, goes to review."""
        m = self.MPCALL.match(s)
        word = m.group(1).strip() if m else None
        if word is None and getattr(f, 'dispatch', {}).get(ln):
            t = self.TABIDX.search(s)
            word = (t.group(1) or t.group(2)) if t else None
            if word is None:
                return False  # a table without a member index: not a procedure dispatch (the resolved-table site below)
            if not re.match(r'^\s*[^;{}]*;\s*$', s) or re.match(r'^\s*(return|if|else|for|while)\b', s) or '=' in s.split('(', 1)[0]:
                self.add(ln, f, 'proc', 'review', '14b(2)', 'procedure dispatch on %s inside an expression: hold it by hand (t60c::procBegin / procEnd)' % word)
                return True
        if word is None:
            return False
        field = re.sub(r'\[.*$', '', word).split('->')[-1].split('.')[-1].strip()
        # a setter that falls through to the new body: assigns the word and dispatches on it again in the same function
        if any(re.search(r'\b' + re.escape(field) + r'\b\s*=[^=]', self.code[k]) for k in range(f.body, ln)):
            self.add(ln, f, 'proc', 'review', '14b(2)', 'procedure set and dispatched in %s (falls through to the new body): place the hold by hand' % f.short)
            return True
        for g in self.exe:
            if g is f:
                continue
            body = self.code[g.body:g.end + 1]
            sets = [i for i, l in enumerate(body) if re.search(r'\b' + re.escape(field) + r'\b\s*=[^=]', l)]
            if sets and any(re.search(r'->\s*\*|\bcall_ptr\b|\bdispatch\s*\(', l) and re.search(r'\b' + re.escape(field) + r'\b', l)
                            for l in body[sets[0] + 1:]):
                self.add(ln, f, 'proc', 'review', '14b(2)', 'procedure setter %s runs the new body itself (falls through inside the dispatch): place the hold by hand' % g.short)
                return True
        raw = self.raw[ln]
        indent = re.match(r'\s*', raw).group(0)
        site = self.next_site()
        w = 't60c::procWord(%s)' % word
        ti = self.TABIDX.search(word)
        kf = (ti.group(1) or ti.group(2)) if ti else word  # the key: the actor's own procedure word (an index, not a shared table)
        k = '&(%s)' % kf
        self.new[ln] = indent + 'if (t60c::procBegin(%d, %s, %s)) { %s t60c::procEnd(%d, %s, %s); } /* true60 (t60conv): procedure dispatch, rule note 14b(2) */' % (site, k, w, raw.strip(), site, k, w)
        tg = getattr(f, 'dispatch', {}).get(ln)
        self.add(ln, f, 'proc', 'auto', '14b(2) (procedure dispatch: a new procedure starts next frame, or on the tick30 step when set before the dispatch)',
                 'dispatch on %s%s' % (word, '; targets ' + ', '.join(sorted(tg)) + ' analysed as callees' if tg else ''), self.new[ln])
        return True

    EVENTCOND = re.compile(r'Chk\w*(Landing|Hit)\w*\s*\(|\bChkGround\w*\s*\(|\b(is|check)Landing\w*\s*\(|ChkWallHit|ChkRoofHit')

    def event_count(self, s, kinds, m):
        """ 2026-11-18 (spc_item01's bounce count): an increment under an event test (a landing or a hit: true
        on one step of the frame, at any step rate the same number of times) is a per-event counter, not a decision
        timer: unchanged. Returns the event test, or None."""
        if not re.search(r'\+\+|\+=\s*1|\+\s*1\s*;', s) or re.search(r'--|-=|-\s*1\s*;', s):
            return None
        conds = [c for k, c in kinds if k in ('if', 'else if')]
        if s.lstrip().startswith('if'):
            conds.append(s)
        for c in conds[::-1][:1]:  # the innermost condition decides
            e = self.EVENTCOND.search(c or '')
            if e:
                return e.group(0).strip()
        return None

    # ---- per-line rules; each returns True when it handled the line
    def r_timer(self, f, ln, s, kinds):
        """timer / counter decrements and increments as whole statements"""
        pats = [
            (r'^\s*if\s*\(\s*' + LV + r'\s*(?:!=\s*0|>\s*0)\s*\)\s*\{?\s*\1\s*(--|-=\s*1|\+\+|\+=\s*1)\s*;\s*\}?\s*$', 'guarded'),
            (r'^\s*if\s*\(\s*' + LV + r'\s*\)\s*\{?\s*\1\s*(--|-=\s*1)\s*;\s*\}?\s*$', 'guarded'),
            (r'^\s*' + LV + r'\s*(--|\+\+|-=\s*1|\+=\s*1)\s*;\s*$', 'plain'),
            (r'^\s*(?:--|\+\+)\s*' + LV + r'\s*;\s*$', 'plain'),
            (r'^\s*' + LV + r'\s*=\s*\(?\s*\w*\s*\)?\s*\(?\s*\1\s*\)?\s*([-+])\s*1\s*;\s*$', 'plain'),
            (r'^\s*(?:\(void\)\s*)?cLib_calcTimer\w*(?:<[^>]*>)?\s*\(\s*&\s*' + LV + r'\s*\)\s*;\s*$', 'calc'),
        ]
        for p, kind in pats:
            m = re.match(p, s)
            if not m:
                continue
            lv = m.group(1)
            name = base_field(lv)
            if not is_member(lv):
                return False
            if self.event_count(s, kinds, m):
                self.add(ln, f, 'tmr', 'auto', '14d note (per-event counter: unchanged)', 'field %s counts an event seen on one step (%s)' % (name, self.event_count(s, kinds, m)))
                return True
            if re.search(r'(?i)^(i|j|k|idx|index|n|count)$', name) or not re.match(r'm[A-Z_]|field_|unk|[a-z]\w*', name):
                return False
            if 'for' in [k for k, _ in kinds] and re.match(r'^(i|j|k)$', name):
                return False
            decide = field_reads_decide(name, self.code)
            gate = 't60c::first()' if decide else 't60c::end()'
            rule = '14d (decision timer: first step)' if decide else '14d / 14b (visual counter: tick30 step)'
            raw = self.raw[ln]
            indent = re.match(r'\s*', raw).group(0)
            body = raw.strip()
            self.new[ln] = indent + 'if (%s) { %s } /* true60 (t60conv): %s */' % (gate, body, 'counted on the frame\'s first step' if decide else 'counted on the tick30 step')
            self.add(ln, f, 'tmr', 'auto', rule, 'field %s %s' % (name, 'read by a decision' if decide else 'visual only'), self.new[ln])
            self.timers.add(name)
            if decide:
                self.first_timers.add(name)
            return True
        m = re.match(r'^\s*' + LV + r'\s*=\s*([^;]+);\s*$', s)
        if m and is_member(m.group(1)):
            lv = m.group(1)
            u = uncast(m.group(2))
            if re.fullmatch(re.escape(lv) + r'\s*([-+])\s*1u?', u) and re.search(r'(?i)tim|cnt|count|wait|delay|frame|age|life|ctr|step', base_field(lv)):
                name = base_field(lv)
                decide = field_reads_decide(name, self.code)
                gate = 't60c::first()' if decide else 't60c::end()'
                raw = self.raw[ln]
                indent = re.match(r'\s*', raw).group(0)
                self.new[ln] = indent + 'if (%s) { %s } /* true60 (t60conv): %s */' % (gate, raw.strip(), 'counted on the frame\'s first step' if decide else 'counted on the tick30 step')
                self.add(ln, f, 'tmr', 'auto', '14d (decision timer: first step)' if decide else '14d / 14b (visual counter: tick30 step)', 'field %s (cast form)' % name, self.new[ln])
                self.timers.add(name)
                if decide:
                    self.first_timers.add(name)
                return True
        # a counter carried through a local: `s16 t = mX; ... mX = t + 1;` or `t--; ... mX = t;`
        m = re.match(r'^\s*' + LV + r'\s*=\s*([^;]+);\s*$', s)
        if m and is_member(m.group(1)):
            mem, u = m.group(1), uncast(m.group(2))
            mm = re.fullmatch(r'([A-Za-z_]\w*)(?:\s*([-+])\s*1u?)?', u)
            if mm and not is_member(mm.group(1)):
                loc = mm.group(1)
                body = self.code[f.body:ln]
                src = any(re.search(r'\b' + loc + r'\s*=\s*[^;=]*\b' + re.escape(mem) + r'\b', c) for c in body)
                stepped = mm.group(2) is not None or any(re.search(r'\b' + loc + r'\s*(--|\+\+|[-+]=\s*1\b)|\b' + loc + r'\s*=\s*[^;]*\b' + loc + r'\s*[-+]\s*1', c) for c in body)
                stepj = [f.body + i for i, c in enumerate(body) if re.match(r'^\s*(?:' + loc + r'\s*(--|\+\+|[-+]=\s*1u?)|(?:--|\+\+)\s*' + loc + r'|' + loc + r'\s*=\s*[^;]*\b' + loc + r'\b[^;]*[-+]\s*1u?\b[^;]*)\s*;\s*$', c)]
                if src and stepped and mm.group(2) is None and stepj:
                    j = stepj[-1]
                    name = base_field(mem)
                    raw = self.raw[j]
                    indent = re.match(r'\s*', raw).group(0)
                    self.new[j] = indent + 'if (t60c::first()) { %s } /* true60 (t60conv): counted on the frame\'s first step */' % raw.strip()
                    # the step line was seen first by the review rule below (a decrement through a local): it is placed now
                    self.sites = [x for x in self.sites if not (x.ln == j and x.cls == 'review' and x.cat == 'tmr')]
                    self.add(j, f, 'tmr', 'auto', '14d (counter carried through a local: its step on the first step)',
                             'field %s via local %s; later steps see the stored value unchanged' % (name, loc), self.new[j])
                    self.timers.add(name)
                    self.first_timers.add(name)
                    self.timer_locals.setdefault(f.name, set()).add(loc)
                    self._first_step_end_tests(f, j, loc, name)
                    return True
                if src and stepped and mm.group(2) is not None:
                    name = base_field(mem)
                    raw = self.raw[ln]
                    indent = re.match(r'\s*', raw).group(0)
                    self.new[ln] = indent + 'if (t60c::first()) { %s } /* true60 (t60conv): counted on the frame\'s first step */' % raw.strip()
                    self.add(ln, f, 'tmr', 'auto', '14d (counter carried through a local: its store on the first step)',
                             'field %s via local %s; later steps keep the stored value (decisions on the local in the same function: check them)' % (name, loc), self.new[ln])
                    self.timers.add(name)
                    self.first_timers.add(name)
                    self.timer_locals.setdefault(f.name, set()).add(loc)
                    return True
        # a counter stepped in a local's initialiser and stored back (`s16 count = s16(u16(mLitCount) + (hit ? 1u :
        # 0xFFFFu)); <clamp>; mLitCount = count;`, tag_light): the step on the first step, later steps start from the
        # stored value (14d)
        m = re.match(r'^(\s*(?:const\s+)?(?:s8|u8|s16|u16|s32|u32|int)\s+)([A-Za-z_]\w*)\s*=\s*([^;]+);\s*$', s)
        if m and not any(k == 'event' for k, _ in kinds):
            loc, expr = m.group(2), m.group(3)
            mems = [x for x in re.findall(r'(?<![\w.>])((?:this->)?m[A-Z]\w*)\b', expr)]
            if len(set(mems)) == 1 and re.search(r'(?<![\w.])(1u?|0xFFFFu?|0xFFFFFFFFu?)\b', expr) and not re.search(r'\bcM_rnd', expr):
                mem = mems[0]
                store = [j for j in range(ln + 1, min(ln + 12, f.end)) if re.match(r'^\s*' + re.escape(mem) + r'\s*=\s*(?:\(\w+\)\s*)?' + loc + r'\s*;', self.code[j])]
                if store and uncast(expr) != mem:
                    raw = self.raw[ln]
                    i = raw.find(m.group(3).strip()[:20])
                    rhs = raw[raw.find('=', len(m.group(1))) + 1:raw.rstrip().rfind(';')].strip()
                    typ = m.group(1).split()[-1]
                    self.new[ln] = raw[:raw.find('=', len(m.group(1))) + 1] + ' t60c::first() ? %s(%s) : %s(%s);' % (typ, rhs, typ, mem) + \
                        ' /* true60 (t60conv): the count on the frame\'s first step */'
                    name = base_field(mem)
                    self.add(ln, f, 'tmr', 'auto', '14d (counter stepped in a local\'s initialiser: first step)', 'field %s via local %s' % (name, loc), self.new[ln])
                    self.timers.add(name)
                    self.first_timers.add(name)
                    self.timer_locals.setdefault(f.name, set()).add(loc)
                    return True
        # a decrement through a local or an expression: review
        m = re.search(LV + r'\s*=\s*[^;]*\b\1\s*-\s*1\b', s) or re.search(r'(\w+)\s*=\s*\(?\w*\)?\s*' + LV + r'\s*-\s*1\s*;', s)
        if m and re.search(r'(?i)tim|cnt|count|wait|delay|frame|age|life', s):
            self.add(ln, f, 'tmr', 'review', '4 / 14d', 'timer decrement through an expression or a local: place the count by hand')
            return True
        if re.search(r'\bcLib_calcTimer\w*\s*\(', s):
            m = re.search(r'cLib_calcTimer\w*(?:<[^>]*>)?\s*\(\s*&\s*' + LV + r'\s*\)', s)
            if m:
                lv = m.group(1)
                call = m.group(0)
                self.new[ln] = self.new[ln].replace(call, '(t60c::first() ? %s : %s)' % (call, lv), 1)
                self.add(ln, f, 'tmr', 'auto', '14d (decision timer in an expression: first step)', 'cLib_calcTimer in a condition; later steps read the counted value', self.new[ln])
                self.timers.add(base_field(lv))
                self.first_timers.add(base_field(lv))
                return True
        return False

    def _if_cond_span(self, text):
        """(start, end) of the condition inside the first `if (...)` of a raw line (strings and comments skipped)"""
        m = re.search(r'\bif\s*\(', text)
        if not m:
            return None
        i, depth, q = m.end(), 1, None
        while i < len(text):
            ch = text[i]
            if q:
                if ch == '\\':
                    i += 2
                    continue
                if ch == q:
                    q = None
            elif ch in '"\'':
                q = ch
            elif text.startswith('/*', i):
                j = text.find('*/', i + 2)
                i = len(text) if j < 0 else j + 2
                continue
            elif ch == '(':
                depth += 1
            elif ch == ')':
                depth -= 1
                if depth == 0:
                    return m.end(), i
            i += 1
        return None

    def _body_range(self, ln):
        """lines of the body of the if / else header at ln: (first, last), braced or one statement"""
        c = self.code[ln]
        if c.rstrip().endswith('{'):
            depth, k = 0, ln
            while k < len(self.code):
                t = re.sub(r'^\s*\}', '', self.code[k]) if k == ln else self.code[k]
                for ch in t:  # left to right: a `} else {` line closes the body at its '}'
                    depth += (ch == '{') - (ch == '}')
                    if depth <= 0 and k > ln:
                        return ln + 1, k - 1, k
                k += 1
            return None
        if c.rstrip().endswith(';'):
            return ln, ln, ln
        k = ln + 1
        while k < len(self.code) and not self.code[k].strip():
            k += 1
        if k < len(self.code) and self.code[k].rstrip().endswith(';') and '{' not in self.code[k]:
            return k, k, k
        return None

    def _else_of(self, rng):
        """the line of the else belonging to an if whose body range is rng (from _body_range), or None"""
        lo, hi, close = rng
        if re.match(r'^\s*\}\s*else\b', self.code[close]) and close != lo:
            return close
        k = close + 1
        while k < len(self.code) and not self.code[k].strip():
            k += 1
        if k < len(self.code) and re.match(r'^\s*else\b', self.code[k]):
            return k
        return None

    def _first_step_end_tests(self, f, j, loc, name):
        """a counter stepped through a local on the first step (above): a later test of that local in the function (the
        counter's end / reach test: `if (!(frame < max)) mX = 0;`) is a timer decision and moves to the first step with
        it (14d); on a later step it saw the unstepped local, or a sibling branch's first-step start, a frame early
        (d_a_arrow mBtkFrame, T-02 input 1000265). A plain `if` without an else is gated
        (`if (t60c::first() && (...))`); an `else if` or an `if` with an else branch goes to review (gating its condition
        would run the else on the later steps)."""
        for k in range(j + 1, f.end):
            c = self.code[k]
            m = re.match(r'^\s*(\}\s*else\s+)?if\s*\(', c)
            if not m:
                continue
            open_i = c.index('(', m.end() - 1)
            depth, close_i = 0, -1
            for q in range(open_i, len(c)):
                depth += {'(': 1, ')': -1}.get(c[q], 0)
                if depth == 0:
                    close_i = q
                    break
            if close_i < 0 or not re.search(r'\b' + re.escape(loc) + r'\b', c[open_i:close_i]):
                continue
            if 't60c::' in c or self.new[k] != self.raw[k]:
                continue
            has_else = bool(m.group(1))
            if not has_else:  # the statement's end: braces balanced (or the single body line); an else right after it
                depth, q = 0, k
                while q < f.end:
                    depth += self.code[q].count('{') - self.code[q].count('}')
                    if depth <= 0 and (q > k or '{' not in self.code[k]):
                        if q == k and '{' not in self.code[k] and not c.rstrip().endswith(';'):
                            q += 1  # `if (...)` alone: its body is the next line
                        break
                    q += 1
                tail = self.code[q] if q < f.end else ''
                nxt = self.code[q + 1] if q + 1 < f.end else ''
                has_else = bool(re.search(r'\}\s*else\b', tail) or re.match(r'^\s*else\b', nxt))
            if has_else:
                self.add(k, f, 'tmr', 'review', '14d (end test on a first-step counter)',
                         'field %s via local %s: this test has an else branch; place it by hand (first step, the else '
                         'on the later steps?)' % (name, loc), None)
                continue
            raw = self.raw[k]
            ri = raw.find(c[open_i:close_i + 1])
            if ri < 0:
                self.add(k, f, 'tmr', 'review', '14d (end test on a first-step counter)',
                         'field %s via local %s: condition not found in the raw line' % (name, loc), None)
                continue
            cond = c[open_i:close_i + 1]
            self.new[k] = raw[:ri] + '(t60c::first() && ' + cond + ')' + raw[ri + len(cond):]
            self.add(k, f, 'tmr', 'auto', '14d (end test on a first-step counter: first step)',
                     'field %s via local %s' % (name, loc), self.new[k])

    def _gate_enclosing_if(self, ln, f, why):
        """the innermost if around line ln (and its plain else) decided on the first step; False when it has motion, an
        else-if chain, or was rewritten already"""
        k, depth, hdr = ln - 1, 0, None
        while k > f.body:
            c = self.code[k]
            depth += c.count('}') - c.count('{')
            if depth < 0:
                if re.match(r'^\s*(?:\}\s*)?(?:else\s+)?if\s*\(', c):
                    hdr = k
                break
            k -= 1
        if hdr is None or self.new[hdr] != self.raw[hdr]:
            return False
        rng = self._body_range(hdr)
        if not rng or self._motion(rng[0], rng[1]):
            return False
        e = self._else_of(rng)
        if e is not None:
            if re.match(r'^\s*(\}\s*)?else\s+if\b', self.code[e]) or self.new[e] != self.raw[e]:
                return False
            er = self._body_range(e)
            if not er or self._motion(er[0], er[1]):
                return False
        raw = self.new[hdr]
        sp = self._if_cond_span(raw)
        if not sp:
            return False
        self.new[hdr] = raw[:sp[0]] + 't60c::first() && (' + raw[sp[0]:sp[1]] + ')' + raw[sp[1]:].rstrip() + ' /* true60 (t60conv): %s */' % why
        if e is not None:
            self.new[e] = re.sub(r'\belse\b', 'else if (t60c::first())', self.raw[e], count=1).rstrip() + ' /* true60 (t60conv): the other branch, on the first step */'
        return True

    def _motion(self, lo, hi):
        for k in range(lo, hi + 1):
            if self.STEPWORK.search(self.code[k]):
                return True
            for g in self.exe:
                if re.search(r'\b' + re.escape(g.short) + r'\s*\(', self.code[k]) and self.step_work(g):
                    return True
        return False

    def r_edgeclear(self, f, ln, s, kinds):
        """an edge flag cleared at the top of a step function and set again in the same function when something changes
        (`M_state = M_state & ~STATE_2; ... if (changed) M_state |= STATE_2;`, tag_waterlevel): the flag means "changed
        this frame", so it is cleared on the frame's first step only (a later step would clear the frame's edge; 14d)"""
        if conditional(kinds):
            return False
        m = re.match(r'^\s*([A-Za-z_][\w.\->\[\]]*)\s*(?:&=\s*~\s*\(?\s*(\w+)\s*\)?|=\s*\(?\s*\1\s*\)?\s*&\s*~\s*\(?\s*(\w+)\s*\)?)\s*;\s*$', s)
        if not m:
            return False
        x, k = m.group(1), m.group(2) or m.group(3)
        later = '\n'.join(self.code[ln + 1:f.end + 1])
        if not re.search(re.escape(x) + r'\s*(?:\|=[^;]*\b' + k + r'\b|=[^;]*\b' + re.escape(x) + r'\b[^;]*\|[^;]*\b' + k + r'\b)', later):
            return False
        raw = self.raw[ln]
        indent = re.match(r'\s*', raw).group(0)
        self.new[ln] = indent + 'if (t60c::first()) { %s } /* true60 (t60conv): the frame\'s edge flag cleared on the first step */' % raw.strip()
        self.add(ln, f, 'frm', 'auto', '14d (edge flag: cleared on the first step)', 'edge flag %s of %s' % (k, x), self.new[ln])
        return True

    def r_timerdecide(self, f, ln, s, kinds):
        """a decision on a timer counted on the frame's first step (`T == 0`, `!T`, `T <= 0`, `T == v`) in a function
        that runs on every step without a whole-function gate: the decision is taken on the first step only, so a later
        step does not repeat its one-shot work (the first-step AI-timer rule (14d)). A body with per-step motion keeps the decision for review
        (hold it and gate the frame-level statements by hand)."""
        if any(k == 'event' for k, _ in kinds) or not re.match(r'^\s*(?:\}\s*)?(?:else\s+)?if\s*\(', s):
            return False
        names = set(self.first_timers) | self.timer_locals.get(f.name, set())
        if not names:
            return False
        span = self._if_cond_span(s)
        if not span:
            return False
        cond = s[span[0]:span[1]]
        alt = '|'.join(re.escape(n) for n in sorted(names))
        tv = r'(?:\(\s*\w+\s*\)\s*)?(?:this->)?(?:' + alt + r')\b(?:\[[^\]]*\])?'
        if not (re.search(tv + r'\s*==', cond) or re.search(tv + r'\s*(<=\s*0|<\s*1)\b', cond) or
                re.search(r'!\s*' + tv + r'(?!\s*[!=<>])', cond) or re.search(r'==\s*' + tv, cond)):
            return False
        if re.search(tv + r'\s*<\s*0\b', cond):
            return False
        rng = self._body_range(ln)
        if not rng:
            return False
        lo, hi, close = rng
        # the guard of the count itself (`if (T < 250) T++;`) is not a decision
        if any(x.cat == 'tmr' and x.after and lo <= x.ln <= hi for x in self.sites):
            return False
        body = ' '.join(self.code[k] for k in range(lo, hi + 1)).strip()
        if re.fullmatch(r'(?:\{\s*)?(?:return\b[^;]*|break|continue)\s*;\s*\}?', re.sub(r'^.*\)\s*', '', body) if lo == ln else body):
            return False
        if self._motion(lo, hi):
            self.add(ln, f, 'tmr', 'review', '14d', 'decision on a first-step timer with per-step motion in its body: hold the decision (t60c::held) and gate the frame-level statements on the first step by hand')
            return True
        # the else branch (plain else only): frame-level too, then also on the first step
        else_ln = None
        k = close + 1 if lo != ln or close != ln else ln + 1
        if self.code[close].strip().startswith('}') and re.match(r'^\s*\}\s*else\b', self.code[close]):
            else_ln = close
        else:
            while k < len(self.code) and not self.code[k].strip():
                k += 1
            if k < len(self.code) and re.match(r'^\s*else\b', self.code[k]):
                else_ln = k
        if else_ln is not None:
            if re.match(r'^\s*(\}\s*)?else\s+if\b', self.code[else_ln]):
                self.add(ln, f, 'tmr', 'review', '14d', 'decision on a first-step timer with an else-if chain: place the first-step decision by hand')
                return True
            erng = self._body_range(else_ln)
            if not erng or self._motion(erng[0], erng[1]):
                self.add(ln, f, 'tmr', 'review', '14d', 'decision on a first-step timer whose else branch moves: hold the decision by hand')
                return True
        raw = self.new[ln]
        sp = self._if_cond_span(raw)
        if not sp:
            return False
        self.new[ln] = raw[:sp[0]] + 't60c::first() && (' + raw[sp[0]:sp[1]] + ')' + raw[sp[1]:].rstrip() + \
            ' /* true60 (t60conv): a decision on the first-step timer, on the first step */'
        self.add(ln, f, 'tmr', 'auto', '14d (decision on a first-step timer: first step)', 'timer decision %s' % cond.strip()[:60], self.new[ln])
        if else_ln is not None and self.new[else_ln] == self.raw[else_ln]:
            self.new[else_ln] = re.sub(r'\belse\b', 'else if (t60c::first())', self.raw[else_ln], count=1).rstrip() + \
                ' /* true60 (t60conv): the other branch of the timer decision, on the first step */'
            self.add(else_ln, f, 'tmr', 'auto', '14d (decision on a first-step timer: first step)', 'else of the timer decision', self.new[else_ln])
        return True

    def r_oneshot(self, f, ln, s, kinds):
        """`timer == v` (v != 0) after a count on the first step: a one-shot; only on the first step"""
        hit = False
        for name in list(self.timers):
            for m in re.finditer(r'(' + LV + r')\s*==\s*(' + LIT_INT + r')\b', s):
                if base_field(m.group(2)) == name and m.group(3) not in ('0', '0x0'):
                    hit = True
        if hit and re.match(r'^\s*(?:\}\s*)?(?:else\s+)?if\s*\(', s):
            self.new[ln] = re.sub(r'\bif\s*\(', 'if (t60c::first() && ', self.new[ln], count=1)
            self.add(ln, f, 'frm', 'auto', '14d (timer one-shot on the first step)', 'timer == v crossing', self.new[ln])
            return True
        return False

    def r_rnd(self, f, ln, s, kinds):
        ms = list(RND.finditer(s))
        if not ms:
            return False
        # decision: a comparison in an if condition
        if re.match(r'^\s*(?:\}\s*)?(?:else\s+)?if\s*\(', s) and re.search(r'cM_rnd\w*\s*\([^)]*\)\s*[<>]|[<>]=?\s*cM_rnd', s):
            # an if with an else (next lines) would change its else path on later steps: review then
            nxt = ''.join(self.code[ln + 1:ln + 40])
            if self._delete_block(ln, f):
                self.add(ln, f, 'rnd', 'auto', '8 (per-event draw, unchanged)', 'chance comparison in a block that deletes the actor: one draw per event')
                return True
            has_else = self._has_else(ln)
            if has_else:
                self.add(ln, f, 'rnd', 'review', '8 / 14d addendum', 'decision draw with an else branch: gating would move later steps onto the else path')
                return True
            self.new[ln] = re.sub(r'\bif\s*\(', 'if (t60c::first() && ', self.new[ln], count=1)
            self.add(ln, f, 'rnd', 'auto', '14d addendum (decision draw on the first step, once per frame)', 'draw in a chance comparison', self.new[ln])
            return True
        if any(k == 'event' for k, _ in kinds) or (conditional(kinds) and (self._in_change_block(ln, f) or self._timer_cond(kinds))):
            self.add(ln, f, 'rnd', 'auto', '8 (per-event draw, unchanged)', 'draw inside a state-change / timer block')
            return True
        if not conditional(kinds):
            # per-frame visual draw: on the tick30 step (original RNG position), the mean between
            out = self.new[ln]
            for m in ms:
                kind, arg = m.group(1) or '', m.group(2).strip()
                if kind == 'F':
                    mean = '(%s) * 0.5f' % arg
                elif kind == 'FX':
                    mean = '0.0f'
                else:
                    mean = '0.5f'
                out = out.replace(m.group(0), '(t60c::end() ? %s : %s)' % (m.group(0), mean), 1)
            self.new[ln] = out
            self.add(ln, f, 'rnd', 'auto', '8 (visual draw on the tick30 step; the mean between)', 'unconditional per-frame draw', out)
            return True
        self.add(ln, f, 'rnd', 'review', '8 / 14d', 'draw in a conditional block that is not a clear state change')
        return True

    def _has_else(self, ln):
        depth = 0
        for k in range(ln, min(len(self.code), ln + 80)):
            c = self.code[k]
            if k > ln and depth == 0 and re.match(r'^\s*(\}\s*)?else\b', c):
                return True
            depth += c.count('{') - c.count('}')
            if k > ln and depth <= 0 and not re.match(r'^\s*(\}\s*)?else\b', self.code[k + 1] if k + 1 < len(self.code) else ''):
                if '}' in c or (depth == 0 and c.strip().endswith(';')):
                    return False
        return False

    def _in_change_block(self, ln, f):
        """the line sits in a block that assigns a mode/proc/state field or sets a timer (an event block)"""
        k = ln
        depth = 0
        while k > f.body:
            c = self.code[k]
            depth += c.count('}') - c.count('{')
            if depth < 0:
                break
            k -= 1
        start = k
        if start <= f.body:
            return False
        depth, k = 0, start
        while k <= f.end:
            c = self.code[k]
            if re.search(r'\bm\w*(Mode|Proc|Action|Step|State|Seq)\w*\s*=[^=]|setAction|setMode|changeProc|procChange|\bm\w*(Timer|Cnt|Count|Wait)\w*(\[[^\]]*\])?\s*=[^=]', c):
                return True
            depth += c.count('{') - c.count('}')
            if depth <= 0 and k > start:
                break
            k += 1
        return False

    def _block_start(self, ln, f):
        """the line that opens the innermost block around line ln (None at the function body)"""
        k, depth = ln - 1, 0
        while k > f.body:
            c = self.code[k]
            depth -= c.count('{')
            if depth < 0:
                return k
            depth += c.count('}')
            k -= 1
        return None

    def _block_lines(self, start, f):
        """(line, depth) of the block opened at start; depth 1 = directly in the block"""
        depth, k, out = 0, start, []
        while k <= f.end:
            c = self.code[k]
            if k > start:
                out.append((k, depth))
            depth += c.count('{') - c.count('}')
            if depth <= 0 and k > start:
                break
            k += 1
        return out

    def _hit_block(self, ln, f):
        """the line sits in a block opened by a collision hit test (a line / ground cross, a collider hit): an event
        (esa bg_check's `angle.y -= 0x8000` under cBgS_LineCross)"""
        st = self._block_start(ln, f)
        return st is not None and bool(re.search(r'LineCross|GroundCross|\bChk\w*Hit\b|ChkWallHit|getHitObj|\bCross\s*\(', self.code[st]))

    def _dispatch_block(self, ln, f):
        """the line sits directly in a state-dispatch block (`if (X == K) {`, `else if (X == K) {`, `case K:`) whose state
        changes are all nested in their own conditions: the block runs on every frame of its state (esa act 3's bob,
)"""
        st = self._block_start(ln, f)
        if st is None or not re.search(r'\bif\s*\(\s*[\w.\->]+\s*==\s*[\w]+\s*\)\s*\{\s*$|\bcase\s+\w+\s*:', self.code[st]):
            return False
        chg = r'\bm\w*(Mode|Proc|Action|Step|State|Seq)\w*\s*=[^=]|setAction|setMode|changeProc|procChange|\bm\w*(Timer|Cnt|Count|Wait)\w*(\[[^\]]*\])?\s*=[^=]|fopAcM_delete'
        return not any(d == 1 and re.search(chg, self.code[k]) for k, d in self._block_lines(st, f))

    def _delete_block(self, ln, f):
        """a chance comparison inside a block that deletes the actor: one draw per event (esa's bait taken)"""
        st = self._block_start(ln, f)
        return st is not None and any(re.search(r'fopAcM_delete\b', self.code[k]) for k, _ in self._block_lines(st, f))

    def r_phase(self, f, ln, s, kinds):
        """integer per-frame adds: X += K (K a literal, not 1) -> share; X += expr (integer field) -> frameAdd"""
        m = re.match(r'^\s*' + LV + r'\s*(\+=|-=)\s*(' + LIT_INT + r')\s*;\s*$', s)
        if m and m.group(3) not in ('1', '0x1') and is_member(m.group(1)) and self._hit_block(ln, f):
            self.add(ln, f, 'smth', 'auto', '8 (per-event add under a collision hit, unchanged)', 'constant add to %s in a hit block' % m.group(1))
            return True
        if m and m.group(3) not in ('1', '0x1') and is_member(m.group(1)):
            lv, op, k = m.group(1), m.group(2), m.group(3)
            if not ANGLEISH.search(lv) and not re.search(r'(?i)phase|wave|angle|rot|cnt|frame', base_field(lv)):
                pass
            raw = self.raw[ln]
            self.new[ln] = raw.replace(k, 't60c::share(%s)' % k, 1).rstrip() + ' /* true60 (t60conv): the step\'s share */'
            self.add(ln, f, 'smth', 'auto', '6 / cloth60_share (integer per-frame add: exact per 30 Hz frame)', 'constant add to %s' % lv, self.new[ln])
            return True
        m = re.match(r'^\s*(?:(?:s16|u16|s32|u32|int)\s+)?' + LV + r'\s*=\s*([^;]+);\s*$', s)
        if m:
            tgt, u = m.group(1), uncast(m.group(2))
            mm = re.fullmatch(r'((?:this->)?[A-Za-z_][\w.\->\[\]]*)\s*([-+])\s*(' + LIT_INT + r')', u)
            if mm and is_member(mm.group(1)) and mm.group(3) not in ('1', '0x1') and \
                    not re.search(r'(?i)tim|cnt|count|wait|delay', base_field(mm.group(1))):
                mem, k = mm.group(1), mm.group(3)
                ok = tgt == mem or any(re.match(r'^\s*' + re.escape(mem) + r'\s*=\s*(?:\(\w+\)\s*)?' + re.escape(tgt) + r'\s*;', self.code[j])
                                       for j in range(ln + 1, min(ln + 4, len(self.code))))
                if ok and (not self._in_change_block(ln, f) or self._dispatch_block(ln, f)):
                    bf = re.escape(base_field(mem))
                    early = [j for j in range(f.body, ln) if re.search(r'\b' + bf + r'\b', self.code[j])
                             and not re.search(r'\b' + bf + r'\b\s*=\s*[^=]', self.code[j])
                             and not re.match(r'^\s*(?:\w+\s+)?\w+\s*=\s*\(?\s*(?:\(\w+\)\s*)?\(?\s*(?:\(\w+\)\s*)?' + r'(?:this->)?' + bf + r'\s*\)?\s*[-+]', self.code[j])]
                    raw = self.raw[ln]
                    i = raw.rfind(k)
                    self.new[ln] = (raw[:i] + 't60c::share(%s)' % k + raw[i + len(k):]).rstrip() + ' /* true60 (t60conv): the step\'s share */'
                    if early:
                        self.add(ln, f, 'smth', 'review', '6 / cloth60_share; 14b', 'constant add to %s, read earlier in the function (line %d): values derived before the add end the frame from an intermediate phase; T-02 exact fails on them: accept with a tolerance, or keep the phase on the 30 Hz grid (14b)' % (mem, early[0] + 1), self.new[ln])
                    else:
                        self.add(ln, f, 'smth', 'auto', '6 / cloth60_share (integer per-frame add, cast form)', 'constant add to %s' % mem, self.new[ln])
                    return True
        m = None
        if m and m.group(2) not in ('1', '0x1') and is_member(m.group(1)):
            k = m.group(2)
            raw = self.raw[ln]
            self.new[ln] = re.sub(r'\+\s*' + re.escape(k), '+ t60c::share(%s)' % k, raw, count=1).rstrip() + ' /* true60 (t60conv): the step\'s share */'
            self.add(ln, f, 'smth', 'auto', '6 / cloth60_share', 'constant add', self.new[ln])
            return True
        m = re.match(r'^\s*' + LV + r'\s*(\+=|-=)\s*([A-Za-z_]\w*)\s*;\s*$', s)
        if m:
            local = m.group(3)
            body = '\n'.join(self.code[f.body:ln])
            if re.search(r'\b(s32|s16|u16|u32|int|s8|u8)\s+' + local + r'\s*=', body) and not conditional(kinds) and not self._in_change_block(ln, f):
                lv = m.group(1)
                sign = '' if m.group(2) == '+=' else '-'
                self.new[ln] = self.raw[ln].replace(local, 't60c::frameAdd(&%s, %d, %s)' % (lv, self.next_site(), local), 1).rstrip() + ' /* true60 (t60conv): the step\'s share, exact per frame */'
                self.add(ln, f, 'smth', 'auto', '6 / sail60_wobbleAdd form (integer per-frame add of a computed amount: exact per frame)', 'add of a local to %s' % lv, self.new[ln])
                return True
        return False

    def r_float(self, f, ln, s, kinds):
        """float motion and damping"""
        # a hand-stepped float carried through a local: `f32 f = mFrame + 1.0f; mFrame = f;` (tornado btk frames): the animation-rate rule (7)
        m = re.match(r'^\s*(?:(?:f32|float)\s+)?([A-Za-z_]\w*)\s*=\s*' + LV + r'\s*([-+])\s*(' + LIT_FLT + r'|attr\(\)\s*(?:\.|->)\s*\w+|\w+\(\)\s*->\s*m\w+)\s*;\s*$', s)
        if m and is_member(m.group(2)) and not is_member(m.group(1)) and not any(k == 'event' for k, _ in kinds) and \
                not (conditional(kinds) and self._in_change_block(ln, f) and not re.search(r'(?i)step|frame|speed', m.group(2))):
            loc, mem, lit = m.group(1), m.group(2), m.group(4)
            if any(re.match(r'^\s*' + re.escape(mem) + r'\s*=\s*' + re.escape(loc) + r'\s*;', self.code[j])
                   for j in range(ln + 1, min(ln + 9, len(self.code)))):
                raw = self.raw[ln]
                i = raw.rfind(lit)
                self.new[ln] = raw[:i] + 't60c::step(%s)' % lit + raw[i + len(lit):]
                self.add(ln, f, 'smth', 'auto', '7 (per-frame float step x dt, through a local)', 'hand-stepped %s' % mem, self.new[ln])
                self.float_sites = True
                return True
        lvm = re.match(r'^\s*' + LV, s)
        if lvm and not is_member(lvm.group(1)) and not re.search(r'madds|quatSlerp', s):
            # a local carried into a member a few lines later (time += 0.4f; save->mTime = time;)
            loc = lvm.group(1)
            if not any(re.match(r'^\s*' + LV + r'\s*=\s*' + re.escape(loc) + r'\s*;', self.code[j]) and is_member(re.match(r'^\s*' + LV, self.code[j]).group(1))
                       for j in range(ln + 1, min(ln + 5, len(self.code)))):
                return False
        # X *= lit (0 < lit < 1): damping
        m = re.match(r'^\s*' + LV + r'\s*\*=\s*(' + LIT_FLT + r')\s*;\s*$', s)
        if m:
            v = float(m.group(2).rstrip('f'))
            if 0.0 < v < 1.0:
                self.new[ln] = self.raw[ln].replace(m.group(2), 't60c::damp(%s)' % m.group(2), 1)
                self.add(ln, f, 'smth', 'auto', '3 (damping d -> d^dt)', 'per-frame damping', self.new[ln])
                return True
        # X += (T - X) * lit / fmadds(T - X, lit, X)
        m = re.match(r'^\s*' + LV + r'\s*\+=\s*\(([^;]*)-\s*\1\s*\)\s*\*\s*(' + LIT_FLT + r')\s*;\s*$', s)
        if m:
            self.new[ln] = self.raw[ln].replace(m.group(3), 't60c::ratio(%s)' % m.group(3), 1)
            self.add(ln, f, 'smth', 'auto', '3 (ratio r -> 1-(1-r)^dt)', 'raw smoothing', self.new[ln])
            return True
        m = re.search(r'\bf(?:n)?madds\s*\(\s*([^,;]*)-\s*([\w.\->\[\]]+)\s*,\s*(' + LIT_FLT + r')\s*,\s*\2\s*\)', s)
        if m:
            self.new[ln] = self.raw[ln].replace(m.group(0), m.group(0).replace(m.group(3), 't60c::ratio(%s)' % m.group(3), 1), 1)
            self.add(ln, f, 'smth', 'auto', '3 (ratio)', 'fused smoothing', self.new[ln])
            return True
        m = re.search(r'\bmDoMtx_quatSlerp\s*\(([^;]*),\s*(' + LIT_FLT + r')\s*\)', s)
        if m:
            self.new[ln] = self.raw[ln].replace(m.group(0), m.group(0)[:-len(m.group(2)) - 1] + 't60c::ratio(%s))' % m.group(2), 1)
            self.add(ln, f, 'smth', 'auto', '3 (slerp ratio)', 'per-frame slerp', self.new[ln])
            return True
        # a plain local (not a member) is built fresh on each call: `y += 50.f` there is an offset, not a per-frame
        # rate (d_a_coming3 collision_main eyePos = centre + 50; step(50) put it 25 units low at N 2)
        mloc = re.match(r'^\s*([A-Za-z_]\w*)\s*(\+=|-=)', s)
        if mloc and not is_member(mloc.group(1)) and re.search(r'\b(?:f32|f64|float|double|s16|s32|u16|u32|int)\s+'
                                                              + re.escape(mloc.group(1)) + r'\b', '\n'.join(self.code[f.body:ln])):
            return False
        # pos += speed (float, speed-like rhs)
        m = re.match(r'^\s*' + LV + r'\s*(\+=|-=)\s*([^;]+?)\s*;\s*$', s)
        if m and SPEEDISH.search(m.group(3)) and not re.search(LIT_INT + r'$', m.group(3).strip()) and \
                re.search(r'(?i)pos|\.x$|\.y$|\.z$|speed|vel|height|ofs|offset|scale|y$', m.group(1)) and not re.search(r'\(.*\)', m.group(3)):
            self.new[ln] = self.raw[ln].replace(m.group(3), 't60c::step(%s)' % m.group(3), 1)
            self.add(ln, f, 'mov', 'auto', '6 (p += v x dt)', 'inline integration (T-02 float tolerance)', self.new[ln])
            self.float_sites = True
            return True
        m = re.match(r'^\s*' + LV + r'\s*(\+=|-=)\s*(' + LIT_FLT + r')\s*;\s*$', s)
        if m:
            self.new[ln] = self.raw[ln].replace(m.group(3), 't60c::step(%s)' % m.group(3), 1)
            self.add(ln, f, 'smth', 'auto', '7 (per-frame float step x dt)', 'constant float add (hand-stepped frame / ramp)', self.new[ln])
            self.float_sites = True
            return True
        return False

    def r_hits(self, f, ln, s, kinds):
        ms = list(HITREAD.finditer(s))
        if not ms:
            return False
        out = self.new[ln]
        for m in ms:
            obj, op = m.group(1), m.group(2)
            key = obj if op == '->' else '&' + obj
            out = out.replace(m.group(0), 't60c::hit(%s, %d, %s)' % (m.group(0), self.next_site(), key), 1)
        self.new[ln] = out
        self.add(ln, f, 'hit', 'auto', '12b (hit read held over the frame)', 'collision hit read', out)
        return True

    def r_isstop(self, f, ln, s, kinds):
        ms = list(ISSTOP.finditer(s))
        if not ms:
            return False
        out = self.new[ln]
        for m in ms:
            obj, op = m.group(1), m.group(2)
            key = obj if op == '->' else '&' + obj
            if self.morf_type(obj):
                out = out.replace(m.group(0), 'en_t60_isStop(%s)' % (obj if op == '->' else '&' + obj), 1)
                self.helpers.add('enemy_anm')
            else:
                out = out.replace(m.group(0), 't60c::held(%s, %d, %s)' % (m.group(0), self.next_site(), key), 1)
        self.new[ln] = out
        self.add(ln, f, 'anm', 'review', '14e (stop test on the first step, held)', 'isStop decision: check the play order (play before / after the test) and animation sets in the frame', out)
        return True

    def morf_type(self, obj):
        name = base_field(obj)
        hdr = os.path.join(ROOT, 'wwhd_src', 'include', os.path.splitext(self.src.split('wwhd_src/')[-1])[0] + '.h')
        texts = '\n'.join(self.raw)
        if os.path.exists(hdr):
            texts += open(hdr).read()
        return re.search(r'(?:mDoExt_McaMorf\s*\*|gptr<\s*mDoExt_McaMorf\s*>)\s*' + re.escape(name) + r'\b', texts) is not None

    EMIT = re.compile(r'(->|\.)(playCreateParticle|stopCreateParticle)\s*\(\s*\)')

    def r_emitter(self, f, ln, s, kinds):
        """an emitter's creation switched on / off by the actor (`mpEmitter->playCreateParticle()` /
        `stopCreateParticle()`, mostly with a flag that records it): frame-level, toggled on the frame's first step,
        so the state the frame's later steps leave is the frame's (mmusic: on in the execute, off again in manage_se;
        the first-step frame-level rule (14f) / 8a). The enclosing if (the flag test) is decided on the first step; a bare statement is gated."""
        if not self.EMIT.search(s) or any(k == 'event' for k, _ in kinds):
            return False
        # the innermost enclosing if header of this line
        k, depth = ln - 1, 0
        hdr = None
        while k > f.body:
            c = self.code[k]
            depth += c.count('}') - c.count('{')
            if depth < 0:
                if re.match(r'^\s*(?:\}\s*)?(?:else\s+)?if\s*\(', c):
                    hdr = k
                break
            k -= 1
        if hdr is not None:
            rng = self._body_range(hdr)
            if rng and not self._motion(rng[0], rng[1]) and self.new[hdr] == self.raw[hdr] and self._else_of(rng) is None:
                raw = self.new[hdr]
                sp = self._if_cond_span(raw)
                if sp:
                    self.new[hdr] = raw[:sp[0]] + 't60c::first() && (' + raw[sp[0]:sp[1]] + ')' + raw[sp[1]:].rstrip() + \
                        ' /* true60 (t60conv): an emitter switched on / off once per frame, on the first step */'
                    self.add(hdr, f, 'ptc', 'auto', '14f / 8a (emitter on / off toggle on the first step)', 'emitter toggle block', self.new[hdr])
                    return True
        if re.match(r'^\s*[\w\[\]>.-]+(->|\.)(playCreateParticle|stopCreateParticle)\s*\(\s*\)\s*;\s*$', s):
            raw = self.raw[ln]
            indent = re.match(r'\s*', raw).group(0)
            self.new[ln] = indent + 'if (t60c::first()) { %s } /* true60 (t60conv): an emitter switched on / off once per frame, on the first step */' % raw.strip()
            self.add(ln, f, 'ptc', 'auto', '14f / 8a (emitter on / off toggle on the first step)', 'emitter toggle', self.new[ln])
            return True
        self.add(ln, f, 'ptc', 'review', '14f / 8a', 'emitter toggle inside an expression or a block with motion / else')
        return True

    LOOKUP = re.compile(r'^\s*(m\w+)\s*=\s*(?:\([^()]*\)\s*)?\w*(?:Search|search|Judge)\w*\s*\(.*\)\s*;\s*$')

    def _opener(self, k, f, depth=0):
        """the line (k or above) holding the '{' that opens the block line k + 1 sits in; braces scanned right to left"""
        while k > f.body:
            for ch in reversed(self.code[k]):
                depth += (ch == '}') - (ch == '{')
                if depth < 0:
                    return k
            k -= 1
        return None

    def _if_header_of_else(self, k, f):
        """the line of the `if (...) {` whose body the `} else {` / `else {` at line k follows (None if not found)"""
        line = self.code[k]
        if line.strip().startswith('}'):
            # the '}' before `else` closes the if body: the opener of the text left of it
            j, d = k, 1
            head = line[:line.index('}')]
            for ch in reversed(head):
                d += (ch == '}') - (ch == '{')
                if d <= 0:
                    return k
            return self._opener(k - 1, f, d - 1)
        j = k - 1
        while j > f.body and not self.code[j].strip():
            j -= 1
        if not self.code[j].rstrip().endswith('}'):
            return None
        return self._opener(j - 1, f, 0) if self.code[j].strip() == '}' else None

    def r_lookup(self, f, ln, s, kinds):
        """a member pointer looked up while it is null (`if (mpX != nullptr) { ...use... } else { mpX = Search(...); }`):
        the original finds it on frame f and uses it from frame f + 1. Looked up on a frame's earlier step, a later step
        of the same frame would already use it (Ygush00: the gush's sound one frame early; found by the T-02 call count).
        The lookup moves to the tick30 step (14b: a value the next frame reads, written at the end of the frame)."""
        m = self.LOOKUP.match(s)
        if not m or any(k == 'event' for k, _ in kinds):
            return False
        var = m.group(1)
        k = self._opener(ln - 1, f)
        if k is None:
            return False
        opener = self.code[k]
        nullv = r'(?:nullptr|NULL|0)'
        ok = False
        mi = re.match(r'^\s*(?:\}\s*else\s+)?if\s*\((.*)\)\s*\{\s*$', opener)
        if mi and re.fullmatch(r'\s*(?:!\s*%s|%s\s*==\s*%s)\s*' % (var, var, nullv), mi.group(1)):
            ok = True  # the body of a null test
        elif re.match(r'^\s*(?:\}\s*)?else\s*\{\s*$', opener):
            j = self._if_header_of_else(k, f)
            mh = re.match(r'^\s*(?:\}\s*else\s+)?if\s*\((.*)\)\s*\{\s*$', self.code[j]) if j is not None else None
            if mh and re.fullmatch(r'\s*(?:%s|%s\s*!=\s*%s)\s*' % (var, var, nullv), mh.group(1)):
                ok = True
        if not ok:
            return False
        raw = self.raw[ln]
        indent = re.match(r'\s*', raw).group(0)
        self.new[ln] = indent + 'if (t60c::end()) { %s } /* true60 (t60conv): a cached lookup stored on the tick30 step (read from the next frame, as the original) */' % raw.strip()
        self.add(ln, f, 'look', 'auto', '14b (cached lookup on the tick30 step)', 'lookup of %s while it is null' % var, self.new[ln])
        return True

    def init_branches(self, exe_lines):
        """an init branch: the branch an if takes while a member is not yet set, which sets it (`if (mpEmitter ==
        nullptr) { ... mpEmitter = create(...); }`, or the else of `if (mpX != nullptr) { use } else { mpX = find(); }`).
        The original runs it once on frame f and the other branch from frame f + 1. At N > 1 the member set on an
        earlier step would let the frame's later steps already run the other branch (mmusic: the emitter created on
        step 1, stopped again by the cull branch on step 2; Ygush00: the gush sound one frame early). The whole init
        branch runs on the tick30 step instead (14b: what the next frame reads is written at the end of the frame);
        rules inside it are skipped (the branch is once per frame as a whole). Returns the lines it took."""
        done = set()
        nullv = r'(?:nullptr|NULL|0)'
        lines_of = {}
        for f, ln, kinds in exe_lines:
            lines_of.setdefault(f.name, (f, []))[1].append((ln, kinds))
        for fname, (f, lns) in lines_of.items():
            for ln, kinds in lns:
                if ln in done or any(k == 'event' for k, _ in kinds):
                    continue
                m = re.match(r'^\s*(?:\}\s*else\s+)?if\s*\((.*)\)\s*\{\s*$', self.code[ln])
                if not m:
                    continue
                cond = m.group(1).strip()
                mn = re.fullmatch(r'!\s*(m\w+)|(m\w+)\s*==\s*' + nullv, cond)
                ms = re.fullmatch(r'(m\w+)|(m\w+)\s*!=\s*' + nullv, cond)
                rng = self._body_range(ln)
                if not rng:
                    continue
                if mn:
                    var, hdr, body = mn.group(1) or mn.group(2), ln, rng
                elif ms:
                    var = ms.group(1) or ms.group(2)
                    e = self._else_of(rng)
                    if e is None or not self.code[e].rstrip().endswith('{') or re.search(r'\belse\s+if\b', self.code[e]):
                        continue
                    hdr, body = e, self._body_range(e)
                    if not body:
                        continue
                else:
                    continue
                lo, hi, close = body
                # pointers only (a cached object / emitter / found actor): a flag compared with 0 / 1 is a toggle, which
                # the frame's later steps must see at once (mmusic mEmitting)
                if not (var.startswith('mp') or re.search(r'nullptr|NULL', cond)):
                    continue
                if not any(re.search(r'\b%s\s*=[^=]' % re.escape(var), self.code[k]) for k in range(lo, hi + 1)):
                    continue
                if self._motion(lo, hi) or any(k in done for k in range(hdr, close + 1)):
                    continue
                if not self.raw[hdr].rstrip().endswith('{') or not self.raw[close].lstrip().startswith('}') or \
                        self.new[hdr] != self.raw[hdr] or self.new[close] != self.raw[close]:
                    continue
                rh = self.raw[hdr].rstrip()
                self.new[hdr] = rh + ' if (t60c::end()) { /* true60 (t60conv): init branch (sets %s) once per frame, on the tick30 step */' % var
                rc = self.raw[close]
                i = rc.index('}')
                self.new[close] = rc[:i] + '} ' + rc[i:]
                self.add(hdr, f, 'init', 'auto', '14b (init branch on the tick30 step)', 'branch taken while %s is unset sets it' % var, self.new[hdr])
                done |= set(range(hdr, close + 1))
        return done

    GCOUNT = re.compile(r'\b\w*(?:[Tt]imer|[Cc]ounter|[Cc]ount|[Ff]rame)\w*\s*\(')

    def r_globalcount(self, f, ln, s, kinds):
        """a periodic test on a frame counter the actor does not own (`if (dKy_get_schbit_timer() % k == 0)`, through a
        local): the counter is stepped once per 30 Hz frame elsewhere (KANKYO on tick30, g_Counter), so it reads the same
        value on every step of the frame and the test fires on all N steps (fallrock_tag: a rock and its sound per step;
        found by the T-02 call count). The test is decided on the first step (14d)."""
        m = re.match(r'^\s*(?:\}\s*else\s+)?if\s*\((.*)\)\s*\{?\s*$', s)
        if not m or '%' not in m.group(1) or any(k == 'event' for k, _ in kinds):
            return False
        cond = m.group(1)
        ok = bool(self.GCOUNT.search(cond.split('%')[0]))
        if not ok:
            mv = re.match(r'^\s*\(?\s*([a-z_]\w*)\s*%', cond)
            if mv:
                var = mv.group(1)
                for k in range(f.body, ln):
                    if re.search(r'\b%s\s*=\s*[^=;]*' % var, self.code[k]) and self.GCOUNT.search(self.code[k]):
                        ok = True
                        break
        if not ok or self.new[ln] != self.raw[ln]:
            return False
        raw = self.raw[ln]
        sp = self._if_cond_span(raw)
        if not sp:
            return False
        self.new[ln] = raw[:sp[0]] + 't60c::first() && (' + raw[sp[0]:sp[1]] + ')' + raw[sp[1]:].rstrip() + \
            ' /* true60 (t60conv): a periodic test on a 30 Hz counter, decided on the first step */'
        self.add(ln, f, 'gcnt', 'auto', '14d (periodic test on a 30 Hz counter: first step)', 'modulo test on %s' % cond.strip()[:60], self.new[ln])
        return True

    def r_sound(self, f, ln, s, kinds):
        m = SND.search(s) or PTC.search(s) or any(re.search(r'\b' + w + r'\s*\(', s) for w in self.sound_wrap)
        if not m:
            return False
        if any(k == 'event' for k, _ in kinds):
            self.add(ln, f, 'snd', 'auto', '11 (one-shot start in an event function, unchanged)', 'per-event start')
            return True
        is_ptc = PTC.search(s) is not None
        cat = 'ptc' if is_ptc else 'snd'
        level = re.search(r'Level|seStartHD', s) is not None
        statement = re.match(r'^\s*[\w:]+\s*\(.*\)\s*;\s*$', s) is not None
        if not statement:
            # an emitter set behind a null-guard return (`if (mpE[0] || mpE[1]) return; ... mpE[0] = particle_set(..)`,
            # demo_item setParticle; finding (c)): a failed set (null) is retried on every step; the original
            # retries once per frame. The function returns on the frame's later steps after its guards (8a).
            mv = re.match(r'^\s*(m\w+)(?:\[[^\]]*\])?\s*=\s*[^=]', s)
            g = self._retry_guard(f, ln, mv.group(1)) if is_ptc and mv else None
            if g is not None:
                if f.name not in self.retry_gated and self.new[g] == self.raw[g]:
                    self.new[g] = self.raw[g].rstrip() + ' if (!t60c::first()) return; /* true60 (t60conv): the emitter set is tried once per frame, on the first step (8a: a failed set retries next frame) */'
                    self.retry_gated.add(f.name)
                    self.add(g, f, cat, 'auto', '8a (emitter set behind a null guard: first step)', 'null-guard return on %s' % mv.group(1), self.new[g])
                if f.name in self.retry_gated:
                    self.add(ln, f, cat, 'auto', '8a (emitter set on the first step; a failed set retried next frame)', 'emitter set behind the null-guard return')
                    return True
            # an emitter created into a pointer under a condition (`mpEmitter = ... particle_set(...)` behind a null / flag
            # test): created once per event, unchanged (the sound-gate rule (8a): emitter created once per event)
            if is_ptc and conditional(kinds) and re.match(r'^\s*(?:[\w:<>]+\s*\*\s*)?[\w.\->\[\]]+\s*=\s*[^=]', s):
                # the creation is frame-level (14f): its decision (the enclosing if and a plain else) on the first step,
                # so a flag a step set earlier in the frame starts the emitter on the next frame, as the original
                if self._gate_enclosing_if(ln, f, 'an emitter created once, decided on the first step'):
                    self.add(ln, f, cat, 'auto', '8a / 14f (emitter creation decided on the first step)', 'emitter creation behind a condition')
                    return True
                self.add(ln, f, cat, 'auto', '8a (emitter created once per event, unchanged)', 'emitter creation behind a condition')
                return True
            self.add(ln, f, cat, 'review', '8a / 11', 'start inside an expression')
            return True
        latch = self._latch_block(ln, f) if conditional(kinds) else None
        if latch:
            # `if (!mbPlayedSe) { se; mbPlayedSe = 1; }` (ib; finding (a)): the block runs once (its own flag);
            # a tick30 gate would lose the start (the flag is set on the step that first sees the block)
            self.add(ln, f, cat, 'auto', '11 (one-shot start behind a latch flag the block sets, unchanged)', 'latch %s' % latch)
            return True
        if (level or not conditional(kinds)) or (conditional(kinds) and not self._in_change_block(ln, f) and not self._anm_cond(kinds)):
            raw = self.raw[ln]
            indent = re.match(r'\s*', raw).group(0)
            self.new[ln] = indent + 'if (t60c::end()) %s /* true60 (t60conv): a per-frame start, once per 30 Hz frame */' % raw.strip()
            self.add(ln, f, cat, 'auto', '11 / 8a (per-frame start on the tick30 step)', 'continuous per-frame start', self.new[ln])
            return True
        self.add(ln, f, cat, 'auto', '11 (one-shot start, unchanged)', 'start inside a state change / crossing block')
        return True

    def _latch_block(self, ln, f):
        """the flag of a one-shot latch block the line sits in directly: `if (!X) {` / `if (X == 0) {` whose body sets X
        to a non-zero constant (None otherwise)"""
        k = self._opener(ln - 1, f)
        if k is None:
            return None
        m = re.match(r'^\s*(?:\}\s*else\s+)?if\s*\((.*)\)\s*\{\s*$', self.code[k])
        if not m:
            return None
        mv = re.fullmatch(r'\s*(?:!\s*([\w.\->\[\]]+)|([\w.\->\[\]]+)\s*==\s*(?:0|false|FALSE))\s*', m.group(1))
        if not mv:
            return None
        var = mv.group(1) or mv.group(2)
        rng = self._body_range(k)
        if not rng:
            return None
        lo, hi, _ = rng
        if any(re.search(r'(?<![\w.>])%s\s*=\s*(?:true|TRUE|[1-9]\w*)\s*;' % re.escape(var), self.code[j]) for j in range(lo, hi + 1)):
            return var
        return None

    def _retry_guard(self, f, ln, var):
        """the last top-level null-guard return before line ln that tests the member var (`if (... var ...) return;` or
        `if (...) {` + `return;` + `}`): the line to put the first-step return after (None if there is none)"""
        d, found = 0, None
        k = f.body
        while k < ln:
            c = self.code[k]
            if d == 1 and re.search(r'\b%s\b' % re.escape(var), c):
                m = re.match(r'^\s*if\s*\((.*)\)\s*(\{)?\s*(return\s*;)?\s*$', c)
                if m and m.group(3) and not m.group(2):
                    found = k
                elif m and m.group(2) and k + 2 < ln and re.fullmatch(r'\s*return\s*;\s*', self.code[k + 1]) and \
                        self.code[k + 2].strip() == '}':
                    found = k + 2
            d += c.count('{') - c.count('}')
            k += 1
        return found

    LOOPFLAG = re.compile(r'^\s*(?:bool|BOOL|int|s32|u8|u32)\s+(\w+)\s*=\s*[^;]*\bcheckState\s*\(\s*J3DFrameCtrl::STATE_LOOP_E\s*\)\s*;\s*$')

    def r_loopmark(self, f, ln, s, kinds):
        """a loop flag read into a local after a per-step play (`bool looped = m.checkState(STATE_LOOP_E);`) and used only
        by tests that start a sound (ib animControl, finding (b)): the tick30 sound gate saw only the last step's
        flag, so a loop on an earlier step was lost. The flag is gathered over the frame's steps (t60c::mark) and the
        test runs once on the tick30 step with the loop of any step counted; a crossing window `frame - rate` in the same
        test is the frame-end window there (rate = the 30 Hz rate)."""
        m = self.LOOPFLAG.match(self.code[ln])
        if not m or '::' not in f.name or re.search(r'\bstatic\b', self.code[f.body]) or self.new[ln] != self.raw[ln]:
            return False
        L = m.group(1)
        uses = []
        for j in range(ln + 1, f.end + 1):
            if not re.search(r'\b%s\b' % L, self.code[j]):
                continue
            sp = self._if_cond_span(self.raw[j])
            if not sp or not re.search(r'\b%s\b' % L, self.raw[j][sp[0]:sp[1]]) or self.new[j] != self.raw[j]:
                return False
            rng = self._body_range(j)
            if not rng:
                return False
            lo, hi, _ = rng
            body = [self.code[k] for k in range(lo, hi + 1) if self.code[k].strip() and self.code[k].strip() not in ('{', '}')]
            if not body or not all(re.match(r'^\s*[\w:]+\s*\(.*\)\s*;\s*$', b) and (SND.search(b) or any(
                    re.search(r'\b' + w + r'\s*\(', b) for w in self.sound_wrap)) for b in body):
                return False
            uses.append((j, lo, hi))
        if not uses:
            return False
        sid = self.next_site()
        self.new[ln] = self.raw[ln].rstrip() + ' if (t60c::first()) t60c::unmark(%d, this); if (%s) t60c::mark(%d, this); ' \
            '/* true60 (t60conv): the frame\'s loop state, gathered over its steps */' % (sid, L, sid)
        self.add(ln, f, 'anm', 'auto', '11 / 14e (loop flag gathered over the frame: t60c::mark)', 'loop flag %s after the play' % L, self.new[ln])
        for j, lo, hi in uses:
            raw = self.raw[j]
            sp = self._if_cond_span(raw)
            self.new[j] = raw[:sp[0]] + 't60c::end() && (' + raw[sp[0]:sp[1]] + ' || t60c::marked(%d, this))' % sid + raw[sp[1]:].rstrip() + \
                ' /* true60 (t60conv): the loop / crossing test once per frame on the tick30 step, a loop on any step counted */'
            self.add(j, f, 'anm', 'auto', '11 / 14e (loop / crossing test on the tick30 step)', 'test of %s' % L, self.new[j])
            self.skip.add(j)
            for k in range(lo, hi + 1):
                if k != j and self.code[k].strip() not in ('', '{', '}'):
                    self.skip.add(k)
                    self.add(k, f, 'snd', 'auto', '11 (start under the tick30 loop / crossing test, unchanged)', 'start')
        return True

    def _anm_cond(self, kinds):
        return self._timer_cond(kinds) or any(re.search(r'\b(' + '|'.join(map(re.escape, self.timers or {'@@'})) + r')\b', c) for _, c in kinds)

    def call_contexts(self):
        """per execute-path function: 'step' (reached unconditionally from a root: runs every step), 'event' (every call
        site sits in a state-change / timer block: per-event work) or 'cond' (other conditional calls)"""
        ctxs = {f.name: block_context(self.code, f) for f in self.exe}
        mode = {f.name: 'step' for f in self.roots}
        sound_wrap = set(self.ext_sound)  # glue names of sound starts (sound_ext_addrs)
        for f in self.exe:
            if f not in self.roots and any(SND.search(self.code[k]) or PTC.search(self.code[k]) for k in range(f.body, f.end + 1)) \
                    and f.end - f.body < 12:
                sound_wrap.add(f.short)
        for _ in range(6):
            for f in self.exe:
                if f.name not in mode:
                    continue
                for ln in range(f.body, f.end + 1):
                    refs = IDENT.findall(self.code[ln]) + re.findall(r'&\s*(?:\w+::)*(\w+)\b(?!\s*[\(\[.-])', self.code[ln])
                    for c in refs:
                        for g in self.exe:
                            if g.short != c or g is f:
                                continue
                            kinds = ctxs[f.name].get(ln, [])
                            if mode[f.name] == 'step' and not conditional(kinds):
                                m = 'step'
                            elif mode[f.name] == 'event' or self._in_change_block(ln, f) or self._timer_cond(kinds):
                                m = 'event'
                            else:
                                m = 'cond'
                            rank = {'step': 3, 'cond': 2, 'event': 1}
                            if g.name not in mode or rank[m] > rank[mode[g.name]]:
                                mode[g.name] = m
        self.fmode = mode
        self.sound_wrap = sound_wrap
        return ctxs

    def _timer_cond(self, kinds):
        return any(re.search(r'(?i)timer|cnt|count|wait|isStop|checkPass|checkFrame|checkState', c) for _, c in kinds)

    STEPWORK = re.compile(r'\bcLib_(addCalc|chase)\w*|\bfopAcM_(posMove|calcSpeed)\w*|(->|\.)(play|playMorf)\s*\(|_play\b|'
                          r'\b(speed|speedF|gravity)\b|\bpos\.[xyz]\s*[-+*]?=|current\.pos\s*=|\bmDoMtx_quatSlerp|'
                          r'(->|\.)Move\s*\(|\bCrrPos\b|\bcM_ssin|\bcM_scos|[-+*]=\s*[\d.]+f\b|\bf(n)?madds')
    TIMERSITE = re.compile(r'(?i)(m\w*(timer|cnt|count|wait|delay|time)\w*)(\[[^\]]*\])?\s*(--|\+\+|[-+]=)|'
                           r'=\s*\(?[^;]*(timer|cnt|count|wait|delay|time)\w*(\[[^\]]*\])?\)?\s*[-+]\s*1u?\b|cLib_calcTimer|'
                           r'\bcM_rnd|seStart|\bm\w*(Mode|Proc|State|Action|Step)\w*\s*=[^=]')

    def step_work(self, f, seen=None):
        seen = seen or set()
        if f.name in seen:
            return False
        seen.add(f.name)
        for ln in range(f.body, f.end + 1):
            c = self.code[ln]
            if self.STEPWORK.search(c) and not self.timer_formula(c):
                return True
            # an ext glue name of a step-work primitive (2026-11-18: dai_item / demo_item / ib PASS (auto) misses)
            if self.ext_step and any(re.search(r'\b' + re.escape(x) + r'\s*\(', c) for x in self.ext_step):
                return True
            # an indirect call the tool did not resolve: its callees are unknown, so no proof of no motion
            if DISPATCH.search(c) and not getattr(f, 'dispatch', {}).get(ln) and not self.helper_resolved(f):
                return True
        for g in self.exe:
            if g.short in f.calls and g is not f and self.fmode.get(g.name) == 'step' and self.step_work(g, seen):
                return True
        return False

    def reaches(self, f, pred, seen=None):
        """pred holds on a code line of f or of a same-unit callee on the execute path"""
        seen = seen or set()
        if f.name in seen:
            return False
        seen.add(f.name)
        if any(pred(self.code[k]) for k in range(f.body, f.end + 1)):
            return True
        return any(self.reaches(g, pred, seen) for g in self.exe if g.short in f.calls and g is not f)

    TIMERSET = re.compile(r'(?i)\bm\w*(timer|cnt|count|wait|delay)\w*(\[[^\]]*\])?\s*=[^=]')

    def timer_formula(self, line):
        """a STEPWORK hit that is no motion: a fused multiply-add in a timer reload or a random draw (mmusic
        `mTimer = ftoi(fmadds(60, cM_rndF(1), 120))`)"""
        m = self.STEPWORK.search(line)
        return bool(m and re.match(r'\bf(n)?madds', m.group(0)) and (self.TIMERSET.search(line) or 'cM_rnd' in line))

    EVONLY = re.compile(r'^\s*if\s*\(\s*(?:dComIfGp_event_runCheck\(\)\s*==\s*(?:FALSE|false|0)|!\s*dComIfGp_event_runCheck\(\))\s*\)\s*\{?\s*(return\b[^;]*;)?')

    def event_only(self):
        """step functions whose first statement returns unless an event runs: they run only while the world is held at
        N = 1 (true60.h: the 30 Hz fallback during events), so no conversion and no T-02"""
        ev = set()
        for f in self.exe:
            if self.fmode.get(f.name) != 'step':
                continue
            for k in range(f.body + 1, min(f.end, f.body + 8)):
                c = self.code[k]
                if not c.strip() or c.strip() == '{' or re.match(r'^\s*[\w:<>*& ]+\s+\w+\s*;\s*$', c):
                    continue
                m = self.EVONLY.match(c)
                if m and (m.group(1) or re.match(r'^\s*return\b', self.code[k + 1])):
                    ev.add(f.name)
                    self.add(k, f, 'evt', 'auto', '9 (runs only while an event holds the world at N = 1)', 'event-only function: unchanged, no T-02')
                break
        self.evonly = ev
        for f in self.exe:
            if f.name in ev:
                self.fmode[f.name] = 'event'

    def logic_gate(self):
        """functions that run every step but hold only frame-level logic (timers, decisions, draws, sounds, state, no
        motion): their whole body on the frame's first step (the first-step AI-timer rule (14d) / 14f)"""
        gated = []
        for f in self.exe:
            if self.fmode.get(f.name) not in ('step', 'cond') or f.short in self.sound_wrap:
                continue
            if any(g.name in [x.name for x in gated] for g in self.exe if g.short == f.inlined_into):
                continue
            if self.step_work(f):
                continue
            if not any(self.TIMERSITE.search(self.code[k]) for k in range(f.body, f.end + 1)):
                continue
            head = ' '.join(self.code[f.start:f.body + 1])
            rets = set(re.findall(r'\breturn\b\s*([^;]*);', '\n'.join(self.code[f.body:f.end + 1])))
            if re.match(r'^\s*(static\s+)?(inline\s+)?void\b', head):
                ret = ''
            elif len(rets) == 1 and re.match(r'^(-?\d+|true|false|TRUE|FALSE)$', next(iter(rets)).strip()):
                ret = ' ' + next(iter(rets)).strip()
            else:
                continue
            gated.append(f)
            b = f.body
            if self.has_at and self.reaches(f, lambda c: CCSET.search(c)):
                # an AT registration behind the gate (hitobj, 2026-11-18): the attack is registered for the
                # frame's collision, which runs on the tick30 step (the one-sweep-per-frame attack rule (12c)): the whole body there
                self.new[b] = self.new[b].rstrip() + ' if (!t60c::end()) return%s; /* true60 (t60conv): frame-level logic with an AT registration on the tick30 step (rule note 12c) */' % ret
                self.add(b, f, 'gate', 'auto', '12c (frame-level logic with an AT collider registration: tick30 step)', 'whole body gated', self.new[b])
                continue
            if self.reaches(f, lambda c: WORLDSTATE.search(c)):
                # the shared-world-state rule (14s): switches / event bits / save flags (read or written): the tick30 step, A's line order
                self.new[b] = self.new[b].rstrip() + ' if (!t60c::end()) return%s; /* true60 (t60conv): frame-level logic on shared world state on the tick30 step (rule note 14s) */' % ret
                self.add(b, f, 'gate', 'auto', '14s (frame-level logic on switches / event bits / save flags: tick30 step at the line place)', 'whole body gated', self.new[b])
                continue
            self.new[b] = self.new[b].rstrip() + ' if (!t60c::first()) return%s; /* true60 (t60conv): frame-level logic on the frame\'s first step (rule notes 14d / 14f) */' % ret
            self.add(b, f, 'gate', 'auto', '14d / 14f (frame-level logic on the first step; no per-step motion in the function or its callees)', 'whole body gated', self.new[b])
        self.gated = gated
        # callees reached only from gated functions are covered by the gate
        covered = set(x.name for x in gated) | set(self.evonly)
        for _ in range(6):
            for g in self.exe:
                if g.name in covered or g in self.roots:
                    continue
                callers = [f for f in self.exe if g.short in f.calls and f is not g]
                if callers and all(c.name in covered for c in callers):
                    covered.add(g.name)
        self.covered = covered

    def run(self):
        self.timers = set()
        self.first_timers = set()
        self.timer_locals = {}
        self.float_sites = False
        exe_lines = []
        self.skip = set()  # lines a rule has already placed (the sound under a tick30 crossing test, (b))
        self.retry_gated = set()  # functions whose emitter set got the first-step guard ((c))
        ctxs = self.call_contexts()
        self.event_only()
        self.logic_gate()
        for f in self.exe:
            if f.short in self.sound_wrap or f.name in self.covered or f.name in self.evonly:
                continue
            ctx = ctxs[f.name]
            for ln in range(f.body, f.end + 1):
                kinds = ctx.get(ln, [])
                if self.fmode.get(f.name) == 'event':
                    kinds = kinds + [('event', 'called from a state-change block')]
                elif self.fmode.get(f.name) == 'cond':
                    kinds = kinds + [('if', 'called conditionally')]
                exe_lines.append((f, ln, kinds))
        # pass 0: init branches (whole branch on the tick30 step; nothing inside is rewritten again)
        done = self.init_branches(exe_lines)
        # pass 1: timers (so the one-shot rule knows them)
        for f, ln, kinds in exe_lines:
            if ln in done:
                continue
            if self.r_timer(f, ln, self.code[ln], kinds):
                done.add(ln)
        for f, ln, kinds in exe_lines:
            if ln in done or ln in self.skip:
                continue
            s = self.code[ln]
            if not s.strip():
                continue
            # judgement / not convertible first
            if INP.search(s):
                self.add(ln, f, 'inp', 'manual', '12a', 'input read'); continue
            if EVT_ORDER.search(s):
                self.add(ln, f, 'evt', 'review', '14f', 'event order from a converted actor'); continue
            if MSG.search(s):
                self.add(ln, f, 'msg', 'review', '10', 'message code (N = 1 fallback while a message runs)'); continue
            if EVT.search(s):
                self.add(ln, f, 'evt', 'auto', '9 (event-time code: the world runs at N = 1 during events)', 'event query, unchanged'); continue
            if CCSET.search(s):
                if self.has_at:
                    self.add(ln, f, 'cc', 'review', '12c', 'collider registration in a unit with an AT collider')
                else:
                    self.add(ln, f, 'cc', 'auto', '6l (f) (TG / CO registered per step)', 'unchanged')
                continue
            if BGMOVE.search(s):
                self.add(ln, f, 'bg', 'review', 'M2-COL-B01', 'background Move / ride callback: per step is right for a BG that stays put; a carrier that moves riders couples with M2-COL-B01'); continue
            if self.r_procdispatch(f, ln, s, kinds):
                continue
            if DISPATCH.search(s) and not getattr(f, 'dispatch', {}).get(ln) and self.helper_resolved(f):
                self.add(ln, f, 'misc', 'auto', 'guest table resolved', 'dispatch helper: every call site names its guest table (targets analysed there)'); continue
            if getattr(f, 'dispatch', {}).get(ln):
                self.add(ln, f, 'misc', 'auto', 'guest table resolved', 'indirect dispatch through a guest table: targets ' + ', '.join(sorted(f.dispatch[ln])) + ' analysed as callees'); continue
            if re.search(r'\bdispatch\s*\(|\bcall_ptr\b|\btail_ptr\b|(->|\.)\*\s*\w', s) and \
                    not any(re.search(r'&\s*\w+::\w+', self.code[k]) for k in range(f.body, f.end + 1)):
                self.add(ln, f, 'misc', 'review', '?', 'indirect dispatch through a guest table: the functions behind it are not analysed (convert them by hand or name them in the source)'); continue
            if CHECKPASS.search(s) and not SND.search(s) and self.r_loopmark(f, ln, s, kinds):
                continue
            if CHECKPASS.search(s) and not SND.search(s):
                self.add(ln, f, 'anm', 'review', '5 / 14e', 'checkPass / checkFrame / checkState: lookahead or per-step crossing, decide by hand'); continue
            handled = False
            for rule in (self.r_globalcount, self.r_lookup, self.r_edgeclear, self.r_timerdecide, self.r_oneshot, self.r_hits, self.r_isstop, self.r_rnd, self.r_emitter, self.r_sound, self.r_phase, self.r_float):
                if rule(f, ln, s, kinds):
                    handled = True
                    break
            if handled:
                continue
            if CENTRAL.search(s):
                self.add(ln, f, 'calc', 'auto', 'central (M1)', 'primitive converted centrally'); continue
            if re.search(r'\bg_Counter\b|mCounter0', s):
                self.add(ln, f, 'gcnt', 'review', '13', 'g_Counter phase'); continue
            if PATH.search(s) and re.search(r'\+\+|\+=|--', s):
                self.add(ln, f, 'mov', 'manual', '6 (path step)', 'path / spline step'); continue
            # anything the board scan would count that no rule took
            if re.search(r'(\+\+|--|[-+*]=)', s) and not re.search(r'\bfor\s*\(', s) and re.search(r'\bm[A-Z]\w*|->|\.', s):
                lv = re.match(r'^\s*' + LV, s)
                name = base_field(lv.group(1)) if lv else ''
                if name and is_member(lv.group(1)) and re.match(r'^\s*' + LV + r'\s*(\+\+|--|[-+*]=)', s):
                    self.add(ln, f, 'misc', 'review', '?', 'per-frame update of a member that no rule covers')
        # the actor's draw path (t60_draw.h): draw-side writes of 30 Hz state
        draw_pass(self)

    # ------------------------------------------------------------------ outputs
    def classify_functions(self):
        out = {}
        for f in self.exe:
            ss = [s for s in self.sites if s.func == f.name]
            cls = 'unchanged'
            if any(s.cls == 'manual' for s in ss):
                cls = 'not convertible'
            elif any(s.cls == 'review' for s in ss):
                cls = 'needs review'
            elif any(s.after for s in ss):
                cls = 'auto-converted'
            elif ss:
                cls = 'auto (no change needed)'
            out[f.name] = dict(addr=address_of(f, self.funcs), cls=cls, sites=len(ss),
                               reasons=sorted({s.reason for s in ss if s.cls != 'auto'}))
        return out


# ------------------------------------------------------------------ draw path (t60_draw.h)
# At N > 1 the game draws after every step. Draw code that WRITES state is a 30 Hz frame step: a per-draw counter or
# smoothing step ran N times as fast, an answer the execute reads was taken half a frame early. The port puts actor
# memory back after passes without logic only for UNCONVERTED actors, so every conversion checks its own draw path.
# Auto form: the write on the draw after the frame's tick30 step only (t60_draw_frame_pass); answers read by the
# execute are listed for review (form: a local copy for this pass's draw, the member written on the frame pass only).
DRAW_ROOT = re.compile(r'(?i)draw')
DRAW_NOT = re.compile(r'(?i)create|heap|ctor|dtor|delete|init')
# an lvalue that may start at an accessor call (mainFields(i_this)->mFlashColor)
LVX = r'((?:this->)?(?:[A-Za-z_][\w:]*\([^()]*\)|[A-Za-z_]\w*)(?:\[[^\]]+\])?(?:(?:->|\.)[A-Za-z_]\w*(?:\[[^\]]+\])?)*)'
DRAW_SMOOTH = re.compile(r'^\s*(?:\(void\)\s*)?(cLib_(?:addCalc|chase)\w*)\s*\(\s*&\s*' + LVX + r'\s*,.*\)\s*;\s*$')
DRAW_SMOOTH_ANY = re.compile(r'\bcLib_(?:addCalc|chase)\w*\s*\(')
DRAW_INC = re.compile(r'^\s*(?:' + LVX + r'\s*(\+\+|--)|(\+\+|--)\s*' + LVX + r')\s*;\s*$')
DRAW_CMP = re.compile(r'^\s*' + LVX + r'\s*([-+*/|&^]|<<|>>)=(?!=)\s*(.+?)\s*;\s*$')
DRAW_SET = re.compile(r'^\s*' + LVX + r'\s*=(?!=)\s*(.+?)\s*;\s*$')
DRAW_PORT_KEPT = re.compile(r'\btevStr\b|\bmTevStr\b')  # the port keeps tevstrs once per step (interp_tevstr_keep)
DRAW_VALUE_LOCAL = re.compile(r'^\s*(?:const\s+)?(?:gabi::Local<[^;]*?>|[A-Za-z_][\w:]*(?:<[^;*&]*?>)?)\s+([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*(?:[;=({]|$)')
DRAW_PTR_LOCAL = re.compile(r'^\s*(?:const\s+)?[A-Za-z_][\w:<>,\s]*?[*&]\s*(?:const\s+)?([A-Za-z_]\w*)\s*=')
DRAW_SKIP_FIELD = re.compile(r'(?i)mtx|matrix|^(x|y|z|r|g|b|a|i|j|k)$')
DRAW_STEP_KINDS = ('SMOOTH', 'COUNTER', 'STEP')
# a flag constant (literal, ALL_CAPS, Scope::Name, kName), optionally inverted: `x |= C` / `x &= ~C` are idempotent
DRAW_FLAGC = r'^~?\s*\(?\s*(?:-?(?:0x[0-9A-Fa-f]+|\d+)[uU]?|[A-Z_][A-Z0-9_]*|[\w:]+::\w+|k[A-Z]\w*)\s*\)?$'


def draw_functions(conv):
    """the actor's draw path: draw-named roots and their same-file callees, without functions the execute path shares"""
    exe_ids = {id(f) for f in conv.exe}
    by_short = {}
    for f in conv.funcs:
        by_short.setdefault(f.short, []).append(f)
    roots = [f for f in conv.funcs if DRAW_ROOT.search(f.short) and not DRAW_NOT.search(f.short) and id(f) not in exe_ids]
    seen, todo, out = set(), list(roots), []
    while todo:
        f = todo.pop()
        if id(f) in seen:
            continue
        seen.add(id(f))
        out.append(f)
        for c in f.calls:
            for g in by_short.get(c, []):
                if EXEC_NAME.search(g.short) or DRAW_NOT.search(g.short) or id(g) in exe_ids:
                    continue
                todo.append(g)
    return sorted(out, key=lambda f: f.body)


def draw_pass(conv):
    """scan the draw path; the step forms (smoothing, counters, accumulators, self-referencing updates of members and
    globals) get `if (t60_draw_frame_pass())`, answers the execute reads are listed for review. Returns the draw functions
    that hold sites."""
    dfuncs = draw_functions(conv)
    dids = {id(f) for f in dfuncs}
    exe_text = []
    for f in conv.exe:
        if id(f) in dids:
            continue
        for k in range(f.body, f.end + 1):
            l = conv.code[k]
            m = re.match(r'^\s*([A-Za-z_][\w.\->\[\]]*)\s*=(?!=)(.*)$', l)
            exe_text.append(m.group(2) if m else l)
    exe_blob = '\n'.join(exe_text)
    resets = set()
    for f in dfuncs:
        depth = 0
        for k in range(f.body, f.end + 1):
            c = conv.code[k]
            m = re.match(r'^\s*' + LVX + r'\s*=\s*(?:\(\w+\)\s*)?-?' + LIT_INT + r'\s*;', c)
            if m and depth == 1:
                resets.add(m.group(1))
            depth += c.count('{') - c.count('}')
    # a draw function whose every call in the draw path sits behind a frame-pass gate (`if (t60_draw_frame_pass()) f();`)
    callers_gated = {}
    for g in dfuncs:
        for k in range(g.body + 1, g.end + 1):
            for h in dfuncs:
                if h is g or not re.search(r'\b' + re.escape(h.short) + r'\s*\(', conv.code[k]):
                    continue
                ok = 't60_draw_frame_pass' in conv.code[k] or \
                    (k > 0 and 't60_draw_frame_pass' in conv.code[k - 1] and not conv.code[k - 1].rstrip().endswith(';'))
                callers_gated.setdefault(id(h), []).append(ok)
    hit = []
    for f in dfuncs:
        if callers_gated.get(id(f)) and all(callers_gated[id(f)]):
            continue  # gated at its calls (a hand-made t60_draw.h form)
        values, ptrs, tevptrs = set(), set(), set()
        for k in range(f.body, f.end + 1):
            c = conv.code[k]
            m = DRAW_PTR_LOCAL.match(c) or re.match(r'^\s*auto\s*\*\s*([A-Za-z_]\w*)\s*=', c)
            if m:
                ptrs.add(m.group(1))
                if re.search(r'(?i)tevstr', c):
                    tevptrs.add(m.group(1))
                continue
            m = DRAW_VALUE_LOCAL.match(c)
            if m and m.group(1) not in ('return', 'else', 'delete', 'new', 'case', 'goto'):
                values.add(m.group(1))
        # a local loaded from a member (`s32 t = this->mX;`): `this->mX = t - 1` is that member's counter
        carry = {}
        for k in range(f.body, f.end + 1):
            m = re.match(r'^\s*(?:const\s+)?[\w:<>]+\s+([A-Za-z_]\w*)\s*=\s*(?:\(\w+\)\s*)?' + LVX + r'\s*;', conv.code[k])
            if m:
                carry[m.group(1)] = m.group(2)
        localptrs = set(ptrs)
        gates = {'t60_draw_frame_pass'} | {m.group(1) for k in range(f.body, f.end + 1)
                                          for m in [re.search(r'\b([A-Za-z_]\w*)\s*=\s*t60_draw_frame_pass\s*\(', conv.code[k])] if m}
        gate_re = re.compile(r'\b(?:' + '|'.join(sorted(gates)) + r')\b')

        def gated(ln):
            # the line already sits under a frame-pass gate (a hand-made t60_draw.h form)
            j = ln - 1
            while j > f.body and not conv.code[j].strip():
                j -= 1
            if gate_re.search(conv.code[j]) and not conv.code[j].rstrip().endswith(';'):
                return True
            depth = 0
            for k in range(ln - 1, f.body, -1):
                c = conv.code[k]
                depth += c.count('}') - c.count('{')
                if depth < 0:
                    hdr = c + (' ' + conv.code[k - 1] if c.strip() == '{' else '')
                    if gate_re.search(hdr):
                        return True
                    depth = 0
            return False
        # parameters: `T* p` / `T& p` alias, `T p` is a value
        head = ' '.join(conv.code[f.start:f.body + 1])
        pm = re.search(r'\(([^()]*(?:\([^()]*\)[^()]*)*)\)\s*(?:const\s*)?\{?\s*$', head)
        if pm:
            for p in pm.group(1).split(','):
                w = re.findall(r'[A-Za-z_]\w*', p)
                if w:
                    (ptrs if ('*' in p or '&' in p) else values).add(w[-1])
        nsite = 0
        for ln in range(f.body + 1, f.end + 1):
            if ln in conv.cont or conv.new[ln] != conv.raw[ln]:
                continue
            s = conv.code[ln]
            if not s.strip() or re.match(r'^\s*(?:\}\s*)?(?:else\b|if\b|for\b|while\b|switch\b|return\b|case\b|do\b)', s):
                continue
            if re.search(r'(?i)\bt60|true60', conv.raw[ln]) or gated(ln):
                continue  # already in a true60 form
            kind, lv, why = None, None, ''
            m = DRAW_SMOOTH.match(s)
            if m:
                kind, lv, why = 'SMOOTH', m.group(2), '%s steps once per 30 Hz frame' % m.group(1)
            if not kind:
                m = DRAW_INC.match(s)
                if m:
                    kind, lv, why = 'COUNTER', m.group(1) or m.group(4), 'counts once per 30 Hz frame'
            if not kind:
                m = DRAW_CMP.match(s)
                if m and m.group(2) in ('|', '&') and not re.search(r'\b' + re.escape(base_field(m.group(1))) + r'\b', m.group(3)):
                    continue  # a flag set / clear: idempotent
                if m and m.group(2) == '^':
                    conv.add(ln, f, 'draw', 'review', 'draw pass (t60_draw.h)', 'toggle %s in the draw (a double buffer flips per draw?): decide by hand' % base_field(m.group(1)))
                    nsite += 1
                    continue
                if m:
                    lit = re.match(r'^-?(?:' + LIT_INT + '|' + LIT_FLT + r')$', uncast(m.group(3)))
                    kind, lv, why = ('COUNTER' if lit else 'STEP'), m.group(1), 'accumulates once per 30 Hz frame'
            set_rhs = None
            if not kind:
                m = DRAW_SET.match(s)
                if m:
                    lv, set_rhs = m.group(1), m.group(2)
            if not lv:
                if DRAW_SMOOTH_ANY.search(s):
                    conv.add(ln, f, 'draw', 'review', 'draw pass (t60_draw.h)',
                             'smoothing step in the draw through a pointer or inside an expression: once per 30 Hz frame by hand')
                    nsite += 1
                continue
            lv = lv.strip()
            root = re.split(r'->|\.|\[', re.sub(r'^this->', '', lv))[0]
            if DRAW_PORT_KEPT.search(lv) or root in tevptrs:
                continue  # a light state (dKy_tevstr_c): the port keeps it once per step (interp_tevstr_keep)
            if kind == 'COUNTER' and lv in resets:
                continue  # rebuilt on every draw (reset at the top of the draw path)
            root = re.split(r'->|\.|\[', re.sub(r'^this->', '', lv))[0]
            fld = base_field(lv)
            member = is_member(lv) or bool(re.match(r'^(g_|s_|l_|m_)|^[a-z]\w*::', lv))
            if not member or not fld or DRAW_SKIP_FIELD.search(fld) or len(fld) < 3:
                continue
            if root == lv and (root in values or root in ptrs):
                continue  # the local itself
            if root in values and root not in ptrs:
                continue  # a member of a local value (a stack copy)
            if set_rhs is not None:
                rhs = re.sub(r'\bstatic_cast<[^<>]*>\s*\(([^()]*)\)', r'\1', set_rhs)
                rhs = CAST.sub('', re.sub(r'\b(?:u32|s32|u16|s16|u8|s8|int|f32)\s*\(([^()]*)\)', r'\1', rhs)).strip()
                while rhs.startswith('(') and rhs.endswith(')') and rhs.count('(') == 1:
                    rhs = rhs[1:-1].strip()
                if re.match(r'^(?:' + re.escape(lv) + r'\s*==\s*0|!\s*' + re.escape(lv) + r'|0\s*==\s*' + re.escape(lv) + r')(?:\s*\?\s*1\s*:\s*0)?$', rhs):
                    conv.add(ln, f, 'draw', 'review', 'draw pass (t60_draw.h)', 'toggle %s in the draw (a double buffer flips per draw?): decide by hand' % fld)
                    nsite += 1
                    continue
                fm = re.match(r'^' + re.escape(lv) + r'\s*[|&]\s*(.+)$', rhs) or re.match(r'^(.+?)\s*[|&]\s*' + re.escape(lv) + r'$', rhs)
                if fm and not re.search(r'\b' + re.escape(fld) + r'\b', fm.group(1)):
                    continue  # a flag set / clear: idempotent
                fm = re.match(r'^([A-Za-z_]\w*)\s*[|&]\s*(.+)$', rhs)
                if fm and carry.get(fm.group(1), '').strip() == lv and not re.search(r'\b' + re.escape(fld) + r'\b', fm.group(2)):
                    continue  # a flag set / clear through a local copy: idempotent
                if re.search(r'(?i)init', fld) and re.match(r'^-?' + LIT_INT + r'$', rhs):
                    continue  # a one-time initialisation guard
                carried = re.match(r'^([A-Za-z_]\w*)\s*[-+]\s*' + LIT_INT + r'$', rhs)
                if lv in resets and re.match(r'^' + re.escape(lv) + r'\s*[-+]\s*' + LIT_INT + r'$', rhs):
                    continue  # a per-draw count rebuilt from its reset
                if re.search(re.escape(lv) + r'(?![\w\[(])', set_rhs):
                    kind, why = 'STEP', 'self-referencing update, once per 30 Hz frame'
                elif carried and carried.group(1) in carry and carry[carried.group(1)] == lv:
                    kind, why = 'COUNTER', 'a counter carried through a local, once per 30 Hz frame'
                elif re.search(r'(?i)old|prev|last|before|bak', fld) and re.match(r'^' + LVX + r'$', rhs) and is_member(rhs):
                    kind, why = 'STEP', 'the previous-frame value latched once per 30 Hz frame'
                elif re.search(r'\b' + re.escape(fld) + r'\b', exe_blob):
                    kind = 'ANSWER'
                else:
                    continue  # recomputed from scratch on every draw: the same on every pass
            if kind in DRAW_STEP_KINDS and root in localptrs and root != lv:
                conv.add(ln, f, 'draw', 'review', 'draw pass (t60_draw.h)',
                         '%s %s through a local pointer (%s): actor state or a per-draw list? decide by hand' % (kind.lower(), fld, root))
                nsite += 1
                continue
            if kind in DRAW_STEP_KINDS and not re.search(r'[{}]', s) and s.rstrip().endswith(';'):
                raw = conv.new[ln]
                ind = re.match(r'\s*', conv.raw[ln]).group(0)
                conv.new[ln] = ind + 'if (t60_draw_frame_pass()) %s /* true60 (t60conv draw): %s (t60_draw.h) */' % (raw.strip(), why)
                conv.add(ln, f, 'draw', 'auto', 'draw pass (t60_draw.h: %s on the frame\'s draw only)' % kind.lower(),
                         '%s %s' % (kind.lower(), fld), conv.new[ln])
            elif kind == 'ANSWER' and ('(' in root or re.match(r'^(g_|s_|l_)|^[a-z]\w*::', root)):
                conv.add(ln, f, 'draw', 'review', 'draw pass (t60_draw.h, state outside the actor)',
                         '%s written by the draw into state outside the actor (%s; the port does not put it back): decide by hand' % (fld, root))
            elif kind == 'ANSWER':
                conv.add(ln, f, 'draw', 'review', 'draw pass (t60_draw.h answer form)',
                         'answer %s written by the draw, read by the execute: local copy for this pass, member on the frame pass' % fld)
            else:
                conv.add(ln, f, 'draw', 'review', 'draw pass (t60_draw.h)', '%s %s in a compound statement' % ((kind or '?').lower(), fld))
            nsite += 1
        if nsite:
            hit.append(f)
    conv.draw_funcs = hit
    return hit


UNBRACED_HEAD = re.compile(r'^\s*(?:\}\s*)?(?:(?:else\s+)?if|for|while)\s*\(.*\)\s*$|^\s*(?:\}\s*)?else\s*$')


def brace_unbraced_bodies(conv):
    """a rewritten statement that is the unbraced body of an if / else / loop and now starts with its own `if (`
    gets braces of its own, so a following `else` keeps binding to the original if (tide: `if (count > 0)
    mRescueTimer = count - 1; else ...` became a dangling else; scale-up 2026-11-17)"""
    for ln, new in enumerate(conv.new):
        old = conv.raw[ln]
        if new == old or not re.match(r'^\s*if\s*\(', new) or re.match(r'^\s*if\s*\(', old):
            continue
        j = ln - 1
        while j >= 0 and not conv.code[j].strip():
            j -= 1
        if j >= 0 and UNBRACED_HEAD.match(conv.code[j]):
            ind = re.match(r'^\s*', new).group(0)
            body, _, cmt = new.strip().partition(' /* true60')
            conv.new[ln] = ind + '{ ' + body.rstrip() + ' }' + (' /* true60' + cmt if cmt else '')


def gate_timer_else(conv):
    """a timer counted on the first step under `if (T > 0)` (or `if (T)`, `if (T != 0)`) whose `else` acts on the
    expired timer: the else is a decision on the count and moves to the first step too (`else if (t60c::first())`),
    so a later step of the frame that counted the timer down does not act on it one frame early (tide mode_gmtw:
    mRescueTimer; scale-up 2026-11-17). dt = 1: first() is true, the original."""
    code, n = conv.code, len(conv.code)
    for ln, new in enumerate(conv.new):
        if 'if (t60c::first()) {' not in new or 'counted on the frame' not in new:
            continue
        j = ln - 1
        while j >= 0 and not code[j].strip():
            j -= 1
        if j < 0:
            continue
        m = re.match(r'^\s*(?:\}\s*else\s+)?if\s*\((.*)\)\s*(\{)?\s*$', code[j])
        if not m:
            continue
        cond = m.group(1)
        var = re.match(r'^\s*(?:\(\w+\)\s*)?([\w.>-]+?)\s*(?:>\s*0|!=\s*0)?\s*$', cond)
        if not var or not re.search(r'\b' + re.escape(var.group(1).split('->')[-1].split('.')[-1]) + r'\b', code[ln]):
            continue
        if m.group(2):  # braced: find the closing brace of the if body
            depth, k = 0, j
            while k < n:
                depth += code[k].count('{') - code[k].count('}')
                if depth <= 0 and k > j:
                    break
                k += 1
            e = k
            if e >= n or not re.match(r'^\s*\}\s*else\s*(\{\s*)?$', code[e]):
                continue
        else:  # unbraced: the statement line itself, then the else line
            e = ln + 1
            while e < n and not code[e].strip():
                e += 1
            if e >= n or not re.match(r'^\s*else\s*(\{\s*)?$', code[e]):
                continue
        if conv.new[e] != conv.raw[e]:
            continue
        conv.new[e] = re.sub(r'\belse\b', 'else if (t60c::first())', conv.raw[e], count=1).rstrip() + \
            ' /* true60 (t60conv): the expired-timer branch decides on the first step */'
        conv.add(e, next((f for f in conv.funcs if f.body <= e <= f.end), conv.funcs[0]), 'tmr', 'auto',
                 '14d (decision on the counted timer: first step)', 'else of the timer test %s' % cond.strip(), conv.new[e])


def actor_stem(src):
    return os.path.splitext(os.path.basename(src))[0]


def unit_src(unit):
    """the unit's one source; a multi-file src line (d_a_ks.txt: d_a_ks.cpp d_a_ks_exec.cpp) fails loudly: the caller
    must name the file (--src), not silently take the first one (the single-src pattern of the d_a_ks finding)"""
    srcs = unit_srcs(unit)
    if len(srcs) != 1:
        raise SystemExit('t60conv: unit %s has %d sources (%s): name the one to work on with --src'
                         % (unit, len(srcs), ' '.join(srcs)))
    return srcs[0]


def write_header(conv, stem, funcs_cls):
    path = os.path.join(ROOT, 'wwhd_src', 'include', 'd', 'actor', stem + '_t60.h')
    auto = [s for s in conv.sites if s.after]
    lines = ['/* %s_t60.h (WWHD, branch true60; generated by tools/t60conv, %s): %s on a general step' % (stem, time.strftime('%Y-%m-%d'), os.path.basename(conv.src)),
             ' * length (dt = 1/N). Decomp build only.',
             ' *',
             ' * Converted, the actor executes on every step with g_dt = 1/N; at dt = 1 (N = 1, the 30 Hz fallback, an unconverted',
             ' * actor) every helper returns the original per-frame value (T-01 bit-exact). The helpers are the shared ones of',
             ' * d/t60conv.h; %s keeps thin calls only (rule 2026-11-12).' % os.path.basename(conv.src),
             ' *',
             ' * Sites (tool classification; review items are left unchanged in the source):']
    for s in conv.sites:
        lines.append(' *   %5d %-22s %-6s %-7s %s%s' % (s.ln + 1, (s.func.split('::')[-1])[:22], s.cat, s.cls, s.rule,
                                                      ('' if s.cls == 'auto' else ': ' + s.reason)))
    lines.append(' *')
    lines.append(' * Not in kT60Converted: the profile is added after the T-03 pairs. */')
    lines.append('#pragma once')
    lines.append('#include "d/t60conv.h"')
    if any(s.cat == 'draw' and s.after for s in conv.sites):
        lines.append('#include "d/t60_draw.h"')
    if 'enemy_anm' in conv.helpers:
        lines.append('#include "d/actor/d_a_enemy_anm_t60.h"')
    open(path, 'w').write('\n'.join(lines) + '\n')
    return path


def insert_include(new, stem):
    inc = '#include "d/actor/%s_t60.h" /* true60 (t60conv): step helpers */' % stem
    if any(inc.split(' /*')[0] in l for l in new):
        return new
    last = None
    for i, l in enumerate(new[:200]):
        if l.startswith('#include'):
            last = i
    if last is None:
        return [inc] + new
    return new[:last + 1] + [inc] + new[last + 1:]


def conv_roots(conv):
    return {address_of(f, conv.funcs) for f in conv.roots}


# ------------------------------------------------------------------ effect call targets (T-02 `t60calls`)
CALLADDR = re.compile(r'gabi::call\s*(?:<[^<>]*(?:<[^<>]*>[^<>]*)*>)?\s*\(\s*(0x[0-9A-Fa-f]{8})\b')
DEFHEAD = re.compile(r'\b([A-Za-z_]\w*)\s*\((?:[^;{}()]|\([^()]*\))*\)\s*(?:const\s*)?\{')
# actor creation: invisible to the memory comparison as much as a sound start (fallrock_tag: a rock per step)
CREATE = re.compile(r'\b(fopAcM_(?:create|fastCreate|createChild|createItem|createDisappear)\w*)\s*\(')
EXTGLUE = re.compile(r'^\s*ext\s+([0-9A-Fa-f]{8})\s+[^\n(]*?\b(\w+)\s*\(', re.M)
NOTFN = {'if', 'for', 'while', 'switch', 'return', 'sizeof', 'catch'}
_DEFS = None


def _scan_defs(text, into):
    """name -> [bodies] of the function definitions in text (inline helpers, file-local wrappers)"""
    for m in DEFHEAD.finditer(text):
        if m.group(1) in NOTFN:
            continue
        into.setdefault(m.group(1), []).append(braced(text, m.end() - 1))


def include_defs():
    global _DEFS
    if _DEFS is None:
        _DEFS = {}
        for dp, _, fs in os.walk(os.path.join(ROOT, 'wwhd_src', 'include')):
            for fn in fs:
                if fn.endswith('.h'):
                    try:
                        _scan_defs(open(os.path.join(dp, fn), errors='replace').read(), _DEFS)
                    except OSError:
                        pass
    return _DEFS


def effect_targets(src_text):
    """the guest addresses of the sound starts / particle sets the source calls (the one-emitter-per-event rule (11) / 8a effects): every SND / PTC
    name the source calls, resolved through the inline definitions (the file's own first, then the includes; nested
    effect wrappers followed 4 deep) to its gabi::call addresses. Returns (addresses, unresolved names). T-02 counts the
    calls to these over the N steps against the one step (`t60calls`): any effect start, per frame or per event, must be
    made as often at N steps as in one step."""
    local = {}
    _scan_defs(src_text, local)
    inc = include_defs()
    # autoglue `ext ADDR proto` lines (the file's WWHD glue block): name -> address
    glue = {}
    for m in EXTGLUE.finditer(src_text):
        glue.setdefault(m.group(2), set()).add(int(m.group(1), 16))
    code = strip_code(src_text.split('\n'))
    code = '\n'.join(code) if isinstance(code, list) else code
    # queries (mDoAud_check*, *is*, get*) are not effects
    names = {m.group(1) for r in (SND, PTC, CREATE) for m in r.finditer(code)
             if not re.match(r'(?:mDoAud_)?(?:check|is|get|Check|Is|Get)', m.group(1).split('_', 1)[-1] if m.group(1).startswith('mDoAud_') else m.group(1))}
    # glue names of sound starts under a local name (sound_ext_addrs), when the source calls them
    snd = sound_ext_addrs()
    for nm, ads in glue.items():
        if any(('%08X' % a) in snd for a in ads) and re.search(r'\b' + nm + r'\s*\(', code):
            names.add(nm)
    addrs, unresolved = set(), set()

    def resolve(name, depth=0):
        if name in glue:
            return set(glue[name])
        bodies = local.get(name) or inc.get(name) or []
        found = set()
        for b in bodies:
            found |= {int(a, 16) for a in CALLADDR.findall(b)}
            if not found and depth < 4:
                for m in (SND.finditer(b), PTC.finditer(b), CREATE.finditer(b)):
                    for mm in m:
                        if mm.group(1) != name:
                            found |= resolve(mm.group(1), depth + 1)
        return found

    for n in sorted(names):
        a = resolve(n)
        if a:
            addrs |= a
        else:
            unresolved.add(n)
    return sorted('%08X' % a for a in addrs), sorted(unresolved)


# c_lib eases (true60's c_lib is dt-aware: t60_ratio scale, steps x dt): T-02 runs their CONVERTED code by default
# (sweep: a mocked ease ignores the dt a caller sets, so a per-frame ease at the step's dt passed
# T-02;'s ph finding). The form is the himo2 / player one: c_lib.cpp on a `src` line plus `real` and
# `t60cand` for the eases (a plain `real` runs the ORIGINAL recompiled ease, which ignores dt too). A spec that still fails with them real carries `# known-fail clib-real: <owner item>`
# (verdict KNOWN FAIL, kept visible) until its owner fixes the form or adds a reasoned tolerance.
CLIB_EASE = {'0200ECD4': 'cLib_addCalc', '0200ED84': 'cLib_addCalc2', '0200EDC8': 'cLib_addCalc0', '0200EE00': 'cLib_addCalcPos',
             '0200EF78': 'cLib_addCalcPosXZ', '0200F164': 'cLib_addCalcPos2', '0200F268': 'cLib_addCalcPosXZ2',
             '0200F378': 'cLib_addCalcAngleS', '0200F428': 'cLib_addCalcAngleS2', '0200F474': 'cLib_addCalcAngleL',
             '0200F4FC': 'cLib_chaseUC', '0200F564': 'cLib_chaseS', '0200F5C8': 'cLib_chaseF', '0200F62C': 'cLib_chasePos',
             '0200F764': 'cLib_chasePosXZ', '0200F8D0': 'cLib_chaseAngleS'}


def clib_called(src_text):
    """the c_lib ease addresses a source calls (canonical names, `ext` glue lines, direct gabi::call addresses)"""
    out = set()
    for a, n in CLIB_EASE.items():
        if re.search(r'\b%s\s*\(' % n, src_text) or re.search(r'0x%s\b' % a, src_text, re.I):
            out.add(a)
    for m in re.finditer(r'^\s*ext\s+([0-9A-Fa-f]{8})\s', src_text, re.M):
        if m.group(1).upper() in CLIB_EASE:
            out.add(m.group(1).upper())
    return out


def clib_ext_glue(src_text):
    """the c_lib ease names the source's autoglue `ext ADDR ... cLib_x(...)` lines define (they clash with c_lib.cpp)"""
    return sorted({m.group(2) for m in re.finditer(r'^\s*ext\s+([0-9A-Fa-f]{8})\s.*?\b(cLib_\w+)\s*\(', src_text, re.M)
                   if m.group(1).upper() in CLIB_EASE})


def ensure_clib_real(unit, rep=None):
    """add a `real` line for the c_lib eases the unit calls and its _t60 spec leaves mocked (the default); returns the
    added addresses"""
    path = os.path.join(UNITS, '_t60', unit + '.txt')
    if not os.path.exists(path):
        return []
    txt = open(path).read()
    if '# mock-clib:' in txt:  # an explicit, reasoned opt-out (none at landing)
        return []
    try:
        srcs = unit_srcs(unit)
    except Exception:
        srcs = []
    src = '\n'.join(open(os.path.join(ROOT, s), errors='replace').read() for s in srcs)
    cand = set(a.upper() for l in txt.split('\n') if l.startswith('t60cand ') for a in re.findall(r'\b([0-9A-Fa-f]{8})\b', l))
    add = sorted(clib_called(src) - cand)
    clash = clib_ext_glue(src)
    if add and clash and rep is not None:  # the unit's `ext` glue defines cLib_* names; the harness makes that glue weak
        rep.setdefault('notes', []).append('c_lib eases %s: the unit\'s `ext` glue defines %s (weak in the verify harness, so '
                                           'c_lib.cpp\'s converted code is linked)' % (' '.join(add), ' '.join(clash)))
    if add:
        open(path, 'w').write(txt.rstrip('\n') + '\n' + clib_lines(txt, add))
    return add


def clib_lines(spec_text, addrs):
    """the spec lines that run the eases' converted code: c_lib.cpp as a src (once), real + t60cand"""
    out = '# c_lib eases run their converted (dt-aware) code (t60conv default, 2026-11-20; a mocked or plain-real ease ignores the step dt)\n'
    if not re.search(r'^src\s+wwhd_src/SSystem/SComponent/c_lib\.cpp\b', spec_text, re.M):
        out += 'src wwhd_src/SSystem/SComponent/c_lib.cpp\n'
    return out + 'real ' + ' '.join(addrs) + '\nt60cand ' + ' '.join(addrs) + '\n'


def known_fail(unit):
    """the `# known-fail clib-real: <item>` note of the unit's _t60 spec, or None"""
    path = os.path.join(UNITS, '_t60', unit + '.txt')
    if not os.path.exists(path):
        return None
    m = re.search(r'^#\s*known-fail clib-real:\s*(.*)$', open(path).read(), re.M)
    return m.group(1).strip() if m else None


def ensure_calls_line(unit, rep):
    """add the `t60calls` line (effect targets) to the unit's T-02 spec when it has none; records it in rep"""
    spec = os.path.join(UNITS, '_t60', unit + '.txt')
    if not os.path.exists(spec):
        return
    txt = open(spec).read()
    TAG = '# effect starts (sound / particle; tools/t60conv): as many calls over the N steps as in the one step\n'
    TAG2 = '# effect calls (sound / particle starts, actor creation; tools/t60conv): as many calls over the N steps as in the one step\n'
    # the tool's own line is refreshed; a hand-written t60calls line (no tool comment above it) is kept
    txt2 = re.sub(r'^(?:' + re.escape(TAG) + '|' + re.escape(TAG2) + r')t60calls[^\n]*\n', '', txt, flags=re.M)
    if re.search(r'^t60calls\b', txt2, re.M):
        return
    addrs, unres = effect_targets(open(os.path.join(ROOT, rep['src'])).read())
    rep['effect_calls'], rep['effect_unresolved'] = addrs, unres
    if txt2 != txt:
        open(spec, 'w').write(txt2)
    if addrs:
        open(spec, 'a').write(TAG2 + 't60calls ' + ' '.join(addrs) + '\n')


# ------------------------------------------------------------------ whole form (t60_whole.h: the whole frame on tick30)
# 2026-11-18 (d_a_npc_people): a whole-form spec compares memory only, so on mocked callees its negative control
# (the execute on every step) can give the same result twice; counting the execute's calls (`t60calls`) makes it fail.
# The whole form runs the original execute once per 30 Hz frame on the tick30 step; the earlier steps only show the
# picture (the Ops mtx / picture function). So every call the execute makes, except those of the picture, is made as
# often over the N steps as in the one step.
WHOLE_CALL = re.compile(r'\b(\w*t60\w*)::(?:begin\s*\(|Guard\s*<[^<>;]*>\s*\w+\s*\()')
QCALL = re.compile(r'((?:\w+\s*::\s*)*)([A-Za-z_]\w*)\s*(?:<[^<>;()]*>)?\s*\(')
WHOLE_SKIPQ = ('t60', 'gabi', 'std')
_NAMES = None


def names_map():
    """build/names.tsv: qualified name -> {addresses} (the rpx's function names)"""
    global _NAMES
    if _NAMES is None:
        _NAMES = {}
        try:
            for l in open(os.path.join(ROOT, 'build', 'names.tsv'), errors='replace'):
                c = l.rstrip('\n').split('\t')
                if len(c) >= 2 and re.match(r'^[0-9A-Fa-f]{8}$', c[0]):
                    _NAMES.setdefault(c[1], set()).add(c[0].upper())
        except OSError:
            pass
    return _NAMES


def _call_args(text, i):
    """the text between the '(' at text[i] and its matching ')'"""
    d = 0
    for j in range(i, len(text)):
        d += (text[j] == '(') - (text[j] == ')')
        if d == 0:
            return text[i + 1:j]
    return text[i + 1:]


def whole_calls(srcs, header=''):
    """the `t60calls` addresses of a whole-form unit: [(function, [addresses], [excluded picture addresses])] for each
    function of the sources whose body opens the whole form (t60w::begin / a *_t60::Guard). Callees resolved through
    the unit's own definitions (followed, 6 deep), build/names.tsv (Class::name, then the free name) and the include
    files' inline gabi::call wrappers; calls qualified by a t60 / gabi / std namespace are the form's own. Indirect
    calls through member-pointer / guest tables ((this->*mode_proc.mEntries[mMode])(), call_ptr(TABLE[i]), a member
    pointer assigned &Class::fn) count as calls of every target resolve_dispatch names for that line (s25h,
    2026-11-18: d_a_obj_correct's mode_proc / demo_proc modes were not followed, so its switch / search / event calls
    needed a hand `t60calls` line). The picture function (the Guard's / begin's second argument, or the mtx member of an
    Ops table) and what it calls are left out: the earlier steps call it."""
    raw_all, code_all = [], []
    for sp in srcs:
        raw = open(os.path.join(ROOT, sp), errors='replace').read().split('\n')
        raw_all += raw
        code_all += strip_code(raw)
    hcode = '\n'.join(strip_code(header.split('\n')))
    code_txt = '\n'.join(code_all)
    funcs = parse_functions(raw_all, code_all)
    resolve_dispatch(raw_all, code_all, funcs, header)  # f.dispatch[line]: the targets of its table / member-pointer calls
    local = {}
    for f in funcs:
        local.setdefault(f.short, []).append(f)
    hdefs = {}
    _scan_defs(hcode, hdefs)
    inc = include_defs()
    nm = names_map()
    # the sources' WWHD glue `ext ADDR TYPE name(...)` lines (in a comment block, so read from the raw text): a glue
    # name is that address (d_a_obj_correct correctOnSwitch 025B9E38, correctSearch 025D5218; s25h 2026-11-18)
    glue = {}
    for gm in re.finditer(r'^\s*ext\s+([0-9A-Fa-f]{8})\s+[^()\n]*?\b([A-Za-z_]\w*)\s*\(', '\n'.join(raw_all) + '\n' + header, re.M):
        glue.setdefault(gm.group(2), set()).add(gm.group(1).upper())

    def body(f):
        return '\n'.join(code_all[f.body:f.end + 1])

    def resolve(name, cls, seen, depth):
        out = set()
        if name in NOTFN or depth > 6:
            return out
        if name in local:
            for g in local[name]:
                if g.addr:
                    out.add(g.addr)
                if g.name not in seen:
                    seen.add(g.name)
                    gcls = g.name.rsplit('::', 1)[0] if '::' in g.name else cls
                    out |= fn_calls(g, gcls, seen, depth + 1)
            return out
        if name in glue:
            return set(glue[name])
        for key in ((cls + '::' + name) if cls else None, name):
            if key and key in nm:
                return set(nm[key])
        for b in inc.get(name, []):
            out |= {('%08X' % int(a, 16)) for a in CALLADDR.findall(b)}
        return out

    def fn_calls(g, cls, seen, depth):
        """a local function's calls: its body's calls and its dispatch lines' targets (followed like direct calls)"""
        out = calls_of(body(g), cls, seen, depth)
        for tg in getattr(g, 'dispatch', {}).values():
            for t in sorted(tg):
                out |= resolve(t, cls, seen, depth)
        return out

    def calls_of(text, cls, seen, depth):
        out = {('%08X' % int(a, 16)) for a in CALLADDR.findall(text)}
        for m in QCALL.finditer(text):
            q = m.group(1).replace(' ', '')
            if any(x in q for x in WHOLE_SKIPQ):
                continue
            out |= resolve(m.group(2), cls, seen, depth)
        return out

    res = []
    for f in funcs:
        b = body(f)
        m = WHOLE_CALL.search(b)
        if not m:
            continue
        cls = f.name.rsplit('::', 1)[0] if '::' in f.name else ''
        args = [x.strip() for x in _call_args(b, b.index('(', m.end() - 1)).split(',')]
        pic = set()
        for a in args[1:]:
            mo = re.match(r'^(?:(\w+)::)?(\w+)$', a)
            if not mo:
                continue
            nm_ = mo.group(2)
            if nm_ in local or nm_ in hdefs:
                for g in local.get(nm_, []):
                    pic |= fn_calls(g, cls, set(), 0)
                pic |= calls_of('\n'.join(hdefs.get(nm_, [])), cls, set(), 0)
                continue
            ini = re.search(r'\b' + re.escape(nm_) + r'\s*=\s*\{([^{}]*)\}', hcode + '\n' + code_txt)
            if ini:  # an Ops table {n, nPos, get, set, mtx, held}: the mtx member
                el = [x.strip() for x in ini.group(1).split(',')]
                if len(el) >= 5:
                    mt = el[4]
                    txt = '\n'.join(body(g) for g in local.get(mt, [])) + '\n'.join(hdefs.get(mt, []))
                    pic |= calls_of(txt, cls, set(), 0)
        allc = fn_calls(f, cls, {f.name}, 0)
        if f.addr:
            allc.discard(f.addr)
        res.append((f.name, sorted(allc - pic), sorted(pic)))
    return res


def unit_srcs(unit):
    """every source of a unit's `src` line(s)"""
    out = []
    for line in open(os.path.join(UNITS, unit + '.txt')):
        m = re.match(r'^src\s+(.*)$', line)
        if m:
            out += m.group(1).split('#')[0].split()
    return out


def whole_header(srcs):
    """the text of the *_t60.h headers the sources include (the whole form's Ops table / picture helpers)"""
    txt, seen = '', set()
    for sp in srcs:
        for inc in re.findall(r'^\s*#\s*include\s+"([^"]*_t60\.h)"', open(os.path.join(ROOT, sp), errors='replace').read(), re.M):
            h = os.path.join(ROOT, 'wwhd_src', 'include', inc)
            if h not in seen and os.path.exists(h):
                seen.add(h)
                txt += open(h, errors='replace').read() + '\n'
    return txt


WHOLE_TAG = '# whole-form calls (tools/t60conv): every call of the whole-form execute but its picture, as many over the N steps as in the one step\n'
WHOLE_TAG_MIXED = ('# whole-form calls (tools/t60conv, mixed form): every call of the whole-form execute but its picture, as many over '
                   'the N steps as in the one step, on the inputs the t60wsel line selects\n')


def whole_mixed(srcs):
    """the mixed form (2026-11-19, first case d_a_nz): functions whose whole form (t60w::begin / a
    *_t60::Guard) opens inside a nested block (under a condition), so only some frames run whole and the others per
    step. -> [(function, line text)]. Such a unit's whole-form calls are compared only on the frames that take the
    Guard path: the spec names them with `t60wsel FUNC rN+OFF TYPE V,...` (the frame-start value that selects it)."""
    raw_all, code_all = [], []
    for sp in srcs:
        raw = open(os.path.join(ROOT, sp), errors='replace').read().split('\n')
        raw_all += raw
        code_all += strip_code(raw)
    out = []
    for f in parse_functions(raw_all, code_all):
        b = '\n'.join(code_all[f.body:f.end + 1])
        m = WHOLE_CALL.search(b)
        if not m:
            continue
        head = b[:m.start()]
        if head.count('{') - head.count('}') > 1:
            out.append((f.name, b[b.rfind('\n', 0, m.start()) + 1:b.find('\n', m.start())].strip()))
    return out


def ensure_whole_calls(t02unit, rep=None):
    """add the whole-form `t60calls` line to the T-02 spec _t60/T02UNIT.txt: the addresses no other t60calls line
    lists (hand lines kept; the tool's own line refreshed). Returns [(function, addresses, picture)] or [] when no
    source of the unit is whole-form."""
    spec = os.path.join(UNITS, '_t60', t02unit + '.txt')
    txt = open(spec).read()
    inc = re.search(r'^include\s+(\S+)\.txt', txt, re.M)
    base = inc.group(1) if inc else t02unit
    srcs = unit_srcs(base)
    found = whole_calls(srcs, whole_header(srcs))
    if rep is not None:
        rep['whole'] = found
    if not found:
        return []
    # the tool's own line (either form) is refreshed; hand t60calls / t60wcalls lines are kept. With a `t60wsel` line
    # (mixed form) the line is `t60wcalls`: compared only on the inputs the selector picks (verify harness)
    txt2 = re.sub(r'^(?:' + re.escape(WHOLE_TAG) + '|' + re.escape(WHOLE_TAG_MIXED) + r')t60w?calls[^\n]*\n', '', txt, flags=re.M)
    have = {a.upper() for l in re.findall(r'^t60w?calls\s+(.*)$', txt2, re.M) for a in l.split()}
    want = sorted({a for _, ad, _ in found for a in ad} - have)
    mixed = re.search(r'^t60wsel\s', txt2, re.M) is not None
    if rep is not None:
        rep['whole_wsel'] = mixed
    if txt2 != txt:
        open(spec, 'w').write(txt2)
    if want:
        open(spec, 'a').write((WHOLE_TAG_MIXED + 't60wcalls ' if mixed else WHOLE_TAG + 't60calls ') + ' '.join(want) + '\n')
    return found


def whole_check(t02unit, n=2000, seed=1, shards=4):
    """the whole-form gate (2026-11-18): the `t60calls` line (ensure_whole_calls), T-02 N 2 / 4 on the spec, and
    the negative control (the sources with T60C_NEGCTRL: the execute on every step at dt) must FAIL on a whole-form
    function. -> dict(verdict, ...)"""
    od = os.path.join(OUT, t02unit + '-whole')
    os.makedirs(od, exist_ok=True)
    found = ensure_whole_calls(t02unit)
    if not found:
        return dict(verdict='NOT WHOLE-FORM (no t60w::begin / *_t60::Guard in the sources)')
    spec = os.path.join(UNITS, '_t60', t02unit + '.txt')
    txt = open(spec).read()
    inc0 = re.search(r'^include\s+(\S+)\.txt', txt, re.M)
    mixed = whole_mixed(unit_srcs(inc0.group(1) if inc0 else t02unit))
    wsel = re.search(r'^t60wsel\s', txt, re.M) is not None
    if mixed and not wsel:
        return dict(whole=found, mixed=mixed, verdict=(
            'NEEDS t60wsel (mixed form: the whole form opens under a condition in %s: %s): add `t60wsel FUNC rN+OFF TYPE '
            'V,...` naming the frame-start value that selects the Guard path, then run whole again' % (
                ', '.join(f for f, _ in mixed), mixed[0][1][:80])))
    conv = sorted({a.upper() for l in re.findall(r'^t60conv\s+(.*)$', txt, re.M) for a in l.split()})
    base = ['-n', str(n), '-nulltrap', '-shards', str(shards), '-only', ','.join(conv), '-seed', str(seed)]
    res = dict(whole=found, t02={}, nc={})
    for N in (2, 4):
        rc, out = verify('_t60/' + t02unit, base + ['-t60steps', str(N)], os.path.join(od, 't02_n%d.log' % N))
        res['t02'][N] = rows(out)
    # negative control: every source copied with T60C_NEGCTRL defined
    inc = re.search(r'^include\s+(\S+)\.txt', txt, re.M)
    bunit = inc.group(1) if inc else t02unit
    d = os.path.join(ROOT, 'build', 't60conv')
    os.makedirs(d, exist_ok=True)
    mapping = {}
    for sp in unit_srcs(bunit):
        cp = os.path.join(d, 'wnc_' + os.path.basename(sp))
        open(cp, 'w').write('#define T60C_NEGCTRL 1 /* t60conv negative control */\n' + open(os.path.join(ROOT, sp)).read())
        mapping[sp] = 'build/t60conv/wnc_' + os.path.basename(sp)
    uspec = subst_srcs(open(os.path.join(UNITS, bunit + '.txt')).read(), mapping)
    t02 = [l for l in txt.split('\n') if not l.startswith('include ')]
    ncname = '_t60/%s_t60wnc' % t02unit
    open(os.path.join(UNITS, ncname + '.txt'), 'w').write('# GENERATED by tools/t60conv (whole-form negative control, not for commit)\n' + uspec + '\n' + '\n'.join(t02) + '\n')
    try:
        for N in (2, 4):
            rc, out = verify(ncname, base + ['-t60steps', str(N)], os.path.join(od, 'nc_n%d.log' % N))
            res['nc'][N] = rows(out)
    finally:
        os.remove(os.path.join(UNITS, ncname + '.txt'))
    t02ok = all(res['t02'].values()) and all(row_ok(r) for rr in res['t02'].values() for r in rr.values())
    ncfail = any(not row_ok(r) for rr in res['nc'].values() for r in rr.values())
    ncran = all(res['nc'].get(N) for N in (2, 4))
    if not t02ok:
        res['verdict'] = 'FAIL T-02 (the whole form differs from the one step, or a counted call is made on an earlier step)'
    elif not ncran:
        # (an NC unit that does not build / run gives no rows; before, that read as 'the negative control passes')
        res['verdict'] = 'NC DID NOT RUN (no result rows: build or run error, see nc_n2.log / nc_n4.log)'
    elif not ncfail:
        res['verdict'] = ('FAIL WHOLE NC: the negative control passes (the execute on every step gives the same memory and '
                          'call counts): add the calls it repeats to t60calls, or inputs that reach them')
    else:
        res['verdict'] = 'PASS (whole form: T-02 exact with the calls counted; the negative control fails)'
    if mixed:
        res['mixed'] = mixed
        res['verdict'] += '; MIXED FORM (whole-form calls compared on the t60wsel inputs only)'
    # the draw path's state writes, for review (2026-11-18): a converted process is drawn on every step
    dw = [x for x in draw_writes(unit_srcs(bunit)) if x[2] == 'state']
    res['draw_writes'] = dw
    dr = draw_rnd(unit_srcs(bunit))
    res['draw_rnd'] = dr
    if dr:
        # informational (2026-11-18): draws on hold passes keep the RNG seeds (interp_fx hold path, proved it), so
        # a draw path reaching cM_rnd is no gate issue; what matters is cM_rnd inside an execute (T-02 counts those)
        res['verdict'] += '; DRAW RNG %d info (the draw path reaches %s; hold-pass draws keep the RNG seeds, so no action: only cM_rnd inside an execute matters)' % (
            len(dr), ', '.join(sorted({c for _, _, c, _ in dr})))
    if dw:
        res['verdict'] += '; DRAW WRITES %d for review (logic state -> t60npc::drawStep()): %s' % (
            len(dw), '; '.join('%s:%d %s' % (f.split('::')[-1], ln, tx[:60]) for f, ln, _, tx in dw[:4]))
    json.dump(res, open(os.path.join(od, 'whole.json'), 'w'), indent=1, default=str)
    return res


DRAW_FN = re.compile(r'(?:^|_)(?:draw|Draw)$|^_?draw\w*$|\w*_[Dd]raw$')
DRAW_SKIPFN = re.compile(r'[Ee]xecute')
WR_TARGET = re.compile(r'^\s*(?:\(\s*\*\s*)?([A-Za-z_]\w*)((?:\s*(?:->|\.)\s*[A-Za-z_]\w*|\[[^\]]*\]|\)|\s*\(\s*\))*)\s*([+\-*/|&^]|<<|>>)?=(?!=)')
WR_INCDEC = re.compile(r'(?:\+\+|--)\s*([A-Za-z_]\w*)((?:(?:->|\.)[A-Za-z_]\w*|\[[^\]]*\])*)|([A-Za-z_]\w*)((?:(?:->|\.)[A-Za-z_]\w*|\[[^\]]*\])*)\s*(?:\+\+|--)')
WR_DECL = re.compile(r'(?:^|[;({,]\s*)(?:(?:const|static|volatile|unsigned|struct)\s+)*[A-Za-z_][\w:]*(?:\s*<[^;()]*?>)?[\s*&]+([A-Za-z_]\w*)\s*(?:=|;|\[|,|\)|$)')
WR_RENDER = ('tevStr', 'mDoExt', 'J3D', 'GX', 'g_env_light', 'j3dSys', 'mDoMtx_stack', 'l_debug', 'Guard', 'guard', 'Initialized', 'initialized', 'Init')


def draw_writes(srcs):
    """the draw path's writes for review (2026-11-18, Ym1: a converted process is drawn on every step, so a draw
    that changes logic state runs N times a frame): the unit's draw functions (draw / _draw / *_Draw) and the unit's own
    functions they call (4 deep, executes left out), each assignment / ++ / -- whose target is not a local of the
    function. -> [(function, line, kind, text)], kind 'state' (a field or global: review: gate with t60npc::drawStep() if
    logic) or 'render' (a name of WR_RENDER: render state, debug / static-init guards)."""
    raw_all, code_all = [], []
    for sp in srcs:
        raw = open(os.path.join(ROOT, sp), errors='replace').read().split('\n')
        raw_all += raw
        code_all += strip_code(raw)
    funcs = parse_functions(raw_all, code_all)
    local = {}
    for f in funcs:
        local.setdefault(f.short, []).append(f)
    todo = [(f, 0) for f in funcs if DRAW_FN.search(f.short) and not DRAW_SKIPFN.search(f.short)]
    seen, out = set(), []
    while todo:
        f, depth = todo.pop(0)
        if f.name in seen:
            continue
        seen.add(f.name)
        lines = code_all[f.start:f.end + 1]
        sig = ' '.join(code_all[f.start:f.body + 1])
        params = set(re.findall(r'([A-Za-z_]\w*)\s*(?:,|\))', sig[sig.find('('):]))
        locs = set()
        for l in lines:
            locs |= set(WR_DECL.findall(l))
        for st0 in ' '.join(lines).split(';'):
            for st in re.split(r'[{}]', st0):
                m = re.match(r'^\s*(?:(?:const|static|volatile|unsigned|struct)\s+)*[A-Za-z_][\w:]*(?:\s*<[^;()]*?>)?[\s*&]+([A-Za-z_]\w*.*)$', st)
                if m and not re.match(r'^\s*(?:return|delete|goto|case|else|new)\b', st):
                    depth, part = 0, ''
                    for ch in m.group(1).replace('->', '.') + ',':
                        if ch in '([{<':
                            depth += 1
                        elif ch in ')]}>':
                            depth -= 1
                        if ch == ',' and depth == 0:
                            mm = re.match(r'\s*[*&]*\s*([A-Za-z_]\w*)', part)
                            if mm:
                                locs.add(mm.group(1))
                            part = ''
                        else:
                            part += ch
        for l in lines:
            locs |= set(re.findall(r'\bfor\s*\(\s*[A-Za-z_][\w:]*[\s*&]+([A-Za-z_]\w*)', l))
        for k in range(f.body, f.end + 1):
            l = code_all[k]
            hits = []
            for st in re.split(r'(?<=;)', l):
                m = WR_TARGET.match(st)
                if m:
                    hits.append((m.group(1), m.group(2) or ''))
                for mm in WR_INCDEC.finditer(st):
                    hits.append((mm.group(1) or mm.group(3), mm.group(2) or mm.group(4) or ''))
            for root, acc in hits:
                if root in ('return', 'if', 'else', 'case') or root in params and not acc:
                    continue
                if root in locs and not (root in params or '->' in acc):
                    continue  # a local value (a pointer local written through -> stays: object state)
                if root in locs and '->' in acc and re.search(r'\b%s\b' % re.escape(root), ' '.join(lines)) and \
                        re.search(r'(?:gabi::Local|Local)\s*<[^>]*>\s*%s\b' % re.escape(root), ' '.join(lines)):
                    continue  # a gabi::Local temporary
                txt = raw_all[k].strip()
                kind = 'render' if any(w in root + acc or w in txt.split('=')[0] for w in WR_RENDER) else 'state'
                out.append((f.name, k + 1, kind, txt[:140]))
        if depth < 4:
            body = '\n'.join(code_all[f.body:f.end + 1])
            for name in set(re.findall(r'\b([A-Za-z_]\w*)\s*\(', body)):
                if name in local and not DRAW_SKIPFN.search(name):
                    for g in local[name]:
                        if g.name not in seen:
                            todo.append((g, depth + 1))
    return out


DRAW_RND = re.compile(r'\b(cM_rnd\w*|\w*settingTevStruct\w*|\w*plightcol_plus\w*)\s*\(')


def draw_rnd(srcs):
    """the draw path's random draws (2026-11-18): settingTevStruct -> plightcol_plus draws cM_rndF (point-light
    flicker), so an actor drawn on every step advances the shared RNG N times a frame. The unit's draw functions and the
    unit functions they call (as draw_writes), each call of cM_rnd* or a settingTevStruct / plightcol_plus wrapper.
    -> [(function, line, callee, text)]. Informational since 2026-11-18: draws on hold passes keep the RNG seeds
    (interp_fx hold path; proved it), so only cM_rnd inside an execute matters (T-02 / the whole form's call counts)."""
    raw_all, code_all = [], []
    for sp in srcs:
        raw = open(os.path.join(ROOT, sp), errors='replace').read().split('\n')
        raw_all += raw
        code_all += strip_code(raw)
    funcs = parse_functions(raw_all, code_all)
    local = {}
    for f in funcs:
        local.setdefault(f.short, []).append(f)
    todo = [(f, 0) for f in funcs if DRAW_FN.search(f.short) and not DRAW_SKIPFN.search(f.short)]
    seen, out = set(), []
    while todo:
        f, depth = todo.pop(0)
        if f.name in seen:
            continue
        seen.add(f.name)
        for k in range(f.body, f.end + 1):
            for m in DRAW_RND.finditer(code_all[k]):
                if m.group(1) not in local or DRAW_RND.match(m.group(1) + '(') and m.group(1).startswith('cM_rnd'):
                    out.append((f.name, k + 1, m.group(1), raw_all[k].strip()[:120]))
        if depth < 4:
            body = '\n'.join(code_all[f.body:f.end + 1])
            for name in set(re.findall(r'\b([A-Za-z_]\w*)\s*\(', body)):
                if name in local and not DRAW_SKIPFN.search(name):
                    for g in local[name]:
                        if g.name not in seen:
                            todo.append((g, depth + 1))
    return out


def write_spec(unit, addrs, conv):
    path = os.path.join(UNITS, '_t60', unit + '.txt')
    lines = ['# true60 T-02 spec (not a gate unit: units/_t60/ is left out of verify_all and the game build\'s unit list)',
             'include %s.txt' % unit,
             '# ---- true60 T-02 (generated by tools/t60conv %s): the execute path on a general step length; callees are mocks'
             % time.strftime('%Y-%m-%d'),
             '# (the same answer on every step and on the one step). Timers / flags / modes exact; t60tol lines below only for',
             '# float rounding of inline integration sites (tool tolerance loop, each listed in the tool report).',
             't60conv ' + ' '.join(addrs)]
    cand = [a for a in getattr(conv, 'cand', []) if a not in conv_roots(conv)]
    if cand:
        lines.append('# same-unit callees with converted sites run their candidate code under their callers (tools/t60conv)')
        lines.append('real ' + ' '.join(cand))
        lines.append('t60cand ' + ' '.join(cand))
    ease = sorted(clib_called('\n'.join(conv.raw)) - set(cand))
    if ease:
        lines.extend(clib_lines('\n'.join(lines), ease).rstrip('\n').split('\n'))
    for t in conv.tols:
        lines.append(t)
    open(path, 'w').write('\n'.join(lines) + '\n')
    return path


def convert(unit, src=None, dry=False):
    src = src or unit_src(unit)
    if not dry and '(t60conv)' in open(os.path.join(ROOT, src)).read():
        raise SystemExit('%s is already converted (t60conv marker): restore it (git checkout) before a new conversion' % src)
    conv = Converter(unit, src)
    conv.tols = []
    conv.run()
    stem = actor_stem(src)
    funcs_cls = conv.classify_functions()
    addrs = sorted({v['addr'] for k, v in funcs_cls.items() if v['addr']})
    gated = {g.name for g in conv.gated}
    step_names = {f.name for f in conv.exe if conv.fmode.get(f.name) in ('step', 'cond') and (f.name not in conv.covered or f.name in gated)}
    # an address holds inlined code too: drop an address any of whose functions is not a step function
    by_addr = {}
    for f in conv.exe:
        a = address_of(f, conv.funcs)
        if a:
            by_addr.setdefault(a, []).append(f.name)
    step_addrs = sorted(a for a, names in by_addr.items() if any(n in step_names for n in names) and
                        not any(n in conv.evonly for n in names))
    changed = any(s.after for s in conv.sites)
    changed_exec = any(s.after and s.cat != 'draw' for s in conv.sites)
    draw_addrs = sorted({address_of(f, conv.funcs) for f in getattr(conv, 'draw_funcs', [])} - {None})
    od = os.path.join(OUT, unit)
    os.makedirs(od, exist_ok=True)
    rep = dict(unit=unit, src=src, roots=[f.name for f in conv.roots], exe=[f.name for f in conv.exe],
               addrs=addrs, step_addrs=step_addrs, modes=conv.fmode, functions=funcs_cls, sites=[s.js() for s in conv.sites], changed=changed, changed_exec=changed_exec, draw_addrs=draw_addrs,
               float_sites=conv.float_sites)
    json.dump(rep, open(os.path.join(od, 'convert.json'), 'w'), indent=1)
    if dry:
        return rep
    if changed:
        gate_timer_else(conv)
        brace_unbraced_bodies(conv)
        new = insert_include(write_source(conv, conv.new), stem)
        open(os.path.join(ROOT, src), 'w').write('\n'.join(new))
        write_header(conv, stem, funcs_cls)
    # same-unit callees with their own address that hold changed sites: real + t60cand, so a caller's T-02 runs their
    # candidate code (otherwise they are mocks and the caller's steps never reach the converted sites)
    conv.cand = sorted({s.addr for s in conv.sites if s.after and s.cat != 'draw' and s.addr and s.addr not in step_addrs[:1]} - set(step_addrs[:0]))
    if step_addrs:
        write_spec(unit, step_addrs, conv)
    elif os.path.exists(os.path.join(UNITS, '_t60', unit + '.txt')):
        open(os.path.join(UNITS, '_t60', unit + '.txt'), 'w').write(
            '# true60 (tools/t60conv %s): no T-02 for this unit: no function of its execute path runs at N > 1\n'
            '# (event-only or per-event functions; see the tool report). Do not run it with -t60steps.\ninclude %s.txt\n'
            % (time.strftime('%Y-%m-%d'), unit))
    return rep


def draw_unit(unit, src=None, dry=False, n=10000, seeds=(1, 7), shards=4):
    """the draw-path pass alone (t60_draw.h), on a unit converted before the tool had it (or any unit): rewrite the
    step forms, extend <actor>_t60.h, T-01 over the draw functions with sites (seeds s1/s7, the unchanged source as the
    baseline for failing rows). Records: $T60CONV_OUT/<unit>/draw.json, DRAW.md."""
    src = src or unit_src(unit)
    conv = Converter(unit, src)
    conv.exe, conv.roots = execute_path(conv.funcs)
    draw_pass(conv)
    sites = [s for s in conv.sites if s.cat == 'draw']
    addrs = sorted({address_of(f, conv.funcs) for f in conv.draw_funcs} - {None})
    od = os.path.join(OUT, unit)
    os.makedirs(od, exist_ok=True)
    rep = dict(unit=unit, src=src, draw_funcs=[f.name for f in conv.draw_funcs], draw_addrs=addrs,
               sites=[s.js() for s in sites], changed=any(s.after for s in sites))
    res = dict(t01={})
    if not dry and rep['changed']:
        brace_unbraced_bodies(conv)
        new = write_source(conv, conv.new)
        stem = actor_stem(src)
        hdr = os.path.join(ROOT, 'wwhd_src', 'include', 'd', 'actor', stem + '_t60.h')
        if os.path.exists(hdr) and any(('%s_t60.h' % stem) in l for l in new):
            h = open(hdr).read()
            add = ''.join(' *   %5d %-22s %-6s %-7s %s%s\n' % (s.ln + 1, (s.func.split('::')[-1])[:22], s.cat, s.cls, s.rule,
                                                                ('' if s.cls == 'auto' else ': ' + s.reason)) for s in sites)
            mark = ' *\n * Not in kT60Converted'
            h = h.replace(mark, ' * Draw path (t60conv draw pass %s; t60_draw.h):\n%s%s' % (time.strftime('%Y-%m-%d'), add, mark), 1) \
                if mark in h else h
            if '#include "d/t60_draw.h"' not in h:
                h = h.replace('#include "d/t60conv.h"\n', '#include "d/t60conv.h"\n#include "d/t60_draw.h"\n', 1)
            open(hdr, 'w').write(h)
        elif not any('d/t60_draw.h' in l for l in new):
            inc = '#include "d/t60_draw.h" /* true60 (t60conv draw): draw-path writes on the frame\'s draw only */'
            last = max([i for i, l in enumerate(new[:200]) if l.startswith('#include')] or [-1])
            new = new[:last + 1] + [inc] + new[last + 1:]
        open(os.path.join(ROOT, src), 'w').write('\n'.join(new))
        t0 = time.time()
        for s in seeds:
            rc, out = verify(unit, ['-n', str(n), '-nulltrap', '-shards', str(shards), '-only', ','.join(addrs), '-seed', str(s)],
                             os.path.join(od, 'draw_t01_s%d.log' % s))
            res['t01'][s] = rows(out)
        bad = sorted({a for rr in res['t01'].values() for a, r in rr.items() if not row_ok(r)})
        if bad:
            bunit = base_unit(unit, rep)
            res['t01_base'] = {}
            for s in seeds:
                rc, out = verify(bunit, ['-n', str(n), '-nulltrap', '-shards', str(shards), '-only', ','.join(bad), '-seed', str(s)],
                                 os.path.join(od, 'draw_t01base_s%d.log' % s))
                res['t01_base'][s] = rows(out)
            os.remove(os.path.join(UNITS, bunit + '.txt'))
        missing = [a for a in addrs if not any(a in rr for rr in res['t01'].values())]
        pre = all(not row_ok(res['t01_base'][s][a]) for s in res['t01'] for a, r in res['t01'][s].items()
                  if not row_ok(r) and a in res.get('t01_base', {}).get(s, {})) if bad else True
        res['verdict'] = ('PASS' if not bad else 'PASS (PRE-EXISTING T-01 rows)' if pre and res.get('t01_base') else 'FAIL T-01') + \
            (' (no verify row for %s: by form only)' % ','.join(missing) if missing else '') + \
            (', REVIEW items open' if any(s.cls != 'auto' for s in sites) else '')
        res['seconds'] = round(time.time() - t0)
    else:
        res['verdict'] = 'no change' + (', REVIEW items open' if any(s.cls != 'auto' for s in sites) else '') if not dry else 'dry'
    json.dump(dict(rep=rep, res=res), open(os.path.join(od, 'draw.json'), 'w'), indent=1, default=str)
    L = ['# t60conv draw pass: %s (%s)' % (unit, src), '', '**Verdict:** %s' % res['verdict'], '',
         '| line | function | class | rule | note |', '|---:|---|---|---|---|']
    L += ['| %d | %s | %s | %s | %s |' % (s.ln + 1, s.func.split('::')[-1], s.cls, s.rule, s.reason) for s in sites]
    for s, rr in res['t01'].items():
        for a, r in rr.items():
            L.append('- T01 s%s %s %s: %d/%d %s' % (s, a, r['name'], r['ok'], r['n'], r['verdict'][:160]))
    open(os.path.join(od, 'DRAW.md'), 'w').write('\n'.join(L) + '\n')
    return rep, res


# ------------------------------------------------------------------ checks
def load_wait(limit=90):
    while True:
        la = float(subprocess.run(['sysctl', '-n', 'vm.loadavg'], capture_output=True, text=True).stdout.split()[1])
        if la <= limit:
            return
        time.sleep(30)


def disk_ok():
    st = os.statvfs('/')
    return st.f_bavail * st.f_frsize / 2**30 >= 7


def verify(unit, args, log):
    load_wait()
    if not disk_ok():
        raise SystemExit('SSD below 7 GB free: stop')
    # result cache on again (2026-11-17): vcache's keys cover the candidate closures of the spec's `t60cand`
    # callees (vcache_t60cand_test.py); had it off after a stale bflower row
    env = dict(os.environ, WWHD_VERIFY_SCRATCH=SCRATCH)
    os.makedirs(SCRATCH, exist_ok=True)
    cmd = ['python3', 'tools/verify/verify.py', unit] + args
    with open(log, 'w') as fh:
        fh.write('$ ' + ' '.join(cmd) + '\n')
        fh.flush()
        r = subprocess.run(cmd, cwd=ROOT, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        out = r.stdout[-5_000_000:]
        fh.write(out)
        fh.write('\nexit %d\n' % r.returncode)
    return r.returncode, out


ROW = re.compile(r'^([0-9A-F]{8})\s+(\S.*?)\s+(\d+)/(\d+)\s+(\d+)/(\d+)\s+(\d+)/(\d+)\s+(.*)$')


def rows(out):
    res = {}
    for l in out.split('\n'):
        m = ROW.match(l.strip())
        if m:
            res[m.group(1)] = dict(name=m.group(2), ok=int(m.group(3)), n=int(m.group(4)), verdict=m.group(9).strip())
    return res


def row_ok(r):
    return r['verdict'].startswith('ok')


def subst_srcs(spec, mapping):
    """the unit spec text with every source named on its `src` lines replaced by mapping[source] (a src line may name
    several files, e.g. d_a_ks.txt: d_a_ks.cpp d_a_ks_exec.cpp). Fails loudly when a mapped source is not on a src
    line or a src line names a file the mapping lacks: a negative control / baseline that compiled the converted
    source would pass silently (the d_a_ks finding, 2026-11-18)."""
    out, done = [], set()
    for line in spec.split('\n'):
        m = re.match(r'^(src\s+)(.*)$', line)
        if m:
            body, _, cmt = m.group(2).partition('#')
            toks = body.split()
            miss = [t for t in toks if t not in mapping]
            if miss:
                raise SystemExit('t60conv: src line names %s with no substitute: %s' % (' '.join(miss), line))
            done.update(toks)
            line = m.group(1) + ' '.join(mapping[t] for t in toks) + ((' #' + cmt) if cmt else '')
        out.append(line)
    left = sorted(set(mapping) - done)
    if left:
        raise SystemExit('t60conv: source(s) %s not on a src line of the spec' % ' '.join(left))
    return '\n'.join(out)


def negctrl_unit(unit, rep):
    """a T-02 spec over copies of every source of the unit built with T60C_NEGCTRL (every helper in the per-step
    original form)"""
    nc_dir = os.path.join(ROOT, 'build', 't60conv')
    os.makedirs(nc_dir, exist_ok=True)
    mapping = {}
    for sp in unit_srcs(unit):
        nc_src = os.path.join(nc_dir, os.path.basename(sp))
        open(nc_src, 'w').write('#define T60C_NEGCTRL 1 /* t60conv negative control */\n' + open(os.path.join(ROOT, sp)).read())
        mapping[sp] = 'build/t60conv/' + os.path.basename(sp)
    spec = subst_srcs(open(os.path.join(UNITS, unit + '.txt')).read(), mapping)
    t02 = open(os.path.join(UNITS, '_t60', unit + '.txt')).read().split('\n')
    t02 = [l for l in t02 if not l.startswith('include ')]
    name = '_t60/%s_t60nc' % unit
    open(os.path.join(UNITS, name + '.txt'), 'w').write(
        '# GENERATED by tools/t60conv (negative control, not for commit)\n' + spec + '\n' + '\n'.join(t02) + '\n')
    return name


def base_unit(unit, rep):
    """a unit over the git HEAD copies of every source (generated, removed after its run)"""
    d = os.path.join(ROOT, 'build', 't60conv')
    os.makedirs(d, exist_ok=True)
    mapping = {}
    for sp in unit_srcs(unit):
        text = subprocess.run(['git', 'show', 'HEAD:' + sp], cwd=ROOT, capture_output=True, text=True, check=True).stdout
        open(os.path.join(d, 'base_' + os.path.basename(sp)), 'w').write(text)
        mapping[sp] = 'build/t60conv/base_' + os.path.basename(sp)
    spec = subst_srcs(open(os.path.join(UNITS, unit + '.txt')).read(), mapping)
    name = '_t60/%s_t60base' % unit
    open(os.path.join(UNITS, name + '.txt'), 'w').write('# GENERATED by tools/t60conv (unconverted baseline, not for commit)\n' + spec)
    return name


BYTEDIFF = re.compile(r't60 N=\d+:\s+byte (\S+) one step')
DIFF = re.compile(r't60 N=(\d+): (\S+) (f32|f64) at \S+: one step (\S+), \d+ steps (\S+)')


# t60c::mark users (the T-02 frame-tag finding, 2026-11-18): the harness keeps one frame tag across generated
# inputs (and a 30 Hz frame may set a mark on any step), so every (site, key) that t60c::mark sets must be cleared by a
# t60c::unmark of the same site and key under a first-step gate (`if (t60c::first())` / `t60::first_step()`), else a
# mark of an earlier frame / input carries into the next one
MARKCALL = re.compile(r't60c::(mark|unmark|marked)\s*\(\s*(\d+)\s*,\s*([^;]*?)\)\s*[;)&|]')
FIRSTGATE = re.compile(r't60c::first\(\)|first_step\(\)')


def _match_back(t, i, close, open_):
    """index of the bracket open_ that matches the close at t[i] (scanning back), or -1"""
    d = 0
    while i >= 0:
        if t[i] == close:
            d += 1
        elif t[i] == open_:
            d -= 1
            if d == 0:
                return i
        i -= 1
    return -1


def _if_cond_before(t, j):
    """the condition of an `if (COND)` whose closing paren is the last non-blank character before t[j], else None"""
    k = j - 1
    while k >= 0 and t[k].isspace():
        k -= 1
    if k < 0 or t[k] != ')':
        return None
    o = _match_back(t, k, ')', '(')
    if o < 0:
        return None
    m = re.search(r'(?:^|[^\w])(else\s+)?if\s*$', t[:o])
    if not m or m.group(1):
        return None
    return t[o + 1:k]


def _unmark_gate(t, i):
    """the first-step gate controlling the statement at t[i]: the if (COND) right before it, or the if (COND) { block
    it sits in; '' when none (an unconditional or else-branch statement)"""
    c = _if_cond_before(t, i)
    if c is not None:
        return c
    o, d, k = -1, 0, i - 1
    while k >= 0:
        if t[k] == '}':
            d += 1
        elif t[k] == '{':
            if d == 0:
                o = k
                break
            d -= 1
        k -= 1
    if o < 0:
        return ''
    c = _if_cond_before(t, o)
    return c if c is not None else ''


def mark_lint(texts):
    """problems of the t60c::mark users in the given source texts (unit source + its _t60.h): a (site, key) marked
    but never unmarked under a first-step gate"""
    marks, cleared, gated_bad = {}, set(), []
    for name, raw in texts:
        t = '\n'.join(strip_code(raw.split('\n')))
        for m in MARKCALL.finditer(t):
            kind, site, key = m.group(1), m.group(2), re.sub(r'\s+', '', m.group(3))
            ln = t.count('\n', 0, m.start()) + 1
            if kind == 'mark':
                marks.setdefault((site, key), '%s:%d' % (name, ln))
            elif kind == 'unmark':
                g = _unmark_gate(t, m.start())
                if FIRSTGATE.search(g) and '||' not in g:
                    cleared.add((site, key))
                else:
                    gated_bad.append('%s:%d t60c::unmark(%s, %s) is not under a first-step gate' % (name, ln, site, key))
    out = ['%s t60c::mark(%s, %s) has no t60c::unmark(%s, %s) under a first-step gate (the mark carries into the next '
           'frame / T-02 input)' % (where, s, k, s, k) for (s, k), where in sorted(marks.items()) if (s, k) not in cleared]
    return out + [b for b in gated_bad if marks]


def mark_lint_unit(src):
    """mark_lint over a unit source (path under ROOT or absolute) and its include/d/actor/<stem>_t60.h"""
    paths = [src if os.path.isabs(src) else os.path.join(ROOT, src)]
    h = os.path.join(ROOT, 'wwhd_src', 'include', 'd', 'actor', actor_stem(src) + '_t60.h')
    if os.path.exists(h):
        paths.append(h)
    return mark_lint([(os.path.relpath(p, ROOT), open(p, errors='replace').read()) for p in paths])


def check(unit, n=10000, seeds=(1, 7), shards=4, rep=None):
    od = os.path.join(OUT, unit)
    rep = rep or json.load(open(os.path.join(od, 'convert.json')))
    addrs = rep['addrs']
    if not addrs:
        return dict(verdict='no execute-path functions with an address')
    if os.environ.get('T60CONV_CLIB', '1') != '0':
        ensure_clib_real(unit, rep)
    if os.environ.get('T60CONV_CALLS', '1') != '0':
        ensure_calls_line(unit, rep)
        if os.path.exists(os.path.join(UNITS, '_t60', unit + '.txt')):
            ensure_whole_calls(unit, rep)  # whole-form sources: every call of the execute but its picture
    only = ','.join(addrs)
    base = ['-n', str(n), '-nulltrap', '-shards', str(shards), '-only', only]
    # T-01 also covers the draw functions with draw-pass sites (t60_draw.h; T-02 runs the execute path only)
    only01 = ','.join(sorted(set(addrs) | set(rep.get('draw_addrs') or [])))
    base01 = base[:-1] + [only01]
    res = dict(t01={}, t02={}, nc={}, tol_added=[])
    res['mark_lint'] = mark_lint_unit(rep['src'])
    res['known_fail'] = known_fail(unit)
    t0 = time.time()
    for s in seeds:
        rc, out = verify(unit, base01 + ['-seed', str(s)], os.path.join(od, 't01_s%d.log' % s))
        res['t01'][s] = rows(out)
    bad01 = sorted({a for rr in res['t01'].values() for a, r in rr.items() if not row_ok(r)})
    if bad01:
        # the same rows on the unconverted source (git HEAD): a failure there is pre-existing, not the tool's
        bunit = base_unit(unit, rep)
        res['t01_base'] = {}
        for s in seeds:
            rc, out = verify(bunit, ['-n', str(n), '-nulltrap', '-shards', str(shards), '-only', ','.join(bad01), '-seed', str(s)],
                             os.path.join(od, 't01base_s%d.log' % s))
            res['t01_base'][s] = rows(out)
        os.remove(os.path.join(UNITS, bunit + '.txt'))
    tol_rounds = 0
    while rep.get('step_addrs'):
        res['t02'] = {}
        fixes = []
        for s in seeds:
            for N in (2, 4):
                rc, out = verify('_t60/' + unit, base + ['-seed', str(s), '-t60steps', str(N)], os.path.join(od, 't02_s%d_n%d.log' % (s, N)))
                res['t02']['s%d_n%d' % (s, N)] = rows(out)
                for m in DIFF.finditer(out):
                    fixes.append(m.groups())
        bad = [(k, a) for k, rr in res['t02'].items() for a, r in rr.items() if not row_ok(r)]
        if not bad or not rep.get('float_sites') or tol_rounds >= 3:
            break
        # tolerance loop: only float fields whose N-step result is within float rounding of the one step
        new = []
        for N, field, typ, one, many in fixes:
            try:
                a, b = float(one), float(many)
            except ValueError:
                continue
            import math
            if math.isfinite(a) and math.isfinite(b) and max(abs(a), abs(b)) >= 1e-3 and \
                    abs(a - b) <= 1e-4 * max(1.0, abs(a), abs(b)):
                line = 't60tol * %s %s absrel 0.0001 0.0001' % (field, typ)
                if line not in new:
                    new.append(line)
        # a byte difference inside a float (no tolerance line yet): propose the aligned word as f32 with a rounding
        # bound (absrel 1e-4 1e-4: only float rounding passes it)
        for m in BYTEDIFF.finditer('\n'.join(open(os.path.join(od, 't02_s%d_n%d.log' % (s_, N_))).read() for s_ in seeds for N_ in (2, 4))):
            fld = m.group(1)
            mm = re.match(r'(r\d+)\+0x([0-9A-Fa-f]+)$', fld)
            if mm:
                fld = '%s+0x%X' % (mm.group(1), int(mm.group(2), 16) & ~3)
            elif re.match(r'^[0-9A-Fa-f]{8}$', fld):
                fld = '%08X' % (int(fld, 16) & ~3)
            else:
                continue
            line = 't60tol * %s f32 abs 0' % fld  # typed: the next round's difference line shows the values
            if line not in new:
                new.append(line)
        spec = os.path.join(UNITS, '_t60', unit + '.txt')
        txt = open(spec).read()
        new = [l for l in new if l not in txt]
        if not new:
            break
        for l in new:  # an absrel line replaces the typed exact probe of the same field
            if l.endswith('absrel 0.0001 0.0001'):
                txt = txt.replace(l.replace('absrel 0.0001 0.0001', 'abs 0') + '\n', '')
        open(spec, 'w').write(txt)
        open(spec, 'a').write('# tool tolerance loop round %d (float rounding of inline integration)\n' % (tol_rounds + 1) + '\n'.join(new) + '\n')
        res['tol_added'] += new
        tol_rounds += 1
    # functions that run changed code under their callers (t60cand) but are on no spec's t60conv line: never N-step
    # compared (verify.py's T-02 warning)
    res['uncompared'] = sorted({m for s_ in seeds for N_ in (2, 4)
                                if os.path.exists(os.path.join(od, 't02_s%d_n%d.log' % (s_, N_)))
                                for m in re.findall(r't60cand ([0-9A-F]{8}) runs changed code but is on no spec',
                                                    open(os.path.join(od, 't02_s%d_n%d.log' % (s_, N_))).read())})
    # negative control: only when the source changed
    if rep.get('changed_exec', rep.get('changed')) and rep.get('step_addrs'):
        nc = negctrl_unit(unit, rep)
        for s in seeds[:1]:
            for N in (2, 4):
                rc, out = verify(nc, base[:1] + [str(min(n, 2000))] + base[2:] + ['-seed', str(s), '-t60steps', str(N)],
                                 os.path.join(od, 'nc_s%d_n%d.log' % (s, N)))
                res['nc']['s%d_n%d' % (s, N)] = rows(out)
        os.remove(os.path.join(UNITS, nc + '.txt'))
    res['seconds'] = round(time.time() - t0)
    res['verdict'] = verdict(rep, res)
    json.dump(res, open(os.path.join(od, 'check.json'), 'w'), indent=1, default=str)
    write_report(unit, rep, res)
    return res


def verdict(rep, res):
    t01 = all(row_ok(r) for rr in res['t01'].values() for r in rr.values()) and all(res['t01'].values())
    t02 = all(row_ok(r) for rr in res['t02'].values() for r in rr.values()) and all(res['t02'].values())
    # the negative control must fail on at least one converted function that has an auto site
    conv_addrs = {s['addr'] for s in rep['sites'] if s['after'] and s['cat'] != 'draw'}
    nc_fail = None
    changed = rep.get('changed_exec', rep.get('changed'))
    if changed:
        # the negative control differs from the conversion only in the helper forms, so a failing row anywhere in it
        # (a converted function, or a caller that runs converted callees through t60cand) shows the forms are exercised
        nc_fail = any(not row_ok(r) for rr in res['nc'].values() for a, r in rr.items()
                      if a in conv_addrs or a in rep.get('step_addrs', []))
    review = any(f['cls'] in ('needs review', 'not convertible') for f in rep['functions'].values())
    review = review or any(s['cat'] == 'draw' and s['cls'] != 'auto' for s in rep['sites'])
    if res.get('mark_lint'):
        return 'FAIL MARK (%d): %s' % (len(res['mark_lint']), res['mark_lint'][0])
    if not t01:
        base = res.get('t01_base', {})
        pre = base and all(not row_ok(base[s][a]) and base[s][a]['ok'] == res['t01'][s][a]['ok']
                           for s in res['t01'] for a, r in res['t01'][s].items() if not row_ok(r) and a in base.get(s, {}))
        if pre:
            return 'PRE-EXISTING T-01 failure (the unconverted source fails the same rows); T-02: %s' % ('pass' if t02 else 'FAIL')
        return 'FAIL T-01 (dt = 1 path differs: tool bug)'
    if not rep.get('step_addrs'):
        return 'PASS (no function runs at N > 1: event-only / per-event code; T-01 only)' + (', REVIEW items open' if review else '')
    if not t02:
        kf = res.get('known_fail')
        if kf:
            return 'KNOWN FAIL clib-real (%s): T-02 differs with the c_lib eases real (the spec\'s known-fail line)' % kf
        return 'FAIL T-02 (per-frame work left or a wrong form: see the first difference)'
    if res.get('uncompared'):
        return ('INCOMPLETE T-02: t60cand %s run changed code but no spec compares their N steps (put them on the '
                't60conv line, or `# t60once ADDR: why` for a once-per-call init)' % ' '.join(res['uncompared']))
    if changed and not nc_fail and rep.get('whole'):
        return 'FAIL WHOLE NC: the negative control passes on a whole-form unit (add the calls it repeats to t60calls, or inputs that reach them)'
    if changed and not nc_fail and all(x['cat'] in ('snd', 'ptc') for x in rep['sites'] if x['after'] and x['cat'] != 'draw'):
        return 'PASS (auto; sound / particle gates only: T-02 compares memory, not mock calls, so the gate is proven by its form)' + (', REVIEW items open' if review else '')
    if changed and not nc_fail:
        return 'WEAK (T-01/T-02 pass; the negative control passes too: the changed sites act through calls (T-02 compares memory only) or the inputs do not reach them)'
    if review:
        return 'PASS function checks, REVIEW items open'
    return 'PASS (auto)'


def write_report(unit, rep, res):
    od = os.path.join(OUT, unit)
    L = ['# t60conv report: %s (%s)' % (unit, rep['src']), '', '**Verdict:** %s  ' % res['verdict'],
         'Check time: %s s. Logs: %s' % (res.get('seconds'), od), '',
         '## Functions (execute path)', '', '| function | addr | class | sites | open items |', '|---|---|---|---:|---|']
    for name, v in rep['functions'].items():
        L.append('| %s | %s | %s | %d | %s |' % (name, v['addr'] or '-', v['cls'], v['sites'], '; '.join(v['reasons'])))
    L += ['', '## Sites', '', '| line | function | cat | class | rule | note |', '|---:|---|---|---|---|---|']
    for s in rep['sites']:
        L.append('| %d | %s | %s | %s | %s | %s |' % (s['line'], s['func'].split('::')[-1], s['cat'], s['cls'], s['rule'], s['reason']))
    L += ['', '## Checks', '']
    for kind in ('t01', 't02', 'nc'):
        for k, rr in res.get(kind, {}).items():
            for a, r in rr.items():
                L.append('- %s %s %s %s: %d/%d %s' % (kind.upper(), k, a, r['name'], r['ok'], r['n'], r['verdict'][:160]))
    if rep.get('effect_calls') or rep.get('effect_unresolved'):
        L += ['', 'T-02 call count (`t60calls`): %s; effect names not resolved (add by hand): %s' % (
            ' '.join(rep.get('effect_calls') or []) or 'none', ', '.join(rep.get('effect_unresolved') or []) or 'none')]
    if res.get('tol_added'):
        L += ['', 'Tolerance lines added by the tool loop:', ''] + ['- `%s`' % t for t in res['tol_added']]
    if res.get('mark_lint'):
        L += ['', 't60c::mark check (every mark cleared on the first step):', ''] + ['- %s' % t for t in res['mark_lint']]
    open(os.path.join(od, 'REPORT.md'), 'w').write('\n'.join(L) + '\n')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['convert', 'check', 'run', 'scan', 'batch', 'draw', 'marks', 'whole', 'wholecalls', 'drawwrites'])
    ap.add_argument('unit', nargs='+')
    ap.add_argument('--src')
    ap.add_argument('--dry', action='store_true')
    ap.add_argument('--n', type=int, default=10000)
    ap.add_argument('--seeds', default='1,7')
    a = ap.parse_args()
    seeds = tuple(int(x) for x in a.seeds.split(','))
    if a.cmd == 'batch':
        summ = os.path.join(OUT, 'batch.tsv')
        for u in a.unit:
            t0 = time.time()
            try:
                rep = convert(u)
                t1 = time.time()
                res = check(u, a.n, seeds, rep=rep) if rep['addrs'] else dict(verdict='no execute path found')
                v = res['verdict']
            except SystemExit as e:
                v, t1 = 'TOOL STOP: %s' % e, time.time()
            except Exception as e:
                v, t1 = 'TOOL ERROR: %r' % e, time.time()
            with open(summ, 'a') as fh:
                fh.write('%s\t%s\t%d\t%d\t%s\n' % (time.strftime('%H:%M:%S'), u, round(t1 - t0), round(time.time() - t0), v))
        return
    if a.cmd == 'marks':
        # marks FILE|all: the t60c::mark check on sources (paths under the decomp root; `all`: every wwhd_src/d source
        # that uses t60c::mark, with its _t60.h)
        srcs = a.unit
        if srcs == ['all']:
            srcs = sorted(os.path.relpath(os.path.join(dp, f), ROOT)
                          for dp, _, fs in os.walk(os.path.join(ROOT, 'wwhd_src', 'd')) for f in fs if f.endswith('.cpp'))
        bad = 0
        for src in srcs:
            stem = actor_stem(src)
            h = os.path.join(ROOT, 'wwhd_src', 'include', 'd', 'actor', stem + '_t60.h')
            txt = open(os.path.join(ROOT, src), errors='replace').read() + (open(h).read() if os.path.exists(h) else '')
            if 't60c::mark(' not in txt:
                continue
            p = mark_lint_unit(src)
            bad += len(p)
            print('MARKS %s %s' % (src, 'ok' if not p else 'FAIL'))
            for x in p:
                print('  ' + x)
        print('marks: %s' % ('FAIL' if bad else 'PASS'))
        sys.exit(1 if bad else 0)
    if a.cmd == 'draw':
        for u in a.unit:
            try:
                rep, res = draw_unit(u, a.src, dry=a.dry, n=a.n, seeds=seeds)
            except (SystemExit, Exception) as e:
                print('DRAW', u, 'TOOL STOP: %r' % e)
                continue
            for s in rep['sites']:
                print('%5d %-28s %-7s %s | %s' % (s['line'], s['func'][-28:], s['cls'], s['rule'], s['reason']))
            print('DRAW', u, res['verdict'])
        return
    if a.cmd == 'drawwrites':
        # drawwrites UNIT...: the draw path's writes (state ones to review; render / guard ones listed with -v: --dry)
        for u in a.unit:
            for fn, ln, kind, tx in draw_writes(unit_srcs(u)):
                if kind == 'state' or a.dry:
                    print('DRAWWRITE %s %s:%d %s | %s' % (u, fn, ln, kind, tx))
            for fn, ln, c, tx in draw_rnd(unit_srcs(u)):
                print('DRAWRND %s %s:%d %s | %s' % (u, fn, ln, c, tx))
        return
    if a.cmd == 'wholecalls':
        # wholecalls SPEC...: print the whole-form call addresses (no spec change)
        for u in a.unit:
            srcs = unit_srcs(u)
            for fn, ad, pic in whole_calls(srcs, whole_header(srcs)):
                print('WHOLE %s %s: t60calls %s (picture, left out: %s)' % (u, fn, ' '.join(ad), ' '.join(pic) or '-'))
        return
    if a.cmd == 'whole':
        # whole T02SPEC...: the whole-form gate (t60calls line, T-02, negative control must fail)
        bad = 0
        for u in a.unit:
            res = whole_check(u, n=min(a.n, 2000), seed=seeds[0])
            print('WHOLE', u, res['verdict'])
            bad += not res['verdict'].startswith(('PASS', 'NOT WHOLE'))
        sys.exit(1 if bad else 0)
    a.unit = a.unit[0]
    if a.cmd in ('convert', 'scan', 'run'):
        rep = convert(a.unit, a.src, dry=(a.dry or a.cmd == 'scan'))
        for s in rep['sites']:
            print('%5d %-28s %-5s %-7s %s | %s' % (s['line'], s['func'][-28:], s['cat'], s['cls'], s['rule'], s['reason']))
        for name, v in rep['functions'].items():
            print('FUNC %-40s %s %s' % (name, v['addr'], v['cls']))
    if a.cmd in ('check', 'run'):
        res = check(a.unit, a.n, seeds)
        print('VERDICT', a.unit, res['verdict'], '(%s s)' % res.get('seconds'))


if __name__ == '__main__':
    main()
