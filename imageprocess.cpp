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


void imageprocess::rotar(uchar * imgO, uchar * imgD, float angle)
{
    short cw;
    int sin1000, cos1000;

    sin1000 = sin(angle)*1000.;
    cos1000 = cos(angle)*1000.;

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

void imageprocess::zoom(uchar * imgO, uchar * imgD, float s, int dx, int dy)
{
    short cw;
    int sInt;
    int ampliar;


    if(s>=1)
    {
        sInt = s;
        ampliar = 1;
    }
    else
    {
        sInt = rint(1./s);
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



void imageprocess::volteoHorizontal(uchar * imgO, uchar * imgD)
{

    asm volatile(
        "\n\t"


        :
        : "m" (imgO),	"m" (imgD)
        : "memory"
    );
  
}

void imageprocess::volteoVertical(uchar * imgO, uchar * imgD)
{

    asm volatile(
        "\n\t"


        :
        : "m" (imgO),	"m" (imgD)
        : "memory"
    );

}


void imageprocess::iluminarLUT(uchar * tablaLUT, uchar gW)
{
    asm volatile(
        "\n\t"


        :
        : "m" (tablaLUT), "m" (gW)
        : "memory"
    );

}

void imageprocess::oscurecerLUT(uchar * tablaLUT, uchar gB)
{
    asm volatile(
        "\n\t"


        :
        : "m" (tablaLUT), "m" (gB)
        : "memory"
    );

}


void imageprocess::iluminarLUTMejorado(uchar * tablaLUT, uchar gW)
{
    asm volatile(
        "\n\t"


        :
        : "m" (tablaLUT), "m" (gW)
        : "memory"
    );

}



void imageprocess::oscurecerLUTMejorado(uchar * tablaLUT, uchar gB)
{
    asm volatile(
        "\n\t"


        :
        : "m" (tablaLUT), "m" (gB)
        : "memory"
    );

}



void imageprocess::aplicarTablaLUT(uchar * tablaLUT, uchar * imgO, uchar * imgD)
{
    asm volatile(
        "\n\t"


        :
        : "m" (imgO), "m" (tablaLUT), "m" (imgD)
        : "memory"
    );

}

