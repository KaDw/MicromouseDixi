import numpy as np
import matplotlib.pyplot as plt

class optimizer:
    L, R, t = [], []  # left/right wheel positiom and time

    def fromCSV(self, filename):
        """ from file in format: L R
        values are in int variable"""
        try:
            data = []
            lastT = 0
            with open(filename) as f:
                for l in f.readlines():
                    s = [x.strip() for x in l.split()]
                    data.append((int(s[0]), int(s[1]), lastT))
                    lastT += 1
            L, R, t = zip(*data)


    def 
