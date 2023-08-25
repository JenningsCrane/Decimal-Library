#include "decimal.h"

int getsign(const decimal* src, int i) {
  uint32_t mask = 1u << (i % BIT_LEN);
  return !!(src->bits[i / BIT_LEN] & mask);
}

void setsign(decimal* src, int i, int sign) {
  if (sign) {
    src->bits[i / BIT_LEN] |= 1u << i % BIT_LEN;
  } else {
    src->bits[i / BIT_LEN] &= ~(1u << i % BIT_LEN);
  }
}

decimal_info* get_info(decimal* src) {
  return (decimal_info*)(src->bits + INFO);
}

uint32_t ftou(float num) { return *(uint32_t*)&num; }

int first_sign(decimal* src) {
  int idx = -1;
  for (int i = 0; i < MAX_LEN; ++i) {
    if (getsign(src, i)) idx = i;
  }
  return idx;
}

int last_sign(decimal* src) {
  int idx = -1;
  for (int i = MAX_LEN - 1; i >= 0 && idx == -1; --i) {
    if (getsign(src, i)) idx = i;
  }
  return idx;
}

bool is_zero(decimal* src) {
  return src->bits[LOW] == 0 && src->bits[MID] == 0 && src->bits[HIGH] == 0;
}

bool is_max(decimal* src) {
  return src->bits[LOW] == UINT32_MAX && src->bits[MID] == UINT32_MAX &&
         src->bits[HIGH] == UINT32_MAX;
}

bool can_10_shift(const decimal* src) {
  return !(getsign(src, MAX_LEN - 1) || getsign(src, MAX_LEN - 2) ||
           getsign(src, MAX_LEN - 3) || getsign(src, MAX_LEN - 4));
}

void shift_right(decimal* src, int shift) {
  for (int i = shift; i < MAX_LEN; ++i) {
    setsign(src, i - shift, getsign(src, i));
  }
}

int shift_left(decimal* src, int shift) {
  int err = RES_OK;
  int sign_idx = last_sign(src);

  if (sign_idx != -1 && sign_idx > MAX_LEN - shift - 1) {
    err = RES_OVERFLOW;
  }

  if (!err) {
    for (int i = MAX_LEN - shift - 1; i >= 0; --i) {
      setsign(src, i + shift, getsign(src, i));
    }
    for (int i = 0; i < shift; ++i) {
      setsign(src, i, 0);
    }
  }

  return err;
}

int shift_10_left(decimal* src, int shift) {
  int err = RES_OK;
  for (int i = 0; i < shift && !err; ++i) {
    decimal a = *src;
    decimal b = *src;

    err = shift_left(&a, 3);
    if (err) continue;
    shift_left(&b, 1);

    err = sum(&a, &b, src);
  }
  if (err) err = RES_OVERFLOW;
  return err;
}

decimal shift_10_right(decimal* src, int shift) {
  decimal ten = {.bits = {10, 0, 0, 0}};

  decimal mod = ZERO_DECIMAL;

  for (int i = 0; i < shift; ++i) {
    mod = div_int(*src, ten, src);
  }

  return mod;
}

void reverse(decimal* src) {
  for (int i = 0; i < RANK_COUNT; ++i) {
    src->bits[i] = ~(src->bits[i]);
  }
}

void set_zero(decimal* src) {
  for (int i = 0; i < BIT_COUNT; ++i) src->bits[i] = 0;
}

void set_one(decimal* src) {
  src->bits[LOW] = 1;
  src->bits[MID] = 0;
  src->bits[HIGH] = 0;
  src->bits[INFO] = 0;
}

void set_max(decimal* src) {
  for (int i = 0; i < BIT_COUNT; ++i) src->bits[i] = UINT32_MAX;
  src->bits[INFO] = 0;
}
