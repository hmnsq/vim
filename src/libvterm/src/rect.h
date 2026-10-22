/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
સ્થિર શૂન્ય rect_expand(VTermRect *dst, VTermRect *src)
{
  જો(dst->start_row > src->start_row) dst->start_row = src->start_row;
  જો(dst->start_col > src->start_col) dst->start_col = src->start_col;
  જો(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  જો(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
સ્થિર શૂન્ય rect_clip(VTermRect *dst, VTermRect *bounds)
{
  જો(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  જો(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  જો(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  જો(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  જો(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  જો(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
સ્થિર પૂર્ણાંક rect_equal(VTermRect *a, VTermRect *b)
{
  પાછા (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
સ્થિર પૂર્ણાંક rect_contains(VTermRect *big, VTermRect *small)
{
  જો(small->start_row < big->start_row) પાછા 0;
  જો(small->start_col < big->start_col) પાછા 0;
  જો(small->end_row   > big->end_row)   પાછા 0;
  જો(small->end_col   > big->end_col)   પાછા 0;
  પાછા 1;
}

/* True if the rectangles overlap at all */
સ્થિર પૂર્ણાંક rect_intersects(VTermRect *a, VTermRect *b)
{
  જો(a->start_row > b->end_row || b->start_row > a->end_row)
    પાછા 0;
  જો(a->start_col > b->end_col || b->start_col > a->end_col)
    પાછા 0;
  પાછા 1;
}
