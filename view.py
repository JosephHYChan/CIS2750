#!/usr/bin/python

import ast
import os
import sys
import platform
import time
import glob

def file_separator():
    separator = "/"
    return separator;

def printText(text):
    print text,
    
def getByteSize(string):
    bytesPerNewLine = 2
    return len(string)+(bytesPerNewLine-1);

def getLastReadByte(dataFile, noOfRead):
    if (noOfRead>0):
        string = dataFile.readline()
        if (noOfRead>1):
            return getLastReadByte(dataFile, noOfRead-1);
        else:
            return int(string);
    else:
        return -1;     
        
def getNumberOfReadMessages(usersFile, username):
    string = usersFile.readline()
    noOfWordsInSuppliedName = username.count(' ')+1
    noOfWordsInRetrievedName = string.count(' ')
    if ((string != "") and (noOfWordsInSuppliedName == noOfWordsInRetrievedName)):
        pos = string.find(username)
        if (pos != -1):
            result = string[pos+len(username):]
            result =result.strip()
            return int(result)
        else:
            return getNumberOfReadMessages(usersFile, username)
    else:
        return -2;

def getDate(msg):
    findAt = msg[1].find("Date: ")
    return msg[1][findAt+len("Date: "):-1];

def getAuthor(msg):
    findAt = msg[0].find("Sender: ")
    return msg[0][findAt+len("Sender: "):-1];

def openStream(path, streamName):
    name = path + file_separator() + streamName
    return open(name, "r");

def openData(path, streamName):
    name = path + file_separator() + streamName + "Data"
    return open(name, "r");

def openUsers(path, streamName):
    name = path + file_separator() + streamName + "Users"
    return open(name, "r+");

def openCache(path, mode):
    name = path + file_separator() + "view" + "Cache"
    return open(name, mode);

def cacheState(path, internalState):
    cacheFile = openCache(path, "w")
    cacheFile.write(str(len(internalState))+'\n')
    for eachState in internalState:
        cacheFile.write(str(eachState)+'\n')
    cacheFile.close()
    
def loadState(path, internalState):
    noOfStates = 0
    del internalState[:]
    cacheFile = openCache(path, "r")
    noOfStates = int(cacheFile.readline().rstrip())
    while (noOfStates>0):
        internalState.append(cacheFile.readline().rstrip())
        noOfStates -= 1
    cacheFile.close()

def cacheFirstUnreadMessageForStream(path, stream, offset, ):
    cacheFile = openCache(path, stream, "w")
    cacheFile.write(str(offset))
    cacheFile.write(str(offset))
    cacheFile.close()
    
def getFirstUnreadMessageForStream(path, stream):
    cacheFile = openCache(path, stream, "r")
    offset = cacheFile.readline()
    cacheFile.close()
    return int(offset);

def locateFirstUnreadMessageForStream(path, username, stream):
    usersFile = openUsers(path, stream)
    offset = getNumberOfReadMessages(usersFile, username) + 1
    usersFile.close()
    return offset;

def findUser(fileId, username):
    line = fileId.readline()
    if (line == ""): 
        return False
    elif (line.find(username) != -1): 
        return True
    else: 
        return findUser(fileId, username);
    
def getSubscribedStreams(path, username):
    filelist = glob.glob(path + file_separator() + "*Users")
    subscription = []
    for eachfile in filelist:
        left = eachfile.find(path + file_separator())+len(path + file_separator())
        right = eachfile.find("Users")
        streamName = eachfile[left:right]
        fileId = openUsers(path, streamName)
        fileId.seek(0, 0)
        if (findUser(fileId, username) == True):
            subscription.append(streamName)
        fileId.close()
    return subscription

def rewriteUsersFile(usersFile, username, noOfMessageRead):
    line = []
    replace = []
    
    usersFile.seek(0,0)
    readIn = usersFile.readline()
    while (readIn != ""):   
        line.append(readIn) 
        readIn = usersFile.readline()
    
    for each in line:
        if (each.find(username) != -1):
            replace.append(username + " " + str(noOfMessageRead) + '\n')
        else:
            replace.append(each)
    
    usersFile.seek(0,0)
    for each in replace:
       usersFile.write(str(each))
        
def updateNumberOfReadMessages(usersFile, username, noOfMessageRead):
    rewriteUsersFile(usersFile, username, noOfMessageRead)

def cacheUnreadMessageForStream(path, stream, offset, msgNo, msgCount, streamUnreadPosting):
    index = 0
    readCount = 0
    dataFile = openData(path, stream)
    streamFile = openStream(path, stream)
    endByte = getLastReadByte(dataFile, offset-1)
    streamFile.seek(endByte+1, 0)
    line = dataFile.readline()
    while ((line != "") and (readCount<msgCount)):
        streamUnreadPosting.append([])        
        startByte = 1 + endByte
        endByte = int(line)
        messageSize = (endByte-startByte)+1
        bytesRead = 0
        streamUnreadPosting[msgNo].append(stream)
        streamUnreadPosting[msgNo].append([])
        while (bytesRead<messageSize):
            line = streamFile.readline()
            streamUnreadPosting[msgNo][1].append(line)
            bytesRead += getByteSize(line)
        msgNo += 1
        readCount += 1
        line = dataFile.readline()
    streamFile.close()
    dataFile.close()
    return msgNo, readCount;
        
