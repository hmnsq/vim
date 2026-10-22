#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#அந்தர்பாவ "vterm.h"

#அந்தர்பாவ <stdarg.h>

#எனில் defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#இல்லையெனில்
# define INTERNAL
#endif

#எனில் defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#இல்லையெனில்
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#இல்லையெனில்
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

வகைவரை கட்டமைப்பு VTermEncoding VTermEncoding;

வகைவரை கட்டமைப்பு {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  எழுத்து           data[4*அளவு(uint32_t)];
} VTermEncodingInstance;

கட்டமைப்பு VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned முழு bold:1;
  unsigned முழு underline:2;
  unsigned முழு italic:1;
  unsigned முழு blink:1;
  unsigned முழு reverse:1;
  unsigned முழு conceal:1;
  unsigned முழு strike:1;
  unsigned முழு font:4; /* To store 0-9 */
  unsigned முழு small:1;
  unsigned முழு baseline:2;
};

கட்டமைப்பு VTermState
{
  VTerm *vt;

  மாறிலி VTermStateCallbacks *callbacks;
  வெறுமை *cbdata;
  முழு callbacks_has_premove;

  மாறிலி VTermStateFallbacks *fallbacks;
  வெறுமை *fbdata;

  முழு rows;
  முழு cols;

  /* Current cursor position */
  VTermPos pos;

  முழு at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  முழு scrollregion_உச்சி;
  முழு scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  முழு scrollregion_இடது;
#define SCROLLREGION_இடது(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_இடது : 0)
  முழு scrollregion_வலது; /* -1 means unbounded */
#define SCROLLREGION_வலது(state) ((state)->mode.leftrightmargin && (state)->scrollregion_வலது > -1 ? (state)->scrollregion_வலது : (state)->cols)

  /* Bitvector of tab stops */
  unsigned எழுத்து *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_வரி_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_வரி_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  முழு mouse_col, mouse_row;
  முழு mouse_buttons;
  முழு mouse_flags;

  எண்ணிக்கை { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  முழு combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  கட்டமைப்பு {
    unsigned முழு keypad:1;
    unsigned முழு cursor:1;
    unsigned முழு autowrap:1;
    unsigned முழு செருகு:1;
    unsigned முழு newline:1;
    unsigned முழு cursor_visible:1;
    unsigned முழு cursor_blink:1;
    unsigned முழு cursor_shape:2;
    unsigned முழு alt_screen:1;
    unsigned முழு origin:1;
    unsigned முழு screen:1;
    unsigned முழு leftrightmargin:1;
    unsigned முழு bracketpaste:1;
    unsigned முழு report_focus:1;
    unsigned முழு modify_other_சாவிகள்:1;
    unsigned முழு kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  முழு gl_set, gr_set, gsingle_set;

  கட்டமைப்பு VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  முழு bold_is_highbright;

  unsigned முழு protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  கட்டமைப்பு {
    VTermPos pos;
    கட்டமைப்பு VTermPen pen;

    கட்டமைப்பு {
      unsigned முழு cursor_visible:1;
      unsigned முழு cursor_blink:1;
      unsigned முழு cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  ஒன்றிப்பு {
    எழுத்து decrqss[4];
    கட்டமைப்பு {
      uint16_t mask;
      எண்ணிக்கை {
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

  கட்டமைப்பு {
    மாறிலி VTermSelectionCallbacks *callbacks;
    வெறுமை *user;
    எழுத்து *buffer;
    size_t buflen;
  } selection;
};

கட்டமைப்பு VTerm
{
  மாறிலி VTermAllocatorFunctions *allocator;
  வெறுமை *allocdata;

  முழு rows;
  முழு cols;

  கட்டமைப்பு {
    unsigned முழு utf8:1;
    unsigned முழு ctrl8bit:1;
  } mode;

  கட்டமைப்பு {
    எண்ணிக்கை VTermParserState {
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

    unsigned முழு in_esc : 1;

    முழு intermedlen;
    எழுத்து intermed[INTERMED_MAX];

    ஒன்றிப்பு {
      கட்டமைப்பு {
        முழு leaderlen;
        எழுத்து leader[CSI_LEADER_MAX];

        முழு argi;
        long args[CSI_ARGS_MAX];
      } csi;
      கட்டமைப்பு {
        முழு command;
      } osc;
      கட்டமைப்பு {
        முழு commandlen;
        எழுத்து command[CSI_LEADER_MAX];
      } dcs;
    } v;

    மாறிலி VTermParserCallbacks *callbacks;
    வெறுமை *cbdata;

    முழு string_initial;

    முழு emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  வெறுமை                *outdata;

  எழுத்து  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  எழுத்து  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  முழு in_backspace;
};

கட்டமைப்பு VTermEncoding {
  வெறுமை (*init) (VTermEncoding *enc, வெறுமை *data);
  வெறுமை (*decode)(VTermEncoding *enc, வெறுமை *data,
                 uint32_t cp[], முழு *cpi, முழு cplen,
                 மாறிலி எழுத்து bytes[], size_t *pos, size_t len);
};

வகைவரை எண்ணிக்கை {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

வெறுமை *vterm_allocator_malloc(VTerm *vt, size_t size);
வெறுமை  vterm_allocator_free(VTerm *vt, வெறுமை *ptr);

வெறுமை vterm_தள்ளு_output_bytes(VTerm *vt, மாறிலி எழுத்து *bytes, size_t len);
வெறுமை vterm_தள்ளு_output_vsprintf(VTerm *vt, மாறிலி எழுத்து *format, va_list args);
வெறுமை vterm_தள்ளு_output_sprintf(VTerm *vt, மாறிலி எழுத்து *format, ...);
வெறுமை vterm_தள்ளு_output_sprintf_ctrl(VTerm *vt, unsigned எழுத்து ctrl, மாறிலி எழுத்து *fmt, ...);
வெறுமை vterm_தள்ளு_output_sprintf_str(VTerm *vt, unsigned எழுத்து ctrl, முழு term, மாறிலி எழுத்து *fmt, ...);

வெறுமை vterm_state_free(VTermState *state);

வெறுமை vterm_state_newpen(VTermState *state);
வெறுமை vterm_state_resetpen(VTermState *state);
வெறுமை vterm_state_setpen(VTermState *state, மாறிலி long args[], முழு argcount);
முழு  vterm_state_getpen(VTermState *state, long args[], முழு argcount);
வெறுமை vterm_state_savepen(VTermState *state, முழு save);

எண்ணிக்கை {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

வெறுமை vterm_state_தள்ளு_output_sprintf_CSI(VTermState *vts, மாறிலி எழுத்து *format, ...);

வெறுமை vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, எழுத்து designation);

முழு vterm_unicode_width(uint32_t codepoint);
முழு vterm_unicode_is_combining(uint32_t codepoint);
முழு vterm_unicode_is_ambiguous(uint32_t codepoint);
முழு vterm_get_special_pty_type(வெறுமை);

#எனில் (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#இல்லையெனில்
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
முழு VSNPRINTF(எழுத்து *str, size_t str_m, மாறிலி எழுத்து *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
முழு SNPRINTF(எழுத்து *str, size_t str_m, மாறிலி எழுத்து *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
