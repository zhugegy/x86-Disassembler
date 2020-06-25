#include "stdafx.h"
#include "Dasm.h"


CDasm::CDasm(void)
{
	m_nOPcodeAmount = 1;
	m_nAddressingSizeJugger = 32;
	m_nOpearantSizeJugger = 32;
	m_nOperCodeNumberJugger = 1;
	m_szAddSymbol = "+";
	m_szRSB = "]";
	m_szLSB = "[";
	m_szColon = ":";
}

CDasm::~CDasm(void)
{
}

bool CDasm::SetStartByte(BYTE *pStartPoint)
{
	m_pStartPoint = pStartPoint;
	return true;
}

BYTE *CDasm::NewStart()
{
	m_pCurrentPoint++;
	m_pStartPoint = m_pCurrentPoint;
	return m_pStartPoint;
}

BYTE CDasm::GetCurrentByte()
{
	return *m_pCurrentPoint;
}

BYTE CDasm::GetStartByte()
{
	return *m_pStartPoint;
}

std::vector<std::string> CDasm::GiveSzValue(const std::string &s)
{
	int nCount = 0;
	std::vector<std::string> elems;  
	size_t pos = 0;  
	size_t len = s.length();  
	size_t delim_len = 1;  
	if (delim_len == 0) return elems;  
	while (pos < len)  
	{  
		int find_pos = s.find(",", pos);  
		if (find_pos < 0)  
		{  
			elems.push_back(s.substr(pos, len - pos));  
			m_szASMCommand[nCount] = s.substr(pos, find_pos - pos);
			nCount++;
			break;  
		}  
		elems.push_back(s.substr(pos, find_pos - pos));  
		m_szASMCommand[nCount] = s.substr(pos, find_pos - pos);
		nCount++;
		pos = find_pos + delim_len;  
	}  
	return elems;  
}

void CDasm::CleanSzValue()
{
	for (int i = 0; i < 4; i++)
	{
		m_szASMCommand[i] = "";
	}
}

//SIB有个[*]是很麻烦的，需要单独设置一个jugger来判断是否遇到他，如果遇到了，后面的displace就提前用了， 在上一层循环中就不需要再一次调用displace函数了，以免造成读取位置错乱。这个jugger要在每次输出一条汇编指令后归零。

//done
string CDasm::TransSIB_Index(int SS, int Index)
{
	switch (SS)
	{
	case 0:
		switch (Index)
		{
		case 0:
			return "eax";
		case 1:
			return "ecx";
		case 2:
			return "edx";
		case 3:
			return "ebx";
		case 4:
			return "";
		case 5:
			return "ebp";
		case 6:
			return "esi";
		case 7:
			return "edi";
		default:
			return "";
			break;
		}
		break;
	case 1:
		switch (Index)
		{
		case 0:
			return "eax*2";
		case 1:
			return "ecx*2";
		case 2:
			return "edx*2";
		case 3:
			return "ebx*2";
		case 4:
			return "";
		case 5:
			return "ebp*2";
		case 6:
			return "esi*2";
		case 7:
			return "edi*2";
		default:
			return "";
			break;
		}
		break;
	case 2:
		switch (Index)
		{
		case 0:
			return "eax*4";
		case 1:
			return "ecx*4";
		case 2:
			return "edx*4";
		case 3:
			return "ebx*4";
		case 4:
			return "";
		case 5:
			return "ebp*4";
		case 6:
			return "esi*4";
		case 7:
			return "edi*4";
		default:
			return "";
			break;
		}
	case 3:
		switch (Index)
		{
		case 0:
			return "eax*8";
		case 1:
			return "ecx*8";
		case 2:
			return "edx*8";
		case 3:
			return "ebx*8";
		case 4:
			return "";
		case 5:
			return "ebp*8";
		case 6:
			return "esi*8";
		case 7:
			return "edi*8";
		default:
			return "";
			break;
		}
	default:
		return "";
		break;
	}
}

//done
string CDasm::TransSIB_Base_Speical(int Mod)
{
	string szTemp = "";
	switch (Mod)
	{
	case 0:
		//继续向下读取32个偏移地址
		szTemp = Disp32();
		return szTemp;
	case 1:
		szTemp = Disp8();
		szTemp = "ebp" + m_szAddSymbol + szTemp;
		return szTemp;
	case 2:
		szTemp = Disp32();
		szTemp = "ebp" + m_szAddSymbol + szTemp;
		return szTemp;
	case 3:
		//应该不会出现case 3这种情况
		return szTemp;
	default:
		return szTemp;
		break;
	}
}

