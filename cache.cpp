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
 *	cache.cpp
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
#include "cache_line.hpp"
#include "cache_ccfsm.hpp"
#include "cache_set.hpp"
#include "cache.hpp"
#include "bsb.hpp"
#include "fsb.hpp"

cache::cache(const std::uint32_t id, bsb * b, fsb * f) : sid(id)
{
	bbus = b;
	fbus = f;
	fbus->attach(sid);
}

void cache::cache_thread(cache * obj)
{
	while (true)
	{
		obj->bsb_slave_process();
	}
}

void cache::snoop_thread(cache * obj)
{
	bool mdev = false;

	while (true)
	{
		if (mdev)
			mdev = obj->fsb_master_process();
		else
			mdev = obj->fsb_slave_process();
	}
}

void cache::bsb_slave_process(void)
{
	bbus->PrXX_wait();

	switch (bbus->get_ctrl_bus())
	{
		case bsb_control_t::PrRd:
			{
				pr_data_val_t value;

				if (!local_read(bbus->get_addr_bus(), value))
					remote_read(bbus->get_addr_bus(), value);

				bbus->PrRd_ack(value);
			}
			break;

		case bsb_control_t::PrWr:
			{
				if (!local_write(bbus->get_addr_bus(), bbus->get_data_bus()))
					remote_write(bbus->get_addr_bus(), bbus->get_data_bus());

				bbus->PrWr_ack();
			}
			break;

#if defined(HAVE_STORE_BUFFER)
		case bsb_control_t::PrWr_async:
			{
				if (!local_write(bbus->get_addr_bus(), bbus->get_data_bus()))
					bbus->PrWr_async_ack(false);
				else
					bbus->PrWr_async_ack(true);
			}
			break;
#endif	//	HAVE_STORE_BUFFER
		default:
			system::oops(__FILE__, __LINE__);
	}
}

bool cache::fsb_slave_process(void)
{
	if (fbus->slave_snoop(sid))
	{
#ifndef	HAVE_FAIR_BUS_ARBITRATION
		switch (work.get_state())
		{
			case cache_ccfsm_t::Reading:
			case cache_ccfsm_t::Writing:
				if (bus_tryrace())
					break;
				else
					return false;	//	竞争失败

			default:
				system::oops(__FILE__, __LINE__);
		}
#endif
		switch (work.get_state())
		{
			case cache_ccfsm_t::Reading:
				bus_read(true);
				break;

			case cache_ccfsm_t::Writing:
				bus_write(true);
				break;

			default:
				system::oops(__FILE__, __LINE__);
		}

		return true;	//	仲裁获取了总线控制权,转换成主设备
	}
	else
	{
		switch (fbus->get_ctrl_bus())
		{
			case fsb_control_t::None:
				system::oops(__FILE__, __LINE__);
				break;

			case fsb_control_t::BusRd:
				slave_on_BusRd();
				break;

			case fsb_control_t::BusUgr:
				slave_on_BusUgr();
				break;

			case fsb_control_t::BusRdX:
				slave_on_BusRdX();
				break;

			case fsb_control_t::Flush:
				slave_on_Flush();
				break;

			default:
				system::oops(__FILE__, __LINE__);
		}

		return false;
	}
}

bool cache::fsb_master_process(void)
{
	fbus->master_snoop(sid);

	switch (fbus->get_ctrl_bus())
	{
		case fsb_control_t::None:
			system::oops(__FILE__, __LINE__);
			return false;

		case fsb_control_t::BusRd:
			master_on_BusRd();
			work.commit();
			return false;	//	完成主设备的事务,切回从设备角色

		case fsb_control_t::BusUgr:
			master_on_BusUgr();
			work.commit();
			return false;	//	完成主设备的事务,切回从设备角色

		case fsb_control_t::BusRdX:
			master_on_BusRdX();
			work.commit();
			return false;	//	完成主设备的事务,切回从设备角色

		case fsb_control_t::Flush:
			master_on_Flush();
			return true;	//	继续作为主设备完成其他事务

		default:
			system::oops(__FILE__, __LINE__);
			return false;
	}
}

