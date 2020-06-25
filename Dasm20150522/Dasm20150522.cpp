// Dasm20150522.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "Dasm.h"

bool SetEnvironment();

bool InitByteArray(BYTE InputArray[], int ArrayLength);
bool FillByteArray(BYTE InuputArray[], int ArrayLength, FILE *pInput);


string szTableA2[0x10][0x10] = {
	/*0*/{"ADD,Eb,Gb", "ADD,Ev,Gv", "ADD,Gb,Eb", "ADD,Gv,Ev", "ADD,AL,Ib", "ADD,rAX,Iz", "PUSH,es", "POP,es", 
	"OR,Eb,Gb", "OR,Ev,Gv", "OR,Gb,Eb", "OR,Gv,Ev", "OR,AL,Ib", "OR,rAX,Iz", "PUSH,cs", "2ESC"},
	/*1*/{"ADC,Eb,Gb", "ADC,Ev,Gv", "ADC,Gb,Eb", "ADC,Gv,Ev", "ADC,AL,Ib", "ADC,rAX,Iz", "PUSH,ss", "POP,ss", 
	"SBB,Eb,Gb", "SBB,Ev,Gv", "SBB,Gb,Eb", "SBB,Gv,Ev", "SBB,AL,Ib", "SBB,rAX,Iz", "PUSH,ds", "POP,ds"},
	/*2*/{"AND,Eb,Gb", "AND,Ev,Gv", "AND,Gb,Eb", "AND,Gv,Ev", "AND,AL,Ib", "AND,rAX,Iz", "SEGES", "DAA", 
	"SUB,Eb,Gb", "SUB,Ev,Gv", "SUB,Gb,Eb", "SUB,Gv,Ev", "SUB,AL,Ib", "SUB,rAX,Iz", "SEGCS", "DAS"},
	/*3*/{"XOR,Eb,Gb", "XOR,Ev,Gv", "XOR,Gb,Eb", "XOR,Gv,Ev", "XOR,AL,Ib", "XOR,rAX,Iz", "SEGSS", "AAA", 
	"CMP,Eb,Gb", "CMP,Ev,Gv", "CMP,Gb,Eb", "CMP,Gv,Ev", "CMP,AL,Ib", "CMP,rAX,Iz", "SEGDS", "AAS"},
	/*4*/{"INC,eAX", "INC,eCX", "INC,eDX", "INC,eBX", "INC,eSP", "INC,eBP", "INC,eSI", "INC,eDI", 
	"DEC,eAX", "DEC,eCX", "DEC,eDX", "DEC,eBX", "DEC,eSP", "DEC,eBP", "DEC,eSI", "DEC,eDI"},
	/*5*/{"PUSH,rAX", "PUSH,rCX", "PUSH,rDX", "PUSH,rBX", "PUSH,rSP", "PUSH,rBP", "PUSH,rSI", "PUSH,rDI", 
	"POP,rAX", "POP,rCX", "POP,rDX", "POP,rBX", "POP,rSP", "POP,rBP", "POP,rSI", "POP,rDI"},
	/*6*/{"PUSHA", "POPA", "BOUND,Gv,Ma", "ARPL,Ew,Gw", "SEGFS", "SEGGS", "OPERSIZE", "ADDRSIZE", 
	"PUSH,Iz", "IMUL,Gv,Ev,Iz", "PUSH,Ib", "IMUL,Gv,Ev,Ib", "INS,Yb,DX", "INS,Yz,DX", "OUTS,DX,Xb", "OUTS,DX,Xz"},
	/*7*/{"JO,Jump", "JNO,Jump", "JB,Jump", "JAE,Jump", "JE,Jump", "JNE,Jump", "JBE,Jump", "JA,Jump", 
	"JS,Jump", "JNS,Jump", "JPE,Jump", "JPO,Jump", "JL,Jump", "JGE,Jump", "JLE,Jump", "JG,Jump"},
	/*8*/{"Grp1,Eb,Ib", "Grp1,Ev,Iz", "Grp1,Eb,Ib", "Grp1,Ev,Ib", "TEST,Eb,Gb", "TEST,Ev,Gv", "XCHG,Eb,Gb", "XCHG,Ev,Gv", 
	"MOV,Eb,Gb", "MOV,Ev,Gv", "MOV,Gb,Eb", "MOV,Gv,Ev", "MOV,Ev,Sw", "LEA,Gv,M", "MOV,Sw,Ew", "POP,Grp1A"},
	/*9*/{"NOP", "XCHG,rAX,rCX", "XCHG,rAX,rDX", "XCHG,rAX,rBX", "XCHG,rAX,rSP", "XCHG,rAX,rBP", "XCHG,rAX,rSI", "XCHG,rAX,rDI", 
	"CWDE", "CDQ", "CALL,CallAddr", "WAIT", "PUSHFD", "POPFD", "SAHF", "LAHF"},
	/*a*/{"MOV,AL,Ob", "MOV,rAX,Ov", "MOV,Ob,AL", "MOV,Ov,rAX", "MOVS,Yb,Xb", "MOVS,Yv,Xv", "CMPS,Xb,Yb", "CMPS,Xv,Yv", 
	"TEST,AL,Ib", "TEST,rAX,Iz", "STOS,Yb", "STOS,Yv", "LODS,Xb", "LODS,Xv", "SCAS,Yb", "SCAS,Yv"},
	/*b*/{"MOV,AL,Ib", "MOV,CL,Ib", "MOV,DL,Ib", "MOV,BL,Ib", "MOV,AH,Ib", "MOV,CH,Ib", "MOV,DH,Ib", "MOV,BH,Ib", 
	"MOV,rAX,Iv", "MOV,rCX,Iv", "MOV,rDX,Iv", "MOV,rBX,Iv", "MOV,rSP,Iv", "MOV,rBP,Iv",  "MOV,rSI,Iv", "MOV,rDI,Iv"},
	/*c*/{"Grp2,Eb,Ib", "Grp2,Ev,Ib", "RET,Iw", "RET", "LES,Gz,Mp", "LDS,Gz,Mp", "MOV,Eb,Ib", "MOV,Ev,Iz", 
	"ENTER,Iw,Ib", "LEAVE", "RETF,Iw", "RETF", "INT3", "INT,Ib", "INTO", "IRETD"},
	/*d*/{"Grp2,Eb,1", "Grp2,Ev,1", "Grp2,Eb,CL", "Grp2,Ev,CL", "AAM,Ib", "AAD,Ib", "BLANK", "XLAT,Xlat", 
	"BLANK", "BLANK", "BLANK", "BLANK", "BLANK", "BLANK", "BLANK", "BLANK"},
	/*e*/{"LOOPNZ,Jb", "LOOPZ,Jb", "LOOP,Jb", "JECXZ,Jb", "IN,AL,Ib", "IN,eAX,Ib", "OUT,Ib,AL", "OUT,Ib,eAX", 
	"CALL,Jz", "JMP,Jz", "JMP,Ap", "JMP,Jb", "IN,AL,DX", "IN,eAX,DX", "OUT,DX,AL", "OUT,DX,eAX"},
	/*f*/{"LOCK", " ", "REPNE", "REP", "HLT", "CMC", "Grp3,Eb", "Grp3,Ev", 
	"CLC", "STC", "CLI", "STI", "CLD", "STD", "Grp4", "Grp5"}
};

