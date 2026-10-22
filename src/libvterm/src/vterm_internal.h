#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __VTERM_INTERNAL_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __VTERM_INTERNAL_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "vterm.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>

#ਜੇ defined(__GNUC__) && !defined(__MINGW32__)
# define ਅੰਦਰੂਨੀ __attribute__((visibility("internal")))
#ਨਹੀਂਤਾਂ
# define ਅੰਦਰੂਨੀ
#ਜੇ_ਅੰਤ

#ਜੇ defined(__GNUC__) || defined(__MINGW32__)
# define ਅਣਵਰਤਿਆ __attribute__((ਅਣਵਰਤਿਆ_2))
#ਨਹੀਂਤਾਂ
# define ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#ਨਹੀਂਤਾਂ
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESC_S "\x1b"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTERMED_MAX 16

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ਦਲੀਲਾਂ_MAX 16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_LEADER_MAX 16

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFIDX_PRIMARY   0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFIDX_ALTSCREEN 1

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ VTermEncoding VTermEncoding;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  ਅੱਖਰ           ਅੰਕੜੇ[4*ਆਕਾਰ(uint32_t)];
} VTermEncodingInstance;

ਬਣਤਰ VTermPen
{
  VTermColor fg;
  VTermColor bg;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ bold:1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ underline:2;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ italic:1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ blink:1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਉਲਟ:1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ conceal:1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ strike:1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ font:4; /* To store 0-9 */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ small:1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ baseline:2;
};

ਬਣਤਰ VTermState
{
  VTerm *vt;

  ਅਚਲ VTermStateCallbacks *callbacks;
  ਖਾਲੀ *cbdata;
  ਪੂਰਨ callbacks_has_premove;

  ਅਚਲ VTermStateFallbacks *fallbacks;
  ਖਾਲੀ *fbdata;

  ਪੂਰਨ ਪੰਕਤੀਆਂ_2;
  ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ;

  /* Current cursor position */
  VTermPos pos;

  ਪੂਰਨ at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  ਪੂਰਨ scrollregion_ਸਿਖਰ;
  ਪੂਰਨ scrollregion_bottom; /* -1 means unbounded */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->ਪੰਕਤੀਆਂ_2)
  ਪੂਰਨ scrollregion_ਖੱਬਾ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCROLLREGION_ਖੱਬਾ(state)  ((state)->ਬਹੁਲਕ.leftrightmargin ? (state)->scrollregion_ਖੱਬਾ : 0)
  ਪੂਰਨ scrollregion_ਸੱਜਾ; /* -1 means unbounded */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCROLLREGION_ਸੱਜਾ(state) ((state)->ਬਹੁਲਕ.leftrightmargin && (state)->scrollregion_ਸੱਜਾ > -1 ? (state)->scrollregion_ਸੱਜਾ : (state)->ਖਾਨੇ_ਗਿਣਤੀ)

  /* Bitvector of tab stops */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_ਸਤਰ_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_ਸਤਰ_Info *lineinfo;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ROWWIDTH(state,ਪੰਕਤੀ) ((state)->lineinfo[(ਪੰਕਤੀ)].doublewidth ? ((state)->ਖਾਨੇ_ਗਿਣਤੀ / 2) : (state)->ਖਾਨੇ_ਗਿਣਤੀ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.ਪੰਕਤੀ)

  /* Mouse state */
  ਪੂਰਨ mouse_ਖਾਨਾ, mouse_ਪੰਕਤੀ;
  ਪੂਰਨ mouse_buttons;
  ਪੂਰਨ mouse_flags;

  ਗਿਣਤੀ { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_ਮਾਪ; // Number of ELEMENTS in the above
  ਪੂਰਨ combine_ਚੌੜਾਈ; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  ਬਣਤਰ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ keypad:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cursor:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ autowrap:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਪਾਓ:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ newline:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cursor_visible:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cursor_blink:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cursor_shape:2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ alt_screen:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ origin:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ screen:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ leftrightmargin:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ bracketpaste:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ report_focus:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ modify_other_ਕੁੰਜੀਆਂ:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ kitty_keyboard:1;
  } ਬਹੁਲਕ;

  VTermEncodingInstance encoding[4], encoding_utf8;
  ਪੂਰਨ gl_ਸੈੱਟ, gr_ਸੈੱਟ, gsingle_ਸੈੱਟ;

  ਬਣਤਰ VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  ਪੂਰਨ bold_is_highbright;

  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  ਬਣਤਰ {
    VTermPos pos;
    ਬਣਤਰ VTermPen pen;

    ਬਣਤਰ {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cursor_visible:1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cursor_blink:1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cursor_shape:2;
    } ਬਹੁਲਕ;
  } saved;

  /* Temporary state for DECRQSS parsing */
  ਸੰਘ {
    ਅੱਖਰ decrqss[4];
    ਬਣਤਰ {
      uint16_t ਨਕਾਬ;
      ਗਿਣਤੀ {
        SELECTION_INITIAL,
        SELECTION_SELECTED,
        SELECTION_QUERY,
        SELECTION_ਸੈੱਟ_INITIAL,
        SELECTION_ਸੈੱਟ,
        SELECTION_INVALID,
      } state : 8;
      uint32_t recvpartial;
      uint32_t sendpartial;
    } selection;
  } tmp;

  ਬਣਤਰ {
    ਅਚਲ VTermSelectionCallbacks *callbacks;
    ਖਾਲੀ *user;
    ਅੱਖਰ *ਬਫਰ;
    size_t buflen;
  } selection;
};

