#pragma once
namespace LTESim
{
    //��db��ֵת��Ϊʵ��
    inline double dBToReal(double db)
    {
        return std::pow(10.0, db/10.0);
    }
    //��ʵ��ת��Ϊdb��ֵ
    inline double RealTodB(double realval)
    {
        return 10*log10(realval);
    }
    class DB
    {
    public:
        ///���캯��
        /*
        * @param dbvalue��double�͵�dbֵ
        */
        explicit DB(double dbvalue) : m_dbval(dbvalue)
        {

        }
    public:
        ///��������
        ~DB(void)
        {
        }

        //���ؼӷ������
        DB operator + ( const DB& _db ) const	
        {
            DB db(*this);
            db.m_dbval += _db.m_dbval;
            return db;
        }
        //���ؼ���Ԥ���
        DB operator - ( const DB& _db ) const	
        {
            DB db(*this);
            db.m_dbval -= _db.m_dbval;
            return db;
        }

        //���ء�+=��
        DB& operator += (const DB& _db)	
        {
            this->m_dbval += _db.m_dbval;
            return *this;
        }

        //ȡ��
        DB operator -() const	
        {
            DB db(-this->m_dbval);
            return db;
        }

        //��ʽ����ת��
        operator double() const	
        {
            return m_dbval;
        }

        //ת��Ϊʵֵ
        static double dBToReal(const DB& dB)
        {
            return LTESim::dBToReal(dB.m_dbval);
        }

	public:
        double m_dbval;
    };

}

