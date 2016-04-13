#include "Matrix.h"

namespace LTESim
{
	Matrix MatrixPlus( Matrix MatrixA,Matrix MatrixB )
	{
		Matrix TempMatrix;
		Matrix::const_iterator iterA1 = MatrixA.begin();
		Matrix::const_iterator iterB1 = MatrixB.begin();

		for(;iterA1 != MatrixA.end();++iterA1,++iterB1)
		{
			vector<complex<double> > tempResult;
			tempResult.clear();
			for(int inum = 0;inum != iterA1->size();++inum)
			{
				tempResult.push_back((*iterA1)[inum] + (*iterB1)[inum]);
			}
			TempMatrix.push_back(tempResult);
		}

		return TempMatrix;
	}

	Matrix MatrixSubstract( Matrix MatrixA,Matrix MatrixB )
	{
		Matrix TempMatrix;
		Matrix::const_iterator iterA1 = MatrixA.begin();
		Matrix::const_iterator iterB1 = MatrixB.begin();

		for(;iterA1 != MatrixA.end();++iterA1,++iterB1)
		{
			vector<complex<double> > tempResult;
			tempResult.clear();
			for(int inum = 0;inum != iterA1->size();++inum)
			{
				tempResult.push_back((*iterA1)[inum] - (*iterB1)[inum]);
			}
			TempMatrix.push_back(tempResult);
		}

		return TempMatrix;
	}

	Matrix MatrixMultiply( Matrix MatrixA,Matrix MatrixB )
	{
		Matrix TempMatrix;
		Matrix::const_iterator iterA1 = MatrixA.begin();
		Matrix::const_iterator iterB1 = MatrixB.begin();

		for(int row = 0;row != MatrixA.size();++row)
		{
			vector<complex<double> > tempresult;
			tempresult.clear();
			for(int column = 0;column != iterB1->size();++column)
			{
				complex<double> tempCom(0);
				tempresult.push_back(tempCom);
			}
			TempMatrix.push_back(tempresult);
		}

		for(int row = 0;row != MatrixA.size();++row)
		{
			for(int columnB = 0;columnB != iterB1->size();++columnB)
			{
				for(int columnA = 0;columnA != iterA1->size();++columnA)
				{
					TempMatrix[row][columnB] += MatrixA[row][columnA] * MatrixB[columnA][columnB];
				}
			}
		}

		return TempMatrix;
	}

	Matrix MatrixTranspose( Matrix MatrixA )
	{
		Matrix MatrixTransposeResult;
		Matrix::const_iterator iter = MatrixA.begin();
		vector<complex<double> > temp;

		for(int column = 0;column != iter->size();++column)
		{
			temp.clear();
			for(int row = 0;row != MatrixA.size();++row)
			{
				temp.push_back(MatrixA[row][column]);
			}
			MatrixTransposeResult.push_back(temp);
		}

		return MatrixTransposeResult;
	}

	Matrix MatrixConjugate( Matrix MatrixA )
	{
		Matrix MatrixConjugateResult;
		Matrix::const_iterator iter = MatrixA.begin();
		vector<complex<double> > temp;

		for(int row = 0;row != MatrixA.size();++row)
		{
			temp.clear();
			for(int column = 0;column != iter->size();++column)
			{
				temp.push_back(conj(MatrixA[row][column]));
			}
			MatrixConjugateResult.push_back(temp);
		}
		return MatrixConjugateResult;
	}

	Matrix MatrixConjTranspose( Matrix MatrixA )
	{
		Matrix ConjMatrix = MatrixConjugate(MatrixA);
		Matrix ConjTransMatrix = MatrixTranspose(ConjMatrix);
		return ConjTransMatrix;
	}

