#include "s21_sprintf.h"

#include <float.h>
#include <stdarg.h>
#include <stdint.h>  //нужна для работы с uintptr_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// bitset for flag
#define FLAGS_ZEROPAD 1
#define FLAGS_LEFT 2
#define FLAGS_PLUS 4
#define FLAGS_SPACE 8
#define FLAGS_HASH 16
#define FLAGS_PRECISION 32
#define FLAGS_LONG 64
#define FLAGS_LONG_LONG 128
#define FLAGS_LONG_DOUBLE 256
#define FLAGS_CHAR 512
#define FLAGS_SHORT 1024
#define FLAGS_UPPERCASE 2048
#define FLAGS_ADAPT_EXP 4096

#define MAXINT_BUFFER_SIZE 32
#define FLOAT_BUFFER_SIZE 32
#define DEFAULT_FLOAT_PRECISION 6
#define MAX_PRINTABLE_FLOAT 1e9

// output function
void out(char character, char* str, size_t index, size_t maxlen) {
  if (index < maxlen) {
    str[index] = character;
  }
}

// reverse order output
size_t out_rev(char* str, size_t index, size_t maxlen, size_t length,
               const char* buffer, unsigned int width, unsigned int flags) {
  const size_t start_idx = index;
  // заполнение пробелами до заданной ширины
  if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
    for (size_t i = length; i < width; i++) {
      out(' ', str, index++, maxlen);
    }
  }

  // обратная строка
  while (length) {
    out(buffer[--length], str, index++, maxlen);
  }

  // добавляем пространство заполнения по значению width
  if (flags & FLAGS_LEFT) {
    while (index - start_idx < width) {
      out(' ', str, index++, maxlen);
    }
  }
  return index;
}

size_t len_s(const char* str, size_t maxsize) {
  const char* s;
  for (s = str; *s && maxsize--; ++s)
    ;
  return (size_t)(s - str);
}

// проверка, является ли символ цифоровым
int isDigit(char ch) { return (ch >= '0' && ch <= '9'); }

// конвертация строки в целое число (need to be remastered)
unsigned int strtoint(const char** str) {
  unsigned int i = 0;
  while (isDigit(**str)) {
    i = i * 10 + (unsigned int)(*((*str)++) - '0');
  }
  return i;
}

