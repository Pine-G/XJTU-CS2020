
#include "../rays/bsdf.h"
#include "../util/rand.h"

namespace PT {

static Vec3 reflect(Vec3 dir) {

    // TODO (PathTracer): Task 5
    // Return reflection of dir about the surface normal (0,1,0).

    return Vec3(-dir.x, dir.y, -dir.z);
}

static Vec3 refract(Vec3 out_dir, float index_of_refraction, bool& was_internal) {

    // TODO (PathTracer): Task 5
    // Use Snell's Law to refract out_dir through the surface.
    // Return the refracted direction. Set was_internal to true if
    // refraction does not occur due to total internal reflection,
    // and false otherwise.

    // When dot(out_dir,normal=(0,1,0)) is positive, then out_dir corresponds to a
    // ray exiting the surface into vaccum (ior = 1). However, note that
    // you should actually treat this case as _entering_ the surface, because
    // you want to compute the 'input' direction that would cause this output,
    // and to do so you can simply find the direction that out_dir would refract
    // _to_, as refraction is symmetric.

    was_internal = false;

    float ni = 1.0f, nt = index_of_refraction;
    if(out_dir.y > 0) {
        ni = index_of_refraction;
        nt = 1.0f;
    }

    float cos2_theta_t = out_dir.y * out_dir.y;
    float sin2_theta_t = 1 - cos2_theta_t;
    float sin2_theta_i = (nt / ni) * (nt / ni) * sin2_theta_t;
    float cos2_theta_i = 1 - sin2_theta_i;

    if(cos2_theta_i < 0) {
        //È«·´Éä
        was_internal = true;
        return Vec3{};
    }

    Vec3 in_dir(-out_dir.x, 0.0f, -out_dir.z);
    in_dir.unit();
    in_dir *= sqrt(sin2_theta_i);
    in_dir.y = sqrt(cos2_theta_i);
    if(out_dir.y > 0) in_dir.y = -in_dir.y;

    return in_dir;
}

Scatter BSDF_Lambertian::scatter(Vec3 out_dir) const {

    // TODO (PathTracer): Task 4

    // Sample the BSDF distribution using the cosine-weighted hemisphere sampler.
    // You can use BSDF_Lambertian::evaluate() to compute attenuation.

    Scatter ret;
    ret.direction = sampler.sample();
    ret.attenuation = evaluate(out_dir, ret.direction);
    return ret;
}

Spectrum BSDF_Lambertian::evaluate(Vec3 out_dir, Vec3 in_dir) const {

    // TODO (PathTracer): Task 4

    // Compute the ratio of reflected/incoming radiance when light from in_dir
    // is reflected through out_dir: albedo * cos(theta).

    float cosTheta = in_dir.y;
    return albedo * cosTheta;
}

float BSDF_Lambertian::pdf(Vec3 out_dir, Vec3 in_dir) const {

    // TODO (PathTracer): Task 4

    // Compute the PDF for sampling in_dir from the cosine-weighted hemisphere distribution.
    
    float cosTheta = in_dir.y;
    return cosTheta / PI_F;
}

Scatter BSDF_Mirror::scatter(Vec3 out_dir) const {

    // TODO (PathTracer): Task 5

    Scatter ret;
    ret.direction = reflect(out_dir);
    ret.attenuation = reflectance;
    return ret;
}

Scatter BSDF_Glass::scatter(Vec3 out_dir) const {

    // TODO (PathTracer): Task 5

    // (1) Compute Fresnel coefficient. Tip: Schlick's approximation.
    // (2) Reflect or refract probabilistically based on Fresnel coefficient. Tip: RNG::coin_flip
    // (3) Compute attenuation based on reflectance or transmittance

    // Be wary of your eta1/eta2 ratio - are you entering or leaving the surface?
    // What happens upon total internal reflection?

    Scatter ret;
    bool was_internal;
    Vec3 in_dir = refract(out_dir, index_of_refraction, was_internal);

    if(was_internal) {
        ret.direction = reflect(out_dir);
        ret.attenuation = reflectance;
    } else {
        float ni = 1.0f, nt = index_of_refraction;
        if(out_dir.y > 0) {
            ni = index_of_refraction;
            nt = 1.0f;
        }

        float cosTheta = fabsf(in_dir.y);
        float R0 = (float)pow((ni - nt) / (ni + nt), 2);
        float Fr = R0 + (1.0f - R0) * (float)pow((1 - cosTheta), 5);

        if(RNG::coin_flip(Fr)) {
            ret.direction = reflect(out_dir);
            ret.attenuation = reflectance;
        } else {
            ret.direction = in_dir;
            ret.attenuation = transmittance;
        }
    }

    return ret;
}

Scatter BSDF_Refract::scatter(Vec3 out_dir) const {

    // OPTIONAL (PathTracer): Task 5

    // When debugging BSDF_Glass, it may be useful to compare to a pure-refraction BSDF

    Scatter ret;
    bool was_internal;
    ret.direction = refract(out_dir, index_of_refraction, was_internal);
    ret.attenuation = was_internal ? Spectrum() : transmittance;

    return ret;
}

Spectrum BSDF_Diffuse::emissive() const {
    return radiance;
}

} // namespace PT
