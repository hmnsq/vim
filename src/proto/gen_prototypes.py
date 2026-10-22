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
దిగుమతి os
దిగుమతి re
దిగుమతి sys
తః pathlib దిగుమతి మార్గం_2
తః typing దిగుమతి List, Tuple
తః clang.cindex దిగుమతి సూచీ_3, Cursor_రకం, StorageClass, Token_రకం

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") కాదు లో ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") కాదు లో ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

నిర్వచనం చివర్లు_with_bs(పంక్తి: str) -> bool:
    తిరిగి పంక్తి.rstrip("\r\n").rstrip().endswith("\\")

నిర్వచనం _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    కొరకు a లో argv:
        ఒకవేళ a == "-DPROTO" లేదా a.startswith("-DPROTO="):
            తిరిగి నిజం
    తిరిగి అబద్ధం

నిర్వచనం _eval_condition_with_defined_proto(cond: str, proto_is_defined: bool) -> bool:
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

    ఒకవేళ DEBUG_LOG:
        ముద్రించు(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        ముద్రించు(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    ఒకవేళ proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    లేకపోతే:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    ఒకవేళ proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    లేకపోతే:
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

    ప్రయత్నించు:
        తిరిగి bool(eval(s, {}, {}))
    మినహాయింపు Exception:
        # Fail-open to avoid accidentally dropping code
        ఒకవేళ DEBUG_LOG:
            ముద్రించు(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        తిరిగి నిజం

నిర్వచనం rewrite_conditionals_మొదటి_branch(పాఠ్యం: str) -> str:
    """
    Keep only the first branch of #if/#ifdef/#ifndef groups, remove others.
    Extensions:
      - Drop whole group for #if 0.
      - Drop whole group for #ifndef PROTO and #if !defined(PROTO).
      - If an '#if <expr>' contains defined(PROTO) anywhere in the expression,
        evaluate the condition: if False, drop the whole group; if True, keep
        only the first branch (same as legacy behavior).
    """
    పంక్తులు = పాఠ్యం.splitlines(keepends=నిజం)
    బహిర్గతం: List[str] = []
    i, n = 0, పొడవు(పంక్తులు)

    నిర్వచనం collect_సమూహం(ప్రారంభం: పూర్ణాంకం) -> Tuple[పూర్ణాంకం, List[Tuple[str, పూర్ణాంకం, పూర్ణాంకం]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = ప్రారంభం
        # Skip backslash-continued header lines
        అయితే నిజం:
            h += 1
            ఒకవేళ h >= n లేదా కాదు చివర్లు_with_bs(పంక్తులు[h - 1]):
                ఆపు
        లోతు = 1
        j = h
        marks: List[Tuple[str, పూర్ణాంకం]] = [("if", ప్రారంభం)]
        అయితే j < n మరియు లోతు > 0:
            m = _DIR_RE.సరిపోలిక(పంక్తులు[j])
            ఒకవేళ m:
                kw2 = m.సమూహం(1)
                ఒకవేళ kw2 లో ("if", "ifdef", "ifndef"):
                    లోతు += 1
                లేదావేళ kw2 == "endif":
                    లోతు -= 1
                    ఒకవేళ లోతు == 0:
                        ఆపు
                లేదావేళ లోతు == 1 మరియు kw2 లో ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        లేకపోతే:
            # Unterminated: keep as-is from start to EOF
            తిరిగి n, [("text", ప్రారంభం, n)]

        నిర్వచనం after_శీర్షిక_పంక్తి(pos: పూర్ణాంకం) -> పూర్ణాంకం:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            అయితే k < n మరియు చివర్లు_with_bs(పంక్తులు[k - 1]):
                k += 1
            తిరిగి k

        నిర్వచనం after_శీర్షిక_పంక్తి(pos: పూర్ణాంకం) -> పూర్ణాంకం:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            అయితే k < n మరియు చివర్లు_with_bs(పంక్తులు[k - 1]):
                k += 1
            తిరిగి k

        bodies: List[Tuple[str, పూర్ణాంకం, పూర్ణాంకం]] = []
        శీర్షిక_positions = [pos కొరకు _, pos లో marks] + [j]
        ట్యాగులు = [ట్యాగ్ కొరకు ట్యాగ్, _ లో marks]
        కొరకు idx, ట్యాగ్ లో enumerate(ట్యాగులు):
            శీర్షిక = శీర్షిక_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_శీర్షిక_పంక్తి(శీర్షిక)
            తదుపరి_శీర్షిక = శీర్షిక_positions[idx + 1]
            body_e = తదుపరి_శీర్షిక
            bodies.append((ట్యాగ్, body_s, body_e))
        తిరిగి j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    అయితే i < n:
        పంక్తి = పంక్తులు[i]
        m = _DIR_RE.సరిపోలిక(పంక్తి)
        ఒకవేళ కాదు m:
            బహిర్గతం.append(పంక్తి)
            i += 1
            కొనసాగు

        kw = m.సమూహం(1)
        ఒకవేళ kw లో ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            ఒకవేళ _IF0_RE.సరిపోలిక(పంక్తి) లేదా _IFDEF_DEBUG_RE.సరిపోలిక(పంక్తి) లేదా _IFNDEF_PROTO_RE.సరిపోలిక(పంక్తి) లేదా _IF_NOTDEF_PROTO_RE.సరిపోలిక(పంక్తి):
                సమూహం_చివర, _ = collect_సమూహం(i)
                i = సమూహం_చివర
                కొనసాగు

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") మరియు (
                _DEFINED_CALL_RE.శోధన(పంక్తి) అయిన కాదు ఏమీలేదు
            )

            ఒకవేళ evaluate:
                # Extract condition text after 'if'
                ప్రయత్నించు:
                    cond_పాఠ్యం = పంక్తి.విభజించు("if", 1)[1]
                మినహాయింపు Exception:
                    cond_పాఠ్యం = ""
                keep_మొదటి = _eval_condition_with_defined_proto(cond_పాఠ్యం, proto_is_defined)
                ఒకవేళ కాదు keep_మొదటి:
                    సమూహం_చివర, _ = collect_సమూహం(i)
                    i = సమూహం_చివర
                    కొనసాగు

            # Keep only the first branch
            సమూహం_చివర, bodies = collect_సమూహం(i)
            ఒకవేళ కాదు bodies:
                i = సమూహం_చివర
                కొనసాగు
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_పాఠ్యం = "".join(పంక్తులు[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_మొదటి_branch(kept_పాఠ్యం)

            ఒకవేళ DEBUG_LOG:
                ముద్రించు(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                కొరకు ln లో kept_పాఠ్యం.splitlines():
                    ఒకవేళ ln.strip():
                        ముద్రించు(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        ఆపు

            బహిర్గతం.append(kept_rewritten)
            i = సమూహం_చివర
        లేకపోతే:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            బహిర్గతం.append("\n" ఒకవేళ పంక్తి.endswith("\n") లేకపోతే "")
            i += 1

    తిరిగి "".join(బహిర్గతం)

నిర్వచనం collapse_నక్షత్రం_spaces(s: str) -> str:
    తిరిగి re.sub(r"\*\s+", "*", s)

నిర్వచనం fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    తిరిగి s

నిర్వచనం strip_వాడని_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    తిరిగి s

నిర్వచనం normalize_ws(s: str) -> str:
    తిరిగి re.sub(r"\s+", " ", s).strip()

నిర్వచనం టోకెన్లు_for_function_శీర్షిక(cur) -> List[str]:
    toks = [t కొరకు t లో cur.get_టోకెన్లు() ఒకవేళ t.రకం != Token_రకం.COMMENT]
    ఒకవేళ కాదు toks:
        తిరిగి []
    name_idx = ఏమీలేదు
    కొరకు i, t లో enumerate(toks):
        ఒకవేళ t.రకం == Token_రకం.IDENTIFIER మరియు t.spelling == cur.spelling:
            name_idx = i
            ఆపు
    ఒకవేళ name_idx అయిన ఏమీలేదు:
        తిరిగి []
    lpar_idx = ఏమీలేదు
    కొరకు i లో శ్రేణి(name_idx + 1, పొడవు(toks)):
        ఒకవేళ toks[i].spelling == '(':
            lpar_idx = i
            ఆపు
    ఒకవేళ lpar_idx అయిన ఏమీలేదు:
        తిరిగి []
    లోతు, rpar_idx = 0, ఏమీలేదు
    కొరకు i లో శ్రేణి(lpar_idx, పొడవు(toks)):
        ఒకవేళ toks[i].spelling == '(':
            లోతు += 1
        లేదావేళ toks[i].spelling == ')':
            లోతు -= 1
            ఒకవేళ లోతు == 0:
                rpar_idx = i
                ఆపు
    ఒకవేళ rpar_idx అయిన ఏమీలేదు:
        తిరిగి []
    తిరిగి [t.spelling కొరకు t లో toks[:rpar_idx + 1]]

నిర్వచనం join_టోకెన్లు(టోకెన్లు: List[str]) -> str:
    s = " ".join(టోకెన్లు)
    s = strip_వాడని_macros(s)
    s = normalize_ws(s)
    s = collapse_నక్షత్రం_spaces(s)
    s = fix_spacing(s)
    తిరిగి s

నిర్వచనం శీర్షిక_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    ప్రయత్నించు:
        ret = getattr(cur, "result_type", ఏమీలేదు)
        ret_sp = ret.spelling ఒకవేళ ret అయిన కాదు ఏమీలేదు లేకపోతే ""
    మినహాయింపు Exception:
        ret_sp = ""
    ఒకవేళ కాదు ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        ప్రయత్నించు:
            ret_sp = cur.type.get_ఫలితం().spelling
        మినహాయింపు Exception:
            ret_sp = "void"
    name_sp = cur.spelling లేదా ""
    params = []
    ప్రయత్నించు:
        కొరకు a లో cur.get_arguments() లేదా []:
            t = getattr(a, "type", ఏమీలేదు)
            t_sp = t.spelling ఒకవేళ t అయిన కాదు ఏమీలేదు లేకపోతే ""
            a_sp = a.spelling లేదా ""
            seg = (t_sp + (" " + a_sp ఒకవేళ a_sp లేకపోతే "")).strip()
            params.append(seg ఒకవేళ seg లేకపోతే "void")
    మినహాయింపు Exception:
        దాటు
    params_s = ", ".join(params) ఒకవేళ params లేకపోతే "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_వాడని_macros(s)
    s = normalize_ws(s)
    s = collapse_నక్షత్రం_spaces(s)
    s = fix_spacing(s)
    తిరిగి s

నిర్వచనం లోపల_this_file(cur, src_మార్గం: మార్గం_2) -> bool:
    ప్రయత్నించు:
        loc = cur.extent.ప్రారంభం
        f = (loc.file లేదా cur.location.file)
        ఒకవేళ కాదు f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            ప్రయత్నించు:
                toks = టోకెన్లు_for_function_శీర్షిక(cur)
                తిరిగి bool(toks)
            మినహాయింపు Exception:
                తిరిగి అబద్ధం
        cur_మార్గం = os.మార్గం.normcase(os.మార్గం.abspath(str(f.name)))
        src_abs  = os.మార్గం.normcase(os.మార్గం.abspath(src_మార్గం.as_posix()))
        # Accept the main source file
        ఒకవేళ cur_మార్గం == src_abs లేదా os.మార్గం.basename(cur_మార్గం) == os.మార్గం.basename(src_abs):
            తిరిగి నిజం
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.మార్గం.normcase(os.మార్గం.abspath(src_మార్గం.మాతృ.as_posix()))
        ఒకవేళ cur_మార్గం.endswith(".c") మరియు os.మార్గం.dirname(cur_మార్గం) == src_dir:
            తిరిగి నిజం
        తిరిగి అబద్ధం
    మినహాయింపు Exception:
        తిరిగి అబద్ధం

నిర్వచనం collect_function_defs(tu, src_మార్గం: మార్గం_2):
    నిర్వచనం walk(కణుపు, లోతు=0):
        ఒకవేళ DEBUG_LOG:
            ముద్రించు(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        ఒకవేళ కణుపు.రకం == Cursor_రకం.FUNCTION_DECL:
            ఒకవేళ లోపల_this_file(కణుపు, src_మార్గం):
                ఒకవేళ కణుపు.storage_class != StorageClass.STATIC మరియు కణుపు.is_definition():
                    ఇవ్వు కణుపు
        కొరకు ch లో కణుపు.get_children():
            ఇవ్వు తః walk(ch, లోతు+1)
    ఇవ్వు తః walk(tu.cursor)

నిర్వచనం parse_with_clang(src_మార్గం: మార్గం_2, పాఠ్యం: str, వాదనలు: List[str]):
    సూచీ = సూచీ_3.create()
    src_abs = os.మార్గం.abspath(src_మార్గం.as_posix())
    unsaved = [(src_abs, పాఠ్యం)]
    proto_h = src_మార్గం.మాతృ / "proto.h"
    ఒకవేళ proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_వాదనలు = (వాదనలు లేదా []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = సూచీ.parse(src_abs, వాదనలు=final_వాదనలు, unsaved_files=unsaved, options=0)
    ఒకవేళ SHOW_DIAGS:
        కొరకు d లో getattr(tu, "diagnostics", []):
            ముద్రించు(d, file=sys.stderr)
    తిరిగి tu

నిర్వచనం generate_prototypes(tu, src_మార్గం: మార్గం_2) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    కొరకు fn లో collect_function_defs(tu, src_మార్గం):
        ఒకవేళ DEBUG_LOG:
            ప్రయత్నించు:
                sr = fn.extent.ప్రారంభం
                er = fn.extent.చివర
                ముద్రించు(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            మినహాయింపు Exception:
                ముద్రించు(f"[def] {fn.spelling}", file=sys.stderr)
        toks = టోకెన్లు_for_function_శీర్షిక(fn)
        శీర్షిక = join_టోకెన్లు(toks) ఒకవేళ toks లేకపోతే శీర్షిక_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        శీర్షిక = re.sub(r'\b_Bool\b', 'bool', శీర్షిక)
        ఒకవేళ శీర్షిక:
            protos.append(f"{header};")

    చూసిన, unique = సమితి_2(), []
    కొరకు p లో protos:
        ఒకవేళ p కాదు లో చూసిన:
            unique.append(p)
            చూసిన.కూడు(p)
    తిరిగి unique

నిర్వచనం write_prototypes(బహిర్గతం_మార్గం: మార్గం_2, శీర్షికలు: List[str], src_name: str) -> ఏమీలేదు:
    """Write collected prototypes to the output .pro file."""
    బహిర్గతం_మార్గం.మాతృ.mkdir(parents=నిజం, exist_ok=నిజం)
    ప్రయత్నించు:
        తోడుగా బహిర్గతం_మార్గం.open("w", encoding="utf-8", newline="\n") ఇవ f:
            f.లిఖ(f"// {src_name}\n")
            కొరకు h లో శీర్షికలు:
                f.లిఖ(h + "\n")
            f.లిఖ("// vim: ft=c\n")
    మినహాయింపు Exception ఇవ e:
        ముద్రించు(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    ముద్రించు(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

నిర్వచనం ప్రధాన():
    వాదనలు = sys.argv[1:]
    ఒకవేళ కాదు వాదనలు:
        ముద్రించు("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_మార్గం = మార్గం_2(వాదనలు[0])
    clang_వాదనలు = వాదనలు[1:]
    ఒకవేళ కాదు src_మార్గం.exists():
        ముద్రించు(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    ప్రయత్నించు:
        original = src_మార్గం.read_పాఠ్యం(encoding="utf-8", errors="ignore")
    మినహాయింపు Exception ఇవ e:
        ముద్రించు(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_మొదటి_branch(original)

    # Parse with libclang
    ప్రయత్నించు:
        tu = parse_with_clang(src_మార్గం, rewritten, clang_వాదనలు)
    మినహాయింపు Exception ఇవ e:
        ముద్రించు(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    శీర్షికలు = generate_prototypes(tu, src_మార్గం)
    బహిర్గతం_file = మార్గం_2("proto") / (src_మార్గం.stem + ".pro")
    write_prototypes(బహిర్గతం_file, శీర్షికలు, src_మార్గం.name)

ఒకవేళ __name__ == "__main__":
    ప్రధాన()
