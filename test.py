import sys
#sys.path.insert(0, '/usr/lib/python2.7/dist-packages')
import cv2
import imutils
import numpy
import time
from Queue import Queue
import urllib
import threading
from threading import Thread, Event, ThreadError

#print sys.path

searchMode = 0
forward = 1
forward_left = 2
forward_right = 3
left = 4
right = 5
bananaDance = 6

def printData(data):
    return data*data

def openCV(data):

    cap = cv2.VideoCapture('http://192.168.1.130:8080/videoView')
    #cap = cv2.VideoCapture('http://192.168.8.148:8080/videoView')
    ret, frame = cap.read()
    frame = imutils.rotate(frame, -90)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    lower_range = numpy.array([20,100,100])
    upper_range = numpy.array([30, 255, 255])

    mask = cv2.inRange(hsv, lower_range, upper_range)

    result = cv2.bitwise_and(frame, frame, mask=mask)

    blurred = cv2.GaussianBlur(mask, (5, 5), 0)
    thresh = cv2.threshold(blurred, 60, 255, cv2.THRESH_BINARY)[1]
    cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cnts = cnts[0] if imutils.is_cv2() else cnts[1]

    area2 = 0
    for c in cnts:
        M = cv2.moments(c)
        area = cv2.contourArea(c)
        if(M["m00"] != 0 and area > 250):
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])

            #cv2.drawContours(result, [c], -1, (0,255,0), 2)
            #cv2.circle(result, (cX, cY), 7, (255, 255, 255), -1)
            #cv2.putText(result, "Possible banana!", (cX - 20, cY - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 2)
            if(area > area2):
                area2 = area
                c2X = cX
                c2Y = cY

    res = 0
    
    if(area2 > 400):
        if((c2X > 145 and c2X < 190) and (c2Y > 75 and c2Y < 110)):
            res = 6
        elif((c2X > 145 and c2X < 190) and (c2Y < 75)):
            res = 1
        elif((c2X < 145) and (c2Y < 75)):
            res = 2
        elif((c2X > 190) and c2Y < 75):
            res = 3
        elif((c2X < 145) and (c2Y > 75 and c2Y < 110)):
            res = 4
        elif((c2X > 190) and (c2Y > 75 and c2Y < 110)):
            res = 5
    else:
        res = 0

    #cv2.imshow('Result', result)

    return res
