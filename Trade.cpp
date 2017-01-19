#include "Trade.h"
#include "./include/TapAPIError.h"
#include "TradeConfig.h"
#include "StructFunction.h"
#include "strategy.h"
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

extern Strategy* g_strategy;//策略类指针

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

   first_inquiry_order = true;// 是否首次查询报单
   first_inquiry_trade = true;//是否首次查询成交
   firs_inquiry_Detail = true;//是否首次查询持仓明细
   firs_inquiry_TradingAccount = true;//是否首次查询资金账号
   firs_inquiry_Position = true;//是否首次查询投资者持仓
   first_inquiry_Instrument = true;//是否首次查询合约
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

   QryRight();
   //sleep(1);
   /*
    * @shilei. 执行查询.
    * 2017.01.10
    */
   // 请求查询报单.
   QryOrder();
   //sleep(1);

   // 查询交易所信息
   //QryExchange();

   // 查询系统品种信息
   //QryCommodity();

   //请求查询成交.
   QryFill();
   //sleep(1);
   
   // 查询资金账号
   QryAccount();
   //sleep(1);

   // 查询客户资金
   QryFund();
   //sleep(1);

   // 这个地方在原始的要求里面写的是；查询投资者持仓明细.
   // 请求查询投资者持仓
   QryPosition();
   //sleep(1); 

   // 查询所有合约持仓信息.
   // 

   // 请求查询合约信息，所有合约
   QryContract();
   //sleep(1); 

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
   //stLoginAuth.ISDDA = APIYNFLAG_NO;
   int iErr = m_ptradeApi->Login(&stLoginAuth);
   if(TAPIERROR_SUCCEED != iErr) {
      cout << "Login Error:" << iErr <<endl;
      //return iErr;
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

void Trade::QryRight() {
   TAPIRightIDType iRight = TAPI_RIGHT_ORDER_CHECK;
   int iRet = m_ptradeApi->HaveCertainRight(iRight);
   if (iRet) {
      cout << "有权限" << endl;
   } else {
      cout << "无权限" << endl;
   }
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
   cerr<<" 请求 | 发送资金账号查询..."<<((ret == 0)?"成功":"失败")<<endl;

   if (!ret) m_Event.WaitEvent();
}

void TAP_CDECL Trade::OnRspQryAccount( TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo *info ) {
   /*
    * TODO:
    */
   cout << __FUNCTION__ << " is called." << endl;
   cout << "请求查询资金账号响应:OnRspQryAccount" << ". info :" << info << endl;
   if (!errorCode && info) {
      cout << "响应 | 投资者帐号:" << info->AccountNo << " 账号类型:" << info->AccountType << " 账号状态:" << info->AccountState << " 交易状态:" << info->AccountTradeRight << " 可交易品种组.:" << info->CommodityGroupNo << " 账号简称: " << info->AccountShortName << " 账号英文简称:" << info->AccountEnShortName << endl;
   } else {
      cout << "请求查询资金账号响应: 无信息" << endl;
      /*
      if(firs_inquiry_TradingAccount == true)
      {
         firs_inquiry_TradingAccount = false;
         sleep(1); 
         //cerr<<"资金查询出错,查询投资者持仓:"<<endl;        
         //ReqQryInvestorPosition_all();//查询所有合约持仓信息 
      }
      */
   }
   m_Event.SignalEvent();
}

void Trade::QryFund() {
   TAPIUINT32 sessionID = 0;
   TapAPIFundReq stFund;
   memset(&stFund,0,sizeof(stFund));
   strcpy(stFund.AccountNo, username); 
   int ret = m_ptradeApi->QryFund(&sessionID, &stFund);
   cerr<<" 请求 | 发送客户资金查询..."<<((ret == 0)?"成功":"失败")<<endl;

   if (!ret) m_Event.WaitEvent();
}

void TAP_CDECL Trade::OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   cerr<<"请求查询资金响应:OnRspQryTradingAccount"<<" info :" << info << endl;

   if (!errorCode && info) {
      cerr<<"响应 | 投资者帐号:"<< info->AccountNo 
        <<" 上次结算准备金(静态权益，期初权益):" << info->PreBalance
        << " 期货结算准备金(动态权益):" << info->Balance
        << " 可用资金:"<< info->Available
        << " 可取资金:" << info->CanDraw
        << " 当前保证金总额: " << "" 
        << " 平仓盈亏:" << info->CloseProfit
        << " 持仓盈亏:" << info->PositionProfit
        << " 手续费:" << "" 
        << " 冻结保证金:" << info->FrozenDeposit
        << endl;

      if(firs_inquiry_TradingAccount == true) {
         firs_inquiry_TradingAccount = false;
         sleep(1); 
         cerr<<"资金查询正常，查询投资者持仓:"<<endl;    
         m_Event.SignalEvent();
         //ReqQryInvestorPosition_all();//查询所有合约持仓信息 
      } 
   } else {
      if(firs_inquiry_TradingAccount == true) {
         firs_inquiry_TradingAccount = false;
         sleep(1); 
         cerr<<"资金查询出错,查询投资者持仓:"<<endl;        
         m_Event.SignalEvent();
         //ReqQryInvestorPosition_all();//查询所有合约持仓信息 
      }
   } 
   cerr<<"-----------------------------------------------请求查询资金响应结束"<<endl;
}

