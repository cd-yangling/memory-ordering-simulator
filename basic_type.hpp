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
 *	basic_type.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_BASIC_TYPE_HEADER__
#define	__MO_SIM_BASIC_TYPE_HEADER__

#include <cstdint>
#include <array>
#include <limits>

//	本文件描述了模拟器的处理单元的字宽定义
//	1: 处理器的字长是 32 Bits
//	2: 主内存的字长是128 Bits
//	3: 缓存行的字长是128 Bits
//
//	主内存模拟的最大值是 4GiB
//
//	我并没实现Cache N way 的处理为了方便测试写回
//	没有 index 字段,我们的测试目的该字段并不重要
//	下图描述了一个处理器的字长被切割的位图描述图
//
// 31                           3  0
//	+---------------------------+--+
//	|A                          |B |
//	+------------------------------+
//	A: cache line tag
//	B: cache line off (16字节)
//
//	Processor 提供的操作地址必须是 4 字节对齐
//

//	std::bitset 满足不了部分操作需求.所以没使用

namespace mos
{
	template<std::uint64_t nr_bits>
	class bitexp
	{
		static_assert(nr_bits <= 64 && 0 != nr_bits);

	private:
		constexpr std::uint64_t mask_limit(void)
		{
			if (64 != nr_bits)
				return (((std::uint64_t)1 << nr_bits) - 1);
			else
				return std::numeric_limits<std::uint64_t>::max();
		}

	private:
		std::uint64_t v;
		const std::uint64_t m;

	public:
		std::uint64_t to_number(void) const {return v & m;}

	public:
		bitexp();
		bitexp(const bitexp &);
		bitexp(const std::uint64_t x);

	public:
		bitexp & operator =  (const bitexp &);
		bitexp & operator ++ (void);

		bool operator != (const bitexp &) const;
		bool operator == (const bitexp &) const;
		bool operator <  (const bitexp &) const;
		bool operator <= (const bitexp &) const;
		bool operator >  (const bitexp &) const;
		bool operator >= (const bitexp &) const;

//		bitexp operator >> (const std::uint64_t x) const;
//		bitexp operator << (const std::uint64_t x) const;

		bitexp operator +  (const bitexp &) const;
		bitexp operator -  (const bitexp &) const;
		bitexp operator *  (const bitexp &) const;
		bitexp operator /  (const bitexp &) const;
		bitexp operator %  (const bitexp &) const;

		bitexp operator &  (const bitexp &) const;
		bitexp operator |  (const bitexp &) const;
		bitexp operator ^  (const bitexp &) const;
		bitexp operator ~  (void) const;
	};
};

using bit_28 = mos::bitexp<28>;
using bit_30 = mos::bitexp<30>;
using bit_32 = mos::bitexp<32>;

//	处理器字长 (32 Bits)
using pr_word_len_t = bit_32;

//	主内存字长 (128 Bits)
using mm_word_len_t = std::array<pr_word_len_t, 4>;

//	缓存行字长 (128 Bits)
using cl_word_len_t = std::array<pr_word_len_t, 4>;

//	缓存行 tag
using cl_data_tag_t = bit_28;

//	缓存行 dat
using cl_data_val_t = cl_word_len_t;

//	处理器操作数类型
using pr_oper_num_t = pr_word_len_t;

//	处理器寄存器类型
using pr_register_t = pr_word_len_t;

//	处理器数据操作字长 ( 30 Bits)(4字节对齐)
using pr_data_opt_t = bit_30;

//	处理器数据内容字长 ( 32 Bits)
using pr_data_val_t = pr_word_len_t;

//	内存控制器地址索引 (28 Bits)
using mm_data_tag_t = bit_28;

//	内存控制器内容字长 (128 Bits)
using mm_data_val_t = mm_word_len_t;

#endif	//	__MO_SIM_BASIC_TYPE_HEADER__

