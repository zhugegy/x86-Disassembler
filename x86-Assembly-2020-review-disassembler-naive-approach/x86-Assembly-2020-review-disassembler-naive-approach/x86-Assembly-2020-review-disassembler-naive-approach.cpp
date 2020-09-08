#define _CRT_SECURE_NO_WARNINGS
// 20200906-Simple-ASM-Disassembler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

#include <iomanip>   // for std::setw()

#define HEX_NUM_STR_MAX_LEN 10

/* === Origin ASM === */
/*
B8 90 78         MOV AX, 7890
01 C1            ADD CX, AX
01 C3            ADD BX, AX
89 C1            MOV CX, AX
BE 78 56         MOV SI, 5678
BC 78 56         MOV SP, 5678
89 D9            MOV CX, BX
89 C3            MOV BX, AX
D0 E0            SHL AL, 1
A2 D8 99         MOV [99D8], AL
80 3E D4 99 00   CMP BYTE PTR [99D4], 00
81 EA 78 56      SUB DX, 5678
81 C7 78 56      ADD DI, 5678
29 D9            SUB CX, BX
29 C3            SUB BX, AX
D0 E0            SHL AL, 1
D0 E0            SHL AL, 1
A2 D2 99         MOV [99D2], AL
80 3E D4 99 00   CMP BYTE PTR [99D4], 00
75 24            JNZ 015B
A2 D8 99         MOV [99D8], AL
0A C9            OR CL, CL
*/

/* === Registers === */
/*
ax 000
cx 001
dx 010 ?
bx 011
sp 100
bp 101 ?
si 110
di 111 c

al 0
cl 1
dl 2 c
bl 3 c
ah 4 c
ch 5 c
dh 6 c
bh 7 c
*/

/* ====== Disassembling Group A ====== */
/*
B8 90 78         MOV AX, 7890     1011 1 ==  000 imm(16 bits)
BE 78 56         MOV SI, 5678     1011 1 ==  110 imm(16 bits)
BC 78 56         MOV SP, 5678     1011 1 ==  100 imm(16 bits)

89 C1            MOV CX, AX       1000 1001 11 ==  00-0   001
89 D9            MOV CX, BX       1000 1001 11 ==  01-1   001
89 C3            MOV BX, AX       1000 1001 11 ==  00-0   011

A2 D8 99         MOV [99D8], AL   1010 0010 ==  imm(16 bits)
A2 D2 99         MOV [99D2], AL
A2 D8 99         MOV [99D8], AL
*/

/* ====== Disassembling Group B ====== */
/*
01 C1            ADD CX, AX       0000 0001 11 ==   00-0 001
01 C3            ADD BX, AX       0000 0001 11 ==   00-0 011

29 D9            SUB CX, BX       0010 1001 11 ==   01-1 001
29 C3            SUB BX, AX       0010 1001 11 ==   00-0 011

81 C7 78 56      ADD DI, 5678     1000 0001 11 = 00 0 ==  111   imm(16 bits)

81 EA 78 56      SUB DX, 5678     1000 0001 11 = 10 1 ==  010   imm(16 bits)
*/

/* ====== Disassembling Group C ====== */
/*
D0 E0            SHL AL, 1        1101 0000 1110 0 ==  000
                                 (1101 0000 1110 0 ==  011)  SHL BL, 1
                                 (1101 0000 1110 0 ==  100)  SHL AH, 1
D0 E0            SHL AL, 1
D0 E0            SHL AL, 1

80 3E D4 99 00   CMP BYTE PTR [99D4], 00       1000 0000 0011 1110  ==  imm(16 bits)  imm(8 bits)
80 3E D4 99 00   CMP BYTE PTR [99D4], 00       1000 0000 0011 1110  ==  imm(16 bits)  imm(8 bits)

75 24            JNZ 015B         0111 0101 ==  imm(8 bits)      (addr + imm + 2)

0A C9            OR CL, CL        0000 1010 11 ==  00-1 001
                                 (0000 1000 11 ==  00-0 000) OR AL, AL
*/

