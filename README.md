# NKUER4
2022年全国大学生计算机系统能力培养大赛编译系统设计赛项目  
团队学校：南开大学  
团队名称：NKUER4  
团队成员：时浩铭 严诗慧 林坤 梅骏逸  
指导教师：王刚 李忠伟

## 项目简介
一个单前端单后端的编译器，对SysY语言进行编译，生成arm架构汇编。  
前端、中端和后端三部分。前端基于lex和yacc实现，中间代码与llvm IR兼容，后端生成arm汇编代码。  
实现了一些优化算法。

## 项目构建
在项目根目录执行
```
make app
```
## 项目运行
```
Usage: build/compiler [options] file
Options: 
    -a              Generate ast.
    -i              Generate IR.
    -o              Specify output file.
    -O              Generate optimized code.
    -S              Generate Arm assemble code.
```

## 项目特性
### 中间代码优化  
* 全局变量局部化
* Mem2Reg
* 函数内联
* 常量传播
* 稀有条件常量传播
* 公共子表达式删除
* 死代码删除
* 全局值编号
* 循环优化
* 指令调度
* 控制流优化

### 目标代码优化

* 死代码删除
* 窥孔优化
* 局部值编号
* 控制流优化
* 乘除法优化
* 寄存器分配

详细介绍见[文档](./NKUER4_final.pdf)

## 项目演示
对于SysY代码：
``` c
// test if-else
int ifElse() {
  int a;
  a = 5;
  if (a == 5) {
    a = 25;
  } else {
    a = a * 2;
  }
  return (a);
}


int main() {
  return (ifElse());
}
```

编译器生成的语法树如下：
```
program
    FunctionDefine	function name: @ifElse	type: i32()
        CompoundStmt
            DeclStmt
                Id	name: a	scope: 2	type: i32
            AssignStmt
                Id	name: a	scope: 2	type: i32
                IntegerLiteral	value: 5	type: i32
            IfElseStmt
                BinaryExpr	op: equal	type: i1
                    Id	name: a	scope: 2	type: i32
                    IntegerLiteral	value: 5	type: i32
                CompoundStmt
                    AssignStmt
                        Id	name: a	scope: 2	type: i32
                        IntegerLiteral	value: 25	type: i32
                CompoundStmt
                    AssignStmt
                        Id	name: a	scope: 2	type: i32
                        BinaryExpr	op: mul	type: i32
                            Id	name: a	scope: 2	type: i32
                            IntegerLiteral	value: 2	type: i32
            ReturnStmt
                Id	name: a	scope: 2	type: i32
    FunctionDefine	function name: @main	type: i32()
        CompoundStmt
            ReturnStmt
                CallExpr	function name: @ifElse	scope: 0	type: i32()

```
生成的中间代码：
``` llvm
define i32 @ifElse() {
B10:
  %t11 = alloca i32, align 4
  store i32 5, i32* %t11, align 4
  %t2 = load i32, i32* %t11, align 4
  %t3 = icmp eq i32 %t2, 5
  br i1 %t3, label %B12, label %B17
B12:                               	; preds = %B10
  store i32 25, i32* %t11, align 4
  br label %B14
B14:                               	; preds = %B12, %B13
  %t8 = load i32, i32* %t11, align 4
  ret i32 %t8
B17:                               	; preds = %B10
  br label %B13
B13:                               	; preds = %B17
  %t6 = load i32, i32* %t11, align 4
  %t7 = mul i32 %t6, 2
  store i32 %t7, i32* %t11, align 4
  br label %B14
}
define i32 @main() {
B18:
  %t9 = call i32 @ifElse()
  ret i32 %t9
}

```

生成的未经优化的汇编代码：
``` arm
	.cpu cortex-a72
	.arch armv8-a
	.fpu vfpv3-d16
	.arch_extension crc
	.text
	.global ifElse
	.type ifElse , %function
ifElse:
	push {r3, r4, r5, r6, fp, lr}
	mov fp, sp
	sub sp, sp, #8
.L10:
	ldr r4, =5
	str r4, [fp, #-4]
	ldr r4, [fp, #-4]
	cmp r4, #5
	moveq r4, #1
	movne r4, #0
	beq .L12
	b .L17
.L12:
	ldr r4, =25
	str r4, [fp, #-4]
	b .L14
.L14:
	ldr r4, [fp, #-4]
	mov r0, r4
	add sp, sp, #8
	pop {r3, r4, r5, r6, fp, lr}
	bx lr
.L17:
	b .L13
.L13:
	ldr r4, [fp, #-4]
	ldr r5, =2
	mul r6, r4, r5
	str r6, [fp, #-4]
	b .L14

	.global main
	.type main , %function
main:
	push {r3, r4, fp, lr}
	mov fp, sp
.L18:
	bl ifElse
	mov r4, r0
	mov r0, r4
	pop {r3, r4, fp, lr}
	bx lr

	.ident "NKUER4"
```

经过优化后生成：
``` arm
	.cpu cortex-a72
	.arch armv8-a
	.fpu vfpv3-d16
	.arch_extension crc
	.text
	.global main
	.type main , %function
main:
	push {fp, lr}
	mov fp, sp
.L18:
	mov r0, #25
	pop {fp, lr}
	bx lr

	.ident "NKUER4"

```

## 项目框架
基本框架源自[Emanual20/2021NKUCS-Compilers-Lab: For 2021Fall NKUCS, the course of the principle of compilers. (github.com)](https://github.com/Emanual20/2021NKUCS-Compilers-Lab)