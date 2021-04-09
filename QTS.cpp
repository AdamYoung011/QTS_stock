#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <math.h>
#include "stock_cal.h"

using namespace std;
#define stock_number 30
#define generation 1000
#define particle_number 10
#define rotate_angle 0.001
#define rotate_angle_up 0.004
#define rotate_angle_down 0.004

vector <double> probability, trend;
vector <int> result[particle_number];
vector <int> index[particle_number], result_max;
int big = 0, small = 0;
vector <int> stock_choose_number;
vector <double> money[particle_number], expected_return, risk;
double trend_max = 0, expected_return_max = 0, risk_max = 0, initial_investment = 10000000;
int gen_max = 0;
vector <double>  remain_money[particle_number], each_money[particle_number];
vector <int> stock_buy_number[particle_number];
vector <double>  remain_money_max, each_money_max, money_max;
vector <int> stock_buy_number_max, index_max;
int stock_choose_number_max = 0;

//初始化機率陣列(全部皆為0.5)
void initial_probability()
{
	for (int i = 0; i < stock_number; i++)
	{
		probability.push_back(0.5);
	}
}

//random probability進行比較
void measure()
{
	double a = 0;
	
	for (int i = 0; i < particle_number; i++)
	{
		int choose_number_count = 0;
		for (int j = 0; j < stock_number; j++)
		{
			a = rand() / 32767.0;
			if (a < probability[j])
			{
				result[i].push_back(1);
				choose_number_count++;
				index[i].push_back(j);
			}
			else
			{
				result[i].push_back(0);
			}
		}
		stock_choose_number.push_back(choose_number_count);
	}
}

//計算fitness
void fitness_cal(int day,vector <string> name,vector <double> d,int gen)
{
	vector <double>  stock_price;
	double divide_remain_money;
	int divide_money;
	string str;
	
	int time_ch = 0;
	int times = day + 1;

	for (int i = 0; i < particle_number; i++)
	{
		for (int j = 0; j < day; j++)
		{
			if (time_ch == 0)
			{
				divide_money = initial_investment / stock_choose_number[i];

				for (int h = 0; h < stock_choose_number[i]; h++)
				{
					stock_price.push_back(d[index[i][h]]);
					stock_buy_number[i].push_back(divide_money / stock_price[h]);
					remain_money[i].push_back(divide_money - stock_price[h] * stock_buy_number[i][h]);
					if (stock_choose_number[i] > 1)
					{
						each_money[i].push_back(divide_money);
					}
				}
				divide_remain_money = initial_investment - divide_money * stock_choose_number[i];

				money[i].push_back(initial_investment);
			}
			else
			{
				double tmp = 0;
				for (int h = 0; h < stock_choose_number[i]; h++)
				{
					if (stock_choose_number[i] > 1)
					{
						each_money[i] .push_back( d[index[i][h] + name.size() * time_ch] * stock_buy_number[i][h] + remain_money[i][h]);
						tmp = tmp + each_money[i][h + j * stock_choose_number[i]];
						if (h == stock_choose_number[i] - 1)
						{
							tmp = tmp + divide_remain_money;
						}
					}
					else
					{
						tmp = tmp + d[index[i][h] + name.size() * time_ch] * stock_buy_number[i][h] + remain_money[i][h];
					}
				}
				money[i].push_back(tmp);
				tmp = 0;
			}
			stock_price.clear();
			time_ch++;
		}
		expected_return.push_back(expected_return_cal(money[i], day + 1, initial_investment));
		risk.push_back(risk_cal(expected_return[i], times, initial_investment, money[i]));
		trend.push_back(trend_ratio_cal(expected_return[i], risk[i]));
		time_ch = 0;
	}

}

//找fitness最大及最小
void find_max_min(int gen, int day)
{
	for (int i = 0; i < particle_number; i++)
	{
		if (i == 0)
		{
			big = i;
			small = i;
		}
		else
		{
			if (trend[i] > trend[big])
			{
				big = i;
			}
			else if (trend[i] < trend[small])
			{
				small = i;
			}
		}
	}
	if (trend[big] > trend_max)
	{
		trend_max = trend[big];
		expected_return_max = expected_return[big];
		risk_max = risk[big];
		gen_max = gen;
		stock_choose_number_max = stock_choose_number[big];

		if (stock_choose_number_max > 1)
		{
			each_money_max.clear();
			for (int j = 0; j < day; j++)
			{
				for (int k = 0; k < stock_choose_number_max; k++)
				{
					each_money_max.push_back(each_money[big][k + j * stock_choose_number_max]);
				}
			}
		}
		remain_money_max.clear();
		stock_buy_number_max.clear();
		index_max.clear();
		money_max.clear();
		result_max.clear();
		for (int k = 0; k < stock_number; k++)
		{
			result_max.push_back(result[big][k]);
		}
		for (int k = 0; k < stock_choose_number_max; k++)
		{
			remain_money_max.push_back(remain_money[big][k]);
			stock_buy_number_max.push_back(stock_buy_number[big][k]);
			index_max.push_back(index[big][k]);
		}
		for (int j = 0; j < day; j++)
		{
			money_max.push_back(money[big][j]);
		}
	}
}

