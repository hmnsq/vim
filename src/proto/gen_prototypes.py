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
درآمد os
درآمد re
درآمد sys
سے pathlib درآمد راستہ_2
سے typing درآمد List, Tuple
سے clang.cindex درآمد اشاریہ_3, Cursor_قسم, StorageClass, Token_قسم

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") نہیں میں ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") نہیں میں ("", "0", "false", "no")

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
    واپس سطر.rstrip("\r\n").rstrip().endswith("\\")

تعریف _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    برائے a میں argv:
        اگر a == "-DPROTO" یا a.startswith("-DPROTO="):
            واپس سچ
    واپس جھوٹ

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

    اگر DEBUG_LOG:
        چھاپو(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        چھاپو(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    اگر proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ورنہ:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    اگر proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ورنہ:
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

    کوشش:
        واپس bool(eval(s, {}, {}))
    استثنا Exception:
        # Fail-open to avoid accidentally dropping code
        اگر DEBUG_LOG:
            چھاپو(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        واپس سچ

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
    برآمد: List[str] = []
    i, n = 0, لمبائی(سطریں)

    تعریف collect_گروہ(آغاز: صحیح) -> Tuple[صحیح, List[Tuple[str, صحیح, صحیح]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = آغاز
        # Skip backslash-continued header lines
        جب سچ:
            h += 1
            اگر h >= n یا نہیں سرے_with_bs(سطریں[h - 1]):
                توڑو
        گہرائی = 1
        j = h
        marks: List[Tuple[str, صحیح]] = [("if", آغاز)]
        جب j < n اور گہرائی > 0:
            m = _DIR_RE.مطابقت(سطریں[j])
            اگر m:
                kw2 = m.گروہ(1)
                اگر kw2 میں ("if", "ifdef", "ifndef"):
                    گہرائی += 1
                وگرنہ_اگر kw2 == "endif":
                    گہرائی -= 1
                    اگر گہرائی == 0:
                        توڑو
                وگرنہ_اگر گہرائی == 1 اور kw2 میں ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        ورنہ:
            # Unterminated: keep as-is from start to EOF
            واپس n, [("text", آغاز, n)]

        تعریف after_سرخی_سطر(pos: صحیح) -> صحیح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            جب k < n اور سرے_with_bs(سطریں[k - 1]):
                k += 1
            واپس k

        تعریف after_سرخی_سطر(pos: صحیح) -> صحیح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            جب k < n اور سرے_with_bs(سطریں[k - 1]):
                k += 1
            واپس k

        bodies: List[Tuple[str, صحیح, صحیح]] = []
        سرخی_positions = [pos برائے _, pos میں marks] + [j]
        لیبل_فہرست = [لیبل برائے لیبل, _ میں marks]
        برائے idx, لیبل میں enumerate(لیبل_فہرست):
            سرخی = سرخی_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_سرخی_سطر(سرخی)
            اگلا_سرخی = سرخی_positions[idx + 1]
            body_e = اگلا_سرخی
            bodies.append((لیبل, body_s, body_e))
        واپس j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    جب i < n:
        سطر = سطریں[i]
        m = _DIR_RE.مطابقت(سطر)
        اگر نہیں m:
            برآمد.append(سطر)
            i += 1
            جاری

        kw = m.گروہ(1)
        اگر kw میں ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            اگر _IF0_RE.مطابقت(سطر) یا _IFDEF_DEBUG_RE.مطابقت(سطر) یا _IFNDEF_PROTO_RE.مطابقت(سطر) یا _IF_NOTDEF_PROTO_RE.مطابقت(سطر):
                گروہ_انتہا, _ = collect_گروہ(i)
                i = گروہ_انتہا
                جاری

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") اور (
                _DEFINED_CALL_RE.تلاش(سطر) ہے نہیں کچھ_نہیں
            )

            اگر evaluate:
                # Extract condition text after 'if'
                کوشش:
                    cond_متن = سطر.بانٹو("if", 1)[1]
                استثنا Exception:
                    cond_متن = ""
                keep_پہلا = _eval_condition_with_defined_proto(cond_متن, proto_is_defined)
                اگر نہیں keep_پہلا:
                    گروہ_انتہا, _ = collect_گروہ(i)
                    i = گروہ_انتہا
                    جاری

            # Keep only the first branch
            گروہ_انتہا, bodies = collect_گروہ(i)
            اگر نہیں bodies:
                i = گروہ_انتہا
                جاری
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_متن = "".join(سطریں[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_پہلا_branch(kept_متن)

            اگر DEBUG_LOG:
                چھاپو(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                برائے ln میں kept_متن.splitlines():
                    اگر ln.strip():
                        چھاپو(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        توڑو

            برآمد.append(kept_rewritten)
            i = گروہ_انتہا
        ورنہ:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            برآمد.append("\n" اگر سطر.endswith("\n") ورنہ "")
            i += 1

    واپس "".join(برآمد)

تعریف collapse_ستارہ_spaces(s: str) -> str:
    واپس re.sub(r"\*\s+", "*", s)

تعریف fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    واپس s

تعریف strip_غیر_مستعمل_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    واپس s

تعریف normalize_ws(s: str) -> str:
    واپس re.sub(r"\s+", " ", s).strip()

تعریف علامات_for_function_سرخی(cur) -> List[str]:
    toks = [t برائے t میں cur.get_علامات() اگر t.قسم != Token_قسم.COMMENT]
    اگر نہیں toks:
        واپس []
    name_idx = کچھ_نہیں
    برائے i, t میں enumerate(toks):
        اگر t.قسم == Token_قسم.IDENTIFIER اور t.spelling == cur.spelling:
            name_idx = i
            توڑو
    اگر name_idx ہے کچھ_نہیں:
        واپس []
    lpar_idx = کچھ_نہیں
    برائے i میں سلسلہ(name_idx + 1, لمبائی(toks)):
        اگر toks[i].spelling == '(':
            lpar_idx = i
            توڑو
    اگر lpar_idx ہے کچھ_نہیں:
        واپس []
    گہرائی, rpar_idx = 0, کچھ_نہیں
    برائے i میں سلسلہ(lpar_idx, لمبائی(toks)):
        اگر toks[i].spelling == '(':
            گہرائی += 1
        وگرنہ_اگر toks[i].spelling == ')':
            گہرائی -= 1
            اگر گہرائی == 0:
                rpar_idx = i
                توڑو
    اگر rpar_idx ہے کچھ_نہیں:
        واپس []
    واپس [t.spelling برائے t میں toks[:rpar_idx + 1]]

تعریف join_علامات(علامات: List[str]) -> str:
    s = " ".join(علامات)
    s = strip_غیر_مستعمل_macros(s)
    s = normalize_ws(s)
    s = collapse_ستارہ_spaces(s)
    s = fix_spacing(s)
    واپس s

تعریف سرخی_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    کوشش:
        ret = getattr(cur, "result_type", کچھ_نہیں)
        ret_sp = ret.spelling اگر ret ہے نہیں کچھ_نہیں ورنہ ""
    استثنا Exception:
        ret_sp = ""
    اگر نہیں ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        کوشش:
            ret_sp = cur.type.get_نتیجہ().spelling
        استثنا Exception:
            ret_sp = "void"
    name_sp = cur.spelling یا ""
    params = []
    کوشش:
        برائے a میں cur.get_arguments() یا []:
            t = getattr(a, "type", کچھ_نہیں)
            t_sp = t.spelling اگر t ہے نہیں کچھ_نہیں ورنہ ""
            a_sp = a.spelling یا ""
            seg = (t_sp + (" " + a_sp اگر a_sp ورنہ "")).strip()
            params.append(seg اگر seg ورنہ "void")
    استثنا Exception:
        گزرو
    params_s = ", ".join(params) اگر params ورنہ "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_غیر_مستعمل_macros(s)
    s = normalize_ws(s)
    s = collapse_ستارہ_spaces(s)
    s = fix_spacing(s)
    واپس s

تعریف اندر_this_file(cur, src_راستہ: راستہ_2) -> bool:
    کوشش:
        loc = cur.extent.آغاز
        f = (loc.file یا cur.location.file)
        اگر نہیں f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            کوشش:
                toks = علامات_for_function_سرخی(cur)
                واپس bool(toks)
            استثنا Exception:
                واپس جھوٹ
        cur_راستہ = os.راستہ.normcase(os.راستہ.abspath(str(f.name)))
        src_abs  = os.راستہ.normcase(os.راستہ.abspath(src_راستہ.as_posix()))
        # Accept the main source file
        اگر cur_راستہ == src_abs یا os.راستہ.basename(cur_راستہ) == os.راستہ.basename(src_abs):
            واپس سچ
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.راستہ.normcase(os.راستہ.abspath(src_راستہ.والد.as_posix()))
        اگر cur_راستہ.endswith(".c") اور os.راستہ.dirname(cur_راستہ) == src_dir:
            واپس سچ
        واپس جھوٹ
    استثنا Exception:
        واپس جھوٹ

تعریف collect_function_defs(tu, src_راستہ: راستہ_2):
    تعریف walk(گرہ, گہرائی=0):
        اگر DEBUG_LOG:
            چھاپو(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        اگر گرہ.قسم == Cursor_قسم.FUNCTION_DECL:
            اگر اندر_this_file(گرہ, src_راستہ):
                اگر گرہ.storage_class != StorageClass.STATIC اور گرہ.is_definition():
                    دو گرہ
        برائے ch میں گرہ.get_children():
            دو سے walk(ch, گہرائی+1)
    دو سے walk(tu.cursor)

تعریف parse_with_clang(src_راستہ: راستہ_2, متن: str, دلائل: List[str]):
    اشاریہ = اشاریہ_3.create()
    src_abs = os.راستہ.abspath(src_راستہ.as_posix())
    unsaved = [(src_abs, متن)]
    proto_h = src_راستہ.والد / "proto.h"
    اگر proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_دلائل = (دلائل یا []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = اشاریہ.parse(src_abs, دلائل=final_دلائل, unsaved_files=unsaved, options=0)
    اگر SHOW_DIAGS:
        برائے d میں getattr(tu, "diagnostics", []):
            چھاپو(d, file=sys.stderr)
    واپس tu

تعریف generate_prototypes(tu, src_راستہ: راستہ_2) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    برائے fn میں collect_function_defs(tu, src_راستہ):
        اگر DEBUG_LOG:
            کوشش:
                sr = fn.extent.آغاز
                er = fn.extent.انتہا
                چھاپو(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            استثنا Exception:
                چھاپو(f"[def] {fn.spelling}", file=sys.stderr)
        toks = علامات_for_function_سرخی(fn)
        سرخی = join_علامات(toks) اگر toks ورنہ سرخی_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        سرخی = re.sub(r'\b_Bool\b', 'bool', سرخی)
        اگر سرخی:
            protos.append(f"{header};")

    نظر_آیا, unique = سیٹ_2(), []
    برائے p میں protos:
        اگر p نہیں میں نظر_آیا:
            unique.append(p)
            نظر_آیا.جوڑو(p)
    واپس unique

تعریف write_prototypes(برآمد_راستہ: راستہ_2, سرخیاں: List[str], src_name: str) -> کچھ_نہیں:
    """Write collected prototypes to the output .pro file."""
    برآمد_راستہ.والد.mkdir(parents=سچ, exist_ok=سچ)
    کوشش:
        ساتھ برآمد_راستہ.open("w", encoding="utf-8", newline="\n") بطور f:
            f.اكتب(f"// {src_name}\n")
            برائے h میں سرخیاں:
                f.اكتب(h + "\n")
            f.اكتب("// vim: ft=c\n")
    استثنا Exception بطور e:
        چھاپو(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    چھاپو(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

تعریف مین():
    دلائل = sys.argv[1:]
    اگر نہیں دلائل:
        چھاپو("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_راستہ = راستہ_2(دلائل[0])
    clang_دلائل = دلائل[1:]
    اگر نہیں src_راستہ.exists():
        چھاپو(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    کوشش:
        original = src_راستہ.read_متن(encoding="utf-8", errors="ignore")
    استثنا Exception بطور e:
        چھاپو(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_پہلا_branch(original)

    # Parse with libclang
    کوشش:
        tu = parse_with_clang(src_راستہ, rewritten, clang_دلائل)
    استثنا Exception بطور e:
        چھاپو(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    سرخیاں = generate_prototypes(tu, src_راستہ)
    برآمد_file = راستہ_2("proto") / (src_راستہ.stem + ".pro")
    write_prototypes(برآمد_file, سرخیاں, src_راستہ.name)

اگر __name__ == "__main__":
    مین()
