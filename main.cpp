#include "Quote.h"
#include "Trade.h" 
#include "strategy.h"
#include "config.h"

#include <iostream>
#include <string.h>
#include <unistd.h>
using namespace std;

int requestId=0;//请求编号
Strategy* g_strategy;//策略类指针
Trade *g_pUserSpi_tradeAll; //全局的TD回调处理类对象，人机交互函数需用到

void *hcInteraction(void *arg); 
void usage() {
   cout << "Usage : ./trading [path_to_config_file]" << endl;
}

int main(int argc, char **argv) {
   //取得API的版本信息
   if (argc != 2) {
      usage();
      return -1;
   }

   Configure config;
   memset(&config, 0, sizeof(config));
   loadConfigureFile(argv[argc - 1], config);
   
   //创建API实例
   int errorcode = 0;
   cout << GetITapTradeAPIVersion() << endl;
   cout << GetTapQuoteAPIVersion() << endl;//>>>>
   ITapQuoteAPI *p_mdApi = Quote::createTapQuoteApi(config.auth_code_md, config.log_path, errorcode);
   if (NULL == p_mdApi){
      cout << "创建Quote API实例失败，错误码：" << errorcode <<endl;
      return -1;
   }

   //设定ITapQuoteAPINotify的实现类，用于异步消息的接收
   Quote *objQuote = new Quote();
   p_mdApi->SetAPINotify(objQuote); 

   objQuote->SetAPI(p_mdApi);
   objQuote->setFront(config.ip_md, config.port_md);
   objQuote->setAccount(config.username_md, config.passwd_md);
   objQuote->setTreatys(config.all_treatys);

   //--------------初始化交易UserApi，创建交易API实例----------------------------------
   //cout << GetTapTradeAPIVersion() << endl;
   ITapTradeAPI *ptradeAPI = Trade::createTapTradeApi(config.auth_code_td, config.log_path, errorcode);
   if (NULL == ptradeAPI){
      cout << "创建Trade API实例失败，错误码：" << errorcode <<endl;
      return -1;
   }
   Trade *objTrade = new Trade(ptradeAPI, p_mdApi, objQuote);
   ptradeAPI->SetAPINotify(objTrade); 
   objTrade->setFront(config.ip_td, config.port_td);
   objTrade->setAccount(config.username_td, config.passwd_td);

   g_pUserSpi_tradeAll = objTrade;//全局的TD回调处理类对象，人机交互函数需用到
   //--------------创建策略实例--------------------------------------------------------
   g_strategy = new Strategy(objTrade);
   g_strategy->setQuote(objQuote);

   // 先启动TD线程
   bool ret = objTrade->init();
   if (!ret) {
      cout << "error happen in initial!" << endl;
      return -1;
   }
   // 再启动MD线程.
   //--------------人机交互模块--------------------------------------------------------
   pthread_t pid;
   pthread_create(&pid, NULL, hcInteraction, NULL);

   sleep(3);
   ret = objQuote->init();
   if (ret) {
      cout << "error happen in initial!" << endl;
      return -1;
   } 

   while(1) {
   }
   return 0;
}

void *hcInteraction(void *) {
   string str;

   int a;
   cerr<<"--------------------------------------------------------人机交互功能已启动"<<endl;
   cerr<<endl<<"请输入指令(查看持仓:show,强平持仓:close,允许开仓:yxkc, 禁止开仓:jzkc)：" << endl;//>>;

   while(1) {
      cin>>str;
      if(str == "show")
         a = 0;   
      else if(str == "close")
         a = 1;
      else if(str == "yxkc")
         a = 2;
      else if(str == "jzkc")
         a = 3;
      else
         a = 4;

      switch(a){
      case 0:
         {     
            cerr<<"查看账户持仓:"<<endl;
            g_pUserSpi_tradeAll->printTrade_message_map();
            cerr<<"请输入指令(查看持仓:show,强平持仓:close,允许开仓:yxkc, 禁止开仓:jzkc)："<<endl;
            break;
         }
      case 1:
         {
            cerr<<"强平账户持仓:"<<endl;
            g_pUserSpi_tradeAll->forceClose();
            cerr<<"请输入指令(查看持仓:show,强平持仓:close,允许开仓:yxkc, 禁止开仓:jzkc)："<<endl;
            break;
         }
      case 2:
         {
            cerr<<"允许开仓:"<<endl;
            g_strategy->set_allow_open(true);
            cerr<<"请输入指令(查看持仓:show,强平持仓:close,允许开仓:yxkc, 禁止开仓:jzkc)："<<endl;
            break; 
         }
      case 3:
         {
            cerr<<"禁止开仓:"<<endl;
            g_strategy->set_allow_open(false);
            cerr<<"请输入指令(查看持仓:show,强平持仓:close,允许开仓:yxkc, 禁止开仓:jzkc)："<<endl;
            break; 
         }
      case 4:
         {
            cerr<<endl<<"输入错误，请重新输入指令(查看持仓:show,强平持仓:close,允许开仓:yxkc, 禁止开仓:jzkc)："<<endl;
            break;
         } 
      } 
   } 
   return 0;
} 
