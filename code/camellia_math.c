#include "camellia_math.h"

// Credit to: https://github.com/HandmadeMath/Handmade-Math

#define _USE_MATH_DEFINES
#include <time.h>
#include <stdlib.h>
#include <math.h>

#ifdef CAMELLIA_SSE
internal
__m128 LinearCombineSSE(__m128 Left, m4 Right)
{
	__m128 Result;

	Result = _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0x00), Right.Columns[0]);
	Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0x55), Right.Columns[1]));
	Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0xaa), Right.Columns[2]));
	Result = _mm_add_ps(Result, _mm_mul_ps(_mm_shuffle_ps(Left, Left, 0xff), Right.Columns[3]));

	return(Result);
}
#endif

v2 V2New(f32 X, f32 Y)
{
	v2 Result;
	Result.X = X;
	Result.Y = Y;
	return(Result);
}

v2 V2Scalar(f32 Scalar)
{
	v2 Result;
	Result.X = Scalar;
	Result.Y = Scalar;
	return(Result);
}

v2 V2Zero()
{
	return(V2Scalar(0.0f));
}

v2 V2Add(v2 Left, v2 Right)
{
	return(V2New(Left.X + Right.X, Left.Y + Right.Y));
}

v2 V2Addf(v2 Left, f32 Right)
{
	return(V2New(Left.X + Right, Left.Y + Right));
}

v2 V2Sub(v2 Left, v2 Right)
{
	return(V2New(Left.X + Right.X, Left.Y + Right.Y));
}

v2 V2Subf(v2 Left, f32 Right)
{
	return(V2New(Left.X - Right, Left.Y - Right));
}

v2 V2Mul(v2 Left, v2 Right)
{
	return(V2New(Left.X * Right.X, Left.Y * Right.Y));
}

v2 V2Mulf(v2 Left, f32 Right)
{
	return(V2New(Left.X * Right, Left.Y * Right));
}

v2 V2Div(v2 Left, v2 Right)
{
	return(V2New(Left.X / Right.X, Left.Y / Right.Y));
}

v2 V2Divf(v2 Left, f32 Right)
{
	return(V2New(Left.X / Right, Left.Y / Right));
}

f32 V2Dot(v2 Left, v2 Right)
{
	return((Left.X * Right.X) + (Left.Y * Right.Y));
}

f32 V2Length(v2 Vector)
{
	return(sqrtf(V2Dot(Vector, Vector)));
}

v2 V2Normalize(v2 Vector)
{
	v2 Result = V2Zero();

	f32 Length = V2Length(Vector);
	if (Length != 0.0f)
	{
		Result.X = Vector.X * (1.0f / Length);
		Result.Y = Vector.Y * (1.0f / Length);
	}

	return(Result);
}

v3 V3New(f32 X, f32 Y, f32 Z)
{
	v3 Result;
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	return(Result);
}

v3 V3Scalar(f32 Scalar)
{
	return(V3New(Scalar, Scalar, Scalar));
}

v3 V3Zero()
{
	return V3Scalar(0.0f);
}

v3 V3Add(v3 Left, v3 Right)
{
	v3 Result;
	Result.X = Left.X + Right.X;
	Result.Y = Left.Y + Right.Y;
	Result.Z = Left.Z + Right.Z;
	return(Result);
}

v3 V3Addf(v3 Left, f32 Right)
{
	v3 Result;
	Result.X = Left.X + Right;
	Result.Y = Left.Y + Right;
	Result.Z = Left.Z + Right;
	return(Result);
}

v3 V3Sub(v3 Left, v3 Right)
{
	v3 Result;
	Result.X = Left.X - Right.X;
	Result.Y = Left.Y - Right.Y;
	Result.Z = Left.Z - Right.Z;
	return(Result);
}

v3 V3Subf(v3 Left, f32 Right)
{
	v3 Result;
	Result.X = Left.X - Right;
	Result.Y = Left.Y - Right;
	Result.Z = Left.Z - Right;
	return(Result);
}

v3 V3Mul(v3 Left, v3 Right)
{
	v3 Result;
	Result.X = Left.X * Right.X;
	Result.Y = Left.Y * Right.Y;
	Result.Z = Left.Z * Right.Z;
	return(Result);
}

v3 V3Mulf(v3 Left, f32 Right)
{
	v3 Result;
	Result.X = Left.X * Right;
	Result.Y = Left.Y * Right;
	Result.Z = Left.Z * Right;
	return(Result);
}

