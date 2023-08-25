#include "decimal.h"

int equalize_expo(decimal* val1, decimal* val2, bool buisness_round) {
  int err = RES_OK;

  decimal_info* info1 = get_info(val1);
  decimal_info* info2 = get_info(val2);

  int expo1 = info1->expo;
  int expo2 = info2->expo;

  int min_expo = expo1 < expo2 ? expo1 : expo2;
  int max_expo = expo1 < expo2 ? expo2 : expo1;

  decimal* min_dec = min_expo == expo1 ? val1 : val2;
  decimal* max_dec = max_expo == expo1 ? val1 : val2;

  decimal_info* min_info = min_expo == expo1 ? info1 : info2;
  decimal_info* max_info = max_expo == expo1 ? info1 : info2;

  if (min_expo != max_expo) {
    int expo_delta = max_expo - min_expo;

    for (int i = 0; i < expo_delta && can_10_shift(min_dec); ++i) {
      shift_10_left(min_dec, 1);
      min_expo += 1;
      min_info->expo = min_expo;
    }
  }

  if (min_expo != max_expo) {
    int expo_delta = max_expo - min_expo;

    shift_10_right(max_dec, expo_delta);
    max_info->expo = min_expo;

    if (buisness_round && is_zero(max_dec)) {
      int sign = info1->sign != info2->sign;

      if (sign == SIGN_MINUS) {
        setsign(min_dec, 0, 0);
      } else {
        if (is_max(val1)) {
          err = RES_OVERFLOW;
        } else {
          if (getsign(min_dec, 0)) {
            decimal one;
            set_one(&one);
            sum(min_dec, &one, min_dec);
          }
        }
      }
    }
  }

  return err;
}

int sum(decimal* val1, decimal* val2, decimal* res) {
  int s1 = 0;
  int s2 = 0;
  int tmp = 0;

  for (int i = 0; i < MAX_LEN; ++i) {
    s1 = getsign(val1, i);
    s2 = getsign(val2, i);

    setsign(res, i, s1 ^ s2 ^ tmp);

    tmp = (s1 && s2) || ((s1 || s2) && tmp);
  }

  return tmp;
}

void reverse_to_pos(decimal* src) {
  decimal one;
  set_max(&one);
  sum(src, &one, src);
  reverse(src);
}

void reverse_to_neg(decimal* src) {
  decimal one;
  set_one(&one);

  reverse(src);

  sum(src, &one, src);
}

int add(decimal value_1, decimal value_2, decimal* result) {
  set_zero(result);

  int err = RES_OK;

  decimal_info* info1 = get_info(&value_1);
  decimal_info* info2 = get_info(&value_2);
  decimal_info* res_info = get_info(result);

  if (!is_zero(&value_1) && !is_zero(&value_2)) {
    err = equalize_expo(&value_1, &value_2, true);
  }

  if (is_zero(&value_1) && !err) info1->sign = 0;
  if (is_zero(&value_2) && !err) info2->sign = 0;

  int sign1 = info1->sign;
  int sign2 = info2->sign;

  bool sign_difference = sign1 != sign2;
  int sign = SIGN_PLUS;

  if (err == RES_OVERFLOW) {
    if (info1->sign == SIGN_MINUS && info2->sign == SIGN_MINUS)
      sign = SIGN_MINUS;
  }

  if (sign1 == SIGN_MINUS && sign2 == SIGN_MINUS) sign = SIGN_MINUS;

  if (sign_difference) {
    if (sign1 == SIGN_MINUS) reverse_to_neg(&value_1);
    if (sign2 == SIGN_MINUS) reverse_to_neg(&value_2);
  }

  if (!err) {
    int tmp = sum(&value_1, &value_2, result);

    if ((sign1 == SIGN_MINUS || sign2 == SIGN_MINUS) && sign_difference) {
      sign = sign1 ^ sign2 ^ tmp;
    } else if (tmp) {
      err = RES_OVERFLOW;
    }
  }

  if (!err) {
    if (sign == SIGN_MINUS && sign_difference) reverse_to_pos(result);
    res_info->sign = sign;
    res_info->expo = info1->expo;
  } else {
    set_zero(result);
  }

  if (err == RES_OVERFLOW) {
    err = sign == SIGN_PLUS ? RES_INF_POSITIVE : RES_INF_NEGATIVE;
  }

  return err;
}

int sub(decimal value_1, decimal value_2, decimal* result) {
  decimal_info* v2_info = get_info(&value_2);

  if (first_sign(&value_2) == -1) {  // if value_2 is zero
    v2_info->sign = 0;
  } else {
    v2_info->sign ^= 1;
  }
  return add(value_1, value_2, result);
}

