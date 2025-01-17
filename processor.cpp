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
 *	processor.cpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#include <cstdint>
#include <array>
#include <string>
#include <semaphore>
#include <fstream>
#include <iostream>
#include "basic_type.cpp"
#include "system.hpp"
#include "instruction.hpp"
#include "bsb.hpp"
#include "processor.hpp"

pr_register_t & processor::reg_addressing(const pr_oper_num_t & opn)
{
	switch (static_cast<pr_reg_addr_t>(opn.to_number()))
	{
		case pr_reg_addr_t::R0: return R0;

		case pr_reg_addr_t::R1: return R1;

		case pr_reg_addr_t::R2: return R2;

		case pr_reg_addr_t::R3: return R3;

		default:
			processor_exception(__FILE__, __LINE__, "Invalid Opcode");
			return RX;	//	just let out compiler happy
	}
}

void processor::do_LDR_3(const pr_oper_num_t & op1, const pr_oper_num_t & op2)
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(op2)>, bit_32>, "MUST be bit_32");

	if (op2.to_number() & 0x3)
		processor_exception(__FILE__, __LINE__, "Alignment Check");
	else
		bbus->PrRd(op2.to_number() >> 2, reg_addressing(op1)), ++IR;
}

void processor::do_STR_3(const pr_oper_num_t & op1, const pr_oper_num_t & op2)
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(op1)>, bit_32>, "MUST be bit_32");

	if (op1.to_number() & 0x3)
		processor_exception(__FILE__, __LINE__, "Alignment Check");
	else
		bbus->PrWr(op1.to_number() >> 2, reg_addressing(op2)), ++IR;
}

void processor::do_RMB_0(void)
{
	++IR;
}

void processor::do_WMB_0(void)
{
#if defined(HAVE_STORE_BUFFER)
	bbus->PrWMB();
#endif	//	HAVE_STORE_BUFFER
	++IR;
}

void processor::do_MB_0(void)
{
#if defined(HAVE_STORE_BUFFER)
	bbus->PrMB();
#endif	//	HAVE_STORE_BUFFER
	++IR;
}

void processor::do_IQF_0(void)
{
	++IR;
}

void processor::do_SBF_0(void)
{
#if defined(HAVE_STORE_BUFFER)
	bbus->PrSBF();
#endif	//	HAVE_STORE_BUFFER
	++IR;
}

void processor::do_MOV_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2)
{
	reg_addressing(op1) = op2, ++IR;
}

void processor::do_MOV_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2)
{
	do_MOV_1(op1, reg_addressing(op2));
}

void processor::do_JMP_0(const pr_oper_num_t & op1)
{
	IR = op1;
}

void processor::do_JEQ_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	(reg_addressing(op2) == op3) ? IR = op1 : ++IR;
}

void processor::do_JEQ_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_JEQ_1(op1, op2, reg_addressing(op3));
}

void processor::do_JNE_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	(reg_addressing(op2) != op3) ? IR = op1 : ++IR;
}

void processor::do_JNE_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_JNE_1(op1, op2, reg_addressing(op3));
}

void processor::do_BUG_0(void)
{
	std::printf(
		"The BUG instruction has been processed by processor %u"
		"  (IR: 0x%zx R0: 0x%zx: R1: 0x%zx R2: 0x%zx R3: 0x%zx)",
		pid, IR.to_number(), R0.to_number(), R1.to_number(), R2.to_number(), R3.to_number());
}

void processor::do_END_0(void)
{
	std::printf(
		"The END instruction has been processed by processor %u",
		pid);
}

void processor::do_SUP_1(const pr_oper_num_t & op1)
{
	std::binary_semaphore * sem = system::sem_ref(op1.to_number());

	if (sem)
	{
		sem->release(); ++IR;
	}
	else
	{
		processor_exception(__FILE__, __LINE__, "Invalid Opcode");
	}
}

