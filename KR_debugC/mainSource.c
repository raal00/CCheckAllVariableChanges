#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <stdbool.h>

const char* TestCFilePath = "D:/Программирование/backend/C/ByteC/ByteC/Source.c";

int  GetLineLength(char* Line);
int  VariableContains(char* Line, int LineLength, int StartIndex, char* SubString, int SubStringLength);

int main() 
{
	#pragma region variables
	char**	searchedVariablesArray;
	int		searchedVariablesArrayLength;
	int		searchedVariableLength = 20;
	int		variableLength;

	int		LineNumber = 1;
	int		ContainsAtLineResult;
	int		OperatorIndex;
	int		VariableFirstIndex;
	int		index;

	char*	CurrentCodeLine;
	char*	variable;
	char*	Operator;
	char*	locale;

	int		lineLength;
	int		MaxLineLength = 255;
	int		MaxOperatorLength = 127;

	bool SearchingOperator = false;
	bool variableChanged = false;
	bool VariableFoundAtFunction;
	FILE*	InputFileThread;
	
	#pragma endregion

	locale = setlocale(LC_ALL, "Russian");
	if ((InputFileThread = fopen(TestCFilePath, "r")) == NULL)
	{
		printf("1) Файл %s не существует\n", TestCFilePath);
		return 1;
	}
	else
	{
		printf("1) Файл %s получен\n", TestCFilePath);
	}
	
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
	Operator = (char*)malloc(sizeof(char) * MaxOperatorLength);
	
	// Search all variables
	for (int i = 0; i < searchedVariablesArrayLength; i++)
	{
		variable = searchedVariablesArray[i];
		printf("%d) %s\n", i, variable);
		variableLength = GetLineLength(variable);

		while (fgets(CurrentCodeLine, MaxLineLength, InputFileThread) != NULL)
		{
			index = 0;
			lineLength = GetLineLength(CurrentCodeLine);

			while (index < lineLength) 
			{
				if (SearchingOperator)
				{
					while (1)
					{
						if (index == lineLength) 
						{
							break; 
						}
						if (CurrentCodeLine[index] != ' ' && CurrentCodeLine[index] != '\t')
						{
							Operator[OperatorIndex] = CurrentCodeLine[index];
							OperatorIndex++;
						}
						if (CurrentCodeLine[index] == '+' || CurrentCodeLine[index] == '-' ||
							CurrentCodeLine[index] == '/' || CurrentCodeLine[index] == '%' ||
							CurrentCodeLine[index] == '*' || 
								(CurrentCodeLine[index] == '=' && CurrentCodeLine[index-1] != '=' && 
								 CurrentCodeLine[index + 1] != '=' && CurrentCodeLine[index - 1] != '!'))
						{
							variableChanged = true;
						}

						if (CurrentCodeLine[index] == ';')
						{
							if (variableChanged == true) 
							{
								Operator[OperatorIndex] = '\0';
								printf("Operator on line %d: %s %s", LineNumber, variable, Operator);
							}
							variableChanged = false;
							SearchingOperator = false;
							printf("%c", '\n');
							break;
						}
						index++;
					}
				}
				if (index == lineLength) 
				{ 
					break; 
				}

				ContainsAtLineResult = VariableContains(CurrentCodeLine, lineLength, index, variable, variableLength);
				if (ContainsAtLineResult >= 0)
				{
					VariableFirstIndex = ContainsAtLineResult;
					VariableFoundAtFunction = false;
					while (VariableFirstIndex > 0)
					{
						VariableFirstIndex--;
						if (CurrentCodeLine[VariableFirstIndex] == '=')
						{
							VariableFoundAtFunction = true;
							break;
						}
						if (CurrentCodeLine[VariableFirstIndex] == ';') 
						{ 
							break; 
						}
					}
					if (VariableFoundAtFunction == false) 
					{
						SearchingOperator = true;
						for (OperatorIndex = 0; OperatorIndex < MaxOperatorLength; OperatorIndex++)
						{
							Operator[OperatorIndex] = NULL;
						}
						OperatorIndex = 0;
					}
					else 
					{
						VariableFoundAtFunction = false;
					}
					index = ContainsAtLineResult + variableLength;
				}
				else 
				{ 
					break; 
				}
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
	#pragma region  variables
	
	int index = 0;
	
	#pragma endregion

	while ((Line[index] != NULL) && (Line[index] != '\n') && (Line[index] != '\0') )
	{
		index++;
	}
	return index;
}


/// <example>
///		> Line: "int   value1 = 1; val=3; float VALue;" SubString: "value"
///		< -1
/// </example>
int VariableContains(char* Line, int LineLength, int SearchFrom, char* SubString, int SubStringLength)
{
	#pragma region  variables

	char	CurrentLetter;
	int		LineIndex = 0;
	int firstIndex;

	#pragma endregion

	
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
				firstIndex = j;
				do
				{
					j++;
					k++;
					if (j == firstIndex + SubStringLength - 1)
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