size_t inttostr(unsigned long long value, unsigned int isNegative, char* str,
                size_t idx, unsigned int base, unsigned int precision,
                unsigned int width, unsigned int flags) {
  // длина буфера
  // const unsigned int buffsize = (precision > 0 && flags & FLAGS_PRECISION) ?
  // precision : MAXINT_BUFFER_SIZE;
  char buff[1024];
  size_t length = 0;

  if (!value) {
    flags &= ~FLAGS_HASH;
  }
  // разбираем число на символы исходя из системы счисления
  if (!(flags & FLAGS_PRECISION) || value) {
    do {
      const char digit = (char)(value % base);
      buff[length] = digit < 10
                         ? '0' + digit
                         : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
      length++;
      value /= base;
    } while (value && length < 1024);
  }

  // заполнение лидирующими нулями
  if (width && (flags & FLAGS_ZEROPAD) &&
      (isNegative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
    width--;
  }
  while ((length < precision) && (length < 1024)) {
    buff[length] = '0';
    length++;
  }
  while ((flags & FLAGS_ZEROPAD) && (length < width) && (length < 1024)) {
    buff[length] = '0';
    length++;
  }

  if (flags & FLAGS_HASH) {
    if (!(flags & FLAGS_PRECISION) && length &&
        ((length == precision) || (length == width))) {
      length--;
      if (length && (base == 16)) {
        length--;
      }
    }
    if ((base == 16) && !(flags & FLAGS_UPPERCASE) && (length < 1024)) {
      buff[length++] = 'x';
    } else if ((base == 16) && (flags & FLAGS_UPPERCASE) && (length < 1024)) {
      buff[length++] = 'X';
    }
    if (length < 1024) {
      buff[length++] = '0';
    }
  }

  // обработка  знака - флагов + и space
  if (length < 1024) {
    if (isNegative) {
      buff[length++] = '-';
    } else if (flags & FLAGS_PLUS) {
      buff[length++] = '+';  // игнорируем spce если '+' существует
    } else if (flags & FLAGS_SPACE) {
      buff[length++] = ' ';
    }
  }

  return out_rev(str, idx, 1024, length, buff, width, flags);
}

size_t exptostr(long double value, char* str, size_t idx,
                unsigned int precision, unsigned int width, unsigned int flags);

size_t floattostr(long double value, char* str, size_t idx,
                  unsigned int precision, unsigned int width,
                  unsigned int flags) {
  char buff[1024];
  size_t length = 0;
  long double diff = 0.0;

  // обработка специальных значений
  if (value != value) {
    return out_rev(str, idx, 1024, 3, "nan", width, flags);
  } else if (value < -LDBL_MAX && flags & FLAGS_LONG_DOUBLE) {
    return out_rev(str, idx, 1024, 3, "fni-", width, flags);
  }
  if (value > LDBL_MAX && flags & FLAGS_LONG_DOUBLE) {
    return out_rev(str, idx, 1024, 3, "fni", width, flags);
  } else if (value < -DBL_MAX) {
    return out_rev(str, idx, 1024, 3, "fni-", width, flags);
  } else if (value > DBL_MAX) {
    return out_rev(str, idx, 1024, 3, "fni", width, flags);
  }

  const int isNegative = (value < 0);
  if (isNegative) {
    value = -value;
  }

  // тестируем на очень большие значения
  if (value > MAX_PRINTABLE_FLOAT) {
    exptostr(value, str, idx, precision, width, flags);
  }

  // задаем точноcть по умолчанию
  if (!(flags & FLAGS_PRECISION)) precision = DEFAULT_FLOAT_PRECISION;

  // определяем множитель для дробной части исходя из точности
  unsigned long long pow10 = 1;
  if (precision > 0) {
    for (unsigned int i = 0; i < precision; i++) pow10 *= 10;
  }

  // отделяем целую часть от дробной
  unsigned long long whole = (unsigned long long)value;
  long double tmp = (value - whole) * pow10;
  unsigned long long fraction = (unsigned long long)tmp;
  diff = tmp - fraction;

  if (diff > 0.5) {
    fraction++;
    if (fraction >= pow10) {
      fraction = 0;
      ++whole;
    }
  }

  if (precision == 0) {
    diff = value - whole;
    if (diff >= 0.5) {
      ++whole;
    }
  } else {
    // работаем с дробной частью
    unsigned int count = precision;
    while (length < 1024) {
      --count;
      buff[length++] = '0' + (fraction % 10);
      if (!(fraction /= 10)) {
        break;
      }
    }
    while ((length < 1024) && (count-- > 0)) {
      buff[length++] = '0';
    }
    if (length < 1024) {
      // добавляем разделитель
      buff[length++] = '.';
    }
  }
  while (length < 1024) {
    buff[length++] = (char)('0' + (whole % 10));
    if (!(whole /= 10)) {
      break;
    }
  }

  // заполнение ведущими нулями
  if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD)) {
    if (width && (isNegative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
      width--;
    }
    while ((length < width) && (length < 1024)) {
      buff[length++] = '0';
    }
  }

  if (length < 1024) {
    if (isNegative) {
      buff[length++] = '-';
    } else if (flags & FLAGS_PLUS) {
      buff[length++] = '+';
    } else if (flags & FLAGS_SPACE) {
      buff[length++] = ' ';
    }
  }

  return out_rev(str, idx, 1024, length, buff, width, flags);
}

