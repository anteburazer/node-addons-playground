cmd_Release/obj.target/png2bmp/cpp/lodepng.o := c++ '-DNODE_GYP_MODULE_NAME=png2bmp' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-DV8_DEPRECATION_WARNINGS' '-DV8_IMMINENT_DEPRECATION_WARNINGS' '-D_DARWIN_USE_64_BIT_INODE=1' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-DOPENSSL_THREADS' '-DBUILDING_NODE_EXTENSION' -I/Users/bura/.node-gyp/12.4.0/include/node -I/Users/bura/.node-gyp/12.4.0/src -I/Users/bura/.node-gyp/12.4.0/deps/openssl/config -I/Users/bura/.node-gyp/12.4.0/deps/openssl/openssl/include -I/Users/bura/.node-gyp/12.4.0/deps/uv/include -I/Users/bura/.node-gyp/12.4.0/deps/zlib -I/Users/bura/.node-gyp/12.4.0/deps/v8/include -I../node_modules/nan  -Os -gdwarf-2 -mmacosx-version-min=10.10 -arch x86_64 -Wall -Wendif-labels -W -Wno-unused-parameter -std=gnu++1y -stdlib=libc++ -fno-rtti -fno-exceptions -fno-strict-aliasing -MMD -MF ./Release/.deps/Release/obj.target/png2bmp/cpp/lodepng.o.d.raw -I/usr/local/opt/zlib/include  -c -o Release/obj.target/png2bmp/cpp/lodepng.o ../cpp/lodepng.cpp
Release/obj.target/png2bmp/cpp/lodepng.o: ../cpp/lodepng.cpp \
  ../cpp/lodepng.h
../cpp/lodepng.cpp:
../cpp/lodepng.h: