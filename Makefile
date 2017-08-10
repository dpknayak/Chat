# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# the build target executable:
TARGET = chat

SOURCES = \
        ChatData.cpp \
		main.cpp \
		ReceiverThread.cpp \
		SenderThread.cpp \
		Server.cpp
        
all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) -lpthread

clean:
	$(RM) $(TARGET)
