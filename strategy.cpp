#include "strategy.h"
#include <fstream>
#include <string.h>

using namespace std;

void Strategy::OnTickData(TapAPIQuoteWhole *pDepthMarketData) {	
   //计算账户的盈亏信息
   cout << "in On Tick Data" << endl;
   /*
   CalculateEarningsInfo(pDepthMarketData); 

   cout << pDepthMarketData->InstrumentID << endl;//>>>
   cout << m_instId << endl; //>>>>
   if(strcmp(pDepthMarketData->InstrumentID, m_instId) == 0) {
      cerr<<"策略收到行情:"<<pDepthMarketData->InstrumentID<<","<<pDepthMarketData->TradingDay<<","<<pDepthMarketData->UpdateTime<<",最新价:"<<pDepthMarketData->LastPrice<<",涨停价:"<<pDepthMarketData->UpperLimitPrice<<",跌停价:"<<pDepthMarketData->LowerLimitPrice<<endl;

      //保存数据到vector
      SaveDataVec(pDepthMarketData);

      //保存数据到txt和csv
      SaveDataTxtCsv(pDepthMarketData);

      //撤单追单,撤单成功后再追单
      TDSpi_stgy->CancelOrder(pDepthMarketData->UpdateTime, pDepthMarketData->LastPrice);

      //开仓平仓（策略主逻辑的计算）
      StrategyCalculate(pDepthMarketData); 
   } 
   */
}

//设置交易的合约代码
void Strategy::setInstId(string instId) {
   strcpy(m_instId, instId.c_str());
   /*
    * strcpy_s to strcpy.
    */ 
}

//策略主逻辑的计算，70条行情里涨了0.6元，则做多，下跌则做空
void Strategy::StrategyCalculate(TapAPIQuoteWhole *pDepthMarketData) {
   cout << " ----------------In StrategyCalculate " << endl;//>>>>
   /*
   TThostFtdcInstrumentIDType    instId;//合约,合约代码在结构体里已经有了
   TThostFtdcDirectionType       dir;//方向,'0'买，'1'卖
   TThostFtdcCombOffsetFlagType  kpp;//开平，"0"开，"1"平,"3"平今
   TThostFtdcPriceType           price;//价格，0是市价,上期所不支持
   TThostFtdcVolumeType          vol;//数量


   if(m_futureData_vec.size() >= 70) {
      //持仓查询，进行仓位控制
      if(strcmp(pDepthMarketData->InstrumentID, m_instId) == 0) {
         if(m_futureData_vec.size() % 20 == 0) {
            if(TDSpi_stgy->SendHolding_long(m_instId) + TDSpi_stgy->SendHolding_short(m_instId) < 3) {//多空共满仓3手 
               //做多
               if(m_futureData_vec[m_futureData_vec.size()-1].new1 - m_futureData_vec[m_futureData_vec.size()-70].new1 >= 0.6) {
                  strcpy(instId, m_instId);
                  dir = '0';
                  strcpy(kpp, "0");
                  price = pDepthMarketData->LastPrice + 3;
                  vol = 1;

                  if(m_allow_open == true) {
                     TDSpi_stgy->ReqOrderInsert(instId, dir, kpp, price, vol); 
                  }
               } //做空
               else if(m_futureData_vec[m_futureData_vec.size()-70].new1 - m_futureData_vec[m_futureData_vec.size()-1].new1 >= 0.6) {
                  strcpy(instId, m_instId);
                  dir = '1';
                  strcpy(kpp, "0");
                  price = pDepthMarketData->LastPrice - 3;
                  vol = 1;
                  if(m_allow_open == true) {
                     TDSpi_stgy->ReqOrderInsert(instId, dir, kpp, price, vol); 
                  }
               } 
            }
         } 
      } 
      //强平所有持仓
      if(m_futureData_vec.size() % 140 == 0)
         TDSpi_stgy->ForceClose();
   } 
*/
} 

//设置是否允许开仓
void Strategy::set_allow_open(bool x) {
   m_allow_open = x;

   if(m_allow_open == true) {
      cerr<<"当前设置结果：允许开仓"<<endl; 
   } else if(m_allow_open == false) {
      cerr<<"当前设置结果：禁止开仓"<<endl; 
   }

} 

