cmd_Release/primes.node := c++ -bundle -stdlib=libc++ -undefined dynamic_lookup -Wl,-no_pie -Wl,-search_paths_first -mmacosx-version-min=10.7 -arch x86_64 -L./Release -stdlib=libc++ -L/usr/local/opt/zlib/lib -o Release/primes.node Release/obj.target/primes/cpp/prime.o 