//done
string CDasm::TransSIB_Base(int Base)
{
	BYTE *pTemp = m_pCurrentPoint - 1;  //用于特殊情况[*],需要往回找MOD
	int nTempHighTwo;
	int nTempThreeA;
	int nTempThreeB;

	switch (Base)
	{
	case 0:
		return "eax";
	case 1:
		return "ecx";
	case 2:
		return "edx";
	case 3:
		return "ebx";
	case 4:
		return "esp";
	case 5:
		m_nSIBSpecialSituationJugger = 1;
		HexDivisionS(*(pTemp), nTempHighTwo, nTempThreeA, nTempThreeB);
		return TransSIB_Base_Speical(nTempHighTwo);
	case 6:
		return "esi";
	case 7:
		return "edi";
	default:
		return "";
		break;
	}
}

//done
string CDasm::TransSIB()
{
	m_pCurrentPoint ++;
	int nHighTwo;
	int nMidThree;
	int nLowThree;
	HexDivisionS(*(m_pCurrentPoint), nHighTwo, nMidThree, nLowThree);
	string szTempIndex;
	string szTempBase;
	string szTemp;
	szTempIndex = TransSIB_Index(nHighTwo, nMidThree);
	szTempBase = TransSIB_Base(nLowThree);
	if (szTempIndex != "")
	{
		szTemp = szTempIndex + m_szAddSymbol + szTempBase;
	}
	else
	{
		szTemp = szTempBase;
	}
	return szTemp;
}

//done
string CDasm::Disp32()
{
	/*m_pCurrentPoint = m_pStartPoint; //测试用，用完删掉啊，千万注意*/
	string szTemp = "";
	for (int i = 0; i < 4; i++)
	{
		m_pCurrentPoint++;
		szTemp = ByteToString(*(m_pCurrentPoint)) + szTemp;
	}
	return szTemp;
}

//done
string CDasm::Disp16()
{
	string szTemp = "";
	for (int i = 0; i < 2; i++)
	{
		m_pCurrentPoint++;
		szTemp = ByteToString(*(m_pCurrentPoint)) + szTemp;
	}
	return szTemp;
}

//done
string CDasm::Disp8()
{
	string szTemp = "";
	for (int i = 0; i < 1; i++)
	{
		m_pCurrentPoint++;
		szTemp = ByteToString(*(m_pCurrentPoint)) + szTemp;
	}
	return szTemp;
}

//done
string CDasm::TransModRM_REG(int Reg, int Type)
{
	switch (Reg)
	{
	case 0:
		switch (Type)
		{
		case 1:
			return "al";
		case 2:
			return "ax";
		case 3:
			return "eax";
		case 4:
			return "mm0";
		case 5:
			return "xmm0";
		default:
			return "";
			break;
		}
		break;
	case 1:
		switch (Type)
		{
		case 1:
			return "cl";
		case 2:
			return "cx";
		case 3:
			return "ecx";
		case 4:
			return "mm1";
		case 5:
			return "xmm1";
		default:
			return "";
			break;
		}
		break;
	case 2:
		switch (Type)
		{
		case 1:
			return "dl";
		case 2:
			return "dx";
		case 3:
			return "edx";
		case 4:
			return "mm2";
		case 5:
			return "xmm2";
		default:
			return "";
			break;
		}
		break;
	case 3:
		switch (Type)
		{
		case 1:
			return "bl";
		case 2:
			return "bx";
		case 3:
			return "ebx";
		case 4:
			return "mm3";
		case 5:
			return "xmm3";
		default:
			return "";
			break;
		}
		break;
	case 4:
		switch (Type)
		{
		case 1:
			return "ah";
		case 2:
			return "sp";
		case 3:
			return "esp";
		case 4:
			return "mm4";
		case 5:
			return "xmm4";
		default:
			return "";
			break;
		}
		break;
	case 5:
		switch (Type)
		{
		case 1:
			return "ch";
		case 2:
			return "bp";
		case 3:
			return "ebp";
		case 4:
			return "mm5";
		case 5:
			return "xmm5";
		default:
			return "";
			break;
		}
		break;
	case 6:
		switch (Type)
		{
		case 1:
			return "dh";
		case 2:
			return "si";
		case 3:
			return "esi";
		case 4:
			return "mm6";
		case 5:
			return "xmm6";
		default:
			return "";
			break;
		}
		break;
	case 7:
		switch (Type)
		{
		case 1:
			return "bh";
		case 2:
			return "di";
		case 3:
			return "edi";
		case 4:
			return "mm7";
		case 5:
			return "xmm7";
		default:
			return "";
			break;
		}
		break;
	default:
		return "";
		break;
	}
}

