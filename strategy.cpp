#include "strategy.h"
#include "utiltool.h"
#include <fstream>
#include <string.h>
#include <sstream>
#include <stdlib.h>

using namespace std;

long Strategy::getTimeSpan(string* str) {
   long timespan = 0;
   std::vector<std::string> result= utiltool::string_split(*str,":");
   for(size_t i=0; i<result.size(); i++) {
      int number = atoi(result[i].c_str() );
      if(i == 0) {
         timespan += number*3600*1000;
      } else if(i == 1) {
         timespan += number*60*1000;
      } else if(i == 2) {
         timespan += number*1000;
      } else if(i == 3) {
         timespan += number;
      }
   }
   return timespan;
}

string Strategy::getUpdateTime(long span) {
   int hour = span/(3600*1000);
   stringstream ss;
   int remainder = span%(3600*1000);
   int minutes = remainder/(60*1000);
   int second =remainder%(60*1000)/1000;
   ss<<hour<<":"<<minutes<<":"<<second;
   string str = ss.str();
   return str;
}

/*
 * 0 表示第一次启动程序。
 * 1，2，3...表示不同的时间段
 * -1 表示该时间段不应该保存数据.
 */
/*
int Strategy::checkTime(string* str) {
   //exchange_time type;
   long timespan = getTimeSpan(str);

   for (size_t i = 0; i < contracts_info.size(); ++i) {
      if (timespan >= getTimeSpan(&(se[i].start_time)) && timespan <= getTimeSpan(&se[i].end_time)) {
         return (i + 1);
      }
   }
   return -1;
}
*/

void Strategy::OnTickData(TapAPIQuoteWhole *info) {	
   //计算账户的盈亏信息
   //cout << "in On Tick Data" << endl;
   //CalculateEarningsInfo(info); 

   cout << "策略收到行情：" << info->Contract.Commodity.CommodityNo << info->Contract.ContractNo1 << "," << info->DateTimeStamp << ",最新价:" << info->QLastPrice << ",涨停价:" << info->QLimitUpPrice << ",跌停价:" << info->QLimitDownPrice << endl;

   // 
   //stringstream ss;
   /*
      ss << info->UpdateTime<<":"<<info->UpdateMillisec;
      string new_time = ss.str();
    */
   //string new_time = info->DateTimeStamp;
   /*
   char new_time_t[32] = {0};
   memcpy(new_time_t, info->DateTimeStamp + 11, 12);
   new_time_t[8] = ':';
   string new_time = new_time_t;

   int new_type = checkTime(&new_time);
   if (type == 0) {
      type = new_type;
      last_time = new_time;
      memcpy(&last_data, info, sizeof(last_data));
   } else {
      if (type == new_type) {
         if (type != -1) {
            long new_timeSpan = getTimeSpan(&new_time);
            long last_timeSpan = getTimeSpan(&last_time);
            long span = new_timeSpan - last_timeSpan; 
            if (span == 500) {
               SaveDataVec(info);
            } else if (span > 0){ 
               cerr<<"需要补数据,时间差"<<span-500<<endl;
               while (true) {
                  last_timeSpan =last_timeSpan+500;
                  if(last_timeSpan >= new_timeSpan) {
                     memcpy(&last_data,info,sizeof(last_data));
                     SaveDataVec(info);
                     break;
                  } 
                  //string date = last_data.TradingDay;
                  double buy1price = last_data.QBidPrice[0];
                  int buy1vol = last_data.QBidQty[0];
                  double new1 = last_data.QLastPrice;
                  double sell1price = last_data.QAskPrice[0];
                  int sell1vol = last_data.QAskQty[0];
                  double vol = last_data.QTotalQty;
                  double openinterest = last_data.QPositionQty;//持仓量

                  FutureData futureData;
                  //futureData.date = date;
                  futureData.time = getUpdateTime(last_timeSpan);
                  //futureData.millisec = last_timeSpan%1000;
                  futureData.buy1price = buy1price;
                  futureData.buy1vol = buy1vol;
                  futureData.new1 = new1;
                  futureData.sell1price = sell1price;
                  futureData.sell1vol = sell1vol;
                  futureData.vol = vol;
                  futureData.openinterest = openinterest;

                  m_futureData_vec.push_back(futureData); 
                  //cur_len += span / 500 - 1;
               }
            }
            last_time = new_time;
            memcpy(&last_data,info,sizeof(TapAPIQuoteWhole));
         } else if (type == None){
            type = new_type;
            last_time = new_time;
            memcpy(&last_data,info,sizeof(TapAPIQuoteWhole));
         }
      }
   } 
*/
   //TDSpi_stgy->CancelOrder(info->UpdateTime, info->LastPrice); 
   //StrategyCalculate(info); 
}

//设置交易的合约代码
void Strategy::setInstId(string instId) {
   strcpy(m_instId, instId.c_str());
   /*
    * strcpy_s to strcpy.
    */ 
}

void Strategy::setQuote(Quote *_quote) {
   quote = _quote;
}