v3 V3Div(v3 Left, v3 Right)
{
	v3 Result;
	Result.X = Left.X / Right.X;
	Result.Y = Left.Y / Right.Y;
	Result.Z = Left.Z / Right.Z;
	return(Result);
}

v3 V3Divf(v3 Left, f32 Right)
{
	v3 Result;
	Result.X = Left.X / Right;
	Result.Y = Left.Y / Right;
	Result.Z = Left.Z / Right;
	return(Result);
}

f32 V3Dot(v3 Left, v3 Right)
{
	return(Left.X * Right.X + Left.Y * Right.Y + Left.Z * Right.Z);
}

v3 V3Cross(v3 Left, v3 Right)
{
	v3 Result;
	Result.X = (Left.Y * Right.Z) - (Left.Z * Right.Y);
	Result.Y = (Left.Z * Right.X) - (Left.X * Right.Z);
	Result.Z = (Left.X * Right.Y) - (Left.Y * Right.X);
	return(Result);
}

f32 V3Length(v3 Vector)
{
	return(sqrtf(V3Dot(Vector, Vector)));
}

v3 V3Normalize(v3 Vector)
{
	v3 Result = V3Zero();

	f32 Length = V3Length(Vector);
	if (Length != 0.0f)
	{
		Result.X = Vector.X * (1.0f / Length);
		Result.Y = Vector.Y * (1.0f / Length);
		Result.Z = Vector.Z * (1.0f / Length);
	}

	return(Result);
}

v4 V4New(f32 X, f32 Y, f32 Z, f32 W)
{
	v4 Result;
	
#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_setr_ps(X, Y, Z, W);
#else
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	Result.W = W;
#endif

	return(Result);
}

v4 V4Scalar(f32 Scalar)
{
	return(V4New(Scalar, Scalar, Scalar, Scalar));
}

v4 V4Zero()
{
	return(V4Scalar(0.0f));
}

v4 V4Add(v4 Left, v4 Right)
{
	v4 Result;

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_add_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else
	Result.X = Left.X + Right.X;
	Result.Y = Left.Y + Right.Y;
	Result.Z = Left.Z + Right.Z;
#endif

	return(Result);
}

v4 V4Addf(v4 Left, f32 Right)
{
	v4 Result;

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_add_ps(Left.InternalElementsSSE, _mm_setr_ps(Right, Right, Right, Right));
#else
	Result.X = Left.X + Right;
	Result.Y = Left.Y + Right;
	Result.Z = Left.Z + Right;
#endif

	return(Result);
}

v4 V4Sub(v4 Left, v4 Right)
{
	v4 Result;

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_sub_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else
	Result.X = Left.X - Right.X;
	Result.Y = Left.Y - Right.Y;
	Result.Z = Left.Z - Right.Z;
#endif

	return(Result);
}

v4 V4Subf(v4 Left, f32 Right)
{
	v4 Result;

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_sub_ps(Left.InternalElementsSSE, _mm_setr_ps(Right, Right, Right, Right));
#else
	Result.X = Left.X - Right;
	Result.Y = Left.Y - Right;
	Result.Z = Left.Z - Right;
#endif

	return(Result);
}

v4 V4Mul(v4 Left, v4 Right)
{
	v4 Result;

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_mul_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else                
	Result.X = Left.X * Right.X;
	Result.Y = Left.Y * Right.Y;
	Result.Z = Left.Z * Right.Z;
#endif

	return(Result);
}

v4 V4Mulf(v4 Left, f32 Right)
{
	v4 Result;

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_mul_ps(Left.InternalElementsSSE, _mm_setr_ps(Right, Right, Right, Right));
#else                
	Result.X = Left.X * Right;
	Result.Y = Left.Y * Right;
	Result.Z = Left.Z * Right;
#endif

	return(Result);
}

v4 V4Div(v4 Left, v4 Right)
{
	v4 Result;

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_div_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
#else                
	Result.X = Left.X / Right.X;
	Result.Y = Left.Y / Right.Y;
	Result.Z = Left.Z / Right.Z;
#endif

	return(Result);
}
	