//undone 32位的表完成了
string CDasm::TransModRM_RM(int Mod, int RM, int Type)
{
	/*为了防止指针的错乱，建立两个临时字符串变量，用于一步一步操作获取字符串的
	函数，因为这些函数内部包含指针的变化，不能用复合表达式，否则容易出错*/
	string szTemp = "";
	string szTempAssistant = "";

	if (m_nAddressingSizeJugger == 32)
	{
		/*当RM==5(4?) 的时候要注意了，进入SIB的时候，在后面加displace的时候，要注意SIBjugger的值，如果值等于1, 说明SIB里调用了特殊的那一栏，说明displace已经被读取了，就不需要（不可以）再次读取displace了*/
		switch (Mod)
		{
		case 0:
			switch (RM)
			{
			case 0:
				return "[eax]";
			case 1:
				return "[ecx]";
			case 2:
				return "[edx]";
			case 3:
				return "[ebx]";
			case 4:
				//20150520晚上写到这里 SIB部分需要一个函数
				//string szTemp = ;//SIB
				szTemp = TransSIB();
				szTemp = m_szLSB + szTemp + m_szRSB;
				return szTemp;
			case 5:
				szTemp = Disp32();
				szTemp = m_szLSB + szTemp + m_szRSB;
				return szTemp;
			case 6:
				return "[esi]";
			case 7:
				return "[edi]";
			default:
				return szTemp;
				break;
			}
			break;
		case 1:	
			switch (RM)
			{
			case 0:
				szTemp = Disp8(); //并不是冗余代码，此函数内部包含指针m_pcurrentpoint的变化，为了防止出错，必须一步一步来
				szTemp = m_szLSB + "eax" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 1:
				szTemp = Disp8();
				szTemp = m_szLSB + "ecx" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 2:
				szTemp = Disp8();
				szTemp = m_szLSB + "edx" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 3:
				szTemp = Disp8();
				szTemp = m_szLSB + "ebx" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 4:
				szTemp = TransSIB();

				//如果没有触发SIB Special这个区域[*]，证明后面的地址还没有读取
				if (m_nSIBSpecialSituationJugger == 0)
				{
					szTempAssistant = Disp8();
					szTemp = m_szLSB + szTemp + m_szAddSymbol + szTempAssistant 
						+ m_szRSB;
				}
				else
				{
					szTemp = m_szLSB + szTemp + m_szRSB;
				}
				return szTemp;
			case 5:
				szTemp = Disp8();
				szTemp = m_szLSB + "ebp" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 6:
				szTemp = Disp8();
				szTemp = m_szLSB + "esi" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 7:
				szTemp = Disp8();
				szTemp = m_szLSB + "edi" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			default:
				return szTemp;
				break;
			}
			break;
		case 2:
			switch (RM)
			{
			case 0:
				szTemp = Disp32(); //并不是冗余代码，此函数内部包含指针m_pcurrentpoint的变化，为了防止出错，必须一步一步来
				szTemp = m_szLSB + "eax" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 1:
				szTemp = Disp32();
				szTemp = m_szLSB + "ecx" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 2:
				szTemp = Disp32();
				szTemp = m_szLSB + "edx" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 3:
				szTemp = Disp32();
				szTemp = m_szLSB + "ebx" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 4:
				szTemp = TransSIB();
				if (m_nSIBSpecialSituationJugger == 0)
				{
					szTempAssistant = Disp32();
					szTemp = m_szLSB + szTemp + m_szAddSymbol + szTempAssistant 
						+ m_szRSB;
				}
				else
				{
					szTemp = m_szLSB + szTemp + m_szRSB;
				}
				return szTemp;
			case 5:
				szTemp = Disp32();
				szTemp = m_szLSB + "ebp" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 6:
				szTemp = Disp32();
				szTemp = m_szLSB + "esi" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			case 7:
				szTemp = Disp32();
				szTemp = m_szLSB + "edi" + m_szAddSymbol + szTemp + m_szRSB;
				return szTemp;
			default:
				return szTemp;
				break;
			}
			break;
		case 3:
			szTemp = TransModRM_REG(RM, Type);
			return szTemp;  
			break;
		default:
			return szTemp;
			break;
		}
	}
	else if (m_nAddressingSizeJugger == 16)
	{
		return szTemp;//16位地址寻址暂时空着吧，等前缀时候再说
	}
	return szTemp;
}

string CDasm::TransSw(int MidREG)
{
	switch (MidREG)
	{
	case 0:
		return "es";
	case 1:
		return "cs";
	case 2:
		return "ss";
	case 3:
		return "ds";
	case 4:
		return "fs";
	case 5:
		return "gs";
	case 6:
		return "seg6Error";
	case 7:
		return "seg7Error";
	default:
		return "";
		break;
	}
}