unsigned char get_first_4_bits(unsigned char nOpcode);

std::string get_half_reg_name(int nValue);
std::string get_reg_name(int nValue);
std::string get_reg_name_reg_indirect_addressing(int nValue);

int start_with_0000(unsigned char* pBuffer, int nIndex, std::string& strAsm);
int start_with_0010(unsigned char* pBuffer, int nIndex, std::string& strAsm);
int start_with_0111(unsigned char* pBuffer, int nIndex, std::string& strAsm);
int start_with_1000(unsigned char* pBuffer, int nIndex, std::string& strAsm);
int start_with_1010(unsigned char* pBuffer, int nIndex, std::string& strAsm);
int start_with_1011(unsigned char* pBuffer, int nIndex, std::string& strAsm);
int start_with_1101(unsigned char* pBuffer, int nIndex, std::string& strAsm);



int main()
{
    std::cout << "Target file's size: ";
    FILE* pFile;
    pFile = fopen("TEST.COM", "rb");  // r for read, b for binary
    fseek(pFile, 0, SEEK_END);
    int size = ftell(pFile);
    std::cout << size << "\r\n\r\n";
    fclose(pFile);

    unsigned char* pBuffer = new unsigned char[size];
    pFile = fopen("TEST.COM", "rb");  // r for read, b for binary
    fread(pBuffer, size, 1, pFile);

    for (int i = 0; i < size; i++)
    {
        std::string strAsm;
        int offset = 0;

        switch (get_first_4_bits(pBuffer[i]))
        {
        case 0x0:
            offset = start_with_0000(pBuffer, i, strAsm);
            break;
        case 0x2:
            offset = start_with_0010(pBuffer, i, strAsm);
            break;
        case 0x7:
            offset = start_with_0111(pBuffer, i, strAsm);
            break;
        case 0x8:
            offset = start_with_1000(pBuffer, i, strAsm);
            break;
        case 0xa:
            offset = start_with_1010(pBuffer, i, strAsm);
            break;
        case 0xb:
            offset = start_with_1011(pBuffer, i, strAsm);
            break;
        case 0xd:
            offset = start_with_1101(pBuffer, i, strAsm);
            break;
        default:
            break;
        }

        std::string strAsmOpCode = "";
        for (int j = 0; j <= offset; j++)
        {
            char buff[10];
            sprintf(buff, "%02X ", pBuffer[i + j]); // prints a series of bytes
            // printf("%02X ", pBuffer[i+j]); // prints a series of bytes
            strAsmOpCode += buff;
        }

        std::cout << std::setw(18) << std::left << strAsmOpCode << strAsm << "\r\n";
        i += offset;
    }

    fclose(pFile);

    system("pause");
}

unsigned char get_first_4_bits(unsigned char nOperand)
{
    return nOperand >> 4;
}

std::string get_half_reg_name(int nValue)
{
    switch (nValue)
    {
    case 0x0:
        return "AL";
        break;
    case 0x1:
        return "CL";
        break;
    case 0x2:
        return "DL";
        break;
    case 0x3:
        return "BL";
        break;
    case 0x4:
        return "AH";
        break;
    case 0x5:
        return "CH";
        break;
    case 0x6:
        return "DH";
        break;
    case 0x7:
        return "BH";
        break;
    default:
        break;
    }

    return "NULL-get_half_reg_name";
}

std::string get_reg_name(int nValue)
{
    switch (nValue)
    {
    case 0x0:
        return "AX";
        break;
    case 0x1:
        return "CX";
        break;
    case 0x2:
        return "DX";
        break;
    case 0x3:
        return "BX";
        break;
    case 0x4:
        return "SP";
        break;
    case 0x5:
        return "BP";
        break;
    case 0x6:
        return "SI";
        break;
    case 0x7:
        return "DI";
        break;
    default:
        break;
    }

    return "NULL-get_reg_name";
}

