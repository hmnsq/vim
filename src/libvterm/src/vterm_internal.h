#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#અન્તર્ભાવ "vterm.h"

#અન્તર્ભાવ <stdarg.h>

#જો defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#નહીંતર
# define INTERNAL
#endif

#જો defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#નહીંતર
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#નહીંતર
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

પ્રકારવ્યાખ્યા સંરચના VTermEncoding VTermEncoding;

પ્રકારવ્યાખ્યા સંરચના {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  અક્ષર           data[4*કદ(uint32_t)];
} VTermEncodingInstance;

સંરચના VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned પૂર્ણાંક bold:1;
  unsigned પૂર્ણાંક underline:2;
  unsigned પૂર્ણાંક italic:1;
  unsigned પૂર્ણાંક blink:1;
  unsigned પૂર્ણાંક reverse:1;
  unsigned પૂર્ણાંક conceal:1;
  unsigned પૂર્ણાંક strike:1;
  unsigned પૂર્ણાંક font:4; /* To store 0-9 */
  unsigned પૂર્ણાંક small:1;
  unsigned પૂર્ણાંક baseline:2;
};

સંરચના VTermState
{
  VTerm *vt;

  અચલ VTermStateCallbacks *callbacks;
  શૂન્ય *cbdata;
  પૂર્ણાંક callbacks_has_premove;

  અચલ VTermStateFallbacks *fallbacks;
  શૂન્ય *fbdata;

  પૂર્ણાંક rows;
  પૂર્ણાંક cols;

  /* Current cursor position */
  VTermPos pos;

  પૂર્ણાંક at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  પૂર્ણાંક scrollregion_ટોચ;
  પૂર્ણાંક scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  પૂર્ણાંક scrollregion_ડાબું;
#define SCROLLREGION_ડાબું(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_ડાબું : 0)
  પૂર્ણાંક scrollregion_જમણું; /* -1 means unbounded */
#define SCROLLREGION_જમણું(state) ((state)->mode.leftrightmargin && (state)->scrollregion_જમણું > -1 ? (state)->scrollregion_જમણું : (state)->cols)

  /* Bitvector of tab stops */
  unsigned અક્ષર *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_લીટી_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_લીટી_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  પૂર્ણાંક mouse_col, mouse_row;
  પૂર્ણાંક mouse_buttons;
  પૂર્ણાંક mouse_flags;

  ગણના { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  પૂર્ણાંક combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  સંરચના {
    unsigned પૂર્ણાંક keypad:1;
    unsigned પૂર્ણાંક cursor:1;
    unsigned પૂર્ણાંક autowrap:1;
    unsigned પૂર્ણાંક ઉમેરો:1;
    unsigned પૂર્ણાંક newline:1;
    unsigned પૂર્ણાંક cursor_visible:1;
    unsigned પૂર્ણાંક cursor_blink:1;
    unsigned પૂર્ણાંક cursor_shape:2;
    unsigned પૂર્ણાંક alt_screen:1;
    unsigned પૂર્ણાંક origin:1;
    unsigned પૂર્ણાંક screen:1;
    unsigned પૂર્ણાંક leftrightmargin:1;
    unsigned પૂર્ણાંક bracketpaste:1;
    unsigned પૂર્ણાંક report_focus:1;
    unsigned પૂર્ણાંક modify_other_ચાવીઓ:1;
    unsigned પૂર્ણાંક kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  પૂર્ણાંક gl_set, gr_set, gsingle_set;

  સંરચના VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  પૂર્ણાંક bold_is_highbright;

  unsigned પૂર્ણાંક protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  સંરચના {
    VTermPos pos;
    સંરચના VTermPen pen;

    સંરચના {
      unsigned પૂર્ણાંક cursor_visible:1;
      unsigned પૂર્ણાંક cursor_blink:1;
      unsigned પૂર્ણાંક cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  સંઘ {
    અક્ષર decrqss[4];
    સંરચના {
      uint16_t mask;
      ગણના {
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

  સંરચના {
    અચલ VTermSelectionCallbacks *callbacks;
    શૂન્ય *user;
    અક્ષર *buffer;
    size_t buflen;
  } selection;
};

સંરચના VTerm
{
  અચલ VTermAllocatorFunctions *allocator;
  શૂન્ય *allocdata;

  પૂર્ણાંક rows;
  પૂર્ણાંક cols;

  સંરચના {
    unsigned પૂર્ણાંક utf8:1;
    unsigned પૂર્ણાંક ctrl8bit:1;
  } mode;

  સંરચના {
    ગણના VTermParserState {
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

    unsigned પૂર્ણાંક in_esc : 1;

    પૂર્ણાંક intermedlen;
    અક્ષર intermed[INTERMED_MAX];

    સંઘ {
      સંરચના {
        પૂર્ણાંક leaderlen;
        અક્ષર leader[CSI_LEADER_MAX];

        પૂર્ણાંક argi;
        long args[CSI_ARGS_MAX];
      } csi;
      સંરચના {
        પૂર્ણાંક command;
      } osc;
      સંરચના {
        પૂર્ણાંક commandlen;
        અક્ષર command[CSI_LEADER_MAX];
      } dcs;
    } v;

    અચલ VTermParserCallbacks *callbacks;
    શૂન્ય *cbdata;

    પૂર્ણાંક string_initial;

    પૂર્ણાંક emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  શૂન્ય                *outdata;

  અક્ષર  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  અક્ષર  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  પૂર્ણાંક in_backspace;
};

સંરચના VTermEncoding {
  શૂન્ય (*init) (VTermEncoding *enc, શૂન્ય *data);
  શૂન્ય (*decode)(VTermEncoding *enc, શૂન્ય *data,
                 uint32_t cp[], પૂર્ણાંક *cpi, પૂર્ણાંક cplen,
                 અચલ અક્ષર bytes[], size_t *pos, size_t len);
};

પ્રકારવ્યાખ્યા ગણના {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

શૂન્ય *vterm_allocator_malloc(VTerm *vt, size_t size);
શૂન્ય  vterm_allocator_free(VTerm *vt, શૂન્ય *ptr);

શૂન્ય vterm_ધકેલો_output_bytes(VTerm *vt, અચલ અક્ષર *bytes, size_t len);
શૂન્ય vterm_ધકેલો_output_vsprintf(VTerm *vt, અચલ અક્ષર *format, va_list args);
શૂન્ય vterm_ધકેલો_output_sprintf(VTerm *vt, અચલ અક્ષર *format, ...);
શૂન્ય vterm_ધકેલો_output_sprintf_ctrl(VTerm *vt, unsigned અક્ષર ctrl, અચલ અક્ષર *fmt, ...);
શૂન્ય vterm_ધકેલો_output_sprintf_str(VTerm *vt, unsigned અક્ષર ctrl, પૂર્ણાંક term, અચલ અક્ષર *fmt, ...);

શૂન્ય vterm_state_free(VTermState *state);

શૂન્ય vterm_state_newpen(VTermState *state);
શૂન્ય vterm_state_resetpen(VTermState *state);
શૂન્ય vterm_state_setpen(VTermState *state, અચલ long args[], પૂર્ણાંક argcount);
પૂર્ણાંક  vterm_state_getpen(VTermState *state, long args[], પૂર્ણાંક argcount);
શૂન્ય vterm_state_savepen(VTermState *state, પૂર્ણાંક save);

ગણના {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

શૂન્ય vterm_state_ધકેલો_output_sprintf_CSI(VTermState *vts, અચલ અક્ષર *format, ...);

શૂન્ય vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, અક્ષર designation);

પૂર્ણાંક vterm_unicode_width(uint32_t codepoint);
પૂર્ણાંક vterm_unicode_is_combining(uint32_t codepoint);
પૂર્ણાંક vterm_unicode_is_ambiguous(uint32_t codepoint);
પૂર્ણાંક vterm_get_special_pty_type(શૂન્ય);

#જો (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#નહીંતર
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
પૂર્ણાંક VSNPRINTF(અક્ષર *str, size_t str_m, અચલ અક્ષર *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
પૂર્ણાંક SNPRINTF(અક્ષર *str, size_t str_m, અચલ અક્ષર *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
