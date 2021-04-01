//--------------------------------------------------------------------------------------
// Matrix4x4 class (cut down version) to hold matrices for 3D
//--------------------------------------------------------------------------------------
// Code in .cpp file

#ifndef _CMATRIX4X4_H_DEFINED_
#define _CMATRIX4X4_H_DEFINED_

#include "CVector3.h"
#include "CVector4.h"
#include <cmath>
#include "BaseMath.h"


// Matrix class
class CMatrix4x4
{
// Concrete class - public access
public:
	// Matrix elements
	float e00, e01, e02, e03;
	float e10, e11, e12, e13;
	float e20, e21, e22, e23;
	float e30, e31, e32, e33;

 
    /*-----------------------------------------------------------------------------------------
        Member functions
    -----------------------------------------------------------------------------------------*/

	// Set a single row (range 0-3) of the matrix using a CVector3. Fourth element left unchanged
    // Can be used to set position or x,y,z axes in a matrix
    void SetRow(int iRow, const CVector3& v);

    // Get a single row (range 0-3) of the matrix into a CVector3. Fourth element is ignored
    // Can be used to access position or x,y,z axes from a matrix
    CVector3 GetRow(int iRow) const;

    // Initialise this matrix with a pointer to 16 floats 
    void SetValues(float* matrixValues)  { *this = *reinterpret_cast<CMatrix4x4*>(matrixValues); }
	void FaceTarget
	(
		const CVector3& target,
		const CVector3& up /*= CVector3::kYAxis*/,
		const bool      bLH /*= true*/
	);
	
    // Helper functions
    CVector3 GetXAxis() const { return GetRow(0); }
    CVector3 GetYAxis() const { return GetRow(1); }
    CVector3 GetZAxis() const { return GetRow(2); }
    CVector3 GetPosition() const  { return GetRow(3); }
    CVector3 GetEulerAngles();
    CVector3 GetScale() const  { return { Length(GetXAxis()), Length(GetYAxis()) , Length(GetZAxis()) }; }

    
	// Post-multiply this matrix by the given one
    CMatrix4x4& operator*=(const CMatrix4x4& m);

	// Return the given CVector4 transformed by this matrix
	CVector4 operator*=(const CVector4& v);



    // Make this matrix an affine 3D transformation matrix to face from current position to given
    // target (in the Z direction). Can pass up vector for the constructed matrix and specify
    // handedness (right-handed Z axis will face away from target)
    // Will retain the matrix's current scaling
    void FaceTarget(const CVector3& target);


    // Transpose the matrix (rows become columns). There are two ways to store a matrix, by rows or by columns.
    // Different apps use different methods. Use Transpose to swap when necessary.
    void Transpose();
	void RotateWorldX(const float x)
	{
		// Perform minimum of calculations rather than use full matrix multiply
		float sX, cX;
		gen::SinCos(x, &sX, &cX);
		float t;
		t = e01 * sX + e02 * cX;
		e01 = e01 * cX - e02 * sX;
		e02 = t;
		t = e11 * sX + e12 * cX;
		e11 = e11 * cX - e12 * sX;
		e12 = t;
		t = e21 * sX + e22 * cX;
		e21 = e21 * cX - e22 * sX;
		e22 = t;
		t = e31 * sX + e32 * cX;
		e31 = e31 * cX - e32 * sX;
		e32 = t;
	}

	// Rotate an affine transformation matrix by given angle (radians) around world Y axis/origin
	// Note: The position (translation) will also rotate round the world Y axis, use RotateY to
	// perform a world Y rotation around local origin
	void RotateWorldY(const float y)
	{
		// Perform minimum of calculations rather than use full matrix multiply
		float sY, cY;
		gen::SinCos(y, &sY, &cY);
		float t;
		t = e00 * cY + e02 * sY;
		e02 = e02 * cY - e00 * sY;
		e00 = t;
		t = e10 * cY + e12 * sY;
		e12 = e12 * cY - e10 * sY;
		e10 = t;
		t = e20 * cY + e22 * sY;
		e22 = e22 * cY - e20 * sY;
		e20 = t;
		t = e30 * cY + e32 * sY;
		e32 = e32 * cY - e30 * sY;
		e30 = t;
	}