std::string get_reg_name_reg_indirect_addressing(int nValue)
{
    switch (nValue)
    {
    case 0x0:
        return "BX+SI";
        break;
    case 0x1:
        return "BX+DI";
        break;
    case 0x2:
        return "BP+SI";
        break;
    case 0x3:
        return "BP+DI";
        break;
    case 0x4:
        return "SI";
        break;
    case 0x5:
        return "DI";
        break;
    case 0x6:
        return "BP";
        break;
    case 0x7:
        return "BX";
        break;
    default:
        break;
    }

    return "NULL-get_reg_name";
}

/*
DONE B8 90 78         MOV AX, 7890
DONE 01 C1            ADD CX, AX
DONE 01 C3            ADD BX, AX
DONE 89 C1            MOV CX, AX
DONE BE 78 56         MOV SI, 5678
DONE BC 78 56         MOV SP, 5678
DONE 89 D9            MOV CX, BX
DONE 89 C3            MOV BX, AX
DONE D0 E0            SHL AL, 1
DONE A2 D8 99         MOV [99D8], AL
DONE 80 3E D4 99 00   CMP BYTE PTR [99D4], 00
DONE 81 EA 78 56      SUB DX, 5678
DONE 81 C7 78 56      ADD DI, 5678
DONE 29 D9            SUB CX, BX
DONE 29 C3            SUB BX, AX
DONE D0 E0            SHL AL, 1
DONE D0 E0            SHL AL, 1
DONE A2 D2 99         MOV [99D2], AL
DONE 80 3E D4 99 00   CMP BYTE PTR [99D4], 00
DONE 75 24            JNZ 015B
DONE A2 D8 99         MOV [99D8], AL
DONE 0A C9            OR CL, CL
*/

int start_with_0000(unsigned char* pBuffer, int nIndex, std::string& strAsm)
{
    unsigned char last4Bits = pBuffer[nIndex] & 0xf;

    // ADD Reg16, Reg16
    // 01 C1            ADD CX, AX       0000 0001 11 ==   00-0 001
    // 01 C3            ADD BX, AX       0000 0001 11 ==   00-0 011
    if (last4Bits == 0x1 && pBuffer[nIndex + 1] >> 6 == 0x3)
    {
        std::string strSource = get_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        std::string strDest = get_reg_name(pBuffer[nIndex + 1] & 0x7);
        strAsm = "ADD " + strDest + ", " + strSource;

        return 1;
    }

    // OR Reg8, Reg8
    // 0A C9            OR CL, CL        0000 1010 11 ==  00-1 001
    if (last4Bits == 0xa && pBuffer[nIndex + 1] >> 6 == 0x3)
    {
        std::string strSource = get_half_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        std::string strDest = get_half_reg_name(pBuffer[nIndex + 1] & 0x7);
        strAsm = "OR " + strDest + ", " + strSource;

        return 1;
    }

    return 0;
}

int start_with_0010(unsigned char* pBuffer, int nIndex, std::string& strAsm)
{
    unsigned char last4Bits = pBuffer[nIndex] & 0xf;

    // SUB Reg16, Reg16
    // 29 D9            SUB CX, BX       0010 1001 11 == 01 - 1 001
    // 29 C3            SUB BX, AX       0010 1001 11 == 00 - 0 011
    if (last4Bits == 1 + 8 && pBuffer[nIndex + 1] >> 6 == 0x3)
    {
        std::string strSource = get_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        std::string strDest = get_reg_name(pBuffer[nIndex + 1] & 0x7);
        strAsm = "SUB " + strDest + ", " + strSource;

        return 1;
    }

    return 0;
}

int start_with_0111(unsigned char* pBuffer, int nIndex, std::string& strAsm)
{
    unsigned char last4Bits = pBuffer[nIndex] & 0xf;

    // JNZ imm8
    // 75 24            JNZ 015B         0111 0101 ==  imm(8 bits)      (addr + imm + 2)
    if (last4Bits == 1 + 4)
    {
        int nJmpOffset = (0x100 + nIndex) + pBuffer[nIndex + 1] + 2;

        std::string strTmp = "";
        char buff[HEX_NUM_STR_MAX_LEN];
        sprintf(buff, "%04X", nJmpOffset);
        strAsm = "JNZ " + strTmp + buff;

        return 1;
    }

    return 0;
}

