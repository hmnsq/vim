#ضمن "vterm_internal.h"

#ضمن <stdio.h>
#ضمن <string.h>

#undef DEBUG_PARSER

ساكن صحيح is_intermed(unsigned حرف c)
{
  החזר c >= 0x20 && c <= 0x2f;
}

ساكن فراغ do_control(VTerm *vt, unsigned حرف control)
{
  אם(vt->parser.callbacks && vt->parser.callbacks->control)
    אם((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      החזר;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

ساكن فراغ do_csi(VTerm *vt, حرف command)
{
#ifdef DEBUG_PARSER
  printf("Parsed CSI args as:\n", arglen, args);
  printf(" leader: %s\n", vt->parser.v.csi.leader);
  עבור(صحيح argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    printf(" %lu", CSI_ARG(vt->parser.v.csi.args[argi]));
    אם(!CSI_ARG_HAS_MORE(vt->parser.v.csi.args[argi]))
      printf("\n");
  printf(" intermed: %s\n", vt->parser.intermed);
  }
#endif

  אם(vt->parser.callbacks && vt->parser.callbacks->csi)
    אם((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : عدم,
          vt->parser.v.csi.args,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : عدم,
          command,
          vt->parser.cbdata))
      החזר;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

ساكن فراغ do_escape(VTerm *vt, حرف command)
{
  حرف seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  אם(vt->parser.callbacks && vt->parser.callbacks->escape)
    אם((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      החזר;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

ساكن فراغ string_fragment(VTerm *vt, ثابت حرف *str, size_t len, صحيح final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  بدل(vt->parser.state) {
    حالة OSC:
      אם(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      اكسر;

    حالة DCS:
      אם(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      اكسر;

    حالة APC:
      אם(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      اكسر;

    حالة PM:
      אם(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      اكسر;

    حالة SOS:
      אם(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      اكسر;

    حالة NORMAL:
    حالة CSI_LEADER:
    حالة CSI_ARGS:
    حالة CSI_INTERMED:
    حالة OSC_COMMAND:
    حالة DCS_COMMAND:
      اكسر;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_input_write(VTerm *vt, ثابت حرف *bytes, size_t len)
{
  size_t pos = 0;
  ثابت حرف *string_start = عدم;  // init to avoid gcc warning

  vt->in_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  بدل(vt->parser.state) {
  حالة NORMAL:
  حالة CSI_LEADER:
  حالة CSI_ARGS:
  حالة CSI_INTERMED:
  حالة OSC_COMMAND:
  حالة DCS_COMMAND:
    string_start = عدم;
    اكسر;
  حالة OSC:
  حالة DCS:
  حالة APC:
  حالة PM:
  حالة SOS:
    string_start = bytes;
    اكسر;
  }

#define ENTER_STATE(st)        עשה { vt->parser.state = st; string_start = عدم; } כלעוד(0)
#define ENTER_NORMAL_STATE()   ENTER_STATE(NORMAL)

#define IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  עבור( ; pos < len; pos++) {
    unsigned حرف c = bytes[pos];
    صحيح c1_allowed = !vt->mode.utf8;

    אם(c == 0x00 || c == 0x7f) { // NUL, DEL
      אם(IS_STRING_STATE()) {
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
        string_start = bytes + pos + 1;
      }
      אם(vt->parser.emit_nul)
        do_control(vt, c);
      استمر;
    }
    אם(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.in_esc = FALSE;
      ENTER_NORMAL_STATE();
      אם(vt->parser.emit_nul)
        do_control(vt, c);
      استمر;
    }
    אחרת אם(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      אם(!IS_STRING_STATE())
        vt->parser.state = NORMAL;
      vt->parser.in_esc = TRUE;
      استمر;
    }
    אחרת אם(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    אחרת אם(c < 0x20) { // other C0
      אם(vt->parser.state == SOS)
        استمر; // All other C0s permitted in SOS

      אם(vterm_get_special_pty_type() == 2) {
        אם(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          אם(pos + 2 < len && bytes[pos + 1] == 0x20 && bytes[pos + 2] == 0x08)
            vt->in_backspace = 2; // Trigger when count down to 1
      }
      אם(IS_STRING_STATE())
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
      do_control(vt, c);
      אם(IS_STRING_STATE())
        string_start = bytes + pos + 1;
      استمر;
    }
    // else fallthrough

    size_t string_len = bytes + pos - string_start;

    אם(vt->parser.in_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      אם(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        אם(string_len)
          string_len -= 1;
        vt->parser.in_esc = FALSE;
      }
      אחרת {
        string_start = عدم;
        vt->parser.state = NORMAL;
      }
    }

    بدل(vt->parser.state) {
    حالة CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      אם(c >= 0x3c && c <= 0x3f) {
        אם(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        اكسر;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.args[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_ARGS;

      /* fallthrough */
    حالة CSI_ARGS:
      /* Numerical value of argument */
      אם(c >= '0' && c <= '9') {
        long arg_max = CSI_ARG_MISSING - 1;
        long *arg = &vt->parser.v.csi.args[vt->parser.v.csi.argi];
        صحيح digit = c - '0';

        אם(*arg == CSI_ARG_MISSING)
          *arg = 0;
        אם(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        אחרת
          *arg = *arg * 10 + digit;
        اكسر;
      }
      אם(c == ':') {
        vt->parser.v.csi.args[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_MORE;
        c = ';';
      }
      אם(c == ';') {
        אם(vt->parser.v.csi.argi >= CSI_ARGS_MAX - 1)
          اكسر; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.args[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        اكسر;
      }

      /* else fallthrough */
      vt->parser.v.csi.argi++;
      vt->parser.intermedlen = 0;
      vt->parser.state = CSI_INTERMED;
      // FALLTHROUGH
    حالة CSI_INTERMED:
      אם(is_intermed(c)) {
        אם(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        اكسر;
      }
      אחרת אם(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      אחרת אם(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_NORMAL_STATE();
      اكسر;

    حالة OSC_COMMAND:
      /* Numerical value of command */
      אם(c >= '0' && c <= '9') {
        אם(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        אחרת
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        اكسر;
      }
      אם(c == ';') {
        vt->parser.state = OSC;
        string_start = bytes + pos + 1;
        اكسر;
      }

      /* else fallthrough */
      string_start = bytes + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      اذهب string_state;

    حالة DCS_COMMAND:
      אם(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      אם(c >= 0x40 && c<= 0x7e) {
        string_start = bytes + pos + 1;
        vt->parser.state = DCS;
      }
      اكسر;

string_state:
    حالة OSC:
    حالة DCS:
    حالة APC:
    حالة PM:
    حالة SOS:
      אם(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_start, string_len, TRUE);
        ENTER_NORMAL_STATE();
      }
      اكسر;

    حالة NORMAL:
      אם(vt->parser.in_esc) {
        אם(is_intermed(c)) {
          אם(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        אחרת אם(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.in_esc = 0;
          ENTER_NORMAL_STATE();
        }
        אחרת {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        اكسر;
      }
      אם(c1_allowed && c >= 0x80 && c < 0xa0) {
        بدل(c) {
        حالة 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          اكسر;
        حالة 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_start = bytes + pos + 1;
          string_len = 0;
          اكسر;
        حالة 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          اكسر;
        حالة 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_start = bytes + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          اكسر;
        حالة 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_start = bytes + pos + 1;
          string_len = 0;
          اكسر;
        حالة 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_start = bytes + pos + 1;
          string_len = 0;
          اكسر;
        افتراضي:
          do_control(vt, c);
          اكسر;
        }
      }
      אחרת {
        size_t eaten = 0;
        אם(vt->parser.callbacks && vt->parser.callbacks->text)
          eaten = (*vt->parser.callbacks->text)(bytes + pos, len - pos, vt->parser.cbdata);

        אם(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      اكسر;
    }
  }

  אם(string_start) {
    size_t string_len = bytes + pos - string_start;
    אם(vt->parser.in_esc)
      string_len -= 1;
    string_fragment(vt, string_start, string_len, FALSE);
  }

  החזר len;
}

فراغ vterm_parser_set_callbacks(VTerm *vt, ثابت VTermParserCallbacks *callbacks, فراغ *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

فراغ *vterm_parser_get_cbdata(VTerm *vt)
{
  החזר vt->parser.cbdata;
}

فراغ vterm_parser_set_emit_nul(VTerm *vt, صحيح emit)
{
  vt->parser.emit_nul = emit;
}
