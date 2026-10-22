#include "vterm_internal.h"

#include <stdio.h>
#include <string.h>

#undef DEBUG_PARSER

static ཧྲིལ is_intermed(unsigned var_na c)
{
  ལོག c >= 0x20 && c <= 0x2f;
}

static void do_control(VTerm *vt, unsigned var_na control)
{
  གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->control)
    གལ་སྲིད((*vt->parser.callbacks->control)(control, vt->parser.cbdata))
      ལོག;

  DEBUG_LOG1("libvterm: Unhandled control 0x%02x\n", control);
}

static void do_csi(VTerm *vt, var_na command)
{
#ifdef DEBUG_PARSER
  printf("Parsed CSI args as:\n", arglen, args);
  printf(" leader: %s\n", vt->parser.v.csi.leader);
  དོན་ལུ(ཧྲིལ argi = 0; argi < vt->parser.v.csi.argi; argi++) {
    printf(" %lu", CSI_ARG(vt->parser.v.csi.args[argi]));
    གལ་སྲིད(!CSI_ARG_HAS_MORE(vt->parser.v.csi.args[argi]))
      printf("\n");
  printf(" intermed: %s\n", vt->parser.intermed);
  }
#endif

  གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->csi)
    གལ་སྲིད((*vt->parser.callbacks->csi)(
          vt->parser.v.csi.leaderlen ? vt->parser.v.csi.leader : NULL,
          vt->parser.v.csi.args,
          vt->parser.v.csi.argi,
          vt->parser.intermedlen ? vt->parser.intermed : NULL,
          command,
          vt->parser.cbdata))
      ལོག;

  DEBUG_LOG1("libvterm: Unhandled CSI %c\n", command);
}

static void do_escape(VTerm *vt, var_na command)
{
  var_na seq[INTERMED_MAX+1];

  size_t len = vt->parser.intermedlen;
  strncpy(seq, vt->parser.intermed, len);
  seq[len++] = command;
  seq[len]   = 0;

  གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->escape)
    གལ་སྲིད((*vt->parser.callbacks->escape)(seq, len, vt->parser.cbdata))
      ལོག;

  DEBUG_LOG1("libvterm: Unhandled escape ESC 0x%02x\n", command);
}

static void string_fragment(VTerm *vt, const var_na *str, size_t len, ཧྲིལ final)
{
  VTermStringFragment frag;

  frag.str = str;
  frag.len = len;
  frag.initial = vt->parser.string_initial;
  frag.final = final;

  switch(vt->parser.state) {
    pak_sa OSC:
      གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->osc)
        (*vt->parser.callbacks->osc)(vt->parser.v.osc.command, frag, vt->parser.cbdata);
      bha_nga;

    pak_sa DCS:
      གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->dcs)
        (*vt->parser.callbacks->dcs)(vt->parser.v.dcs.command, vt->parser.v.dcs.commandlen, frag, vt->parser.cbdata);
      bha_nga;

    pak_sa APC:
      གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->apc)
        (*vt->parser.callbacks->apc)(frag, vt->parser.cbdata);
      bha_nga;

    pak_sa PM:
      གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->pm)
        (*vt->parser.callbacks->pm)(frag, vt->parser.cbdata);
      bha_nga;

    pak_sa SOS:
      གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->sos)
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

#define ENTER_STATE(st)        do { vt->parser.state = st; string_start = NULL; } སྐབས(0)
#define ENTER_NORMAL_STATE()   ENTER_STATE(NORMAL)

