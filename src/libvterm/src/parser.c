#అన్తర్భావ "vterm_internal.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#నిర్వచనం_రద్దు DEBUG_PARSER

స్థిర పూర్ణాంకం is_intermed(గుర్తులేని అక్షరం c)
{
  తిరిగి c >= 0x20 && c <= 0x2f;
}

స్థిర శూన్య do_control(VTerm *vt, గుర్తులేని అక్షరం control)
{
  ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->control)
    ఒకవేళ((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      తిరిగి;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

స్థిర శూన్య do_csi(VTerm *vt, అక్షరం command)
{
#నిర్వచితమైతే DEBUG_PARSER
  రాయి("Parsed CSI args as:\n", arglen, వాదనలు);
  రాయి(" leader: %s\n", vt->parser.v.csi.leader);
  కొరకు(పూర్ణాంకం argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    రాయి(" %lu", CSI_ARG(vt->parser.v.csi.వాదనలు[argi]));
    ఒకవేళ(!CSI_ARG_HAS_మరిన్ని(vt->parser.v.csi.వాదనలు[argi]))
      రాయి("\n");
  రాయి(" intermed: %s\n", vt->parser.intermed);
  }
#అయితే_ముగింపు

  ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->csi)
    ఒకవేళ((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : ఖాళీ,
          vt->parser.v.csi.వాదనలు,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : ఖాళీ,
          command,
          vt->parser.cbdata))
      తిరిగి;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

స్థిర శూన్య do_escape(VTerm *vt, అక్షరం command)
{
  అక్షరం seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->escape)
    ఒకవేళ((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      తిరిగి;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

స్థిర శూన్య string_fragment(VTerm *vt, స్థిరరాశి అక్షరం *str, size_t len, పూర్ణాంకం final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  ఎంపిక(vt->parser.state) {
    స్థితి OSC:
      ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      ఆపు;

    స్థితి DCS:
      ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      ఆపు;

    స్థితి APC:
      ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      ఆపు;

    స్థితి PM:
      ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      ఆపు;

    స్థితి SOS:
      ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      ఆపు;

    స్థితి సాధారణ_2:
    స్థితి CSI_LEADER:
    స్థితి CSI_వాదనలు:
    స్థితి CSI_INTERMED:
    స్థితి OSC_COMMAND:
    స్థితి DCS_COMMAND:
      ఆపు;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_ఆదానం_write(VTerm *vt, స్థిరరాశి అక్షరం *బైట్లు, size_t len)
{
  size_t pos = 0;
  స్థిరరాశి అక్షరం *string_ప్రారంభం = ఖాళీ;  // init to avoid gcc warning

  vt->లోపల_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  ఎంపిక(vt->parser.state) {
  స్థితి సాధారణ_2:
  స్థితి CSI_LEADER:
  స్థితి CSI_వాదనలు:
  స్థితి CSI_INTERMED:
  స్థితి OSC_COMMAND:
  స్థితి DCS_COMMAND:
    string_ప్రారంభం = ఖాళీ;
    ఆపు;
  స్థితి OSC:
  స్థితి DCS:
  స్థితి APC:
  స్థితి PM:
  స్థితి SOS:
    string_ప్రారంభం = బైట్లు;
    ఆపు;
  }

#నిర్వచించు ENTER_STATE(st)        చేయి { vt->parser.state = st; string_ప్రారంభం = ఖాళీ; } అయితే(0)
#నిర్వచించు ENTER_సాధారణ_STATE()   ENTER_STATE(సాధారణ_2)

#నిర్వచించు IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  కొరకు( ; pos < len; pos++) {
    గుర్తులేని అక్షరం c = బైట్లు[pos];
    పూర్ణాంకం c1_allowed = !vt->బహుళకం.utf8;

    ఒకవేళ(c == 0x00 || c == 0x7f) { // NUL, DEL
      ఒకవేళ(IS_STRING_STATE()) {
        string_fragment(vt, string_ప్రారంభం, బైట్లు + pos - string_ప్రారంభం, FALSE);
        string_ప్రారంభం = బైట్లు + pos + 1;
      }
      ఒకవేళ(vt->parser.emit_nul)
        do_control(vt, c);
      కొనసాగు;
    }
    ఒకవేళ(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.లోపల_esc = FALSE;
      ENTER_సాధారణ_STATE();
      ఒకవేళ(vt->parser.emit_nul)
        do_control(vt, c);
      కొనసాగు;
    }
    లేకపోతే ఒకవేళ(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      ఒకవేళ(!IS_STRING_STATE())
        vt->parser.state = సాధారణ_2;
      vt->parser.లోపల_esc = TRUE;
      కొనసాగు;
    }
    లేకపోతే ఒకవేళ(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    లేకపోతే ఒకవేళ(c < 0x20) { // other C0
      ఒకవేళ(vt->parser.state == SOS)
        కొనసాగు; // All other C0s permitted in SOS

      ఒకవేళ(vterm_get_special_pty_type() == 2) {
        ఒకవేళ(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          ఒకవేళ(pos + 2 < len && బైట్లు[pos + 1] == 0x20 && బైట్లు[pos + 2] == 0x08)
            vt->లోపల_backspace = 2; // Trigger when count down to 1
      }
      ఒకవేళ(IS_STRING_STATE())
        string_fragment(vt, string_ప్రారంభం, బైట్లు + pos - string_ప్రారంభం, FALSE);
      do_control(vt, c);
      ఒకవేళ(IS_STRING_STATE())
        string_ప్రారంభం = బైట్లు + pos + 1;
      కొనసాగు;
    }
    // else fallthrough

    size_t string_len = బైట్లు + pos - string_ప్రారంభం;

    ఒకవేళ(vt->parser.లోపల_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      ఒకవేళ(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        ఒకవేళ(string_len)
          string_len -= 1;
        vt->parser.లోపల_esc = FALSE;
      }
      లేకపోతే {
        string_ప్రారంభం = ఖాళీ;
        vt->parser.state = సాధారణ_2;
      }
    }

    ఎంపిక(vt->parser.state) {
    స్థితి CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      ఒకవేళ(c >= 0x3c && c <= 0x3f) {
        ఒకవేళ(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        ఆపు;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.వాదనలు[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_వాదనలు;

      /* fallthrough */
    స్థితి CSI_వాదనలు:
      /* Numerical value of argument */
      ఒకవేళ(c >= '0' && c <= '9') {
        దీర్ఘ arg_max = CSI_ARG_MISSING - 1;
        దీర్ఘ *arg = &vt->parser.v.csi.వాదనలు[vt->parser.v.csi.argi];
        పూర్ణాంకం digit = c - '0';

        ఒకవేళ(*arg == CSI_ARG_MISSING)
          *arg = 0;
        ఒకవేళ(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        లేకపోతే
          *arg = *arg * 10 + digit;
        ఆపు;
      }
      ఒకవేళ(c == ':') {
        vt->parser.v.csi.వాదనలు[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_మరిన్ని;
        c = ';';
      }
      ఒకవేళ(c == ';') {
        ఒకవేళ(vt->parser.v.csi.argi >= CSI_వాదనలు_MAX - 1)
          ఆపు; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.వాదనలు[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        ఆపు;
      }

      /* else fallthrough */
      vt->parser.v.csi.argi++;
      vt->parser.intermedlen = 0;
      vt->parser.state = CSI_INTERMED;
      // FALLTHROUGH
    స్థితి CSI_INTERMED:
      ఒకవేళ(is_intermed(c)) {
        ఒకవేళ(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        ఆపు;
      }
      లేకపోతే ఒకవేళ(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      లేకపోతే ఒకవేళ(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_సాధారణ_STATE();
      ఆపు;

    స్థితి OSC_COMMAND:
      /* Numerical value of command */
      ఒకవేళ(c >= '0' && c <= '9') {
        ఒకవేళ(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        లేకపోతే
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        ఆపు;
      }
      ఒకవేళ(c == ';') {
        vt->parser.state = OSC;
        string_ప్రారంభం = బైట్లు + pos + 1;
        ఆపు;
      }

      /* else fallthrough */
      string_ప్రారంభం = బైట్లు + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      వెళ్ళు string_state;

    స్థితి DCS_COMMAND:
      ఒకవేళ(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      ఒకవేళ(c >= 0x40 && c<= 0x7e) {
        string_ప్రారంభం = బైట్లు + pos + 1;
        vt->parser.state = DCS;
      }
      ఆపు;

string_state:
    స్థితి OSC:
    స్థితి DCS:
    స్థితి APC:
    స్థితి PM:
    స్థితి SOS:
      ఒకవేళ(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_ప్రారంభం, string_len, TRUE);
        ENTER_సాధారణ_STATE();
      }
      ఆపు;

    స్థితి సాధారణ_2:
      ఒకవేళ(vt->parser.లోపల_esc) {
        ఒకవేళ(is_intermed(c)) {
          ఒకవేళ(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        లేకపోతే ఒకవేళ(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.లోపల_esc = 0;
          ENTER_సాధారణ_STATE();
        }
        లేకపోతే {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        ఆపు;
      }
      ఒకవేళ(c1_allowed && c >= 0x80 && c < 0xa0) {
        ఎంపిక(c) {
        స్థితి 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          ఆపు;
        స్థితి 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_ప్రారంభం = బైట్లు + pos + 1;
          string_len = 0;
          ఆపు;
        స్థితి 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          ఆపు;
        స్థితి 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_ప్రారంభం = బైట్లు + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          ఆపు;
        స్థితి 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_ప్రారంభం = బైట్లు + pos + 1;
          string_len = 0;
          ఆపు;
        స్థితి 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_ప్రారంభం = బైట్లు + pos + 1;
          string_len = 0;
          ఆపు;
        అప్రమేయ:
          do_control(vt, c);
          ఆపు;
        }
      }
      లేకపోతే {
        size_t eaten = 0;
        ఒకవేళ(vt->parser.callbacks && vt->parser.callbacks->పాఠ్యం)
          eaten = (*vt->parser.callbacks->పాఠ్యం)(బైట్లు + pos, len - pos, vt->parser.cbdata);

        ఒకవేళ(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      ఆపు;
    }
  }

  ఒకవేళ(string_ప్రారంభం) {
    size_t string_len = బైట్లు + pos - string_ప్రారంభం;
    ఒకవేళ(vt->parser.లోపల_esc)
      string_len -= 1;
    string_fragment(vt, string_ప్రారంభం, string_len, FALSE);
  }

  తిరిగి len;
}

శూన్య vterm_parser_సమితి_callbacks(VTerm *vt, స్థిరరాశి VTermParserCallbacks *callbacks, శూన్య *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

శూన్య *vterm_parser_get_cbdata(VTerm *vt)
{
  తిరిగి vt->parser.cbdata;
}

శూన్య vterm_parser_సమితి_emit_nul(VTerm *vt, పూర్ణాంకం emit)
{
  vt->parser.emit_nul = emit;
}
