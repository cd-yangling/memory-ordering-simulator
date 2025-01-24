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
 *	mos_asm.y
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

/*	内存乱序模拟器汇编指令语法分析器*/

%{

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "../instruction.h"

extern FILE *yyin;
extern int yylineno;
extern int yylex(void);
extern void yyerror(const char *str);

static void create_instr(
	unsigned int opc, unsigned int op1,
	unsigned int op2, unsigned int op3, char * label);
static void create_label(char *);

%}

%union
{
	char *label;
	unsigned int number;
	unsigned int reg_id;
};

%token OPC_LDR OPC_STR OPC_RMB OPC_WMB OPC_MB OPC_IQF OPC_SBF
%token OPC_MOV
%token OPC_JMP OPC_JEQ OPC_JNE
%token OPC_BUG OPC_END OPC_SUP OPC_SDW OPC_PID
%token OPC_ADD OPC_SUB OPC_MUL OPC_DIV OPC_MOD OPC_AND OPC_OR OPC_XOR OPC_NOT

%token ';' ':' '[' ']'

%token number label reg_id

%type <label> label
%type <number> number
%type <reg_id> reg_id

%%

prog
	: line
	| prog line
	;

line
	: instr
	| labelled_instr
	;

labelled_instr
	: labelled instr
	;

instr
	: ldr
	| str
	| rmb
	| wmb
	| mb
	| iqf
	| sbf
	| mov
	| jmp
	| jeq
	| jne
	| bug
	| end
	| sup
	| sdw
	| pid
	| add
	| sub
	| mul
	| div
	| mod
	| and
	| or
	| xor
	| not
	;

labelled
	: label ':' { create_label($1); }
	;

ldr
	: OPC_LDR reg_id '[' number ']' { create_instr(MOS_INS_LDR_3, $2, $4, 0, NULL); }
	;

str
	: OPC_STR '[' number ']' reg_id { create_instr(MOS_INS_STR_3, $3, $5, 0, NULL); }
	;

ldr
	: OPC_LDR reg_id '[' reg_id ']' { create_instr(MOS_INS_LDR_4, $2, $4, 0, NULL); }
	;

str
	: OPC_STR '[' reg_id ']' reg_id { create_instr(MOS_INS_STR_4, $3, $5, 0, NULL); }
	;

rmb
	: OPC_RMB { create_instr(MOS_INS_RMB_0, 0, 0, 0, NULL); }
	;

wmb
	: OPC_WMB { create_instr(MOS_INS_WMB_0, 0, 0, 0, NULL); }
	;

mb
	: OPC_MB { create_instr(MOS_INS_MB_0, 0, 0, 0, NULL); }
	;

iqf
	: OPC_IQF { create_instr(MOS_INS_IQF_0, 0, 0, 0, NULL); }
	;

sbf
	: OPC_SBF { create_instr(MOS_INS_SBF_0, 0, 0, 0, NULL); }
	;

mov
	: OPC_MOV reg_id number { create_instr(MOS_INS_MOV_1, $2, $3, 0, NULL); }
	| OPC_MOV reg_id reg_id { create_instr(MOS_INS_MOV_2, $2, $3, 0, NULL); }
	;

jmp
	: OPC_JMP label { create_instr(MOS_INS_JMP_0, 0, 0, 0, $2); }
	;

jeq
	: OPC_JEQ label reg_id number { create_instr(MOS_INS_JEQ_1, 0, $3, $4, $2); }
	| OPC_JEQ label reg_id reg_id { create_instr(MOS_INS_JEQ_2, 0, $3, $4, $2); }
	;

jne
	: OPC_JNE label reg_id number { create_instr(MOS_INS_JNE_1, 0, $3, $4, $2); }
	| OPC_JNE label reg_id reg_id { create_instr(MOS_INS_JNE_2, 0, $3, $4, $2); }
	;

bug
	: OPC_BUG { create_instr(MOS_INS_BUG_0, 0, 0, 0, NULL); }
	;

end
	: OPC_END { create_instr(MOS_INS_END_0, 0, 0, 0, NULL); }
	;

sup
	: OPC_SUP number { create_instr(MOS_INS_SUP_1, $2, 0, 0, NULL); }
	;

sdw
	: OPC_SDW number { create_instr(MOS_INS_SDW_1, $2, 0, 0, NULL); }
	;

pid
	: OPC_PID reg_id { create_instr(MOS_INS_PID_2, $2, 0, 0, NULL); }

add
	: OPC_ADD reg_id reg_id number { create_instr(MOS_INS_ADD_1, $2, $3, $4, NULL); }
	| OPC_ADD reg_id reg_id reg_id { create_instr(MOS_INS_ADD_2, $2, $3, $4, NULL); }
	;

sub
	: OPC_SUB reg_id reg_id number { create_instr(MOS_INS_SUB_1, $2, $3, $4, NULL); }
	| OPC_SUB reg_id reg_id reg_id { create_instr(MOS_INS_SUB_2, $2, $3, $4, NULL); }
	;

