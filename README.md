# CHIP-8

> CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker made on his 1802 Microprocessor. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. CHIP-8 programs are run on a CHIP-8 virtual machine. It was made to allow video games to be more easily programmed for these computers.
>

CHIP-8 是一种解释型语言，设计之初就是为了更新简单的编写和设计小游戏。我猜是作者嫌老机器的汇编语言太复杂繁琐，从而自己设计了一门汇编语言，并且摆脱硬件的束缚，在模拟器上运行。其实这个思想和Java等基于虚拟机的高级语言也是类似的，提供方便程序员编写的指令集，在硬件之上空架一层虚拟机，实现 "Write Once, Run Everywhere"。

## [虚拟机描述](https://en.wikipedia.org/wiki/CHIP-8?useskin=vector#Virtual_machine_description)

- Memory：CHIP-8 最多有 4096 字节的内存

> CHIP-8 解释器本身占用这些机器上的前 512 字节内存空间。因此，为原始系统编写的大多数程序都从内存位置 512 (0x200) 开始，并且不会访问位置 512 (0x200) 以下的任何内存。最上面的 256 个字节 (0xF00-0xFFF) 保留用于显示刷新，下面的 96 个字节 (0xEA0-0xEFF) 保留用于调用堆栈、内部使用和其他变量。

- Program Counter：16 位的 PC，因为需要访问最多 4K 的内存（0xFFF）

- Stack：16位地址的堆栈，用于调用函数和返回。栈调用深度最初设计位12层，可以自行调整。

- Registers：
  - 16 个 8 位数据寄存器（data register），名为 V0 至 VF。 VF 寄存器兼作某些指令的标志；因此，应该避免这种情况。在加法运算中，VF 是进位标志，而在减法运算中，VF 是“无借位”标志。在绘制指令中，VF 在像素冲突时设置。
  - 一个 16 位索引寄存器（index register），用于记录内存指针

- Timers
  - 8 位延迟定时器，以 60 Hz（每秒 60 次）的速率递减，直至达到 0
  - 8 位声音定时器，当其值非零时，会发出蜂鸣声。

- Display：64 x 32 像素（或 128 x 64 对于 SUPER-CHIP）单色，即黑或白

- Inputs：16 个输入键，与前 16 个十六进制值匹配：0 到 F。

- Opcode：CHIP-8 有 35 个操作码，都是两个字节长并以大端存储。下面列出了操作码，以十六进制表示，并带有以下符号：
  - NNN: 地址
  - NN: 8位常量
  - N: 4位常数
  - X 和 Y：4 位寄存器标识符
  - PC : Program Counter
  - I：16bit寄存器（用于内存地址）（类似于void指针）；
  - VN：16 个可用变量之一。 N可以是0到F（十六进制）；
