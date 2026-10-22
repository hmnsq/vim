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
આયાત os
આયાત re
આયાત sys
તઃ pathlib આયાત Path
તઃ typing આયાત List, Tuple
તઃ clang.cindex આયાત Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") નહીં માં ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") નહીં માં ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(લીટી: str) -> bool:
    પાછા લીટી.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    માટે a માં argv:
        જો a == "-DPROTO" અથવા a.startswith("-DPROTO="):
            પાછા સાચું
    પાછા ખોટું

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

    જો DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    જો proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    નહીંતર:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    જો proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    નહીંતર:
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

    પ્રયાસ:
        પાછા bool(eval(s, {}, {}))
    અપવાદ Exception:
        # Fail-open to avoid accidentally dropping code
        જો DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        પાછા સાચું

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
    લીટીઓ = text.splitlines(keepends=સાચું)
    out: List[str] = []
    i, n = 0, len(લીટીઓ)

    def _collect_group(start: પૂર્ણાંક) -> Tuple[પૂર્ણાંક, List[Tuple[str, પૂર્ણાંક, પૂર્ણાંક]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        જ્યારે સાચું:
            h += 1
            જો h >= n અથવા નહીં _ends_with_bs(લીટીઓ[h - 1]):
                તોડો
        depth = 1
        j = h
        marks: List[Tuple[str, પૂર્ણાંક]] = [("if", start)]
        જ્યારે j < n અને depth > 0:
            m = _DIR_RE.match(લીટીઓ[j])
            જો m:
                kw2 = m.group(1)
                જો kw2 માં ("if", "ifdef", "ifndef"):
                    depth += 1
                અથવાજો kw2 == "endif":
                    depth -= 1
                    જો depth == 0:
                        તોડો
                અથવાજો depth == 1 અને kw2 માં ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        નહીંતર:
            # Unterminated: keep as-is from start to EOF
            પાછા n, [("text", start, n)]

        def after_header_લીટી(pos: પૂર્ણાંક) -> પૂર્ણાંક:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            જ્યારે k < n અને _ends_with_bs(લીટીઓ[k - 1]):
                k += 1
            પાછા k

        def after_header_લીટી(pos: પૂર્ણાંક) -> પૂર્ણાંક:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            જ્યારે k < n અને _ends_with_bs(લીટીઓ[k - 1]):
                k += 1
            પાછા k

        bodies: List[Tuple[str, પૂર્ણાંક, પૂર્ણાંક]] = []
        header_positions = [pos માટે _, pos માં marks] + [j]
        tags = [tag માટે tag, _ માં marks]
        માટે idx, tag માં enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_લીટી(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        પાછા j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    જ્યારે i < n:
        લીટી = લીટીઓ[i]
        m = _DIR_RE.match(લીટી)
        જો નહીં m:
            out.append(લીટી)
            i += 1
            ચાલુ

        kw = m.group(1)
        જો kw માં ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            જો _IF0_RE.match(લીટી) અથવા _IFDEF_DEBUG_RE.match(લીટી) અથવા _IFNDEF_PROTO_RE.match(લીટી) અથવા _IF_NOTDEF_PROTO_RE.match(લીટી):
                group_end, _ = _collect_group(i)
                i = group_end
                ચાલુ

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") અને (
                _DEFINED_CALL_RE.શોધ(લીટી) is નહીં None
            )

            જો evaluate:
                # Extract condition text after 'if'
                પ્રયાસ:
                    cond_text = લીટી.split("if", 1)[1]
                અપવાદ Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                જો નહીં keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    ચાલુ

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            જો નહીં bodies:
                i = group_end
                ચાલુ
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(લીટીઓ[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            જો DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                માટે ln માં kept_text.splitlines():
                    જો ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        તોડો

            out.append(kept_rewritten)
            i = group_end
        નહીંતર:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" જો લીટી.endswith("\n") નહીંતર "")
            i += 1

    પાછા "".join(out)

def collapse_star_spaces(s: str) -> str:
    પાછા re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    પાછા s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    પાછા s

def normalize_ws(s: str) -> str:
    પાછા re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t માટે t માં cur.get_tokens() જો t.kind != TokenKind.COMMENT]
    જો નહીં toks:
        પાછા []
    name_idx = None
    માટે i, t માં enumerate(toks):
        જો t.kind == TokenKind.IDENTIFIER અને t.spelling == cur.spelling:
            name_idx = i
            તોડો
    જો name_idx is None:
        પાછા []
    lpar_idx = None
    માટે i માં range(name_idx + 1, len(toks)):
        જો toks[i].spelling == '(':
            lpar_idx = i
            તોડો
    જો lpar_idx is None:
        પાછા []
    depth, rpar_idx = 0, None
    માટે i માં range(lpar_idx, len(toks)):
        જો toks[i].spelling == '(':
            depth += 1
        અથવાજો toks[i].spelling == ')':
            depth -= 1
            જો depth == 0:
                rpar_idx = i
                તોડો
    જો rpar_idx is None:
        પાછા []
    પાછા [t.spelling માટે t માં toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    પાછા s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    પ્રયાસ:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling જો ret is નહીં None નહીંતર ""
    અપવાદ Exception:
        ret_sp = ""
    જો નહીં ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        પ્રયાસ:
            ret_sp = cur.type.get_result().spelling
        અપવાદ Exception:
            ret_sp = "void"
    name_sp = cur.spelling અથવા ""
    params = []
    પ્રયાસ:
        માટે a માં cur.get_arguments() અથવા []:
            t = getattr(a, "type", None)
            t_sp = t.spelling જો t is નહીં None નહીંતર ""
            a_sp = a.spelling અથવા ""
            seg = (t_sp + (" " + a_sp જો a_sp નહીંતર "")).strip()
            params.append(seg જો seg નહીંતર "void")
    અપવાદ Exception:
        pass
    params_s = ", ".join(params) જો params નહીંતર "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    પાછા s

def in_this_file(cur, src_path: Path) -> bool:
    પ્રયાસ:
        loc = cur.extent.start
        f = (loc.file અથવા cur.location.file)
        જો નહીં f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            પ્રયાસ:
                toks = tokens_for_function_header(cur)
                પાછા bool(toks)
            અપવાદ Exception:
                પાછા ખોટું
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        જો cur_path == src_abs અથવા os.path.basename(cur_path) == os.path.basename(src_abs):
            પાછા સાચું
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        જો cur_path.endswith(".c") અને os.path.dirname(cur_path) == src_dir:
            પાછા સાચું
        પાછા ખોટું
    અપવાદ Exception:
        પાછા ખોટું

def collect_function_defs(tu, src_path: Path):
    def walk(ગાંઠ, depth=0):
        જો DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        જો ગાંઠ.kind == CursorKind.FUNCTION_DECL:
            જો in_this_file(ગાંઠ, src_path):
                જો ગાંઠ.storage_class != StorageClass.STATIC અને ગાંઠ.is_definition():
                    આપો ગાંઠ
        માટે ch માં ગાંઠ.get_children():
            આપો તઃ walk(ch, depth+1)
    આપો તઃ walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    જો proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args અથવા []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    જો SHOW_DIAGS:
        માટે d માં getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    પાછા tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    માટે fn માં collect_function_defs(tu, src_path):
        જો DEBUG_LOG:
            પ્રયાસ:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            અપવાદ Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) જો toks નહીંતર header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        જો header:
            protos.append(f"{header};")

    seen, unique = set(), []
    માટે p માં protos:
        જો p નહીં માં seen:
            unique.append(p)
            seen.add(p)
    પાછા unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=સાચું, exist_ok=સાચું)
    પ્રયાસ:
        with out_path.open("w", encoding="utf-8", newline="\n") ઇવ f:
            f.લિખ(f"// {src_name}\n")
            માટે h માં headers:
                f.લિખ(h + "\n")
            f.લિખ("// vim: ft=c\n")
    અપવાદ Exception ઇવ e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def મુખ્ય():
    args = sys.argv[1:]
    જો નહીં args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    જો નહીં src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    પ્રયાસ:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    અપવાદ Exception ઇવ e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    પ્રયાસ:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    અપવાદ Exception ઇવ e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

જો __name__ == "__main__":
    મુખ્ય()
