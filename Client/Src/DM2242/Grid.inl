template <class T>
Grid<T>::Grid():
	Grid(
		(T)0,
		(T)0,
		(T)0,
		0,
		0
	)
{
	UpdateData();
}

template <class T>
Grid<T>::Grid(T cellWidth, T cellHeight, T lineThickness, int rows, int cols):
	im_CellWidth(cellWidth),
	im_CellHeight(cellHeight),
	im_LineThickness(lineThickness),
	im_Rows(rows),
	im_Cols(cols)
{
	UpdateData();
}

template <class T>
void Grid<T>::SetData(EntityType data, ptrdiff_t row, ptrdiff_t col){
	this->data[row][col] = data;
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
T Grid<T>::GetLineThickness() const{
	return im_LineThickness;
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
void Grid<T>::SetLineThickness(T lineThickness){
	im_LineThickness = lineThickness;
}

template <class T>
void Grid<T>::SetRows(int rows){
	im_Rows = rows;
	UpdateData();
}

template <class T>
void Grid<T>::SetCols(int cols){
	im_Cols = cols;
	UpdateData();
}

template <class T>
void Grid<T>::UpdateData(){
	std::vector<std::vector<EntityType>> oldData = data; //Make copy of data
	data = std::vector<std::vector<EntityType>>(im_Rows);

	///Create vars
	size_t i, j;
	const size_t oldDataSize = oldData.size();

	for(i = (size_t)0; i < im_Rows; ++i){
		data[i] = std::vector<EntityType>(im_Cols);
		for(j = (size_t)0; j < im_Cols; ++j){
			data[i][j] = EntityType::Null; //All data becomes EntityType::Null
		}
	}

	for(i = (size_t)0; i < (im_Rows < oldDataSize ? im_Rows : oldDataSize); ++i){
		const size_t amtOfCols = data[i].size();
		const size_t oldAmtOfCols = oldData[i].size();
		for(j = (size_t)0; j < (amtOfCols < oldAmtOfCols ? amtOfCols : oldAmtOfCols); ++j){
			data[i][j] = oldData[i][j];
		}
	}
}