#ifndef TRADE_H
#define TRADE_H

#include "./include/TapTradeAPI.h"
#include "./include/TapQuoteAPI.h"
#include "SimpleEvent.h"
#include "Quote.h"

#include <vector>
#include <string>
#include <map>

using namespace std;

class Trade : public ITapTradeAPINotify {
public:
   Trade(void);
   Trade(ITapTradeAPI *ptradeApi, ITapQuoteAPI *pmdApi, Quote *mdApi);
   ~Trade(void);

   void SetAPI(ITapTradeAPI *pAPI);
   //void RunTest();

public:
   //对ITapTradeAPINotify的实现
   virtual void TAP_CDECL OnConnect();
   virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo *loginRspInfo);
   virtual void TAP_CDECL OnAPIReady();
   virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);
   virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode);
   virtual void TAP_CDECL OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info);
   virtual void TAP_CDECL OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo *info);
   virtual void TAP_CDECL OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info);
   virtual void TAP_CDECL OnRtnFund(const TapAPIFundData *info);
   virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info);
   virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo *info);
   virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo *info);
   virtual void TAP_CDECL OnRtnContract(const TapAPITradeContractInfo *info);
   virtual void TAP_CDECL OnRtnOrder(const TapAPIOrderInfoNotice *info);
   virtual void TAP_CDECL OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info);
   virtual void TAP_CDECL OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info);
   virtual void TAP_CDECL OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info);
   virtual void TAP_CDECL OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info);
   virtual void TAP_CDECL OnRtnFill(const TapAPIFillInfo *info);
   virtual void TAP_CDECL OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo *info);
   virtual void TAP_CDECL OnRtnPosition(const TapAPIPositionInfo *info);
   virtual void TAP_CDECL OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo *info);
   virtual void TAP_CDECL OnRtnClose(const TapAPICloseInfo *info);
   virtual void TAP_CDECL OnRtnPositionProfit(const TapAPIPositionProfitNotice *info);
   virtual void TAP_CDECL OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIDeepQuoteQryRsp *info);
   virtual void TAP_CDECL OnRspQryExchangeStateInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast,const TapAPIExchangeStateInfo * info);
   virtual void TAP_CDECL OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice * info);
   virtual void TAP_CDECL OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice *info); //V9.0.2.0 20150520
   virtual void TAP_CDECL OnRspUpperChannelInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo * info);

public:
   bool init();

   void setFront(char *_ip, int _port);
   void setAccount(char *_username, char *_passwd);
   int login();
   void Disconnect();
   void QryOrder();
   void QryFill();
   void QryPosition();
   void QryAccount();
   void QryContract();

   void printTradeMessageMap();
   void forceClose();

   static ITapTradeAPI *createTapTradeApi(TAPIAUTHCODE authCode, TAPISTR_300 keyOperationLogPath, int errorCode);

private:
   ITapTradeAPI *m_ptradeApi;
   ITapQuoteAPI *m_pmdApi;
   Quote *m_mdSpi;

   TAPIUINT32	m_uiSessionID;
   SimpleEvent m_Event;
   bool m_bIsAPIReady;

   bool first_inquiry_order;//是否首次查询报单
   bool first_inquiry_trade;//是否首次查询成交
   bool firs_inquiry_Detail;//是否首次查询持仓明细
   bool firs_inquiry_TradingAccount;//是否首次查询资金账号
   bool firs_inquiry_Position;//是否首次查询投资者持仓
   bool first_inquiry_Instrument;//是否首次查询合约

   vector<TapAPIOrderInfo*> orderList;//委托记录，全部合约
   vector<TapAPIFillInfo*> tradeList;//成交记录，全部合约

   string m_Instrument_all;//所有合约代码合在一起
   map<string, TapAPITradeContractInfo *> m_instMessage_map;//保存合约信息的map

   char ip[64];
   int port;
   char username[64];
   char passwd[64];
};

#endif // TRADE_H