void TAP_CDECL Trade::OnRtnFund( const TapAPIFundData *info ) {
   cout << __FUNCTION__ << " is called." << endl;
}

void Trade::QryExchange() {
   TAPIUINT32 sessionID = 0;
   int ret = m_ptradeApi->QryExchange(&sessionID);
   cerr<<" 请求 | 发送交易所信息查询..."<<((ret == 0)?"成功":"失败")<<endl;

   if (!ret) m_Event.WaitEvent();
}

void TAP_CDECL Trade::OnRspQryExchange( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   if (!errorCode && info) {
      cout << "查询交易所信息成功" << endl;
   } else {
      cout << "查询交易所信息失败" << endl;
   }
   m_Event.SignalEvent();
}

void Trade::QryCommodity() {
   TAPIUINT32 sessionID = 0;
   int ret = m_ptradeApi->QryCommodity(&sessionID);

   cerr<<" 请求 | 发送系统品种信息查询..."<<((ret == 0)?"成功":"失败")<<endl;

   if (!ret) m_Event.WaitEvent();
}

void TAP_CDECL Trade::OnRspQryCommodity( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   if (!errorCode && info) {
      cout << "品种信息:" << endl;
   }

   m_Event.SignalEvent();
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
   //cout << __FUNCTION__ << " is called." << endl;
   if (!errorCode && info) {
      if(first_inquiry_Instrument == true) {
         m_Instrument_all = m_Instrument_all + info->ExchangeNo + ","; 
         TapAPITradeContractInfo* instField = new TapAPITradeContractInfo();
         memcpy(instField,  info, sizeof(TapAPITradeContractInfo));
         m_instMessage_map.insert(pair<string, TapAPITradeContractInfo*> (instField->ExchangeNo, instField));

         cout << "info->ExchangeNo: " << info->ExchangeNo << endl;
         cout << "info->ContractNo1: " << info->ContractNo1 << endl;
         cout << "info->CommodityNo: " << info->CommodityNo << endl;
         cout << "info->CommodityType: " << info->CommodityType << endl;
         /*
            if(strcmp(m_instId, info->ContractNo1) == 0) {
            cerr<<" 响应 | 合约:"<<info->InstrumentID
            <<" 合约名称:"<<info->InstrumentName
            <<" 合约在交易所的代码:"<<info->ExchangeInstID
            <<" 产品代码:"<<info->ProductID
            <<" 产品类型:"<<info->ProductClass
            <<" 交割月:"<<info->DeliveryMonth
            <<" 多头保证金率:"<<info->LongMarginRatio
            <<" 空头保证金率:"<<info->ShortMarginRatio
            <<" 合约数量乘数:"<<info->VolumeMultiple
            <<" 最小变动价位:"<<info->PriceTick
            <<" 交易所代码:"<<info->ExchangeID
            <<" 交割年份:"<<info->DeliveryYear
            <<" 交割月:"<<info->DeliveryMonth
            <<" 创建日:"<<info->CreateDate
            <<" 上市日:"<<info->OpenDate
            <<" 到期日:"<<info->ExpireDate
            <<" 开始交割日:"<<info->StartDelivDate
            <<" 结束交割日:"<<info->EndDelivDate
            <<" 合约生命周期状态:"<<info->InstLifePhase
            <<" 当前是否交易:"<<info->IsTrading
            <<endl; 
            }
          */

         if(isLast) {
            first_inquiry_Instrument = false; 
            m_Instrument_all = m_Instrument_all.substr(0,m_Instrument_all.length()-1);

            cerr<<"m_Instrument_all大小："<<m_Instrument_all.length()<<","<<m_Instrument_all<<endl; 
            cerr<<"map大小(合约个数）："<<m_instMessage_map.size()<<endl;

            map<string, TapAPITradeContractInfo*> map_tmp = m_instMessage_map;
            g_strategy->set_instMessage_map_stgy(m_instMessage_map); 
            cerr<<"----------------------------输出合约信息map的内容----------------------------"<<endl; 
            cerr<<"TD初始化完成:"<<endl;
            //m_pMDUserApi_td->Init(); 
         } 
      }
   }
   m_Event.SignalEvent();
}

