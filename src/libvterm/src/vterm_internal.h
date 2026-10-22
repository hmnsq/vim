#اگر_تعریف_نشده __VTERM_INTERNAL_H__
#تعریف_کن __VTERM_INTERNAL_H__

#ضمن "vterm.h"

#ضمن <stdarg.h>

#إذا defined(__GNUC__) && !defined(__MINGW32__)
# define داخلی __attribute__((visibility("internal")))
#وإلا
# define داخلی
#پایان_اگر

#إذا defined(__GNUC__) || defined(__MINGW32__)
# define استفادهنشده __attribute__((استفادهنشده_2))
#وإلا
# define استفادهنشده
#پایان_اگر

#اگر_تعریف_شده DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#وإلا
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#پایان_اگر

#تعریف_کن ESC_S "\x1b"

#تعریف_کن INTERMED_MAX 16

#تعریف_کن CSI_آرگومانها_MAX 16
#تعریف_کن CSI_LEADER_MAX 16

#تعریف_کن BUFIDX_PRIMARY   0
#تعریف_کن BUFIDX_ALTSCREEN 1

تعريفنوع بنية VTermEncoding VTermEncoding;

تعريفنوع بنية {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  حرف           داده[4*حجم(uint32_t)];
} VTermEncodingInstance;

بنية VTermPen
{
  VTermColor fg;
  VTermColor bg;
  بیعلامت صحيح bold:1;
  بیعلامت صحيح underline:2;
  بیعلامت صحيح italic:1;
  بیعلامت صحيح blink:1;
  بیعلامت صحيح معکوس:1;
  بیعلامت صحيح conceal:1;
  بیعلامت صحيح strike:1;
  بیعلامت صحيح font:4; /* To store 0-9 */
  بیعلامت صحيح small:1;
  بیعلامت صحيح baseline:2;
};

