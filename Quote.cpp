#include "Quote.h"
#include "./include/TapAPIError.h"
//#include "QuoteConfig.h"
//#include <Windows.h>
#include <iostream>
#include <string.h>
#include "strategy.h"

using namespace std;

extern Strategy* g_strategy;

Quote::Quote(void):
	m_pApi(NULL),
	m_bIsAPIReady(false),
	is_last(false) {
	   memset(username, 0, 64);
	   memset(passwd, 0, 64);
	   memset(ip, 0, 32);
	}


Quote::~Quote(void) {}


void Quote::SetAPI(ITapQuoteAPI *pAPI) {
	m_pApi = pAPI;
}

void Quote::setFront(char *_ip, int _port) {
   strcpy(ip, _ip);
   port = _port;
}

void Quote::setAccount(char *_username, char *_passwd) {
   strcpy(username, _username);
   strcpy(passwd, _passwd);
}

void Quote::setTreatys(vector<treaty *> &treatys) {
   all_treatys = treatys;
   cout << "all treatys size : " << all_treatys.size() << endl;

   for (size_t i = 0; i < all_treatys.size(); ++i) {
      all_treatys[i]->startPthread();
   }
}

/*
 * @shilei.
 * 2017.01.09
 */
int Quote::init() {
   if(NULL == m_pApi) {
      cout << "Error: m_pApi is NULL." << endl;
      return -1;
   }
   TAPIINT32 iErr = TAPIERROR_SUCCEED;
   iErr = m_pApi->SetHostAddress(ip, port);
   cout << "IP : " << ip << endl;//>>>>>>
   cout << "Port : " << port << endl;//>>>>>>
   if(TAPIERROR_SUCCEED != iErr) {
      cout << "SetHostAddress Error:" << iErr <<endl;
      return -1;
   }
   TapAPIQuoteLoginAuth loginAuth;
   memset(&loginAuth, 0, sizeof(loginAuth));
   strcpy(loginAuth.UserNo, username);
   strcpy(loginAuth.Password, passwd);
   loginAuth.ISModifyPassword = APIYNFLAG_NO;
   loginAuth.ISDDA = APIYNFLAG_NO;
   cout << "Username : " << loginAuth.UserNo << endl;//>>>>>>
   cout << "Password : " << loginAuth.Password << endl;//>>>>>>

   // TODO:
   // 登陆.
   bool err = login(&loginAuth);
   if (!err) {
      cerr << "login error." << endl;
      return -1;
   }
   cout << "login success" << endl;

   QryCommodity();
   QryContract();
   // 订阅行情.
   /*
   char* exchange_no = "NYMEX";
   char *commodity_no = "CL";
   char *contract_no = "1706";
   */
   SubscribeQuote();
   return 0;
}


bool Quote::login(TapAPIQuoteLoginAuth *loginAuth) {
   TAPIINT32 err = TAPIERROR_SUCCEED;
   err = m_pApi->Login(loginAuth);
   if (err != TAPIERROR_SUCCEED) {
      cout << "Login error." << " errorCode = " << err << endl;
      return false;
   }
   cout << "connect server success" << endl; 

   /*
    * Login()函数调用成功只是代表于服务器建立了链路连接，只有回调OnLogin()的返回能指示用户是否成功登录了。 
    * 所以我们需要等待OnRspLogin()的回调.
    */
   m_Event.WaitEvent();
   if (!m_bIsAPIReady) {
      return false;
   }
   return true;
}

void TAP_CDECL Quote::OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info) {
   if(TAPIERROR_SUCCEED == errorCode) {
      cout << "登录成功，等待API初始化..." << endl;
      m_bIsAPIReady = true; 
      /*
       * @shilei
       * Because if OnRspLogin success. the function OnAPIReady() will be called soonly, and the signal event will be called.
       * 2017.01.08.
       */
   } else {
      cout << "登录失败，错误码:" << errorCode << endl;
      m_Event.SignalEvent();	
   }
}

void TAP_CDECL Quote::OnAPIReady() {
   cout << "API初始化完成" << endl;
   m_Event.SignalEvent();	
}

/*
 * @shilei
 * 2017.01.08.
 */
void Quote::disconnect() {
   m_pApi->Disconnect();
}

void TAP_CDECL Quote::OnDisconnect(TAPIINT32 reasonCode) {
   cout << "API断开,断开原因:"<<reasonCode << endl;
}

/*
 * @shilei. From the TapQuoteAPI docs, the function of ChangePassword and OnRspChangePassword has not been implement yet.
 * 2017.01.08
 */
void Quote::changePassword(char *old_passwd, char *new_passwd) {
   unsigned int sessionID = 0;
   TapAPIChangePasswordReq req;
   strcpy(req.OldPassword, old_passwd);
   strcpy(req.NewPassword, new_passwd);
   //m_pApi->ChangePassword(&sessionID, &req);
}

void TAP_CDECL Quote::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode) {
   cout << __FUNCTION__ << " is called." << endl;
}

/*
 * @shilei
 * 2017.01.08
 */
