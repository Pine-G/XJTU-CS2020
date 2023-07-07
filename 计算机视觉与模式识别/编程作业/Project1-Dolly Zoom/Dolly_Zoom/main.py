def compute_focal_length(d_ref, f_ref, pos):
    """
    compute camera focal length using given camera position

    Input:
    - d_ref: 1 by 1 double, distance of the object whose size remains constant
    - f_ref: 1 by 1 double, previous camera focal length
    - pos: 1 by n, each element represent camera center position on the z axis.
    Output:
    - f: 1 by n, camera focal length
    """

    # IMPLEMENT HERE
    f = f_ref * (d_ref - pos) / d_ref

    return f


'''
Test
'''
d_ref = 4
f_ref = 400
pos = -5
f = compute_focal_length(d_ref, f_ref, pos)
print(f)


def compute_f_pos(d1_ref, d2_ref, H1, H2, ratio, f_ref):
    """
    compute camera focal length and camera position to achieve certain ratio
    between objects

    Input:
    - d1_ref: distance of the first object
    - d2_ref: distance of the second object
    - H1: height of the first object in physical world
    - H2: height of the second object in physical world
    - ratio: ratio between two objects (h1/h2)
    - f_ref: 1 by 1 double, previous camera focal length
    Output:
    - f: 1 by 1, camera focal length
    - pos: 1 by 1, camera position
    """

    # IMPLEMENT HERE
    pos = (H1 * d2_ref - ratio * H2 * d1_ref) / (H1 - ratio * H2)
    f = f_ref * (d1_ref - pos) / d1_ref

    return f, pos


'''
Test
'''
d1_ref = 4
d2_ref = 20
H1 = 4
H2 = 6
ratio = 2
f_ref = 400
f, pos = compute_f_pos(d1_ref, d2_ref, H1, H2, ratio, f_ref)
print(f, pos)