void TAP_CDECL Trade::OnRtnContract( const TapAPITradeContractInfo *info ) {
   //	cout << __FUNCTION__ << " is called." << endl;
}

void Trade::insertOrder() {
   TAPIUINT32 sessionID = 0;
   TapAPINewOrder stNewOrder;
   memset(&stNewOrder,0,sizeof(stNewOrder));

   strcpy(stNewOrder.AccountNo, username);
   /*
    * HKEX_HSI_1703 可作为一个认证标志.
    * TODO:
    */
   strcpy(stNewOrder.ExchangeNo,"HKEX");
   strcpy(stNewOrder.CommodityNo,"HSI");
   strcpy(stNewOrder.ContractNo,"1703");
   stNewOrder.CommodityType = TAPI_COMMODITY_TYPE_FUTURES;
   stNewOrder.HedgeFlag = TAPI_HEDGEFLAG_T;
   stNewOrder.OrderType = TAPI_ORDER_TYPE_STOP_LIMIT;
   stNewOrder.IsRiskOrder = APIYNFLAG_NO;
   stNewOrder.OrderSide = TAPI_SIDE_BUY;
   stNewOrder.OrderPrice = 2320;
   stNewOrder.OrderQty = 3;
   stNewOrder.OrderMinQty = 1;
   stNewOrder.RefInt = 1000;
   stNewOrder.StopPrice = 2320;
   stNewOrder.PositionEffect = TAPI_PositionEffect_NONE;
   stNewOrder.PositionEffect2 = TAPI_PositionEffect_NONE;
   stNewOrder.CallOrPutFlag = TAPI_CALLPUT_FLAG_NONE;
   stNewOrder.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;
   stNewOrder.OrderSource = TAPI_ORDER_SOURCE_ESUNNY_API;
   stNewOrder.TimeInForce = TAPI_ORDER_TIMEINFORCE_GFD;
   stNewOrder.TacticsType = TAPI_TACTICS_TYPE_NONE;
   stNewOrder.TriggerCondition = TAPI_TRIGGER_CONDITION_NONE;
   stNewOrder.TriggerPriceType = TAPI_TRIGGER_PRICE_NONE;
   stNewOrder.AddOneIsValid = APIYNFLAG_NO;
   
   int ret = m_ptradeApi->InsertOrder(&sessionID, &stNewOrder);
   cerr<<" 请求 | 发送下单请求..."<<((ret == 0)?"成功":"失败")<<" ret:"<<ret<<endl;
   if (!ret) m_Event.WaitEvent();
}

