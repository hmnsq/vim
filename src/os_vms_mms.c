#ضمن	<stdio.h>
#ضمن	<stdlib.h>
#ضمن	<string.h>
#ضمن	<unixio.h>
#ضمن	"vim.h"
صحيح main(صحيح argc, حرف *argv[])
{
    FILE	*fpi, *fpo;
    حرف	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    صحيح		err = 0, err_line = 0;

    mms = "mms";
    argc--;
    argv++;
    ܟܡܐ (argc-- > 0)
    {
	argp = *argv++;
	ܐܢ (*argp == '-')
	{
	    بدل (*++argp)
	    {
		حالة 'm':
		    mms = ++argp;
		    اكسر;
		حالة 'e':
		    ܐܢ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    اكسر;
		افتراضي:
		    اكسر;
	    }
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, حجم(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ܟܡܐ (fgets(buf, BUFSIZ, fpi))
    {
	ܐܢ (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (err_line)
	    {
		ܐܢ (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		ܐܚܪܢܐ
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ܟܡܐ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ܦܢܐ(0);
}
