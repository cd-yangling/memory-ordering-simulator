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
 *	mob.cpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2025-01-13 Created By YangLing (yl.tienon@gmail.com)
 */

#if defined(HAVE_STORE_BUFFER)

#include <mutex>
#include <condition_variable>
#include <deque>
#include <map>
#include "basic_type.hpp"
#include "bsb.hpp"
#include "store_buffer.hpp"
#include "mob.hpp"
#include "system.hpp"

mob::mob(bsb * b, const std::size_t m) : max(m)
{
	bus = b;
}

void mob::PrRd(const pr_data_opt_t & addr, pr_data_val_t & data)
{
	{
		//	Store Buffer Forwarding
		std::unique_lock lck(cs);

		if (const auto & s = dat_map.find(addr); s != dat_map.end())
		{
			data = s->second->get_data();
			return;
		}

		bus->PrRd(addr, data);
	}
}

void mob::PrWr(const pr_data_opt_t & addr, const pr_data_val_t & data)
{
	{
		std::unique_lock lck(cs);

		//	Store Buffer Forwarding
		if (const auto & s = dat_map.find(addr); s != dat_map.end())
		{
			s->second->set_data(data);
			return;
		}

		/*
		 *
		 *                    +-----------> Store Buffer
		 *                    |                /|\
		 *                   Yes                |
		 *                    |                 |
		 *  ---> 存在写栅栏---+           +-----+
		 *                    |           |
		 *                   No       Write Miss
		 *                    |           |
		 *                    +--- PrWr---+
		 *                                |
		 *                            Write Hit
		 *                                |
		 *                                +-------- 结束
		 *
		 */

		//	如果没有写栅栏,尝试直接写 cache, 如果 cache hit.就直接返回了
		if (wmb_map.empty() && 0/*bus->PrWr(addr, data, false*/)
			return;

		//	Store Buffer 最多只能有 max 个, 超过只能等待
		while (dat_que.size() >= max)
		{
			cv_full.wait(lck);
		}

		//	经过了 cv 条件等待, 那么是有可能存在竞争条件的.必须检查程序逻辑错误
		if (const auto & s = dat_map.find(addr); s != dat_map.end())
			//	不可能发生,一定有逻辑问题
			system::oops(__FILE__, __LINE__);

		store_buffer * sbp = new store_buffer(addr, data);

		dat_map[addr] = sbp;
		dat_que.push_back(sbp);
	}

	cv_empty.notify_one();
}

void mob::PrWMB(void)
{
	{
		std::unique_lock lck(cs);

		if (dat_que.size())
		{
			store_buffer * sbp = dat_que.back();

			sbp->set_flag();

			if (const auto & s = wmb_map.find(sbp->get_addr()); s == wmb_map.end())
				wmb_map[sbp->get_addr()] = sbp;
		}
	}
}

void mob::process(void)
{
	{
		std::unique_lock lck(cs);

		while (dat_que.size() == 0)
		{
			cv_empty.wait(lck);
		}
	}

	//	故意离开一下锁,更好的制造出来 WMB 的效果

	{
		bool notify = false;

		std::unique_lock lck(cs);

		if (dat_que.size() >= max)
			notify = true;		//	可能有人因为 Store Buffer 满了而等待

		if (dat_que.empty())
			system::oops(__FILE__, __LINE__);

		store_buffer * sbp = dat_que.front();

		bus->PrWr(sbp->get_addr(), sbp->get_data());

		dat_que.pop_front();

		if (const auto & s = dat_map.find(sbp->get_addr()); s == dat_map.end())
			system::oops(__FILE__, __LINE__);
		else
			dat_map.erase(sbp->get_addr());

		if (const auto & s = wmb_map.find(sbp->get_addr()); s != wmb_map.end())
			wmb_map.erase(sbp->get_addr());

		lck.unlock();

		if (notify)
			cv_full.notify_one();

		delete sbp;
	}

}

void mob::mob_thread(mob * obj)
{
	while (true)
	{
		obj->process();
	}
}
#endif	//	HAVE_STORE_BUFFER

