import cv2
import sys
import imutils
import numpy
import time
from Queue import Queue
import urllib
import threading
from threading import Thread, Event, ThreadError


url = "http://192.168.1.49:8080/videoView"
cap = cv2.VideoCapture('http://192.168.8.148:8020/videoView')
#cap2 = cv2.VideoCapture(0)

#lowerBound=numpy.array([33,80,40])
#upperBound=numpy.array([102,255,255])

#ont=cv2.cv.InitFont(cv2.cv.CV_FONT_HERSHEY_SIMPLEX,2,0.5,0,3,1)


while True:
    
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

    for c in cnts:
        M = cv2.moments(c)
        if(M["m00"] != 0):
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])

            area = cv2.contourArea(c)
            #print str(area)
            print "Coordinates: X - " + str(cX) + " Y - " + str(cY) 
            
            if(area > 250):
                cv2.drawContours(result, [c], -1, (0,255,0), 2)
                cv2.circle(result, (cX, cY), 7, (255, 255, 255), -1)
                cv2.putText(result, "area: " + str(area), (cX - 20, cY - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 2)

    
    cv2.imshow('Result', result)


    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    #img=cv2.resize(frame,(340,220))
    #cv2.imshow('Video', frame)

    if cv2.waitKey(1) == 27:
        exit(0)
    
