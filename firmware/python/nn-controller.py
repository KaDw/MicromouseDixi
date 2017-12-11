import tensorflow as tf
import matplotlib.pyplot as plt
import numpy as np
import common
import SLAM


def build_nn(size=(1, 2)):
    X = tf.placeholder(tf.float, shape=[None, size[0]])
    Y = tf.placeholder(tf.float, shape=[None, size[-1]])

    layers =[X]

    last_in = X
    last_n = size[0]
    for n in size[1:-1]:
        w = tf.Variable(tf.random_normal([last_n, n]))
        b = tf.Variable(tf.random_normal([last_n, 1]))
        V = tf.nn.relu(tf.matmul(last_in*w+b))
        layers.append[w*last_n+b]

    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=Y, logits=layers[-1]))
    updates = tf.train.GradientDescentOptimizer(0.01).minimize(cost)

    return updates, X, Y


def learn(net, Xtrain, iter, Xtest=None):
    updates_fun, X, Y = net
    sesion = tf.Session()
    init = tf.global_variables_initializer()
    sesion.run(init)

    for i in range(iter):
        for x in range(len(Xtrain)):
            sesion.run(updates_fun, feed_dict={X: Xtrain[i: i+1]})

        train_accuracy = np.mean(np.argmax)

        if Xtest is None:
            print("iter {}, train accuracy = {:.2f}".format(i+1, 100.0*))