//策略主逻辑的计算，70条行情里涨了0.6元，则做多，下跌则做空
void Strategy::StrategyCalculate(TapAPIQuoteWhole *info) {
   cout << " ----------------In StrategyCalculate " << endl;//>>>>
   /*
      TThostFtdcInstrumentIDType    instId;//合约,合约代码在结构体里已经有了
      TThostFtdcDirectionType       dir;//方向,'0'买，'1'卖
      TThostFtdcCombOffsetFlagType  kpp;//开平，"0"开，"1"平,"3"平今
      TThostFtdcPriceType           price;//价格，0是市价,上期所不支持
      TThostFtdcVolumeType          vol;//数量


      if(m_futureData_vec.size() >= 70) {
   //持仓查询，进行仓位控制
   if(strcmp(info->InstrumentID, m_instId) == 0) {
   if(m_futureData_vec.size() % 20 == 0) {
   if(TDSpi_stgy->SendHolding_long(m_instId) + TDSpi_stgy->SendHolding_short(m_instId) < 3) {//多空共满仓3手 
   //做多
   if(m_futureData_vec[m_futureData_vec.size()-1].new1 - m_futureData_vec[m_futureData_vec.size()-70].new1 >= 0.6) {
   strcpy(instId, m_instId);
   dir = '0';
   strcpy(kpp, "0");
   price = info->LastPrice + 3;
   vol = 1;

   if(m_allow_open == true) {
   TDSpi_stgy->ReqOrderInsert(instId, dir, kpp, price, vol); 
   }
   } //做空
   else if(m_futureData_vec[m_futureData_vec.size()-70].new1 - m_futureData_vec[m_futureData_vec.size()-1].new1 >= 0.6) {
   strcpy(instId, m_instId);
   dir = '1';
   strcpy(kpp, "0");
   price = info->LastPrice - 3;
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
/*
void Strategy::SaveDataTxtCsv(TapAPIQuoteWhole *info) {
   cout << "-----------in SaveDataTxtCsv() ------------" << endl;
   //保存行情到txt
   string date = info->DateTimeStamp;

   string time = info->DateTimeStamp;
   double buy1price = info->QBidPrice[0];
   int buy1vol = info->QBidQty[0];
   double new1 = info->QLastPrice;
   double sell1price = info->QAskPrice[0];
   int sell1vol = info->QAskQty[0];
   double vol = info->QTotalQty;
   double openinterest = info->QPositionQty;//持仓量
   char instId_t[32] = {0};
   sprintf(instId_t, "%s%s", info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1);
   string instId = instId_t;
   //string instId = info->Contract.Commodity.CommodityNo;

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
}
*/

//保存数据到vector
/*
void Strategy::SaveDataVec(TapAPIQuoteWhole *info) {
   cout << "-----------in SaveDataVec() ------------" << endl;
   //string date = info->TradingDay;
   string time = info->DateTimeStamp;
   double buy1price = info->QBidPrice[0];
   int buy1vol = info->QBidQty[0];
   double new1 = info->QLastPrice;
   double sell1price = info->QAskPrice[0];
   int sell1vol = info->QAskQty[0];
   double vol = info->QTotalQty;
   double openinterest = info->QPositionQty;//持仓量

   //futureData.date = date;
   futureData.time = time;
   futureData.buy1price = buy1price;
   futureData.buy1vol = buy1vol;
   futureData.new1 = new1;
   futureData.sell1price = sell1price;
   futureData.sell1vol = sell1vol;
   futureData.vol = vol;
   futureData.openinterest = openinterest;

   m_futureData_vec.push_back(futureData);	
}
*/

void Strategy::set_instMessage_map_stgy(map<string, TapAPICommodityInfo*> instMessage_map_stgy) {
   //cout << instMessage_map_stgy.size() << endl;//>>>>
   map<string, TapAPICommodityInfo*> map_tmp;
   map_tmp = instMessage_map_stgy;
   m_instMessage_map_stgy = instMessage_map_stgy;
   cerr<<"收到合约个数:"<<m_instMessage_map_stgy.size()<<endl; 
}

//计算账户的盈亏信息
void Strategy::CalculateEarningsInfo(TapAPIQuoteWhole *info) {
   //更新合约的最新价，没有持仓就不需要更新，有持仓的，不是交易的合约也要更新。要先计算盈亏信息再计算策略逻辑

   cout << "-----------in CalculateEarningsInfo() ------------" << endl;
   /*
   //判断该合约是否有持仓
   if(TDSpi_stgy->send_trade_message_map_KeyNum(info->InstrumentID) > 0)
   TDSpi_stgy->setLastPrice(info->InstrumentID, info->LastPrice); 

   //整个账户的浮动盈亏,按开仓价算
   m_openProfit_account = TDSpi_stgy->sendOpenProfit_account(info->InstrumentID, info->LastPrice); 

   //整个账户的平仓盈亏
   m_closeProfit_account = TDSpi_stgy->sendCloseProfit();

   cerr<<" 平仓盈亏:"<<m_closeProfit_account<<",浮动盈亏:"<<m_openProfit_account<<"当前合约:"<<info->InstrumentID<<" 最新价:"<<info->LastPrice<<" 时间:"<<info->UpdateTime<<endl;//double类型为0有时候会是-1.63709e-010，是小于0的，但+1后的值是1

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

/*
void Strategy::setContractsInfo(vector<contract> &_contracts_info) {
   contracts_info = _contracts_info;
   cout << "收到的合约个数次数为:" << contracts_info.size() << endl;
   for (size_t i = 0; i < contracts_info.size(); ++i) {
      contracts_info[i].strg = this;
   }
}
*/

/*
void *handleData(void *arg) { 
   contract *con = (contract *)arg;
   while (1) {
      con->ev.WaitEvent();
      // TODO.
   }
}
*/

/*
 * 这个必须在调用了setContractsInfo之后调用.
 */
/*
bool Strategy::setDataReadThread() { 
   for (size_t i = 0; i < contracts_info.size(); ++i) {
      pthread_create(&contracts_info[i].pid, NULL, handleData, (void *)(&contracts_info[i]));
   }
   return true;
}
*/
