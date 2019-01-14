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
	"mov %0, %%rsi\n\t"					// Copia en %rsi la direccion de la imagen origen imgO
	"mov %1, %%rdi\n\t"					// Copia en %rdi la direccion de la imagen destino imgD
	"xor %%r8, %%r8\n\t"				// Inicializa el contador del bucle externo (fD) %r8 a 0 (cada fila)
	"bucleFilasR:\n\t"
 		"xor %%r9, %%r9\n\t"			// Inicializa el contador del nucle interno (cD) %r9 a 0 (cada columna)
 		"bucleColumnasR:\n\t"
			"xor %%rax, %%rax\n\t"
			"mov %4, %%eax\n\t"      	// %rax = sin1000
			"xor %%rcx, %%rcx\n\t"
			"mov %5, %%ecx\n\t"      	// %rcx = cos1000
			"mov %%r8, %%r12\n\t"
			"sub $400, %%r12\n\t"      	// %r12 = fD - 400
			"mov %%r9, %%r13\n\t"
			"sub $400, %%r13\n\t"      	// %r13 = cD - 400

			"mov %%r12, %%r14\n\t"     // %r14 = fD - 400
			"imul %%ecx, %%r14d\n\t"   // %r14 = cos1000 * (fD - 400)
			"imul %%eax, %%r12d\n\t"   // %r12 = sin1000 * (fD - 400)

			"mov %%r13, %%r15\n\t"     // %%r15 = cD - 400
			"imul %%ecx, %%r15d\n\t"   // %r15 = cos1000 * (cD - 400)
			"imul %%eax, %%r13d\n\t"   // %r13 = sin1000 * (cD - 400)

			"add %%r13d, %%r14d\n\t"   // %r14 = sin1000 * (cD - 400) + cos1000 * (fD - 400)
			"sub %%r12d, %%r15d\n\t"   // %r15 = cos1000 * (cD - 400) - sin1000 * (fD - 400)

			"mov $1000, %%rcx\n\t"
			"mov %%r14d, %%eax\n\t"
			"cltd\n\t"
			"idiv %%ecx\n\t"      		// %eax = (sin1000 * (cD - 400) + cos1000 * (fD - 400)) / 1000
			"mov %%eax, %%r10d\n\t"		// %r10 = fO

			"mov %%r15d, %%eax\n\t"
			"cltd\n\t"
			"idiv %%ecx\n\t"      		// %eax = (cos1000 * (cD - 400) - sin1000 * (fD - 400)) / 1000
			"mov %%eax, %%r11d\n\t"		// &r11 = cO

			"add $240, %%r10d\n\t"     // %r10 (fO) += 240
			"add $320, %%r11d\n\t"     // %r11 (cO) += 320

   			// Comprueba la condición de renderizado en ventana (fO >= 0 && fO < 480 && cO >= 0 && cO < 640)
	 		"cmp $0, %%r10\n\t"
	 		"jl elseR\n\t"
	 		"cmp $480, %%r10\n\t"
	 		"jge elseR\n\t"
	 		"cmp $0, %%r11\n\t"
	 		"jl elseR\n\t"
	 		"cmp $640, %%r11\n\t"
	 		"jge elseR\n\t"

			// El píxel está dentro de la ventana. Debe renderizarse
	 		"mov $640, %%rax\n\t"
			"imul %%r10, %%rax\n\t"
			"add %%r11, %%rax\n\t"		// %rax (indiceO) = fO * 640 + cO (Cálculo del índice lineal equivalente)
			"mov (%%rsi, %%rax), %%r11b\n\t"
			"mov %%r11b, (%%rdi)\n\t"	// Copia el píxel origen calculado en el destino
			"jmp sgteIterR\n\t"
	 		"elseR:\n\t"
	 			"movb $0, (%%rdi)\n\t"	// El píxel está fuera de la ventana. Debe representarse con negro (0)
			"sgteIterR:\n\t"
 				"inc %%rdi\n\t"			// Siguiente píxel de la imagen destino
 				"inc %%r9\n\t"
 				"cmp $800, %%r9\n\t"	// Control de iteración del segundo bucle (cD < 800)
 				"jl bucleColumnasR\n\t"
		"inc %%r8\n\t"
		"cmp $800, %%r8\n\t"			// Control de iteración del primer bucle (fD < 800)
		"jl bucleFilasR\n\t"

	"fldcw %3\n\t"

	:
	: "m" (imgO), "m" (imgD), "m" (angle), "m" (cw), "m" (sin1000), "m" (cos1000)
	: "%rax", "%rcx", "%rdx", "%rsi", "%rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "memory"
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

	"mov %0, %%rsi\n\t"					// Copia en %rsi la direccion de la imagen origen imgO
	"mov %1, %%rdi\n\t"					// Copia en %rdi la direccion de la imagen destino imgD
	"xor %%r8, %%r8\n\t"				// Inicializa el contador del bucle externo (fD) %r8 a 0 (cada fila)
	"xor %%r10, %%r10\n\t"				// %r10 = fO
	"bucleFilasZ:\n\t"
		"mov %%r8, %%rax\n\t"
		"add %4, %%eax\n\t"				// %rax = fD + dy
		"cmpl $0, %7\n\t"				// Comprueba si es ampliación o reducción
		"je reducirF_Z\n\t"
		"xor %%edx, %%edx\n\t"
		"idivl %6\n\t"					// Es ampliación
		"jmp almacenarFO_Z\n\t"
		"reducirF_Z:\n\t"
			"imull %6\n\t"				// Es reducción
		"almacenarFO_Z:\n\t"
			"mov %%eax, %%r10d\n\t"		// %r10 (fO) = (fD + dy) # sInt
		"xor %%r9, %%r9\n\t"			// Inicializa el contador del nucle interno (cD) %r9 a 0 (cada columna)
		"xor %%r11, %%r11\n\t"			// %r11 = cO
		"bucleColumnasZ:\n\t"
  			"mov %%r9, %%rax\n\t"
  			"add %3, %%eax\n\t"			// %rax = cD + dx
			"cmpl $0, %7\n\t"			// Comprueba si es ampliación o reducción
			"je reducirC_Z\n\t"
   			"xor %%edx, %%edx\n\t"
  			"idivl %6\n\t"				// Es ampliación
  			"jmp almacenarCO_Z\n\t"
  			"reducirC_Z:\n\t"
	 			"imull %6\n\t"			// Es reducción
			"almacenarCO_Z:\n\t"
   				"mov %%eax, %%r11d\n\t"	// %r11 (cO) = (cD + dx) # sInt

	   		// Comprueba la condición de renderizado en ventana (fO >= 0 && fO < 800 && cO >= 0 && cO < 800)
	   		"cmp $0, %%r10\n\t"
	 		"jl elseZ\n\t"
	 		"cmp $800, %%r10\n\t"
	 		"jge elseZ\n\t"
	 		"cmp $0, %%r11\n\t"
	 		"jl elseZ\n\t"
	 		"cmp $800, %%r11\n\t"
	 		"jge elseZ\n\t"

			// El píxel está dentro de la ventana. Debe renderizarse
			"mov $800, %%rax\n\t"
			"imul %%r10, %%rax\n\t"
			"add %%r11, %%rax\n\t"		// %rax (indiceO) = fO * 800 + cO (Cálculo del índice lineal equivalente)
			"mov (%%rsi, %%rax), %%r11b\n\t"
			"mov %%r11b, (%%rdi)\n\t"	// Copia el píxel origen calculado en el destino
			"jmp sgteIterZ\n\t"
			"elseZ:\n\t"
				"movb $0, (%%rdi)\n\t"	// El píxel está fuera de la ventana. Debe representarse con negro (0)
			"sgteIterZ:\n\t"
				"inc %%rdi\n\t"			// Siguiente píxel de la imagen destino
				"inc %%r9\n\t"
				"cmp $640, %%r9\n\t"	// Control de iteración del segundo bucle (cD < 640)
				"jl bucleColumnasZ\n\t"
		"inc %%r8\n\t"
		"cmp $480, %%r8\n\t"			// Control de iteración del primer bucle (fD < 480)
		"jl bucleFilasZ\n\t"

	"fldcw %5\n\t"

	:
	: "m" (imgO), "m" (imgD), "m" (s), "m" (dx), "m" (dy), "m" (cw), "m" (sInt), "m" (ampliar)
	: "%rax", "%rdx", "%rsi", "%rdi", "r8", "r9", "r10", "r11", "memory"
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
            "cmp $640, %%rdx\n\t"		// Control de iteracion del segundo bucle (c < 640)
            "jl bucleColumnasVH\n\t"
        "add $1280, %%rsi\n\t"

        "inc %%rcx\n\t"					// Incrementa el contador del bucle externo %rcx
        "cmp $480, %%rcx\n\t"			// Control de iteracion del primer bucle (f < 480)
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
			"cmp $640, %%rdx\n\t"		// Control de iteracion del segundo bucle (c < 640)
			"jl bucleColumnasVV\n\t"
		"sub $1280, %%rsi\n\t"

		"inc %%rcx\n\t"					// Incrementa el contador del bucle externo %rcx
		"cmp $480, %%rcx\n\t"			// Control de iteracion del primer bucle (f < 480)
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

