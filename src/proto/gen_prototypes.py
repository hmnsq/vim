#!/usr/bin/env python3
#
# gen_prototypes.py : Generate function prototypes (.pro files) for Vim source
#
# This script scans C source files, extracts non-static function definitions
# using libclang, and writes corresponding prototypes to files under proto/.
# It is intended to be run via `make proto` in the Vim source directory.
#
# The following specifications are used for processing.
# 1. Preprocessor directives in C files are selected and discarded based on
#    the following criteria:
#    - Standalone `#if 0`, `#ifdef _DEBUG`, `#ifndef PROTO`, and
#      `#if !defined(PROTO)` are treated as false, and the block is discarded.
#      (If a block like `#else` exists, that block is adopted.)
#    - If the condition of `#if <expr>` includes `defined(PROTO)`, it is
#      evaluated, and if it is true, that block is adopted.
#      (If it is false, if a block like `#else` exists, that block is adopted)
#    - Other `#if <expr>` and `#if !<expr>` are treated as true, and that
#      block is adopted.
# 2. The above results are passed to libclang for AST analysis.
#    - `#include` does not result in an error even if the file cannot be found.
#      (The include file search path specifies only `.`.)
#    - Generates a prototype declaration file (proto/*.pro) based on
#      non-static function definition information.
#
# Notes:
# - Execute `make proto` only after confirming that the build was successful
#   with `make`.
#
# Author: Hirohito Higashi (@h-east)
# Copyright: Vim license applies, see ":help license"
# Last Change: 2025 Oct 08
#
استورد os
استورد re
استورد sys
من pathlib استورد رستو_2
من typing استورد List, Tuple
من clang.cindex استورد اشاريو_3, Cursor_قسم, StorageClass, Token_قسم

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") ليس في ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") ليس في ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

وصف پڇاڙيون_with_bs(سٽ: str) -> bool:
    أرجع سٽ.rstrip("\r\n").rstrip().endswith("\\")

وصف _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    لكل a في argv:
        إذا a == "-DPROTO" أو a.startswith("-DPROTO="):
            أرجع سچ
    أرجع خطأ

