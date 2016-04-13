#include "RandomManager.h"
#include <ctime>
namespace LTESim
{

RandomManager::RandomManager(void)      
: m_SeedGen(base_random_generator_type(), boost::uniform_int<unsigned int>(0u, 100000u))
{
    
}

RandomManager::~RandomManager(void)
{

}

UNIFORM_INT_GEN RandomManager::GetUniformIntGen( int min, int max )
{
	//ͨ����һ�����һ��������ӳ�ʼ��һ�����ȷֲ�
    m_basegenerator.seed(m_SeedGen());
    return UNIFORM_INT_GEN(m_basegenerator,  boost::uniform_int<>(min, max));
}

UNIFORM_REAL_GEN RandomManager::GetUniformRealGen( double min, double max )
{
	//ͨ����һ�����һ��������ӳ�ʼ��һ�����ȷֲ�
    m_basegenerator.seed(m_SeedGen());
    return UNIFORM_REAL_GEN(m_basegenerator,  boost::uniform_real<>(min, max));
}

NORMAL_DOUBLE_GEN RandomManager::GetNormalGen( double mean, double sigma )
{
	//ͨ����һ�����һ��������ӳ�ʼ��һ����̬�ֲ�
    m_basegenerator.seed(m_SeedGen());
    return NORMAL_DOUBLE_GEN(m_basegenerator,  boost::normal_distribution<>(mean, sigma));
}

void RandomManager::Seed( boost::uint32_t s )
{
	//����
    m_SeedGen.engine().seed(s);
    //m_pSeedGen.reset(new SEED_GEN(m_seedbase, boost::uniform_int<unsigned int>(0u, 100000u)));
}

RandomManager& RandomManager::GetRandomManager()
{
	//��������������һ���������������
    static RandomManager randman;
    return randman;
}

EXPONENTIAL_DOUBLE_GEN RandomManager::GetExponenialDoubleGen( double beta/*lambda��ָ��ֵ*/ )
{
	//ͨ����һ�����һ��������ӳ�ʼ��һ��ָ���ֲ�
	m_basegenerator.seed(m_SeedGen());
	return EXPONENTIAL_DOUBLE_GEN(m_basegenerator, boost::exponential_distribution<>(beta));
}

LOGNORMAL_DOUBLE_GEN RandomManager::GetLognormalDoubleGen( double mean/*��ֵ*/, double sigma/*��׼��*/ )
{
	//ͨ����һ�����һ��������ӳ�ʼ��һ��������̬�ֲ�
	m_basegenerator.seed(m_SeedGen());
	return LOGNORMAL_DOUBLE_GEN(m_basegenerator, boost::lognormal_distribution<>(mean, sigma));
}
}
