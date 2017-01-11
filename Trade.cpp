#include "Trade.h"
#include "./include/TapAPIError.h"
#include "TradeConfig.h"
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

Trade::Trade(void):
   m_ptradeApi(NULL),
   m_bIsAPIReady(false) {
      memset(username, 0, 64);
      memset(passwd, 0, 64);
      memset(ip, 0, 64);
   }

Trade::Trade(ITapTradeAPI *ptradeApi, ITapQuoteAPI *pmdApi, Quote *mdApi) : m_ptradeApi(ptradeApi), m_pmdApi(pmdApi), m_mdSpi(mdApi) {
   memset(username, 0, 64);
   memset(passwd, 0, 64);
   memset(ip, 0, 64);
   m_bIsAPIReady = false;
}

Trade::~Trade(void) {}


void Trade::SetAPI(ITapTradeAPI *pAPI) {
   m_ptradeApi = pAPI;
}

void Trade::setFront(char *_ip, int _port) {
   strcpy(ip, _ip);
   port = _port;
}

void Trade::setAccount(char *_username, char *_passwd) {
   strcpy(username, _username);
   strcpy(passwd, _passwd);
}

bool Trade::init() {
   if(NULL == m_ptradeApi) {
      cout << "Error: m_ptradeApi is NULL." << endl;
      return false;
   }

   TAPIINT32 iErr = TAPIERROR_SUCCEED;
   iErr = m_ptradeApi->SetHostAddress(ip, port);
   if(TAPIERROR_SUCCEED != iErr) {
      cout << "SetHostAddress Error:" << iErr <<endl;
      return false;
   }

   int ret = login();
   if (ret != 0) {
      cerr << "trade login error :" << ret << endl;
      return false;
   }
   cout << "trade login success" << endl;

   /*
    * @shilei. 执行查询.
    * 2017.01.10
    */
   // 请求查询报单.
   QryOrder();

   //请求查询成交.
   QryFill();

   //请求查询投资者持仓明细.
   QryPosition();

   //请求查询资金.
   QryAccount();

   // 查询所有合约持仓信息.
   //

   // 请求查询合约信息，所有合约
   QryContract();

   // 启动MD 
   // 这个步骤提取到了主函数里面，方便理解。
   //m_mdSpi->init();

   return true;
}

int Trade::login() { 
   TapAPITradeLoginAuth stLoginAuth;
   memset(&stLoginAuth, 0, sizeof(stLoginAuth));
   strcpy(stLoginAuth.UserNo, username);
   strcpy(stLoginAuth.Password, passwd);
   stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
   stLoginAuth.ISDDA = APIYNFLAG_NO;
   int iErr = m_ptradeApi->Login(&stLoginAuth);
   if(TAPIERROR_SUCCEED != iErr) {
      //cout << "Login Error:" << iErr <<endl;
      return iErr;
   }

   m_Event.WaitEvent();
   if (!m_bIsAPIReady){
      return -1;
   }
   return 0;
}

