#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#ଅନ୍ତର୍ଭାଵ "vterm.h"

#ଅନ୍ତର୍ଭାଵ <stdarg.h>

#ଯଦି defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#ଅନ୍ୟଥା
# define INTERNAL
#endif

#ଯଦି defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#ଅନ୍ୟଥା
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#ଅନ୍ୟଥା
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VTermEncoding VTermEncoding;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  ଅକ୍ଷର           data[4*ଆକାର(uint32_t)];
} VTermEncodingInstance;

ସଂରଚନା VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold:1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା underline:2;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା italic:1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା blink:1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reverse:1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା conceal:1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା strike:1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା font:4; /* To store 0-9 */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା small:1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା baseline:2;
};

ସଂରଚନା VTermState
{
  VTerm *vt;

  ଅଚଳ VTermStateCallbacks *callbacks;
  ଶୂନ୍ୟ *cbdata;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା callbacks_has_premove;

  ଅଚଳ VTermStateFallbacks *fallbacks;
  ଶୂନ୍ୟ *fbdata;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols;

  /* Current cursor position */
  VTermPos pos;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା scrollregion_ଶୀର୍ଷ;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା scrollregion_ବାମ;
#define SCROLLREGION_ବାମ(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_ବାମ : 0)
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା scrollregion_ଡାହାଣ; /* -1 means unbounded */
#define SCROLLREGION_ଡାହାଣ(state) ((state)->mode.leftrightmargin && (state)->scrollregion_ଡାହାଣ > -1 ? (state)->scrollregion_ଡାହାଣ : (state)->cols)

  /* Bitvector of tab stops */
  unsigned ଅକ୍ଷର *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_ଧାଡ଼ି_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_ଧାଡ଼ି_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mouse_col, mouse_row;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mouse_buttons;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mouse_flags;

  ଗଣନା { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  ସଂରଚନା {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା keypad:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursor:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା autowrap:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଭର୍ତ୍ତି:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା newline:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursor_visible:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursor_blink:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursor_shape:2;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା alt_screen:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା origin:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftrightmargin:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା bracketpaste:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା report_focus:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା modify_other_ଚାବିଗୁଡ଼ିକ:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା gl_set, gr_set, gsingle_set;

  ସଂରଚନା VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold_is_highbright;

  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  ସଂରଚନା {
    VTermPos pos;
    ସଂରଚନା VTermPen pen;

    ସଂରଚନା {
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursor_visible:1;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursor_blink:1;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  ସଂଘ {
    ଅକ୍ଷର decrqss[4];
    ସଂରଚନା {
      uint16_t mask;
      ଗଣନା {
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

  ସଂରଚନା {
    ଅଚଳ VTermSelectionCallbacks *callbacks;
    ଶୂନ୍ୟ *user;
    ଅକ୍ଷର *buffer;
    size_t buflen;
  } selection;
};

ସଂରଚନା VTerm
{
  ଅଚଳ VTermAllocatorFunctions *allocator;
  ଶୂନ୍ୟ *allocdata;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols;

  ସଂରଚନା {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8:1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl8bit:1;
  } mode;

  ସଂରଚନା {
    ଗଣନା VTermParserState {
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

    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_esc : 1;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା intermedlen;
    ଅକ୍ଷର intermed[INTERMED_MAX];

    ସଂଘ {
      ସଂରଚନା {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା leaderlen;
        ଅକ୍ଷର leader[CSI_LEADER_MAX];

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା argi;
        long args[CSI_ARGS_MAX];
      } csi;
      ସଂରଚନା {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା command;
      } osc;
      ସଂରଚନା {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା commandlen;
        ଅକ୍ଷର command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ଅଚଳ VTermParserCallbacks *callbacks;
    ଶୂନ୍ୟ *cbdata;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_initial;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  ଶୂନ୍ୟ                *outdata;

  ଅକ୍ଷର  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  ଅକ୍ଷର  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_backspace;
};

ସଂରଚନା VTermEncoding {
  ଶୂନ୍ୟ (*init) (VTermEncoding *enc, ଶୂନ୍ୟ *data);
  ଶୂନ୍ୟ (*decode)(VTermEncoding *enc, ଶୂନ୍ୟ *data,
                 uint32_t cp[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cpi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cplen,
                 ଅଚଳ ଅକ୍ଷର bytes[], size_t *pos, size_t len);
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

ଶୂନ୍ୟ *vterm_allocator_malloc(VTerm *vt, size_t size);
ଶୂନ୍ୟ  vterm_allocator_free(VTerm *vt, ଶୂନ୍ୟ *ptr);

ଶୂନ୍ୟ vterm_ଠେଲ_output_bytes(VTerm *vt, ଅଚଳ ଅକ୍ଷର *bytes, size_t len);
ଶୂନ୍ୟ vterm_ଠେଲ_output_vsprintf(VTerm *vt, ଅଚଳ ଅକ୍ଷର *format, va_list args);
ଶୂନ୍ୟ vterm_ଠେଲ_output_sprintf(VTerm *vt, ଅଚଳ ଅକ୍ଷର *format, ...);
ଶୂନ୍ୟ vterm_ଠେଲ_output_sprintf_ctrl(VTerm *vt, unsigned ଅକ୍ଷର ctrl, ଅଚଳ ଅକ୍ଷର *fmt, ...);
ଶୂନ୍ୟ vterm_ଠେଲ_output_sprintf_str(VTerm *vt, unsigned ଅକ୍ଷର ctrl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା term, ଅଚଳ ଅକ୍ଷର *fmt, ...);

ଶୂନ୍ୟ vterm_state_free(VTermState *state);

ଶୂନ୍ୟ vterm_state_newpen(VTermState *state);
ଶୂନ୍ୟ vterm_state_resetpen(VTermState *state);
ଶୂନ୍ୟ vterm_state_setpen(VTermState *state, ଅଚଳ long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା  vterm_state_getpen(VTermState *state, long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount);
ଶୂନ୍ୟ vterm_state_savepen(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା save);

ଗଣନା {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

ଶୂନ୍ୟ vterm_state_ଠେଲ_output_sprintf_CSI(VTermState *vts, ଅଚଳ ଅକ୍ଷର *format, ...);

ଶୂନ୍ୟ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ଅକ୍ଷର designation);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_unicode_width(uint32_t codepoint);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_unicode_is_combining(uint32_t codepoint);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_unicode_is_ambiguous(uint32_t codepoint);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_get_special_pty_type(ଶୂନ୍ୟ);

#ଯଦି (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#ଅନ୍ୟଥା
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା VSNPRINTF(ଅକ୍ଷର *str, size_t str_m, ଅଚଳ ଅକ୍ଷର *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା SNPRINTF(ଅକ୍ଷର *str, size_t str_m, ଅଚଳ ଅକ୍ଷର *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
