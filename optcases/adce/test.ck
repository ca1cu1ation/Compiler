Input file: /mnt/e/Compiler/NKU-Compiler2025/optcases/adce//test.ll
Step: -check
Output: standard output
Optimize level: 0

inline:
    very_small_function:
        Function: main, Block: Block3
            -> %reg_2130 = 调用函数 'add' 应该被内联: Very small function (2 instructions <= 15)
