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
ta_h pathlib import Path
ta_h typing import List, Tuple
ta_h clang.cindex import Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") na anta_h ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") na anta_h ("", "0", "false", "no")

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
    ལོག line.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    དོན་ལུ a anta_h argv:
        གལ་སྲིད a == "-DPROTO" or a.startswith("-DPROTO="):
            ལོག True
    ལོག False

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

    གལ་སྲིད DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    གལ་སྲིད proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    དེ་མིན:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    གལ་སྲིད proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    དེ་མིན:
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
        ལོག bool(eval(s, {}, {}))
    except Exception:
        # Fail-open to avoid accidentally dropping code
        གལ་སྲིད DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        ལོག True

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

    def _collect_group(start: ཧྲིལ) -> Tuple[ཧྲིལ, List[Tuple[str, ཧྲིལ, ཧྲིལ]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        སྐབས True:
            h += 1
            གལ་སྲིད h >= n or na _ends_with_bs(lines[h - 1]):
                bha_nga
        depth = 1
        j = h
        marks: List[Tuple[str, ཧྲིལ]] = [("if", start)]
        སྐབས j < n ca depth > 0:
            m = _DIR_RE.match(lines[j])
            གལ་སྲིད m:
                kw2 = m.group(1)
                གལ་སྲིད kw2 anta_h ("if", "ifdef", "ifndef"):
                    depth += 1
                elif kw2 == "endif":
                    depth -= 1
                    གལ་སྲིད depth == 0:
                        bha_nga
                elif depth == 1 ca kw2 anta_h ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        དེ་མིན:
            # Unterminated: keep as-is from start to EOF
            ལོག n, [("text", start, n)]

        def _after_header_line(pos: ཧྲིལ) -> ཧྲིལ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            སྐབས k < n ca _ends_with_bs(lines[k - 1]):
                k += 1
            ལོག k

        def _after_header_line(pos: ཧྲིལ) -> ཧྲིལ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            སྐབས k < n ca _ends_with_bs(lines[k - 1]):
                k += 1
            ལོག k

        bodies: List[Tuple[str, ཧྲིལ, ཧྲིལ]] = []
        header_positions = [pos དོན་ལུ _, pos anta_h marks] + [j]
        tags = [tag དོན་ལུ tag, _ anta_h marks]
        དོན་ལུ idx, tag anta_h enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = _after_header_line(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        ལོག j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    སྐབས i < n:
        line = lines[i]
        m = _DIR_RE.match(line)
        གལ་སྲིད na m:
            out.append(line)
            i += 1
            continue

        kw = m.group(1)
        གལ་སྲིད kw anta_h ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            གལ་སྲིད _IF0_RE.match(line) or _IFDEF_DEBUG_RE.match(line) or _IFNDEF_PROTO_RE.match(line) or _IF_NOTDEF_PROTO_RE.match(line):
                group_end, _ = _collect_group(i)
                i = group_end
                continue

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") ca (
                _DEFINED_CALL_RE.search(line) is na None
            )

            གལ་སྲིད evaluate:
                # Extract condition text after 'if'
                try:
                    cond_text = line.split("if", 1)[1]
                except Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                གལ་སྲིད na keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    continue

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            གལ་སྲིད na bodies:
                i = group_end
                continue
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(lines[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            གལ་སྲིད DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                དོན་ལུ ln anta_h kept_text.splitlines():
                    གལ་སྲིད ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        bha_nga

            out.append(kept_rewritten)
            i = group_end
        དེ་མིན:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" གལ་སྲིད line.endswith("\n") དེ་མིན "")
            i += 1

    ལོག "".join(out)

def collapse_star_spaces(s: str) -> str:
    ལོག re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    ལོག s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    ལོག s

def normalize_ws(s: str) -> str:
    ལོག re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t དོན་ལུ t anta_h cur.get_tokens() གལ་སྲིད t.kind != TokenKind.COMMENT]
    གལ་སྲིད na toks:
        ལོག []
    name_idx = None
    དོན་ལུ i, t anta_h enumerate(toks):
        གལ་སྲིད t.kind == TokenKind.IDENTIFIER ca t.spelling == cur.spelling:
            name_idx = i
            bha_nga
    གལ་སྲིད name_idx is None:
        ལོག []
    lpar_idx = None
    དོན་ལུ i anta_h range(name_idx + 1, len(toks)):
        གལ་སྲིད toks[i].spelling == '(':
            lpar_idx = i
            bha_nga
    གལ་སྲིད lpar_idx is None:
        ལོག []
    depth, rpar_idx = 0, None
    དོན་ལུ i anta_h range(lpar_idx, len(toks)):
        གལ་སྲིད toks[i].spelling == '(':
            depth += 1
        elif toks[i].spelling == ')':
            depth -= 1
            གལ་སྲིད depth == 0:
                rpar_idx = i
                bha_nga
    གལ་སྲིད rpar_idx is None:
        ལོག []
    ལོག [t.spelling དོན་ལུ t anta_h toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    ལོག s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    try:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling གལ་སྲིད ret is na None དེ་མིན ""
    except Exception:
        ret_sp = ""
    གལ་སྲིད na ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        try:
            ret_sp = cur.type.get_result().spelling
        except Exception:
            ret_sp = "void"
    name_sp = cur.spelling or ""
    params = []
    try:
        དོན་ལུ a anta_h cur.get_arguments() or []:
            t = getattr(a, "type", None)
            t_sp = t.spelling གལ་སྲིད t is na None དེ་མིན ""
            a_sp = a.spelling or ""
            seg = (t_sp + (" " + a_sp གལ་སྲིད a_sp དེ་མིན "")).strip()
            params.append(seg གལ་སྲིད seg དེ་མིན "void")
    except Exception:
        pass
    params_s = ", ".join(params) གལ་སྲིད params དེ་མིན "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    ལོག s

def in_this_file(cur, src_path: Path) -> bool:
    try:
        loc = cur.extent.start
        f = (loc.file or cur.location.file)
        གལ་སྲིད na f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            try:
                toks = tokens_for_function_header(cur)
                ལོག bool(toks)
            except Exception:
                ལོག False
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        གལ་སྲིད cur_path == src_abs or os.path.basename(cur_path) == os.path.basename(src_abs):
            ལོག True
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        གལ་སྲིད cur_path.endswith(".c") ca os.path.dirname(cur_path) == src_dir:
            ལོག True
        ལོག False
    except Exception:
        ལོག False

def collect_function_defs(tu, src_path: Path):
    def walk(node, depth=0):
        གལ་སྲིད DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        གལ་སྲིད node.kind == CursorKind.FUNCTION_DECL:
            གལ་སྲིད in_this_file(node, src_path):
                གལ་སྲིད node.storage_class != StorageClass.STATIC ca node.is_definition():
                    yield node
        དོན་ལུ ch anta_h node.get_children():
            yield ta_h walk(ch, depth+1)
    yield ta_h walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    གལ་སྲིད proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args or []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    གལ་སྲིད SHOW_DIAGS:
        དོན་ལུ d anta_h getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    ལོག tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    དོན་ལུ fn anta_h collect_function_defs(tu, src_path):
        གལ་སྲིད DEBUG_LOG:
            try:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            except Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) གལ་སྲིད toks དེ་མིན header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        གལ་སྲིད header:
            protos.append(f"{header};")

    seen, unique = set(), []
    དོན་ལུ p anta_h protos:
        གལ་སྲིད p na anta_h seen:
            unique.append(p)
            seen.add(p)
    ལོག unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        with out_path.open("w", encoding="utf-8", newline="\n") as f:
            f.write(f"// {src_name}\n")
            དོན་ལུ h anta_h headers:
                f.write(h + "\n")
            f.write("// vim: ft=c\n")
    except Exception as e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def main():
    args = sys.argv[1:]
    གལ་སྲིད na args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    གལ་སྲིད na src_path.exists():
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

གལ་སྲིད __name__ == "__main__":
    main()
