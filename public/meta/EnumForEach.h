#pragma once

// 纵向迭代专用

#define ENUM_FOR_EACH_0(x)
#define ENUM_FOR_EACH_1(x) ENUM_FOR_EACH_0(x) x(1)
#define ENUM_FOR_EACH_2(x) ENUM_FOR_EACH_1(x) x(2)
#define ENUM_FOR_EACH_3(x) ENUM_FOR_EACH_2(x) x(3)
#define ENUM_FOR_EACH_4(x) ENUM_FOR_EACH_3(x) x(4)
#define ENUM_FOR_EACH_5(x) ENUM_FOR_EACH_4(x) x(5)
#define ENUM_FOR_EACH_6(x) ENUM_FOR_EACH_5(x) x(6)
#define ENUM_FOR_EACH_7(x) ENUM_FOR_EACH_6(x) x(7)
#define ENUM_FOR_EACH_8(x) ENUM_FOR_EACH_7(x) x(8)
#define ENUM_FOR_EACH_9(x) ENUM_FOR_EACH_8(x) x(9)
#define ENUM_FOR_EACH_10(x) ENUM_FOR_EACH_9(x) x(10)
#define ENUM_FOR_EACH_11(x) ENUM_FOR_EACH_10(x) x(11)
#define ENUM_FOR_EACH_12(x) ENUM_FOR_EACH_11(x) x(12)
#define ENUM_FOR_EACH_13(x) ENUM_FOR_EACH_12(x) x(13)
#define ENUM_FOR_EACH_14(x) ENUM_FOR_EACH_13(x) x(14)
#define ENUM_FOR_EACH_15(x) ENUM_FOR_EACH_14(x) x(15)
#define ENUM_FOR_EACH_16(x) ENUM_FOR_EACH_15(x) x(16)
#define ENUM_FOR_EACH_17(x) ENUM_FOR_EACH_16(x) x(17)
#define ENUM_FOR_EACH_18(x) ENUM_FOR_EACH_17(x) x(18)
#define ENUM_FOR_EACH_19(x) ENUM_FOR_EACH_18(x) x(19)
#define ENUM_FOR_EACH_20(x) ENUM_FOR_EACH_19(x) x(20)
#define ENUM_FOR_EACH_21(x) ENUM_FOR_EACH_20(x) x(21)
#define ENUM_FOR_EACH_22(x) ENUM_FOR_EACH_21(x) x(22)
#define ENUM_FOR_EACH_23(x) ENUM_FOR_EACH_22(x) x(23)
#define ENUM_FOR_EACH_24(x) ENUM_FOR_EACH_23(x) x(24)
#define ENUM_FOR_EACH_25(x) ENUM_FOR_EACH_24(x) x(25)
#define ENUM_FOR_EACH_26(x) ENUM_FOR_EACH_25(x) x(26)
#define ENUM_FOR_EACH_27(x) ENUM_FOR_EACH_26(x) x(27)
#define ENUM_FOR_EACH_28(x) ENUM_FOR_EACH_27(x) x(28)
#define ENUM_FOR_EACH_29(x) ENUM_FOR_EACH_28(x) x(29)
#define ENUM_FOR_EACH_30(x) ENUM_FOR_EACH_29(x) x(30)
#define ENUM_FOR_EACH_31(x) ENUM_FOR_EACH_30(x) x(31)
#define ENUM_FOR_EACH_32(x) ENUM_FOR_EACH_31(x) x(32)


#define ENUM_FOR_EACH(x, N) ENUM_FOR_EACH_##N(x)