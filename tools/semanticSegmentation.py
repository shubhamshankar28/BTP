from keras_segmentation.pretrained import pspnet_50_ADE_20K , pspnet_101_cityscapes, pspnet_101_voc12
from keras_segmentation.predict import *
import os
import sys




if (len(sys.argv) != 3):
    print("usage error")
# model = pspnet_50_ADE_20K() # load the pretrained model trained on ADE20k dataset

# model = pspnet_101_cityscapes() # load the pretrained model trained on Cityscapes dataset

model = pspnet_101_voc12() # load the pretrained model trained on Pascal VOC 2012 dataset

location = sys.argv[1] + "/rgb"
dataset = sys.argv[2]

# load any of the 3 pretrained models

#out = model.predict_segmentation(
#   inp="1341846313.553992.png",
#    out_fname="out.png"
#)


folder = location + "/"

l = os.listdir(location)
length = len(l)

newFolder = "../" + dataset + "_semanticSegmentationResults" + "/"
os.system("mkdir -p " + newFolder)

length = len(l)
for i in range(length):
	print("processing image %d out of %d ...." % (i,length))
	finFile = newFolder + l[i][:-4] + "_segment" + ".png"
	model.predict_segmentation(inp=folder+l[i], out_fname=finFile)
	# x = predict(model = model1 , inp = folder+'1341846332.222699.png',out_fname = finFile)
	# print(model1.input_height)
	# print(model1.input_width)
	# print(model1.output_height)
	# print(model1.output_width)
	# print(x.shape)
	# print(type(x))

	# for i in range(473):
	# 	print(x[i][0])
	# print(x)