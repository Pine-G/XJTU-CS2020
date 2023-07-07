'''
  File name: helpers.py
  Author: Tarmily Wen
  Date created: Dec. 8, 2019
'''

'''
  File clarification:
    Helpers file that contributes the project
    You can design any helper function in this file to improve algorithm
'''

import numpy as np

def get_gradient_angle(angleDeg):
    discrete = [0, 45, 90, 135, 180, -45, -90, -135, -180]
    dir = min(discrete, key=lambda x:abs(x-angleDeg))

    return dir


def get_edge_angle(a):
    discrete = [0, 45, 90, 135, 180]
    dir = min(discrete, key=lambda x:abs(x-a))

    return dir


def get_discrete_orientation(Ori):
    angle_Ori = np.degrees(Ori)

    get_gradient_angle_vect = np.vectorize(get_gradient_angle)

    discrete_gradient_orientation = get_gradient_angle_vect(angle_Ori)

    get_edge_angle_vect = np.vectorize(get_edge_angle)

    discrete_edge_orientation = get_edge_angle_vect(np.absolute(angle_Ori))

    return discrete_gradient_orientation, discrete_edge_orientation
