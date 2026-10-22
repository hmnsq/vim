#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_internal.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEBUG_PARSER

ਸਥਿਰ ਪੂਰਨ is_intermed(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
  ਵਾਪਸ c >= 0x20 && c <= 0x2f;
}

ਸਥਿਰ ਖਾਲੀ do_control(VTerm *vt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ control)
{
  ਜੇ(vt->parser.callbacks && vt->parser.callbacks->control)
    ਜੇ((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      ਵਾਪਸ;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

ਸਥਿਰ ਖਾਲੀ do_csi(VTerm *vt, ਅੱਖਰ command)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_PARSER
  ਲਿਖੋ("Parsed CSI args as:\n", arglen, ਦਲੀਲਾਂ);
  ਲਿਖੋ(" leader: %s\n", vt->parser.v.csi.leader);
  ਲਈ(ਪੂਰਨ argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    ਲਿਖੋ(" %lu", CSI_ARG(vt->parser.v.csi.ਦਲੀਲਾਂ[argi]));
    ਜੇ(!CSI_ARG_HAS_ਹੋਰ(vt->parser.v.csi.ਦਲੀਲਾਂ[argi]))
      ਲਿਖੋ("\n");
  ਲਿਖੋ(" intermed: %s\n", vt->parser.intermed);
  }
#ਜੇ_ਅੰਤ

  ਜੇ(vt->parser.callbacks && vt->parser.callbacks->csi)
    ਜੇ((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : ਨਿਰਾ,
          vt->parser.v.csi.ਦਲੀਲਾਂ,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : ਨਿਰਾ,
          command,
          vt->parser.cbdata))
      ਵਾਪਸ;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

ਸਥਿਰ ਖਾਲੀ do_escape(VTerm *vt, ਅੱਖਰ command)
{
  ਅੱਖਰ seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  ਜੇ(vt->parser.callbacks && vt->parser.callbacks->escape)
    ਜੇ((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      ਵਾਪਸ;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

ਸਥਿਰ ਖਾਲੀ string_fragment(VTerm *vt, ਅਚਲ ਅੱਖਰ *str, size_t len, ਪੂਰਨ final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  ਚੋਣ(vt->parser.state) {
    ਹਾਲਤ OSC:
      ਜੇ(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      ਰੋਕੋ;

    ਹਾਲਤ DCS:
      ਜੇ(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      ਰੋਕੋ;

    ਹਾਲਤ APC:
      ਜੇ(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      ਰੋਕੋ;

    ਹਾਲਤ PM:
      ਜੇ(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      ਰੋਕੋ;

    ਹਾਲਤ SOS:
      ਜੇ(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      ਰੋਕੋ;

    ਹਾਲਤ ਸਾਧਾਰਨ_2:
    ਹਾਲਤ CSI_LEADER:
    ਹਾਲਤ CSI_ਦਲੀਲਾਂ:
    ਹਾਲਤ CSI_INTERMED:
    ਹਾਲਤ OSC_COMMAND:
    ਹਾਲਤ DCS_COMMAND:
      ਰੋਕੋ;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_ਨਿਵੇਸ਼_write(VTerm *vt, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t len)
{
  size_t pos = 0;
  ਅਚਲ ਅੱਖਰ *string_ਸ਼ੁਰੂ = ਨਿਰਾ;  // init to avoid gcc warning

  vt->ਵਿੱਚ_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  ਚੋਣ(vt->parser.state) {
  ਹਾਲਤ ਸਾਧਾਰਨ_2:
  ਹਾਲਤ CSI_LEADER:
  ਹਾਲਤ CSI_ਦਲੀਲਾਂ:
  ਹਾਲਤ CSI_INTERMED:
  ਹਾਲਤ OSC_COMMAND:
  ਹਾਲਤ DCS_COMMAND:
    string_ਸ਼ੁਰੂ = ਨਿਰਾ;
    ਰੋਕੋ;
  ਹਾਲਤ OSC:
  ਹਾਲਤ DCS:
  ਹਾਲਤ APC:
  ਹਾਲਤ PM:
  ਹਾਲਤ SOS:
    string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ;
    ਰੋਕੋ;
  }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENTER_STATE(st)        ਕਰੋ { vt->parser.state = st; string_ਸ਼ੁਰੂ = ਨਿਰਾ; } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENTER_ਸਾਧਾਰਨ_STATE()   ENTER_STATE(ਸਾਧਾਰਨ_2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  ਲਈ( ; pos < len; pos++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = ਬਾਈਟਾਂ[pos];
    ਪੂਰਨ c1_allowed = !vt->ਬਹੁਲਕ.utf8;

    ਜੇ(c == 0x00 || c == 0x7f) { // NUL, DEL
      ਜੇ(IS_STRING_STATE()) {
        string_fragment(vt, string_ਸ਼ੁਰੂ, ਬਾਈਟਾਂ + pos - string_ਸ਼ੁਰੂ, FALSE);
        string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos + 1;
      }
      ਜੇ(vt->parser.emit_nul)
        do_control(vt, c);
      ਜਾਰੀ;
    }
    ਜੇ(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.ਵਿੱਚ_esc = FALSE;
      ENTER_ਸਾਧਾਰਨ_STATE();
      ਜੇ(vt->parser.emit_nul)
        do_control(vt, c);
      ਜਾਰੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      ਜੇ(!IS_STRING_STATE())
        vt->parser.state = ਸਾਧਾਰਨ_2;
      vt->parser.ਵਿੱਚ_esc = TRUE;
      ਜਾਰੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    ਨਹੀਂਤਾਂ ਜੇ(c < 0x20) { // other C0
      ਜੇ(vt->parser.state == SOS)
        ਜਾਰੀ; // All other C0s permitted in SOS

      ਜੇ(vterm_get_special_pty_type() == 2) {
        ਜੇ(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          ਜੇ(pos + 2 < len && ਬਾਈਟਾਂ[pos + 1] == 0x20 && ਬਾਈਟਾਂ[pos + 2] == 0x08)
            vt->ਵਿੱਚ_backspace = 2; // Trigger when count down to 1
      }
      ਜੇ(IS_STRING_STATE())
        string_fragment(vt, string_ਸ਼ੁਰੂ, ਬਾਈਟਾਂ + pos - string_ਸ਼ੁਰੂ, FALSE);
      do_control(vt, c);
      ਜੇ(IS_STRING_STATE())
        string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos + 1;
      ਜਾਰੀ;
    }
    // else fallthrough

    size_t string_len = ਬਾਈਟਾਂ + pos - string_ਸ਼ੁਰੂ;

    ਜੇ(vt->parser.ਵਿੱਚ_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      ਜੇ(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        ਜੇ(string_len)
          string_len -= 1;
        vt->parser.ਵਿੱਚ_esc = FALSE;
      }
      ਨਹੀਂਤਾਂ {
        string_ਸ਼ੁਰੂ = ਨਿਰਾ;
        vt->parser.state = ਸਾਧਾਰਨ_2;
      }
    }

    ਚੋਣ(vt->parser.state) {
    ਹਾਲਤ CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      ਜੇ(c >= 0x3c && c <= 0x3f) {
        ਜੇ(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        ਰੋਕੋ;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.ਦਲੀਲਾਂ[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_ਦਲੀਲਾਂ;

      /* fallthrough */
    ਹਾਲਤ CSI_ਦਲੀਲਾਂ:
      /* Numerical value of argument */
      ਜੇ(c >= '0' && c <= '9') {
        ਲੰਮਾ arg_max = CSI_ARG_MISSING - 1;
        ਲੰਮਾ *arg = &vt->parser.v.csi.ਦਲੀਲਾਂ[vt->parser.v.csi.argi];
        ਪੂਰਨ digit = c - '0';

        ਜੇ(*arg == CSI_ARG_MISSING)
          *arg = 0;
        ਜੇ(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        ਨਹੀਂਤਾਂ
          *arg = *arg * 10 + digit;
        ਰੋਕੋ;
      }
      ਜੇ(c == ':') {
        vt->parser.v.csi.ਦਲੀਲਾਂ[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_ਹੋਰ;
        c = ';';
      }
      ਜੇ(c == ';') {
        ਜੇ(vt->parser.v.csi.argi >= CSI_ਦਲੀਲਾਂ_MAX - 1)
          ਰੋਕੋ; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.ਦਲੀਲਾਂ[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        ਰੋਕੋ;
      }

      /* else fallthrough */
      vt->parser.v.csi.argi++;
      vt->parser.intermedlen = 0;
      vt->parser.state = CSI_INTERMED;
      // FALLTHROUGH
    ਹਾਲਤ CSI_INTERMED:
      ਜੇ(is_intermed(c)) {
        ਜੇ(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      ਨਹੀਂਤਾਂ ਜੇ(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_ਸਾਧਾਰਨ_STATE();
      ਰੋਕੋ;

    ਹਾਲਤ OSC_COMMAND:
      /* Numerical value of command */
      ਜੇ(c >= '0' && c <= '9') {
        ਜੇ(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        ਨਹੀਂਤਾਂ
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        ਰੋਕੋ;
      }
      ਜੇ(c == ';') {
        vt->parser.state = OSC;
        string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos + 1;
        ਰੋਕੋ;
      }

      /* else fallthrough */
      string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      ਜਾਓ string_state;

    ਹਾਲਤ DCS_COMMAND:
      ਜੇ(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      ਜੇ(c >= 0x40 && c<= 0x7e) {
        string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos + 1;
        vt->parser.state = DCS;
      }
      ਰੋਕੋ;

string_state:
    ਹਾਲਤ OSC:
    ਹਾਲਤ DCS:
    ਹਾਲਤ APC:
    ਹਾਲਤ PM:
    ਹਾਲਤ SOS:
      ਜੇ(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_ਸ਼ੁਰੂ, string_len, TRUE);
        ENTER_ਸਾਧਾਰਨ_STATE();
      }
      ਰੋਕੋ;

    ਹਾਲਤ ਸਾਧਾਰਨ_2:
      ਜੇ(vt->parser.ਵਿੱਚ_esc) {
        ਜੇ(is_intermed(c)) {
          ਜੇ(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        ਨਹੀਂਤਾਂ ਜੇ(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.ਵਿੱਚ_esc = 0;
          ENTER_ਸਾਧਾਰਨ_STATE();
        }
        ਨਹੀਂਤਾਂ {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        ਰੋਕੋ;
      }
      ਜੇ(c1_allowed && c >= 0x80 && c < 0xa0) {
        ਚੋਣ(c) {
        ਹਾਲਤ 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          ਰੋਕੋ;
        ਹਾਲਤ 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos + 1;
          string_len = 0;
          ਰੋਕੋ;
        ਹਾਲਤ 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          ਰੋਕੋ;
        ਹਾਲਤ 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          ਰੋਕੋ;
        ਹਾਲਤ 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos + 1;
          string_len = 0;
          ਰੋਕੋ;
        ਹਾਲਤ 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_ਸ਼ੁਰੂ = ਬਾਈਟਾਂ + pos + 1;
          string_len = 0;
          ਰੋਕੋ;
        ਮੂਲ:
          do_control(vt, c);
          ਰੋਕੋ;
        }
      }
      ਨਹੀਂਤਾਂ {
        size_t eaten = 0;
        ਜੇ(vt->parser.callbacks && vt->parser.callbacks->ਪਾਠ)
          eaten = (*vt->parser.callbacks->ਪਾਠ)(ਬਾਈਟਾਂ + pos, len - pos, vt->parser.cbdata);

        ਜੇ(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      ਰੋਕੋ;
    }
  }

  ਜੇ(string_ਸ਼ੁਰੂ) {
    size_t string_len = ਬਾਈਟਾਂ + pos - string_ਸ਼ੁਰੂ;
    ਜੇ(vt->parser.ਵਿੱਚ_esc)
      string_len -= 1;
    string_fragment(vt, string_ਸ਼ੁਰੂ, string_len, FALSE);
  }

  ਵਾਪਸ len;
}

ਖਾਲੀ vterm_parser_ਸੈੱਟ_callbacks(VTerm *vt, ਅਚਲ VTermParserCallbacks *callbacks, ਖਾਲੀ *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

ਖਾਲੀ *vterm_parser_get_cbdata(VTerm *vt)
{
  ਵਾਪਸ vt->parser.cbdata;
}

ਖਾਲੀ vterm_parser_ਸੈੱਟ_emit_nul(VTerm *vt, ਪੂਰਨ emit)
{
  vt->parser.emit_nul = emit;
}
