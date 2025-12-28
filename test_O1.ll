; Function Declarations
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(ptr)
declare float @getfloat()
declare i32 @getfarray(ptr)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, ptr)
declare void @putfloat(float)
declare void @putfarray(i32, ptr)
declare void @_sysy_starttime(i32)
declare void @_sysy_stoptime(i32)
declare void @llvm.memset.p0.i32(ptr, i8, i32, i1)

; Global Variable Declarations


; Function Definitions
define i32 @main()
{
Block0:
	%reg_2 = call i32 @getint()
	%reg_6 = mul i32 %reg_2, 1
	%reg_10 = mul i32 %reg_2, 2
	%reg_14 = mul i32 %reg_2, 3
	%reg_18 = mul i32 %reg_2, 4
	%reg_22 = mul i32 %reg_2, 5
	%reg_26 = mul i32 %reg_2, 6
	%reg_30 = mul i32 %reg_2, 7
	%reg_34 = mul i32 %reg_2, 8
	%reg_38 = add i32 %reg_2, %reg_2
	%reg_42 = sdiv i32 %reg_2, 5
	%reg_46 = mul i32 %reg_2, 11
	%reg_50 = add i32 %reg_42, %reg_10
	%reg_54 = add i32 %reg_18, %reg_22
	%reg_58 = add i32 %reg_14, %reg_18
	%reg_60 = add i32 %reg_58, %reg_26
	%reg_62 = add i32 %reg_60, %reg_2
	%reg_66 = add i32 %reg_10, %reg_18
	%reg_68 = add i32 %reg_66, %reg_26
	%reg_70 = add i32 %reg_68, %reg_42
	%reg_74 = sdiv i32 %reg_2, 5
	%reg_78 = mul i32 %reg_2, 11
	%reg_82 = add i32 %reg_42, %reg_10
	%reg_86 = add i32 %reg_18, %reg_22
	%reg_90 = add i32 %reg_14, %reg_18
	%reg_92 = add i32 %reg_90, %reg_26
	%reg_94 = add i32 %reg_92, %reg_2
	%reg_96 = add i32 %reg_94, 13
	%reg_100 = add i32 %reg_10, %reg_18
	%reg_102 = add i32 %reg_100, %reg_26
	%reg_104 = add i32 %reg_102, %reg_42
	%reg_106 = add i32 %reg_104, 5
	call void @putint(i32 %reg_6)
	call void @putch(i32 10)
	call void @putint(i32 %reg_14)
	call void @putch(i32 10)
	call void @putint(i32 %reg_22)
	call void @putch(i32 10)
	br label %Block1
Block1:
	%reg_337 = phi i32 [ %reg_277, %Block8 ], [ 0, %Block0 ]
	%reg_336 = phi i32 [ %reg_335, %Block8 ], [ 0, %Block0 ]
	%reg_333 = phi i32 [ %reg_332, %Block8 ], [ 5, %Block0 ]
	%reg_330 = phi i32 [ %reg_329, %Block8 ], [ 11, %Block0 ]
	%reg_328 = phi i32 [ %reg_327, %Block8 ], [ 14, %Block0 ]
	%reg_326 = phi i32 [ %reg_325, %Block8 ], [ 15, %Block0 ]
	%reg_324 = phi i32 [ %reg_323, %Block8 ], [ 19, %Block0 ]
	%reg_322 = phi i32 [ %reg_321, %Block8 ], [ 23, %Block0 ]
	%reg_320 = phi i32 [ %reg_319, %Block8 ], [ 33, %Block0 ]
	%reg_318 = phi i32 [ %reg_317, %Block8 ], [ 123, %Block0 ]
	%reg_316 = phi i32 [ %reg_315, %Block8 ], [ 134, %Block0 ]
	%reg_314 = phi i32 [ %reg_313, %Block8 ], [ 0, %Block0 ]
	%reg_312 = phi i32 [ %reg_311, %Block8 ], [ 0, %Block0 ]
	%reg_310 = phi i32 [ %reg_309, %Block8 ], [ 0, %Block0 ]
	%reg_145 = icmp slt i32 %reg_337, 100000
	br i1 %reg_145, label %Block2, label %Block3
Block2:
	%reg_148 = srem i32 %reg_337, 9961
	%reg_150 = icmp eq i32 %reg_148, 0
	br i1 %reg_150, label %Block4, label %Block5
Block3:
	call void @putint(i32 %reg_336)
	call void @putch(i32 32)
	call void @putint(i32 %reg_333)
	call void @putch(i32 32)
	call void @putint(i32 %reg_330)
	call void @putch(i32 32)
	call void @putint(i32 %reg_328)
	call void @putch(i32 32)
	call void @putint(i32 %reg_326)
	call void @putch(i32 32)
	call void @putint(i32 %reg_324)
	call void @putch(i32 32)
	call void @putint(i32 %reg_322)
	call void @putch(i32 32)
	call void @putint(i32 %reg_320)
	call void @putch(i32 32)
	call void @putint(i32 %reg_318)
	call void @putch(i32 32)
	call void @putint(i32 %reg_316)
	call void @putch(i32 32)
	call void @putint(i32 %reg_314)
	call void @putch(i32 32)
	call void @putint(i32 %reg_312)
	call void @putch(i32 32)
	call void @putint(i32 %reg_310)
	call void @putch(i32 10)
	ret i32 0
Block4:
	%reg_153 = add i32 %reg_336, %reg_6
	%reg_158 = add i32 %reg_333, %reg_10
	%reg_163 = add i32 %reg_330, %reg_18
	%reg_168 = add i32 %reg_328, %reg_26
	%reg_173 = add i32 %reg_326, %reg_30
	%reg_178 = add i32 %reg_324, %reg_34
	%reg_183 = add i32 %reg_322, %reg_38
	%reg_188 = add i32 %reg_320, %reg_42
	%reg_193 = add i32 %reg_318, %reg_46
	%reg_195 = add i32 %reg_193, %reg_50
	%reg_197 = add i32 %reg_195, %reg_54
	%reg_202 = add i32 %reg_316, %reg_62
	%reg_204 = add i32 %reg_202, %reg_70
	%reg_209 = add i32 %reg_197, %reg_78
	%reg_211 = add i32 %reg_209, %reg_82
	%reg_213 = add i32 %reg_211, %reg_86
	%reg_218 = add i32 %reg_204, %reg_74
	%reg_220 = add i32 %reg_218, %reg_96
	%reg_222 = add i32 %reg_220, %reg_106
	br label %Block5
Block5:
	%reg_334 = phi i32 [ %reg_336, %Block2 ], [ %reg_153, %Block4 ]
	%reg_331 = phi i32 [ %reg_333, %Block2 ], [ %reg_158, %Block4 ]
	%reg_329 = phi i32 [ %reg_330, %Block2 ], [ %reg_163, %Block4 ]
	%reg_327 = phi i32 [ %reg_328, %Block2 ], [ %reg_168, %Block4 ]
	%reg_325 = phi i32 [ %reg_326, %Block2 ], [ %reg_173, %Block4 ]
	%reg_323 = phi i32 [ %reg_324, %Block2 ], [ %reg_178, %Block4 ]
	%reg_321 = phi i32 [ %reg_322, %Block2 ], [ %reg_183, %Block4 ]
	%reg_319 = phi i32 [ %reg_320, %Block2 ], [ %reg_188, %Block4 ]
	%reg_317 = phi i32 [ %reg_318, %Block2 ], [ %reg_213, %Block4 ]
	%reg_315 = phi i32 [ %reg_316, %Block2 ], [ %reg_222, %Block4 ]
	br label %Block6
Block6:
	%reg_335 = phi i32 [ %reg_334, %Block5 ], [ %reg_241, %Block7 ]
	%reg_332 = phi i32 [ %reg_331, %Block5 ], [ %reg_267, %Block7 ]
	%reg_313 = phi i32 [ %reg_314, %Block5 ], [ %reg_248, %Block7 ]
	%reg_311 = phi i32 [ %reg_312, %Block5 ], [ %reg_255, %Block7 ]
	%reg_309 = phi i32 [ %reg_310, %Block5 ], [ %reg_262, %Block7 ]
	%reg_308 = phi i32 [ 0, %Block5 ], [ %reg_272, %Block7 ]
	%reg_229 = icmp slt i32 %reg_308, 5
	br i1 %reg_229, label %Block9, label %Block10
Block7:
	%reg_241 = add i32 %reg_335, %reg_308
	%reg_247 = mul i32 %reg_308, %reg_308
	%reg_248 = add i32 %reg_313, %reg_247
	%reg_254 = mul i32 2, %reg_308
	%reg_255 = add i32 %reg_311, %reg_254
	%reg_261 = mul i32 3, %reg_308
	%reg_262 = add i32 %reg_309, %reg_261
	%reg_267 = add i32 %reg_332, %reg_308
	%reg_272 = add i32 %reg_308, 1
	br label %Block6
Block8:
	%reg_277 = add i32 %reg_337, 1
	br label %Block1
Block9:
	%reg_233 = srem i32 %reg_337, 100
	%reg_235 = icmp eq i32 %reg_233, 1
	%reg_236 = zext i1 %reg_235 to i32
	br label %Block11
Block10:
	br label %Block11
Block11:
	%reg_338 = phi i32 [ 0, %Block10 ], [ %reg_236, %Block9 ]
	%reg_238 = icmp ne i32 %reg_338, 0
	br i1 %reg_238, label %Block7, label %Block8
}
