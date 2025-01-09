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
 *	bsb.cpp
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
#include <mutex>
#include <condition_variable>
#include "basic_type.cpp"
#include "bsb.hpp"
#include "system.hpp"

bsb::bsb()
{
	ctrl_bus = bsb_control_t::None;

	work_req = false;
	work_rsp = false;
}

void bsb::PrRd(const pr_data_opt_t & addr, pr_data_val_t & data)
{
	{
		std::unique_lock lck(cs);

		if (work_req || work_rsp || ctrl_bus != bsb_control_t::None)
		{
			system::oops(__FILE__, __LINE__);
		}

		ctrl_bus = bsb_control_t::PrRd;
		addr_bus = addr;

		work_req = true;
	}

	slave.notify_one();

	{
		std::unique_lock lck(cs);

		while (!work_rsp)
		{
			master.wait(lck);
		}

		if (work_req || bsb_control_t::PrRd != ctrl_bus)
		{
			system::oops(__FILE__, __LINE__);
		}

		ctrl_bus = bsb_control_t::None;
		data = data_bus;

		work_rsp = false;
	}
}

void bsb::PrWr(const pr_data_opt_t & addr, const pr_data_val_t & data)
{
	{
		std::unique_lock lck(cs);

		if (work_req || work_rsp || ctrl_bus != bsb_control_t::None)
		{
			system::oops(__FILE__, __LINE__);
		}

		ctrl_bus = bsb_control_t::PrWr;
		addr_bus = addr;
		data_bus = data;

		work_req = true;
	}

	slave.notify_one();

	{
		std::unique_lock lck(cs);

		while (!work_rsp)
		{
			master.wait(lck);
		}

		if (work_req || bsb_control_t::PrWr != ctrl_bus)
		{
			system::oops(__FILE__, __LINE__);
		}

		ctrl_bus = bsb_control_t::None;

		work_rsp = false;
	}
}

void bsb::PrXX_wait(void) const
{
	{
		std::unique_lock lck(cs);

		while (!work_req)
		{
			slave.wait(lck);
		}

		if (work_rsp || bsb_control_t::None == ctrl_bus)
		{
			system::oops(__FILE__, __LINE__);
		}
	}

	return;
}

void bsb::PrRd_ack(const pr_data_val_t & data)
{
	{
		std::unique_lock lck(cs);

		if (!work_req || work_rsp || bsb_control_t::PrRd != ctrl_bus)
		{
			system::oops(__FILE__, __LINE__);
		}

		data_bus = data;

		work_req = false;
		work_rsp = true;
	}

	master.notify_one();
}

void bsb::PrWr_ack(void)
{
	{
		std::unique_lock lck(cs);

		if (!work_req || work_rsp || bsb_control_t::PrWr != ctrl_bus)
		{
			system::oops(__FILE__, __LINE__);
		}

		work_req = false;
		work_rsp = true;
	}

	master.notify_one();
}

