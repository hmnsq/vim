#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEBUG_PARSER

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_intermed(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
  ಹಿಂತಿರುಗಿ c >= 0x20 && c <= 0x2f;
}

ಸ್ಥಿರ ಶೂನ್ಯ do_control(VTerm *vt, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ control)
{
  ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->control)
    ಆದರೆ((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      ಹಿಂತಿರುಗಿ;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

ಸ್ಥಿರ ಶೂನ್ಯ do_csi(VTerm *vt, ಅಕ್ಷರ command)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_PARSER
  ಬರೆ("Parsed CSI args as:\n", arglen, ವಾದಗಳು);
  ಬರೆ(" leader: %s\n", vt->parser.v.csi.leader);
  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    ಬರೆ(" %lu", CSI_ARG(vt->parser.v.csi.ವಾದಗಳು[argi]));
    ಆದರೆ(!CSI_ARG_HAS_ಇನ್ನಷ್ಟು(vt->parser.v.csi.ವಾದಗಳು[argi]))
      ಬರೆ("\n");
  ಬರೆ(" intermed: %s\n", vt->parser.intermed);
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->csi)
    ಆದರೆ((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : ಖಾಲಿ,
          vt->parser.v.csi.ವಾದಗಳು,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : ಖಾಲಿ,
          command,
          vt->parser.cbdata))
      ಹಿಂತಿರುಗಿ;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

ಸ್ಥಿರ ಶೂನ್ಯ do_escape(VTerm *vt, ಅಕ್ಷರ command)
{
  ಅಕ್ಷರ seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->escape)
    ಆದರೆ((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      ಹಿಂತಿರುಗಿ;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

ಸ್ಥಿರ ಶೂನ್ಯ string_fragment(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t len, ಪೂರ್ಣಾಂಕ final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  ಆಯ್ಕೆ(vt->parser.state) {
    ಸ್ಥಿತಿ OSC:
      ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ DCS:
      ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ APC:
      ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ PM:
      ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SOS:
      ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ಸಾಮಾನ್ಯ_2:
    ಸ್ಥಿತಿ CSI_LEADER:
    ಸ್ಥಿತಿ CSI_ವಾದಗಳು:
    ಸ್ಥಿತಿ CSI_INTERMED:
    ಸ್ಥಿತಿ OSC_COMMAND:
    ಸ್ಥಿತಿ DCS_COMMAND:
      ನಿಲ್ಲಿಸು;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_ಒಳಹರಿವು_write(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t len)
{
  size_t pos = 0;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string_ಆರಂಭ = ಖಾಲಿ;  // init to avoid gcc warning

  vt->ಒಳ_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  ಆಯ್ಕೆ(vt->parser.state) {
  ಸ್ಥಿತಿ ಸಾಮಾನ್ಯ_2:
  ಸ್ಥಿತಿ CSI_LEADER:
  ಸ್ಥಿತಿ CSI_ವಾದಗಳು:
  ಸ್ಥಿತಿ CSI_INTERMED:
  ಸ್ಥಿತಿ OSC_COMMAND:
  ಸ್ಥಿತಿ DCS_COMMAND:
    string_ಆರಂಭ = ಖಾಲಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ OSC:
  ಸ್ಥಿತಿ DCS:
  ಸ್ಥಿತಿ APC:
  ಸ್ಥಿತಿ PM:
  ಸ್ಥಿತಿ SOS:
    string_ಆರಂಭ = ಬೈಟುಗಳು;
    ನಿಲ್ಲಿಸು;
  }

#ವ್ಯಾಖ್ಯಾನಿಸು ENTER_STATE(st)        ಮಾಡು { vt->parser.state = st; string_ಆರಂಭ = ಖಾಲಿ; } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು ENTER_ಸಾಮಾನ್ಯ_STATE()   ENTER_STATE(ಸಾಮಾನ್ಯ_2)

#ವ್ಯಾಖ್ಯಾನಿಸು IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  ಗಾಗಿ( ; pos < len; pos++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = ಬೈಟುಗಳು[pos];
    ಪೂರ್ಣಾಂಕ c1_allowed = !vt->ಬಹುಲಕ.utf8;

    ಆದರೆ(c == 0x00 || c == 0x7f) { // NUL, DEL
      ಆದರೆ(IS_STRING_STATE()) {
        string_fragment(vt, string_ಆರಂಭ, ಬೈಟುಗಳು + pos - string_ಆರಂಭ, FALSE);
        string_ಆರಂಭ = ಬೈಟುಗಳು + pos + 1;
      }
      ಆದರೆ(vt->parser.emit_nul)
        do_control(vt, c);
      ಮುಂದುವರಿ;
    }
    ಆದರೆ(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.ಒಳ_esc = FALSE;
      ENTER_ಸಾಮಾನ್ಯ_STATE();
      ಆದರೆ(vt->parser.emit_nul)
        do_control(vt, c);
      ಮುಂದುವರಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      ಆದರೆ(!IS_STRING_STATE())
        vt->parser.state = ಸಾಮಾನ್ಯ_2;
      vt->parser.ಒಳ_esc = TRUE;
      ಮುಂದುವರಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c < 0x20) { // other C0
      ಆದರೆ(vt->parser.state == SOS)
        ಮುಂದುವರಿ; // All other C0s permitted in SOS

      ಆದರೆ(vterm_get_special_pty_type() == 2) {
        ಆದರೆ(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          ಆದರೆ(pos + 2 < len && ಬೈಟುಗಳು[pos + 1] == 0x20 && ಬೈಟುಗಳು[pos + 2] == 0x08)
            vt->ಒಳ_backspace = 2; // Trigger when count down to 1
      }
      ಆದರೆ(IS_STRING_STATE())
        string_fragment(vt, string_ಆರಂಭ, ಬೈಟುಗಳು + pos - string_ಆರಂಭ, FALSE);
      do_control(vt, c);
      ಆದರೆ(IS_STRING_STATE())
        string_ಆರಂಭ = ಬೈಟುಗಳು + pos + 1;
      ಮುಂದುವರಿ;
    }
    // else fallthrough

    size_t string_len = ಬೈಟುಗಳು + pos - string_ಆರಂಭ;

    ಆದರೆ(vt->parser.ಒಳ_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      ಆದರೆ(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        ಆದರೆ(string_len)
          string_len -= 1;
        vt->parser.ಒಳ_esc = FALSE;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        string_ಆರಂಭ = ಖಾಲಿ;
        vt->parser.state = ಸಾಮಾನ್ಯ_2;
      }
    }

    ಆಯ್ಕೆ(vt->parser.state) {
    ಸ್ಥಿತಿ CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      ಆದರೆ(c >= 0x3c && c <= 0x3f) {
        ಆದರೆ(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        ನಿಲ್ಲಿಸು;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.ವಾದಗಳು[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_ವಾದಗಳು;

      /* fallthrough */
    ಸ್ಥಿತಿ CSI_ವಾದಗಳು:
      /* Numerical value of argument */
      ಆದರೆ(c >= '0' && c <= '9') {
        ದೀರ್ಘ arg_max = CSI_ARG_MISSING - 1;
        ದೀರ್ಘ *arg = &vt->parser.v.csi.ವಾದಗಳು[vt->parser.v.csi.argi];
        ಪೂರ್ಣಾಂಕ digit = c - '0';

        ಆದರೆ(*arg == CSI_ARG_MISSING)
          *arg = 0;
        ಆದರೆ(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        ಇಲ್ಲದಿದ್ದರೆ
          *arg = *arg * 10 + digit;
        ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(c == ':') {
        vt->parser.v.csi.ವಾದಗಳು[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_ಇನ್ನಷ್ಟು;
        c = ';';
      }
      ಆದರೆ(c == ';') {
        ಆದರೆ(vt->parser.v.csi.argi >= CSI_ವಾದಗಳು_MAX - 1)
          ನಿಲ್ಲಿಸು; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.ವಾದಗಳು[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        ನಿಲ್ಲಿಸು;
      }

      /* else fallthrough */
      vt->parser.v.csi.argi++;
      vt->parser.intermedlen = 0;
      vt->parser.state = CSI_INTERMED;
      // FALLTHROUGH
    ಸ್ಥಿತಿ CSI_INTERMED:
      ಆದರೆ(is_intermed(c)) {
        ಆದರೆ(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_ಸಾಮಾನ್ಯ_STATE();
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ OSC_COMMAND:
      /* Numerical value of command */
      ಆದರೆ(c >= '0' && c <= '9') {
        ಆದರೆ(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        ಇಲ್ಲದಿದ್ದರೆ
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(c == ';') {
        vt->parser.state = OSC;
        string_ಆರಂಭ = ಬೈಟುಗಳು + pos + 1;
        ನಿಲ್ಲಿಸು;
      }

      /* else fallthrough */
      string_ಆರಂಭ = ಬೈಟುಗಳು + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      ಹೋಗು string_state;

    ಸ್ಥಿತಿ DCS_COMMAND:
      ಆದರೆ(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      ಆದರೆ(c >= 0x40 && c<= 0x7e) {
        string_ಆರಂಭ = ಬೈಟುಗಳು + pos + 1;
        vt->parser.state = DCS;
      }
      ನಿಲ್ಲಿಸು;

string_state:
    ಸ್ಥಿತಿ OSC:
    ಸ್ಥಿತಿ DCS:
    ಸ್ಥಿತಿ APC:
    ಸ್ಥಿತಿ PM:
    ಸ್ಥಿತಿ SOS:
      ಆದರೆ(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_ಆರಂಭ, string_len, TRUE);
        ENTER_ಸಾಮಾನ್ಯ_STATE();
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ಸಾಮಾನ್ಯ_2:
      ಆದರೆ(vt->parser.ಒಳ_esc) {
        ಆದರೆ(is_intermed(c)) {
          ಆದರೆ(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.ಒಳ_esc = 0;
          ENTER_ಸಾಮಾನ್ಯ_STATE();
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(c1_allowed && c >= 0x80 && c < 0xa0) {
        ಆಯ್ಕೆ(c) {
        ಸ್ಥಿತಿ 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_ಆರಂಭ = ಬೈಟುಗಳು + pos + 1;
          string_len = 0;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_ಆರಂಭ = ಬೈಟುಗಳು + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_ಆರಂಭ = ಬೈಟುಗಳು + pos + 1;
          string_len = 0;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_ಆರಂಭ = ಬೈಟುಗಳು + pos + 1;
          string_len = 0;
          ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
          do_control(vt, c);
          ನಿಲ್ಲಿಸು;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        size_t eaten = 0;
        ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->ಪಠ್ಯ)
          eaten = (*vt->parser.callbacks->ಪಠ್ಯ)(ಬೈಟುಗಳು + pos, len - pos, vt->parser.cbdata);

        ಆದರೆ(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(string_ಆರಂಭ) {
    size_t string_len = ಬೈಟುಗಳು + pos - string_ಆರಂಭ;
    ಆದರೆ(vt->parser.ಒಳ_esc)
      string_len -= 1;
    string_fragment(vt, string_ಆರಂಭ, string_len, FALSE);
  }

  ಹಿಂತಿರುಗಿ len;
}

ಶೂನ್ಯ vterm_parser_ಗಣ_callbacks(VTerm *vt, ಸ್ಥಿರಾಂಕ VTermParserCallbacks *callbacks, ಶೂನ್ಯ *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

ಶೂನ್ಯ *vterm_parser_get_cbdata(VTerm *vt)
{
  ಹಿಂತಿರುಗಿ vt->parser.cbdata;
}

ಶೂನ್ಯ vterm_parser_ಗಣ_emit_nul(VTerm *vt, ಪೂರ್ಣಾಂಕ emit)
{
  vt->parser.emit_nul = emit;
}
