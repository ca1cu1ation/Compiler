.PHONY: all, pre, lexer, ast-gcc, ast-llvm, cfg, ir-gcc, ir-llvm, asm, obj, exe, antiobj, antiexe, bc, opt, opt-mem2reg, opt-instcombine, view-opt, stats-opt, time-opt, clean, clean-all

all: pre lexer ast-gcc ast-llvm cfg ir-gcc ir-llvm asm obj antiobj exe antiexe clean

pre:
	gcc main.c -E -o main.i

lexer:
	clang -E -Xclang -dump-tokens main.c

#生成`main.c.003t.original`
ast-gcc:
	gcc -fdump-tree-original-raw main.c

#生成`main.ll`
ast-llvm:
	clang -E -Xclang -ast-dump main.c

#会生成多个阶段的文件(.dot)，可以被graphviz可视化，可以直接使用vscode插件
# (Graphviz (dot) language support for Visual Studio Code)。
#此时的可读性还很强。`main.c.011t.cfg.dot`
cfg:
	gcc -O0 -fdump-tree-all-graph main.c

#此时可读性不好，简要了解各阶段更迭过程即可。
ir-gcc:
	gcc -O0 -fdump-rtl-all-graph main.c

ir-llvm:
	clang -S -emit-llvm main.c

asm:
	gcc -O0 -o main.S -S -masm=att main.i

obj:
	gcc -O0 -c -o main.o main.S

antiobj:
	objdump -d main.o > main-anti-obj.S
	nm main.o > main-nm-obj.txt

exe:
	gcc -O0 -o main main.o

antiexe:
	objdump -d main > main-anti-exe.S
	nm main > main-nm-exe.txt

# --- LLVM Bitcode and Optimization Verification ---

# 生成 LLVM bitcode (.bc)
bc:
	clang -emit-llvm -c main.c -o main.bc

# 查看优化后的 IR。用法: make view-opt PASS=<pass_name>
view-opt: bc
	@echo "--- LLVM IR after [$(PASS)] pass ---"
	opt -passes=$(PASS) -S main.bc

# 统计优化信息。用法: make stats-opt PASS=<pass_name>
stats-opt: bc
	@echo "--- Statistics for [$(PASS)] pass ---"
	opt -passes=$(PASS) -stats main.bc -o /dev/null

# 比较优化前后的执行时间。用法: make time-opt PASS=<pass_name>
time-opt: bc
	@echo "--- Timing comparison for [$(PASS)] pass ---"
	# 编译未优化的版本
	clang main.bc -o main_no_opt
	# 编译优化后的版本
	opt -passes=$(PASS) main.bc -o main_opt.bc
	clang main_opt.bc -o main_opt
	# 运行并计时
	@echo "Running unoptimized version:"
	time ./main_no_opt
	@echo "\nRunning optimized version:"
	time ./main_opt
	@rm -f main_no_opt main_opt main_opt.bc

# 使用 opt 进行通用优化, 用法: make opt PASS=<pass-name>
# 例如: make opt PASS=mem2reg
opt: bc
	@echo "Running optimization pass: $(PASS)..."
	opt -passes=$(PASS) main.bc -o /dev/null

# --- 预设的典型优化示例 ---

# 内存到寄存器
opt-mem2reg: bc
	@echo "Running mem2reg pass..."
	opt -passes=mem2reg main.bc -o /dev/null

# 指令合并
opt-instcombine: bc
	@echo "Running instcombine pass..."
	opt -passes=instcombine main.bc -o /dev/null

clean:
	rm -rf *.c.* main.bc main.o main.i main.S main main-anti-obj.S main-nm-obj.txt main-anti-exe.S main-nm-exe.txt

clean-all:
	rm -rf *.c.* *.o *.S *.dot *.out *.txt *.ll *.i main