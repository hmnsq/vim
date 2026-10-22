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
ഇറക്കുമതി os
ഇറക്കുമതി re
ഇറക്കുമതി sys
തഃ pathlib ഇറക്കുമതി Path
തഃ typing ഇറക്കുമതി List, Tuple
തഃ clang.cindex ഇറക്കുമതി Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") അല്ല ഇൽ ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") അല്ല ഇൽ ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(വരി: str) -> bool:
    തിരികെ വരി.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    വേണ്ടി a ഇൽ argv:
        എങ്കിൽ a == "-DPROTO" അഥവാ a.startswith("-DPROTO="):
            തിരികെ സത്യം
    തിരികെ അസത്യം

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

    എങ്കിൽ DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    എങ്കിൽ proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    അല്ലെങ്കിൽ:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    എങ്കിൽ proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    അല്ലെങ്കിൽ:
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

    ശ്രമിക്കുക:
        തിരികെ bool(eval(s, {}, {}))
    അപവാദം Exception:
        # Fail-open to avoid accidentally dropping code
        എങ്കിൽ DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        തിരികെ സത്യം

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
    വരികൾ = text.splitlines(keepends=സത്യം)
    out: List[str] = []
    i, n = 0, len(വരികൾ)

    def _collect_group(start: പൂർണ്ണസംഖ്യ) -> Tuple[പൂർണ്ണസംഖ്യ, List[Tuple[str, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        ആകുമ്പോൾ സത്യം:
            h += 1
            എങ്കിൽ h >= n അഥവാ അല്ല _ends_with_bs(വരികൾ[h - 1]):
                നിർത്തുക
        depth = 1
        j = h
        marks: List[Tuple[str, പൂർണ്ണസംഖ്യ]] = [("if", start)]
        ആകുമ്പോൾ j < n ഒപ്പം depth > 0:
            m = _DIR_RE.match(വരികൾ[j])
            എങ്കിൽ m:
                kw2 = m.group(1)
                എങ്കിൽ kw2 ഇൽ ("if", "ifdef", "ifndef"):
                    depth += 1
                അല്ലെങ്കിലെങ്കിൽ kw2 == "endif":
                    depth -= 1
                    എങ്കിൽ depth == 0:
                        നിർത്തുക
                അല്ലെങ്കിലെങ്കിൽ depth == 1 ഒപ്പം kw2 ഇൽ ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        അല്ലെങ്കിൽ:
            # Unterminated: keep as-is from start to EOF
            തിരികെ n, [("text", start, n)]

        def after_header_വരി(pos: പൂർണ്ണസംഖ്യ) -> പൂർണ്ണസംഖ്യ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ആകുമ്പോൾ k < n ഒപ്പം _ends_with_bs(വരികൾ[k - 1]):
                k += 1
            തിരികെ k

        def after_header_വരി(pos: പൂർണ്ണസംഖ്യ) -> പൂർണ്ണസംഖ്യ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ആകുമ്പോൾ k < n ഒപ്പം _ends_with_bs(വരികൾ[k - 1]):
                k += 1
            തിരികെ k

        bodies: List[Tuple[str, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ]] = []
        header_positions = [pos വേണ്ടി _, pos ഇൽ marks] + [j]
        tags = [tag വേണ്ടി tag, _ ഇൽ marks]
        വേണ്ടി idx, tag ഇൽ enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_വരി(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        തിരികെ j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    ആകുമ്പോൾ i < n:
        വരി = വരികൾ[i]
        m = _DIR_RE.match(വരി)
        എങ്കിൽ അല്ല m:
            out.append(വരി)
            i += 1
            തുടരുക

        kw = m.group(1)
        എങ്കിൽ kw ഇൽ ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            എങ്കിൽ _IF0_RE.match(വരി) അഥവാ _IFDEF_DEBUG_RE.match(വരി) അഥവാ _IFNDEF_PROTO_RE.match(വരി) അഥവാ _IF_NOTDEF_PROTO_RE.match(വരി):
                group_end, _ = _collect_group(i)
                i = group_end
                തുടരുക

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") ഒപ്പം (
                _DEFINED_CALL_RE.തിരയൽ(വരി) is അല്ല None
            )

            എങ്കിൽ evaluate:
                # Extract condition text after 'if'
                ശ്രമിക്കുക:
                    cond_text = വരി.split("if", 1)[1]
                അപവാദം Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                എങ്കിൽ അല്ല keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    തുടരുക

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            എങ്കിൽ അല്ല bodies:
                i = group_end
                തുടരുക
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(വരികൾ[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            എങ്കിൽ DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                വേണ്ടി ln ഇൽ kept_text.splitlines():
                    എങ്കിൽ ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        നിർത്തുക

            out.append(kept_rewritten)
            i = group_end
        അല്ലെങ്കിൽ:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" എങ്കിൽ വരി.endswith("\n") അല്ലെങ്കിൽ "")
            i += 1

    തിരികെ "".join(out)

def collapse_star_spaces(s: str) -> str:
    തിരികെ re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    തിരികെ s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    തിരികെ s

def normalize_ws(s: str) -> str:
    തിരികെ re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t വേണ്ടി t ഇൽ cur.get_tokens() എങ്കിൽ t.kind != TokenKind.COMMENT]
    എങ്കിൽ അല്ല toks:
        തിരികെ []
    name_idx = None
    വേണ്ടി i, t ഇൽ enumerate(toks):
        എങ്കിൽ t.kind == TokenKind.IDENTIFIER ഒപ്പം t.spelling == cur.spelling:
            name_idx = i
            നിർത്തുക
    എങ്കിൽ name_idx is None:
        തിരികെ []
    lpar_idx = None
    വേണ്ടി i ഇൽ range(name_idx + 1, len(toks)):
        എങ്കിൽ toks[i].spelling == '(':
            lpar_idx = i
            നിർത്തുക
    എങ്കിൽ lpar_idx is None:
        തിരികെ []
    depth, rpar_idx = 0, None
    വേണ്ടി i ഇൽ range(lpar_idx, len(toks)):
        എങ്കിൽ toks[i].spelling == '(':
            depth += 1
        അല്ലെങ്കിലെങ്കിൽ toks[i].spelling == ')':
            depth -= 1
            എങ്കിൽ depth == 0:
                rpar_idx = i
                നിർത്തുക
    എങ്കിൽ rpar_idx is None:
        തിരികെ []
    തിരികെ [t.spelling വേണ്ടി t ഇൽ toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    തിരികെ s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    ശ്രമിക്കുക:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling എങ്കിൽ ret is അല്ല None അല്ലെങ്കിൽ ""
    അപവാദം Exception:
        ret_sp = ""
    എങ്കിൽ അല്ല ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        ശ്രമിക്കുക:
            ret_sp = cur.type.get_result().spelling
        അപവാദം Exception:
            ret_sp = "void"
    name_sp = cur.spelling അഥവാ ""
    params = []
    ശ്രമിക്കുക:
        വേണ്ടി a ഇൽ cur.get_arguments() അഥവാ []:
            t = getattr(a, "type", None)
            t_sp = t.spelling എങ്കിൽ t is അല്ല None അല്ലെങ്കിൽ ""
            a_sp = a.spelling അഥവാ ""
            seg = (t_sp + (" " + a_sp എങ്കിൽ a_sp അല്ലെങ്കിൽ "")).strip()
            params.append(seg എങ്കിൽ seg അല്ലെങ്കിൽ "void")
    അപവാദം Exception:
        pass
    params_s = ", ".join(params) എങ്കിൽ params അല്ലെങ്കിൽ "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    തിരികെ s

def in_this_file(cur, src_path: Path) -> bool:
    ശ്രമിക്കുക:
        loc = cur.extent.start
        f = (loc.file അഥവാ cur.location.file)
        എങ്കിൽ അല്ല f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            ശ്രമിക്കുക:
                toks = tokens_for_function_header(cur)
                തിരികെ bool(toks)
            അപവാദം Exception:
                തിരികെ അസത്യം
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        എങ്കിൽ cur_path == src_abs അഥവാ os.path.basename(cur_path) == os.path.basename(src_abs):
            തിരികെ സത്യം
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        എങ്കിൽ cur_path.endswith(".c") ഒപ്പം os.path.dirname(cur_path) == src_dir:
            തിരികെ സത്യം
        തിരികെ അസത്യം
    അപവാദം Exception:
        തിരികെ അസത്യം

def collect_function_defs(tu, src_path: Path):
    def walk(കണ്ണി, depth=0):
        എങ്കിൽ DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        എങ്കിൽ കണ്ണി.kind == CursorKind.FUNCTION_DECL:
            എങ്കിൽ in_this_file(കണ്ണി, src_path):
                എങ്കിൽ കണ്ണി.storage_class != StorageClass.STATIC ഒപ്പം കണ്ണി.is_definition():
                    നൽകുക കണ്ണി
        വേണ്ടി ch ഇൽ കണ്ണി.get_children():
            നൽകുക തഃ walk(ch, depth+1)
    നൽകുക തഃ walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    എങ്കിൽ proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args അഥവാ []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    എങ്കിൽ SHOW_DIAGS:
        വേണ്ടി d ഇൽ getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    തിരികെ tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    വേണ്ടി fn ഇൽ collect_function_defs(tu, src_path):
        എങ്കിൽ DEBUG_LOG:
            ശ്രമിക്കുക:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            അപവാദം Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) എങ്കിൽ toks അല്ലെങ്കിൽ header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        എങ്കിൽ header:
            protos.append(f"{header};")

    seen, unique = set(), []
    വേണ്ടി p ഇൽ protos:
        എങ്കിൽ p അല്ല ഇൽ seen:
            unique.append(p)
            seen.add(p)
    തിരികെ unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=സത്യം, exist_ok=സത്യം)
    ശ്രമിക്കുക:
        with out_path.open("w", encoding="utf-8", newline="\n") ഇവ f:
            f.ലിഖ(f"// {src_name}\n")
            വേണ്ടി h ഇൽ headers:
                f.ലിഖ(h + "\n")
            f.ലിഖ("// vim: ft=c\n")
    അപവാദം Exception ഇവ e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def പ്രധാന():
    args = sys.argv[1:]
    എങ്കിൽ അല്ല args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    എങ്കിൽ അല്ല src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    ശ്രമിക്കുക:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    അപവാദം Exception ഇവ e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    ശ്രമിക്കുക:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    അപവാദം Exception ഇവ e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

എങ്കിൽ __name__ == "__main__":
    പ്രധാന()
