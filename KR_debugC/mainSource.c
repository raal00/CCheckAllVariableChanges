#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <stdbool.h>

const char* TestCFilePath = "test.c";

typedef struct
{	
	char* VarName;
	int   Length;
}Var;

int  GetLineLength(char* Line);
int  getCountOfVariables(char* CFilePath);
int  VariableContains(char* Line, int LineLength, int StartIndex, char* SubString, int SubStringLength);

int main() 
{
	char**	searchedVariablesArray;
	int		searchedVariablesArrayLength;
	int		searchedVariableLength = 20;
	int		variableLength;

	int		LineNumber = 1;
	int		ContainsAtLineResult;
	int		OperatorIndex;
	int		VariableFirstIndex;
	int		index;
	int		prevIndex;
	int		i;
	int		variablesCount;

	char*	CurrentCodeLine;
	char*	variable;
	char*	Operator;
	char*	locale;
	char	LastSymbAtOperator;

	int		lineLength;
	int		MaxLineLength = 256;
	int		MaxOperatorLength = 64;

	bool SearchingOperator = false;
	bool variableChanged = false;
	bool VariableFoundAtFunction;
	bool decPrev = false;
	bool incPrev = false;
	FILE*	InputFileThread;

	locale = setlocale(LC_ALL, "Russian");

	if ((InputFileThread = fopen(TestCFilePath, "r")) == NULL)
	{
		printf("1) Файл %s не существует\n", TestCFilePath);
		return -7;
	}
	else
	{
		printf("1) Файл %s получен\n", TestCFilePath);
	}

	variablesCount = getCountOfVariables(TestCFilePath);
	if (variablesCount > 0) 
	{
		printf("Найдено переменных в файле: %d\n", variablesCount);
	}
	else 
	{
		printf("В тексте программы не найдено переменных\n");
		return -3;
	}

	printf("2) Введите количество переменных: ");

	if (scanf("%d", &searchedVariablesArrayLength) != 1)
	{
		printf("Значение количества переменных должно быть целое число");
		return -1;
	}
	rewind(stdin);

	if (searchedVariablesArrayLength <= 0)
	{
		printf("3) Число искомых переменных должно быть >0\n");
		return -2;
	}
	else if (searchedVariablesArrayLength > variablesCount)
	{
		printf("Вы ввели слишком большое количество переменных! Максимальное значение = %d\n", variablesCount);
		searchedVariablesArrayLength = variablesCount;
	}
	else
	{
		printf("\n******************************************\n");
	}
	searchedVariablesArray = (char**)malloc(sizeof(void *) * searchedVariablesArrayLength);
	if (searchedVariablesArray == NULL) 
	{
		printf("Не удалось выделить память под имена переменных\n");
		return -3;
	}
	for (i = 0; i < searchedVariablesArrayLength; i++) 
	{
		searchedVariablesArray[i] = (char*)malloc(sizeof(char) * searchedVariableLength);
		if (searchedVariablesArray[i] == NULL) 
		{
			printf("Не удалось выделить память под имя переменной\n");
			return -4;
		}
		printf("%d. Введите имя переменной: ", i);
		if (scanf("%s", searchedVariablesArray[i]) != 1) 
		{
			printf("Не удалось считать имя переменной\n");
			return -8;
		}
		rewind(stdin);
		printf("\n");
	}
	printf("3) Начинаем поиск\n");

	CurrentCodeLine = (char*)malloc(sizeof(char) * MaxLineLength);
	Operator = (char*)malloc(sizeof(char) * MaxOperatorLength);
	if (CurrentCodeLine == NULL) 
	{
		printf("Не удалось выделить память под строку из файла\n");
		return -5;
	}
	if (Operator == NULL)
	{
		printf("Не удалось выделить память под строку оператора\n");
		return -6;
	}
	// Search all variables
	for (i = 0; i < searchedVariablesArrayLength; i++)
	{
		variable = searchedVariablesArray[i];
		printf("%d) %s:\n", i, variable);
		variableLength = GetLineLength(variable);
		LineNumber = 1;
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
						if (CurrentCodeLine[index] == '+' || CurrentCodeLine[index] == '-' ||
							CurrentCodeLine[index] == '/' || CurrentCodeLine[index] == '%' ||
							CurrentCodeLine[index] == '*' || CurrentCodeLine[index] == '^' ||
							CurrentCodeLine[index] == '|' || CurrentCodeLine[index] == '&' ||
							(CurrentCodeLine[index] == '=' && CurrentCodeLine[index - 1] != '=' &&
								CurrentCodeLine[index + 1] != '=' && CurrentCodeLine[index - 1] != '!'))
						{
							variableChanged = true;
						}
						if (CurrentCodeLine[index] == LastSymbAtOperator || CurrentCodeLine[index] == '\0')
						{
							if (variableChanged == true) 
							{
								Operator[OperatorIndex] = '\0';
								if (VariableFoundAtFunction == true)
								printf("Operator on line %d: %s%c\n", LineNumber, Operator, LastSymbAtOperator);
								else 
								printf("Operator on line %d: %s%s%c\n", LineNumber, variable ,Operator, LastSymbAtOperator);
							}
							OperatorIndex = 0;
							variableChanged = false;
							SearchingOperator = false;
							break;
						}
						Operator[OperatorIndex] = CurrentCodeLine[index];
						OperatorIndex++;
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
					if (ContainsAtLineResult >= 2)
					{
						prevIndex = ContainsAtLineResult;
						while (prevIndex > 0)
						{
							if (CurrentCodeLine[prevIndex] == '-')
							{
								if (decPrev)
								{
									printf("Operator on line %d: %s%s;\n", LineNumber, "--", variable);
									index = ContainsAtLineResult + variableLength;
									break;
								}
								else 
								{ 
									decPrev = true; 
								}
							}
							if (CurrentCodeLine[prevIndex] == '+')
							{
								if (incPrev)
								{
									printf("Operator on line %d: %s%s;\n", LineNumber, "++", variable);
									index = ContainsAtLineResult + variableLength;
									break;
								}
								else 
								{ 
									incPrev = true; 
								}
							} 
							else if (CurrentCodeLine[prevIndex] == ';') 
							{
								index = ContainsAtLineResult + variableLength;
								break;
							}

							prevIndex--;
						}

					}
					if (!decPrev && !incPrev) 
					{
						VariableFirstIndex = ContainsAtLineResult;
						VariableFoundAtFunction = false;
						while (VariableFirstIndex > 0)
						{
							VariableFirstIndex--;
							if (CurrentCodeLine[VariableFirstIndex] == '(')
							{
								VariableFoundAtFunction = true;
								break;
							}
							if (CurrentCodeLine[VariableFirstIndex] == ';')
							{
								break;
							}
						}
						if (VariableFoundAtFunction)
						{
							while (index > 0 && CurrentCodeLine[index-1] != ';')
							{
								index--;
							}
						}
						else
						{
							index = ContainsAtLineResult + variableLength;
						}
						LastSymbAtOperator = ';';
						SearchingOperator = true;
						for (OperatorIndex = 0; OperatorIndex < MaxOperatorLength; OperatorIndex++)
						{
							Operator[OperatorIndex] = '\0';
						}
						OperatorIndex = 0;
						
					}
					decPrev = false;
					incPrev = false;
				}
				else 
				{ 
					break; 
				}
			}
			LineNumber++;
		}
		if (fseek(InputFileThread, 0, SEEK_SET) != 0) 
		{
			printf("Не удалось перейти в начало файла\n");
			return -21;
		}
	}
	printf("Finish");
	system("pause");
	return 0;
}


