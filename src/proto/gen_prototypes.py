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
ಆಮದು os
ಆಮದು re
ಆಮದು sys
ತಃ pathlib ಆಮದು ಮಾರ್ಗ_2
ತಃ typing ಆಮದು List, Tuple
ತಃ clang.cindex ಆಮದು ಸೂಚ್ಯಂಕ_3, Cursor_ಬಗೆ, StorageClass, Token_ಬಗೆ

SHOW_DIAGS = os.environ.get("GENPROTO_SHOW_DIAGS", "") ಅಲ್ಲ ಒಳಗೆ ("", "0", "false", "no")
DEBUG_LOG  = os.environ.get("GENPROTO_DEBUG", "") ಅಲ್ಲ ಒಳಗೆ ("", "0", "false", "no")

# Preprocessor directive detection
_DIR_RE = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b')
_IF0_RE = re.compile(r'^\s*#\s*if\s+0\s*(//.*)?$')
_IFDEF_DEBUG_RE = re.compile(r'^\s*#\s*ifdef\s+_DEBUG\s*(//.*)?$')

# Keep legacy behavior: drop whole group for #ifndef PROTO and #if !defined(PROTO)
_IFNDEF_PROTO_RE   = re.compile(r'^\s*#\s*ifndef\s+PROTO\s*(//.*)?$')
_IF_NOTDEF_PROTO_RE = re.compile(r'^\s*#\s*if\s*!defined\s*\(\s*PROTO\s*\)\s*(//.*)?$')

# Helpers to detect defined(PROTO) inside an expression line
_DEFINED_CALL_RE      = re.compile(r'\bdefined\s*\(\s*PROTO\s*\)')

ವ್ಯಾಖ್ಯಾನ ತುದಿಗಳು_with_bs(ಸಾಲು: str) -> bool:
    ಹಿಂತಿರುಗಿ ಸಾಲು.rstrip("\r\n").rstrip().endswith("\\")

ವ್ಯಾಖ್ಯಾನ _has_proto_defined_from_argv(argv: List[str]) -> bool:
    """Return True if -DPROTO (or -DPROTO=...) appears in clang args."""
    ಗಾಗಿ a ಒಳಗೆ argv:
        ಆದರೆ a == "-DPROTO" ಅಥವಾ a.startswith("-DPROTO="):
            ಹಿಂತಿರುಗಿ ನಿಜ
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು

ವ್ಯಾಖ್ಯಾನ _eval_condition_with_defined_proto(cond: str, proto_is_defined: bool) -> bool:
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

    ಆದರೆ DEBUG_LOG:
        ಮುದ್ರಿಸು(f"[eval] raw condition: {cond.strip()}", file=sys.stderr)
        ಮುದ್ರಿಸು(f"[eval] proto_is_defined={proto_is_defined}", file=sys.stderr)

    # Normalize spacing
    s = s.strip()

    # Replace !defined(PROTO) first, then defined(PROTO)
    ಆದರೆ proto_is_defined:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ಇಲ್ಲದಿದ್ದರೆ:
        s = re.sub(r'!+\s*defined\s*\(\s*PROTO\s*\)', "True", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "False", s)

    # Translate C logical ops to Python
    # Replace defined(PROTO) and !defined(PROTO) depending on -DPROTO
    ಆದರೆ proto_is_defined:
        s = re.sub(r'!defined\s*\(\s*PROTO\s*\)', "False", s)
        s = re.sub(r'defined\s*\(\s*PROTO\s*\)', "True", s)
    ಇಲ್ಲದಿದ್ದರೆ:
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

    ಪ್ರಯತ್ನಿಸು:
        ಹಿಂತಿರುಗಿ bool(eval(s, {}, {}))
    ಅಪವಾದ Exception:
        # Fail-open to avoid accidentally dropping code
        ಆದರೆ DEBUG_LOG:
            ಮುದ್ರಿಸು(f"[warn] condition eval failed: {cond!r} -> {s!r}", file=sys.stderr)
        ಹಿಂತಿರುಗಿ ನಿಜ

