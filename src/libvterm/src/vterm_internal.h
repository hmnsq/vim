#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#अन्तर्भाव "vterm.h"

#अन्तर्भाव <stdarg.h>

#जइ defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#अण्णहा
# define INTERNAL
#endif

#जइ defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#अण्णहा
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#अण्णहा
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

प्रकारनिर्देश संरचना VTermEncoding VTermEncoding;

प्रकारनिर्देश संरचना {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  वर्ण           data[4*परिमाण(uint32_t)];
} VTermEncodingInstance;

संरचना VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned पूर्णाङ्क bold:1;
  unsigned पूर्णाङ्क underline:2;
  unsigned पूर्णाङ्क italic:1;
  unsigned पूर्णाङ्क blink:1;
  unsigned पूर्णाङ्क reverse:1;
  unsigned पूर्णाङ्क conceal:1;
  unsigned पूर्णाङ्क strike:1;
  unsigned पूर्णाङ्क font:4; /* To store 0-9 */
  unsigned पूर्णाङ्क small:1;
  unsigned पूर्णाङ्क baseline:2;
};

संरचना VTermState
{
  VTerm *vt;

  ध्रुव VTermStateCallbacks *callbacks;
  शून्य *cbdata;
  पूर्णाङ्क callbacks_has_premove;

  ध्रुव VTermStateFallbacks *fallbacks;
  शून्य *fbdata;

  पूर्णाङ्क rows;
  पूर्णाङ्क cols;

  /* Current cursor position */
  VTermPos pos;

  पूर्णाङ्क at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  पूर्णाङ्क scrollregion_शिखरम्;
  पूर्णाङ्क scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  पूर्णाङ्क scrollregion_वामः;
#define SCROLLREGION_वामः(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_वामः : 0)
  पूर्णाङ्क scrollregion_दक्षिणः; /* -1 means unbounded */
#define SCROLLREGION_दक्षिणः(state) ((state)->mode.leftrightmargin && (state)->scrollregion_दक्षिणः > -1 ? (state)->scrollregion_दक्षिणः : (state)->cols)

  /* Bitvector of tab stops */
  unsigned वर्ण *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_पङ्क्तिः_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_पङ्क्तिः_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  पूर्णाङ्क mouse_col, mouse_row;
  पूर्णाङ्क mouse_buttons;
  पूर्णाङ्क mouse_flags;

  गणना { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  पूर्णाङ्क combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  संरचना {
    unsigned पूर्णाङ्क keypad:1;
    unsigned पूर्णाङ्क cursor:1;
    unsigned पूर्णाङ्क autowrap:1;
    unsigned पूर्णाङ्क निवेशय:1;
    unsigned पूर्णाङ्क newline:1;
    unsigned पूर्णाङ्क cursor_visible:1;
    unsigned पूर्णाङ्क cursor_blink:1;
    unsigned पूर्णाङ्क cursor_shape:2;
    unsigned पूर्णाङ्क alt_screen:1;
    unsigned पूर्णाङ्क origin:1;
    unsigned पूर्णाङ्क screen:1;
    unsigned पूर्णाङ्क leftrightmargin:1;
    unsigned पूर्णाङ्क bracketpaste:1;
    unsigned पूर्णाङ्क report_focus:1;
    unsigned पूर्णाङ्क modify_other_कुञ्चिकाः:1;
    unsigned पूर्णाङ्क kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  पूर्णाङ्क gl_set, gr_set, gsingle_set;

  संरचना VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  पूर्णाङ्क bold_is_highbright;

  unsigned पूर्णाङ्क protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  संरचना {
    VTermPos pos;
    संरचना VTermPen pen;

    संरचना {
      unsigned पूर्णाङ्क cursor_visible:1;
      unsigned पूर्णाङ्क cursor_blink:1;
      unsigned पूर्णाङ्क cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  सङ्घ {
    वर्ण decrqss[4];
    संरचना {
      uint16_t mask;
      गणना {
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

  संरचना {
    ध्रुव VTermSelectionCallbacks *callbacks;
    शून्य *user;
    वर्ण *buffer;
    size_t buflen;
  } selection;
};

संरचना VTerm
{
  ध्रुव VTermAllocatorFunctions *allocator;
  शून्य *allocdata;

  पूर्णाङ्क rows;
  पूर्णाङ्क cols;

  संरचना {
    unsigned पूर्णाङ्क utf8:1;
    unsigned पूर्णाङ्क ctrl8bit:1;
  } mode;

  संरचना {
    गणना VTermParserState {
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

    unsigned पूर्णाङ्क in_esc : 1;

    पूर्णाङ्क intermedlen;
    वर्ण intermed[INTERMED_MAX];

    सङ्घ {
      संरचना {
        पूर्णाङ्क leaderlen;
        वर्ण leader[CSI_LEADER_MAX];

        पूर्णाङ्क argi;
        long args[CSI_ARGS_MAX];
      } csi;
      संरचना {
        पूर्णाङ्क command;
      } osc;
      संरचना {
        पूर्णाङ्क commandlen;
        वर्ण command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ध्रुव VTermParserCallbacks *callbacks;
    शून्य *cbdata;

    पूर्णाङ्क string_initial;

    पूर्णाङ्क emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  शून्य                *outdata;

  वर्ण  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  वर्ण  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  पूर्णाङ्क in_backspace;
};

संरचना VTermEncoding {
  शून्य (*init) (VTermEncoding *enc, शून्य *data);
  शून्य (*decode)(VTermEncoding *enc, शून्य *data,
                 uint32_t cp[], पूर्णाङ्क *cpi, पूर्णाङ्क cplen,
                 ध्रुव वर्ण bytes[], size_t *pos, size_t len);
};

प्रकारनिर्देश गणना {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

शून्य *vterm_allocator_malloc(VTerm *vt, size_t size);
शून्य  vterm_allocator_free(VTerm *vt, शून्य *ptr);

शून्य vterm_प्रक्षिप_output_bytes(VTerm *vt, ध्रुव वर्ण *bytes, size_t len);
शून्य vterm_प्रक्षिप_output_vsprintf(VTerm *vt, ध्रुव वर्ण *format, va_list args);
शून्य vterm_प्रक्षिप_output_sprintf(VTerm *vt, ध्रुव वर्ण *format, ...);
शून्य vterm_प्रक्षिप_output_sprintf_ctrl(VTerm *vt, unsigned वर्ण ctrl, ध्रुव वर्ण *fmt, ...);
शून्य vterm_प्रक्षिप_output_sprintf_str(VTerm *vt, unsigned वर्ण ctrl, पूर्णाङ्क term, ध्रुव वर्ण *fmt, ...);

शून्य vterm_state_free(VTermState *state);

शून्य vterm_state_newpen(VTermState *state);
शून्य vterm_state_resetpen(VTermState *state);
शून्य vterm_state_setpen(VTermState *state, ध्रुव long args[], पूर्णाङ्क argcount);
पूर्णाङ्क  vterm_state_getpen(VTermState *state, long args[], पूर्णाङ्क argcount);
शून्य vterm_state_savepen(VTermState *state, पूर्णाङ्क save);

गणना {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

शून्य vterm_state_प्रक्षिप_output_sprintf_CSI(VTermState *vts, ध्रुव वर्ण *format, ...);

शून्य vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, वर्ण designation);

पूर्णाङ्क vterm_unicode_width(uint32_t codepoint);
पूर्णाङ्क vterm_unicode_is_combining(uint32_t codepoint);
पूर्णाङ्क vterm_unicode_is_ambiguous(uint32_t codepoint);
पूर्णाङ्क vterm_get_special_pty_type(शून्य);

#जइ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#अण्णहा
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
पूर्णाङ्क VSNPRINTF(वर्ण *str, size_t str_m, ध्रुव वर्ण *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
पूर्णाङ्क SNPRINTF(वर्ण *str, size_t str_m, ध्रुव वर्ण *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
