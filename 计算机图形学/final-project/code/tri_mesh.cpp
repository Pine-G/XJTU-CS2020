
#include "../rays/tri_mesh.h"
#include "../rays/samplers.h"

namespace PT {

BBox Triangle::bbox() const {

    // TODO (PathTracer): Task 2 or 3
    // Compute the bounding box of the triangle.

    // Beware of flat/zero-volume boxes! You may need to
    // account for that here, or later on in BBox::hit.

    BBox box;
    return box;
}

Trace Triangle::hit(const Ray& ray) const {

    // Each vertex contains a postion and surface normal
    Tri_Mesh_Vert v_0 = vertex_list[v0];
    Tri_Mesh_Vert v_1 = vertex_list[v1];
    Tri_Mesh_Vert v_2 = vertex_list[v2];

    // TODO (PathTracer): Task 2
    // Intersect the ray with the triangle defined by the three vertices.

    Vec3 e1 = v_1.position - v_0.position;
    Vec3 e2 = v_2.position - v_0.position;
    Vec3 s = ray.point - v_0.position;

    Trace ret;
    ret.origin = ray.point;
    ret.hit = false;
    ret.distance = 0.0f;
    ret.position = Vec3{};
    ret.normal = Vec3{};

    float denominator = dot(cross(e1, ray.dir), e2);
    //粒子运动方向平行于三角形面片，不可能发生碰撞
    if(denominator == 0) return ret;

    //求解三维向量方程：-s + u*e1 + v*e2 = t*ray.dir（均为点乘）
    float u = -1 * dot(cross(s, e2), ray.dir) / denominator;
    float v = dot(cross(e1, ray.dir), s) / denominator;
    float t = -1 * dot(cross(s, e2), e1) / denominator;

    if(u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1) {
        if(t >= ray.dist_bounds.x && t <= ray.dist_bounds.y) {
            //发生碰撞
            ret.hit = true;
            ret.distance = t;
            ret.position = (1 - u - v) * v_0.position + u * v_1.position + v * v_2.position;
            ret.normal = ((1 - u - v) * v_0.normal + u * v_1.normal + v * v_2.normal).unit();
        }
    }

    return ret;
}

Triangle::Triangle(Tri_Mesh_Vert* verts, unsigned int v0, unsigned int v1, unsigned int v2)
    : vertex_list(verts), v0(v0), v1(v1), v2(v2) {
}

Vec3 Triangle::sample(Vec3 from) const {
    Tri_Mesh_Vert v_0 = vertex_list[v0];
    Tri_Mesh_Vert v_1 = vertex_list[v1];
    Tri_Mesh_Vert v_2 = vertex_list[v2];
    Samplers::Triangle sampler(v_0.position, v_1.position, v_2.position);
    Vec3 pos = sampler.sample();
    return (pos - from).unit();
}

float Triangle::pdf(Ray wray, const Mat4& T, const Mat4& iT) const {

    Ray tray = wray;
    tray.transform(iT);

    Trace trace = hit(tray);
    if(trace.hit) {
        trace.transform(T, iT.T());
        Vec3 v_0 = T * vertex_list[v0].position;
        Vec3 v_1 = T * vertex_list[v1].position;
        Vec3 v_2 = T * vertex_list[v2].position;
        float a = 2.0f / cross(v_1 - v_0, v_2 - v_0).norm();
        float g =
            (trace.position - wray.point).norm_squared() / std::abs(dot(trace.normal, wray.dir));
        return a * g;
    }
    return 0.0f;
}

void Tri_Mesh::build(const GL::Mesh& mesh, bool bvh) {

    use_bvh = bvh;
    verts.clear();
    triangle_bvh.clear();
    triangle_list.clear();

    for(const auto& v : mesh.verts()) {
        verts.push_back({v.pos, v.norm});
    }

    const auto& idxs = mesh.indices();

    std::vector<Triangle> tris;
    for(size_t i = 0; i < idxs.size(); i += 3) {
        tris.push_back(Triangle(verts.data(), idxs[i], idxs[i + 1], idxs[i + 2]));
    }

    if(use_bvh) {
        triangle_bvh.build(std::move(tris), 4);
    } else {
        triangle_list = List<Triangle>(std::move(tris));
    }
}

Tri_Mesh::Tri_Mesh(const GL::Mesh& mesh, bool use_bvh) {
    build(mesh, use_bvh);
}

Tri_Mesh Tri_Mesh::copy() const {
    Tri_Mesh ret;
    ret.verts = verts;
    ret.triangle_bvh = triangle_bvh.copy();
    ret.triangle_list = triangle_list.copy();
    ret.use_bvh = use_bvh;
    return ret;
}

BBox Tri_Mesh::bbox() const {
    if(use_bvh) return triangle_bvh.bbox();
    return triangle_list.bbox();
}

Trace Tri_Mesh::hit(const Ray& ray) const {
    if(use_bvh) return triangle_bvh.hit(ray);
    return triangle_list.hit(ray);
}

size_t Tri_Mesh::visualize(GL::Lines& lines, GL::Lines& active, size_t level,
                           const Mat4& trans) const {
    if(use_bvh) return triangle_bvh.visualize(lines, active, level, trans);
    return 0;
}

Vec3 Tri_Mesh::sample(Vec3 from) const {
    if(use_bvh) {
        die("Sampling BVH-based triangle meshes is not yet supported.");
    }
    return triangle_list.sample(from);
}

float Tri_Mesh::pdf(Ray ray, const Mat4& T, const Mat4& iT) const {
    if(use_bvh) {
        die("Sampling BVH-based triangle meshes is not yet supported.");
    }
    return triangle_list.pdf(ray, T, iT);
}

} // namespace PT