	// Rotate an affine transformation matrix by given angle (radians) around world Z axis/origin
	// Note: The position (translation) will also rotate round the world Z axis, use RotateZ to
	// perform a world Z rotation around local origin
	void RotateWorldZ(const float z)
	{
		// Perform minimum of calculations rather than use full matrix multiply
		float sZ, cZ;
		gen::SinCos(z, &sZ, &cZ);
		float t;
		t = e00 * sZ + e01 * cZ;
		e00 = e00 * cZ - e01 * sZ;
		e01 = t;
		t = e10 * sZ + e11 * cZ;
		e10 = e10 * cZ - e11 * sZ;
		e11 = t;
		t = e20 * sZ + e21 * cZ;
		e20 = e20 * cZ - e21 * sZ;
		e21 = t;
		t = e30 * sZ + e31 * cZ;
		e30 = e30 * cZ - e31 * sZ;
		e31 = t;
	}
	// Rotate an affine transformation by given angle (radians) around world X axis & local origin
	// Note: The position (translation) will not be altered, use RotateWorldX to perform a world X
	// rotation around world origin
	void RotateX(const float x)
	{
		// Perform minimum of calculations rather than use full matrix multiply
		float sX, cX;
		gen::SinCos(x, &sX, &cX);
		float t;
		t = e01 * sX + e02 * cX;
		e01 = e01 * cX - e02 * sX;
		e02 = t;
		t = e11 * sX + e12 * cX;
		e11 = e11 * cX - e12 * sX;
		e12 = t;
		t = e21 * sX + e22 * cX;
		e21 = e21 * cX - e22 * sX;
		e22 = t;
	}

	// Rotate an affine transformation by given angle (radians) around world Y axis & local origin
	// Note: The position (translation) will not be altered, use RotateWorldY to perform a world Y
	// rotation around world origin
	void RotateY(const float y)
	{
		// Perform minimum of calculations rather than use full matrix multiply
		float sY, cY;
		gen::SinCos(y, &sY, &cY);
		float t;
		t = e00 * cY + e02 * sY;
		e02 = e02 * cY - e00 * sY;
		e00 = t;
		t = e10 * cY + e12 * sY;
		e12 = e12 * cY - e10 * sY;
		e10 = t;
		t = e20 * cY + e22 * sY;
		e22 = e22 * cY - e20 * sY;
		e20 = t;
	}

	// Rotate an affine transformation by given angle (radians) around world Z axis & local origin
	// Note: The position (translation) will not be altered, use RotateWorldZ to perform a world Z
	// rotation around world origin
	void RotateZ(const float z)
	{
		// Perform minimum of calculations rather than use full matrix multiply
		float sZ, cZ;
		gen::SinCos(z, &sZ, &cZ);
		float t;
		t = e00 * sZ + e01 * cZ;
		e00 = e00 * cZ - e01 * sZ;
		e01 = t;
		t = e10 * sZ + e11 * cZ;
		e10 = e10 * cZ - e11 * sZ;
		e11 = t;
		t = e20 * sZ + e21 * cZ;
		e20 = e20 * cZ - e21 * sZ;
		e21 = t;
	}


	// Rotate an affine transformation matrix by given angle (radians) around local X axis/origin
	// Assumes matrix built in the order: M = Scale*Rotation*Translation. Use RotateLocalXUnscaled
	// if there is no scaling (more efficient)
	void RotateLocalX(const float x)
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot*Trans,
		// becoming Scale*[XRot*Rot]*Trans
		float scaleSqY = e10 * e10 + e11 * e11 + e12 * e12;
		float scaleSqZ = e20 * e20 + e21 * e21 + e22 * e22;
		GEN_ASSERT_OPT(!IsZero(scaleSqY) && !IsZero(scaleSqZ), "Singular matrix");
		float scaleYZ = gen::Sqrt(scaleSqY) * gen::InvSqrt(scaleSqZ);

		float sX, cX, sXY, sXZ;
		gen::SinCos(x, &sX, &cX);
		sXY = sX * scaleYZ;
		sXZ = sX / scaleYZ;

