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
ਆਯਾਤ os
ਆਯਾਤ re
ਆਯਾਤ sys
ਤਃ pathlib ਆਯਾਤ ਰਸਤਾ_2
ਤਃ typing ਆਯਾਤ List, Tuple
ਤਃ clang.cindex ਆਯਾਤ ਸੂਚਕਾਂਕ_3, Cursor_ਕਿਸਮ, StorageClass, Token_ਕਿਸਮ

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") ਨਹੀਂ ਵਿੱਚ ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") ਨਹੀਂ ਵਿੱਚ ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

ਪਰਿਭਾਸ਼ਾ ਸਿਰੇ_with_bs(ਸਤਰ: str) -> bool:
    ਵਾਪਸ ਸਤਰ.rstrip("\r\n").rstrip().endswith("\\")

ਪਰਿਭਾਸ਼ਾ _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    ਲਈ a ਵਿੱਚ argv:
        ਜੇ a == "-DPROTO" ਜਾਂ a.startswith("-DPROTO="):
            ਵਾਪਸ ਸੱਚ
    ਵਾਪਸ ਝੂਠ

ਪਰਿਭਾਸ਼ਾ _eval_condition_with_defined_proto(cond: str, proto_is_defined: bool) -> bool:
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

    ਜੇ DEBUG_LOG:
        ਛਾਪੋ(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        ਛਾਪੋ(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    ਜੇ proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ਨਹੀਂਤਾਂ:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    ਜੇ proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ਨਹੀਂਤਾਂ:
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

    ਕੋਸ਼ਿਸ਼:
        ਵਾਪਸ bool(eval(s, {}, {}))
    ਅਪਵਾਦ Exception:
        # Fail-open to avoid accidentally dropping code
        ਜੇ DEBUG_LOG:
            ਛਾਪੋ(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        ਵਾਪਸ ਸੱਚ

ਪਰਿਭਾਸ਼ਾ rewrite_conditionals_ਪਹਿਲਾ_branch(ਪਾਠ: str) -> str:
    """
    Keep only the first branch of #if/#ifdef/#ifndef groups, remove others.
    Extensions:
      - Drop whole group for #if 0.
      - Drop whole group for #ifndef PROTO and #if !defined(PROTO).
      - If an '#if <expr>' contains defined(PROTO) anywhere in the expression,
        evaluate the condition: if False, drop the whole group; if True, keep
        only the first branch (same as legacy behavior).
    """
    ਸਤਰਾਂ = ਪਾਠ.splitlines(keepends=ਸੱਚ)
    ਬਾਹਰਲਾ: List[str] = []
    i, n = 0, ਲੰਬਾਈ(ਸਤਰਾਂ)

    ਪਰਿਭਾਸ਼ਾ collect_ਸਮੂਹ(ਸ਼ੁਰੂ: ਪੂਰਨ) -> Tuple[ਪੂਰਨ, List[Tuple[str, ਪੂਰਨ, ਪੂਰਨ]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = ਸ਼ੁਰੂ
        # Skip backslash-continued header lines
        ਜਦੋਂ ਸੱਚ:
            h += 1
            ਜੇ h >= n ਜਾਂ ਨਹੀਂ ਸਿਰੇ_with_bs(ਸਤਰਾਂ[h - 1]):
                ਰੋਕੋ
        ਡੂੰਘਾਈ = 1
        j = h
        marks: List[Tuple[str, ਪੂਰਨ]] = [("if", ਸ਼ੁਰੂ)]
        ਜਦੋਂ j < n ਅਤੇ ਡੂੰਘਾਈ > 0:
            m = _DIR_RE.ਮੇਲ(ਸਤਰਾਂ[j])
            ਜੇ m:
                kw2 = m.ਸਮੂਹ(1)
                ਜੇ kw2 ਵਿੱਚ ("if", "ifdef", "ifndef"):
                    ਡੂੰਘਾਈ += 1
                ਜਾਂਜੇ kw2 == "endif":
                    ਡੂੰਘਾਈ -= 1
                    ਜੇ ਡੂੰਘਾਈ == 0:
                        ਰੋਕੋ
                ਜਾਂਜੇ ਡੂੰਘਾਈ == 1 ਅਤੇ kw2 ਵਿੱਚ ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        ਨਹੀਂਤਾਂ:
            # Unterminated: keep as-is from start to EOF
            ਵਾਪਸ n, [("text", ਸ਼ੁਰੂ, n)]

        ਪਰਿਭਾਸ਼ਾ after_ਸਿਰਲੇਖ_ਸਤਰ(pos: ਪੂਰਨ) -> ਪੂਰਨ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ਜਦੋਂ k < n ਅਤੇ ਸਿਰੇ_with_bs(ਸਤਰਾਂ[k - 1]):
                k += 1
            ਵਾਪਸ k

        ਪਰਿਭਾਸ਼ਾ after_ਸਿਰਲੇਖ_ਸਤਰ(pos: ਪੂਰਨ) -> ਪੂਰਨ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ਜਦੋਂ k < n ਅਤੇ ਸਿਰੇ_with_bs(ਸਤਰਾਂ[k - 1]):
                k += 1
            ਵਾਪਸ k

        bodies: List[Tuple[str, ਪੂਰਨ, ਪੂਰਨ]] = []
        ਸਿਰਲੇਖ_positions = [pos ਲਈ _, pos ਵਿੱਚ marks] + [j]
        ਟੈਗ_ਸੂਚੀ = [ਟੈਗ ਲਈ ਟੈਗ, _ ਵਿੱਚ marks]
        ਲਈ idx, ਟੈਗ ਵਿੱਚ enumerate(ਟੈਗ_ਸੂਚੀ):
            ਸਿਰਲੇਖ = ਸਿਰਲੇਖ_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_ਸਿਰਲੇਖ_ਸਤਰ(ਸਿਰਲੇਖ)
            ਅਗਲਾ_ਸਿਰਲੇਖ = ਸਿਰਲੇਖ_positions[idx + 1]
            body_e = ਅਗਲਾ_ਸਿਰਲੇਖ
            bodies.append((ਟੈਗ, body_s, body_e))
        ਵਾਪਸ j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    ਜਦੋਂ i < n:
        ਸਤਰ = ਸਤਰਾਂ[i]
        m = _DIR_RE.ਮੇਲ(ਸਤਰ)
        ਜੇ ਨਹੀਂ m:
            ਬਾਹਰਲਾ.append(ਸਤਰ)
            i += 1
            ਜਾਰੀ

        kw = m.ਸਮੂਹ(1)
        ਜੇ kw ਵਿੱਚ ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            ਜੇ _IF0_RE.ਮੇਲ(ਸਤਰ) ਜਾਂ _IFDEF_DEBUG_RE.ਮੇਲ(ਸਤਰ) ਜਾਂ _IFNDEF_PROTO_RE.ਮੇਲ(ਸਤਰ) ਜਾਂ _IF_NOTDEF_PROTO_RE.ਮੇਲ(ਸਤਰ):
                ਸਮੂਹ_ਸਿਰਾ, _ = collect_ਸਮੂਹ(i)
                i = ਸਮੂਹ_ਸਿਰਾ
                ਜਾਰੀ

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") ਅਤੇ (
                _DEFINED_CALL_RE.ਖੋਜ(ਸਤਰ) ਹੈ ਨਹੀਂ ਕੁਝ_ਨਹੀਂ
            )

            ਜੇ evaluate:
                # Extract condition text after 'if'
                ਕੋਸ਼ਿਸ਼:
                    cond_ਪਾਠ = ਸਤਰ.ਵੰਡੋ("if", 1)[1]
                ਅਪਵਾਦ Exception:
                    cond_ਪਾਠ = ""
                keep_ਪਹਿਲਾ = _eval_condition_with_defined_proto(cond_ਪਾਠ, proto_is_defined)
                ਜੇ ਨਹੀਂ keep_ਪਹਿਲਾ:
                    ਸਮੂਹ_ਸਿਰਾ, _ = collect_ਸਮੂਹ(i)
                    i = ਸਮੂਹ_ਸਿਰਾ
                    ਜਾਰੀ

            # Keep only the first branch
            ਸਮੂਹ_ਸਿਰਾ, bodies = collect_ਸਮੂਹ(i)
            ਜੇ ਨਹੀਂ bodies:
                i = ਸਮੂਹ_ਸਿਰਾ
                ਜਾਰੀ
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_ਪਾਠ = "".join(ਸਤਰਾਂ[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_ਪਹਿਲਾ_branch(kept_ਪਾਠ)

            ਜੇ DEBUG_LOG:
                ਛਾਪੋ(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                ਲਈ ln ਵਿੱਚ kept_ਪਾਠ.splitlines():
                    ਜੇ ln.strip():
                        ਛਾਪੋ(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        ਰੋਕੋ

            ਬਾਹਰਲਾ.append(kept_rewritten)
            i = ਸਮੂਹ_ਸਿਰਾ
        ਨਹੀਂਤਾਂ:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            ਬਾਹਰਲਾ.append("\n" ਜੇ ਸਤਰ.endswith("\n") ਨਹੀਂਤਾਂ "")
            i += 1

    ਵਾਪਸ "".join(ਬਾਹਰਲਾ)

ਪਰਿਭਾਸ਼ਾ collapse_ਤਾਰਾ_spaces(s: str) -> str:
    ਵਾਪਸ re.sub(r"\*\s+", "*", s)

ਪਰਿਭਾਸ਼ਾ fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    ਵਾਪਸ s

ਪਰਿਭਾਸ਼ਾ strip_ਅਣਵਰਤਿਆ_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    ਵਾਪਸ s

ਪਰਿਭਾਸ਼ਾ normalize_ws(s: str) -> str:
    ਵਾਪਸ re.sub(r"\s+", " ", s).strip()

ਪਰਿਭਾਸ਼ਾ ਟੋਕਨ_for_function_ਸਿਰਲੇਖ(cur) -> List[str]:
    toks = [t ਲਈ t ਵਿੱਚ cur.get_ਟੋਕਨ() ਜੇ t.ਕਿਸਮ != Token_ਕਿਸਮ.COMMENT]
    ਜੇ ਨਹੀਂ toks:
        ਵਾਪਸ []
    name_idx = ਕੁਝ_ਨਹੀਂ
    ਲਈ i, t ਵਿੱਚ enumerate(toks):
        ਜੇ t.ਕਿਸਮ == Token_ਕਿਸਮ.IDENTIFIER ਅਤੇ t.spelling == cur.spelling:
            name_idx = i
            ਰੋਕੋ
    ਜੇ name_idx ਹੈ ਕੁਝ_ਨਹੀਂ:
        ਵਾਪਸ []
    lpar_idx = ਕੁਝ_ਨਹੀਂ
    ਲਈ i ਵਿੱਚ ਸੀਮਾ(name_idx + 1, ਲੰਬਾਈ(toks)):
        ਜੇ toks[i].spelling == '(':
            lpar_idx = i
            ਰੋਕੋ
    ਜੇ lpar_idx ਹੈ ਕੁਝ_ਨਹੀਂ:
        ਵਾਪਸ []
    ਡੂੰਘਾਈ, rpar_idx = 0, ਕੁਝ_ਨਹੀਂ
    ਲਈ i ਵਿੱਚ ਸੀਮਾ(lpar_idx, ਲੰਬਾਈ(toks)):
        ਜੇ toks[i].spelling == '(':
            ਡੂੰਘਾਈ += 1
        ਜਾਂਜੇ toks[i].spelling == ')':
            ਡੂੰਘਾਈ -= 1
            ਜੇ ਡੂੰਘਾਈ == 0:
                rpar_idx = i
                ਰੋਕੋ
    ਜੇ rpar_idx ਹੈ ਕੁਝ_ਨਹੀਂ:
        ਵਾਪਸ []
    ਵਾਪਸ [t.spelling ਲਈ t ਵਿੱਚ toks[:rpar_idx + 1]]

ਪਰਿਭਾਸ਼ਾ join_ਟੋਕਨ(ਟੋਕਨ: List[str]) -> str:
    s = " ".join(ਟੋਕਨ)
    s = strip_ਅਣਵਰਤਿਆ_macros(s)
    s = normalize_ws(s)
    s = collapse_ਤਾਰਾ_spaces(s)
    s = fix_spacing(s)
    ਵਾਪਸ s

ਪਰਿਭਾਸ਼ਾ ਸਿਰਲੇਖ_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    ਕੋਸ਼ਿਸ਼:
        ret = getattr(cur, "result_type", ਕੁਝ_ਨਹੀਂ)
        ret_sp = ret.spelling ਜੇ ret ਹੈ ਨਹੀਂ ਕੁਝ_ਨਹੀਂ ਨਹੀਂਤਾਂ ""
    ਅਪਵਾਦ Exception:
        ret_sp = ""
    ਜੇ ਨਹੀਂ ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        ਕੋਸ਼ਿਸ਼:
            ret_sp = cur.type.get_ਨਤੀਜਾ().spelling
        ਅਪਵਾਦ Exception:
            ret_sp = "void"
    name_sp = cur.spelling ਜਾਂ ""
    params = []
    ਕੋਸ਼ਿਸ਼:
        ਲਈ a ਵਿੱਚ cur.get_arguments() ਜਾਂ []:
            t = getattr(a, "type", ਕੁਝ_ਨਹੀਂ)
            t_sp = t.spelling ਜੇ t ਹੈ ਨਹੀਂ ਕੁਝ_ਨਹੀਂ ਨਹੀਂਤਾਂ ""
            a_sp = a.spelling ਜਾਂ ""
            seg = (t_sp + (" " + a_sp ਜੇ a_sp ਨਹੀਂਤਾਂ "")).strip()
            params.append(seg ਜੇ seg ਨਹੀਂਤਾਂ "void")
    ਅਪਵਾਦ Exception:
        ਲੰਘੋ
    params_s = ", ".join(params) ਜੇ params ਨਹੀਂਤਾਂ "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_ਅਣਵਰਤਿਆ_macros(s)
    s = normalize_ws(s)
    s = collapse_ਤਾਰਾ_spaces(s)
    s = fix_spacing(s)
    ਵਾਪਸ s

ਪਰਿਭਾਸ਼ਾ ਵਿੱਚ_this_file(cur, src_ਰਸਤਾ: ਰਸਤਾ_2) -> bool:
    ਕੋਸ਼ਿਸ਼:
        loc = cur.extent.ਸ਼ੁਰੂ
        f = (loc.file ਜਾਂ cur.location.file)
        ਜੇ ਨਹੀਂ f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            ਕੋਸ਼ਿਸ਼:
                toks = ਟੋਕਨ_for_function_ਸਿਰਲੇਖ(cur)
                ਵਾਪਸ bool(toks)
            ਅਪਵਾਦ Exception:
                ਵਾਪਸ ਝੂਠ
        cur_ਰਸਤਾ = os.ਰਸਤਾ.normcase(os.ਰਸਤਾ.abspath(str(f.name)))
        src_abs  = os.ਰਸਤਾ.normcase(os.ਰਸਤਾ.abspath(src_ਰਸਤਾ.as_posix()))
        # Accept the main source file
        ਜੇ cur_ਰਸਤਾ == src_abs ਜਾਂ os.ਰਸਤਾ.basename(cur_ਰਸਤਾ) == os.ਰਸਤਾ.basename(src_abs):
            ਵਾਪਸ ਸੱਚ
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.ਰਸਤਾ.normcase(os.ਰਸਤਾ.abspath(src_ਰਸਤਾ.ਮਾਪਾ.as_posix()))
        ਜੇ cur_ਰਸਤਾ.endswith(".c") ਅਤੇ os.ਰਸਤਾ.dirname(cur_ਰਸਤਾ) == src_dir:
            ਵਾਪਸ ਸੱਚ
        ਵਾਪਸ ਝੂਠ
    ਅਪਵਾਦ Exception:
        ਵਾਪਸ ਝੂਠ

ਪਰਿਭਾਸ਼ਾ collect_function_defs(tu, src_ਰਸਤਾ: ਰਸਤਾ_2):
    ਪਰਿਭਾਸ਼ਾ walk(ਗੰਢ, ਡੂੰਘਾਈ=0):
        ਜੇ DEBUG_LOG:
            ਛਾਪੋ(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        ਜੇ ਗੰਢ.ਕਿਸਮ == Cursor_ਕਿਸਮ.FUNCTION_DECL:
            ਜੇ ਵਿੱਚ_this_file(ਗੰਢ, src_ਰਸਤਾ):
                ਜੇ ਗੰਢ.storage_class != StorageClass.STATIC ਅਤੇ ਗੰਢ.is_definition():
                    ਦਿਓ ਗੰਢ
        ਲਈ ch ਵਿੱਚ ਗੰਢ.get_children():
            ਦਿਓ ਤਃ walk(ch, ਡੂੰਘਾਈ+1)
    ਦਿਓ ਤਃ walk(tu.cursor)

ਪਰਿਭਾਸ਼ਾ parse_with_clang(src_ਰਸਤਾ: ਰਸਤਾ_2, ਪਾਠ: str, ਦਲੀਲਾਂ: List[str]):
    ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ_3.create()
    src_abs = os.ਰਸਤਾ.abspath(src_ਰਸਤਾ.as_posix())
    unsaved = [(src_abs, ਪਾਠ)]
    proto_h = src_ਰਸਤਾ.ਮਾਪਾ / "proto.h"
    ਜੇ proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_ਦਲੀਲਾਂ = (ਦਲੀਲਾਂ ਜਾਂ []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = ਸੂਚਕਾਂਕ.parse(src_abs, ਦਲੀਲਾਂ=final_ਦਲੀਲਾਂ, unsaved_files=unsaved, options=0)
    ਜੇ SHOW_DIAGS:
        ਲਈ d ਵਿੱਚ getattr(tu, "diagnostics", []):
            ਛਾਪੋ(d, file=sys.stderr)
    ਵਾਪਸ tu

ਪਰਿਭਾਸ਼ਾ generate_prototypes(tu, src_ਰਸਤਾ: ਰਸਤਾ_2) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    ਲਈ fn ਵਿੱਚ collect_function_defs(tu, src_ਰਸਤਾ):
        ਜੇ DEBUG_LOG:
            ਕੋਸ਼ਿਸ਼:
                sr = fn.extent.ਸ਼ੁਰੂ
                er = fn.extent.ਸਿਰਾ
                ਛਾਪੋ(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            ਅਪਵਾਦ Exception:
                ਛਾਪੋ(f"[def] {fn.spelling}", file=sys.stderr)
        toks = ਟੋਕਨ_for_function_ਸਿਰਲੇਖ(fn)
        ਸਿਰਲੇਖ = join_ਟੋਕਨ(toks) ਜੇ toks ਨਹੀਂਤਾਂ ਸਿਰਲੇਖ_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        ਸਿਰਲੇਖ = re.sub(r'\b_Bool\b', 'bool', ਸਿਰਲੇਖ)
        ਜੇ ਸਿਰਲੇਖ:
            protos.append(f"{header};")

    ਵੇਖਿਆ, unique = ਸੈੱਟ_2(), []
    ਲਈ p ਵਿੱਚ protos:
        ਜੇ p ਨਹੀਂ ਵਿੱਚ ਵੇਖਿਆ:
            unique.append(p)
            ਵੇਖਿਆ.ਜੋੜੋ(p)
    ਵਾਪਸ unique

ਪਰਿਭਾਸ਼ਾ write_prototypes(ਬਾਹਰਲਾ_ਰਸਤਾ: ਰਸਤਾ_2, ਸਿਰਲੇਖਾਂ: List[str], src_name: str) -> ਕੁਝ_ਨਹੀਂ:
    """Write collected prototypes to the output .pro file."""
    ਬਾਹਰਲਾ_ਰਸਤਾ.ਮਾਪਾ.mkdir(parents=ਸੱਚ, exist_ok=ਸੱਚ)
    ਕੋਸ਼ਿਸ਼:
        ਨਾਲ ਬਾਹਰਲਾ_ਰਸਤਾ.open("w", encoding="utf-8", newline="\n") ਇਵ f:
            f.ਲਿਖ(f"// {src_name}\n")
            ਲਈ h ਵਿੱਚ ਸਿਰਲੇਖਾਂ:
                f.ਲਿਖ(h + "\n")
            f.ਲਿਖ("// vim: ft=c\n")
    ਅਪਵਾਦ Exception ਇਵ e:
        ਛਾਪੋ(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    ਛਾਪੋ(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

ਪਰਿਭਾਸ਼ਾ ਮੁੱਖ():
    ਦਲੀਲਾਂ = sys.argv[1:]
    ਜੇ ਨਹੀਂ ਦਲੀਲਾਂ:
        ਛਾਪੋ("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_ਰਸਤਾ = ਰਸਤਾ_2(ਦਲੀਲਾਂ[0])
    clang_ਦਲੀਲਾਂ = ਦਲੀਲਾਂ[1:]
    ਜੇ ਨਹੀਂ src_ਰਸਤਾ.exists():
        ਛਾਪੋ(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    ਕੋਸ਼ਿਸ਼:
        original = src_ਰਸਤਾ.read_ਪਾਠ(encoding="utf-8", errors="ignore")
    ਅਪਵਾਦ Exception ਇਵ e:
        ਛਾਪੋ(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_ਪਹਿਲਾ_branch(original)

    # Parse with libclang
    ਕੋਸ਼ਿਸ਼:
        tu = parse_with_clang(src_ਰਸਤਾ, rewritten, clang_ਦਲੀਲਾਂ)
    ਅਪਵਾਦ Exception ਇਵ e:
        ਛਾਪੋ(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    ਸਿਰਲੇਖਾਂ = generate_prototypes(tu, src_ਰਸਤਾ)
    ਬਾਹਰਲਾ_file = ਰਸਤਾ_2("proto") / (src_ਰਸਤਾ.stem + ".pro")
    write_prototypes(ਬਾਹਰਲਾ_file, ਸਿਰਲੇਖਾਂ, src_ਰਸਤਾ.name)

ਜੇ __name__ == "__main__":
    ਮੁੱਖ()