string CDasm::TransGrp1A(int Value)
{
	switch (Value)
	{
	case 0:
		return "[eax]";
	case 1:
		return "[ecx]";
	case 2:
		return "[edx]";
	case 3:
		return "[ebx]";
	case 4:
		/*这地方实在不想弄了，糊弄一下*/
		m_pCurrentPoint++;
		return "[eax + eax]";
	case 5:
		return m_szLSB + Disp32() + m_szRSB;
	case 6:
		return "[esi]";
	case 7:
		return "[edi]";
	default:
		return "";
		break;
	}
}

string CDasm::TransCallAddr()
{
	string szTemp1;
	string szTemp2;
	szTemp1 = Disp32();
	szTemp2 = Disp16();
	szTemp1 = "far " + szTemp2 + m_szColon + szTemp1;
	return szTemp1;
}

//undone
void CDasm::OverwriteAbbreviation(int i)
{
	int nHighTwo;
	int nMidThree;
	int nLowThree;
	if (m_szASMCommand[i] == "ADD" || m_szASMCommand[i] == "ADC" || m_szASMCommand[i] == "PUSH" 
		|| m_szASMCommand[i] == "es" || m_szASMCommand[i] == "POP" || m_szASMCommand[i] == "OR" 
		|| m_szASMCommand[i] == "cs" || m_szASMCommand[i] == "" || m_szASMCommand[i] == "ss" 
		|| m_szASMCommand[i] == "SBB" || m_szASMCommand[i] == "ds" || m_szASMCommand[i] == "AND" 
		|| m_szASMCommand[i] == "SEGES" || m_szASMCommand[i] == " " || m_szASMCommand[i] == "SUB" 
		|| m_szASMCommand[i] == "SEGCS" || m_szASMCommand[i] == "DAS" || m_szASMCommand[i] == "XOR" 
		|| m_szASMCommand[i] == "SEGSS" || m_szASMCommand[i] == "AAA" || m_szASMCommand[i] == "CMP" 
		|| m_szASMCommand[i] == "SEGDS" || m_szASMCommand[i] == "AAS" || m_szASMCommand[i] == "INC" 
		|| m_szASMCommand[i] == "DEC" || m_szASMCommand[i] == "BOUND" || m_szASMCommand[i] == "ARPL" 
		|| m_szASMCommand[i] == "SEGFS" || m_szASMCommand[i] == "SEGGS" || m_szASMCommand[i] == "IMUL" 
		|| m_szASMCommand[i] == "INS" || m_szASMCommand[i] == "OUTS" || m_szASMCommand[i] == "JO" 
		|| m_szASMCommand[i] == "JNO" || m_szASMCommand[i] == "JB" || m_szASMCommand[i] == "JAE" 
		|| m_szASMCommand[i] == "JE" || m_szASMCommand[i] == "JNE" || m_szASMCommand[i] == "JBE" 
		|| m_szASMCommand[i] == "JA" || m_szASMCommand[i] == "JS" || m_szASMCommand[i] == "JNS" 
		|| m_szASMCommand[i] == "JPE" || m_szASMCommand[i] == "JPO" || m_szASMCommand[i] == "JL" 
		|| m_szASMCommand[i] == "JGE" || m_szASMCommand[i] == "JLE" || m_szASMCommand[i] == "JG" 
		|| m_szASMCommand[i] == "TEST" || m_szASMCommand[i] == "XCHG" || m_szASMCommand[i] == "MOV" 
		|| m_szASMCommand[i] == "LEA" || m_szASMCommand[i] == "NOP" || m_szASMCommand[i] == "CWDE" 
		|| m_szASMCommand[i] == "CDQ" || m_szASMCommand[i] == "CALL" || m_szASMCommand[i] == "WAIT" 
		|| m_szASMCommand[i] == "PSUHFD" || m_szASMCommand[i] == "POPFD" || m_szASMCommand[i] == "SAHF" 
		|| m_szASMCommand[i] == "LAHF" || m_szASMCommand[i] == "MOVS" || m_szASMCommand[i] == "CMPS" 
		|| m_szASMCommand[i] == "STOS" || m_szASMCommand[i] == "LODS" || m_szASMCommand[i] == "SCAS" 
		|| m_szASMCommand[i] == "RET" || m_szASMCommand[i] == "LES" || m_szASMCommand[i] == "LDS" 
		|| m_szASMCommand[i] == "ENTER" || m_szASMCommand[i] == "LEAVE" || m_szASMCommand[i] == "RETF" 
		|| m_szASMCommand[i] == "INT3" || m_szASMCommand[i] == "INT" || m_szASMCommand[i] == "INTO" 
		|| m_szASMCommand[i] == "IRETD" || m_szASMCommand[i] == "1" || m_szASMCommand[i] == "AAM" 
		|| m_szASMCommand[i] == "AAD" || m_szASMCommand[i] == "BLANK" || m_szASMCommand[i] == "DAA" 
		|| m_szASMCommand[i] == "XLAT" || m_szASMCommand[i] == "LOOPNZ" || m_szASMCommand[i] == "LOOPZ" 
		|| m_szASMCommand[i] == "LOOP" || m_szASMCommand[i] == "JECXZ" || m_szASMCommand[i] == "IN" 
		|| m_szASMCommand[i] == "OUT" || m_szASMCommand[i] == "JMP" || m_szASMCommand[i] == "LOCK" 
		|| m_szASMCommand[i] == "REPNE" || m_szASMCommand[i] == "REP" || m_szASMCommand[i] == "HLT" 
		|| m_szASMCommand[i] == "CMC" || m_szASMCommand[i] == "CLC" || m_szASMCommand[i] == "STC" 
		|| m_szASMCommand[i] == "CLI" || m_szASMCommand[i] == "STI" || m_szASMCommand[i] == "CLD" 
		|| m_szASMCommand[i] == "STD" || m_szASMCommand[i] == "SLDT" || m_szASMCommand[i] == "MOVZX" 
		|| m_szASMCommand[i] == "2DEF" || m_szASMCommand[i] == "2DEFR" || m_szASMCommand[i] == "SETCC" 
		|| m_szASMCommand[i] == "MOVSX")
	{
		;
	}
	else if (m_szASMCommand[i] == "2ESC")
	{
		m_nOperCodeNumberJugger = 2;
	}
	else if (m_szASMCommand[i] == "Eb")
	{
		m_pCurrentPoint = m_pStartPoint + m_nOPcodeAmount;
		/*m_szASMCommand[i] = *(m_pCurrentPoint);*/  //测试用
		HexDivisionS(*(m_pCurrentPoint), nHighTwo, nMidThree, nLowThree);
		if (nHighTwo == 3)
		{
			m_szASMCommand[i] = TransModRM_RM(nHighTwo, nLowThree, 1);
		}
		else
		{
			m_szASMCommand[i] = "byte ptr " + TransModRM_RM(nHighTwo, nLowThree, 1);
		}
	}
	else if (m_szASMCommand[i] == "Ew")
	{
		m_pCurrentPoint = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(m_pCurrentPoint), nHighTwo, nMidThree, nLowThree);
		if (nHighTwo == 3)
		{
			m_szASMCommand[i] = TransModRM_RM(nHighTwo, nLowThree, 2);
		}
		else
		{
			m_szASMCommand[i] = "word ptr " + TransModRM_RM(nHighTwo, nLowThree, 2);
		}
	}
	else if (m_szASMCommand[i] == "Gb")
	{
		/*涉及到Gb,Eb的默认位置顺序，这个是不一定的，
		为了让Gb不影响整体的读操作，给他一个临时指针，
		有关寄存器的读操作，都不会改变m_pCurrentPoint的值*/
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		//CDasm::TransModRM_REG 的 Type是1，因为是b缩写
		m_szASMCommand[i] = TransModRM_REG(nMidThree, 1);
	}
	else if (m_szASMCommand[i] == "Gb")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		m_szASMCommand[i] = TransModRM_REG(nMidThree, 2);
	}
	else if (m_szASMCommand[i] == "Ob")
	{
		m_szASMCommand[i] = "byte ptr " + m_szLSB + Disp32() + m_szRSB;
	}
	else if (m_szASMCommand[i] == "Ov")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "dword ptr " + m_szLSB + Disp32() + m_szRSB;
		}
		else
		{
			m_szASMCommand[i] = "word ptr " + m_szLSB + Disp32() + m_szRSB;
		}
	}
	else if (m_szASMCommand[i] == "Ma")
	{
		m_pCurrentPoint = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(m_pCurrentPoint), nHighTwo, nMidThree, nLowThree);
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "qword ptr " + TransModRM_RM(nHighTwo, nLowThree, 3);
		}
		else
		{
			/*由于Ma一定是内存，不可能是寄存器，所以Type的值也无所谓了*/
			m_szASMCommand[i] = "dword ptr " + TransModRM_RM(nHighTwo, nLowThree, 2);
		}
	}
	else if (m_szASMCommand[i] == "Mp")
	{
		m_pCurrentPoint = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(m_pCurrentPoint), nHighTwo, nMidThree, nLowThree);
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "fword ptr " + TransModRM_RM(nHighTwo, nLowThree, 3);
		}
		else
		{
			m_szASMCommand[i] = "dword ptr " + TransModRM_RM(nHighTwo, nLowThree, 2);
		}
	}
	else if (m_szASMCommand[i] == "M")
	{
		m_pCurrentPoint = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(m_pCurrentPoint), nHighTwo, nMidThree, nLowThree);
		m_szASMCommand[i] = TransModRM_RM(nHighTwo, nLowThree, 3);
	}
	else if (m_szASMCommand[i] == "Ev")
	{
		m_pCurrentPoint = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(m_pCurrentPoint), nHighTwo, nMidThree, nLowThree);
		if (m_nOpearantSizeJugger == 32)
		{
			if (nHighTwo == 3)
			{
				m_szASMCommand[i] = TransModRM_RM(nHighTwo, nLowThree, 3);
			}
			else
			{
				m_szASMCommand[i] = "dword ptr " + TransModRM_RM(nHighTwo, nLowThree, 3);
			}
		}
		else
		{
			if (nHighTwo == 3)
			{
				m_szASMCommand[i] = TransModRM_RM(nHighTwo, nLowThree, 2);
			}
			else
			{
				m_szASMCommand[i] = "word ptr " + TransModRM_RM(nHighTwo, nLowThree, 2);
			}
		}	
	}
	else if (m_szASMCommand[i] == "Ep")
	{
		m_pCurrentPoint = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(m_pCurrentPoint), nHighTwo, nMidThree, nLowThree);
		if (m_nOpearantSizeJugger == 32)
		{
			if (nHighTwo == 3)
			{
				m_szASMCommand[i] = TransModRM_RM(nHighTwo, nLowThree, 3);
			}
			else
			{
				m_szASMCommand[i] = "fword ptr " + TransModRM_RM(nHighTwo, nLowThree, 3);
			}
		}
		else
		{
			if (nHighTwo == 3)
			{
				m_szASMCommand[i] = TransModRM_RM(nHighTwo, nLowThree, 2);
			}
			else
			{
				m_szASMCommand[i] = "dword ptr " + TransModRM_RM(nHighTwo, nLowThree, 2);
			}
		}
	}
	else if (m_szASMCommand[i] == "Gv" || m_szASMCommand[i] == "Gz")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = TransModRM_REG(nMidThree, 3);
		}
		else
		{
			m_szASMCommand[i] = TransModRM_REG(nMidThree, 2);
		}
	}
	else if (m_szASMCommand[i] == "Yb")
	{
		m_szASMCommand[i] = "byte ptr " + m_szLSB + "edi" + m_szRSB;
	}
	else if (m_szASMCommand[i] == "Xb")
	{
		m_szASMCommand[i] = "byte ptr " + m_szLSB + "esi" + m_szRSB;
	}
	else if (m_szASMCommand[i] == "Yz" || m_szASMCommand[i] == "Yv")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "dword ptr " + m_szLSB + "edi" + m_szRSB;
		}
		else
		{
			m_szASMCommand[i] = "word ptr " + m_szLSB + "edi" + m_szRSB;
		}
	}
	else if (m_szASMCommand[i] == "Xz" || m_szASMCommand[i] == "Xv")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "dword ptr " + m_szLSB + "esi" + m_szRSB;
		}
		else
		{
			m_szASMCommand[i] = "word ptr " + m_szLSB + "esi" + m_szRSB;
		}
	}
	else if (m_szASMCommand[i] == "AL")
	{
		m_szASMCommand[i] = "al";
	}
	else if (m_szASMCommand[i] == "AH")
	{
		m_szASMCommand[i] = "ah";
	}
	else if (m_szASMCommand[i] == "CH")
	{
		m_szASMCommand[i] = "ch";
	}
	else if (m_szASMCommand[i] == "DH")
	{
		m_szASMCommand[i] = "dh";
	}
	else if (m_szASMCommand[i] == "BH")
	{
		m_szASMCommand[i] = "bh";
	}
	else if (m_szASMCommand[i] == "CL")
	{
		m_szASMCommand[i] = "cl";
	}
	else if (m_szASMCommand[i] == "DL")
	{
		m_szASMCommand[i] = "dl";
	}
	else if (m_szASMCommand[i] == "BL")
	{
		m_szASMCommand[i] = "bl";
	}
	else if (m_szASMCommand[i] == "DX")
	{
		m_szASMCommand[i] = "dx";
	}
	else if (m_szASMCommand[i] == "Ib")
	{
		m_szASMCommand[i] = Disp8();
	}
	else if (m_szASMCommand[i] == "Iw")
	{
		m_szASMCommand[i] = Disp16();
	}
	else if (m_szASMCommand[i] == "rAX" || m_szASMCommand[i] == "eAX")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "eax";
		}
		else
		{
			m_szASMCommand[i] = "ax";
		}
	}
	else if (m_szASMCommand[i] == "Iz" || m_szASMCommand[i] == "Iv" 
		|| m_szASMCommand[i] == "Jz")
	{
		/*为了方便以后64位的升级，放弃使用三目运算符*/
		/*m_szASMCommand[i] = m_nOpearantSizeJugger == 32? Disp32():Disp16();*/
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = Disp32();
		}
		else
		{
			m_szASMCommand[i] = Disp16();
		}
	}
	else if (m_szASMCommand[i] == "rCX" || m_szASMCommand[i] == "eCX")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "ecx";
		}
		else
		{
			m_szASMCommand[i] = "cx";
		}
	}
	else if (m_szASMCommand[i] == "rDX" || m_szASMCommand[i] == "eDX")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "edx";
		}
		else
		{
			m_szASMCommand[i] = "dx";
		}
	}
	else if (m_szASMCommand[i] == "rBX" || m_szASMCommand[i] == "eBX")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "ebx";
		}
		else
		{
			m_szASMCommand[i] = "bx";
		}
	}
	else if (m_szASMCommand[i] == "rSP" || m_szASMCommand[i] == "eSP")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "esp";
		}
		else
		{
			m_szASMCommand[i] = "sp";
		}
	}
	else if (m_szASMCommand[i] == "rBP" || m_szASMCommand[i] == "eBP")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "ebp";
		}
		else
		{
			m_szASMCommand[i] = "bp";
		}
	}
	else if (m_szASMCommand[i] == "rSI" || m_szASMCommand[i] == "eSI")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "esi";
		}
		else
		{
			m_szASMCommand[i] = "si";
		}
	}
	else if (m_szASMCommand[i] == "rDI" || m_szASMCommand[i] == "eDI")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "edi";
		}
		else
		{
			m_szASMCommand[i] = "di";
		}
	}
	else if (m_szASMCommand[i] == "PUSHA")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "PUSHAD";
		}
	}
	else if (m_szASMCommand[i] == "POPA")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_szASMCommand[i] = "POPAD";
		}
	}
	else if (m_szASMCommand[i] == "OPERSIZE")
	{
		if (m_nOpearantSizeJugger == 32)
		{
			m_nOpearantSizeJugger = 16;
		}
		else
		{
			m_nOpearantSizeJugger = 32;
		}
	}
	else if (m_szASMCommand[i] == "ADDRSIZE")
	{
		if (m_nAddressingSizeJugger == 32)
		{
			m_nAddressingSizeJugger = 16;
		}
		else
		{
			m_nAddressingSizeJugger = 32;
		}
	}
	else if (m_szASMCommand[i] == "Jump" || m_szASMCommand[i] == "Jb")
	{
		m_szASMCommand[i] = "short " + Disp8();
	}
	else if (m_szASMCommand[i] == "Loop")
	{
		m_szASMCommand[i] = Disp8();
	}
	else if (m_szASMCommand[i] == "Grp1")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		m_szASMCommand[i] = TransGrp1(nMidThree);
	}
	else if (m_szASMCommand[i] == "Grp2")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		m_szASMCommand[i] = TransGrp2(nMidThree);
	}
	else if (m_szASMCommand[i] == "Grp3")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		m_szASMCommand[i] = TransGrp3(nMidThree);
	}
	else if (m_szASMCommand[i] == "Grp4")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		m_szASMCommand[i] = TransGrp4(nMidThree);
	}
	else if (m_szASMCommand[i] == "Grp5")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		m_szASMCommand[i] = TransGrp5(nMidThree);
	}
	else if (m_szASMCommand[i] == "Sw")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		HexDivisionS(*(pTemp), nHighTwo, nMidThree, nLowThree);
		m_szASMCommand[i] = TransSw(nMidThree);
	}
	else if (m_szASMCommand[i] == "Grp1A")
	{
		BYTE *pTemp = m_pStartPoint + m_nOPcodeAmount;
		if (*pTemp < 8)
		{
			m_pCurrentPoint++;
			if (m_nOpearantSizeJugger == 32)
			{
				m_szASMCommand[i] = "dword ptr " + TransGrp1A(*(m_pCurrentPoint));
			}
			else
			{
				m_szASMCommand[i] = "word ptr " + TransGrp1A(*(m_pCurrentPoint));
			}
		}
		else
		{
			m_szASMCommand[i] = "Error in POPGrp1A";
		}
	}
	else if (m_szASMCommand[i] == "CallAddr" || m_szASMCommand[i] == "Ap")
	{
		m_szASMCommand[i] = TransCallAddr();
	}
	else if (m_szASMCommand[i] == "Xlat")
	{
		m_szASMCommand[i] = "byte ptr [ebx+al]";
	}
}

