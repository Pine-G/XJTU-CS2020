# Linear Perceptron
import numpy as np
import glob
from helperP import *


# Define linear perceptron class
class PrimalPerceptron(object):
	def __init__(self, x, y, w=None, b=None):
		num_sample, num_dims = x.shape
		np.random.seed(0)
		####################################
		# TODO: YOUR CODE HERE： init weights
		####################################
		if not w:
			w = np.random.rand(1, num_dims)
		if not b:
			b = np.random.rand(1)
		self.x, self.y, self.w, self.b = x, y, w, b
		self.lr = 0.1
		
	def predict(self):
		####################################
		# TODO: YOUR CODE HERE, forward
		####################################
		preds = np.dot(self.x, np.transpose(self.w)) + self.b
		y_hat = np.sign(preds)
		return preds, y_hat

	def update(self):
		####################################
		# TODO: YOUR CODE HERE, backward
		####################################
		# update the weights and bias
		preds, y_hat = self.predict()
		for i in range(self.x.shape[0]):
			if y_hat[i, 0] * self.y[i, 0] < 0:
				self.w += self.lr * self.y[i, 0] * self.x[i, :]
				self.b += self.lr * self.y[i, 0]
		return


# ### Train linear perceptron
if __name__ == '__main__':
	# Load data
	file_names = glob.glob('DATASET/data_emoji/*/*.*')
	reduced, images, labels = load_image(file_names)

	reduced = reduced.reshape(reduced.shape[0], -1)
	# Instantiate Perceptron
	p = PrimalPerceptron(reduced, labels)
	# Iterate over data and update

	plt.rcParams["figure.figsize"] = [20, 20]
	for i in range(100):
		p.update()
		preds, y_hat = p.predict()
		visualize_results(images, preds, labels, None)
