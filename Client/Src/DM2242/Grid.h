#pragma once

template <class T>
class Grid final{
public:
	Grid<T>();
	Grid<T>(T cellWidth, T cellHeight, int rows, int cols);
	~Grid<T>() = default;

	///Getters
	T GetCellWidth() const;
	T GetCellHeight() const;
	int GetRows() const;
	int GetCols() const;

	///Setters
	void SetCellWidth(T cellWidth);
	void SetCellHeight(T cellHeight);
	void SetRows(int rows);
	void SetCols(int cols);
private:
	T im_CellWidth;
	T im_CellHeight;
	int im_Rows;
	int im_Cols;
};

#include "Grid.inl"