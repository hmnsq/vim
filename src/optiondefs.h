/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * optiondefs.h: option definitions
 */

// The options that are local to a window or buffer have "indir" set to one of
// these values.  Special values:
// PV_NONE: global option.
// PV_WIN is added: window-local option
// PV_BUF is added: buffer-local option
// PV_BOTH is added: global option which also has a local value.
#ವ್ಯಾಖ್ಯಾನಿಸು PV_BOTH 0x1000
#ವ್ಯಾಖ್ಯಾನಿಸು PV_WIN  0x2000
#ವ್ಯಾಖ್ಯಾನಿಸು PV_BUF  0x4000
#ವ್ಯಾಖ್ಯಾನಿಸು PV_ಮುಖವಾಡ 0x0fff
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_WIN(x)  (idopt_T)(PV_WIN + (ಪೂರ್ಣಾಂಕ)(x))
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_BUF(x)  (idopt_T)(PV_BUF + (ಪೂರ್ಣಾಂಕ)(x))
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_BOTH(x) (idopt_T)(PV_BOTH + (ಪೂರ್ಣಾಂಕ)(x))

// Definition of the PV_ values for buffer-local options.
// The BV_ values are defined in option.h.
#ವ್ಯಾಖ್ಯಾನಿಸು PV_AC		OPT_BOTH(OPT_BUF(BV_AC))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_AI		OPT_BUF(BV_AI)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_AR		OPT_BOTH(OPT_BUF(BV_AR))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_BKC		OPT_BOTH(OPT_BUF(BV_BKC))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_BH		OPT_BUF(BV_BH)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_BT		OPT_BUF(BV_BT)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
# define PV_EFM		OPT_BOTH(OPT_BUF(BV_EFM))
# define PV_GEFM	OPT_BOTH(OPT_BUF(BV_GEFM))
# define PV_GP		OPT_BOTH(OPT_BUF(BV_GP))
# define PV_MP		OPT_BOTH(OPT_BUF(BV_MP))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_BIN		OPT_BUF(BV_BIN)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_BL		OPT_BUF(BV_BL)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_BOMB		OPT_BUF(BV_BOMB)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CI		OPT_BUF(BV_CI)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CIN		OPT_BUF(BV_CIN)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CINK		OPT_BUF(BV_CINK)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CINO		OPT_BUF(BV_CINO)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CINSD	OPT_BUF(BV_CINSD)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CINW		OPT_BUF(BV_CINW)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CM		OPT_BOTH(OPT_BUF(BV_CM))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CMS		OPT_BUF(BV_CMS)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_COM		OPT_BUF(BV_COM)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_COT		OPT_BOTH(OPT_BUF(BV_COT))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CPT		OPT_BUF(BV_CPT)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_DICT		OPT_BOTH(OPT_BUF(BV_DICT))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
# define PV_DIA		OPT_BOTH(OPT_BUF(BV_DIA))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_TSR		OPT_BOTH(OPT_BUF(BV_TSR))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FFU		OPT_BOTH(OPT_BUF(BV_FFU))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CSL		OPT_BUF(BV_CSL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
# define PV_CFU		OPT_BUF(BV_CFU)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
# define PV_DEF		OPT_BOTH(OPT_BUF(BV_DEF))
# define PV_INC		OPT_BOTH(OPT_BUF(BV_INC))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_EOF		OPT_BUF(BV_EOF)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_EOL		OPT_BUF(BV_EOL)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FIXEOL	OPT_BUF(BV_FIXEOL)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_EP		OPT_BOTH(OPT_BUF(BV_EP))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_ET		OPT_BUF(BV_ET)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FENC		OPT_BUF(BV_FENC)
#ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
# define PV_BEXPR	OPT_BOTH(OPT_BUF(BV_BEXPR))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FP		OPT_BOTH(OPT_BUF(BV_FP))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# define PV_FEX		OPT_BUF(BV_FEX)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FF		OPT_BUF(BV_FF)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FLP		OPT_BUF(BV_FLP)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FO		OPT_BUF(BV_FO)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
# define PV_FS		OPT_BOTH(OPT_BUF(BV_FS))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FT		OPT_BUF(BV_FT)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_IMI		OPT_BUF(BV_IMI)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_IMS		OPT_BUF(BV_IMS)
#ಆದರೆ defined(FEAT_EVAL)
# define PV_INDE	OPT_BUF(BV_INDE)
# define PV_INDK	OPT_BUF(BV_INDK)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
# define PV_INEX	OPT_BUF(BV_INEX)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_INF		OPT_BUF(BV_INF)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_ISK		OPT_BUF(BV_ISK)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
# define PV_ಕೀಲಿ		OPT_BUF(BV_ಕೀಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
# define PV_KMAP	OPT_BUF(BV_KMAP)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_KP		OPT_BOTH(OPT_BUF(BV_KP))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_LISP		OPT_BUF(BV_LISP)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_LOP		OPT_BUF(BV_LOP)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_LW		OPT_BOTH(OPT_BUF(BV_LW))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_MENC		OPT_BOTH(OPT_BUF(BV_MENC))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_MA		OPT_BUF(BV_MA)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_ML		OPT_BUF(BV_ML)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_MOD		OPT_BUF(BV_MOD)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_MPS		OPT_BUF(BV_MPS)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_NF		OPT_BUF(BV_NF)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
# define PV_OFU		OPT_BUF(BV_OFU)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_ಮಾರ್ಗ		OPT_BOTH(OPT_BUF(BV_ಮಾರ್ಗ))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_PI		OPT_BUF(BV_PI)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_QE		OPT_BUF(BV_QE)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_RO		OPT_BUF(BV_RO)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SI		OPT_BUF(BV_SI)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SN		OPT_BUF(BV_SN)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
# define PV_SMC		OPT_BUF(BV_SMC)
# define PV_SYN		OPT_BUF(BV_SYN)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
# define PV_SPC		OPT_BUF(BV_SPC)
# define PV_SPF		OPT_BUF(BV_SPF)
# define PV_SPL		OPT_BUF(BV_SPL)
# define PV_SPO		OPT_BUF(BV_SPO)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_STS		OPT_BUF(BV_STS)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SUA		OPT_BUF(BV_SUA)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SW		OPT_BUF(BV_SW)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SWF		OPT_BUF(BV_SWF)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# define PV_TFU		OPT_BUF(BV_TFU)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
# define PV_TSRFU	OPT_BOTH(OPT_BUF(BV_TSRFU))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_ಟ್ಯಾಗುಗಳು		OPT_BOTH(OPT_BUF(BV_ಟ್ಯಾಗುಗಳು))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_TC		OPT_BOTH(OPT_BUF(BV_TC))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_TS		OPT_BUF(BV_TS)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_TW		OPT_BUF(BV_TW)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_TX		OPT_BUF(BV_TX)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
# define PV_UDF		OPT_BUF(BV_UDF)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_WM		OPT_BUF(BV_WM)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
# define PV_VSTS	OPT_BUF(BV_VSTS)
# define PV_VTS		OPT_BUF(BV_VTS)
#ಆದರೆ_ಅಂತ್ಯ

// Definition of the PV_ values for window-local options.
// The WV_ values are defined in option.h.
#ವ್ಯಾಖ್ಯಾನಿಸು PV_LIST		OPT_WIN(WV_LIST)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
# define PV_ARAB	OPT_WIN(WV_ARAB)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
# define PV_BRI		OPT_WIN(WV_BRI)
# define PV_BRIOPT	OPT_WIN(WV_BRIOPT)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_WCR		OPT_WIN(WV_WCR)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
# define PV_DIFF	OPT_WIN(WV_DIFF)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_EIW		OPT_WIN(WV_EIW)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
# define PV_FDC		OPT_WIN(WV_FDC)
# define PV_FEN		OPT_WIN(WV_FEN)
# define PV_FDI		OPT_WIN(WV_FDI)
# define PV_FDL		OPT_WIN(WV_FDL)
# define PV_FDM		OPT_WIN(WV_FDM)
# define PV_FML		OPT_WIN(WV_FML)
# define PV_FDN		OPT_WIN(WV_FDN)
# ifdef FEAT_EVAL
#  define PV_FDE	OPT_WIN(WV_FDE)
#  define PV_FDT	OPT_WIN(WV_FDT)
# endif
# define PV_FMR		OPT_WIN(WV_FMR)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
# define PV_LBR		OPT_WIN(WV_LBR)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_LCS		OPT_BOTH(OPT_WIN(WV_LCS))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_FCS		OPT_BOTH(OPT_WIN(WV_FCS))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_NU		OPT_WIN(WV_NU)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_RNU		OPT_WIN(WV_RNU)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_VE		OPT_BOTH(OPT_WIN(WV_VE))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
# define PV_NUW		OPT_WIN(WV_NUW)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_QUICKFIX)
# define PV_PVW		OPT_WIN(WV_PVW)
# define PV_LHI         OPT_WIN(WV_LHI)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
# define PV_RL		OPT_WIN(WV_RL)
# define PV_RLC		OPT_WIN(WV_RLC)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SCBIND	OPT_WIN(WV_SCBIND)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SCROLL	OPT_WIN(WV_SCROLL)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SMS		OPT_WIN(WV_SMS)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SISO		OPT_BOTH(OPT_WIN(WV_SISO))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SO		OPT_BOTH(OPT_WIN(WV_SO))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_SOP		OPT_BOTH(OPT_WIN(WV_SOP))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
# define PV_SPELL	OPT_WIN(WV_SPELL)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
# define PV_CUC		OPT_WIN(WV_CUC)
# define PV_CUL		OPT_WIN(WV_CUL)
# define PV_CULOPT	OPT_WIN(WV_CULOPT)
# define PV_CC		OPT_WIN(WV_CC)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
# define PV_SBR		OPT_BOTH(OPT_WIN(WV_SBR))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
# define PV_STL		OPT_BOTH(OPT_WIN(WV_STL))
# define PV_STLO	OPT_BOTH(OPT_WIN(WV_STLO))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PV_UL		OPT_BOTH(OPT_BUF(BV_UL))
#ವ್ಯಾಖ್ಯಾನಿಸು PV_WFB		OPT_WIN(WV_WFB)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_WFH		OPT_WIN(WV_WFH)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_WFW		OPT_WIN(WV_WFW)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_WHL		OPT_WIN(WV_WHL)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_WRAP		OPT_WIN(WV_WRAP)
#ವ್ಯಾಖ್ಯಾನಿಸು PV_CRBIND	OPT_WIN(WV_CRBIND)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
# define PV_COCU	OPT_WIN(WV_COCU)
# define PV_COLE	OPT_WIN(WV_COLE)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
# define PV_TWK		OPT_WIN(WV_TWK)
# define PV_TWS		OPT_WIN(WV_TWS)
# define PV_TWSL	OPT_BUF(BV_TWSL)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
# define PV_SCL		OPT_WIN(WV_SCL)
#ಆದರೆ_ಅಂತ್ಯ

// WV_ and BV_ values get typecasted to this for the "indir" field
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    PV_NONE = 0,
    PV_MAXVAL = 0xffff    // to avoid warnings for value out of range
} idopt_T;

// Options local to a window have a value local to a buffer and global to all
// buffers.  Indicate this by setting "var" to VAR_WIN.
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_WIN ((char_u *)-1)

// Saved values for when 'bin' is set.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	p_et_nobin;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	p_ml_nobin;
ಸ್ಥಿರ ದೀರ್ಘ	p_tw_nobin;
ಸ್ಥಿರ ದೀರ್ಘ	p_wm_nobin;

