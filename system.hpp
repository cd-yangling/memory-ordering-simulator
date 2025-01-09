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
 *	system.hpp
 *
 *	Copyright (C) 2024 SCTY
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2024-12-14 Created By YangLing (yl.tienon@gmail.com)
 */

#ifndef	__MO_SIM_SYSTEM_HEADER__
#define	__MO_SIM_SYSTEM_HEADER__

#include <cstdint>
#include <string>
#include <thread>
#include <semaphore>
#include <cassert>
#include <chrono>

class system
{
private:
	static std::binary_semaphore sem_a;
	static std::binary_semaphore sem_b;
	static std::binary_semaphore sem_c;
	static std::binary_semaphore sem_d;
	static std::binary_semaphore sem_e;
	static std::binary_semaphore sem_f;

public:
	static void oops(const char * f, std::uint32_t l)
	{
		while (1)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1000ms);
			std::printf("BUG occurred: %s %d\n", f, l);

			assert(0);
		}
	}

	static std::binary_semaphore * sem_ref(const std::uint32_t n)
	{
		switch (n)
		{
			case 0: return &sem_a;
			case 1: return &sem_b;
			case 2: return &sem_c;
			case 3: return &sem_d;
			case 4: return &sem_e;
			case 5: return &sem_f;
		}

		return nullptr;
	}

	static std::uint64_t get_count_tick(void)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}
};

#endif	//	__MO_SIM_SYSTEM_HEADER__

