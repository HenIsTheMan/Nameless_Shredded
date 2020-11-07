#pragma once

#include "Core.h"

template <class T>
class Grid final{ //Row-major order
public:
	Grid<T>();
	Grid<T>(T cellWidth, T cellHeight, T lineThickness, int rows, int cols);
	~Grid<T>() = default;

	///Getters
	T GetCellWidth() const;
	T GetCellHeight() const;
	T GetLineThickness() const;
	int GetRows() const;
	int GetCols() const;

	///Setters
	void SetCellWidth(T cellWidth);
	void SetCellHeight(T cellHeight);
	void SetLineThickness(T lineThickness);
	void SetRows(int rows);
	void SetCols(int cols);
private:
	T im_CellWidth;
	T im_CellHeight;
	T im_LineThickness;

	int im_Rows;
	int im_Cols;
};

#include "Grid.inl"