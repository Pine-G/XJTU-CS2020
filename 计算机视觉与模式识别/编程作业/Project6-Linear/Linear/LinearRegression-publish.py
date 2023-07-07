# Age Estimation
import numpy as np
import os
from helperP import *


# Loading Data
base_dir = 'DATASET/'

age_train, features_train = prepare_data('train', base_dir)
age_val, features_val = prepare_data('val', base_dir)
_, features_test = prepare_data('test', base_dir)
show_data(base_dir)


# Implement Closed Form Solution
# ```
# Arguments:
#     age          -- numpy array, shape (n, )
#     features     -- numpy array, shape (n, 2048)
# Returns:
#     weights      -- numpy array, (2048, )
#     bias         -- numpy array, (1, )
# ```
def closed_form_solution(age, features):
    # Preprocess
    H = features
    ones = np.ones(len(H))
    H = np.column_stack((ones, H))
    Y = age
    
    ##########################################################################
    # TODO: YOUR CODE HERE
    ##########################################################################

    # calculate the closed form solution
    weights = np.dot(np.dot(np.linalg.inv(np.dot(np.transpose(H), H)), np.transpose(H)), Y)

    # separate the weights and bias
    bias = weights[0]
    weights = weights[1:]
    
    return weights, bias


# Validate
w, b = closed_form_solution(age_train, features_train)
loss, pred = evaluate(w, b, age_val, features_val)
print("Your validate loss is:", round(loss, 3))


# Test and Generate results file
prediction = test(w, b, features_test, 'cfs.txt')
print("Test results has saved to cfs.txt")
print(prediction[:10])


# Implement Gradient descent
# Gradient descent is an optimization algorithm used to minimize some function by iteratively moving in the direction of steepest descent as defined by the negative of the gradient. In machine learning, we use gradient descent to update the parameters of our model.
# 
# ```
# Arguments:
#     age          -- numpy array, label, (n, )
#     feature      -- numpy array, features, (n, 2048)
# Return:
#     weights      -- numpy array, (2048, )
#     bias         -- numpy array, (1, )
# ```
def gradient_descent(age, feature):
    assert len(age) == len(feature)

    # Init weights and bias
    weights = np.random.randn(2048, 1)
    bias = np.random.randn(1, 1)
    
    # Learning rate
    lr = 10e-3

    global features_val, age_val
    best_loss = float('inf')
    best_weights = weights
    best_bias = bias
    m = len(age)
    
    for e in range(epoch):
        ##########################################################################
        # TODO: YOUR CODE HERE
        ##########################################################################

        # forward pass
        age_pred = np.dot(feature, weights) + bias

        # calculate loss
        diff = age_pred - age.reshape(-1, 1)
        
        # calculate gradient
        grad_w = np.dot(np.transpose(feature), diff) / m
        grad_b = np.sum(diff) / m

        # update weights
        weights -= lr * grad_w
        bias -= lr * grad_b
        
        if momentum:
            pass    # You  can also consider the gradient descent with momentum

        loss_val = np.sum(np.square(np.dot(features_val, weights) + bias - age_val.reshape(-1, 1))) / (2 * len(age_val))
        if loss_val < best_loss:
            best_loss = loss_val
            best_weights = weights
            best_bias = bias

    return best_weights, best_bias


# Train and validate
w, b = gradient_descent(age_train, features_train)
loss, pred = evaluate(w, b, age_val, features_val)
print("Your validate score is:", round(loss, 3))


# Test and Generate results file
prediction = test(w, b, features_test, 'gd.txt')
print("Test results has saved to gd.txt")
print(prediction[:10])


# Implement Stochastic Gradient descent
# Stochastic Gradient Descent (SGD) is a simple yet very efficient approach to discriminative learning of linear classifiers under convex loss functions such as (linear) Support Vector Machines and Logistic Regression. Even though SGD has been around in the machine learning community for a long time, it has received a considerable amount of attention just recently in the context of large-scale learning.
# ```
# Arguments:
#     age          -- numpy array, label, (n, )
#     feature      -- numpy array, features, (n, 2048)
# Return:
#     weights      -- numpy array, (2048, )
#     bias         -- numpy array, (1, )
# ```
def stochastic_gradient_descent(age, feature):
    # check the inputs
    assert len(age) == len(feature)
    
    # Set the random seed
    np.random.seed(0)

    # Init weights and bias
    weights = np.random.rand(2048, 1)
    bias = np.random.rand(1, 1)

    # Learning rate
    lr = 10e-5

    # Batch size
    batch_size = 16
 
    # Number of mini-batches
    t = len(age) // batch_size

    global features_val, age_val
    best_loss = float('inf')
    best_weights = weights
    best_bias = bias

    for e in range(epoch_sgd):
        # Shuffle training data
        n = np.random.permutation(len(feature))
        
        for m in range(t):
            # Providing mini batch with fixed batch size of 16
            batch_feature = feature[n[m * batch_size: (m+1) * batch_size]]
            batch_age = age[n[m * batch_size: (m+1) * batch_size]]
            
            ##########################################################################
            # TODO: YOUR CODE HERE
            ##########################################################################

            # forward pass
            age_pred = np.dot(batch_feature, weights) + bias

            # calculate loss
            diff = age_pred - batch_age.reshape(-1, 1)

            # calculate gradient
            grad_w = np.dot(np.transpose(batch_feature), diff) / batch_size
            grad_b = np.sum(diff) / batch_size

            # update weights
            weights -= lr * grad_w
            bias -= lr * grad_b
                
            if momentum:
                pass    # You can also consider the gradient descent with momentum
        
        print('=> epoch:', e + 1, '  Loss:', round(loss, 4))

        loss_val = np.sum(np.square(np.dot(features_val, weights) + bias - age_val.reshape(-1, 1))) / (2 * len(age_val))
        if loss_val < best_loss:
            best_loss = loss_val
            best_weights = weights
            best_bias = bias

    return best_weights, best_bias


# Train and validate
w, b = stochastic_gradient_descent(age_train, features_train)
loss, pred = evaluate(w, b, age_val, features_val)
print("Your validate score is:", round(loss, 3))


# Test and Generate results file
prediction = test(w, b, features_test, 'sgd.txt')
print("Test results has saved to sgd.txt")
print(prediction[:10])
