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
 *	cache.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_CACHE_HEADER__
#define	__MO_SIM_CACHE_HEADER__

#include <cstdint>
#include <mutex>
#include <condition_variable>
#include "basic_type.cpp"
#include "cache_ccfsm.hpp"
#include "cache_set.hpp"
#include "bsb.hpp"
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

class cache
{
private:
	const std::uint32_t				sid;
	mutable std::mutex				cs;
	cache_set						set;
	cache_ccfsm						work;
	bsb						*		bbus;	//	后端总线(Cpu --- Cache)
	fsb						*		fbus;	//	前端总线(Cache --- Cache/Mem)

public:
	cache() = delete;
	cache(const cache & ) = delete;
	cache & operator=(const cache &) = delete;
	cache(const std::uint32_t id, bsb * b, fsb * f);

public:
	static void cache_thread(cache * obj);	//	处理后端总线事务线程
	static void snoop_thread(cache * obj);	//	处理前端总线事务线程

private:
	void bsb_slave_process(void);
	bool fsb_slave_process(void);
	bool fsb_master_process(void);

private:
	void slave_on_BusRd(void);
	void slave_on_BusUgr(void);
	void slave_on_BusRdX(void);
	void slave_on_Flush(void) const;

	void master_on_BusRd(void);
	void master_on_BusUgr(void);
	void master_on_BusRdX(void);
	void master_on_Flush(void);

private:
	void bus_acquire(void) const;
	void bus_release(void) const;
#ifndef	HAVE_FAIR_BUS_ARBITRATION
	bool bus_tryrace(void) const;
#endif

	bool bus_write_back(void) const;
	void bus_read(bool first);
	void bus_write(bool first);

private:
	bool local_read(const pr_data_opt_t & maddr, pr_data_val_t & value) const;
	void remote_read(const pr_data_opt_t & maddr, pr_data_val_t & value);

	bool local_write(const pr_data_opt_t & maddr, const pr_data_val_t & value);
	void remote_write(const pr_data_opt_t & maddr, const pr_data_val_t & value);

public:
	void assert_cache_line(const pr_data_opt_t & maddr, const pr_data_val_t & value, const cache_state_t state) const;
	void assert_cache_line(const pr_data_opt_t & maddr) const;
};

#endif	//	__MO_SIM_CACHE_HEADER__

