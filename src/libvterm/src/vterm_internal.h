#که_نه_تعریف_شوی __VTERM_INTERNAL_H__
#تعریف_کړه __VTERM_INTERNAL_H__

#ضمن "vterm.h"

#ضمن <stdarg.h>

#إذا defined(__GNUC__) && !defined(__MINGW32__)
# define دننی __attribute__((visibility("internal")))
#وإلا
# define دننی
#د_که_پای

#إذا defined(__GNUC__) || defined(__MINGW32__)
# define ناکارول_شوی __attribute__((ناکارول_شوی_2))
#وإلا
# define ناکارول_شوی
#د_که_پای

#که_تعریف_شوی DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#وإلا
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#د_که_پای

#تعریف_کړه ESC_S "\x1b"

#تعریف_کړه INTERMED_MAX 16

#تعریف_کړه CSI_دلیلونه_MAX 16
#تعریف_کړه CSI_LEADER_MAX 16

#تعریف_کړه BUFIDX_PRIMARY   0
#تعریف_کړه BUFIDX_ALTSCREEN 1

تعريفنوع بنية VTermEncoding VTermEncoding;

تعريفنوع بنية {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  حرف           معلومات[4*حجم(uint32_t)];
} VTermEncodingInstance;

بنية VTermPen
{
  VTermColor fg;
  VTermColor bg;
  بینښه صحيح bold:1;
  بینښه صحيح underline:2;
  بینښه صحيح italic:1;
  بینښه صحيح blink:1;
  بینښه صحيح سرچپه:1;
  بینښه صحيح conceal:1;
  بینښه صحيح strike:1;
  بینښه صحيح font:4; /* To store 0-9 */
  بینښه صحيح small:1;
  بینښه صحيح baseline:2;
};

بنية VTermState
{
  VTerm *vt;

  ثابت VTermStateCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_premove;

  ثابت VTermStateFallbacks *fallbacks;
  فراغ *fbdata;

  صحيح لیکې_2;
  صحيح ستون_شمېر;

  /* Current cursor position */
  VTermPos pos;

  صحيح at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  صحيح scrollregion_سر;
  صحيح scrollregion_bottom; /* -1 means unbounded */
#تعریف_کړه SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->لیکې_2)
  صحيح scrollregion_کیڼ;
#تعریف_کړه SCROLLREGION_کیڼ(state)  ((state)->ډېرپېښ.leftrightmargin ? (state)->scrollregion_کیڼ : 0)
  صحيح scrollregion_ښی; /* -1 means unbounded */
#تعریف_کړه SCROLLREGION_ښی(state) ((state)->ډېرپېښ.leftrightmargin && (state)->scrollregion_ښی > -1 ? (state)->scrollregion_ښی : (state)->ستون_شمېر)

  /* Bitvector of tab stops */
  بینښه حرف *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_کرښه_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_کرښه_Info *lineinfo;
#تعریف_کړه ROWWIDTH(state,لیکه) ((state)->lineinfo[(لیکه)].doublewidth ? ((state)->ستون_شمېر / 2) : (state)->ستون_شمېر)
#تعریف_کړه THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.لیکه)

  /* Mouse state */
  صحيح mouse_ستون, mouse_لیکه;
  صحيح mouse_buttons;
  صحيح mouse_flags;

  تعداد { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_اندازه; // Number of ELEMENTS in the above
  صحيح combine_پلنوالی; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  بنية {
    بینښه صحيح keypad:1;
    بینښه صحيح cursor:1;
    بینښه صحيح autowrap:1;
    بینښه صحيح ورننباسه:1;
    بینښه صحيح newline:1;
    بینښه صحيح cursor_visible:1;
    بینښه صحيح cursor_blink:1;
    بینښه صحيح cursor_shape:2;
    بینښه صحيح alt_screen:1;
    بینښه صحيح origin:1;
    بینښه صحيح screen:1;
    بینښه صحيح leftrightmargin:1;
    بینښه صحيح bracketpaste:1;
    بینښه صحيح report_focus:1;
    بینښه صحيح modify_other_کلیګانې:1;
    بینښه صحيح kitty_keyboard:1;
  } ډېرپېښ;

  VTermEncodingInstance encoding[4], encoding_utf8;
  صحيح gl_سېټ, gr_سېټ, gsingle_سېټ;

  بنية VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  صحيح bold_is_highbright;

  بینښه صحيح protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  بنية {
    VTermPos pos;
    بنية VTermPen pen;

    بنية {
      بینښه صحيح cursor_visible:1;
      بینښه صحيح cursor_blink:1;
      بینښه صحيح cursor_shape:2;
    } ډېرپېښ;
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
        SELECTION_سېټ_INITIAL,
        SELECTION_سېټ,
        SELECTION_INVALID,
      } state : 8;
      uint32_t recvpartial;
      uint32_t sendpartial;
    } selection;
  } tmp;

  بنية {
    ثابت VTermSelectionCallbacks *callbacks;
    فراغ *user;
    حرف *بفر;
    size_t buflen;
  } selection;
};

