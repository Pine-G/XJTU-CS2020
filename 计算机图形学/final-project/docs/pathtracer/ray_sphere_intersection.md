---
layout: default
title: Ray Sphere Intersection
permalink: /pathtracer/ray_sphere_intersection
grand_parent: "A3: Pathtracer"
parent: (Task 2) Intersections
usemathjax: true
---

# Ray Sphere Intersection

## Step 2: `Sphere::hit`

The second intersect routine you will implement is the `hit` routine for spheres in `student/shapes.cpp`. 

Down below will be our main diagram that will illustrate this algorithm:

<center><img src="figures\sphere_intersect_diagram.png" style="height:320px"></center>

We will use an algebraic approach to solve for any potential intersection points.

First, similar to `Triangle::hit`, we can express any points that lie on our ray with origin $$\textbf{o}$$ and **normalized** direction $$\textbf{d}$$ as a function of time $$t$$:

$$\textbf{r}(t) = \textbf{o} + t\textbf{d}$$

Next, we need to consider the equation of a sphere. We can think of this as all the points that have a distance of $$r$$ from the center of the sphere:

$$||\textbf{x} - \textbf{c}||^2 - r^2 = 0.$$

Thus, if our ray intersects the sphere, then we know that for some time $$t$$, $$\textbf{x} = \textbf{o} + t\textbf{d}$$ will satisfy the equation of a sphere. To simplify the problem, we will consider doing the intersection in local spherical space, where the center is at $$(0, 0, 0)$$. Thus, we have the following equations:

$$
\begin{align*}
||x - c||^2 - r^2 &= 0 \\
||\textbf{o} + t\textbf{d}||^2 - r^2 &= 0 \\ 
\underbrace{||\textbf{d}||^2}_{a} \cdot t^2 + \underbrace{2 \cdot (\textbf{o} \cdot \textbf{d})}_{b} \cdot t + \underbrace{||\textbf{o}||^2 - r^2}_{c} &= 0 \\
\hline \\
t = \frac{-2 \cdot (\textbf{o} \cdot \textbf{d}) \pm \sqrt{4 \cdot (\textbf{o} \cdot \textbf{d})^2 - 4 \cdot ||\textbf{d}||^2 \cdot (||\textbf{o}||^2 - r^2)}}{2 \cdot ||\textbf{d}||^2}
\end{align*}
$$

Notice how there are potentially two solutions to the quadratic - this makes sense as if we go into the sphere in some direction, then we will go out of the sphere on the other side. Consider what happens when the discriminant is negative or zero, and how to take care of that case.

Once you've implemented both this and ray-triangle intersections, you should be able to render the normals of `cbox.dae` and any other scene that involves spheres.


A few final notes and thoughts:
- Take care **NOT** to use the `Vec3::normalize()` method when computing your
normal vector. You should instead use `Vec3::unit()`, since `Vec3::normalize()`
will actually change the `Vec3` calling object rather than returning a
normalized version.
- Remember that your intersection tests should respect the ray's `dist_bounds`, and that normals should be out-facing.
- A common mistake is to forget to check the case where the first
interesection time $$t_1$$ is out of bounds but the second interesection time $$t_2$$ is
(in which case you should return $$t_2$$).

<center><img src="figures\sphere_specialcase_diagram.png" style="height:220px"></center>
