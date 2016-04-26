import tensorflow as tf

h,w=240,320
batch_size=64
in_channels=200
out_channels=3 # desired number of channels (i.e. RGB)

x=tf.placeholder(tf.float32,(batch_size,in_channels))
x=tf.reshape(x,(batch_size,1,1,in_channels))

kernel=tf.get_variable('weights',[3,4, out_channels, in_channels],initializer=tf.truncated_normal_initializer(stddev=1e-4))
conv = tf.nn.deconv2d(x, kernel, [batch_size, h, w, out_channels], [1, 1, 1, 1], padding='SAME')

print(conv.get_shape()) # TensorShape([Dimension(64), Dimension(240), Dimension(320), Dimension(3)])