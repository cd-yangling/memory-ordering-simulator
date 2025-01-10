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
 *	fsb.cpp
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
#include "fsb.hpp"
#include "system.hpp"

static const std::uint32_t none_winner = 0xFFFFFFFF;

fsb::fsb()
{
	mask = 0;

	winner = none_winner;

#ifdef	HAVE_FAIR_BUS_ARBITRATION
	mbit = 0;
#else
	mreq = 0;
#endif

	work_ack = 0xFFFFFFFF;
	data_dirty = 0;
	data_shared = 0;

	ctrl_bus = fsb_control_t::None;
}

void fsb::attach(const std::uint32_t id)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	std::unique_lock lck(cs);

	if (tst_bit(id, mask))
		system::oops(__FILE__, __LINE__);
	else
		set_bit(id, mask);
}

void fsb::detach(const std::uint32_t id)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	std::unique_lock lck(cs);

	if (tst_bit(id, mask))
		clr_bit(id, mask);
	else
		system::oops(__FILE__, __LINE__);
}

void fsb::acquire(const std::uint32_t id)
{
	bool notify;

	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		//	确保代码逻辑不能出现重复申请的情况
		if (id == winner)
			system::oops(__FILE__, __LINE__);

#ifdef	HAVE_FAIR_BUS_ARBITRATION
		if (tst_bit(id, mbit))
			system::oops(__FILE__, __LINE__);
		else
			set_bit(id, mbit);

		if (none_winner == winner)
		{
			winner = id;
			notify = true;
		}
		else
		{
			mque.push(id);	//	进入 FIFO 的仲裁队列等待
			notify = false;
		}
#else
		if (tst_bit(id, mreq))
			system::oops(__FILE__, __LINE__);	//	不可能发生情况,重复设置申请意向
		else
			set_bit(id, mreq);					//	设置有申请意向,但是未得到控制权

		//	如果总线空闲, 通知所有的 snoop 进行总线竞争
		notify = (none_winner == winner) ? true : false;
#endif
	}

	if (notify)
		slave.notify_all();
}

void fsb::release(const std::uint32_t id)
{
	bool notify;

	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		//	确保代码逻辑不能出现重复释放的情况
		if (id != winner)
			system::oops(__FILE__, __LINE__);

#ifdef	HAVE_FAIR_BUS_ARBITRATION
		if (tst_bit(id, mbit))
			clr_bit(id, mbit);
		else
			system::oops(__FILE__, __LINE__);

		if (mque.empty())
		{
			notify = false;
			winner = none_winner;
		}
		else
		{
			notify = true;
			winner = mque.front();
			mque.pop();
		}
#else
		//	既然是主设备释放控制权,那么不应该还出现在意向位图中
		if (tst_bit(id, mreq))
			system::oops(__FILE__, __LINE__);

		winner = none_winner;

		//	如果有申请意向者, 考虑通知他们可以开始竞争了
		notify = mreq ? true : false;
#endif
	}

	if (notify)
		slave.notify_all();
}
#ifndef	HAVE_FAIR_BUS_ARBITRATION
bool fsb::tryrace(const std::uint32_t id)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		//	没有申请意向, 这是不应该发生的
		if (!tst_bit(id, mreq))
			system::oops(__FILE__, __LINE__);

		if (none_winner != winner)
			return false;	//	竞争失败

		winner = id;
		clr_bit(id, mreq);

		return true;
	}
}
#endif

void fsb::master_snoop(const std::uint32_t id) const
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	std::unique_lock lck(cs);

	if (id != winner)
		system::oops(__FILE__, __LINE__);

	while (!((work_ack & mask) == mask))
	{
		master.wait(lck);
	}
}

bool fsb::slave_snoop(const std::uint32_t id) const
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	std::unique_lock lck(cs);

	while (true)
	{
		if (!tst_bit(id, work_ack))
		{
			return false;	//	有总线请求尚未应答
		}

#ifdef	HAVE_FAIR_BUS_ARBITRATION
		if (id == winner)
		{
			return true;	//	获得了总线控制权
		}
#else
		if ((none_winner == winner) && tst_bit(id, mreq))
		{
			return true;	//	总线空闲,准备竞争
		}
#endif

		slave.wait(lck);
	}
}