v4 V4Divf(v4 Left, f32 Right)
{
	v4 Result;

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = _mm_div_ps(Left.InternalElementsSSE, _mm_setr_ps(Right, Right, Right, Right));
#else                
	Result.X = Left.X / Right;
	Result.Y = Left.Y / Right;
	Result.Z = Left.Z / Right;
#endif

	return(Result);
}

f32 V4Dot(v4 Left, v4 Right)
{
#ifdef CAMELLIA_SSE
	f32 Result = 0.0f;
	__m128 SSEResultOne = _mm_mul_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
	__m128 SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(2, 3, 0, 1));
	SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
	SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(0, 1, 2, 3));
	SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
	_mm_store_ss(&Result, SSEResultOne);
	return(Result);
#else
	return(Left.X * Right.X + Left.Y * Right.Y + Left.Z * Right.Z + Left.W * Right.W);
#endif
}

f32 V4Length(v4 Vector)
{
	return(sqrtf(V4Dot(Vector, Vector)));
}

v4 V4Normalize(v4 Vector)
{
	v4 Result = V4Zero();

	f32 Length = V4Length(Vector);
	if (Length != 0.0f)
	{
		f32 Multiplier = 1.0f / Length;

#ifdef CAMELLIA_SSE
		__m128 SSEMultiplier = _mm_set1_ps(Multiplier);
		Result.InternalElementsSSE = _mm_mul_ps(Vector.InternalElementsSSE, SSEMultiplier);
#else
		Result.X = Vector.X * Multiplier;
		Result.Y = Vector.Y * Multiplier;
		Result.Z = Vector.Z * Multiplier;
		Result.W = Vector.W * Multiplier;
#endif
	}

	return(Result);
}

m4 M4Zero()
{
	m4 Result;
	memset(&Result, 0, sizeof(m4));
	return(Result);
}

m4 M4d(f32 Diagonal)
{
	m4 Result = M4Zero();

	Result.Elements[0][0] = Diagonal;
	Result.Elements[1][1] = Diagonal;
	Result.Elements[2][2] = Diagonal;
	Result.Elements[3][3] = Diagonal;

	return(Result);
}

m4 M4Identity()
{
	return M4d(1.0f);
}

m4 M4Transpose(m4 Matrix)
{
	m4 Result = M4Identity();

#ifdef CAMELLIA_SSE
	_MM_TRANSPOSE4_PS(Result.Columns[0], Result.Columns[1], Result.Columns[2], Result.Columns[3]);
#else
	i32 Columns;
	for(Columns = 0; Columns < 4; ++Columns)
	{
		i32 Rows;
		for(Rows = 0; Rows < 4; ++Rows)
		{
			Result.Elements[Rows][Columns] = Matrix.Elements[Columns][Rows];
		}
	}
#endif

	return(Result);
}

m4 M4Add(m4 Left, m4 Right)
{
	m4 Result = M4Identity();

#ifdef CAMELLIA_SSE
	Result.Columns[0] = _mm_add_ps(Left.Columns[0], Right.Columns[0]);
	Result.Columns[1] = _mm_add_ps(Left.Columns[1], Right.Columns[1]);
	Result.Columns[2] = _mm_add_ps(Left.Columns[2], Right.Columns[2]);
	Result.Columns[3] = _mm_add_ps(Left.Columns[3], Right.Columns[3]);
#else
	i32 Columns;
	for(Columns = 0; Columns < 4; ++Columns)
	{
		i32 Rows;
		for(Rows = 0; Rows < 4; ++Rows)
		{
			Result.Elements[Columns][Rows] = Left.Elements[Columns][Rows] + Right.Elements[Columns][Rows];
		}
	}
#endif
	
	return(Result);
}

m4 M4Sub(m4 Left, m4 Right)
{
	m4 Result = M4Identity();

#ifdef CAMELLIA_SSE
	Result.Columns[0] = _mm_sub_ps(Left.Columns[0], Right.Columns[0]);
	Result.Columns[1] = _mm_sub_ps(Left.Columns[1], Right.Columns[1]);
	Result.Columns[2] = _mm_sub_ps(Left.Columns[2], Right.Columns[2]);
	Result.Columns[3] = _mm_sub_ps(Left.Columns[3], Right.Columns[3]);
#else
	i32 Columns;
	for(Columns = 0; Columns < 4; ++Columns)
	{
		i32 Rows;
		for(Rows = 0; Rows < 4; ++Rows)
		{
			Result.Elements[Columns][Rows] = Left.Elements[Columns][Rows] - Right.Elements[Columns][Rows];
		}
	}
#endif
	
	return(Result);
}

