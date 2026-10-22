/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned puur_naa_nka utf8_seqlen(long codepoint);

#ᯀᯮᯝ defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned puur_naa_nka utf8_seqlen(long codepoint)
{
  ᯀᯮᯝ(codepoint < 0x0000080) ᯀᯮᯞ 1;
  ᯀᯮᯝ(codepoint < 0x0000800) ᯀᯮᯞ 2;
  ᯀᯮᯝ(codepoint < 0x0010000) ᯀᯮᯞ 3;
  ᯀᯮᯝ(codepoint < 0x0200000) ᯀᯮᯞ 4;
  ᯀᯮᯝ(codepoint < 0x4000000) ᯀᯮᯞ 5;
  ᯀᯮᯞ 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
puur_naa_nka fill_utf8(long codepoint, var_na *str);

#ᯀᯮᯝ defined(DEFINE_INLINES) || USE_INLINE
INLINE puur_naa_nka fill_utf8(long codepoint, var_na *str)
{
  puur_naa_nka nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  puur_naa_nka b = nbytes;
  ᯀᯬᯝ(b > 1) {
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

  ᯀᯮᯞ nbytes;
}
#endif
/* end copy */