int start_with_1000(unsigned char* pBuffer, int nIndex, std::string& strAsm)
{
    unsigned char last4Bits = pBuffer[nIndex] & 0xf;

    // CMP BYTE PTR [imm16], imm8
    // 80 3E D4 99 00   CMP BYTE PTR[99D4], 00       1000 0000 0011 1110 == imm(16 bits)  imm(8 bits)
    // 80 3E D4 99 00   CMP BYTE PTR[99D4], 00       1000 0000 0011 1110 == imm(16 bits)  imm(8 bits)
    if (last4Bits == 0x0 && pBuffer[nIndex + 1] == 0x3E)
    {
        std::string strTmp = "";
        char buffSource[HEX_NUM_STR_MAX_LEN];
        sprintf(buffSource, "%02X", pBuffer[nIndex + 4]);

        char buffDest[HEX_NUM_STR_MAX_LEN];
        sprintf(buffDest, "%02X%02X", pBuffer[nIndex + 3], pBuffer[nIndex + 2]);
        strAsm = "CMP BYTE PTR [" + strTmp + buffDest;
        strAsm += "],";
        strAsm += buffSource;

        return 4;
    }

    // SUB Reg16, imm16
    // 81 EA 78 56      SUB DX, 5678     1000 0001 11 = 10 1 ==  010   imm(16 bits)
    // 
    // ADD Reg16, imm16
    // 81 C7 78 56      ADD DI, 5678     1000 0001 11 = 00 0 == 111   imm(16 bits)
    if (last4Bits == 0x1 && pBuffer[nIndex + 1] >> 6 == 3)
    {
        // SUB Reg16, imm16
        if ((pBuffer[nIndex + 1] & (1 + 2 + 4 + 8 + 16 + 32)) >> 3 == 1 + 4)
        {
            std::string strDest = get_reg_name(pBuffer[nIndex + 1] & 0x7);

            char buff[HEX_NUM_STR_MAX_LEN];
            sprintf(buff, "%02X%02X", pBuffer[nIndex + 3], pBuffer[nIndex + 2]);

            strAsm = "SUB " + strDest + ", " + buff;

            return 3;
        }

        // ADD Reg16, imm16
        if ((pBuffer[nIndex + 1] & (1 + 2 + 4 + 8 + 16 + 32)) >> 3 == 0x0)
        {
            std::string strDest = get_reg_name(pBuffer[nIndex + 1] & 0x7);

            char buff[HEX_NUM_STR_MAX_LEN];
            sprintf(buff, "%02X%02X", pBuffer[nIndex + 3], pBuffer[nIndex + 2]);

            strAsm = "ADD " + strDest + ", " + buff;

            return 3;
        }


        return 0;
    }

    // MOV Reg16, Reg16
    // 89 C1            MOV CX, AX       1000 1001 11 == 00 - 0   001
    // 89 D9            MOV CX, BX       1000 1001 11 == 01 - 1   001
    // 89 C3            MOV BX, AX       1000 1001 11 == 00 - 0   011
    if (last4Bits == 0x9 && pBuffer[nIndex + 1] >> 6 == 0x3)
    {
        std::string strSource = get_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        std::string strDest = get_reg_name(pBuffer[nIndex + 1] & 0x7);
        strAsm = "MOV " + strDest + ", " + strSource;

        return 1;
    }

    // MOV Reg16, [imm16]
    // 8B 0E 90 78 -> 8B 00-00 1-110 xx xx     MOV     CX, [7890]
    // 8B 3E 90 78 -> xx 00-11 1-110 xx xx     MOV     DI, [7890]
    // 8B 36 90 78 -> xx 00-11 0-110 xx xx     MOV     SI, [7890]
    // 8B 06 90 78 -> xx 00-00 0-110 xx xx     MOV     AX, [7890]
    if (last4Bits == 0xb && pBuffer[nIndex + 1] >> 6 == 0x0 && (pBuffer[nIndex + 1] & 0x7) == 0x6)
    {
        std::string strDest = get_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        char buff[HEX_NUM_STR_MAX_LEN];
        sprintf(buff, "%02X%02X", pBuffer[nIndex + 3], pBuffer[nIndex + 2]);

        strAsm = "MOV " + strDest + ", [" + buff + "]";

        return 3;
    }

    // MOV Reg16, [Reg16(addr-only)]
    // 8B 07 -> xx 00-00 0-111       MOV     AX, [BX]
    // 8B 04 -> xx 00-00 0-100       MOV     AX, [SI]
    // 8B 05 -> xx 00-00 0-101       MOV     AX, [DI]
    // 8B 3D -> xx 00-11 1-101       MOV     DI, [DI]
    if (last4Bits == 0xb && pBuffer[nIndex + 1] >> 6 == 0x0 && (pBuffer[nIndex + 1] & 0x7) != 0x6)
    {
        std::string strDest = get_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        std::string strSource = get_reg_name_reg_indirect_addressing(pBuffer[nIndex + 1] & 0x7);

        strAsm = "MOV " + strDest + ", [" + strSource + "]";

        return 1;
    }

    // MOV Reg16, [BP + imm8]
    // 8B 46 00 -> xx 01-00 0-110 xx       MOV     AX, [BP + 00]
    // 8B 7E 00 -> xx 01-11 1-110 xx       MOV     DI, [BP + 00]
    /*
    0B27:0100 8B4600        MOV     AX,[BP+00]
    0B27:0103 8B4604        MOV     AX,[BP+04]
    0B27:0106 8B4640        MOV     AX,[BP+40]
    0B27:0109 8B4644        MOV     AX,[BP+44]
    0B27:010C 8B467F        MOV     AX,[BP+7F]
    0B27:010F 8B868000      MOV     AX,[BP+0080]  ;boundary detected
    */
    if (last4Bits == 0xb && pBuffer[nIndex + 1] >> 6 == 0x1 && (pBuffer[nIndex + 1] & 0x7) == 0x6)
    {
        std::string strDest = get_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        std::string strSource = "BP";
        char buff[HEX_NUM_STR_MAX_LEN];
        sprintf(buff, "%02X", pBuffer[nIndex + 2]);

        strAsm = "MOV " + strDest + ", [" + strSource + " + " + buff + "]";

        return 2;
    }

    // MOV Reg16, [Reg16(addr-only) + imm8]
    // 8B 47 04 -> xx 01-00 0-111 xx       MOV     AX, [BX + 04]
    // 8B 47 08 ->                08       MOV     AX, [BX + 08]
    // 8B 7F 04 -> xx 01-11 1-111 xx       MOV     DI, [BX + 04]
    // 8B 7C 04 -> xx 01-11 1-100 xx       MOV     DI, [SI + 04]
    // 8B 7E 04 -> xx 01-11 1-110 xx       MOV     DI, [BP + 04]
    // 8B 4E 04 -> xx 01-00 1-110 xx       MOV     CX, [BP + 04]
    if (last4Bits == 0xb && pBuffer[nIndex + 1] >> 6 == 0x1)
    {
        std::string strDest = get_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        std::string strSource = get_reg_name_reg_indirect_addressing(pBuffer[nIndex + 1] & 0x7);
        char buff[HEX_NUM_STR_MAX_LEN];
        sprintf(buff, "%02X", pBuffer[nIndex + 2]);

        strAsm = "MOV " + strDest + ", [" + strSource + " + " + buff + "]";

        return 2;
    }

    // MOV Reg16, [Reg16(addr-only) + imm16]
    // 8B 85 90 78  xx 10 - 00 0 - 101 xx xx   MOV     AX, [DI + 7890]
    // 8B 8D 90 78  xx 10 - 00 1 - 101 xx xx   MOV     CX, [DI + 7890]
    // 8B 8C 90 78  xx 10 - 00 1 - 100 xx xx   MOV     CX, [SI + 7890]
    // 8B 8F 90 78  xx 10 - 00 1 - 111 xx xx   MOV     CX, [BX + 7890]
    // 8B 8E 90 78  xx 10 - 00 1 - 110 xx xx   MOV     CX, [BP + 7890]
    // 8B 97 90 78  xx 10 - 01 0 - 111 xx xx   MOV     DX, [BX + 7890]
    if (last4Bits == 0xb && pBuffer[nIndex + 1] >> 6 == 0x2)
    {
        std::string strDest = get_reg_name((pBuffer[nIndex + 1] & 0x3f) >> 3);
        std::string strSource = get_reg_name_reg_indirect_addressing(pBuffer[nIndex + 1] & 0x7);
        char buff[HEX_NUM_STR_MAX_LEN];
        sprintf(buff, "%02X%02X", pBuffer[nIndex + 3], pBuffer[nIndex + 2]);

        strAsm = "MOV " + strDest + ", [" + strSource + " + " + buff + "]";

        return 3;
    }


    return 0;
}


