#pragma once
#include <string>
#include <map>
namespace LTESim
{
	class IntValue;//存储整型数据的类
	class DoubleValue;//存储浮点型数据的类
	class BoolValue;//存储布尔型数据的类
	class StringValue;//存储字符型数据的类

	//针对某个Run的参数
	class RunParameters
	{
	public:
		RunParameters(void);
	public:
		virtual ~RunParameters(void);
		//该Run的Drop数
		int GetDrops() const;

		//每个Drop的时长
		int GetDropTime() const;
		void SetRunIndex(int RunIndex)
		{
			m_RunIndex = RunIndex;
		};
		bool Save( std::ostream& savefile ) const;//在文本中显示
		bool AddInt(const IntValue& value);
		bool AddDouble(const DoubleValue& value);
		bool AddBoolean(const BoolValue& value);
		bool AddString(const StringValue& value);

		int GetInt(const std::string& key) const;
		double GetDouble(const std::string& key) const;
		bool GetBool(const std::string& key) const;
		const std::string& GetString(const std::string& key) const;

	protected:
		int m_RunIndex;//Run的编号
		mutable std::map<std::string, int > m_IntValues;  //存放整型数
		mutable std::map<std::string, double> m_DoubleValues; //存放浮点型数
		mutable std::map<std::string, bool> m_BoolValues; //存放布尔型
		mutable std::map<std::string, std::string> m_StringValues; //存放字符型数
	};

}