void cache::slave_on_BusRd(void)
{
	const cl_data_tag_t & tag = fbus->get_addr_bus();
	bool shared = false;
	bool dirty = false;
	cl_data_val_t dat;

	do
	{
		std::unique_lock lck(cs);

		cache_line * cl = set.ref_cache_line(tag, true);

		if (nullptr == cl)
			break;

#if defined(WITH_INVALID_QUEUE)
#endif	//	WITH_INVALID_QUEUE

		switch (cl->get_state())
		{
			case cache_state_t::MESI_CACHE_I:
				break;

			case cache_state_t::MESI_CACHE_S:
				dat = cl->get_dat();
				shared = true;
				break;

			case cache_state_t::MESI_CACHE_E:
				dat = cl->get_dat();
				dirty = true;
				cl->set_state(cache_state_t::MESI_CACHE_S);
				break;

			case cache_state_t::MESI_CACHE_M:
				dat = cl->get_dat();
				dirty = true;
				cl->set_state(cache_state_t::MESI_CACHE_S);
				break;

			default:
				system::oops(__FILE__, __LINE__);
		}
	} while(0);

	fbus->BusRd_ack(sid, dat, dirty, shared);
}

void cache::slave_on_BusUgr(void)
{
	const cl_data_tag_t & tag = fbus->get_addr_bus();

	do
	{
		std::unique_lock lck(cs);

		cache_line * cl = set.ref_cache_line(tag, true);

		if (nullptr == cl)
			break;

#if defined(WITH_INVALID_QUEUE)
#endif	//	WITH_INVALID_QUEUE

		switch (cl->get_state())
		{
			case cache_state_t::MESI_CACHE_I:
				break;

			case cache_state_t::MESI_CACHE_S:
				cl->set_state(cache_state_t::MESI_CACHE_I);
				break;

			case cache_state_t::MESI_CACHE_E:
				cl->set_state(cache_state_t::MESI_CACHE_I);
				break;

			case cache_state_t::MESI_CACHE_M:
				cl->set_state(cache_state_t::MESI_CACHE_I);
				break;

			default:
				system::oops(__FILE__, __LINE__);
		}
	} while(0);

	fbus->BusUgr_ack(sid);
}

void cache::slave_on_BusRdX(void)
{
	const cl_data_tag_t & tag = fbus->get_addr_bus();
	bool shared = false;
	bool dirty = false;
	cl_data_val_t dat;

	do
	{
		std::unique_lock lck(cs);

		cache_line * cl = set.ref_cache_line(tag, true);

		if (nullptr == cl)
			break;

#if defined(WITH_INVALID_QUEUE)
#endif	//	WITH_INVALID_QUEUE

		switch (cl->get_state())
		{
			case cache_state_t::MESI_CACHE_I:
				break;

			case cache_state_t::MESI_CACHE_S:
				dat = cl->get_dat();
				shared = true;
				cl->set_state(cache_state_t::MESI_CACHE_I);
				break;

			case cache_state_t::MESI_CACHE_E:
				dat = cl->get_dat();
				dirty = true;
				cl->set_state(cache_state_t::MESI_CACHE_I);
				break;

			case cache_state_t::MESI_CACHE_M:
				dat = cl->get_dat();
				dirty = true;
				cl->set_state(cache_state_t::MESI_CACHE_I);
				break;

			default:
				system::oops(__FILE__, __LINE__);
		}
	} while(0);

	fbus->BusRdX_ack(sid, dat, dirty, shared);
}

void cache::slave_on_Flush(void) const
{
	fbus->Flush_ack(sid);
}

void cache::master_on_BusRd(void)
{
	const cl_data_tag_t & tag = fbus->get_addr_bus();
	const cl_data_val_t & dat = fbus->get_data_bus();
	const std::uint32_t dirty = fbus->get_data_dirty();
	const std::uint32_t shared= fbus->get_data_shared();

	std::unique_lock lck(cs);

	cache_line * cl = set.ref_cache_line(tag, true);

	if (nullptr == cl)
	{
		system::oops(__FILE__, __LINE__);
	}
	else
	{
		switch (cl->get_state())
		{
			case cache_state_t::MESI_CACHE_I:
				cl->set_dat(dat);

				if (!dirty && !shared)
					cl->set_state(cache_state_t::MESI_CACHE_E);
				else
					cl->set_state(cache_state_t::MESI_CACHE_S);

				break;

			default:
				system::oops(__FILE__, __LINE__);
				break;
		}
	}
}

