import socket
from time import sleep
from pushbullet import Pushbullet
import requests
import RPi.GPIO as GPIO 

storedValue = "Hello, this is Node 4"
thingspeakAPI = 'https://api.thingspeak.com/update?api_key=1Y2E4B8SNAJOG9FN&field1=Gruppe4'
pushAPI_KEY = "o.r50KNbkRuYMHUl6ykITa40fZphOKn24j"

def connectToUDPClient(ddns_name,port):
    """
    connectToUDPClient:
        creates UDP Socket
        pings UDP Client and listens 2 seconds for a reply before it repeats
        if UDP Client replies the reply is printed and returned with the created UDP Socket 
        
        :param ddns_name: domain name of the udp client
        :param port: open port of the udp client
        :return: udp sockt and udp client's reply
    """
    host = socket.gethostbyname(ddns_name)
    print("Connecting to client with IP:", host, "port",port,"...")
    UDPsocket= socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    UDPsocket.settimeout(2)
    reply = ""
    while not reply: 
        try:
            UDPsocket.sendto(str.encode("Start"),(host,port))
            reply, addr = UDPsocket.recvfrom(1024)
            reply = reply.decode('utf-8')
        except socket.timeout as e:
            print(".")
    print("Connection established. Client says : ")
    print(reply)
    return (UDPsocket,reply)

def sendDatatoThingspeak(API): 
    req = requests.get(API)
    print("Feedback > " + req.text)

def sendPushNotification(API_KEY):
    pb = Pushbullet(API_KEY)
    push = pb.push_note("This is the title", "This is the subject")

def setupServer():
    """
    setUpServer creates a TCP socket and assigns to local adress and local port  
    :return: TCP Socket
    """ 
    host = ''
    port = 5560 
    TCPsocket= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("Socket created.")
    try: 
        TCPsocket.bind((host, port))
    except socket.error as msg:
        print(msg)
    print("Socket bind complete.")
    return TCPsocket

#Set up client connection to socketServer
def setupConnection():
    """
    setupConnection enables the TCP server to accept one connection  
    :return: the established connection
    """
    TCPsocket.listen(1) #Ermöglicht jeweils eine Verbindung
    conn, address = TCPsocket.accept() #accept received connection and address
    print("Connected to: " + address[0] + ":" + str(address[1])) #connection confirmation 
    return conn

def dataSend(conn,reply):
    conn.sendall(str.encode(reply))

def dataReceive(conn):
    data = conn.recv(1024) #buffer size kann man ändern
    data = data.decode('utf-8') #decode data
    dataMessage = data.split(' ', 1) #split data to extract command from the rest of the data
    reply = dataMessage[0][0:len(dataMessage[0])-1]#save reply and take out last element 
    print("Reply received >  " + reply)
    return reply
 
#get UDP Server (node 3) Reply
UDPsocket, UDPreply = connectToUDPClient("gruppe3.ddns.net",50001)
#Package verification
while UDPreply != "hallo Gruppe 4 ": #TODO UDPserver confimation festlegen
    UDPsocket, UDPreply = connectToUDPClient()
#Send back confirmation
UDPsocket.sendto(str.encode("acknowledge"),(host,port))
#Send data to think speak
sendDatatoThingspeak(thingspeakAPI)
# Use physical pin numbering
GPIO.setmode(GPIO.BOARD)
# Set pin 8 to be an output pin and set initial value to low (off)
GPIO.setup(8, GPIO.OUT, initial=GPIO.LOW) 
# Turn on
GPIO.output(8, GPIO.HIGH) 
#Send push notification
sendPushNotification(pushAPI_KEY)

#Set up socketServer
TCPsocket = setupServer()
#Set up connection to node5
TCPconn =    setupConnection()
for i in range (1,10):
    #Send data to node 5
    dataSend(TCPconn,"Hallo gruppe 5")
    print("Data has been sent "+ str(i) +" time(s)")
    #get node 5 reply 
    TCPClientreply = dataReceive(TCPconn)
    #check confirmation
    if TCPClientreply == 'Hallo':
        print("CONFIRMATION_OK")
        break #break from for loop if confirmation ok 
    else:
        print("CONFIRMATION_NOT_OK")
    i= i + 1
TCPconn.close()
print("Connection closed...Server is shutting down...")
TCPsocket.close()
#Turn LED OFF
GPIO.output(8, GPIO.LOW)
