#include "stdafx.h"
#include "logic.h"

void HexDivision(int Origin, int &HighFour, int &LowFour)
{
	LowFour = Origin & 0x0F;
	HighFour = (Origin - LowFour) / 0x10;
}

void HexDivisionS(int Origin, int &HighTwo, int &MidThree, int &LowThree)
{
	LowThree = Origin & 0x07;
	MidThree = (Origin & 0x38) / 0x08;
	HighTwo = (Origin & 0xC0) / 0x40;
}

string HalfByteTranslation(int number)
{
	switch (number)
	{
	case 0:
		return "0";
	case 1:
		return "1";
	case 2:
		return "2";
	case 3:
		return "3";
	case 4:
		return "4";
	case 5:
		return "5";
	case 6:
		return "6";
	case 7:
		return "7";
	case 8:
		return "8";
	case 9:
		return "9";
	case 10:
		return "A";
	case 11:
		return "B";
	case 12:
		return "C";
	case 13:
		return "D";
	case 14:
		return "E";
	case 15:
		return "F";
	default:
		return "";
		break;
	}
}

string ByteToString(int OriginByte)
{
	int Low;
	int High;
	string szTemp;
	HexDivision(OriginByte, High, Low);
	szTemp = HalfByteTranslation(High) + HalfByteTranslation(Low);
	return szTemp;
}

int TransCharToInt(char va)
{
	switch (va)
	{
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
	case 'a':
		return 10;
	case 'B':
	case 'b':
		return 11;
	case 'C':
	case 'c':
		return 12;
	case 'D':
	case 'd':
		return 13;
	case 'E':
	case 'e':
		return 14;
	case 'F':
	case 'f':
		return 15;
	default:
		return 0;
		break;
	}
}

bool FillByteArray(BYTE InuputArray[], int ArrayLength, FILE *pInput)
{
	int nFactor16 = 16;
	int nFactor1 = 1;
	int nSum = 0;
	char temp = ' ';
	for (int i = 0; i < ArrayLength; i++)
	{
		temp = fgetc(pInput);
		while (temp == ' ' || temp == '\n')
		{
			temp = fgetc(pInput);
		}
		if (temp == EOF)
		{
			return true;
		}
		nSum = TransCharToInt(temp) * nFactor16;

		temp = fgetc(pInput);
		while (temp == ' ' || temp == '\n')
		{
			temp = fgetc(pInput);
		}
		if (temp == EOF)
		{
			return true;
		}
		nSum = nSum + TransCharToInt(temp) * nFactor1;
		InuputArray[i] = nSum;
	}
	return true;
}

bool InitByteArray(BYTE InputArray[], int ArrayLength)
{
	for (int i = 0; i < ArrayLength; i++)
	{
		InputArray[i] = 241;  //0xF1
	}
	return true;
}