def cacheAllMessages(path, username, streams, streamUnreadPosting, fresh, addNew, streamOffset):
    msgNo = 0

    if (fresh==True):
        streamOffset.clear()
    for eachStream in streams:
        if (fresh==True):
            offset = locateFirstUnreadMessageForStream(path, username, eachStream)
            msgCount = sys.maxsize
        elif (addNew==True):
            offset = int(streamOffset.get(eachStream)[0])
            msgCount = sys.maxsize
        else:
            offset = int(streamOffset.get(eachStream)[0])
            msgCount = int(streamOffset.get(eachStream)[1])
        if (offset>=0):
            msgNo, msgCount = cacheUnreadMessageForStream(path, eachStream, offset, msgNo, msgCount, streamUnreadPosting)
        if (fresh==True):
            streamOffset.setdefault(eachStream, [offset, msgCount])
        elif (addNew==True):
		    streamOffset[eachStream] = [offset, msgCount]
    return;

def sortPostingByAuthors(streamUnreadPosting, sortedUnreadPosting):
    del sortedUnreadPosting[:]
    MsgNo = 0
    authorList = {}
    for eachMsg in streamUnreadPosting:
        author = getAuthor(eachMsg[1])
        if (authorList.get(author)==None):
            authorList.setdefault(author, [])
        MsgList = authorList.get(author)
        MsgList.append(MsgNo)
        MsgNo += 1
    MsgNo = 0
    for eachAuthor in sorted(authorList):
        for eachMsgId in authorList.get(eachAuthor):
            sortedUnreadPosting.append(streamUnreadPosting[eachMsgId])
    return;

def sortPostingByDates(streamUnreadPosting, sortedUnreadPosting):
    del sortedUnreadPosting[:]
    MsgNo = 0
    dateList = {}
    for eachMsg in streamUnreadPosting:
        date = getDate(eachMsg[1])
        if (dateList.get(date)==None):
            dateList.setdefault(date, [])
        MsgList = dateList.get(date)
        MsgList.append(MsgNo)
        MsgNo += 1
    for eachDate in sorted(dateList):
        for eachMsgId in dateList.get(eachDate):
            sortedUnreadPosting.append(streamUnreadPosting[eachMsgId])
    return;

def countMessage(sortedUnreadPosting, stream):
    count = 0
    for each in sortedUnreadPosting:
        if (each[0]==stream):
            count += 1
    return count;
        
def markOneRead(path, username, sortedUnreadPosting, msgNo):
    stream = sortedUnreadPosting[msgNo][0]
    usersFile = openUsers(path, stream)
    noOfMessageRead = getNumberOfReadMessages(usersFile, username)
    noOfMessageRead += 1
    updateNumberOfReadMessages(usersFile, username, noOfMessageRead)
    usersFile.close()
    
def markAllRead(path, streams, streamOffset, username, lastMsgNo, sortedUnreadPosting):
    for eachStream in streams:
        usersFile = openUsers(path, eachStream)
        noOfMessageRead = int(streamOffset.get(eachStream)[0]) + countMessage(sortedUnreadPosting, eachStream) - 1
        updateNumberOfReadMessages(usersFile, username, noOfMessageRead)
        usersFile.close()

def identifyStreams(path, streams, username):
    del streams[:]
    for each in getSubscribedStreams(path, username):
        printText(each+' ')
    printText("all")

def buildStreams(path, streams, username, choice):  
    del streams[:]
    if (choice != "all"):
        for each in list(choice.split(' ')):
            streams.append(each)
    else:
        for each in getSubscribedStreams(path, username):
            streams.append(each)

def loadStreams(path, streams, username, fresh, addNew, sortMode, streamUnreadPosting, sortedUnreadPosting, streamOffset):
    del streamUnreadPosting[:]
    
    cacheAllMessages(path, username, streams, streamUnreadPosting, fresh, addNew, streamOffset)
    if (sortMode == 0):
        sortPostingByDates(streamUnreadPosting, sortedUnreadPosting)
    else:
        sortPostingByAuthors(streamUnreadPosting, sortedUnreadPosting)
    return;
    
