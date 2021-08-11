gcc -o client  -lm client1.c ./lib/libssl.a ./lib/libcrypto.a -I./lib/include -lpthread -ldl
