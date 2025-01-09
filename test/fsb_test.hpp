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
 *	test/fsb_test.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#if defined(HAVE_FSB_TEST)

#ifndef	___MO_SIM_FSB_TEST_HEADER__
#define	___MO_SIM_FSB_TEST_HEADER__

#include <cstdio>
#include <cassert>
#include <thread>
#include "../bsb.hpp"
#include "../fsb.hpp"
#include "../cache.hpp"
#include "../memory.hpp"

class fsb_tester
{
public:
	static void fsb_tester_cpuX(
		const std::uint32_t addr, const std::uint32_t nr_test,
		const std::uint32_t nr_cpus, const std::uint32_t c_id,
		bsb * bbus, cache * cobj, memory * mem);

public:
	static void fsb_tester_main(void);
};

#endif	//	___MO_SIM_FSB_TEST_HEADER__
#endif	//	HAVE_FSB_TEST