int start_with_1010(unsigned char* pBuffer, int nIndex, std::string& strAsm)
{
    unsigned char last4Bits = pBuffer[nIndex] & 0xf;

    // MOV Acc16, [imm16]
    // A19078        MOV     AX, [7890]
    if (last4Bits == 0x1)
    {
        std::string strDest = "AX";
        //std::string strTmp = "";

        char buff[HEX_NUM_STR_MAX_LEN];
        sprintf(buff, "%02X%02X", pBuffer[nIndex + 2], pBuffer[nIndex + 1]);
        strAsm = "MOV " + strDest + ", [" + buff + "]";

        return 2;
    }

    // MOV [imm16], Acc8
    // A2 D8 99         MOV[99D8], AL   1010 0010 == imm(16 bits)
    // A2 D2 99         MOV[99D2], AL
    // A2 D8 99         MOV[99D8], AL
    if (last4Bits == 0x2)
    {
        std::string strSource = "AL";
        std::string strTmp = "";

        char buff[HEX_NUM_STR_MAX_LEN];
        sprintf(buff, "%02X%02X", pBuffer[nIndex + 2], pBuffer[nIndex + 1]);
        strAsm = "MOV [" + strTmp + buff;
        strAsm += "]," + strSource;

        return 2;
    }

    return 0;
}

