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
  ᰀᰧ(ptr)
    memset(ptr, 0, size);
  ᰀᰧᰌ ptr;
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
  ᰀᰧᰌ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(puur_naa_nka rows, puur_naa_nka cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  sa_nracanaa VTermBuilder builder;
  memset(&builder, 0, parimaa_na(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ᰀᰧᰌ vterm_build(&builder);
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

  ᰀᰧ (vt->tmpbuffer == NULL
      || vt->outbuffer == NULL
      || vt->tmpbuffer == NULL)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ᰀᰧᰌ NULL;
  }

  ᰀᰧᰌ vt;
}

void vterm_free(VTerm *vt)
{
  ᰀᰧ(vt->screen)
    vterm_screen_free(vt->screen);

  ᰀᰧ(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL void *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  ᰀᰧᰌ (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  ᰀᰧ (ptr)
    (*vt->allocator->free)(ptr, vt->allocdata);
}

void vterm_get_size(const VTerm *vt, puur_naa_nka *rowsp, puur_naa_nka *colsp)
{
  ᰀᰧ(rowsp)
    *rowsp = vt->rows;
  ᰀᰧ(colsp)
    *colsp = vt->cols;
}

void vterm_set_size(VTerm *vt, puur_naa_nka rows, puur_naa_nka cols)
{
  ᰀᰧ(rows < 1 || cols < 1)
    ᰀᰧᰌ;

  vt->rows = rows;
  vt->cols = cols;

  ᰀᰧ(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

puur_naa_nka vterm_get_utf8(const VTerm *vt)
{
  ᰀᰧᰌ vt->mode.utf8;
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
  ᰀᰧ(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    ᰀᰧᰌ;
  }

  ᰀᰧ(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ᰀᰧᰌ;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL void vterm_push_output_vsprintf(VTerm *vt, const var_na *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#ᰀᰧᰀ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  ᰀᰧ (len >= vt->tmpbuffer_len)
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

  ᰀᰧ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  ᰀᰧᰀ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ᰀᰧ(cur >= vt->tmpbuffer_len)
    ᰀᰧᰌ;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_str(VTerm *vt, unsigned var_na ctrl, puur_naa_nka term, const var_na *fmt, ...)
{
  size_t cur;

  ᰀᰧ(ctrl) {
    ᰀᰧ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    ᰀᰧᰀ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ᰀᰧ(cur >= vt->tmpbuffer_len)
      ᰀᰧᰌ;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  ᰀᰧ(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ᰀᰧ(cur >= vt->tmpbuffer_len)
      ᰀᰧᰌ;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(const VTerm *vt)
{
  ᰀᰧᰌ vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(const VTerm *vt)
{
  ᰀᰧᰌ vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(const VTerm *vt)
{
  ᰀᰧᰌ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, var_na *buffer, size_t len)
{
  ᰀᰧ(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  ᰀᰧ(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ᰀᰧᰌ len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  switch(attr) {
    pak_sa VTERM_ATTR_BOLD:       ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_UNDERLINE:  ᰀᰧᰌ VTERM_VALUETYPE_INT;
    pak_sa VTERM_ATTR_ITALIC:     ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_BLINK:      ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_REVERSE:    ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_CONCEAL:    ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_STRIKE:     ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_FONT:       ᰀᰧᰌ VTERM_VALUETYPE_INT;
    pak_sa VTERM_ATTR_FOREGROUND: ᰀᰧᰌ VTERM_VALUETYPE_COLOR;
    pak_sa VTERM_ATTR_BACKGROUND: ᰀᰧᰌ VTERM_VALUETYPE_COLOR;
    pak_sa VTERM_ATTR_SMALL:      ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_BASELINE:   ᰀᰧᰌ VTERM_VALUETYPE_INT;

    pak_sa VTERM_N_ATTRS: ᰀᰧᰌ 0;
  }
  ᰀᰧᰌ 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  switch(prop) {
    pak_sa VTERM_PROP_CURSORVISIBLE: ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORBLINK:   ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_ALTSCREEN:     ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_TITLE:         ᰀᰧᰌ VTERM_VALUETYPE_STRING;
    pak_sa VTERM_PROP_ICONNAME:      ᰀᰧᰌ VTERM_VALUETYPE_STRING;
    pak_sa VTERM_PROP_REVERSE:       ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORSHAPE:   ᰀᰧᰌ VTERM_VALUETYPE_INT;
    pak_sa VTERM_PROP_MOUSE:         ᰀᰧᰌ VTERM_VALUETYPE_INT;
    pak_sa VTERM_PROP_FOCUSREPORT:   ᰀᰧᰌ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORCOLOR:   ᰀᰧᰌ VTERM_VALUETYPE_STRING;

    pak_sa VTERM_N_PROPS: ᰀᰧᰌ 0;
  }
  ᰀᰧᰌ 0; /* UNREACHABLE */
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

  ᰀᰧ(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ᰀᰧᰌ;
  }

  ᰀᰧ(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  ᰀᰧᰀ {
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

  ᰀᰧ(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  ᰀᰧᰀ {
    puur_naa_nka upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  ᰀᰧ(moverect)
    (*moverect)(dest, src, user);

  ᰀᰧ(downward > 0)
    rect.start_row = rect.end_row - downward;
  ᰀᰧᰀ ᰀᰧ(downward < 0)
    rect.end_row = rect.start_row - downward;

  ᰀᰧ(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  ᰀᰧᰀ ᰀᰧ(rightward < 0)
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

  ᰀᰧ(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  ᰀᰧᰀ /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  ᰀᰧ(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  ᰀᰧᰀ /* rightward >= 0 */ {
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
  ᰀᰧ(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  ᰀᰧ(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
