#include "treaty.h"
#include "include/TapAPICommDef.h"
#include "include/TapQuoteAPIDataType.h"
#include <stdlib.h>
#include <string.h>


//合约时间数据用逗号分割
TapAPIQuoteWhole last_data;



/*
treaty::treaty(void) {
   init = false;
   log_file = NULL;
   printf("0cur_log_level = %d", cur_log_level);
   cur_log_level = DEBUG; // For Debug.
   printf("0cur_log_level = %d", cur_log_level);
}
*/


treaty::~treaty(void) {
   if (log_file) {
      fclose(log_file);
   }
   free(use_for_strategy);
}


//检查时间sh在时间段
int treaty::checkTime(string* str) {
   long timespan = getTimeSpan(str); 
   for (size_t i = 0; i < se.size(); ++i) {
      if (timespan >= getTimeSpan(&(se[i].start_time)) && timespan <= getTimeSpan(&se[i].end_time)) {
         return (i + 1);
      }
   }
   return -1;
}

//获得时间戳
long treaty::getTimeSpan(string* str) {
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
         for (int j = 1; j < 1000 / quote_interval; ++j) {
            if (number < quote_interval * j) {
               timespan += quote_interval * j;
               break;
            }
         }
      }
   }
   return timespan;
}

char *getLevelStr(int LOG_LEVEL) {
   if (LOG_LEVEL == INFO) return (char *)"INFO";
   if (LOG_LEVEL == WARNING) return (char *)"WARNING";
   if (LOG_LEVEL == ERROR) return (char *)"ERROR";
   if (LOG_LEVEL == DEBUG) return (char *)"DEBUG";
   return (char *)"WTF";
}

void treaty::LOG(int LOG_LEVEL, char *info) {
   if (LOG_LEVEL > cur_log_level) {
      return ;
   }
   char *log_level = getLevelStr(LOG_LEVEL);
   fprintf(log_file, "%s : %s\n", log_level, info);
   fflush(log_file);
   //printf("----------------------in LOG. log_level = %s, info = %s\n", log_level, info);
}