ವ್ಯಾಖ್ಯಾನ rewrite_conditionals_ಮೊದಲ_branch(ಪಠ್ಯ: str) -> str:
    """
    Keep only the first branch of #if/#ifdef/#ifndef groups, remove others.
    Extensions:
      - Drop whole group for #if 0.
      - Drop whole group for #ifndef PROTO and #if !defined(PROTO).
      - If an '#if <expr>' contains defined(PROTO) anywhere in the expression,
        evaluate the condition: if False, drop the whole group; if True, keep
        only the first branch (same as legacy behavior).
    """
    ಸಾಲುಗಳು = ಪಠ್ಯ.splitlines(keepends=ನಿಜ)
    ಹೊರಹೋಗು: List[str] = []
    i, n = 0, ಉದ್ದ(ಸಾಲುಗಳು)

    ವ್ಯಾಖ್ಯಾನ collect_ಗುಂಪು(ಆರಂಭ: ಪೂರ್ಣಾಂಕ) -> Tuple[ಪೂರ್ಣಾಂಕ, List[Tuple[str, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ]]]:
        """
        Collect a full #if...#endif group and return (end_index, bodies).
        bodies is a list of (tag, body_s, body_e) for each if/elif/else branch.
        """
        h = ಆರಂಭ
        # Skip backslash-continued header lines
        ಆಗ ನಿಜ:
            h += 1
            ಆದರೆ h >= n ಅಥವಾ ಅಲ್ಲ ತುದಿಗಳು_with_bs(ಸಾಲುಗಳು[h - 1]):
                ನಿಲ್ಲಿಸು
        ಆಳ = 1
        j = h
        marks: List[Tuple[str, ಪೂರ್ಣಾಂಕ]] = [("if", ಆರಂಭ)]
        ಆಗ j < n ಮತ್ತು ಆಳ > 0:
            m = _DIR_RE.ಹೊಂದಿಕೆ(ಸಾಲುಗಳು[j])
            ಆದರೆ m:
                kw2 = m.ಗುಂಪು(1)
                ಆದರೆ kw2 ಒಳಗೆ ("if", "ifdef", "ifndef"):
                    ಆಳ += 1
                ಅಥವಾಆದರೆ kw2 == "endif":
                    ಆಳ -= 1
                    ಆದರೆ ಆಳ == 0:
                        ನಿಲ್ಲಿಸು
                ಅಥವಾಆದರೆ ಆಳ == 1 ಮತ್ತು kw2 ಒಳಗೆ ("elif", "else"):
                    marks.append((kw2, j))
            j += 1
        ಇಲ್ಲದಿದ್ದರೆ:
            # Unterminated: keep as-is from start to EOF
            ಹಿಂತಿರುಗಿ n, [("text", ಆರಂಭ, n)]

        ವ್ಯಾಖ್ಯಾನ after_ಶೀರ್ಷಿಕೆ_ಸಾಲು(pos: ಪೂರ್ಣಾಂಕ) -> ಪೂರ್ಣಾಂಕ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ಆಗ k < n ಮತ್ತು ತುದಿಗಳು_with_bs(ಸಾಲುಗಳು[k - 1]):
                k += 1
            ಹಿಂತಿರುಗಿ k

        ವ್ಯಾಖ್ಯಾನ after_ಶೀರ್ಷಿಕೆ_ಸಾಲು(pos: ಪೂರ್ಣಾಂಕ) -> ಪೂರ್ಣಾಂಕ:
            """
            Return the first index after a possibly backslash-continued header
            line starting at `pos`. It consumes all continuation lines that
            belong to the directive header.
            """
            k = pos + 1
            # Consume lines as long as the previous line ends with a backslash
            ಆಗ k < n ಮತ್ತು ತುದಿಗಳು_with_bs(ಸಾಲುಗಳು[k - 1]):
                k += 1
            ಹಿಂತಿರುಗಿ k

        bodies: List[Tuple[str, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ]] = []
        ಶೀರ್ಷಿಕೆ_positions = [pos ಗಾಗಿ _, pos ಒಳಗೆ marks] + [j]
        ಟ್ಯಾಗುಗಳು = [ಟ್ಯಾಗ್ ಗಾಗಿ ಟ್ಯಾಗ್, _ ಒಳಗೆ marks]
        ಗಾಗಿ idx, ಟ್ಯಾಗ್ ಒಳಗೆ enumerate(ಟ್ಯಾಗುಗಳು):
            ಶೀರ್ಷಿಕೆ = ಶೀರ್ಷಿಕೆ_positions[idx]
            # Start body right after the whole (possibly multi-line) header
            body_s = after_ಶೀರ್ಷಿಕೆ_ಸಾಲು(ಶೀರ್ಷಿಕೆ)
            ಮುಂದಿನ_ಶೀರ್ಷಿಕೆ = ಶೀರ್ಷಿಕೆ_positions[idx + 1]
            body_e = ಮುಂದಿನ_ಶೀರ್ಷಿಕೆ
            bodies.append((ಟ್ಯಾಗ್, body_s, body_e))
        ಹಿಂತಿರುಗಿ j + 1, bodies

    # Detect whether PROTO is defined from argv (clang args passed after src
    # path)
    proto_is_defined = _has_proto_defined_from_argv(sys.argv[2:])

    ಆಗ i < n:
        ಸಾಲು = ಸಾಲುಗಳು[i]
        m = _DIR_RE.ಹೊಂದಿಕೆ(ಸಾಲು)
        ಆದರೆ ಅಲ್ಲ m:
            ಹೊರಹೋಗು.append(ಸಾಲು)
            i += 1
            ಮುಂದುವರಿ

        kw = m.ಗುಂಪು(1)
        ಆದರೆ kw ಒಳಗೆ ("if", "ifdef", "ifndef"):
            # Hard drops first (legacy behavior)
            ಆದರೆ _IF0_RE.ಹೊಂದಿಕೆ(ಸಾಲು) ಅಥವಾ _IFDEF_DEBUG_RE.ಹೊಂದಿಕೆ(ಸಾಲು) ಅಥವಾ _IFNDEF_PROTO_RE.ಹೊಂದಿಕೆ(ಸಾಲು) ಅಥವಾ _IF_NOTDEF_PROTO_RE.ಹೊಂದಿಕೆ(ಸಾಲು):
                ಗುಂಪು_ಕೊನೆ, _ = collect_ಗುಂಪು(i)
                i = ಗುಂಪು_ಕೊನೆ
                ಮುಂದುವರಿ

            # If '#if <expr>' contains defined(PROTO), evaluate; otherwise,
            # legacy behavior
            evaluate = (kw == "if") ಮತ್ತು (
                _DEFINED_CALL_RE.ಹುಡುಕು(ಸಾಲು) ಆಗಿರುವ ಅಲ್ಲ ಏನೂಇಲ್ಲ
            )

            ಆದರೆ evaluate:
                # Extract condition text after 'if'
                ಪ್ರಯತ್ನಿಸು:
                    cond_ಪಠ್ಯ = ಸಾಲು.ವಿಭಜಿಸು("if", 1)[1]
                ಅಪವಾದ Exception:
                    cond_ಪಠ್ಯ = ""
                keep_ಮೊದಲ = _eval_condition_with_defined_proto(cond_ಪಠ್ಯ, proto_is_defined)
                ಆದರೆ ಅಲ್ಲ keep_ಮೊದಲ:
                    ಗುಂಪು_ಕೊನೆ, _ = collect_ಗುಂಪು(i)
                    i = ಗುಂಪು_ಕೊನೆ
                    ಮುಂದುವರಿ

            # Keep only the first branch
            ಗುಂಪು_ಕೊನೆ, bodies = collect_ಗುಂಪು(i)
            ಆದರೆ ಅಲ್ಲ bodies:
                i = ಗುಂಪು_ಕೊನೆ
                ಮುಂದುವರಿ
            keep_s, keep_e = bodies[0][1], bodies[0][2]
            kept_ಪಠ್ಯ = "".join(ಸಾಲುಗಳು[keep_s:keep_e])
            kept_rewritten = rewrite_conditionals_ಮೊದಲ_branch(kept_ಪಠ್ಯ)

            ಆದರೆ DEBUG_LOG:
                ಮುದ್ರಿಸು(f"[group] first body lines {keep_s}:{keep_e}", file=sys.stderr)
                # Print the first non-empty line of the kept body for quick
                # context
                ಗಾಗಿ ln ಒಳಗೆ kept_ಪಠ್ಯ.splitlines():
                    ಆದರೆ ln.strip():
                        ಮುದ್ರಿಸು(f"[group] kept starts with: {ln.strip()}", file=sys.stderr)
                        ನಿಲ್ಲಿಸು

            ಹೊರಹೋಗು.append(kept_rewritten)
            i = ಗುಂಪು_ಕೊನೆ
        ಇಲ್ಲದಿದ್ದರೆ:
            # For 'elif/else/endif' lines encountered directly, output a blank
            # line to preserve line count
            ಹೊರಹೋಗು.append("\n" ಆದರೆ ಸಾಲು.endswith("\n") ಇಲ್ಲದಿದ್ದರೆ "")
            i += 1

    ಹಿಂತಿರುಗಿ "".join(ಹೊರಹೋಗು)

