#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>

int getStringLength(char* string);

typedef struct TextFormat
{
	unsigned char  stringLength;
	unsigned char  countOfLetters;
	int   sumOfLetterCodes;
	unsigned char* string;

}Format;

int main(void)
{
	int a ;
	float b;
	char c;
	
	b = 1;
	a = b * b;

	c  = '1' +
		 '2' +
		 '3';
	return 0;
}