string CDasm::TransGrp1(int Mid)
{
	switch (Mid)
	{
	case 0:
		return "ADD";
	case 1:
		return "OR";
	case 2:
		return "ADC";
	case 3:
		return "SBB";
	case 4:
		return "AND";
	case 5:
		return "SUB";
	case 6:
		return "XOR";
	case 7:
		return "CMP";
	default:
		return "";
		break;
	}
}

string CDasm::TransGrp2(int Mid)
{
	switch (Mid)
	{
	case 0:
		return "ROL";
	case 1:
		return "ROR";
	case 2:
		return "RCL";
	case 3:
		return "RCR";
	case 4:
		return "SHL/SAL";
	case 5:
		return "SHR";
	case 6:
		return "";
	case 7:
		return "SAR";
	default:
		return "";
		break;
	}
}

string CDasm::TransGrp3(int Mid)
{
	switch (Mid)
	{
	case 0:
		m_szASMCommand[2] = "Ib";
		return "TEST";
	case 1:
		return "";
	case 2:
		return "NOT";
	case 3:
		return "NEG";
	case 4:
		return "MUL";
	case 5:
		return "IMUL";
	case 6:
		return "DIV";
	case 7:
		return "IDIV";
	default:
		return "";
		break;
	}
}

string CDasm::TransGrp4(int Mid)
{
	switch (Mid)
	{
	case 0:
		m_szASMCommand[1] = "Eb";
		return "INC";
	case 1:
		m_szASMCommand[1] = "Eb";
		return "DEC";
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		return "";
	default:
		return "";
		break;
	}
}