ವ್ಯಾಖ್ಯಾನ collapse_ನಕ್ಷತ್ರ_spaces(s: str) -> str:
    ಹಿಂತಿರುಗಿ re.sub(r"\*\s+", "*", s)

ವ್ಯಾಖ್ಯಾನ fix_spacing(s: str) -> str:
    s = re.sub(r"\(\s+", "(", s)
    s = re.sub(r"\s+\)", ")", s)
    s = re.sub(r"(?<=[\w\)])\s+\((?!\*)", "(", s)
    s = re.sub(r"\s+,", ",", s)
    s = re.sub(r",(?=[^\s),])", ", ", s)
    s = re.sub(r"\s+\[", "[", s)
    s = re.sub(r"\[\s+", "[", s)
    s = re.sub(r"\s+\]", "]", s)
    ಹಿಂತಿರುಗಿ s

ವ್ಯಾಖ್ಯಾನ strip_ಬಳಸದ_macros(s: str) -> str:
    s = re.sub(r"\bUNUSED\b\s*\([^)]*\)", " ", s)
    s = re.sub(r"\bUNUSED\b", " ", s)
    ಹಿಂತಿರುಗಿ s

ವ್ಯಾಖ್ಯಾನ normalize_ws(s: str) -> str:
    ಹಿಂತಿರುಗಿ re.sub(r"\s+", " ", s).strip()

