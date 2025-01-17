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
 *	instruction.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_INSTRUCTION_HEADER__
#define	__MO_SIM_INSTRUCTION_HEADER__

#include <cstdint>
#include "basic_type.cpp"
#include "instruction.h"

enum class instruction_t
{
	LDR_3	= MOS_INS_LDR_3,
	STR_3	= MOS_INS_STR_3,
	RMB_0	= MOS_INS_RMB_0,
	WMB_0	= MOS_INS_WMB_0,
	MB_0	= MOS_INS_MB_0,
	IQF_0	= MOS_INS_IQF_0,
	SBF_0	= MOS_INS_SBF_0,

	MOV_1	= MOS_INS_MOV_1,
	MOV_2	= MOS_INS_MOV_2,

	JMP_0	= MOS_INS_JMP_0,
	JEQ_1	= MOS_INS_JEQ_1,
	JEQ_2	= MOS_INS_JEQ_2,
	JNE_1	= MOS_INS_JNE_1,
	JNE_2	= MOS_INS_JNE_2,

	BUG_0	= MOS_INS_BUG_0,
	END_0	= MOS_INS_END_0,
	SUP_1	= MOS_INS_SUP_1,
	SDW_1	= MOS_INS_SDW_1,
	PID_2	= MOS_INS_PID_2,

	ADD_1	= MOS_INS_ADD_1,
	ADD_2	= MOS_INS_ADD_2,
	SUB_1	= MOS_INS_SUB_1,
	SUB_2	= MOS_INS_SUB_2,
	MUL_1	= MOS_INS_MUL_1,
	MUL_2	= MOS_INS_MUL_2,
	DIV_1	= MOS_INS_DIV_1,
	DIV_2	= MOS_INS_DIV_2,
	MOD_1	= MOS_INS_MOD_1,
	MOD_2	= MOS_INS_MOD_2,
	AND_1	= MOS_INS_AND_1,
	AND_2	= MOS_INS_AND_2,
	OR_1	= MOS_INS_OR_1,
	OR_2	= MOS_INS_OR_2,
	XOR_1	= MOS_INS_XOR_1,
	XOR_2	= MOS_INS_XOR_2,
	NOT_1	= MOS_INS_NOT_1,
	NOT_2	= MOS_INS_NOT_2,
};

enum class pr_reg_addr_t
{
	R0		= MOS_REG_ID_R0,
	R1		= MOS_REG_ID_R1,
	R2		= MOS_REG_ID_R2,
	R3		= MOS_REG_ID_R3,
};

class instruction
{
private:
	instruction_t				opc;
	pr_oper_num_t				op1;
	pr_oper_num_t				op2;
	pr_oper_num_t				op3;

public:
	instruction() = delete;
	instruction(const instruction & ) = delete;
	instruction & operator=(const instruction &) = delete;  
	instruction(
		const instruction_t & v0, const pr_oper_num_t & v1,
		const pr_oper_num_t & v2, const pr_oper_num_t & v3)
	{
		opc = v0; op1 = v1, op2 = v2, op3 = v3;
	}

public:
	const instruction_t & get_opc(void) const { return opc; }
	const pr_oper_num_t & get_op1(void) const { return op1; }
	const pr_oper_num_t & get_op2(void) const { return op2; }
	const pr_oper_num_t & get_op3(void) const { return op3; }
};

#endif	//	__MO_SIM_INSTRUCTION_HEADER__

