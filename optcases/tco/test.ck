Input file: /mnt/e/Compiler/NKU-Compiler2025/optcases/tco//test.ll
Step: -check
Output: standard output
Optimize level: 0

inline:
    very_small_function:
        Function: main, Block: Block0
            -> %reg_2 = 调用函数 'fib' 应该被内联: Very small function (2 instructions <= 15)

tco:
    tail_recursion:
        Function: fib_impl, Block: Block4
            -> %reg_22 = 对 'fib_impl' 的尾递归调用可以转换为循环 (%reg_5, %reg_2, %reg_6)