//接受数据
void treaty::receiveData(TapAPIQuoteWhole* info) {
   char log[512];
   bzero(log, 512);
   //printf("receive Data : %s, UpdateTime : %d", info->InstrumentID, info->UpdateMillisec);
   sprintf(log, "receive Data : %s%s, UpdateTime : %s", info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1, info->DateTimeStamp);
   LOG(DEBUG, log);
   /*
   stringstream ss;
   ss << info->UpdateTime<<":"<<info->UpdateMillisec;
   */
   //cout << "UpdateTime:" << data->UpdateMillisec << endl;//>>>>>
   char new_time_t[32] = {0};
   memcpy(new_time_t, info->DateTimeStamp + 11, 12);
   new_time_t[8] = ':';
   string new_time = new_time_t;
   if(init == false) {
      bzero(log, 512);
      sprintf(log, "init = false, last_time = %s", info->DateTimeStamp);
      LOG(DEBUG, log);
      last_time = new_time;
      //cout <<"last time = " << last_time << endl;//>>>>>>
      //memcpy(&last_data,info,sizeof(TapAPIQuoteWhole));
      TapAPIQuoteWhole q_t;//= new TapAPIQuoteWhole();

      memcpy(&q_t, info, sizeof(TapAPIQuoteWhole));
      quote_t.push_back(q_t);
      type = checkTime(&last_time);
      //cout << "type = " << type << endl;//>>>>>>
      if(type != -1) {
         SaveDataVec(info, getTimeSpan(&new_time));
      }
      memcpy(&last_data,info,sizeof(last_data));
      init = true;
   } else {
      /*
         string new_time = ss.str();
         exchange_time new_type = checkTime(&new_time);
       */
      int new_type = checkTime(&new_time);
      bzero(log, 512);
      sprintf(log, "init = true. last_time : %s,new_time : %s, new_type : %d, type : %d", last_time.c_str(), new_time.c_str(), new_type, type);
      LOG(DEBUG, log);
      if (new_type != type) {
         //开始和结束数据
         bzero(log, 512);
         sprintf(log, "new_type != type, 开始和结束数据, new_type = %d, type = %d", new_type, type);
         LOG(DEBUG, log);
         if(new_type == -1) {
            type = new_type;
            bzero(log, 512);
            sprintf(log, "new_type != type, new_type == NONE");
            LOG(DEBUG, log);
            //SaveDataVec(info);
            SaveDataTxtCsv(info);
         } else {
            type = new_type;
            bzero(log, 512);
            sprintf(log, "new_type != type, new_type != NONE");
            LOG(DEBUG, log);
            last_time = new_time;
            TapAPIQuoteWhole q_t;
            memcpy(&q_t, info, sizeof(TapAPIQuoteWhole));
            quote_t.push_back(q_t); 
            //SaveDataMap(data);
         }
      } else if(new_type == type) {
         if(type != -1) {
            long new_timeSpan = getTimeSpan(&new_time);
            long last_timeSpan = getTimeSpan(&last_time);
            //cout << "new_timeSpan = " << new_timeSpan << ". last_timeSpan = " << last_timeSpan << endl;
            bzero(log, 512);
            sprintf(log, "new_timeSpan = %ld, last_timeSpan = %ld", new_timeSpan, last_timeSpan);
            LOG(DEBUG, log);
            long span = new_timeSpan - last_timeSpan;
            if (span == 0) {
               TapAPIQuoteWhole q_t;
               memcpy(&q_t, info, sizeof(TapAPIQuoteWhole));
               quote_t.push_back(q_t);
            }
            if(span == quote_interval) {  //不缺少数据
               //SaveDataMap(info);
               SaveDataVec(info, new_timeSpan);
               memcpy(&last_data,info,sizeof(last_data));
            } else if(span > 0) { //需要补数据 
               cerr<<"需要补数据,时间差"<<span-quote_interval<<endl;
               bzero(log, 512);
               sprintf(log, "需要补数据,时间差 : %ld", span - quote_interval);
               LOG(INFO, log);
               while(true) {
                  last_timeSpan =last_timeSpan+quote_interval;
                  if(last_timeSpan >= new_timeSpan) {
                     SaveDataVec(info, new_timeSpan);
                     //SaveTxt(info); 
                     //SaveDataMap(info);
                     memcpy(&last_data,info,sizeof(last_data));
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
                  memcpy(&(use_for_strategy[(++end) % array_size_for_strategy]), &futureData, sizeof(futureData));
                  total_data_len = (total_data_len >= array_size_for_strategy) ? array_size_for_strategy : total_data_len + 1;

                  //m_futureData_map.insert(map<long, FutureData>::value_type(last_timeSpan,futureData)); 
                  m_futureData_vec.push_back(futureData);
               }
            }
            if (span > 0) {
               last_time = new_time;
               memcpy(&last_data,info,sizeof(TapAPIQuoteWhole));
            }
         } else if(type == -1) {
            type = new_type;
            last_time = new_time;
            memcpy(&last_data,info,sizeof(TapAPIQuoteWhole));
         }
         bzero(log, 512);
         sprintf(log, "m_futureData_map.size = %ld", m_futureData_map.size());
         LOG(DEBUG, log);
         //cout << "m_futureData_map.size = " << m_futureData_map.size() << endl;//>>>>>>
         /*
            if(m_futureData_map.size() > 10) {
            SaveTxt(info);
            }
          */
      }
      if (m_futureData_vec.size() > 10) {
         SaveTxt(info);
      }
      //SaveDataTxtCsv(info);
   }
}

void treaty::SaveTxt(TapAPIQuoteWhole* data) {
   cout << "Save Txt running" << endl;//>>>>
   string date = data->DateTimeStamp;
   date = date.substr(0, date.find(' '));
   cout << "data = " << date << endl;//>>>>>>
   //更改date的格式
   string a = date.substr(0,4);
   string b = date.substr(4,2);
   string c = date.substr(6,2);

   string date_new = a + b + c;
   char instId_t[32] = {0};
   sprintf(instId_t, "%s%s%s", data->Contract.Commodity.ExchangeNo, data->Contract.Commodity.CommodityNo, data->Contract.ContractNo1);
   string instId = instId_t;

   char output_file[512];
   bzero(output_file, 512); 
   sprintf(output_file, "./output/%s_%s.txt", instId.c_str(), date.c_str());
   printf("output_file = %s\n", output_file);
   char log[512];
   bzero(log, 512);
   sprintf(log, "output_file = %s", output_file);
   LOG(DEBUG, log);
   FILE *fp = fopen(output_file, "a+");
   if (!fp) {
      printf("file open error\n");
      return ;
   }

   map<long, FutureData>::iterator i,iend,j;
   iend = m_futureData_map.end();

   //vector<FutureData> vec;
   for (size_t i = 0; i < m_futureData_vec.size(); ++i) {
      FutureData f_data = (m_futureData_vec[i]);
      //vec.push_back(f_data);

      string date = f_data.date;
      string time = f_data.time;
      //int  millisec = f_data.millisec;
      double buy1price = f_data.buy1price;
      int buy1vol = f_data.buy1vol;
      double new1 = f_data.new1;
      double sell1price = f_data.sell1price;
      int sell1vol = f_data.sell1vol;
      double vol = f_data.vol;
      double openinterest = f_data.openinterest;//持仓量


      fprintf(fp, "%s,%f,%d,%f,%f,%d,%f,%f\n", time.c_str(), buy1price, buy1vol, new1, sell1price, sell1vol, vol, openinterest);
      //fout_data<<date_new<<","<<time<<":"<<millisec<<","<<buy1price<<","<<buy1vol<<","<<new1<<","<<sell1price<<","<<sell1vol<<","<<vol<<","<<openinterest<<endl;
      cout<<time<<":"<<","<<buy1price<<","<<buy1vol<<","<<new1<<","<<sell1price<<","<<sell1vol<<","<<vol<<","<<openinterest<<endl;
   }
   fflush(fp);
   fclose(fp);
   //fout_data.close();
   m_futureData_map.clear();
   m_futureData_vec.clear();
   cerr<<"数据保存成功!"<<endl;
   bzero(log, 512);
   sprintf(log, "数据保存成功!");
   LOG(INFO, log);
}

void treaty::SaveDataTxtCsv(TapAPIQuoteWhole *info) {
   cout << "-----------in SaveDataTxtCsv() ------------" << endl;
   //保存行情到txt
   string date = info->DateTimeStamp;
   date = date.substr(0, date.find(' '));

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

   string date_new = a + b + c;

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

void treaty::SaveDataVec(TapAPIQuoteWhole *info, long timespan) {
   cout << "-----------in SaveDataVec() ------------" << endl;
   FutureData futureData;
   //futureData.date = date;
   futureData.time = getUpdateTime(timespan);
   futureData.buy1price = info->QBidPrice[0];
   futureData.buy1vol = info->QBidQty[0];
   futureData.new1 = info->QLastPrice;
   futureData.sell1price = info->QAskPrice[0];
   futureData.sell1vol = info->QAskQty[0];
   futureData.vol = info->QTotalQty;
   futureData.openinterest = info->QPositionQty;//持仓量;

   for (size_t i = 0; i < quote_t.size(); ++i) { 
      futureData.vol += quote_t[i].QTotalQty;
   }
   quote_t.clear();
   m_futureData_vec.push_back(futureData);
   //end = (end + 1) % array_size_for_strategy;
   memcpy(&(use_for_strategy[(++end) % array_size_for_strategy]), &futureData, sizeof(futureData));
   total_data_len = (total_data_len >= array_size_for_strategy) ? array_size_for_strategy : total_data_len + 1;
   char log[128];
   memset(log, 0, 128);
   sprintf(log, "use_for_strategy len : %ld, end : %ld", total_data_len, end);
   LOG(DEBUG, log);
}

//保存数据到map
/*
   void treaty::SaveDataMap(TapAPIQuoteWhole *pDepthMarketData) {  
//cout << "in SaveDataMap" << endl;//>>>>
string date = pDepthMarketData->TradingDay;
string time = pDepthMarketData->UpdateTime;
int  millisec = pDepthMarketData->UpdateMillisec;
double buy1price = pDepthMarketData->BidPrice1;
int buy1vol = pDepthMarketData->BidVolume1;
double new1 = pDepthMarketData->LastPrice;
double sell1price = pDepthMarketData->AskPrice1;
int sell1vol = pDepthMarketData->AskVolume1;
double vol = pDepthMarketData->Volume;
double openinterest = pDepthMarketData->OpenInterest;//持仓量

FutureData futureData;
futureData.date = date;
futureData.time = time;
futureData.millisec = millisec;
futureData.buy1price = buy1price;
futureData.buy1vol = buy1vol;
futureData.new1 = new1;
futureData.sell1price = sell1price;
futureData.sell1vol = sell1vol;
futureData.vol = vol;
futureData.openinterest = openinterest;


long timespan = getTimeSpan(&last_time);
m_futureData_map.insert(map<long, FutureData>::value_type(timespan,futureData)); 
}
 */

//保存数据到map
void treaty::SaveDataMap(long timespan,FutureData *futureData) {
   m_futureData_map.insert(map<long,FutureData>::value_type(timespan,*futureData));

}

string treaty::getUpdateTime(long span) {
   long hour = span/(3600*1000);
   stringstream ss;
   long remainder = span%(3600*1000);
   long minutes = remainder/(60*1000);
   long second =remainder%(60*1000)/1000;
   long usecond = remainder % 1000;
   ss<<hour<<":"<<minutes<<":"<<second<<":"<< usecond;
   string str = ss.str();
   return str;
}

void treaty::setArraySizeAndAllocMem(long array_size) {
   array_size_for_strategy = array_size;
   use_for_strategy = (FutureData *)malloc(sizeof(FutureData) * array_size_for_strategy);
}

void *handleData(void *arg) {
   treaty *tv = (treaty *)arg;
   while (1) {
      tv->ev.WaitEvent();
      tv->receiveData(&(tv->quotewhole));
   }
}

/*
 * @shilei
 * 2017.01.06
 */
bool treaty::startPthread() {
   int ret = pthread_create(&handle_thread, NULL, handleData, this);
   return (ret == 0) ? true : false;
}