	vector<complex<double> > LUP_Solve( Matrix MatrixA,vector<complex<double> > BVector )
	{
		vector<complex<double> > TempVectorX;
		vector<complex<double> > TempVectorY;
		vector<int> Permutation;
		Matrix MatrixL;
		Matrix MatrixU;
		vector<complex<double> > tempresultL;
		vector<complex<double> > tempresultU;
		int kExchange = 0;

		for(int row = 0;row != MatrixA.size();++row)
		{
			complex<double> tempCom0(0);
			complex<double> tempCom1(1);
			TempVectorX.push_back(tempCom0);
			TempVectorY.push_back(tempCom0);
			Permutation.push_back(row);
			tempresultL.clear();
			tempresultU.clear();
			for(int column = 0;column != MatrixA.size();++column)
			{
				tempresultU.push_back(tempCom0);
				if(column == row)
				{
					tempresultL.push_back(tempCom1);
				}
				else
				{
					tempresultL.push_back(tempCom0);
				}
			}
			MatrixL.push_back(tempresultL);
			MatrixU.push_back(tempresultU);
		}

		for(int k = 0;k != MatrixA.size();++k)
		{
			complex<double> p(0);
			double AbsMatrixAEle = 0;
			double AbsP =0;

			for(int i = k;i != MatrixA.size();++i)
			{
				complex<double> temp(MatrixA[i][k]);
				AbsMatrixAEle = sqrt(pow(temp.real(),2.0) + pow(temp.imag(),2.0));
				AbsP = sqrt(pow(p.real(),2.0) + pow(p.imag(),2.0));
				if(AbsMatrixAEle > AbsP)
				{
					p = MatrixA[i][k];
					AbsP = AbsMatrixAEle;
					kExchange = i;
				}
			}

			if(AbsP < 0.00001)
			{
				//cerr<<"singular matrix"<<endl;
			}

			int tempint = Permutation[k];
			Permutation[k] = Permutation[kExchange];
			Permutation[kExchange] = tempint;

			for(int column = 0;column != MatrixA.size();++column)
			{
				double RealPart = (MatrixA[k][column]).real();
				double ImagPart = (MatrixA[k][column]).imag();
				complex<double> tempcom(RealPart,ImagPart);
				MatrixA[k][column] = MatrixA[kExchange][column];
				MatrixA[kExchange][column] = tempcom;
			}

			for(int row = k + 1;row != MatrixA.size();++row)
			{
				MatrixA[row][k] = MatrixA[row][k] / MatrixA[k][k];
				for(int column = k + 1;column != MatrixA.size();++column)
				{
					MatrixA[row][column] -= MatrixA[row][k] * MatrixA[k][column];
				}
			}
		}

		for(int i = 0;i != MatrixA.size();++i)
		{
			for(int j = 0;j != MatrixA.size();++j)
			{
				if(i > j)
				{
					MatrixL[i][j] = MatrixA[i][j];
				}
				else
				{
					MatrixU[i][j] = MatrixA[i][j];
				}
			}
		}

		for(int i = 0;i != MatrixL.size();++i)
		{
			complex<double> temp(0);
			if(i != 0)
			{
				for(int j = 0;j <= i - 1;++j)
				{
					temp += MatrixL[i][j] * TempVectorY[j];
				}
			}
			TempVectorY[i] = BVector[Permutation[i]] - temp;
		}

		for(int i = (int)(MatrixA.size()) - 1;i >= 0;--i)
		{
			complex<double> temp(0);
			if(i != MatrixA.size() - 1)
			{
				for(int j = i + 1;j < (int)(MatrixA.size());++j)
				{
					temp += MatrixU[i][j] * TempVectorX[j];
				}
			}
			TempVectorX[i] = (TempVectorY[i] - temp) / MatrixU[i][i];
		}
		return TempVectorX;
	}

	Matrix MatrixInverse( Matrix MatrixA )
	{
		Matrix InverseMatrix;
		Matrix InverseMatrixResult;
		vector<complex<double> > Bvector;
		complex<double> tempcom0(0);
		complex<double> tempcom1(1);

		for(int inum = 0;inum != MatrixA.size();++inum)
		{
			Bvector.clear();
			for(int i = 0;i != MatrixA.size();++i)
			{
				if(i == inum)
				{
					Bvector.push_back(tempcom1);
				}
				else
				{
					Bvector.push_back(tempcom0);
				}
			}
			InverseMatrix.push_back(LUP_Solve(MatrixA,Bvector));
		}
		InverseMatrixResult = MatrixTranspose(InverseMatrix);
		return InverseMatrixResult;
	}

	void PrintMatrix( const Matrix& MatrixA )
	{
		Matrix::const_iterator iter = MatrixA.begin();

		for(int row = 0;row != MatrixA.size();++row)
		{
			for(int column = 0;column != iter->size();++column)
			{
				cout<<MatrixA[row][column]<<"	";
			}
			cout<<endl;
		}
	}

	Matrix MatrixMultiplyNumber( Matrix MatrixA,complex<double> number )
	{
		Matrix MatrixMultiplyNumberResult;
		Matrix::const_iterator iter = MatrixA.begin();
		vector<complex<double> > temp;

		for(int row = 0;row != MatrixA.size();++row)
		{
			temp.clear();
			for(int column = 0;column != iter->size();++column)
			{
				temp.push_back(MatrixA[row][column] * number);
			}
			MatrixMultiplyNumberResult.push_back(temp);
		}
		return MatrixMultiplyNumberResult;
	}

	Matrix InialMatrix( int rownum, int columnnum )
	{
		Matrix inialMatrix;
		vector<complex<double> > temp;
		complex<double> tempcom0(0);

		for(int row = 0;row < rownum;++row)
		{
			temp.clear();
			for(int column = 0;column < columnnum;++column)
			{
				temp.push_back(tempcom0);
			}
			inialMatrix.push_back(temp);
		}

		return inialMatrix;
	}

	Matrix InialUnitMatrix( int dimension )
	{
		Matrix UnitMatrix = InialMatrix( dimension, dimension );
		for (int index = 0; index < dimension; ++index )
		{
			UnitMatrix[index][index] = 1.0;
		}
		return UnitMatrix;
	}

}
