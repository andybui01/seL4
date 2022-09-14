/*
 * Lightweight AML Interpreter
 * Copyright (C) 2018-2022 The lai authors
 */

#pragma once

#include <stdarg.h>

// LAI internal header

void *lai_calloc(word_t, word_t);
word_t lai_strlen(const char *);
char *lai_strcpy(char *, const char *);
int lai_strcmp(const char *, const char *);

void lai_vsnprintf(char *, word_t, const char *, va_list);
void lai_snprintf(char *, word_t, const char *, ...);

#define LAI_MIN(x, y) ((x) > (y) ? (y) : (x))
