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
 *	memory.cpp
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
#include <map>
#include "basic_type.cpp"
#include "system.hpp"
#include "fsb.hpp"
#include "memory.hpp"

memory::memory(const std::uint32_t id, fsb * f) : sid(id)
{
	bus = f;
	bus->attach(id);
}

void memory::snoop_thread(memory * obj)
{
	while (true)
	{
		obj->fsb_slave_process();
	}
}

void memory::fsb_slave_process(void)
{
	bus->slave_snoop(sid);

	switch (bus->get_ctrl_bus())
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
}

void memory::slave_on_BusRd(void) const
{
	mm_data_val_t v;

	read(bus->get_addr_bus(), v);

	bus->BusRd_ack(sid, v);
}

void memory::slave_on_BusUgr(void) const
{
	bus->BusUgr_ack(sid);
}

void memory::slave_on_BusRdX(void) const
{
	mm_data_val_t v;

	read(bus->get_addr_bus(), v);

	bus->BusRdX_ack(sid, v);
}

void memory::slave_on_Flush(void)
{
	write(bus->get_addr_bus(), bus->get_data_bus());

	bus->Flush_ack(sid);
}

void memory::read(const mm_data_tag_t & idx, mm_data_val_t & val) const
{
	if (auto r = mem.find(idx); r != mem.end())
		val = r->second;
	else
		val = zero;
}

void memory::write(const mm_data_tag_t & idx, const mm_data_val_t & val)
{
	if (auto r = mem.find(idx); r != mem.end())
		mem[idx] = val;		//	Update Value
	else
		mem[idx] = val;		//	Insert Value
}