void processor::do_SDW_1(const pr_oper_num_t & op1)
{
	std::binary_semaphore * sem = system::sem_ref(op1.to_number());

	if (sem)
	{
		sem->acquire(); ++IR;
	}
	else
	{
		processor_exception(__FILE__, __LINE__, "Invalid Opcode");
	}
}

void processor::do_PID_2(const pr_oper_num_t & op1)
{
	reg_addressing(op1) = pid; ++IR;
}

void processor::do_ADD_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	reg_addressing(op1) = op2 + op3; ++IR;
}

void processor::do_ADD_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_ADD_0(op1, reg_addressing(op2), op3);
}

void processor::do_ADD_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_ADD_1(op1, op2, reg_addressing(op3));
}

void processor::do_SUB_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	reg_addressing(op1) = op2 - op3; ++IR;
}

void processor::do_SUB_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_SUB_0(op1, reg_addressing(op2), op3);
}

void processor::do_SUB_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_SUB_1(op1, op2, reg_addressing(op3));
}

void processor::do_MUL_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	reg_addressing(op1) = op2 * op3; ++IR;
}

void processor::do_MUL_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_MUL_0(op1, reg_addressing(op2), op3);
}

void processor::do_MUL_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_MUL_1(op1, op2, reg_addressing(op3));
}

void processor::do_DIV_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	if (0 == op3.to_number())
	{
		processor_exception(__FILE__, __LINE__, "Division Error");
	}

	reg_addressing(op1) = op2 / op3; ++IR;
}

void processor::do_DIV_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_DIV_0(op1, reg_addressing(op2), op3);
}

void processor::do_DIV_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_DIV_1(op1, op2, reg_addressing(op3));
}

void processor::do_MOD_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	if (0 == op3.to_number())
	{
		processor_exception(__FILE__, __LINE__, "Division Error");
	}

	reg_addressing(op1) = op2 % op3; ++IR;
}

void processor::do_MOD_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_MOD_0(op1, reg_addressing(op2), op3);
}

void processor::do_MOD_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_MOD_1(op1, op2, reg_addressing(op3));
}

void processor::do_AND_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	reg_addressing(op1) = op2 & op3; ++IR;
}

void processor::do_AND_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_AND_0(op1, reg_addressing(op2), op3);
}

void processor::do_AND_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_AND_1(op1, op2, reg_addressing(op3));
}

void processor::do_OR_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	reg_addressing(op1) = op2 | op3; ++IR;
}

void processor::do_OR_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_OR_0(op1, reg_addressing(op2), op3);
}

void processor::do_OR_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_OR_1(op1, op2, reg_addressing(op3));
}

void processor::do_XOR_0(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	reg_addressing(op1) = op2 ^ op3; ++IR;
}

void processor::do_XOR_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_XOR_0(op1, reg_addressing(op2), op3);
}

void processor::do_XOR_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2, const pr_oper_num_t & op3)
{
	do_XOR_1(op1, op2, reg_addressing(op3));
}

void processor::do_NOT_1(const pr_oper_num_t & op1, const pr_oper_num_t & op2)
{
	reg_addressing(op1) = ~op2; ++IR;
}

void processor::do_NOT_2(const pr_oper_num_t & op1, const pr_oper_num_t & op2)
{
	do_NOT_1(op1, reg_addressing(op2));
}

