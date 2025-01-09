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
 *	cache_ccfsm.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_CACHE_CCFSM_HEADER__
#define	__MO_SIM_CACHE_CCFSM_HEADER__

#include <cstdint>
#include <array>
#include <mutex>
#include <condition_variable>
#include "basic_type.cpp"

/**
 * Cache Coherence Finite State Machine 缓存同步有限状态机对象
 *
 * +--->Idle--->Reading/Writeing--->Commit--->Confirm---+
 * |                                                    |
 * +------------------<---------------------------------+
 *
 * 状态机用于完成 Cache 的 Snoop 模块与 Cache 模块之间的时序同步
 *
 *     Idle: 初始状态
 * Reading: 已提交了总线读请求,但是其他子设备尚未应答
 * Writing: 已提交了总线写请求,但是其他子设备尚未应答
 *  Commit: 其他子设备已经在总线回复了请求
 * Confirm: 对 Commit 进行确认
 *
 * 时序描述:
 *
 *          Cache                 Snoop
 *            |
 *            |
 *            |
 *            +------- begin ------>+
 *                                  |
 *          等待          master 角色处理总线事务
 *                                  |
 *            +<------complete------+
 *            |
 *    提取 cache_line 数据         等待
 *            |
 *            +------ confirm ----->+
 *                                  |
 *                         安全的进入slave设备状态
 *                                  |
 *            +<------- Idle -------+
 *            |                     |
 *            |                     |
 *        应答处理器            继续Snoop
 */

enum class cache_ccfsm_t { Idle, Reading, Writing, Complete, Confirm };
class cache_ccfsm
{
private:
	mutable std::mutex				cs;
	mutable std::condition_variable	cv;

	cache_ccfsm_t					state;

	pr_data_opt_t					maddr;
	pr_data_val_t					value;

public:
	cache_ccfsm();
	cache_ccfsm(const cache_ccfsm & ) = delete;
	cache_ccfsm & operator=(const cache_ccfsm &) = delete;

public:
	//	Cache 接口
	void begin(const pr_data_opt_t & a);
	void begin(const pr_data_opt_t & a, const pr_data_val_t & v);

	void confirm(void);

	void wait_complete(void) const;
	void wait_idle(void) const;

	//	Snoop 接口
	void commit(void);

	const cache_ccfsm_t & get_state(void) const { return state; }

	const pr_data_opt_t & get_maddr(void) const { return maddr; }
	const pr_data_val_t & get_value(void) const { return value; }
};

#endif	//	__MO_SIM_CACHE_CCFSM_HEADER__

