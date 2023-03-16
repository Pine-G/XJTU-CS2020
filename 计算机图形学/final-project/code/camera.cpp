
#include "../util/camera.h"
#include "../rays/samplers.h"
#include "debug.h"

Ray Camera::generate_ray(Vec2 screen_coord) const {

    // TODO (PathTracer): Task 1
    // compute the position of the input sensor sample coordinate on the
    // canonical sensor plane one unit away from the pinhole.
    // Tip: Compute the ray direction in camera space and use
    // the camera transform to transform it back into world space.

    float screenH = 2 * tan(vert_fov / 180.0f * PI_F / 2.0f);
    float screenW = screenH * aspect_ratio;

    float x = screenW * (screen_coord.x - 0.5f);
    float y = screenH * (screen_coord.y - 0.5f);

    Vec3 origin(0.0f, 0.0f, 0.0f);
    Vec3 direction(x, y, -1.0f);

    Ray ray(origin, direction.unit());
    ray.transform(iview);

    return ray;
}
