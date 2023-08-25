#include "decimal.h"

#define COMP_LOWER -1
#define COMP_EQUALS 0
#define COMP_GREATER 1

int check_num(uint32_t n1, uint32_t n2) {
  int res = COMP_EQUALS;
  if (n1 > n2)
    res = COMP_GREATER;
  else if (n1 < n2)
    res = COMP_LOWER;
  return res;
}

int is_greater_int(decimal* val1, decimal* val2) {
  int res = check_num(val1->bits[HIGH], val2->bits[HIGH]);
  if (!res) res = check_num(val1->bits[MID], val2->bits[MID]);
  if (!res) res = check_num(val1->bits[LOW], val2->bits[LOW]);
  res = res == COMP_GREATER;
  return res;
}

int is_lower_int(decimal* val1, decimal* val2) {
  int res = check_num(val1->bits[HIGH], val2->bits[HIGH]);
  if (!res) res = check_num(val1->bits[MID], val2->bits[MID]);
  if (!res) res = check_num(val1->bits[LOW], val2->bits[LOW]);
  res = res == COMP_LOWER;
  return res;
}

int is_equals_int(decimal* val1, decimal* val2) {
  return val1->bits[LOW] == val2->bits[LOW] &&
         val1->bits[MID] == val2->bits[MID] &&
         val1->bits[HIGH] == val2->bits[HIGH];
}

int is_equal(decimal val1, decimal val2) {
  decimal_info* v1_info = get_info(&val1);
  decimal_info* v2_info = get_info(&val2);

  int res = is_equals_int(&val1, &val2);

  if (v1_info->expo != v2_info->expo) {
    res = 0;
  }

  if (v1_info->sign != v2_info->sign) {  // проверка знака
    res = 0;
  }

  return res;
}

int is_not_equal(decimal val1, decimal val2) {
  return !is_equal(val1, val2);
}

int is_less(decimal val1, decimal val2) {
  decimal_info* info1 = get_info(&val1);
  decimal_info* info2 = get_info(&val2);

  equalize_expo(&val1, &val2, 0);  // нормализация коэффициентов масштабирования
  int res = 0;

  if (info1->sign == SIGN_PLUS && info2->sign == SIGN_PLUS) {
    res = is_lower_int(&val1, &val2);
  } else if (info1->sign == SIGN_MINUS && info2->sign == SIGN_MINUS) {
    res = is_greater_int(&val1, &val2);
  } else {
    res = !(info2->sign);
  }

  return res;
}

int is_greater(decimal val1, decimal val2) {
  decimal_info* info1 = get_info(&val1);
  decimal_info* info2 = get_info(&val2);

  equalize_expo(&val1, &val2, 0);
  int res = 0;

  if (info1->sign == SIGN_PLUS && info2->sign == SIGN_PLUS) {
    res = is_greater_int(&val1, &val2);
  } else if (info1->sign == SIGN_MINUS && info2->sign == SIGN_MINUS) {
    res = is_lower_int(&val1, &val2);
  } else {
    res = !(info1->sign);
  }
  return res;
}

int is_less_or_equal(decimal val1, decimal val2) {
  equalize_expo(&val1, &val2, 0);
  int res = is_less(val1, val2) == 1 || is_equal(val1, val2) == 1;
  return res;
}

int is_greater_or_equal(decimal val1, decimal val2) {
  equalize_expo(&val1, &val2, 0);
  int res = is_greater(val1, val2) == 1 || is_equal(val1, val2) == 1;
  return res;
}
