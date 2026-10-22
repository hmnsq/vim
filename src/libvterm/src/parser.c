#شامل "vterm_internal.h"

#شامل <stdio.h>
#شامل <string.h>

#undef DEBUG_PARSER

ایستا صحیح is_intermed(unsigned نویسه c)
{
  بازگشت c >= 0x20 && c <= 0x2f;
}

ایستا تهی do_control(VTerm *vt, unsigned نویسه control)
{
  اگر(vt->parser.callbacks && vt->parser.callbacks->control)
    اگر((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      بازگشت;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

ایستا تهی do_csi(VTerm *vt, نویسه command)
{
#ifdef DEBUG_PARSER
  printf("Parsed CSI args as:\n", arglen, args);
  printf(" leader: %s\n", vt->parser.v.csi.leader);
  برای(صحیح argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    printf(" %lu", CSI_ARG(vt->parser.v.csi.args[argi]));
    اگر(!CSI_ARG_HAS_MORE(vt->parser.v.csi.args[argi]))
      printf("\n");
  printf(" intermed: %s\n", vt->parser.intermed);
  }
#endif

  اگر(vt->parser.callbacks && vt->parser.callbacks->csi)
    اگر((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : پوچ,
          vt->parser.v.csi.args,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : پوچ,
          command,
          vt->parser.cbdata))
      بازگشت;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

ایستا تهی do_escape(VTerm *vt, نویسه command)
{
  نویسه seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  اگر(vt->parser.callbacks && vt->parser.callbacks->escape)
    اگر((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      بازگشت;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

ایستا تهی string_fragment(VTerm *vt, ثابت نویسه *str, size_t len, صحیح final)
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
      بشکن;

    حالت DCS:
      اگر(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      بشکن;

    حالت APC:
      اگر(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      بشکن;

    حالت PM:
      اگر(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      بشکن;

    حالت SOS:
      اگر(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      بشکن;

    حالت NORMAL:
    حالت CSI_LEADER:
    حالت CSI_ARGS:
    حالت CSI_INTERMED:
    حالت OSC_COMMAND:
    حالت DCS_COMMAND:
      بشکن;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_درون_داد_write(VTerm *vt, ثابت نویسه *bytes, size_t len)
{
  size_t pos = 0;
  ثابت نویسه *string_start = پوچ;  // init to avoid gcc warning

  vt->in_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  انتخاب(vt->parser.state) {
  حالت NORMAL:
  حالت CSI_LEADER:
  حالت CSI_ARGS:
  حالت CSI_INTERMED:
  حالت OSC_COMMAND:
  حالت DCS_COMMAND:
    string_start = پوچ;
    بشکن;
  حالت OSC:
  حالت DCS:
  حالت APC:
  حالت PM:
  حالت SOS:
    string_start = bytes;
    بشکن;
  }

#define ENTER_STATE(st)        انجامبده { vt->parser.state = st; string_start = پوچ; } وقتی(0)
#define ENTER_NORMAL_STATE()   ENTER_STATE(NORMAL)

#define IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  برای( ; pos < len; pos++) {
    unsigned نویسه c = bytes[pos];
    صحیح c1_allowed = !vt->mode.utf8;

    اگر(c == 0x00 || c == 0x7f) { // NUL, DEL
      اگر(IS_STRING_STATE()) {
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
        string_start = bytes + pos + 1;
      }
      اگر(vt->parser.emit_nul)
        do_control(vt, c);
      ادامه;
    }
    اگر(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.in_esc = FALSE;
      ENTER_NORMAL_STATE();
      اگر(vt->parser.emit_nul)
        do_control(vt, c);
      ادامه;
    }
    وگرنه اگر(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      اگر(!IS_STRING_STATE())
        vt->parser.state = NORMAL;
      vt->parser.in_esc = TRUE;
      ادامه;
    }
    وگرنه اگر(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    وگرنه اگر(c < 0x20) { // other C0
      اگر(vt->parser.state == SOS)
        ادامه; // All other C0s permitted in SOS

      اگر(vterm_get_special_pty_type() == 2) {
        اگر(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          اگر(pos + 2 < len && bytes[pos + 1] == 0x20 && bytes[pos + 2] == 0x08)
            vt->in_backspace = 2; // Trigger when count down to 1
      }
      اگر(IS_STRING_STATE())
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
      do_control(vt, c);
      اگر(IS_STRING_STATE())
        string_start = bytes + pos + 1;
      ادامه;
    }
    // else fallthrough

    size_t string_len = bytes + pos - string_start;

    اگر(vt->parser.in_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      اگر(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        اگر(string_len)
          string_len -= 1;
        vt->parser.in_esc = FALSE;
      }
      وگرنه {
        string_start = پوچ;
        vt->parser.state = NORMAL;
      }
    }

    انتخاب(vt->parser.state) {
    حالت CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      اگر(c >= 0x3c && c <= 0x3f) {
        اگر(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        بشکن;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.args[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_ARGS;

      /* fallthrough */
    حالت CSI_ARGS:
      /* Numerical value of argument */
      اگر(c >= '0' && c <= '9') {
        long arg_max = CSI_ARG_MISSING - 1;
        long *arg = &vt->parser.v.csi.args[vt->parser.v.csi.argi];
        صحیح digit = c - '0';

        اگر(*arg == CSI_ARG_MISSING)
          *arg = 0;
        اگر(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        وگرنه
          *arg = *arg * 10 + digit;
        بشکن;
      }
      اگر(c == ':') {
        vt->parser.v.csi.args[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_MORE;
        c = ';';
      }
      اگر(c == ';') {
        اگر(vt->parser.v.csi.argi >= CSI_ARGS_MAX - 1)
          بشکن; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.args[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        بشکن;
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
        بشکن;
      }
      وگرنه اگر(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      وگرنه اگر(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_NORMAL_STATE();
      بشکن;

    حالت OSC_COMMAND:
      /* Numerical value of command */
      اگر(c >= '0' && c <= '9') {
        اگر(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        وگرنه
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        بشکن;
      }
      اگر(c == ';') {
        vt->parser.state = OSC;
        string_start = bytes + pos + 1;
        بشکن;
      }

      /* else fallthrough */
      string_start = bytes + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      برو string_state;

    حالت DCS_COMMAND:
      اگر(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      اگر(c >= 0x40 && c<= 0x7e) {
        string_start = bytes + pos + 1;
        vt->parser.state = DCS;
      }
      بشکن;

string_state:
    حالت OSC:
    حالت DCS:
    حالت APC:
    حالت PM:
    حالت SOS:
      اگر(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_start, string_len, TRUE);
        ENTER_NORMAL_STATE();
      }
      بشکن;

    حالت NORMAL:
      اگر(vt->parser.in_esc) {
        اگر(is_intermed(c)) {
          اگر(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        وگرنه اگر(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.in_esc = 0;
          ENTER_NORMAL_STATE();
        }
        وگرنه {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        بشکن;
      }
      اگر(c1_allowed && c >= 0x80 && c < 0xa0) {
        انتخاب(c) {
        حالت 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          بشکن;
        حالت 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_start = bytes + pos + 1;
          string_len = 0;
          بشکن;
        حالت 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          بشکن;
        حالت 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_start = bytes + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          بشکن;
        حالت 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_start = bytes + pos + 1;
          string_len = 0;
          بشکن;
        حالت 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_start = bytes + pos + 1;
          string_len = 0;
          بشکن;
        پیشفرض:
          do_control(vt, c);
          بشکن;
        }
      }
      وگرنه {
        size_t eaten = 0;
        اگر(vt->parser.callbacks && vt->parser.callbacks->text)
          eaten = (*vt->parser.callbacks->text)(bytes + pos, len - pos, vt->parser.cbdata);

        اگر(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      بشکن;
    }
  }

  اگر(string_start) {
    size_t string_len = bytes + pos - string_start;
    اگر(vt->parser.in_esc)
      string_len -= 1;
    string_fragment(vt, string_start, string_len, FALSE);
  }

  بازگشت len;
}

تهی vterm_parser_set_callbacks(VTerm *vt, ثابت VTermParserCallbacks *callbacks, تهی *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

تهی *vterm_parser_get_cbdata(VTerm *vt)
{
  بازگشت vt->parser.cbdata;
}

تهی vterm_parser_set_emit_nul(VTerm *vt, صحیح emit)
{
  vt->parser.emit_nul = emit;
}
