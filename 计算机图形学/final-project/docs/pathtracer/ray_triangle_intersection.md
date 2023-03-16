---
layout: default
title: Ray Triangle Intersection
permalink: /pathtracer/ray_triangle_intersection
grand_parent: "A3: Pathtracer"
nav_order: 1
parent: (Task 2) Intersections
usemathjax: true
---

# Ray Triangle Intersection

## Step 1: `Triangle::hit`

The first intersect routine you will implement is the `hit` routine for triangle meshes in `student/tri_mesh.cpp`.

We recommend that you implement the [*Moller-Trumbore algorithm*](https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection), a fast algorithm
that takes advantage of a barycentric coordinates parameterization of the
intersection point, for ray-triangle intersection. We will provide motivation for the algorithm down below, but feel free to look at other resources for ray-triangle intersections.

Down below will be our main diagram that will illustrate this algorithm:

<center><img src="figures\triangle_intersect_diagram.png" style="height:320px"></center>

First, we can parameterize any point $$\textbf{P}$$ in terms of the vertices $$\textbf{p}_0$$, $$\textbf{p}_1$$ andd $$\textbf{p}_2$$ with barycentric coordinates $$u$$, $$v$$ and $$w$$:

$$\textbf{P} = w \cdot \textbf{p}_0 + u \cdot \textbf{p}_1 + v \cdot \textbf{p}_2.$$

We can simplify this by using the fact that if an intersection point lies within the triangle (which is what we aim to find), then the sum of the barycentric coordinates is equal to $$1$$ and each of the coordinates are non-negative. If any of the coordinates fail to satisfy this property, then we can simply return that there was no intersection. This allows us to simplify the equation from above:

$$
\begin{align*}
\textbf{P} &= w \cdot \textbf{p}_0 + u \cdot \textbf{p}_1 + v \cdot \textbf{p}_2 \\
 & = (1 - u - v) \cdot \textbf{p}_0 + u \cdot \textbf{p}_1 + v \cdot \textbf{p}_2 \\ 
 & = \textbf{p}_0 + u \cdot (\textbf{p}_1 - \textbf{p}_0) + v \cdot (\textbf{p}_2 - \textbf{p}_0) 
\end{align*}
$$

On the other hand, we can express any point on the ray with origin $$\textbf{o}$$ and **normalized** direction $$\textbf{d}$$ as a function of time $$t$$. Thus, if our ray does intersect the triangle, then we can set these two equations equal:

<center>
$$\textbf{o} + t \cdot \textbf{d} = \textbf{p}_0 + u \cdot (\textbf{p}_1 - \textbf{p}_0) + v \cdot (\textbf{p}_2 - \textbf{p}_0)$$
$$\textbf{o} + t \cdot \textbf{d} = \textbf{p}_0 + u \cdot \textbf{e}_1 + v \cdot \textbf{e}_2$$
$$\implies \textbf{o} - \textbf{p}_0 = u \cdot \textbf{e}_1 + v \cdot \textbf{e}_2 + t \cdot (-\textbf{d})$$
$$\implies \begin{bmatrix} \textbf{e}_1 & \textbf{e}_2 & -\textbf{d} \end{bmatrix} \cdot \begin{bmatrix} u \\ v \\ t \end{bmatrix} = \textbf{o} - \textbf{p}_0 = \textbf{s}$$
</center>

Where $$\textbf{s}$$, $$\textbf{e}_1$$ and $$\textbf{e}_2$$ are defined in the initial diagram.

We can now use Cramer's rule where we solve for each variable as the fraction of determinants to solve the above equation:
<center>
$$ \begin{bmatrix} u \\ v \\ t \end{bmatrix} = \frac{1}{(\textbf{e}_1 \times \textbf{d}) \cdot \textbf{e}_2} \cdot \begin{bmatrix} -(\textbf{s} \times \textbf{e}_2) \cdot \textbf{d} \\ (\textbf{e}_1 \times \textbf{d}) \cdot \textbf{s} \\ -(\textbf{s} \times \textbf{e}_2) \cdot \textbf{e}_1 \end{bmatrix}$$
</center>

Once you've successfully implemented triangle intersection, you will be able to render many of the scenes in the media directory. However, your ray tracer will be very slow!

A few final notes and thoughts:
- While you are working with `student/tri_mesh.cpp`, you can choose to implement `Triangle::bbox` as well (pretty straightforward to do), which is needed for task $$3$$.
- If the denominator $$(\textbf{e}_1 \times \textbf{d}) \cdot \textbf{e}_2)$$ is zero, what does that mean about the relationship of the ray and the triangle? Can a triangle with this area be hit by a ray? Given $$u$$ and $$v$$, how do you know if the ray hits the triangle? Don't forget that the intersection point on the ray should be within the ray's `dist_bounds`.
- **Don't** use `abs()`. In gcc, this is the integer-only absolute value function. To get the float version, use `std::abs()` or `fabsf()`.
