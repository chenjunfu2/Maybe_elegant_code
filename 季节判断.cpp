#include <stdio.h>

void GetYMD(int &iYear, int &iMonth, int &iDay)
{
	constexpr static const int iDaysOfMonth[2][12] =//[0][n-1]����ƽ��n�����·��� [1][n-1]��������n�����·��������ʽ��������·��ж�����
	{
		{31,28,31,30,31,30,31,31,30,31,30,31,},
		{31,29,31,30,31,30,31,31,30,31,30,31,},
	};

	printf("������������:");

	while (true)
	{
		while (scanf("%d%d%d", &iYear, &iMonth, &iDay) != 3)
		{
			while (getchar() != '\n')//����һ��
			{
				continue;
			}
			printf("�����������������:");
		}

		while (getchar() != '\n')//����ʣ������
		{
			continue;
		}

		//�ж�����
		bool bLeapYear = (iYear % 4 == 0 && iYear % 100 != 0) || iYear % 400 == 0;

		if (iYear < 0)
		{
			printf("��ݴ�������������:\n");
			continue;
		}

		if (iMonth < 1 || iMonth > 12)
		{
			printf("�·ݴ�������������:\n");
			continue;
		}

		if (iDay < 1 || iDay > iDaysOfMonth[bLeapYear][iMonth - 1])//ע��������iMonth - 1
		{
			printf("���ڴ�������������:\n");
			continue;
		}

		break;//������ȷ���˳�
	}
}

const char* Season(int iYear, int iMonth, int iDay)
{
	/*
	����		3��20����6��20��
	�ļ�		6��21����9��21��
	�＾		9��22����12��20��
	����		12��21��������3��19��
	*/

	constexpr static const struct
	{
		int iMonthBeg;//�·���ʼֵ(������)�����ж���ȡ���Ž��и����ж�
		int iDayBeg;//������ʼֵ(������)�����ж�
		int iMonthEnd;//�·���ֵֹ(������)�����ж���ȡ���Ž��и����ж�
		int iDayEnd;//������ֵֹ(������)�����ж�
		const char *pSeasonName;//��������
		bool bAnd;//�ջ����ǿ������ջ�(true)�ж�ֵ�ڱ������ڳ���������(false)�ж�ֵ�ڱ����������
	}
	stSeason[] =//�ṹ�����飬�Զ����С
	{
		{ 3,20, 6,20,"����",true},// [ 3.20, 6.20]����
		{ 6,21, 9,21,"�ļ�",true},// [ 6.21, 9.21]����
		{ 9,22,12,20,"�＾",true},// [ 9.22,12,20]����
		{12,21, 3,19,"����",false},//[12.21,+��)||(-��,3.19]����
	};

	const char *ret = nullptr;
	for (int i = 0; i < sizeof(stSeason) / sizeof(stSeason[0]); ++i)
	{
		//�ж��·��������ȣ�ֻ���·�����ȡ��ʱ�ж�����
		bool bBeg = iMonth > stSeason[i].iMonthBeg || (iMonth == stSeason[i].iMonthBeg && iDay >= stSeason[i].iDayBeg);//�ж���ʼ�·ݺ������Ƿ������������
		bool bEnd = iMonth < stSeason[i].iMonthEnd || (iMonth == stSeason[i].iMonthEnd && iDay <= stSeason[i].iDayEnd);//�ж���ֹ�·ݺ������Ƿ����ұ�������
		bool bOpt = stSeason[i].bAnd ? bBeg && bEnd : bBeg || bEnd;//���ݿ��ջ�״̬�����ʼֵ����ֵֹ�ǰ������仹���ų�����

		if (bOpt)//��������״̬�ж��Ƿ�����·�����ѭ��
		{
			ret = stSeason[i].pSeasonName;
			break;
		}
	}

	return ret;
}

/*
��������
for (int m = 1; m <= 12; ++m)
{
	printf("m:%d\n", m);
	for (int d = 1; d <= 31; ++d)
	{
		printf("d:%d,%s ", d, Season(0, m, d));
	}
	putchar('\n');
}
*/

int main(void)
{
	int y, m, d;
	while (true)
	{
		GetYMD(y, m, d);
		printf("%s\n", Season(y, m, d));
	}
	return 0;
}