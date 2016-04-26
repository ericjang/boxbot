import matplotlib.pyplot as plt
import ipdb as pdb

def compare(x0,x1):
	fig,arr=plt.subplots(1,2)
	arr[0].imshow(x0,cmap=plt.cm.gray)
	arr[1].imshow(x1,cmap=plt.cm.gray)
	plt.show()