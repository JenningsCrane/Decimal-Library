#include "decimal.h"

int truncate(decimal value, decimal* result) {
  int err = result == NULL;

  if (!err) {
    decimal_info* info = get_info(&value);
    shift_10_right(&value, info->expo);
    info->expo = 0;
    *result = value;
  }

  return err;
}

int negate(decimal value, decimal* result) {
  int err = result == NULL;

  if (!err) {
    *result = value;
    get_info(result)->sign = !(get_info(&value)->sign);
  }

  return err;
}

int floor(decimal value, decimal* result) {
  int err = result == NULL;

  if (!err) {
    set_zero(result);

    decimal_info* info = get_info(&value);
    bool has_expo = info->expo > 0;

    if (has_expo) {
      truncate(value, &value);
    }

    if (info->sign == SIGN_MINUS && has_expo) {
      if (is_max(&value)) {
        err = RES_ERR;
      } else {
        decimal one;
        set_one(&one);
        sum(&value, &one, &value);
      }
    }

    if (!err) {
      *result = value;
    }
  }
  return err;
}

int round(decimal value, decimal* result) {
  int err = result == NULL;

  if (!err) {
    set_zero(result);

    decimal_info* info = get_info(&value);
    decimal mod = shift_10_right(&value, info->expo);
    info->expo = 0;

    if (mod.bits[LOW] >= 5) {
      if (is_max(&value)) {
        err = RES_ERR;
      } else {
        decimal one;
        set_one(&one);
        sum(&value, &one, &value);
      }
    }

    if (!err) {
      *result = value;
    }
  }
  return err;
}
