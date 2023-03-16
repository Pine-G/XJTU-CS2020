---
layout: default
title: (Task 5) Materials
permalink: /pathtracer/materials
parent: "A3: Pathtracer"
has_children: false
has_toc: false
usemathjax: true
---

# (Task 5) Materials

## Walkthrough

<video width="750" height="500" controls>
    <source src="videos/Task6_Materials.mp4" type="video/mp4">
</video>

Now that you have implemented the ability to sample more complex light paths, it's time to add support for more types of materials. In this task you will add support for two types of specular materials: mirrors and glass. These materials are implemented in `student/bsdf.cpp`.

- In the diagrams below, both `out_dir` and `in_dir` are pointing _away_ from the intersection point. This is so that we can more consistently consider their angles with respect to the surface normal.
- Remember that we are tracing rays _backwards_, from the camera into the scene. This is why the computed scattering direction (`in_dir`) corresponds with the _incoming_ light.

<center><img src="figures\rays_dir.png" style="height:420px"></center>

First, take another at the BSDF interface in `rays/bsdf.h`. There are a number of key methods you should understand in `BSDF`:

- `Scatter scatter(Vec3 out_dir)`: given `out_dir`, generates a random sample for `in_dir`. It returns a `Scatter`, which contains both the sampled `direction` and the `attenuation` for the in/out pair.
- `Spectrum evaluate(Vec3 out_dir, Vec3 in_dir)`: evaluates the BSDF for a given pair of directions. This is only defined for continuous BSDFs.
- `float pdf(Vec3 out_dir, Vec3 in_dir)`: computes the PDF for sampling `in_dir` from the BSDF distribution, given `out_dir`. This is only defined for continuous BSDFs.
- `Spectrum emissive()`: returns emitted light. This is only defined for the diffuse light source BSDF.

To complete the mirror and glass materials, you will only need to implement their `scatter` functions, as they are both discrete BSDFs (i.e. they have a finite number of possible `in_dir`s for any `out_dir`). Additionally, you will want to complete two helper functions:

