#!/usr/bin/env python
import numpy as np
from sklearn import svm 
#@from sklearn.svm import LinearSVC 
f = open("./data2.txt")
f.readline()  # skip the header
data = np.loadtxt(f)

X = data[:, 1:]  # select columns 1 through end
y = data[:, 0]   # select column 0, the stock price

c=svm.LinearSVC()
c.fit(X,y)


def get_beh_svm_result(list):
	return int(c.predict(list)[0])



if __name__ == "__main__":
	new=[0,0,0,0,0,0,1,0,0,0,0,0,0,0,0]
	print get_beh_svm_result(new)
	

