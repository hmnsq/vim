#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#ضمن "vterm.h"

#ضمن <stdarg.h>

#إذا defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#وإلا
# define INTERNAL
#endif

#إذا defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#وإلا
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#وإلا
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#endif

#define ESC_S "\x1b"

#define INTERMED_MAX 16

#define CSI_وسائط_MAX 16
#define CSI_LEADER_MAX 16

#define BUFIDX_PRIMARY   0
#define BUFIDX_ALTSCREEN 1

تعريفنوع بنية VTermEncoding VTermEncoding;

تعريفنوع بنية {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  حرف           data[4*حجم(uint32_t)];
} VTermEncodingInstance;

بنية VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned صحيح bold:1;
  unsigned صحيح underline:2;
  unsigned صحيح italic:1;
  unsigned صحيح blink:1;
  unsigned صحيح reverse:1;
  unsigned صحيح conceal:1;
  unsigned صحيح strike:1;
  unsigned صحيح font:4; /* To store 0-9 */
  unsigned صحيح small:1;
  unsigned صحيح baseline:2;
};

بنية VTermState
{
  VTerm *vt;

  ثابت VTermStateCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_premove;

  ثابت VTermStateFallbacks *fallbacks;
  فراغ *fbdata;

  صحيح rows;
  صحيح cols;

  /* Current cursor position */
  VTermPos pos;

  صحيح at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  صحيح scrollregion_قمة;
  صحيح scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  صحيح scrollregion_يسار;
#define SCROLLREGION_يسار(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_يسار : 0)
  صحيح scrollregion_يمين; /* -1 means unbounded */
#define SCROLLREGION_يمين(state) ((state)->mode.leftrightmargin && (state)->scrollregion_يمين > -1 ? (state)->scrollregion_يمين : (state)->cols)

  /* Bitvector of tab stops */
  unsigned حرف *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_سطر_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_سطر_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  صحيح mouse_col, mouse_row;
  صحيح mouse_buttons;
  صحيح mouse_flags;

  تعداد { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  صحيح combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  بنية {
    unsigned صحيح keypad:1;
    unsigned صحيح cursor:1;
    unsigned صحيح autowrap:1;
    unsigned صحيح أدرج:1;
    unsigned صحيح newline:1;
    unsigned صحيح cursor_visible:1;
    unsigned صحيح cursor_blink:1;
    unsigned صحيح cursor_shape:2;
    unsigned صحيح alt_screen:1;
    unsigned صحيح origin:1;
    unsigned صحيح screen:1;
    unsigned صحيح leftrightmargin:1;
    unsigned صحيح bracketpaste:1;
    unsigned صحيح report_focus:1;
    unsigned صحيح modify_other_مفاتيح:1;
    unsigned صحيح kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  صحيح gl_set, gr_set, gsingle_set;

  بنية VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  صحيح bold_is_highbright;

  unsigned صحيح protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  بنية {
    VTermPos pos;
    بنية VTermPen pen;

    بنية {
      unsigned صحيح cursor_visible:1;
      unsigned صحيح cursor_blink:1;
      unsigned صحيح cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  اتحاد {
    حرف decrqss[4];
    بنية {
      uint16_t mask;
      تعداد {
        SELECTION_INITIAL,
        SELECTION_SELECTED,
        SELECTION_QUERY,
        SELECTION_SET_INITIAL,
        SELECTION_SET,
        SELECTION_INVALID,
      } state : 8;
      uint32_t recvpartial;
      uint32_t sendpartial;
    } selection;
  } tmp;

  بنية {
    ثابت VTermSelectionCallbacks *callbacks;
    فراغ *user;
    حرف *buffer;
    size_t buflen;
  } selection;
};

بنية VTerm
{
  ثابت VTermAllocatorFunctions *allocator;
  فراغ *allocdata;

  صحيح rows;
  صحيح cols;

  بنية {
    unsigned صحيح utf8:1;
    unsigned صحيح ctrl8bit:1;
  } mode;

  بنية {
    تعداد VTermParserState {
      NORMAL,
      CSI_LEADER,
      CSI_وسائط,
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

    unsigned صحيح in_esc : 1;

    صحيح intermedlen;
    حرف intermed[INTERMED_MAX];

    اتحاد {
      بنية {
        صحيح leaderlen;
        حرف leader[CSI_LEADER_MAX];

        صحيح argi;
        long وسائط[CSI_وسائط_MAX];
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

  صحيح in_backspace;
};

بنية VTermEncoding {
  فراغ (*init) (VTermEncoding *enc, فراغ *data);
  فراغ (*decode)(VTermEncoding *enc, فراغ *data,
                 uint32_t cp[], صحيح *cpi, صحيح cplen,
                 ثابت حرف bytes[], size_t *pos, size_t len);
};

تعريفنوع تعداد {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

فراغ *vterm_allocator_malloc(VTerm *vt, size_t size);
فراغ  vterm_allocator_free(VTerm *vt, فراغ *ptr);

فراغ vterm_ادفع_output_bytes(VTerm *vt, ثابت حرف *bytes, size_t len);
فراغ vterm_ادفع_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list وسائط);
فراغ vterm_ادفع_output_sprintf(VTerm *vt, ثابت حرف *format, ...);
فراغ vterm_ادفع_output_sprintf_ctrl(VTerm *vt, unsigned حرف ctrl, ثابت حرف *fmt, ...);
فراغ vterm_ادفع_output_sprintf_str(VTerm *vt, unsigned حرف ctrl, صحيح term, ثابت حرف *fmt, ...);

فراغ vterm_state_free(VTermState *state);

فراغ vterm_state_newpen(VTermState *state);
فراغ vterm_state_resetpen(VTermState *state);
فراغ vterm_state_setpen(VTermState *state, ثابت long وسائط[], صحيح argcount);
صحيح  vterm_state_getpen(VTermState *state, long وسائط[], صحيح argcount);
فراغ vterm_state_savepen(VTermState *state, صحيح save);

تعداد {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

فراغ vterm_state_ادفع_output_sprintf_CSI(VTermState *vts, ثابت حرف *format, ...);

فراغ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, حرف designation);

صحيح vterm_unicode_width(uint32_t codepoint);
صحيح vterm_unicode_is_combining(uint32_t codepoint);
صحيح vterm_unicode_is_ambiguous(uint32_t codepoint);
صحيح vterm_get_special_pty_type(فراغ);

#إذا (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
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
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