- `Vec3 reflect(Vec3 dir)`: returns a direction that is the **perfect specular reflection** of `dir` about `{0, 1, 0}`. More detail about specular reflection can be found [here](http://15462.courses.cs.cmu.edu/fall2015/lecture/reflection/slide_028).

- `Vec3 refract(Vec3 out_dir, float index_of_refraction, bool& was_internal)`: returns the ray that results from refracting `out_dir` through the surface according to [Snell's Law](http://15462.courses.cs.cmu.edu/fall2015/lecture/reflection/slide_032). Your implementation should assume that when `in_dir` **enters** the surface (that is, if $$\cos(\theta_t) > 0$$) then the ray was previously travelling in a vacuum (i.e. index of refraction = 1.0). If $$\cos(\theta_t) < 0$$, then `in_dir` is **leaving** the surface and entering a vacuum.

Finally, when working with Snell's law, there is a special case to account for: total internal reflection. This occurs when a ray hits a refractive boundary at an angle greater than the _critical angle_. The critical angle is the incident \theta_i that causes the refracted \theta_t to be >= 90 degrees, hence can produce no real solution to Snell's Law. In this case, you should set `was_internal` to `true`.

<center>
$$\eta_i \sin(\theta_i) = \eta_t\sin(\theta_t)$$
$$\frac{\eta_i\sin(\theta_i)}{\eta_t} = \sin(\theta_t)$$
$$\frac{\eta_i\sin(\theta_i)}{\eta_t}\overset{?}{\geq} 1$$
</center>

<center><img src="figures\bsdf_diagrams.png" style="height:200px"></center>

---

## Step 1: `BSDF_Mirror`

Implement `reflect` and `BSDF_Mirror::scatter()`.

Because discrete BSDFs do not require Monte Carlo integration (we can simply analytically evaluate each possible direction), we do not implement `BSDF::pdf`. Perhaps more interestingly, we also do not require `BSDF::evaluate`. This is because evaluating the BSDF is only necessary when sampling directions from distributions other than the BSDF itself. When the BSDF is discrete, like a perfect mirror, we can assume other distributions never sample the single (infinitesimal) direction at which the BSDF is non-zero.

Therefore, we must update our path tracing procedure in `Pathtracer::sample_(in)direct_lighting`: when the BSDF is discrete (`BSDF::is_discrete`), we are not doing a Monte Carlo estimate, hence should not use `BSDF::pdf`. Instead, simply multiply the scattering attenuation and the incoming light. Note that failing to make this check will cause the invalid BSDF calls to abort.

## Step 2: `BSDF_Glass`

Implement `refract` and `BSDF_Glass::scatter()`.

Glass is a material that can both reflect and transmit light. As discussed in class, the fraction of light that is reflected vs. transmitted is governed by the dielectric (non-conductive) Fresnel equations. To simulate this, we may sample `in_dir` from either reflection or refraction with probability proportional to the Fresnel reflectance. For example, if the Fresnel reflectance is $$0.9$$, then you should generate a reflected ray 90% of the time. Note that instead of computing the full Fresnel equations, you have the option to use [Schlick's approximation](https://en.wikipedia.org/wiki/Schlick's_approximation) instead.

In the description below, $$\eta_i$$ and $$\theta_i$$ refer to the index of refraction of the medium containing the incoming ray and the angle of that ray with respect to the boundary surface normal. $$\eta_t$$ and $$\theta_t$$ refer to the index of refraction of the new medium and the angle to the boundary normal of the transmitted ray.

The Fresnel equations state that reflection from a surface is a function of the surface's index of refraction and the polarity of the incoming light. Since our renderer doesn't account for polarity, we'll apply a common approximation of averaging the reflectance of light polarized in perpendicular and parallel directions:

<center>
$$F_r = \frac{1}{2}(r^2_{\parallel}+r^2_{\perp})$$
</center>

The parallel and perpendicular terms are given by:

<center>
$$r_{\parallel} = \frac{\eta_t\cos(\theta_i) - \eta_i\cos(\theta_t)}{\eta_t\cos(\theta_i)+\eta_i\cos(\theta_t)}$$
$$r_{\perp} = \frac{\eta_i\cos(\theta_i) - \eta_t\cos(\theta_t)}{\eta_i\cos(\theta_i)+\eta_t\cos(\theta_t)}$$
</center>

Therefore, for a dielectric material, the fraction of reflected light will be given by $$F_r$$, and the amount of transmitted light will be given by $$1-F_r$$.

Alternatively, you may compute $$F_r$$ using [Schlick's approximation](https://en.wikipedia.org/wiki/Schlick%27s_approximation).

### Distribution Function for Transmitted Light

Although we described the BRDF for perfect specular reflection in class, we did not discuss the distribution function for transmitted light. Unlike reflection, refraction "spreads" or "condenses" a differential beam of light. Hence, a refraction event should change the radiance along a ray.

After using Snell's Law to find the direction of refracted rays, compute the BSDF attenuation using the distribution function found in Pharr, Jakob, and and Humphries's book [Physically Based Rendering](http://www.pbr-book.org/3ed-2018/Reflection_Models/Specular_Reflection_and_Transmission.html). It also includes a derivation based Snell's Law and the relation $$d\Phi_t = (1-F_r)d\Phi_i$$. Of course, you are more than welcome to attempt a derivation on your own!

---

## Tips

- Check your sphere intersection code, as you may have bugs there that were not exposed by rendering Lambertian spheres in Task 4.
- Check the behavior of your refract function when `index_of_refraction = 1.f`. This should not change the transmitted direction, hence make the glass sphere transparent.
- Test reflection and refraction separately, i.e. ignore the Fresnel coefficient and only refract or reflect. Once you've verified that those are correct, then go ahead and reintroduce the Fresnel coefficient and split rays between reflection  and refraction.

<center><img src="images/cbox_debug.png"></center>

---

## Reference Results

When you are done, you will be able to render images with specular materials, like the Cornell Box with a metal and glass sphere (`cbox.dae`, 1024 samples, max depth 8):

<center><img src="images/cbox.png"></center>

---

## Extra Credit
- Add a more advanced parameterized BSDF, such as Blinn-Phong, GGX, or Disney. More information about evaluating and sampling each of these distributions can be found in [Physically Based Rendering](http://www.pbr-book.org/3ed-2018/) chapters 8 & 9.
- Add textures (either procedural or mapped) defining spatially varying BSDF attributes within an object. Refer to [Physically Based Rendering](http://www.pbr-book.org/3ed-2018/) chapter 10.

