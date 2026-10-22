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
  නම්(ptr)
    memset(ptr, 0, size);
  ආපසු ptr;
}

static void default_free(void *ptr, void *allocdata UNUSED)
{
  free(ptr);
}

static VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols)
{
  sa_nracanaa VTermBuilder builder;
  memset(&builder, 0, parimaa_na(builder));
  builder.rows = rows;
  builder.cols = cols;
  ආපසු vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  sa_nracanaa VTermBuilder builder;
  memset(&builder, 0, parimaa_na(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ආපසු vterm_build(&builder);
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

  නම් (vt->tmpbuffer == NULL
      || vt->outbuffer == NULL
      || vt->tmpbuffer == NULL)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ආපසු NULL;
  }

  ආපසු vt;
}

void vterm_free(VTerm *vt)
{
  නම්(vt->screen)
    vterm_screen_free(vt->screen);

  නම්(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL void *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  ආපසු (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  නම් (ptr)
    (*vt->allocator->free)(ptr, vt->allocdata);
}

void vterm_get_size(const VTerm *vt, පූර්ණ_සංඛ්‍යා *rowsp, පූර්ණ_සංඛ්‍යා *colsp)
{
  නම්(rowsp)
    *rowsp = vt->rows;
  නම්(colsp)
    *colsp = vt->cols;
}

void vterm_set_size(VTerm *vt, පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols)
{
  නම්(rows < 1 || cols < 1)
    ආපසු;

  vt->rows = rows;
  vt->cols = cols;

  නම්(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

පූර්ණ_සංඛ්‍යා vterm_get_utf8(const VTerm *vt)
{
  ආපසු vt->mode.utf8;
}

void vterm_set_utf8(VTerm *vt, පූර්ණ_සංඛ්‍යා is_utf8)
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
  නම්(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    ආපසු;
  }

  නම්(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ආපසු;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL void vterm_push_output_vsprintf(VTerm *vt, const var_na *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#නැතහොත්
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  නම් (len >= vt->tmpbuffer_len)
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

  නම්(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  නැතහොත්
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  නම්(cur >= vt->tmpbuffer_len)
    ආපසු;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_str(VTerm *vt, unsigned var_na ctrl, පූර්ණ_සංඛ්‍යා term, const var_na *fmt, ...)
{
  size_t cur;

  නම්(ctrl) {
    නම්(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    නැතහොත්
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    නම්(cur >= vt->tmpbuffer_len)
      ආපසු;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  නම්(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    නම්(cur >= vt->tmpbuffer_len)
      ආපසු;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(const VTerm *vt)
{
  ආපසු vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(const VTerm *vt)
{
  ආපසු vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(const VTerm *vt)
{
  ආපසු vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, var_na *buffer, size_t len)
{
  නම්(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  නම්(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ආපසු len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  switch(attr) {
    pak_sa VTERM_ATTR_BOLD:       ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_UNDERLINE:  ආපසු VTERM_VALUETYPE_INT;
    pak_sa VTERM_ATTR_ITALIC:     ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_BLINK:      ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_REVERSE:    ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_CONCEAL:    ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_STRIKE:     ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_FONT:       ආපසු VTERM_VALUETYPE_INT;
    pak_sa VTERM_ATTR_FOREGROUND: ආපසු VTERM_VALUETYPE_COLOR;
    pak_sa VTERM_ATTR_BACKGROUND: ආපසු VTERM_VALUETYPE_COLOR;
    pak_sa VTERM_ATTR_SMALL:      ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_BASELINE:   ආපසු VTERM_VALUETYPE_INT;

    pak_sa VTERM_N_ATTRS: ආපසු 0;
  }
  ආපසු 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  switch(prop) {
    pak_sa VTERM_PROP_CURSORVISIBLE: ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORBLINK:   ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_ALTSCREEN:     ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_TITLE:         ආපසු VTERM_VALUETYPE_STRING;
    pak_sa VTERM_PROP_ICONNAME:      ආපසු VTERM_VALUETYPE_STRING;
    pak_sa VTERM_PROP_REVERSE:       ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORSHAPE:   ආපසු VTERM_VALUETYPE_INT;
    pak_sa VTERM_PROP_MOUSE:         ආපසු VTERM_VALUETYPE_INT;
    pak_sa VTERM_PROP_FOCUSREPORT:   ආපසු VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORCOLOR:   ආපසු VTERM_VALUETYPE_STRING;

    pak_sa VTERM_N_PROPS: ආපසු 0;
  }
  ආපසු 0; /* UNREACHABLE */
}

void vterm_scroll_rect(VTermRect rect,
    පූර්ණ_සංඛ්‍යා downward,
    පූර්ණ_සංඛ්‍යා rightward,
    පූර්ණ_සංඛ්‍යා (*moverect)(VTermRect src, VTermRect dest, void *user),
    පූර්ණ_සංඛ්‍යා (*eraserect)(VTermRect rect, පූර්ණ_සංඛ්‍යා selective, void *user),
    void *user)
{
  VTermRect src;
  VTermRect dest;

  නම්(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ආපසු;
  }

  නම්(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  නැතහොත් {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    පූර්ණ_සංඛ්‍යා leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  නම්(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  නැතහොත් {
    පූර්ණ_සංඛ්‍යා upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  නම්(moverect)
    (*moverect)(dest, src, user);

  නම්(downward > 0)
    rect.start_row = rect.end_row - downward;
  නැතහොත් නම්(downward < 0)
    rect.end_row = rect.start_row - downward;

  නම්(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  නැතහොත් නම්(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

void vterm_copy_cells(VTermRect dest,
    VTermRect src,
    void (*copycell)(VTermPos dest, VTermPos src, void *user),
    void *user)
{
  පූර්ණ_සංඛ්‍යා downward  = src.start_row - dest.start_row;
  පූර්ණ_සංඛ්‍යා rightward = src.start_col - dest.start_col;

  පූර්ණ_සංඛ්‍යා init_row, test_row, init_col, test_col;
  පූර්ණ_සංඛ්‍යා inc_row, inc_col;

  නම්(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  නැතහොත් /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  නම්(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  නැතහොත් /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  සඳහා(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    සඳහා(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

void vterm_check_version(පූර්ණ_සංඛ්‍යා major, පූර්ණ_සංඛ්‍යා minor)
{
  නම්(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  නම්(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
