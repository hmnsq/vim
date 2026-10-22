/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
স্থির শূন্য rect_expand(VTermRect *dst, VTermRect *src)
{
  যদি(dst->start_row > src->start_row) dst->start_row = src->start_row;
  যদি(dst->start_col > src->start_col) dst->start_col = src->start_col;
  যদি(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  যদি(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
স্থির শূন্য rect_clip(VTermRect *dst, VTermRect *bounds)
{
  যদি(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  যদি(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  যদি(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  যদি(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  যদি(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  যদি(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
স্থির পূর্ণাঙ্ক rect_equal(VTermRect *a, VTermRect *b)
{
  ঘূৰাই (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
স্থির পূর্ণাঙ্ক rect_contains(VTermRect *big, VTermRect *small)
{
  যদি(small->start_row < big->start_row) ঘূৰাই 0;
  যদি(small->start_col < big->start_col) ঘূৰাই 0;
  যদি(small->end_row   > big->end_row)   ঘূৰাই 0;
  যদি(small->end_col   > big->end_col)   ঘূৰাই 0;
  ঘূৰাই 1;
}

/* True if the rectangles overlap at all */
স্থির পূর্ণাঙ্ক rect_intersects(VTermRect *a, VTermRect *b)
{
  যদি(a->start_row > b->end_row || b->start_row > a->end_row)
    ঘূৰাই 0;
  যদি(a->start_col > b->end_col || b->start_col > a->end_col)
    ঘূৰাই 0;
  ঘূৰাই 1;
}
