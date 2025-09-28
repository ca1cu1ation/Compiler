.arch armv8-a               //指定CPU架构为armv8-a (64位)
    .text                       //声明以下为代码段
    .global	a                   //声明全局变量'a'，外部可见
    .section	.rodata           //切换到只读数据段
    .align	2                   //4字节对齐 (2^2)
    .type	a, %object          //声明'a'是一个数据对象
    .size	a, 4                //声明'a'的大小为4字节
a:
    .word	7                   //定义'a'的值为7
    .global	b                   //声明全局变量'b'，外部可见
    .data                       //切换到可读写数据段
    .align	3                   //8字节对齐 (2^3)
    .type	b, %object          //声明'b'是一个数据对象
    .size	b, 8                //声明'b'的大小为8字节
b:
    .word	1                   //定义数组'b'的第一个元素为1
    .word	2                   //定义数组'b'的第二个元素为2
    .text                       //切换回代码段
    .align	2                   //4字节对齐
    .global	recursive_sum       //声明全局函数'recursive_sum'
    .type	recursive_sum, %function //声明'recursive_sum'的类型为函数
recursive_sum:
.LFB0:                      //函数开始的局部标签
    .cfi_startproc              //(CFI) 标记过程(函数)的开始
    stp	x29, x30, [sp, -32]!    //将x29(FP)和x30(LR)入栈，同时sp-=32
    .cfi_def_cfa_offset 32      //(CFI) 定义调用帧地址(CFA)的偏移为32
    .cfi_offset 29, -32         //(CFI) 记录x29保存在CFA-32的位置
    .cfi_offset 30, -24         //(CFI) 记录x30保存在CFA-24的位置
    mov	x29, sp                 //设置新的栈帧指针: FP = SP
    str	w0, [sp, 28]            //将参数n (在w0中)保存到栈上[sp+28]
    ldr	w0, [sp, 28]            //从栈上加载n到w0
    cmp	w0, 0                   //比较 n 和 0 (if n == 0)
    bne	.L2                     //如果不等于0，跳转到.L2
    mov	w0, 0                   //(n==0) 返回值为0
    b	.L3                     //跳转到函数出口
.L2:                        //(n!=0)
    ldr	w0, [sp, 28]            //从栈上加载n到w0
    sub	w0, w0, #1              //计算 n-1
    bl	recursive_sum           //递归调用 recursive_sum(n-1)
    mov	w1, w0                  //将递归调用的返回值(在w0)暂存到w1
    ldr	w0, [sp, 28]            //从栈上加载n到w0
    add	w0, w1, w0              //计算 n + recursive_sum(n-1)，结果在w0
.L3:                        //函数出口
    ldp	x29, x30, [sp], 32      //将x29(FP)和x30(LR)出栈，同时sp+=32
    .cfi_restore 30             //(CFI) 恢复x30
    .cfi_restore 29             //(CFI) 恢复x29
    .cfi_def_cfa_offset 0       //(CFI) 恢复CFA偏移
    ret                         //从函数返回
    .cfi_endproc                //(CFI) 标记过程(函数)的结束
.LFE0:                      //函数结束的局部标签
    .size	recursive_sum, .-recursive_sum //定义函数'recursive_sum'的大小
    .section	.rodata           //切换到只读数据段
    .align	3                   //8字节对齐
.LC0:
    .string	"%d"                //定义格式化字符串"%d"
    .text                       //切换回代码段
    .section	.rodata           //切换到只读数据段
    .align	3                   //8字节对齐
.LC1:
    .string	"%d\n"              //定义格式化字符串"%d\n"
    .text                       //切换回代码段
    .align	2                   //4字节对齐
    .global	getint              //声明全局函数'getint'
    .type	getint, %function   //声明'getint'的类型为函数

    .align	2                   //4字节对齐
    .global	putint              //声明全局函数'putint'
    .type	putint, %function   //声明'putint'的类型为函数
    .align	2                   //4字节对齐
    .global	main                //声明全局函数'main'
    .type	main, %function     //声明'main'的类型为函数
