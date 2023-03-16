/*
    This method should flip the given edge counter-clockwise and return an iterator to the
    flipped edge.
*/

std::optional<Halfedge_Mesh::EdgeRef> Halfedge_Mesh::flip_edge(Halfedge_Mesh::EdgeRef e) {

	/*
	    ע����߷���Ϊ˳ʱ��

	                   v1                           v1
	                  /|\                           /\
	                 / | \                         /  \
	                /  |  \                       /    \
	               /  ||   \                     /<--h--\
	           v3 /  h||    \ v4    =====>   v3 /________\ v4
	              \   || f2 /                   \        /
	               \ \||   /                     \  f2  /
	                \  |  /                       \    /
	                 \ | /                         \  /
	                  \|/                           \/
	                   v2                           v2

	                 ��תǰ                       ��ת��
	*/

	//���
	HalfedgeRef h = e->halfedge();
	HalfedgeRef h_twin = h->twin();
	HalfedgeRef h_2_3 = h->next();
	HalfedgeRef h_3_1 = h_2_3->next();
	HalfedgeRef h_1_4 = h_twin->next();
	HalfedgeRef h_4_2 = h_1_4->next();

	//����
	VertexRef v1 = h->vertex();
	VertexRef v2 = h_twin->vertex();
	VertexRef v3 = h_3_1->vertex();
	VertexRef v4 = h_4_2->vertex();

	//��
	FaceRef f1 = h->face();
	FaceRef f2 = h_twin->face();

	//�������Ӹ�����Ԫ��
	h->set_neighbors(h_3_1, h_twin, v4, e, f1);
	h_twin->set_neighbors(h_4_2, h, v3, e, f2);
	h_3_1->set_neighbors(h_1_4, h_3_1->twin(), v3, h_3_1->edge(), f1);
	h_1_4->set_neighbors(h, h_1_4->twin(), v1, h_1_4->edge(), f1);
	h_4_2->set_neighbors(h_2_3, h_4_2->twin(), v4, h_4_2->edge(), f2);
	h_2_3->set_neighbors(h_twin, h_2_3->twin(), v2, h_2_3->edge(), f2);

	//���ܴ�������һ�������
	//��תǰ��f2��Ӧ�İ��Ϊh_1_4������ת��h_1_4������f1
	//�ٱ��磺��תǰ��v1��Ӧ�İ��Ϊh������ת��h��������v1
	//����Ҫ����Ӧ�ĵ�����޸İ��
	f1->halfedge() = h;
	f2->halfedge() = h_twin;

	v1->halfedge() = h_1_4;
	v2->halfedge() = h_2_3;

	return e;
}

/*
    This method should split the given edge and return an iterator to the
    newly inserted vertex. The halfedge of this vertex should point along
    the edge that was split, rather than the new edges.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::split_edge(Halfedge_Mesh::EdgeRef e) {

	/*
	    ע����߷���Ϊ˳ʱ�룬h = h_1_3

	                   v4
	                  /|\
	                 / | \
	                /  |  \
	               /  ||   \
	              /   ||    \
	             /  h4||     \
	            /     ||  f1  \
	           /     \||       \
	          /  f4    | <-h1-- \
	      v3 /_________|_________\ v1
	         \  --h3-> |v0       /
	          \        |   f2   /
	           \       ||\     /
	            \  f3  ||     /
	             \     ||h2  /
	              \    ||   /
	               \   ||  /
	                \  |  /
	                 \ | /
	                  \|/
	                   v2

	*/

	//���
	HalfedgeRef h = e->halfedge();
	HalfedgeRef h_twin = h->twin();
	HalfedgeRef h_1_2 = h_twin->next();
	HalfedgeRef h_2_3 = h_1_2->next();
	HalfedgeRef h_3_4 = h->next();
	HalfedgeRef h_4_1 = h_3_4->next();

	HalfedgeRef h1 = h;
	HalfedgeRef h1_twin = h_twin;
	HalfedgeRef h2 = new_halfedge();
	HalfedgeRef h2_twin = new_halfedge();
	HalfedgeRef h3 = new_halfedge();
	HalfedgeRef h3_twin = new_halfedge();
	HalfedgeRef h4 = new_halfedge();
	HalfedgeRef h4_twin = new_halfedge();

	//����
	VertexRef v1 = h_1_2->vertex();
	VertexRef v2 = h_2_3->vertex();
	VertexRef v3 = h_3_4->vertex();
	VertexRef v4 = h_4_1->vertex();

	VertexRef v0 = new_vertex();

	//��
	FaceRef f1 = h->face();
	FaceRef f2 = h_twin->face();
	FaceRef f3 = new_face();
	FaceRef f4 = new_face();

	//��
	EdgeRef e1 = e;
	EdgeRef e2 = new_edge();
	EdgeRef e3 = new_edge();
	EdgeRef e4 = new_edge();

	//����
	h1->set_neighbors(h4_twin, h1_twin, v1, e1, f1);
	h1_twin->set_neighbors(h_1_2, h1, v0, e1, f2);

	h2->set_neighbors(h1_twin, h2_twin, v2, e2, f2);
	h2_twin->set_neighbors(h_2_3, h2, v0, e2, f3);

	h3->set_neighbors(h2_twin, h3_twin, v3, e3, f3);
	h3_twin->set_neighbors(h_3_4, h3, v0, e3, f4);

	h4->set_neighbors(h3_twin, h4_twin, v4, e4, f4);
	h4_twin->set_neighbors(h_4_1, h4, v0, e4, f1);

	h_1_2->set_neighbors(h2, h_1_2->twin(), v1, h_1_2->edge(), f2);
	h_2_3->set_neighbors(h3, h_2_3->twin(), v2, h_2_3->edge(), f3);
	h_3_4->set_neighbors(h4, h_3_4->twin(), v3, h_3_4->edge(), f4);
	h_4_1->set_neighbors(h1, h_4_1->twin(), v4, h_4_1->edge(), f1);

	v1->halfedge() = h1;
	v2->halfedge() = h2;
	v3->halfedge() = h3;
	v4->halfedge() = h4;

	e1->halfedge() = h1;
	e2->halfedge() = h2;
	e3->halfedge() = h3;
	e4->halfedge() = h4;

	//ֻ���ܶ�e2��e4ִ��flip��������ֻ�轫������������Ϊ�±�
	e2->is_new = true;
	e4->is_new = true;

	f1->halfedge() = h1;
	f2->halfedge() = h2;
	f3->halfedge() = h3;
	f4->halfedge() = h4;

	v0->pos = (v1->pos + v3->pos) / 2;
	v0->is_new = true;
	v0->halfedge() = h3_twin;

	return v0;
}

