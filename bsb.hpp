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
 *	bsb.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_BSB_HEADER__
#define	__MO_SIM_BSB_HEADER__

#include <cstdint>
#include <mutex>
#include <condition_variable>
#include "basic_type.cpp"

//	back side bus 后端总线 CPU (主设备) ---> Cache (从设备)
enum class bsb_control_t
{
	None,
	PrRd,
	PrWr,
#if defined(HAVE_STORE_BUFFER)
	PrWr_async
#endif	//	HAVE_STORE_BUFFER
};

class bsb
{
private:
	mutable std::mutex					cs;
	mutable std::condition_variable		master;
	mutable std::condition_variable		slave;

	bsb_control_t						ctrl_bus;		//	控制线(总线规定:只能主设备操作)
	pr_data_opt_t						addr_bus;		//	地址线(总线规定:只能主设备操作)
	pr_data_val_t						data_bus;		//	数据线

	bool								work_req;		//	事务请求信号线
	bool								work_rsp;		//	事务应答信号线
#if defined(HAVE_STORE_BUFFER)
	bool								write_hit;		//	是否 write_hit
#endif	//	HAVE_STORE_BUFFER

public:
	bsb();
	bsb(const bsb & ) = delete;
	bsb & operator=(const bsb &) = delete;

public:
	const bsb_control_t & get_ctrl_bus(void) const { return ctrl_bus; }
	const pr_data_opt_t & get_addr_bus(void) const { return addr_bus; }
	const pr_data_val_t & get_data_bus(void) const { return data_bus; }

public:
	//	提供给 Cpu 的操作方法
	void PrRd(const pr_data_opt_t & addr, pr_data_val_t & data);
	void PrWr(const pr_data_opt_t & addr, const pr_data_val_t & data);
#if defined(HAVE_STORE_BUFFER)
	bool PrWr_async(const pr_data_opt_t & addr, const pr_data_val_t & data);
#endif	//	HAVE_STORE_BUFFER

	//	提供给 Cache 的操作方法
	void PrXX_wait(void) const;

	void PrRd_ack(const pr_data_val_t & data);
	void PrWr_ack(void);
#if defined(HAVE_STORE_BUFFER)
	void PrWr_async_ack(bool wh);
#endif	//	HAVE_STORE_BUFFER
};

#endif	//	__MO_SIM_BSB_HEADER__