ವ್ಯಾಖ್ಯಾನ ಟೋಕನುಗಳು_for_function_ಶೀರ್ಷಿಕೆ(cur) -> List[str]:
    toks = [t ಗಾಗಿ t ಒಳಗೆ cur.get_ಟೋಕನುಗಳು() ಆದರೆ t.ಬಗೆ != Token_ಬಗೆ.COMMENT]
    ಆದರೆ ಅಲ್ಲ toks:
        ಹಿಂತಿರುಗಿ []
    name_idx = ಏನೂಇಲ್ಲ
    ಗಾಗಿ i, t ಒಳಗೆ enumerate(toks):
        ಆದರೆ t.ಬಗೆ == Token_ಬಗೆ.IDENTIFIER ಮತ್ತು t.spelling == cur.spelling:
            name_idx = i
            ನಿಲ್ಲಿಸು
    ಆದರೆ name_idx ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        ಹಿಂತಿರುಗಿ []
    lpar_idx = ಏನೂಇಲ್ಲ
    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(name_idx + 1, ಉದ್ದ(toks)):
        ಆದರೆ toks[i].spelling == '(':
            lpar_idx = i
            ನಿಲ್ಲಿಸು
    ಆದರೆ lpar_idx ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        ಹಿಂತಿರುಗಿ []
    ಆಳ, rpar_idx = 0, ಏನೂಇಲ್ಲ
    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(lpar_idx, ಉದ್ದ(toks)):
        ಆದರೆ toks[i].spelling == '(':
            ಆಳ += 1
        ಅಥವಾಆದರೆ toks[i].spelling == ')':
            ಆಳ -= 1
            ಆದರೆ ಆಳ == 0:
                rpar_idx = i
                ನಿಲ್ಲಿಸು
    ಆದರೆ rpar_idx ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        ಹಿಂತಿರುಗಿ []
    ಹಿಂತಿರುಗಿ [t.spelling ಗಾಗಿ t ಒಳಗೆ toks[:rpar_idx + 1]]

