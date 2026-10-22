#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unixio.h>
#include	"vim.h"
целое основной(целое argc, char *argv[])
{
    FILE	*fpi, *fpo;
    char	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    целое		err = 0, err_line = 0;

    mms = "mms";
    argc--;
    argv++;
    пока (argc-- > 0)
    {
	argp = *argv++;
	если (*argp == '-')
	{
	    switch (*++argp)
	    {
		case 'm':
		    mms = ++argp;
		    break;
		case 'e':
		    если (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    break;
		default:
		    break;
	    }
	}
	иначе
	{
	    если (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, sizeof(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    пока (fgets(buf, BUFSIZ, fpi))
    {
	если (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	иначе
	{
	    если (err_line)
	    {
		если (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		иначе
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    пока (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    вернуть(0);
}
