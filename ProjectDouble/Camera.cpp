//--------------------------------------------------------------------------------------
// Class encapsulating a camera
//--------------------------------------------------------------------------------------
// Holds position, rotation, near/far clip and field of view. These to a view and projection matrices as required

#include "Camera.h"
#include "Common.h"


void Camera::SetRotation(CVector3 rotation)
{
    mWorldMatrix = MatrixRotationZ(rotation.z) * MatrixRotationX(rotation.x) * MatrixRotationY(rotation.y) * MatrixTranslation(Position());
}


// Control the camera's position and rotation using keys provided
void Camera::Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
                                      KeyCode moveForward, KeyCode moveBackward, KeyCode moveLeft, KeyCode moveRight)
{
	//**** ROTATION ****
	if (KeyHeld(turnUp))
	{
		mWorldMatrix = MatrixRotationX(-ROTATION_SPEED * frameTime) * mWorldMatrix; // Use of frameTime to ensure same speed on different machines
	}
	if (KeyHeld(turnDown))
	{
		mWorldMatrix = MatrixRotationX(ROTATION_SPEED * frameTime) * mWorldMatrix; // Use of frameTime to ensure same speed on different machines
	}
	if (KeyHeld(turnRight))
	{
		auto tempPosition = Position();
		mWorldMatrix *= MatrixRotationY(ROTATION_SPEED * frameTime); // Use of frameTime to ensure same speed on different machines
		Position() = tempPosition;
	}
	if (KeyHeld(turnLeft))
	{
		auto tempPosition = Position();
		mWorldMatrix *= MatrixRotationY(-ROTATION_SPEED * frameTime); // Use of frameTime to ensure same speed on different machines
		Position() = tempPosition;
	}

	//**** LOCAL MOVEMENT ****
	if (KeyHeld(moveRight))
	{
		Position() += MOVEMENT_SPEED * XAxis() * frameTime; // Use of frameTime to ensure same speed on different machines
	}
	if (KeyHeld(moveLeft))
	{
		Position() -= MOVEMENT_SPEED * XAxis() * frameTime; // Use of frameTime to ensure same speed on different machines
	}
	if (KeyHeld(moveForward))
	{
		Position() += MOVEMENT_SPEED * ZAxis() * frameTime; // Use of frameTime to ensure same speed on different machines
	}
	if (KeyHeld(moveBackward))
	{
		Position() -= MOVEMENT_SPEED * ZAxis() * frameTime; // Use of frameTime to ensure same speed on different machines
	}
}


// Update the matrices used for the camera in the rendering pipeline
void Camera::UpdateMatrices()
{
    // View matrix is the usual matrix used for the camera in shaders, it is the inverse of the world matrix (see lectures)
    mViewMatrix = InverseAffine(mWorldMatrix);

    // Projection matrix, how to flatten the 3D world onto the screen (needs field of view, near and far clip, aspect ratio)
    float tanFOVx = std::tan(mFOVx * 0.5f);
    float scaleX = 1.0f / tanFOVx;
    float scaleY = mAspectRatio / tanFOVx;
    float scaleZa = mFarClip / (mFarClip - mNearClip);
    float scaleZb = -mNearClip * scaleZa;

    mProjectionMatrix = { scaleX,   0.0f,    0.0f,   0.0f,
                            0.0f, scaleY,    0.0f,   0.0f,
                            0.0f,   0.0f, scaleZa,   1.0f,
                            0.0f,   0.0f, scaleZb,   0.0f };

    // The view-projection matrix combines the two matrices usually used for the camera into one, which can save a multiply in the shaders (optional)
    mViewProjectionMatrix = mViewMatrix * mProjectionMatrix;
}


//-----------------------------------------------------------------------------
// Camera picking
//-----------------------------------------------------------------------------

// Return pixel coordinates corresponding to given world point when viewing from this
// camera. Pass the viewport width and height. The returned CVector3 contains the pixel
// coordinates in x and y and the Z-distance to the world point in z. If the Z-distance
// is less than the camera near clip (use NearClip() member function), then the world
// point is behind the camera and the 2D x and y coordinates are to be ignored.
CVector3 Camera::PixelFromWorldPt(CVector3 worldPoint, unsigned int viewportWidth, unsigned int viewportHeight)
{
	CVector3 pixelPoint;

	UpdateMatrices();

	// Transform world point into camera space and return immediately if point is behind camera near clip (it won't be on screen - no 2D pixel position)
	CVector4 cameraPt = CVector4(worldPoint, 1.0f) * mViewMatrix;
	if (cameraPt.z < mNearClip)
	{
		return { 0, 0, cameraPt.z };
	}

	// Now transform into viewport (2D) space
	CVector4 viewportPt = cameraPt * mProjectionMatrix;

	viewportPt.x /= viewportPt.w;
	viewportPt.y /= viewportPt.w;

	float x = (viewportPt.x + 1.0f) * viewportWidth  * 0.5f;
	float y = (1.0f - viewportPt.y) * viewportHeight * 0.5f;

	return { x, y, cameraPt.z };
}


// Return the size of a pixel in world space at the given Z distance. Allows us to convert the 2D size of areas on the screen to actualy sizes in the world
// Pass the viewport width and height
CVector2 Camera::PixelSizeInWorldSpace(float Z, unsigned int viewportWidth, unsigned int viewportHeight)
{
	CVector2 size;

	// Size of the entire viewport in world space at the near clip distance - uses same geometry work that was shown in the camera picking lecture
	CVector2 viewportSizeAtNearClip;
    viewportSizeAtNearClip.x = 2 * mNearClip * std::tan(mFOVx * 0.5f);
    viewportSizeAtNearClip.y = viewportSizeAtNearClip.x /  mAspectRatio;

	// Size of the entire viewport in world space at the given Z distance
	CVector2 viewportSizeAtZ = viewportSizeAtNearClip * Z / mNearClip;
	
	// Return world size of single pixel at given Z distance
	return { viewportSizeAtZ.x / viewportWidth, viewportSizeAtZ.y / viewportHeight };
}
