#pragma once

/* ====================== CTABLE base addresses ====================== */
/* Allows the compiler to emit LBCO/SBCO instructions instead of
 * LDI-LDI-LBBO/SBBO tripplets */
#pragma ctable_entry 0 0x00020000
#pragma ctable_entry 1 0x48040000
#pragma ctable_entry 2 0x4802a000
#pragma ctable_entry 3 0x00030000
#pragma ctable_entry 4 0x00026000
#pragma ctable_entry 5 0x48060000
#pragma ctable_entry 6 0x48030000
#pragma ctable_entry 7 0x00028000
#pragma ctable_entry 8 0x46000000
#pragma ctable_entry 9 0x4a100000
#pragma ctable_entry 10 0x48318000
#pragma ctable_entry 11 0x48022000
#pragma ctable_entry 12 0x48024000
#pragma ctable_entry 13 0x48318000
#pragma ctable_entry 14 0x481cc000
#pragma ctable_entry 15 0x481d0000
#pragma ctable_entry 16 0x481a0000
#pragma ctable_entry 17 0x4819c000
#pragma ctable_entry 18 0x48300000
#pragma ctable_entry 19 0x48302000
#pragma ctable_entry 20 0x48304000
#pragma ctable_entry 21 0x00032400
#pragma ctable_entry 22 0x480c8000
#pragma ctable_entry 23 0x480ca000
/* 24-31 are user-defined base addresses. */

#define CPU_FREQ (200 * 1000 * 1000)
