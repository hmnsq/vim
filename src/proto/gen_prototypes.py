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
ଆମଦାନୀ os
ଆମଦାନୀ re
ଆମଦାନୀ sys
ତଃ pathlib ଆମଦାନୀ Path
ତଃ typing ଆମଦାନୀ List, Tuple
ତଃ clang.cindex ଆମଦାନୀ Index, CursorKind, StorageClass, TokenKind

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") ନୁହେଁ ଭିତରେ ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") ନୁହେଁ ଭିତରେ ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

def _ends_with_bs(ଧାଡ଼ି: str) -> bool:
    ଫେରନ୍ତୁ ଧାଡ଼ି.rstrip("\r\n").rstrip().endswith("\\")

def _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    ପାଇଁ a ଭିତରେ argv:
        ଯଦି a == "-DPROTO" କିମ୍ବା a.startswith("-DPROTO="):
            ଫେରନ୍ତୁ ସତ୍ୟ
    ଫେରନ୍ତୁ ମିଥ୍ୟା

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

    ଯଦି DEBUG_LOG:
        print(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        print(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    ଯଦି proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ଅନ୍ୟଥା:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    ଯଦି proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ଅନ୍ୟଥା:
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

    ଚେଷ୍ଟା:
        ଫେରନ୍ତୁ bool(eval(s, {}, {}))
    ବ୍ୟତିକ୍ରମ Exception:
        # Fail-open to avoid accidentally dropping code
        ଯଦି DEBUG_LOG:
            print(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        ଫେରନ୍ତୁ ସତ୍ୟ

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
    ଧାଡ଼ିଗୁଡ଼ିକ = text.splitlines(keepends=ସତ୍ୟ)
    out: List[str] = []
    i, n = 0, len(ଧାଡ଼ିଗୁଡ଼ିକ)

    def _collect_group(start: ପୂର୍ଣ୍ଣସଂଖ୍ୟା) -> Tuple[ପୂର୍ଣ୍ଣସଂଖ୍ୟା, List[Tuple[str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = start
        # Skip backslash-continued header lines
        ଯେତେବେଳେ ସତ୍ୟ:
            h += 1
            ଯଦି h >= n କିମ୍ବା ନୁହେଁ _ends_with_bs(ଧାଡ଼ିଗୁଡ଼ିକ[h - 1]):
                ଭାଙ୍ଗ
        depth = 1
        j = h
        marks: List[Tuple[str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା]] = [("if", start)]
        ଯେତେବେଳେ j < n ଏବଂ depth > 0:
            m = _DIR_RE.match(ଧାଡ଼ିଗୁଡ଼ିକ[j])
            ଯଦି m:
                kw2 = m.group(1)
                ଯଦି kw2 ଭିତରେ ("if", "ifdef", "ifndef"):
                    depth += 1
                ନଚେତଯଦି kw2 == "endif":
                    depth -= 1
                    ଯଦି depth == 0:
                        ଭାଙ୍ଗ
                ନଚେତଯଦି depth == 1 ଏବଂ kw2 ଭିତରେ ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        ଅନ୍ୟଥା:
            # Unterminated: keep as-is from start to EOF
            ଫେରନ୍ତୁ n, [("text", start, n)]

        def after_header_ଧାଡ଼ି(pos: ପୂର୍ଣ୍ଣସଂଖ୍ୟା) -> ପୂର୍ଣ୍ଣସଂଖ୍ୟା:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ଯେତେବେଳେ k < n ଏବଂ _ends_with_bs(ଧାଡ଼ିଗୁଡ଼ିକ[k - 1]):
                k += 1
            ଫେରନ୍ତୁ k

        def after_header_ଧାଡ଼ି(pos: ପୂର୍ଣ୍ଣସଂଖ୍ୟା) -> ପୂର୍ଣ୍ଣସଂଖ୍ୟା:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ଯେତେବେଳେ k < n ଏବଂ _ends_with_bs(ଧାଡ଼ିଗୁଡ଼ିକ[k - 1]):
                k += 1
            ଫେରନ୍ତୁ k

        bodies: List[Tuple[str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା]] = []
        header_positions = [pos ପାଇଁ _, pos ଭିତରେ marks] + [j]
        tags = [tag ପାଇଁ tag, _ ଭିତରେ marks]
        ପାଇଁ idx, tag ଭିତରେ enumerate(tags):
            header = header_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_header_ଧାଡ଼ି(header)
            next_header = header_positions[idx + 1]
            body_e = next_header
            bodies.append((tag, body_s, body_e))
        ଫେରନ୍ତୁ j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    ଯେତେବେଳେ i < n:
        ଧାଡ଼ି = ଧାଡ଼ିଗୁଡ଼ିକ[i]
        m = _DIR_RE.match(ଧାଡ଼ି)
        ଯଦି ନୁହେଁ m:
            out.append(ଧାଡ଼ି)
            i += 1
            ଜାରି

        kw = m.group(1)
        ଯଦି kw ଭିତରେ ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            ଯଦି _IF0_RE.match(ଧାଡ଼ି) କିମ୍ବା _IFDEF_DEBUG_RE.match(ଧାଡ଼ି) କିମ୍ବା _IFNDEF_PROTO_RE.match(ଧାଡ଼ି) କିମ୍ବା _IF_NOTDEF_PROTO_RE.match(ଧାଡ଼ି):
                group_end, _ = _collect_group(i)
                i = group_end
                ଜାରି

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") ଏବଂ (
                _DEFINED_CALL_RE.ସନ୍ଧାନ(ଧାଡ଼ି) is ନୁହେଁ None
            )

            ଯଦି evaluate:
                # Extract condition text after 'if'
                ଚେଷ୍ଟା:
                    cond_text = ଧାଡ଼ି.split("if", 1)[1]
                ବ୍ୟତିକ୍ରମ Exception:
                    cond_text = ""
                keep_first = _eval_condition_with_defined_proto(cond_text, proto_is_defined)
                ଯଦି ନୁହେଁ keep_first:
                    group_end, _ = _collect_group(i)
                    i = group_end
                    ଜାରି

            # Keep only the first branch
            group_end, bodies = _collect_group(i)
            ଯଦି ନୁହେଁ bodies:
                i = group_end
                ଜାରି
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_text = "".join(ଧାଡ଼ିଗୁଡ଼ିକ[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_first_branch(kept_text)

            ଯଦି DEBUG_LOG:
                print(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                ପାଇଁ ln ଭିତରେ kept_text.splitlines():
                    ଯଦି ln.strip():
                        print(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        ଭାଙ୍ଗ

            out.append(kept_rewritten)
            i = group_end
        ଅନ୍ୟଥା:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            out.append("\n" ଯଦି ଧାଡ଼ି.endswith("\n") ଅନ୍ୟଥା "")
            i += 1

    ଫେରନ୍ତୁ "".join(out)

def collapse_star_spaces(s: str) -> str:
    ଫେରନ୍ତୁ re.sub(r"\*\s+", "*", s)

def fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    ଫେରନ୍ତୁ s

def strip_unused_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    ଫେରନ୍ତୁ s

def normalize_ws(s: str) -> str:
    ଫେରନ୍ତୁ re.sub(r"\s+", " ", s).strip()

def tokens_for_function_header(cur) -> List[str]:
    toks = [t ପାଇଁ t ଭିତରେ cur.get_tokens() ଯଦି t.kind != TokenKind.COMMENT]
    ଯଦି ନୁହେଁ toks:
        ଫେରନ୍ତୁ []
    name_idx = None
    ପାଇଁ i, t ଭିତରେ enumerate(toks):
        ଯଦି t.kind == TokenKind.IDENTIFIER ଏବଂ t.spelling == cur.spelling:
            name_idx = i
            ଭାଙ୍ଗ
    ଯଦି name_idx is None:
        ଫେରନ୍ତୁ []
    lpar_idx = None
    ପାଇଁ i ଭିତରେ range(name_idx + 1, len(toks)):
        ଯଦି toks[i].spelling == '(':
            lpar_idx = i
            ଭାଙ୍ଗ
    ଯଦି lpar_idx is None:
        ଫେରନ୍ତୁ []
    depth, rpar_idx = 0, None
    ପାଇଁ i ଭିତରେ range(lpar_idx, len(toks)):
        ଯଦି toks[i].spelling == '(':
            depth += 1
        ନଚେତଯଦି toks[i].spelling == ')':
            depth -= 1
            ଯଦି depth == 0:
                rpar_idx = i
                ଭାଙ୍ଗ
    ଯଦି rpar_idx is None:
        ଫେରନ୍ତୁ []
    ଫେରନ୍ତୁ [t.spelling ପାଇଁ t ଭିତରେ toks[:rpar_idx + 1]]

def join_tokens(tokens: List[str]) -> str:
    s = " ".join(tokens)
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    ଫେରନ୍ତୁ s

def header_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    ଚେଷ୍ଟା:
        ret = getattr(cur, "result_type", None)
        ret_sp = ret.spelling ଯଦି ret is ନୁହେଁ None ଅନ୍ୟଥା ""
    ବ୍ୟତିକ୍ରମ Exception:
        ret_sp = ""
    ଯଦି ନୁହେଁ ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        ଚେଷ୍ଟା:
            ret_sp = cur.type.get_result().spelling
        ବ୍ୟତିକ୍ରମ Exception:
            ret_sp = "void"
    name_sp = cur.spelling କିମ୍ବା ""
    params = []
    ଚେଷ୍ଟା:
        ପାଇଁ a ଭିତରେ cur.get_arguments() କିମ୍ବା []:
            t = getattr(a, "type", None)
            t_sp = t.spelling ଯଦି t is ନୁହେଁ None ଅନ୍ୟଥା ""
            a_sp = a.spelling କିମ୍ବା ""
            seg = (t_sp + (" " + a_sp ଯଦି a_sp ଅନ୍ୟଥା "")).strip()
            params.append(seg ଯଦି seg ଅନ୍ୟଥା "void")
    ବ୍ୟତିକ୍ରମ Exception:
        pass
    params_s = ", ".join(params) ଯଦି params ଅନ୍ୟଥା "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_unused_macros(s)
    s = normalize_ws(s)
    s = collapse_star_spaces(s)
    s = fix_spacing(s)
    ଫେରନ୍ତୁ s

def in_this_file(cur, src_path: Path) -> bool:
    ଚେଷ୍ଟା:
        loc = cur.extent.start
        f = (loc.file କିମ୍ବା cur.location.file)
        ଯଦି ନୁହେଁ f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            ଚେଷ୍ଟା:
                toks = tokens_for_function_header(cur)
                ଫେରନ୍ତୁ bool(toks)
            ବ୍ୟତିକ୍ରମ Exception:
                ଫେରନ୍ତୁ ମିଥ୍ୟା
        cur_path = os.path.normcase(os.path.abspath(str(f.name)))
        src_abs  = os.path.normcase(os.path.abspath(src_path.as_posix()))
        # Accept the main source file
        ଯଦି cur_path == src_abs କିମ୍ବା os.path.basename(cur_path) == os.path.basename(src_abs):
            ଫେରନ୍ତୁ ସତ୍ୟ
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.path.normcase(os.path.abspath(src_path.parent.as_posix()))
        ଯଦି cur_path.endswith(".c") ଏବଂ os.path.dirname(cur_path) == src_dir:
            ଫେରନ୍ତୁ ସତ୍ୟ
        ଫେରନ୍ତୁ ମିଥ୍ୟା
    ବ୍ୟତିକ୍ରମ Exception:
        ଫେରନ୍ତୁ ମିଥ୍ୟା

def collect_function_defs(tu, src_path: Path):
    def walk(ଗଣ୍ଠି, depth=0):
        ଯଦି DEBUG_LOG:
            print(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        ଯଦି ଗଣ୍ଠି.kind == CursorKind.FUNCTION_DECL:
            ଯଦି in_this_file(ଗଣ୍ଠି, src_path):
                ଯଦି ଗଣ୍ଠି.storage_class != StorageClass.STATIC ଏବଂ ଗଣ୍ଠି.is_definition():
                    ଦିଅ ଗଣ୍ଠି
        ପାଇଁ ch ଭିତରେ ଗଣ୍ଠି.get_children():
            ଦିଅ ତଃ walk(ch, depth+1)
    ଦିଅ ତଃ walk(tu.cursor)

def parse_with_clang(src_path: Path, text: str, args: List[str]):
    index = Index.create()
    src_abs = os.path.abspath(src_path.as_posix())
    unsaved = [(src_abs, text)]
    proto_h = src_path.parent / "proto.h"
    ଯଦି proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_args = (args କିମ୍ବା []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = index.parse(src_abs, args=final_args, unsaved_files=unsaved, options=0)
    ଯଦି SHOW_DIAGS:
        ପାଇଁ d ଭିତରେ getattr(tu, "diagnostics", []):
            print(d, file=sys.stderr)
    ଫେରନ୍ତୁ tu

def generate_prototypes(tu, src_path: Path) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    ପାଇଁ fn ଭିତରେ collect_function_defs(tu, src_path):
        ଯଦି DEBUG_LOG:
            ଚେଷ୍ଟା:
                sr = fn.extent.start
                er = fn.extent.end
                print(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            ବ୍ୟତିକ୍ରମ Exception:
                print(f"[def] {fn.spelling}", file=sys.stderr)
        toks = tokens_for_function_header(fn)
        header = join_tokens(toks) ଯଦି toks ଅନ୍ୟଥା header_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        header = re.sub(r'\b_Bool\b', 'bool', header)
        ଯଦି header:
            protos.append(f"{header};")

    seen, unique = set(), []
    ପାଇଁ p ଭିତରେ protos:
        ଯଦି p ନୁହେଁ ଭିତରେ seen:
            unique.append(p)
            seen.add(p)
    ଫେରନ୍ତୁ unique

def write_prototypes(out_path: Path, headers: List[str], src_name: str) -> None:
    """Write collected prototypes to the output .pro file."""
    out_path.parent.mkdir(parents=ସତ୍ୟ, exist_ok=ସତ୍ୟ)
    ଚେଷ୍ଟା:
        with out_path.open("w", encoding="utf-8", newline="\n") ଇଵ f:
            f.ଲିଖ(f"// {src_name}\n")
            ପାଇଁ h ଭିତରେ headers:
                f.ଲିଖ(h + "\n")
            f.ଲିଖ("// vim: ft=c\n")
    ବ୍ୟତିକ୍ରମ Exception ଇଵ e:
        print(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    print(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

def ମୁଖ୍ୟ():
    args = sys.argv[1:]
    ଯଦି ନୁହେଁ args:
        print("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_path = Path(args[0])
    clang_args = args[1:]
    ଯଦି ନୁହେଁ src_path.exists():
        print(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    ଚେଷ୍ଟା:
        original = src_path.read_text(encoding="utf-8", errors="ignore")
    ବ୍ୟତିକ୍ରମ Exception ଇଵ e:
        print(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_first_branch(original)

    # Parse with libclang
    ଚେଷ୍ଟା:
        tu = parse_with_clang(src_path, rewritten, clang_args)
    ବ୍ୟତିକ୍ରମ Exception ଇଵ e:
        print(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    headers = generate_prototypes(tu, src_path)
    out_file = Path("proto") / (src_path.stem + ".pro")
    write_prototypes(out_file, headers, src_path.name)

ଯଦି __name__ == "__main__":
    ମୁଖ୍ୟ()
