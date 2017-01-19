#ifndef TRADE_H
#define TRADE_H

#include "./include/iTapTradeAPI.h"
#include "./include/TapQuoteAPI.h"
#include "SimpleEvent.h"
#include "Quote.h"
#include "StructFunction.h"

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
   virtual void TAP_CDECL OnRspQryCurrency(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICurrencyInfo *info);
   virtual void TAP_CDECL OnRspQryTradeMessage(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeMessage *info);
   virtual void TAP_CDECL OnRtnTradeMessage(const TapAPITradeMessage *info);
   virtual void TAP_CDECL OnRspQryHisOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisOrderQryRsp *info);
   virtual void TAP_CDECL OnRspQryHisOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisOrderProcessQryRsp *info);
   virtual void TAP_CDECL OnRspQryHisMatch(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisMatchQryRsp *info);
   virtual void TAP_CDECL OnRspQryHisPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisPositionQryRsp *info);
   virtual void TAP_CDECL OnRspQryHisDelivery(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisDeliveryQryRsp *info);
   virtual void TAP_CDECL OnRspQryAccountCashAdjust(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountCashAdjustQryRsp *info);
   virtual void TAP_CDECL OnRspQryBill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIBillQryRsp *info);
   //对ITapTradeAPINotify的实现

public:
   bool init();

   void setFront(char *_ip, int _port);
   void setAccount(char *_username, char *_passwd);
   int login();
   void Disconnect();
   void QryRight();
   void QryOrder();
   void QryFill();
   void QryPosition();
   void QryAccount();
   void QryContract();
   void QryFund();
   void QryExchange();
   void QryCommodity();

   void insertOrder(); // 报单录入请求
   void orderAction(); // 报单操作请求
   void printTrades(); // 打印成交记录
   void CancelOrder(); // 撤单，如需追单，可在报单回报里面等撤单成功后再进行

   void printTrade_message_map();
   void forceClose();

   double sendCloseProfit(); //计算平仓盈亏
   double sendOpenProfit_account(string instId, double lastPrice); // 计算账户的浮动盈亏，以开仓价算 
   double sendPositionProfit(); //计算持仓盈亏，以昨结计算，若是日内，与浮动盈亏相同（实际交易中应用比较少）

   void showInstMessage(); //打印所有合约信息
   void setLastPrice(string instID, double price); //更新合约的最新价

   int send_trade_message_map_KeyNum(string instID); //合约交易信息结构体的map，KEY的个数，为0表示没有该合约的交易信息，即该合约没有持仓(开仓后已经平仓的，KEY不为0）

   int SendHolding_long(string instID); //返回某合约多单持仓量

   int SendHolding_short(string instID); //返回某合约空单持仓量

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

   TAPISTR_10 m_instId;// 合约代码

   double m_closeProfit;//平仓盈亏，所有合约一起算后的值，另外在m_trade_message_map有单独计算每个合约的平仓盈亏值

   double m_OpenProfit;//浮动盈亏，所有合约一起算后的值，另外在m_trade_message_map有单独计算每个合约的浮动盈亏值

   vector<TapAPIOrderInfo*> orderList;//委托记录，全部合约
   vector<TapAPIFillInfo*> tradeList;//成交记录，全部合约

   string m_Instrument_all;//所有合约代码合在一起
   map<string, TapAPITradeContractInfo *> m_instMessage_map;//保存合约信息的map
   map<string, trade_message*> m_trade_message_map;//合约交易信息结构体的map

   char ip[64];
   int port;
   char username[64];
   char passwd[64];
};

#endif // TRADE_H
