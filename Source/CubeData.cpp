/*
  ==============================================================================

    CubeData.cpp
    Created: 21 Mar 2021 8:37:16pm
    Author:  corbin

  ==============================================================================
*/

#include "CubeData.h"

CubeData::CubeData(Type type) : type{type}, state{static_cast<int>(type)}, quat{0, 0, 0, 1} {}