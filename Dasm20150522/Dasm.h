#pragma once
extern string szTableA2[0x10][0x10];
extern string szTableA3[0x10][0x10];
void HexDivision(int Origin, int &HighFour, int &LowFour);
void HexDivisionS(int Origin, int &HighTwo, int &MidThree, int &LowThree);
string ByteToString(int OriginByte);

class CDasm
{
public:
	CDasm(void);
	~CDasm(void);

	bool SetStartByte(BYTE *pStartPoint);


	BYTE GetCurrentByte(); //测试用的，正式版本删除
	BYTE GetStartByte();  //测试用的，正式版本删除

	void TokenIteration();




private:
	string m_szASMCommand[4];
	BYTE *m_pCurrentPoint;
	BYTE *m_pStartPoint;
	int m_nOPcodeAmount;  //用来记录操作数的字节，默认是1
	int m_nHighFour;
	int m_nLowFour;
	int m_nAddressingSizeJugger; //默认32位
	int m_nOpearantSizeJugger; //默认32位
	//int m_nByteCounter;  //当涉及一次读写多位时，用来控制读的操作的结束
	int m_nSIBSpecialSituationJugger;
	int m_nOperCodeNumberJugger;
	string m_szAddSymbol;
	string m_szRSB;  //Right Square Bracket
	string m_szLSB;  //Left Square Bracket
	string m_szColon;
	/*预留前缀状态 */

	std::vector<std::string> GiveSzValue(const std::string &s);
	BYTE *NewStart();
	void CleanSzValue();
	void OverwriteAbbreviation(int i);
	string TransModRM_RM(int Mod, int RM, int Type);
	string TransModRM_REG(int Reg, int Type);
	string TransSIB_Index(int SS, int Index);
	string TransSIB_Base(int Base);
	string TransSIB();
	string TransSIB_Base_Speical(int Mod);
	string Disp32();
	string Disp16();
	string Disp8();
	string TransGrp1(int Mid);
	string TransGrp2(int Mid);
	string TransGrp3(int Mid);
	string TransGrp4(int Mid);
	string TransGrp5(int Mid);
	string TransSw(int MidREG);
	string TransGrp1A(int Value);
	string TransCallAddr();
};

