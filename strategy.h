#ifndef _STRATEGY_H
#define _STRATEGY_H

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Trade.h"
#include "StructFunction.h"



class Strategy
{
public:
	Strategy(Trade* TDSpi):TDSpi_stgy(TDSpi) {
		m_allow_open = false;
		GetHistoryData(); 
	}

	//行情回调函数，每收到一笔行情就触发一次
	void OnTickData(TapAPIQuoteWhole *pDepthMarketData);

	//设置交易的合约代码
	void setInstId(string instId);
	
	//策略主逻辑的计算，70条行情里涨了0.6元，则做多，下跌则做空（系统默认禁止开仓，可在界面输入"yxkc"来允许开仓）
	void StrategyCalculate(TapAPIQuoteWhole *pDepthMarketData);

	//设置是否允许开仓
	void set_allow_open(bool x);
	
	//保存行情数据到vector
	void SaveDataVec(TapAPIQuoteWhole *pDepthMarketData);
	
	//保存行情数据到txt和csv
	void SaveDataTxtCsv(TapAPIQuoteWhole *pDepthMarketData);

	//设置合约-合约信息结构体的map
	void set_instMessage_map_stgy(map<string, TapAPITradeContractInfo *> instMessage_map_stgy);

	//计算账户的盈亏信息
	void CalculateEarningsInfo(TapAPIQuoteWhole *pDepthMarketData);
	
	//读取历史数据
	void GetHistoryData();


private:

	Trade* TDSpi_stgy;//TD指针

	char m_instId[31];//合约代码

	FutureData futureData;//自定义的行情数据结构体

	vector<FutureData> m_futureData_vec;//保存行情数据的vector,如果用指针需要new后再保存

	bool m_allow_open;//TRUE允许开仓，FALSE禁止开仓
		
	map<string, TapAPITradeContractInfo*> m_instMessage_map_stgy;//保存合约信息的map,通过set_instMessage_map_stgy()函数从TD复制
	
	double m_openProfit_account;//整个账户的浮动盈亏,按开仓价算
	
	double m_closeProfit_account;//整个账户的平仓盈亏

	vector<History_data> history_data_vec;//保存历史数据的vector

};

#endif
