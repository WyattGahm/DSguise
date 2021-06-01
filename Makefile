CXX = g++
CFLAGS = 
LFLAGS = -lcrypto
OBJECTS = dsguise.o
TARGET_NAME = dsguise
VERSION_VER = 0.3
VERSION_DATE = built on `date`

ifeq ($(OS),Windows_NT)
	TARGET = $(TARGET_NAME).exe
	LFLAGS += -static -static-libgcc
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		CFLAGS += -I/usr/local/opt/openssl/include
		LFLAGS += -L/usr/local/opt/openssl/lib
	endif
	TARGET = $(TARGET_NAME)
endif

all: $(TARGET)

%.o: %.cpp 
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $^ -o $@ $(LFLAGS)
	
clean:
	rm -f $(OBJECTS) $(TARGET) 