بنية VTerm
{
  ثابت VTermAllocatorFunctions *allocator;
  فراغ *allocdata;

  صحيح لیکې_2;
  صحيح ستون_شمېر;

  بنية {
    بینښه صحيح utf8:1;
    بینښه صحيح ctrl8bit:1;
  } ډېرپېښ;

  بنية {
    تعداد VTermParserState {
      نورمال_2,
      CSI_LEADER,
      CSI_دلیلونه,
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

    بینښه صحيح دننه_esc : 1;

    صحيح intermedlen;
    حرف intermed[INTERMED_MAX];

    اتحاد {
      بنية {
        صحيح leaderlen;
        حرف leader[CSI_LEADER_MAX];

        صحيح argi;
        اوږد دلیلونه[CSI_دلیلونه_MAX];
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

  صحيح دننه_backspace;
};

بنية VTermEncoding {
  فراغ (*init) (VTermEncoding *enc, فراغ *معلومات);
  فراغ (*decode)(VTermEncoding *enc, فراغ *معلومات,
                 uint32_t cp[], صحيح *cpi, صحيح cplen,
                 ثابت حرف بایټونه[], size_t *pos, size_t len);
};

تعريفنوع تعداد {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

فراغ *vterm_allocator_malloc(VTerm *vt, size_t اندازه);
فراغ  vterm_allocator_free(VTerm *vt, فراغ *ptr);

فراغ vterm_ټېل_output_بایټونه(VTerm *vt, ثابت حرف *بایټونه, size_t len);
فراغ vterm_ټېل_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list دلیلونه);
فراغ vterm_ټېل_output_sprintf(VTerm *vt, ثابت حرف *format, ...);
فراغ vterm_ټېل_output_sprintf_ctrl(VTerm *vt, بینښه حرف ctrl, ثابت حرف *fmt, ...);
فراغ vterm_ټېل_output_sprintf_str(VTerm *vt, بینښه حرف ctrl, صحيح غړی, ثابت حرف *fmt, ...);

فراغ vterm_state_free(VTermState *state);

فراغ vterm_state_newpen(VTermState *state);
فراغ vterm_state_resetpen(VTermState *state);
فراغ vterm_state_setpen(VTermState *state, ثابت اوږد دلیلونه[], صحيح argcount);
صحيح  vterm_state_getpen(VTermState *state, اوږد دلیلونه[], صحيح argcount);
فراغ vterm_state_savepen(VTermState *state, صحيح save);

تعداد {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

فراغ vterm_state_ټېل_output_sprintf_CSI(VTermState *vts, ثابت حرف *format, ...);

فراغ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, حرف designation);

صحيح vterm_unicode_پلنوالی(uint32_t codepoint);
صحيح vterm_unicode_is_combining(uint32_t codepoint);
صحيح vterm_unicode_is_ambiguous(uint32_t codepoint);
صحيح vterm_get_special_pty_type(فراغ);

#إذا (defined(XOPEN_سرچینه) && XOPEN_سرچینه >= 500) \
	|| defined(ISOC99_سرچینه) || defined(BSD_سرچینه)
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
#د_که_پای
#که_نه_تعریف_شوی SNPRINTF
# define SNPRINTF snprintf
#د_که_پای


#د_که_پای
