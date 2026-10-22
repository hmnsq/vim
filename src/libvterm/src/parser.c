#include "vterm_internal.h"

#include <stdio.h>
#include <string.h>

#undef DEBUG_PARSER

static පූර්ණ_සංඛ්‍යා is_intermed(unsigned var_na c)
{
  ආපසු c >= 0x20 && c <= 0x2f;
}

static void do_control(VTerm *vt, unsigned var_na control)
{
  නම්(vt->parser.callbacks && vt->parser.callbacks->control)
    නම්((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      ආපසු;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

static void do_csi(VTerm *vt, var_na command)
{
#ifdef DEBUG_PARSER
  printf("Parsed CSI args as:\n", arglen, args);
  printf(" leader: %s\n", vt->parser.v.csi.leader);
  සඳහා(පූර්ණ_සංඛ්‍යා argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    printf(" %lu", CSI_ARG(vt->parser.v.csi.args[argi]));
    නම්(!CSI_ARG_HAS_MORE(vt->parser.v.csi.args[argi]))
      printf("\n");
  printf(" intermed: %s\n", vt->parser.intermed);
  }
#endif

  නම්(vt->parser.callbacks && vt->parser.callbacks->csi)
    නම්((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : NULL,
          vt->parser.v.csi.args,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : NULL,
          command,
          vt->parser.cbdata))
      ආපසු;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

static void do_escape(VTerm *vt, var_na command)
{
  var_na seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  නම්(vt->parser.callbacks && vt->parser.callbacks->escape)
    නම්((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      ආපසු;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

static void string_fragment(VTerm *vt, const var_na *str, size_t len, පූර්ණ_සංඛ්‍යා final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  switch(vt->parser.state) {
    pak_sa OSC:
      නම්(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      bha_nga;

    pak_sa DCS:
      නම්(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      bha_nga;

    pak_sa APC:
      නම්(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      bha_nga;

    pak_sa PM:
      නම්(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      bha_nga;

    pak_sa SOS:
      නම්(vt->parser.callbacks && vt->parser.callbacks->sos)
        (*vt->parser.callbacks->sos)(frag, vt->parser.cbdata);
      bha_nga;

    pak_sa NORMAL:
    pak_sa CSI_LEADER:
    pak_sa CSI_ARGS:
    pak_sa CSI_INTERMED:
    pak_sa OSC_COMMAND:
    pak_sa DCS_COMMAND:
      bha_nga;
  }

  vt->parser.string_initial = FALSE;
}

size_t vterm_input_write(VTerm *vt, const var_na *bytes, size_t len)
{
  size_t pos = 0;
  const var_na *string_start = NULL;  // init to avoid gcc warning

  vt->in_backspace = 0;		    // Count down with BS key and activate when
				    // it reaches 1

  switch(vt->parser.state) {
  pak_sa NORMAL:
  pak_sa CSI_LEADER:
  pak_sa CSI_ARGS:
  pak_sa CSI_INTERMED:
  pak_sa OSC_COMMAND:
  pak_sa DCS_COMMAND:
    string_start = NULL;
    bha_nga;
  pak_sa OSC:
  pak_sa DCS:
  pak_sa APC:
  pak_sa PM:
  pak_sa SOS:
    string_start = bytes;
    bha_nga;
  }

#define ENTER_STATE(st)        do { vt->parser.state = st; string_start = NULL; } අතරතුර(0)
#define ENTER_NORMAL_STATE()   ENTER_STATE(NORMAL)

#define IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  සඳහා( ; pos < len; pos++) {
    unsigned var_na c = bytes[pos];
    පූර්ණ_සංඛ්‍යා c1_allowed = !vt->mode.utf8;

    නම්(c == 0x00 || c == 0x7f) { // NUL, DEL
      නම්(IS_STRING_STATE()) {
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
        string_start = bytes + pos + 1;
      }
      නම්(vt->parser.emit_nul)
        do_control(vt, c);
      continue;
    }
    නම්(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.in_esc = FALSE;
      ENTER_NORMAL_STATE();
      නම්(vt->parser.emit_nul)
        do_control(vt, c);
      continue;
    }
    නැතහොත් නම්(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      නම්(!IS_STRING_STATE())
        vt->parser.state = NORMAL;
      vt->parser.in_esc = TRUE;
      continue;
    }
    නැතහොත් නම්(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    නැතහොත් නම්(c < 0x20) { // other C0
      නම්(vt->parser.state == SOS)
        continue; // All other C0s permitted in SOS

      නම්(vterm_get_special_pty_type() == 2) {
        නම්(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          නම්(pos + 2 < len && bytes[pos + 1] == 0x20 && bytes[pos + 2] == 0x08)
            vt->in_backspace = 2; // Trigger when count down to 1
      }
      නම්(IS_STRING_STATE())
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
      do_control(vt, c);
      නම්(IS_STRING_STATE())
        string_start = bytes + pos + 1;
      continue;
    }
    // else fallthrough

    size_t string_len = bytes + pos - string_start;

    නම්(vt->parser.in_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      නම්(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        නම්(string_len)
          string_len -= 1;
        vt->parser.in_esc = FALSE;
      }
      නැතහොත් {
        string_start = NULL;
        vt->parser.state = NORMAL;
      }
    }

    switch(vt->parser.state) {
    pak_sa CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      නම්(c >= 0x3c && c <= 0x3f) {
        නම්(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
          vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen++] = c;
        bha_nga;
      }

      /* else fallthrough */
      vt->parser.v.csi.leader[vt->parser.v.csi.leaderlen] = 0;

      vt->parser.v.csi.argi = 0;
      vt->parser.v.csi.args[0] = CSI_ARG_MISSING;
      vt->parser.state = CSI_ARGS;

      /* fallthrough */
    pak_sa CSI_ARGS:
      /* Numerical value of argument */
      නම්(c >= '0' && c <= '9') {
        long arg_max = CSI_ARG_MISSING - 1;
        long *arg = &vt->parser.v.csi.args[vt->parser.v.csi.argi];
        පූර්ණ_සංඛ්‍යා digit = c - '0';

        නම්(*arg == CSI_ARG_MISSING)
          *arg = 0;
        නම්(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        නැතහොත්
          *arg = *arg * 10 + digit;
        bha_nga;
      }
      නම්(c == ':') {
        vt->parser.v.csi.args[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_MORE;
        c = ';';
      }
      නම්(c == ';') {
        නම්(vt->parser.v.csi.argi >= CSI_ARGS_MAX - 1)
          bha_nga; /* drop excess args */
        vt->parser.v.csi.argi++;
        vt->parser.v.csi.args[vt->parser.v.csi.argi] = CSI_ARG_MISSING;
        bha_nga;
      }

      /* else fallthrough */
      vt->parser.v.csi.argi++;
      vt->parser.intermedlen = 0;
      vt->parser.state = CSI_INTERMED;
      // FALLTHROUGH
    pak_sa CSI_INTERMED:
      නම්(is_intermed(c)) {
        නම්(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        bha_nga;
      }
      නැතහොත් නම්(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      නැතහොත් නම්(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_NORMAL_STATE();
      bha_nga;

    pak_sa OSC_COMMAND:
      /* Numerical value of command */
      නම්(c >= '0' && c <= '9') {
        නම්(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        නැතහොත්
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        bha_nga;
      }
      නම්(c == ';') {
        vt->parser.state = OSC;
        string_start = bytes + pos + 1;
        bha_nga;
      }

      /* else fallthrough */
      string_start = bytes + pos;
      string_len   = 0;
      vt->parser.state = OSC;
      goto string_state;

    pak_sa DCS_COMMAND:
      නම්(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      නම්(c >= 0x40 && c<= 0x7e) {
        string_start = bytes + pos + 1;
        vt->parser.state = DCS;
      }
      bha_nga;

string_state:
    pak_sa OSC:
    pak_sa DCS:
    pak_sa APC:
    pak_sa PM:
    pak_sa SOS:
      නම්(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_start, string_len, TRUE);
        ENTER_NORMAL_STATE();
      }
      bha_nga;

    pak_sa NORMAL:
      නම්(vt->parser.in_esc) {
        නම්(is_intermed(c)) {
          නම්(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        නැතහොත් නම්(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.in_esc = 0;
          ENTER_NORMAL_STATE();
        }
        නැතහොත් {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        bha_nga;
      }
      නම්(c1_allowed && c >= 0x80 && c < 0xa0) {
        switch(c) {
        pak_sa 0x90: // DCS
          vt->parser.string_initial = TRUE;
          vt->parser.v.dcs.commandlen = 0;
          ENTER_STATE(DCS_COMMAND);
          bha_nga;
        pak_sa 0x98: // SOS
          vt->parser.string_initial = TRUE;
          ENTER_STATE(SOS);
          string_start = bytes + pos + 1;
          string_len = 0;
          bha_nga;
        pak_sa 0x9b: // CSI
          vt->parser.v.csi.leaderlen = 0;
          ENTER_STATE(CSI_LEADER);
          bha_nga;
        pak_sa 0x9d: // OSC
          vt->parser.v.osc.command = -1;
          vt->parser.string_initial = TRUE;
          string_start = bytes + pos + 1;
          ENTER_STATE(OSC_COMMAND);
          bha_nga;
        pak_sa 0x9e: // PM
          vt->parser.string_initial = TRUE;
          ENTER_STATE(PM);
          string_start = bytes + pos + 1;
          string_len = 0;
          bha_nga;
        pak_sa 0x9f: // APC
          vt->parser.string_initial = TRUE;
          ENTER_STATE(APC);
          string_start = bytes + pos + 1;
          string_len = 0;
          bha_nga;
        default:
          do_control(vt, c);
          bha_nga;
        }
      }
      නැතහොත් {
        size_t eaten = 0;
        නම්(vt->parser.callbacks && vt->parser.callbacks->text)
          eaten = (*vt->parser.callbacks->text)(bytes + pos, len - pos, vt->parser.cbdata);

        නම්(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      bha_nga;
    }
  }

  නම්(string_start) {
    size_t string_len = bytes + pos - string_start;
    නම්(vt->parser.in_esc)
      string_len -= 1;
    string_fragment(vt, string_start, string_len, FALSE);
  }

  ආපසු len;
}

void vterm_parser_set_callbacks(VTerm *vt, const VTermParserCallbacks *callbacks, void *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

void *vterm_parser_get_cbdata(VTerm *vt)
{
  ආපසු vt->parser.cbdata;
}

void vterm_parser_set_emit_nul(VTerm *vt, පූර්ණ_සංඛ්‍යා emit)
{
  vt->parser.emit_nul = emit;
}