bool processor::exec_ins(void)
{
	if (IR >= ipl.size())
	{
		processor_exception(__FILE__, __LINE__, "Bound Range Exceeded");
		return false;
	}

	const instruction * ins = ipl[IR.to_number()];

	switch (ins->get_opc())
	{
		case instruction_t::LDR_3:
			do_LDR_3(ins->get_op1(), ins->get_op2());
			return true;

		case instruction_t::STR_3:
			do_STR_3(ins->get_op1(), ins->get_op2());
			return true;

		case instruction_t::RMB_0:
			do_RMB_0();
			return true;

		case instruction_t::WMB_0:
			do_WMB_0();
			return true;

		case instruction_t::MB_0:
			do_MB_0();
			return true;

		case instruction_t::IQF_0:
			do_IQF_0();
			return true;

		case instruction_t::SBF_0:
			do_SBF_0();
			return true;

		case instruction_t::MOV_1:
			do_MOV_1(ins->get_op1(), ins->get_op2());
			return true;

		case instruction_t::MOV_2:
			do_MOV_2(ins->get_op1(), ins->get_op2());
			return true;

		case instruction_t::JMP_0:
			do_JMP_0(ins->get_op1());
			return true;

		case instruction_t::JEQ_1:
			do_JEQ_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::JEQ_2:
			do_JEQ_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::JNE_1:
			do_JNE_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::JNE_2:
			do_JNE_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::BUG_0:
			do_BUG_0();
			return false;

		case instruction_t::END_0:
			do_END_0();
			return false;

		case instruction_t::SUP_1:
			do_SUP_1(ins->get_op1());
			return true;

		case instruction_t::SDW_1:
			do_SDW_1(ins->get_op1());
			return true;

		case instruction_t::PID_2:
			do_PID_2(ins->get_op1());
			return true;

		case instruction_t::ADD_1:
			do_ADD_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::ADD_2:
			do_ADD_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::SUB_1:
			do_SUB_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::SUB_2:
			do_SUB_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::MUL_1:
			do_MUL_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::MUL_2:
			do_MUL_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::DIV_1:
			do_DIV_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::DIV_2:
			do_DIV_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::MOD_1:
			do_MOD_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::MOD_2:
			do_MOD_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::AND_1:
			do_AND_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::AND_2:
			do_AND_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::OR_1:
			do_OR_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::OR_2:
			do_OR_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::XOR_1:
			do_XOR_1(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::XOR_2:
			do_XOR_2(ins->get_op1(), ins->get_op2(), ins->get_op3());
			return true;

		case instruction_t::NOT_1:
			do_NOT_1(ins->get_op1(), ins->get_op2());
			return true;

		case instruction_t::NOT_2:
			do_NOT_2(ins->get_op1(), ins->get_op2());
			return true;

		default:
			processor_exception(__FILE__, __LINE__, "Invalid Opcode");
			return false;
	}
}

void processor::processor_thread(processor * pr)
{
	bool running = true;
	std::uint64_t nr_ins = 0;

	auto t0 = system::get_count_tick();

	if (pr->load_ins())
	{
		while (running)
		{
			running = pr->exec_ins();
			nr_ins++;

			if (0 == nr_ins % 100000)
			{
				auto t1 = system::get_count_tick();

				std::printf("  ===> Processor: %u <%zu instruction be processed Total Consume %lu ms>\n", pr->pid, nr_ins, t1 - t0);
			}
		}
	}

	auto t1 = system::get_count_tick();

	std::printf("  ===> Processor: %u <%zu instruction be processed Total Consume %lu ms>\n", pr->pid, nr_ins, t1 - t0);
}

bool processor::load_ins(void)
{
	std::ifstream file(insf, std::ios::in | std::ios::binary | std::ios::ate);

	if (!file.is_open())
	{
		std::cout << "Unable to open file: " << insf << std::endl;
		return false;
	}

	int size = file.tellg();

	if (0 != size % 16)
	{
		std::cout << "Bad instruction file: " << insf << std::endl;
		file.close();
		return false;
	}

	file.seekg (0, std::ios::beg);

	for (int i = 0; i < (size / 16); ++i)
	{
		instruction_t opc;
		std::uint32_t op1;
		std::uint32_t op2;
		std::uint32_t op3;

		file.read(reinterpret_cast<char *>(&opc), sizeof(std::uint32_t));
		file.read(reinterpret_cast<char *>(&op1), sizeof(std::uint32_t));
		file.read(reinterpret_cast<char *>(&op2), sizeof(std::uint32_t));
		file.read(reinterpret_cast<char *>(&op3), sizeof(std::uint32_t));

		ipl[i] = new instruction(opc, op1, op2, op3);
	}

	file.close();

	if (ipl.empty())
	{
		std::cout << "Empty instruction file: " << insf << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}

