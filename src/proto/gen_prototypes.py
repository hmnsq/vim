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
واردکن os
واردکن re
واردکن sys
از pathlib واردکن Path
از typing واردکن List, Tuple
از clang.cindex واردکن Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") نه در ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") نه در ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(سطر: str) -> bool:
    بازگشت سطر.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    برای a در argv:
        اگر a == "-DPROTO" یا a.startswith("-DPROTO="):
            بازگشت درست
    بازگشت نادرست

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

    اگر DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    اگر proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    وگرنه:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    اگر proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    وگرنه:
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

    تلاش:
        بازگشت bool(eval(s, {}, {}))
    استثنا Exception:
        # Fail-open to avoid accidentally dropping code
        اگر DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        بازگشت درست

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
    سطرها = text.splitlines(keepends=درست)
    out: List[str] = []
    i, n = 0, len(سطرها)

    def _collect_group(start: صحیح) -> Tuple[صحیح, List[Tuple[str, صحیح, صحیح]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        وقتی درست:
            h += 1
            اگر h >= n یا نه _ends_with_bs(سطرها[h - 1]):
                بشکن
        depth = 1
        j = h
        marks: List[Tuple[str, صحیح]] = [("if", start)]
        وقتی j < n و depth > 0:
            m = _DIR_RE.match(سطرها[j])
            اگر m:
                kw2 = m.group(1)
                اگر kw2 در ("if", "ifdef", "ifndef"):
                    depth += 1
                وگرنهاگر kw2 == "endif":
                    depth -= 1
                    اگر depth == 0:
                        بشکن
                وگرنهاگر depth == 1 و kw2 در ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        وگرنه:
            # Unterminated: keep as-is from start to EOF
            بازگشت n, [("text", start, n)]

        def after_header_سطر(pos: صحیح) -> صحیح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            وقتی k < n و _ends_with_bs(سطرها[k - 1]):
                k += 1
            بازگشت k

        def after_header_سطر(pos: صحیح) -> صحیح:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            وقتی k < n و _ends_with_bs(سطرها[k - 1]):
                k += 1
            بازگشت k

        bodies: List[Tuple[str, صحیح, صحیح]] = []
        header_positions = [pos برای _, pos در marks] + [j]
        tags = [tag برای tag, _ در marks]
        برای idx, tag در enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_سطر(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        بازگشت j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    وقتی i < n:
        سطر = سطرها[i]
        m = _DIR_RE.match(سطر)
        اگر نه m:
            out.append(سطر)
            i += 1
            ادامه

        kw = m.group(1)
        اگر kw در ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            اگر _IF0_RE.match(سطر) یا _IFDEF_DEBUG_RE.match(سطر) یا _IFNDEF_PROTO_RE.match(سطر) یا _IF_NOTDEF_PROTO_RE.match(سطر):
                group_end, _ = _collect_group(i)
                i = group_end
                ادامه

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") و (
                _DEFINED_CALL_RE.جستجو(سطر) is نه None
            )

            اگر evaluate:
                # Extract condition text after 'if'
                تلاش:
                    cond_text = سطر.split("if", 1)[1]
                استثنا Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                اگر نه keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    ادامه

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            اگر نه bodies:
                i = group_end
                ادامه
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(سطرها[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            اگر DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                برای ln در kept_text.splitlines():
                    اگر ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        بشکن

            out.append(kept_rewritten)
            i = group_end
        وگرنه:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" اگر سطر.endswith("\n") وگرنه "")
            i += 1

    بازگشت "".join(out)

def collapse_star_spaces(s: str) -> str:
    بازگشت re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    بازگشت s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    بازگشت s

def normalize_ws(s: str) -> str:
    بازگشت re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t برای t در cur.get_tokens() اگر t.kind != TokenKind.COMMENT]
    اگر نه toks:
        بازگشت []
    name_idx = None
    برای i, t در enumerate(toks):
        اگر t.kind == TokenKind.IDENTIFIER و t.spelling == cur.spelling:
            name_idx = i
            بشکن
    اگر name_idx is None:
        بازگشت []
    lpar_idx = None
    برای i در range(name_idx + 1, len(toks)):
        اگر toks[i].spelling == '(':
            lpar_idx = i
            بشکن
    اگر lpar_idx is None:
        بازگشت []
    depth, rpar_idx = 0, None
    برای i در range(lpar_idx, len(toks)):
        اگر toks[i].spelling == '(':
            depth += 1
        وگرنهاگر toks[i].spelling == ')':
            depth -= 1
            اگر depth == 0:
                rpar_idx = i
                بشکن
    اگر rpar_idx is None:
        بازگشت []
    بازگشت [t.spelling برای t در toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    بازگشت s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    تلاش:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling اگر ret is نه None وگرنه ""
    استثنا Exception:
        ret_sp = ""
    اگر نه ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        تلاش:
            ret_sp = cur.type.get_result().spelling
        استثنا Exception:
            ret_sp = "void"
    name_sp = cur.spelling یا ""
    params = []
    تلاش:
        برای a در cur.get_arguments() یا []:
            t = getattr(a, "type", None)
            t_sp = t.spelling اگر t is نه None وگرنه ""
            a_sp = a.spelling یا ""
            seg = (t_sp + (" " + a_sp اگر a_sp وگرنه "")).strip()
            params.append(seg اگر seg وگرنه "void")
    استثنا Exception:
        pass
    params_s = ", ".join(params) اگر params وگرنه "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    بازگشت s

def in_this_file(cur, src_path: Path) -> bool:
    تلاش:
        loc = cur.extent.start
        f = (loc.file یا cur.location.file)
        اگر نه f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            تلاش:
                toks = tokens_for_function_header(cur)
                بازگشت bool(toks)
            استثنا Exception:
                بازگشت نادرست
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        اگر cur_path == src_abs یا os.path.basename(cur_path) == os.path.basename(src_abs):
            بازگشت درست
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        اگر cur_path.endswith(".c") و os.path.dirname(cur_path) == src_dir:
            بازگشت درست
        بازگشت نادرست
    استثنا Exception:
        بازگشت نادرست

def collect_function_defs(tu, src_path: Path):
    def walk(گره, depth=0):
        اگر DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        اگر گره.kind == CursorKind.FUNCTION_DECL:
            اگر in_this_file(گره, src_path):
                اگر گره.storage_class != StorageClass.STATIC و گره.is_definition():
                    بده گره
        برای ch در گره.get_children():
            بده از walk(ch, depth+1)
    بده از walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    اگر proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args یا []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    اگر SHOW_DIAGS:
        برای d در getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    بازگشت tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    برای fn در collect_function_defs(tu, src_path):
        اگر DEBUG_LOG:
            تلاش:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            استثنا Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) اگر toks وگرنه header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        اگر header:
            protos.append(f"{header};")

    seen, unique = set(), []
    برای p در protos:
        اگر p نه در seen:
            unique.append(p)
            seen.add(p)
    بازگشت unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=درست, exist_ok=درست)
    تلاش:
        with out_path.open("w", encoding="utf-8", newline="\n") بهعنوان f:
            f.اكتب(f"// {src_name}\n")
            برای h در headers:
                f.اكتب(h + "\n")
            f.اكتب("// vim: ft=c\n")
    استثنا Exception بهعنوان e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def اصلی():
    args = sys.argv[1:]
    اگر نه args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    اگر نه src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    تلاش:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    استثنا Exception بهعنوان e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    تلاش:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    استثنا Exception بهعنوان e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

اگر __name__ == "__main__":
    اصلی()
