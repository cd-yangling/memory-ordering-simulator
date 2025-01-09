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
 *	test/bsb_test.cpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#if defined(HAVE_BSB_TEST)

#include <cstdio>
#include <cassert>
#include <thread>
#include "../basic_type.cpp"
#include "../bsb.hpp"
#include "bsb_test.hpp"

void bsb_tester::bsb_tester_work(bsb * b_bus)
{
	
	pr_data_val_t value = 0;

	while (1)
	{
		b_bus->PrXX_wait();
	
		switch (b_bus->get_ctrl_bus())
		{
			case bsb_control_t::PrRd:
				b_bus->PrRd_ack(value);
				break;

			case bsb_control_t::PrWr:
				value = b_bus->get_data_bus();;
				b_bus->PrWr_ack();

				break;

			default:
				assert(0);
		}
	}
}

void bsb_tester::bsb_tester_main(void)
{
	bsb b_bus;

	pr_data_val_t r;

	std::thread worker(bsb_tester_work, &b_bus);

	for (unsigned int i = 0; i < 100000000; i++)
	{
		b_bus.PrWr(0x1, i);

		b_bus.PrRd(0x1, r);

		if (0 == (i % 100000))
			std::printf("write=%u, read=%zu\n", i, r.to_number());

		assert(r == i);
	}
}

#endif	//	HAVE_BSB_TEST

