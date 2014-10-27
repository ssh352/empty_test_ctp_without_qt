#include"ctp_quote.h"
#include<iostream>
#include<string>
#include<list>
#include"gpp_qt/wfunction/wfunction.h"
#include"gpp_qt/cfg/cfg.h"

extern CThostFtdcMdApi * pUserApi;
extern cfg simucfg;

using namespace std;

void ctp_quote::init()
{
	nRequestID=0;
	nppInstrumentID=0;
		
	string BROKER_ID="2030";
	string INVESTOR_ID="00092";
	string PASSWORD="888888";

	req=new CThostFtdcReqUserLoginField;
	memset(req, 0, sizeof(*req));
	strncpy(req->BrokerID,const_cast<char*>(BROKER_ID.c_str()),sizeof(req->BrokerID));
	strncpy(req->UserID,const_cast<char*>(PASSWORD.c_str()),sizeof(req->UserID));
	strncpy(req->Password,const_cast<char*>(PASSWORD.c_str()),sizeof(req->Password));


	ppInstrumentID=new char * [MAX_CONTRACT_NUMBER];
	list<string> contracts=wfunction::splitstring(simucfg.getparam("INSTRUMENT_ID"));
	for(list<string>::iterator iter=contracts.begin();iter!=contracts.end();iter++)
	{
		ppInstrumentID[nppInstrumentID]=new char [MAX_CONTRACT_NAME];
		memset(ppInstrumentID[nppInstrumentID],'\0',MAX_CONTRACT_NAME);
		strncpy(ppInstrumentID[nppInstrumentID],iter->c_str(),iter->size());
		nppInstrumentID++;
	}
}

void ctp_quote::ReqUserLogin()
{
	int iResult = pUserApi->ReqUserLogin(req, ++nRequestID);
	cout << "--->>> �����û���¼����: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void ctp_quote::OnRspError(CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast)
{
	//�в�����ھ�����������»ᴥ���˻ص�
	cout<< "--->>> "<< "OnRspError\t RequeseID="<<nRequestID<< endl;
	IsErrorRspInfo(pRspInfo);
}
void ctp_quote::OnFrontDisconnected(int nReason)
{
	cout << "--->>> " << "OnFrontDisconnected" << endl;
	cout << "Reasons\n\t0x1001 �����ʧ��\n\t0x1002 ����дʧ��\n\t0x2001 ����������ʱ\n\t0x2002 ��������ʧ��\n\t0x2003 �յ�������"<<endl;
	cout << "\tReason = " << nReason << endl;
}

///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
void ctp_quote::OnFrontConnected()
{
	cout << "--->>> " << "OnFrontConnected" << endl;
	ReqUserLogin();
}
	
///��¼������Ӧ
void ctp_quote::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///��ȡ��ǰ������
		cout << "--->>> ��ȡ��ǰ������ = " << pUserApi->GetTradingDay()<< endl;
		// ����������
		SubscribeMarketData();
	}
}
void ctp_quote::SubscribeMarketData()
{
/*	
	char * p[]={"IF1411","IF1412"};
	int np=2;
*/	


	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, nppInstrumentID);
	//int iResult = pUserApi->SubscribeMarketData(p, np);
	cout << "--->>> �������鶩������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

///��������Ӧ��
void ctp_quote::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspSubMarketData" << endl;
}

///ȡ����������Ӧ��
void ctp_quote::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspUnSubMarketData" << endl;
}

///�������֪ͨ
void ctp_quote::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	cout<< pDepthMarketData->TradingDay;
	cout<< "," << pDepthMarketData->UpdateTime;
	cout<< ":" << pDepthMarketData->UpdateMillisec;
	cout<< "," << pDepthMarketData->InstrumentID;
	cout<< "," << pDepthMarketData->BidPrice1;
	cout<< "," << pDepthMarketData->AskPrice1;
	cout<< "," << pDepthMarketData->BidVolume1;
	cout<< "," << pDepthMarketData->AskVolume1;
	cout<< "," << pDepthMarketData->LastPrice;
	cout<< "," << pDepthMarketData->HighestPrice;
	cout<< "," << pDepthMarketData->LowestPrice;
	cout<< "," << pDepthMarketData->Volume;
	cout<< endl;
}
bool ctp_quote::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cout << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}
