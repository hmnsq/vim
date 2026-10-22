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
import os
import re
import sys
from pathlib import Path
from typing import List, Tuple
from clang.cindex import Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") not in ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") not in ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(line: str) -> bool:
    반환 line.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    위해 a in argv:
        만약 a == "-DPROTO" or a.startswith("-DPROTO="):
            반환 True
    반환 False

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

    만약 DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    만약 proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    아니면:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    만약 proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    아니면:
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

    try:
        반환 bool(eval(s, {}, {}))
    except Exception:
        # Fail-open to avoid accidentally dropping code
        만약 DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        반환 True

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
    lines = text.splitlines(keepends=True)
    out: List[str] = []
    i, n = 0, len(lines)

    def _collect_group(start: 정수) -> Tuple[정수, List[Tuple[str, 정수, 정수]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        동안 True:
            h += 1
            만약 h >= n or not _ends_with_bs(lines[h - 1]):
                break
        depth = 1
        j = h
        marks: List[Tuple[str, 정수]] = [("if", start)]
        동안 j < n and depth > 0:
            m = _DIR_RE.match(lines[j])
            만약 m:
                kw2 = m.group(1)
                만약 kw2 in ("if", "ifdef", "ifndef"):
                    depth += 1
                elif kw2 == "endif":
                    depth -= 1
                    만약 depth == 0:
                        break
                elif depth == 1 and kw2 in ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        아니면:
            # Unterminated: keep as-is from start to EOF
            반환 n, [("text", start, n)]

        def _after_header_line(pos: 정수) -> 정수:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            동안 k < n and _ends_with_bs(lines[k - 1]):
                k += 1
            반환 k

        def _after_header_line(pos: 정수) -> 정수:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            동안 k < n and _ends_with_bs(lines[k - 1]):
                k += 1
            반환 k

        bodies: List[Tuple[str, 정수, 정수]] = []
        header_positions = [pos 위해 _, pos in marks] + [j]
        tags = [tag 위해 tag, _ in marks]
        위해 idx, tag in enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = _after_header_line(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        반환 j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    동안 i < n:
        line = lines[i]
        m = _DIR_RE.match(line)
        만약 not m:
            out.append(line)
            i += 1
            continue

        kw = m.group(1)
        만약 kw in ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            만약 _IF0_RE.match(line) or _IFDEF_DEBUG_RE.match(line) or _IFNDEF_PROTO_RE.match(line) or _IF_NOTDEF_PROTO_RE.match(line):
                group_end, _ = _collect_group(i)
                i = group_end
                continue

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") and (
                _DEFINED_CALL_RE.search(line) is not None
            )

            만약 evaluate:
                # Extract condition text after 'if'
                try:
                    cond_text = line.split("if", 1)[1]
                except Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                만약 not keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    continue

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            만약 not bodies:
                i = group_end
                continue
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(lines[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            만약 DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                위해 ln in kept_text.splitlines():
                    만약 ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        break

            out.append(kept_rewritten)
            i = group_end
        아니면:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" 만약 line.endswith("\n") 아니면 "")
            i += 1

    반환 "".join(out)

def collapse_star_spaces(s: str) -> str:
    반환 re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    반환 s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    반환 s

def normalize_ws(s: str) -> str:
    반환 re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t 위해 t in cur.get_tokens() 만약 t.kind != TokenKind.COMMENT]
    만약 not toks:
        반환 []
    name_idx = None
    위해 i, t in enumerate(toks):
        만약 t.kind == TokenKind.IDENTIFIER and t.spelling == cur.spelling:
            name_idx = i
            break
    만약 name_idx is None:
        반환 []
    lpar_idx = None
    위해 i in range(name_idx + 1, len(toks)):
        만약 toks[i].spelling == '(':
            lpar_idx = i
            break
    만약 lpar_idx is None:
        반환 []
    depth, rpar_idx = 0, None
    위해 i in range(lpar_idx, len(toks)):
        만약 toks[i].spelling == '(':
            depth += 1
        elif toks[i].spelling == ')':
            depth -= 1
            만약 depth == 0:
                rpar_idx = i
                break
    만약 rpar_idx is None:
        반환 []
    반환 [t.spelling 위해 t in toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    반환 s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    try:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling 만약 ret is not None 아니면 ""
    except Exception:
        ret_sp = ""
    만약 not ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        try:
            ret_sp = cur.type.get_result().spelling
        except Exception:
            ret_sp = "void"
    name_sp = cur.spelling or ""
    params = []
    try:
        위해 a in cur.get_arguments() or []:
            t = getattr(a, "type", None)
            t_sp = t.spelling 만약 t is not None 아니면 ""
            a_sp = a.spelling or ""
            seg = (t_sp + (" " + a_sp 만약 a_sp 아니면 "")).strip()
            params.append(seg 만약 seg 아니면 "void")
    except Exception:
        pass
    params_s = ", ".join(params) 만약 params 아니면 "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    반환 s

def in_this_file(cur, src_path: Path) -> bool:
    try:
        loc = cur.extent.start
        f = (loc.file or cur.location.file)
        만약 not f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            try:
                toks = tokens_for_function_header(cur)
                반환 bool(toks)
            except Exception:
                반환 False
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        만약 cur_path == src_abs or os.path.basename(cur_path) == os.path.basename(src_abs):
            반환 True
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        만약 cur_path.endswith(".c") and os.path.dirname(cur_path) == src_dir:
            반환 True
        반환 False
    except Exception:
        반환 False

def collect_function_defs(tu, src_path: Path):
    def walk(node, depth=0):
        만약 DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        만약 node.kind == CursorKind.FUNCTION_DECL:
            만약 in_this_file(node, src_path):
                만약 node.storage_class != StorageClass.STATIC and node.is_definition():
                    yield node
        위해 ch in node.get_children():
            yield from walk(ch, depth+1)
    yield from walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    만약 proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args or []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    만약 SHOW_DIAGS:
        위해 d in getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    반환 tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    위해 fn in collect_function_defs(tu, src_path):
        만약 DEBUG_LOG:
            try:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            except Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) 만약 toks 아니면 header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        만약 header:
            protos.append(f"{header};")

    seen, unique = set(), []
    위해 p in protos:
        만약 p not in seen:
            unique.append(p)
            seen.add(p)
    반환 unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        with out_path.open("w", encoding="utf-8", newline="\n") as f:
            f.write(f"// {src_name}\n")
            위해 h in headers:
                f.write(h + "\n")
            f.write("// vim: ft=c\n")
    except Exception as e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def main():
    args = sys.argv[1:]
    만약 not args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    만약 not src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    try:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    except Exception as e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    try:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    except Exception as e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

만약 __name__ == "__main__":
    main()
