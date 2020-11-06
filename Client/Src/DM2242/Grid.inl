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
	im_Data(),
	im_CellWidth(cellWidth),
	im_CellHeight(cellHeight),
	im_LineThickness(lineThickness),
	im_Rows(rows),
	im_Cols(cols)
{
	UpdateData();
}

template <class T>
const std::vector<std::vector<EntityType>>& Grid<T>::GetData() const{
	return im_Data;
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
void Grid<T>::SetData(EntityType data, ptrdiff_t row, ptrdiff_t col){
	this->im_Data[row][col] = data;
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
	std::vector<std::vector<EntityType>> oldData = im_Data; //Make copy of data
	im_Data = std::vector<std::vector<EntityType>>(im_Rows);

	///Create vars
	size_t i, j;
	const size_t oldDataSize = oldData.size();

	for(i = (size_t)0; i < im_Rows; ++i){
		im_Data[i] = std::vector<EntityType>(im_Cols);
		for(j = (size_t)0; j < im_Cols; ++j){
			im_Data[i][j] = EntityType::Null; //All data becomes EntityType::Null
		}
	}

	for(i = (size_t)0; i < (im_Rows < oldDataSize ? im_Rows : oldDataSize); ++i){
		const size_t amtOfCols = im_Data[i].size();
		const size_t oldAmtOfCols = oldData[i].size();
		for(j = (size_t)0; j < (amtOfCols < oldAmtOfCols ? amtOfCols : oldAmtOfCols); ++j){
			im_Data[i][j] = oldData[i][j];
		}
	}
}