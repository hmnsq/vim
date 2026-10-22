#അന്തര്ഭാവ "vterm_internal.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#undef DEBUG_PARSER

സ്ഥിതം പൂർണ്ണസംഖ്യ is_intermed(unsigned അക്ഷരം c)
{
  തിരികെ c >= 0x20 && c <= 0x2f;
}

സ്ഥിതം ശൂന്യം do_control(VTerm *vt, unsigned അക്ഷരം control)
{
  എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->control)
    എങ്കിൽ((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      തിരികെ;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

സ്ഥിതം ശൂന്യം do_csi(VTerm *vt, അക്ഷരം command)
{
#ifdef DEBUG_PARSER
  printf("Parsed CSI args as:\n", arglen, args);
  printf(" leader: %s\n", vt->parser.v.csi.leader);
  വേണ്ടി(പൂർണ്ണസംഖ്യ argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    printf(" %lu", CSI_ARG(vt->parser.v.csi.args[argi]));
    എങ്കിൽ(!CSI_ARG_HAS_MORE(vt->parser.v.csi.args[argi]))
      printf("\n");
  printf(" intermed: %s\n", vt->parser.intermed);
  }
#endif

  എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->csi)
    എങ്കിൽ((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : ശൂന്യ,
          vt->parser.v.csi.args,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : ശൂന്യ,
          command,
          vt->parser.cbdata))
      തിരികെ;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

സ്ഥിതം ശൂന്യം do_escape(VTerm *vt, അക്ഷരം command)
{
  അക്ഷരം seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->escape)
    എങ്കിൽ((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      തിരികെ;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

സ്ഥിതം ശൂന്യം string_fragment(VTerm *vt, സ്ഥിരം അക്ഷരം *str, size_t len, പൂർണ്ണസംഖ്യ final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  തിരഞ്ഞെടുപ്പ്(vt->parser.state) {
    അവസ്ഥ OSC:
      എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      നിർത്തുക;

    അവസ്ഥ DCS:
      എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      നിർത്തുക;

    അവസ്ഥ APC:
      എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      നിർത്തുക;

    അവസ്ഥ PM:
      എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      നിർത്തുക;

    അവസ്ഥ SOS:
      എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      നിർത്തുക;

    അവസ്ഥ NORMAL:
    അവസ്ഥ CSI_LEADER:
    അവസ്ഥ CSI_ARGS:
    അവസ്ഥ CSI_INTERMED:
    അവസ്ഥ OSC_COMMAND:
    അവസ്ഥ DCS_COMMAND:
      നിർത്തുക;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_നിവേശം_write(VTerm *vt, സ്ഥിരം അക്ഷരം *bytes, size_t len)
{
  size_t pos = 0;
  സ്ഥിരം അക്ഷരം *string_start = ശൂന്യ;  // init to avoid gcc warning

  vt->in_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  തിരഞ്ഞെടുപ്പ്(vt->parser.state) {
  അവസ്ഥ NORMAL:
  അവസ്ഥ CSI_LEADER:
  അവസ്ഥ CSI_ARGS:
  അവസ്ഥ CSI_INTERMED:
  അവസ്ഥ OSC_COMMAND:
  അവസ്ഥ DCS_COMMAND:
    string_start = ശൂന്യ;
    നിർത്തുക;
  അവസ്ഥ OSC:
  അവസ്ഥ DCS:
  അവസ്ഥ APC:
  അവസ്ഥ PM:
  അവസ്ഥ SOS:
    string_start = bytes;
    നിർത്തുക;
  }

#define ENTER_STATE(st)        ചെയ്യുക { vt->parser.state = st; string_start = ശൂന്യ; } ആകുമ്പോൾ(0)
#define ENTER_NORMAL_STATE()   ENTER_STATE(NORMAL)

#define IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  വേണ്ടി( ; pos < len; pos++) {
    unsigned അക്ഷരം c = bytes[pos];
    പൂർണ്ണസംഖ്യ c1_allowed = !vt->mode.utf8;

    എങ്കിൽ(c == 0x00 || c == 0x7f) { // NUL, DEL
      എങ്കിൽ(IS_STRING_STATE()) {
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
        string_start = bytes + pos + 1;
      }
      എങ്കിൽ(vt->parser.emit_nul)
        do_control(vt, c);
      തുടരുക;
    }
    എങ്കിൽ(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.in_esc = FALSE;
      ENTER_NORMAL_STATE();
      എങ്കിൽ(vt->parser.emit_nul)
        do_control(vt, c);
      തുടരുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      എങ്കിൽ(!IS_STRING_STATE())
        vt->parser.state = NORMAL;
      vt->parser.in_esc = TRUE;
      തുടരുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    അല്ലെങ്കിൽ എങ്കിൽ(c < 0x20) { // other C0
      എങ്കിൽ(vt->parser.state == SOS)
        തുടരുക; // All other C0s permitted in SOS

      എങ്കിൽ(vterm_get_special_pty_type() == 2) {
        എങ്കിൽ(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          എങ്കിൽ(pos + 2 < len && bytes[pos + 1] == 0x20 && bytes[pos + 2] == 0x08)
            vt->in_backspace = 2; // Trigger when count down to 1
      }
      എങ്കിൽ(IS_STRING_STATE())
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
      do_control(vt, c);
      എങ്കിൽ(IS_STRING_STATE())
        string_start = bytes + pos + 1;
      തുടരുക;
    }
    // else fallthrough

    size_t string_len = bytes + pos - string_start;

    എങ്കിൽ(vt->parser.in_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      എങ്കിൽ(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        എങ്കിൽ(string_len)
          string_len -= 1;
        vt->parser.in_esc = FALSE;
      }
      അല്ലെങ്കിൽ {
        string_start = ശൂന്യ;
        vt->parser.state = NORMAL;
      }
    }

    തിരഞ്ഞെടുപ്പ്(vt->parser.state) {
    അവസ്ഥ CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      എങ്കിൽ(c >= 0x3c && c <= 0x3f) {
        എങ്കിൽ(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        നിർത്തുക;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.args[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_ARGS;

      /* fallthrough */
    അവസ്ഥ CSI_ARGS:
      /* Numerical value of argument */
      എങ്കിൽ(c >= '0' && c <= '9') {
        long arg_max = CSI_ARG_MISSING - 1;
        long *arg = &vt->parser.v.csi.args[vt->parser.v.csi.argi];
        പൂർണ്ണസംഖ്യ digit = c - '0';

        എങ്കിൽ(*arg == CSI_ARG_MISSING)
          *arg = 0;
        എങ്കിൽ(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        അല്ലെങ്കിൽ
          *arg = *arg * 10 + digit;
        നിർത്തുക;
      }
      എങ്കിൽ(c == ':') {
        vt->parser.v.csi.args[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_MORE;
        c = ';';
      }
      എങ്കിൽ(c == ';') {
        എങ്കിൽ(vt->parser.v.csi.argi >= CSI_ARGS_MAX - 1)
          നിർത്തുക; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.args[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        നിർത്തുക;
      }

      /* else fallthrough */
      vt->parser.v.csi.argi++;
      vt->parser.intermedlen = 0;
      vt->parser.state = CSI_INTERMED;
      // FALLTHROUGH
    അവസ്ഥ CSI_INTERMED:
      എങ്കിൽ(is_intermed(c)) {
        എങ്കിൽ(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        നിർത്തുക;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      അല്ലെങ്കിൽ എങ്കിൽ(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_NORMAL_STATE();
      നിർത്തുക;

    അവസ്ഥ OSC_COMMAND:
      /* Numerical value of command */
      എങ്കിൽ(c >= '0' && c <= '9') {
        എങ്കിൽ(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        അല്ലെങ്കിൽ
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        നിർത്തുക;
      }
      എങ്കിൽ(c == ';') {
        vt->parser.state = OSC;
        string_start = bytes + pos + 1;
        നിർത്തുക;
      }

      /* else fallthrough */
      string_start = bytes + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      പോകുക string_state;

    അവസ്ഥ DCS_COMMAND:
      എങ്കിൽ(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      എങ്കിൽ(c >= 0x40 && c<= 0x7e) {
        string_start = bytes + pos + 1;
        vt->parser.state = DCS;
      }
      നിർത്തുക;

string_state:
    അവസ്ഥ OSC:
    അവസ്ഥ DCS:
    അവസ്ഥ APC:
    അവസ്ഥ PM:
    അവസ്ഥ SOS:
      എങ്കിൽ(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_start, string_len, TRUE);
        ENTER_NORMAL_STATE();
      }
      നിർത്തുക;

    അവസ്ഥ NORMAL:
      എങ്കിൽ(vt->parser.in_esc) {
        എങ്കിൽ(is_intermed(c)) {
          എങ്കിൽ(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.in_esc = 0;
          ENTER_NORMAL_STATE();
        }
        അല്ലെങ്കിൽ {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        നിർത്തുക;
      }
      എങ്കിൽ(c1_allowed && c >= 0x80 && c < 0xa0) {
        തിരഞ്ഞെടുപ്പ്(c) {
        അവസ്ഥ 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          നിർത്തുക;
        അവസ്ഥ 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_start = bytes + pos + 1;
          string_len = 0;
          നിർത്തുക;
        അവസ്ഥ 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          നിർത്തുക;
        അവസ്ഥ 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_start = bytes + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          നിർത്തുക;
        അവസ്ഥ 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_start = bytes + pos + 1;
          string_len = 0;
          നിർത്തുക;
        അവസ്ഥ 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_start = bytes + pos + 1;
          string_len = 0;
          നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
          do_control(vt, c);
          നിർത്തുക;
        }
      }
      അല്ലെങ്കിൽ {
        size_t eaten = 0;
        എങ്കിൽ(vt->parser.callbacks && vt->parser.callbacks->text)
          eaten = (*vt->parser.callbacks->text)(bytes + pos, len - pos, vt->parser.cbdata);

        എങ്കിൽ(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      നിർത്തുക;
    }
  }

  എങ്കിൽ(string_start) {
    size_t string_len = bytes + pos - string_start;
    എങ്കിൽ(vt->parser.in_esc)
      string_len -= 1;
    string_fragment(vt, string_start, string_len, FALSE);
  }

  തിരികെ len;
}

ശൂന്യം vterm_parser_set_callbacks(VTerm *vt, സ്ഥിരം VTermParserCallbacks *callbacks, ശൂന്യം *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

ശൂന്യം *vterm_parser_get_cbdata(VTerm *vt)
{
  തിരികെ vt->parser.cbdata;
}

ശൂന്യം vterm_parser_set_emit_nul(VTerm *vt, പൂർണ്ണസംഖ്യ emit)
{
  vt->parser.emit_nul = emit;
}
