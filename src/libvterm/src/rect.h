/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
நிலையான வெறுமை rect_expand(VTermRect *dst, VTermRect *src)
{
  எனில்(dst->start_row > src->start_row) dst->start_row = src->start_row;
  எனில்(dst->start_col > src->start_col) dst->start_col = src->start_col;
  எனில்(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  எனில்(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
நிலையான வெறுமை rect_clip(VTermRect *dst, VTermRect *bounds)
{
  எனில்(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  எனில்(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  எனில்(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  எனில்(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  எனில்(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  எனில்(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
நிலையான முழு rect_equal(VTermRect *a, VTermRect *b)
{
  திரும்பு (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
நிலையான முழு rect_contains(VTermRect *big, VTermRect *small)
{
  எனில்(small->start_row < big->start_row) திரும்பு 0;
  எனில்(small->start_col < big->start_col) திரும்பு 0;
  எனில்(small->end_row   > big->end_row)   திரும்பு 0;
  எனில்(small->end_col   > big->end_col)   திரும்பு 0;
  திரும்பு 1;
}

/* True if the rectangles overlap at all */
நிலையான முழு rect_intersects(VTermRect *a, VTermRect *b)
{
  எனில்(a->start_row > b->end_row || b->start_row > a->end_row)
    திரும்பு 0;
  எனில்(a->start_col > b->end_col || b->start_col > a->end_col)
    திரும்பு 0;
  திரும்பு 1;
}
