/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned ကိန်းပြည့် utf8_seqlen(long codepoint);

#အကယ်၍ defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned ကိန်းပြည့် utf8_seqlen(long codepoint)
{
  အကယ်၍(codepoint < 0x0000080) ပြန်ပေး 1;
  အကယ်၍(codepoint < 0x0000800) ပြန်ပေး 2;
  အကယ်၍(codepoint < 0x0010000) ပြန်ပေး 3;
  အကယ်၍(codepoint < 0x0200000) ပြန်ပေး 4;
  အကယ်၍(codepoint < 0x4000000) ပြန်ပေး 5;
  ပြန်ပေး 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
ကိန်းပြည့် fill_utf8(long codepoint, var_na *str);

#အကယ်၍ defined(DEFINE_INLINES) || USE_INLINE
INLINE ကိန်းပြည့် fill_utf8(long codepoint, var_na *str)
{
  ကိန်းပြည့် nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ကိန်းပြည့် b = nbytes;
  နေစဉ်(b > 1) {
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

  ပြန်ပေး nbytes;
}
#endif
/* end copy */