		// Perform minimum of calculations rather than use full matrix multiply
		float t;
		t = e10 * cX + e20 * sXY;
		e20 = e20 * cX - e10 * sXZ;
		e10 = t;
		t = e11 * cX + e21 * sXY;
		e21 = e21 * cX - e11 * sXZ;
		e11 = t;
		t = e12 * cX + e22 * sXY;
		e22 = e22 * cX - e12 * sXZ;
		e12 = t;

	}
	void MakeRotation
	(
		const CVector3& axis,
		const float  fAngle
	);
	// Rotate an affine transformation matrix by given angle (radians) around local Y axis/origin
	// Assumes matrix built in the order: M = Scale*Rotation*Translation. Use RotateLocalYUnscaled
	// if there is no scaling (more efficient)
	void RotateLocalY(const float y)
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot*Trans,
		// becoming Scale*[YRot*Rot]*Trans
		float scaleSqX = e00 * e00 + e01 * e01 + e02 * e02;
		float scaleSqZ = e20 * e20 + e21 * e21 + e22 * e22;
		GEN_ASSERT_OPT(!IsZero(scaleSqX) && !IsZero(scaleSqZ), "Singular matrix");
		float scaleZX = gen::Sqrt(scaleSqZ) * gen::InvSqrt(scaleSqX);

		float sY, cY, sYZ, sYX;
		gen::SinCos(y, &sY, &cY);
		sYZ = sY * scaleZX;
		sYX = sY / scaleZX;

		// Perform minimum of calculations rather than use full matrix multiply
		float t;
		t = e20 * cY + e00 * sYZ;
		e00 = e00 * cY - e20 * sYX;
		e20 = t;
		t = e21 * cY + e01 * sYZ;
		e01 = e01 * cY - e21 * sYX;
		e21 = t;
		t = e22 * cY + e02 * sYZ;
		e02 = e02 * cY - e22 * sYX;
		e22 = t;
	}
	// Rotate an affine transformation matrix by given angle (radians) around local Z axis/origin
// Assumes matrix built in the order: M = Scale*Rotation*Translation. Use RotateLocalZUnscaled
// if there is no scaling (more efficient)
	void RotateLocalZ(const float z)
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot*Trans,
		// becoming Scale*[ZRot*Rot]*Translation
		float scaleSqX = e00 * e00 + e01 * e01 + e02 * e02;
		float scaleSqY = e10 * e10 + e11 * e11 + e12 * e12;
		GEN_ASSERT_OPT(!IsZero(scaleSqX) && !IsZero(scaleSqY), "Singular matrix");
		float scaleXY = gen::Sqrt(scaleSqX) * gen::InvSqrt(scaleSqY);

		float sZ, cZ, sZX, sZY;
		gen::SinCos(z, &sZ, &cZ);
		sZX = sZ * scaleXY;
		sZY = sZ / scaleXY;

		// Perform minimum of calculations rather than use full matrix multiply
		float t;
		t = e00 * cZ + e10 * sZX;
		e10 = e10 * cZ - e00 * sZY;
		e00 = t;
		t = e01 * cZ + e11 * sZX;
		e11 = e11 * cZ - e01 * sZY;
		e01 = t;
		t = e02 * cZ + e12 * sZX;
		e12 = e12 * cZ - e02 * sZY;
		e02 = t;
	}
	// Make this matrix the identity matrix
	void MakeIdentity();


	// Make this matrix a translation by the given vector
	void MakeTranslation(const CVector3& translate);


	// Make this matrix an X-axis rotation by the given angle (radians)
	void MakeRotationX(const float x);

	// Make this matrix a Y-axis rotation by the given angle (radians)
	void MakeRotationY(const float y);

	// Make this matrix a Z-axis rotation by the given angle (radians)
	void MakeRotationZ(const float z);


};


/*-----------------------------------------------------------------------------------------
    Non-member Operators
-----------------------------------------------------------------------------------------*/

// Matrix-matrix multiplication
CMatrix4x4 operator*(const CMatrix4x4& m1, const CMatrix4x4& m2);

// Return the given CVector4 transformed by the given matrix
CVector4 operator*(const CVector4& v, const CMatrix4x4& m);


/*-----------------------------------------------------------------------------------------
  Non-member functions
-----------------------------------------------------------------------------------------*/

// The following functions create a new matrix holding a particular transformation
// They can be used as temporaries in calculations, e.g.
//     CMatrix4x4 m = MatrixScaling( 3.0f ) * MatrixTranslation( CVector3(10.0f, -10.0f, 20.0f) );

// Return an identity matrix
CMatrix4x4 MatrixIdentity();

// Return a translation matrix of the given vector
CMatrix4x4 MatrixTranslation(const CVector3& t);


// Return an X-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationX(float x);

// Return a Y-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationY(float y);

// Return a Z-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationZ(float z);


// Return a matrix that is a scaling in X,Y and Z of the values in the given vector
CMatrix4x4 MatrixScaling(const CVector3& s);

// Return a matrix that is a uniform scaling of the given amount
CMatrix4x4 MatrixScaling(const float s);



// Return the inverse of given matrix assuming that it is an affine matrix
// Advanced calulation needed to get the view matrix from the camera's positioning matrix
CMatrix4x4 InverseAffine(const CMatrix4x4& m);


#endif // _CMATRIX4X4_H_DEFINED_
