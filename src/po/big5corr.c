/*
 * Simplistic program to correct Big5 inside strings.  When a trail byte is a
 * backslash it needs to be doubled.
 * Public domain.
 */

/*
 * 06.11.23, added by Restorer:
 * For more details, see:
 * https://github.com/vim/vim/pull/3261
 * https://github.com/vim/vim/pull/3476
 * https://github.com/vim/vim/pull/12153
 * (read all comments)
 *
 * I checked the workability on the list of backslash characters
 * specified in zh_TW.UTF-8.po. It works.
 * But it is better to have someone native speaker check it.
 *
 */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

	पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
	वर्ण buffer[BUFSIZ];
	वर्ण *p;

	जाव (fgets(buffer, BUFSIZ, stdin) != रिक्त)
	{
		प्रति (p = buffer; *p != 0; p++)
		{
			जइ (strncmp(p, "charset=utf-8", 13) == 0
				|| strncmp(p, "charset=UTF-8", 13) == 0)
			{
				fputs("charset=BIG-5", stdout);
				p += 12;
			}
			अण्णहा जइ (strncmp(p, "# Original translations", 23) == 0)
			{
				fputs("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", stdout);
				जाव (p[1] != '\n')
					++p;
			}
			अण्णहा
			{
				जइ (*(unsigned वर्ण *)p >= 0xA1)
				{
					putchar(*p++);
					जइ (*p == '\\')
						putchar(*p);
				}
				putchar(*p);
			}
		}
	}
}