main:
.LFB3:                      //函数开始的局部标签
    .cfi_startproc              //(CFI) 标记过程(函数)的开始
    stp	x29, x30, [sp, -32]!    //将x29(FP)和x30(LR)入栈，同时sp-=32
    .cfi_def_cfa_offset 32      //(CFI) 定义调用帧地址(CFA)的偏移为32
    .cfi_offset 29, -32         //(CFI) 记录x29保存在CFA-32的位置
    .cfi_offset 30, -24         //(CFI) 记录x30保存在CFA-24的位置
    mov	x29, sp                 //设置新的栈帧指针: FP = SP
    bl	getint                  //调用getint()函数，返回值在w0
    str	w0, [sp, 24]            //将输入的n保存到栈上[sp+24]
    ldr	w0, [sp, 24]            //从栈上加载n到w0
    and	w0, w0, 1               //计算 n % 2 (n & 1)
    cmp	w0, 1                   //比较 n%2 和 1 (if n is odd)
    bne	.L9                     //如果不等于1 (n是偶数)，跳转到.L9
    ldr	w0, [sp, 24]            //(n是奇数) 从栈上加载n到w0
    bl	recursive_sum           //调用 recursive_sum(n)
    mov	w1, w0                  //将返回值暂存到w1
    adrp	x0, b                   //加载全局变量b的高位地址
    add	x0, x0, :lo12:b         //加载全局变量b的低位地址，得到完整地址
    ldr	w0, [x0]                //加载b[0]的值到w0
    sub	w0, w1, w0              //计算 sum_res - b[0]
    str	w0, [sp, 28]            //将结果保存到栈上
    ldr	w0, [sp, 28]            //从栈上加载结果到w0
    bl	putint                  //调用putint()打印结果
    b	.L10                    //跳转到程序出口
.L9:                        //(n是偶数)
    mov	w0, 1                   //i = 1
    str	w0, [sp, 20]            //将i保存到栈上[sp+20]
    b	.L11                    //跳转到while循环条件检查
.L12:                       //while循环体
    ldr	w0, [sp, 20]            //从栈上加载i
    add	w0, w0, 1               //i = i + 1
    str	w0, [sp, 20]            //将新的i保存回栈上
.L11:                       //while循环条件检查
    adrp	x0, b                   //加载全局变量b的高位地址
    add	x0, x0, :lo12:b         //加载全局变量b的低位地址
    ldr	w0, [x0, 4]             //加载b[1]的值 (地址b+4)
    ldr	w1, [sp, 20]            //从栈上加载i
    cmp	w1, w0                  //比较 i 和 b[1] (while i < b[1])
    blt	.L12                    //如果 i < b[1]，继续循环
    mov	w0, 7                   //a = 7 (加载立即数7)
    ldr	w1, [sp, 20]            //从栈上加载i
    mul	w0, w1, w0              //a = a * i (实际上是 7*i)
    str	w0, [sp, 20]            //将结果保存到栈上(复用i的位置)
    ldr	w0, [sp, 24]            //从栈上加载n
    bl	recursive_sum           //调用 recursive_sum(n)
    mov	w1, w0                  //将返回值暂存到w1
    ldr	w0, [sp, 20]            //从栈上加载 a*i 的结果
    sdiv	w0, w1, w0              //计算 recursive_sum(n) / (a*i)
    str	w0, [sp, 28]            //将结果保存到栈上
    ldr	w0, [sp, 28]            //从栈上加载结果到w0
    bl	putint                  //调用putint()打印结果
.L10:                       //程序出口
    mov	w0, 0                   //设置main函数的返回值为0
    ldp	x29, x30, [sp], 32      //将x29(FP)和x30(LR)出栈，同时sp+=32
    .cfi_restore 30             //(CFI) 恢复x30
    .cfi_restore 29             //(CFI) 恢复x29
    .cfi_def_cfa_offset 0       //(CFI) 恢复CFA偏移
    ret                         //从main函数返回
    .cfi_endproc                //(CFI) 标记过程(函数)的结束
.LFE3:                      //函数结束的局部标签
    .size	main, .-main        //定义函数'main'的大小
