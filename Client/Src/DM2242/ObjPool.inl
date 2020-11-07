template <class T>
ObjPool<T>::ObjPool():
	im_ObjPool()
{
}

template <class T>
ObjPool<T>::~ObjPool(){
	const size_t poolSize = im_ObjPool.size();
	for(size_t i = 0; i < poolSize; ++i){
		T*& obj = im_ObjPool[i].second;
		if(obj){
			delete obj;
			obj = nullptr;
		}
	}
}

template <class T>
const std::vector<std::pair<bool, T*>>& ObjPool<T>::GetObjPool(){
	return im_ObjPool;
}

template <class T>
T* const& ObjPool<T>::RetrieveInactiveObj(){
	const size_t poolSize = im_ObjPool.size();
	for(size_t i = 0; i < poolSize; ++i){
		if(!im_ObjPool[i].first){
			im_ObjPool[i].first = true;
			return im_ObjPool[i].second;
		}
	}

	im_ObjPool.push_back({false, new T()});
	return im_ObjPool.back().second;
}

template <class T>
void ObjPool<T>::CreateObjs(int amt){
	assert(im_ObjPool.size() == (size_t)0 && "This func shld only be called once");

	im_ObjPool.resize(amt);
	for(int i = 0; i < amt; ++i){
		im_ObjPool[i] = std::make_pair(false, new T());
	}
}