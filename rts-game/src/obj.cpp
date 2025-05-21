#include "obj.h"

#include <string>
#include <vector>
#include <variant>
#include <fstream>

#include "common.h"
#include "mesh.h"

namespace OBJ {

template <typename A, typename B>
struct Either {
	std::variant<A, B> data;
	operator bool() {
		return std::holds_alternative<A>(data);
	}
	A& operator*() {
		return std::get<0>(data);
	}
	Either(A a) { data = std::variant<A, B>(a); }
	Either(B b) { data = std::variant<A, B>(b); }
};

struct v3i {
	int x;
	int y;
	int z;
};

bool consume(std::string& a, const char* b) {
	int i = 0;
	while (b[i] != '\0') {
		if (a.c_str()[i] != b[i]) return false;
		i++;
	}
	a.erase(0, i);
	return true;
}

std::string parseWord(std::string& a) {
	size_t i = a.find_first_of(' ');
	std::string ret = a.substr(0, i);
	a.erase(0, i);
	if (i == 0) {
		a.erase(0, 1);
		return parseWord(a);
	} else return ret;
}

struct Parsed {
	enum Type {
		MtlLib, UseMtl, Object, Group, SmoothShading, Line, FaceIndices, Comment, Vertex, ParameterSpace, Normal, Texcoord, Error, Blank
	};
	Type type;
	union {
		struct {
			u8 numElements;
			XMFLOAT4 elements;
		};
		struct {
			v3i f1;
			v3i f2;
			v3i f3;
		};
	};
	const char* str;
};

enum Error {
	FloatParseError, IntParseError, FaceParseError
};

Either<float, Error> tryParseFloat(std::string& s) {
	std::string word = parseWord(s);
	const char* start = word.c_str();
	char* end;
	float ret = strtof(start, &end);
	if (end == start)
		return FloatParseError;

	return ret;
}

Either<int, Error> parseInt(std::string& s) {
	const char* start = s.c_str();
	char* end;
	int ret = (int)strtol(start, &end, 10);
	if (end == start)
		return IntParseError;

	s.erase(0, end - start);
	return ret;
}

bool parseFloats(int num, std::string& line, Parsed& ret) {
	ret.numElements = 0;
	Either<float, Error> res = FloatParseError;
	if (!(res = tryParseFloat(line)))
		goto fin;
	ret.elements.x = *res;
	++ret.numElements;
	if (!(res = tryParseFloat(line)))
		goto fin;
	ret.elements.y = *res;
	++ret.numElements;
	if (!(res = tryParseFloat(line)))
		goto fin;
	ret.elements.z = *res;
	++ret.numElements;
	if (!(res = tryParseFloat(line)))
		goto fin;
	ret.elements.w = *res;
	++ret.numElements;

fin:
	if (ret.numElements == 0) {
		ret.str = "No elements found for vector";
		ret.type = Parsed::Error;
		return false;
	}

	return true;
}

Either<v3i, Error> parseFace(std::string& line) {
	v3i ret = {-1, -1, -1};
	Either<int, Error> res = FaceParseError;
	if (!(res = parseInt(line)))
		return FaceParseError;

	ret.x = *res;
	if (!consume(line, "/"))
		return ret;

	if (res = parseInt(line))
		ret.y = *res;

	if (!consume(line, "/"))
		return ret;

	if (res = parseInt(line))
		ret.z = *res;

	return ret;
}

Parsed parseLine(std::string line) {
	Parsed ret = {};
	ret.type = Parsed::Error;
	if (consume(line, "#")) {
		ret.type = Parsed::Comment;
		ret.str = line.c_str();
	} else if (consume(line, "v ")) {
		if (parseFloats(3, line, ret)) {
			ret.type = Parsed::Vertex;
		} else {
			ret.str = "Could not parse vertex: ";// + line;
			return ret;
		}
	} else if (consume(line, "vn")) {
		if (parseFloats(3, line, ret)) {
			ret.type = Parsed::Normal;
		} else {
			ret.str = "Could not parse normal: ";// + line;
			return ret;
		}
	} else if (consume(line, "vt")) {
		if (parseFloats(3, line, ret)) {
			ret.type = Parsed::Texcoord;
		} else {
			ret.str = "Could not parse texcoord: ";// + line;
			return ret;
		}
	} else if (consume(line, "vp")) {
		if (parseFloats(3, line, ret)) {
			ret.type = Parsed::ParameterSpace;
		} else {
			ret.str = "Could not parse parameter space: ";// + line;
			return ret;
		}
	} else if (consume(line, "f")) {
		// Currently just supports triangles
		Either<v3i, Error> res = FaceParseError;

		if (!(res = parseFace(line))) {
			ret.type = Parsed::Error;
			ret.str = "Failed to parse first face in line ";// + line;
			return ret;
		}
		ret.f1 = *res;

		if (!(res = parseFace(line))) {
			ret.type = Parsed::Error;
			ret.str = "Failed to parse second face in line ";// + line;
			return ret;
		}
		ret.f2 = *res;

		if (!(res = parseFace(line))) {
			ret.type = Parsed::Error;
			ret.str = "Failed to parse third face in line ";// + line;
			return ret;
		}
		ret.f3 = *res;
		ret.type = Parsed::FaceIndices;
	} else if (line == "") {
		ret.type = Parsed::Blank;
	} else if (consume(line, "mtllib")) {
		std::string path = parseWord(line);
		ret.type = Parsed::MtlLib;
		ret.str = path.c_str();
	} else if (consume(line, "usemtl")) {
		std::string mtl = parseWord(line);
		ret.type = Parsed::UseMtl;
		ret.str = mtl.c_str();
	} else if (consume(line, "g")) {
		std::string groupName = parseWord(line);
		ret.type = Parsed::Group;
		ret.str = groupName.c_str();
	}

	if (ret.type == Parsed::Error)
		ret.str = "Failed to parse line '";// + line + "'";

	return ret;
}
}

