#ifndef _STRUCTFUNCTION_H
#define _STRUCTFUNCTION_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;


//交易信息的结构体
struct trade_message
{
	trade_message()
	{
		instId = "";
		lastPrice = 0.0;
		PreSettlementPrice = 0.0;
		holding_long = 0;
		holding_short = 0;
		TodayPosition_long = 0;
		YdPosition_long = 0;
		TodayPosition_short = 0;
		YdPosition_short = 0;

		closeProfit_long = 0.0;
		OpenProfit_long = 0.0;
		closeProfit_short = 0.0;
		OpenProfit_short = 0.0;
	}


	string instId;//合约代码
	double lastPrice;//最新价，时刻保存合约的最新价，平仓用
	double PreSettlementPrice;//上次结算价，对隔夜仓有时候要用，快期有用
	int holding_long;//多单持仓量
	int holding_short;//空单持仓量

	int TodayPosition_long;//多单今日持仓
	int YdPosition_long;//多单上日持仓

	int TodayPosition_short;//空单今日持仓
	int YdPosition_short;//空单上日持仓

	double closeProfit_long;//多单平仓盈亏
	double OpenProfit_long;//多单浮动盈亏

	double closeProfit_short;//空单平仓盈亏
	double OpenProfit_short;//空单浮动盈亏

};




struct FutureData//行情结构体定义
{
	string date;
	string time;
	double buy1price;
	int buy1vol;
	double new1;
	double sell1price;
	int sell1vol;
	double vol;
	double openinterest;//持仓量

};



//历史K线
struct History_data
{
	string date;
	string time;
	double buy1price;//买一
	double sell1price;//卖一
	double open;
	double high;
	double low;
	double close;

};



//读取历史K线
void ReadDatas(string fileName, vector<History_data> &history_data_vec);



int Store_fileName(string path, vector<string> &FileName);

#endif