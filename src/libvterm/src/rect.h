/*
 * Some utility functions on VTermRect structures
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRFrect "(%d,%d-%d,%d)"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARGSrect(r) (r).ਸ਼ੁਰੂ_ਪੰਕਤੀ, (r).ਸ਼ੁਰੂ_ਖਾਨਾ, (r).ਸਿਰਾ_ਪੰਕਤੀ, (r).ਸਿਰਾ_ਖਾਨਾ

/* Expand dst to contain src as well */
ਸਥਿਰ ਖਾਲੀ rect_expand(VTermRect *dst, VTermRect *src)
{
  ਜੇ(dst->ਸ਼ੁਰੂ_ਪੰਕਤੀ > src->ਸ਼ੁਰੂ_ਪੰਕਤੀ) dst->ਸ਼ੁਰੂ_ਪੰਕਤੀ = src->ਸ਼ੁਰੂ_ਪੰਕਤੀ;
  ਜੇ(dst->ਸ਼ੁਰੂ_ਖਾਨਾ > src->ਸ਼ੁਰੂ_ਖਾਨਾ) dst->ਸ਼ੁਰੂ_ਖਾਨਾ = src->ਸ਼ੁਰੂ_ਖਾਨਾ;
  ਜੇ(dst->ਸਿਰਾ_ਪੰਕਤੀ   < src->ਸਿਰਾ_ਪੰਕਤੀ)   dst->ਸਿਰਾ_ਪੰਕਤੀ   = src->ਸਿਰਾ_ਪੰਕਤੀ;
  ਜੇ(dst->ਸਿਰਾ_ਖਾਨਾ   < src->ਸਿਰਾ_ਖਾਨਾ)   dst->ਸਿਰਾ_ਖਾਨਾ   = src->ਸਿਰਾ_ਖਾਨਾ;
}

/* Clip the dst to ensure it does not step outside of bounds */
ਸਥਿਰ ਖਾਲੀ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  ਜੇ(dst->ਸ਼ੁਰੂ_ਪੰਕਤੀ < bounds->ਸ਼ੁਰੂ_ਪੰਕਤੀ) dst->ਸ਼ੁਰੂ_ਪੰਕਤੀ = bounds->ਸ਼ੁਰੂ_ਪੰਕਤੀ;
  ਜੇ(dst->ਸ਼ੁਰੂ_ਖਾਨਾ < bounds->ਸ਼ੁਰੂ_ਖਾਨਾ) dst->ਸ਼ੁਰੂ_ਖਾਨਾ = bounds->ਸ਼ੁਰੂ_ਖਾਨਾ;
  ਜੇ(dst->ਸਿਰਾ_ਪੰਕਤੀ   > bounds->ਸਿਰਾ_ਪੰਕਤੀ)   dst->ਸਿਰਾ_ਪੰਕਤੀ   = bounds->ਸਿਰਾ_ਪੰਕਤੀ;
  ਜੇ(dst->ਸਿਰਾ_ਖਾਨਾ   > bounds->ਸਿਰਾ_ਖਾਨਾ)   dst->ਸਿਰਾ_ਖਾਨਾ   = bounds->ਸਿਰਾ_ਖਾਨਾ;
  /* Ensure it doesn't end up negatively-sized */
  ਜੇ(dst->ਸਿਰਾ_ਪੰਕਤੀ < dst->ਸ਼ੁਰੂ_ਪੰਕਤੀ) dst->ਸਿਰਾ_ਪੰਕਤੀ = dst->ਸ਼ੁਰੂ_ਪੰਕਤੀ;
  ਜੇ(dst->ਸਿਰਾ_ਖਾਨਾ < dst->ਸ਼ੁਰੂ_ਖਾਨਾ) dst->ਸਿਰਾ_ਖਾਨਾ = dst->ਸ਼ੁਰੂ_ਖਾਨਾ;
}

/* True if the two rectangles are equal */
ਸਥਿਰ ਪੂਰਨ rect_equal(VTermRect *a, VTermRect *b)
{
  ਵਾਪਸ (a->ਸ਼ੁਰੂ_ਪੰਕਤੀ == b->ਸ਼ੁਰੂ_ਪੰਕਤੀ) &&
         (a->ਸ਼ੁਰੂ_ਖਾਨਾ == b->ਸ਼ੁਰੂ_ਖਾਨਾ) &&
         (a->ਸਿਰਾ_ਪੰਕਤੀ   == b->ਸਿਰਾ_ਪੰਕਤੀ)   &&
         (a->ਸਿਰਾ_ਖਾਨਾ   == b->ਸਿਰਾ_ਖਾਨਾ);
}

/* True if small is contained entirely within big */
ਸਥਿਰ ਪੂਰਨ rect_ਸ਼ਾਮਲ_ਹੈ(VTermRect *big, VTermRect *small)
{
  ਜੇ(small->ਸ਼ੁਰੂ_ਪੰਕਤੀ < big->ਸ਼ੁਰੂ_ਪੰਕਤੀ) ਵਾਪਸ 0;
  ਜੇ(small->ਸ਼ੁਰੂ_ਖਾਨਾ < big->ਸ਼ੁਰੂ_ਖਾਨਾ) ਵਾਪਸ 0;
  ਜੇ(small->ਸਿਰਾ_ਪੰਕਤੀ   > big->ਸਿਰਾ_ਪੰਕਤੀ)   ਵਾਪਸ 0;
  ਜੇ(small->ਸਿਰਾ_ਖਾਨਾ   > big->ਸਿਰਾ_ਖਾਨਾ)   ਵਾਪਸ 0;
  ਵਾਪਸ 1;
}

/* True if the rectangles overlap at all */
ਸਥਿਰ ਪੂਰਨ rect_intersects(VTermRect *a, VTermRect *b)
{
  ਜੇ(a->ਸ਼ੁਰੂ_ਪੰਕਤੀ > b->ਸਿਰਾ_ਪੰਕਤੀ || b->ਸ਼ੁਰੂ_ਪੰਕਤੀ > a->ਸਿਰਾ_ਪੰਕਤੀ)
    ਵਾਪਸ 0;
  ਜੇ(a->ਸ਼ੁਰੂ_ਖਾਨਾ > b->ਸਿਰਾ_ਖਾਨਾ || b->ਸ਼ੁਰੂ_ਖਾਨਾ > a->ਸਿਰਾ_ਖਾਨਾ)
    ਵਾਪਸ 0;
  ਵਾਪਸ 1;
}