void cache::master_on_BusUgr(void)
{
	const cl_data_tag_t & tag = fbus->get_addr_bus();

	std::unique_lock lck(cs);

	cache_line * cl = set.ref_cache_line(tag, true);

	if (nullptr == cl)
	{
		system::oops(__FILE__, __LINE__);
	}
	else
	{
		switch (cl->get_state())
		{
			case cache_state_t::MESI_CACHE_I:
				system::oops(__FILE__, __LINE__);
				break;

			case cache_state_t::MESI_CACHE_S:
				cl->set_state(cache_state_t::MESI_CACHE_M);
				break;

			default:
				system::oops(__FILE__, __LINE__);
				break;
		}
	}
}

void cache::master_on_BusRdX(void)
{
	const cl_data_tag_t & tag = fbus->get_addr_bus();
	const cl_data_val_t & dat = fbus->get_data_bus();

	std::unique_lock lck(cs);

	cache_line * cl = set.ref_cache_line(tag, true);

	if (nullptr == cl)
	{
		system::oops(__FILE__, __LINE__);
	}
	else
	{
		switch (cl->get_state())
		{
			case cache_state_t::MESI_CACHE_I:
				cl->set_dat(dat);
				cl->set_state(cache_state_t::MESI_CACHE_M);
				break;

			default:
				system::oops(__FILE__, __LINE__);
				break;
		}
	}
}

void cache::master_on_Flush(void)
{
	switch (work.get_state())
	{
		case cache_ccfsm_t::Reading:
			bus_read(false);
			break;

		case cache_ccfsm_t::Writing:
			bus_write(false);
			break;

		default:
			system::oops(__FILE__, __LINE__);
	}
}

void cache::bus_acquire(void) const
{
	fbus->acquire(sid);
}

void cache::bus_release(void) const
{
	fbus->release(sid);
}
#ifndef	HAVE_FAIR_BUS_ARBITRATION
bool cache::bus_tryrace(void) const
{
	return fbus->tryrace(sid);
}
#endif

bool cache::bus_write_back(void) const
{
	const cache_line * cl = set.ref_cache_line(work.get_maddr(), false);

	if (nullptr == cl)
		system::oops(__FILE__, __LINE__);

	if (cl->cmp_tag(work.get_maddr()))
		return false;

	//	缓存行冲突(可能需要写回)
	switch (cl->get_state())
	{
		case cache_state_t::MESI_CACHE_I:
		case cache_state_t::MESI_CACHE_S:
		case cache_state_t::MESI_CACHE_E:
			return false;	//	不需要写回

		case cache_state_t::MESI_CACHE_M:
			break;

		default:
			system::oops(__FILE__, __LINE__);
	}

#if defined(WITH_INVALID_QUEUE)
		/*	这里需要处理 Invalid Queue 的事务*/
#endif	//	WITH_INVALID_QUEUE

	fbus->Flush(sid, cl->get_tag(), cl->get_dat());

	return true;
}

void cache::bus_read(bool first)
{
	if (first && bus_write_back())
	{
		return;
	}

	cache_line * cl = set.ref_cache_line(work.get_maddr(), false);

	if (nullptr == cl)
		system::oops(__FILE__, __LINE__);

	if (!cl->cmp_tag(work.get_maddr()))
	{
		cl->set_tag(work.get_maddr());
		cl->set_state(cache_state_t::MESI_CACHE_I);
	}

	switch (cl->get_state())
	{
		case cache_state_t::MESI_CACHE_I:
			fbus->BusRd(sid, cl->get_tag());
			break;

		default:
			system::oops(__FILE__, __LINE__);
			break;
	}
}

