#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unixio.h>
#include	"vim.h"
정수 main(정수 argc, char *argv[])
{
    FILE	*fpi, *fpo;
    char	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    정수		err = 0, err_line = 0;

    mms = "mms";
    argc--;
    argv++;
    동안 (argc-- > 0)
    {
	argp = *argv++;
	만약 (*argp == '-')
	{
	    switch (*++argp)
	    {
		case 'm':
		    mms = ++argp;
		    break;
		case 'e':
		    만약 (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    break;
		default:
		    break;
	    }
	}
	아니면
	{
	    만약 (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, sizeof(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    동안 (fgets(buf, BUFSIZ, fpi))
    {
	만약 (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	아니면
	{
	    만약 (err_line)
	    {
		만약 (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		아니면
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    동안 (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    반환(0);
}
