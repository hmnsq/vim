/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
గుర్తులేని పూర్ణాంకం utf8_seqlen(దీర్ఘ codepoint);

#ఒకవేళ defined(DEFINE_INLINES) || USE_INLINE
INLINE గుర్తులేని పూర్ణాంకం utf8_seqlen(దీర్ఘ codepoint)
{
  ఒకవేళ(codepoint < 0x0000080) తిరిగి 1;
  ఒకవేళ(codepoint < 0x0000800) తిరిగి 2;
  ఒకవేళ(codepoint < 0x0010000) తిరిగి 3;
  ఒకవేళ(codepoint < 0x0200000) తిరిగి 4;
  ఒకవేళ(codepoint < 0x4000000) తిరిగి 5;
  తిరిగి 6;
}
#అయితే_ముగింపు

/* Does NOT NUL-terminate the buffer */
పూర్ణాంకం fill_utf8(దీర్ఘ codepoint, అక్షరం *str);

#ఒకవేళ defined(DEFINE_INLINES) || USE_INLINE
INLINE పూర్ణాంకం fill_utf8(దీర్ఘ codepoint, అక్షరం *str)
{
  పూర్ణాంకం nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  పూర్ణాంకం b = nbytes;
  అయితే(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  ఎంపిక(nbytes) {
    స్థితి 1: str[0] =        (codepoint & 0x7f); ఆపు;
    స్థితి 2: str[0] = 0xc0 | (codepoint & 0x1f); ఆపు;
    స్థితి 3: str[0] = 0xe0 | (codepoint & 0x0f); ఆపు;
    స్థితి 4: str[0] = 0xf0 | (codepoint & 0x07); ఆపు;
    స్థితి 5: str[0] = 0xf8 | (codepoint & 0x03); ఆపు;
    స్థితి 6: str[0] = 0xfc | (codepoint & 0x01); ఆపు;
  }

  తిరిగి nbytes;
}
#అయితే_ముగింపు
/* end copy */
