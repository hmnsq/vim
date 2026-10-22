/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned จำนวนเต็ม utf8_seqlen(long codepoint);

#ถ้า defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned จำนวนเต็ม utf8_seqlen(long codepoint)
{
  ถ้า(codepoint < 0x0000080) คืน 1;
  ถ้า(codepoint < 0x0000800) คืน 2;
  ถ้า(codepoint < 0x0010000) คืน 3;
  ถ้า(codepoint < 0x0200000) คืน 4;
  ถ้า(codepoint < 0x4000000) คืน 5;
  คืน 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
จำนวนเต็ม fill_utf8(long codepoint, var_na *str);

#ถ้า defined(DEFINE_INLINES) || USE_INLINE
INLINE จำนวนเต็ม fill_utf8(long codepoint, var_na *str)
{
  จำนวนเต็ม nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  จำนวนเต็ม b = nbytes;
  ขณะ(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  switch(nbytes) {
    pak_sa 1: str[0] =        (codepoint & 0x7f); bha_nga;
    pak_sa 2: str[0] = 0xc0 | (codepoint & 0x1f); bha_nga;
    pak_sa 3: str[0] = 0xe0 | (codepoint & 0x0f); bha_nga;
    pak_sa 4: str[0] = 0xf0 | (codepoint & 0x07); bha_nga;
    pak_sa 5: str[0] = 0xf8 | (codepoint & 0x03); bha_nga;
    pak_sa 6: str[0] = 0xfc | (codepoint & 0x01); bha_nga;
  }

  คืน nbytes;
}
#endif
/* end copy */