#define IS_STRING_STATE()      (vt->parser.state >= OSC_COMMAND)

  དོན་ལུ( ; pos < len; pos++) {
    unsigned var_na c = bytes[pos];
    ཧྲིལ c1_allowed = !vt->mode.utf8;

    གལ་སྲིད(c == 0x00 || c == 0x7f) { // NUL, DEL
      གལ་སྲིད(IS_STRING_STATE()) {
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
        string_start = bytes + pos + 1;
      }
      གལ་སྲིད(vt->parser.emit_nul)
        do_control(vt, c);
      continue;
    }
    གལ་སྲིད(c == 0x18 || c == 0x1a) { // CAN, SUB
      vt->parser.in_esc = FALSE;
      ENTER_NORMAL_STATE();
      གལ་སྲིད(vt->parser.emit_nul)
        do_control(vt, c);
      continue;
    }
    དེ་མིན གལ་སྲིད(c == 0x1b) { // ESC
      vt->parser.intermedlen = 0;
      གལ་སྲིད(!IS_STRING_STATE())
        vt->parser.state = NORMAL;
      vt->parser.in_esc = TRUE;
      continue;
    }
    དེ་མིན གལ་སྲིད(c == 0x07 &&  // BEL, can stand for ST in OSC or DCS state
            IS_STRING_STATE()) {
      // fallthrough
    }
    དེ་མིན གལ་སྲིད(c < 0x20) { // other C0
      གལ་སྲིད(vt->parser.state == SOS)
        continue; // All other C0s permitted in SOS

      གལ་སྲིད(vterm_get_special_pty_type() == 2) {
        གལ་སྲིད(c == 0x08) // BS
          // Set the trick for BS output after a sequence, to delay backspace
          // activation
          གལ་སྲིད(pos + 2 < len && bytes[pos + 1] == 0x20 && bytes[pos + 2] == 0x08)
            vt->in_backspace = 2; // Trigger when count down to 1
      }
      གལ་སྲིད(IS_STRING_STATE())
        string_fragment(vt, string_start, bytes + pos - string_start, FALSE);
      do_control(vt, c);
      གལ་སྲིད(IS_STRING_STATE())
        string_start = bytes + pos + 1;
      continue;
    }
    // else fallthrough

    size_t string_len = bytes + pos - string_start;

    གལ་སྲིད(vt->parser.in_esc) {
      // Hoist an ESC letter into a C1 if we're not in a string mode
      // Always accept ESC \ == ST even in string mode
      གལ་སྲིད(!vt->parser.intermedlen &&
          c >= 0x40 && c < 0x60 &&
          ((!IS_STRING_STATE() || c == 0x5c))) {
        c += 0x40;
        c1_allowed = TRUE;
        གལ་སྲིད(string_len)
          string_len -= 1;
        vt->parser.in_esc = FALSE;
      }
      དེ་མིན {
        string_start = NULL;
        vt->parser.state = NORMAL;
      }
    }

    switch(vt->parser.state) {
    pak_sa CSI_LEADER:
      /* Extract leader bytes 0x3c to 0x3f */
      གལ་སྲིད(c >= 0x3c && c <= 0x3f) {
        གལ་སྲིད(vt->parser.v.csi.leaderlen < CSI_LEADER_MAX-1)
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
      གལ་སྲིད(c >= '0' && c <= '9') {
        long arg_max = CSI_ARG_MISSING - 1;
        long *arg = &vt->parser.v.csi.args[vt->parser.v.csi.argi];
        ཧྲིལ digit = c - '0';

        གལ་སྲིད(*arg == CSI_ARG_MISSING)
          *arg = 0;
        གལ་སྲིད(*arg > (arg_max - digit) / 10)
          *arg = arg_max;
        དེ་མིན
          *arg = *arg * 10 + digit;
        bha_nga;
      }
      གལ་སྲིད(c == ':') {
        vt->parser.v.csi.args[vt->parser.v.csi.argi] |= CSI_ARG_FLAG_MORE;
        c = ';';
      }
      གལ་སྲིད(c == ';') {
        གལ་སྲིད(vt->parser.v.csi.argi >= CSI_ARGS_MAX - 1)
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
      གལ་སྲིད(is_intermed(c)) {
        གལ་སྲིད(vt->parser.intermedlen < INTERMED_MAX-1)
          vt->parser.intermed[vt->parser.intermedlen++] = c;
        bha_nga;
      }
      དེ་མིན གལ་སྲིད(c == 0x1b) {
        /* ESC in CSI cancels */
      }
      དེ་མིན གལ་སྲིད(c >= 0x40 && c <= 0x7e) {
        vt->parser.intermed[vt->parser.intermedlen] = 0;
        do_csi(vt, c);
      }
      /* else was invalid CSI */

      ENTER_NORMAL_STATE();
      bha_nga;

    pak_sa OSC_COMMAND:
      /* Numerical value of command */
      གལ་སྲིད(c >= '0' && c <= '9') {
        གལ་སྲིད(vt->parser.v.osc.command == -1)
          vt->parser.v.osc.command = 0;
        དེ་མིན
          vt->parser.v.osc.command *= 10;
        vt->parser.v.osc.command += c - '0';
        bha_nga;
      }
      གལ་སྲིད(c == ';') {
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
      གལ་སྲིད(vt->parser.v.dcs.commandlen < CSI_LEADER_MAX)
        vt->parser.v.dcs.command[vt->parser.v.dcs.commandlen++] = c;

      གལ་སྲིད(c >= 0x40 && c<= 0x7e) {
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
      གལ་སྲིད(c == 0x07 || (c1_allowed && c == 0x9c)) {
        string_fragment(vt, string_start, string_len, TRUE);
        ENTER_NORMAL_STATE();
      }
      bha_nga;

    pak_sa NORMAL:
      གལ་སྲིད(vt->parser.in_esc) {
        གལ་སྲིད(is_intermed(c)) {
          གལ་སྲིད(vt->parser.intermedlen < INTERMED_MAX-1)
            vt->parser.intermed[vt->parser.intermedlen++] = c;
        }
        དེ་མིན གལ་སྲིད(c >= 0x30 && c < 0x7f) {
          do_escape(vt, c);
          vt->parser.in_esc = 0;
          ENTER_NORMAL_STATE();
        }
        དེ་མིན {
          DEBUG_LOG1("TODO: Unhandled byte %02x in Escape\n", c);
        }
        bha_nga;
      }
      གལ་སྲིད(c1_allowed && c >= 0x80 && c < 0xa0) {
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
      དེ་མིན {
        size_t eaten = 0;
        གལ་སྲིད(vt->parser.callbacks && vt->parser.callbacks->text)
          eaten = (*vt->parser.callbacks->text)(bytes + pos, len - pos, vt->parser.cbdata);

        གལ་སྲིད(!eaten) {
          DEBUG_LOG("libvterm: Text callback did not consume any input\n");
          /* force it to make progress */
          eaten = 1;
        }

        pos += (eaten - 1); // we'll ++ it again in a moment
      }
      bha_nga;
    }
  }

  གལ་སྲིད(string_start) {
    size_t string_len = bytes + pos - string_start;
    གལ་སྲིད(vt->parser.in_esc)
      string_len -= 1;
    string_fragment(vt, string_start, string_len, FALSE);
  }

  ལོག len;
}

void vterm_parser_set_callbacks(VTerm *vt, const VTermParserCallbacks *callbacks, void *user)
{
  vt->parser.callbacks = callbacks;
  vt->parser.cbdata = user;
}

void *vterm_parser_get_cbdata(VTerm *vt)
{
  ལོག vt->parser.cbdata;
}

void vterm_parser_set_emit_nul(VTerm *vt, ཧྲིལ emit)
{
  vt->parser.emit_nul = emit;
}
