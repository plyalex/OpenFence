
# Use a database already created on mongolab 
server = 'ds145385.mlab.com'
port = 45385
db_name = 'openfence'
username = 'openfence1'
password = '$9GF7bz!qC$t'

import serial
import struct
import time
import json
import datetime
import sched

from pymongo import MongoClient
# what versions are we using
import sys
print ('Python version', sys.version)
import pymongo
print ('Pymongo version', pymongo.version)

print ('\nConnecting ...')
client = MongoClient('mongodb://'+username+':'+password+'@ds145385.mlab.com:45385/openfence')

db = client.openfence

fencelocations = db.fencelocations
animals = db.animals
animallocations = db.animallocations
print ('\nCollection Opened')


arduinoSerialData = serial.Serial('/dev/cu.usbmodemfa131', 115200)

def uploadPt2Mongo(data):
	flag, sender, lat, lon, time, date, alerts, shocks, ver, junk1, junk2 = struct.unpack('<BBffIIHBBcc',myData)
	if (date==0):
		return
	animalinfo = animals.find_one({'RF_ID' : sender },{'_id':1})
	print (flag)
	print (sender)
	print (lat)
	print (lon)
	print (time)
	print (date)
	print (alerts)
	print (shocks)

	New_Point = {	"animalid": animalinfo['_id'],
					"paddock":0,
					"location":[lon,lat],
					"alerts":alerts,
					"shocks":shocks,
					"sent_at":datetime.datetime(2000+date%100, (date//100)%100, (date//10000), (time//10000), (time//100)%100, (time%100), 0)
	}
	animallocations.save(New_Point)
	print ("Uploaded new Point to Server")


def uploadFence2LoRa(data):
	# Parse Data to variables
	flag = 1
	index=0
	lat = []
	lon = []
	for item in data:
		location=item['location']
		lat.append(location[1])
		lon.append(location[0])
		version=item['version']
		index=index+1
	print(index)
	index=len(lat)
	print (index)
	for i in range(0, index):
		if (i%2 == 0): #Even
			lat0=lat[i]
			lon0=lon[i]
			lat1=lat[i+1]
			lon1=lon[i+1]
			print ("Sending Serial Data")
			arduinoSerialData.write(struct.pack('<BBBBBffff',flag,2,i==index,index,i,lat0,lon0,lat1,lon1))
	print (data)
	# arduinoSerialData.write(struct.pack('<BBBBBffff',flag,version,last,numPts,X,lat0,lon0,lat1,lon1))



# let it initialize
time.sleep(2)


starttime=time.time()
fenceversion=1;

while (1==1):
	#Receive Packets

	if (arduinoSerialData.inWaiting()>0):
		myData = arduinoSerialData.readline()
		# print (myData)
		uploadPt2Mongo(myData)
		#print (struct.unpack('<BBffIIHBBcc',myData))

	if ((time.time()-starttime) > 60):
		# Check Database
		print ("Check if got latest fence locations from DB")
		serverfenceversion=fencelocations.find_one({},{'version':1})
		serverfenceversion=serverfenceversion['version']
		if (serverfenceversion != fenceversion):
			fenceversion = serverfenceversion
			print ("New version available: Downloading...")
			fencedata = fencelocations.find({}).sort('order', pymongo.ASCENDING)
			# uploadFence2LoRa(fencedata)
			for item in fencedata:
				print (item)
			
		starttime=time.time()







# # send the first int in binary format
# arduinoSerialData.write(struct.pack('>B', 45))

#               memcpy(&d1[index].ver,       &buffer[0],  1);   //Dest, Orig, Bytes
#               memcpy(&d1[index].last,      &buffer[1],  1);
#               memcpy(&d1[index].numPts,    &buffer[2],  1);
#               memcpy(&d1[index].X,         &buffer[3],  1);
#               memcpy(&d1[index].lat0,      &buffer[4],  4);
#               memcpy(&d1[index].lon0,      &buffer[8],  4);
#               memcpy(&d1[index].lat1,      &buffer[12], 4);
#               memcpy(&d1[index].lon1,      &buffer[16], 4);


