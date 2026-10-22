/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
ایستا تهی rect_expand(VTermRect *dst, VTermRect *src)
{
  اگر(dst->start_row > src->start_row) dst->start_row = src->start_row;
  اگر(dst->start_col > src->start_col) dst->start_col = src->start_col;
  اگر(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  اگر(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
ایستا تهی rect_clip(VTermRect *dst, VTermRect *bounds)
{
  اگر(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  اگر(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  اگر(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  اگر(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  اگر(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  اگر(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
ایستا صحیح rect_equal(VTermRect *a, VTermRect *b)
{
  بازگشت (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
ایستا صحیح rect_contains(VTermRect *big, VTermRect *small)
{
  اگر(small->start_row < big->start_row) بازگشت 0;
  اگر(small->start_col < big->start_col) بازگشت 0;
  اگر(small->end_row   > big->end_row)   بازگشت 0;
  اگر(small->end_col   > big->end_col)   بازگشت 0;
  بازگشت 1;
}

/* True if the rectangles overlap at all */
ایستا صحیح rect_intersects(VTermRect *a, VTermRect *b)
{
  اگر(a->start_row > b->end_row || b->start_row > a->end_row)
    بازگشت 0;
  اگر(a->start_col > b->end_col || b->start_col > a->end_col)
    بازگشت 0;
  بازگشت 1;
}
