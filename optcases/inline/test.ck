Input file: /mnt/e/Compiler/NKU-Compiler2025/optcases/inline//test.ll
Step: -check
Output: standard output
Optimize level: 0

inline:
    very_small_function:
        Function: xor, Block: Block0
            -> %reg_7 = 调用函数 'not' 应该被内联: Very small function (6 instructions <= 15)

licm:
    invariant_call:
        Function: main, Block: Block8
            -> %reg_126 = 调用函数 'mod'可以被提升（参数为循环不变量且函数无副作用）
    scalar_invariant:
        Function: main, Block: Block8
            -> %reg_157 = 标量指令是循环不变的但未被提升
