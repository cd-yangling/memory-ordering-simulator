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
 *	cache_line.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_CACHE_LINE_HEADER__
#define	__MO_SIM_CACHE_LINE_HEADER__

#include <cstdint>
#include "basic_type.cpp"

enum class cache_state_t
{
	MESI_CACHE_I	=	0,
	MESI_CACHE_S	=	1,
	MESI_CACHE_E	=	2,
	MESI_CACHE_M	=	3,
};

class cache_line
{
public:
	cache_line();
	cache_line(const cache_line & ) = delete;
	cache_line & operator=(const cache_line &) = delete;

private:
	cl_data_tag_t	tag;	//	缓存行的 tag
	cl_data_val_t	dat;	//	缓存行的数据
	cache_state_t	state;	//	缓存行的状态

public:
	//	tag
	bool cmp_tag(const pr_data_opt_t & par) const;
	bool cmp_tag(const cl_data_tag_t & par) const;

	const cl_data_tag_t & get_tag(void) const;

	void set_tag(const pr_data_opt_t & par);
	void set_tag(const cl_data_tag_t & par);

	//	dat
	const pr_data_val_t & get_dat(const pr_data_opt_t & par) const;
	const cl_data_val_t & get_dat(void) const;

	void set_dat(const pr_data_opt_t & par, const pr_data_val_t & val);
	void set_dat(const cl_data_val_t & par);

	//	state
	cache_state_t get_state(void) const;
	void set_state(const cache_state_t s);
};

#endif	//	__MO_SIM_CACHE_LINE_HEADER__

