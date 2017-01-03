#!/usr/bin/python
import serial 
import MySQLdb
 
#establish connection to MySQL. You'll have to change this for your database.
#dbConn = MySQLdb.connect("localhost","database_username","password","database_name") or die ("could not connect to database")
dbConn = MySQLdb.connect("localhost","root","1234","mote1") or die ("could not connect to database")
#open a cursor to the database
#cursor = dbConn.cursor()
 
#device = '/dev/tty.usbmodem1411' #this will have to be changed to the serial port you are using
device = '/dev/ttyUSB3' #this will have to be changed to the serial port you are using
try:
  print "Trying...",device 
  skyMote = serial.Serial(device, 115200) 
except: 
  print "Failed to connect on",device    
 
while True:
 #open a cursor to the database
 cursor = dbConn.cursor() 
 try:  
   data = skyMote.readline()  #read the data from the sky mote
   pieces = data.split("/")  #split the data by the tab
   print "pieces[0]=",pieces[0]
   print "pieces[1]=",pieces[1]
   print "pieces[2]=",pieces[2]
   print "pieces[3]=",pieces[3]
   print "pieces[4]=",pieces[4]
   print "pieces[5]=",pieces[5]
   print "pieces[6]=",pieces[6]
   print "pieces[7]=",pieces[7]
   #Here we are going to insert the data into the Database
   try:
     cursor.execute("INSERT INTO LinkStatisticsPaper (id, nodeID, secuencia, tempC, light, RSSI, LQI, latency) VALUES (NULL,%s,%s,%s,%s,%s,%s,%s)", (pieces[1],pieces[2],pieces[3],pieces[4],pieces[5],pieces[6],pieces[7]))
     dbConn.commit() #commit the insert
     cursor.close()  #close the cursor
   except MySQLdb.IntegrityError:
     print "failed to insert data"
   finally:
     cursor.close()  #close just incase it failed
 except:
   print "Failed to get data from Sky mote!"
