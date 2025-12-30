Input file: /mnt/e/Compiler/NKU-Compiler2025/optcases/licm_scalar//test.ll
Step: -check
Output: standard output
Optimize level: 0

licm:
    scalar_invariant:
        Function: main, Block: Block2
            -> %reg_87 = 标量指令是循环不变的但未被提升
