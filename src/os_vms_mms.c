#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unixio.h>
#include	"vim.h"
පූර්ණ_සංඛ්‍යා main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
    FILE	*fpi, *fpo;
    var_na	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    පූර්ණ_සංඛ්‍යා		err = 0, err_line = 0;

    mms = "mms";
    argc--;
    argv++;
    අතරතුර (argc-- > 0)
    {
	argp = *argv++;
	නම් (*argp == '-')
	{
	    switch (*++argp)
	    {
		pak_sa 'm':
		    mms = ++argp;
		    bha_nga;
		pak_sa 'e':
		    නම් (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    bha_nga;
		default:
		    bha_nga;
	    }
	}
	නැතහොත්
	{
	    නම් (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, parimaa_na(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    අතරතුර (fgets(buf, BUFSIZ, fpi))
    {
	නම් (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	නැතහොත්
	{
	    නම් (err_line)
	    {
		නම් (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		නැතහොත්
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    අතරතුර (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ආපසු(0);
}
