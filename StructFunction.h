#ifndef _STRUCTFUNCTION_H
#define _STRUCTFUNCTION_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;


//������Ϣ�Ľṹ��
struct trade_message
{
	trade_message()
	{
		instId = "";
		lastPrice = 0.0;
		PreSettlementPrice = 0.0;
		holding_long = 0;
		holding_short = 0;
		TodayPosition_long = 0;
		YdPosition_long = 0;
		TodayPosition_short = 0;
		YdPosition_short = 0;

		closeProfit_long = 0.0;
		OpenProfit_long = 0.0;
		closeProfit_short = 0.0;
		OpenProfit_short = 0.0;
	}


	string instId;//��Լ����
	double lastPrice;//���¼ۣ�ʱ�̱����Լ�����¼ۣ�ƽ����
	double PreSettlementPrice;//�ϴν���ۣ��Ը�ҹ����ʱ��Ҫ�ã���������
	int holding_long;//�൥�ֲ���
	int holding_short;//�յ��ֲ���

	int TodayPosition_long;//�൥���ճֲ�
	int YdPosition_long;//�൥���ճֲ�

	int TodayPosition_short;//�յ����ճֲ�
	int YdPosition_short;//�յ����ճֲ�

	double closeProfit_long;//�൥ƽ��ӯ��
	double OpenProfit_long;//�൥����ӯ��

	double closeProfit_short;//�յ�ƽ��ӯ��
	double OpenProfit_short;//�յ�����ӯ��

};




struct FutureData//����ṹ�嶨��
{
	string date;
	string time;
	double buy1price;
	int buy1vol;
	double new1;
	double sell1price;
	int sell1vol;
	double vol;
	double openinterest;//�ֲ���

};



//��ʷK��
struct History_data
{
	string date;
	string time;
	double buy1price;//��һ
	double sell1price;//��һ
	double open;
	double high;
	double low;
	double close;

};



//��ȡ��ʷK��
void ReadDatas(string fileName, vector<History_data> &history_data_vec);



int Store_fileName(string path, vector<string> &FileName);

#endif