#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>

const char* TestCFilePath = "test.c";
const char* LogFile = "log.txt";

int  GetLineLength(char* line);
void FindAllVariableChanges(char* line, char** variablesArray, int variablesArrayLenght, int lineNumber);

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
	while (fgets(CurrentCodeLine, MaxLineLength, InputFileThread) != NULL)
	{
		FindAllVariableChanges(CurrentCodeLine, searchedVariablesArray, searchedVariablesArrayLength, LineNumber);
		LineNumber++;
	}
	
	getchar();
	return 0;
}


int  GetLineLength(char* line)
{
	int index = 0;
	while ((line[index] != NULL) && (line[index] != '\n') && (line[index] != '\0') )
	{
		index++;
	}
	return index;
}

void FindAllVariableChanges(char* line, char** variablesArray, int variablesArrayLenght, int lineNumber)
{
	int LineLength = GetLineLength(line);
	int VariableLength;
	char currentLetter;

	for (int i = 0; i < variablesArrayLenght; i++)
	{
		VariableLength = GetLineLength(variablesArray[i]);
		for (int j = 0; j < LineLength; j++)
		{
			currentLetter = line[j];
			for (int k = 0; k < VariableLength; k++)
			{
				if (currentLetter == variablesArray[i][k]) // first founded
				{
					int firstIndex = j;
					j++;
					k++;
					while (line[j] == variablesArray[i][k]) 
					{
						j++;
						k++;
						if (j == firstIndex + VariableLength)
						{
							while (line[j] == ' ') 
							{ 
								if (line[j] == NULL) break;
								j++; 
							}
							char Operator[2]; 
							Operator[0] = line[j];
							Operator[1] = line[j + 1];
							if ((Operator[0] == '+' && Operator[1] == '=') 
							|| (Operator[0] == '+' && Operator[1] == '+')
							|| (Operator[0] == '%' && Operator[1] == '=')
							|| (Operator[0] == '-' && Operator[1] == '=')
							|| (Operator[0] == '='))
								printf("%s Найдено изменение в строке %d оператор <%c%c>\n", variablesArray[i], lineNumber, Operator[0], Operator[1]);
							break;
						}
					}
				}
			}
		}
	}
}