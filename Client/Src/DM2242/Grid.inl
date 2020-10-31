template <class T>
Grid<T>::Grid():
	Grid(
	(T)0,
	(T)0,
	0,
	0
	)
{
}

template <class T>
Grid<T>::Grid(T cellWidth, T cellHeight, int rows, int cols):
	im_CellWidth(cellWidth),
	im_CellHeight(cellHeight),
	im_Rows(rows),
	im_Cols(cols)
{
}

template <class T>
T Grid<T>::GetCellWidth() const{
	return im_CellWidth;
}

template <class T>
T Grid<T>::GetCellHeight() const{
	return im_CellHeight;
}

template <class T>
int Grid<T>::GetRows() const{
	return im_Rows;
}

template <class T>
int Grid<T>::GetCols() const{
	return im_Cols;
}

template <class T>
void Grid<T>::SetCellWidth(T cellWidth){
	im_CellWidth = cellWidth;
}

template <class T>
void Grid<T>::SetCellHeight(T cellHeight){
	im_CellHeight = cellHeight;
}

template <class T>
void Grid<T>::SetRows(int rows){
	im_Rows = rows;
}

template <class T>
void Grid<T>::SetCols(int cols){
	im_Cols = cols;
}