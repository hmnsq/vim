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
ಆನಯನ os
ಆನಯನ re
ಆನಯನ sys
ತಃ pathlib ಆನಯನ Path
ತಃ typing ಆನಯನ List, Tuple
ತಃ clang.cindex ಆನಯನ Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") ನ ಅನ್ತಃ ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") ನ ಅನ್ತಃ ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(ಸಾಲು: str) -> bool:
    ಪ್ರತ್ಯಾಗಮ ಸಾಲು.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    ಪ್ರತಿ a ಅನ್ತಃ argv:
        ಯದಿ a == "-DPROTO" ವಾ a.startswith("-DPROTO="):
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ

def _eval_condition_with_defined_proto(cond: str, proto_is_defined: bool) -> bool:
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

    ಯದಿ DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    ಯದಿ proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ಅನ್ಯಥಾ:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    ಯದಿ proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ಅನ್ಯಥಾ:
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

    ಯತ್ನ:
        ಪ್ರತ್ಯಾಗಮ bool(eval(s, {}, {}))
    ಅಪವಾದ Exception:
        # Fail-open to avoid accidentally dropping code
        ಯದಿ DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ

def rewrite_conditionals_first_branch(text: str) -> str:
    """
    Keep only the first branch of #if/#ifdef/#ifndef groups, remove others.
    Extensions:
      - Drop whole group for #if 0.
      - Drop whole group for #ifndef PROTO and #if !defined(PROTO).
      - If an '#if <expr>' contains defined(PROTO) anywhere in the expression,
        evaluate the condition: if False, drop the whole group; if True, keep
        only the first branch (same as legacy behavior).
    """
    ಸಾಲುಗಳು = text.splitlines(keepends=ಸತ್ಯ)
    out: List[str] = []
    i, n = 0, len(ಸಾಲುಗಳು)

    def _collect_group(start: ಪೂರ್ಣಾಙ್ಕ) -> Tuple[ಪೂರ್ಣಾಙ್ಕ, List[Tuple[str, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        ಯಾವತ್ ಸತ್ಯ:
            h += 1
            ಯದಿ h >= n ವಾ ನ _ends_with_bs(ಸಾಲುಗಳು[h - 1]):
                ಭಙ್ಗ
        depth = 1
        j = h
        marks: List[Tuple[str, ಪೂರ್ಣಾಙ್ಕ]] = [("if", start)]
        ಯಾವತ್ j < n ಚ depth > 0:
            m = _DIR_RE.match(ಸಾಲುಗಳು[j])
            ಯದಿ m:
                kw2 = m.group(1)
                ಯದಿ kw2 ಅನ್ತಃ ("if", "ifdef", "ifndef"):
                    depth += 1
                ನೋಚೇದ್ಯದಿ kw2 == "endif":
                    depth -= 1
                    ಯದಿ depth == 0:
                        ಭಙ್ಗ
                ನೋಚೇದ್ಯದಿ depth == 1 ಚ kw2 ಅನ್ತಃ ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        ಅನ್ಯಥಾ:
            # Unterminated: keep as-is from start to EOF
            ಪ್ರತ್ಯಾಗಮ n, [("text", start, n)]

        def after_header_ಸಾಲು(pos: ಪೂರ್ಣಾಙ್ಕ) -> ಪೂರ್ಣಾಙ್ಕ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ಯಾವತ್ k < n ಚ _ends_with_bs(ಸಾಲುಗಳು[k - 1]):
                k += 1
            ಪ್ರತ್ಯಾಗಮ k

        def after_header_ಸಾಲು(pos: ಪೂರ್ಣಾಙ್ಕ) -> ಪೂರ್ಣಾಙ್ಕ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ಯಾವತ್ k < n ಚ _ends_with_bs(ಸಾಲುಗಳು[k - 1]):
                k += 1
            ಪ್ರತ್ಯಾಗಮ k

        bodies: List[Tuple[str, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ]] = []
        header_positions = [pos ಪ್ರತಿ _, pos ಅನ್ತಃ marks] + [j]
        tags = [tag ಪ್ರತಿ tag, _ ಅನ್ತಃ marks]
        ಪ್ರತಿ idx, tag ಅನ್ತಃ enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_ಸಾಲು(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        ಪ್ರತ್ಯಾಗಮ j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    ಯಾವತ್ i < n:
        ಸಾಲು = ಸಾಲುಗಳು[i]
        m = _DIR_RE.match(ಸಾಲು)
        ಯದಿ ನ m:
            out.append(ಸಾಲು)
            i += 1
            ಅನುವರ್ತ

        kw = m.group(1)
        ಯದಿ kw ಅನ್ತಃ ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            ಯದಿ _IF0_RE.match(ಸಾಲು) ವಾ _IFDEF_DEBUG_RE.match(ಸಾಲು) ವಾ _IFNDEF_PROTO_RE.match(ಸಾಲು) ವಾ _IF_NOTDEF_PROTO_RE.match(ಸಾಲು):
                group_end, _ = _collect_group(i)
                i = group_end
                ಅನುವರ್ತ

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") ಚ (
                _DEFINED_CALL_RE.ಹುಡುಕು(ಸಾಲು) is ನ None
            )

            ಯದಿ evaluate:
                # Extract condition text after 'if'
                ಯತ್ನ:
                    cond_text = ಸಾಲು.split("if", 1)[1]
                ಅಪವಾದ Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                ಯದಿ ನ keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    ಅನುವರ್ತ

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            ಯದಿ ನ bodies:
                i = group_end
                ಅನುವರ್ತ
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(ಸಾಲುಗಳು[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            ಯದಿ DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                ಪ್ರತಿ ln ಅನ್ತಃ kept_text.splitlines():
                    ಯದಿ ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        ಭಙ್ಗ

            out.append(kept_rewritten)
            i = group_end
        ಅನ್ಯಥಾ:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" ಯದಿ ಸಾಲು.endswith("\n") ಅನ್ಯಥಾ "")
            i += 1

    ಪ್ರತ್ಯಾಗಮ "".join(out)

def collapse_star_spaces(s: str) -> str:
    ಪ್ರತ್ಯಾಗಮ re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    ಪ್ರತ್ಯಾಗಮ s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    ಪ್ರತ್ಯಾಗಮ s

def normalize_ws(s: str) -> str:
    ಪ್ರತ್ಯಾಗಮ re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t ಪ್ರತಿ t ಅನ್ತಃ cur.get_tokens() ಯದಿ t.kind != TokenKind.COMMENT]
    ಯದಿ ನ toks:
        ಪ್ರತ್ಯಾಗಮ []
    name_idx = None
    ಪ್ರತಿ i, t ಅನ್ತಃ enumerate(toks):
        ಯದಿ t.kind == TokenKind.IDENTIFIER ಚ t.spelling == cur.spelling:
            name_idx = i
            ಭಙ್ಗ
    ಯದಿ name_idx is None:
        ಪ್ರತ್ಯಾಗಮ []
    lpar_idx = None
    ಪ್ರತಿ i ಅನ್ತಃ range(name_idx + 1, len(toks)):
        ಯದಿ toks[i].spelling == '(':
            lpar_idx = i
            ಭಙ್ಗ
    ಯದಿ lpar_idx is None:
        ಪ್ರತ್ಯಾಗಮ []
    depth, rpar_idx = 0, None
    ಪ್ರತಿ i ಅನ್ತಃ range(lpar_idx, len(toks)):
        ಯದಿ toks[i].spelling == '(':
            depth += 1
        ನೋಚೇದ್ಯದಿ toks[i].spelling == ')':
            depth -= 1
            ಯದಿ depth == 0:
                rpar_idx = i
                ಭಙ್ಗ
    ಯದಿ rpar_idx is None:
        ಪ್ರತ್ಯಾಗಮ []
    ಪ್ರತ್ಯಾಗಮ [t.spelling ಪ್ರತಿ t ಅನ್ತಃ toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    ಪ್ರತ್ಯಾಗಮ s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    ಯತ್ನ:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling ಯದಿ ret is ನ None ಅನ್ಯಥಾ ""
    ಅಪವಾದ Exception:
        ret_sp = ""
    ಯದಿ ನ ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        ಯತ್ನ:
            ret_sp = cur.type.get_result().spelling
        ಅಪವಾದ Exception:
            ret_sp = "void"
    name_sp = cur.spelling ವಾ ""
    params = []
    ಯತ್ನ:
        ಪ್ರತಿ a ಅನ್ತಃ cur.get_arguments() ವಾ []:
            t = getattr(a, "type", None)
            t_sp = t.spelling ಯದಿ t is ನ None ಅನ್ಯಥಾ ""
            a_sp = a.spelling ವಾ ""
            seg = (t_sp + (" " + a_sp ಯದಿ a_sp ಅನ್ಯಥಾ "")).strip()
            params.append(seg ಯದಿ seg ಅನ್ಯಥಾ "void")
    ಅಪವಾದ Exception:
        pass
    params_s = ", ".join(params) ಯದಿ params ಅನ್ಯಥಾ "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    ಪ್ರತ್ಯಾಗಮ s

def in_this_file(cur, src_path: Path) -> bool:
    ಯತ್ನ:
        loc = cur.extent.start
        f = (loc.file ವಾ cur.location.file)
        ಯದಿ ನ f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            ಯತ್ನ:
                toks = tokens_for_function_header(cur)
                ಪ್ರತ್ಯಾಗಮ bool(toks)
            ಅಪವಾದ Exception:
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        ಯದಿ cur_path == src_abs ವಾ os.path.basename(cur_path) == os.path.basename(src_abs):
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        ಯದಿ cur_path.endswith(".c") ಚ os.path.dirname(cur_path) == src_dir:
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ
    ಅಪವಾದ Exception:
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ

def collect_function_defs(tu, src_path: Path):
    def walk(ಗಂಟು, depth=0):
        ಯದಿ DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        ಯದಿ ಗಂಟು.kind == CursorKind.FUNCTION_DECL:
            ಯದಿ in_this_file(ಗಂಟು, src_path):
                ಯದಿ ಗಂಟು.storage_class != StorageClass.STATIC ಚ ಗಂಟು.is_definition():
                    ದಾ ಗಂಟು
        ಪ್ರತಿ ch ಅನ್ತಃ ಗಂಟು.get_children():
            ದಾ ತಃ walk(ch, depth+1)
    ದಾ ತಃ walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    ಯದಿ proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args ವಾ []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    ಯದಿ SHOW_DIAGS:
        ಪ್ರತಿ d ಅನ್ತಃ getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    ಪ್ರತ್ಯಾಗಮ tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    ಪ್ರತಿ fn ಅನ್ತಃ collect_function_defs(tu, src_path):
        ಯದಿ DEBUG_LOG:
            ಯತ್ನ:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            ಅಪವಾದ Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) ಯದಿ toks ಅನ್ಯಥಾ header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        ಯದಿ header:
            protos.append(f"{header};")

    seen, unique = set(), []
    ಪ್ರತಿ p ಅನ್ತಃ protos:
        ಯದಿ p ನ ಅನ್ತಃ seen:
            unique.append(p)
            seen.add(p)
    ಪ್ರತ್ಯಾಗಮ unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=ಸತ್ಯ, exist_ok=ಸತ್ಯ)
    ಯತ್ನ:
        with out_path.open("w", encoding="utf-8", newline="\n") ಇವ f:
            f.ಲಿಖ(f"// {src_name}\n")
            ಪ್ರತಿ h ಅನ್ತಃ headers:
                f.ಲಿಖ(h + "\n")
            f.ಲಿಖ("// vim: ft=c\n")
    ಅಪವಾದ Exception ಇವ e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def main():
    args = sys.argv[1:]
    ಯದಿ ನ args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    ಯದಿ ನ src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    ಯತ್ನ:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    ಅಪವಾದ Exception ಇವ e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    ಯತ್ನ:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    ಅಪವಾದ Exception ಇವ e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

ಯದಿ __name__ == "__main__":
    main()
