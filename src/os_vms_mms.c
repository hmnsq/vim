#شامل	<stdio.h>
#شامل	<stdlib.h>
#شامل	<string.h>
#شامل	<unixio.h>
#شامل	"vim.h"
صحیح اصلی(صحیح argc, نویسه *argv[])
{
    FILE	*fpi, *fpo;
    نویسه	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    صحیح		err = 0, err_سطر = 0;

    mms = "mms";
    argc--;
    argv++;
    وقتی (argc-- > 0)
    {
	argp = *argv++;
	اگر (*argp == '-')
	{
	    انتخاب (*++argp)
	    {
		حالت 'm':
		    mms = ++argp;
		    بشکن;
		حالت 'e':
		    اگر (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    بشکن;
		پیشفرض:
		    بشکن;
	    }
	}
	وگرنه
	{
	    اگر (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, اندازه(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    وقتی (fgets(buf, BUFSIZ, fpi))
    {
	اگر (!memcmp(buf, "%CC-", 4))
	{
	    err_سطر++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	وگرنه
	{
	    اگر (err_سطر)
	    {
		اگر (strstr(buf, _("At line")))
		{
		    err_سطر = 0;
		    fprintf(fpo, "@");
		}
		وگرنه
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    وقتی (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    بازگشت(0);
}
