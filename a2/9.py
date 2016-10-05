import math


def enigma(X):
    A = [0] * len(X)
    A[0] = X[0]
    s = X[0] * X[0]
    for i in range(1, len(X)):
        s += X[i] * X[i]
        print(s)
        A[i] = math.sqrt(s)
    return A
