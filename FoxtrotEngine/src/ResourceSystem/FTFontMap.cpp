#include "ResourceSystem/FTFontMap.h"

#include <fstream>
#include <sstream>
#include <queue>
#include <assert.h>
#include <string>

#include "Debugging/DebugMemAlloc.h"
#include "Tile.h"

//void FTFontMap::ReadCSV()
//{
//    if (GetTiles())
//        delete[] GetTiles();
//
//    std::wifstream myFile;
//    std::queue<wchar_t> result;
//
//    // Open an existing file 
//    myFile.open(GetRelativePath(), std::fstream::in);
//    assert(myFile);
//    std::wstring line;
//    wchar_t val;
//    int column = 0; int row = 0;
//    while (std::getline(myFile, line))
//    {
//        std::wstringstream ss(line);
//        // Create a stringstream of the current line
//        // Extract each integer
//        while (ss >> val)
//        {
//            result.push(val);
//            // if (ss.peek() == ',') ss.ignore();
//            ++column;
//        }
//        ++row;
//    }
//    if (row != 0)
//        column /= row;
//    myFile.close();
//
//    SetMaxCountOnScreenX(column);
//    SetMaxCountOnScreenY(row);
//
//    SetTiles(DBG_NEW Tile[column * row]);
//    for (int r = 0; r < row; ++r)
//    {
//        for (int c = 0; c < column; ++c)
//        {
//            if (!result.empty())
//            {
//                InitializeTile(GetTile(r,c), c, r, result.front());
//                result.pop();
//            }
//        }
//    }
//}
