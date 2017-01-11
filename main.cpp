#include "Quote.h"
#include "Trade.h" 
#include "strategy.h"

#include <iostream>
#include <string.h>
#include <unistd.h>
using namespace std;

int requestId=0;//请求编号
Strategy* g_strategy;//策略类指针
Trade *g_pUserSpi_tradeAll; //全局的TD回调处理类对象，人机交互函数需用到

void *hcInteraction(void *arg);

int main() {
   //取得API的版本信息

   char authCode[] = "67EA896065459BECDFDB924B29CB7DF1946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC5211AF9FEE541DDE41BCBAB68D525B0D111A0884D847D57163FF7F329FA574E7946CED32E26C1EAC946CED32E26C1EAC733827B0CE853869ABD9B8F170E14F8847D3EA0BF4E191F5D97B3DFE4CCB1F01842DD2B3EA2F4B20CAD19B8347719B7E20EA1FA7A3D1BFEFF22290F4B5C43E6C520ED5A40EC1D50ACDF342F46A92CCF87AEE6D73542C42EC17818349C7DEDAB0E4DB16977714F873D505029E27B3D57EB92D5BEDA0A710197EB67F94BB1892B30F58A3F211D9C3B3839BE2D73FD08DD776B9188654853DDA57675EBB7D6FBBFC"; 
   
   //创建API实例
   int errorcode = 0;
   cout << GetTapQuoteAPIVersion() << endl;
   ITapQuoteAPI *p_mdApi = Quote::createTapQuoteApi(authCode, "", errorcode);
   if (NULL == p_mdApi){
      cout << "创建API实例失败，错误码：" << errorcode <<endl;
      return -1;
   }

   //设定ITapQuoteAPINotify的实现类，用于异步消息的接收
   Quote *objQuote = new Quote();
   p_mdApi->SetAPINotify(objQuote); 

   objQuote->SetAPI(p_mdApi);
   objQuote->setFront("222.88.40.170", 6161);
   objQuote->setAccount("ESUNNY", "Es123456");

   //--------------初始化交易UserApi，创建交易API实例----------------------------------
   cout << GetTapTradeAPIVersion() << endl;
   ITapTradeAPI *ptradeAPI = Trade::createTapTradeApi(authCode, "", errorcode);
   if (NULL == ptradeAPI){
      cout << "创建API实例失败，错误码：" << errorcode <<endl;
      return -1;
   }
   Trade *objTrade = new Trade(ptradeAPI, p_mdApi, objQuote);
   ptradeAPI->SetAPINotify(objTrade); 
   objTrade->setFront("123.15.58.21", 6060);
   objTrade->setAccount("ESUNNY", "Es123456");

   // 先启动TD线程
   bool ret = objTrade->init();
   if (!ret) {
      cout << "error happen in initial!" << endl;
      return -1;
   }
   // 再启动MD线程.
   objQuote->init();

   //--------------人机交互模块--------------------------------------------------------
   pthread_t pid;
   pthread_create(&pid, NULL, hcInteraction, NULL);

   while(1) {
   }
   return 0;
}

void *hcInteraction(void *) {
   string str;

   int a;
   cerr<<"--------------------------------------------------------人机交互功能已启动"<<endl;
   cerr<<endl<<"请输入指令(查看持仓:show,强平持仓:close,允许开仓:yxkc, 禁止开仓:jzkc)：";

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
            g_pUserSpi_tradeAll->printTradeMessageMap();
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
