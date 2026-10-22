#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#অন্তর্ভাৱ "vterm.h"

#অন্তর্ভাৱ <stdarg.h>

#যদি defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#অন্যথা
# define INTERNAL
#endif

#যদি defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#অন্যথা
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#অন্যথা
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

প্রকারনির্দেশ সংরচনা VTermEncoding VTermEncoding;

প্রকারনির্দেশ সংরচনা {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  ৱর্ণ           data[4*পরিমাণ(uint32_t)];
} VTermEncodingInstance;

সংরচনা VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned পূর্ণাঙ্ক bold:1;
  unsigned পূর্ণাঙ্ক underline:2;
  unsigned পূর্ণাঙ্ক italic:1;
  unsigned পূর্ণাঙ্ক blink:1;
  unsigned পূর্ণাঙ্ক reverse:1;
  unsigned পূর্ণাঙ্ক conceal:1;
  unsigned পূর্ণাঙ্ক strike:1;
  unsigned পূর্ণাঙ্ক font:4; /* To store 0-9 */
  unsigned পূর্ণাঙ্ক small:1;
  unsigned পূর্ণাঙ্ক baseline:2;
};

সংরচনা VTermState
{
  VTerm *vt;

  ধ্রুৱ VTermStateCallbacks *callbacks;
  শূন্য *cbdata;
  পূর্ণাঙ্ক callbacks_has_premove;

  ধ্রুৱ VTermStateFallbacks *fallbacks;
  শূন্য *fbdata;

  পূর্ণাঙ্ক rows;
  পূর্ণাঙ্ক cols;

  /* Current cursor position */
  VTermPos pos;

  পূর্ণাঙ্ক at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  পূর্ণাঙ্ক scrollregion_শীৰ্ষ;
  পূর্ণাঙ্ক scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  পূর্ণাঙ্ক scrollregion_বাম;
#define SCROLLREGION_বাম(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_বাম : 0)
  পূর্ণাঙ্ক scrollregion_ডান; /* -1 means unbounded */
#define SCROLLREGION_ডান(state) ((state)->mode.leftrightmargin && (state)->scrollregion_ডান > -1 ? (state)->scrollregion_ডান : (state)->cols)

  /* Bitvector of tab stops */
  unsigned ৱর্ণ *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_সাৰি_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_সাৰি_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  পূর্ণাঙ্ক mouse_col, mouse_row;
  পূর্ণাঙ্ক mouse_buttons;
  পূর্ণাঙ্ক mouse_flags;

  গণনা { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  পূর্ণাঙ্ক combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  সংরচনা {
    unsigned পূর্ণাঙ্ক keypad:1;
    unsigned পূর্ণাঙ্ক cursor:1;
    unsigned পূর্ণাঙ্ক autowrap:1;
    unsigned পূর্ণাঙ্ক ঢোকাও:1;
    unsigned পূর্ণাঙ্ক newline:1;
    unsigned পূর্ণাঙ্ক cursor_visible:1;
    unsigned পূর্ণাঙ্ক cursor_blink:1;
    unsigned পূর্ণাঙ্ক cursor_shape:2;
    unsigned পূর্ণাঙ্ক alt_screen:1;
    unsigned পূর্ণাঙ্ক origin:1;
    unsigned পূর্ণাঙ্ক screen:1;
    unsigned পূর্ণাঙ্ক leftrightmargin:1;
    unsigned পূর্ণাঙ্ক bracketpaste:1;
    unsigned পূর্ণাঙ্ক report_focus:1;
    unsigned পূর্ণাঙ্ক modify_other_চাবিগুলি:1;
    unsigned পূর্ণাঙ্ক kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  পূর্ণাঙ্ক gl_set, gr_set, gsingle_set;

  সংরচনা VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  পূর্ণাঙ্ক bold_is_highbright;

  unsigned পূর্ণাঙ্ক protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  সংরচনা {
    VTermPos pos;
    সংরচনা VTermPen pen;

    সংরচনা {
      unsigned পূর্ণাঙ্ক cursor_visible:1;
      unsigned পূর্ণাঙ্ক cursor_blink:1;
      unsigned পূর্ণাঙ্ক cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  সঙ্ঘ {
    ৱর্ণ decrqss[4];
    সংরচনা {
      uint16_t mask;
      গণনা {
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

  সংরচনা {
    ধ্রুৱ VTermSelectionCallbacks *callbacks;
    শূন্য *user;
    ৱর্ণ *buffer;
    size_t buflen;
  } selection;
};

সংরচনা VTerm
{
  ধ্রুৱ VTermAllocatorFunctions *allocator;
  শূন্য *allocdata;

  পূর্ণাঙ্ক rows;
  পূর্ণাঙ্ক cols;

  সংরচনা {
    unsigned পূর্ণাঙ্ক utf8:1;
    unsigned পূর্ণাঙ্ক ctrl8bit:1;
  } mode;

  সংরচনা {
    গণনা VTermParserState {
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

    unsigned পূর্ণাঙ্ক in_esc : 1;

    পূর্ণাঙ্ক intermedlen;
    ৱর্ণ intermed[INTERMED_MAX];

    সঙ্ঘ {
      সংরচনা {
        পূর্ণাঙ্ক leaderlen;
        ৱর্ণ leader[CSI_LEADER_MAX];

        পূর্ণাঙ্ক argi;
        long args[CSI_ARGS_MAX];
      } csi;
      সংরচনা {
        পূর্ণাঙ্ক command;
      } osc;
      সংরচনা {
        পূর্ণাঙ্ক commandlen;
        ৱর্ণ command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ধ্রুৱ VTermParserCallbacks *callbacks;
    শূন্য *cbdata;

    পূর্ণাঙ্ক string_initial;

    পূর্ণাঙ্ক emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  শূন্য                *outdata;

  ৱর্ণ  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  ৱর্ণ  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  পূর্ণাঙ্ক in_backspace;
};

সংরচনা VTermEncoding {
  শূন্য (*init) (VTermEncoding *enc, শূন্য *data);
  শূন্য (*decode)(VTermEncoding *enc, শূন্য *data,
                 uint32_t cp[], পূর্ণাঙ্ক *cpi, পূর্ণাঙ্ক cplen,
                 ধ্রুৱ ৱর্ণ bytes[], size_t *pos, size_t len);
};

প্রকারনির্দেশ গণনা {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

শূন্য *vterm_allocator_malloc(VTerm *vt, size_t size);
শূন্য  vterm_allocator_free(VTerm *vt, শূন্য *ptr);

শূন্য vterm_ঠেলো_output_bytes(VTerm *vt, ধ্রুৱ ৱর্ণ *bytes, size_t len);
শূন্য vterm_ঠেলো_output_vsprintf(VTerm *vt, ধ্রুৱ ৱর্ণ *format, va_list args);
শূন্য vterm_ঠেলো_output_sprintf(VTerm *vt, ধ্রুৱ ৱর্ণ *format, ...);
শূন্য vterm_ঠেলো_output_sprintf_ctrl(VTerm *vt, unsigned ৱর্ণ ctrl, ধ্রুৱ ৱর্ণ *fmt, ...);
শূন্য vterm_ঠেলো_output_sprintf_str(VTerm *vt, unsigned ৱর্ণ ctrl, পূর্ণাঙ্ক term, ধ্রুৱ ৱর্ণ *fmt, ...);

শূন্য vterm_state_free(VTermState *state);

শূন্য vterm_state_newpen(VTermState *state);
শূন্য vterm_state_resetpen(VTermState *state);
শূন্য vterm_state_setpen(VTermState *state, ধ্রুৱ long args[], পূর্ণাঙ্ক argcount);
পূর্ণাঙ্ক  vterm_state_getpen(VTermState *state, long args[], পূর্ণাঙ্ক argcount);
শূন্য vterm_state_savepen(VTermState *state, পূর্ণাঙ্ক save);

গণনা {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

শূন্য vterm_state_ঠেলো_output_sprintf_CSI(VTermState *vts, ধ্রুৱ ৱর্ণ *format, ...);

শূন্য vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ৱর্ণ designation);

পূর্ণাঙ্ক vterm_unicode_width(uint32_t codepoint);
পূর্ণাঙ্ক vterm_unicode_is_combining(uint32_t codepoint);
পূর্ণাঙ্ক vterm_unicode_is_ambiguous(uint32_t codepoint);
পূর্ণাঙ্ক vterm_get_special_pty_type(শূন্য);

#যদি (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#অন্যথা
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
পূর্ণাঙ্ক VSNPRINTF(ৱর্ণ *str, size_t str_m, ধ্রুৱ ৱর্ণ *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
পূর্ণাঙ্ক SNPRINTF(ৱর্ণ *str, size_t str_m, ধ্রুৱ ৱর্ণ *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
