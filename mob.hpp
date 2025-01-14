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
 *	mob.hpp
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

#ifndef	__MO_SIM_MOB_HEADER__
#define	__MO_SIM_MOB_HEADER__

#include <mutex>
#include <condition_variable>
#include <deque>
#include <map>
#include "basic_type.hpp"
#include "bsb.hpp"
#include "store_buffer.hpp"

class mob
{
private:
	mutable std::mutex							cs;
	mutable std::condition_variable				cv_empty;
	mutable std::condition_variable				cv_full;
	const std::size_t							max;
	std::deque<store_buffer*>					dat_que;
	std::map<pr_data_opt_t, store_buffer*>		wmb_map;
	std::map<pr_data_opt_t, store_buffer*>		dat_map;
	bsb								*			bus;

public:
	mob() = delete;
	mob(const mob &) = delete;
	mob & operator=(const mob &) = delete;

	mob(bsb * b, const std::size_t m);

public:
	static void mob_thread(mob *);

public:
	void process(void);

public:
	void PrRd(const pr_data_opt_t & addr, pr_data_val_t & data);
	void PrWr(const pr_data_opt_t & addr, const pr_data_val_t & data);
	void PrWMB(void);
};

#endif	//	__MO_SIM_MOB_HEADER__
#endif	//	HAVE_STORE_BUFFER

