# 内存屏障模拟器

## 程序总体介绍
本程序是参考 [pdf目录](https://github.com/cd-yangling/memory-ordering-simulator/tree/main/pdf) 模仿硬件的行为实现的一个虚拟CPU。整个结构包括 处理器(processor),后端总线(bsb),L1缓存,前端总线(fsb),内存控制器。他们共同组合成一个完整的Soc。前端总线与真正的总线一样，具备总线仲裁，总线共享等特点。成为实现 MESI 协议的关键。总体系统的架构如下

 
             cpu                         cpu
              |                           |
              |                           |
     +-----------------+         +-----------------+
     |                 |         |                 |
     |   cache         |         |   cache         |
     |         +-------+         |         +-------+
     |         |       |         |         |       |
     |         | snoop |         |         | snoop |
     |         |       |         |         |       |
     +---------+-------+         +---------+-------+
              |                            |
              |                            |
              +-------------bus------------+
                             |
                             |
                            mem


## 程序目录说明

[汇编器](https://github.com/cd-yangling/memory-ordering-simulator/tree/main/assembler) 用于编译适合本虚拟CPU的汇编编译器

[example](https://github.com/cd-yangling/memory-ordering-simulator/tree/main/example) 用于测试本虚拟CPU的样例汇编代码，包括基本测试，伪共享测试，（StoreLoad测试,StoreStroe测试） 尚未完成 StoreBuffer和Invalid Queue。基于MESI协议的处理情况下，目前是强内存序。

## 程序基本使用

最低需要 c++20(主要里面使用一个信号量), 用于该虚拟机的汇编指令说明请参考 instruction.h

```bash
git clone https://github.com/cd-yangling/memory-ordering-simulator
cd memory-ordering-simulator
mkdir build
cd build
cmake ..
make

# 编译测试汇编代码
mos_asm example/basic_test.asm                # 基本测试用例
mos_asm example/cache_mesi_cross_inc.asm      # 测试MESI协议的正确性的交叉++的用例
mos_asm example/false_sharing.asm             # 伪共享测试用例
mos_asm example/single_mm.asm                 # 基本读/写用例
mos_asm example/store_load.asm                # SL 用例
mos_asm example/store_store.asm               # SS 用例
# 编译的汇编代码最终默认是 a.out，mos_run运行即可, mos_run 也是默认打开a.out文件运行
./mos_run 
```


    