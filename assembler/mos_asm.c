/*                            _ooOoo_ 
                            o8888888o 
                             88" . "88 
                             (| -_- |) 
                              O\ = /O 
                          ____/`---'\____ 
                        .   ' \\| |// `. 
                         / \\||| : |||// \ 
                       / _||||| -:- |||||- \ 
                         | | \\\ - /// | | 
                       | \_| ''\---/'' | | 
                        \ .-\__ `-` ___/-. / 
                     ___`. .' /--.--\ `. . __ 
                  ."" '< `.___\_<|>_/___.' >'"". 
                 | | : `- \`.;`\ _ /`;.`/ - ` : | | 
                   \ \ `-. \_ __\ /__ _/ .-` / / 
           ======`-.____`-.___\_____/___.-`____.-'====== 
                              `=---=' 
   
           ............................................. 
                    佛祖保佑             永无BUG
            佛曰: 
                    写字楼里写字间，写字间里程序员； 
                    程序人员写程序，又拿程序换酒钱。 
                    酒醒只在网上坐，酒醉还来网下眠； 
                    酒醉酒醒日复日，网上网下年复年。 
                    但愿老死电脑间，不愿鞠躬老板前； 
                    奔驰宝马贵者趣，公交自行程序员。 
                    别人笑我忒疯癫，我笑自己命太贱； 
                    不见满街漂亮妹，哪个归得程序员？  
*/
/**
 *	mos_asm.c
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

extern void mos_asm_compile(FILE *, FILE *, int);

static void print_version(void)
{
	fprintf(stderr, "Version: %s\n", "1.0.1");
}

static void print_usage(void)
{
	fprintf(stderr, "mos_asm [-o outfile] infile\n");
	fprintf(stderr, "  -o   Set output file, if -o is not specified, the default is to put an executable file in a.out.\n");
	fprintf(stderr, "  -d   Enable output final machine code for debugging .\n");
	fprintf(stderr, "  -h   Print the help info and quit.\n");
	fprintf(stderr, "  -v   Print the version and quit.\n");
}

int main(int argc, char *argv[])
{
	FILE *ifp = NULL;
	FILE *ofp = NULL;
	int c;
	int debug = 0;
	const char * optstr = "o:dvh";

	if (1 == argc)
	{
		print_usage();
		return EINVAL;
	}

	while ((c = getopt(argc, argv, optstr)) > 0)
	{
		switch (c)
		{
			case 'o':
				ofp = fopen(optarg, "wb");
				if (NULL == ofp)
				{
					fprintf(stderr, "failed to create output file: %s\n", optarg);
					return EINVAL;
				}
				break;

			case 'd':
				debug = 1;
				break;

			case 'v':
				print_version();
				return 0;

			case 'h':
				print_usage();
				return EINVAL;
		}
	}

	if (NULL == ofp)
	{
		ofp = fopen("a.out", "wb");
		if (NULL == ofp)
		{
			fprintf(stderr, "failed to create output file: %s\n", optarg);
			return EINVAL;
		}
	}

	do
	{
		ifp = fopen(argv[argc - 1], "r");
		if (NULL == ifp)
		{
			fclose(ofp);
			fprintf(stderr, "failed to open input file: %s\n", argv[argc - 1]);
			return EINVAL;
		}
	} while(0);

	mos_asm_compile(ifp, ofp, debug);

	fclose(ifp);
	fclose(ofp);

	return 0;
}
