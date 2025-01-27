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
 *	mos_asm.l
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

/*	内存乱序模拟器汇编指令词法分析器*/

%{

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "../instruction.h"

extern void yyerror(const char *str);

%}

%option nounput
%option noinput
%option noyywrap

%option caseless
%option yylineno

%%

"ldr"		{ return OPC_LDR; }
"str"		{ return OPC_STR; }
"rmb"		{ return OPC_RMB; }
"wmb"		{ return OPC_WMB; }
"mb"		{ return OPC_MB; }
"iqf"		{ return OPC_IQF; }
"sbf"		{ return OPC_SBF; }

"mov"		{ return OPC_MOV; }

"jmp"		{ return OPC_JMP; }
"jeq"		{ return OPC_JEQ; }
"jne"		{ return OPC_JNE; }

"bug"		{ return OPC_BUG; }
"end"		{ return OPC_END; }
"sup"		{ return OPC_SUP; }
"sdw"		{ return OPC_SDW; }
"pid"		{ return OPC_PID; }
"drv"		{ return OPC_DRV; }

"add"		{ return OPC_ADD; }
"sub"		{ return OPC_SUB; }
"mul"		{ return OPC_MUL; }
"div"		{ return OPC_DIV; }
"mod"		{ return OPC_MOD; }
"and"		{ return OPC_AND; }
"or"		{ return OPC_OR; }
"xor"		{ return OPC_XOR; }
"not"		{ return OPC_NOT; }

";"			{ return ';'; }
":"			{ return ':'; }
"["			{ return '['; }
"]"			{ return ']'; }
"R0"		{ yylval.reg_id = MOS_REG_ID_R0; return reg_id; }
"R1"		{ yylval.reg_id = MOS_REG_ID_R1; return reg_id; }
"R2"		{ yylval.reg_id = MOS_REG_ID_R2; return reg_id; }
"R3"		{ yylval.reg_id = MOS_REG_ID_R3; return reg_id; }

([0][x][a-fA-F0-9]+) {
			yylval.number = strtoul(yytext, NULL, 16);
			return number;
		}
([0][b][0-1]+)	{
			yylval.number = strtol(yytext + 2, NULL, 2);
			return number;
		}
(([0])|([-+]?[1-9][0-9]*)) {
			yylval.number = strtol(yytext, NULL, 10);
			return number;
		}
([0][0-7]+)	{
			yylval.number = strtol(yytext + 1, NULL, 8);
			return number;
		}
[a-zA-Z_][a-zA-Z0-9_]+ {
			yylval.label = strdup(yytext);
			return label;
		}

"/*"([^\*]|\*[^/])*"*/"		{ /* NOP */ }
";"[^\n]*					{ /* NOP */ }
^#.*						{ /* NOP */ }
[ \t]+						{ /* NOP */ }
[ \n]+						{ /* NOP */ }

.		{
			printf("unknown character \'%s\'", yytext);
			yyerror("lex unknown character");
		}

%%

