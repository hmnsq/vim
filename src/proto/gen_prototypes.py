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
இறக்குமதி os
இறக்குமதி re
இறக்குமதி sys
தஃ pathlib இறக்குமதி Path
தஃ typing இறக்குமதி List, Tuple
தஃ clang.cindex இறக்குமதி Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") இல்லை உள் ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") இல்லை உள் ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(வரி: str) -> bool:
    திரும்பு வரி.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    க்கு a உள் argv:
        எனில் a == "-DPROTO" அல்லது a.startswith("-DPROTO="):
            திரும்பு உண்மை
    திரும்பு பொய்

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

    எனில் DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    எனில் proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    இல்லையெனில்:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    எனில் proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    இல்லையெனில்:
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

    முயல்:
        திரும்பு bool(eval(s, {}, {}))
    விதிவிலக்கு Exception:
        # Fail-open to avoid accidentally dropping code
        எனில் DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        திரும்பு உண்மை

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
    வரிகள் = text.splitlines(keepends=உண்மை)
    out: List[str] = []
    i, n = 0, len(வரிகள்)

    def _collect_group(start: முழு) -> Tuple[முழு, List[Tuple[str, முழு, முழு]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        போது உண்மை:
            h += 1
            எனில் h >= n அல்லது இல்லை _ends_with_bs(வரிகள்[h - 1]):
                நிறுத்து
        depth = 1
        j = h
        marks: List[Tuple[str, முழு]] = [("if", start)]
        போது j < n மற்றும் depth > 0:
            m = _DIR_RE.match(வரிகள்[j])
            எனில் m:
                kw2 = m.group(1)
                எனில் kw2 உள் ("if", "ifdef", "ifndef"):
                    depth += 1
                அல்லதெனில் kw2 == "endif":
                    depth -= 1
                    எனில் depth == 0:
                        நிறுத்து
                அல்லதெனில் depth == 1 மற்றும் kw2 உள் ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        இல்லையெனில்:
            # Unterminated: keep as-is from start to EOF
            திரும்பு n, [("text", start, n)]

        def after_header_வரி(pos: முழு) -> முழு:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            போது k < n மற்றும் _ends_with_bs(வரிகள்[k - 1]):
                k += 1
            திரும்பு k

        def after_header_வரி(pos: முழு) -> முழு:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            போது k < n மற்றும் _ends_with_bs(வரிகள்[k - 1]):
                k += 1
            திரும்பு k

        bodies: List[Tuple[str, முழு, முழு]] = []
        header_positions = [pos க்கு _, pos உள் marks] + [j]
        tags = [tag க்கு tag, _ உள் marks]
        க்கு idx, tag உள் enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_வரி(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        திரும்பு j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    போது i < n:
        வரி = வரிகள்[i]
        m = _DIR_RE.match(வரி)
        எனில் இல்லை m:
            out.append(வரி)
            i += 1
            தொடர்

        kw = m.group(1)
        எனில் kw உள் ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            எனில் _IF0_RE.match(வரி) அல்லது _IFDEF_DEBUG_RE.match(வரி) அல்லது _IFNDEF_PROTO_RE.match(வரி) அல்லது _IF_NOTDEF_PROTO_RE.match(வரி):
                group_end, _ = _collect_group(i)
                i = group_end
                தொடர்

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") மற்றும் (
                _DEFINED_CALL_RE.தேடல்(வரி) is இல்லை None
            )

            எனில் evaluate:
                # Extract condition text after 'if'
                முயல்:
                    cond_text = வரி.split("if", 1)[1]
                விதிவிலக்கு Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                எனில் இல்லை keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    தொடர்

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            எனில் இல்லை bodies:
                i = group_end
                தொடர்
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(வரிகள்[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            எனில் DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                க்கு ln உள் kept_text.splitlines():
                    எனில் ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        நிறுத்து

            out.append(kept_rewritten)
            i = group_end
        இல்லையெனில்:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" எனில் வரி.endswith("\n") இல்லையெனில் "")
            i += 1

    திரும்பு "".join(out)

def collapse_star_spaces(s: str) -> str:
    திரும்பு re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    திரும்பு s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    திரும்பு s

def normalize_ws(s: str) -> str:
    திரும்பு re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t க்கு t உள் cur.get_tokens() எனில் t.kind != TokenKind.COMMENT]
    எனில் இல்லை toks:
        திரும்பு []
    name_idx = None
    க்கு i, t உள் enumerate(toks):
        எனில் t.kind == TokenKind.IDENTIFIER மற்றும் t.spelling == cur.spelling:
            name_idx = i
            நிறுத்து
    எனில் name_idx is None:
        திரும்பு []
    lpar_idx = None
    க்கு i உள் range(name_idx + 1, len(toks)):
        எனில் toks[i].spelling == '(':
            lpar_idx = i
            நிறுத்து
    எனில் lpar_idx is None:
        திரும்பு []
    depth, rpar_idx = 0, None
    க்கு i உள் range(lpar_idx, len(toks)):
        எனில் toks[i].spelling == '(':
            depth += 1
        அல்லதெனில் toks[i].spelling == ')':
            depth -= 1
            எனில் depth == 0:
                rpar_idx = i
                நிறுத்து
    எனில் rpar_idx is None:
        திரும்பு []
    திரும்பு [t.spelling க்கு t உள் toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    திரும்பு s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    முயல்:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling எனில் ret is இல்லை None இல்லையெனில் ""
    விதிவிலக்கு Exception:
        ret_sp = ""
    எனில் இல்லை ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        முயல்:
            ret_sp = cur.type.get_result().spelling
        விதிவிலக்கு Exception:
            ret_sp = "void"
    name_sp = cur.spelling அல்லது ""
    params = []
    முயல்:
        க்கு a உள் cur.get_arguments() அல்லது []:
            t = getattr(a, "type", None)
            t_sp = t.spelling எனில் t is இல்லை None இல்லையெனில் ""
            a_sp = a.spelling அல்லது ""
            seg = (t_sp + (" " + a_sp எனில் a_sp இல்லையெனில் "")).strip()
            params.append(seg எனில் seg இல்லையெனில் "void")
    விதிவிலக்கு Exception:
        pass
    params_s = ", ".join(params) எனில் params இல்லையெனில் "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    திரும்பு s

def in_this_file(cur, src_path: Path) -> bool:
    முயல்:
        loc = cur.extent.start
        f = (loc.file அல்லது cur.location.file)
        எனில் இல்லை f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            முயல்:
                toks = tokens_for_function_header(cur)
                திரும்பு bool(toks)
            விதிவிலக்கு Exception:
                திரும்பு பொய்
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        எனில் cur_path == src_abs அல்லது os.path.basename(cur_path) == os.path.basename(src_abs):
            திரும்பு உண்மை
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        எனில் cur_path.endswith(".c") மற்றும் os.path.dirname(cur_path) == src_dir:
            திரும்பு உண்மை
        திரும்பு பொய்
    விதிவிலக்கு Exception:
        திரும்பு பொய்

def collect_function_defs(tu, src_path: Path):
    def walk(முனை, depth=0):
        எனில் DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        எனில் முனை.kind == CursorKind.FUNCTION_DECL:
            எனில் in_this_file(முனை, src_path):
                எனில் முனை.storage_class != StorageClass.STATIC மற்றும் முனை.is_definition():
                    தா முனை
        க்கு ch உள் முனை.get_children():
            தா தஃ walk(ch, depth+1)
    தா தஃ walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    எனில் proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args அல்லது []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    எனில் SHOW_DIAGS:
        க்கு d உள் getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    திரும்பு tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    க்கு fn உள் collect_function_defs(tu, src_path):
        எனில் DEBUG_LOG:
            முயல்:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            விதிவிலக்கு Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) எனில் toks இல்லையெனில் header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        எனில் header:
            protos.append(f"{header};")

    seen, unique = set(), []
    க்கு p உள் protos:
        எனில் p இல்லை உள் seen:
            unique.append(p)
            seen.add(p)
    திரும்பு unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=உண்மை, exist_ok=உண்மை)
    முயல்:
        with out_path.open("w", encoding="utf-8", newline="\n") இவ f:
            f.லிக(f"// {src_name}\n")
            க்கு h உள் headers:
                f.லிக(h + "\n")
            f.லிக("// vim: ft=c\n")
    விதிவிலக்கு Exception இவ e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def முதன்மை():
    args = sys.argv[1:]
    எனில் இல்லை args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    எனில் இல்லை src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    முயல்:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    விதிவிலக்கு Exception இவ e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    முயல்:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    விதிவிலக்கு Exception இவ e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

எனில் __name__ == "__main__":
    முதன்மை()