ವ್ಯಾಖ್ಯಾನ join_ಟೋಕನುಗಳು(ಟೋಕನುಗಳು: List[str]) -> str:
    s = " ".join(ಟೋಕನುಗಳು)
    s = strip_ಬಳಸದ_macros(s)
    s = normalize_ws(s)
    s = collapse_ನಕ್ಷತ್ರ_spaces(s)
    s = fix_spacing(s)
    ಹಿಂತಿರುಗಿ s

ವ್ಯಾಖ್ಯಾನ ಶೀರ್ಷಿಕೆ_from_cursor(cur) -> str:
    """
    Fallback header builder for cases where get_tokens() returns empty
    (e.g., macro-expanded function definitions). Uses cursor spelling and type
    information to construct "ret name(param, ...)".
    """
    ಪ್ರಯತ್ನಿಸು:
        ret = getattr(cur, "result_type", ಏನೂಇಲ್ಲ)
        ret_sp = ret.spelling ಆದರೆ ret ಆಗಿರುವ ಅಲ್ಲ ಏನೂಇಲ್ಲ ಇಲ್ಲದಿದ್ದರೆ ""
    ಅಪವಾದ Exception:
        ret_sp = ""
    ಆದರೆ ಅಲ್ಲ ret_sp:
        # As a last resort, try cur.type.get_result() if available.
        ಪ್ರಯತ್ನಿಸು:
            ret_sp = cur.type.get_ಫಲಿತಾಂಶ().spelling
        ಅಪವಾದ Exception:
            ret_sp = "void"
    name_sp = cur.spelling ಅಥವಾ ""
    params = []
    ಪ್ರಯತ್ನಿಸು:
        ಗಾಗಿ a ಒಳಗೆ cur.get_arguments() ಅಥವಾ []:
            t = getattr(a, "type", ಏನೂಇಲ್ಲ)
            t_sp = t.spelling ಆದರೆ t ಆಗಿರುವ ಅಲ್ಲ ಏನೂಇಲ್ಲ ಇಲ್ಲದಿದ್ದರೆ ""
            a_sp = a.spelling ಅಥವಾ ""
            seg = (t_sp + (" " + a_sp ಆದರೆ a_sp ಇಲ್ಲದಿದ್ದರೆ "")).strip()
            params.append(seg ಆದರೆ seg ಇಲ್ಲದಿದ್ದರೆ "void")
    ಅಪವಾದ Exception:
        ದಾಟು
    params_s = ", ".join(params) ಆದರೆ params ಇಲ್ಲದಿದ್ದರೆ "void"
    s = f"{ret_sp} {name_sp}({params_s})"
    s = strip_ಬಳಸದ_macros(s)
    s = normalize_ws(s)
    s = collapse_ನಕ್ಷತ್ರ_spaces(s)
    s = fix_spacing(s)
    ಹಿಂತಿರುಗಿ s

ವ್ಯಾಖ್ಯಾನ ಒಳ_this_file(cur, src_ಮಾರ್ಗ: ಮಾರ್ಗ_2) -> bool:
    ಪ್ರಯತ್ನಿಸು:
        loc = cur.extent.ಆರಂಭ
        f = (loc.file ಅಥವಾ cur.location.file)
        ಆದರೆ ಅಲ್ಲ f:
            # Macro-expanded function definitions may lack an attached file.
            # Allow them only if we can actually extract a concrete function
            # header.
            ಪ್ರಯತ್ನಿಸು:
                toks = ಟೋಕನುಗಳು_for_function_ಶೀರ್ಷಿಕೆ(cur)
                ಹಿಂತಿರುಗಿ bool(toks)
            ಅಪವಾದ Exception:
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
        cur_ಮಾರ್ಗ = os.ಮಾರ್ಗ.normcase(os.ಮಾರ್ಗ.abspath(str(f.name)))
        src_abs  = os.ಮಾರ್ಗ.normcase(os.ಮಾರ್ಗ.abspath(src_ಮಾರ್ಗ.as_posix()))
        # Accept the main source file
        ಆದರೆ cur_ಮಾರ್ಗ == src_abs ಅಥವಾ os.ಮಾರ್ಗ.basename(cur_ಮಾರ್ಗ) == os.ಮಾರ್ಗ.basename(src_abs):
            ಹಿಂತಿರುಗಿ ನಿಜ
        # Additionally accept quoted .c includes located in the same directory
        # (e.g., #include "regexp_nfa.c" next to the main source).
        src_dir = os.ಮಾರ್ಗ.normcase(os.ಮಾರ್ಗ.abspath(src_ಮಾರ್ಗ.ಪೋಷಕ.as_posix()))
        ಆದರೆ cur_ಮಾರ್ಗ.endswith(".c") ಮತ್ತು os.ಮಾರ್ಗ.dirname(cur_ಮಾರ್ಗ) == src_dir:
            ಹಿಂತಿರುಗಿ ನಿಜ
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು
    ಅಪವಾದ Exception:
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು

