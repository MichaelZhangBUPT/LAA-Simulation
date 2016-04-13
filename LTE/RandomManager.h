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
	//工厂方法
    static RandomManager& GetRandomManager();

	//初始化设置第一层随机种子
    void Seed(boost::uint32_t seed);
    
	//产生一个INT均匀分布的生成器
	UNIFORM_INT_GEN GetUniformIntGen(int min/*下界*/, int max/*上界*/);

	//产生一个double均匀分布的生成器
	UNIFORM_REAL_GEN GetUniformRealGen(double min/*下界*/, double max/*上界*/);

    //产生一个正态分布的生成器
    NORMAL_DOUBLE_GEN GetNormalGen(double mean/*均值*/, double sigma/*标准差*/);

	//产生一个指数分布的生成器
	EXPONENTIAL_DOUBLE_GEN GetExponenialDoubleGen(double beta/*lambda即指数值*/);

	//产生一个对数正态分布的生成器
	LOGNORMAL_DOUBLE_GEN GetLognormalDoubleGen(double mean/*均值*/, double sigma/*标准差*/);

protected:
	//产生随机分布的母生成器
    base_random_generator_type m_basegenerator;
protected:
    //下面的几个对象用作各随机变量seed的设置（采用两层随机种子设置，第一层种子负责生成一随机变量，
	//该随机变量的采样作为第二层生成器的种子，第二层生成器是最终用户使用的随机变量。
	//这样通过对第一层设置seed，可以影响到第二层的seed，第二层生成器之间使用第一层产生的不同的seed并且互不相关

	//产生seed的生成器类型（均匀分布）
    typedef ::boost::variate_generator< base_random_generator_type, boost::uniform_int<unsigned int> > SEED_GEN;
	
	//产生seed的生成器
    SEED_GEN m_SeedGen;
};

}
