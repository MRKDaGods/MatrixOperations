#include <string>
#include <iostream>

struct matrix 
{
	unsigned int orderx;
	unsigned int ordery;
	float **vals;
};

matrix *allocs;
matrix identity;
unsigned int allocSize;
float **cachedResult = 0;
//float (*cachedResult)[4];

void ProcessCmd(_STD string cmd);
void ZeroMatrix(matrix &matrix);
void SetMatrixChild(matrix &matrix, unsigned int x, unsigned int y, float value);

int main()
{
	_STD cout << "Matrix operations\n";

	identity = matrix{
		2,
		2,
		0
	};
	ZeroMatrix(identity);
	SetMatrixChild(identity, 0, 0, 1.f);
	SetMatrixChild(identity, 1, 1, 1.f);

	while (true)
	{
		_STD string cmd;
		_STD getline(_STD cin, cmd);
		ProcessCmd(cmd);
	}
	return 0;
}

template<typename T>
void ReAlloc(T **arr, unsigned int sz, unsigned int newSz)
{
	//static_assert(newSz > sz && arr);
	T *newAlloc = new T[newSz];
	if (*arr && *arr != (void*)0xcdcdcdcd) //debug
	{
		memcpy(newAlloc, *arr, sizeof(T) * sz);
		//delete[] * arr;
	}
	*arr = newAlloc;
}

void SplitStr(_STD string str, char delim, _STD string **arr, unsigned int *sz)
{
	if (!arr)
		return;
	_STD string c;
	_STD string *ar = 0;
	unsigned int arSz = 0;

	for (char &ch : str)
	{
		if (ch == delim && c.size() > 0)
		{
			ReAlloc<_STD string>(&ar, arSz, arSz + 1);
			ar[arSz] = c;
			arSz++;
			c = "";
		}
		else
			c += ch;
	}
	if (c.size() > 0)
	{
		ReAlloc<_STD string>(&ar, arSz, arSz + 1);
		ar[arSz] = c;
		arSz++;
	}
	*arr = ar;
	*sz = arSz;
}

void ZeroMatrix(matrix &matrix)
{
	ReAlloc<float*>(&matrix.vals, matrix.orderx, matrix.orderx);
	for (unsigned int i = 0; i < matrix.orderx; i++)
	{
		ReAlloc<float>(&matrix.vals[i], matrix.ordery, matrix.ordery);
		for (unsigned int j = 0; j < matrix.ordery; j++)
			matrix.vals[i][j] = 0.f;
	}
}

void SetMatrixChild(matrix &matrix, unsigned int x, unsigned int y, float value)
{
	matrix.vals[x][y] = value;
}

unsigned int AllocateMatrix(matrix &m)
{
	ReAlloc<matrix>(&allocs, allocSize, allocSize + 1);
	allocs[allocSize] = m;
	allocSize++;
	return allocSize - 1;
}


