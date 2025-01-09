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
 *	cache_line.cpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#include <cstdint>
#include "cache_line.hpp"
#include "basic_type.cpp"

cache_line::cache_line()
{
	state = cache_state_t::MESI_CACHE_I;
}

//	tag
bool cache_line::cmp_tag(const pr_data_opt_t & par) const
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, bit_30>, "MUST be bit_30");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(tag)>, bit_28>, "MUST be bit_28");

	return tag.to_number() == (par.to_number() >> 2);
}

bool cache_line::cmp_tag(const cl_data_tag_t & par) const
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, bit_28>, "MUST be bit_28");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(tag)>, bit_28>, "MUST be bit_28");

	return tag == par;
}

const cl_data_tag_t & cache_line::get_tag(void) const
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(tag)>, bit_28>, "MUST be bit_28");

	return tag;
}

void cache_line::set_tag(const pr_data_opt_t & par)
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, bit_30>, "MUST be bit_30");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(tag)>, bit_28>, "MUST be bit_28");

	tag = par.to_number() >> 2;
}

void cache_line::set_tag(const cl_data_tag_t & par)
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, bit_28>, "MUST be bit_28");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(tag)>, bit_28>, "MUST be bit_28");

	tag = par;
}

//	dat
const pr_data_val_t & cache_line::get_dat(const pr_data_opt_t & par) const
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(dat)>, std::array<bit_32, 4>>, "Must be bit_128");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, bit_30>, "MUST be bit_30");

	return dat[par.to_number() % 4];
}

const cl_data_val_t & cache_line::get_dat(void) const
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(dat)>, std::array<bit_32, 4>>, "Must be bit_128");

	return dat;
}

void cache_line::set_dat(const pr_data_opt_t & par, const pr_data_val_t & val)
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(dat)>, std::array<bit_32, 4>>, "Must be bit_128");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, bit_30>, "MUST be bit_30");

	dat[par.to_number() % 4] = val;
}

void cache_line::set_dat(const cl_data_val_t & par)
{
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(dat)>, std::array<bit_32, 4>>, "Must be bit_128");
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(par)>, std::array<bit_32, 4>>, "Must be bit_128");

	dat = par;
}

//	state
cache_state_t cache_line::get_state(void) const
{
	return state;
}

void cache_line::set_state(const cache_state_t s)
{
	state = s;
}

