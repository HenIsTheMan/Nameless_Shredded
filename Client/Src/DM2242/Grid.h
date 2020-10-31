#pragma once

template <class T>
class Grid final{
public:
	Grid<T>();
	Grid<T>(T cellWidth, T cellHeight, T rows, T cols);
	~Grid<T>() = default;
	
private:
	T im_CellWidth;
	T im_CellHeight;
	T im_Rows;
	T im_Cols;
};

#include "Grid.inl"