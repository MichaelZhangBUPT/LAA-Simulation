#pragma once
#include "commondef.h"
#include <complex>
#include <vector>

namespace LTESim
{
	using std::vector;
	using std::complex;
	using std::cerr;

	typedef vector<vector<complex<double> > > Matrix;

	//矩阵加法
	Matrix			MatrixPlus(Matrix MatrixA,Matrix MatrixB);

	//矩阵减法
	Matrix			MatrixSubstract(Matrix MatrixA,Matrix MatrixB);

	//矩阵乘法
	Matrix			MatrixMultiply(Matrix MatrixA,Matrix MatrixB);

	//矩阵求逆
	Matrix			MatrixInverse(Matrix MatrixA);

	//矩阵转置
	Matrix			MatrixTranspose(Matrix MatrixA);

	//矩阵共轭
	Matrix			MatrixConjugate(Matrix MatrixA);

	//矩阵共轭转置
	Matrix			MatrixConjTranspose(Matrix MatrixA);

	//矩阵数乘
	Matrix			MatrixMultiplyNumber(Matrix MatrixA,complex<double> number);

	//打印矩阵
	void			PrintMatrix(const Matrix& MatrixA);

	//初始化一个确定大小的0矩阵
	Matrix			InialMatrix(int rownum, int columnnum);
	//获得一个单位矩阵
	Matrix			InialUnitMatrix(int dimension);
	vector<complex<double> >	LUP_Solve(Matrix MatrixA,vector<complex<double> > BVector);
}