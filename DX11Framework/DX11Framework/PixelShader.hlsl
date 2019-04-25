
float4 main(float4 position : SV_POSITION, float4 colour : COLOUR) : SV_TARGET
{
	//Pass variables in the same order between shaders
	return colour;
}