وصف _eval_condition_with_defined_proto(cond: str, proto_is_defined: bool) -> bool:
    """
    Evaluate a simple C preprocessor condition only when it contains
    defined(PROTO).
    - Replace defined(PROTO)/!defined(PROTO) based on proto_is_defined
    - Map C logical operators (&&, ||, !) to Python (and, or, not)
    - Treat unknown identifiers as 0 (false)
    - Prevent tokens like 'True(' / 'False(' / '0(' turning into call syntax
    On any parsing error, return True (fail-open -> keep the branch).
    """
    s = cond

    إذا DEBUG_LOG:
        ڇاپيو(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        ڇاپيو(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    إذا proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    وإلا:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    إذا proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    وإلا:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Any other defined(MACRO) should be considered unknown
    s = re.sub(r'!defined\s*\(\s*[A-Za-z_]\w*\s*\)', "True", s)
    s = re.sub(r'defined\s*\(\s*[A-Za-z_]\w*\s*\)', "False", s)

    # Translate C logical operators to Python
    s = s.replace("&&", " and ")
    s = s.replace("||", " or ")
    # Replace '!' with ' not ' but do not touch '!='
    s = re.sub(r'(?<![=!])!', " not ", s)

    # Replace remaining identifiers (macros) with 0 (false)
    # Keep True/False and numbers as-is; also keep Python logical keywords
    s = re.sub(r'\b(?!True\b|False\b|and\b|or\b|not\b)[A-Za-z_]\w*', "0", s)

    # Prevent literals followed by '(' from looking like a call: True(  False(
    # 0(
    s = re.sub(r'\b(True|False|0)\s*\(', r'(\1) and (', s)

    # Safety: collapse excessive whitespace
    s = re.sub(r'\s+', " ", s).strip()

    حاول:
        أرجع bool(eval(s, {}, {}))
    استثناء Exception:
        # Fail-open to avoid accidentally dropping code
        إذا DEBUG_LOG:
            ڇاپيو(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        أرجع سچ

وصف rewrite_conditionals_پهريون_branch(متن: str) -> str:
    """
    Keep only the first branch of #if/#ifdef/#ifndef groups, remove others.
    Extensions:
      - Drop whole group for #if 0.
      - Drop whole group for #ifndef PROTO and #if !defined(PROTO).
      - If an '#if <expr>' contains defined(PROTO) anywhere in the expression,
        evaluate the condition: if False, drop the whole group; if True, keep
        only the first branch (same as legacy behavior).
    """
    سٽون = متن.splitlines(keepends=سچ)
    ٻاهريون: List[str] = []
    i, n = 0, ڊيگهه(سٽون)

    وصف collect_گروهه(شروعات: صحيح) -> Tuple[صحيح, List[Tuple[str, صحيح, صحيح]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = شروعات
        # Skip backslash-continued header lines
        طالما سچ:
            h += 1
            إذا h >= n أو ليس پڇاڙيون_with_bs(سٽون[h - 1]):
                اكسر
        اونهائي = 1
        j = h
        marks: List[Tuple[str, صحيح]] = [("if", شروعات)]
        طالما j < n و اونهائي > 0:
            m = _DIR_RE.ميلاپ(سٽون[j])
            إذا m:
                kw2 = m.گروهه(1)
                إذا kw2 في ("if", "ifdef", "ifndef"):
                    اونهائي += 1
                وإلاإذا kw2 == "endif":
                    اونهائي -= 1
                    إذا اونهائي == 0:
                        اكسر
                وإلاإذا اونهائي == 1 و kw2 في ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        وإلا:
            # Unterminated: keep as-is from start to EOF
            أرجع n, [("text", شروعات, n)]

        وصف after_سرو_سٽ(pos: صحيح) -> صحيح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            طالما k < n و پڇاڙيون_with_bs(سٽون[k - 1]):
                k += 1
            أرجع k

        وصف after_سرو_سٽ(pos: صحيح) -> صحيح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            طالما k < n و پڇاڙيون_with_bs(سٽون[k - 1]):
                k += 1
            أرجع k

        bodies: List[Tuple[str, صحيح, صحيح]] = []
        سرو_positions = [pos لكل _, pos في marks] + [j]
        ٽيگ_فهرست = [ٽيگ لكل ٽيگ, _ في marks]
        لكل idx, ٽيگ في enumerate(ٽيگ_فهرست):
            سرو = سرو_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_سرو_سٽ(سرو)
            ايندڙ_سرو = سرو_positions[idx + 1]
            body_e = ايندڙ_سرو
            bodies.append((ٽيگ, body_s, body_e))
        أرجع j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    طالما i < n:
        سٽ = سٽون[i]
        m = _DIR_RE.ميلاپ(سٽ)
        إذا ليس m:
            ٻاهريون.append(سٽ)
            i += 1
            استمر

        kw = m.گروهه(1)
        إذا kw في ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            إذا _IF0_RE.ميلاپ(سٽ) أو _IFDEF_DEBUG_RE.ميلاپ(سٽ) أو _IFNDEF_PROTO_RE.ميلاپ(سٽ) أو _IF_NOTDEF_PROTO_RE.ميلاپ(سٽ):
                گروهه_پڇاڙي, _ = collect_گروهه(i)
                i = گروهه_پڇاڙي
                استمر

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") و (
                _DEFINED_CALL_RE.ڳولا(سٽ) آهي ليس ڪجهه_نه
            )

            إذا evaluate:
                # Extract condition text after 'if'
                حاول:
                    cond_متن = سٽ.ورهايو("if", 1)[1]
                استثناء Exception:
                    cond_متن = ""
                keep_پهريون = _eval_condition_with_defined_proto(cond_متن, proto_is_defined)
                إذا ليس keep_پهريون:
                    گروهه_پڇاڙي, _ = collect_گروهه(i)
                    i = گروهه_پڇاڙي
                    استمر

            # Keep only the first branch
            گروهه_پڇاڙي, bodies = collect_گروهه(i)
            إذا ليس bodies:
                i = گروهه_پڇاڙي
                استمر
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_متن = "".join(سٽون[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_پهريون_branch(kept_متن)

            إذا DEBUG_LOG:
                ڇاپيو(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                لكل ln في kept_متن.splitlines():
                    إذا ln.strip():
                        ڇاپيو(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        اكسر

            ٻاهريون.append(kept_rewritten)
            i = گروهه_پڇاڙي
        وإلا:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            ٻاهريون.append("\n" إذا سٽ.endswith("\n") وإلا "")
            i += 1

    أرجع "".join(ٻاهريون)

وصف collapse_تارو_spaces(s: str) -> str:
    أرجع re.sub(r"\*\s+", "*", s)

وصف fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    أرجع s

وصف strip_اڻ_استعمال_ٿيل_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    أرجع s

وصف normalize_ws(s: str) -> str:
    أرجع re.sub(r"\s+", " ", s).strip()

وصف ٽوڪن_for_function_سرو(cur) -> List[str]:
    toks = [t لكل t في cur.get_ٽوڪن() إذا t.قسم != Token_قسم.COMMENT]
    إذا ليس toks:
        أرجع []
    name_idx = ڪجهه_نه
    لكل i, t في enumerate(toks):
        إذا t.قسم == Token_قسم.IDENTIFIER و t.spelling == cur.spelling:
            name_idx = i
            اكسر
    إذا name_idx آهي ڪجهه_نه:
        أرجع []
    lpar_idx = ڪجهه_نه
    لكل i في سلسلو(name_idx + 1, ڊيگهه(toks)):
        إذا toks[i].spelling == '(':
            lpar_idx = i
            اكسر
    إذا lpar_idx آهي ڪجهه_نه:
        أرجع []
    اونهائي, rpar_idx = 0, ڪجهه_نه
    لكل i في سلسلو(lpar_idx, ڊيگهه(toks)):
        إذا toks[i].spelling == '(':
            اونهائي += 1
        وإلاإذا toks[i].spelling == ')':
            اونهائي -= 1
            إذا اونهائي == 0:
                rpar_idx = i
                اكسر
    إذا rpar_idx آهي ڪجهه_نه:
        أرجع []
    أرجع [t.spelling لكل t في toks[:rpar_idx + 1]]

وصف join_ٽوڪن(ٽوڪن: List[str]) -> str:
    s = " ".join(ٽوڪن)
    s = strip_اڻ_استعمال_ٿيل_macros(s)
    s = normalize_ws(s)
    s = collapse_تارو_spaces(s)
    s = fix_spacing(s)
    أرجع s

وصف سرو_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    حاول:
        ret = getattr(cur, "result_type", ڪجهه_نه)
        ret_sp = ret.spelling إذا ret آهي ليس ڪجهه_نه وإلا ""
    استثناء Exception:
        ret_sp = ""
    إذا ليس ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        حاول:
            ret_sp = cur.type.get_نتيجو().spelling
        استثناء Exception:
            ret_sp = "void"
    name_sp = cur.spelling أو ""
    params = []
    حاول:
        لكل a في cur.get_arguments() أو []:
            t = getattr(a, "type", ڪجهه_نه)
            t_sp = t.spelling إذا t آهي ليس ڪجهه_نه وإلا ""
            a_sp = a.spelling أو ""
            seg = (t_sp + (" " + a_sp إذا a_sp وإلا "")).strip()
            params.append(seg إذا seg وإلا "void")
    استثناء Exception:
        لنگهه
    params_s = ", ".join(params) إذا params وإلا "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_اڻ_استعمال_ٿيل_macros(s)
    s = normalize_ws(s)
    s = collapse_تارو_spaces(s)
    s = fix_spacing(s)
    أرجع s

وصف اندريون_this_file(cur, src_رستو: رستو_2) -> bool:
    حاول:
        loc = cur.extent.شروعات
        f = (loc.file أو cur.location.file)
        إذا ليس f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            حاول:
                toks = ٽوڪن_for_function_سرو(cur)
                أرجع bool(toks)
            استثناء Exception:
                أرجع خطأ
        cur_رستو = os.رستو.normcase(os.رستو.abspath(str(f.name)))
        src_abs  = os.رستو.normcase(os.رستو.abspath(src_رستو.as_posix()))
        # Accept the main source file
        إذا cur_رستو == src_abs أو os.رستو.basename(cur_رستو) == os.رستو.basename(src_abs):
            أرجع سچ
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.رستو.normcase(os.رستو.abspath(src_رستو.والد.as_posix()))
        إذا cur_رستو.endswith(".c") و os.رستو.dirname(cur_رستو) == src_dir:
            أرجع سچ
        أرجع خطأ
    استثناء Exception:
        أرجع خطأ

وصف collect_function_defs(tu, src_رستو: رستو_2):
    وصف walk(ڳنڍ, اونهائي=0):
        إذا DEBUG_LOG:
            ڇاپيو(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        إذا ڳنڍ.قسم == Cursor_قسم.FUNCTION_DECL:
            إذا اندريون_this_file(ڳنڍ, src_رستو):
                إذا ڳنڍ.storage_class != StorageClass.STATIC و ڳنڍ.is_definition():
                    أنتج ڳنڍ
        لكل ch في ڳنڍ.get_children():
            أنتج من walk(ch, اونهائي+1)
    أنتج من walk(tu.cursor)

وصف parse_with_clang(src_رستو: رستو_2, متن: str, دليلون: List[str]):
    اشاريو = اشاريو_3.create()
    src_abs = os.رستو.abspath(src_رستو.as_posix())
    unsaved = [(src_abs, متن)]
    proto_h = src_رستو.والد / "proto.h"
    إذا proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_دليلون = (دليلون أو []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = اشاريو.parse(src_abs, دليلون=final_دليلون, unsaved_files=unsaved, options=0)
    إذا SHOW_DIAGS:
        لكل d في getattr(tu, "diagnostics", []):
            ڇاپيو(d, file=sys.stderr)
    أرجع tu

وصف generate_prototypes(tu, src_رستو: رستو_2) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    لكل fn في collect_function_defs(tu, src_رستو):
        إذا DEBUG_LOG:
            حاول:
                sr = fn.extent.شروعات
                er = fn.extent.پڇاڙي
                ڇاپيو(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            استثناء Exception:
                ڇاپيو(f"[def] {fn.spelling}", file=sys.stderr)
        toks = ٽوڪن_for_function_سرو(fn)
        سرو = join_ٽوڪن(toks) إذا toks وإلا سرو_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        سرو = re.sub(r'\b_Bool\b', 'bool', سرو)
        إذا سرو:
            protos.append(f"{header};")

    ڏٺل, unique = سيٽ_2(), []
    لكل p في protos:
        إذا p ليس في ڏٺل:
            unique.append(p)
            ڏٺل.جوڙيو(p)
    أرجع unique

وصف write_prototypes(ٻاهريون_رستو: رستو_2, سرا: List[str], src_name: str) -> ڪجهه_نه:
    """Write collected prototypes to the output .pro file."""
    ٻاهريون_رستو.والد.mkdir(parents=سچ, exist_ok=سچ)
    حاول:
        سان ٻاهريون_رستو.open("w", encoding="utf-8", newline="\n") كـ f:
            f.اكتب(f"// {src_name}\n")
            لكل h في سرا:
                f.اكتب(h + "\n")
            f.اكتب("// vim: ft=c\n")
    استثناء Exception كـ e:
        ڇاپيو(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    ڇاپيو(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

وصف مکيه():
    دليلون = sys.argv[1:]
    إذا ليس دليلون:
        ڇاپيو("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_رستو = رستو_2(دليلون[0])
    clang_دليلون = دليلون[1:]
    إذا ليس src_رستو.exists():
        ڇاپيو(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    حاول:
        original = src_رستو.read_متن(encoding="utf-8", errors="ignore")
    استثناء Exception كـ e:
        ڇاپيو(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_پهريون_branch(original)

    # Parse with libclang
    حاول:
        tu = parse_with_clang(src_رستو, rewritten, clang_دليلون)
    استثناء Exception كـ e:
        ڇاپيو(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    سرا = generate_prototypes(tu, src_رستو)
    ٻاهريون_file = رستو_2("proto") / (src_رستو.stem + ".pro")
    write_prototypes(ٻاهريون_file, سرا, src_رستو.name)

إذا __name__ == "__main__":
    مکيه()
