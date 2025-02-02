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
 *	store_buffer.hpp
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

#ifndef	__MO_SIM_STORE_BUFFER_HEADER__
#define	__MO_SIM_STORE_BUFFER_HEADER__

#include <cstddef>
#include "basic_type.cpp"

class store_buffer
{
private:
	bool							flag;	//	true: 写栅栏标志
	const pr_data_opt_t				addr;
	pr_data_val_t					data;

public:
	store_buffer() = delete;
	store_buffer(const store_buffer &) = delete;
	store_buffer & operator=(const store_buffer &) = delete;

	store_buffer(const pr_data_opt_t & a, const pr_data_val_t & d);

public:
	void set_flag(void);
	void set_data(const pr_data_val_t &);
	const pr_data_val_t & get_data(void) const;
	const pr_data_opt_t & get_addr(void) const;
};

#endif	//	__MO_SIM_STORE_BUFFER_HEADER__
#endif	//	HAVE_STORE_BUFFER