void TAP_CDECL Trade::OnRtnOrder( const TapAPIOrderInfoNotice *info ) {
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
         }
      }
   }
   m_Event.SignalEvent();
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
   //qry_req.OrderQryType = TAPI_ORDER_QRY_TYPE_ALL;
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
         }
      }
   } else {
      if(first_inquiry_order == true ) {
         first_inquiry_order = false;
         sleep(1);
         cerr<<"查询报单出错，或没有报单，首次查询成交:"<<endl;
      }
   }
   m_Event.SignalEvent();
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
   strcpy(qry_req.AccountNo, username);

   int ret = m_ptradeApi->QryFill(&sessionID, &qry_req);
   cerr<<" 请求 | 发送查询成交..."<<((ret == 0)?"成功":"失败")<<" ret:"<<ret<<endl;

   m_Event.WaitEvent();

   cout << "请求 | 查询成交完成" << endl;
}

void TAP_CDECL Trade::OnRspQryFill( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   cerr<<"请求查询成交响应:OnRspQryTrade. info = " << info << endl;

   if ((errorCode == 0) && info) {
      if(first_inquiry_trade == true) {
         TapAPIFillInfo* trade = new TapAPIFillInfo();
         memcpy(trade,  info, sizeof(TapAPIFillInfo));
         tradeList.push_back(trade); 

         if(isLast) {
            first_inquiry_trade = false;

            cerr<<"成交次数:"<<tradeList.size()<<endl;

            cerr<<"--------------------------------------------------------------------成交start"<<endl;
            for(vector<TapAPIFillInfo*>::iterator iter = tradeList.begin(); iter != tradeList.end(); iter++) {  
               cerr<<"合约代码:"<< (*iter)->ContractNo << endl<<" 用户代码:"<< (*iter)->AccountNo << endl<<" 成交编号:"<< (*iter)->OrderNo << endl<<" 买卖方向:"<< endl<<" 开平标志:"<< (*iter)->PositionEffect << endl
                 <<" 投机套保标志:"<< endl<<" 价格:"<< (*iter)->MatchPrice << endl<<" 数量:"<< (*iter)->MatchQty << endl<<" 成交时间:"<<endl
                 <<" 成交类型:"<< endl<<" 报单编号:"<< "?" << endl<<" 报单引用:"<< "?" << endl<<" 本地报单编号:"<< (*iter)->MatchNo << endl
                 <<" 业务单元:"<<endl<<" 序号:"<<endl<<" 经纪公司报单编号:"<<endl
                 <<" 成交时期:"<< (*iter)->MatchDateTime << endl<<" 交易日:"

                 << "?" << endl<<endl;  
            }
            cerr<<"--------------------------------------------------------------------成交end"<<endl; 
            sleep(1); 
            cerr<<"查询成交正常，首次查询投资者持仓明细:"<<endl; 
         }
      }
   } else {
      if(first_inquiry_trade == true) {
         first_inquiry_trade = false;
         sleep(1);

         cerr<<"查询成交出错，或没有成交，首次查询投资者持仓明细: errorcode = " << errorCode << endl;
      }
   }
   m_Event.SignalEvent();
}

void TAP_CDECL Trade::OnRtnFill( const TapAPIFillInfo *info ) {
   //	cout << __FUNCTION__ << " is called." << endl;
}

// 查询用户持仓
void Trade::QryPosition() {
   TAPIUINT32 sessionID = 0;
   TapAPIPositionQryReq qry_req;
   memset(&qry_req, 0, sizeof(TapAPIPositionQryReq));
   strcpy(qry_req.AccountNo, username);

   int ret = m_ptradeApi->QryPosition(&sessionID, &qry_req);
   cerr<<" 请求 | 发送查询持仓..."<<((ret == 0)?"成功":"失败")<<" ret:"<<ret<<endl;

   if (!ret) m_Event.WaitEvent(); 
   return ;
}

