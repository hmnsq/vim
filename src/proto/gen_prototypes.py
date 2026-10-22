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
आनयन os
आनयन re
आनयन sys
तः pathlib आनयन Path
तः typing आनयन List, Tuple
तः clang.cindex आनयन Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") ण अन्तः ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") ण अन्तः ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(पङ्क्तिः: str) -> bool:
    प्रत्यागम पङ्क्तिः.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    प्रति a अन्तः argv:
        जइ a == "-DPROTO" वा a.startswith("-DPROTO="):
            प्रत्यागम सच्च
    प्रत्यागम मिच्छ

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

    जइ DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    जइ proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    अण्णहा:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    जइ proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    अण्णहा:
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

    यत्न:
        प्रत्यागम bool(eval(s, {}, {}))
    अपवाद Exception:
        # Fail-open to avoid accidentally dropping code
        जइ DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        प्रत्यागम सच्च

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
    पङ्क्तयः = text.splitlines(keepends=सच्च)
    out: List[str] = []
    i, n = 0, len(पङ्क्तयः)

    def _collect_group(start: पूर्णाङ्क) -> Tuple[पूर्णाङ्क, List[Tuple[str, पूर्णाङ्क, पूर्णाङ्क]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        जाव सच्च:
            h += 1
            जइ h >= n वा ण _ends_with_bs(पङ्क्तयः[h - 1]):
                भङ्ग
        depth = 1
        j = h
        marks: List[Tuple[str, पूर्णाङ्क]] = [("if", start)]
        जाव j < n य depth > 0:
            m = _DIR_RE.match(पङ्क्तयः[j])
            जइ m:
                kw2 = m.group(1)
                जइ kw2 अन्तः ("if", "ifdef", "ifndef"):
                    depth += 1
                नोचेद्यदि kw2 == "endif":
                    depth -= 1
                    जइ depth == 0:
                        भङ्ग
                नोचेद्यदि depth == 1 य kw2 अन्तः ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        अण्णहा:
            # Unterminated: keep as-is from start to EOF
            प्रत्यागम n, [("text", start, n)]

        def after_header_पङ्क्तिः(pos: पूर्णाङ्क) -> पूर्णाङ्क:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            जाव k < n य _ends_with_bs(पङ्क्तयः[k - 1]):
                k += 1
            प्रत्यागम k

        def after_header_पङ्क्तिः(pos: पूर्णाङ्क) -> पूर्णाङ्क:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            जाव k < n य _ends_with_bs(पङ्क्तयः[k - 1]):
                k += 1
            प्रत्यागम k

        bodies: List[Tuple[str, पूर्णाङ्क, पूर्णाङ्क]] = []
        header_positions = [pos प्रति _, pos अन्तः marks] + [j]
        tags = [tag प्रति tag, _ अन्तः marks]
        प्रति idx, tag अन्तः enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_पङ्क्तिः(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        प्रत्यागम j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    जाव i < n:
        पङ्क्तिः = पङ्क्तयः[i]
        m = _DIR_RE.match(पङ्क्तिः)
        जइ ण m:
            out.append(पङ्क्तिः)
            i += 1
            अनुवर्त

        kw = m.group(1)
        जइ kw अन्तः ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            जइ _IF0_RE.match(पङ्क्तिः) वा _IFDEF_DEBUG_RE.match(पङ्क्तिः) वा _IFNDEF_PROTO_RE.match(पङ्क्तिः) वा _IF_NOTDEF_PROTO_RE.match(पङ्क्तिः):
                group_end, _ = _collect_group(i)
                i = group_end
                अनुवर्त

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") य (
                _DEFINED_CALL_RE.अन्वेषणम्(पङ्क्तिः) is ण None
            )

            जइ evaluate:
                # Extract condition text after 'if'
                यत्न:
                    cond_text = पङ्क्तिः.split("if", 1)[1]
                अपवाद Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                जइ ण keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    अनुवर्त

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            जइ ण bodies:
                i = group_end
                अनुवर्त
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(पङ्क्तयः[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            जइ DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                प्रति ln अन्तः kept_text.splitlines():
                    जइ ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        भङ्ग

            out.append(kept_rewritten)
            i = group_end
        अण्णहा:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" जइ पङ्क्तिः.endswith("\n") अण्णहा "")
            i += 1

    प्रत्यागम "".join(out)

def collapse_star_spaces(s: str) -> str:
    प्रत्यागम re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    प्रत्यागम s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    प्रत्यागम s

def normalize_ws(s: str) -> str:
    प्रत्यागम re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t प्रति t अन्तः cur.get_tokens() जइ t.kind != TokenKind.COMMENT]
    जइ ण toks:
        प्रत्यागम []
    name_idx = None
    प्रति i, t अन्तः enumerate(toks):
        जइ t.kind == TokenKind.IDENTIFIER य t.spelling == cur.spelling:
            name_idx = i
            भङ्ग
    जइ name_idx is None:
        प्रत्यागम []
    lpar_idx = None
    प्रति i अन्तः range(name_idx + 1, len(toks)):
        जइ toks[i].spelling == '(':
            lpar_idx = i
            भङ्ग
    जइ lpar_idx is None:
        प्रत्यागम []
    depth, rpar_idx = 0, None
    प्रति i अन्तः range(lpar_idx, len(toks)):
        जइ toks[i].spelling == '(':
            depth += 1
        नोचेद्यदि toks[i].spelling == ')':
            depth -= 1
            जइ depth == 0:
                rpar_idx = i
                भङ्ग
    जइ rpar_idx is None:
        प्रत्यागम []
    प्रत्यागम [t.spelling प्रति t अन्तः toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    प्रत्यागम s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    यत्न:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling जइ ret is ण None अण्णहा ""
    अपवाद Exception:
        ret_sp = ""
    जइ ण ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        यत्न:
            ret_sp = cur.type.get_result().spelling
        अपवाद Exception:
            ret_sp = "void"
    name_sp = cur.spelling वा ""
    params = []
    यत्न:
        प्रति a अन्तः cur.get_arguments() वा []:
            t = getattr(a, "type", None)
            t_sp = t.spelling जइ t is ण None अण्णहा ""
            a_sp = a.spelling वा ""
            seg = (t_sp + (" " + a_sp जइ a_sp अण्णहा "")).strip()
            params.append(seg जइ seg अण्णहा "void")
    अपवाद Exception:
        pass
    params_s = ", ".join(params) जइ params अण्णहा "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    प्रत्यागम s

def in_this_file(cur, src_path: Path) -> bool:
    यत्न:
        loc = cur.extent.start
        f = (loc.file वा cur.location.file)
        जइ ण f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            यत्न:
                toks = tokens_for_function_header(cur)
                प्रत्यागम bool(toks)
            अपवाद Exception:
                प्रत्यागम मिच्छ
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        जइ cur_path == src_abs वा os.path.basename(cur_path) == os.path.basename(src_abs):
            प्रत्यागम सच्च
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        जइ cur_path.endswith(".c") य os.path.dirname(cur_path) == src_dir:
            प्रत्यागम सच्च
        प्रत्यागम मिच्छ
    अपवाद Exception:
        प्रत्यागम मिच्छ

def collect_function_defs(tu, src_path: Path):
    def walk(ग्रन्थिः, depth=0):
        जइ DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        जइ ग्रन्थिः.kind == CursorKind.FUNCTION_DECL:
            जइ in_this_file(ग्रन्थिः, src_path):
                जइ ग्रन्थिः.storage_class != StorageClass.STATIC य ग्रन्थिः.is_definition():
                    दा ग्रन्थिः
        प्रति ch अन्तः ग्रन्थिः.get_children():
            दा तः walk(ch, depth+1)
    दा तः walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    जइ proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args वा []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    जइ SHOW_DIAGS:
        प्रति d अन्तः getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    प्रत्यागम tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    प्रति fn अन्तः collect_function_defs(tu, src_path):
        जइ DEBUG_LOG:
            यत्न:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            अपवाद Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) जइ toks अण्णहा header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        जइ header:
            protos.append(f"{header};")

    seen, unique = set(), []
    प्रति p अन्तः protos:
        जइ p ण अन्तः seen:
            unique.append(p)
            seen.add(p)
    प्रत्यागम unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=सच्च, exist_ok=सच्च)
    यत्न:
        with out_path.open("w", encoding="utf-8", newline="\n") इव f:
            f.लिख(f"// {src_name}\n")
            प्रति h अन्तः headers:
                f.लिख(h + "\n")
            f.लिख("// vim: ft=c\n")
    अपवाद Exception इव e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def main():
    args = sys.argv[1:]
    जइ ण args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    जइ ण src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    यत्न:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    अपवाद Exception इव e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    यत्न:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    अपवाद Exception इव e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

जइ __name__ == "__main__":
    main()