//保存数据到txt和csv
void Strategy::SaveDataTxtCsv(TapAPIQuoteWhole *pDepthMarketData) {
   cout << "-----------in SaveDataTxtCsv() ------------" << endl;
   //保存行情到txt
   /*
   string date = pDepthMarketData->TradingDay;
   string time = pDepthMarketData->UpdateTime;
   double buy1price = pDepthMarketData->BidPrice1;
   int buy1vol = pDepthMarketData->BidVolume1;
   double new1 = pDepthMarketData->LastPrice;
   double sell1price = pDepthMarketData->AskPrice1;
   int sell1vol = pDepthMarketData->AskVolume1;
   double vol = pDepthMarketData->Volume;
   double openinterest = pDepthMarketData->OpenInterest;//持仓量 

   string instId = pDepthMarketData->InstrumentID;

   //更改date的格式
   string a = date.substr(0,4);
   string b = date.substr(4,2);
   string c = date.substr(6,2);

   string date_new = a + "-" + b + "-" + c;

   char output_file[128];
   bzero(output_file, 128);
   sprintf(output_file, "./output/%s_%s.txt", instId.c_str(), date.c_str());
   ofstream fout_data(output_file, ios::app);
   //ofstream fout_data("output/" + instId + "_" + date + ".txt",ios::app);

   fout_data<<date_new<<","<<time<<","<<buy1price<<","<<buy1vol<<","<<new1<<","<<sell1price<<","<<sell1vol<<","<<vol<<","<<openinterest<<endl;

   fout_data.close(); 

   bzero(output_file, 128);
   sprintf(output_file, "./output/%s_%s.csv", instId.c_str(), date.c_str());
   ofstream fout_data_csv(output_file, ios::app);
   //ofstream fout_data_csv("output/" + instId + "_" + date + ".csv",ios::app);

   fout_data_csv<<date_new<<","<<time<<","<<buy1price<<","<<buy1vol<<","<<new1<<","<<sell1price<<","<<sell1vol<<","<<vol<<","<<openinterest<<endl;

   fout_data_csv.close(); 
   */
}

//保存数据到vector
void Strategy::SaveDataVec(TapAPIQuoteWhole *pDepthMarketData) {
   cout << "-----------in SaveDataVec() ------------" << endl;
   /*
   string date = pDepthMarketData->TradingDay;
   string time = pDepthMarketData->UpdateTime;
   double buy1price = pDepthMarketData->BidPrice1;
   int buy1vol = pDepthMarketData->BidVolume1;
   double new1 = pDepthMarketData->LastPrice;
   double sell1price = pDepthMarketData->AskPrice1;
   int sell1vol = pDepthMarketData->AskVolume1;
   double vol = pDepthMarketData->Volume;
   double openinterest = pDepthMarketData->OpenInterest;//持仓量

   futureData.date = date;
   futureData.time = time;
   futureData.buy1price = buy1price;
   futureData.buy1vol = buy1vol;
   futureData.new1 = new1;
   futureData.sell1price = sell1price;
   futureData.sell1vol = sell1vol;
   futureData.vol = vol;
   futureData.openinterest = openinterest;

   m_futureData_vec.push_back(futureData);	
   */
}

void Strategy::set_instMessage_map_stgy(map<string, TapAPITradeContractInfo*> instMessage_map_stgy) {
   cout << instMessage_map_stgy.size() << endl;//>>>>
   map<string, TapAPITradeContractInfo*> map_tmp;
   map_tmp = instMessage_map_stgy;
   for (map<string, TapAPITradeContractInfo*>::iterator it = map_tmp.begin(); it != map_tmp.end(); ++it) {
      cout << it->first << endl;//>>>
      cout << it->second << endl;//>>>
   }
   m_instMessage_map_stgy = instMessage_map_stgy;
   cerr<<"收到合约个数:"<<m_instMessage_map_stgy.size()<<endl; 
}

//计算账户的盈亏信息
void Strategy::CalculateEarningsInfo(TapAPIQuoteWhole *pDepthMarketData) {
   //更新合约的最新价，没有持仓就不需要更新，有持仓的，不是交易的合约也要更新。要先计算盈亏信息再计算策略逻辑

   cout << "-----------in CalculateEarningsInfo() ------------" << endl;
   /*
   //判断该合约是否有持仓
   if(TDSpi_stgy->send_trade_message_map_KeyNum(pDepthMarketData->InstrumentID) > 0)
   TDSpi_stgy->setLastPrice(pDepthMarketData->InstrumentID, pDepthMarketData->LastPrice); 

   //整个账户的浮动盈亏,按开仓价算
   m_openProfit_account = TDSpi_stgy->sendOpenProfit_account(pDepthMarketData->InstrumentID, pDepthMarketData->LastPrice); 

   //整个账户的平仓盈亏
   m_closeProfit_account = TDSpi_stgy->sendCloseProfit();

   cerr<<" 平仓盈亏:"<<m_closeProfit_account<<",浮动盈亏:"<<m_openProfit_account<<"当前合约:"<<pDepthMarketData->InstrumentID<<" 最新价:"<<pDepthMarketData->LastPrice<<" 时间:"<<pDepthMarketData->UpdateTime<<endl;//double类型为0有时候会是-1.63709e-010，是小于0的，但+1后的值是1

   TDSpi_stgy->printTrade_message_map(); 
    */
}

//读取历史数据
void Strategy::GetHistoryData() {
   vector<string> data_fileName_vec;
   Store_fileName("./input/历史K线数据", data_fileName_vec); 
   cout<<"历史K线文本数:"<<data_fileName_vec.size()<<endl;

   for(vector<string>::iterator iter = data_fileName_vec.begin(); iter != data_fileName_vec.end(); iter++) {
      cout<<*iter<<endl;
      ReadDatas(*iter, history_data_vec); 
   } 
   cout<<"K线条数:"<<history_data_vec.size()<<endl; 
}
