#define DEFINE_INLINES

// vim: set sw=2 :
#include "vterm_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "utf8.h"

/*****************
 * API functions *
 *****************/

static void *default_malloc(size_t size, void *allocdata UNUSED)
{
  void *ptr = malloc(size);
  ပေႃး(ptr)
    memset(ptr, 0, size);
  ပွၵ်ႈ ptr;
}

static void default_free(void *ptr, void *allocdata UNUSED)
{
  free(ptr);
}

static VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(puur_naa_nka rows, puur_naa_nka cols)
{
  sa_nracanaa VTermBuilder builder;
  memset(&builder, 0, parimaa_na(builder));
  builder.rows = rows;
  builder.cols = cols;
  ပွၵ်ႈ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(puur_naa_nka rows, puur_naa_nka cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  sa_nracanaa VTermBuilder builder;
  memset(&builder, 0, parimaa_na(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ပွၵ်ႈ vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(const sa_nracanaa VTermBuilder *builder)
{
  const VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(parimaa_na(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = NULL;
  vt->parser.cbdata    = NULL;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = NULL;
  vt->outdata = NULL;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  ပေႃး (vt->tmpbuffer == NULL
      || vt->outbuffer == NULL
      || vt->tmpbuffer == NULL)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ပွၵ်ႈ NULL;
  }

  ပွၵ်ႈ vt;
}

void vterm_free(VTerm *vt)
{
  ပေႃး(vt->screen)
    vterm_screen_free(vt->screen);

  ပေႃး(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL void *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  ပွၵ်ႈ (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  ပေႃး (ptr)
    (*vt->allocator->free)(ptr, vt->allocdata);
}

void vterm_get_size(const VTerm *vt, puur_naa_nka *rowsp, puur_naa_nka *colsp)
{
  ပေႃး(rowsp)
    *rowsp = vt->rows;
  ပေႃး(colsp)
    *colsp = vt->cols;
}

void vterm_set_size(VTerm *vt, puur_naa_nka rows, puur_naa_nka cols)
{
  ပေႃး(rows < 1 || cols < 1)
    ပွၵ်ႈ;

  vt->rows = rows;
  vt->cols = cols;

  ပေႃး(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

puur_naa_nka vterm_get_utf8(const VTerm *vt)
{
  ပွၵ်ႈ vt->mode.utf8;
}

void vterm_set_utf8(VTerm *vt, puur_naa_nka is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

void vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, void *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL void vterm_push_output_bytes(VTerm *vt, const var_na *bytes, size_t len)
{
  ပေႃး(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    ပွၵ်ႈ;
  }

  ပေႃး(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ပွၵ်ႈ;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL void vterm_push_output_vsprintf(VTerm *vt, const var_na *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#ပေႃးဢမ်ႇ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  ပေႃး (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_push_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL void vterm_push_output_sprintf(VTerm *vt, const var_na *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_push_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_ctrl(VTerm *vt, unsigned var_na ctrl, const var_na *fmt, ...)
{
  size_t cur;

  ပေႃး(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  ပေႃးဢမ်ႇ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ပေႃး(cur >= vt->tmpbuffer_len)
    ပွၵ်ႈ;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_str(VTerm *vt, unsigned var_na ctrl, puur_naa_nka term, const var_na *fmt, ...)
{
  size_t cur;

  ပေႃး(ctrl) {
    ပေႃး(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    ပေႃးဢမ်ႇ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ပေႃး(cur >= vt->tmpbuffer_len)
      ပွၵ်ႈ;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  ပေႃး(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ပေႃး(cur >= vt->tmpbuffer_len)
      ပွၵ်ႈ;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(const VTerm *vt)
{
  ပွၵ်ႈ vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(const VTerm *vt)
{
  ပွၵ်ႈ vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(const VTerm *vt)
{
  ပွၵ်ႈ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, var_na *buffer, size_t len)
{
  ပေႃး(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  ပေႃး(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ပွၵ်ႈ len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  switch(attr) {
    pak_sa VTERM_ATTR_BOLD:       ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_UNDERLINE:  ပွၵ်ႈ VTERM_VALUETYPE_INT;
    pak_sa VTERM_ATTR_ITALIC:     ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_BLINK:      ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_REVERSE:    ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_CONCEAL:    ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_STRIKE:     ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_FONT:       ပွၵ်ႈ VTERM_VALUETYPE_INT;
    pak_sa VTERM_ATTR_FOREGROUND: ပွၵ်ႈ VTERM_VALUETYPE_COLOR;
    pak_sa VTERM_ATTR_BACKGROUND: ပွၵ်ႈ VTERM_VALUETYPE_COLOR;
    pak_sa VTERM_ATTR_SMALL:      ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_BASELINE:   ပွၵ်ႈ VTERM_VALUETYPE_INT;

    pak_sa VTERM_N_ATTRS: ပွၵ်ႈ 0;
  }
  ပွၵ်ႈ 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  switch(prop) {
    pak_sa VTERM_PROP_CURSORVISIBLE: ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORBLINK:   ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_ALTSCREEN:     ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_TITLE:         ပွၵ်ႈ VTERM_VALUETYPE_STRING;
    pak_sa VTERM_PROP_ICONNAME:      ပွၵ်ႈ VTERM_VALUETYPE_STRING;
    pak_sa VTERM_PROP_REVERSE:       ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORSHAPE:   ပွၵ်ႈ VTERM_VALUETYPE_INT;
    pak_sa VTERM_PROP_MOUSE:         ပွၵ်ႈ VTERM_VALUETYPE_INT;
    pak_sa VTERM_PROP_FOCUSREPORT:   ပွၵ်ႈ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORCOLOR:   ပွၵ်ႈ VTERM_VALUETYPE_STRING;

    pak_sa VTERM_N_PROPS: ပွၵ်ႈ 0;
  }
  ပွၵ်ႈ 0; /* UNREACHABLE */
}

void vterm_scroll_rect(VTermRect rect,
    puur_naa_nka downward,
    puur_naa_nka rightward,
    puur_naa_nka (*moverect)(VTermRect src, VTermRect dest, void *user),
    puur_naa_nka (*eraserect)(VTermRect rect, puur_naa_nka selective, void *user),
    void *user)
{
  VTermRect src;
  VTermRect dest;

  ပေႃး(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ပွၵ်ႈ;
  }

  ပေႃး(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  ပေႃးဢမ်ႇ {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    puur_naa_nka leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  ပေႃး(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  ပေႃးဢမ်ႇ {
    puur_naa_nka upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  ပေႃး(moverect)
    (*moverect)(dest, src, user);

  ပေႃး(downward > 0)
    rect.start_row = rect.end_row - downward;
  ပေႃးဢမ်ႇ ပေႃး(downward < 0)
    rect.end_row = rect.start_row - downward;

  ပေႃး(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  ပေႃးဢမ်ႇ ပေႃး(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

void vterm_copy_cells(VTermRect dest,
    VTermRect src,
    void (*copycell)(VTermPos dest, VTermPos src, void *user),
    void *user)
{
  puur_naa_nka downward  = src.start_row - dest.start_row;
  puur_naa_nka rightward = src.start_col - dest.start_col;

  puur_naa_nka init_row, test_row, init_col, test_col;
  puur_naa_nka inc_row, inc_col;

  ပေႃး(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  ပေႃးဢမ်ႇ /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  ပေႃး(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  ပေႃးဢမ်ႇ /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  for(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    for(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

void vterm_check_version(puur_naa_nka major, puur_naa_nka minor)
{
  ပေႃး(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  ပေႃး(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
