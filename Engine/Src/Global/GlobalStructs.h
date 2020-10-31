#pragma once
#include "../Core.h"

struct SetUpTexsParams final{
    str texPath = str();
    bool flipTex = false;
    int texTarget = 0;
    int texWrapParam = 0;
    int texFilterMin = 0;
    int texFilterMag = 0;
};