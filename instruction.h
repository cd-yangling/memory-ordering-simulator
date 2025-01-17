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
 *	instruction.h
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	___MO_SIM_INSTRUCTION_C_HEADER__
#define	___MO_SIM_INSTRUCTION_C_HEADER__

/**
 * 本文件提供给汇编编译器使用,也同时提供给模拟器使用
 */

/**
 * 处理器提供的指令集包括以下几个(为了实验 memory ordering 这个几个指令足够了)
 *
 * 机器/汇编 指令定义
 *
 * REG := [ R0 | R1 | R2 | R3 ]
 * num := [ 0x<数字> | 0b<数字> | <数字> | 0<数值> ]
 *
 * 所有的指令操作均遵守下面的原则
 *
 * OPC DST SRC1 SRC2 (操作码 目标 源1 源2)
 *
 * 其中 SRC2 可选, 部分指令不需要
 *
 * ======== ======== ======== ======== ========
 * 操作码   目的     源1      源2      描述
 * ======== ======== ======== ======== ========
 *
 * a: 内存相关指令
 *
 * LDR      REG      [num]             ;REG = *num
 * STR      [num]    REG               ;*num = REG
 * RMB                                 ; 读内存栅栏(确保读顺序)
 * WMB                                 ; 写内存栅栏(确保写顺序)
 * MB                                  ; 读写内存栅栏(确保读写顺序,同时确保可见性)
 * IQF                                 ; 读内存栅栏(确保读顺序,同时确保可见性)
 * SBF                                 ; 写内存栅栏(确保写顺序,同时确保可见性)
 *
 * b: 传输相关指令
 *
 * MOV      REG      num               ;REG = num
 * MOV      REG      REG               ;REG = REG
 *
 * c: 跳转相关指令
 *
 * JMP      label
 * JEQ      label    REG      num      ;Jump to label on REG == num
 * JEQ      label    REG      REG      ;Jump to label on REG == REG
 * JNE      label    REG      num      ;Jump to label on REG != num
 * JNE      label    REG      REG      ;Jump to label on REG != REG
 *
 * d: 系统相关指令
 *
 * BUG                                 ;程序异常结束
 * END                                 ;程序正常结束
 * SUP      num                        ;系统第 num 个信号量   Up 操作(主要用于构造并行运行测试环境)
 * SDW      num                        ;系统第 num 个信号量 Down 操作(主要用于构造并行运行测试环境)
 * PID      REG                        ;提取processor ID 到指定寄存器
 *
 * e: 逻辑运算指令
 *
 * ADD      REG      REG      num      ;REG = REG + num
 * ADD      REG      REG      REG      ;REG = REG + REG
 * SUB      REG      REG      num      ;REG = REG - num
 * SUB      REG      REG      REG      ;REG = REG - REG
 * MUL      REG      REG      num      ;REG = REG * num
 * MUL      REG      REG      REG      ;REG = REG * REG
 * DIV      REG      REG      num      ;REG = REG / num
 * DIV      REG      REG      REG      ;REG = REG / REG
 * MOD      REG      REG      num      ;REG = REG % num
 * MOD      REG      REG      REG      ;REG = REG % REG
 *
 * AND      REG      REG      num      ;REG = REG & num
 * AND      REG      REG      REG      ;REG = REG & REG
 * OR       REG      REG      num      ;REG = REG | num
 * OR       REG      REG      REG      ;REG = REG | REG
 * XOR      REG      REG      num      ;REG = REG ^ num
 * XOR      REG      REG      REG      ;REG = REG ^ REG
 * NOT      REG      num               ;REG = ~num
 * NOT      REG      REG               ;REG = ~REG
 */

#ifdef __cplusplus
extern "C" {
#endif

//	指令代码定义
#define	MOS_INS_LDR_3		(0x000a0103)
#define	MOS_INS_STR_3		(0x000a0203)
#define	MOS_INS_RMB_0		(0x000a0300)
#define	MOS_INS_WMB_0		(0x000a0400)
#define	MOS_INS_MB_0		(0x000a0500)
#define	MOS_INS_IQF_0		(0x000a0600)
#define	MOS_INS_SBF_0		(0x000a0700)

#define	MOS_INS_MOV_1		(0x000b0101)
#define	MOS_INS_MOV_2		(0x000b0102)

#define	MOS_INS_JMP_0		(0x000c0100)
#define	MOS_INS_JEQ_1		(0x000c0201)
#define	MOS_INS_JEQ_2		(0x000c0202)
#define	MOS_INS_JNE_1		(0x000c0301)
#define	MOS_INS_JNE_2		(0x000c0302)

#define	MOS_INS_BUG_0		(0x000d0100)
#define	MOS_INS_END_0		(0x000d0200)
#define	MOS_INS_SUP_1		(0x000d0301)
#define	MOS_INS_SDW_1		(0x000d0401)
#define	MOS_INS_PID_2		(0x000d0502)

#define	MOS_INS_ADD_1		(0x000e0101)
#define	MOS_INS_ADD_2		(0x000e0102)
#define	MOS_INS_SUB_1		(0x000e0201)
#define	MOS_INS_SUB_2		(0x000e0202)
#define	MOS_INS_MUL_1		(0x000e0301)
#define	MOS_INS_MUL_2		(0x000e0302)
#define	MOS_INS_DIV_1		(0x000e0401)
#define	MOS_INS_DIV_2		(0x000e0402)
#define	MOS_INS_MOD_1		(0x000e0501)
#define	MOS_INS_MOD_2		(0x000e0502)
#define	MOS_INS_AND_1		(0x000e0601)
#define	MOS_INS_AND_2		(0x000e0602)
#define	MOS_INS_OR_1		(0x000e0701)
#define	MOS_INS_OR_2		(0x000e0702)
#define	MOS_INS_XOR_1		(0x000e0801)
#define	MOS_INS_XOR_2		(0x000e0802)
#define	MOS_INS_NOT_1		(0x000e0901)
#define	MOS_INS_NOT_2		(0x000e0902)

#define	MOS_REG_ID_R0		(0x000000a0)
#define	MOS_REG_ID_R1		(0x000000a1)
#define	MOS_REG_ID_R2		(0x000000a2)
#define	MOS_REG_ID_R3		(0x000000a3)

#ifdef __cplusplus
}
#endif
#endif	//	___MO_SIM_INSTRUCTION_C_HEADER__

