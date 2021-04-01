//--------------------------------------------------------------------------------------
// Water height pixel shader
//--------------------------------------------------------------------------------------
// A pixel shader that simply outputs the world-y coordinate of each pixel - used to store the water height so the clipping boundary can be done correctly

#include "Common.hlsli"


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Water height pixel shader, stores for each screen pixel the world y-coordinate of the water at that point
float main(WorldPositionPixelShaderInput input) : SV_Target
{
  // Calculation was done in vertex shader already
  return input.worldPosition.y;
}