//Either<Mesh*, std::string> importOBJ(std::string name, LPCWSTR filepath) {
std::unique_ptr<MeshData> importOBJ(std::string name, const char* filepath) {
	using namespace OBJ;
	std::ifstream file(filepath, std::ios_base::in);
	assrt(file.is_open(), "Failed to open file %ls"); //, filepath);
	std::string line;

	std::vector<Parsed> parsed;
	while (std::getline(file, line)) {
		Parsed res = parseLine(line);
		if (res.type == Parsed::Error) {
			debug_print(LogLevel::BREAK_IF_DEBUGGING, "Error while parsing OBJ file %s. '%s'."); //, filepath, res.str.s);
		}
		parsed.push_back(res);
	}

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> uvs;
	std::vector<v3i> splitIndices;

	std::string mtlLib;
	std::string usemtl;
	for (int i = 0; i < parsed.size(); ++i) {
		Parsed& p = parsed[i];
		if (p.type == Parsed::Blank || p.type == Parsed::Comment)
			continue;
		else if (p.type == Parsed::Group)
			continue; // ignoring groups for now
		else if (p.type == Parsed::MtlLib)
			mtlLib = p.str;
		else if (p.type == Parsed::UseMtl)
			usemtl = p.str;
		else if (p.type == Parsed::Vertex)
			positions.push_back({p.elements.x, p.elements.y, p.elements.z});
		else if (p.type == Parsed::Normal)
			normals.push_back({p.elements.x, p.elements.y, p.elements.z});
		else if (p.type == Parsed::Texcoord)
			uvs.push_back({p.elements.x, p.elements.y});
		else if (p.type == Parsed::FaceIndices) {
			splitIndices.push_back(p.f1);
			splitIndices.push_back(p.f2);
			splitIndices.push_back(p.f3);
		} else {
			debug_print(LogLevel::PRINT, "Error: unsupported line type used: "); // + p.type);)
			return nullptr;
		}
	}

	// these will get copied but whatever
	std::vector<MeshVert> verts;
	std::vector<u32> indices;
	for (int i = 0; i < splitIndices.size(); i += 1) {
		XMFLOAT3 pos = positions[splitIndices[i].x - 1];
		XMFLOAT2 pos_2d = {pos.x,pos.y};
		XMFLOAT2 tc = uvs[splitIndices[i].y - 1];
		tc.y = 1 - tc.y;
		//XMFLOAT3 norm = normals[splitIndices[i].z - 1];

		size_t index_u64 = verts.size();
		assrt(index_u64 <= UINT_MAX, "Too many verts to fit in u32 indices!");
		u32 index = (u32)index_u64;
		verts.push_back({pos_2d, tc});
		indices.push_back(index);
	}

	// TODO: import and use textures from mtl definition

	return std::make_unique<MeshData>(std::move(verts), std::move(indices));
}