string CDasm::TransGrp5(int Mid)
{
	switch (Mid)
	{
	case 0:
		m_szASMCommand[1] = "Ev";
		return "INC";
	case 1:
		m_szASMCommand[1] = "Ev";
		return "DEC";
	case 2:
		m_szASMCommand[1] = "Ev";
		return "CALL";
	case 3:
		m_szASMCommand[1] = "Ep"; 
		return "CALL";
	case 4:
		m_szASMCommand[1] = "Ev";
		return "JMP";
	case 5:
		m_szASMCommand[1] = "Mp";
		return "JMP";
	case 6:
		m_szASMCommand[1] = "Ev";
		return "PUSH";
	case 7:
		return "";
	default:
		return "";
		break;
	}
}

void CDasm::TokenIteration()
{
	FILE *fpOutputFile = fopen("output.txt", "w");
	int nEndJugger = 0;
	int nASMCounter = 0;
	string szTemp;
	do 
	{
		//当前指针置为开始指针
		m_pCurrentPoint = m_pStartPoint;

		//进行一系列犀利的操作
		HexDivision(*(m_pStartPoint), m_nHighFour, m_nLowFour);
		if (m_nOperCodeNumberJugger == 1)
		{
			szTemp = szTableA2[m_nHighFour][m_nLowFour];
		}
		else if(m_nOperCodeNumberJugger == 2)
		{
			szTemp = szTableA3[m_nHighFour][m_nLowFour];
			m_nOperCodeNumberJugger = 1;
		}
		
		GiveSzValue(szTemp);

		//对SZCommand数组从第一个值到第四个值进行重写覆盖，覆盖成汇编指令
		OverwriteAbbreviation(0);
		OverwriteAbbreviation(1);
		OverwriteAbbreviation(2);
		OverwriteAbbreviation(3);

		//把整个数组输出打印
		std::cout<<m_szASMCommand[0]<<"\t"<<m_szASMCommand[1];
		szTemp = m_szASMCommand[0] + "\t" + m_szASMCommand[1];
		if (m_szASMCommand[2] != "")
		{
			std::cout<<", "<<m_szASMCommand[2];
			szTemp = szTemp + ", " + m_szASMCommand[2];
		}
		if (m_szASMCommand[3] != "")
		{
			std::cout<<", "<<m_szASMCommand[3];
			szTemp = szTemp + ", " + m_szASMCommand[3];
		}
		std::cout<<std::endl;
		szTemp = szTemp + "\n";
		fprintf(fpOutputFile, "%s", szTemp.c_str());
		nASMCounter++;

		//队尾检测,连续输出两次" "之后，判断编译结束，退出循环
		if (m_szASMCommand[0]  == " ")
		{
			if (nEndJugger == 1)
			{
				break;
			}
			nEndJugger = 1;
		}
		else
		{
			nEndJugger = 0;
		}

		//不要忘记SZ字符串数组归零
		szTemp = "";
		CleanSzValue();
		m_nSIBSpecialSituationJugger = 0;
		m_nAddressingSizeJugger = 32;
		m_nOpearantSizeJugger = 32;
		NewStart();
	} while (nASMCounter < 200);  //设置最多输出多少行汇编代码
	fclose(fpOutputFile);
	printf("Result has been written to: output.txt");
}

/*while (*(CDasm::NewStart()) != 0xcc && nASMCounter < 100);*/