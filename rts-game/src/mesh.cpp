#include "mesh.h"

#include "common.h"
#include "ofbx.h"
#include "renderer/renderer.h"

const char* mesh_resource_path = "../resources/models";

Mesh::Mesh(const char* name) {
	load(name);
}

HRESULT Mesh::load(const char* name) {
	char path[1024];
	snprintf(path, 1024, "%s/%s", mesh_resource_path, name);
	FILE* fp;
	size_t size;
	if (!(fp = fopen(path, "r")))
		debug_print(LogLevel::FATAL, "Could not find model file to load");

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	u8* contents = (u8*)malloc(size);
	if (!contents)
		debug_print(LogLevel::FATAL, "Malloc failed");

	if (!size == fread(contents, 1, size, fp))
		debug_print(LogLevel::FATAL, "Did not read enough bytes");


	ofbx::IScene* scene = ofbx::load(contents, size, 0u);
	if (!scene)
		debug_print(LogLevel::FATAL, "Failed to load scene");

	u32 num_meshes = scene->getMeshCount();

	for (u32 i = 0; i < num_meshes; ++i) {
		const ofbx::Mesh* mesh = scene->getMesh(i);
		const ofbx::GeometryData& gd = mesh->getGeometryData();
		const ofbx::Vec3Attributes positions = gd.getPositions();
		assrt(positions.count < USHRT_MAX, "Too many indices for this mesh section. Use u32.");
		assrt(positions.indices, "Mesh does not use indices. Implement non-indexed mesh support.");

		u16* indices = (u16*)malloc(sizeof(u16)*positions.count);
		MeshVert* verts = (MeshVert*)malloc(sizeof(MeshVert)*positions.values_count);

		for (int j = 0; j < positions.count; ++j)
			indices[j] = positions.indices[j];
		for (int j = 0; j < positions.values_count; ++j) {
			ofbx::Vec3 pos = positions.values[j];
			verts[j].pos.x = pos.x;
			verts[j].pos.y = pos.y;
			verts[j].pos.z = pos.z;
		}

		// vertex buffer
		{
			D3D11_BUFFER_DESC bd{};
			bd.ByteWidth = sizeof(MeshVert) * positions.values_count;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			bd.StructureByteStride = sizeof(MeshVert);

			D3D11_SUBRESOURCE_DATA vert_data{};
			vert_data.pSysMem = verts;
			vert_data.SysMemPitch = sizeof(MeshVert);
			device->CreateBuffer(&bd, &vert_data, &vb);
		}

		// index buffer
		{
			D3D11_BUFFER_DESC bd{};
			bd.ByteWidth = sizeof(u16) * positions.count;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			bd.StructureByteStride = sizeof(u16);

			D3D11_SUBRESOURCE_DATA index_data{};
			index_data.pSysMem = indices;
			device->CreateBuffer(&bd, &index_data, &ib);
		}

		free(verts);
		free(indices);
	}
	scene->destroy();

	fclose(fp);
	free(contents);
	return S_OK;
}