m4 M4Mul(m4 Left, m4 Right)
{
	m4 Result = M4Identity();

#ifdef CAMELLIA_SSE
	Result.Columns[0] = LinearCombineSSE(Left.Columns[0], Right);
	Result.Columns[1] = LinearCombineSSE(Left.Columns[1], Right);
	Result.Columns[2] = LinearCombineSSE(Left.Columns[2], Right);
	Result.Columns[3] = LinearCombineSSE(Left.Columns[3], Right);
#else 
	i32 Columns;
	for(Columns = 0; Columns < 4; ++Columns)
	{
		i32 Rows;
		for(Rows = 0; Rows < 4; ++Rows)
		{
			f32 Sum = 0;
			i32 CurrentMatrice;
			for(CurrentMatrice = 0; CurrentMatrice < 4; ++CurrentMatrice)
			{
				Sum += Left.Elements[CurrentMatrice][Rows] * Right.Elements[Columns][CurrentMatrice];
			}

			Result.Elements[Columns][Rows] = Sum;
		}
	}
#endif

	return(Result);
}

m4 M4Mulf(m4 Left, f32 Right)
{
	m4 Result = M4Identity();

#ifdef CAMELLIA_SSE
	__m128 ScalarSSE = _mm_setr_ps(Right, Right, Right, Right);
	Result.Columns[0] = _mm_mul_ps(Left.Columns[0], ScalarSSE);
	Result.Columns[1] = _mm_mul_ps(Left.Columns[1], ScalarSSE);
	Result.Columns[2] = _mm_mul_ps(Left.Columns[2], ScalarSSE);
	Result.Columns[3] = _mm_mul_ps(Left.Columns[3], ScalarSSE);
#else 
	i32 Columns;
	for(Columns = 0; Columns < 4; ++Columns)
	{
		i32 Rows;
		for(Rows = 0; Rows < 4; ++Rows)
		{
			f32 Sum = 0;
			i32 CurrentMatrice;
			for(CurrentMatrice = 0; CurrentMatrice < 4; ++CurrentMatrice)
			{
				Sum += Left.Elements[CurrentMatrice][Rows] * Right;
			}

			Result.Elements[Columns][Rows] = Sum;
		}
	}
#endif

	return(Result);
}

v4 M4MulV4(m4 Left, v4 Right)
{
	v4 Result = V4Zero();

#ifdef CAMELLIA_SSE
	Result.InternalElementsSSE = LinearCombineSSE(Right.InternalElementsSSE, Left);
#else
	i32 Columns, Rows;
	for(Rows = 0; Rows < 4; ++Rows)
	{
		f32 Sum = 0;
		for(Columns = 0; Columns < 4; ++Columns)
		{
			Sum += Matrix.Elements[Columns][Rows] * Vector.Elements[Columns];
		}

		Result.Elements[Rows] = Sum;
	}
#endif

	return(Result);
}

m4 M4Divf(m4 Left, f32 Right)
{
	m4 Result = M4Identity();

#ifdef CAMELLIA_SSE
	__m128 ScalarSSE = _mm_setr_ps(Right, Right, Right, Right);
	Result.Columns[0] = _mm_div_ps(Left.Columns[0], ScalarSSE);
	Result.Columns[1] = _mm_div_ps(Left.Columns[1], ScalarSSE);
	Result.Columns[2] = _mm_div_ps(Left.Columns[2], ScalarSSE);
	Result.Columns[3] = _mm_div_ps(Left.Columns[3], ScalarSSE);
#else 
	i32 Columns;
	for(Columns = 0; Columns < 4; ++Columns)
	{
		i32 Rows;
		for(Rows = 0; Rows < 4; ++Rows)
		{
			f32 Sum = 0;
			i32 CurrentMatrice;
			for(CurrentMatrice = 0; CurrentMatrice < 4; ++CurrentMatrice)
			{
				Sum += Left.Elements[CurrentMatrice][Rows] / Right;
			}

			Result.Elements[Columns][Rows] = Sum;
		}
	}
#endif

	return(Result);
}