ವ್ಯಾಖ್ಯಾನ collect_function_defs(tu, src_ಮಾರ್ಗ: ಮಾರ್ಗ_2):
    ವ್ಯಾಖ್ಯಾನ walk(ಗಂಟು, ಆಳ=0):
        ಆದರೆ DEBUG_LOG:
            ಮುದ್ರಿಸು(f"{'  '*depth}{node.kind} {node.spelling}", file=sys.stderr)
        ಆದರೆ ಗಂಟು.ಬಗೆ == Cursor_ಬಗೆ.FUNCTION_DECL:
            ಆದರೆ ಒಳ_this_file(ಗಂಟು, src_ಮಾರ್ಗ):
                ಆದರೆ ಗಂಟು.storage_class != StorageClass.STATIC ಮತ್ತು ಗಂಟು.is_definition():
                    ನೀಡು ಗಂಟು
        ಗಾಗಿ ch ಒಳಗೆ ಗಂಟು.get_children():
            ನೀಡು ತಃ walk(ch, ಆಳ+1)
    ನೀಡು ತಃ walk(tu.cursor)

ವ್ಯಾಖ್ಯಾನ parse_with_clang(src_ಮಾರ್ಗ: ಮಾರ್ಗ_2, ಪಠ್ಯ: str, ವಾದಗಳು: List[str]):
    ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ_3.create()
    src_abs = os.ಮಾರ್ಗ.abspath(src_ಮಾರ್ಗ.as_posix())
    unsaved = [(src_abs, ಪಠ್ಯ)]
    proto_h = src_ಮಾರ್ಗ.ಪೋಷಕ / "proto.h"
    ಆದರೆ proto_h.exists():
        unsaved.append((str(proto_h), "/* stubbed */\n"))
    final_ವಾದಗಳು = (ವಾದಗಳು ಅಥವಾ []) + [
        "-std=c99", "-ferror-limit=0", "-w",
        "-Wno-implicit-function-declaration"
    ]
    tu = ಸೂಚ್ಯಂಕ.parse(src_abs, ವಾದಗಳು=final_ವಾದಗಳು, unsaved_files=unsaved, options=0)
    ಆದರೆ SHOW_DIAGS:
        ಗಾಗಿ d ಒಳಗೆ getattr(tu, "diagnostics", []):
            ಮುದ್ರಿಸು(d, file=sys.stderr)
    ಹಿಂತಿರುಗಿ tu

ವ್ಯಾಖ್ಯಾನ generate_prototypes(tu, src_ಮಾರ್ಗ: ಮಾರ್ಗ_2) -> List[str]:
    """Collect unique function prototypes from a translation unit."""
    protos: List[str] = []
    ಗಾಗಿ fn ಒಳಗೆ collect_function_defs(tu, src_ಮಾರ್ಗ):
        ಆದರೆ DEBUG_LOG:
            ಪ್ರಯತ್ನಿಸು:
                sr = fn.extent.ಆರಂಭ
                er = fn.extent.ಕೊನೆ
                ಮುದ್ರಿಸು(f"[def] {fn.spelling}  {sr.line}:{sr.column} - {er.line}:{er.column}", file=sys.stderr)
            ಅಪವಾದ Exception:
                ಮುದ್ರಿಸು(f"[def] {fn.spelling}", file=sys.stderr)
        toks = ಟೋಕನುಗಳು_for_function_ಶೀರ್ಷಿಕೆ(fn)
        ಶೀರ್ಷಿಕೆ = join_ಟೋಕನುಗಳು(toks) ಆದರೆ toks ಇಲ್ಲದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_from_cursor(fn)
        # libclang expands the bool macro (from <stdbool.h>) to _Bool.
        # Restore it to bool for readability.
        ಶೀರ್ಷಿಕೆ = re.sub(r'\b_Bool\b', 'bool', ಶೀರ್ಷಿಕೆ)
        ಆದರೆ ಶೀರ್ಷಿಕೆ:
            protos.append(f"{header};")

    ನೋಡಿದ, unique = ಗಣ_2(), []
    ಗಾಗಿ p ಒಳಗೆ protos:
        ಆದರೆ p ಅಲ್ಲ ಒಳಗೆ ನೋಡಿದ:
            unique.append(p)
            ನೋಡಿದ.ಕೂಡಿಸು(p)
    ಹಿಂತಿರುಗಿ unique

