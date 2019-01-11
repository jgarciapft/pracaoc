/***************************************************************************
 *   Copyright (C) 2018 by pilar   *
 *   pilarb@unex.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "imageprocess.h"


void imageprocess::rotar(uchar *imgO, uchar *imgD, float angle) {
	short cw;
	int sin, cos;
	int cO, fO, cD, fD;

	asm volatile(
	"fstcw %3\n\t"
	"fwait\n\t"
	"mov %3, %%ax\n\t"
	"and $0xf3ff, %%ax\n\t"
	"or $0x0c00, %%ax\n\t"
	"push %%rax\n\t"
	"fldcw (%%rsp)\n\t"
	"pop %%rax\n\t"


	//Insertar aquí el código del procedimiento

	"fldcw %3\n\t"

	:
	: "m" (imgO), "m" (imgD), "m" (angle), "m" (cw), "m" (sin), "m" (cos), "m" (cO), "m" (fO), "m" (cD), "m" (fD)
	: "%rsi", "%rdi", "%rcx", "%rdx", "memory"

	);
}

void imageprocess::zoom(uchar *imgO, uchar *imgD, float s, int dx, int dy) {
	short cw;
	int sInt;
	int ampliar;


	if (s >= 1) {
		sInt = s;
		ampliar = 1;
	} else {
		sInt = rint(1. / s);
		ampliar = 0;
	}

	asm volatile(
	"fstcw %5\n\t"
	"fwait\n\t"
	"mov %5, %%ax\n\t"
	"and $0xf3ff, %%ax\n\t"
	"or $0x0c00, %%ax\n\t"
	"push %%rax\n\t"
	"fldcw (%%rsp)\n\t"
	"pop %%rax\n\t"

	//Insertar aquí el código del procedimiento


	"fldcw %5\n\t"

	:
	: "m" (imgO), "m" (imgD), "m" (s), "m" (dx), "m" (dy), "m" (cw), "m" (sInt), "m" (ampliar)
	: "memory"

	);


}


void imageprocess::volteoHorizontal(uchar *imgO, uchar *imgD) {

	asm volatile(
	"mov %0, %%rsi\n\t"					// Copia en %rsi la direccion de la imagen origen imgO
	"mov %1, %%rdi\n\t"					// Copia en %rdi la direccion de la imagen destino imgD
	"add $639, %%rsi\n\t"

	"xor %%rcx, %%rcx\n\t"				// Inicializa el contador del bucle externo %rcx a 0 (cada fila)
	"bucleFilasVH:\n\t"
		"xor %%rdx, %%rdx\n\t"			// Inicializa el contador del bucle interno %rdx a 0 (cada columna)
		"bucleColumnasVH:\n\t"
  			// Copia cada pixel calculado de imgO en imgD
            "mov (%%rsi), %%r8b\n\t"
            "mov %%r8b, (%%rdi)\n\t"

            "dec %%rsi\n\t"
            "inc %%rdi\n\t"

			"inc %%rdx\n\t"				// Incrementa el contador del bucle interno %rdx
			"cmp $640, %%rdx\n\t"		// Control de iteracion del bucle interno
			"jl bucleColumnasVH\n\t"
		"add $1280, %%rsi\n\t"

		"inc %%rcx\n\t"					// Incrementa el contador del bucle externo %rcx
		"cmp $480, %%rcx\n\t"			// Control de iteracion del bucle externo
		"jl bucleFilasVH\n\t"

	:
	: "m" (imgO), "m" (imgD)
	: "%rsi", "%rdi", "%rcx", "%rdx", "r8", "memory"
	);

}

void imageprocess::volteoVertical(uchar *imgO, uchar *imgD) {

	asm volatile(
	"mov %0, %%rsi\n\t"					// Copia en %rsi la direccion de la imagen origen imgO
	"mov %1, %%rdi\n\t"					// Copia en %rdi la direccion de la imagen destino imgD
	"add $306560, %%rsi\n\t"			// 479*640 = 306560

	"xor %%rcx, %%rcx\n\t"				// Inicializa el contador del bucle externo %rcx a 0 (cada fila)
	"bucleFilasVV:\n\t"
		"xor %%rdx, %%rdx\n\t"			// Inicializa el contador del bucle interno %rdx a 0 (cada columna)
		"bucleColumnasVV:\n\t"
  			// Copia cada pixel calculado de imgO en imgD
			"mov (%%rsi), %%r8b\n\t"
			"mov %%r8b, (%%rdi)\n\t"

			"inc %%rsi\n\t"
			"inc %%rdi\n\t"

			"inc %%rdx\n\t"				// Incrementa el contador del bucle interno %rdx
			"cmp $640, %%rdx\n\t"		// Control de iteracion del bucle interno
			"jl bucleColumnasVV\n\t"
		"sub $1280, %%rsi\n\t"

		"inc %%rcx\n\t"					// Incrementa el contador del bucle externo %rcx
		"cmp $480, %%rcx\n\t"			// Control de iteracion del bucle externo
		"jl bucleFilasVV\n\t"

	:
	: "m" (imgO), "m" (imgD)
	: "%rsi", "%rdi", "%rcx", "%rdx", "r8", "memory"
	);

}


void imageprocess::iluminarLUT(uchar *tablaLUT, uchar gW) {
	asm volatile(
	"\n\t"


	:
	: "m" (tablaLUT), "m" (gW)
	: "memory"
	);

}

void imageprocess::oscurecerLUT(uchar *tablaLUT, uchar gB) {
	asm volatile(
	"\n\t"


	:
	: "m" (tablaLUT), "m" (gB)
	: "memory"
	);

}


void imageprocess::iluminarLUTMejorado(uchar *tablaLUT, uchar gW) {
	asm volatile(
	"\n\t"


	:
	: "m" (tablaLUT), "m" (gW)
	: "memory"
	);

}


void imageprocess::oscurecerLUTMejorado(uchar *tablaLUT, uchar gB) {
	asm volatile(
	"\n\t"


	:
	: "m" (tablaLUT), "m" (gB)
	: "memory"
	);

}


void imageprocess::aplicarTablaLUT(uchar *tablaLUT, uchar *imgO, uchar *imgD) {
	asm volatile(
	"\n\t"


	:
	: "m" (imgO), "m" (tablaLUT), "m" (imgD)
	: "memory"
	);

}

