#pragma once
namespace LTESim
{
    //将db数值转化为实数
    inline double dBToReal(double db)
    {
        return std::pow(10.0, db/10.0);
    }
    //将实数转化为db数值
    inline double RealTodB(double realval)
    {
        return 10*log10(realval);
    }
    class DB
    {
    public:
        ///构造函数
        /*
        * @param dbvalue，double型的db值
        */
        explicit DB(double dbvalue) : m_dbval(dbvalue)
        {

        }
    public:
        ///析构函数
        ~DB(void)
        {
        }

        //重载加法运算符
        DB operator + ( const DB& _db ) const	
        {
            DB db(*this);
            db.m_dbval += _db.m_dbval;
            return db;
        }
        //重载减法预算符
        DB operator - ( const DB& _db ) const	
        {
            DB db(*this);
            db.m_dbval -= _db.m_dbval;
            return db;
        }

        //重载“+=”
        DB& operator += (const DB& _db)	
        {
            this->m_dbval += _db.m_dbval;
            return *this;
        }

        //取负
        DB operator -() const	
        {
            DB db(-this->m_dbval);
            return db;
        }

        //显式类型转换
        operator double() const	
        {
            return m_dbval;
        }

        //转换为实值
        static double dBToReal(const DB& dB)
        {
            return LTESim::dBToReal(dB.m_dbval);
        }

	public:
        double m_dbval;
    };

}

