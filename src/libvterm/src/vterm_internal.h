#నిర్వచితం_కాకపోతే __VTERM_INTERNAL_H__
#నిర్వచించు __VTERM_INTERNAL_H__

#అన్తర్భావ "vterm.h"

#అన్తర్భావ <stdarg.h>

#ఒకవేళ defined(__GNUC__) && !defined(__MINGW32__)
# define అంతర్గత __attribute__((visibility("internal")))
#లేకపోతే
# define అంతర్గత
#అయితే_ముగింపు

#ఒకవేళ defined(__GNUC__) || defined(__MINGW32__)
# define వాడని __attribute__((వాడని_2))
#లేకపోతే
# define వాడని
#అయితే_ముగింపు

#నిర్వచితమైతే DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#లేకపోతే
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#అయితే_ముగింపు

#నిర్వచించు ESC_S "\x1b"

#నిర్వచించు INTERMED_MAX 16

#నిర్వచించు CSI_వాదనలు_MAX 16
#నిర్వచించు CSI_LEADER_MAX 16

#నిర్వచించు BUFIDX_PRIMARY   0
#నిర్వచించు BUFIDX_ALTSCREEN 1

రకనిర్వచనం నిర్మాణం VTermEncoding VTermEncoding;

రకనిర్వచనం నిర్మాణం {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  అక్షరం           దత్తాంశం[4*పరిమాణం(uint32_t)];
} VTermEncodingInstance;

నిర్మాణం VTermPen
{
  VTermColor fg;
  VTermColor bg;
  గుర్తులేని పూర్ణాంకం bold:1;
  గుర్తులేని పూర్ణాంకం underline:2;
  గుర్తులేని పూర్ణాంకం italic:1;
  గుర్తులేని పూర్ణాంకం blink:1;
  గుర్తులేని పూర్ణాంకం తిరగేయి:1;
  గుర్తులేని పూర్ణాంకం conceal:1;
  గుర్తులేని పూర్ణాంకం strike:1;
  గుర్తులేని పూర్ణాంకం font:4; /* To store 0-9 */
  గుర్తులేని పూర్ణాంకం small:1;
  గుర్తులేని పూర్ణాంకం baseline:2;
};

