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
	int sin1000, cos1000;

	sin1000 = sin(angle) * 1000.;
	cos1000 = cos(angle) * 1000.;

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
	: "m" (imgO), "m" (imgD), "m" (angle), "m" (cw), "m" (sin1000), "m" (cos1000)
	: "memory"

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
	"add $479*640, %%rsi\n\t"

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
	"mov %0, %%rbx\n\t"					// Copia en %rbx la dirección de la tabla LUT
	"xor %%rcx, %%rcx\n\t"				// Inicializa el contador los bucles %rcx a 0 (g). Rango [0, 255]
	"xor %%rdx, %%rdx\n\t"				// Inicializa los 8 bytes de %rdx a 0 para almacenar en el byte bajo 'gW'
	"mov $255, %%r8\n\t"				// Inicializa &r8 con la constante 255 para realizar la multiplicacion g * 255
 	"mov %1, %%dl\n\t"					// Copia 'gW' en el byte bajo de %rdx (%dl)
 	// PRIMER BUCLE: Calcula el nuevo nivel de gris (segun ecuacion*) de los píxeles de nivel de gris inferior a 'gW'
   	// *gn = (g * 255) / gW
	"bucleIL:\n\t"
		"mov %%rcx, %%rax\n\t"			// Copia 'g' a %rax para realizar posteriormente la división por 'gW' directamente
		"mul %%r8b\n\t"					// Resultado parcial 1 (%al) = g * 255
		"cmp $0, %%dl\n\t"				// Comprueba que 'gW' no sea 0 para evitar una SIGFPE por division por 0
		"je divCeroIL\n\t"
		"div %%dl\n\t"					// gW != 0 -> gn = 255 -> %al
		"jmp cargarResIL\n\t"
		"divCeroIL:\n\t"
  			"mov $255, %%al\n\t"		// gW == 0 -> El resultado es el nivel de gris más blanco (255)
		"cargarResIL:\n\t"
			"mov %%al, (%%rbx, %%rcx)\n\t"	// Copia el nuevo píxel calculado a la tabla LUT | LUT[g] = gn = %al
		"inc %%rcx\n\t"
		"cmp %%rdx, %%rcx\n\t"			// Control del primer bucle (g < gW)
		"jl bucleIL\n\t"
  	"mov %%rdx, %%rcx\n\t"
	// SEGUNUDO BUCLE: Modifica los píxeles de nivel de gris superior o igual a 'gW' a completamente blanco (255)
	"bucleIL2:\n\t"
		"movb $255, (%%rbx, %%rcx)\n\t"	// LUT[g] = gn = 255
		"inc %%rcx\n\t"
		"cmp $256, %%rcx\n\t"			// Control del segundo bucle (g < 256)
		"jl bucleIL2\n\t"

	:
	: "m" (tablaLUT), "m" (gW)
	: "%rax", "%rbx", "%rcx", "%rdx", "r8", "memory"
	);

}

void imageprocess::oscurecerLUT(uchar *tablaLUT, uchar gB) {

	asm volatile(
	"mov %0, %%rbx\n\t"					// Copia en %rbx la dirección de la tabla LUT
	"xor %%rcx, %%rcx\n\t"				// Inicializa el contador los bucles %rcx a 0 (g). Rango [0, 255]
 	// PRIMER BUCLE: Modifica los píxeles de nivel de gris inferior o igual a 'gB' a completamente negro (0)
	"bucleOL:\n\t"
		"movb $0, (%%rbx, %%rcx)\n\t"	// LUT[g] = gn = 0
		"inc %%rcx\n\t"
		"cmp %1, %%cl\n\t"				// Control del primer bucle (g <= gB)
		"jle bucleOL\n\t"
	"mov $255, %%r9\n\t"				// Almacena en %%r9 la constante 255 para realizar la multiplicacion (g - gB) * 255
	"xor %%rcx, %%rcx\n\t"
	"mov %1, %%cl\n\t"					// Reinicializa el contador del bucle %rcx a gB + 1
	"inc %%cl\n\t"
 	// SEGUNDO BUCLE: Calcula el nuevo nivel de gris (segun ecuacion*) de los píxeles de nivel de gris superior a 'gB'
   	// *gn = ((g - gB) * 255)) / (255 - gB)
	"bucleOL2:\n\t"
		"mov $255, %%r8\n\t"			// Almacena en %%r8 la constante 255 para realizar la resta 255 - gB
		"sub %1, %%r8b\n\t"				// Resultado parcial 1 (%r8b) =  255 - gB
		"mov %%rcx, %%rax\n\t"
		"sub %1, %%al\n\t"				// Resultado parcial 2 (%al) =  g - gB
		"mul %%r9b\n\t"					// Resultado parcial 3 (%al) =  (g - gB) * 255
		"cmp $0, %%r8\n\t"				// Comprueba que 'gB' no sea 255 para evitar una SIGFPE por division por 0 (255 - gB)
		"je divCeroOL\n\t"
		"div %%r8b\n\t"					// gB != 255 -> Resultado final gn = ResultadoParcial3 / ResultadoParcial1
		"jmp cargarResOL\n\t"
		"divCeroOL:\n\t"
  			"mov $0, %%al\n\t"			// gB != 255 -> gn = 0 -> %al
		"cargarResOL:\n\t"
			"mov %%al, (%%rbx, %%rcx)\n\t"	// Copia el nuevo píxel calculado a la tabla LUT | LUT[g] = gn
		"inc %%rcx\n\t"
		"cmp $256, %%rcx\n\t"			// Contol del segundo bucle (g < 256)
		"jl bucleOL2\n\t"

	:
	: "m" (tablaLUT), "m" (gB)
	: "%rax", "%rbx", "%rcx", "r8", "r9", "memory"
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
	"mov %0, %%rbx\n\t"					// Copia en %rbx la dirección de la tabla LUT
	"mov %1, %%rsi\n\t"					// Copia en %rsi la dirección de la imagen de origen
	"mov %2, %%rdi\n\t"					// Copia en %rdi la dirección de la imagen de destino
	"xor %%rcx, %%rcx\n\t"				// Inicializa el contador del bucle %rcx a 0. Rango [0, w*h) (cada pixel de la imagen)
	"xor %%r8, %%r8\n\t"				// Inicializa los 8 bytes de %r8 a 0 para almacenar en el byte bajo cada pixel tratado
 	// Aplica la tabla LUT a cada pixel de la imagen destino y almacena el resultado en la imagen destino | imgD[p] = LUT[p]
	"bucleAL:\n\t"
		"mov (%%rsi), %%r8b\n\t"		// Almacena cada pixel origen
		"mov (%%rbx, %%r8), %%r8b\n\t"	// Halla su equivalente en el destino por la tabla LUT
		"mov %%r8b, (%%rdi)\n\t"		// Copia el pixel calculado a cada pixel destino
		"inc %%rsi\n\t"
		"inc %%rdi\n\t"
		"inc %%rcx\n\t"
		"cmp $640*480, %%rcx\n\t"		// Control del bucle (p < w*h) (cada pixel de la imagen)
		"jl bucleAL\n\t"

	:
	: "m" (tablaLUT), "m" (imgO), "m" (imgD)
	: "%rbx", "%rcx", "%rsi", "%rdi", "r8", "memory"
	);

}

