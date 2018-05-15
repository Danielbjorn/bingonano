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
#cap = cv2.VideoCapture('http://192.168.1.90:8080/videoView')
cap = cv2.VideoCapture('http://192.168.1.130:8080/videoView')
#cap2 = cv2.VideoCapture(0)

#lowerBound=numpy.array([33,80,40])
#upperBound=numpy.array([102,255,255])

#ont=cv2.cv.InitFont(cv2.cv.CV_FONT_HERSHEY_SIMPLEX,2,0.5,0,3,1)


while True:
    
    ret, frame = cap.read()
    #frame = cv2.resize(frame,(340,220))
    frame = imutils.rotate(frame, -90)
    frame2 = frame
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    lower_range = numpy.array([20,100,100])
    #upper_range = numpy.array([57, 84, 82])
    upper_range = numpy.array([30, 255, 255])

    mask = cv2.inRange(hsv, lower_range, upper_range)

    result = cv2.bitwise_and(frame, frame, mask=mask)
    
    blurred = cv2.GaussianBlur(mask, (5, 5), 0)
    thresh = cv2.threshold(blurred, 60, 255, cv2.THRESH_BINARY)[1]
    
    cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cnts = cnts[0] if imutils.is_cv2() else cnts[1]
    '''
    for c in cnts:
        M = cv2.moments(c)
        area = cv2.contourArea(c)
        if(M["m00"] != 0 and area > 350):
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])

            #area = cv2.contourArea(c)
            #print str(area)
            #print "Coordinates: X - " + str(cX) + " Y - " + str(cY) 
            
            if(area > 250):
                cv2.drawContours(result, [c], -1, (0,255,0), 2)
                cv2.circle(result, (cX, cY), 7, (255, 255, 255), -1)
                cv2.putText(result, "area: " + str(area), (cX - 20, cY - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 2)
    '''
    area2 = 0
    for c in cnts:
        M = cv2.moments(c)
        area = cv2.contourArea(c)
        if(M["m00"] != 0 and area > 200):
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])

            cv2.drawContours(result, [c], -1, (0,255,0), 2)
            cv2.circle(result, (cX, cY), 7, (255,255,255), -1)
            cv2.putText(result, "Possible banana!", (cX - 20, cY - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 2)
            if(area > area2):
                area2 = area
                c2X = cX
                c2Y = cY
                                                                

    res = 0
    print str(c2X) + "  " + str(c2Y)

    if(area2 > 450):
        if((c2X > 145 and c2X < 190) and (c2Y > 110 and c2Y < 140)):
            res = 6
            text = "Banana Dance"
        elif((c2X > 145 and c2X < 190) and (c2Y < 110)):
            res = 1
            text = "forward"
        elif((c2X < 145) and (c2Y < 110)):
            res = 2
            text = "forward-left"
        elif((c2X > 190) and c2Y < 110):
            res = 3
            text = "forward-right"
        elif((c2X < 145) and (c2Y > 110 and c2Y < 140)):
            res = 4
            text = "left"
        elif((c2X > 190) and (c2Y > 110 and c2Y < 140)):
            res = 5
            text = "right"
    else:
        res = 0

    #print str(res) + ", area: " + str(area2) + ", Action: " + text
    
    cv2.imshow('Result', frame)

    #X range: 145-190px
    #Y range: 110-140px 


    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    #img=cv2.resize(frame,(340,220))
    #cv2.imshow('Video', frame)

    if cv2.waitKey(1) == 27:
        exit(0)
    