int  GetLineLength(char* Line)
{
	int index = 0;

	while ((Line[index] != '\n') && (Line[index] != '\0') )
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
	char	CurrentLetter;
	int		LineIndex = 0;
	int		firstIndex;
	int		j;
	int		k;

	if (SubStringLength > LineLength - SearchFrom) 
	{
		return -2; 
	}

	for (j = SearchFrom; j < LineLength; j++)
	{
		CurrentLetter = Line[j];
		for (k = 0; k < SubStringLength; k++)
		{
			// first letter found -> check prev symb 
			if (CurrentLetter == SubString[k] && (Line[j - 1] == ' '
												  || Line[j - 1] == '\n' || Line[j - 1] == '\t'
												  || Line[j - 1] == '\0' || Line[j - 1] == '('
												  || Line[j - 1] == '+'  || Line[j - 1] == '-'))
																								
			{
				firstIndex = j;
				do
				{
					if (j == firstIndex + SubStringLength - 1)
					{
						// whole word found -> check next symb
						if (Line[j + 1] == '\0' || Line[j + 1] == ' '
							|| Line[j + 1] == '\n' || Line[j + 1] == '\t' || Line[j + 1] == '='
							|| Line[j + 1] == '%' || Line[j + 1] == '-' || Line[j + 1] == '/'
							|| Line[j + 1] == '+' || Line[j + 1] == '*' || Line[j + 1] == ';'
							|| Line[j + 1] == ')' || Line[j + 1] == ',' || Line[j + 1] == '^'
							|| Line[j + 1] == '&')
						{
							return firstIndex;
						}
					}
					j++;
					k++;
				} 
				while (Line[j] == SubString[k]);
				
			}
			else continue;
		}
	}
	return -1;
}