void cache::bus_write(bool first)
{
	if (first && bus_write_back())
	{
		return;
	}

	//	注意: 这里需要根据 缓存行的状态 来发送 RdX 还是 X 总线信号
	//	实际上, 对于 MESI 协议的这2个区别,目前我也没有真正理解透彻
	//	目前我理解的是这样可以借用 snoop 来接力 FlushOpt 操作,可以
	//	让缓存行在 Cache 之间进行传递,确保了状态转移的同时数据迁移
	cache_line * cl = set.ref_cache_line(work.get_maddr(), false);

	if (nullptr == cl)
		system::oops(__FILE__, __LINE__);

	if (!cl->cmp_tag(work.get_maddr()))
	{
		cl->set_tag(work.get_maddr());
		cl->set_state(cache_state_t::MESI_CACHE_I);
	}

	switch (cl->get_state())
	{
		case cache_state_t::MESI_CACHE_I:
			fbus->BusRdX(sid, cl->get_tag());
			break;

		case cache_state_t::MESI_CACHE_S:
#if defined(WITH_INVALID_QUEUE)
			/*	这里需要处理 Invalid Queue 的事务*/
#endif	//	WITH_INVALID_QUEUE

		fbus->BusUgr(sid, cl->get_tag());
			break;

		default:
			system::oops(__FILE__, __LINE__);
			break;
	}
}

bool cache::local_read(const pr_data_opt_t & maddr, pr_data_val_t & value) const
{
	std::unique_lock lck(cs);

	const cache_line * cl = set.ref_cache_line(maddr, true);

	if (cl)
	{
		switch (cl->get_state())
		{
			case cache_state_t::MESI_CACHE_I:
				return false;

			case cache_state_t::MESI_CACHE_S:
			case cache_state_t::MESI_CACHE_E:
			case cache_state_t::MESI_CACHE_M:
				value = cl->get_dat(maddr);
				return true;

			default:
				system::oops(__FILE__, __LINE__);
		}
	}

	return false;
}

void cache::remote_read(const pr_data_opt_t & maddr, pr_data_val_t & value)
{
	work.begin(maddr);

	bus_acquire();

	//	等待 MESI 协议同步的结果
	work.wait_complete();

	if (!local_read(maddr, value))
		system::oops(__FILE__, __LINE__);

	bus_release();

	work.confirm();

	work.wait_idle();
}

bool cache::local_write(const pr_data_opt_t & maddr, const pr_data_val_t & value)
{
	std::unique_lock lck(cs);

	cache_line * cl = set.ref_cache_line(maddr, true);

	if (cl)
	{
		switch (cl->get_state())
		{
			case cache_state_t::MESI_CACHE_I:
			case cache_state_t::MESI_CACHE_S:
				return false;

			case cache_state_t::MESI_CACHE_E:
				cl->set_state(cache_state_t::MESI_CACHE_M);
				__attribute__ ((fallthrough));

			case cache_state_t::MESI_CACHE_M:
				cl->set_dat(maddr, value);
				return true;

			default:
				system::oops(__FILE__, __LINE__);
		}
	}

	return false;
}

void cache::remote_write(const pr_data_opt_t & maddr, const pr_data_val_t & value)
{
	work.begin(maddr, value);

	bus_acquire();

	//	等待 MESI 协议同步的结果
	work.wait_complete();

	if (!local_write(maddr, value))
		system::oops(__FILE__, __LINE__);

	bus_release();

	work.confirm();

	work.wait_idle();
}

void cache::assert_cache_line(const pr_data_opt_t & maddr, const pr_data_val_t & value, const cache_state_t state) const
{
	auto cl = set.ref_cache_line(maddr, true);

	if (cl)
	{
		if (value != cl->get_dat(maddr))
			system::oops(__FILE__, __LINE__);

		if (state != cl->get_state())
			system::oops(__FILE__, __LINE__);
	}
	else
	{
		system::oops(__FILE__, __LINE__);
	}
}

void cache::assert_cache_line(const pr_data_opt_t & maddr) const
{
	auto cl = set.ref_cache_line(maddr, true);

	if (cl && cache_state_t::MESI_CACHE_I != cl->get_state())
		system::oops(__FILE__, __LINE__);
}