float CalculateDeterminant(matrix &m)
{
	if (m.orderx != m.ordery) //square-check
		return 0.f;
	//2x2
	if (m.orderx == 2 && m.ordery == 2)
		return m.vals[0][0] * m.vals[1][1] - m.vals[0][1] * m.vals[1][0]; //ad - bc
	if (m.orderx == 3 && m.ordery == 3)
		return m.vals[0][0] * (m.vals[1][1] * m.vals[2][2] - m.vals[1][2] * m.vals[2][1]) -
		m.vals[0][1] * (m.vals[1][0] * m.vals[2][2] - m.vals[1][2] * m.vals[2][0]) +
		m.vals[0][2] * (m.vals[1][0] * m.vals[2][1] - m.vals[1][1] * m.vals[2][0]);
	if (m.orderx == 4 && m.ordery == 4)
	{
		float target[4][4];
		float tempArr[12];
		float piv[16];
		for (unsigned int i = 0; i < 4; i++)
		{
			piv[i + 0] = m.vals[i][0];
			piv[i + 4] = m.vals[i][1];
			piv[i + 8] = m.vals[i][2];
			piv[i + 12] = m.vals[i][3];
		}
		tempArr[0] = piv[10] * piv[15];
		tempArr[1] = piv[11] * piv[14];
		tempArr[2] = piv[9] * piv[15];
		tempArr[3] = piv[11] * piv[13];
		tempArr[4] = piv[9] * piv[14];
		tempArr[5] = piv[10] * piv[13];
		tempArr[6] = piv[8] * piv[15];
		tempArr[7] = piv[11] * piv[12];
		tempArr[8] = piv[8] * piv[14];
		tempArr[9] = piv[10] * piv[12];
		tempArr[10] = piv[8] * piv[13];
		tempArr[11] = piv[9] * piv[12];
		target[0][0] = tempArr[0] * piv[5] + tempArr[3] * piv[6] + tempArr[4] * piv[7];
		target[0][0] -= tempArr[1] * piv[5] + tempArr[2] * piv[6] + tempArr[5] * piv[7];
		target[0][1] = tempArr[1] * piv[4] + tempArr[6] * piv[6] + tempArr[9] * piv[7];
		target[0][1] -= tempArr[0] * piv[4] + tempArr[7] * piv[6] + tempArr[8] * piv[7];
		target[0][2] = tempArr[2] * piv[4] + tempArr[7] * piv[5] + tempArr[10] * piv[7];
		target[0][2] -= tempArr[3] * piv[4] + tempArr[6] * piv[5] + tempArr[11] * piv[7];
		target[0][3] = tempArr[5] * piv[4] + tempArr[8] * piv[5] + tempArr[11] * piv[6];
		target[0][3] -= tempArr[4] * piv[4] + tempArr[9] * piv[5] + tempArr[10] * piv[6];
		target[1][0] = tempArr[1] * piv[1] + tempArr[2] * piv[2] + tempArr[5] * piv[3];
		target[1][0] -= tempArr[0] * piv[1] + tempArr[3] * piv[2] + tempArr[4] * piv[3];
		target[1][1] = tempArr[0] * piv[0] + tempArr[7] * piv[2] + tempArr[8] * piv[3];
		target[1][1] -= tempArr[1] * piv[0] + tempArr[6] * piv[2] + tempArr[9] * piv[3];
		target[1][2] = tempArr[3] * piv[0] + tempArr[6] * piv[1] + tempArr[11] * piv[3];
		target[1][2] -= tempArr[2] * piv[0] + tempArr[7] * piv[1] + tempArr[10] * piv[3];
		target[1][3] = tempArr[4] * piv[0] + tempArr[9] * piv[1] + tempArr[10] * piv[2];
		target[1][3] -= tempArr[5] * piv[0] + tempArr[8] * piv[1] + tempArr[11] * piv[2];
		tempArr[0] = piv[2] * piv[7];
		tempArr[1] = piv[3] * piv[6];
		tempArr[2] = piv[1] * piv[7];
		tempArr[3] = piv[3] * piv[5];
		tempArr[4] = piv[1] * piv[6];
		tempArr[5] = piv[2] * piv[5];
		tempArr[6] = piv[0] * piv[7];
		tempArr[7] = piv[3] * piv[4];
		tempArr[8] = piv[0] * piv[6];
		tempArr[9] = piv[2] * piv[4];
		tempArr[10] = piv[0] * piv[5];
		tempArr[11] = piv[1] * piv[4];
		target[2][0] = tempArr[0] * piv[13] + tempArr[3] * piv[14] + tempArr[4] * piv[15];
		target[2][0] -= tempArr[1] * piv[13] + tempArr[2] * piv[14] + tempArr[5] * piv[15];
		target[2][1] = tempArr[1] * piv[12] + tempArr[6] * piv[14] + tempArr[9] * piv[15];
		target[2][1] -= tempArr[0] * piv[12] + tempArr[7] * piv[14] + tempArr[8] * piv[15];
		target[2][2] = tempArr[2] * piv[12] + tempArr[7] * piv[13] + tempArr[10] * piv[15];
		target[2][2] -= tempArr[3] * piv[12] + tempArr[6] * piv[13] + tempArr[11] * piv[15];
		target[2][3] = tempArr[5] * piv[12] + tempArr[8] * piv[13] + tempArr[11] * piv[14];
		target[2][3] -= tempArr[4] * piv[12] + tempArr[9] * piv[13] + tempArr[10] * piv[14];
		target[3][0] = tempArr[2] * piv[10] + tempArr[5] * piv[11] + tempArr[1] * piv[9];
		target[3][0] -= tempArr[4] * piv[11] + tempArr[0] * piv[9] + tempArr[3] * piv[10];
		target[3][1] = tempArr[8] * piv[11] + tempArr[0] * piv[8] + tempArr[7] * piv[10];
		target[3][1] -= tempArr[6] * piv[10] + tempArr[9] * piv[11] + tempArr[1] * piv[8];
		target[3][2] = tempArr[6] * piv[9] + tempArr[11] * piv[11] + tempArr[3] * piv[8];
		target[3][2] -= tempArr[10] * piv[11] + tempArr[2] * piv[8] + tempArr[7] * piv[9];
		target[3][3] = tempArr[10] * piv[10] + tempArr[4] * piv[8] + tempArr[9] * piv[9];
		target[3][3] -= tempArr[8] * piv[9] + tempArr[11] * piv[10] + tempArr[5] * piv[8];
		if (cachedResult)
			delete[] cachedResult;
		cachedResult = new float*[4];
		for (unsigned int i = 0; i < 4; i++)
		{
			cachedResult[i] = new float[4];
			for (unsigned int j = 0; j < 4; j++)
			{
				cachedResult[i][j] = target[i][j];
			}
		}
		return piv[0] * target[0][0] + piv[1] * target[0][1] + piv[2] * target[0][2] + piv[3] * target[0][3];
	}
	return 0.f;
}

