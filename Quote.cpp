#include "Quote.h"
#include "./include/TapAPIError.h"
//#include "QuoteConfig.h"
//#include <Windows.h>
#include <iostream>
#include <string.h>

using namespace std;

Quote::Quote(void):
	m_pApi(NULL),
	m_bIsAPIReady(false),
	is_last(false) {}


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

/*
 * @shilei.
 * 2017.01.09
 */
void Quote::init() {
   if(NULL == m_pApi) {
      cout << "Error: m_pApi is NULL." << endl;
      return;
   }
   TAPIINT32 iErr = TAPIERROR_SUCCEED;
   iErr = m_pApi->SetHostAddress(ip, port);
   if(TAPIERROR_SUCCEED != iErr) {
      cout << "SetHostAddress Error:" << iErr <<endl;
      return;
   }
   TapAPIQuoteLoginAuth loginAuth;
   memset(&loginAuth, 0, sizeof(loginAuth));
   strcpy(loginAuth.UserNo, username);
   strcpy(loginAuth.Password, passwd);
   loginAuth.ISModifyPassword = APIYNFLAG_NO;
   loginAuth.ISDDA = APIYNFLAG_NO;

   // TODO:
   // 登陆.
   bool err = login(&loginAuth);
   if (!err) {
      cerr << "login error." << endl;
      return ;
   }
   cout << "login success" << endl;

   // 订阅行情.
   //SubscribeQuote();
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
   m_pApi->ChangePassword(&sessionID, &req);
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
   int errcode = m_pApi->QryExchange(sessionID);
   isErrorCode(errcode);  
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
      return false;
   }
   return true;
}

void TAP_CDECL Quote::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
   if (errorCode != 0) {
      cerr << "QryCommodity error. Error Code : " << errorCode << endl;
      return ;
   }
   cout << "isLast : " << isLast << endl;//>>>>>>
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
   int err = m_pApi->QryTradingTimeBucketOfCommodity(sessionID, qryReq);
   isErrorCode(err);
}

void TAP_CDECL Quote::OnRspQryTimeBucketOfCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITimeBucketOfCommodityInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Quote::OnRtnTimeBucketOfCommodity(const TapAPITimeBucketOfCommodityInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
}

/*
 * @shilei. TODO
 * 使用此函数前需要先QryCommodity()取得品种信息， 然后选择需要的品种将信息填入TapAPICommodity结构体中完成查询请求。 
 * 2017.01.09
 */
void Quote::QryContract() {
}

void TAP_CDECL Quote::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Quote::OnRtnContract(const TapAPIQuoteContractInfo *info) {
   cout << __FUNCTION__ << " is called." << endl;
}

/*
 * @shilei. 
 * 2017.01.09
 */
bool Quote::SubscribeQuote(char *exchange_no, TAPICommodityType type, char *commodity_no, char *contract_no) {
   unsigned int sessionID = 0;
   TapAPIContract contract;
   memset(&contract, 0, sizeof(contract));
   strcpy(contract.Commodity.ExchangeNo, exchange_no);
   contract.Commodity.CommodityType = type;
   strcpy(contract.Commodity.CommodityNo, commodity_no);
   strcpy(contract.ContractNo1, contract_no);
   contract.CallOrPutFlag1 = TAPI_CALLPUT_FLAG_NONE;
   contract.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;

   cout << "exchange_no : " << exchange_no << endl
     << "type : " << type << endl << "commodity_no : " << commodity_no << endl
     << "contract_no:" << contract_no << endl
     << "CallOrPutFlag1 : " << contract.CallOrPutFlag1 << endl
     << "CallOrPutFlag2 : " << contract.CallOrPutFlag2 << endl;

   int err = m_pApi->SubscribeQuote(&sessionID, &contract);
   if (err != TAPIERROR_SUCCEED) {
      cout << "SubscribeQuote Error:" << err <<endl;
      return false;
   }
   cout << "订阅请求发送成功" << endl; 
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
   if (NULL != info)
   {
      cout << "行情更新:" 
        << info->DateTimeStamp << " "
        << info->Contract.Commodity.ExchangeNo << " "
        << info->Contract.Commodity.CommodityType << " "
        << info->Contract.Commodity.CommodityNo << " "
        << info->Contract.ContractNo1 << " "
        << info->QLastPrice
        // ...		
        <<endl;
   }
}

void Quote::QryHisQuote(TapAPIHisQuoteQryReq *qryReq) {
   unsigned int sessionID = 0;
   int err = m_pApi->QryHisQuote(&sessionID, qryReq);
   if (err != 0) {
      cerr << "error in QryHisQuote " << endl;
   }
   cout << "QryHisQuote success" << endl;
}

void TAP_CDECL Quote::OnRspQryHisQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisQuoteQryRsp *info) {
   cout << __FUNCTION__ << " is called." << endl;
}

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
   errcode = iResult;
   return pApi;
}
