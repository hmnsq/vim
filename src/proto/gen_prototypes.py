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
من pathlib استورد لار_2
من typing استورد List, Tuple
من clang.cindex استورد شاخص_3, Cursor_ډول, StorageClass, Token_ډول

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

تعریف پایونه_with_bs(کرښه: str) -> bool:
    أرجع کرښه.rstrip("\r\n").rstrip().endswith("\\")

تعریف _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    لكل a في argv:
        إذا a == "-DPROTO" أو a.startswith("-DPROTO="):
            أرجع ریښتیا
    أرجع خطأ

تعریف _eval_condition_with_defined_proto(cond: str, proto_is_defined: bool) -> bool:
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
        چاپ(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        چاپ(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

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
            چاپ(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        أرجع ریښتیا

تعریف rewrite_conditionals_لومړی_branch(متن: str) -> str:
    """
    Keep only the first branch of #if/#ifdef/#ifndef groups, remove others.
    Extensions:
      - Drop whole group for #if 0.
      - Drop whole group for #ifndef PROTO and #if !defined(PROTO).
      - If an '#if <expr>' contains defined(PROTO) anywhere in the expression,
        evaluate the condition: if False, drop the whole group; if True, keep
        only the first branch (same as legacy behavior).
    """
    کرښې = متن.splitlines(keepends=ریښتیا)
    بهر: List[str] = []
    i, n = 0, اوږدوالی(کرښې)

    تعریف collect_ډله(پیل: صحيح) -> Tuple[صحيح, List[Tuple[str, صحيح, صحيح]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = پیل
        # Skip backslash-continued header lines
        طالما ریښتیا:
            h += 1
            إذا h >= n أو ليس پایونه_with_bs(کرښې[h - 1]):
                اكسر
        ژوروالی = 1
        j = h
        marks: List[Tuple[str, صحيح]] = [("if", پیل)]
        طالما j < n و ژوروالی > 0:
            m = _DIR_RE.سمون(کرښې[j])
            إذا m:
                kw2 = m.ډله(1)
                إذا kw2 في ("if", "ifdef", "ifndef"):
                    ژوروالی += 1
                وإلاإذا kw2 == "endif":
                    ژوروالی -= 1
                    إذا ژوروالی == 0:
                        اكسر
                وإلاإذا ژوروالی == 1 و kw2 في ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        وإلا:
            # Unterminated: keep as-is from start to EOF
            أرجع n, [("text", پیل, n)]

        تعریف after_سرلیک_کرښه(pos: صحيح) -> صحيح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            طالما k < n و پایونه_with_bs(کرښې[k - 1]):
                k += 1
            أرجع k

        تعریف after_سرلیک_کرښه(pos: صحيح) -> صحيح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            طالما k < n و پایونه_with_bs(کرښې[k - 1]):
                k += 1
            أرجع k

        bodies: List[Tuple[str, صحيح, صحيح]] = []
        سرلیک_positions = [pos لكل _, pos في marks] + [j]
        نښانونه = [نښان لكل نښان, _ في marks]
        لكل idx, نښان في enumerate(نښانونه):
            سرلیک = سرلیک_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_سرلیک_کرښه(سرلیک)
            بل_سرلیک = سرلیک_positions[idx + 1]
            body_e = بل_سرلیک
            bodies.append((نښان, body_s, body_e))
        أرجع j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    طالما i < n:
        کرښه = کرښې[i]
        m = _DIR_RE.سمون(کرښه)
        إذا ليس m:
            بهر.append(کرښه)
            i += 1
            استمر

        kw = m.ډله(1)
        إذا kw في ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            إذا _IF0_RE.سمون(کرښه) أو _IFDEF_DEBUG_RE.سمون(کرښه) أو _IFNDEF_PROTO_RE.سمون(کرښه) أو _IF_NOTDEF_PROTO_RE.سمون(کرښه):
                ډله_پای_ټکی, _ = collect_ډله(i)
                i = ډله_پای_ټکی
                استمر

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") و (
                _DEFINED_CALL_RE.پلټنه(کرښه) دی ليس هیڅ
            )

            إذا evaluate:
                # Extract condition text after 'if'
                حاول:
                    cond_متن = کرښه.وېشل("if", 1)[1]
                استثناء Exception:
                    cond_متن = ""
                keep_لومړی = _eval_condition_with_defined_proto(cond_متن, proto_is_defined)
                إذا ليس keep_لومړی:
                    ډله_پای_ټکی, _ = collect_ډله(i)
                    i = ډله_پای_ټکی
                    استمر

            # Keep only the first branch
            ډله_پای_ټکی, bodies = collect_ډله(i)
            إذا ليس bodies:
                i = ډله_پای_ټکی
                استمر
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_متن = "".join(کرښې[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_لومړی_branch(kept_متن)

            إذا DEBUG_LOG:
                چاپ(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                لكل ln في kept_متن.splitlines():
                    إذا ln.strip():
                        چاپ(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        اكسر

            بهر.append(kept_rewritten)
            i = ډله_پای_ټکی
        وإلا:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            بهر.append("\n" إذا کرښه.endswith("\n") وإلا "")
            i += 1

    أرجع "".join(بهر)

تعریف collapse_ستوری_spaces(s: str) -> str:
    أرجع re.sub(r"\*\s+", "*", s)

تعریف fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    أرجع s

تعریف strip_ناکارول_شوی_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    أرجع s

تعریف normalize_ws(s: str) -> str:
    أرجع re.sub(r"\s+", " ", s).strip()

تعریف نښې_for_function_سرلیک(cur) -> List[str]:
    toks = [t لكل t في cur.get_نښې() إذا t.ډول != Token_ډول.COMMENT]
    إذا ليس toks:
        أرجع []
    name_idx = هیڅ
    لكل i, t في enumerate(toks):
        إذا t.ډول == Token_ډول.IDENTIFIER و t.spelling == cur.spelling:
            name_idx = i
            اكسر
    إذا name_idx دی هیڅ:
        أرجع []
    lpar_idx = هیڅ
    لكل i في لړ(name_idx + 1, اوږدوالی(toks)):
        إذا toks[i].spelling == '(':
            lpar_idx = i
            اكسر
    إذا lpar_idx دی هیڅ:
        أرجع []
    ژوروالی, rpar_idx = 0, هیڅ
    لكل i في لړ(lpar_idx, اوږدوالی(toks)):
        إذا toks[i].spelling == '(':
            ژوروالی += 1
        وإلاإذا toks[i].spelling == ')':
            ژوروالی -= 1
            إذا ژوروالی == 0:
                rpar_idx = i
                اكسر
    إذا rpar_idx دی هیڅ:
        أرجع []
    أرجع [t.spelling لكل t في toks[:rpar_idx + 1]]

تعریف join_نښې(نښې: List[str]) -> str:
    s = " ".join(نښې)
    s = strip_ناکارول_شوی_macros(s)
    s = normalize_ws(s)
    s = collapse_ستوری_spaces(s)
    s = fix_spacing(s)
    أرجع s

تعریف سرلیک_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    حاول:
        ret = getattr(cur, "result_type", هیڅ)
        ret_sp = ret.spelling إذا ret دی ليس هیڅ وإلا ""
    استثناء Exception:
        ret_sp = ""
    إذا ليس ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        حاول:
            ret_sp = cur.type.get_پایله().spelling
        استثناء Exception:
            ret_sp = "void"
    name_sp = cur.spelling أو ""
    params = []
    حاول:
        لكل a في cur.get_arguments() أو []:
            t = getattr(a, "type", هیڅ)
            t_sp = t.spelling إذا t دی ليس هیڅ وإلا ""
            a_sp = a.spelling أو ""
            seg = (t_sp + (" " + a_sp إذا a_sp وإلا "")).strip()
            params.append(seg إذا seg وإلا "void")
    استثناء Exception:
        تیرشه
    params_s = ", ".join(params) إذا params وإلا "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_ناکارول_شوی_macros(s)
    s = normalize_ws(s)
    s = collapse_ستوری_spaces(s)
    s = fix_spacing(s)
    أرجع s

تعریف دننه_this_file(cur, src_لار: لار_2) -> bool:
    حاول:
        loc = cur.extent.پیل
        f = (loc.file أو cur.location.file)
        إذا ليس f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            حاول:
                toks = نښې_for_function_سرلیک(cur)
                أرجع bool(toks)
            استثناء Exception:
                أرجع خطأ
        cur_لار = os.لار.normcase(os.لار.abspath(str(f.name)))
        src_abs  = os.لار.normcase(os.لار.abspath(src_لار.as_posix()))
        # Accept the main source file
        إذا cur_لار == src_abs أو os.لار.basename(cur_لار) == os.لار.basename(src_abs):
            أرجع ریښتیا
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.لار.normcase(os.لار.abspath(src_لار.پلار.as_posix()))
        إذا cur_لار.endswith(".c") و os.لار.dirname(cur_لار) == src_dir:
            أرجع ریښتیا
        أرجع خطأ
    استثناء Exception:
        أرجع خطأ

تعریف collect_function_defs(tu, src_لار: لار_2):
    تعریف walk(غوټه, ژوروالی=0):
        إذا DEBUG_LOG:
            چاپ(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        إذا غوټه.ډول == Cursor_ډول.FUNCTION_DECL:
            إذا دننه_this_file(غوټه, src_لار):
                إذا غوټه.storage_class != StorageClass.STATIC و غوټه.is_definition():
                    أنتج غوټه
        لكل ch في غوټه.get_children():
            أنتج من walk(ch, ژوروالی+1)
    أنتج من walk(tu.cursor)

تعریف parse_with_clang(src_لار: لار_2, متن: str, دلیلونه: List[str]):
    شاخص = شاخص_3.create()
    src_abs = os.لار.abspath(src_لار.as_posix())
    unsaved = [(src_abs, متن)]
    proto_h = src_لار.پلار / "proto.h"
    إذا proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_دلیلونه = (دلیلونه أو []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = شاخص.parse(src_abs, دلیلونه=final_دلیلونه, unsaved_files=unsaved, options=0)
    إذا SHOW_DIAGS:
        لكل d في getattr(tu, "diagnostics", []):
            چاپ(d, file=sys.stderr)
    أرجع tu

تعریف generate_prototypes(tu, src_لار: لار_2) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    لكل fn في collect_function_defs(tu, src_لار):
        إذا DEBUG_LOG:
            حاول:
                sr = fn.extent.پیل
                er = fn.extent.پای_ټکی
                چاپ(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            استثناء Exception:
                چاپ(f"[def] {fn.spelling}", file=sys.stderr)
        toks = نښې_for_function_سرلیک(fn)
        سرلیک = join_نښې(toks) إذا toks وإلا سرلیک_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        سرلیک = re.sub(r'\b_Bool\b', 'bool', سرلیک)
        إذا سرلیک:
            protos.append(f"{header};")

    کتل_شوی, unique = سېټ_2(), []
    لكل p في protos:
        إذا p ليس في کتل_شوی:
            unique.append(p)
            کتل_شوی.جمع_کړه(p)
    أرجع unique

تعریف write_prototypes(بهر_لار: لار_2, سرلیکونه: List[str], src_name: str) -> هیڅ:
    """Write collected prototypes to the output .pro file."""
    بهر_لار.پلار.mkdir(parents=ریښتیا, exist_ok=ریښتیا)
    حاول:
        سره بهر_لار.open("w", encoding="utf-8", newline="\n") كـ f:
            f.اكتب(f"// {src_name}\n")
            لكل h في سرلیکونه:
                f.اكتب(h + "\n")
            f.اكتب("// vim: ft=c\n")
    استثناء Exception كـ e:
        چاپ(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    چاپ(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

تعریف اصلي():
    دلیلونه = sys.argv[1:]
    إذا ليس دلیلونه:
        چاپ("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_لار = لار_2(دلیلونه[0])
    clang_دلیلونه = دلیلونه[1:]
    إذا ليس src_لار.exists():
        چاپ(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    حاول:
        original = src_لار.read_متن(encoding="utf-8", errors="ignore")
    استثناء Exception كـ e:
        چاپ(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_لومړی_branch(original)

    # Parse with libclang
    حاول:
        tu = parse_with_clang(src_لار, rewritten, clang_دلیلونه)
    استثناء Exception كـ e:
        چاپ(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    سرلیکونه = generate_prototypes(tu, src_لار)
    بهر_file = لار_2("proto") / (src_لار.stem + ".pro")
    write_prototypes(بهر_file, سرلیکونه, src_لار.name)

إذا __name__ == "__main__":
    اصلي()
