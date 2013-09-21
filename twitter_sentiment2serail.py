#!/opt/local/bin/python
# -*- coding: utf-8 -*-

#
# Libraries needed:
#  - pySerial
#  - textwrap
#  - tweepy
#  - enchant
#  - py27-nltk
#    * matplotlib
#  - install the nltk US english disctionary using nltk.download()
#  - enchant will complain about the dictionary, so do the following
#        sudo port install aspell-dict-en

# Note:  If you are using Macports to install the libraries, make sure that the macports version is selected
#        sudo port select python python27

import tweepy
from textwrap import TextWrapper
from calculation import *
import serial

# Needed for sleep() to test the LED cube
import time

# configures the serial port
ser = serial.Serial('/dev/tty.usbmodem1421', 9600)

# Keys to talk to Twitter
f = open('secrets.txt', 'r')
consumer_key = f.readline()
consumer_secret = f.readline()

access_token_key = f.readline()
access_token_secret = f.readline()

auth1 = tweepy.OAuthHandler(consumer_key, consumer_secret)
auth1.set_access_token(access_token_key, access_token_secret)

# New York city
#locArea = [-74,40,-73,41]
#setTerms = ['hello', 'goodbye', 'goodnight', 'good morning']
#setTerms = ['hello', 'goodbye', 'goodnight', 'good morning']
# location for Northampton/Amherst, SE to NW
# -72.75, 42.25,   -72.4, 42.5   

# Northampton/Amherst, MA
#locArea = [-72.75,42.25,-72.4, 42.5]
#setTerms = ['amherst']

# Ann Arbor/ Detroit area
locArea = [-83.93, 42.01, -83.17, 42.34]


# Translates the measurements into cube coordinates
# For each axis, 
#     readings between the lower and upper quartile will light the middle LED
#     readings above the upper quartile will light the distal LED
#     readings below the lower quartile will light the proximal LED

def getLEDCoordinates(valence, arousal, dominance):
    layer = 0
    x = 0
    y = 0
    if valence < 5.416 :
        layer = 0
    elif valence < 7.311 :
        layer = 1
    else:
        layer = 2

    if arousal < 4.744 :
        x = 0
    elif arousal < 5.690:
        x = 1
    else:
        x = 2

    if dominance < 5.068:
        y = 0
    elif dominance < 5.884 :
        y = 1
    else:
        y = 2

    return [layer, x, y]

    
def testLEDcube():
    for layer in range (0, 3):
        for x in range (0, 3):
            for y in range (0, 3):
                serStr = '%i%i%i' % (layer, x, y)
                ser.write(serStr)
                time.sleep(0.2)
    # Sets the LED in the middle to start
    ser.write("111")




# class StreamListener(tweepy.StreamListener):
#     def on_status(self, tweet):
#         print 'Ran on_status'

#     def on_error(self, status_code):
#         print 'Error: ' + repr(status_code)
#         return False

#     def on_data(self, data):
#         print 'running' + data


class StreamListener(tweepy.StreamListener):
    status_wrapper = TextWrapper(width=60, initial_indent='    ', subsequent_indent='    ')
    def on_status(self, status):
        try:
            tweetText =  self.status_wrapper.fill(status.text)
            #print self.status_wrapper.fill(status.text)
            #print '\n %s  %s  via %s\n' % (status.author.screen_name, status.created_at, status.source)
            # TODO:  Do the ANEW calculations here, then send it to the LED cube.
            print "Raw Tweet:", tweetText
            curSentiment = getAnewSentiment(tweetText)
            if curSentiment != None:
                # Gets the coordinates for the LED cube
                a = getLEDCoordinates(curSentiment['valence'], curSentiment['arousal'], curSentiment['dominance'])
                serStr = '%i%i%i' % (a[0], a[1], a[2])
                ser.write(serStr)
                # prints stuff for fun
                print "==> Tweet:", tweetText
                print "Sentiment:",curSentiment
                print "LED coordinates (valence=layer, arousal=x dominance=y):", serStr
                
        except Exception, e:
            # Catch any unicode errors while printing to console
            # and just ignore them to avoid breaking application.
            pass

# The first thing we do is to test the LED cube
testLEDcube()

# We then open a listener
l = StreamListener()
streamer = tweepy.Stream(auth=auth1, listener=l)

#streamer.filter(track = setTerms, locations = locArea)
streamer.filter(locations = locArea)
