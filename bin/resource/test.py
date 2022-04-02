import cv2 as cv
import os
import numpy as np

for i in os.listdir("./resource/gun/"):
    if not i.endswith(".png"):
        continue
    img = cv.imread("./resource/gun/"+i)
    sum = np.sum(img)
    w,h,d = img.shape
    total = w * h * 3 * 255
    print(i,round(sum/total*100))
