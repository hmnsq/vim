/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned ចំនួនគត់ utf8_seqlen(long codepoint);

#ប្រសិនបើ defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned ចំនួនគត់ utf8_seqlen(long codepoint)
{
  ប្រសិនបើ(codepoint < 0x0000080) ត្រឡប់ 1;
  ប្រសិនបើ(codepoint < 0x0000800) ត្រឡប់ 2;
  ប្រសិនបើ(codepoint < 0x0010000) ត្រឡប់ 3;
  ប្រសិនបើ(codepoint < 0x0200000) ត្រឡប់ 4;
  ប្រសិនបើ(codepoint < 0x4000000) ត្រឡប់ 5;
  ត្រឡប់ 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
ចំនួនគត់ fill_utf8(long codepoint, var_na *str);

#ប្រសិនបើ defined(DEFINE_INLINES) || USE_INLINE
INLINE ចំនួនគត់ fill_utf8(long codepoint, var_na *str)
{
  ចំនួនគត់ nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  ចំនួនគត់ b = nbytes;
  ខណៈពេល(b > 1) {
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

  ត្រឡប់ nbytes;
}
#endif
/* end copy */
