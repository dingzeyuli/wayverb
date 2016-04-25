#pragma once

#include "surface_owner.h"
#include "vec.h"

#include <map>
#include <vector>

class CuboidBoundary;
class MeshBoundary;
struct Triangle;

struct aiScene;

class SurfaceLoader {
public:
    using size_type = std::vector<Surface>::size_type;

    explicit SurfaceLoader(const std::string& fpath);

    const std::vector<Surface>& get_surfaces() const;
    size_type get_index(const std::string& name) const;

private:
    void add_surface(const std::string& name, const Surface& surface);

    std::vector<Surface> surfaces;
    std::map<std::string, size_type> surface_indices;

    friend class SurfaceOwner;
};

class SceneData : public SurfaceOwner {
public:
    using size_type = std::vector<Surface>::size_type;

    SceneData(const std::string& fpath,
              const std::string& mat_file,
              float scale = 1);
    SceneData(const aiScene* const scene,
              const std::string& mat_file,
              float scale = 1);
    SceneData(const std::vector<Triangle>& triangles,
              const std::vector<cl_float3>& vertices,
              const std::vector<Surface>& surfaces);
    SceneData(std::vector<Triangle>&& triangles,
              std::vector<cl_float3>&& vertices,
              std::vector<Surface>&& surfaces);
    virtual ~SceneData() noexcept = default;

    CuboidBoundary get_aabb() const;
    std::vector<Vec3f> get_converted_vertices() const;
    std::vector<int> get_triangle_indices() const;

    const std::vector<Triangle>& get_triangles() const;
    const std::vector<cl_float3>& get_vertices() const;

private:
    SceneData(const aiScene* const scene,
              const SurfaceLoader& loader,
              float scale = 1);

    struct Contents {
        std::vector<Triangle> triangles;
        std::vector<cl_float3> vertices;
        std::vector<Surface> surfaces;
    };

    explicit SceneData(const Contents& contents);
    explicit SceneData(Contents&& contents);

    static Contents get_contents(const aiScene* const scene,
                                 const SurfaceLoader& loader,
                                 float scale);

    std::vector<Triangle> triangles;
    std::vector<cl_float3> vertices;
};