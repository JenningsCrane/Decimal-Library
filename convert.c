#include "decimal.h"

int from_int_to_decimal(int src, decimal *dst) {
  int err = dst == NULL;

  if (!err) {
    int new_src = src;

    set_zero(dst);
    decimal_info *dec_info = get_info(dst);

    if (src < 0) {
      dec_info->sign = 1;
      new_src *= -1;
    }
    dst->bits[LOW] = new_src;
  }

  return err;
}

int from_decimal_to_int(decimal src, int *dst) {
  truncate(src, &src);

  decimal_info *src_info = get_info(&src);

  int err = last_sign(&src) >= 31;
  err = dst == NULL;

  if (!err) {
    *dst = src.bits[LOW];
    if (src_info->sign) {
      *dst *= -1;
    }
  }

  return err;
}

int read_exp(float src) {
  int nsrc = *(uint32_t *)&src;

  int mask = 0b11111111u << 23;
  int expo = nsrc & mask;
  expo >>= 23;

  return expo - 127;
}

int from_float_to_decimal(float src, decimal *dst) {
  int err = dst == NULL;

  if (!err) {
    set_zero(dst);
    decimal_info *dst_info = get_info(dst);

    int fexp = read_exp(src);

    if (fexp > LAST_BIT) {
      err = RES_ERR;
    } else {
      if (src < 0) {
        src = -src;
        dst_info->sign = 1;
      }

      float srct = src;
      float mul = 1.0f;

      for (; !((int)srct); srct *= 10, mul *= 10) dst_info->expo += 1;
      for (; srct < 10000000 && srct != (int)srct; srct *= 10, mul *= 10)
        dst_info->expo += 1;

      if (mul != 1.0f && fmodf(src * mul, 10.0f) == 0) {
        mul /= 10;
        dst_info->expo -= 1;
      }

      if (mul != 1.0f) {
        src *= mul;
      }

      fexp = read_exp(src);

      if (fexp < -94 || dst_info->expo > MAX_EXPO) {
        err = RES_ERR;
      } else {
        uint32_t usrc = ftou(src);

        setsign(dst, fexp, 1);
        fexp -= 1;

        for (int mask = 0x400000; mask > 0 && fexp >= 0;
             mask >>= 1, fexp -= 1) {
          int bit = !!(mask & usrc);
          setsign(dst, fexp, bit);
        }
      }
    }
  }

  if (err && dst != NULL) {
    set_zero(dst);
  }

  return err;
}

int from_decimal_to_float(decimal src, float *dst) {
  int err = dst == NULL;

  if (!err) {
    float res = 0.0f;
    decimal_info *src_info = get_info(&src);

    for (int i = 0; i < MAX_LEN; ++i) {
      if (getsign(&src, i)) {
        res += powf(2.0f, (float)i);
      }
    }

    float delim = 1.0f;
    for (int i = 0; i < src_info->expo; ++i) {
      delim *= 10.0f;
    }

    if (delim != 1.0f) {
      res /= delim;
    }

    if (src_info->sign) {
      res = -res;
    }

    *dst = res;
  }

  return err;
}