int getCountOfVariables(char* CFilePath) 
{	
	char*	CurrentCodeLine;

	int		lineLength;
	int		MaxLineLength = 255;
	int		numberOfVar = 0;
	int		i;
	int		countOfVariables = 0;
	int		searchFrom;
	Var*	types;
	int		countOfTypes = 9;
	int		VariablesInline;
	int		index = 0;
	int		indexPrev = 0;
	FILE*	inputThread;
	bool	isFunctionType;
	bool	isModVar;

	// <Variable' types>
	Var INT, SHORT, CHAR, FLOAT, DOUBLE, LONG, UNSIGNED, SIGNED, VOID;
	INT.VarName		 = "int";
	SHORT.VarName	 = "short";
	CHAR.VarName	 = "char";
	FLOAT.VarName	 = "float";
	DOUBLE.VarName	 = "double";
	LONG.VarName	 = "long";
	UNSIGNED.VarName = "unsigned";
	SIGNED.VarName	 = "signed";
	VOID.VarName	 = "void";

	INT.Length		=	strlen(INT.VarName);
	SHORT.Length	=	strlen(SHORT.VarName);
	CHAR.Length		=	strlen(CHAR.VarName);
	FLOAT.Length	=	strlen(FLOAT.VarName);
	DOUBLE.Length	=	strlen(DOUBLE.VarName);
	LONG.Length		=	strlen(LONG.VarName);
	UNSIGNED.Length =	strlen(UNSIGNED.VarName);
	SIGNED.Length	=	strlen(SIGNED.VarName);
	VOID.Length		=	strlen(VOID.VarName);
	// </Variable' types>

	if ((inputThread = fopen(CFilePath, "r")) == NULL)
	{
		printf("Файл %s не существует\n", TestCFilePath);
		return 1;
	}

	types = malloc(sizeof(Var) * countOfTypes);
	if (types == NULL)
	{
		printf("Не удалось выделить память под массив переменных\n");
		return -11;
	}
	types[0] = INT;		 types[1] = SHORT;	types[2] = CHAR;
	types[3] = FLOAT;	 types[4] = DOUBLE;	types[5] = LONG;
	types[6] = UNSIGNED; types[7] = SIGNED;	types[8] = VOID;

	CurrentCodeLine = (char*)malloc(sizeof(char) * MaxLineLength);
	if (CurrentCodeLine == NULL) 
	{
		printf("Не удалось выделить память под строку из файла\n");
		return -12;
	}
	while (fgets(CurrentCodeLine, MaxLineLength, inputThread) != NULL)
	{
		lineLength = GetLineLength(CurrentCodeLine);      
		for (i = 0; i < countOfTypes; i++) 
		{
			index = 0;
			searchFrom = 0;
			while ((searchFrom = VariableContains(CurrentCodeLine, lineLength, searchFrom, types[i].VarName, types[i].Length)) >= 0)
			{
				VariablesInline = 1;
				indexPrev = searchFrom - 1;
				searchFrom += types[i].Length;
				isFunctionType = false;
				isModVar = false;
				index = searchFrom;

				while (CurrentCodeLine[indexPrev] != NULL) 
				{
					if (CurrentCodeLine[indexPrev] == ';' || CurrentCodeLine[indexPrev] == NULL) break;
					if (CurrentCodeLine[indexPrev] == 'g')
						isModVar = true;
					indexPrev--;
				}
				if (isModVar == false) 
				{
					while (CurrentCodeLine[index] != '\0')
					{
						if (CurrentCodeLine[index] == ';') break;
						else if (CurrentCodeLine[index] == '(' || CurrentCodeLine[index] == ')')
						{
							isFunctionType = true;
						}
						else if (CurrentCodeLine[index] == ',')
						{
							VariablesInline++;
						}
						index++;
					}
					if (isFunctionType == false)
					{
						index = searchFrom;
						printf("\n%d) %s", numberOfVar, types[i].VarName);
						numberOfVar++;
						while (CurrentCodeLine[index] != ';' && CurrentCodeLine[index] != '=' && CurrentCodeLine[index] != '\0')
						{
							if (CurrentCodeLine[index] == ',')
							{
								printf("\n%d) %s ", numberOfVar, types[i].VarName);
								numberOfVar++;
							}
							else printf("%c", CurrentCodeLine[index]);
							index++;
						}
						countOfVariables += VariablesInline;
					}
				}
			}
		}
	}
	printf("\n");
	return countOfVariables;
}