int start_with_1011(unsigned char* pBuffer, int nIndex, std::string& strAsm)
{
    unsigned char last4Bits = pBuffer[nIndex] & 0xf;

    // MOV Reg16, imm16
    // B8 90 78         MOV AX, 7890     1011 1 == 000 imm(16 bits)
    // BE 78 56         MOV SI, 5678     1011 1 == 110 imm(16 bits)
    // BC 78 56         MOV SP, 5678     1011 1 == 100 imm(16 bits)
    if (last4Bits >> 3 == 0x1)
    {
        std::string strDest = get_reg_name(last4Bits & 0x7);

        char buff[HEX_NUM_STR_MAX_LEN];
        sprintf(buff, "%02X%02X", pBuffer[nIndex + 2], pBuffer[nIndex + 1]);
        strAsm = "MOV " + strDest + ", " + buff;

        return 2;
    }

    return 0;
}

int start_with_1101(unsigned char* pBuffer, int nIndex, std::string& strAsm)
{
    unsigned char last4Bits = pBuffer[nIndex] & 0xf;

    // SHL Reg8, 1
    // D0 E0            SHL AL, 1        1101 0000 1110 0 == 000
    //                                  (1101 0000 1110 0 == 011)  SHL BL, 1
    //                                  (1101 0000 1110 0 == 100)  SHL AH, 1
    if (last4Bits == 0x0 && pBuffer[nIndex + 1] >> 3 == 4 + 8 + 16)
    {
        std::string strSource = get_half_reg_name(pBuffer[nIndex + 1] & 0x7);
        strAsm = "SHL " + strSource + ", " + "1";

        return 1;
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
