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
আনযন os
আনযন re
আনযন sys
তঃ pathlib আনযন Path
তঃ typing আনযন List, Tuple
তঃ clang.cindex আনযন Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") ন অন্তঃ ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") ন অন্তঃ ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(সারি: str) -> bool:
    প্রত্যাগম সারি.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    প্রতি a অন্তঃ argv:
        যদি a == "-DPROTO" বা a.startswith("-DPROTO="):
            প্রত্যাগম সত্য
    প্রত্যাগম অসত্য

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

    যদি DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    যদি proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    অন্যথা:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    যদি proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    অন্যথা:
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

    যত্ন:
        প্রত্যাগম bool(eval(s, {}, {}))
    অপবাদ Exception:
        # Fail-open to avoid accidentally dropping code
        যদি DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        প্রত্যাগম সত্য

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
    সারিগুলি = text.splitlines(keepends=সত্য)
    out: List[str] = []
    i, n = 0, len(সারিগুলি)

    def _collect_group(start: পূর্ণাঙ্ক) -> Tuple[পূর্ণাঙ্ক, List[Tuple[str, পূর্ণাঙ্ক, পূর্ণাঙ্ক]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        যাবত্ সত্য:
            h += 1
            যদি h >= n বা ন _ends_with_bs(সারিগুলি[h - 1]):
                ভঙ্গ
        depth = 1
        j = h
        marks: List[Tuple[str, পূর্ণাঙ্ক]] = [("if", start)]
        যাবত্ j < n চ depth > 0:
            m = _DIR_RE.match(সারিগুলি[j])
            যদি m:
                kw2 = m.group(1)
                যদি kw2 অন্তঃ ("if", "ifdef", "ifndef"):
                    depth += 1
                নোচেদ্যদি kw2 == "endif":
                    depth -= 1
                    যদি depth == 0:
                        ভঙ্গ
                নোচেদ্যদি depth == 1 চ kw2 অন্তঃ ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        অন্যথা:
            # Unterminated: keep as-is from start to EOF
            প্রত্যাগম n, [("text", start, n)]

        def after_header_সারি(pos: পূর্ণাঙ্ক) -> পূর্ণাঙ্ক:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            যাবত্ k < n চ _ends_with_bs(সারিগুলি[k - 1]):
                k += 1
            প্রত্যাগম k

        def after_header_সারি(pos: পূর্ণাঙ্ক) -> পূর্ণাঙ্ক:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            যাবত্ k < n চ _ends_with_bs(সারিগুলি[k - 1]):
                k += 1
            প্রত্যাগম k

        bodies: List[Tuple[str, পূর্ণাঙ্ক, পূর্ণাঙ্ক]] = []
        header_positions = [pos প্রতি _, pos অন্তঃ marks] + [j]
        tags = [tag প্রতি tag, _ অন্তঃ marks]
        প্রতি idx, tag অন্তঃ enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_সারি(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        প্রত্যাগম j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    যাবত্ i < n:
        সারি = সারিগুলি[i]
        m = _DIR_RE.match(সারি)
        যদি ন m:
            out.append(সারি)
            i += 1
            অনুবর্ত

        kw = m.group(1)
        যদি kw অন্তঃ ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            যদি _IF0_RE.match(সারি) বা _IFDEF_DEBUG_RE.match(সারি) বা _IFNDEF_PROTO_RE.match(সারি) বা _IF_NOTDEF_PROTO_RE.match(সারি):
                group_end, _ = _collect_group(i)
                i = group_end
                অনুবর্ত

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") চ (
                _DEFINED_CALL_RE.অনুসন্ধান(সারি) is ন None
            )

            যদি evaluate:
                # Extract condition text after 'if'
                যত্ন:
                    cond_text = সারি.split("if", 1)[1]
                অপবাদ Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                যদি ন keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    অনুবর্ত

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            যদি ন bodies:
                i = group_end
                অনুবর্ত
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(সারিগুলি[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            যদি DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                প্রতি ln অন্তঃ kept_text.splitlines():
                    যদি ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        ভঙ্গ

            out.append(kept_rewritten)
            i = group_end
        অন্যথা:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" যদি সারি.endswith("\n") অন্যথা "")
            i += 1

    প্রত্যাগম "".join(out)

def collapse_star_spaces(s: str) -> str:
    প্রত্যাগম re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    প্রত্যাগম s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    প্রত্যাগম s

def normalize_ws(s: str) -> str:
    প্রত্যাগম re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t প্রতি t অন্তঃ cur.get_tokens() যদি t.kind != TokenKind.COMMENT]
    যদি ন toks:
        প্রত্যাগম []
    name_idx = None
    প্রতি i, t অন্তঃ enumerate(toks):
        যদি t.kind == TokenKind.IDENTIFIER চ t.spelling == cur.spelling:
            name_idx = i
            ভঙ্গ
    যদি name_idx is None:
        প্রত্যাগম []
    lpar_idx = None
    প্রতি i অন্তঃ range(name_idx + 1, len(toks)):
        যদি toks[i].spelling == '(':
            lpar_idx = i
            ভঙ্গ
    যদি lpar_idx is None:
        প্রত্যাগম []
    depth, rpar_idx = 0, None
    প্রতি i অন্তঃ range(lpar_idx, len(toks)):
        যদি toks[i].spelling == '(':
            depth += 1
        নোচেদ্যদি toks[i].spelling == ')':
            depth -= 1
            যদি depth == 0:
                rpar_idx = i
                ভঙ্গ
    যদি rpar_idx is None:
        প্রত্যাগম []
    প্রত্যাগম [t.spelling প্রতি t অন্তঃ toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    প্রত্যাগম s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    যত্ন:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling যদি ret is ন None অন্যথা ""
    অপবাদ Exception:
        ret_sp = ""
    যদি ন ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        যত্ন:
            ret_sp = cur.type.get_result().spelling
        অপবাদ Exception:
            ret_sp = "void"
    name_sp = cur.spelling বা ""
    params = []
    যত্ন:
        প্রতি a অন্তঃ cur.get_arguments() বা []:
            t = getattr(a, "type", None)
            t_sp = t.spelling যদি t is ন None অন্যথা ""
            a_sp = a.spelling বা ""
            seg = (t_sp + (" " + a_sp যদি a_sp অন্যথা "")).strip()
            params.append(seg যদি seg অন্যথা "void")
    অপবাদ Exception:
        pass
    params_s = ", ".join(params) যদি params অন্যথা "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    প্রত্যাগম s

def in_this_file(cur, src_path: Path) -> bool:
    যত্ন:
        loc = cur.extent.start
        f = (loc.file বা cur.location.file)
        যদি ন f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            যত্ন:
                toks = tokens_for_function_header(cur)
                প্রত্যাগম bool(toks)
            অপবাদ Exception:
                প্রত্যাগম অসত্য
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        যদি cur_path == src_abs বা os.path.basename(cur_path) == os.path.basename(src_abs):
            প্রত্যাগম সত্য
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        যদি cur_path.endswith(".c") চ os.path.dirname(cur_path) == src_dir:
            প্রত্যাগম সত্য
        প্রত্যাগম অসত্য
    অপবাদ Exception:
        প্রত্যাগম অসত্য

def collect_function_defs(tu, src_path: Path):
    def walk(গ্রন্থি, depth=0):
        যদি DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        যদি গ্রন্থি.kind == CursorKind.FUNCTION_DECL:
            যদি in_this_file(গ্রন্থি, src_path):
                যদি গ্রন্থি.storage_class != StorageClass.STATIC চ গ্রন্থি.is_definition():
                    দা গ্রন্থি
        প্রতি ch অন্তঃ গ্রন্থি.get_children():
            দা তঃ walk(ch, depth+1)
    দা তঃ walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    যদি proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args বা []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    যদি SHOW_DIAGS:
        প্রতি d অন্তঃ getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    প্রত্যাগম tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    প্রতি fn অন্তঃ collect_function_defs(tu, src_path):
        যদি DEBUG_LOG:
            যত্ন:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            অপবাদ Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) যদি toks অন্যথা header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        যদি header:
            protos.append(f"{header};")

    seen, unique = set(), []
    প্রতি p অন্তঃ protos:
        যদি p ন অন্তঃ seen:
            unique.append(p)
            seen.add(p)
    প্রত্যাগম unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=সত্য, exist_ok=সত্য)
    যত্ন:
        with out_path.open("w", encoding="utf-8", newline="\n") ইব f:
            f.লিখ(f"// {src_name}\n")
            প্রতি h অন্তঃ headers:
                f.লিখ(h + "\n")
            f.লিখ("// vim: ft=c\n")
    অপবাদ Exception ইব e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def main():
    args = sys.argv[1:]
    যদি ন args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    যদি ন src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    যত্ন:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    অপবাদ Exception ইব e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    যত্ন:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    অপবাদ Exception ইব e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

যদি __name__ == "__main__":
    main()
