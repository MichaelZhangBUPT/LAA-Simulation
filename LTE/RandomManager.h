#pragma once
#include <boost/random.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost::random;
namespace LTESim
{
typedef ::boost::mt19937 base_random_generator_type;
typedef ::boost::variate_generator< base_random_generator_type, boost::uniform_int<> > UNIFORM_INT_GEN;
typedef ::boost::variate_generator< base_random_generator_type, boost::uniform_real<> > UNIFORM_REAL_GEN;
typedef ::boost::variate_generator< base_random_generator_type, boost::normal_distribution<> > NORMAL_DOUBLE_GEN;
typedef ::boost::variate_generator< base_random_generator_type, boost::exponential_distribution<> > EXPONENTIAL_DOUBLE_GEN;
typedef ::boost::variate_generator< base_random_generator_type, boost::lognormal_distribution<> > LOGNORMAL_DOUBLE_GEN;

class RandomManager : public boost::noncopyable
{

protected:
    RandomManager(void);
public:
    ~RandomManager(void);
	//��������
    static RandomManager& GetRandomManager();

	//��ʼ�����õ�һ���������
    void Seed(boost::uint32_t seed);
    
	//����һ��INT���ȷֲ���������
	UNIFORM_INT_GEN GetUniformIntGen(int min/*�½�*/, int max/*�Ͻ�*/);

	//����һ��double���ȷֲ���������
	UNIFORM_REAL_GEN GetUniformRealGen(double min/*�½�*/, double max/*�Ͻ�*/);

    //����һ����̬�ֲ���������
    NORMAL_DOUBLE_GEN GetNormalGen(double mean/*��ֵ*/, double sigma/*��׼��*/);

	//����һ��ָ���ֲ���������
	EXPONENTIAL_DOUBLE_GEN GetExponenialDoubleGen(double beta/*lambda��ָ��ֵ*/);

	//����һ��������̬�ֲ���������
	LOGNORMAL_DOUBLE_GEN GetLognormalDoubleGen(double mean/*��ֵ*/, double sigma/*��׼��*/);

protected:
	//��������ֲ���ĸ������
    base_random_generator_type m_basegenerator;
protected:
    //����ļ��������������������seed�����ã�������������������ã���һ�����Ӹ�������һ���������
	//����������Ĳ�����Ϊ�ڶ��������������ӣ��ڶ����������������û�ʹ�õ����������
	//����ͨ���Ե�һ������seed������Ӱ�쵽�ڶ����seed���ڶ���������֮��ʹ�õ�һ������Ĳ�ͬ��seed���һ������

	//����seed�����������ͣ����ȷֲ���
    typedef ::boost::variate_generator< base_random_generator_type, boost::uniform_int<unsigned int> > SEED_GEN;
	
	//����seed��������
    SEED_GEN m_SeedGen;
};

}
