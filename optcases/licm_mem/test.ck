Input file: /mnt/e/Compiler/NKU-Compiler2025/optcases/licm_mem//test.ll
Step: -check
Output: standard output
Optimize level: 0

cse:
    domtree_cse:
        Function: main, Block: Block5
            -> %reg_41 = add 是 %reg_24 的公共子表达式（定义在支配块 Block3 中）
        Function: main, Block: Block5
            -> %reg_46 = add 是 %reg_29 的公共子表达式（定义在支配块 Block3 中）
        Function: main, Block: Block5
            -> %reg_50 = add 是 %reg_24 的公共子表达式（定义在支配块 Block3 中）
        Function: main, Block: Block5
            -> %reg_55 = add 是 %reg_29 的公共子表达式（定义在支配块 Block3 中）
        Function: main, Block: Block5
            -> %reg_60 = add 是 %reg_24 的公共子表达式（定义在支配块 Block3 中）
        Function: main, Block: Block5
            -> %reg_64 = add 是 %reg_29 的公共子表达式（定义在支配块 Block3 中）
    local_cse:
        Function: main, Block: Block5
            -> %reg_50 = add 是 %reg_41 的公共子表达式
        Function: main, Block: Block5
            -> %reg_55 = add 是 %reg_46 的公共子表达式
        Function: main, Block: Block5
            -> %reg_60 = add 是 %reg_41 的公共子表达式
        Function: main, Block: Block5
            -> %reg_64 = add 是 %reg_46 的公共子表达式

licm:
    invariant_load:
        Function: main, Block: Block5
            -> %reg_44 = 从全局变量 @g 的 load 可以被提升
    scalar_invariant:
        Function: main, Block: Block5
            -> %reg_41 = 标量指令是循环不变的但未被提升

sccp:
    constant_operand:
        Function: main, Block: Block3
            -> 操作数 %8 可以替换为常量（i32 0）
        Function: main, Block: Block3
            -> 操作数 %10 可以替换为常量（i32 1）
        Function: main, Block: Block5
            -> 操作数 %17 可以替换为常量（i32 0）
        Function: main, Block: Block5
            -> 操作数 %22 可以替换为常量（i32 1）
        Function: main, Block: Block5
            -> 操作数 %24 可以替换为常量（i32 0）
        Function: main, Block: Block5
            -> 操作数 %29 可以替换为常量（i32 1）
        Function: main, Block: Block5
            -> 操作数 %31 可以替换为常量（i32 0）
        Function: main, Block: Block5
            -> 操作数 %35 可以替换为常量（i32 1）
    foldable_instruction:
        Function: main, Block: Block3
            -> %reg_24 = 指令结果为常量（i32 0）但未被折叠
        Function: main, Block: Block3
            -> %reg_29 = 指令结果为常量（i32 1）但未被折叠
        Function: main, Block: Block5
            -> %reg_41 = 指令结果为常量（i32 0）但未被折叠
        Function: main, Block: Block5
            -> %reg_46 = 指令结果为常量（i32 1）但未被折叠
        Function: main, Block: Block5
            -> %reg_50 = 指令结果为常量（i32 0）但未被折叠
        Function: main, Block: Block5
            -> %reg_55 = 指令结果为常量（i32 1）但未被折叠
        Function: main, Block: Block5
            -> %reg_60 = 指令结果为常量（i32 0）但未被折叠
        Function: main, Block: Block5
            -> %reg_64 = 指令结果为常量（i32 1）但未被折叠