void Quote::QryExchange() {
   unsigned int *sessionID = 0;
   //int errcode = m_pApi->QryExchange(sessionID);
   //isErrorCode(errcode);  
}

/*
 * @shilei. here we may do some work.
 * 2017.01.08
 */
void TAP_CDECL Quote::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
   if (errorCode != 0) {
      cerr << "QryExchange Error. " << "ErrorCode : " << errorCode << endl;
      return ;
   }
   //is_last = isLast;
   cout << "Exchange Info : " << info << endl;
}

/*
 * @shilei
 * 2017.01.08
 */
bool Quote::QryCommodity() {
   unsigned int err, session;
   if ((err = m_pApi->QryCommodity(&session)) != 0) {
      cout << "查询品种出错" << endl;//>>>>
      return false;
   }
   m_Event.WaitEvent();
   return true;
}

void TAP_CDECL Quote::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info) {
   //cout << __FUNCTION__ << " is called." << endl;
   if (errorCode != 0) {
      cerr << "QryCommodity error. Error Code : " << errorCode << endl;
      return ;
   }
   
   if (isLast == 'Y') {
      cout << "CommodityName : " << info->CommodityName << endl;//>>>>>
      cout << "isLast : " << isLast << endl;//>>>>>>
      m_Event.SignalEvent();
   }
   // TODO:
}

/*
 * @shilei. TODO.
 * 使用此函数前一般先通过QryCommodity得到服务器提供的品种信息， 再从品种信息中选择需要的品种填入Commodity，完成函数的调用。
 * 2017.01.09
 */
void Quote::QryTradingTimeBucketOfCommodity() {
   unsigned int *sessionID;
   TapAPICommodity *qryReq;
   //int err = m_pApi->QryTradingTimeBucketOfCommodity(sessionID, qryReq);
   //isErrorCode(err);
}

/*
void TAP_CDECL Quote::OnRspQryTimeBucketOfCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITimeBucketOfCommodityInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
}
*/

/*
void TAP_CDECL Quote::OnRtnTimeBucketOfCommodity(const TapAPITimeBucketOfCommodityInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
}
*/

/*
 * @shilei. TODO
 * 使用此函数前需要先QryCommodity()取得品种信息， 然后选择需要的品种将信息填入TapAPICommodity结构体中完成查询请求。 
 * 2017.01.09
 */
void Quote::QryContract() {
   TAPIUINT32 sessionID = 0;
   TapAPICommodity qryReq;
   memset(&qryReq, 0, sizeof(qryReq));
   strcpy(qryReq.ExchangeNo, "HKEX");
   strcpy(qryReq.CommodityNo, "HSI");
   qryReq.CommodityType = TAPI_COMMODITY_TYPE_FUTURES;
   cout << "开始查询合约..." << endl;//>>>>
   //strcpy(qryReq.ExchangeNo, "HKEX");
   int ret = m_pApi->QryContract(&sessionID, &qryReq);
   cout << "合约查询" << ((ret == 0) ? "成功" : "失败") << "..." << endl;//>>>>>>>>
   if (!ret) m_Event.WaitEvent();
}

void TAP_CDECL Quote::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info) {
   //cout << __FUNCTION__ << " is called." << endl;
   if (!errorCode && info) {
      cout << "contract : " << info->Contract.Commodity.CommodityNo << info->Contract.ContractNo1 << endl;//>>>>>>>
   }
   if (isLast == 'Y') {
      m_Event.SignalEvent();
   }
}

void TAP_CDECL Quote::OnRtnContract(const TapAPIQuoteContractInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
}

/*
 * @shilei. 
 * 2017.01.09
 */
//bool Quote::SubscribeQuote(char *exchange_no, TAPICommodityType type, char *commodity_no, char *contract_no) {
bool Quote::SubscribeQuote() {
   TAPIUINT32 sessionID = 0;

   for (size_t i = 0; i < all_treatys.size(); ++i) {
      TapAPIContract contract;
      memset(&contract, 0, sizeof(contract));
      strcpy(contract.Commodity.ExchangeNo, all_treatys[i]->exchange_name);
      contract.Commodity.CommodityType = 'F';
      strcpy(contract.Commodity.CommodityNo, all_treatys[i]->commodity);
      strcpy(contract.ContractNo1, all_treatys[i]->contract_no);
      contract.CallOrPutFlag1 = TAPI_CALLPUT_FLAG_NONE;
      contract.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;

      cout << "exchange_no : " << contract.Commodity.ExchangeNo 
        << ",type : " << 'F' << ",commodity_no : " << contract.Commodity.CommodityNo << 
        ",contract_no:" << contract.ContractNo1 << endl;

      int err = m_pApi->SubscribeQuote(&sessionID, &contract);
      //cout << "here?" << endl;//>>>>
      if (err != TAPIERROR_SUCCEED) {
         cout << "SubscribeQuote Error:" << err <<endl;
         return false;
      }
      cout << contract.Commodity.ExchangeNo << contract.Commodity.CommodityNo << contract.ContractNo1 << "订阅请求发送成功" << endl; 
   }
   /*
    * @shilei. 这儿不需要等待订阅成功，因为如果不订阅成功救不回有回调.
    * 2017.01.09
    */
   /*
      while (true) {
      m_Event.WaitEvent();
      }
    */
   //TODO:
   return true;
}