matrix CalculateInverse(matrix &m, float det)
{
	if (m.orderx == 2 && m.ordery == 2)
	{
		float mult = 1.f / det;
		matrix inv = matrix{
			2,
			2,
			0
		};
		ZeroMatrix(inv);
		SetMatrixChild(inv, 0, 0, m.vals[1][1] * mult);
		SetMatrixChild(inv, 0, 1, -m.vals[0][1] * mult);
		SetMatrixChild(inv, 1, 0, -m.vals[1][0] * mult);
		SetMatrixChild(inv, 1, 1, m.vals[0][0] * mult);
		return inv;
	}

	if (m.orderx == 3 && m.ordery == 3)
	{
		matrix inv = matrix{
			3,
			3,
			0
		};
		ZeroMatrix(inv);
		float mult = 1.f / det;
		SetMatrixChild(inv, 0, 0, (m.vals[1][1] * m.vals[2][2] - m.vals[2][1] * m.vals[1][2]) * mult);
		SetMatrixChild(inv, 0, 1, (m.vals[0][2] * m.vals[2][1] - m.vals[0][1] * m.vals[2][2]) * mult);
		SetMatrixChild(inv, 0, 2, (m.vals[0][1] * m.vals[1][2] - m.vals[0][2] * m.vals[1][1]) * mult);
		SetMatrixChild(inv, 1, 0, (m.vals[1][2] * m.vals[2][0] - m.vals[1][0] * m.vals[2][2]) * mult);
		SetMatrixChild(inv, 1, 1, (m.vals[0][0] * m.vals[2][2] - m.vals[0][2] * m.vals[2][0]) * mult);
		SetMatrixChild(inv, 1, 2, (m.vals[1][0] * m.vals[0][2] - m.vals[0][0] * m.vals[1][2]) * mult);
		SetMatrixChild(inv, 2, 0, (m.vals[1][0] * m.vals[2][1] - m.vals[2][0] * m.vals[1][1]) * mult);
		SetMatrixChild(inv, 2, 1, (m.vals[2][0] * m.vals[0][1] - m.vals[0][0] * m.vals[2][1]) * mult);
		SetMatrixChild(inv, 2, 2, (m.vals[0][0] * m.vals[1][1] - m.vals[1][0] * m.vals[0][1]) * mult);
		return inv;
	}

	if (m.orderx == 4 && m.ordery == 4)
	{
		matrix inv = matrix{
			4,
			4,
			0
		};
		ZeroMatrix(inv);
		float mult = 1.f / det;
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				SetMatrixChild(inv, i, j, cachedResult[i][j] * mult);
			}
		}
		return inv;
	}
	return identity;
}

