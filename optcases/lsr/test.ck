Input file: /mnt/e/Compiler/NKU-Compiler2025/optcases/lsr//test.ll
Step: -check
Output: standard output
Optimize level: 0

lsr:
    gep_strength_reduce:
        Function: main, Block: Block2
            -> %reg_13 = getelementptr %reg_5[%reg_12] 其中 %reg_12 = {1023, +, -1}
            可以通过强度削减变为:
            - 循环前: ptr = getelementptr %reg_5[1023]
            - 循环内: ptr = ptr + (-1 * 4 字节)
        Function: main, Block: Block7
            -> %reg_38 = getelementptr %reg_5[%reg_37] 其中 %reg_37 = {%reg_21, +, -2}
            可以通过强度削减变为:
            - 循环前: ptr = getelementptr %reg_5[%reg_21]
            - 循环内: ptr = ptr + (-2 * 4 字节)
    mul_strength_reduce:
        Function: main, Block: Block7
            -> %reg_33 = 4 * {%reg_21, +, -1} 可以通过强度削减变为辅助归纳变量 {4*%reg_21, +, 4*-1}
        Function: main, Block: Block7
            -> %reg_37 = 2 * {%reg_21, +, -1} 可以通过强度削减变为辅助归纳变量 {2*%reg_21, +, 2*-1}