int mul(decimal value_1, decimal value_2, decimal* result) {
  set_zero(result);
  int err = RES_OK;

  decimal_info* v1_info = get_info(&value_1);
  decimal_info* v2_info = get_info(&value_2);
  decimal_info* res_info = get_info(result);

  int sign = v1_info->sign ^ v2_info->sign;

  int length1 = last_sign(&value_1) + 1;
  int length2 = last_sign(&value_2) + 1;

  int s1 = 0;
  int s2 = 0;

  decimal tmp = ZERO_DECIMAL;

  if (length1 != 1 && length2 != 1 && length1 + length2 >= MAX_LEN + 2) {
    err = RES_OVERFLOW;
  }

  for (int v2i = 0; v2i < length2 && !err; ++v2i) {
    set_zero(&tmp);

    s1 = getsign(&value_2, v2i);

    for (int v1i = 0; v1i < length1; ++v1i) {
      s2 = getsign(&value_1, v1i);

      setsign(&tmp, v1i + v2i, s1 && s2);
    }

    err = sum(result, &tmp, result);
    if (err) err = RES_OVERFLOW;
  }

  if (err == RES_OVERFLOW)
    err = sign == SIGN_PLUS ? RES_INF_POSITIVE : RES_INF_NEGATIVE;

  if (err) {
    set_zero(result);
  } else {
    if (length1 == 0 || length2 == 0) {
      res_info->sign = 0;
    } else {
      res_info->sign = sign;
      int expo = v1_info->expo + v2_info->expo;

      if (expo > MAX_EXPO) {
        decimal mod = shift_10_right(result, expo - MAX_EXPO);
        decimal one;
        set_one(&one);

        if (mod.bits[LOW] >= 5) sum(result, &one, result);

        expo = MAX_EXPO;
      }

      res_info->expo = expo;
    }
  }

  return err;
}

decimal div_int(decimal val1, decimal val2,
                        decimal* result) {
  result->bits[LOW] = result->bits[MID] = result->bits[HIGH] = 0;
  decimal v2_save = val2;

  int len1 = 0;
  int len2 = last_sign(&val2);
  int len_delta = 0;

  while (is_equals_int(&val1, &val2) || is_greater_int(&val1, &val2)) {
    len1 = last_sign(&val1);
    len_delta = len1 - len2;
    shift_left(&val2, len_delta);

    if (is_greater_int(&val2, &val1)) {
      shift_right(&val2, 1);
      len_delta -= 1;
    }

    reverse_to_neg(&val2);
    sum(&val1, &val2, &val1);

    decimal res;
    set_one(&res);
    shift_left(&res, len_delta);

    sum(result, &res, result);

    val2 = v2_save;
  }

  decimal mod = val1;
  return mod;
}

int div(decimal value_1, decimal value_2, decimal* result) {
  int err = RES_OK;
  set_zero(result);

  if (is_zero(&value_2)) {
    err = RES_ZERO_DIVISION;
  }

  if (!err) {
    equalize_expo(&value_1, &value_2, false);
    if (is_zero(&value_2)) err = RES_OVERFLOW;
  }

  decimal_info* v1_info = get_info(&value_1);
  decimal_info* v2_info = get_info(&value_2);
  decimal_info* res_info = get_info(result);

  int sign = v1_info->sign != v2_info->sign;

  while (!is_zero(&value_1) && can_10_shift(result) && !err &&
         res_info->expo < MAX_EXPO) {
    int expo = 0;
    while (is_lower_int(&value_1, &value_2) && can_10_shift(&value_1) &&
           can_10_shift(result)) {
      shift_10_left(&value_1, 1);
      shift_10_left(result, 1);
      expo += 1;
    }

    if (expo == 0 && is_lower_int(&value_1, &value_2)) {
      value_1 = ZERO_DECIMAL;
    }

    decimal res = ZERO_DECIMAL;
    value_1 = div_int(value_1, value_2, &res);

    err = sum(result, &res, result);
    res_info->expo += expo;
  }

  if (res_info->expo > MAX_EXPO) {
    shift_10_right(result, res_info->expo - MAX_EXPO + 1);
    res_info->expo = MAX_EXPO;
  }

  if (err == RES_OVERFLOW)
    err = sign == SIGN_PLUS ? RES_INF_POSITIVE : RES_INF_NEGATIVE;

  if (err) {
    set_zero(result);
  } else if (is_zero(result)) {
    res_info->sign = 0;
    res_info->expo = 0;
  } else {
    res_info->sign = sign;
  }

  return err;
}

int mod(decimal value_1, decimal value_2, decimal* result) {
  int err = RES_OK;
  set_zero(result);

  if (is_zero(&value_2)) {
    err = RES_ZERO_DIVISION;
  }

  if (!err) {
    equalize_expo(&value_1, &value_2, false);
  }

  bool is_zero_delim = is_zero(&value_2);

  decimal_info* v1_info = get_info(&value_1);
  decimal_info* v2_info = get_info(&value_2);
  decimal_info* res_info = get_info(result);

  int inv_sign = v1_info->sign != v2_info->sign;
  int sign = v2_info->sign;

  if (!err && !is_zero_delim) {
    int expo = v1_info->expo > v2_info->expo ? v1_info->expo : v2_info->expo;

    decimal tmp = ZERO_DECIMAL;
    decimal mod = div_int(value_1, value_2, &tmp);

    if (inv_sign && !is_zero(&mod)) {
      reverse_to_neg(&mod);
      sum(&value_2, &mod, result);
    } else {
      *result = mod;
    }

    res_info->sign = sign;
    res_info->expo = expo;
  }

  if (err == RES_OVERFLOW)
    err = sign == SIGN_PLUS ? RES_INF_POSITIVE : RES_INF_NEGATIVE;

  if (err) {
    set_zero(result);
  } else if (is_zero(result)) {
    res_info->sign = 0;
    res_info->expo = 0;
  }

  return err;
}
