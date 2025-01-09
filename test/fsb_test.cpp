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
 *	test/fsb_test.cpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#if defined(HAVE_FSB_TEST)

#include <cstdio>
#include <cassert>
#include <thread>
#include <array>
#include "../basic_type.cpp"
#include "../bsb.hpp"
#include "../fsb.hpp"
#include "../cache.hpp"
#include "../fsb.hpp"
#include "../memory.hpp"
#include "fsb_test.hpp"

void fsb_tester::fsb_tester_main(void)
{
	const std::uint32_t nr_cpus = 2;
	std::uint32_t i;
	fsb * sys_bus;
	memory * sys_mem;
	std::array<bsb *, nr_cpus> pr_bus_array;
	std::array<cache *, nr_cpus> pr_cache_array;

	//	初始化一个 nr_cpus 的模拟架构
	sys_bus = new fsb;
	sys_mem = new memory(nr_cpus, sys_bus);
	for (i = 0; i < nr_cpus; i++)
	{
		pr_bus_array[i] = new bsb;
		pr_cache_array[i] = new cache(i, pr_bus_array[i], sys_bus);
	}

	//	启动测试运行环境
	std::thread t0f(memory::snoop_thread, sys_mem);

	std::thread t1f(cache::snoop_thread, pr_cache_array[0]);
	std::thread t1b(cache::cache_thread, pr_cache_array[0]);

	std::thread t2f(cache::snoop_thread, pr_cache_array[1]);
	std::thread t2b(cache::cache_thread, pr_cache_array[1]);

	pr_data_val_t r;

	std::printf("basic logic testing\n");
	//	基本逻辑测试
	//	CPU 0 R 0x00000000
	pr_bus_array[0]->PrRd(0x00000000, r);
	assert(0 == r.to_number());
	pr_cache_array[0]->assert_cache_line(0x00000000, 0, cache_state_t::MESI_CACHE_E);
	pr_cache_array[1]->assert_cache_line(0x00000000);

	//	CPU 1 R 0x00000000
	pr_bus_array[1]->PrRd(0x00000000, r);
	assert(0 == r.to_number());
	pr_cache_array[0]->assert_cache_line(0x00000000, 0, cache_state_t::MESI_CACHE_S);
	pr_cache_array[1]->assert_cache_line(0x00000000, 0, cache_state_t::MESI_CACHE_S);

	//	CPU 0 W 0x00000000 --> 2
	pr_bus_array[0]->PrWr(0x00000000, 2);
	pr_cache_array[0]->assert_cache_line(0x00000000, 2, cache_state_t::MESI_CACHE_M);
	pr_cache_array[1]->assert_cache_line(0x00000000, 0, cache_state_t::MESI_CACHE_I);

	std::printf("write back testing\n");
	//	进行压力测试
	//	循环 CPU 0 write back
	const std::uint32_t nr_wb_count = 1000000;
	pr_bus_array[1]->PrRd(0x00000400, r);
	assert(0 == r.to_number());

	pr_bus_array[1]->PrRd(0x00000800, r);
	assert(0 == r.to_number());

	pr_bus_array[0]->PrWr(0x00000400, 0);
	pr_bus_array[0]->PrWr(0x00000800, 0);

	for (i = 1; i < nr_wb_count; i++)
	{
		pr_bus_array[0]->PrWr(0x00000400, i);

		pr_cache_array[0]->assert_cache_line(0x00000400, i, cache_state_t::MESI_CACHE_M);
		pr_cache_array[1]->assert_cache_line(0x00000400);

//		sys_mem->assert_memory(0x00000400, i - 1);

		pr_bus_array[0]->PrWr(0x00000800, i);

		pr_cache_array[0]->assert_cache_line(0x00000800, i, cache_state_t::MESI_CACHE_M);
		pr_cache_array[1]->assert_cache_line(0x00000800, 0, cache_state_t::MESI_CACHE_I);

//		sys_mem->assert_memory(0x00000400, i);

		if (0 == i % 10000)
			std::printf("CPU write back test:%i\n", i);
	}

	std::printf("test done\n");
	t0f.join();

	t1f.join();
	t1b.join();

	t2f.join();
	t2b.join();
}

#endif	//	HAVE_FSB_TEST

