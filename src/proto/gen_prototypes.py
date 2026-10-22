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
आयात os
आयात re
आयात sys
तः pathlib आयात Path
तः typing आयात List, Tuple
तः clang.cindex आयात Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") नाही मध्ये ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") नाही मध्ये ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(ओळ: str) -> bool:
    परत ओळ.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    साठी a मध्ये argv:
        जर a == "-DPROTO" किंवा a.startswith("-DPROTO="):
            परत सत्य
    परत असत्य

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

    जर DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    जर proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    अन्यथा:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    जर proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    अन्यथा:
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

    प्रयत्न:
        परत bool(eval(s, {}, {}))
    अपवाद Exception:
        # Fail-open to avoid accidentally dropping code
        जर DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        परत सत्य

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
    ओळी = text.splitlines(keepends=सत्य)
    out: List[str] = []
    i, n = 0, len(ओळी)

    def _collect_group(start: पूर्णांक) -> Tuple[पूर्णांक, List[Tuple[str, पूर्णांक, पूर्णांक]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        पर्यंत सत्य:
            h += 1
            जर h >= n किंवा नाही _ends_with_bs(ओळी[h - 1]):
                थांबा
        depth = 1
        j = h
        marks: List[Tuple[str, पूर्णांक]] = [("if", start)]
        पर्यंत j < n आणि depth > 0:
            m = _DIR_RE.match(ओळी[j])
            जर m:
                kw2 = m.group(1)
                जर kw2 मध्ये ("if", "ifdef", "ifndef"):
                    depth += 1
                किंवाजर kw2 == "endif":
                    depth -= 1
                    जर depth == 0:
                        थांबा
                किंवाजर depth == 1 आणि kw2 मध्ये ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        अन्यथा:
            # Unterminated: keep as-is from start to EOF
            परत n, [("text", start, n)]

        def after_header_ओळ(pos: पूर्णांक) -> पूर्णांक:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            पर्यंत k < n आणि _ends_with_bs(ओळी[k - 1]):
                k += 1
            परत k

        def after_header_ओळ(pos: पूर्णांक) -> पूर्णांक:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            पर्यंत k < n आणि _ends_with_bs(ओळी[k - 1]):
                k += 1
            परत k

        bodies: List[Tuple[str, पूर्णांक, पूर्णांक]] = []
        header_positions = [pos साठी _, pos मध्ये marks] + [j]
        tags = [tag साठी tag, _ मध्ये marks]
        साठी idx, tag मध्ये enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_ओळ(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        परत j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    पर्यंत i < n:
        ओळ = ओळी[i]
        m = _DIR_RE.match(ओळ)
        जर नाही m:
            out.append(ओळ)
            i += 1
            सुरूठेवा

        kw = m.group(1)
        जर kw मध्ये ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            जर _IF0_RE.match(ओळ) किंवा _IFDEF_DEBUG_RE.match(ओळ) किंवा _IFNDEF_PROTO_RE.match(ओळ) किंवा _IF_NOTDEF_PROTO_RE.match(ओळ):
                group_end, _ = _collect_group(i)
                i = group_end
                सुरूठेवा

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") आणि (
                _DEFINED_CALL_RE.शोध(ओळ) is नाही None
            )

            जर evaluate:
                # Extract condition text after 'if'
                प्रयत्न:
                    cond_text = ओळ.split("if", 1)[1]
                अपवाद Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                जर नाही keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    सुरूठेवा

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            जर नाही bodies:
                i = group_end
                सुरूठेवा
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(ओळी[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            जर DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                साठी ln मध्ये kept_text.splitlines():
                    जर ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        थांबा

            out.append(kept_rewritten)
            i = group_end
        अन्यथा:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" जर ओळ.endswith("\n") अन्यथा "")
            i += 1

    परत "".join(out)

def collapse_star_spaces(s: str) -> str:
    परत re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    परत s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    परत s

def normalize_ws(s: str) -> str:
    परत re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t साठी t मध्ये cur.get_tokens() जर t.kind != TokenKind.COMMENT]
    जर नाही toks:
        परत []
    name_idx = None
    साठी i, t मध्ये enumerate(toks):
        जर t.kind == TokenKind.IDENTIFIER आणि t.spelling == cur.spelling:
            name_idx = i
            थांबा
    जर name_idx is None:
        परत []
    lpar_idx = None
    साठी i मध्ये range(name_idx + 1, len(toks)):
        जर toks[i].spelling == '(':
            lpar_idx = i
            थांबा
    जर lpar_idx is None:
        परत []
    depth, rpar_idx = 0, None
    साठी i मध्ये range(lpar_idx, len(toks)):
        जर toks[i].spelling == '(':
            depth += 1
        किंवाजर toks[i].spelling == ')':
            depth -= 1
            जर depth == 0:
                rpar_idx = i
                थांबा
    जर rpar_idx is None:
        परत []
    परत [t.spelling साठी t मध्ये toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    परत s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    प्रयत्न:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling जर ret is नाही None अन्यथा ""
    अपवाद Exception:
        ret_sp = ""
    जर नाही ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        प्रयत्न:
            ret_sp = cur.type.get_result().spelling
        अपवाद Exception:
            ret_sp = "void"
    name_sp = cur.spelling किंवा ""
    params = []
    प्रयत्न:
        साठी a मध्ये cur.get_arguments() किंवा []:
            t = getattr(a, "type", None)
            t_sp = t.spelling जर t is नाही None अन्यथा ""
            a_sp = a.spelling किंवा ""
            seg = (t_sp + (" " + a_sp जर a_sp अन्यथा "")).strip()
            params.append(seg जर seg अन्यथा "void")
    अपवाद Exception:
        pass
    params_s = ", ".join(params) जर params अन्यथा "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    परत s

def in_this_file(cur, src_path: Path) -> bool:
    प्रयत्न:
        loc = cur.extent.start
        f = (loc.file किंवा cur.location.file)
        जर नाही f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            प्रयत्न:
                toks = tokens_for_function_header(cur)
                परत bool(toks)
            अपवाद Exception:
                परत असत्य
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        जर cur_path == src_abs किंवा os.path.basename(cur_path) == os.path.basename(src_abs):
            परत सत्य
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        जर cur_path.endswith(".c") आणि os.path.dirname(cur_path) == src_dir:
            परत सत्य
        परत असत्य
    अपवाद Exception:
        परत असत्य

def collect_function_defs(tu, src_path: Path):
    def walk(गाठ, depth=0):
        जर DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        जर गाठ.kind == CursorKind.FUNCTION_DECL:
            जर in_this_file(गाठ, src_path):
                जर गाठ.storage_class != StorageClass.STATIC आणि गाठ.is_definition():
                    द्या गाठ
        साठी ch मध्ये गाठ.get_children():
            द्या तः walk(ch, depth+1)
    द्या तः walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    जर proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args किंवा []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    जर SHOW_DIAGS:
        साठी d मध्ये getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    परत tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    साठी fn मध्ये collect_function_defs(tu, src_path):
        जर DEBUG_LOG:
            प्रयत्न:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            अपवाद Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) जर toks अन्यथा header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        जर header:
            protos.append(f"{header};")

    seen, unique = set(), []
    साठी p मध्ये protos:
        जर p नाही मध्ये seen:
            unique.append(p)
            seen.add(p)
    परत unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=सत्य, exist_ok=सत्य)
    प्रयत्न:
        with out_path.open("w", encoding="utf-8", newline="\n") इव f:
            f.लिख(f"// {src_name}\n")
            साठी h मध्ये headers:
                f.लिख(h + "\n")
            f.लिख("// vim: ft=c\n")
    अपवाद Exception इव e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def main():
    args = sys.argv[1:]
    जर नाही args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    जर नाही src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    प्रयत्न:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    अपवाद Exception इव e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    प्रयत्न:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    अपवाद Exception इव e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

जर __name__ == "__main__":
    main()
