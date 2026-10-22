#अन्तर्भाव	<stdio.h>
#अन्तर्भाव	<stdlib.h>
#अन्तर्भाव	<string.h>
#अन्तर्भाव	<unixio.h>
#अन्तर्भाव	"vim.h"
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण *argv[])
{
    FILE	*fpi, *fpo;
    वर्ण	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    पूर्णाङ्क		err = 0, err_पङ्क्तिः = 0;

    mms = "mms";
    argc--;
    argv++;
    याव (argc-- > 0)
    {
	argp = *argv++;
	सचे (*argp == '-')
	{
	    विकल्प (*++argp)
	    {
		पक्ष 'm':
		    mms = ++argp;
		    भङ्ग;
		पक्ष 'e':
		    सचे (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    भङ्ग;
		स्वभाव:
		    भङ्ग;
	    }
	}
	अञ्ञथा
	{
	    सचे (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, परिमाण(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    याव (fgets(buf, BUFSIZ, fpi))
    {
	सचे (!memcmp(buf, "%CC-", 4))
	{
	    err_पङ्क्तिः++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	अञ्ञथा
	{
	    सचे (err_पङ्क्तिः)
	    {
		सचे (strstr(buf, _("At line")))
		{
		    err_पङ्क्तिः = 0;
		    fprintf(fpo, "@");
		}
		अञ्ञथा
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    याव (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    पटिनिवत्त(0);
}
