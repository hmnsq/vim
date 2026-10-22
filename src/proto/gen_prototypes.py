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
আমদানি os
আমদানি re
আমদানি sys
তঃ pathlib আমদানি Path
তঃ typing আমদানি List, Tuple
তঃ clang.cindex আমদানি সূচক_3, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") নয় মধ্যে ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") নয় মধ্যে ("", "0", "false", "no")

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
    ফেরত সারি.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    জন্য a মধ্যে argv:
        যদি a == "-DPROTO" অথবা a.startswith("-DPROTO="):
            ফেরত সত্য
    ফেরত মিথ্যা

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

    চেষ্টা:
        ফেরত bool(eval(s, {}, {}))
    ব্যতিক্রম Exception:
        # Fail-open to avoid accidentally dropping code
        যদি DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        ফেরত সত্য

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

    def _collect_group(start: পূর্ণসংখ্যা) -> Tuple[পূর্ণসংখ্যা, List[Tuple[str, পূর্ণসংখ্যা, পূর্ণসংখ্যা]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        যখন সত্য:
            h += 1
            যদি h >= n অথবা নয় _ends_with_bs(সারিগুলি[h - 1]):
                বিরতি
        depth = 1
        j = h
        marks: List[Tuple[str, পূর্ণসংখ্যা]] = [("if", start)]
        যখন j < n এবং depth > 0:
            m = _DIR_RE.match(সারিগুলি[j])
            যদি m:
                kw2 = m.group(1)
                যদি kw2 মধ্যে ("if", "ifdef", "ifndef"):
                    depth += 1
                নতুবা_যদি kw2 == "endif":
                    depth -= 1
                    যদি depth == 0:
                        বিরতি
                নতুবা_যদি depth == 1 এবং kw2 মধ্যে ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        অন্যথা:
            # Unterminated: keep as-is from start to EOF
            ফেরত n, [("text", start, n)]

        def after_header_সারি(pos: পূর্ণসংখ্যা) -> পূর্ণসংখ্যা:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            যখন k < n এবং _ends_with_bs(সারিগুলি[k - 1]):
                k += 1
            ফেরত k

        def after_header_সারি(pos: পূর্ণসংখ্যা) -> পূর্ণসংখ্যা:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            যখন k < n এবং _ends_with_bs(সারিগুলি[k - 1]):
                k += 1
            ফেরত k

        bodies: List[Tuple[str, পূর্ণসংখ্যা, পূর্ণসংখ্যা]] = []
        header_positions = [pos জন্য _, pos মধ্যে marks] + [j]
        tags = [tag জন্য tag, _ মধ্যে marks]
        জন্য idx, tag মধ্যে enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_সারি(header)
            পরবর্তী_header = header_positions[idx + 1]
            body_e = পরবর্তী_header
            bodies.append((tag, body_s, body_e))
        ফেরত j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    যখন i < n:
        সারি = সারিগুলি[i]
        m = _DIR_RE.match(সারি)
        যদি নয় m:
            out.append(সারি)
            i += 1
            চালু

        kw = m.group(1)
        যদি kw মধ্যে ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            যদি _IF0_RE.match(সারি) অথবা _IFDEF_DEBUG_RE.match(সারি) অথবা _IFNDEF_PROTO_RE.match(সারি) অথবা _IF_NOTDEF_PROTO_RE.match(সারি):
                group_end, _ = _collect_group(i)
                i = group_end
                চালু

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") এবং (
                _DEFINED_CALL_RE.অনুসন্ধান(সারি) is নয় None
            )

            যদি evaluate:
                # Extract condition text after 'if'
                চেষ্টা:
                    cond_text = সারি.split("if", 1)[1]
                ব্যতিক্রম Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                যদি নয় keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    চালু

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            যদি নয় bodies:
                i = group_end
                চালু
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(সারিগুলি[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            যদি DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                জন্য ln মধ্যে kept_text.splitlines():
                    যদি ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        বিরতি

            out.append(kept_rewritten)
            i = group_end
        অন্যথা:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" যদি সারি.endswith("\n") অন্যথা "")
            i += 1

    ফেরত "".join(out)

def collapse_star_spaces(s: str) -> str:
    ফেরত re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    ফেরত s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    ফেরত s

def normalize_ws(s: str) -> str:
    ফেরত re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t জন্য t মধ্যে cur.get_tokens() যদি t.kind != TokenKind.COMMENT]
    যদি নয় toks:
        ফেরত []
    name_idx = None
    জন্য i, t মধ্যে enumerate(toks):
        যদি t.kind == TokenKind.IDENTIFIER এবং t.spelling == cur.spelling:
            name_idx = i
            বিরতি
    যদি name_idx is None:
        ফেরত []
    lpar_idx = None
    জন্য i মধ্যে range(name_idx + 1, len(toks)):
        যদি toks[i].spelling == '(':
            lpar_idx = i
            বিরতি
    যদি lpar_idx is None:
        ফেরত []
    depth, rpar_idx = 0, None
    জন্য i মধ্যে range(lpar_idx, len(toks)):
        যদি toks[i].spelling == '(':
            depth += 1
        নতুবা_যদি toks[i].spelling == ')':
            depth -= 1
            যদি depth == 0:
                rpar_idx = i
                বিরতি
    যদি rpar_idx is None:
        ফেরত []
    ফেরত [t.spelling জন্য t মধ্যে toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    ফেরত s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    চেষ্টা:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling যদি ret is নয় None অন্যথা ""
    ব্যতিক্রম Exception:
        ret_sp = ""
    যদি নয় ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        চেষ্টা:
            ret_sp = cur.type.get_ফলাফল().spelling
        ব্যতিক্রম Exception:
            ret_sp = "void"
    name_sp = cur.spelling অথবা ""
    params = []
    চেষ্টা:
        জন্য a মধ্যে cur.get_arguments() অথবা []:
            t = getattr(a, "type", None)
            t_sp = t.spelling যদি t is নয় None অন্যথা ""
            a_sp = a.spelling অথবা ""
            seg = (t_sp + (" " + a_sp যদি a_sp অন্যথা "")).strip()
            params.append(seg যদি seg অন্যথা "void")
    ব্যতিক্রম Exception:
        pass
    params_s = ", ".join(params) যদি params অন্যথা "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    ফেরত s

def in_this_file(cur, src_path: Path) -> bool:
    চেষ্টা:
        loc = cur.extent.start
        f = (loc.file অথবা cur.location.file)
        যদি নয় f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            চেষ্টা:
                toks = tokens_for_function_header(cur)
                ফেরত bool(toks)
            ব্যতিক্রম Exception:
                ফেরত মিথ্যা
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        যদি cur_path == src_abs অথবা os.path.basename(cur_path) == os.path.basename(src_abs):
            ফেরত সত্য
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        যদি cur_path.endswith(".c") এবং os.path.dirname(cur_path) == src_dir:
            ফেরত সত্য
        ফেরত মিথ্যা
    ব্যতিক্রম Exception:
        ফেরত মিথ্যা

def collect_function_defs(tu, src_path: Path):
    def walk(গ্রন্থি, depth=0):
        যদি DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        যদি গ্রন্থি.kind == CursorKind.FUNCTION_DECL:
            যদি in_this_file(গ্রন্থি, src_path):
                যদি গ্রন্থি.storage_class != StorageClass.STATIC এবং গ্রন্থি.is_definition():
                    দাও গ্রন্থি
        জন্য ch মধ্যে গ্রন্থি.get_children():
            দাও তঃ walk(ch, depth+1)
    দাও তঃ walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    সূচক = সূচক_3.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    যদি proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args অথবা []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = সূচক.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    যদি SHOW_DIAGS:
        জন্য d মধ্যে getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    ফেরত tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    জন্য fn মধ্যে collect_function_defs(tu, src_path):
        যদি DEBUG_LOG:
            চেষ্টা:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            ব্যতিক্রম Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) যদি toks অন্যথা header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        যদি header:
            protos.append(f"{header};")

    seen, unique = set(), []
    জন্য p মধ্যে protos:
        যদি p নয় মধ্যে seen:
            unique.append(p)
            seen.add(p)
    ফেরত unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=সত্য, exist_ok=সত্য)
    চেষ্টা:
        with out_path.open("w", encoding="utf-8", newline="\n") ইব f:
            f.লিখ(f"// {src_name}\n")
            জন্য h মধ্যে headers:
                f.লিখ(h + "\n")
            f.লিখ("// vim: ft=c\n")
    ব্যতিক্রম Exception ইব e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def প্রধান():
    args = sys.argv[1:]
    যদি নয় args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    যদি নয় src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    চেষ্টা:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    ব্যতিক্রম Exception ইব e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    চেষ্টা:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    ব্যতিক্রম Exception ইব e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

যদি __name__ == "__main__":
    প্রধান()