void TAP_CDECL Trade::OnRspQryPosition( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
   if (!errorCode && info) {
      cerr<<"请求查询持仓响应:OnRspQryInvestorPosition"<< "info:" << info << endl;//>>>>>>>>
      cerr<<" 响应 | 合约:"<< info->ContractNo
        << " 持仓多空方向:"<< "?"
        << " 映射后的方向" << "?"
        << " 总持仓(今日持仓,错误名字):"<< info->PositionQty
        << " 上日持仓:" << "?"
        << " 今日持仓:" << info->PositionQty
        << " 保证金:" << "?"
        << " 持仓成本:" << info->PositionPrice
        << " 开仓量:" << "?"
        << " 平仓量:" << "?"
        << " 持仓日期:" << "?"
        << " 平仓盈亏(按昨结):" << "?"
        << " 持仓盈亏:" << info->PositionProfit
        << " 逐日盯市平仓盈亏(按昨结):" << "?"
        << " 逐笔对冲平仓盈亏(按开平仓价):" << "?"
        << endl;//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

      bool find_trade_message_map = false;
      for(map<string, trade_message*>::iterator iter = m_trade_message_map.begin(); iter!= m_trade_message_map.end();iter++) {
         if(strcmp( (iter->first).c_str(), info->ContractNo)==0) { //合约已存在 
            find_trade_message_map = true;
            break;
         }
      }

      if(!find_trade_message_map) {
         cerr<<"--------------------------------------------没有这个合约，构造交易信息结构体"<<endl; 
         trade_message* trade_message_p = new trade_message(); 

         trade_message_p->instId = info->ContractNo;

         m_trade_message_map.insert(pair<string, trade_message*> (info->ContractNo, trade_message_p));
      }

      /*
       * @shilei
       * TODO. 
       * 没有发现对应的PosiDirection 这个字段.
       *
       * @update.
       * 1，首先判断现在持仓状况。如果持仓等于0.买入并成交。持有多头
       *                                        卖出并成交。 持有空头
       * 2，如果现在持有空仓，买入同等手数并成交。平仓
       *                      买入大于空仓持仓手数并成交，多头
       *                      卖出手数并成交。空头
       * 3，如果现在持有多头。同上
       * 2017.01.16
       */
      /*
         if (info->PositionQty == 0) { // 持仓等于0
         if (info->MatchSide == TAPI_SIDE_BUY) { // 买入并成交
      // 持有多头
      m_trade_message_map[info->ContractNo]->holding_long = info->PositionQty;
      }
      }
       */
      /*
         if(info->PosiDirection == '2'){//多单 
         m_trade_message_map[info->InstrumentID]->holding_long = info->Position;
         m_trade_message_map[info->InstrumentID]->TodayPosition_long = info->TodayPosition;
         m_trade_message_map[info->InstrumentID]->YdPosition_long = info->Position - info->TodayPosition;
         m_trade_message_map[info->InstrumentID]->closeProfit_long =  info->CloseProfit;
         m_trade_message_map[info->InstrumentID]->OpenProfit_long = info->PositionProfit;
         } else if(pInvestorPosition->PosiDirection == '3'){
         m_trade_message_map[pInvestorPosition->InstrumentID]->holding_short = pInvestorPosition->Position;
         m_trade_message_map[pInvestorPosition->InstrumentID]->TodayPosition_short = pInvestorPosition->TodayPosition;
         m_trade_message_map[pInvestorPosition->InstrumentID]->YdPosition_short = pInvestorPosition->Position - m_trade_message_map[pInvestorPosition->InstrumentID]->TodayPosition_short;
         m_trade_message_map[pInvestorPosition->InstrumentID]->closeProfit_short = pInvestorPosition->CloseProfit;
         m_trade_message_map[pInvestorPosition->InstrumentID]->OpenProfit_short = pInvestorPosition->PositionProfit;
         }
       */
      if (isLast) {
         firs_inquiry_Position = false;
         for(map<string, trade_message*>::iterator iter = m_trade_message_map.begin(); iter != m_trade_message_map.end(); iter++) {
            cerr<<"合约代码:"<< iter->first<<","<<iter->second->instId<<endl
              <<" 多单持仓量:"<<iter->second->holding_long<<endl
              <<" 空单持仓量:"<<iter->second->holding_short<<endl
              <<" 多单今日持仓:"<<iter->second->TodayPosition_long<<endl
              <<" 多单上日持仓:"<<iter->second->YdPosition_long<<endl
              <<" 空单今日持仓:"<<iter->second->TodayPosition_short<<endl
              <<" 空单上日持仓:"<<iter->second->YdPosition_short<<endl
              <<" 多单平仓盈亏:"<<iter->second->closeProfit_long<<endl
              <<" 多单浮动盈亏:"<<iter->second->OpenProfit_long<<endl
              <<" 空单平仓盈亏:"<<iter->second->closeProfit_short<<endl
              <<" 空单浮动盈亏:"<<iter->second->OpenProfit_short<<endl
              <<endl;
            m_closeProfit = m_closeProfit + iter->second->closeProfit_long + iter->second->closeProfit_short;
            m_OpenProfit = m_OpenProfit + iter->second->OpenProfit_long + iter->second->OpenProfit_short;
         } 
         cerr<<"查询持仓返回结束:"<<endl;
         cerr<<"账户平仓盈亏:"<<m_closeProfit<<endl
           <<"账户浮动盈亏:"<<m_OpenProfit<<endl
           <<endl;
         sleep(1);
         cerr<<"查询持仓正常，首次查询合约（所有合约）:"<<endl; 
      }
   } else {
      if(firs_inquiry_Detail == true) {
         firs_inquiry_Detail = false;
         sleep(1); 
         cerr<<"查询投资者持仓明细出错，或没有投资者持仓明细，查询资金账户:"<<endl;
      } 
   } 
   cerr<<"-----------------------------------------------请求查询投资者持仓明细响应结束"<<endl;
   m_Event.SignalEvent();
}

