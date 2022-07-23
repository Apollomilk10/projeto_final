/*
 * util.h
 *
 *  Created on: Jun 23, 2022
 *      Author: ea871
 */

#ifndef UTIL_H_
#define UTIL_H_

void delay_10us (unsigned int multiplos);

char *ttoa (uint32_t seconds, char *string);

char *itoa (int n, char *string, char b);

void s2dhms (uint32_t segundos, uint32_t *DD, uint32_t *HH, uint32_t *MM, uint32_t *SS);

void dhms2s (uint8_t DD, uint8_t HH, uint8_t MM, uint8_t SS, uint32_t *segundos);

char *ftoa(float valor, char* string);

int atoOp(char *string, char *op, float *fvalor, int *ivalor, int *efloat);



#endif /* UTIL_H_ */
