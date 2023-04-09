import sys
sys.path.append('core')

import argparse
import os
import cv2
import glob
import numpy as np
import torch
from PIL import Image

from raft import RAFT
from utils import flow_viz
from utils.utils import InputPadder
import matplotlib.pyplot as plt





DEVICE = 'cpu'

def load_image(imfile):
    img = np.array(Image.open(imfile)).astype(np.uint8)
    img = torch.from_numpy(img).permute(2, 0, 1).float()
    return img[None].to(DEVICE)


internal_counter = 0
def viz(img, flo):
    img = img[0].permute(1,2,0).cpu().numpy()
    flo = flo[0].permute(1,2,0).cpu().numpy()
    
    # map flow to rgb image
    flo = flow_viz.flow_to_image(flo)
    img_flo = np.concatenate([img, flo], axis=0)


    global internal_counter
    # import matplotlib.pyplot as plt
    # plt.imshow(img_flo / 255.0)
    # plt.show()
    print(img_flo.shape)
    newmat = img_flo[:, :, [2,1,0]].copy()
    print(newmat.shape)
    matfin = newmat.reshape((960,640,3))
    cv2.imshow('image', img_flo[:, :, [2,1,0]]/255.0)
    cv2.waitKey(10)
    cv2.imwrite("testpics/" + str(internal_counter)+".png" , matfin/255.0 )
    internal_counter = internal_counter+1

def get_cpu_model(model):
    new_model = dict()
    # get all layer's names from model
    for name in model:
        # create new name and update new model
        new_name = name[7:]
        new_model[new_name] = model[name]
    return new_model

def demo(args):
    
    # model = torch.nn.DataParallel(RAFT(args))
    # model.load_state_dict(torch.load(args.model))

    # model = model.module
    # model.to(DEVICE)
    # model.eval()

    model = RAFT(args)
    # load pretrained weights
    pretrained_weights = torch.load(args.model , map_location = torch.device('cpu'))

    if torch.cuda.is_available():
        device = "cuda"
        # parallel between available GPUs
        model = torch.nn.DataParallel(model)
        # load the pretrained weights into model
        model.load_state_dict(pretrained_weights)
        model.to(device)
    else:
        device = "cpu"
        # change key names for CPU runtime
        pretrained_weights = get_cpu_model(pretrained_weights)
        # load the pretrained weights into model
        model.load_state_dict(pretrained_weights)

    newFolder = "../../" + args.dataset + "_raftResults" + "/"
    os.system("mkdir -p " + newFolder)
    print("done")
    counter = 0
    # file = open("/home/cse/segmentationTest/raftFlowResults/" + l[i][:-4] + "_objectDetect","w")
    with torch.no_grad():
        images = glob.glob(os.path.join(args.path, '*.png')) + \
                 glob.glob(os.path.join(args.path, '*.jpg'))
        
        images = sorted(images)
        for imfile1, imfile2 in zip(images[:-1], images[1:]):
            print(counter)
            image1 = load_image(imfile1)
            image2 = load_image(imfile2)

            padder = InputPadder(image1.shape)
            image1, image2 = padder.pad(image1, image2)

            flow_low, flow_up = model(image1, image2, iters=20, test_mode=True)

            
            nflow_up = flow_up[0][0].numpy().flatten()
            # plt.plot(nflow_up.flatten())
            # plt.show()
            nflow_up_a = flow_up[0][1].numpy().flatten()
            # print(np.median(nflow_up) , np.max(nflow_up) , np.min(nflow_up))
            # print(np.median(nflow_up_a) , np.max(nflow_up_a) , np.min(nflow_up_a))


            # if(counter == 0):
            #     file = open("/home/cse/segmentationTest/raftFlowResults/" + imfile1[-21:-4] + "_flow","w")
            #     file.write("-1\n")

            file = open(newFolder + imfile2[-21:-4] + "_flow","w")
            
            file.write(str(np.median(nflow_up)) + " " + str(np.median(nflow_up_a)))
            file.write("\n")
            # print("writing to " + "/home/cse/segmentationTest/raftFlowResults_walking_rpy/" + imfile2[-21:-4] + "_flow" )
            for j in range(480):
                for k in range(640):
                    # print(flow_up[0][0][j][k].numpy().tolist())
                    file.write(str(flow_up[0][0][j][k].numpy().tolist()))
                    file.write(" ")
                    file.write(str(flow_up[0][1][j][k].numpy().tolist()))
                    if( k != 639):
                        file.write(" ")
                
                if(j != 479):
                    file.write("\n")

            # print("first image : " , imfile1)
            # print("second image : ", imfile2)
            # print(flow_up[0][0][20][10], flow_up[0][1][20][10])
            # print(flow_up[0][0][45][557] , flow_up[0][1][45][557])
            
            # print(flow_up[0][0][232][524] , flow_up[0][1][232][524])
            # print(flow_up[0][0][340][480] , flow_up[0][1][340][480])
            # print(flow_up[0][0][226][526] , flow_up[0][1][226][526])
            # print(flow_up.shape)
            # viz(image1, flow_up)
            counter = counter+1

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--model', help="restore checkpoint")
    parser.add_argument('--path', help="dataset for evaluation")
    parser.add_argument('--small', action='store_true', help='use small model')
    parser.add_argument('--mixed_precision', action='store_true', help='use mixed precision')
    parser.add_argument('--alternate_corr', action='store_true', help='use efficent correlation implementation')
    parser.add_argument('--dataset' , help="name of the dataset")
    args = parser.parse_args()

    demo(args)