m4 M4Ortho(f32 Right, f32 Left, f32 Top, f32 Bottom, f32 Near, f32 Far)
{
	m4 Result = M4Identity();

	Result.Elements[0][0] = 2.0f / (Right - Left);
	Result.Elements[1][1] = 2.0f / (Top - Bottom);
	Result.Elements[2][2] = 2.0f / (Near - Far);
	Result.Elements[3][3] = 1.0f;

	Result.Elements[3][0] = (Left + Right) / (Left - Right);
	Result.Elements[3][1] = (Bottom + Top) / (Bottom - Top);
	Result.Elements[3][2] = (Far + Near) / (Near - Far);

	return(Result);
}

m4 M4Perspective(f32 FOV, f32 AspectRatio, f32 Near, f32 Far)
{
	m4 Result = M4Identity();

	f32 Cotangent = 1.0f / tanf(FOV * (M_PI / 360.0f));

	Result.Elements[0][0] = Cotangent / AspectRatio;
	Result.Elements[1][1] = Cotangent;
	Result.Elements[2][3] = -1.0f;
	Result.Elements[2][2] = (Near + Far) / (Near - Far);
	Result.Elements[3][2] = (2.0f * Near * Far) / (Near - Far);
	Result.Elements[3][3] = 0.0f;

	return(Result);
}

m4 M4Translate(v3 Translation)
{
	m4 Result = M4Identity();

	Result.Elements[3][0] = Translation.X;
	Result.Elements[3][1] = Translation.Y;
	Result.Elements[3][2] = Translation.Z;

	return(Result);
}

m4 M4Rotate(f32 Angle, v3 Axis)
{
	m4 Result = M4Identity();

	Axis = V3Normalize(Axis);
	f32 SinTheta = sinf(ToRadians(Angle));
	f32 CosTheta = cosf(ToRadians(Angle));
	f32 CosValue = 1.0f - CosTheta;

	Result.Elements[0][0] = (Axis.X * Axis.X * CosValue) + CosTheta;
	Result.Elements[0][1] = (Axis.X * Axis.Y * CosValue) + (Axis.Z * SinTheta);
	Result.Elements[0][2] = (Axis.X * Axis.Z * CosValue) - (Axis.Y * SinTheta);

	Result.Elements[1][0] = (Axis.Y * Axis.X * CosValue) - (Axis.Z * SinTheta);
	Result.Elements[1][1] = (Axis.Y * Axis.Y * CosValue) + CosTheta;
	Result.Elements[1][2] = (Axis.Y * Axis.Z * CosValue) + (Axis.X * SinTheta);

	Result.Elements[2][0] = (Axis.Z * Axis.X * CosValue) + (Axis.Y * SinTheta);
	Result.Elements[2][1] = (Axis.Z * Axis.Y * CosValue) - (Axis.X * SinTheta);
	Result.Elements[2][2] = (Axis.Z * Axis.Z * CosValue) + CosTheta;

	return (Result);
}

m4 M4Scale(v3 Scale)
{
	m4 Result;

	Result.Elements[0][0] = Scale.X;
	Result.Elements[1][1] = Scale.Y;
	Result.Elements[2][2] = Scale.Z;

	return(Result);
}

m4 M4LookAt(v3 Eye, v3 Center, v3 Up)
{
	m4 Result;

	v3 F = V3Normalize(V3Sub(Center, Eye));
	v3 S = V3Normalize(V3Cross(F, Up));
	v3 U = V3Cross(S, F);

	Result.Elements[0][0] = S.X;
	Result.Elements[0][1] = U.X;
	Result.Elements[0][2] = -F.X;
	Result.Elements[0][3] = 0.0f;

	Result.Elements[1][0] = S.Y;
	Result.Elements[1][1] = U.Y;
	Result.Elements[1][2] = -F.Y;
	Result.Elements[1][3] = 0.0f;

	Result.Elements[2][0] = S.Z;
	Result.Elements[2][1] = U.Z;
	Result.Elements[2][2] = -F.Z;
	Result.Elements[2][3] = 0.0f;

	Result.Elements[3][0] = -V3Dot(S, Eye);
	Result.Elements[3][1] = -V3Dot(U, Eye);
	Result.Elements[3][2] =  V3Dot(F, Eye);
	Result.Elements[3][3] = 1.0f;

	return(Result);
}

q4 Q4New(f32 X, f32 Y, f32 Z, f32 W)
{
	return V4New(X, Y, Z, W);
}

q4 Q4Add(q4 Left, q4 Right)
{
	return V4Add(Left, Right);
}

