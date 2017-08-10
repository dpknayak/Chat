# Chat
Read Me:
#Application Details:
    This Chat application was created in two parts:
    
    1. Server Application
    2. Android Application (Clients)
    
    Detailed description of the two is as follows:
    
    1. Server Application:
        It C++ based application. Application uses 3 threads, 
            2 of which are dedicated to the Message passing between Clients.
                One thread receives the Messages from the Clients and the Second forwards to the recipient client of the message.
            1 of which enrolls the client and notifies other thread about the new received Clients.
        Main thead also ensure the graceful termination of the Server application with the use of lamda function.
        As of now any input from the console will lead to the termination of the Server.
        
    2. Android Application:
        It uses two thread for communication as per the Android rules (as main thread cannot be used for the data intensive activity)
        Main thread is responsible for the UI of the Application.
        
        UI is basically a List View, whenever the message is sent or received with the other user, it is logged in the view.
        
        
#How the test the application:
1. Download the Server application in the a folder.
2. Open the Server.cpp and change the ip addr in line 22 to ip address of your machine.
3. Open the terminal at that location and excute: make all && ./chat
4. It would run the server.
5. Load the android application into the android studio or the eclipse IDE with ADT(Android development Tool)
6. Install the application into the phone with the developer's mode ON with the tool as the unsigned application.
7. On the application UI: enter the IP address of the server.
8. Assigned the unique 5 digit keys to the number of the users installing the app and the key of the user you wish to communicate with.
9. then all you have to do it chat.......
