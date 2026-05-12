#pragma once
namespace D3D11
{
	struct FontKerning
	{
		int	  firstid;	// the first character
		int	  secondid; // the second character
		float amount;	// the amount to add/subtract to second characters x
	};
} // namespace D3D11