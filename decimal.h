#ifndef DECIMAL_H_
#define DECIMAL_H_

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define LOW 0
#define MID 1
#define HIGH 2
#define INFO 3

#define SIGN_PLUS 0
#define SIGN_MINUS 1

#define LAST_BIT 95
#define MAX_LEN 96
#define BIT_LEN 32

#define RES_OK 0
#define RES_ERR 1
#define RES_INF_POSITIVE 1
#define RES_INF_NEGATIVE 2
#define RES_ZERO_DIVISION 3
#define RES_OVERFLOW 4

#define MAX_EXPO 28

#define BIT_COUNT 4
#define RANK_COUNT 3

typedef struct {
  uint32_t _1 : 16;
  uint32_t expo : 8;
  uint32_t _2 : 7;
  uint32_t sign : 1;
} decimal_info;

typedef struct {
  uint32_t bits[BIT_COUNT];
} decimal;

#define ZERO_DECIMAL    \
  (decimal) {       \
    .bits = { 0, 0, 0 } \
  }
#define MAX_DECIMAL                                   \
  (decimal) {                                     \
    .bits = { UINT32_MAX, UINT32_MAX, UINT32_MAX, 0 } \
  }

// ----- Arithmetic -----
int sum(decimal* val1, decimal* val2, decimal* res);

int add(decimal value_1, decimal value_2, decimal* result);

int sub(decimal value_1, decimal value_2, decimal* result);

int mul(decimal value_1, decimal value_2, decimal* result);

int div(decimal value_1, decimal value_2, decimal* result);

int mod(decimal value_1, decimal value_2, decimal* result);

decimal div_int(decimal val1, decimal val2,
                        decimal* result);

void reverse_to_pos(decimal* src);
void reverse_to_neg(decimal* src);
int equalize_expo(decimal* val1, decimal* val2, bool buisness_round);
// ----------------------

// ----- Compare -----
int is_greater_int(decimal* val1, decimal* val2);

int is_lower_int(decimal* val1, decimal* val2);

int is_equals_int(decimal* val1, decimal* val2);

int is_less(decimal val1, decimal val2);

int is_less_or_equal(decimal val1, decimal val2);

int is_greater(decimal val1, decimal val2);

int is_greater_or_equal(decimal val1, decimal val2);

int is_equal(decimal val1, decimal val2);

int is_not_equal(decimal val1, decimal val2);

// -------------------

// ----- Convert -----
int from_int_to_decimal(int src, decimal* dst);

int from_decimal_to_int(decimal src, int* dst);

int from_float_to_decimal(float src, decimal* dst);

int from_decimal_to_float(decimal src, float* dst);

int read_exp(float src);
// -------------------

// ----- Other -----
int truncate(decimal value, decimal* result);

int negate(decimal value, decimal* result);

int floor(decimal value, decimal* result);

int round(decimal value, decimal* result);
// -----------------

// ----- Utils -----
/**
 * Возвращает знак указанного бита
 * @param src Указатель на decimal
 * @param i Номер бита
 * @returns Знак бита из src.bits под номером i
 */
int getsign(const decimal* src, int i);

/**
 * Присваиет знак указанному биту
 * @param src Указатель на decimal
 * @param i Номер бита
 * @param sign Знак
 */
void setsign(decimal* src, int i, int sign);

decimal_info* get_info(decimal* src);

uint32_t ftou(float num);

bool is_zero(decimal* src);
bool is_max(decimal* src);

int first_sign(decimal* src);
int last_sign(decimal* src);

void shift_right(decimal* src, int shift);
int shift_left(decimal* src, int shift);
int shift_10_left(decimal* src, int shift);
decimal shift_10_right(decimal* src, int shift);

bool can_10_shift(const decimal* src);

void reverse(decimal* src);

void set_zero(decimal* src);
void set_one(decimal* src);
void set_max(decimal* src);

void print_binary(uint32_t num);
void print_binary_dec(decimal* dec);
// -----------------

#endif  // DECIMAL_H_