void TAP_CDECL Quote::OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole *info) {
   if (TAPIERROR_SUCCEED == errorCode) {
      cout << "行情订阅成功 ";
      if (NULL != info) {
         cout << info->DateTimeStamp << " "
           << info->Contract.Commodity.ExchangeNo << " "
           << info->Contract.Commodity.CommodityType << " "
           << info->Contract.Commodity.CommodityNo << " "
           << info->Contract.ContractNo1 << " "
           << info->QLastPrice
           // ...		
           <<endl;
      }

      /*
       * @shilei. 收到行情信息后调用策略分析的OnTickData. 详细见strategy.h
       * 2017.01.09
       * 这里有错，OnTickData这个函数是在深度行情订阅信息返回的时候调用的. 具体应该是在Trade里面深度行情回调的时候执行的.
       * 2017.1.19
       */
      //g_strategy->OnTickData(pDepthMarketData); 
   } else {
      cout << "行情订阅失败，错误码：" << errorCode <<endl;
   }
}

/*
 * @shilei.
 * 2017.01.09
 */
void Quote::UnSubscribeQuote(TapAPIContract *contract) {
   unsigned int sessionID = 0;
   int err = m_pApi->UnSubscribeQuote(&sessionID, contract);
   if (err != 0) {
      cerr << "error UnSubscribeQuote " << endl;
   }
   cout << "success UnsubscribeQuote" << endl;
}

void TAP_CDECL Quote::OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract *info) {
   cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Quote::OnRtnQuote(const TapAPIQuoteWhole *info) {
   if (info) {
      /*
         cout << "行情更新:" 
         << info->DateTimeStamp << " "
         << info->Contract.Commodity.ExchangeNo << " "
         << info->Contract.Commodity.CommodityType << " "
         << info->Contract.Commodity.CommodityNo << " "
         << info->Contract.ContractNo1 << " "
         << info->QLastPrice
      // ...		
      <<endl;
       */
      for (size_t i = 0; i < all_treatys.size(); ++i) {
         if (isTheTreaty(all_treatys[i], (TapAPIQuoteWhole *)info)) {
            cout << "保存行情：" << info->Contract.Commodity.CommodityNo << info->Contract.ContractNo1 << "," << info->DateTimeStamp << ",最新价:" << info->QLastPrice << ",涨停价:" << info->QLimitUpPrice << ",跌停价:" << info->QLimitDownPrice << endl;
            memcpy(&(all_treatys[i]->quotewhole), info, sizeof(TapAPIQuoteWhole));
            all_treatys[i]->ev.SignalEvent(); 
            /*
               pthread_mutex_lock(&(all_treatys[i]->queue_mutex)); 
               all_treatys[i]->quote_queue.push(info_t);
               pthread_mutex_unlock(&(all_treatys[i]->queue_mutex))
             */
            break;
         }
      }
      g_strategy->OnTickData((TapAPIQuoteWhole *)info);
   }
}

bool Quote::isTheTreaty(treaty *tv, TapAPIQuoteWhole *info) {
   return ((strcmp(tv->exchange_name, info->Contract.Commodity.ExchangeNo) == 0) && (strcmp(tv->commodity, info->Contract.Commodity.CommodityNo) == 0) && (strcmp(tv->contract_no, info->Contract.ContractNo1) == 0)) ? true : false;
}

/*
   void Quote::QryHisQuote(TapAPIHisQuoteQryReq *qryReq) {
   unsigned int sessionID = 0;
   int err = m_pApi->QryHisQuote(&sessionID, qryReq);
   if (err != 0) {
   cerr << "error in QryHisQuote " << endl;
   }
   cout << "QryHisQuote success" << endl;
   }
 */

/*
   void TAP_CDECL Quote::OnRspQryHisQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisQuoteQryRsp *info) {
   cout << __FUNCTION__ << " is called." << endl;
   }
 */

bool Quote::isErrorCode(int err_code) {
   if (err_code != 0) {
      cerr << " 响应 | Err_Code : " << err_code << endl;
      return false;
   }
   return true;
}

ITapQuoteAPI *Quote::createTapQuoteApi(TAPIAUTHCODE authCode, TAPISTR_300 keyOperationLogPath, int &errcode) {
   TAPIINT32 iResult = TAPIERROR_SUCCEED;
   TapAPIApplicationInfo stAppInfo;
   strcpy(stAppInfo.AuthCode, authCode);
   strcpy(stAppInfo.KeyOperationLogPath, keyOperationLogPath);
   ITapQuoteAPI *pApi = CreateTapQuoteAPI(&stAppInfo, iResult);
   SetTapQuoteAPILogLevel(APILOGLEVEL_ERROR);
   errcode = iResult;
   return pApi;
}