// Saved values for when 'paste' is set
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	p_ai_nopaste;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	p_et_nopaste;
ಸ್ಥಿರ ದೀರ್ಘ	p_sts_nopaste;
ಸ್ಥಿರ ದೀರ್ಘ	p_tw_nopaste;
ಸ್ಥಿರ ದೀರ್ಘ	p_wm_nopaste;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
ಸ್ಥಿರ char_u	*p_vsts_nopaste;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ vimoption
{
    ಅಕ್ಷರ	*fullname;	// full option name
    ಅಕ್ಷರ	*shortname;	// permissible abbreviation
    long_u	flags;		// see in option.h
    char_u	*var;		// global option: pointer to variable;
				// window-local option: VAR_WIN;
				// buffer-local option: global value
    idopt_T	indir;		// global option: PV_NONE;
				// local option: indirect option index
    // callback function to invoke after an option is modified to validate and
    // apply the new value.
    opt_did_ಗಣ_cb_T	opt_did_ಗಣ_cb;

    // callback function to invoke when expanding possible values on the
    // cmdline. Only useful for string options.
    opt_expand_cb_T	opt_expand_cb;

    char_u	*def_val[2];	// default values for variable (vi and vim)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    sctx_T	script_ctx;	// script context where the option was last set
# define SCTX_INIT , {0, 0, 0, 1}
#ಇಲ್ಲದಿದ್ದರೆ
# define SCTX_INIT
#ಆದರೆ_ಅಂತ್ಯ
};

#ವ್ಯಾಖ್ಯಾನಿಸು VI_DEFAULT  0	    // def_val[VI_DEFAULT] is Vi default value
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_DEFAULT 1	    // def_val[VIM_DEFAULT] is Vim default value

#ವ್ಯಾಖ್ಯಾನಿಸು ISK_LATIN1  (char_u *)"@,48-57,_,192-255"

// 'isprint' for latin1 is also used for MS-Windows cp1252, where 0x80 is used
// for the currency sign.
#ಆದರೆ defined(MSWIN)
# define ISP_LATIN1 (char_u *)"@,~-255"
#ಇಲ್ಲದಿದ್ದರೆ
# define ISP_LATIN1 (char_u *)"@,161-255"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು HIGHLIGHT_INIT "8:SpecialKey,~:EndOfBuffer,@:NonText,d:Directory,e:ErrorMsg,i:IncSearch,l:Search,y:CurSearch,m:MoreMsg,M:ModeMsg,n:LineNr,a:LineNrAbove,b:LineNrBelow,N:CursorLineNr,G:CursorLineSign,O:CursorLineFold,r:Question,s:StatusLine,S:StatusLineNC,c:VertSplit,|:VertSplitNC,t:Title,v:Visual,V:VisualNOS,w:WarningMsg,W:WildMenu,f:Folded,F:FoldColumn,A:DiffAdd,C:DiffChange,D:DiffDelete,T:DiffText,E:DiffTextAdd,>:SignColumn,-:Conceal,B:SpellBad,P:SpellCap,R:SpellRare,L:SpellLocal,+:Pmenu,=:PmenuSel,k:PmenuMatch,<:PmenuMatchSel,[:PmenuKind,]:PmenuKindSel,{:PmenuExtra,}:PmenuExtraSel,x:PmenuSbar,X:PmenuThumb,j:PmenuBorder,H:PmenuShadow,p:Popup,J:PopupBorder,Q:PopupTitle,*:TabLine,#:TabLineSel,_:TabLineFill,!:CursorColumn,.:CursorLine,o:ColorColumn,q:QuickFixLine,z:StatusLineTerm,Z:StatusLineTermNC,g:MsgArea,h:ComplMatchIns,%:TabPanel,^:TabPanelSel,&:TabPanelFill,I:PreInsert"

// Default python version for pyx* commands
#ಆದರೆ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	0
#ಅಥವಾಆದರೆ defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	3
#ಅಥವಾಆದರೆ defined(FEAT_PYTHON)
# define DEFAULT_PYTHON_VER	2
#ಇಲ್ಲದಿದ್ದರೆ
# define DEFAULT_PYTHON_VER	0
#ಆದರೆ_ಅಂತ್ಯ

// used for 'cinkeys' and 'indentkeys'
#ವ್ಯಾಖ್ಯಾನಿಸು INDENTKEYS_DEFAULT (char_u *)"0{,0},0),0],:,0#,!^F,o,O,e"

