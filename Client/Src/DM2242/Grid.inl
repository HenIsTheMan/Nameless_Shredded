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