size_t exptostr(long double value, char* str, size_t idx,
                unsigned int precision, unsigned int width,
                unsigned int flags) {
  if ((value != value) || (value > DBL_MAX) || (value < -DBL_MAX)) {
    return floattostr(value, str, idx, precision, width, flags);
  }

  const int isNegative = value < 0;
  if (isNegative) {
    value = -value;
  }

  if (!(flags & FLAGS_PRECISION)) {
    precision = DEFAULT_FLOAT_PRECISION;
  }

  int expval = 0;

  if (value < 1.0 && value) {
    while (value < 1.0) {
      value *= 10;
      expval--;
    }
  } else if (value > 1) {
    while (value / 10 > 1.0) {
      value /= 10;
      expval++;
    }
  }

  unsigned int minwidth = ((expval < 100) && (expval > -100)) ? 4 : 5;

  if (flags & FLAGS_ADAPT_EXP) {
    // do we want to fall-back to "%f" mode?
    if ((value >= 1e-4) && (value < 1e6)) {
      if ((int)precision > expval) {
        precision = (unsigned)((int)precision - expval - 1);
      } else {
        precision = 0;
      }
      flags |= FLAGS_PRECISION;  // make sure _ftoa respects precision
      // no characters in exponent
      minwidth = 0U;
      expval = 0;
    } else {
      // we use one sigfig for the whole part
      if ((precision > 0) && (flags & FLAGS_PRECISION)) {
        --precision;
      }
    }
  }

  // will everything fit?
  unsigned int fwidth = width;
  if (width > minwidth) {
    // we didn't fall-back so subtract the characters required for the exponent
    fwidth -= minwidth;
  } else {
    // not enough characters, so go back to default sizing
    fwidth = 0U;
  }
  if ((flags & FLAGS_LEFT) && minwidth) {
    // if we're padding on the right, DON'T pad the floating part
    fwidth = 0U;
  }

  // output the floating part
  const size_t start_idx = idx;
  idx = floattostr(isNegative ? -value : value, str, idx, precision, fwidth,
                   flags & ~FLAGS_ADAPT_EXP);

  // output the exponent part
  if (minwidth) {
    // output the exponential symbol
    out((flags & FLAGS_UPPERCASE) ? 'E' : 'e', str, idx++, 1024);
    // output the exponent value
    idx = inttostr((expval < 0) ? -expval : expval, (expval < 0), str, idx, 10,
                   expval < 0, minwidth - 1, FLAGS_ZEROPAD | FLAGS_PLUS);
    // might need to right-pad spaces
    if (flags & FLAGS_LEFT) {
      while (idx - start_idx < width) out(' ', str, idx++, 1024);
    }
  }

  return idx;
}