def main(arglist):
    username = ""
    path = "." + file_separator() + "message"
    noError = True
    streams = []
    internalState = []
    streamUnreadPosting = []
    sortedUnreadPosting = []
    streamOffset = {}
    sortMode=0
    pos = 1
    count = len(arglist)
    msgNo = 0
    dateMsgNo = 0
    authorMsgNo = 0
    maxMsgNo = 0
    lastMsgNo=-1
 
    
    if (count>1):
        parameters = arglist[pos]
        pos = pos+1
        while (pos<count):
            parameters = parameters + "," + arglist[pos]
            pos = pos+1
        passInCmd=dict(x.strip().split('=') for x in parameters.split(','))
        if (passInCmd.get('user', None)==None):
            printText("Error : Name of user not specified.")
        else:
            username = passInCmd.get('user')
            username = username.strip()
            if (passInCmd.get('action', None)!=None):
                #view.py "user=$name, action=checkStreams"
                if (passInCmd.get('action')=="checkStreams"):
                    identifyStreams(path, streams, username)
                    return;
                elif (passInCmd.get('action')=="specifyStreams"):
                    #view.py "user=$name, action=specifyStreams, streams=$stream"
                    if (passInCmd.get('streams', None)==None):
                        messageCount = 0
                        noError = False
                    else:
                        choice = passInCmd.get('streams')
                        buildStreams(path, streams, username, choice)
                        loadStreams(path, streams, username, True, False, 0, streamUnreadPosting, sortedUnreadPosting, streamOffset)
                        messageCount = len(streamUnreadPosting)
                        lastMsgNo = messageCount-1
                        if (lastMsgNo<0):
                            noError = False							
                    internalState = [streams, streamOffset, messageCount, msgNo, dateMsgNo, authorMsgNo, maxMsgNo, lastMsgNo, sortMode]
                    cacheState(path, internalState)
                    if ((msgNo==maxMsgNo) and (maxMsgNo<lastMsgNo) and (sortMode==0)):
                        markOneRead(path, username, sortedUnreadPosting, msgNo)
                loadState(path, internalState)
                streams = ast.literal_eval(internalState[0])
                streamOffset = ast.literal_eval(internalState[1])
                messageCount = int(internalState[2])
                msgNo = int(internalState[3])
                dateMsgNo = int(internalState[4])
                authorMsgNo = int(internalState[5])
                maxMsgNo = int(internalState[6])
                lastMsgNo = int(internalState[7])
                sortMode = int(internalState[8])
                loadStreams(path, streams, username, False, False, 0, streamUnreadPosting, sortedUnreadPosting, streamOffset)
                if (lastMsgNo>=0):
                    if (sortMode==1):
                        sortPostingByAuthors(streamUnreadPosting, sortedUnreadPosting)
                    else:
                        sortPostingByDates(streamUnreadPosting, sortedUnreadPosting)
                    if (passInCmd.get('action')=="previous"):
                        if (msgNo>0):
                            msgNo -= 1
                    elif (passInCmd.get('action')=="next"):
                        if (msgNo<lastMsgNo):
                            msgNo += 1
                        if ((msgNo>maxMsgNo) and (sortMode==0)):
                            if (maxMsgNo<lastMsgNo):
                                markOneRead(path, username, sortedUnreadPosting, msgNo)
                                maxMsgNo = msgNo
                    elif (passInCmd.get('action')=="order"):
                        if (sortMode==0):
                            sortMode=1
                            authorMsgNo = msgNo
                            msgNo = dateMsgNo
                        else:
                            sortMode=0
                            dateMsgNo = msgNo
                            msgNo = authorMsgNo
                        if (sortMode==1):
                            sortPostingByAuthors(streamUnreadPosting, sortedUnreadPosting)
                        else:
                            sortPostingByDates(streamUnreadPosting, sortedUnreadPosting)
                    elif (passInCmd.get('action')=="checkNew"):
                        loadStreams(path, streams, username, False, True, 0, streamUnreadPosting, sortedUnreadPosting, streamOffset)
                        messageCount = len(streamUnreadPosting)
                        lastMsgNo = messageCount-1
                        internalState = [streams, streamOffset, messageCount, msgNo, dateMsgNo, authorMsgNo, maxMsgNo, lastMsgNo, sortMode]
                        cacheState(path, internalState)
                    elif (passInCmd.get('action')=="markAll"):
                        markAllRead(path, streams, streamOffset, username, lastMsgNo, sortedUnreadPosting)
                        maxMsgNo = lastMsgNo
                    elif (passInCmd.get('action')!="specifyStreams"):
                        noError = False
                    if (noError==True):
                        for each in sortedUnreadPosting[msgNo][1]:
                            printText(each)
                        else:
                            printText("")
                        internalState = [streams, streamOffset, messageCount, msgNo, dateMsgNo, authorMsgNo, maxMsgNo, lastMsgNo, sortMode]
                        cacheState(path, internalState)
                    else:
                        printText("Error: Unrecognize action")
                else:
                    printText("No unread post")
            else:
                printText("Error: Cannot proceed due to missing parameter(s)")
    else:
        printText("Error: Cannot proceed, all mandatory parameters are missing")
    return;

if __name__ == "__main__": main(sys.argv)
