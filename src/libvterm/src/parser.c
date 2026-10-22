#અન્તર્ભાવ "vterm_internal.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#undef DEBUG_PARSER

સ્થિર પૂર્ણાંક is_intermed(unsigned અક્ષર c)
{
  પાછા c >= 0x20 && c <= 0x2f;
}

સ્થિર શૂન્ય do_control(VTerm *vt, unsigned અક્ષર control)
{
  જો(vt->parser.callbacks && vt->parser.callbacks->control)
    જો((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      પાછા;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

સ્થિર શૂન્ય do_csi(VTerm *vt, અક્ષર command)
{
#ifdef DEBUG_PARSER
  printf("Parsed CSI args as:\n", arglen, args);
  printf(" leader: %s\n", vt->parser.v.csi.leader);
  માટે(પૂર્ણાંક argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    printf(" %lu", CSI_ARG(vt->parser.v.csi.args[argi]));
    જો(!CSI_ARG_HAS_MORE(vt->parser.v.csi.args[argi]))
      printf("\n");
  printf(" intermed: %s\n", vt->parser.intermed);
  }
#endif

  જો(vt->parser.callbacks && vt->parser.callbacks->csi)
    જો((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : ખાલી,
          vt->parser.v.csi.args,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : ખાલી,
          command,
          vt->parser.cbdata))
      પાછા;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

સ્થિર શૂન્ય do_escape(VTerm *vt, અક્ષર command)
{
  અક્ષર seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  જો(vt->parser.callbacks && vt->parser.callbacks->escape)
    જો((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      પાછા;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

સ્થિર શૂન્ય string_fragment(VTerm *vt, અચલ અક્ષર *str, size_t len, પૂર્ણાંક final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  પસંદગી(vt->parser.state) {
    સ્થિતિ OSC:
      જો(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      તોડો;

    સ્થિતિ DCS:
      જો(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      તોડો;

    સ્થિતિ APC:
      જો(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      તોડો;

    સ્થિતિ PM:
      જો(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      તોડો;

    સ્થિતિ SOS:
      જો(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      તોડો;

    સ્થિતિ NORMAL:
    સ્થિતિ CSI_LEADER:
    સ્થિતિ CSI_ARGS:
    સ્થિતિ CSI_INTERMED:
    સ્થિતિ OSC_COMMAND:
    સ્થિતિ DCS_COMMAND:
      તોડો;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_આગત_write(VTerm *vt, અચલ અક્ષર *bytes, size_t len)
{
  size_t pos = 0;
  અચલ અક્ષર *string_start = ખાલી;  // init to avoid gcc warning

  vt->in_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  પસંદગી(vt->parser.state) {
  સ્થિતિ NORMAL:
  સ્થિતિ CSI_LEADER:
  સ્થિતિ CSI_ARGS:
  સ્થિતિ CSI_INTERMED:
  સ્થિતિ OSC_COMMAND:
  સ્થિતિ DCS_COMMAND:
    string_start = ખાલી;
    તોડો;
  સ્થિતિ OSC:
  સ્થિતિ DCS:
  સ્થિતિ APC:
  સ્થિતિ PM:
  સ્થિતિ SOS:
    string_start = bytes;
    તોડો;
  }

#define ENTER_STATE(st)        કરો { vt->parser.state = st; string_start = ખાલી; } જ્યારે(0)
#define ENTER_NORMAL_STATE()   ENTER_STATE(NORMAL)

#define IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  માટે( ; pos < len; pos++) {
    unsigned અક્ષર c = bytes[pos];
    પૂર્ણાંક c1_allowed = !vt->mode.utf8;

    જો(c == 0x00 || c == 0x7f) { // NUL, DEL
      જો(IS_STRING_STATE()) {
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
        string_start = bytes + pos + 1;
      }
      જો(vt->parser.emit_nul)
        do_control(vt, c);
      ચાલુ;
    }
    જો(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.in_esc = FALSE;
      ENTER_NORMAL_STATE();
      જો(vt->parser.emit_nul)
        do_control(vt, c);
      ચાલુ;
    }
    નહીંતર જો(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      જો(!IS_STRING_STATE())
        vt->parser.state = NORMAL;
      vt->parser.in_esc = TRUE;
      ચાલુ;
    }
    નહીંતર જો(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    નહીંતર જો(c < 0x20) { // other C0
      જો(vt->parser.state == SOS)
        ચાલુ; // All other C0s permitted in SOS

      જો(vterm_get_special_pty_type() == 2) {
        જો(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          જો(pos + 2 < len && bytes[pos + 1] == 0x20 && bytes[pos + 2] == 0x08)
            vt->in_backspace = 2; // Trigger when count down to 1
      }
      જો(IS_STRING_STATE())
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
      do_control(vt, c);
      જો(IS_STRING_STATE())
        string_start = bytes + pos + 1;
      ચાલુ;
    }
    // else fallthrough

    size_t string_len = bytes + pos - string_start;

    જો(vt->parser.in_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      જો(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        જો(string_len)
          string_len -= 1;
        vt->parser.in_esc = FALSE;
      }
      નહીંતર {
        string_start = ખાલી;
        vt->parser.state = NORMAL;
      }
    }

    પસંદગી(vt->parser.state) {
    સ્થિતિ CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      જો(c >= 0x3c && c <= 0x3f) {
        જો(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        તોડો;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.args[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_ARGS;

      /* fallthrough */
    સ્થિતિ CSI_ARGS:
      /* Numerical value of argument */
      જો(c >= '0' && c <= '9') {
        long arg_max = CSI_ARG_MISSING - 1;
        long *arg = &vt->parser.v.csi.args[vt->parser.v.csi.argi];
        પૂર્ણાંક digit = c - '0';

        જો(*arg == CSI_ARG_MISSING)
          *arg = 0;
        જો(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        નહીંતર
          *arg = *arg * 10 + digit;
        તોડો;
      }
      જો(c == ':') {
        vt->parser.v.csi.args[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_MORE;
        c = ';';
      }
      જો(c == ';') {
        જો(vt->parser.v.csi.argi >= CSI_ARGS_MAX - 1)
          તોડો; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.args[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        તોડો;
      }

      /* else fallthrough */
      vt->parser.v.csi.argi++;
      vt->parser.intermedlen = 0;
      vt->parser.state = CSI_INTERMED;
      // FALLTHROUGH
    સ્થિતિ CSI_INTERMED:
      જો(is_intermed(c)) {
        જો(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        તોડો;
      }
      નહીંતર જો(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      નહીંતર જો(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_NORMAL_STATE();
      તોડો;

    સ્થિતિ OSC_COMMAND:
      /* Numerical value of command */
      જો(c >= '0' && c <= '9') {
        જો(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        નહીંતર
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        તોડો;
      }
      જો(c == ';') {
        vt->parser.state = OSC;
        string_start = bytes + pos + 1;
        તોડો;
      }

      /* else fallthrough */
      string_start = bytes + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      જાઓ string_state;

    સ્થિતિ DCS_COMMAND:
      જો(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      જો(c >= 0x40 && c<= 0x7e) {
        string_start = bytes + pos + 1;
        vt->parser.state = DCS;
      }
      તોડો;

string_state:
    સ્થિતિ OSC:
    સ્થિતિ DCS:
    સ્થિતિ APC:
    સ્થિતિ PM:
    સ્થિતિ SOS:
      જો(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_start, string_len, TRUE);
        ENTER_NORMAL_STATE();
      }
      તોડો;

    સ્થિતિ NORMAL:
      જો(vt->parser.in_esc) {
        જો(is_intermed(c)) {
          જો(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        નહીંતર જો(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.in_esc = 0;
          ENTER_NORMAL_STATE();
        }
        નહીંતર {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        તોડો;
      }
      જો(c1_allowed && c >= 0x80 && c < 0xa0) {
        પસંદગી(c) {
        સ્થિતિ 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          તોડો;
        સ્થિતિ 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_start = bytes + pos + 1;
          string_len = 0;
          તોડો;
        સ્થિતિ 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          તોડો;
        સ્થિતિ 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_start = bytes + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          તોડો;
        સ્થિતિ 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_start = bytes + pos + 1;
          string_len = 0;
          તોડો;
        સ્થિતિ 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_start = bytes + pos + 1;
          string_len = 0;
          તોડો;
        મૂળભૂત:
          do_control(vt, c);
          તોડો;
        }
      }
      નહીંતર {
        size_t eaten = 0;
        જો(vt->parser.callbacks && vt->parser.callbacks->text)
          eaten = (*vt->parser.callbacks->text)(bytes + pos, len - pos, vt->parser.cbdata);

        જો(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      તોડો;
    }
  }

  જો(string_start) {
    size_t string_len = bytes + pos - string_start;
    જો(vt->parser.in_esc)
      string_len -= 1;
    string_fragment(vt, string_start, string_len, FALSE);
  }

  પાછા len;
}

શૂન્ય vterm_parser_set_callbacks(VTerm *vt, અચલ VTermParserCallbacks *callbacks, શૂન્ય *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

શૂન્ય *vterm_parser_get_cbdata(VTerm *vt)
{
  પાછા vt->parser.cbdata;
}

શૂન્ય vterm_parser_set_emit_nul(VTerm *vt, પૂર્ણાંક emit)
{
  vt->parser.emit_nul = emit;
}