بنية VTermState
{
  VTerm *vt;

  ثابت VTermStateCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_premove;

  ثابت VTermStateFallbacks *fallbacks;
  فراغ *fbdata;

  صحيح ردیفها_2;
  صحيح تعداد_ستون;

  /* Current cursor position */
  VTermPos pos;

  صحيح at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  صحيح scrollregion_بالاترین;
  صحيح scrollregion_bottom; /* -1 means unbounded */
#تعریف_کن SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->ردیفها_2)
  صحيح scrollregion_چپ;
#تعریف_کن SCROLLREGION_چپ(state)  ((state)->نما.leftrightmargin ? (state)->scrollregion_چپ : 0)
  صحيح scrollregion_راست; /* -1 means unbounded */
#تعریف_کن SCROLLREGION_راست(state) ((state)->نما.leftrightmargin && (state)->scrollregion_راست > -1 ? (state)->scrollregion_راست : (state)->تعداد_ستون)

  /* Bitvector of tab stops */
  بیعلامت حرف *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_سطر_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_سطر_Info *lineinfo;
#تعریف_کن ROWWIDTH(state,ردیف) ((state)->lineinfo[(ردیف)].doublewidth ? ((state)->تعداد_ستون / 2) : (state)->تعداد_ستون)
#تعریف_کن THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.ردیف)

  /* Mouse state */
  صحيح mouse_ستون, mouse_ردیف;
  صحيح mouse_buttons;
  صحيح mouse_flags;

  تعداد { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_اندازه; // Number of ELEMENTS in the above
  صحيح combine_عرض; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  بنية {
    بیعلامت صحيح keypad:1;
    بیعلامت صحيح cursor:1;
    بیعلامت صحيح autowrap:1;
    بیعلامت صحيح درج:1;
    بیعلامت صحيح newline:1;
    بیعلامت صحيح cursor_visible:1;
    بیعلامت صحيح cursor_blink:1;
    بیعلامت صحيح cursor_shape:2;
    بیعلامت صحيح alt_screen:1;
    بیعلامت صحيح origin:1;
    بیعلامت صحيح screen:1;
    بیعلامت صحيح leftrightmargin:1;
    بیعلامت صحيح bracketpaste:1;
    بیعلامت صحيح report_focus:1;
    بیعلامت صحيح modify_other_کلیدها:1;
    بیعلامت صحيح kitty_keyboard:1;
  } نما;

  VTermEncodingInstance encoding[4], encoding_utf8;
  صحيح gl_مجموعه, gr_مجموعه, gsingle_مجموعه;

  بنية VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  صحيح bold_is_highbright;

  بیعلامت صحيح protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  بنية {
    VTermPos pos;
    بنية VTermPen pen;

    بنية {
      بیعلامت صحيح cursor_visible:1;
      بیعلامت صحيح cursor_blink:1;
      بیعلامت صحيح cursor_shape:2;
    } نما;
  } saved;

  /* Temporary state for DECRQSS parsing */
  اتحاد {
    حرف decrqss[4];
    بنية {
      uint16_t ماسک;
      تعداد {
        SELECTION_INITIAL,
        SELECTION_SELECTED,
        SELECTION_QUERY,
        SELECTION_مجموعه_INITIAL,
        SELECTION_مجموعه,
        SELECTION_INVALID,
      } state : 8;
      uint32_t recvpartial;
      uint32_t sendpartial;
    } selection;
  } tmp;

  بنية {
    ثابت VTermSelectionCallbacks *callbacks;
    فراغ *user;
    حرف *میانگیر;
    size_t buflen;
  } selection;
};

بنية VTerm
{
  ثابت VTermAllocatorFunctions *allocator;
  فراغ *allocdata;

  صحيح ردیفها_2;
  صحيح تعداد_ستون;

  بنية {
    بیعلامت صحيح utf8:1;
    بیعلامت صحيح ctrl8bit:1;
  } نما;

  بنية {
    تعداد VTermParserState {
      نرمال_2,
      CSI_LEADER,
      CSI_آرگومانها,
      CSI_INTERMED,
      DCS_COMMAND,
      /* below here are the "string states" */
      OSC_COMMAND,
      OSC,
      DCS,
      APC,
      PM,
      SOS,
    } state;

    بیعلامت صحيح درون_esc : 1;

    صحيح intermedlen;
    حرف intermed[INTERMED_MAX];

    اتحاد {
      بنية {
        صحيح leaderlen;
        حرف leader[CSI_LEADER_MAX];

        صحيح argi;
        دراز آرگومانها[CSI_آرگومانها_MAX];
      } csi;
      بنية {
        صحيح command;
      } osc;
      بنية {
        صحيح commandlen;
        حرف command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ثابت VTermParserCallbacks *callbacks;
    فراغ *cbdata;

    صحيح string_initial;

    صحيح emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  فراغ                *outdata;

  حرف  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  حرف  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  صحيح درون_backspace;
};

بنية VTermEncoding {
  فراغ (*init) (VTermEncoding *enc, فراغ *داده);
  فراغ (*decode)(VTermEncoding *enc, فراغ *داده,
                 uint32_t cp[], صحيح *cpi, صحيح cplen,
                 ثابت حرف بایتها[], size_t *pos, size_t len);
};

تعريفنوع تعداد {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

فراغ *vterm_allocator_malloc(VTerm *vt, size_t اندازه);
فراغ  vterm_allocator_free(VTerm *vt, فراغ *ptr);

فراغ vterm_افزودن_output_بایتها(VTerm *vt, ثابت حرف *بایتها, size_t len);
فراغ vterm_افزودن_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list آرگومانها);
فراغ vterm_افزودن_output_sprintf(VTerm *vt, ثابت حرف *format, ...);
فراغ vterm_افزودن_output_sprintf_ctrl(VTerm *vt, بیعلامت حرف ctrl, ثابت حرف *fmt, ...);
فراغ vterm_افزودن_output_sprintf_str(VTerm *vt, بیعلامت حرف ctrl, صحيح جمله, ثابت حرف *fmt, ...);

فراغ vterm_state_free(VTermState *state);

فراغ vterm_state_newpen(VTermState *state);
فراغ vterm_state_resetpen(VTermState *state);
فراغ vterm_state_setpen(VTermState *state, ثابت دراز آرگومانها[], صحيح argcount);
صحيح  vterm_state_getpen(VTermState *state, دراز آرگومانها[], صحيح argcount);
فراغ vterm_state_savepen(VTermState *state, صحيح save);

تعداد {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

فراغ vterm_state_افزودن_output_sprintf_CSI(VTermState *vts, ثابت حرف *format, ...);

فراغ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, حرف designation);

صحيح vterm_unicode_عرض(uint32_t codepoint);
صحيح vterm_unicode_is_combining(uint32_t codepoint);
صحيح vterm_unicode_is_ambiguous(uint32_t codepoint);
صحيح vterm_get_special_pty_type(فراغ);

#إذا (defined(XOPEN_منبع) && XOPEN_منبع >= 500) \
	|| defined(ISOC99_منبع) || defined(BSD_منبع)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#وإلا
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
صحيح VSNPRINTF(حرف *str, size_t str_m, ثابت حرف *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
صحيح SNPRINTF(حرف *str, size_t str_m, ثابت حرف *fmt, ...);
# endif
#پایان_اگر
#اگر_تعریف_نشده SNPRINTF
# define SNPRINTF snprintf
#پایان_اگر


#پایان_اگر
