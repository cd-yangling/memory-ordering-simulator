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
 *	main.cpp
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
#include <unistd.h>
#include "basic_type.cpp"
#include "bsb.hpp"
#include "fsb.hpp"
#include "memory.hpp"
#include "cache.hpp"
#include "processor.hpp"
#include "system.hpp"
#if defined(HAVE_BSB_TEST)
#include "test/bsb_test.hpp"
#endif
#if defined(HAVE_FSB_TEST)
#include "test/fsb_test.hpp"
#endif

static void print_version(void)
{
	fprintf(stderr, "Version: %s\n", "1.0.1");
}

static void print_usage(void)
{
	fprintf(stderr, "mos_run [-0 infile | -1 infile | -2 infile | -3 infile]\n");
	fprintf(stderr, "  -0   Set the instruction file to be processed by processor 0, Default file is a.out.\n");
	fprintf(stderr, "  -1   Set the instruction file to be processed by processor 1, Default file is a.out.\n");
	fprintf(stderr, "  -2   Set the instruction file to be processed by processor 2, Default file is a.out.\n");
	fprintf(stderr, "  -3   Set the instruction file to be processed by processor 3, Default file is a.out.\n");
	fprintf(stderr, "  -h   Print the help info and quit.\n");
	fprintf(stderr, "  -v   Print the version and quit.\n");
}

std::binary_semaphore system::sem_a { 0 };
std::binary_semaphore system::sem_b { 0 };
std::binary_semaphore system::sem_c { 0 };
std::binary_semaphore system::sem_d { 0 };
std::binary_semaphore system::sem_e { 0 };
std::binary_semaphore system::sem_f { 0 };

int main(int argc, char * argv[])
{
	int c;
	const char * optstr = "0:1:2:3:t:vh";
	const unsigned int nr_cpus = 4;
	const char * p0 = "a.out";
	const char * p1 = "a.out";
	const char * p2 = "a.out";
	const char * p3 = "a.out";
	const char * tf = NULL;

	while ((c = getopt(argc, argv, optstr)) > 0)
	{
		switch (c)
		{
			case '0':
				p0 = optarg;
				break;

			case '1':
				p1 = optarg;
				break;

			case '2':
				p2 = optarg;
				break;

			case '3':
				p3 = optarg;
				break;

			case 't':
				tf = optarg;
				break;

			case 'v':
				print_version();
				return 0;

			case 'h':
				print_usage();
				return EINVAL;
		}
	}

	if (tf)
	{
#if defined(HAVE_BSB_TEST)
	if (0 == strcmp("bsb", tf))
		bsb_tester::bsb_tester_main();
#endif	//	HAVE_BSB_TEST

#if defined(HAVE_FSB_TEST)
	if (0 == strcmp("fsb", tf))
		fsb_tester::fsb_tester_main();
#endif	//	HAVE_FSB_TEST
	}
	else
	{
		bsb bsb_obj_0;
		bsb bsb_obj_1;
		bsb bsb_obj_2;
		bsb bsb_obj_3;

		fsb fsb_obj_X;

		memory mem_obj(nr_cpus, &fsb_obj_X);

		cache cache_obj_0(0, &bsb_obj_0, &fsb_obj_X);
		cache cache_obj_1(1, &bsb_obj_1, &fsb_obj_X);
		cache cache_obj_2(2, &bsb_obj_2, &fsb_obj_X);
		cache cache_obj_3(3, &bsb_obj_3, &fsb_obj_X);

		processor cpu_obj_0(0, &bsb_obj_0, p0);
		processor cpu_obj_1(1, &bsb_obj_1, p1);
		processor cpu_obj_2(2, &bsb_obj_2, p2);
		processor cpu_obj_3(3, &bsb_obj_3, p3);

		//	启动(内存工作线程)
		std::thread t_memory(memory::snoop_thread, &mem_obj);

		//	启动(缓存snoop线程: 处理 fsb 事务)
		std::thread t_cache_00(cache::snoop_thread, &cache_obj_0);
		std::thread t_cache_10(cache::snoop_thread, &cache_obj_1);
		std::thread t_cache_20(cache::snoop_thread, &cache_obj_2);
		std::thread t_cache_30(cache::snoop_thread, &cache_obj_3);

		//	启动(缓存processor侧的线程: 处理 bsb 事务)
		std::thread t_cache_01(cache::cache_thread, &cache_obj_0);
		std::thread t_cache_11(cache::cache_thread, &cache_obj_1);
		std::thread t_cache_21(cache::cache_thread, &cache_obj_2);
		std::thread t_cache_31(cache::cache_thread, &cache_obj_3);

		//	启动 processor 指令处理线程
		std::thread t_cpu_0(processor::processor_thread, &cpu_obj_0);
		std::thread t_cpu_1(processor::processor_thread, &cpu_obj_1);
		std::thread t_cpu_2(processor::processor_thread, &cpu_obj_2);
		std::thread t_cpu_3(processor::processor_thread, &cpu_obj_3);

		t_cpu_0.join();
		t_cpu_1.join();
		t_cpu_2.join();
		t_cpu_3.join();

		t_cache_01.join();
		t_cache_11.join();
		t_cache_21.join();
		t_cache_31.join();

		t_cache_00.join();
		t_cache_10.join();
		t_cache_20.join();
		t_cache_30.join();

		t_memory.join();
	}

	return 0;
}

