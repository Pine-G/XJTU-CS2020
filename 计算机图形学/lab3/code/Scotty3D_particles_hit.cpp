// in Scotty3D/src/student/tri_mesh.cpp

Trace Triangle::hit(const Ray &ray) const {

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
	//小球运动方向平行于三角形面片，不可能发生碰撞
	if (denominator == 0) {
		return ret;
	}

	//求解三维向量方程：-s + u*e1 + v*e2 = t*ray.dir（均为点乘）
	float u = -1 * dot(cross(s, e2), ray.dir) / denominator;
	float v = dot(cross(e1, ray.dir), s) / denominator;
	float t = -1 * dot(cross(s, e2), e1) / denominator;

	if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1) {
		if (t >= ray.dist_bounds.x && t <= ray.dist_bounds.y) {
			//发生碰撞
			ret.hit = true;
			ret.distance = t;
			ret.position = (1 - u - v) * v_0.position + u * v_1.position + v * v_2.position;
			ret.normal = ((1 - u - v) * v_0.normal + u * v_1.normal + v * v_2.normal).unit();
		}
	}

	return ret;
}


// in Scotty3D/src/student/particles.cpp

bool Scene_Particles::Particle::update(const PT::Object &scene, float dt, float radius) {

	// TODO(Animation): Task 4

	// Compute the trajectory of this particle for the next dt seconds.
	float tLeft = dt;
	float eps = 1e-3;
	// (1) Build a ray representing the particle's path if it travelled at constant velocity.
	while (tLeft - eps > 0) {
		Ray path(pos, velocity);
		// (2) Intersect the ray with the scene and account for collisions. Be careful when placing
		// collision points using the particle radius. Move the particle to its next position.
		auto intersection = scene.hit(path);
		if (intersection.hit && intersection.distance <= radius)
			velocity = velocity - 2 * dot(velocity, intersection.normal) * intersection.normal;
		// (3) Account for acceleration due to gravity.
		velocity += eps * acceleration;
		pos += eps * velocity;
		// (4) Repeat until the entire time step has been consumed.
		tLeft -= eps;
	}
	// (5) Decrease the particle's age and return whether it should die.
	age -= dt;
	return age > 0;
}