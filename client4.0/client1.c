#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAXBUF 4096

#define CA_CERT_FILE 		"GMCert_GMCA01.cert.pem"

int config_ssl_stc(SSL_CTX *ctx){
//    在上下文中设置标志以要求对等（服务器）证书验证
    SSL_CTX_set_verify(ctx , SSL_VERIFY_PEER ,NULL);
    int ret = SSL_CTX_load_verify_locations(ctx , CA_CERT_FILE, NULL);
    if(ret < 0){
        printf("SSL_CTX_load_verify_locations failed.");
    }

    SSL_CTX_set_default_passwd_cb_userdata(ctx ,"12345678");

    return 0;

}

int main(int argc, char *argv[]){

    struct sockaddr_in serveraddr;
    struct hostent *host;
    int sockfd, len;
    SSL *ssl = NULL;
    if (argc < 4) {
        printf("./%s addr port uri\n", argv[0]);
        exit(1);
    }
    char *addr = argv[1];
    int port = atoi(argv[2]);
    char *uri = argv[3];

    // SSL init
    SSL_library_init();

    //加载SSL错误信息
    SSL_load_error_strings();

//    Gmssl双证书通信
    const SSL_METHOD * meth =  GMTLS_client_method();

    // GM create CTX
    SSL_CTX *ctx = SSL_CTX_new(meth);

    if (!ctx) {
		printf("create ctx is failed.\n");
		exit(1);
    }

    if(config_ssl_stc(ctx) != 0){
        printf( "config_ssl_ctx error!\n" );
		SSL_CTX_free(ctx);
		getchar();
		exit(1);
    }

    if((host = gethostbyname(addr))==0){
     	printf("Error resolving host %s\n", addr);
        exit(1);
    }

    char *ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
//    创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;

    inet_pton(AF_INET, ip, &serveraddr.sin_addr.s_addr);

    serveraddr.sin_port  = htons(port);

    int ok = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    if(ok != 0){
        printf("connnect failed\n");
        exit(0);
    }
//    TCP连接已经建立，将连接交付给SSL
    ssl = SSL_new(ctx);

    if (ssl == NULL) {
		printf("SSL_new error.\n");
		exit(0);
    }
//    socket加入到ssl
    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) == -1) {
		printf("SSL_connect fail.\n");
		ERR_print_errors_fp(stderr);
		exit(1);
    }else {
		printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    }

    // send http request
    {
	char request[1024] = { 0 };
	sprintf(request, "GET %s HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/5.0\r\nHost: %s:%s\r\nConnection: close\r\n\r\n", argv[3], argv[1], argv[2]);
	len = SSL_write(ssl, request, strlen(request));
	if (len < 0){
		printf("send failed\n");
	}
	else
	{
		printf("send OK, %ld bytes:\n%s\n", strlen(request), request);
	}
    }

    // read http response
    {
	char buffer[MAXBUF + 1];
	memset(buffer, 0, MAXBUF + 1);
	len = SSL_read(ssl, buffer, MAXBUF);
	if (len > 0) {
		printf("read OK, %d bytes: \n%s\n", len, buffer);
	}
	else
	{
		printf("recv failed\n");
		goto finish;
	}
    }

finish:
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);

    return 0;
}

