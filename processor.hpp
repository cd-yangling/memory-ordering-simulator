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
 *	processor.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_PROCESSOR_HEADER__
#define	__MO_SIM_PROCESSOR_HEADER__

#include <cstdint>
#include <cassert>
#include <array>
#include <string>
#include "basic_type.cpp"
#include "instruction.hpp"
#include "bsb.hpp"

class processor
{
private:
	const std::uint32_t			pid;

	pr_register_t				IR;		//	程序指令寄存器

	pr_register_t				R0;		//	CPU 寄存器 0
	pr_register_t				R1;		//	CPU 寄存器 1
	pr_register_t				R2;		//	CPU 寄存器 2
	pr_register_t				R3;		//	CPU 寄存器 3

	pr_register_t				RX;

	std::array<const instruction *, 128>	ipl;

	bsb			*				bbus;	//	与 Cache 相连的后端总线
	std::string					insf;	//	指令文件名

public:
	processor() = delete;
	processor(const processor & ) = delete;
	processor & operator=(const processor &) = delete;  
	processor(const std::uint32_t i, bsb * b, const std::string & p) : pid(i), bbus(b), insf(p)
	{
		ipl.fill(nullptr);
	}

	~processor()
	{
		for (const auto& s : ipl)
			if (s != nullptr) delete s;
	}

public:
	static void processor_thread(processor * pr);

	void processor_exception(const char * f, const std::uint32_t l, const char * e)
	{
		while (1)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1000ms);
			std::printf("processor %u generated %s exception: %s %d\n", pid, e, f, l);

			assert(0);
		}
	}

private:
	//	寄存器寻址
	pr_register_t & reg_addressing(const pr_oper_num_t & opn);

private:
	//	内存相关指令
	void do_LDR_3(const pr_oper_num_t & op1, const pr_oper_num_t & op2);
	void do_STR_3(const pr_oper_num_t & op1, const pr_oper_num_t & op2);

	//	传输相关指令
	void do_MOV_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2);
	void do_MOV_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2);
	void do_RMB_0(void);
	void do_WMB_0(void);

	//	跳转相关指令
	void do_JMP_0(const pr_oper_num_t & op1);
	void do_JEQ_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_JEQ_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_JNE_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_JNE_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	//	系统相关指令
	void do_BUG_0(void);
	void do_END_0(void);
	void do_SUP_1(const pr_oper_num_t & op1);
	void do_SDW_1(const pr_oper_num_t & op1);
	void do_PID_2(const pr_oper_num_t & op1);

	//	逻辑运算指令
	void do_ADD_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_ADD_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_ADD_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	void do_SUB_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_SUB_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_SUB_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	void do_MUL_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_MUL_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_MUL_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	void do_DIV_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_DIV_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_DIV_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	void do_MOD_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_MOD_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_MOD_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	void do_AND_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_AND_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_AND_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	void do_OR_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_OR_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_OR_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	void do_XOR_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_XOR_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);
	void do_XOR_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3);

	void do_NOT_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2);
	void do_NOT_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2);

private:
	bool exec_ins(void);
	bool load_ins(void);
};

#endif	//	__MO_SIM_PROCESSOR_HEADER__