నిర్మాణం VTermState
{
  VTerm *vt;

  స్థిరరాశి VTermStateCallbacks *callbacks;
  శూన్య *cbdata;
  పూర్ణాంకం callbacks_has_premove;

  స్థిరరాశి VTermStateFallbacks *fallbacks;
  శూన్య *fbdata;

  పూర్ణాంకం అడ్డువరుసలు_2;
  పూర్ణాంకం నిలువుల_సంఖ్య;

  /* Current cursor position */
  VTermPos pos;

  పూర్ణాంకం at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  పూర్ణాంకం scrollregion_శిఖరం;
  పూర్ణాంకం scrollregion_bottom; /* -1 means unbounded */
#నిర్వచించు SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->అడ్డువరుసలు_2)
  పూర్ణాంకం scrollregion_ఎడమ;
#నిర్వచించు SCROLLREGION_ఎడమ(state)  ((state)->బహుళకం.leftrightmargin ? (state)->scrollregion_ఎడమ : 0)
  పూర్ణాంకం scrollregion_కుడి; /* -1 means unbounded */
#నిర్వచించు SCROLLREGION_కుడి(state) ((state)->బహుళకం.leftrightmargin && (state)->scrollregion_కుడి > -1 ? (state)->scrollregion_కుడి : (state)->నిలువుల_సంఖ్య)

  /* Bitvector of tab stops */
  గుర్తులేని అక్షరం *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_పంక్తి_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_పంక్తి_Info *lineinfo;
#నిర్వచించు ROWWIDTH(state,అడ్డువరుస) ((state)->lineinfo[(అడ్డువరుస)].doublewidth ? ((state)->నిలువుల_సంఖ్య / 2) : (state)->నిలువుల_సంఖ్య)
#నిర్వచించు THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.అడ్డువరుస)

  /* Mouse state */
  పూర్ణాంకం mouse_నిలువు, mouse_అడ్డువరుస;
  పూర్ణాంకం mouse_buttons;
  పూర్ణాంకం mouse_flags;

  గణన { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_కొలత; // Number of ELEMENTS in the above
  పూర్ణాంకం combine_వెడల్పు; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  నిర్మాణం {
    గుర్తులేని పూర్ణాంకం keypad:1;
    గుర్తులేని పూర్ణాంకం cursor:1;
    గుర్తులేని పూర్ణాంకం autowrap:1;
    గుర్తులేని పూర్ణాంకం చొప్పించు:1;
    గుర్తులేని పూర్ణాంకం newline:1;
    గుర్తులేని పూర్ణాంకం cursor_visible:1;
    గుర్తులేని పూర్ణాంకం cursor_blink:1;
    గుర్తులేని పూర్ణాంకం cursor_shape:2;
    గుర్తులేని పూర్ణాంకం alt_screen:1;
    గుర్తులేని పూర్ణాంకం origin:1;
    గుర్తులేని పూర్ణాంకం screen:1;
    గుర్తులేని పూర్ణాంకం leftrightmargin:1;
    గుర్తులేని పూర్ణాంకం bracketpaste:1;
    గుర్తులేని పూర్ణాంకం report_focus:1;
    గుర్తులేని పూర్ణాంకం modify_other_కీలకాలు:1;
    గుర్తులేని పూర్ణాంకం kitty_keyboard:1;
  } బహుళకం;

  VTermEncodingInstance encoding[4], encoding_utf8;
  పూర్ణాంకం gl_సమితి, gr_సమితి, gsingle_సమితి;

  నిర్మాణం VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  పూర్ణాంకం bold_is_highbright;

  గుర్తులేని పూర్ణాంకం protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  నిర్మాణం {
    VTermPos pos;
    నిర్మాణం VTermPen pen;

    నిర్మాణం {
      గుర్తులేని పూర్ణాంకం cursor_visible:1;
      గుర్తులేని పూర్ణాంకం cursor_blink:1;
      గుర్తులేని పూర్ణాంకం cursor_shape:2;
    } బహుళకం;
  } saved;

  /* Temporary state for DECRQSS parsing */
  సంఘం {
    అక్షరం decrqss[4];
    నిర్మాణం {
      uint16_t ముసుగు;
      గణన {
        SELECTION_INITIAL,
        SELECTION_SELECTED,
        SELECTION_QUERY,
        SELECTION_సమితి_INITIAL,
        SELECTION_సమితి,
        SELECTION_INVALID,
      } state : 8;
      uint32_t recvpartial;
      uint32_t sendpartial;
    } selection;
  } tmp;

  నిర్మాణం {
    స్థిరరాశి VTermSelectionCallbacks *callbacks;
    శూన్య *user;
    అక్షరం *బఫరు;
    size_t buflen;
  } selection;
};

నిర్మాణం VTerm
{
  స్థిరరాశి VTermAllocatorFunctions *allocator;
  శూన్య *allocdata;

  పూర్ణాంకం అడ్డువరుసలు_2;
  పూర్ణాంకం నిలువుల_సంఖ్య;

  నిర్మాణం {
    గుర్తులేని పూర్ణాంకం utf8:1;
    గుర్తులేని పూర్ణాంకం ctrl8bit:1;
  } బహుళకం;

  నిర్మాణం {
    గణన VTermParserState {
      సాధారణ_2,
      CSI_LEADER,
      CSI_వాదనలు,
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

    గుర్తులేని పూర్ణాంకం లోపల_esc : 1;

    పూర్ణాంకం intermedlen;
    అక్షరం intermed[INTERMED_MAX];

    సంఘం {
      నిర్మాణం {
        పూర్ణాంకం leaderlen;
        అక్షరం leader[CSI_LEADER_MAX];

        పూర్ణాంకం argi;
        దీర్ఘ వాదనలు[CSI_వాదనలు_MAX];
      } csi;
      నిర్మాణం {
        పూర్ణాంకం command;
      } osc;
      నిర్మాణం {
        పూర్ణాంకం commandlen;
        అక్షరం command[CSI_LEADER_MAX];
      } dcs;
    } v;

    స్థిరరాశి VTermParserCallbacks *callbacks;
    శూన్య *cbdata;

    పూర్ణాంకం string_initial;

    పూర్ణాంకం emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  శూన్య                *outdata;

  అక్షరం  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  అక్షరం  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  పూర్ణాంకం లోపల_backspace;
};

నిర్మాణం VTermEncoding {
  శూన్య (*init) (VTermEncoding *enc, శూన్య *దత్తాంశం);
  శూన్య (*decode)(VTermEncoding *enc, శూన్య *దత్తాంశం,
                 uint32_t cp[], పూర్ణాంకం *cpi, పూర్ణాంకం cplen,
                 స్థిరరాశి అక్షరం బైట్లు[], size_t *pos, size_t len);
};

రకనిర్వచనం గణన {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

శూన్య *vterm_allocator_malloc(VTerm *vt, size_t కొలత);
శూన్య  vterm_allocator_free(VTerm *vt, శూన్య *ptr);

శూన్య vterm_నెట్టు_output_బైట్లు(VTerm *vt, స్థిరరాశి అక్షరం *బైట్లు, size_t len);
శూన్య vterm_నెట్టు_output_vsprintf(VTerm *vt, స్థిరరాశి అక్షరం *format, va_list వాదనలు);
శూన్య vterm_నెట్టు_output_sprintf(VTerm *vt, స్థిరరాశి అక్షరం *format, ...);
శూన్య vterm_నెట్టు_output_sprintf_ctrl(VTerm *vt, గుర్తులేని అక్షరం ctrl, స్థిరరాశి అక్షరం *fmt, ...);
శూన్య vterm_నెట్టు_output_sprintf_str(VTerm *vt, గుర్తులేని అక్షరం ctrl, పూర్ణాంకం పదం, స్థిరరాశి అక్షరం *fmt, ...);

శూన్య vterm_state_free(VTermState *state);

శూన్య vterm_state_newpen(VTermState *state);
శూన్య vterm_state_resetpen(VTermState *state);
శూన్య vterm_state_setpen(VTermState *state, స్థిరరాశి దీర్ఘ వాదనలు[], పూర్ణాంకం argcount);
పూర్ణాంకం  vterm_state_getpen(VTermState *state, దీర్ఘ వాదనలు[], పూర్ణాంకం argcount);
శూన్య vterm_state_savepen(VTermState *state, పూర్ణాంకం save);

గణన {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

శూన్య vterm_state_నెట్టు_output_sprintf_CSI(VTermState *vts, స్థిరరాశి అక్షరం *format, ...);

శూన్య vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, అక్షరం designation);

పూర్ణాంకం vterm_unicode_వెడల్పు(uint32_t codepoint);
పూర్ణాంకం vterm_unicode_is_combining(uint32_t codepoint);
పూర్ణాంకం vterm_unicode_is_ambiguous(uint32_t codepoint);
పూర్ణాంకం vterm_get_special_pty_type(శూన్య);

#ఒకవేళ (defined(XOPEN_ఆధారం) && XOPEN_ఆధారం >= 500) \
	|| defined(ISOC99_ఆధారం) || defined(BSD_ఆధారం)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#లేకపోతే
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
పూర్ణాంకం VSNPRINTF(అక్షరం *str, size_t str_m, స్థిరరాశి అక్షరం *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
పూర్ణాంకం SNPRINTF(అక్షరం *str, size_t str_m, స్థిరరాశి అక్షరం *fmt, ...);
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SNPRINTF
# define SNPRINTF snprintf
#అయితే_ముగింపు


#అయితే_ముగింపు
