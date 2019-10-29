#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <stdbool.h>

const char* TestCFilePath = "test.c";
const char* LogFile = "log.txt";

int  GetLineLength(char* Line);
int  VariableContains(char* Line, int LineLength, int StartIndex, char* SubString, int SubStringLength);
void FindAllVariableChanges(char* Line, char** variablesArray, int variablesArrayLenght, int lineNumber);

int main() 
{
	#pragma region Variables
	char**	searchedVariablesArray;
	int		searchedVariablesArrayLength;
	int		searchedVariableLength = 20;

	int		LineNumber = 1;
	char*	CurrentCodeLine;
	int		MaxLineLength = 255;
	FILE*	InputFileThread;
	char*	locale = setlocale(LC_ALL, "");

	#pragma endregion

	// step 1
	if ((InputFileThread = fopen(TestCFilePath, "r")) == NULL)
	{
		printf("1) Файл %s не существует\n", TestCFilePath);
		return 1;
	}
	else
	{
		printf("1) Файл %s получен\n", TestCFilePath);
	}

	// step 2

	
	printf("2) Введите количество переменных: ");
	scanf("%d", &searchedVariablesArrayLength);
	
	if (searchedVariablesArrayLength <= 0) 
	{
		printf("3) Число искомых переменных должно быть >0\n");
	}
	else 
	{
		printf("\n******************************************\n");
	}

	searchedVariablesArray = (char**)malloc(sizeof(void *) * searchedVariablesArrayLength); // {val1, val2...}

	for (int i = 0; i < searchedVariablesArrayLength; i++) 
	{
		searchedVariablesArray[i] = (char*)malloc(sizeof(char) * searchedVariableLength);
		printf("%d. Введите имя переменной: ", i);
		scanf("%s", searchedVariablesArray[i]);
		printf("\n");
	}
	printf("3) Начинаем поиск\n");

	CurrentCodeLine = (char*)malloc(sizeof(char) * MaxLineLength);
	bool SearchingOperator = false;
	int OperatorCount = 0;
	
	// Search all variables
	for (int i = 0; i < searchedVariablesArrayLength; i++)
	{
		char* variable = searchedVariablesArray[i];
		printf("%d) %s\n", i, variable);
		int	  variableLength = GetLineLength(variable);
		int LBoard = 0;

		while (fgets(CurrentCodeLine, MaxLineLength, InputFileThread) != NULL)
		{
			int index = 0;
			int lineLength = GetLineLength(CurrentCodeLine);

			while (index < lineLength) 
			{
				if (SearchingOperator)
				{
					while (1)
					{
						if (index == lineLength) break;
						if (CurrentCodeLine[index] != ' ' && CurrentCodeLine[index] != '\t') printf("%c",CurrentCodeLine[index]);
						if (CurrentCodeLine[index] == ';')
						{
							SearchingOperator = false;
							printf("%c", '\n');
							break;
						}
						index++;
					}
				}
				if (index == lineLength) break;

				int res = VariableContains(CurrentCodeLine, lineLength, index, variable, variableLength);
				if (res >= 0)
				{
					printf("Operator on line %d: ", LineNumber);
					index = res + variableLength - 1;
					SearchingOperator = true;
				}
				else { break; }
			}
			LineNumber++;
		}

		rewind(InputFileThread);
	}
	printf("Finish");
	getchar();
	return 0;
}


int  GetLineLength(char* Line)
{
	int index = 0;
	while ((Line[index] != NULL) && (Line[index] != '\n') && (Line[index] != '\0') )
	{
		index++;
	}
	return index;
}

int VariableContains(char* Line, int LineLength, int SearchFrom, char* SubString, int SubStringLength)
{
	/// <example>
	///		> Line: "int   value1 = 1; val=3; float VALue;" SubString: "value"
	///		< not found
	/// </example>

	char	CurrentLetter;
	int		LineIndex = 0;

	if (SubStringLength > LineLength - SearchFrom) 
	{
		return -2; 
	}

	for (int j = SearchFrom; j < LineLength; j++)
	{
		CurrentLetter = Line[j];
		for (int k = 0; k < SubStringLength; k++)
		{
			// first letter found -> check prev symb 
			if (CurrentLetter == SubString[k] && (   Line[j - 1] == NULL || Line[j - 1] == ' '
												  || Line[j - 1] == '\n' || Line[j - 1] == '\t'
												  || Line[j - 1] == '\0')) 
																								
			{
				int firstIndex = j;
				do
				{
					j++;
					k++;
					if (j >= firstIndex + SubStringLength)
					{
						// whole word found -> check next symb
						if (Line[j + 1] == NULL || Line[j + 1] == '\0' || Line[j + 1] == ' '
							|| Line[j + 1] == '\n' || Line[j + 1] == '\t' || Line[j + 1] == '='
							|| Line[j + 1] == '%' || Line[j + 1] == '-' || Line[j + 1] == '/'
							|| Line[j + 1] == '+' || Line[j + 1] == '*' || Line[j + 1] == ';')
						{
							return firstIndex;
						}
					}
				} 
				while (Line[j] == SubString[k]);
				
			}
			else continue;
		}
	}
	return -1;
}
