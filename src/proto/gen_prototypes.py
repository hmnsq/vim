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
من pathlib استورد Path
من typing استورد List, Tuple
من clang.cindex استورد Index, CursorKind, StorageClass, TokenKind

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

def _ends_with_bs(سطر: str) -> bool:
    إرجاع سطر.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    لكل a في argv:
        إذا a == "-DPROTO" أو a.startswith("-DPROTO="):
            إرجاع صحيح
    إرجاع خطأ

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

    إذا DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

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
        إرجاع bool(eval(s, {}, {}))
    استثناء Exception:
        # Fail-open to avoid accidentally dropping code
        إذا DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        إرجاع صحيح

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
    أسطر = text.splitlines(keepends=صحيح)
    out: List[str] = []
    i, n = 0, len(أسطر)

    def _collect_group(start: int) -> Tuple[int, List[Tuple[str, int, int]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        بينما صحيح:
            h += 1
            إذا h >= n أو ليس _ends_with_bs(أسطر[h - 1]):
                اكسر
        depth = 1
        j = h
        marks: List[Tuple[str, int]] = [("if", start)]
        بينما j < n و depth > 0:
            m = _DIR_RE.match(أسطر[j])
            إذا m:
                kw2 = m.group(1)
                إذا kw2 في ("if", "ifdef", "ifndef"):
                    depth += 1
                وإلاإذا kw2 == "endif":
                    depth -= 1
                    إذا depth == 0:
                        اكسر
                وإلاإذا depth == 1 و kw2 في ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        وإلا:
            # Unterminated: keep as-is from start to EOF
            إرجاع n, [("text", start, n)]

        def after_header_سطر(pos: int) -> int:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            بينما k < n و _ends_with_bs(أسطر[k - 1]):
                k += 1
            إرجاع k

        def after_header_سطر(pos: int) -> int:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            بينما k < n و _ends_with_bs(أسطر[k - 1]):
                k += 1
            إرجاع k

        bodies: List[Tuple[str, int, int]] = []
        header_positions = [pos لكل _, pos في marks] + [j]
        tags = [tag لكل tag, _ في marks]
        لكل idx, tag في enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_سطر(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        إرجاع j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    بينما i < n:
        سطر = أسطر[i]
        m = _DIR_RE.match(سطر)
        إذا ليس m:
            out.append(سطر)
            i += 1
            استمر

        kw = m.group(1)
        إذا kw في ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            إذا _IF0_RE.match(سطر) أو _IFDEF_DEBUG_RE.match(سطر) أو _IFNDEF_PROTO_RE.match(سطر) أو _IF_NOTDEF_PROTO_RE.match(سطر):
                group_end, _ = _collect_group(i)
                i = group_end
                استمر

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") و (
                _DEFINED_CALL_RE.بحث(سطر) is ليس None
            )

            إذا evaluate:
                # Extract condition text after 'if'
                حاول:
                    cond_text = سطر.split("if", 1)[1]
                استثناء Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                إذا ليس keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    استمر

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            إذا ليس bodies:
                i = group_end
                استمر
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(أسطر[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            إذا DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                لكل ln في kept_text.splitlines():
                    إذا ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        اكسر

            out.append(kept_rewritten)
            i = group_end
        وإلا:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" إذا سطر.endswith("\n") وإلا "")
            i += 1

    إرجاع "".join(out)

def collapse_star_spaces(s: str) -> str:
    إرجاع re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    إرجاع s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    إرجاع s

def normalize_ws(s: str) -> str:
    إرجاع re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t لكل t في cur.get_tokens() إذا t.kind != TokenKind.COMMENT]
    إذا ليس toks:
        إرجاع []
    name_idx = None
    لكل i, t في enumerate(toks):
        إذا t.kind == TokenKind.IDENTIFIER و t.spelling == cur.spelling:
            name_idx = i
            اكسر
    إذا name_idx is None:
        إرجاع []
    lpar_idx = None
    لكل i في range(name_idx + 1, len(toks)):
        إذا toks[i].spelling == '(':
            lpar_idx = i
            اكسر
    إذا lpar_idx is None:
        إرجاع []
    depth, rpar_idx = 0, None
    لكل i في range(lpar_idx, len(toks)):
        إذا toks[i].spelling == '(':
            depth += 1
        وإلاإذا toks[i].spelling == ')':
            depth -= 1
            إذا depth == 0:
                rpar_idx = i
                اكسر
    إذا rpar_idx is None:
        إرجاع []
    إرجاع [t.spelling لكل t في toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    إرجاع s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    حاول:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling إذا ret is ليس None وإلا ""
    استثناء Exception:
        ret_sp = ""
    إذا ليس ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        حاول:
            ret_sp = cur.type.get_result().spelling
        استثناء Exception:
            ret_sp = "void"
    name_sp = cur.spelling أو ""
    params = []
    حاول:
        لكل a في cur.get_arguments() أو []:
            t = getattr(a, "type", None)
            t_sp = t.spelling إذا t is ليس None وإلا ""
            a_sp = a.spelling أو ""
            seg = (t_sp + (" " + a_sp إذا a_sp وإلا "")).strip()
            params.append(seg إذا seg وإلا "void")
    استثناء Exception:
        pass
    params_s = ", ".join(params) إذا params وإلا "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    إرجاع s

def in_this_file(cur, src_path: Path) -> bool:
    حاول:
        loc = cur.extent.start
        f = (loc.file أو cur.location.file)
        إذا ليس f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            حاول:
                toks = tokens_for_function_header(cur)
                إرجاع bool(toks)
            استثناء Exception:
                إرجاع خطأ
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        إذا cur_path == src_abs أو os.path.basename(cur_path) == os.path.basename(src_abs):
            إرجاع صحيح
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        إذا cur_path.endswith(".c") و os.path.dirname(cur_path) == src_dir:
            إرجاع صحيح
        إرجاع خطأ
    استثناء Exception:
        إرجاع خطأ

def collect_function_defs(tu, src_path: Path):
    def walk(عقدة, depth=0):
        إذا DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        إذا عقدة.kind == CursorKind.FUNCTION_DECL:
            إذا in_this_file(عقدة, src_path):
                إذا عقدة.storage_class != StorageClass.STATIC و عقدة.is_definition():
                    أنتج عقدة
        لكل ch في عقدة.get_children():
            أنتج من walk(ch, depth+1)
    أنتج من walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    إذا proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args أو []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    إذا SHOW_DIAGS:
        لكل d في getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    إرجاع tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    لكل fn في collect_function_defs(tu, src_path):
        إذا DEBUG_LOG:
            حاول:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            استثناء Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) إذا toks وإلا header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        إذا header:
            protos.append(f"{header};")

    seen, unique = set(), []
    لكل p في protos:
        إذا p ليس في seen:
            unique.append(p)
            seen.add(p)
    إرجاع unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=صحيح, exist_ok=صحيح)
    حاول:
        with out_path.open("w", encoding="utf-8", newline="\n") كـ f:
            f.اكتب(f"// {src_name}\n")
            لكل h في headers:
                f.اكتب(h + "\n")
            f.اكتب("// vim: ft=c\n")
    استثناء Exception كـ e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def الرئيسية():
    args = sys.argv[1:]
    إذا ليس args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    إذا ليس src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    حاول:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    استثناء Exception كـ e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    حاول:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    استثناء Exception كـ e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

إذا __name__ == "__main__":
    الرئيسية()
