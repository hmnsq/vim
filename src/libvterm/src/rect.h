/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
ساكن فراغ rect_expand(VTermRect *dst, VTermRect *src)
{
  ܐܢ(dst->start_row > src->start_row) dst->start_row = src->start_row;
  ܐܢ(dst->start_col > src->start_col) dst->start_col = src->start_col;
  ܐܢ(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  ܐܢ(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
ساكن فراغ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  ܐܢ(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  ܐܢ(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  ܐܢ(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  ܐܢ(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  ܐܢ(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  ܐܢ(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
ساكن صحيح rect_equal(VTermRect *a, VTermRect *b)
{
  ܦܢܐ (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
ساكن صحيح rect_contains(VTermRect *big, VTermRect *small)
{
  ܐܢ(small->start_row < big->start_row) ܦܢܐ 0;
  ܐܢ(small->start_col < big->start_col) ܦܢܐ 0;
  ܐܢ(small->end_row   > big->end_row)   ܦܢܐ 0;
  ܐܢ(small->end_col   > big->end_col)   ܦܢܐ 0;
  ܦܢܐ 1;
}

/* True if the rectangles overlap at all */
ساكن صحيح rect_intersects(VTermRect *a, VTermRect *b)
{
  ܐܢ(a->start_row > b->end_row || b->start_row > a->end_row)
    ܦܢܐ 0;
  ܐܢ(a->start_col > b->end_col || b->start_col > a->end_col)
    ܦܢܐ 0;
  ܦܢܐ 1;
}
