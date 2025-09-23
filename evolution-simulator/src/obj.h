#pragma once

#include <string>
#include "common.h"
#include "mesh.h"

std::unique_ptr<MeshData> importOBJ(std::string name, const char* filepath);
