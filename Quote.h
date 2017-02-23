#ifndef QUOTE_H
#define QUOTE_H

#include "./include/TapQuoteAPI.h"
#include "treaty.h"
//#include "strategy.h"
#include "SimpleEvent.h"
#include <vector>

using namespace std;

class Quote : public ITapQuoteAPINotify
{
public:
   Quote(void);
   ~Quote(void);

   void SetAPI(ITapQuoteAPI* pAPI);
   void RunTest();

public:
   //对ITapQuoteAPINotify的实现
   virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info);
   virtual void TAP_CDECL OnAPIReady();
   virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);
   virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode);
   virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info);
   virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info);
   //virtual void TAP_CDECL OnRspQryTimeBucketOfCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITimeBucketOfCommodityInfo *info);
   //virtual void TAP_CDECL OnRtnTimeBucketOfCommodity(const TapAPITimeBucketOfCommodityInfo *info);
   virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info);
   virtual void TAP_CDECL OnRtnContract(const TapAPIQuoteContractInfo *info);
   virtual void TAP_CDECL OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole *info);
   virtual void TAP_CDECL OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract *info);
   virtual void TAP_CDECL OnRtnQuote(const TapAPIQuoteWhole *info);
   //virtual void TAP_CDECL OnRspQryHisQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisQuoteQryRsp *info);

public:
   bool login(TapAPIQuoteLoginAuth *loginAuth);
   void disconnect();
   void setHostAddress(char *ip, char *port);
   void changePassword(char *old_passwd, char *new_passwd);
   void QryExchange();
   bool QryCommodity();
   void QryTradingTimeBucketOfCommodity();
   void QryContract();
   bool SubscribeQuote();
   //bool SubscribeQuote(char *exchange_num, TAPICommodityType type, char *commodity_no, char *contract_no);
   void UnSubscribeQuote(TapAPIContract *contract);
   //void QryHisQuote(TapAPIHisQuoteQryReq *qryReq);

   bool isErrorCode(int err_code); 
   void setFront(char *_ip, int _port);
   void setAccount(char *_username, char *_passwd);
   void setTreatys(vector<treaty *> &treatys);
   bool isTheTreaty(treaty *tv, TapAPIQuoteWhole *info);
   int init(); /* 功能类似于CTP中的init. */

   static ITapQuoteAPI *createTapQuoteApi(TAPIAUTHCODE authCode, TAPISTR_300 keyOperationLogPath, int &errcode);

private:
   ITapQuoteAPI* m_pApi;
   TAPIUINT32	m_uiSessionID;
   SimpleEvent m_Event;
   bool		m_bIsAPIReady; 
   bool is_last;

   char ip[32];
   int port;
   char username[64];
   char passwd[64];
   vector<treaty *> all_treatys;
   //Strategy* g_strategy;
};

#endif // QUOTE_H