void TAP_CDECL Trade::OnConnect() {
   cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspLogin( TAPIINT32 errorCode, const TapAPITradeLoginRspInfo *loginRspInfo ) {
   if(TAPIERROR_SUCCEED == errorCode) {
      cout << "登录成功，等待API初始化..." << endl;
      cout << "Trader 响应 | 登录成功...当前交易日:" << 
        loginRspInfo->TradeDate << endl;
   } else {
      cout << "登录失败，错误码:" << errorCode << endl;
      m_Event.SignalEvent();	
   }
}

void TAP_CDECL Trade::OnAPIReady() {
   cout << "API初始化完成" << endl;
   m_bIsAPIReady = true;
   m_Event.SignalEvent();	
}

void Trade::Disconnect() {
   m_ptradeApi->Disconnect();
}

void TAP_CDECL Trade::OnDisconnect( TAPIINT32 reasonCode )
{
cout << "API断开,断开原因:"<<reasonCode << endl;
}

void TAP_CDECL Trade::OnRspChangePassword( TAPIUINT32 sessionID, TAPIINT32 errorCode )
{
cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspSetReservedInfo( TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info ) {
   cout << __FUNCTION__ << " is called." << endl;
}

void Trade::QryAccount() {
   TAPIUINT32 sessionID;
   TapAPIAccQryReq qry_req;

   int ret = m_ptradeApi->QryAccount(&sessionID, &qry_req);
   cerr<<" 请求 | 发送资金查询..."<<((ret == 0)?"成功":"失败")<<endl;

   if (!ret) m_Event.WaitEvent();
}

void TAP_CDECL Trade::OnRspQryAccount( TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo *info ) {
   /*
    * TODO:
    */
   cout << __FUNCTION__ << " is called." << endl;
   m_Event.SignalEvent();
}

void TAP_CDECL Trade::OnRspQryFund( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info )
{
cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnFund( const TapAPIFundData *info )
{
//	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryExchange( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info )
{
cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryCommodity( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo *info )
{
cout << __FUNCTION__ << " is called." << endl;
}

void Trade::QryContract() {
   TAPIUINT32 sessionID = 0;
   TapAPICommodity qry_req;
   memset(&qry_req, 0, sizeof(TapAPICommodity)); // 查询所有.

   int ret = m_ptradeApi->QryContract(&sessionID, &qry_req);
   cerr<<" 请求 | 发送合约信息查询..."<<((ret == 0)?"成功":"失败")<<endl;

   if (!ret) m_Event.WaitEvent();
}

void TAP_CDECL Trade::OnRspQryContract( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   if (!errorCode && info) {
      if(first_inquiry_Instrument == true) {
         m_Instrument_all = m_Instrument_all + info->ExchangeNo + ","; 
         TapAPITradeContractInfo* instField = new TapAPITradeContractInfo();
         memcpy(instField,  info, sizeof(TapAPITradeContractInfo));
         m_instMessage_map.insert(pair<string, TapAPITradeContractInfo*> (instField->ExchangeNo, instField));

         cout << "info->ExchangeNo: " << info->ExchangeNo << endl;
         /*
            if(strcmp(m_instId, pInstrument->InstrumentID) == 0)
            {
            cerr<<" 响应 | 合约:"<<pInstrument->InstrumentID
            <<" 合约名称:"<<pInstrument->InstrumentName
            <<" 合约在交易所的代码:"<<pInstrument->ExchangeInstID
            <<" 产品代码:"<<pInstrument->ProductID
            <<" 产品类型:"<<pInstrument->ProductClass
            <<" 交割月:"<<pInstrument->DeliveryMonth
            <<" 多头保证金率:"<<pInstrument->LongMarginRatio
            <<" 空头保证金率:"<<pInstrument->ShortMarginRatio
            <<" 合约数量乘数:"<<pInstrument->VolumeMultiple
            <<" 最小变动价位:"<<pInstrument->PriceTick
            <<" 交易所代码:"<<pInstrument->ExchangeID
            <<" 交割年份:"<<pInstrument->DeliveryYear
            <<" 交割月:"<<pInstrument->DeliveryMonth
            <<" 创建日:"<<pInstrument->CreateDate
            <<" 上市日:"<<pInstrument->OpenDate
            <<" 到期日:"<<pInstrument->ExpireDate
            <<" 开始交割日:"<<pInstrument->StartDelivDate
            <<" 结束交割日:"<<pInstrument->EndDelivDate
            <<" 合约生命周期状态:"<<pInstrument->InstLifePhase
            <<" 当前是否交易:"<<pInstrument->IsTrading
            <<endl; 
            }
          */ 

         if(isLast) {
            first_inquiry_Instrument = false; 
            m_Instrument_all = m_Instrument_all.substr(0,m_Instrument_all.length()-1);

            cerr<<"m_Instrument_all大小："<<m_Instrument_all.length()<<","<<m_Instrument_all<<endl; 
            cerr<<"map大小(合约个数）："<<m_instMessage_map.size()<<endl;

            //g_strategy->set_instMessage_map_stgy(m_instMessage_map); 
            cerr<<"----------------------------输出合约信息map的内容----------------------------"<<endl;


            cerr<<"TD初始化完成:"<<endl;
            m_Event.SignalEvent();
            //m_pMDUserApi_td->Init(); 
         } 
      }
   }
}

void TAP_CDECL Trade::OnRtnContract( const TapAPITradeContractInfo *info )
{
//	cout << __FUNCTION__ << " is called." << endl;
}


void TAP_CDECL Trade::OnRtnOrder( const TapAPIOrderInfoNotice *info )
{
if(NULL == info){
   return;
}

if (info->ErrorCode != 0) {
   cout << "服务器返回了一个关于委托信息的错误：" << info->ErrorCode << endl;
} else {
   if (info->OrderInfo) {
      if (info->SessionID == m_uiSessionID)
      {
         if (0!= info->OrderInfo->ErrorCode){
            cout << "报单失败，"
              << "错误码:"<<info->OrderInfo->ErrorCode << ","
              << "委托编号:"<<info->OrderInfo->OrderNo
              <<endl;
         } else{
            cout << "报单成功，"
              << "状态:"<<info->OrderInfo->OrderState << ","
              << "委托编号:"<<info->OrderInfo->OrderNo
              <<endl;
         }
         m_Event.SignalEvent();
      }
   }
}
}

void TAP_CDECL Trade::OnRspOrderAction( TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info ) {
   cout << __FUNCTION__ << " is called." << endl;
}

/*
 * @shilei
 * 2017.01.10
 */
void Trade::QryOrder() {
   TAPIUINT32 sessionID;
   TapAPIOrderQryReq qry_req;
   qry_req.OrderQryType = TAPI_ORDER_QRY_TYPE_ALL;
   int ret = m_ptradeApi->QryOrder(&sessionID, &qry_req); 

   cerr<<" 请求 | 发送查询报单..."<<((ret == 0)?"成功":"失败")<<" ret:"<<ret<<endl;
   m_Event.WaitEvent(); /* 这里需要在看看会不会出现没有执行唤醒的时候 */

   cout << "查询完成！" << endl;
}

void TAP_CDECL Trade::OnRspQryOrder( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   cerr<<"请求查询报单响应:OnRspQryOrder"<<endl;

   if (!errorCode && info) {
      cout <<"请求查询报单响应:" <<" 会话编号SessionID:"<<sessionID<<endl;
      if(first_inquiry_order == true) {
         TapAPIOrderInfo* order = new TapAPIOrderInfo();
         memcpy(order,  info, sizeof(TapAPIOrderInfo));
         orderList.push_back(order);


         if(isLast) 
         {
            first_inquiry_order = false;

            cerr<<"所有合约报单次数："<<orderList.size()<<endl;

            /*
             * TODO: 这里可以打印出所有的info里的信息
             * @shilei
             */
            cerr<<"--------------------------------------------------------------------报单start"<<endl;
            for(vector<TapAPIOrderInfo*>::iterator iter = orderList.begin(); iter != orderList.end(); iter++)
               cerr<<" 投资者代码:"<<(*iter)->AccountNo<<endl<<" 合约代码:"<<(*iter)->ContractNo<<endl<<" 买卖方向:"<<endl
                 <<" 组合开平标志:"<<endl<<" 价格:"<<(*iter)->StrikePrice<<endl<<" 数量:"<<endl<<" 报单引用:"<< endl<<" 客户代码:"<<endl
                 <<" 报单状态:"<<endl<<" 委托时间:"<<endl<<" 报单编号:"<<endl<<" GTD日期:"<<endl<<" 交易日:"<<endl
                 <<" 报单日期:"
                 <<endl;

            cerr<<"--------------------------------------------------------------------报单end"<<endl;


            sleep(1);
            cerr<<"查询报单正常，首次查询成交:"<<endl;
            m_Event.SignalEvent();
         }
      }
   } else {
      if(first_inquiry_order == true ) {
         first_inquiry_order = false;
         sleep(1);
         cerr<<"查询报单出错，或没有报单，首次查询成交:"<<endl;
         m_Event.SignalEvent();
      }
   }
}

void TAP_CDECL Trade::OnRspQryOrderProcess( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info )
{
cout << __FUNCTION__ << " is called." << endl;
}

/*
 *
 * @shilei.
 * 2017.01.10
 */
void Trade::QryFill() {
   TAPIUINT32 sessionID = 0;
   TapAPIFillQryReq qry_req;

   int ret = m_ptradeApi->QryFill(&sessionID, &qry_req);
   cerr<<" 请求 | 发送查询成交..."<<((ret == 0)?"成功":"失败")<<" ret:"<<ret<<endl;

   m_Event.WaitEvent();

   cout << "请求 | 查询成交完成" << endl;
}

void TAP_CDECL Trade::OnRspQryFill( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   cerr<<"请求查询成交响应:OnRspQryTrade"<< endl;

   if (!errorCode && info) {
      if(first_inquiry_trade == true) {
         TapAPIFillInfo* trade = new TapAPIFillInfo();
         memcpy(trade,  info, sizeof(TapAPIFillInfo));
         tradeList.push_back(trade); 

         if(isLast) {
            first_inquiry_trade = false;

            cerr<<"成交次数:"<<tradeList.size()<<endl;

            cerr<<"--------------------------------------------------------------------成交start"<<endl;
            /*
               for(vector<CThostFtdcTradeField*>::iterator iter = tradeList.begin(); iter != tradeList.end(); iter++)
               {  
               cerr<<"合约代码:"<<endl<<" 用户代码:"<<endl<<" 成交编号:"<<endl<<" 买卖方向:"<<endl<<" 开平标志:"<<endl
               <<" 投机套保标志:"<<endl<<" 价格:"<<endl<<" 数量:"<<endl<<" 成交时间:"<<endl
               <<" 成交类型:"<<endl<<" 报单编号:"<<endl<<" 报单引用:"<<endl<<" 本地报单编号:"<<endl
               <<" 业务单元:"<<endl<<" 序号:"<<endl<<" 经纪公司报单编号:"<<endl
               <<" 成交时期:"<<endl<<" 交易日:"

               <<endl<<endl;  
               }
             */
            cerr<<"--------------------------------------------------------------------成交end"<<endl; 
            sleep(1); 
            cerr<<"查询成交正常，首次查询投资者持仓明细:"<<endl; 
            m_Event.SignalEvent(); 
         }
      }
   } else {
      if(first_inquiry_trade == true) {
         first_inquiry_trade = false;
         sleep(1);

         cerr<<"查询成交出错，或没有成交，首次查询投资者持仓明细:"<<endl;
         m_Event.SignalEvent();
      }
   }
}

void TAP_CDECL Trade::OnRtnFill( const TapAPIFillInfo *info ) {
   //	cout << __FUNCTION__ << " is called." << endl;
}

void Trade::QryPosition() {
   TAPIUINT32 sessionID = 0;
   TapAPIPositionQryReq qry_req;
   memset(&qry_req, 0, sizeof(TapAPIPositionQryReq));

   int ret = m_ptradeApi->QryPosition(&sessionID, &qry_req);
   cerr<<" 请求 | 发送查询持仓..."<<((ret == 0)?"成功":"失败")<<" ret:"<<ret<<endl;

   if (!ret) return;
   m_Event.WaitEvent();

   return ;
}

void TAP_CDECL Trade::OnRspQryPosition( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   cerr<<"请求查询投资者持仓明细响应：OnRspQryInvestorPositionDetail"<< endl;
}

void TAP_CDECL Trade::OnRtnPosition( const TapAPIPositionInfo *info )
{
//	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryClose( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo *info )
{
cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnClose( const TapAPICloseInfo *info )
{
//	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnPositionProfit( const TapAPIPositionProfitNotice *info )
{
//	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIDeepQuoteQryRsp *info)
{
//	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryExchangeStateInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast,const TapAPIExchangeStateInfo * info)
{
//	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice * info)
{
//	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice *info)
{
//	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspUpperChannelInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo * info)
{

}

void Trade::printTradeMessageMap() {
   cerr<<"------------------------------------------------printTrade_message_map开始"<<endl;
}

void Trade::forceClose() {
   cout << "-----------------------------------in Force Close---------------" << endl;
}

ITapTradeAPI *Trade::createTapTradeApi(TAPIAUTHCODE authCode, TAPISTR_300 keyOperationLogPath, int errorCode) {
   TAPIINT32 iResult = TAPIERROR_SUCCEED;
   TapAPIApplicationInfo stAppInfo;
   strcpy(stAppInfo.AuthCode, authCode);
   strcpy(stAppInfo.KeyOperationLogPath, keyOperationLogPath);
   ITapTradeAPI *pApi = CreateTapTradeAPI(&stAppInfo, iResult);
   errorCode = iResult;
   return pApi;
}
