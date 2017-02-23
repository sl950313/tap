#ifndef TREATY_H_
#define TREATY_H_

#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <strings.h>
#include <pthread.h>
#include "StructFunction.h"
#include "utiltool.h"
#include "SimpleEvent.h"
#include "StructFunction.h"
#include "include/TapQuoteAPIDataType.h"

//合约类
class treaty {
public:
   //treaty(void);
   //treaty(char *Code, char *first_start_time, char *first_end_time, char *second_start_time, char *second_end_time, char *third_start_time, char *third_end_time, char *fourth_start_time, char *fourth_end_time) : treaty_id(Code), first_start_time(first_start_time), first_end_time(first_end_time), second_start_time(second_start_time), second_end_time(second_end_time), third_start_time(third_start_time), third_end_time(third_end_time), fourth_start_time(fourth_start_time), fourth_end_time(fourth_end_time) {
   treaty() {
      bzero(&_info, sizeof(_info) );
      log_file = NULL;
      cur_log_level = DEBUG; // For Debug.
      init = false;
      quote_t.clear();
      pthread_mutex_init(&queue_mutex, NULL);
      end = -1;
      use_for_strategy = NULL;
      array_size_for_strategy = 0;
      total_data_len = 0;
   }

   ~treaty(void); 
   int checkTime(string*); 
   void receiveData(TapAPIQuoteWhole*); 
   long getTimeSpan(string*);

   //是否初始化
   bool init; 
   //合约ID
   string treaty_id; 
   //上次接受数据时间
   string last_time; 
   TapAPIQuoteWhole last_data; 
   //交易时间枚举
   int type; 
   map<long,FutureData> m_futureData_map;//保存行情数据的map,如果用指针需要new后再保存 

   //保存数据到hash_map
   void SaveDataMap(TapAPIQuoteWhole *_info); 
   void SaveDataVec(TapAPIQuoteWhole *info, long timespan);
   void SaveTxt(TapAPIQuoteWhole*); 
   void SaveDataTxtCsv(TapAPIQuoteWhole *info);
   //保存数据到map
   void SaveDataMap(long,FutureData*); 
   string getUpdateTime(long span); 
   /*
    * @shilei 
    * 2017.01.06
    */
   bool startPthread();
   void LOG(int log_level, char *info);
   void setArraySizeAndAllocMem(long array_size);

   //friend void *handleData(void *);

   /*
    * @shilei Add cache depth data.
    * 2017.01.06
    */
   TapAPIQuoteWhole _info;
   unsigned int head, tail;
   pthread_mutex_t head_lock; // Add lock for head. maybe not needed.  

   std::queue<TapAPIQuoteWhole> quote_queue;
   pthread_mutex_t queue_mutex;
   TapAPIQuoteWhole quotewhole;
   vector<FutureData> m_futureData_vec;//保存行情数据的vector,如果用指针需要new后再保存
   //vector<FutureData> use_for_strategy; // 一定长度的数据。保存当前合约的数据.
   FutureData *use_for_strategy; // 一定长度的数据。保存当前合约的数据.
   long array_size_for_strategy; // use_for_strategy数据的长度，默认是10240.可配置.
   /*
    * 每次新加入数据的时候end++, 如果end 达到了array_size_for_strategy, 那么就会begin++,然后end回到开头
    */
   long end;//数据的开头和结尾. 
   // int begin;
   long total_data_len; // 不使用begin, 使用当前数组的长度.
   

   /*
    * @shilei. add event and pthread to each contract.
    * 2017.01.05
    */
   SimpleEvent ev;
   pthread_t handle_thread;
   char exchange_name[16];
   char commodity[16];
   char contract_no[16];
   int quote_interval; // 接受到合约的间隔. 125ms, 250ms or 500ms.  125 is not used.
   std::vector<start_end_time> se;
   std::vector<TapAPIQuoteWhole > quote_t;
   //FILE *output_file;

   char _log_file[32];
   FILE *log_file;
   int cur_log_level;
};
#endif
