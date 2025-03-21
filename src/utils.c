#include <string.h>
#include <stdlib.h>

#include "Rinternals.h"

SEXP rf_get_list_element(SEXP list, const char *str) {
  SEXP elmt = R_NilValue;
  SEXP names = PROTECT(Rf_getAttrib(list, R_NamesSymbol));

  for (R_xlen_t i = 0; i < Rf_xlength(list); i++) {
    if (strcmp(CHAR(STRING_ELT(names, i)), str) == 0) {
       elmt = VECTOR_ELT(list, i);
       break;
    }
  }
  UNPROTECT(1);
  return elmt;
}

SEXP trim_(SEXP x) {
  size_t len = LENGTH(x);

  SEXP out = PROTECT(Rf_allocVector(STRSXP, len));
  size_t num;
  for (num = 0; num < len; ++num) {
    const char* xx = Rf_translateCharUTF8(STRING_ELT(x, num));
    size_t str_len = strlen(xx);

    char* str = (char*)malloc(str_len + 1);
    size_t i = 0, start = 0;
    int new_line = 0;

    /* skip leading blanks on first line */
    while (start < str_len && (xx[start] == ' ' || xx[start] == '\t')) {
      ++start;
    }

    /* Skip first newline */
    if (start < str_len && xx[start] == '\n') {
      new_line = 1;
      ++start;
    }

    i = start;

    /* Ignore first line */
    if (!new_line) {
      while (i < str_len && xx[i] != '\n') {
        ++i;
      }
      new_line = 1;
    }

    size_t indent = 0;

    /* Maximum size of size_t */
    size_t min_indent = (size_t)-1;

    /* find minimum indent */
    while (i < str_len) {
      if (xx[i] == '\n') {
        new_line = 1;
        indent = 0;
      } else if (new_line) {
        if (xx[i] == ' ' || xx[i] == '\t') {
          ++indent;
        } else {
          if (indent < min_indent) {
            min_indent = indent;
          }
          indent = 0;
          new_line = 0;
        }
      }
      ++i;
    }

    /* if string ends with '\n', `indent = 0` only because we made it so */
    if (xx[str_len - 1] != '\n' && new_line && indent < min_indent) {
      min_indent = indent;
    }

    new_line = 1;
    i = start;
    size_t j = 0;

    /*Rprintf("start: %i\nindent: %i\nmin_indent: %i", start, indent,
     * min_indent);*/

    /* copy the string removing the minimum indent from new lines */
    while (i < str_len) {
      if (xx[i] == '\n') {
        new_line = 1;
      } else if (xx[i] == '\\' && i + 1 < str_len && xx[i + 1] == '\n') {
        new_line = 1;
        i += 2;
        continue;
      } else if (new_line) {
        size_t skipped = strspn(xx + i, "\t ");
        /*
         * if the line consists only of tabs and spaces, and if the line is
         * shorter than min_indent, copy the entire line and proceed to the
         * next
         */
        if (*(xx + i + skipped) == '\n' && skipped < min_indent) {
          strncpy(str + j, xx + i, skipped);
          i += skipped;
          j += skipped;
        } else {
          if (i + min_indent < str_len && (xx[i] == ' ' || xx[i] == '\t')) {
            i += min_indent;
          }
        }
        new_line = 0;
        continue;
      }
      str[j++] = xx[i++];
    }
    str[j] = '\0';

    /* Remove trailing whitespace up to the first newline */
    size_t end = j;
    while (j > 0) {
      if (str[j] == '\n') {
        end = j;
        break;
      } else if (str[j] == '\0' || str[j] == ' ' || str[j] == '\t') {
        --j;
      } else {
        break;
      }
    }
    str[end] = '\0';
    SET_STRING_ELT(out, num, Rf_mkCharCE(str, CE_UTF8));
    free(str);
  }

  UNPROTECT(1);
  return out;
}
