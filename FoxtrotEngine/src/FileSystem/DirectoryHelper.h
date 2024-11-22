#pragma once
#include <string>
#include <functional>
#include <filesystem>

class DirectoryHelper
{
public:
	template <class UnaryOperation>
	static void IterateForFileRecurse(
		std::string& dir,
		UnaryOperation&& unaryOp
	)
	{
		for (const std::filesystem::directory_entry& dirEntry :
			std::filesystem::recursive_directory_iterator(dir))
		{
			if(dirEntry.is_regular_file())
				unaryOp(std::move(dirEntry.path().string()));
		}
	}
};