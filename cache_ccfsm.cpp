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
 *	cache_ccfsm.cpp
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
#include <mutex>
#include <condition_variable>
#include "basic_type.cpp"
#include "system.hpp"
#include "cache_ccfsm.hpp"

cache_ccfsm::cache_ccfsm()
{
	state = cache_ccfsm_t::Idle;
}

void cache_ccfsm::begin(const pr_data_opt_t & a)
{
	{
		std::unique_lock lck(cs);

		if (cache_ccfsm_t::Idle != state)
			system::oops(__FILE__, __LINE__);

		state = cache_ccfsm_t::Reading;

		maddr = a;
	}
}

void cache_ccfsm::begin(const pr_data_opt_t & a, const pr_data_val_t & v)
{
	{
		std::unique_lock lck(cs);

		if (cache_ccfsm_t::Idle != state)
			system::oops(__FILE__, __LINE__);

		state = cache_ccfsm_t::Writing;

		maddr = a;
		value = v;
	}
}

void cache_ccfsm::confirm(void)
{
	{
		std::unique_lock lck(cs);

		if (cache_ccfsm_t::Complete != state)
			system::oops(__FILE__, __LINE__);

		state = cache_ccfsm_t::Confirm;
	}

	cv.notify_one();
}

void cache_ccfsm::wait_complete(void) const
{
	{
		std::unique_lock lck(cs);

		while (cache_ccfsm_t::Complete != state)
		{
			cv.wait(lck);
		}
	}
}

void cache_ccfsm::wait_idle(void) const
{
	{
		std::unique_lock lck(cs);

		while(cache_ccfsm_t::Idle != state)
		{
			cv.wait(lck);
		}
	}
}

void cache_ccfsm::commit(void)
{
	{
		std::unique_lock lck(cs);

		switch (state)
		{
			case cache_ccfsm_t::Reading:
			case cache_ccfsm_t::Writing:
				state = cache_ccfsm_t::Complete;
				break;

			default:
				system::oops(__FILE__, __LINE__);
				break;
		}
	}

	cv.notify_one();

	{
		std::unique_lock lck(cs);

		while (cache_ccfsm_t::Confirm != state)
		{
			cv.wait(lck);
		}

		state = cache_ccfsm_t::Idle;
	}

	cv.notify_one();
}