void TAP_CDECL Trade::OnRtnPosition( const TapAPIPositionInfo *info ) {
   //	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryClose( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo *info ) {
   cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnClose( const TapAPICloseInfo *info ) {
   //	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnPositionProfit( const TapAPIPositionProfitNotice *info ) {
   //	cout << __FUNCTION__ << " is called." << endl;
}

/*
   void TAP_CDECL Trade::OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIDeepQuoteQryRsp *info)
   {
//	cout << __FUNCTION__ << " is called." << endl;
}
 */

/*
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
 */

void Trade::OnRspQryCurrency(unsigned int, int, char, ITapTrade::TapAPICurrencyInfo const*) {
}

void Trade::OnRspQryTradeMessage(unsigned int, int, char, ITapTrade::TapAPITradeMessage const*) {
}

void Trade::OnRtnTradeMessage(ITapTrade::TapAPITradeMessage const*) {
}

void Trade::OnRspQryHisOrder(unsigned int, int, char, ITapTrade::TapAPIHisOrderQryRsp const*) {
}

void Trade::OnRspQryHisOrderProcess(unsigned int, int, char, ITapTrade::TapAPIHisOrderQryRsp const*) {
}

void Trade::OnRspQryHisMatch(unsigned int, int, char, ITapTrade::TapAPIHisMatchQryRsp const*) {
}

void Trade::OnRspQryHisPosition(unsigned int, int, char, ITapTrade::TapAPIHisPositionQryRsp const*) {
}

void Trade::OnRspQryHisDelivery(unsigned int, int, char, ITapTrade::TapAPIHisDeliveryQryRsp const*) {
}

void Trade::OnRspQryAccountCashAdjust(unsigned int, int, char, ITapTrade::TapAPIAccountCashAdjustQryRsp const*) {
}

void Trade::OnRspQryBill(unsigned int, int, char, ITapTrade::TapAPIBillQryRsp const*) {
}

void Trade::printTrade_message_map() {
   cerr<<"------------------------------------------------printTrade_message_map开始"<<endl;
   map<string, trade_message*> map_test;
   cout << "map_test.size = " << map_test.size() << endl;//>>>>>>
   cout << "m_trade_message_map.size = " << m_trade_message_map.size();//>>>>
   for(map<string, trade_message*>::iterator iter = m_trade_message_map.begin(); iter != m_trade_message_map.end(); iter++) {
      cout << "first:" << iter->first << endl;//>>>>>
      cout << "second:" << iter->second << endl;//>>>>>
      if(iter->second->holding_long > 0 || iter->second->holding_short > 0) {
         cout << "here" << endl;//>>>>
         cerr<<"合约代码:"<< iter->first<<","<<iter->second->instId<<endl
           <<" 多单持仓量:"<<iter->second->holding_long<<endl
           <<" 空单持仓量:"<<iter->second->holding_short<<endl
           <<" 多单今日持仓:"<<iter->second->TodayPosition_long<<endl
           <<" 多单上日持仓:"<<iter->second->YdPosition_long<<endl
           <<" 空单今日持仓:"<<iter->second->TodayPosition_short<<endl
           <<" 空单上日持仓:"<<iter->second->YdPosition_short<<endl
           <<" 多单平仓盈亏:"<<iter->second->closeProfit_long<<endl
           <<" 多单浮动盈亏:"<<iter->second->OpenProfit_long<<endl
           <<" 空单平仓盈亏:"<<iter->second->closeProfit_short<<endl
           <<" 空单浮动盈亏:"<<iter->second->OpenProfit_short<<endl
           <<endl;
      }
   }
   cerr<<"------------------------------------------------printTrade_message_map结束"<<endl;
}

void Trade::forceClose() {
   cout << "-----------------------------------in Force Close---------------" << endl;
   /*
      TThostFtdcInstrumentIDType    instId;//合约
      TThostFtdcDirectionType       dir;//方向,'0'买，'1'卖
      TThostFtdcCombOffsetFlagType  kpp;//开平，"0"开，"1"平,"3"平今
      TThostFtdcPriceType           price;//价格，0是市价,上期所不支持
      TThostFtdcVolumeType          vol;//数量
    */
   string instId;
   int dir;
   char* kpp;
   int price;
   int vol;

   for(map<string, trade_message*>::iterator iter = m_trade_message_map.begin(); iter != m_trade_message_map.end(); iter++)
   {

      cerr<<"合约代码:"<< iter->first<<","<<iter->second->instId<<endl
        <<" 多单持仓量:"<<iter->second->holding_long<<endl
        <<" 空单持仓量:"<<iter->second->holding_short<<endl
        <<" 多单今日持仓:"<<iter->second->TodayPosition_long<<endl
        <<" 多单上日持仓:"<<iter->second->YdPosition_long<<endl
        <<" 空单今日持仓:"<<iter->second->TodayPosition_short<<endl
        <<" 空单上日持仓:"<<iter->second->YdPosition_short<<endl
        <<" 多单平仓盈亏:"<<iter->second->closeProfit_long<<endl
        <<" 多单浮动盈亏:"<<iter->second->OpenProfit_long<<endl
        <<" 空单平仓盈亏:"<<iter->second->closeProfit_short<<endl
        <<" 空单浮动盈亏:"<<iter->second->OpenProfit_short<<endl
        <<endl;

      //平多
      // TODO: 
   }
}

ITapTradeAPI *Trade::createTapTradeApi(TAPIAUTHCODE authCode, TAPISTR_300 keyOperationLogPath, int errorCode) {
   TAPIINT32 iResult = TAPIERROR_SUCCEED;
   TapAPIApplicationInfo stAppInfo;
   strcpy(stAppInfo.AuthCode, authCode);
   strcpy(stAppInfo.KeyOperationLogPath, keyOperationLogPath);
   ITapTradeAPI *pApi = CreateITapTradeAPI(&stAppInfo, iResult);
   errorCode = iResult;
   return pApi;
}
