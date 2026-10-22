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
من pathlib استورد رستہ_2
من typing استورد List, Tuple
من clang.cindex استورد اشاریہ_3, Cursor_قسم, StorageClass, Token_قسم

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

تعریف سرے_with_bs(سطر: str) -> bool:
    أرجع سطر.rstrip("\r\n").rstrip().endswith("\\")

تعریف _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    لكل a في argv:
        إذا a == "-DPROTO" أو a.startswith("-DPROTO="):
            أرجع سچ
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
        چھاپو(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        چھاپو(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

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
            چھاپو(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        أرجع سچ

تعریف rewrite_conditionals_پہلا_branch(متن: str) -> str:
    """
    Keep only the first branch of #if/#ifdef/#ifndef groups, remove others.
    Extensions:
      - Drop whole group for #if 0.
      - Drop whole group for #ifndef PROTO and #if !defined(PROTO).
      - If an '#if <expr>' contains defined(PROTO) anywhere in the expression,
        evaluate the condition: if False, drop the whole group; if True, keep
        only the first branch (same as legacy behavior).
    """
    سطریں = متن.splitlines(keepends=سچ)
    باہرلا: List[str] = []
    i, n = 0, لمبائی(سطریں)

    تعریف collect_گروہ(شروع: صحيح) -> Tuple[صحيح, List[Tuple[str, صحيح, صحيح]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = شروع
        # Skip backslash-continued header lines
        طالما سچ:
            h += 1
            إذا h >= n أو ليس سرے_with_bs(سطریں[h - 1]):
                اكسر
        ڈونگھائی = 1
        j = h
        marks: List[Tuple[str, صحيح]] = [("if", شروع)]
        طالما j < n و ڈونگھائی > 0:
            m = _DIR_RE.میل(سطریں[j])
            إذا m:
                kw2 = m.گروہ(1)
                إذا kw2 في ("if", "ifdef", "ifndef"):
                    ڈونگھائی += 1
                وإلاإذا kw2 == "endif":
                    ڈونگھائی -= 1
                    إذا ڈونگھائی == 0:
                        اكسر
                وإلاإذا ڈونگھائی == 1 و kw2 في ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        وإلا:
            # Unterminated: keep as-is from start to EOF
            أرجع n, [("text", شروع, n)]

        تعریف after_سرخی_سطر(pos: صحيح) -> صحيح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            طالما k < n و سرے_with_bs(سطریں[k - 1]):
                k += 1
            أرجع k

        تعریف after_سرخی_سطر(pos: صحيح) -> صحيح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            طالما k < n و سرے_with_bs(سطریں[k - 1]):
                k += 1
            أرجع k

        bodies: List[Tuple[str, صحيح, صحيح]] = []
        سرخی_positions = [pos لكل _, pos في marks] + [j]
        ٹیگ_سوچی = [ٹیگ لكل ٹیگ, _ في marks]
        لكل idx, ٹیگ في enumerate(ٹیگ_سوچی):
            سرخی = سرخی_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_سرخی_سطر(سرخی)
            اگلا_سرخی = سرخی_positions[idx + 1]
            body_e = اگلا_سرخی
            bodies.append((ٹیگ, body_s, body_e))
        أرجع j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    طالما i < n:
        سطر = سطریں[i]
        m = _DIR_RE.میل(سطر)
        إذا ليس m:
            باہرلا.append(سطر)
            i += 1
            استمر

        kw = m.گروہ(1)
        إذا kw في ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            إذا _IF0_RE.میل(سطر) أو _IFDEF_DEBUG_RE.میل(سطر) أو _IFNDEF_PROTO_RE.میل(سطر) أو _IF_NOTDEF_PROTO_RE.میل(سطر):
                گروہ_سرا, _ = collect_گروہ(i)
                i = گروہ_سرا
                استمر

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") و (
                _DEFINED_CALL_RE.تلاش(سطر) اے ليس کجھ_نئیں
            )

            إذا evaluate:
                # Extract condition text after 'if'
                حاول:
                    cond_متن = سطر.ونڈو("if", 1)[1]
                استثناء Exception:
                    cond_متن = ""
                keep_پہلا = _eval_condition_with_defined_proto(cond_متن, proto_is_defined)
                إذا ليس keep_پہلا:
                    گروہ_سرا, _ = collect_گروہ(i)
                    i = گروہ_سرا
                    استمر

            # Keep only the first branch
            گروہ_سرا, bodies = collect_گروہ(i)
            إذا ليس bodies:
                i = گروہ_سرا
                استمر
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_متن = "".join(سطریں[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_پہلا_branch(kept_متن)

            إذا DEBUG_LOG:
                چھاپو(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                لكل ln في kept_متن.splitlines():
                    إذا ln.strip():
                        چھاپو(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        اكسر

            باہرلا.append(kept_rewritten)
            i = گروہ_سرا
        وإلا:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            باہرلا.append("\n" إذا سطر.endswith("\n") وإلا "")
            i += 1

    أرجع "".join(باہرلا)

تعریف collapse_تارا_spaces(s: str) -> str:
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

تعریف strip_بن_ورتیا_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    أرجع s

تعریف normalize_ws(s: str) -> str:
    أرجع re.sub(r"\s+", " ", s).strip()

تعریف ٹوکن_for_function_سرخی(cur) -> List[str]:
    toks = [t لكل t في cur.get_ٹوکن() إذا t.قسم != Token_قسم.COMMENT]
    إذا ليس toks:
        أرجع []
    name_idx = کجھ_نئیں
    لكل i, t في enumerate(toks):
        إذا t.قسم == Token_قسم.IDENTIFIER و t.spelling == cur.spelling:
            name_idx = i
            اكسر
    إذا name_idx اے کجھ_نئیں:
        أرجع []
    lpar_idx = کجھ_نئیں
    لكل i في سلسلہ(name_idx + 1, لمبائی(toks)):
        إذا toks[i].spelling == '(':
            lpar_idx = i
            اكسر
    إذا lpar_idx اے کجھ_نئیں:
        أرجع []
    ڈونگھائی, rpar_idx = 0, کجھ_نئیں
    لكل i في سلسلہ(lpar_idx, لمبائی(toks)):
        إذا toks[i].spelling == '(':
            ڈونگھائی += 1
        وإلاإذا toks[i].spelling == ')':
            ڈونگھائی -= 1
            إذا ڈونگھائی == 0:
                rpar_idx = i
                اكسر
    إذا rpar_idx اے کجھ_نئیں:
        أرجع []
    أرجع [t.spelling لكل t في toks[:rpar_idx + 1]]

تعریف join_ٹوکن(ٹوکن: List[str]) -> str:
    s = " ".join(ٹوکن)
    s = strip_بن_ورتیا_macros(s)
    s = normalize_ws(s)
    s = collapse_تارا_spaces(s)
    s = fix_spacing(s)
    أرجع s

تعریف سرخی_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    حاول:
        ret = getattr(cur, "result_type", کجھ_نئیں)
        ret_sp = ret.spelling إذا ret اے ليس کجھ_نئیں وإلا ""
    استثناء Exception:
        ret_sp = ""
    إذا ليس ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        حاول:
            ret_sp = cur.type.get_نتیجہ().spelling
        استثناء Exception:
            ret_sp = "void"
    name_sp = cur.spelling أو ""
    params = []
    حاول:
        لكل a في cur.get_arguments() أو []:
            t = getattr(a, "type", کجھ_نئیں)
            t_sp = t.spelling إذا t اے ليس کجھ_نئیں وإلا ""
            a_sp = a.spelling أو ""
            seg = (t_sp + (" " + a_sp إذا a_sp وإلا "")).strip()
            params.append(seg إذا seg وإلا "void")
    استثناء Exception:
        لنگھو
    params_s = ", ".join(params) إذا params وإلا "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_بن_ورتیا_macros(s)
    s = normalize_ws(s)
    s = collapse_تارا_spaces(s)
    s = fix_spacing(s)
    أرجع s

تعریف وچ_this_file(cur, src_رستہ: رستہ_2) -> bool:
    حاول:
        loc = cur.extent.شروع
        f = (loc.file أو cur.location.file)
        إذا ليس f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            حاول:
                toks = ٹوکن_for_function_سرخی(cur)
                أرجع bool(toks)
            استثناء Exception:
                أرجع خطأ
        cur_رستہ = os.رستہ.normcase(os.رستہ.abspath(str(f.name)))
        src_abs  = os.رستہ.normcase(os.رستہ.abspath(src_رستہ.as_posix()))
        # Accept the main source file
        إذا cur_رستہ == src_abs أو os.رستہ.basename(cur_رستہ) == os.رستہ.basename(src_abs):
            أرجع سچ
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.رستہ.normcase(os.رستہ.abspath(src_رستہ.ماپا.as_posix()))
        إذا cur_رستہ.endswith(".c") و os.رستہ.dirname(cur_رستہ) == src_dir:
            أرجع سچ
        أرجع خطأ
    استثناء Exception:
        أرجع خطأ

تعریف collect_function_defs(tu, src_رستہ: رستہ_2):
    تعریف walk(گرہ, ڈونگھائی=0):
        إذا DEBUG_LOG:
            چھاپو(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        إذا گرہ.قسم == Cursor_قسم.FUNCTION_DECL:
            إذا وچ_this_file(گرہ, src_رستہ):
                إذا گرہ.storage_class != StorageClass.STATIC و گرہ.is_definition():
                    أنتج گرہ
        لكل ch في گرہ.get_children():
            أنتج من walk(ch, ڈونگھائی+1)
    أنتج من walk(tu.cursor)

تعریف parse_with_clang(src_رستہ: رستہ_2, متن: str, دلیلاں: List[str]):
    اشاریہ = اشاریہ_3.create()
    src_abs = os.رستہ.abspath(src_رستہ.as_posix())
    unsaved = [(src_abs, متن)]
    proto_h = src_رستہ.ماپا / "proto.h"
    إذا proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_دلیلاں = (دلیلاں أو []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = اشاریہ.parse(src_abs, دلیلاں=final_دلیلاں, unsaved_files=unsaved, options=0)
    إذا SHOW_DIAGS:
        لكل d في getattr(tu, "diagnostics", []):
            چھاپو(d, file=sys.stderr)
    أرجع tu

تعریف generate_prototypes(tu, src_رستہ: رستہ_2) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    لكل fn في collect_function_defs(tu, src_رستہ):
        إذا DEBUG_LOG:
            حاول:
                sr = fn.extent.شروع
                er = fn.extent.سرا
                چھاپو(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            استثناء Exception:
                چھاپو(f"[def] {fn.spelling}", file=sys.stderr)
        toks = ٹوکن_for_function_سرخی(fn)
        سرخی = join_ٹوکن(toks) إذا toks وإلا سرخی_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        سرخی = re.sub(r'\b_Bool\b', 'bool', سرخی)
        إذا سرخی:
            protos.append(f"{header};")

    تکیا, unique = سیٹ_2(), []
    لكل p في protos:
        إذا p ليس في تکیا:
            unique.append(p)
            تکیا.جوڑو(p)
    أرجع unique

تعریف write_prototypes(باہرلا_رستہ: رستہ_2, سرخیاں: List[str], src_name: str) -> کجھ_نئیں:
    """Write collected prototypes to the output .pro file."""
    باہرلا_رستہ.ماپا.mkdir(parents=سچ, exist_ok=سچ)
    حاول:
        نال باہرلا_رستہ.open("w", encoding="utf-8", newline="\n") كـ f:
            f.اكتب(f"// {src_name}\n")
            لكل h في سرخیاں:
                f.اكتب(h + "\n")
            f.اكتب("// vim: ft=c\n")
    استثناء Exception كـ e:
        چھاپو(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    چھاپو(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

تعریف مکھ():
    دلیلاں = sys.argv[1:]
    إذا ليس دلیلاں:
        چھاپو("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_رستہ = رستہ_2(دلیلاں[0])
    clang_دلیلاں = دلیلاں[1:]
    إذا ليس src_رستہ.exists():
        چھاپو(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    حاول:
        original = src_رستہ.read_متن(encoding="utf-8", errors="ignore")
    استثناء Exception كـ e:
        چھاپو(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_پہلا_branch(original)

    # Parse with libclang
    حاول:
        tu = parse_with_clang(src_رستہ, rewritten, clang_دلیلاں)
    استثناء Exception كـ e:
        چھاپو(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    سرخیاں = generate_prototypes(tu, src_رستہ)
    باہرلا_file = رستہ_2("proto") / (src_رستہ.stem + ".pro")
    write_prototypes(باہرلا_file, سرخیاں, src_رستہ.name)

إذا __name__ == "__main__":
    مکھ()
