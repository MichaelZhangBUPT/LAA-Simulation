#pragma once
#include <string>
#include <map>
namespace LTESim
{
	class IntValue;//�洢�������ݵ���
	class DoubleValue;//�洢���������ݵ���
	class BoolValue;//�洢���������ݵ���
	class StringValue;//�洢�ַ������ݵ���

	//���ĳ��Run�Ĳ���
	class RunParameters
	{
	public:
		RunParameters(void);
	public:
		virtual ~RunParameters(void);
		//��Run��Drop��
		int GetDrops() const;

		//ÿ��Drop��ʱ��
		int GetDropTime() const;
		void SetRunIndex(int RunIndex)
		{
			m_RunIndex = RunIndex;
		};
		bool Save( std::ostream& savefile ) const;//���ı�����ʾ
		bool AddInt(const IntValue& value);
		bool AddDouble(const DoubleValue& value);
		bool AddBoolean(const BoolValue& value);
		bool AddString(const StringValue& value);

		int GetInt(const std::string& key) const;
		double GetDouble(const std::string& key) const;
		bool GetBool(const std::string& key) const;
		const std::string& GetString(const std::string& key) const;

	protected:
		int m_RunIndex;//Run�ı��
		mutable std::map<std::string, int > m_IntValues;  //���������
		mutable std::map<std::string, double> m_DoubleValues; //��Ÿ�������
		mutable std::map<std::string, bool> m_BoolValues; //��Ų�����
		mutable std::map<std::string, std::string> m_StringValues; //����ַ�����
	};

}
