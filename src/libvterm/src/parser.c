#شامل "vterm_internal.h"

#شامل <stdio.h>
#شامل <string.h>

#تعریف_منسوخ DEBUG_PARSER

ساکن صحیح is_intermed(غیرعلامتی حرف c)
{
  واپس c >= 0x20 && c <= 0x2f;
}

ساکن خالی do_control(VTerm *vt, غیرعلامتی حرف control)
{
  اگر(vt->parser.callbacks && vt->parser.callbacks->control)
    اگر((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      واپس;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

ساکن خالی do_csi(VTerm *vt, حرف command)
{
#اگر_تعریف_شدہ DEBUG_PARSER
  لکھو("Parsed CSI args as:\n", arglen, دلائل);
  لکھو(" leader: %s\n", vt->parser.v.csi.leader);
  برائے(صحیح argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    لکھو(" %lu", CSI_ARG(vt->parser.v.csi.دلائل[argi]));
    اگر(!CSI_ARG_HAS_مزید(vt->parser.v.csi.دلائل[argi]))
      لکھو("\n");
  لکھو(" intermed: %s\n", vt->parser.intermed);
  }
#اگر_ختم

  اگر(vt->parser.callbacks && vt->parser.callbacks->csi)
    اگر((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : تہی,
          vt->parser.v.csi.دلائل,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : تہی,
          command,
          vt->parser.cbdata))
      واپس;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

ساکن خالی do_escape(VTerm *vt, حرف command)
{
  حرف seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  اگر(vt->parser.callbacks && vt->parser.callbacks->escape)
    اگر((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      واپس;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

ساکن خالی string_fragment(VTerm *vt, ثابت حرف *str, size_t len, صحیح final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  انتخاب(vt->parser.state) {
    حالت OSC:
      اگر(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      توڑو;

    حالت DCS:
      اگر(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      توڑو;

    حالت APC:
      اگر(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      توڑو;

    حالت PM:
      اگر(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      توڑو;

    حالت SOS:
      اگر(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      توڑو;

    حالت معمول_2:
    حالت CSI_LEADER:
    حالت CSI_دلائل:
    حالت CSI_INTERMED:
    حالت OSC_COMMAND:
    حالت DCS_COMMAND:
      توڑو;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_داخلہ_write(VTerm *vt, ثابت حرف *بائٹیں, size_t len)
{
  size_t pos = 0;
  ثابت حرف *string_آغاز = تہی;  // init to avoid gcc warning

  vt->اندر_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  انتخاب(vt->parser.state) {
  حالت معمول_2:
  حالت CSI_LEADER:
  حالت CSI_دلائل:
  حالت CSI_INTERMED:
  حالت OSC_COMMAND:
  حالت DCS_COMMAND:
    string_آغاز = تہی;
    توڑو;
  حالت OSC:
  حالت DCS:
  حالت APC:
  حالت PM:
  حالت SOS:
    string_آغاز = بائٹیں;
    توڑو;
  }

#تعریف_کرو ENTER_STATE(st)        کرو { vt->parser.state = st; string_آغاز = تہی; } جب(0)
#تعریف_کرو ENTER_معمول_STATE()   ENTER_STATE(معمول_2)

#تعریف_کرو IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  برائے( ; pos < len; pos++) {
    غیرعلامتی حرف c = بائٹیں[pos];
    صحیح c1_allowed = !vt->منوال.utf8;

    اگر(c == 0x00 || c == 0x7f) { // NUL, DEL
      اگر(IS_STRING_STATE()) {
        string_fragment(vt, string_آغاز, بائٹیں + pos - string_آغاز, FALSE);
        string_آغاز = بائٹیں + pos + 1;
      }
      اگر(vt->parser.emit_nul)
        do_control(vt, c);
      جاری;
    }
    اگر(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.اندر_esc = FALSE;
      ENTER_معمول_STATE();
      اگر(vt->parser.emit_nul)
        do_control(vt, c);
      جاری;
    }
    ورنہ اگر(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      اگر(!IS_STRING_STATE())
        vt->parser.state = معمول_2;
      vt->parser.اندر_esc = TRUE;
      جاری;
    }
    ورنہ اگر(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    ورنہ اگر(c < 0x20) { // other C0
      اگر(vt->parser.state == SOS)
        جاری; // All other C0s permitted in SOS

      اگر(vterm_get_special_pty_type() == 2) {
        اگر(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          اگر(pos + 2 < len && بائٹیں[pos + 1] == 0x20 && بائٹیں[pos + 2] == 0x08)
            vt->اندر_backspace = 2; // Trigger when count down to 1
      }
      اگر(IS_STRING_STATE())
        string_fragment(vt, string_آغاز, بائٹیں + pos - string_آغاز, FALSE);
      do_control(vt, c);
      اگر(IS_STRING_STATE())
        string_آغاز = بائٹیں + pos + 1;
      جاری;
    }
    // else fallthrough

    size_t string_len = بائٹیں + pos - string_آغاز;

    اگر(vt->parser.اندر_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      اگر(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        اگر(string_len)
          string_len -= 1;
        vt->parser.اندر_esc = FALSE;
      }
      ورنہ {
        string_آغاز = تہی;
        vt->parser.state = معمول_2;
      }
    }

    انتخاب(vt->parser.state) {
    حالت CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      اگر(c >= 0x3c && c <= 0x3f) {
        اگر(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        توڑو;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.دلائل[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_دلائل;

      /* fallthrough */
    حالت CSI_دلائل:
      /* Numerical value of argument */
      اگر(c >= '0' && c <= '9') {
        طویل arg_max = CSI_ARG_MISSING - 1;
        طویل *arg = &vt->parser.v.csi.دلائل[vt->parser.v.csi.argi];
        صحیح digit = c - '0';

        اگر(*arg == CSI_ARG_MISSING)
          *arg = 0;
        اگر(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        ورنہ
          *arg = *arg * 10 + digit;
        توڑو;
      }
      اگر(c == ':') {
        vt->parser.v.csi.دلائل[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_مزید;
        c = ';';
      }
      اگر(c == ';') {
        اگر(vt->parser.v.csi.argi >= CSI_دلائل_MAX - 1)
          توڑو; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.دلائل[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        توڑو;
      }

      /* else fallthrough */
      vt->parser.v.csi.argi++;
      vt->parser.intermedlen = 0;
      vt->parser.state = CSI_INTERMED;
      // FALLTHROUGH
    حالت CSI_INTERMED:
      اگر(is_intermed(c)) {
        اگر(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        توڑو;
      }
      ورنہ اگر(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      ورنہ اگر(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_معمول_STATE();
      توڑو;

    حالت OSC_COMMAND:
      /* Numerical value of command */
      اگر(c >= '0' && c <= '9') {
        اگر(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        ورنہ
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        توڑو;
      }
      اگر(c == ';') {
        vt->parser.state = OSC;
        string_آغاز = بائٹیں + pos + 1;
        توڑو;
      }

      /* else fallthrough */
      string_آغاز = بائٹیں + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      جاؤ string_state;

    حالت DCS_COMMAND:
      اگر(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      اگر(c >= 0x40 && c<= 0x7e) {
        string_آغاز = بائٹیں + pos + 1;
        vt->parser.state = DCS;
      }
      توڑو;

string_state:
    حالت OSC:
    حالت DCS:
    حالت APC:
    حالت PM:
    حالت SOS:
      اگر(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_آغاز, string_len, TRUE);
        ENTER_معمول_STATE();
      }
      توڑو;

    حالت معمول_2:
      اگر(vt->parser.اندر_esc) {
        اگر(is_intermed(c)) {
          اگر(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        ورنہ اگر(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.اندر_esc = 0;
          ENTER_معمول_STATE();
        }
        ورنہ {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        توڑو;
      }
      اگر(c1_allowed && c >= 0x80 && c < 0xa0) {
        انتخاب(c) {
        حالت 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          توڑو;
        حالت 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_آغاز = بائٹیں + pos + 1;
          string_len = 0;
          توڑو;
        حالت 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          توڑو;
        حالت 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_آغاز = بائٹیں + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          توڑو;
        حالت 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_آغاز = بائٹیں + pos + 1;
          string_len = 0;
          توڑو;
        حالت 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_آغاز = بائٹیں + pos + 1;
          string_len = 0;
          توڑو;
        طےشدہ:
          do_control(vt, c);
          توڑو;
        }
      }
      ورنہ {
        size_t eaten = 0;
        اگر(vt->parser.callbacks && vt->parser.callbacks->متن)
          eaten = (*vt->parser.callbacks->متن)(بائٹیں + pos, len - pos, vt->parser.cbdata);

        اگر(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      توڑو;
    }
  }

  اگر(string_آغاز) {
    size_t string_len = بائٹیں + pos - string_آغاز;
    اگر(vt->parser.اندر_esc)
      string_len -= 1;
    string_fragment(vt, string_آغاز, string_len, FALSE);
  }

  واپس len;
}

خالی vterm_parser_سیٹ_callbacks(VTerm *vt, ثابت VTermParserCallbacks *callbacks, خالی *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

خالی *vterm_parser_get_cbdata(VTerm *vt)
{
  واپس vt->parser.cbdata;
}

خالی vterm_parser_سیٹ_emit_nul(VTerm *vt, صحیح emit)
{
  vt->parser.emit_nul = emit;
}
