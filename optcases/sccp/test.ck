Input file: /mnt/e/Compiler/NKU-Compiler2025/optcases/sccp//test.ll
Step: -check
Output: standard output
Optimize level: 0

licm:
    scalar_invariant:
        Function: main, Block: Block7
            -> %reg_32 = 标量指令是循环不变的但未被提升

lsr:
    gep_strength_reduce:
        Function: main, Block: Block2
            -> %reg_17 = getelementptr %reg_9[%reg_852] 其中 %reg_852 = {0, +, 1}
            可以通过强度削减变为:
            - 循环前: ptr = getelementptr %reg_9[0]
            - 循环内: ptr = ptr + (1 * 4 字节)
        Function: main, Block: Block206
            -> %reg_832 = getelementptr %reg_9[%reg_851] 其中 %reg_851 = {0, +, 1}
            可以通过强度削减变为:
            - 循环前: ptr = getelementptr %reg_9[0]
            - 循环内: ptr = ptr + (1 * 4 字节)

sccp:
    constant_operand:
        Function: main, Block: Block7
            -> 操作数 %11 可以替换为常量（i32 1）
        Function: main, Block: Block9
            -> 操作数 %13 可以替换为常量（i32 2）
        Function: main, Block: Block11
            -> 操作数 %15 可以替换为常量（i32 3）
        Function: main, Block: Block13
            -> 操作数 %17 可以替换为常量（i32 4）
        Function: main, Block: Block15
            -> 操作数 %19 可以替换为常量（i32 5）
        Function: main, Block: Block17
            -> 操作数 %21 可以替换为常量（i32 6）
        Function: main, Block: Block19
            -> 操作数 %23 可以替换为常量（i32 7）
        Function: main, Block: Block21
            -> 操作数 %25 可以替换为常量（i32 8）
        Function: main, Block: Block23
            -> 操作数 %27 可以替换为常量（i32 9）
        Function: main, Block: Block25
            -> 操作数 %29 可以替换为常量（i32 10）
        Function: main, Block: Block27
            -> 操作数 %31 可以替换为常量（i32 11）
        Function: main, Block: Block29
            -> 操作数 %33 可以替换为常量（i32 12）
        Function: main, Block: Block31
            -> 操作数 %35 可以替换为常量（i32 13）
        Function: main, Block: Block33
            -> 操作数 %37 可以替换为常量（i32 14）
        Function: main, Block: Block35
            -> 操作数 %39 可以替换为常量（i32 15）
        Function: main, Block: Block37
            -> 操作数 %41 可以替换为常量（i32 16）
        Function: main, Block: Block39
            -> 操作数 %43 可以替换为常量（i32 17）
        Function: main, Block: Block41
            -> 操作数 %45 可以替换为常量（i32 18）
        Function: main, Block: Block43
            -> 操作数 %47 可以替换为常量（i32 19）
        Function: main, Block: Block45
            -> 操作数 %49 可以替换为常量（i32 20）
        Function: main, Block: Block47
            -> 操作数 %51 可以替换为常量（i32 21）
        Function: main, Block: Block49
            -> 操作数 %53 可以替换为常量（i32 22）
        Function: main, Block: Block51
            -> 操作数 %55 可以替换为常量（i32 23）
        Function: main, Block: Block53
            -> 操作数 %57 可以替换为常量（i32 24）
        Function: main, Block: Block55
            -> 操作数 %59 可以替换为常量（i32 25）
        Function: main, Block: Block57
            -> 操作数 %61 可以替换为常量（i32 26）
        Function: main, Block: Block59
            -> 操作数 %63 可以替换为常量（i32 27）
        Function: main, Block: Block61
            -> 操作数 %65 可以替换为常量（i32 28）
        Function: main, Block: Block63
            -> 操作数 %67 可以替换为常量（i32 29）
        Function: main, Block: Block65
            -> 操作数 %69 可以替换为常量（i32 30）
        Function: main, Block: Block67
            -> 操作数 %71 可以替换为常量（i32 31）
        Function: main, Block: Block69
            -> 操作数 %73 可以替换为常量（i32 32）
        Function: main, Block: Block71
            -> 操作数 %75 可以替换为常量（i32 33）
        Function: main, Block: Block73
            -> 操作数 %77 可以替换为常量（i32 34）
        Function: main, Block: Block75
            -> 操作数 %79 可以替换为常量（i32 35）
        Function: main, Block: Block77
            -> 操作数 %81 可以替换为常量（i32 36）
        Function: main, Block: Block79
            -> 操作数 %83 可以替换为常量（i32 37）
        Function: main, Block: Block81
            -> 操作数 %85 可以替换为常量（i32 38）
        Function: main, Block: Block83
            -> 操作数 %87 可以替换为常量（i32 39）
        Function: main, Block: Block85
            -> 操作数 %89 可以替换为常量（i32 40）
        Function: main, Block: Block87
            -> 操作数 %91 可以替换为常量（i32 41）
        Function: main, Block: Block89
            -> 操作数 %93 可以替换为常量（i32 42）
        Function: main, Block: Block91
            -> 操作数 %95 可以替换为常量（i32 43）
        Function: main, Block: Block93
            -> 操作数 %97 可以替换为常量（i32 44）
        Function: main, Block: Block95
            -> 操作数 %99 可以替换为常量（i32 45）
        Function: main, Block: Block97
            -> 操作数 %101 可以替换为常量（i32 46）
        Function: main, Block: Block99
            -> 操作数 %103 可以替换为常量（i32 47）
        Function: main, Block: Block101
            -> 操作数 %105 可以替换为常量（i32 48）
        Function: main, Block: Block103
            -> 操作数 %107 可以替换为常量（i32 49）
        Function: main, Block: Block105
            -> 操作数 %109 可以替换为常量（i32 50）
        Function: main, Block: Block107
            -> 操作数 %111 可以替换为常量（i32 51）
        Function: main, Block: Block109
            -> 操作数 %113 可以替换为常量（i32 52）
        Function: main, Block: Block111
            -> 操作数 %115 可以替换为常量（i32 53）
        Function: main, Block: Block113
            -> 操作数 %117 可以替换为常量（i32 54）
        Function: main, Block: Block115
            -> 操作数 %119 可以替换为常量（i32 55）
        Function: main, Block: Block117
            -> 操作数 %121 可以替换为常量（i32 56）
        Function: main, Block: Block119
            -> 操作数 %123 可以替换为常量（i32 57）
        Function: main, Block: Block121
            -> 操作数 %125 可以替换为常量（i32 58）
        Function: main, Block: Block123
            -> 操作数 %127 可以替换为常量（i32 59）
        Function: main, Block: Block125
            -> 操作数 %129 可以替换为常量（i32 60）
        Function: main, Block: Block127
            -> 操作数 %131 可以替换为常量（i32 61）
        Function: main, Block: Block129
            -> 操作数 %133 可以替换为常量（i32 62）
        Function: main, Block: Block131
            -> 操作数 %135 可以替换为常量（i32 63）
        Function: main, Block: Block133
            -> 操作数 %137 可以替换为常量（i32 64）
        Function: main, Block: Block135
            -> 操作数 %139 可以替换为常量（i32 65）
        Function: main, Block: Block137
            -> 操作数 %141 可以替换为常量（i32 66）
        Function: main, Block: Block139
            -> 操作数 %143 可以替换为常量（i32 67）
        Function: main, Block: Block141
            -> 操作数 %145 可以替换为常量（i32 68）
        Function: main, Block: Block143
            -> 操作数 %147 可以替换为常量（i32 69）
        Function: main, Block: Block145
            -> 操作数 %149 可以替换为常量（i32 70）
        Function: main, Block: Block147
            -> 操作数 %151 可以替换为常量（i32 71）
        Function: main, Block: Block149
            -> 操作数 %153 可以替换为常量（i32 72）
        Function: main, Block: Block151
            -> 操作数 %155 可以替换为常量（i32 73）
        Function: main, Block: Block153
            -> 操作数 %157 可以替换为常量（i32 74）
        Function: main, Block: Block155
            -> 操作数 %159 可以替换为常量（i32 75）
        Function: main, Block: Block157
            -> 操作数 %161 可以替换为常量（i32 76）
        Function: main, Block: Block159
            -> 操作数 %163 可以替换为常量（i32 77）
        Function: main, Block: Block161
            -> 操作数 %165 可以替换为常量（i32 78）
        Function: main, Block: Block163
            -> 操作数 %167 可以替换为常量（i32 79）
        Function: main, Block: Block165
            -> 操作数 %169 可以替换为常量（i32 80）
        Function: main, Block: Block167
            -> 操作数 %171 可以替换为常量（i32 81）
        Function: main, Block: Block169
            -> 操作数 %173 可以替换为常量（i32 82）
        Function: main, Block: Block171
            -> 操作数 %175 可以替换为常量（i32 83）
        Function: main, Block: Block173
            -> 操作数 %177 可以替换为常量（i32 84）
        Function: main, Block: Block175
            -> 操作数 %179 可以替换为常量（i32 85）
        Function: main, Block: Block177
            -> 操作数 %181 可以替换为常量（i32 86）
        Function: main, Block: Block179
            -> 操作数 %183 可以替换为常量（i32 87）
        Function: main, Block: Block181
            -> 操作数 %185 可以替换为常量（i32 88）
        Function: main, Block: Block183
            -> 操作数 %187 可以替换为常量（i32 89）
        Function: main, Block: Block185
            -> 操作数 %189 可以替换为常量（i32 90）
        Function: main, Block: Block187
            -> 操作数 %191 可以替换为常量（i32 91）
        Function: main, Block: Block189
            -> 操作数 %193 可以替换为常量（i32 92）
        Function: main, Block: Block191
            -> 操作数 %195 可以替换为常量（i32 93）
        Function: main, Block: Block193
            -> 操作数 %197 可以替换为常量（i32 94）
        Function: main, Block: Block195
            -> 操作数 %199 可以替换为常量（i32 95）
        Function: main, Block: Block197
            -> 操作数 %201 可以替换为常量（i32 96）
        Function: main, Block: Block199
            -> 操作数 %203 可以替换为常量（i32 97）
        Function: main, Block: Block201
            -> 操作数 %205 可以替换为常量（i32 98）
        Function: main, Block: Block203
            -> 操作数 %207 可以替换为常量（i32 99）
    foldable_instruction:
        Function: main, Block: Block7
            -> %reg_32 = 指令结果为常量（i32 1）但未被折叠
        Function: main, Block: Block9
            -> %reg_40 = 指令结果为常量（i32 2）但未被折叠
        Function: main, Block: Block11
            -> %reg_48 = 指令结果为常量（i32 3）但未被折叠
        Function: main, Block: Block13
            -> %reg_56 = 指令结果为常量（i32 4）但未被折叠
        Function: main, Block: Block15
            -> %reg_64 = 指令结果为常量（i32 5）但未被折叠
        Function: main, Block: Block17
            -> %reg_72 = 指令结果为常量（i32 6）但未被折叠
        Function: main, Block: Block19
            -> %reg_80 = 指令结果为常量（i32 7）但未被折叠
        Function: main, Block: Block21
            -> %reg_88 = 指令结果为常量（i32 8）但未被折叠
        Function: main, Block: Block23
            -> %reg_96 = 指令结果为常量（i32 9）但未被折叠
        Function: main, Block: Block25
            -> %reg_104 = 指令结果为常量（i32 10）但未被折叠
        Function: main, Block: Block27
            -> %reg_112 = 指令结果为常量（i32 11）但未被折叠
        Function: main, Block: Block29
            -> %reg_120 = 指令结果为常量（i32 12）但未被折叠
        Function: main, Block: Block31
            -> %reg_128 = 指令结果为常量（i32 13）但未被折叠
        Function: main, Block: Block33
            -> %reg_136 = 指令结果为常量（i32 14）但未被折叠
        Function: main, Block: Block35
            -> %reg_144 = 指令结果为常量（i32 15）但未被折叠
        Function: main, Block: Block37
            -> %reg_152 = 指令结果为常量（i32 16）但未被折叠
        Function: main, Block: Block39
            -> %reg_160 = 指令结果为常量（i32 17）但未被折叠
        Function: main, Block: Block41
            -> %reg_168 = 指令结果为常量（i32 18）但未被折叠
        Function: main, Block: Block43
            -> %reg_176 = 指令结果为常量（i32 19）但未被折叠
        Function: main, Block: Block45
            -> %reg_184 = 指令结果为常量（i32 20）但未被折叠
        Function: main, Block: Block47
            -> %reg_192 = 指令结果为常量（i32 21）但未被折叠
        Function: main, Block: Block49
            -> %reg_200 = 指令结果为常量（i32 22）但未被折叠
        Function: main, Block: Block51
            -> %reg_208 = 指令结果为常量（i32 23）但未被折叠
        Function: main, Block: Block53
            -> %reg_216 = 指令结果为常量（i32 24）但未被折叠
        Function: main, Block: Block55
            -> %reg_224 = 指令结果为常量（i32 25）但未被折叠
        Function: main, Block: Block57
            -> %reg_232 = 指令结果为常量（i32 26）但未被折叠
        Function: main, Block: Block59
            -> %reg_240 = 指令结果为常量（i32 27）但未被折叠
        Function: main, Block: Block61
            -> %reg_248 = 指令结果为常量（i32 28）但未被折叠
        Function: main, Block: Block63
            -> %reg_256 = 指令结果为常量（i32 29）但未被折叠
        Function: main, Block: Block65
            -> %reg_264 = 指令结果为常量（i32 30）但未被折叠
        Function: main, Block: Block67
            -> %reg_272 = 指令结果为常量（i32 31）但未被折叠
        Function: main, Block: Block69
            -> %reg_280 = 指令结果为常量（i32 32）但未被折叠
        Function: main, Block: Block71
            -> %reg_288 = 指令结果为常量（i32 33）但未被折叠
        Function: main, Block: Block73
            -> %reg_296 = 指令结果为常量（i32 34）但未被折叠
        Function: main, Block: Block75
            -> %reg_304 = 指令结果为常量（i32 35）但未被折叠
        Function: main, Block: Block77
            -> %reg_312 = 指令结果为常量（i32 36）但未被折叠
        Function: main, Block: Block79
            -> %reg_320 = 指令结果为常量（i32 37）但未被折叠
        Function: main, Block: Block81
            -> %reg_328 = 指令结果为常量（i32 38）但未被折叠
        Function: main, Block: Block83
            -> %reg_336 = 指令结果为常量（i32 39）但未被折叠
        Function: main, Block: Block85
            -> %reg_344 = 指令结果为常量（i32 40）但未被折叠
        Function: main, Block: Block87
            -> %reg_352 = 指令结果为常量（i32 41）但未被折叠
        Function: main, Block: Block89
            -> %reg_360 = 指令结果为常量（i32 42）但未被折叠
        Function: main, Block: Block91
            -> %reg_368 = 指令结果为常量（i32 43）但未被折叠
        Function: main, Block: Block93
            -> %reg_376 = 指令结果为常量（i32 44）但未被折叠
        Function: main, Block: Block95
            -> %reg_384 = 指令结果为常量（i32 45）但未被折叠
        Function: main, Block: Block97
            -> %reg_392 = 指令结果为常量（i32 46）但未被折叠
        Function: main, Block: Block99
            -> %reg_400 = 指令结果为常量（i32 47）但未被折叠
        Function: main, Block: Block101
            -> %reg_408 = 指令结果为常量（i32 48）但未被折叠
        Function: main, Block: Block103
            -> %reg_416 = 指令结果为常量（i32 49）但未被折叠
        Function: main, Block: Block105
            -> %reg_424 = 指令结果为常量（i32 50）但未被折叠
        Function: main, Block: Block107
            -> %reg_432 = 指令结果为常量（i32 51）但未被折叠
        Function: main, Block: Block109
            -> %reg_440 = 指令结果为常量（i32 52）但未被折叠
        Function: main, Block: Block111
            -> %reg_448 = 指令结果为常量（i32 53）但未被折叠
        Function: main, Block: Block113
            -> %reg_456 = 指令结果为常量（i32 54）但未被折叠
        Function: main, Block: Block115
            -> %reg_464 = 指令结果为常量（i32 55）但未被折叠
        Function: main, Block: Block117
            -> %reg_472 = 指令结果为常量（i32 56）但未被折叠
        Function: main, Block: Block119
            -> %reg_480 = 指令结果为常量（i32 57）但未被折叠
        Function: main, Block: Block121
            -> %reg_488 = 指令结果为常量（i32 58）但未被折叠
        Function: main, Block: Block123
            -> %reg_496 = 指令结果为常量（i32 59）但未被折叠
        Function: main, Block: Block125
            -> %reg_504 = 指令结果为常量（i32 60）但未被折叠
        Function: main, Block: Block127
            -> %reg_512 = 指令结果为常量（i32 61）但未被折叠
        Function: main, Block: Block129
            -> %reg_520 = 指令结果为常量（i32 62）但未被折叠
        Function: main, Block: Block131
            -> %reg_528 = 指令结果为常量（i32 63）但未被折叠
        Function: main, Block: Block133
            -> %reg_536 = 指令结果为常量（i32 64）但未被折叠
        Function: main, Block: Block135
            -> %reg_544 = 指令结果为常量（i32 65）但未被折叠
        Function: main, Block: Block137
            -> %reg_552 = 指令结果为常量（i32 66）但未被折叠
        Function: main, Block: Block139
            -> %reg_560 = 指令结果为常量（i32 67）但未被折叠
        Function: main, Block: Block141
            -> %reg_568 = 指令结果为常量（i32 68）但未被折叠
        Function: main, Block: Block143
            -> %reg_576 = 指令结果为常量（i32 69）但未被折叠
        Function: main, Block: Block145
            -> %reg_584 = 指令结果为常量（i32 70）但未被折叠
        Function: main, Block: Block147
            -> %reg_592 = 指令结果为常量（i32 71）但未被折叠
        Function: main, Block: Block149
            -> %reg_600 = 指令结果为常量（i32 72）但未被折叠
        Function: main, Block: Block151
            -> %reg_608 = 指令结果为常量（i32 73）但未被折叠
        Function: main, Block: Block153
            -> %reg_616 = 指令结果为常量（i32 74）但未被折叠
        Function: main, Block: Block155
            -> %reg_624 = 指令结果为常量（i32 75）但未被折叠
        Function: main, Block: Block157
            -> %reg_632 = 指令结果为常量（i32 76）但未被折叠
        Function: main, Block: Block159
            -> %reg_640 = 指令结果为常量（i32 77）但未被折叠
        Function: main, Block: Block161
            -> %reg_648 = 指令结果为常量（i32 78）但未被折叠
        Function: main, Block: Block163
            -> %reg_656 = 指令结果为常量（i32 79）但未被折叠
        Function: main, Block: Block165
            -> %reg_664 = 指令结果为常量（i32 80）但未被折叠
        Function: main, Block: Block167
            -> %reg_672 = 指令结果为常量（i32 81）但未被折叠
        Function: main, Block: Block169
            -> %reg_680 = 指令结果为常量（i32 82）但未被折叠
        Function: main, Block: Block171
            -> %reg_688 = 指令结果为常量（i32 83）但未被折叠
        Function: main, Block: Block173
            -> %reg_696 = 指令结果为常量（i32 84）但未被折叠
        Function: main, Block: Block175
            -> %reg_704 = 指令结果为常量（i32 85）但未被折叠
        Function: main, Block: Block177
            -> %reg_712 = 指令结果为常量（i32 86）但未被折叠
        Function: main, Block: Block179
            -> %reg_720 = 指令结果为常量（i32 87）但未被折叠
        Function: main, Block: Block181
            -> %reg_728 = 指令结果为常量（i32 88）但未被折叠
        Function: main, Block: Block183
            -> %reg_736 = 指令结果为常量（i32 89）但未被折叠
        Function: main, Block: Block185
            -> %reg_744 = 指令结果为常量（i32 90）但未被折叠
        Function: main, Block: Block187
            -> %reg_752 = 指令结果为常量（i32 91）但未被折叠
        Function: main, Block: Block189
            -> %reg_760 = 指令结果为常量（i32 92）但未被折叠
        Function: main, Block: Block191
            -> %reg_768 = 指令结果为常量（i32 93）但未被折叠
        Function: main, Block: Block193
            -> %reg_776 = 指令结果为常量（i32 94）但未被折叠
        Function: main, Block: Block195
            -> %reg_784 = 指令结果为常量（i32 95）但未被折叠
        Function: main, Block: Block197
            -> %reg_792 = 指令结果为常量（i32 96）但未被折叠
        Function: main, Block: Block199
            -> %reg_800 = 指令结果为常量（i32 97）但未被折叠
        Function: main, Block: Block201
            -> %reg_808 = 指令结果为常量（i32 98）但未被折叠
        Function: main, Block: Block203
            -> %reg_816 = 指令结果为常量（i32 99）但未被折叠
