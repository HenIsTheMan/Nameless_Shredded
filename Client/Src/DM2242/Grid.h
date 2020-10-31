#pragma once

template <class T>
class Grid final{
public:
	Grid<T>();
	Grid<T>(T cellWidth, T cellHeight, int rows, int cols);
	~Grid<T>() = default;

private:
	T im_CellWidth;
	T im_CellHeight;
	int im_Rows;
	int im_Cols;
};

#include "Grid.inl"