matrix MultiplyMatrices(matrix &m1, matrix &m2)
{
	if (m1.ordery != m2.orderx)
		return identity;
	matrix r = matrix{
		m1.orderx,
		m2.ordery,
		0
	};
	ZeroMatrix(r);
	for (unsigned int i = 0; i < r.orderx; i++)
	{
		for (unsigned j = 0; j < r.ordery; j++)
		{
			r.vals[i][j] = 0.f;
			for (unsigned int k = 0; k < m1.ordery; k++)
				r.vals[i][j] += m1.vals[i][k] * m2.vals[k][j];
		}
	}
	return r;
}

matrix AddMatrices(matrix &m1, matrix &m2)
{
	if (m1.orderx != m2.orderx || m1.ordery != m2.ordery)
		return identity;
	matrix r = matrix{
		m1.orderx,
		m1.ordery,
		0
	};
	ZeroMatrix(r);
	for (unsigned int i = 0; i < m1.orderx; i++)
		for (unsigned int j = 0; j < m1.ordery; j++)
			SetMatrixChild(r, i, j, m1.vals[i][j] + m2.vals[i][j]);
	return r;
}

matrix SubtractMatrices(matrix &m1, matrix &m2)
{
	if (m1.orderx != m2.orderx || m1.ordery != m2.ordery)
		return identity;
	matrix r = matrix{
		m1.orderx,
		m1.ordery,
		0
	};
	ZeroMatrix(r);
	for (unsigned int i = 0; i < m1.orderx; i++)
		for (unsigned int j = 0; j < m1.ordery; j++)
			SetMatrixChild(r, i, j, m1.vals[i][j] - m2.vals[i][j]);
	return r;
}

matrix MultiplyMatrix(matrix &m, float num)
{
	matrix r = matrix{
		m.orderx,
		m.ordery,
		0
	};
	ZeroMatrix(r);
	for (unsigned int i = 0; i < m.orderx; i++)
		for (unsigned int j = 0; j < m.ordery; j++)
			SetMatrixChild(r, i, j, m.vals[i][j] * num);
	return r;
}

void PrintMatrix(matrix &m)
{
	//demo 3x3 matrix
	//  (  1   0   3  )
	//  ( -1   0   5  )
	//  ( -1   0  20  )
	for (unsigned int x = 0; x < m.orderx; x++)
	{
		_STD cout << "  ( ";
		for (unsigned int y = 0; y < m.ordery; y++)
		{
			_STD string v = _STD to_string(m.vals[x][y]);
			if (v.size() > 6)
				v = v.substr(0, 6);
			for (unsigned int s = 0; s < 6 - v.size(); s++)
				_STD cout << ' ';
			_STD cout << v << ' ';
		}
		_STD cout << ")\n";
	}
}