/*OF修改的后缀，OF00 00,  0F80 - OF8F,0F90-0F9F, OFB6, OFB7, OFBE, OFBF, OFAF,其他的一律改成2DEF,Ev*/
string szTableA3[0x10][0x10] = {
	/*0*/{"SLDT,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*1*/{"2DEFR,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*2*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*3*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*4*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*5*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEFR,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*6*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEFR,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*7*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*8*/{"JO,Jz", "JNO,Jz", "JB,Jz", "JAE,Jz", "JE,Jz", "JNE,Jz", "JBE,Jz", "JA,Jz", 
	"JS,Jz", "JNS,Jz", "JPE,Jz", "JPO,Jz", "JL,Jz", "JGE,Jz", "JLE,Jz", "JG,Jz"},
	/*9*/{"SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", 
	"SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb", "SETCC,Eb"},
	/*a*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "IMUL,Gv,Ev"},
	/*b*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "MOVZX,Gv,Eb", "MOVZX,Gv,Ew", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "MOVSX,Gv,Eb", "MOVSX,Gv,Ew"},
	/*c*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*d*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*e*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEFR,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"},
	/*f*/{"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", 
	"2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev", "2DEF,Ev"}
};

int _tmain(int argc, _TCHAR* argv[])
{
	SetEnvironment();
	
	CDasm obj1;
	
	/*BYTE Buff[] = {0x55, 0x8B, 0xEC, 0x6A, 0xFE, 0x68, 0xF8, 0x87, 0xD3, 0x00, 0x68, 0xC0, 0x33, 0xCD, 0x00, 0x64
		, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x50, 0x83, 0xC4, 0xE0, 0x53, 0x56, 0x57, 0xA1, 0x14, 0xA0, 0xD3
		, 0x00, 0x31, 0x45, 0xF8, 0x33, 0xC5, 0x50, 0x8D, 0x45, 0xF0, 0x64, 0xA3, 0x00, 0x00, 0x00, 0x00
		, 0x89, 0x65, 0xE8, 0xC7, 0x45, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x01, 0x70, 0x00, 0x00, 0x66
		, 0x89, 0x45, 0xE4, 0x6A, 0x02, 0xE8, 0x56, 0x30, 0x00, 0x00, 0x83, 0xC4, 0x04, 0xE8, 0x3E, 0x01
		, 0x00, 0x00, 0x89, 0x45, 0xD4, 0xE8, 0x06, 0x24, 0x00, 0x00, 0x85, 0xC0, 0x75, 0x0A, 0x6A, 0x1C
		, 0xE8, 0xBB, 0x01, 0x00, 0x00, 0x83, 0xC4, 0x04};*/

	BYTE Buff[800] = {0};
	InitByteArray(Buff,800);
	FILE *fpInputFile = fopen("input.txt", "r");
	FillByteArray(Buff,800,fpInputFile);
	fclose(fpInputFile);

	obj1.SetStartByte(Buff);
	obj1.TokenIteration();
	
	getchar();
	return 0;
}


