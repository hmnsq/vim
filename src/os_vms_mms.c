#अन्तर्भाव	<stdio.h>
#अन्तर्भाव	<stdlib.h>
#अन्तर्भाव	<string.h>
#अन्तर्भाव	<unixio.h>
#अन्तर्भाव	"vim.h"
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण *argv[])
{
    FILE	*fpi, *fpo;
    वर्ण	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    पूर्णाङ्क		err = 0, err_ओळ = 0;

    mms = "mms";
    argc--;
    argv++;
    यावत् (argc-- > 0)
    {
	argp = *argv++;
	यदि (*argp == '-')
	{
	    विकल्प (*++argp)
	    {
		पक्ष 'm':
		    mms = ++argp;
		    भङ्ग;
		पक्ष 'e':
		    यदि (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    भङ्ग;
		स्वभाव:
		    भङ्ग;
	    }
	}
	अन्यथा
	{
	    यदि (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, परिमाण(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    यावत् (fgets(buf, BUFSIZ, fpi))
    {
	यदि (!memcmp(buf, "%CC-", 4))
	{
	    err_ओळ++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	अन्यथा
	{
	    यदि (err_ओळ)
	    {
		यदि (strstr(buf, _("At line")))
		{
		    err_ओळ = 0;
		    fprintf(fpo, "@");
		}
		अन्यथा
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    यावत् (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    प्रत्यागम(0);
}
