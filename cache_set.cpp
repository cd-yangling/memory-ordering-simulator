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
 *	cache_set.cpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#include <array>
#include "basic_type.cpp"
#include "cache_line.hpp"
#include "cache_set.hpp"

const cache_line * cache_set::at(const cl_data_tag_t & par, bool chk) const
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(set)>, std::array<cache_line, 16>>, "Must be bit_cl<16>");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, bit_28>, "MUST be bit_28");

	const cache_line * ptr = &set[par.to_number() % 16];

	if (!chk)
		return ptr;

	if (ptr->cmp_tag(par))
		return ptr;
	else
		return nullptr;
}

const cache_line * cache_set::at(const pr_data_opt_t & par, bool chk) const
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(set)>, std::array<cache_line, 16>>, "Must be bit_cl<16>");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, bit_30>, "MUST be bit_30");

	const cache_line * ptr = &set[(par.to_number() >> 2) % 16];

	if (!chk)
		return ptr;

	if (ptr->cmp_tag(par))
		return ptr;
	else
		return nullptr;
}