void fsb::BusRd(const std::uint32_t id, const cl_data_tag_t & tag)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		if (id != winner)
			system::oops(__FILE__, __LINE__);

		work_ack = 0;
		data_dirty = 0;
		data_shared = 0;
		set_bit(id, work_ack);

		ctrl_bus = fsb_control_t::BusRd;
		addr_bus = tag;
	}

	slave.notify_all();
}

void fsb::BusUgr(const std::uint32_t id, const cl_data_tag_t & tag)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		if (id != winner)
			system::oops(__FILE__, __LINE__);

		work_ack = 0;
		data_dirty = 0;
		data_shared = 0;
		set_bit(id, work_ack);

		ctrl_bus = fsb_control_t::BusUgr;
		addr_bus = tag;
	}

	slave.notify_all();
}

void fsb::BusRdX(const std::uint32_t id, const cl_data_tag_t & tag)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		if (id != winner)
			system::oops(__FILE__, __LINE__);

		work_ack = 0;
		data_dirty = 0;
		data_shared = 0;
		set_bit(id, work_ack);

		ctrl_bus = fsb_control_t::BusRdX;
		addr_bus = tag;
	}

	slave.notify_all();
}

void fsb::Flush(const std::uint32_t id, const cl_data_tag_t & tag, const cl_data_val_t & dat)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		if (id != winner)
			system::oops(__FILE__, __LINE__);

		work_ack = 0;
		data_dirty = 0;
		data_shared = 0;
		set_bit(id, work_ack);

		ctrl_bus = fsb_control_t::Flush;
		addr_bus = tag;
		data_bus = dat;
	}

	slave.notify_all();
}

void fsb::BusRd_ack(const std::uint32_t id, const mm_data_val_t & dat)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		if (id == winner)
			system::oops(__FILE__, __LINE__);

		if (tst_bit(id, work_ack))
			system::oops(__FILE__, __LINE__);

		if (!data_dirty && !data_shared)
		{
			data_bus = dat;	//	没有任何 cache 提供,由 memory 提供
		}

		set_bit(id, work_ack);
	}

	master.notify_one();
}

void fsb::BusRdX_ack(const std::uint32_t id, const mm_data_val_t & dat)
{
	BusRd_ack(id, dat);
}

void fsb::BusRd_ack(const std::uint32_t id, const cl_data_val_t & dat, const bool dirty, const bool shared)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	if (dirty && shared)	//	既是 dirty 又是 shared 是不可能的输入参数
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		if (id == winner)
			system::oops(__FILE__, __LINE__);

		if (tst_bit(id, work_ack))
			system::oops(__FILE__, __LINE__);

		if (dirty)
		{
			//	同时存在 2 个 M/E 状态
			if (dirty && data_dirty)
				system::oops(__FILE__, __LINE__);

			data_bus = dat;
			set_bit(id, data_dirty);
		}
		else if (shared)
		{
			//	1: 同时存在 M/E 和 S 状态,  这不应该发生
			//	2: 同时存在 S 状态, 但是他们的数据不一致
			if ((data_dirty) || (data_shared && dat != data_bus))
				system::oops(__FILE__, __LINE__);

			data_bus = dat;
			set_bit(id, data_shared);
		}

		set_bit(id, work_ack);
	}

	master.notify_one();
}

void fsb::BusUgr_ack(const std::uint32_t id)
{
	if (invalid_id(id))
		system::oops(__FILE__, __LINE__);

	{
		std::unique_lock lck(cs);

		if (id == winner)
			system::oops(__FILE__, __LINE__);

		if (tst_bit(id, work_ack))
			system::oops(__FILE__, __LINE__);

		set_bit(id, work_ack);
	}

	master.notify_one();
}

void fsb::BusRdX_ack(const std::uint32_t id, const cl_data_val_t & dat, const bool dirty, const bool shared)
{
	BusRd_ack(id, dat, dirty, shared);
}

void fsb::Flush_ack(const std::uint32_t id)
{
	BusUgr_ack(id);
}