q4 Q4Sub(q4 Left, q4 Right)
{
	return V4Sub(Left, Right);
}

q4 Q4Mul(q4 Left, q4 Right)
{
	q4 Result;

#ifdef CAMELLIA_SSE
	__m128 SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.InternalElementsSSE, Left.InternalElementsSSE, _MM_SHUFFLE(0, 0, 0, 0)), _mm_setr_ps(0.f, -0.f, 0.f, -0.f));
	__m128 SSEResultTwo = _mm_shuffle_ps(Right.InternalElementsSSE, Right.InternalElementsSSE, _MM_SHUFFLE(0, 1, 2, 3));
	__m128 SSEResultThree = _mm_mul_ps(SSEResultTwo, SSEResultOne);

	SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.InternalElementsSSE, Left.InternalElementsSSE, _MM_SHUFFLE(1, 1, 1, 1)) , _mm_setr_ps(0.f, 0.f, -0.f, -0.f));
	SSEResultTwo = _mm_shuffle_ps(Right.InternalElementsSSE, Right.InternalElementsSSE, _MM_SHUFFLE(1, 0, 3, 2));
	SSEResultThree = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));

	SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.InternalElementsSSE, Left.InternalElementsSSE, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setr_ps(-0.f, 0.f, 0.f, -0.f));
	SSEResultTwo = _mm_shuffle_ps(Right.InternalElementsSSE, Right.InternalElementsSSE, _MM_SHUFFLE(2, 3, 0, 1));
	SSEResultThree = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));

	SSEResultOne = _mm_shuffle_ps(Left.InternalElementsSSE, Left.InternalElementsSSE, _MM_SHUFFLE(3, 3, 3, 3));
	SSEResultTwo = _mm_shuffle_ps(Right.InternalElementsSSE, Right.InternalElementsSSE, _MM_SHUFFLE(3, 2, 1, 0));
	Result.InternalElementsSSE = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));
#else
	Result.X = (Left.X * Right.W) + (Left.Y * Right.Z) - (Left.Z * Right.Y) + (Left.W * Right.X);
	Result.Y = (-Left.X * Right.Z) + (Left.Y * Right.W) + (Left.Z * Right.X) + (Left.W * Right.Y);
	Result.Z = (Left.X * Right.Y) - (Left.Y * Right.X) + (Left.Z * Right.W) + (Left.W * Right.Z);
	Result.W = (-Left.X * Right.X) - (Left.Y * Right.Y) - (Left.Z * Right.Z) + (Left.W * Right.W);
#endif

	return(Result);
}

q4 Q4Mulf(q4 Left, f32 Right)
{
	return V4Mulf(Left, Right);
}

q4 Q4Divf(q4 Left, f32 Right)
{
	return V4Divf(Left, Right);
}

f32 Q4Dot(q4 Left, q4 Right)
{
	f32 Result;

#ifdef CAMELLIA_SSE
	__m128 SSEResultOne = _mm_mul_ps(Left.InternalElementsSSE, Right.InternalElementsSSE);
	__m128 SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(2, 3, 0, 1));
	SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
	SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(0, 1, 2, 3));
	SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
	_mm_store_ss(&Result, SSEResultOne);
#else
	Result = (Left.X * Right.X) + (Left.Y * Right.Y) + (Left.Z * Right.Z) + (Left.W * Right.W);
#endif

	return(Result);
}

q4 Q4Inv(q4 Quaternion)
{
	q4 Result;

	Result.X = -Quaternion.X;
	Result.Y = -Quaternion.Y;
	Result.Z = -Quaternion.Z;
	Result.W =  Quaternion.W;
	Result = Q4Divf(Result, Q4Dot(Quaternion, Quaternion));
	
	return(Result);
}

q4 Q4Normalize(q4 Quaternion)
{
	q4 Result;

	f32 Length = sqrt(Q4Dot(Quaternion, Quaternion));
	Result = Q4Divf(Quaternion, (1.0f / Length));

	return(Result);
}