int s21_sprintf(char* str, const char* format, ...) {
  va_list args;
  va_start(args, format);
  // format specifiers %[flags][width].[precision][varlength]
  size_t maxlen = 1024;
  size_t idx = 0;

  while (*format) {
    if (*format == '%') {
      unsigned int flags = 0, width = 0, precision = 0, n = 0;
      format++;

      // обрабатываем флаги если есть. (done)
      do {
        switch (*format) {
          case '0':
            flags |= FLAGS_ZEROPAD;
            format++;
            n = 1;
            break;
          case '+':
            flags |= FLAGS_PLUS;
            format++;
            n = 1;
            break;
          case '-':
            flags |= FLAGS_LEFT;
            format++;
            n = 1;
            break;
          case ' ':
            flags |= FLAGS_SPACE;
            format++;
            n = 1;
            break;
          case '#':
            flags |= FLAGS_HASH;
            format++;
            n = 1;
            break;
          default:
            n = 0;
            break;
        }
      } while (n);

      // обработка спецификатора width (done)
      if (*format == '*') {
        const int w = va_arg(args, int);
        if (w < 0) {
          flags |= FLAGS_LEFT;
          width = (unsigned int)-w;
        } else {
          width = (unsigned int)w;
        }
        format++;
      } else if (isDigit(*format)) {
        width = strtoint(&format);
      }

      // обработка спецификатора точности (done)
      if (*format == '.') {
        flags |= FLAGS_PRECISION;
        format++;
        if (*format == '*') {
          const int prec = va_arg(args, int);
          precision = prec > 0 ? (unsigned int)prec : 0;
          format++;
        } else {
          if (isDigit(*format)) precision = strtoint(&format);
        }
      }

      // обработка длины переменной (done)
      switch (*format) {
        case 'l':
          flags |= FLAGS_LONG;
          format++;
          if (*format == 'l') {
            flags |= FLAGS_LONG_LONG;
            format++;
          }
          break;
        case 'L':
          flags |= FLAGS_LONG_DOUBLE;
          format++;
          break;
        case 'h':
          flags |= FLAGS_SHORT;
          format++;
          if (*format == 'h') {
            flags |= FLAGS_CHAR;
            format++;
          }
          break;
      }

      // data format processing
      switch (*format) {
        // целочисленные форматы
        case 'd':
        case 'i':
        case 'u':
        case 'x':
        case 'X':
        case 'o':
        case 'b': {
          // система счисления
          unsigned int numbase;
          if (*format == 'x' || *format == 'X') {
            numbase = 16;
          } else if (*format == 'o') {
            numbase = 8;
          } else {
            numbase = 10;
            flags &= ~FLAGS_HASH;
          }

          // шестнадцатиричное большими буквами
          if (*format == 'X') {
            flags |= FLAGS_UPPERCASE;
          }

          // отключаем флаги PLUS и SPACE для форматов u, x, X, o, b (unsigned
          // formats)
          if ((*format != 'i') && (*format != 'd')) {
            flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
          }

          // игнорируем флаг ZEROPAD для числа с заданной точностью
          if (flags & FLAGS_PRECISION) {
            flags &= ~FLAGS_ZEROPAD;
          }

          // читаем целое число (со знаком)
          if ((*format == 'i') || (*format == 'd')) {
            if (flags & FLAGS_LONG_LONG) {
              const long long int value = va_arg(args, long long int);
              idx = inttostr((unsigned long long)(value >= 0 ? value : -value),
                             (value < 0), str, idx, numbase, precision, width,
                             flags);
            } else if (flags & FLAGS_LONG) {
              const long value = va_arg(args, long);
              idx = inttostr((unsigned long)(value > 0 ? value : 0 - value),
                             (value < 0), str, idx, numbase, precision, width,
                             flags);
            } else if (flags & FLAGS_CHAR) {
              const int value = (char)va_arg(args, int);
              idx = (char)inttostr(
                  (unsigned int)(value > 0 ? value : 0 - value), (value < 0),
                  str, idx, numbase, precision, width, flags);
            } else if (flags & FLAGS_SHORT) {
              const int value = (short)va_arg(args, int);
              idx = (short)inttostr(
                  (unsigned int)(value > 0 ? value : 0 - value), (value < 0),
                  str, idx, numbase, precision, width, flags);
            } else {
              const int value = va_arg(args, int);
              idx = inttostr((unsigned int)(value > 0 ? value : 0 - value),
                             (value < 0), str, idx, numbase, precision, width,
                             flags);
            }
          } else {
            if (flags & FLAGS_LONG_LONG) {
              const unsigned long long value = va_arg(args, unsigned long long);
              idx =
                  inttostr((unsigned long long)(value > 0 ? value : 0 - value),
                           0, str, idx, numbase, precision, width, flags);
            } else if (flags & FLAGS_LONG) {
              const unsigned long value = va_arg(args, unsigned long);
              idx = inttostr((unsigned long)(value > 0 ? value : 0 - value), 0,
                             str, idx, numbase, precision, width, flags);
            } else if (flags & FLAGS_CHAR) {
              const unsigned int value = (char)va_arg(args, unsigned int);
              idx =
                  (char)inttostr((unsigned int)(value > 0 ? value : 0 - value),
                                 0, str, idx, numbase, precision, width, flags);
            } else if (flags & FLAGS_SHORT) {
              const unsigned int value = (short)va_arg(args, unsigned int);
              idx = (short)inttostr(
                  (unsigned int)(value > 0 ? value : 0 - value), 0, str, idx,
                  numbase, precision, width, flags);
            } else {
              const unsigned int value = va_arg(args, unsigned int);
              idx = inttostr((unsigned int)(value > 0 ? value : 0 - value), 0,
                             str, idx, numbase, precision, width, flags);
            }
          }
          format++;
          break;
        }
        // читаем float
        case 'f': {
          long double value;
          if (!(flags & FLAGS_LONG_DOUBLE))
            value = (long double)va_arg(args, double);
          else
            value = (long double)va_arg(args, long double);
          idx = floattostr(value, str, idx, precision, width, flags);
          format++;
          break;
        }
        // читаем научную нотацию
        case 'e':
        case 'E':
        case 'g':
        case 'G': {
          long double value;
          if (!(flags & FLAGS_LONG_DOUBLE))
            value = (long double)va_arg(args, double);
          else
            value = (long double)va_arg(args, long double);
          if ((*format == 'g') || (*format == 'G')) flags |= FLAGS_ADAPT_EXP;
          if ((*format == 'E') || (*format == 'G')) flags |= FLAGS_UPPERCASE;
          idx = exptostr(value, str, idx, precision, width, flags);
          format++;
          break;
        }

        // читаем символ (done)
        case 'c': {
          unsigned int l = 1;
          // pre padding (done)
          if (!(flags & FLAGS_LEFT)) {
            while (l < width) {
              out(' ', str, idx++, maxlen);
              l++;
            }
          }
          // char output (done)
          out((char)va_arg(args, int), str, idx++, maxlen);

          // post padding (done)
          if (flags & FLAGS_LEFT) {
            while (l < width) {
              out(' ', str, idx++, maxlen);
              l++;
            }
          }
          format++;
          break;
        }

          // читаем строку
        case 's': {
          const char* p = va_arg(args, char*);
          size_t l = len_s(p, precision ? precision : (size_t)-1);
          // pre padding
          if (flags & FLAGS_PRECISION) {
            l = (l < precision ? l : precision);
          }
          if (!(flags & FLAGS_LEFT)) {
            while (l++ < width) {
              out(' ', str, idx++, maxlen);
            }
          }
          // string output
          while ((*p != 0) && (!(flags & FLAGS_PRECISION) || precision--)) {
            out(*(p++), str, idx++, maxlen);
          }
          // post padding (done)
          if (flags & FLAGS_LEFT) {
            while (l++ < width) {
              out(' ', str, idx++, maxlen);
            }
          }
          format++;
          break;
        }

        // читаем указатель
        case 'p': {
          flags |= FLAGS_HASH;
          const size_t is_ll = sizeof(uintptr_t) == sizeof(long long);
          if (is_ll) {
            idx = inttostr((uintptr_t)va_arg(args, void*), 0, str, idx, 16,
                           precision, width, flags);
          } else {
            idx = inttostr((intptr_t)va_arg(args, void*), 0, str, idx, 16,
                           precision, width, flags);
          }

          format++;
          break;
        }
        case 'n': {
          int* val = va_arg(args, int*);
          *val = idx;
          format++;
          break;
        }
        // читаем символ % (done)
        case '%':
          out('%', str, idx++, maxlen);
          format++;
          break;

        // читаем все подряд (done)
        default:
          out(*format, str, idx++, maxlen);
          format++;
          break;
      }
    } else {
      // if nonformat symbol
      out(*format, str, idx++, maxlen);
      format++;
    }
  }
  out('\0', str, idx++, maxlen);
  va_end(args);

  return (int)s21_strlen(str);
}
