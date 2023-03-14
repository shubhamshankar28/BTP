from gluoncv import model_zoo, data, utils
from matplotlib import pyplot as plt
import os
import cv2
import sys

if (len(sys.argv) != 3):
    print("usage error")


location = sys.argv[1] + "/rgb"
dataset = sys.argv[2]

net = model_zoo.get_model('yolo3_darknet53_voc', pretrained=True)


l = os.listdir(location)
folder = location + "/"
length = len(l)

newFolder = "../" + dataset + "_objectDetectionResults" + "/"
os.system("mkdir -p " + newFolder)

for i in range(length):
    print("processing image %d out of %d ...." % (i,length))
    x, img = data.transforms.presets.yolo.load_test(folder + l[i])
    class_IDs, scores, bounding_boxs = net(x)
    file = open(newFolder + l[i][:-4] + "_objectDetect","w")
    counter = 0

    y_ = img.shape[0]
    x_ = img.shape[1]

    xscale = 640/x_
    yscale = 480/y_

    for j in range(100):
        if(class_IDs[0][j][0].squeeze().asnumpy().tolist()[0] == 14):
            counter = counter+1

    file.write(str(counter))
    file.write("\n")

    for j in range(100):
        if(class_IDs[0][j][0].squeeze().asnumpy().tolist()[0] == 14):
            # print(scores[0][j][0].squeeze().asnumpy().tolist()[0])
            s = str(scores[0][j][0].squeeze().asnumpy().tolist()[0]) + " "

            for k in range(4):

                if((k%2)==0):
                    factor = xscale
                else:
                    factor = yscale
                s = s + str(bounding_boxs[0][j][k].squeeze().asnumpy().tolist()[0]*factor)
                if(k!=3):
                    s = s + " "
            file.write(s)
            file.write("\n")

