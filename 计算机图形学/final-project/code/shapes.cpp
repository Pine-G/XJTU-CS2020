
#include "../rays/shapes.h"
#include "debug.h"

namespace PT {

const char* Shape_Type_Names[(int)Shape_Type::count] = {"None", "Sphere"};

BBox Sphere::bbox() const {

    BBox box;
    box.enclose(Vec3(-radius));
    box.enclose(Vec3(radius));
    return box;
}

Trace Sphere::hit(const Ray& ray) const {

    // TODO (PathTracer): Task 2
    // Intersect this ray with a sphere of radius Sphere::radius centered at the origin.

    // If the ray intersects the sphere twice, ret should
    // represent the first intersection, but remember to respect
    // ray.dist_bounds! For example, if there are two intersections,
    // but only the _later_ one is within ray.dist_bounds, you should
    // return that one!

    Trace ret;
    ret.origin = ray.point;
    ret.hit = false;
    ret.distance = 0.0f;
    ret.position = Vec3{};
    ret.normal = Vec3{};

    //���η���ϵ��
    float a = dot(ray.dir, ray.dir);
    float b = 2 * dot(ray.dir, ray.point);
    float c = dot(ray.point, ray.point) - radius * radius;

    float delta = b * b - 4 * a * c;
    //���η����޽�
    if(delta < 0) return ret;

    //�����η���
    float t1 = (-b + sqrt(delta)) / (2 * a);
    float t2 = (-b - sqrt(delta)) / (2 * a);
    float tmin = (t1 < t2) ? t1 : t2;
    float tmax = (t1 > t2) ? t1 : t2;

    //���㲻�����߷�Χ��
    if(tmax < ray.dist_bounds.x || tmin > ray.dist_bounds.y || (tmin < ray.dist_bounds.x && tmax > ray.dist_bounds.y))
        return ret;

    //������ײ
    ret.hit = true;
    ret.distance = (tmin < ray.dist_bounds.x) ? tmax : tmin;
    ret.position = ray.point + ray.dir * ret.distance;
    ret.normal = ret.position.unit();

    return ret;
}

} // namespace PT
