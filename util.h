#ifndef _UTIL_H_
#define _UTIL_H_

uint32_t getCpuCycles();

int32_t positive_modulo(int32_t i, int32_t n);

uint32_t positive_modulo_u(uint32_t i, uint32_t n);

int32_t constrain(int val, int max, int min);

#endif // #ifndef _UTIL_H_