q4 Q4NLerp(q4 Left, f32 Time, q4 Right)
{
	q4 Result;

#ifdef CAMELLIA_SSE
	__m128 ScalarLeft = _mm_set1_ps(1.0f - Time);
	__m128 ScalarRight = _mm_set1_ps(Time);
	__m128 SSEResultOne = _mm_mul_ps(Left.InternalElementsSSE, ScalarLeft);
	__m128 SSEResultTwo = _mm_mul_ps(Right.InternalElementsSSE, ScalarRight);
	Result.InternalElementsSSE = _mm_add_ps(SSEResultOne, SSEResultTwo);
#else
	Result.X = Lerp(Left.X, Time, Right.X);
	Result.Y = Lerp(Left.Y, Time, Right.Y);
	Result.Z = Lerp(Left.Z, Time, Right.Z);
	Result.W = Lerp(Left.W, Time, Right.W);
#endif
	Result = Q4Normalize(Result);

	return (Result);
}

q4 Q4SLerp(q4 Left, f32 Time, q4 Right)
{
	q4 Result;
	q4 QuaternionLeft;
	q4 QuaternionRight;

	f32 CosTheta = Q4Dot(Left, Right);
	f32 Angle = acosf(CosTheta);

	f32 S1 = sinf((1.0f - Time) * Angle);
	f32 S2 = sinf(Time * Angle);
	f32 Is = 1.0f / sinf(Angle);

	QuaternionLeft = Q4Mulf(Left, S1);
	QuaternionRight = Q4Mulf(Right, S2);

	Result = Q4Add(QuaternionLeft, QuaternionRight);
	Result = Q4Mulf(Result, Is);

	return(Result);
}

m4 Q4M4(q4 Quaternion)
{
	m4 Result;

	q4 NormalizedQuaternion = Q4Normalize(Quaternion);

	f32 XX, YY, ZZ,
		XY, XZ, YZ,
		WX, WY, WZ;

	XX = NormalizedQuaternion.X * NormalizedQuaternion.X;
	YY = NormalizedQuaternion.Y * NormalizedQuaternion.Y;
	ZZ = NormalizedQuaternion.Z * NormalizedQuaternion.Z;
	XY = NormalizedQuaternion.X * NormalizedQuaternion.Y;
	XZ = NormalizedQuaternion.X * NormalizedQuaternion.Z;
	YZ = NormalizedQuaternion.Y * NormalizedQuaternion.Z;
	WX = NormalizedQuaternion.W * NormalizedQuaternion.X;
	WY = NormalizedQuaternion.W * NormalizedQuaternion.Y;
	WZ = NormalizedQuaternion.W * NormalizedQuaternion.Z;

	Result.Elements[0][0] = 1.0f - 2.0f * (YY + ZZ);
	Result.Elements[0][1] = 2.0f * (XY + WZ);
	Result.Elements[0][2] = 2.0f * (XZ - WY);
	Result.Elements[0][3] = 0.0f;

	Result.Elements[1][0] = 2.0f * (XY - WZ);
	Result.Elements[1][1] = 1.0f - 2.0f * (XX + ZZ);
	Result.Elements[1][2] = 2.0f * (YZ + WX);
	Result.Elements[1][3] = 0.0f;

	Result.Elements[2][0] = 2.0f * (XZ + WY);
	Result.Elements[2][1] = 2.0f * (YZ - WX);
	Result.Elements[2][2] = 1.0f - 2.0f * (XX + YY);
	Result.Elements[2][3] = 0.0f;

	Result.Elements[3][0] = 0.0f;
	Result.Elements[3][1] = 0.0f;
	Result.Elements[3][2] = 0.0f;
	Result.Elements[3][3] = 1.0f;

	return (Result);
}

q4 Q4FromAxisAngle(v3 Axis, f32 AngleOfRotation)
{
	q4 Result;

	v3 AxisNormalized = V3Normalize(Axis);
	f32 SineOfRotation = sinf(AngleOfRotation / 2.0f);

	Result.XYZ = V3Mulf(AxisNormalized, SineOfRotation);
	Result.W = cosf(AngleOfRotation / 2.0f);

	return (Result);
}

f32 Normalize(f32 Input, f32 Min, f32 Max)
{
    f32 Average = (Min + Max) / 2;
    f32 Range = (Max - Min) / 2;
    return (Input - Average) / Range;
}

i32 Random(i32 Min, i32 Max)
{
    return (rand() % (Max - Min + 1)) + Min;
}

f32 ToRadians(f32 Degrees)
{
	return Degrees * (M_PI / 180.0f);
}

f32 Lerp(f32 A, f32 T, f32 B)
{
	return (1.0f - T) * A + T * B;
}