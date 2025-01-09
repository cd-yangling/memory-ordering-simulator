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
 *	cache_set.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_CACHE_SET_HEADER__
#define	__MO_SIM_CACHE_SET_HEADER__

#include <array>
#include "basic_type.cpp"
#include "cache_line.hpp"

class cache_set
{
private:
	std::array<cache_line, 16>		set;

public:
	cache_set() {}
	cache_set(const cache_set & ) = delete;
	cache_set & operator=(const cache_set &) = delete;

private:
	const cache_line * at(const cl_data_tag_t & par, bool chk) const;
	const cache_line * at(const pr_data_opt_t & par, bool chk) const;

public:
	const cache_line * ref_cache_line(const cl_data_tag_t & par, bool chk) const
	{
		return at(par, chk);
	}

	const cache_line * ref_cache_line(const pr_data_opt_t & par, bool chk) const
	{
		return at(par, chk);
	}

	cache_line * ref_cache_line(const cl_data_tag_t & par, bool chk)
	{
		return const_cast<cache_line*>(at(par, chk));
	}

	cache_line * ref_cache_line(const pr_data_opt_t & par, bool chk)
	{
		return const_cast<cache_line*>(at(par, chk));
	}
};

#endif	//	__MO_SIM_CACHE_SET_HEADER__

