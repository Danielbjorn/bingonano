import cv2
import sys
import numpy
import time
from Queue import Queue
import urllib
import threading
from threading import Thread, Event, ThreadError


url = "http://192.168.1.49:8080/videoView"
'''
cv2.namedWindow("test",1)
response = urllib.urlopen(url)
'''
cap = cv2.VideoCapture('http://192.168.1.49:8080/videoView')

lowerBound=numpy.array([33,80,40])
upperBound=numpy.array([102,255,255])

ont=cv2.cv.InitFont(cv2.cv.CV_FONT_HERSHEY_SIMPLEX,2,0.5,0,3,1)


while True:
    
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    img=cv2.resize(frame,(340,220))
    cv2.imshow('Video', frame)

    if cv2.waitKey(1) == 27:
        exit(0)


    '''        
    response = urllib.urlopen(url)
    img_array = numpy.array(bytearray(response.read()), dtype=numpy.uint8)
    frame = cv2.imdecode(img_array, -1)
    
    cv2.ShowImage("test", frame)
    cv2.waitKey(0)
    '''
    
