/* date = August 12th 2022 10:12 pm */

#ifndef CAMELLIA_MATH_H
#define CAMELLIA_MATH_H

#include "camellia_platform.h"

#ifdef _MSC_VER
	#define CAMELLIA_SSE
	#include <xmmintrin.h>
#endif

typedef union v2 {
	struct {
		f32 X, Y;
	};
	struct {
		f32 U, V;
	};
	struct {
		f32 Left, Right;
	};
	struct {
		f32 Width, Height;
	};

	f32 Elements[2];
} v2;

typedef union v3 {
	struct {
		f32 X, Y, Z;
	};
	struct {
		f32 R, G, B;
	};
	struct {
		f32 U, V, W;
	};

	f32 Elements[3];
} v3;

typedef union v4 {
	struct {
		union {
			v3 XYZ;
			struct {
				f32 X, Y, Z;
			};
		};

		f32 W;
	};
	struct {
		union {
			v3 RGB;
			struct {
				f32 R, G, B;
			};
		};

		f32 A;
	};

	f32 Elements[4];

#ifdef CAMELLIA_SSE
	__m128 InternalElementsSSE;
#endif	
} v4;

typedef union m4 {
	f32 Elements[4][4];

#ifdef CAMELLIA_SSE
	__m128 Columns[4];
#endif
} m4;

typedef v4 q4;

f32 Normalize(f32 Input, f32 Min, f32 Max);
i32 Random(i32 Min, i32 Max);
f32 ToRadians(f32 Degrees);
f32 Lerp(f32 A, f32 T, f32 B);

v2 V2New(f32 X, f32 Y);
v2 V2Scalar(f32 Scalar);
v2 V2Zero();
v2 V2Add(v2 Left, v2 Right);
v2 V2Addf(v2 Left, f32 Right);
v2 V2Sub(v2 Left, v2 Right);
v2 V2Subf(v2 Left, f32 Right);
v2 V2Mul(v2 Left, v2 Right);
v2 V2Mulf(v2 Left, f32 Right);
v2 V2Div(v2 Left, v2 Right);
v2 V2Divf(v2 Left, f32 Right);
f32 V2Dot(v2 Left, v2 Right);
f32 V2Length(v2 Vector);
v2 V2Normalize(v2 Vector);

v3 V3New(f32 X, f32 Y, f32 Z);
v3 V3Scalar(f32 Scalar);
v3 V3Zero();
v3 V3Add(v3 Left, v3 Right);
v3 V3Addf(v3 Left, f32 Right);
v3 V3Sub(v3 Left, v3 Right);
v3 V3Subf(v3 Left, f32 Right);
v3 V3Mul(v3 Left, v3 Right);
v3 V3Mulf(v3 Left, f32 Right);
v3 V3Div(v3 Left, v3 Right);
v3 V3Divf(v3 Left, f32 Right);
f32 V3Dot(v3 Left, v3 Right);
v3 V3Cross(v3 Left, v3 Right);
f32 V3Length(v3 Vector);
v3 V3Normalize(v3 Vector);

v4 V4New(f32 X, f32 Y, f32 Z, f32 W);
v4 V4Scalar(f32 Scalar);
v4 V4Zero();
v4 V4Add(v4 Left, v4 Right);
v4 V4Addf(v4 Left, f32 Right);
v4 V4Sub(v4 Left, v4 Right);
v4 V4Subf(v4 Left, f32 Right);
v4 V4Mul(v4 Left, v4 Right);
v4 V4Mulf(v4 Left, f32 Right);
v4 V4Div(v4 Left, v4 Right);
v4 V4Divf(v4 Left, f32 Right);
f32 V4Dot(v4 Left, v4 Right);
f32 V4Length(v4 Vector);
v4 V4Normalize(v4 Vector);

m4 M4Zero();
m4 M4d(f32 Diagonal);
m4 M4Identity();
m4 M4Transpose(m4 Matrix);
m4 M4Add(m4 Left, m4 Right);
m4 M4Sub(m4 Left, m4 Right);
m4 M4Mul(m4 Left, m4 Right);
m4 M4Mulf(m4 Left, f32 Right);
v4 M4MulV4(m4 Left, v4 Right);
m4 M4Divf(m4 Left, f32 Right);
m4 M4Ortho(f32 Right, f32 Left, f32 Top, f32 Bottom, f32 Near, f32 Far);
m4 M4Perspective(f32 FOV, f32 AspectRatio, f32 Near, f32 Far);
m4 M4LookAt(v3 Eye, v3 Center, v3 Up);
m4 M4Translate(v3 Translation);
m4 M4Rotate(f32 Angle, v3 Axis);
m4 M4Scale(v3 Scale);

q4 Q4New(f32 X, f32 Y, f32 Z, f32 W);
q4 Q4Add(q4 Left, q4 Right);
q4 Q4Sub(q4 Left, q4 Right);
q4 Q4Mul(q4 Left, q4 Right);
q4 Q4Mulf(q4 Left, f32 Right);
q4 Q4Divf(q4 Left, f32 Right);
f32 Q4Dot(q4 Left, q4 Right);
q4 Q4Inv(q4 Quaternion);
q4 Q4Normalize(q4 Quaternion);
q4 Q4NLerp(q4 Left, f32 Time, q4 Right);
q4 Q4SLerp(q4 Left, f32 Time, q4 Right);
m4 Q4M4(q4 Quaternion);
q4 Q4FromAxisAngle(v3 Axis, f32 AngleOfRotation);


#endif //CAMELLIA_MATH_H
