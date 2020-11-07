#pragma once

#include "Core.h"

template <class T>
class ObjPool final{
public:
	ObjPool<T>();
	~ObjPool<T>();

	std::vector<std::pair<bool, T*>>& RetrieveObjPool();
	T*& RetrieveInactiveObj();
	void CreateObjs(int amt);
private:
	std::vector<std::pair<bool, T*>> im_ObjPool;
};

#include "ObjPool.inl"