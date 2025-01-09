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
 *	basic_type.cpp
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
#include <array>
#include <bitset>
#include "basic_type.hpp"

#ifndef	__MO_SIM_BASIC_TYPE_IMPL__
#define	__MO_SIM_BASIC_TYPE_IMPL__

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits>::bitexp() : m(mask_limit())
{
	v = 0;
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits>::bitexp(const mos::bitexp<nr_bits> & x) : m(mask_limit())
{
	v = x.v;
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits>::bitexp(const std::uint64_t x) : m(mask_limit())
{
	v = x & mos::bitexp<nr_bits>::mask_limit();
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> & mos::bitexp<nr_bits>::operator = (const mos::bitexp<nr_bits> & x)
{
	v = x.v;
	return *this;
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> & mos::bitexp<nr_bits>::operator ++ (void)
{
	v++;
	return *this;
}

template<std::uint64_t nr_bits>
bool mos::bitexp<nr_bits>::operator != (const mos::bitexp<nr_bits> & x) const
{
	return v != x.v;
}

template<std::uint64_t nr_bits>
bool mos::bitexp<nr_bits>::operator == (const mos::bitexp<nr_bits> & x) const
{
	return v == x.v;
}

template<std::uint64_t nr_bits>
bool mos::bitexp<nr_bits>::operator <  (const mos::bitexp<nr_bits> & x) const
{
	return v <  x.v;
}

template<std::uint64_t nr_bits>
bool mos::bitexp<nr_bits>::operator <= (const mos::bitexp<nr_bits> & x) const
{
	return v <= x.v;
}

template<std::uint64_t nr_bits>
bool mos::bitexp<nr_bits>::operator >  (const mos::bitexp<nr_bits> & x) const
{
	return v >  x.v;
}

template<std::uint64_t nr_bits>
bool mos::bitexp<nr_bits>::operator >= (const mos::bitexp<nr_bits> & x) const
{
	return v >= x.v;
}

//template<std::uint64_t nr_bits>
//mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator >> (const std::uint64_t x) const
//{
//	return mos::bitexp<nr_bits>(v >> x);
//}

//template<std::uint64_t nr_bits>
//mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator << (const std::uint64_t x) const
//{
//	return mos::bitexp<nr_bits>(v << x);
//}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator +  (const mos::bitexp<nr_bits> & x) const
{
	return mos::bitexp<nr_bits>(v + x.v);
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator -  (const mos::bitexp<nr_bits> & x) const
{
	return mos::bitexp<nr_bits>(v - x.v);
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator *  (const mos::bitexp<nr_bits> & x) const
{
	return mos::bitexp<nr_bits>(v * x.v);
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator /  (const mos::bitexp<nr_bits> & x) const
{
	return mos::bitexp<nr_bits>(v / x.v);
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator %  (const mos::bitexp<nr_bits> & x) const
{
	return mos::bitexp<nr_bits>(v % x.v);
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator &  (const mos::bitexp<nr_bits> & x) const
{
	return mos::bitexp<nr_bits>(v & x.v);
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator |  (const mos::bitexp<nr_bits> & x) const
{
	return mos::bitexp<nr_bits>(v | x.v);
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator ^  (const mos::bitexp<nr_bits> & x) const
{
	return mos::bitexp<nr_bits>(v ^ x.v);
}

template<std::uint64_t nr_bits>
mos::bitexp<nr_bits> mos::bitexp<nr_bits>::operator ~  (void) const
{
	return mos::bitexp<nr_bits>(~v);
}

#endif	//	__MO_SIM_BASIC_TYPE_IMPL__