// options[] is initialized here.
// The order of the options MUST be alphabetic for ":set all" and findoption().
// All option names MUST start with a lowercase letter (for findoption()).
// Exception: "t_" options are at the end.
// The options with a NULL variable are 'hidden': a set command for them is
// ignored and they are not printed.
ಸ್ಥಿರ ರಚನೆ vimoption options[] =
{
    {"aleph",	    "al",   P_ಅಂಕೆ|P_VI_DEF|P_CURSWANT,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    (char_u *)&p_aleph, PV_NONE,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE,
#ಆದರೆ_ಅಂತ್ಯ
			    ಖಾಲಿ, ಖಾಲಿ,
			    {
#ಆದರೆ defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)128L,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)224L,
#ಆದರೆ_ಅಂತ್ಯ
					    (char_u *)0L} SCTX_INIT},
    {"allowrevins", "ari",  P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    (char_u *)&p_ari, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"altkeymap",   "akm",  P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ambiwidth",  "ambw",  P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_ambw, PV_NONE, did_ಗಣ_ambiwidth, expand_ಗಣ_ambiwidth,
			    {(char_u *)"single", (char_u *)0L}
			    SCTX_INIT},
    {"antialias",   "anti", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"arabic",	    "arab", P_BOOL|P_VI_DEF|P_VIM|P_CURSWANT,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
			    (char_u *)VAR_WIN, PV_ARAB, did_ಗಣ_arabic, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"arabicshape", "arshape", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
			    (char_u *)&p_arshape, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"autochdir",  "acd",   P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
			    (char_u *)&p_acd, PV_NONE, did_ಗಣ_autochdir, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    {"autocomplete",  "ac", P_BOOL|P_VI_DEF,
			    (char_u *)&p_ac, PV_AC, ಖಾಲಿ,
			    ಖಾಲಿ, {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autocompletedelay", "acl", P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_acl, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"autocompletetimeout", "act", P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_act, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
#ಆದರೆ_ಅಂತ್ಯ
    {"autoindent",  "ai",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ai, PV_AI, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoprint",   "ap",   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoread",    "ar",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ar, PV_AR, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoshelldir",  "asd",   P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOSHELLDIR
			    (char_u *)&p_asd, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"autowrite",   "aw",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_aw, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autowriteall","awa",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_awa, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"background",  "bg",   P_STRING|P_VI_DEF|P_RCLR|P_HLONLY,
			    (char_u *)&p_bg, PV_NONE, did_ಗಣ_background, expand_ಗಣ_background,
			    {
#ಆದರೆ (defined(MSWIN)) && !defined(FEAT_GUI)
			    (char_u *)"dark",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"light",
#ಆದರೆ_ಅಂತ್ಯ
					    (char_u *)0L} SCTX_INIT},
    {"backspace",   "bs",   P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bs, PV_NONE, did_ಗಣ_backspace, expand_ಗಣ_backspace,
			    {(char_u *)"", (char_u *)"indent,eol,start"} SCTX_INIT},
    {"backup",	    "bk",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_bk, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"backupcopy",  "bkc",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bkc, PV_BKC, did_ಗಣ_backupcopy, expand_ಗಣ_backupcopy,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
			    {(char_u *)"yes", (char_u *)"auto"}
#ಇಲ್ಲದಿದ್ದರೆ
			    {(char_u *)"auto", (char_u *)"auto"}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"backupdir",   "bdir", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_bdir, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_BDIR, (char_u *)0L} SCTX_INIT},
    {"backupext",   "bex",  P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_bex, PV_NONE,
			    did_ಗಣ_backupext_or_patchmode, ಖಾಲಿ,
			    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
			    (char_u *)"_",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"~",
#ಆದರೆ_ಅಂತ್ಯ
					    (char_u *)0L} SCTX_INIT},
    {"backupskip",  "bsk",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bsk, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"balloondelay","bdlay",P_ಅಂಕೆ|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL
			    (char_u *)&p_bdlay, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)600L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"ballooneval", "beval",P_BOOL|P_VI_DEF|P_NO_MKRC,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
			    (char_u *)&p_beval, PV_NONE, did_ಗಣ_ballooneval, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"balloonevalterm", "bevalterm",P_BOOL|P_VI_DEF|P_NO_MKRC,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
			    (char_u *)&p_bevalterm, PV_NONE,
			    did_ಗಣ_balloonevalterm, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"balloonexpr", "bexpr", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
			    (char_u *)&p_bexpr, PV_BEXPR, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"beautify",    "bf",   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"belloff",      "bo",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_bo, PV_NONE, did_ಗಣ_belloff, expand_ಗಣ_belloff,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"binary",	    "bin",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bin, PV_BIN, did_ಗಣ_ದ್ವಿಮಾನ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"bioskey",	    "biosk",P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"bomb",	    ಖಾಲಿ,   P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bomb, PV_BOMB,
			    did_ಗಣ_eof_eol_fixeol_bomb, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"breakat",	    "brk",  P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
			    (char_u *)&p_breakat, PV_NONE, did_ಗಣ_breakat, ಖಾಲಿ,
			    {(char_u *)" \t!@*-+;:,./?", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"breakindent",   "bri",  P_BOOL|P_VI_DEF|P_VIM|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRI, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"breakindentopt", "briopt", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF
						  |P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRIOPT,
			    did_ಗಣ_breakindentopt, expand_ಗಣ_breakindentopt,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"browsedir",   "bsdir",P_STRING|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
			    (char_u *)&p_bsdir, PV_NONE, did_ಗಣ_browsedir, expand_ಗಣ_browsedir,
			    {(char_u *)"last", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"bufhidden",   "bh",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bh, PV_BH, did_ಗಣ_bufhidden, expand_ಗಣ_bufhidden,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"buflisted",   "bl",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bl, PV_BL, did_ಗಣ_buflisted, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L}
			    SCTX_INIT},
    {"buftype",	    "bt",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bt, PV_BT, did_ಗಣ_buftype, expand_ಗಣ_buftype,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"casemap",	    "cmp",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cmp, PV_NONE, did_ಗಣ_casemap, expand_ಗಣ_casemap,
			    {(char_u *)"internal,keepascii", (char_u *)0L}
			    SCTX_INIT},
    {"cdhome",	    "cdh",  P_BOOL|P_VI_DEF|P_VIM|P_SECURE,
			    (char_u *)&p_cdh, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"cdpath",	    "cd",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE|P_COMMA|P_NODUP,
			    (char_u *)&p_cdpath, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)",,", (char_u *)0L}
			    SCTX_INIT},
    {"cedit",	    ಖಾಲಿ,   P_STRING,
			    (char_u *)&p_cedit, PV_NONE, did_ಗಣ_cedit, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)CTRL_F_STR}
			    SCTX_INIT},
    {"charconvert",  "ccv", P_STRING|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(FEAT_EVAL)
			    (char_u *)&p_ccv, PV_NONE, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"chistory",    "chi",  P_ಅಂಕೆ|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)&p_chi, PV_NONE, did_ಗಣ_xhistory, ಖಾಲಿ,
			    {(char_u *)10L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"cindent",	    "cin",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_cin, PV_CIN, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cinkeys",	    "cink", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cink, PV_CINK, ಖಾಲಿ, ಖಾಲಿ,
			    {INDENTKEYS_DEFAULT, (char_u *)0L} SCTX_INIT},
    {"cinoptions",  "cino", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cino, PV_CINO, did_ಗಣ_cinoptions, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"cinscopedecls", "cinsd", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinsd, PV_CINSD, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"public,protected,private", (char_u *)0L}
			    SCTX_INIT},
    {"cinwords",    "cinw", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinw, PV_CINW, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"if,else,while,do,for,switch",
				(char_u *)0L}
			    SCTX_INIT},
    {"clipboard",   "cb",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
			    (char_u *)&p_cb, PV_NONE, did_ಗಣ_clipboard, expand_ಗಣ_clipboard,
# ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
			    {(char_u *)"autoselect,exclude:cons\\|linux",
							       (char_u *)0L}
# ಇಲ್ಲದಿದ್ದರೆ
			    {(char_u *)"", (char_u *)0L}
# endif
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"clipmethod", "cpm",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
			    (char_u *)&p_cpm, PV_NONE, did_ಗಣ_clipmethod, expand_ಗಣ_clipmethod,
# ifdef UNIX
			    {(char_u *)"wayland,x11", (char_u *)0L}
# elif defined(VMS)
			    {(char_u *)"x11", (char_u *)0L}
# ಇಲ್ಲದಿದ್ದರೆ
			    {(char_u *)"", (char_u *)0L}
# endif
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"cmdheight",   "ch",   P_ಅಂಕೆ|P_VI_DEF|P_RALL,
			    (char_u *)&p_ch, PV_NONE, did_ಗಣ_cmdheight, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"cmdwinheight", "cwh", P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_cwh, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)7L, (char_u *)0L} SCTX_INIT},
    {"colorcolumn", "cc",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN|P_HLONLY,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CC, did_ಗಣ_colorcolumn, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"columns",	    "co",   P_ಅಂಕೆ|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&Columns, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
    {"comments",    "com",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_com, PV_COM, did_ಗಣ_comments, ಖಾಲಿ,
			    {(char_u *)"s1:/*,mb:*,ex:*/,://,b:#,:%,:XCOMM,n:>,fb:-",
				(char_u *)0L}
			    SCTX_INIT},
    {"commentstring", "cms", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_cms, PV_CMS, did_ಗಣ_commentstring, ಖಾಲಿ,
			    {(char_u *)"/* %s */", (char_u *)0L}
			    SCTX_INIT},
			    // P_PRI_MKRC isn't needed here, optval_default()
			    // always returns TRUE for 'compatible'
    {"compatible",  "cp",   P_BOOL|P_RALL,
			    (char_u *)&p_cp, PV_NONE, did_ಗಣ_compatible, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)FALSE} SCTX_INIT},
    {"complete",    "cpt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP|P_MLE,
			    (char_u *)&p_cpt, PV_CPT, did_ಗಣ_complete, expand_ಗಣ_complete,
			    {(char_u *)".,w,b,u,t,i", (char_u *)0L}
			    SCTX_INIT},
    {"completefunc", "cfu", P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
			    (char_u *)&p_cfu, PV_CFU, did_ಗಣ_completefunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"completefuzzycollect", "cfc", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cfc, PV_NONE, did_ಗಣ_completefuzzycollect, expand_ಗಣ_completefuzzycollect,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"completeitemalign", "cia", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cia, PV_NONE, did_ಗಣ_completeitemalign, ಖಾಲಿ,
			    {(char_u *)"abbr,kind,menu", (char_u *)0L}
			    SCTX_INIT},
    {"completeopt",   "cot",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cot, PV_COT, did_ಗಣ_completeopt, expand_ಗಣ_completeopt,
			    {(char_u *)"menu,preview", (char_u *)0L}
			    SCTX_INIT},
    {"completepopup", "cpp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_cpp, PV_NONE, did_ಗಣ_completepopup, expand_ಗಣ_completepopup,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"completeslash",   "csl",  P_STRING|P_VI_DEF|P_VIM,
#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
			    (char_u *)&p_csl, PV_CSL, did_ಗಣ_completeslash, expand_ಗಣ_completeslash,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"completetimeout", "cto", P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_cto, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"concealcursor","cocu", P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF|P_FLAGLIST,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COCU, did_ಗಣ_concealcursor, expand_ಗಣ_concealcursor,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"conceallevel","cole", P_ಅಂಕೆ|P_RWIN|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COLE, did_ಗಣ_conceallevel, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)0L, (char_u *)0L}
			    SCTX_INIT},
    {"confirm",     "cf",   P_BOOL|P_VI_DEF,
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			    (char_u *)&p_confirm, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"conskey",	    "consk",P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"copyindent",  "ci",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ci, PV_CI, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cpoptions",   "cpo",  P_STRING|P_VIM|P_RALL|P_FLAGLIST,
			    (char_u *)&p_cpo, PV_NONE, did_ಗಣ_cpoptions, expand_ಗಣ_cpoptions,
			    {(char_u *)CPO_VI, (char_u *)CPO_VIM}
			    SCTX_INIT},
    {"cryptmethod", "cm",   P_STRING|P_ALLOCED|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
			    (char_u *)&p_cm, PV_CM, did_ಗಣ_cryptmethod, expand_ಗಣ_cryptmethod,
			    {(char_u *)"blowfish2", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"cscopepathcomp", "cspc", P_ಅಂಕೆ|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
			    (char_u *)&p_cspc, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeprg",   "csprg", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
			    (char_u *)&p_csprg, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"cscope", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"cscopequickfix", "csqf", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ಆದರೆ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_csqf, PV_NONE,
			    did_ಗಣ_cscopequickfix, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"cscoperelative", "csre", P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
			    (char_u *)&p_csre, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetag",   "cst",  P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
			    (char_u *)&p_cst, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetagorder", "csto", P_ಅಂಕೆ|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
			    (char_u *)&p_csto, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeverbose", "csverb", P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
			    (char_u *)&p_csverbose, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cursorbind",  "crb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_CRBIND, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorcolumn", "cuc", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUC, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorline",   "cul", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUL, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorlineopt", "culopt", P_STRING|P_VI_DEF|P_RWIN|P_HLONLY|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CULOPT,
			    did_ಗಣ_cursorlineopt, expand_ಗಣ_cursorlineopt,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"both", (char_u *)0L} SCTX_INIT},
    {"debug",	    ಖಾಲಿ,   P_STRING|P_VI_DEF,
			    (char_u *)&p_debug, PV_NONE, did_ಗಣ_debug, expand_ಗಣ_debug,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"define",	    "def",  P_STRING|P_ALLOCED|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
			    (char_u *)&p_def, PV_DEF, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"^\\s*#\\s*define", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"delcombine", "deco",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_deco, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"dictionary",  "dict", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_dict, PV_DICT, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"diff",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF|P_RWIN|P_NOGLOB,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			    (char_u *)VAR_WIN, PV_DIFF, did_ಗಣ_diff, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"diffanchors", "dia",  P_STRING|P_VI_DEF|P_ONECOMMA,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			    (char_u *)&p_dia, PV_DIA, did_ಗಣ_diffanchors, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)ಖಾಲಿ} SCTX_INIT},
    {"diffexpr",    "dex",  P_STRING|P_VI_DEF|P_SECURE|P_CURSWANT,
#ಆದರೆ defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_dex, PV_NONE, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"diffopt",	    "dip",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN|P_ONECOMMA|P_COLON
								     |P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			    (char_u *)&p_dip, PV_NONE, did_ಗಣ_diffopt, expand_ಗಣ_diffopt,
			    {(char_u *)"internal,filler,closeoff,indent-heuristic,inline:char",
								(char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"digraph",	    "dg",   P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
			    (char_u *)&p_dg, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"directory",   "dir",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_dir, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_DIR, (char_u *)0L} SCTX_INIT},
    {"display",	    "dy",   P_STRING|P_VI_DEF|P_ONECOMMA|P_RALL|P_NODUP,
			    (char_u *)&p_dy, PV_NONE, did_ಗಣ_display, expand_ಗಣ_display,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eadirection", "ead",  P_STRING|P_VI_DEF,
			    (char_u *)&p_ead, PV_NONE, did_ಗಣ_eadirection, expand_ಗಣ_eadirection,
			    {(char_u *)"both", (char_u *)0L}
			    SCTX_INIT},
    {"edcompatible","ed",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ed, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"emoji",      "emo",   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_emoji, PV_NONE, did_ಗಣ_ambiwidth, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L}
			    SCTX_INIT},
    {"encoding",    "enc",  P_STRING|P_VI_DEF|P_RCLR|P_NO_ML,
			    (char_u *)&p_enc, PV_NONE, did_ಗಣ_encoding, expand_ಗಣ_encoding,
			    {(char_u *)ENC_DFLT, (char_u *)0L}
			    SCTX_INIT},
    {"endoffile",   "eof",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eof, PV_EOF,
			    did_ಗಣ_eof_eol_fixeol_bomb, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"endofline",   "eol",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eol, PV_EOL,
			    did_ಗಣ_eof_eol_fixeol_bomb, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalalways", "ea",   P_BOOL|P_VI_DEF|P_RALL,
			    (char_u *)&p_ea, PV_NONE, did_ಗಣ_equalalways, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalprg",    "ep",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_ep, PV_EP, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"errorbells",  "eb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_eb, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"errorfile",   "ef",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)&p_ef, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_ERRORFILE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"errorformat", "efm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)&p_efm, PV_EFM, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_EFM, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"esckeys",	    "ek",   P_BOOL|P_VIM,
			    (char_u *)&p_ek, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"eventignore", "ei",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ei, PV_NONE, did_ಗಣ_eventignore, expand_ಗಣ_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eventignorewin", "eiw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)VAR_WIN, PV_EIW, did_ಗಣ_eventignore, expand_ಗಣ_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"expandtab",   "et",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_et, PV_ET, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"exrc",	    "ex",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_exrc, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"fileencoding","fenc", P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_RBUF
								   |P_NO_MKRC,
			    (char_u *)&p_fenc, PV_FENC, did_ಗಣ_encoding, expand_ಗಣ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fileencodings","fencs", P_STRING|P_VI_DEF|P_ONECOMMA,
			    (char_u *)&p_fencs, PV_NONE, ಖಾಲಿ, expand_ಗಣ_encoding,
			    {(char_u *)"ucs-bom", (char_u *)0L}
			    SCTX_INIT},
    {"fileformat",  "ff",   P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_NO_MKRC
								  |P_CURSWANT,
			    (char_u *)&p_ff, PV_FF, did_ಗಣ_fileformat, expand_ಗಣ_fileformat,
			    {(char_u *)DFLT_FF, (char_u *)0L} SCTX_INIT},
    {"fileformats", "ffs",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ffs, PV_NONE, did_ಗಣ_fileformats, expand_ಗಣ_fileformat,
			    {(char_u *)DFLT_FFS_VI, (char_u *)DFLT_FFS_VIM}
			    SCTX_INIT},
    {"fileignorecase", "fic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_fic, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CASE_INSENSITIVE_FILENAME
				    (char_u *)TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
				    (char_u *)FALSE,
#ಆದರೆ_ಅಂತ್ಯ
					(char_u *)0L} SCTX_INIT},
    {"filetype",    "ft",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
			    (char_u *)&p_ft, PV_FT,
			    did_ಗಣ_filetype_or_syntax, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fillchars",   "fcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_fcs, PV_FCS, did_ಗಣ_chars_option, expand_ಗಣ_chars_option,
			    {(char_u *)"vert:|,fold:-,eob:~,lastline:@",
								  (char_u *)0L}
			    SCTX_INIT},
    {"findfunc", "ffu",     P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			    (char_u *)&p_ffu, PV_FFU,
			    did_ಗಣ_findfunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"fixendofline",  "fixeol", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_fixeol, PV_FIXEOL,
			    did_ಗಣ_eof_eol_fixeol_bomb, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"fkmap",	    "fk",   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"flash",	    "fl",   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"foldclose",   "fcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)&p_fcl, PV_NONE, did_ಗಣ_foldclose, expand_ಗಣ_foldclose,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldcolumn",  "fdc",  P_ಅಂಕೆ|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDC, did_ಗಣ_foldcolumn, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldenable",  "fen",  P_BOOL|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FEN, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldexpr",    "fde",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#ಆದರೆ defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDE, did_ಗಣ_foldexpr, ಖಾಲಿ,
			    {(char_u *)"0", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldignore",  "fdi",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDI, did_ಗಣ_foldignore, ಖಾಲಿ,
			    {(char_u *)"#", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldlevel",   "fdl",  P_ಅಂಕೆ|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDL, did_ಗಣ_foldlevel, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldlevelstart","fdls", P_ಅಂಕೆ|P_VI_DEF|P_CURSWANT,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)&p_fdls, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)-1L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldmarker",  "fmr",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|
						    P_RWIN|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FMR, did_ಗಣ_foldmarker, ಖಾಲಿ,
			    {(char_u *)"{{{,}}}", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldmethod",  "fdm",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDM, did_ಗಣ_foldmethod, expand_ಗಣ_foldmethod,
			    {(char_u *)"manual", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldminlines","fml",  P_ಅಂಕೆ|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FML, did_ಗಣ_foldminlines, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldnestmax", "fdn",  P_ಅಂಕೆ|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDN, did_ಗಣ_foldnestmax, ಖಾಲಿ,
			    {(char_u *)20L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldopen",    "fdo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_CURSWANT,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			    (char_u *)&p_fdo, PV_NONE, did_ಗಣ_foldopen, expand_ಗಣ_foldopen,
		 {(char_u *)"block,hor,mark,percent,quickfix,search,tag,undo",
						 (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"foldtext",    "fdt",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#ಆದರೆ defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDT, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"foldtext()", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"formatexpr", "fex",   P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			    (char_u *)&p_fex, PV_FEX, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"formatlistpat","flp", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_flp, PV_FLP, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"^\\s*\\d\\+[\\]:.)}\\t ]\\s*",
						 (char_u *)0L} SCTX_INIT},
    {"formatoptions","fo",  P_STRING|P_ALLOCED|P_VIM|P_FLAGLIST,
			    (char_u *)&p_fo, PV_FO, did_ಗಣ_formatoptions, expand_ಗಣ_formatoptions,
			    {(char_u *)DFLT_FO_VI, (char_u *)DFLT_FO_VIM}
			    SCTX_INIT},
    {"formatprg",   "fp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_fp, PV_FP, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"fsync",       "fs",   P_BOOL|P_SECURE|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
			    (char_u *)&p_fs, PV_FS, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"gdefault",    "gd",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_gd, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"graphic",	    "gr",   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"grepformat",  "gfm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)&p_gefm, PV_GEFM, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_GREPFORMAT, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"grepprg",	    "gp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)&p_gp, PV_GP, ಖಾಲಿ, ಖಾಲಿ,
			    {
# ಆದರೆ defined(MSWIN)
			    // may be changed to "grep -n" in os_win32.c
			    (char_u *)"findstr /n",
# elif defined(UNIX)
			    // Add an extra file name so that grep will always
			    // insert a file name in the match line.
			    (char_u *)"grep -n $* /dev/null",
# elif defined(VMS)
			    (char_u *)"SEARCH/NUMBERS ",
# ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"grep -n ",
# endif
			    (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"guicursor",    "gcr", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
			    (char_u *)&p_guicursor, PV_NONE,
			    did_ಗಣ_guicursor, ಖಾಲಿ,
			    {
# ifdef FEAT_GUI
				(char_u *)"n-v-c:block-Cursor/lCursor,ve:ver35-Cursor,o:hor50-Cursor,i-ci:ver25-Cursor/lCursor,r-cr:hor20-Cursor/lCursor,sm:block-Cursor-blinkwait175-blinkoff150-blinkon175",
# ಇಲ್ಲದಿದ್ದರೆ	// Win32 console
				(char_u *)"n-v-c:block,o:hor50,i-ci:hor15,r-cr:hor30,sm:block",
# endif
				    (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"guifont",	    "gfn",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#ಆದರೆ !defined(FEAT_GUI_GTK)
				|P_COLON
#ಆದರೆ_ಅಂತ್ಯ
				,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    (char_u *)&p_guifont, PV_NONE, did_ಗಣ_guifont, expand_ಗಣ_guifont,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"guifontset",  "gfs",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA,
#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_XFONTSET)
			    (char_u *)&p_guifontset, PV_NONE,
			    did_ಗಣ_guifontset, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"guifontwide", "gfw",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#ಆದರೆ !defined(FEAT_GUI_GTK)
				|P_COLON
#ಆದರೆ_ಅಂತ್ಯ
				,
#ಆದರೆ defined(FEAT_GUI)
			    (char_u *)&p_guifontwide, PV_NONE,
			    did_ಗಣ_guifontwide, expand_ಗಣ_guifont,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},


    {"guiheadroom", "ghr",  P_ಅಂಕೆ|P_VI_DEF,
#ಆದರೆ (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
			    (char_u *)&p_ghr, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)50L, (char_u *)0L} SCTX_INIT},

    {"guiligatures", "gli", P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP,
#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_guiligatures, PV_NONE,
			    did_ಗಣ_guiligatures, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},

    {"guioptions",  "go",   P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#ಆದರೆ defined(FEAT_GUI)
			    (char_u *)&p_go, PV_NONE, did_ಗಣ_guioptions, expand_ಗಣ_guioptions,
# ಆದರೆ defined(UNIX)
			    {(char_u *)"aegimrLtT", (char_u *)0L}
# ಇಲ್ಲದಿದ್ದರೆ
			    {(char_u *)"egmrLtT", (char_u *)0L}
# endif
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"guipty",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
#ಆದರೆ defined(FEAT_GUI)
			    (char_u *)&p_guipty, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"guitablabel",  "gtl", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#ಆದರೆ defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtl, PV_NONE, did_ಗಣ_guitablabel, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"guitabtooltip",  "gtt", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#ಆದರೆ defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtt, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"hardtabs",    "ht",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"helpfile",    "hf",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_hf, PV_NONE, did_ಗಣ_helpfile, ಖಾಲಿ,
			    {(char_u *)DFLT_HELPFILE, (char_u *)0L}
			    SCTX_INIT},
    {"helpheight",  "hh",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_hh, PV_NONE,
			    did_ಗಣ_winheight_helpheight, ಖಾಲಿ,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"helplang",    "hlg",  P_STRING|P_VI_DEF|P_ONECOMMA,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
			    (char_u *)&p_hlg, PV_NONE, did_ಗಣ_helplang, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"hidden",	    "hid",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_hid, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"highlight",   "hl",   P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_hl, PV_NONE, did_ಗಣ_highlight, expand_ಗಣ_highlight,
			    {(char_u *)HIGHLIGHT_INIT, (char_u *)0L}
			    SCTX_INIT},
    {"history",	    "hi",   P_ಅಂಕೆ|P_VIM,
			    (char_u *)&p_hi, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)200L} SCTX_INIT},
    {"hkmap",	    "hk",   P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    (char_u *)&p_hkmap, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hkmapp",	    "hkp",  P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    (char_u *)&p_hkmapp, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hlsearch",    "hls",  P_BOOL|P_VI_DEF|P_VIM|P_RALL|P_HLONLY,
#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
			    (char_u *)&p_hls, PV_NONE, did_ಗಣ_hlsearch, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"icon",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_icon, PV_NONE, did_ಗಣ_title_icon, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"iconstring",  ಖಾಲಿ,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_iconstring, PV_NONE,
			    did_ಗಣ_iconstring, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ignorecase",  "ic",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ic, PV_NONE, did_ಗಣ_ignorecase, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imactivatefunc","imaf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#ಆದರೆ defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imaf, PV_NONE,
			    did_ಗಣ_imactivatefunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"imactivatekey","imak",P_STRING|P_VI_DEF,
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imak, PV_NONE,
			    did_ಗಣ_imactivatekey, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"imageprotocol", "ipc",P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
			    (char_u *)&p_ipc, PV_NONE, did_ಗಣ_imageprotocol, expand_ಗಣ_imageprotocol,
			    {(char_u *)"kitty:kitty,foot:sixel,ghostty:kitty,wezterm:kitty,xterm:sixel", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"imcmdline",   "imc",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_imcmdline, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imdisable",   "imd",  P_BOOL|P_VI_DEF,
#ಆದರೆ defined(HAVE_ಒಳಹರಿವು_ವಿಧಾನ)
			    (char_u *)&p_imdisable, PV_NONE,
			    did_ಗಣ_imdisable, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"iminsert",    "imi",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_iminsert, PV_IMI, did_ಗಣ_iminsert, ಖಾಲಿ,
			    {(char_u *)B_IMODE_NONE, (char_u *)0L}
			    SCTX_INIT},
    {"imsearch",    "ims",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_imsearch, PV_IMS, did_ಗಣ_imsearch, ಖಾಲಿ,
			    {(char_u *)B_IMODE_USE_ಸೇರಿಸು, (char_u *)0L}
			    SCTX_INIT},
    {"imstatusfunc","imsf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#ಆದರೆ defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imsf, PV_NONE, did_ಗಣ_imstatusfunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"imstyle",	    "imst", P_ಅಂಕೆ|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imst, PV_NONE, did_ಗಣ_imstyle, ಖಾಲಿ,
			    {(char_u *)IM_OVER_THE_SPOT, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"include",	    "inc",  P_STRING|P_ALLOCED|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
			    (char_u *)&p_inc, PV_INC, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"^\\s*#\\s*include", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"includeexpr", "inex", P_STRING|P_ALLOCED|P_VI_DEF|P_MLE,
#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
			    (char_u *)&p_inex, PV_INEX, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"incsearch",   "is",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_is, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"indentexpr", "inde",  P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#ಆದರೆ defined(FEAT_EVAL)
			    (char_u *)&p_inde, PV_INDE, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"indentkeys", "indk",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ಆದರೆ defined(FEAT_EVAL)
			    (char_u *)&p_indk, PV_INDK, ಖಾಲಿ, ಖಾಲಿ,
			    {INDENTKEYS_DEFAULT, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"infercase",   "inf",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_inf, PV_INF, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"insertmode",  "im",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_im, PV_NONE, did_ಗಣ_insertmode, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"isfname",	    "isf",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isf, PV_NONE, did_ಗಣ_isopt, ಖಾಲಿ,
			    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
				// Excluded are: & and ^ are special in cmd.exe
				// ( and ) are used in text separating fnames
			    (char_u *)"@,48-57,/,\\,.,-,_,+,,,#,$,%,{,},[,],:,@-@,!,~,=",
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(AMIGA)
			    (char_u *)"@,48-57,/,.,-,_,+,,,$,:",
# elif defined(VMS)
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,<,>,[,],:,;,~",
# ಇಲ್ಲದಿದ್ದರೆ // UNIX et al.
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,~,=",
# endif
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"isident",	    "isi",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isi, PV_NONE, did_ಗಣ_isopt, ಖಾಲಿ,
			    {
#ಆದರೆ defined(MSWIN)
			    (char_u *)"@,48-57,_,128-167,224-235",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"@,48-57,_,192-255",
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"iskeyword",   "isk",  P_STRING|P_ALLOCED|P_VIM|P_COMMA|P_NODUP,
			    (char_u *)&p_isk, PV_ISK, did_ಗಣ_iskeyword, ಖಾಲಿ,
			    {
				(char_u *)"@,48-57,_",
#ಆದರೆ defined(MSWIN)
				(char_u *)"@,48-57,_,128-167,224-235"
#ಇಲ್ಲದಿದ್ದರೆ
				ISK_LATIN1
#ಆದರೆ_ಅಂತ್ಯ
			    } SCTX_INIT},
    {"isprint",	    "isp",  P_STRING|P_VI_DEF|P_RALL|P_COMMA|P_NODUP,
			    (char_u *)&p_isp, PV_NONE, did_ಗಣ_isopt, ಖಾಲಿ,
			    {
#ಆದರೆ defined(MSWIN) || defined(VMS)
			    (char_u *)"@,~-255",
#ಇಲ್ಲದಿದ್ದರೆ
			    ISP_LATIN1,
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"joinspaces",  "js",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_js, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"jumpoptions", "jop",  P_STRING|P_VI_DEF|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_jop, PV_NONE, did_ಗಣ_jumpoptions, expand_ಗಣ_jumpoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"key",	    ಖಾಲಿ,   P_STRING|P_ALLOCED|P_VI_DEF|P_NO_MKRC|P_NO_CMD_EXPAND,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
			    (char_u *)&p_ಕೀಲಿ, PV_ಕೀಲಿ, did_ಗಣ_cryptkey, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"keymap",	    "kmp",  P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_RSTAT|P_NFNAME|P_PRI_MKRC,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
			    (char_u *)&p_keymap, PV_KMAP, did_ಗಣ_keymap, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"keymodel",    "km",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_km, PV_NONE, did_ಗಣ_keymodel, expand_ಗಣ_keymodel,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"keyprotocol", "kpc",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_kpc, PV_NONE, did_ಗಣ_keyprotocol, expand_ಗಣ_keyprotocol,
			    {(char_u *)"kitty:kitty,foot:kitty,ghostty:kitty,wezterm:kitty,xterm:mok2", (char_u *)0L}
			    SCTX_INIT},
    {"keywordprg",  "kp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_kp, PV_KP, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ಆದರೆ defined(MSWIN)
			    (char_u *)":help",
#ಅಥವಾಆದರೆ defined(VMS)
			    (char_u *)"help",
#ಅಥವಾಆದರೆ defined(USEMAN_S)
			    (char_u *)"man -s",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"man",
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"langmap",     "lmap", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
			    (char_u *)&p_langmap, PV_NONE, did_ಗಣ_langmap, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"langmenu",    "lm",   P_STRING|P_VI_DEF|P_NFNAME,
#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
			    (char_u *)&p_lm, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"langnoremap",  "lnr",   P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
			    (char_u *)&p_lnr, PV_NONE, did_ಗಣ_langnoremap, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"langremap",  "lrm",   P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
			    (char_u *)&p_lrm, PV_NONE, did_ಗಣ_langremap, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"laststatus",  "ls",   P_ಅಂಕೆ|P_VI_DEF|P_RALL,
			    (char_u *)&p_ls, PV_NONE, did_ಗಣ_laststatus, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"lazyredraw",  "lz",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lz, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lhistory", "lhi",	    P_ಅಂಕೆ|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)VAR_WIN, PV_LHI, did_ಗಣ_xhistory, ಖಾಲಿ,
			    {(char_u *)10L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"linebreak",   "lbr",  P_BOOL|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_LBR, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lines",	    ಖಾಲಿ,   P_ಅಂಕೆ|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&ಅಡ್ಡಸಾಲುಗಳು, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ಆದರೆ defined(MSWIN)
			    (char_u *)25L,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)24L,
#ಆದರೆ_ಅಂತ್ಯ
					    (char_u *)0L} SCTX_INIT},
    {"linespace",   "lsp",  P_ಅಂಕೆ|P_VI_DEF|P_RCLR,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    (char_u *)&p_linespace, PV_NONE,
			    did_ಗಣ_linespace, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
			    {(char_u *)1L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"lisp",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lisp, PV_LISP, did_ಗಣ_lisp, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lispoptions", "lop",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lop, PV_LOP, did_ಗಣ_lispoptions, expand_ಗಣ_lispoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"lispwords",   "lw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lispwords, PV_LW, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)LISPWORD_ಮೌಲ್ಯ, (char_u *)0L} SCTX_INIT},
    {"list",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_LIST, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"listchars",   "lcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_lcs, PV_LCS, did_ಗಣ_chars_option, expand_ಗಣ_chars_option,
			    {(char_u *)"eol:$", (char_u *)0L} SCTX_INIT},
    {"loadplugins", "lpl",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_lpl, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"luadll",      ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(DYNAMIC_LUA)
			    (char_u *)&p_luadll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DYNAMIC_LUA_DLL, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"macatsui",    ಖಾಲಿ,   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"magic",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_magic, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"makeef",	    "mef",  P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)&p_mef, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"makeencoding","menc", P_STRING|P_VI_DEF,
			    (char_u *)&p_menc, PV_MENC, did_ಗಣ_encoding, expand_ಗಣ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"makeprg",	    "mp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)&p_mp, PV_MP, ಖಾಲಿ, ಖಾಲಿ,
# ifdef VMS
			    {(char_u *)"MMS", (char_u *)0L}
# ಇಲ್ಲದಿದ್ದರೆ
			    {(char_u *)"make", (char_u *)0L}
# endif
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"matchpairs",  "mps",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_mps, PV_MPS, did_ಗಣ_matchpairs, ಖಾಲಿ,
			    {(char_u *)"(:),{:},[:]", (char_u *)0L}
			    SCTX_INIT},
    {"matchtime",   "mat",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_mat, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"maxcombine",  "mco",  P_ಅಂಕೆ|P_VI_DEF|P_CURSWANT,
			    (char_u *)&p_mco, PV_NONE, did_ಗಣ_maxcombine, ಖಾಲಿ,
			    {(char_u *)2, (char_u *)0L} SCTX_INIT},
    {"maxfuncdepth", "mfd", P_ಅಂಕೆ|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			    (char_u *)&p_mfd, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)100L, (char_u *)0L} SCTX_INIT},
    {"maxmapdepth", "mmd",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_mmd, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmem",	    "mm",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_mm, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_MAXMEM, (char_u *)0L}
			    SCTX_INIT},
    {"maxmempattern","mmp", P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_mmp, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmemtot",   "mmt",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_mmt, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_MAXMEMTOT, (char_u *)0L}
			    SCTX_INIT},
    {"maxsearchcount", "msc", P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_msc, PV_NONE, did_ಗಣ_maxsearchcount, ಖಾಲಿ,
			    {(char_u *)99L, (char_u *)0L} SCTX_INIT},
    {"menuitems",   "mis",  P_ಅಂಕೆ|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
			    (char_u *)&p_mis, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)25L, (char_u *)0L} SCTX_INIT},
    {"mesg",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"messagesopt","mopt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON|P_NODUP,
			    (char_u *)&p_mopt, PV_NONE, did_ಗಣ_messagesopt, expand_ಗಣ_messagesopt,
			    {(char_u *)"hit-enter,history:500", (char_u *)ಖಾಲಿ} SCTX_INIT},
    {"mkspellmem",  "msm",  P_STRING|P_VI_DEF|P_EXPAND|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
			    (char_u *)&p_msm, PV_NONE, did_ಗಣ_mkspellmem, ಖಾಲಿ,
			    {(char_u *)"460000,2000,500", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"modeline",    "ml",   P_BOOL|P_VIM,
			    (char_u *)&p_ml, PV_ML, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"modelineexpr", "mle",  P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mle, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"modelines",   "mls",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_mls, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"modelinestrict", "mlst", P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mlstr, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modifiable",  "ma",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_ma, PV_MA, did_ಗಣ_modifiable, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modified",    "mod",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_mod, PV_MOD, did_ಗಣ_modified, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"more",	    ಖಾಲಿ,   P_BOOL|P_VIM,
			    (char_u *)&p_ಇನ್ನಷ್ಟು, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"mouse",	    ಖಾಲಿ,   P_STRING|P_VI_DEF|P_FLAGLIST,
			    (char_u *)&p_mouse, PV_NONE, did_ಗಣ_mouse, expand_ಗಣ_mouse,
			    {
#ಆದರೆ defined(MSWIN)
				(char_u *)"a",
#ಇಲ್ಲದಿದ್ದರೆ
				(char_u *)"",
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"mousefocus",   "mousef", P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    (char_u *)&p_mousef, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mousehide",   "mh",   P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    (char_u *)&p_mh, PV_NONE, did_ಗಣ_mousehide, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"mousemodel",  "mousem", P_STRING|P_VI_DEF,
			    (char_u *)&p_mousem, PV_NONE, did_ಗಣ_mousemodel, expand_ಗಣ_mousemodel,
			    {
#ಆದರೆ defined(MSWIN)
				(char_u *)"popup",
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(MACOS_X)
				(char_u *)"popup_setpos",
# ಇಲ್ಲದಿದ್ದರೆ
				(char_u *)"extend",
# endif
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"mousemoveevent",   "mousemev",   P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			    (char_u *)&p_mousemev, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mouseshape",  "mouses",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
			    (char_u *)&p_mouseshape, PV_NONE,
			    did_ಗಣ_mouseshape, ಖಾಲಿ,
			    {(char_u *)"i-r:beam,s:updown,sd:udsizing,vs:leftright,vd:lrsizing,m:no,ml:up-arrow,v:rightup-arrow", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"mousetime",   "mouset",	P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_mouset, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)500L, (char_u *)0L} SCTX_INIT},
    {"mzquantum",  "mzq",   P_ಅಂಕೆ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
			    (char_u *)&p_mzq, PV_NONE, did_ಗಣ_mzquantum, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)100L, (char_u *)100L} SCTX_INIT},
    {"mzschemedll", ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemedll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DYNAMIC_MZSCH_DLL, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"mzschemegcdll", ಖಾಲಿ, P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemegcdll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DYNAMIC_MZGC_DLL, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"novice",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"nrformats",   "nf",   P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_nf, PV_NF, did_ಗಣ_nrformats, expand_ಗಣ_nrformats,
			    {(char_u *)"bin,octal,hex", (char_u *)0L}
			    SCTX_INIT},
    {"number",	    "nu",   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_NU,
			    did_ಗಣ_ಸಂಖ್ಯೆ_relativenumber, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"numberwidth", "nuw",  P_ಅಂಕೆ|P_RWIN|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_NUW, did_ಗಣ_numberwidth, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)8L, (char_u *)4L} SCTX_INIT},
    {"omnifunc",    "ofu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
			    (char_u *)&p_ofu, PV_OFU, did_ಗಣ_omnifunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"open",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"opendevice",  "odev", P_BOOL|P_VI_DEF,
#ಆದರೆ defined(MSWIN)
			    (char_u *)&p_odev, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"operatorfunc", "opfunc", P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
			    (char_u *)&p_opfunc, PV_NONE,
			    did_ಗಣ_operatorfunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"optimize",    "opt",  P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"osctimeoutlen", "ost", P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ost, PV_NONE, did_ಗಣ_osctimeoutlen, ಖಾಲಿ,
			    {(char_u *)1000, (char_u *)0L} SCTX_INIT},
    {"osfiletype",  "oft",  P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"packpath",    "pp",   P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_pp, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"paragraphs",  "para", P_STRING|P_VI_DEF,
			    (char_u *)&p_para, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"IPLPPPQPP TPHPLIPpLpItpplpipbp",
				(char_u *)0L} SCTX_INIT},
    {"paste",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF|P_PRI_MKRC,
			    (char_u *)&p_paste, PV_NONE, did_ಗಣ_paste, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"pastetoggle", "pt",   P_STRING|P_VI_DEF,
			    (char_u *)&p_pt, PV_NONE, did_ಗಣ_pastetoggle, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"patchexpr",   "pex",  P_STRING|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_pex, PV_NONE, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"patchmode",   "pm",   P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_pm, PV_NONE,
			    did_ಗಣ_backupext_or_patchmode, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"path",	    "pa",   P_STRING|P_EXPAND|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_ಮಾರ್ಗ, PV_ಮಾರ್ಗ, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ಆದರೆ defined(AMIGA) || defined(MSWIN)
			    (char_u *)".,,",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)".,/usr/include,,",
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"perldll",     ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(DYNAMIC_PERL)
			    (char_u *)&p_perldll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DYNAMIC_PERL_DLL, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"preserveindent", "pi", P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_pi, PV_PI, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"previewheight", "pvh", P_ಅಂಕೆ|P_VI_DEF,
