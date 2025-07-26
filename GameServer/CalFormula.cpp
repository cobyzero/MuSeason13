#include "stdafx.h"
#include "MemScript.h"
#include "Util.h"
#include "CalFormula.h"

CalFormula gCalFormula;

CalFormula::CalFormula()
{
	this->m_FormulaInfo.clear();
}


CalFormula::~CalFormula()
{
}
void CalFormula::Load(char* path) // OK
{
#if(GAMESERVER_UPDATE>=1220)

	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_FormulaInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					FORMULA_INFO info;

					info.ID = lpMemScript->GetNumber();

					strcpy_s(info.value, lpMemScript->GetAsString());

					this->m_FormulaInfo.insert(std::pair<int, FORMULA_INFO>(info.ID, info));
				}

				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

#endif
}
bool CalFormula::GetFormulaInfo(int ID, FORMULA_INFO * lpInfo)
{
	std::map<int, FORMULA_INFO>::iterator it = this->m_FormulaInfo.find(ID);

	if (it == this->m_FormulaInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}
float  CalFormula::CalculationFormula(int ID, int level)
{
	char *str = (char*)calloc(sizeof(char) * 100, sizeof(char));
	FORMULA_INFO info;
	if (this->GetFormulaInfo(ID, &info) == 0)
	{
		return 0;
	}

	int a = 0;
	int b = 0;
	while (true)
	{
		if (info.value[a]=='\0')
		{
			break;
		}
		if (info.value[a] == 'x')
		{
			_itoa_s(level, &str[b],sizeof(str) ,10);
			//_itoa(level, &str[b], 10);
			while (true)
			{
				if (str[b] != '\0')
				{
					b++;
				}
				else
				{
					a++;
					break;
				}
			}

		}
		else
		{
			str[b] = info.value[a];
			a++;
			b++;
		}

	}
	float operand[1024] = { 0 };                /*����ջ����ʼ��*/
	int  top_num = -1;

	char oper[1024] = { 0 };                  /*������ջ����ʼ��*/
	int top_oper = -1;

	char* temp;
	char dest[1024];

	float num = 0;
	int i = 0;

	while (*str != '\0')
	{
		temp = dest;
		while ((*str >= '0' && *str <= '9') || *str=='.')        /*�ж��Ƿ�������*/
		{
			*temp = *str;
			str++;
			temp++;
		}                               /*���������˳�*/

		if (*str != '(' && *(temp - 1) != '\0')      /*�жϷ����Ƿ�Ϊ'('*/
		{
			*temp = '\0';
			//num = atoi(dest);               /*���ַ���תΪ����*/
			num = atof(dest);
			this->insert_operand(operand, &top_num, num);      /*������ѹ������ջ*/
		}

		while (1)
		{
			i = this->compare(oper, &top_oper, *str);      /*�жϲ��������ȼ�*/
			if (i == 0)
			{
				this->insert_oper(oper, &top_oper, *str);   /*ѹ�������*/
				break;
			}

			else if (i == 1)                         /*�ж������ڵı��ʽ�Ƿ����*/
			{
				str++;
			}

			else if (i == -1)                        /*�������ݴ���*/
			{
				this->deal_date(operand, oper, &top_num, &top_oper);
			}
		}
		str++;                 /*ָ����ʽ��һ���ַ�*/
	}
	return operand[0];
}
int  CalFormula::insert_operand(float *operand, int * top_num, float num)           /*����ѹ������ջ*/
{
	(*top_num)++;
	operand[*top_num] = num;                    /*��������*/

	return 0;                           /*�����˳�*/
}


int  CalFormula::insert_oper(char * oper, int *top_oper, char ch)             /*������ѹ�����ջ*/
{
	(*top_oper)++;
	oper[*top_oper] = ch;                       /*���������*/
	return 0;                           /*�����˳�*/
}

int  CalFormula::compare(char *oper, int *top_oper, char ch)                   /*�Ƚϲ��������ȼ�*/
{
	if ((oper[*top_oper] == '-' || oper[*top_oper] == '+')           /*�жϵ�ǰ���ȼ��Ƿ��ջ�����������ȼ���*/
		&& (ch == '*' || ch == '/'))
	{
		return 0;                      /*������ѹ��ջ*/
	}
	else if (*top_oper == -1 || ch == '(' || (oper[*top_oper] == '(' && ch != ')'))  /*�жϲ�����ջ�Ƿ�Ϊ�գ�ջ������   ���Ƿ�Ϊ'('*/
	{
		return 0;                       /*������ѹ��ջ*/
	}
	else if (oper[*top_oper] == '(' && ch == ')')       /*�ж������ڵı��ʽ�Ƿ�������*/
	{
		(*top_oper)--;
		return 1;                       /*�ԣ������д���*/
	}
	else
	{
		return -1;                                          /*���в�����������*/
	}
}


int  CalFormula::deal_date(float *operand, char *oper, int *top_num, int *top_oper)    /*������������*/
{
	float num_1 = operand[*top_num];              /*ȡ������ջ����������*/
	float num_2 = operand[*top_num - 1];

	float value = 0;

	if (oper[*top_oper] == '+')                  /*�ӷ�����*/
	{
		value = num_1 + num_2;
	}

	else if (oper[*top_oper] == '-')             /*��������*/
	{
		value = num_2 - num_1;
	}

	else if (oper[*top_oper] == '*')             /*�˷�����*/
	{
		value = num_2 * num_1;
	}

	else if (oper[*top_oper] == '/')             /*��������*/
	{
		value = num_2 / num_1;
	}
	else if (oper[*top_oper] == '^')
	{
		value = pow(num_2, num_1);
	}
	(*top_num)--;                              /*������ջ������һλ*/
	operand[*top_num] = value;                  /*���õ���ֵѹ������ջ*/
	(*top_oper)--;                             /*��������ջ������һλ*/
	return 0;
}
