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
 *	fsb.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_FSB_HEADER__
#define	__MO_SIM_FSB_HEADER__

#include <cstdint>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "basic_type.cpp"

/*
 *	front side bus 前端总线 (仲裁方式获得主设备权利)
 *
 *  +-------+      +-------+      +-------+
 *  |       |      |       |      |       |
 *  | cache |      | cache |      | cache |
 *  |       |      |       |      |       |
 *  +-------+      +-------+      +-------+
 *      |              |              |
 *      |              |              |
 *      +-------------bus-------------+
 *                     |
 *                     |
 *                 +-------+
 *                 |       |
 *                 |  mem  |
 *                 |       |
 *                 +-------+
 *
 * Shared:        'OR' of result from all processors
 * Dirty:         'OR' of result from all processors
 * Snoop-pending: 'OR' of result from all processors
 *
 * These three lines are additional bus interconnect hardware!
 * 
 *  每个 Cache 模块存在一个 snoop controller 嗅探总线的情况
 */
enum class fsb_control_t { None, BusRd, BusUgr, BusRdX, Flush };

class fsb
{
private:
	mutable std::mutex					cs;
	mutable std::condition_variable		master;
	mutable std::condition_variable		slave;

	std::uint32_t						mask;			//	所有的连接设备(Cache/Mem)的掩码

#ifdef	HAVE_FAIR_BUS_ARBITRATION
	std::queue<std::uint32_t>			mque;			//	等待总线控制权的等待队列
	std::uint32_t						mbit;			//	获得总线控制权的 id 位图(防止代码重复申请)
#else
	std::uint32_t						mreq;			//	申请总线控制权的 id 位图(有申请意向,但是未得到)
#endif

	std::uint32_t						winner;			//	arbitration

	std::uint32_t						work_ack;		//	(bits) Snoop-pending
	std::uint32_t						data_dirty;		//	(bits) data dirty
	std::uint32_t						data_shared;	//	(bits) data shared

	fsb_control_t						ctrl_bus;		//	控制线(总线规定:只能主设备操作)
	cl_data_tag_t						addr_bus;		//	地址线(总线规定:只能主设备操作)
	cl_data_val_t						data_bus;		//	数据线(通常实际硬件应该是通过多个周期传输)

public:
	fsb();
	fsb(const fsb & ) = delete;
	fsb & operator=(const fsb &) = delete;

private:
	static bool invalid_id(const std::uint32_t id) { return (id >= 32) ? true : false; }
	static void set_bit(const std::uint32_t b, std::uint32_t & v) { v |= ( (1 << b)); }
	static void clr_bit(const std::uint32_t b, std::uint32_t & v) { v &= (~(1 << b)); }
	static bool tst_bit(const std::uint32_t b, const std::uint32_t v) { return ((1 << b) & v) ? true : false; }

public:
	void attach(const std::uint32_t id);
	void detach(const std::uint32_t id);

public:
	void acquire(const std::uint32_t id);	//	申请总线控制权
	void release(const std::uint32_t id);	//	释放总线控制权
#ifndef	HAVE_FAIR_BUS_ARBITRATION
	bool tryrace(const std::uint32_t id);	//	竞争总线控制权
#endif

public:
	const std::uint32_t & get_data_dirty(void) const { return data_dirty; }
	const std::uint32_t & get_data_shared(void) const { return data_shared; }
	const fsb_control_t & get_ctrl_bus(void) const { return ctrl_bus; }
	const cl_data_tag_t & get_addr_bus(void) const { return addr_bus; }
	const cl_data_val_t & get_data_bus(void) const { return data_bus; }

	//	主设备接口
	void master_snoop(const std::uint32_t id) const;

	//	从设备接口
	bool slave_snoop(const std::uint32_t id) const;

	void BusRd(const std::uint32_t id, const cl_data_tag_t & tag);
	void BusUgr(const std::uint32_t id, const cl_data_tag_t & tag);
	void BusRdX(const std::uint32_t id, const cl_data_tag_t & tag);
	void Flush(const std::uint32_t id, const cl_data_tag_t & tag, const cl_data_val_t & dat);

	//	memory 接口
	void BusRd_ack(const std::uint32_t id, const mm_data_val_t & dat);
//	void BusUgr_ack(const std::uint32_t id);
	void BusRdX_ack(const std::uint32_t id, const mm_data_val_t & dat);
//	void Flush_ack(const std::uint32_t id);

	//	cache 接口
	void BusRd_ack(const std::uint32_t id, const cl_data_val_t & dat, const bool dirty, const bool shared);
	void BusUgr_ack(const std::uint32_t id);
	void BusRdX_ack(const std::uint32_t id, const cl_data_val_t & dat, const bool dirty, const bool shared);
	void Flush_ack(const std::uint32_t id);
};

#endif	//	__MO_SIM_FSB_HEADER__

