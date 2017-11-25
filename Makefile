CC = g++
CFLAGS := -D_LARGEFILE_SOURCE \
	-D_FILE_OFFSET_BITS=64 \
	-Wall -Werror -g \
	-I./libuv/include \
	-I/usr/local/include

CPPFLAGS := -g -std=c++14 
CXXFLAGS += -g -O0

INCLFLAGS := -Ideps/http-parser \
	-Ideps/libuv/include

LDFLAGS := -L. \
	-Ldeps/libuv/include \
	-lpthread \
	-framework Foundation \
	-framework CoreServices

CPPFILES := ./test.cc \
	./src/*.cc

test: ./test.cc ./src/*.cc ./include/*.h
	$(CC) $(CPPFLAGS) $(CPPFILES) -o test $(INCLFLAGS) \
		deps/libuv/build/Release/libuv.a \
		$(LDFLAGS)

libuv: ./deps/libuv/include/uv.h
	cd ./deps/libuv/ && \
		./gyp_uv.py -f xcode && \
		xcodebuild -ARCHS="x86_64" -project uv.xcodeproj \
		-configuration Release -target All