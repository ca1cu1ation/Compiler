; 全局变量定义
@a = dso_local constant i32 7
@b = dso_local global [2 x i32] [i32 1, i32 2]

; 计算1到n的和函数
define dso_local i32 @recursive_sum(i32 %n) {
entry:
  %cmp = icmp eq i32 %n, 0
  br i1 %cmp, label %if.then, label %if.else

if.then:
  br label %return

if.else:
  %sub = sub nsw i32 %n, 1
  %call = call i32 @recursive_sum(i32 %sub)
  %add = add nsw i32 %n, %call
  br label %return

return:
  %retval = phi i32 [ 0, %if.then ], [ %add, %if.else ]
  ret i32 %retval
}

; main函数
define dso_local i32 @main() {
entry:
  %n = alloca i32
  %i = alloca i32
  %res = alloca i32

  ; 调用getint函数读取整数
  %call = call i32 @getint()
  store i32 %call, i32* %n

  ; 计算 n % 2
  %n_val = load i32, i32* %n
  %mod = srem i32 %n_val, 2
  %cmp = icmp eq i32 %mod, 1
  br i1 %cmp, label %if.then, label %if.else

if.then:
  ; 计算 recursive_sum(n) - b[0]
  %call1 = call i32 @recursive_sum(i32 %n_val)
  %b0_addr = getelementptr inbounds [2 x i32], [2 x i32]* @b, i32 0, i32 0
  %b0_val = load i32, i32* %b0_addr
  %sub = sub nsw i32 %call1, %b0_val
  store i32 %sub, i32* %res

  ; 输出结果
  %res_val = load i32, i32* %res
  call void @putint(i32 %res_val)
  br label %if.end

if.else:
  ; i = 1
  store i32 1, i32* %i
  br label %while.cond

while.cond:
  %i_val = load i32, i32* %i
  %b1_addr = getelementptr inbounds [2 x i32], [2 x i32]* @b, i32 0, i32 1
  %b1_val = load i32, i32* %b1_addr
  %cmp2 = icmp slt i32 %i_val, %b1_val
  br i1 %cmp2, label %while.body, label %while.end

while.body:
  %i_val3 = load i32, i32* %i
  %add = add nsw i32 %i_val3, 1
  store i32 %add, i32* %i
  br label %while.cond

while.end:
  ; i = a * i
  %i_val4 = load i32, i32* %i
  %a_val = load i32, i32* @a
  %mul = mul nsw i32 %a_val, %i_val4
  store i32 %mul, i32* %i

  ; 计算 recursive_sum(n) / i
  %call5 = call i32 @recursive_sum(i32 %n_val)
  %i_val6 = load i32, i32* %i
  %div = sdiv i32 %call5, %i_val6
  store i32 %div, i32* %res

  ; 输出结果
  %res_val7 = load i32, i32* %res
  call void @putint(i32 %res_val7)
  br label %if.end

if.end:
  ret i32 0
}

; 声明外部函数
declare i32 @getint()
declare void @putint(i32)