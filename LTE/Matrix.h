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

	//����ӷ�
	Matrix			MatrixPlus(Matrix MatrixA,Matrix MatrixB);

	//�������
	Matrix			MatrixSubstract(Matrix MatrixA,Matrix MatrixB);

	//����˷�
	Matrix			MatrixMultiply(Matrix MatrixA,Matrix MatrixB);

	//��������
	Matrix			MatrixInverse(Matrix MatrixA);

	//����ת��
	Matrix			MatrixTranspose(Matrix MatrixA);

	//������
	Matrix			MatrixConjugate(Matrix MatrixA);

	//������ת��
	Matrix			MatrixConjTranspose(Matrix MatrixA);

	//��������
	Matrix			MatrixMultiplyNumber(Matrix MatrixA,complex<double> number);

	//��ӡ����
	void			PrintMatrix(const Matrix& MatrixA);

	//��ʼ��һ��ȷ����С��0����
	Matrix			InialMatrix(int rownum, int columnnum);
	//���һ����λ����
	Matrix			InialUnitMatrix(int dimension);
	vector<complex<double> >	LUP_Solve(Matrix MatrixA,vector<complex<double> > BVector);
}