#ಆದರೆ defined(FEAT_QUICKFIX)
			    (char_u *)&p_pvh, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)12L, (char_u *)0L} SCTX_INIT},
    {"previewpopup", "pvp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			    (char_u *)&p_pvp, PV_NONE, did_ಗಣ_previewpopup, expand_ಗಣ_previewpopup,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"previewwindow", "pvw", P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
#ಆದರೆ defined(FEAT_QUICKFIX)
			    (char_u *)VAR_WIN, PV_PVW, did_ಗಣ_previewwindow, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"printdevice", "pdev", P_STRING|P_VI_DEF|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
			    (char_u *)&p_pdev, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"printencoding", "penc", P_STRING|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_POSTSCRIPT
			    (char_u *)&p_penc, PV_NONE,
			    did_ಗಣ_printencoding, expand_ಗಣ_encoding,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"printexpr", "pexpr",  P_STRING|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
			    (char_u *)&p_pexpr, PV_NONE, did_ಗಣ_optexpr, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"printfont", "pfn",    P_STRING|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
			    (char_u *)&p_pfn, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {
# ifdef MSWIN
				(char_u *)"Courier_New:h10",
# elif defined(FEAT_PRINT_PANGO)
				(char_u *)"Courier 10",
# ಇಲ್ಲದಿದ್ದರೆ
				(char_u *)"courier",
# endif
				(char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"printheader", "pheader",  P_STRING|P_VI_DEF|P_GETTEXT|P_MLE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
			    (char_u *)&p_ಶೀರ್ಷಿಕೆ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    // untranslated to avoid problems when 'encoding'
			    // is changed
			    {(char_u *)"%<%f%h%m%=Page %N", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"printmbcharset", "pmbcs",  P_STRING|P_VI_DEF,
#ಆದರೆ defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmcs, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"printmbfont", "pmbfn",  P_STRING|P_VI_DEF,
#ಆದರೆ defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmfn, PV_NONE, parse_printmbfont, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"printoptions", "popt", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
			    (char_u *)&p_popt, PV_NONE, parse_printoptions, expand_ಗಣ_printoptions,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"prompt",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_prompt, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"pumborder",   "pb",   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON|P_SECURE,
			    (char_u *)&p_pb, PV_NONE, did_ಗಣ_pumborder, expand_ಗಣ_pumborder,
			    {(char_u *)"", (char_u *)ಖಾಲಿ} SCTX_INIT},
    {"pumheight",   "ph",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ph, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pummaxwidth", "pmw",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_pmw, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pumopt",	    ಖಾಲಿ,   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
			    (char_u *)&p_pumopt, PV_NONE,
			    did_ಗಣ_pumopt, expand_ಗಣ_pumopt,
			    {(char_u *)"", (char_u *)ಖಾಲಿ} SCTX_INIT},
    {"pumwidth",    "pw",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_pw, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)15L, (char_u *)15L} SCTX_INIT},
    {"pythondll",   ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(DYNAMIC_PYTHON)
			    (char_u *)&p_pydll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DYNAMIC_PYTHON_DLL, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"pythonhome",  ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(FEAT_PYTHON)
			    (char_u *)&p_pyhome, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"pythonthreedll",  ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(DYNAMIC_PYTHON3)
			    (char_u *)&p_py3dll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DYNAMIC_PYTHON3_DLL, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"pythonthreehome", ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(FEAT_PYTHON3)
			    (char_u *)&p_py3home, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"pyxversion", "pyx",   P_ಅಂಕೆ|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
			    (char_u *)&p_pyx, PV_NONE, did_ಗಣ_pyxversion, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)DEFAULT_PYTHON_VER, (char_u *)0L}
			    SCTX_INIT},
    {"quickfixtextfunc", "qftf", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_SECURE|P_FUNC,
#ಆದರೆ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
			    (char_u *)&p_qftf, PV_NONE,
			    did_ಗಣ_quickfixtextfunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)ಖಾಲಿ}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"quoteescape", "qe",   P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_qe, PV_QE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"\\", (char_u *)0L} SCTX_INIT},
    {"readonly",    "ro",   P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
			    (char_u *)&p_ro, PV_RO, did_ಗಣ_readonly, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redraw",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redrawtime",  "rdt",  P_ಅಂಕೆ|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
			    (char_u *)&p_rdt, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)2000L, (char_u *)0L} SCTX_INIT},
    {"regexpengine", "re",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_re, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"relativenumber", "rnu", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_RNU,
			    did_ಗಣ_ಸಂಖ್ಯೆ_relativenumber, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"remap",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_remap, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"renderoptions", "rop", P_STRING|P_ONECOMMA|P_RCLR|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RENDER_OPTIONS
			    (char_u *)&p_rop, PV_NONE, did_ಗಣ_renderoptions, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"report",	    ಖಾಲಿ,   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_report, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)2L, (char_u *)0L} SCTX_INIT},
    {"restorescreen", "rs", P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
			    (char_u *)&p_rs, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"revins",	    "ri",   P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    (char_u *)&p_ri, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleft",   "rl",   P_BOOL|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RL, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleftcmd", "rlc", P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RLC, did_ಗಣ_rightleftcmd, expand_ಗಣ_rightleftcmd,
			    {(char_u *)"search", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"rubydll",     ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(DYNAMIC_RUBY)
			    (char_u *)&p_rubydll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DYNAMIC_RUBY_DLL, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"ruler",	    "ru",   P_BOOL|P_VIM|P_RSTAT,
			    (char_u *)&p_ru, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"rulerformat", "ruf",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
			    (char_u *)&p_ruf, PV_NONE, did_ಗಣ_rulerformat, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"runtimepath", "rtp",  P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_rtp, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"scroll",	    "scr",  P_ಅಂಕೆ|P_NO_MKRC|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCROLL, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollbind",  "scb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCBIND, did_ಗಣ_scrollbind, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrollfocus", "scf",  P_BOOL|P_VI_DEF,
#ಆದರೆ defined(MSWIN) && defined(FEAT_GUI)
			    (char_u *)&p_scf, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrolljump",  "sj",   P_ಅಂಕೆ|P_VI_DEF|P_VIM,
			    (char_u *)&p_sj, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"scrolloff",   "so",   P_ಅಂಕೆ|P_VI_DEF|P_VIM|P_RALL,
			    (char_u *)&p_so, PV_SO, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrolloffpad", "sop", P_ಅಂಕೆ|P_VI_DEF|P_VIM|P_RALL,
				(char_u *)&p_sop, PV_SOP, ಖಾಲಿ, ಖಾಲಿ,
				{(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollopt",   "sbo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sbo, PV_NONE, did_ಗಣ_scrollopt, expand_ಗಣ_scrollopt,
			    {(char_u *)"ver,jump", (char_u *)0L}
			    SCTX_INIT},
    {"sections",    "sect", P_STRING|P_VI_DEF,
			    (char_u *)&p_sections, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"SHNHH HUnhsh", (char_u *)0L}
			    SCTX_INIT},
    {"secure",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_secure, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"selection",   "sel",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sel, PV_NONE, did_ಗಣ_selection, expand_ಗಣ_selection,
			    {(char_u *)"inclusive", (char_u *)0L}
			    SCTX_INIT},
    {"selectmode",  "slm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_slm, PV_NONE, did_ಗಣ_selectmode, expand_ಗಣ_selectmode,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"sessionoptions", "ssop", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
			    (char_u *)&p_ssop, PV_NONE,
			    did_ಗಣ_sessionoptions, expand_ಗಣ_sessionoptions,
	 {(char_u *)"blank,buffers,curdir,folds,help,options,tabpages,winsize,terminal",
							       (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"shell",	    "sh",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sh, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
			    (char_u *)"-",
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(MSWIN)
			    (char_u *)"",	// set in set_init_1()
# ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"sh",
# endif
#ಆದರೆ_ಅಂತ್ಯ // VMS
				(char_u *)0L} SCTX_INIT},
    {"shellcmdflag","shcf", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shcf, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ಆದರೆ defined(MSWIN)
			    (char_u *)"/c",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"-c",
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"shellpipe",   "sp",   P_STRING|P_VI_DEF|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			    (char_u *)&p_sp, PV_NONE, did_ಗಣ_shellpipe_redir, ಖಾಲಿ,
			    {
# ಆದರೆ defined(UNIX)
			    (char_u *)"| tee",
# ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)">",
# endif
				(char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"shellquote",  "shq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shq, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"shellredir",  "srr",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_srr, PV_NONE, did_ಗಣ_shellpipe_redir, ಖಾಲಿ,
			    {(char_u *)">", (char_u *)0L} SCTX_INIT},
    {"shellslash",  "ssl",   P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
			    (char_u *)&p_ssl, PV_NONE, did_ಗಣ_shellslash, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shelltemp",   "stmp", P_BOOL,
			    (char_u *)&p_stmp, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"shelltype",   "st",   P_ಅಂಕೆ|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
			    (char_u *)&p_st, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"shellxescape", "sxe", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxe, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ಆದರೆ defined(MSWIN)
			    (char_u *)"\"&|<>()@^",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"",
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"shellxquote", "sxq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxq, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ಆದರೆ defined(UNIX) && defined(USE_SYSTEM)
			    (char_u *)"\"",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"",
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"shiftround",  "sr",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sr, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shiftwidth",  "sw",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_sw, PV_SW,
			    did_ಗಣ_shiftwidth_tabstop, ಖಾಲಿ,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"shortmess",   "shm",  P_STRING|P_VIM|P_FLAGLIST,
			    (char_u *)&p_shm, PV_NONE, did_ಗಣ_shortmess, expand_ಗಣ_shortmess,
			    {(char_u *)"S", (char_u *)"filnxtToOS"}
			    SCTX_INIT},
    {"shortname",   "sn",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sn, PV_SN, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showbreak",   "sbr",  P_STRING|P_VI_DEF|P_RALL,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
			    (char_u *)&p_sbr, PV_SBR, did_ಗಣ_showbreak, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"showcmd",	    "sc",   P_BOOL|P_VIM,
			    (char_u *)&p_sc, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showcmdloc",  "sloc", P_STRING|P_RSTAT,
			    (char_u *)&p_sloc, PV_NONE, did_ಗಣ_showcmdloc, expand_ಗಣ_showcmdloc,
			    {(char_u *)"last", (char_u *)"last"} SCTX_INIT},
    {"showfulltag", "sft",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_sft, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmatch",   "sm",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sm, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmode",    "smd",  P_BOOL|P_VIM,
			    (char_u *)&p_smd, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showtabline", "stal", P_ಅಂಕೆ|P_VI_DEF|P_RALL,
			    (char_u *)&p_stal, PV_NONE, did_ಗಣ_showtabline, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
#ಆದರೆ defined(FEAT_TABPANEL)
    {"showtabpanel", "stpl", P_ಅಂಕೆ|P_RALL,
			    (char_u *)&p_stpl, PV_NONE, did_ಗಣ_showtabpanel, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
#ಆದರೆ_ಅಂತ್ಯ
    {"sidescroll",  "ss",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ss, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sidescrolloff", "siso", P_ಅಂಕೆ|P_VI_DEF|P_VIM|P_RBUF,
			    (char_u *)&p_siso, PV_SISO, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"signcolumn",   "scl",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
			    (char_u *)VAR_WIN, PV_SCL, did_ಗಣ_signcolumn, expand_ಗಣ_signcolumn,
			    {(char_u *)"auto", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"slowopen",    "slow", P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartcase",   "scs",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_scs, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartindent", "si",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_si, PV_SI, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smarttab",    "sta",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sta, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smoothscroll", "sms", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_SMS, did_ಗಣ_smoothscroll, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"softtabstop", "sts",  P_ಅಂಕೆ|P_VI_DEF|P_VIM,
			    (char_u *)&p_sts, PV_STS, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sourceany",   ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spell",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
			    (char_u *)VAR_WIN, PV_SPELL, did_ಗಣ_spell, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spellcapcheck", "spc", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_HLONLY,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
			    (char_u *)&p_spc, PV_SPC, did_ಗಣ_spellcapcheck, ಖಾಲಿ,
			    {(char_u *)"[.?!]\\_[\\])'\"	 ]\\+", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"spellfile",   "spf",  P_STRING|P_EXPAND|P_ALLOCED|P_VI_DEF|P_SECURE
								  |P_ONECOMMA,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
			    (char_u *)&p_spf, PV_SPF, did_ಗಣ_spellfile, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"spelllang",   "spl",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA
						     |P_RBUF|P_HLONLY|P_EXPAND,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
			    (char_u *)&p_spl, PV_SPL, did_ಗಣ_spelllang, ಖಾಲಿ,
			    {(char_u *)"en", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"spelloptions", "spo",  P_STRING|P_ALLOCED|P_VI_DEF
					   |P_ONECOMMA|P_NODUP|P_RBUF|P_HLONLY,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
			    (char_u *)&p_spo, PV_SPO, did_ಗಣ_spelloptions, expand_ಗಣ_spelloptions,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"spellsuggest", "sps", P_STRING|P_VI_DEF|P_EXPAND|P_SECURE|P_ONECOMMA,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
			    (char_u *)&p_sps, PV_NONE, did_ಗಣ_spellsuggest, expand_ಗಣ_spellsuggest,
			    {(char_u *)"best", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"splitbelow",  "sb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sb, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"splitkeep",   "spk",  P_STRING,
			    (char_u *)&p_spk, PV_NONE, did_ಗಣ_splitkeep, expand_ಗಣ_splitkeep,
			    {(char_u *)"cursor", (char_u *)"cursor"} SCTX_INIT},
    {"splitright",  "spr",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_spr, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"startofline", "sol",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sol, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"statusline"  ,"stl",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
			    (char_u *)&p_stl, PV_STL, did_ಗಣ_statusline, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"statuslineopt"  ,"stlo",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE
			    |P_ONECOMMA|P_COLON|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
			    (char_u *)&p_stlo, PV_STLO, did_ಗಣ_statuslineopt, expand_ಗಣ_statuslineopt,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"suffixes",    "su",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_su, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)".bak,~,.o,.h,.info,.swp,.obj",
				(char_u *)0L} SCTX_INIT},
    {"suffixesadd", "sua",  P_STRING|P_VI_DEF|P_ALLOCED|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sua, PV_SUA, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"swapfile",    "swf",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_swf, PV_SWF, did_ಗಣ_swapfile, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"swapsync",    "sws",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sws, PV_NONE, did_ಗಣ_swapsync, expand_ಗಣ_swapsync,
			    {(char_u *)"fsync", (char_u *)0L} SCTX_INIT},
    {"switchbuf",   "swb",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_swb, PV_NONE, did_ಗಣ_switchbuf, expand_ಗಣ_switchbuf,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"synmaxcol",   "smc",  P_ಅಂಕೆ|P_VI_DEF|P_RBUF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			    (char_u *)&p_smc, PV_SMC, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)3000L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"syntax",	    "syn",  P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			    (char_u *)&p_syn, PV_SYN,
			    did_ಗಣ_filetype_or_syntax, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"tabclose",    "tcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_tcl, PV_NONE, did_ಗಣ_tabclose, expand_ಗಣ_tabclose,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabline",	    "tal",  P_STRING|P_VI_DEF|P_RALL|P_MLE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
			    (char_u *)&p_tal, PV_NONE, did_ಗಣ_tabline, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpagemax",  "tpm",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_tpm, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)10L, (char_u *)0L} SCTX_INIT},
#ಆದರೆ defined(FEAT_TABPANEL)
    {"tabpanel",  "tpl",    P_STRING|P_VI_DEF|P_RALL|P_MLE,
			    (char_u *)&p_tpl, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpanelopt","tplo",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON
								    |P_NODUP,
			    (char_u *)&p_tplo, PV_NONE, did_ಗಣ_tabpanelopt,
			    expand_ಗಣ_tabpanelopt,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
#ಆದರೆ_ಅಂತ್ಯ
    {"tabstop",	    "ts",   P_ಅಂಕೆ|P_VI_DEF|P_RBUF,
			    (char_u *)&p_ts, PV_TS,
			    did_ಗಣ_shiftwidth_tabstop, ಖಾಲಿ,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"tagbsearch",  "tbs",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbs, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS	// binary searching doesn't appear to work on VMS
			    {(char_u *)0L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    {(char_u *)TRUE, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"tagcase",	    "tc",   P_STRING|P_VIM,
			    (char_u *)&p_tc, PV_TC, did_ಗಣ_tagcase, expand_ಗಣ_tagcase,
			    {(char_u *)"followic", (char_u *)"followic"} SCTX_INIT},
    {"tagfunc",    "tfu",   P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			    (char_u *)&p_tfu, PV_TFU, did_ಗಣ_tagfunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"taglength",   "tl",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_tl, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"tagrelative", "tr",   P_BOOL|P_VIM,
			    (char_u *)&p_tr, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"tags",	    "tag",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ಟ್ಯಾಗುಗಳು, PV_ಟ್ಯಾಗುಗಳು, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ಆದರೆ defined(FEAT_EMACS_ಟ್ಯಾಗುಗಳು) && !defined(CASE_INSENSITIVE_FILENAME)
			    (char_u *)"./tags,./TAGS,tags,TAGS",
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"./tags,tags",
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"tagsecure",   "tsc",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_tagsecure, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)TRUE} SCTX_INIT},
    {"tagstack",    "tgst", P_BOOL|P_VI_DEF,
			    (char_u *)&p_tgst, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"tcldll",      ಖಾಲಿ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(DYNAMIC_TCL)
			    (char_u *)&p_tcldll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DYNAMIC_TCL_DLL, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"term",	    ಖಾಲಿ,   P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_ಗಣ_ಪದ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"termbidi", "tbidi",   P_BOOL|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
			    (char_u *)&p_tbidi, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termencoding", "tenc", P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_tenc, PV_NONE, did_ಗಣ_encoding, expand_ಗಣ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"termguicolors", "tgc", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
			    (char_u *)&p_tgc, PV_NONE, did_ಗಣ_termguicolors, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)FALSE}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u*)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)FALSE}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"termresize", "trz", P_STRING|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
			    (char_u *)&p_trz, PV_NONE, did_ಗಣ_termresize, expand_ಗಣ_termresize,
			    {(char_u *)"", (char_u *)0}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"termsync", "tsy",	    P_BOOL|P_VI_DEF,
			    (char_u *)&p_tsy, PV_NONE, did_ಗಣ_termsync, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termwinkey", "twk",   P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWK, did_ಗಣ_termwinkey, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"termwinscroll", "twsl", P_ಅಂಕೆ|P_VI_DEF|P_VIM|P_RBUF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
			    (char_u *)&p_twsl, PV_TWSL, did_ಗಣ_termwinscroll, ಖಾಲಿ,
			    {(char_u *)10000L, (char_u *)10000L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"termwinsize", "tws",  P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWS, did_ಗಣ_termwinsize, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"termwintype", "twt",  P_STRING|P_ALLOCED|P_VI_DEF,
#ಆದರೆ defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_twt, PV_NONE, did_ಗಣ_termwintype, expand_ಗಣ_termwintype,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"terse",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_terse, PV_NONE, did_ಗಣ_terse, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"textauto",    "ta",   P_BOOL|P_VIM,
			    (char_u *)&p_ta, PV_NONE, did_ಗಣ_textauto, ಖಾಲಿ,
			    {(char_u *)DFLT_TEXTAUTO, (char_u *)TRUE}
			    SCTX_INIT},
    {"textmode",    "tx",   P_BOOL|P_VI_DEF|P_NO_MKRC,
			    (char_u *)&p_tx, PV_TX, did_ಗಣ_textmode, ಖಾಲಿ,
			    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
			    (char_u *)TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)FALSE,
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"textwidth",   "tw",   P_ಅಂಕೆ|P_VI_DEF|P_VIM|P_RBUF|P_HLONLY,
			    (char_u *)&p_tw, PV_TW, did_ಗಣ_textwidth, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"thesaurus",   "tsr",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_tsr, PV_TSR, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"thesaurusfunc", "tsrfu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
			    (char_u *)&p_tsrfu, PV_TSRFU,
			    did_ಗಣ_thesaurusfunc, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"tildeop",	    "top",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ಗಮ್ಯ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"timeout",	    "to",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_timeout, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"timeoutlen",  "tm",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_tm, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"title",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_title, PV_NONE, did_ಗಣ_title_icon, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"titlelen",    ಖಾಲಿ,   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_titlelen, PV_NONE, did_ಗಣ_titlelen, ಖಾಲಿ,
			    {(char_u *)85L, (char_u *)0L} SCTX_INIT},
    {"titleold",    ಖಾಲಿ,   P_STRING|P_VI_DEF|P_GETTEXT|P_SECURE|P_NO_MKRC,
			    (char_u *)&p_titleold, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)N_("Thanks for flying Vim"),
							       (char_u *)0L}
			    SCTX_INIT},
    {"titlestring", ಖಾಲಿ,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_titlestring, PV_NONE,
			    did_ಗಣ_titlestring, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"toolbar",     "tb",   P_STRING|P_ONECOMMA|P_VI_DEF|P_NODUP,
#ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_toolbar, PV_NONE, did_ಗಣ_toolbar, expand_ಗಣ_toolbar,
			    {(char_u *)"icons,tooltips", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"toolbariconsize",	"tbis", P_STRING|P_VI_DEF,
#ಆದರೆ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_tbis, PV_NONE,
			    did_ಗಣ_toolbariconsize, expand_ಗಣ_toolbariconsize,
			    {(char_u *)"small", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"ttimeout",    ಖಾಲಿ,   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ttimeout, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ttimeoutlen", "ttm",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ttm, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)-1L, (char_u *)0L} SCTX_INIT},
    {"ttybuiltin",  "tbi",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbi, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttyfast",	    "tf",   P_BOOL|P_NO_MKRC|P_VI_DEF,
			    (char_u *)&p_tf, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttymouse",    "ttym", P_STRING|P_NODEFAULT|P_NO_MKRC|P_VI_DEF,
#ಆದರೆ defined(UNIX) || defined(VMS)
			    (char_u *)&p_ttym, PV_NONE, did_ಗಣ_ttymouse, expand_ಗಣ_ttymouse,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ttyscroll",   "tsl",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ttyscroll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)999L, (char_u *)0L} SCTX_INIT},
    {"ttytype",	    "tty",  P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_ಗಣ_ಪದ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"undodir",     "udir", P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP|P_SECURE
								    |P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udir, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)".", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"undofile",    "udf",  P_BOOL|P_VI_DEF|P_VIM,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udf, PV_UDF, did_ಗಣ_undofile, ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"undolevels",  "ul",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ul, PV_UL, did_ಗಣ_undolevels, ಖಾಲಿ,
			    {
#ಆದರೆ defined(UNIX) || defined(MSWIN) || defined(VMS)
			    (char_u *)1000L,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)100L,
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"undoreload",  "ur",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ur, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    { (char_u *)10000L, (char_u *)0L} SCTX_INIT},
    {"updatecount", "uc",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_uc, PV_NONE, did_ಗಣ_updatecount, ಖಾಲಿ,
			    {(char_u *)200L, (char_u *)0L} SCTX_INIT},
    {"updatetime",  "ut",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ut, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)4000L, (char_u *)0L} SCTX_INIT},
    {"varsofttabstop", "vsts",  P_STRING|P_VI_DEF|P_VIM|P_COMMA,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
			    (char_u *)&p_vsts, PV_VSTS,
			    did_ಗಣ_varsofttabstop, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"vartabstop",  "vts",  P_STRING|P_VI_DEF|P_VIM|P_RBUF|P_COMMA,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
			    (char_u *)&p_vts, PV_VTS, did_ಗಣ_vartabstop, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"verbose",	    "vbs",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_verbose, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"verbosefile", "vfile", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_vfile, PV_NONE, did_ಗಣ_verbosefile, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"viewdir",     "vdir", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
			    (char_u *)&p_vdir, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)DFLT_VDIR, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"viewoptions", "vop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
			    (char_u *)&p_vop, PV_NONE, did_ಗಣ_viewoptions, expand_ಗಣ_sessionoptions,
			    {(char_u *)"folds,options,cursor,curdir",
								  (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"viminfo",	    "vi",   P_STRING|P_ONECOMMA|P_NODUP|P_SECURE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
			    (char_u *)&p_viminfo, PV_NONE, did_ಗಣ_viminfo, ಖಾಲಿ,
# ಆದರೆ defined(MSWIN)
			    {(char_u *)"", (char_u *)"'100,<50,s10,h,rA:,rB:"}
# elif defined(AMIGA)
			    {(char_u *)"",
				 (char_u *)"'100,<50,s10,h,rdf0:,rdf1:,rdf2:"}
# ಇಲ್ಲದಿದ್ದರೆ
			    {(char_u *)"", (char_u *)"'100,<50,s10,h"}
# endif
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"viminfofile", "vif",  P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP
							    |P_SECURE|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
			    (char_u *)&p_viminfofile, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"virtualedit", "ve",   P_STRING|P_ONECOMMA|P_NODUP|P_VI_DEF
							    |P_VIM|P_CURSWANT,
			    (char_u *)&p_ve, PV_VE, did_ಗಣ_virtualedit, expand_ಗಣ_virtualedit,
			    {(char_u *)"", (char_u *)""}
			    SCTX_INIT},
    {"visualbell",  "vb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_vb, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"w1200",	    ಖಾಲಿ,   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w300",	    ಖಾಲಿ,   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w9600",	    ಖಾಲಿ,   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"warn",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_warn, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"weirdinvert", "wiv",  P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_wiv, PV_NONE, did_ಗಣ_weirdinvert, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"whichwrap",   "ww",   P_STRING|P_VIM|P_ONECOMMA|P_FLAGLIST,
			    (char_u *)&p_ww, PV_NONE, did_ಗಣ_whichwrap, expand_ಗಣ_whichwrap,
			    {(char_u *)"", (char_u *)"b,s"} SCTX_INIT},
    {"wildchar",    "wc",   P_ಅಂಕೆ|P_VIM,
			    (char_u *)&p_wc, PV_NONE, did_ಗಣ_wildchar, ಖಾಲಿ,
			    {(char_u *)(ದೀರ್ಘ)Ctrl_E, (char_u *)(ದೀರ್ಘ)TAB}
			    SCTX_INIT},
    {"wildcharm",   "wcm",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_wcm, PV_NONE, did_ಗಣ_wildchar, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wildignore",  "wig",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wig, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"wildignorecase", "wic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wic, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"wildmenu",    "wmnu", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wmnu, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wildmode",    "wim",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_wim, PV_NONE, did_ಗಣ_wildmode, expand_ಗಣ_wildmode,
			    {(char_u *)"full", (char_u *)0L} SCTX_INIT},
    {"wildoptions", "wop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wop, PV_NONE, did_ಗಣ_wildoptions, expand_ಗಣ_wildoptions,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"winaltkeys",  "wak",  P_STRING|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAK
			    (char_u *)&p_wak, PV_NONE, did_ಗಣ_winaltkeys, expand_ಗಣ_winaltkeys,
			    {(char_u *)"menu", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"wincolor", "wcr",	    P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WCR, did_ಗಣ_wincolor, expand_ಗಣ_wincolor,
			    {(char_u *)"", (char_u *)ಖಾಲಿ}
			    SCTX_INIT},
    {"window",	    "wi",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_ಕಿಟಕಿ, PV_NONE, did_ಗಣ_ಕಿಟಕಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"winfixbuf", "wfb",    P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_WFB, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixheight", "wfh", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFH, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixwidth", "wfw",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFW, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winheight",   "wh",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_wh, PV_NONE,
			    did_ಗಣ_winheight_helpheight, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winhighlight", "whl", P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)VAR_WIN, PV_WHL, did_ಗಣ_winhighlight, expand_ಗಣ_winhighlight,
			    {(char_u *)"", (char_u *)ಖಾಲಿ} SCTX_INIT},
    {"winminheight", "wmh", P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_wmh, PV_NONE, did_ಗಣ_winminheight, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winminwidth", "wmw",  P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_wmw, PV_NONE, did_ಗಣ_winminwidth, ಖಾಲಿ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winptydll", ಖಾಲಿ,	    P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ಆದರೆ defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_winptydll, PV_NONE, ಖಾಲಿ, ಖಾಲಿ, {
# ifdef _WIN64
			    (char_u *)"winpty64.dll",
# ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)"winpty32.dll",
# endif
				(char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"winwidth",   "wiw",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_wiw, PV_NONE, did_ಗಣ_winwidth, ಖಾಲಿ,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"wlseat",	    "wse",  P_STRING|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND
			    (char_u *)&p_wse, PV_NONE, did_ಗಣ_wlseat, ಖಾಲಿ,
			    {(char_u *)"", (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"wlsteal",	    "wst",  P_BOOL|P_VI_DEF, // Deprecated
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
			    SCTX_INIT},
    {"wltimeoutlen", "wtm", P_ಅಂಕೆ|P_VI_DEF,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND
			    (char_u *)&p_wtm, PV_NONE, did_ಗಣ_wltimeoutlen, ಖಾಲಿ,
			    {(char_u *)500L, (char_u *)0L}
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)ಖಾಲಿ, (char_u *)0L}
#ಆದರೆ_ಅಂತ್ಯ
			    SCTX_INIT},
    {"wrap",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WRAP, did_ಗಣ_wrap, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wrapmargin",  "wm",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_wm, PV_WM, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wrapscan",    "ws",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ws, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"write",	    ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_write, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"writeany",    "wa",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_wa, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"writebackup", "wb",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_wb, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WRITEBACKUP
			    (char_u *)TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
			    (char_u *)FALSE,
#ಆದರೆ_ಅಂತ್ಯ
				(char_u *)0L} SCTX_INIT},
    {"writedelay",  "wd",   P_ಅಂಕೆ|P_VI_DEF,
			    (char_u *)&p_wd, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"xtermcodes",  ಖಾಲಿ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_xtermcodes, PV_NONE, ಖಾಲಿ, ಖಾಲಿ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},

// terminal output codes
#ವ್ಯಾಖ್ಯಾನಿಸು p_ಪದ(sss, vvv) \
			    {sss, ಖಾಲಿ, P_STRING|P_VI_DEF|P_RALL|P_SECURE, \
			    (char_u *)&vvv, PV_NONE, did_ಗಣ_ಪದ_option, ಖಾಲಿ, \
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},

    p_ಪದ("t_AB", T_CAB)
    p_ಪದ("t_AF", T_CAF)
    p_ಪದ("t_AU", T_CAU)
    p_ಪದ("t_AL", T_CAL)
    p_ಪದ("t_al", T_AL)
    p_ಪದ("t_bc", T_BC)
    p_ಪದ("t_BE", T_BE)
    p_ಪದ("t_BD", T_BD)
    p_ಪದ("t_cd", T_CD)
    p_ಪದ("t_ce", T_CE)
    p_ಪದ("t_Ce", T_UCE)
    p_ಪದ("t_CF", T_CFO)
    p_ಪದ("t_cl", T_CL)
    p_ಪದ("t_cm", T_CM)
    p_ಪದ("t_Co", T_CCO)
    p_ಪದ("t_CS", T_CCS)
    p_ಪದ("t_Cs", T_UCS)
    p_ಪದ("t_cs", T_CS)
    p_ಪದ("t_CV", T_CSV)
    p_ಪದ("t_da", T_DA)
    p_ಪದ("t_db", T_DB)
    p_ಪದ("t_DL", T_CDL)
    p_ಪದ("t_dl", T_DL)
    p_ಪದ("t_ds", T_DS)
    p_ಪದ("t_Ds", T_CDS)
    p_ಪದ("t_EC", T_CEC)
    p_ಪದ("t_EI", T_CEI)
    p_ಪದ("t_fs", T_FS)
    p_ಪದ("t_fd", T_FD)
    p_ಪದ("t_fe", T_FE)
    p_ಪದ("t_GP", T_CGP)
    p_ಪದ("t_IE", T_CIE)
    p_ಪದ("t_IS", T_CIS)
    p_ಪದ("t_ke", T_KE)
    p_ಪದ("t_ks", T_KS)
    p_ಪದ("t_le", T_LE)
    p_ಪದ("t_mb", T_MB)
    p_ಪದ("t_md", T_MD)
    p_ಪದ("t_me", T_ME)
    p_ಪದ("t_mr", T_MR)
    p_ಪದ("t_ms", T_MS)
    p_ಪದ("t_nd", T_ND)
    p_ಪದ("t_op", T_OP)
    p_ಪದ("t_RF", T_RFG)
    p_ಪದ("t_RB", T_RBG)
    p_ಪದ("t_RC", T_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ)
    p_ಪದ("t_RI", T_CRI)
    p_ಪದ("t_Ri", T_SRI)
    p_ಪದ("t_RK", T_CRK)
    p_ಪದ("t_RS", T_CRS)
    p_ಪದ("t_RT", T_CRT)
    p_ಪದ("t_RV", T_CRV)
    p_ಪದ("t_Sb", T_CSB)
    p_ಪದ("t_SC", T_CSC)
    p_ಪದ("t_se", T_SE)
    p_ಪದ("t_Sf", T_CSF)
    p_ಪದ("t_SH", T_CSH)
    p_ಪದ("t_SI", T_CSI)
    p_ಪದ("t_Si", T_SSI)
    p_ಪದ("t_so", T_SO)
    p_ಪದ("t_SR", T_CSR)
    p_ಪದ("t_sr", T_SR)
    p_ಪದ("t_ST", T_CST)
    p_ಪದ("t_Te", T_STE)
    p_ಪದ("t_te", T_TE)
    p_ಪದ("t_TE", T_CTE)
    p_ಪದ("t_ti", T_TI)
    p_ಪದ("t_TI", T_CTI)
    p_ಪದ("t_Ts", T_STS)
    p_ಪದ("t_ts", T_TS)
    p_ಪದ("t_u7", T_U7)
    p_ಪದ("t_ue", T_UE)
    p_ಪದ("t_us", T_US)
    p_ಪದ("t_Us", T_USS)
    p_ಪದ("t_ut", T_UT)
    p_ಪದ("t_vb", T_VB)
    p_ಪದ("t_ve", T_VE)
    p_ಪದ("t_vi", T_VI)
    p_ಪದ("t_VS", T_CVS)
    p_ಪದ("t_vs", T_VS)
    p_ಪದ("t_WP", T_CWP)
    p_ಪದ("t_WS", T_CWS)
    p_ಪದ("t_XM", T_CXM)
    p_ಪದ("t_xn", T_XN)
    p_ಪದ("t_xs", T_XS)
    p_ಪದ("t_ZH", T_CZH)
    p_ಪದ("t_ZR", T_CZR)
    p_ಪದ("t_8f", T_8F)
    p_ಪದ("t_8b", T_8B)
    p_ಪದ("t_8u", T_8U)
    p_ಪದ("t_xo", T_XON)
    p_ಪದ("t_BS", T_BSU)
    p_ಪದ("t_ES", T_ESU)

// terminal key codes are not in here

    // end marker
    {ಖಾಲಿ, ಖಾಲಿ, 0, ಖಾಲಿ, PV_NONE, ಖಾಲಿ, ಖಾಲಿ, {ಖಾಲಿ, ಖಾಲಿ} SCTX_INIT}
};

#ವ್ಯಾಖ್ಯಾನಿಸು OPTION_ಎಣಿಕೆ ARRAY_ಉದ್ದಳತೆ(options)

// The following is needed to make the gen_opt_test.vim script work.
// {"
