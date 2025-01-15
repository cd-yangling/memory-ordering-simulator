# 内存屏障模拟器

## 程序总体介绍
本程序是参考 [pdf目录](https://github.com/cd-yangling/memory-ordering-simulator/tree/main/pdf) 模仿硬件的行为实现的一个虚拟CPU。
整个结构包括 处理器(processor),后端总线(bsb),L1缓存(cache),前端总线(fsb),内存控制器(mem)。他们共同组合成一个完整的SoC。
前端总线与真正的总线一样，具备总线仲裁，总线共享,总线独占等特点。成为实现 MESI 协议的关键。目前模拟器的总体系统的架构如下


     +-----------------+         +-----------------+
     |                 |         |                 |
     |       cpu       |         |       cpu       |
     |                 |         |                 |
     +-----------------+         +-----------------+
        /|\       |                 /|\       |
         |        |                  |        |
         |   +---------+             |   +---------+
         |<--|store buf|             |<--|store buf|    back side bus
         |   +---------+             |   +---------+
         |        |                  |        | 
         |       \|/                 |       \|/
     +-----------------+         +-----------------+
     |                 |         |                 |
     |   cache         |         |   cache         |
     |         +-------+         |         +-------+
     |         |       |         |         |       |
     |         | snoop |         |         | snoop |
     |         |       |         |         |       |
     +---------+-------+         +---------+-------+
               |                           |
               |                           |
               |                           |
         +-----|-----+               +-----|-----+
         |invalid que|               |invalid que|
         +-----|-----+               +-----|-----+
               |                           |
               |                           |
               |                           |
               +------------bus------------+           front side bus
                             |
                             |
                            mem

## MESI 协议转换图

```bash

                                      +------------+                                            
     +------------------------------->|            |-------------------------------+              
     |                                |      M     |                               |              
     |         +--------------------->|            |----------------------+        |              
     |         |                      +------------+                      |        |              
     |         |                      /|\                                 |        |              
     |         |                       |                                  |        |              
     |         |                       |                                  |        |              
     |         |                     PrWr                                 |        |              
     |         |                       |                            BusRd/Flush    |              
     |    PrWr/BusUgr                  |                                  |        |              
     |         |                       |                                  |        |              
     |         |                      +------------+                      |        |              
     |         |                      |            |                      |        |              
     |         |      +-------------->|      E     |---------------+      |        |              
     |         |      |               |    PrRd    |               |      |        |              
     |         |      |               +------------+               |      |        |              
     |         |      |                           |                |      |        |              
     |         |      |                           |                |      |        |              
     |         |      |                           |                |      |        |              
 PrWr/BusRdX   |      |                         BusRd              |      |   BusRdX/Flush        
     |         |      |                           |                |      |        |              
     |         |      |                           |                |      |        |              
     |         |      |                          \|/               |      |        |              
     |         |      |               +------------+               |      |        |              
     |         |      |               |            |               |      |        |              
     |         +----------------------|      S     |<--------------|------+        |              
     |                |               | PrRd/BusRd |               |               |              
     |                |               +------------+               |               |              
     |                |               /|\         |                |               |              
     |                |                |          |                |               |              
     |                |                |       BusRdX           BusRdX             |              
     |           PrRd/BusRd       PrRd/BusRd      |                |               |              
     |        (没有其他cache拥有)(其他cache拥有)  |                |               |              
     |                |                |          |                |               |              
     |                |                |         \|/               |               |              
     |                |               +------------+               |               |              
     |                +---------------|            |<--------------+               |              
     |                                |      I     |                               |              
     +--------------------------------|            |<------------------------------+              
                                      +------------+                                              

```



## 程序目录说明

[assembler](https://github.com/cd-yangling/memory-ordering-simulator/tree/main/assembler) 用于编译适合本虚拟CPU的汇编编译器

[example](https://github.com/cd-yangling/memory-ordering-simulator/tree/main/example) 用于测试本虚拟CPU的样本汇编代码，包括基本样本，伪共享样本，共享交叉自增样本 (Store-Load 样本, Store-Stroe 样本) 

## 计划添加特征

* StoreBuffer (已完成)
* Invalid Queue

## 程序基本使用

最低需要 c++20 (主要里面使用一个信号量), 如果你需要自己编写测试样本，可以参考 [这里](https://github.com/cd-yangling/memory-ordering-simulator/blob/main/instruction.h) 有详细的介绍指令的格式与说明

### 编译选项介绍

* ENABLE_FAIR_BUS_ARBITRATION

系统总线的仲裁方式,默认是抢占方式, 使用 `cmake -DENABLE_FAIR_BUS_ARBITRATION=ON` 将会使用 FIFO 的方式进行系统总线仲裁

* ENABLE_STORE_BUFFER

是否打开 StoreBuffer 的功能,默认是打开。 使用 `cmake -DENABLE_STORE_BUFFER=OFF` 将可以关闭 Store Buffer 的功能

* ENABLE_INVALID_QUEUE

暂未开发支持


### 模拟器的编译
```bash
git clone https://github.com/cd-yangling/memory-ordering-simulator
cd memory-ordering-simulator && mkdir build && cd build && cmake .. && make
```

### 样本代码清单

|文件名|描述|
|-----|---|
|[basic_test.asm](https://github.com/cd-yangling/memory-ordering-simulator/blob/main/example/basic_test.asm) |  基本测试样本 |
|[cache_mesi_cross_inc.asm](https://github.com/cd-yangling/memory-ordering-simulator/blob/main/example/cache_mesi_cross_inc.asm) |  测试MESI协议的正确性的交叉++ 样本 |
|[false_sharing.asm](https://github.com/cd-yangling/memory-ordering-simulator/blob/main/example/false_sharing.asm) |  伪共享测试样本 |
|[single_mm.asm](https://github.com/cd-yangling/memory-ordering-simulator/blob/main/example/single_mm.asm) |  基本读/写样本 |
|[store_load.asm](https://github.com/cd-yangling/memory-ordering-simulator/blob/main/example/store_load.asm) | Store-Load 样本 |
|[store_store.asm](https://github.com/cd-yangling/memory-ordering-simulator/blob/main/example/store_store.asm) | Store-Store 样本 |

### 编译测试样本代码
```bash
for each in `ls ../example/*.asm`; do assembler/mos_asm ../example/$each -o `basename $each .asm`.bin;  done
```

### 编译器的帮助

```bash
assembler/mos_asm -h

mos_asm [-o outfile] infile
  -o   Set output file, if -o is not specified, the default is to put an executable file in a.out.
  -d   Enable output final machine code for debugging .
  -h   Print the help info and quit.
  -v   Print the version and quit.
```

### 模拟器的帮助

```bash
mos_run [-0 infile | -1 infile | -2 infile | -3 infile]
  -0   Set the instruction file to be processed by processor 0, Default file is a.out.
  -1   Set the instruction file to be processed by processor 1, Default file is a.out.
  -2   Set the instruction file to be processed by processor 2, Default file is a.out.
  -3   Set the instruction file to be processed by processor 3, Default file is a.out.
  -h   Print the help info and quit.
  -v   Print the version and quit.
```


    