ವ್ಯಾಖ್ಯಾನ write_prototypes(ಹೊರಹೋಗು_ಮಾರ್ಗ: ಮಾರ್ಗ_2, ಶೀರ್ಷಿಕೆಗಳು: List[str], src_name: str) -> ಏನೂಇಲ್ಲ:
    """Write collected prototypes to the output .pro file."""
    ಹೊರಹೋಗು_ಮಾರ್ಗ.ಪೋಷಕ.mkdir(parents=ನಿಜ, exist_ok=ನಿಜ)
    ಪ್ರಯತ್ನಿಸು:
        ಜೊತೆ ಹೊರಹೋಗು_ಮಾರ್ಗ.open("w", encoding="utf-8", newline="\n") ಇವ f:
            f.ಲಿಖ(f"// {src_name}\n")
            ಗಾಗಿ h ಒಳಗೆ ಶೀರ್ಷಿಕೆಗಳು:
                f.ಲಿಖ(h + "\n")
            f.ಲಿಖ("// vim: ft=c\n")
    ಅಪವಾದ Exception ಇವ e:
        ಮುದ್ರಿಸು(f"write failed: {e}", file=sys.stderr)
        sys.exit(4)
    ಮುದ್ರಿಸು(f"Generated {out_path.as_posix()} with {len(headers)} prototypes.")

ವ್ಯಾಖ್ಯಾನ ಮುಖ್ಯ():
    ವಾದಗಳು = sys.argv[1:]
    ಆದರೆ ಅಲ್ಲ ವಾದಗಳು:
        ಮುದ್ರಿಸು("Usage: gen_prototypes.py source.c [clang-args...]", file=sys.stderr)
        sys.exit(1)

    src_ಮಾರ್ಗ = ಮಾರ್ಗ_2(ವಾದಗಳು[0])
    clang_ವಾದಗಳು = ವಾದಗಳು[1:]
    ಆದರೆ ಅಲ್ಲ src_ಮಾರ್ಗ.exists():
        ಮುದ್ರಿಸು(f"not found: {src_path}", file=sys.stderr)
        sys.exit(2)

    # Load and preprocess source
    ಪ್ರಯತ್ನಿಸು:
        original = src_ಮಾರ್ಗ.read_ಪಠ್ಯ(encoding="utf-8", errors="ignore")
    ಅಪವಾದ Exception ಇವ e:
        ಮುದ್ರಿಸು(f"read error: {e}", file=sys.stderr)
        sys.exit(2)
    rewritten = rewrite_conditionals_ಮೊದಲ_branch(original)

    # Parse with libclang
    ಪ್ರಯತ್ನಿಸು:
        tu = parse_with_clang(src_ಮಾರ್ಗ, rewritten, clang_ವಾದಗಳು)
    ಅಪವಾದ Exception ಇವ e:
        ಮುದ್ರಿಸು(f"parse failed: {e}", file=sys.stderr)
        sys.exit(3)

    ಶೀರ್ಷಿಕೆಗಳು = generate_prototypes(tu, src_ಮಾರ್ಗ)
    ಹೊರಹೋಗು_file = ಮಾರ್ಗ_2("proto") / (src_ಮಾರ್ಗ.stem + ".pro")
    write_prototypes(ಹೊರಹೋಗು_file, ಶೀರ್ಷಿಕೆಗಳು, src_ಮಾರ್ಗ.name)

ಆದರೆ __name__ == "__main__":
    ಮುಖ್ಯ()
