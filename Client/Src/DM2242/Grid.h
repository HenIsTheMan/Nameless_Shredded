#pragma once

#include "Core.h"

#include "EntityType.hpp"

template <class T>
class Grid final{ //Row-major order
public:
	Grid<T>();
	Grid<T>(T cellWidth, T cellHeight, T lineThickness, int rows, int cols);
	~Grid<T>() = default;

	///Getters
	const std::vector<std::vector<EntityType>>& GetData() const;
	T GetCellWidth() const;
	T GetCellHeight() const;
	T GetLineThickness() const;
	int GetRows() const;
	int GetCols() const;

	///Setters
	void SetData(EntityType data, ptrdiff_t row, ptrdiff_t col);
	void SetCellWidth(T cellWidth);
	void SetCellHeight(T cellHeight);
	void SetLineThickness(T lineThickness);
	void SetRows(int rows);
	void SetCols(int cols);
private:
	std::vector<std::vector<EntityType>> im_Data;

	T im_CellWidth;
	T im_CellHeight;
	T im_LineThickness;

	int im_Rows;
	int im_Cols;

	void UpdateData();
};

#include "Grid.inl"