mul
	: OPC_MUL reg_id reg_id number { create_instr(MOS_INS_MUL_1, $2, $3, $4, NULL); }
	| OPC_MUL reg_id reg_id reg_id { create_instr(MOS_INS_MUL_2, $2, $3, $4, NULL); }
	;

div
	: OPC_DIV reg_id reg_id number { create_instr(MOS_INS_DIV_1, $2, $3, $4, NULL); }
	| OPC_DIV reg_id reg_id reg_id { create_instr(MOS_INS_DIV_2, $2, $3, $4, NULL); }
	;

mod
	: OPC_MOD reg_id reg_id number { create_instr(MOS_INS_MOD_1, $2, $3, $4, NULL); }
	| OPC_MOD reg_id reg_id reg_id { create_instr(MOS_INS_MOD_2, $2, $3, $4, NULL); }
	;

and
	: OPC_AND reg_id reg_id number { create_instr(MOS_INS_AND_1, $2, $3, $4, NULL); }
	| OPC_AND reg_id reg_id reg_id { create_instr(MOS_INS_AND_2, $2, $3, $4, NULL); }
	;

or
	: OPC_OR reg_id reg_id number { create_instr(MOS_INS_OR_1, $2, $3, $4, NULL); }
	| OPC_OR reg_id reg_id reg_id { create_instr(MOS_INS_OR_2, $2, $3, $4, NULL); }
	;

xor
	: OPC_XOR reg_id reg_id number { create_instr(MOS_INS_XOR_1, $2, $3, $4, NULL); }
	| OPC_XOR reg_id reg_id reg_id { create_instr(MOS_INS_XOR_2, $2, $3, $4, NULL); }
	;

not
	: OPC_NOT reg_id number { create_instr(MOS_INS_NOT_1, $2, $3, 0, NULL); }
	| OPC_NOT reg_id reg_id { create_instr(MOS_INS_NOT_2, $2, $3, 0, NULL); }
	;

%%

#define	MOS_MAXINSTR	(128)
static int curr_instr = 0;
static struct mos_instr
{
	unsigned int	opc;
	unsigned int	op1;
	unsigned int	op2;
	unsigned int	op3;
	char *			label;
} instr_set[MOS_MAXINSTR] = { 0 };

static int curr_label = 0;
static struct mos_label
{
	char	*	str;
	unsigned int pos;
} label_set[MOS_MAXINSTR] = { 0 };

static void mos_oops_max(void)
{
	if (curr_instr >= MOS_MAXINSTR)
	{
		fprintf(stderr, "only max %u insns allowed!\n", MOS_MAXINSTR);
		exit(1);
	}
}

static void create_instr(
	unsigned int opc, unsigned int op1,
	unsigned int op2, unsigned int op3, char * label)
{
	mos_oops_max();

	instr_set[curr_instr].opc = opc;
	instr_set[curr_instr].op1 = op1;
	instr_set[curr_instr].op2 = op2;
	instr_set[curr_instr].op3 = op3;
	instr_set[curr_instr].label = label;

	curr_instr++;
}

static unsigned int lookup_label(const char *label)
{
	int i;

	for (i = 0; i < curr_label; ++i)
	{
		if (0 == strcmp(label_set[i].str, label))
			return i;
	}

	return MOS_MAXINSTR;
}

static void create_label(char *label)
{
	if (lookup_label(label) < MOS_MAXINSTR)
	{
		fprintf(stderr, "duplicate label '%s' at line %u\n", label, yylineno);
		exit(1);
	}
	else
	{
		label_set[curr_label].str = label;
		label_set[curr_label].pos = curr_instr;
		curr_label++;
	}
}

static void backfill_label(FILE * ofp, int debug)
{
	int i;

	for (i = 0; i < curr_instr; i++)
	{
		switch (instr_set[i].opc)
		{
			case MOS_INS_JMP_0:
			case MOS_INS_JEQ_1:
			case MOS_INS_JEQ_2:
			case MOS_INS_JNE_1:
			case MOS_INS_JNE_2:
			{
				unsigned int k = lookup_label(instr_set[i].label);

				if (k >= MOS_MAXINSTR)
				{
					fprintf(stderr, "label '%s' used but not defined\n", instr_set[i].label);
					exit(1);
				}

				instr_set[i].op1 = label_set[k].pos;
			}
			break;
		}

		if (debug)
		{
			fprintf(stdout, "(%03d) 0x%08X 0x%08X 0x%08X 0x%08X\n", i,
				instr_set[i].opc, instr_set[i].op1,
				instr_set[i].op2, instr_set[i].op3);
		}

		if (sizeof(int) * 4 != fwrite(instr_set + i, 1, sizeof(int) * 4, ofp))
		{
			fprintf(stderr, "write outfile failed\n");
			exit(1);
		}
	}
}

void mos_asm_compile(FILE *ifp, FILE *ofp, int debug)
{
	yyin = ifp;

	yyparse();

	backfill_label(ofp, debug);
}

void yyerror(const char *str)
{
	fprintf(stderr, "error: %s at line %d\n", str, yylineno);
	exit(1);
}
