#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long ULONGLONG;

int GetWeekWithDate(ULONGLONG year, ULONGLONG month, ULONGLONG day)
{
	if (month == 1 || month == 2)
	{
		year -= 1;
		month += 12;
	}

	return (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7 + 1;
}

bool IsLeapYear(ULONGLONG year)
{
	return (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0));
}

int GetDaysInMonth(ULONGLONG year, ULONGLONG month)
{
	if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
		return 31;
	if (month == 4 || month == 6 || month == 9 || month == 11)
		return 30;
	if (month == 2)
		return IsLeapYear(year) ? 29 : 28;

	return -1;
}

int GetDaysInYear(ULONGLONG year)
{
	return IsLeapYear(year) ? 366 : 365;
}

int main(void)
{
	int y, m;
	printf("请输入\"年 月\":");
	scanf("%d %d", &y, &m);
	system("cls");

	printf("%d年 %d月\n\n", y, m);
	printf("一 二 三 四 五 六 日\n");
	printf("--------------------\n");

	int w = GetWeekWithDate(y, m, 1) - 1;
	printf("%*c", w * 3, ' ');

	for (int i = 1; i < GetDaysInMonth(y, m) + 1; ++i)
	{
		if (GetWeekWithDate(y, m, i) == 1)
		{
			putchar('\n');
		}
		printf("%2d ", i);
	}

	putchar('\n');
	putchar('\n');
	system("pause");
}
