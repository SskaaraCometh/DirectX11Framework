cbuffer ConstantBuffer
{
	float3 Offset;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
};

//Vertex shader
VOut main(float4 position: POSITION, float4 colour: COLOUR)
{
	VOut output;

	output.position = position;
	output.position.x += Offset.x;
	output.position.y += Offset.y;
	output.position.z += Offset.z;
	output.colour = colour;
	
	//return struct
	return output;
}