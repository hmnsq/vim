#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#شامل "vterm.h"

#شامل <stdarg.h>

#اگر defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#وگرنه
# define INTERNAL
#endif

#اگر defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#وگرنه
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#وگرنه
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#endif

#define ESC_S "\x1b"

#define INTERMED_MAX 16

#define CSI_ARGS_MAX 16
#define CSI_LEADER_MAX 16

#define BUFIDX_PRIMARY   0
#define BUFIDX_ALTSCREEN 1

تعریفنوع ساختار VTermEncoding VTermEncoding;

تعریفنوع ساختار {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  نویسه           data[4*اندازه(uint32_t)];
} VTermEncodingInstance;

ساختار VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned صحیح bold:1;
  unsigned صحیح underline:2;
  unsigned صحیح italic:1;
  unsigned صحیح blink:1;
  unsigned صحیح reverse:1;
  unsigned صحیح conceal:1;
  unsigned صحیح strike:1;
  unsigned صحیح font:4; /* To store 0-9 */
  unsigned صحیح small:1;
  unsigned صحیح baseline:2;
};

ساختار VTermState
{
  VTerm *vt;

  ثابت VTermStateCallbacks *callbacks;
  تهی *cbdata;
  صحیح callbacks_has_premove;

  ثابت VTermStateFallbacks *fallbacks;
  تهی *fbdata;

  صحیح rows;
  صحیح cols;

  /* Current cursor position */
  VTermPos pos;

  صحیح at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  صحیح scrollregion_بالاترین;
  صحیح scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  صحیح scrollregion_چپ;
#define SCROLLREGION_چپ(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_چپ : 0)
  صحیح scrollregion_راست; /* -1 means unbounded */
#define SCROLLREGION_راست(state) ((state)->mode.leftrightmargin && (state)->scrollregion_راست > -1 ? (state)->scrollregion_راست : (state)->cols)

  /* Bitvector of tab stops */
  unsigned نویسه *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_سطر_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_سطر_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  صحیح mouse_col, mouse_row;
  صحیح mouse_buttons;
  صحیح mouse_flags;

  شمارش { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  صحیح combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  ساختار {
    unsigned صحیح keypad:1;
    unsigned صحیح cursor:1;
    unsigned صحیح autowrap:1;
    unsigned صحیح درج:1;
    unsigned صحیح newline:1;
    unsigned صحیح cursor_visible:1;
    unsigned صحیح cursor_blink:1;
    unsigned صحیح cursor_shape:2;
    unsigned صحیح alt_screen:1;
    unsigned صحیح origin:1;
    unsigned صحیح screen:1;
    unsigned صحیح leftrightmargin:1;
    unsigned صحیح bracketpaste:1;
    unsigned صحیح report_focus:1;
    unsigned صحیح modify_other_کلیدها:1;
    unsigned صحیح kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  صحیح gl_set, gr_set, gsingle_set;

  ساختار VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  صحیح bold_is_highbright;

  unsigned صحیح protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  ساختار {
    VTermPos pos;
    ساختار VTermPen pen;

    ساختار {
      unsigned صحیح cursor_visible:1;
      unsigned صحیح cursor_blink:1;
      unsigned صحیح cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  اجتماع {
    نویسه decrqss[4];
    ساختار {
      uint16_t mask;
      شمارش {
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

  ساختار {
    ثابت VTermSelectionCallbacks *callbacks;
    تهی *user;
    نویسه *buffer;
    size_t buflen;
  } selection;
};

ساختار VTerm
{
  ثابت VTermAllocatorFunctions *allocator;
  تهی *allocdata;

  صحیح rows;
  صحیح cols;

  ساختار {
    unsigned صحیح utf8:1;
    unsigned صحیح ctrl8bit:1;
  } mode;

  ساختار {
    شمارش VTermParserState {
      NORMAL,
      CSI_LEADER,
      CSI_ARGS,
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

    unsigned صحیح in_esc : 1;

    صحیح intermedlen;
    نویسه intermed[INTERMED_MAX];

    اجتماع {
      ساختار {
        صحیح leaderlen;
        نویسه leader[CSI_LEADER_MAX];

        صحیح argi;
        long args[CSI_ARGS_MAX];
      } csi;
      ساختار {
        صحیح command;
      } osc;
      ساختار {
        صحیح commandlen;
        نویسه command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ثابت VTermParserCallbacks *callbacks;
    تهی *cbdata;

    صحیح string_initial;

    صحیح emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  تهی                *outdata;

  نویسه  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  نویسه  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  صحیح in_backspace;
};

ساختار VTermEncoding {
  تهی (*init) (VTermEncoding *enc, تهی *data);
  تهی (*decode)(VTermEncoding *enc, تهی *data,
                 uint32_t cp[], صحیح *cpi, صحیح cplen,
                 ثابت نویسه bytes[], size_t *pos, size_t len);
};

تعریفنوع شمارش {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

تهی *vterm_allocator_malloc(VTerm *vt, size_t size);
تهی  vterm_allocator_free(VTerm *vt, تهی *ptr);

تهی vterm_افزودن_output_bytes(VTerm *vt, ثابت نویسه *bytes, size_t len);
تهی vterm_افزودن_output_vsprintf(VTerm *vt, ثابت نویسه *format, va_list args);
تهی vterm_افزودن_output_sprintf(VTerm *vt, ثابت نویسه *format, ...);
تهی vterm_افزودن_output_sprintf_ctrl(VTerm *vt, unsigned نویسه ctrl, ثابت نویسه *fmt, ...);
تهی vterm_افزودن_output_sprintf_str(VTerm *vt, unsigned نویسه ctrl, صحیح term, ثابت نویسه *fmt, ...);

تهی vterm_state_free(VTermState *state);

تهی vterm_state_newpen(VTermState *state);
تهی vterm_state_resetpen(VTermState *state);
تهی vterm_state_setpen(VTermState *state, ثابت long args[], صحیح argcount);
صحیح  vterm_state_getpen(VTermState *state, long args[], صحیح argcount);
تهی vterm_state_savepen(VTermState *state, صحیح save);

شمارش {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

تهی vterm_state_افزودن_output_sprintf_CSI(VTermState *vts, ثابت نویسه *format, ...);

تهی vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, نویسه designation);

صحیح vterm_unicode_width(uint32_t codepoint);
صحیح vterm_unicode_is_combining(uint32_t codepoint);
صحیح vterm_unicode_is_ambiguous(uint32_t codepoint);
صحیح vterm_get_special_pty_type(تهی);

#اگر (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#وگرنه
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
صحیح VSNPRINTF(نویسه *str, size_t str_m, ثابت نویسه *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
صحیح SNPRINTF(نویسه *str, size_t str_m, ثابت نویسه *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