/*
    This routine should increase the number of triangles in the mesh
    using Loop subdivision. Note: this is will only be called on triangle meshes.
*/
void Halfedge_Mesh::loop_subdivide() {

	// Each vertex and edge of the original mesh can be associated with a
	// vertex in the new (subdivided) mesh.
	// Therefore, our strategy for computing the subdivided vertex locations is to
	// *first* compute the new positions
	// using the connectivity of the original (coarse) mesh. Navigating this mesh
	// will be much easier than navigating
	// the new subdivided (fine) mesh, which has more elements to traverse.  We
	// will then assign vertex positions in
	// the new mesh based on the values we computed for the original mesh.

	// Compute new positions for all the vertices in the input mesh using
	// the Loop subdivision rule and store them in Vertex::new_pos.
	//    At this point, we also want to mark each vertex as being a vertex of the
	//    original mesh. Use Vertex::is_new for this.

	//float PI = 3.1415926;
	for (auto v = vertices_begin(); v != vertices_end(); ++v) {

		//����һ������������ڽӶ���
		int n = 0;
		Vec3 v_sum = Vec3(0, 0, 0);
		HalfedgeRef h = v->halfedge();
		do {
			v_sum += h->twin()->vertex()->pos;
			++n;
			h = h->twin()->next();
		} while (h != v->halfedge());

		//�����ڲ�����
		//float u = (5.0f / 8.0f - (float)pow(3.0f / 8.0f + 1.0f / 4.0f * cos(2.0f * PI / n), 2)) / n;
		float u = (n == 3) ? (3.0f / 16.0f) : (3.0f / 8.0f / n);
		v->new_pos = (1 - n * u) * v->pos + u * v_sum;
		v->is_new = false;
	}

	// Next, compute the subdivided vertex positions associated with edges, and
	// store them in Edge::new_pos.
	for (auto e = edges_begin(); e != edges_end(); ++e) {
		//�����ڲ��߽綥��
		Vec3 Q1 = e->halfedge()->vertex()->pos;
		Vec3 Q2 = e->halfedge()->twin()->vertex()->pos;
		Vec3 Q3 = e->halfedge()->next()->next()->vertex()->pos;
		Vec3 Q4 = e->halfedge()->twin()->next()->next()->vertex()->pos;
		e->new_pos = (3.0 * (Q1 + Q2) + Q3 + Q4) / 8.0;
		e->is_new = false;
	}

	// Next, we're going to split every edge in the mesh, in any order.
	// We're also going to distinguish subdivided edges that came from splitting
	// an edge in the original mesh from new edges by setting the boolean Edge::is_new.
	// Note that in this loop, we only want to iterate over edges of the original mesh.
	// Otherwise, we'll end up splitting edges that we just split (and the
	// loop will never end!)
	size_t originalEs = edges.size();
	EdgeRef e = edges.begin();

	for (size_t i = 0; i < originalEs; ++i) {
		VertexRef v = *split_edge(e);
		v->new_pos = e->new_pos;
		++e;
	}

	// Now flip any new edge that connects an old and new vertex.
	for (auto edge = edges_begin(); edge != edges_end(); ++edge) {
		if (!(edge->is_new))
			continue;
		if (edge->halfedge()->vertex()->is_new + edge->halfedge()->twin()->vertex()->is_new != 1)
			continue;
		flip_edge(edge);
	}

	// Finally, copy new vertex positions into the Vertex::pos.
	for (auto vertex = vertices_begin(); vertex != vertices_end(); ++vertex) {
		vertex->pos = vertex->new_pos;
	}
}