ਬਣਤਰ VTerm
{
  ਅਚਲ VTermAllocatorFunctions *allocator;
  ਖਾਲੀ *allocdata;

  ਪੂਰਨ ਪੰਕਤੀਆਂ_2;
  ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ;

  ਬਣਤਰ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ utf8:1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ctrl8bit:1;
  } ਬਹੁਲਕ;

  ਬਣਤਰ {
    ਗਿਣਤੀ VTermParserState {
      ਸਾਧਾਰਨ_2,
      CSI_LEADER,
      CSI_ਦਲੀਲਾਂ,
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

    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਿੱਚ_esc : 1;

    ਪੂਰਨ intermedlen;
    ਅੱਖਰ intermed[INTERMED_MAX];

    ਸੰਘ {
      ਬਣਤਰ {
        ਪੂਰਨ leaderlen;
        ਅੱਖਰ leader[CSI_LEADER_MAX];

        ਪੂਰਨ argi;
        ਲੰਮਾ ਦਲੀਲਾਂ[CSI_ਦਲੀਲਾਂ_MAX];
      } csi;
      ਬਣਤਰ {
        ਪੂਰਨ command;
      } osc;
      ਬਣਤਰ {
        ਪੂਰਨ commandlen;
        ਅੱਖਰ command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ਅਚਲ VTermParserCallbacks *callbacks;
    ਖਾਲੀ *cbdata;

    ਪੂਰਨ string_initial;

    ਪੂਰਨ emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  ਖਾਲੀ                *outdata;

  ਅੱਖਰ  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  ਅੱਖਰ  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  ਪੂਰਨ ਵਿੱਚ_backspace;
};

ਬਣਤਰ VTermEncoding {
  ਖਾਲੀ (*init) (VTermEncoding *enc, ਖਾਲੀ *ਅੰਕੜੇ);
  ਖਾਲੀ (*decode)(VTermEncoding *enc, ਖਾਲੀ *ਅੰਕੜੇ,
                 uint32_t cp[], ਪੂਰਨ *cpi, ਪੂਰਨ cplen,
                 ਅਚਲ ਅੱਖਰ ਬਾਈਟਾਂ[], size_t *pos, size_t len);
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

ਖਾਲੀ *vterm_allocator_malloc(VTerm *vt, size_t ਮਾਪ);
ਖਾਲੀ  vterm_allocator_free(VTerm *vt, ਖਾਲੀ *ptr);

ਖਾਲੀ vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(VTerm *vt, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t len);
ਖਾਲੀ vterm_ਧੱਕੋ_output_vsprintf(VTerm *vt, ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ);
ਖਾਲੀ vterm_ਧੱਕੋ_output_sprintf(VTerm *vt, ਅਚਲ ਅੱਖਰ *format, ...);
ਖਾਲੀ vterm_ਧੱਕੋ_output_sprintf_ctrl(VTerm *vt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ctrl, ਅਚਲ ਅੱਖਰ *fmt, ...);
ਖਾਲੀ vterm_ਧੱਕੋ_output_sprintf_str(VTerm *vt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ctrl, ਪੂਰਨ ਪਦ, ਅਚਲ ਅੱਖਰ *fmt, ...);

ਖਾਲੀ vterm_state_free(VTermState *state);

ਖਾਲੀ vterm_state_newpen(VTermState *state);
ਖਾਲੀ vterm_state_resetpen(VTermState *state);
ਖਾਲੀ vterm_state_setpen(VTermState *state, ਅਚਲ ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount);
ਪੂਰਨ  vterm_state_getpen(VTermState *state, ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount);
ਖਾਲੀ vterm_state_savepen(VTermState *state, ਪੂਰਨ save);

ਗਿਣਤੀ {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

ਖਾਲੀ vterm_state_ਧੱਕੋ_output_sprintf_CSI(VTermState *vts, ਅਚਲ ਅੱਖਰ *format, ...);

ਖਾਲੀ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ਅੱਖਰ designation);

ਪੂਰਨ vterm_unicode_ਚੌੜਾਈ(uint32_t codepoint);
ਪੂਰਨ vterm_unicode_is_combining(uint32_t codepoint);
ਪੂਰਨ vterm_unicode_is_ambiguous(uint32_t codepoint);
ਪੂਰਨ vterm_get_special_pty_type(ਖਾਲੀ);

#ਜੇ (defined(XOPEN_ਸਰੋਤ) && XOPEN_ਸਰੋਤ >= 500) \
	|| defined(ISOC99_ਸਰੋਤ) || defined(BSD_ਸਰੋਤ)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#ਨਹੀਂਤਾਂ
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
ਪੂਰਨ VSNPRINTF(ਅੱਖਰ *str, size_t str_m, ਅਚਲ ਅੱਖਰ *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
ਪੂਰਨ SNPRINTF(ਅੱਖਰ *str, size_t str_m, ਅਚਲ ਅੱਖਰ *fmt, ...);
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SNPRINTF
# define SNPRINTF snprintf
#ਜੇ_ਅੰਤ


#ਜੇ_ਅੰਤ
