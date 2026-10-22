/*
 * Some utility functions on VTermRect structures
 */

#تعریف_کرو STRFrect "(%d,%d-%d,%d)"
#تعریف_کرو ARGSrect(r) (r).آغاز_صف, (r).آغاز_ستون, (r).انتہا_صف, (r).انتہا_ستون

/* Expand dst to contain src as well */
ساکن خالی rect_expand(VTermRect *dst, VTermRect *src)
{
  اگر(dst->آغاز_صف > src->آغاز_صف) dst->آغاز_صف = src->آغاز_صف;
  اگر(dst->آغاز_ستون > src->آغاز_ستون) dst->آغاز_ستون = src->آغاز_ستون;
  اگر(dst->انتہا_صف   < src->انتہا_صف)   dst->انتہا_صف   = src->انتہا_صف;
  اگر(dst->انتہا_ستون   < src->انتہا_ستون)   dst->انتہا_ستون   = src->انتہا_ستون;
}

/* Clip the dst to ensure it does not step outside of bounds */
ساکن خالی rect_clip(VTermRect *dst, VTermRect *bounds)
{
  اگر(dst->آغاز_صف < bounds->آغاز_صف) dst->آغاز_صف = bounds->آغاز_صف;
  اگر(dst->آغاز_ستون < bounds->آغاز_ستون) dst->آغاز_ستون = bounds->آغاز_ستون;
  اگر(dst->انتہا_صف   > bounds->انتہا_صف)   dst->انتہا_صف   = bounds->انتہا_صف;
  اگر(dst->انتہا_ستون   > bounds->انتہا_ستون)   dst->انتہا_ستون   = bounds->انتہا_ستون;
  /* Ensure it doesn't end up negatively-sized */
  اگر(dst->انتہا_صف < dst->آغاز_صف) dst->انتہا_صف = dst->آغاز_صف;
  اگر(dst->انتہا_ستون < dst->آغاز_ستون) dst->انتہا_ستون = dst->آغاز_ستون;
}

/* True if the two rectangles are equal */
ساکن صحیح rect_equal(VTermRect *a, VTermRect *b)
{
  واپس (a->آغاز_صف == b->آغاز_صف) &&
         (a->آغاز_ستون == b->آغاز_ستون) &&
         (a->انتہا_صف   == b->انتہا_صف)   &&
         (a->انتہا_ستون   == b->انتہا_ستون);
}

/* True if small is contained entirely within big */
ساکن صحیح rect_شامل_ہے(VTermRect *big, VTermRect *small)
{
  اگر(small->آغاز_صف < big->آغاز_صف) واپس 0;
  اگر(small->آغاز_ستون < big->آغاز_ستون) واپس 0;
  اگر(small->انتہا_صف   > big->انتہا_صف)   واپس 0;
  اگر(small->انتہا_ستون   > big->انتہا_ستون)   واپس 0;
  واپس 1;
}

/* True if the rectangles overlap at all */
ساکن صحیح rect_intersects(VTermRect *a, VTermRect *b)
{
  اگر(a->آغاز_صف > b->انتہا_صف || b->آغاز_صف > a->انتہا_صف)
    واپس 0;
  اگر(a->آغاز_ستون > b->انتہا_ستون || b->آغاز_ستون > a->انتہا_ستون)
    واپس 0;
  واپس 1;
}