//更新機率
void update()
{
	for (int i = 0; i < stock_number; i++)
	{
		if (result[big][i] - result[small][i] == 1)
		{
			probability[i] = probability[i] + 0.001;
		}
		else if (result[big][i] - result[small][i] == -1)
		{
			probability[i] = probability[i] - 0.001;
		}
	}
}

void output_data(vector <string> name, vector <double> d, int day)
{
	ofstream output("train_2019_06(2019 Q1)_QTS_1000代_10粒子_0.001_實驗1.csv");
	output << fixed << setprecision(15);

	output << "代數," << generation << endl;
	output << "粒子數," << particle_number << endl;
	output << "旋轉角度上界," << rotate_angle_up << endl;
	output << "旋轉角度下界," << rotate_angle_down << endl;
	output << "旋轉角度," << rotate_angle << endl;
	output << "實驗次數," << 1 << endl << endl;
	output << "初始資金," << initial_investment << endl;
	output << "最後資金," << money_max[day - 1] << endl;
	output << "真實報酬," << money_max[day - 1] - initial_investment << endl << endl;
	output << "預期報酬," << expected_return_max << endl;
	output << "風險," << risk_max << endl;
	output << "起點值," << trend_max << endl;
	output << "找到最佳解世代," << gen_max << endl << endl;

	output << "投資組合," << stock_choose_number_max << endl;
	for (int p = 0; p < stock_choose_number_max; p++)
	{
		output << name[index_max[p]];
		if (p == stock_choose_number_max - 1)
		{
			output << endl;
		}
		else
		{
			output << ",";
		}
	}
	output << "Stock#,";
	for (int p = 0; p < stock_choose_number_max; p++)
	{
		output << name[index_max[p]];
		if (p == stock_choose_number_max - 1)
		{
			output << endl;
		}
		else
		{
			output << ",";
		}
	}
	output << "張數,";
	for (int p = 0; p < stock_choose_number_max; p++)
	{
		output << stock_buy_number_max[p];
		if (p == stock_choose_number_max - 1)
		{
			output << endl;
		}
		else
		{
			output << ",";
		}
	}
	output << "分配資金,";
	for (int p = 0; p < stock_choose_number_max; p++)
	{
		output << int (initial_investment / stock_choose_number_max);
		if (p == stock_choose_number_max - 1)
		{
			output << endl;
		}
		else
		{
			output << ",";
		}
	}
	output << "剩餘資金,";
	for (int p = 0; p < stock_choose_number_max; p++)
	{
		output << remain_money_max[p];
		if (p == stock_choose_number_max - 1)
		{
			output << endl;
		}
		else
		{
			output << ",";
		}
	}
	for (int i = 0; i < day; i++)
	{
		output << "FS(" << i + 1 << "),";
		if (stock_choose_number_max > 1)
		{
			for (int p = 0; p < stock_choose_number_max; p++)
			{
				output << each_money_max[p + i * stock_choose_number_max];
				output << ",";
			}
			output << money_max[i] << endl;
		}
		else
		{
			output << money_max[i] << endl;
		}

	}
	output.close();
}


int main()
{
	initial_probability();

	ifstream in("train_2019_06(2019 Q1).csv");
	string str;
	int times = 0, day = 0;
	vector <string> name;
	vector <double> d;

	srand(114);
	while (in >> str)
	{
		data_processing(name, d, str, times);
		times++;
	}
	day = times - 1;

	for (int i = 0; i < generation; i++)
	{
		measure();
		fitness_cal(day,name,d,0);
		find_max_min(i+1,day);
		update();
		for (int h = 0; h < particle_number; h++)
		{
			result[h].clear();
			index[h].clear();
			money[h].clear();
			stock_buy_number[h].clear();
			remain_money[h].clear();
			each_money[h].clear();
		}
		stock_choose_number.clear();
		expected_return.clear();
		risk.clear();
		trend.clear();
		
	}
	output_data(name, d, day);
}