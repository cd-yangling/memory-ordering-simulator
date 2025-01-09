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
 *	memory.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_MEMORY_HEADER__
#define	__MO_SIM_MEMORY_HEADER__

#include <cstdint>
#include <map>
#include "basic_type.cpp"
#include "fsb.hpp"

/**
 *
 *            cpu
 *             |
 *             |
 *    +-----------------+
 *    |                 |
 *    |   cache         |
 *    |         +-------+
 *    |         |       |
 *    |         | snoop |
 *    |         |       |
 *    +---------+-------+
 *             |
 *             |
 *            bus
 *             |
 *             |
 *         cache/mem
 */

class memory
{
private:
	const std::uint32_t						sid;
	std::map<mm_data_tag_t, mm_data_val_t>	mem;
	fsb						*				bus;	//	前端总线(Cache --- Cache/Mem)
	const mm_data_val_t						zero;

public:
	memory() = delete;
	memory(const memory & ) = delete;
	memory & operator=(const memory &) = delete;
	memory(std::uint32_t id, fsb * f);

public:
	static void snoop_thread(memory * obj);				//	处理前端总线事务线程

private:
	void fsb_slave_process(void);

private:
	void slave_on_BusRd(void) const;
	void slave_on_BusUgr(void) const;
	void slave_on_BusRdX(void) const;
	void slave_on_Flush(void);

private:
	void read(const mm_data_tag_t & idx, mm_data_val_t & val) const;
	void write(const mm_data_tag_t & idx, const mm_data_val_t & val);
};

#endif	//	__MO_SIM_MEMORY_HEADER__

