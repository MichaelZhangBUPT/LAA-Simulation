#pragma once
namespace LTESim
{
	//����ͳ�Ʋ���Visitorģʽ
	class Service;
	class DLFTPService;
	class ServiceStatistic
	{
	public:
		ServiceStatistic(void);
	public:
		virtual ~ServiceStatistic(void);
		virtual void Visit(Service& service) = 0;								//����ͳ�ƽӿ�	
		virtual const vector<double>& GetStatistic() const = 0;	//���ÿ���û���ͳ������
		virtual const vector<double>& GetAllStatistic() { vector<double> temp(1); return temp; }	//���ԭʼ����
		virtual double GetSumStatistic() const =0;						//���ϵͳ��ͳ������
	};

	//������ͳ�� 
	class ThroughputStatistic : public ServiceStatistic
	{
	public:
		ThroughputStatistic(void);

	public:
		~ThroughputStatistic(void);
		void Visit(Service& service);	//����ͳ�ƽӿ�

		const vector<double>& GetStatistic(void) const;	//����û�FTPҵ��������ͳ�Ʊ�	
		double GetSumStatistic(void) const;	//���ϵͳ����������

	protected:
		vector<double> m_DLFTPthroughput;	//�洢ÿ���û�FTPҵ��������
		double DLFTPSumThroughput;	//ϵͳÿ��Drop��FTP��������

	};
	//��������ͳ��
	class DiscardThroughputStatistic : public ServiceStatistic
	{
	public:
		DiscardThroughputStatistic(void);

	public:
		~DiscardThroughputStatistic(void);
		void Visit(Service& service);	//����ͳ�ƽӿ�

		const vector<double>& GetStatistic(void) const;	//����û�FTPҵ��������������ͳ�Ʊ�	
		double GetSumStatistic(void) const;	//���ϵͳ���ܶ�������������

	protected:
		vector<double> m_DLFTPDiscardThroughput;	//�洢ÿ���û�FTPҵ��������������
		double DLFTPSumDiscardThroughput;	//ϵͳÿ��Drop��FTP�ܶ�������������
	};
	//����ʱ��ͳ��
	class PacketDelayStatistic : public ServiceStatistic
	{
	public:
		PacketDelayStatistic(void);

	public:
		~PacketDelayStatistic(void);
		void Visit(Service& service);	//����ͳ�ƽӿ�

		const vector<double>& GetStatistic(void) const;	//����û�FTPҵ���ʱ��ͳ�Ʊ�
		double GetSumStatistic(void) const;	//���ϵͳ����ʱ��

	protected:
		vector<double> m_DLFTPPacketDelay;	//�洢ÿ���û�FTPҵ���ʱ��
		double DLFTPSumPacketDelay;	//ϵͳÿ��Drop��FTP��ʱ��
	};
	//ͳ��MCS�ȼ�
	class MCSStatistic : public ServiceStatistic
	{
	public:
		MCSStatistic( /*SystemType systemtype*/ );

	public:
		~MCSStatistic(void);
		void Visit(Service& service); //MCS�ȼ�ͳ�ƽӿ�

		const vector<double>& GetStatistic(void) const; //���ϵͳ��MCS�ȼ�ͳ��
		double GetSumStatistic(void) const;
	protected:
		vector< vector<double> > m_MCS;
		vector<double> DLFTPSumMCS;
	};

	//ҵ�������ʱ��ͳ��
	class PacketTimeStatistic : public ServiceStatistic
	{
	public:
		PacketTimeStatistic(void);

	public:
		~PacketTimeStatistic(void);
		void Visit(Service& service);	//����ͳ�ƽӿ�

		const vector<double>& GetStatistic(void) const;	//����ÿ���û���ƽ��ֵ
		const vector<double>& GetAllStatistic(void) ;	//���������û������е�ֵ
		double GetSumStatistic(void) const;		//����ϵͳһ��Drop��ƽ��ֵ
	protected:
		vector<double> m_UEAverPacketTime;	//ÿ���û���ƽ��ֵ
		vector<double> m_AllPacketTime;	//�����û������е�ֵ����������ϵͳ��ƽ��ֵ
	};

	//FullBuffer ����ʱ����ͳ��
	class TransIntervalStatistic : public ServiceStatistic
	{
	public:
		TransIntervalStatistic(void);

	public:
		~TransIntervalStatistic(void);
		void Visit(Service& service);	//����ͳ�ƽӿ�

		const vector<double>& GetStatistic(void) const;	//
		double GetSumStatistic(void) const;
	protected:
		std::vector<double> m_DLFTPAverTransInterval;	//
	};
	//ҵ�񶪰���ͳ��
	class PacketLossStatistic : public ServiceStatistic
	{
	public:
		PacketLossStatistic(void);
	public:
		~PacketLossStatistic(void);
		void Visit(Service& service);	//����ͳ�ƽӿ�
		const vector<double>& GetStatistic(void) const;	//����ÿ���û��Ķ�����
		const vector<double>& GetAllStatistic(void) ;	//����ϵͳ�ܵĶ��������ܵ����ݰ�
		double GetSumStatistic(void) const;	//����ϵͳһ��Drop�Ķ�����
	protected:
		vector<double> m_UEPacketLoss;	//ÿ���û��Ķ�����
		int	m_PacketNum;	//ϵͳ�ܵ����ݰ�����
		int m_DiscardPacketNum;		//�ܵĶ��������ݰ�����
		vector<double> m_DiscardAndSum;
	};
	//�ŵ�������ͳ��,����ʹ��
	class ChannelUtilizationStatistic : public ServiceStatistic
	{
	public:
		ChannelUtilizationStatistic(void);
	public:
		~ChannelUtilizationStatistic(void);
		void Visit(Service& service);	//����ͳ�ƽӿ�
		const vector<double>& GetStatistic(void) const;	//����ÿ���û����ŵ�������
		const vector<double>& GetAllStatistic(void) ;	//����ϵͳ���������������������
		double GetSumStatistic(void) const;	//����ϵͳһ��Drop���ŵ�������
	protected:
		vector<double> m_UEChannelUti;	//ÿ���û����ŵ�������
		double m_SumThroughput;
		double m_SumIdealThroughput;
		vector<double> m_SumAndIdeal;
	};


}

