Input file: /mnt/e/Compiler/NKU-Compiler2025/optcases/mem2reg//test.ll
Step: -check
Output: standard output
Optimize level: 0

cse:
    domtree_cse:
        Function: main, Block: Block5
            -> %reg_37 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
        Function: main, Block: Block8
            -> %reg_47 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
        Function: main, Block: Block11
            -> %reg_57 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
        Function: main, Block: Block14
            -> %reg_67 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
        Function: main, Block: Block17
            -> %reg_77 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
        Function: main, Block: Block20
            -> %reg_87 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
        Function: main, Block: Block23
            -> %reg_100 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
        Function: main, Block: Block26
            -> %reg_115 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
        Function: main, Block: Block29
            -> %reg_130 = srem(%reg_175) 是 %reg_29 的公共子表达式（定义在支配块 Block2 中）
