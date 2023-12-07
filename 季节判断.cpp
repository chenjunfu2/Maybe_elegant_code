#include <stdio.h>

void GetYMD(int &iYear, int &iMonth, int &iDay)
{
	constexpr static const int iDaysOfMonth[2][12] =//[0][n-1]就是平年n就是月份数 [1][n-1]就是闰年n就是月份数，访问结果是这个月份有多少天
	{
		{31,28,31,30,31,30,31,31,30,31,30,31,},
		{31,29,31,30,31,30,31,31,30,31,30,31,},
	};

	printf("请输入年月日:");

	while (true)
	{
		while (scanf("%d%d%d", &iYear, &iMonth, &iDay) != 3)
		{
			while (getchar() != '\n')//清理一行
			{
				continue;
			}
			printf("输入错误，请重新输入:");
		}

		while (getchar() != '\n')//清理剩余内容
		{
			continue;
		}

		//判断闰年
		bool bLeapYear = (iYear % 4 == 0 && iYear % 100 != 0) || iYear % 400 == 0;

		if (iYear < 0)
		{
			printf("年份错误，请重新输入:\n");
			continue;
		}

		if (iMonth < 1 || iMonth > 12)
		{
			printf("月份错误，请重新输入:\n");
			continue;
		}

		if (iDay < 1 || iDay > iDaysOfMonth[bLeapYear][iMonth - 1])//注意这里是iMonth - 1
		{
			printf("日期错误，请重新输入:\n");
			continue;
		}

		break;//输入正确，退出
	}
}

const char* Season(int iYear, int iMonth, int iDay)
{
	/*
	春季		3月20日至6月20日
	夏季		6月21日至9月21日
	秋季		9月22日至12月20日
	冬季		12月21日至次年3月19日
	*/

	constexpr static const struct
	{
		int iMonthBeg;//月份起始值(闭区间)优先判定，取到才进行附加判断
		int iDayBeg;//日期起始值(闭区间)附加判定
		int iMonthEnd;//月份终止值(闭区间)优先判定，取到才进行附加判断
		int iDayEnd;//日期终止值(闭区间)附加判定
		const char *pSeasonName;//季节名称
		bool bAnd;//闭环还是开环，闭环(true)判断值在闭区间内成立，开环(false)判断值在闭区间外成立
	}
	stSeason[] =//结构体数组，自动求大小
	{
		{ 3,20, 6,20,"春季",true},// [ 3.20, 6.20]成立
		{ 6,21, 9,21,"夏季",true},// [ 6.21, 9.21]成立
		{ 9,22,12,20,"秋季",true},// [ 9.22,12,20]成立
		{12,21, 3,19,"冬季",false},//[12.21,+∞)||(-∞,3.19]成立
	};

	const char *ret = nullptr;
	for (int i = 0; i < sizeof(stSeason) / sizeof(stSeason[0]); ++i)
	{
		//判断月份区间优先，只在月份区间取到时判断日期
		bool bBeg = iMonth > stSeason[i].iMonthBeg || (iMonth == stSeason[i].iMonthBeg && iDay >= stSeason[i].iDayBeg);//判断起始月份和日期是否在左闭区间内
		bool bEnd = iMonth < stSeason[i].iMonthEnd || (iMonth == stSeason[i].iMonthEnd && iDay <= stSeason[i].iDayEnd);//判断终止月份和日期是否在右闭区间内
		bool bOpt = stSeason[i].bAnd ? bBeg && bEnd : bBeg || bEnd;//根据开闭环状态组合起始值和终止值是包括区间还是排除区间

		if (bOpt)//根据最终状态判断是否输出月份跳出循环
		{
			ret = stSeason[i].pSeasonName;
			break;
		}
	}

	return ret;
}

/*
暴力测试
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