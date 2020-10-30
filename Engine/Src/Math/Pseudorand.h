#include <random>

template <class T>
inline T PseudorandMinMax(const T& min, const T& max);

template <>
inline int PseudorandMinMax<int>(const int& min, const int& max);

#include "Pseudorand.inl"