void ProcessCmd(_STD string cmd)
{
	_STD string *arr;
	unsigned int sz;
	SplitStr(cmd, ' ', &arr, &sz);
	if (sz > 0)
	{
		for (unsigned int i = 0; i < sz; i++)
		{
			_STD string rawcmd = arr[i];

			if (rawcmd == "add" || rawcmd == "sub")
			{
				if (sz > 1)
				{
					matrix mfinal = identity;
					for (unsigned int j = 0; j < sz - 1; j++)
					{
						_STD string curr = arr[i + 1 + j];
						bool inverse = false;
						if (curr.find('-') == 0)
							inverse = true;
						unsigned int ref = _STD stoi(inverse ? curr.substr(1) : curr);
						if (ref >= allocSize)
						{
							_STD cout << "Matrix " << ref << "doesn't exist\n";
							return;
						}
						matrix referenced = allocs[ref];
						matrix m = inverse ? CalculateInverse(referenced, CalculateDeterminant(referenced)) : referenced;
						matrix (*processor)(matrix&, matrix&) = rawcmd == "add" ? AddMatrices : SubtractMatrices;
						if (j == 0)
							mfinal = m;
						else mfinal = processor(mfinal, m);
					}
					PrintMatrix(mfinal);
				}
			}

			if (rawcmd == "mult")
			{
				if (sz > 1)
				{
					matrix mfinal = identity;
					//RULE: MATRIX nX
					for (unsigned int j = 0; j < sz - 1; j++)
					{
						_STD string curr = arr[i + 1 + j];
						bool inverse = false;
						if (curr.find('n') == 0)
						{
							//num
							float num = _STD stof(curr.substr(1));
							mfinal = MultiplyMatrix(mfinal, num);
							continue;
						}
						if (curr.find('-') == 0)
							inverse = true;
						unsigned int ref = _STD stoi(inverse ? curr.substr(1) : curr);
						if (ref >= allocSize)
						{
							_STD cout << "Matrix " << ref << "doesn't exist\n";
							return;
						}
						matrix referenced = allocs[ref];
						matrix m = inverse ? CalculateInverse(referenced, CalculateDeterminant(referenced)) : referenced;
						if (j == 0)
							mfinal = m;
						else mfinal = MultiplyMatrices(mfinal, m);
					}
					PrintMatrix(mfinal);
				}
			}

			if (rawcmd == "det")
			{
				if (sz == 2)
				{
					unsigned int ref = _STD stoi(arr[i + 1]);
					if (ref >= allocSize)
					{
						_STD cout << "Matrix doesn't exist\n";
						return;
					}
					float det = CalculateDeterminant(allocs[ref]);
					_STD cout << "Determinant: " << det << '\n';
				}
			}

			if (rawcmd == "inverse")
			{
				if (sz == 2)
				{
					unsigned int ref = _STD stoi(arr[i + 1]);
					if (ref >= allocSize)
					{
						_STD cout << "Matrix doesn't exist\n";
						return;
					}
					matrix target = allocs[ref];
					float det = CalculateDeterminant(target);
					if (det == 0.f)
					{
						_STD cout << "Can not determine the inverse of a singular matrix\n";
						return;
					}
					matrix inv = CalculateInverse(target, det);
					PrintMatrix(inv);
				}
			}

			if (rawcmd == "alloc")
			{
				//alloc 2x2( 2 0 0 0 )
				if (sz > 1)
				{
					_STD string px2 = arr[i + 1];
					unsigned int oo = 0;
					_STD string xx;
					_STD string yx;
					bool _xx = false;
					while (oo < px2.size())
					{
						char ch = px2[oo];
						if (isdigit(ch))
						{
							if (_xx)
								yx += ch;
							else xx += ch;
						}
						else 
						{
							if (_xx)
								break;
							_xx = true;
						}
						oo++;
					}
					if (px2[oo] != '(')
						break;
					unsigned int realxx = _STD stoi(xx);
					unsigned int realyy = _STD stoi(yx);
					unsigned int neededCells = realxx * realyy;

					matrix m = matrix{
						realxx,
						realyy,
						0
					};

					ZeroMatrix(m);

					if (sz - 1 < neededCells)
						_STD cout << "Invalid alloc, N:" << neededCells << ", S:" << sz - 1 << '\n';
					else
					{
						unsigned int rowX = 0;
						unsigned int colX = 0;
						for (unsigned int j = 0; j < neededCells; j++)
						{
							_STD string currentDigit = arr[i + 2 + j]; //i + offset + local
							/*if (currentDigit.size() > 1 || !isdigit(currentDigit[0]))
							{
								_STD cout << "Invalid alloc\n";
								break;
							}*/
							SetMatrixChild(m, rowX, colX, _STD stof(currentDigit));
							colX++;
							if (colX == realyy)
							{
								colX = 0;
								rowX++;
							}
						}
						_STD cout << "Integrity: " << (arr[i + 2 + neededCells][0] == ')') << '\n';
					}

					_STD cout << "Allocated matrix[" << m.orderx << "x" << m.ordery << "] [" << AllocateMatrix(m) << "]\n";
					PrintMatrix(m);
					/*for (unsigned int x = 0; x < m.orderx; x++)
					{
						for (unsigned int y = 0; y < m.ordery; y++)
						{
							_STD cout << "m" << x << y << "=" << m.vals[x][y] << '\n';
						}
					}*/
				}
			}
		}
	}
}