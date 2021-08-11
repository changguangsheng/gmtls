#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/netfilter_ipv4.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/types.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>

#define BACKLOG 15	//最大监听数
#define FREE(x) if(x){free(x);x=NULL;}
#define BLOCKSIZE 65536


#define MAXBUF 4096


typedef struct threadinfo_t{
    int ClientFD;
    socklen_t socklen;
    struct sockaddr_in* stClientAddr;
} threadinfo;

SSL_CTX *ctx;
int isRun = 1;//运行标志
int num = 0; //线程数
char *addr ;
int portc ;

char *sigCrt;
char *sigKey;
char *encCrt;
char *encKey;
char *ca_file;

void Stop(int signo){
    printf("停止\n");
    isRun=0;
}

int config_ssl_stc(SSL_CTX *ctx1){
// 是否要求校验对方证书 若不验证客户端身份则设置为： SSL_VERIFY_NONE
//    SSL_CTX_set_verify(ctx1, SSL_VERIFY_NONE, NULL);
    SSL_CTX_set_verify(ctx1 ,SSL_VERIFY_PEER,NULL);
    //若验证,则放置CA证书
    int ret = SSL_CTX_load_verify_locations(ctx, ca_file, NULL);
    if (ret < 0)
    {
        printf("SSL_CTX_load_verify_locations failed.");
    }
    SSL_CTX_set_default_passwd_cb_userdata(ctx ,"12345678");
    //载入服务端数字签名证书
    if (SSL_CTX_use_certificate_file(ctx1, sigCrt, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        return(1);
    }
    //载入服务端签名私钥
    if (SSL_CTX_use_PrivateKey_file(ctx1, sigKey, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stdout);
        return(1);
    }
    //检查用户私钥是否正确
    if (!SSL_CTX_check_private_key(ctx1)){
        ERR_print_errors_fp(stdout);
        return(1);
    }

    //载入服务端加密证书
    if (SSL_CTX_use_certificate_file(ctx1, encCrt, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        return(1);
    }
    //载入加密私钥
    if (SSL_CTX_use_PrivateKey_file(ctx1, encKey, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stdout);
        return(1);
    }
    //检查用户私钥是否正确
    if (!SSL_CTX_check_private_key(ctx1)){
        ERR_print_errors_fp(stdout);
        return(1);
    }

    return 0;

}

int server_socket_init(){

    struct sockaddr_in serveraddr;
    struct hostent *host;
    if((host = gethostbyname(addr))==0){
        printf("Error resolving host %s\n", addr);
        exit(1);
    }
    char *ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
//    创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port  = htons(portc);
    int ok = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(ok != 0){
        printf("connnect failed\n");
        exit(0);
    }
    return  sockfd;
}


int socket_init(int port){
    int fd = socket(AF_INET , SOCK_STREAM , 0);
    if(fd < 0){
        printf("create socket filed\n");
        exit(1);
    }
    //本地地址信息
    struct sockaddr_in stLocalAddr = {0};
    stLocalAddr.sin_family = AF_INET; //概述行表示接收本机或其他机器传输
    stLocalAddr.sin_port = htons(port);//端口号
    stLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP,括号表示本机ip
    memset(stLocalAddr.sin_zero,0,sizeof(stLocalAddr.sin_zero));
    int n = 1;
    int err = setsockopt(fd , SOL_SOCKET , SO_REUSEADDR ,&n , sizeof(int));
    if(err == -1){
        perror("setsockopt error\n");
        exit(1);
    }
    //绑定地址结构体和socket
    if( bind(fd,(struct sockaddr *)(&stLocalAddr),sizeof(stLocalAddr)) <0){
        perror("bind error\n");
        exit(1);
    }
    //开启监听 , 第二参数是最大监听数
    if(listen(fd , BACKLOG)  < 0){
        perror("listen error\n");
        exit(1);
    }
    printf("My Socket is Ready\n");
    printf("localfd:%d\n",fd);
    return  fd;
}


int ssl_init(int port){
    // SSL init
    SSL_library_init();
    //加载SSL错误信息
    SSL_load_error_strings();
    //    Gmssl双证书通信
    const SSL_METHOD * meth =  GMTLS_server_method();
    // GM create CTX
    ctx = SSL_CTX_new(meth);
    if (!ctx) {
        printf("create ctx is failed.\n");
        exit(1);
    }
//    验证证书
    if(config_ssl_stc(ctx) != 0){
        printf( "config_ssl_ctx error!\n" );
        SSL_CTX_free(ctx);
        getchar();
        exit(1);
    }
//    创建socket接口
    int sockfd = socket_init(port);
    //    TCP连接已经建立，将连接交付给SSL

    return sockfd;
}

void *deal(void* args){
    SSL *ssl = NULL;
    struct  threadinfo_t *newthread = (struct threadinfo_t *)args;
    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        printf("SSL_new error.\n");
        exit(0);
    }
//    创建ssl连接
    SSL_set_fd(ssl,newthread->ClientFD);
    if (SSL_accept(ssl) == -1) {
        printf("SSL_accept fail.\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    } else {
        printf("1.Accpet with %s encryption\n", SSL_get_cipher(ssl));
    }

//    连接的id
    int myid = ++num;

    printf("%3d 子线程 ClientFD %d\n" ,myid , newthread->ClientFD);

    /*连接服务器*/
    int ServerFD = server_socket_init();

    printf("建立服务器socket！ Serverfd %d\n",ServerFD);

    printf("代理向服务器建立连接！\n");

    int fdmax = ServerFD > newthread->ClientFD ? ServerFD : newthread->ClientFD;
    struct timeval timeout;
    fd_set fds;
    fd_set fdr;
    //开始监听服务器和客户端套接字
    while(isRun){

        FD_ZERO(&fdr);
        FD_SET(ServerFD,&fdr);
        FD_SET(newthread->ClientFD ,&fdr);

        timeout.tv_sec=10;
        timeout.tv_usec=0;
        switch (select(fdmax + 1 ,&fdr ,NULL ,NULL,&timeout)) {
            case 0:
                printf("%3d 10秒无数据\n", myid);
                break;
            case -1:
                printf("%3d select fail,exit\n", myid);
                isRun = 0;
                close(newthread->ClientFD);
                num--;
                break;
            default:
                //如果客户端发送消息
                if(FD_ISSET(newthread->ClientFD ,&fdr)){
                    unsigned char buf[BLOCKSIZE] = { 0 };
                    int rdata = 1;
                    int sdata = 1;
                    int ret, ret2;
                    unsigned char* upstream_data = NULL;
                    unsigned int upstream_size = 0;

                   /* 接收数据*/
                    while ((rdata)){
                        memset(buf, 0, BLOCKSIZE);
//                        ssl读数据
                        ret = SSL_read(ssl , buf, BLOCKSIZE);
                        if (ret == BLOCKSIZE) {
                            printf("从客户端 %3d 接收%d的数据 \n%s\n",myid,ret ,buf);
                            unsigned int upstream_size_b = upstream_size;
                            upstream_size += BLOCKSIZE;
                            upstream_data = realloc(upstream_data, upstream_size);
                            memcpy(upstream_data + upstream_size_b, buf, BLOCKSIZE);
                        }else{
                            printf("从客户端 %3d 接收%d的数据 \n%s\n",myid,ret ,buf);
                            if (ret < 0) {
                                printf("%d %d %s cli fin \n", myid, ret, buf);
                                close(newthread->ClientFD);
                                newthread->ClientFD = -1;
                            }
                            rdata = 0;
                            unsigned int upstream_size_b = upstream_size;
                            upstream_size += ret;
                            /*重新调整内存大小*/
                            upstream_data = realloc(upstream_data, upstream_size);
                            if(upstream_data==NULL){
                                printf("\n\n\n%3d 内存不足，线程退出!\n\n\n",myid);
                                goto clo;
                            }
                            /*realloc*/
                            memcpy(upstream_data + upstream_size_b, buf, ret);
                        }
                    }
                    printf("接收客户端完成\n%s\n",upstream_data);
                    /*报文接收完成,开始解析*/
                    //upstream_data是报文
                    //upstream_size是大小

                    if(ret > 0){
                        FD_ZERO(&fds);
                        FD_SET(ServerFD, &fds);
                        timeout.tv_sec = 10;
                        if (select(fdmax + 1, NULL, &fds, NULL, &timeout) > 0) {
                            if (FD_ISSET(ServerFD, &fds)) {
                                printf("将解密发送给服务端%u的数据\n%s\n",upstream_size,upstream_data);
                                printf("up:%lu\n", strlen(upstream_data));
                                ret2 = send(ServerFD, upstream_data, upstream_size, 0);
                                printf("ret2:%d\n",ret2);
                                if (ret2  != upstream_size) {
                                    printf("%3d 解密发送数据失败\n",myid);
                                } else {
                                    printf("%3d 发送给服务端 %d data\n",myid ,ret2);
                                }
                            }
                        }else{
                            printf("%3d close ser fin \n", myid);
                            close(ServerFD);
                            ServerFD = -1;
                            FREE(upstream_data);
                            upstream_data = NULL;
                            goto clo;
                        }
                    }else{
                        printf("%3d close ser fin \n", myid);
                        close(ServerFD);
                        ServerFD = -1;
                        FREE(upstream_data);
                        upstream_data = NULL;
                        goto clo;
                    }
                    FREE(upstream_data);
                    upstream_data = NULL;

                }else if(FD_ISSET(ServerFD,&fdr)){
                    char buf[BLOCKSIZE] = {0};
                    int rdata = 1;
                    int ret  , ret2;
                    /*从服务端接收数据*/
                    while(rdata){
                        memset(buf, 0, BLOCKSIZE);
                        ret = recv(ServerFD, buf, BLOCKSIZE, 0);

                        if (ret == BLOCKSIZE) {
                            printf("从服务器端%3d接收%d的数据\n%s",myid ,ret ,buf);
                        }else{
                            printf("从服务器端%3d接收%d的数据\n%s",myid ,ret ,buf);
                            if (ret <= 0) {
                                printf("%d ser fin %d  \n%s \n",myid, ret, buf);
                                close(ServerFD);
                                ServerFD = -1;
                            }
                            rdata = 0;
                        }
//                        从服务端接收数据完成
                        if(ret > 0){
                            FD_ZERO(&fds);
                            FD_SET(newthread->ClientFD, &fds);
                            timeout.tv_sec=10;

                            if(select(fdmax+1,NULL,&fds,NULL,&timeout)>0){
                                if (FD_ISSET(newthread->ClientFD, &fds)) {
/*
                                    SSL_set_fd(ssl,newthread->ClientFD);
                                    if (SSL_accept(ssl) == -1) {
                                        printf("SSL_accept fail.\n");
                                        ERR_print_errors_fp(stderr);
                                        exit(1);
                                    } else {
                                        printf("1.Accpet with %s encryption\n", SSL_get_cipher(ssl));
                                    }
*/
                                    ret2 = SSL_write(ssl , buf ,ret );
                                    if (ret2 != ret) {
                                        printf("%d发送给客户端失败\n",myid);
                                    }else {
                                        printf("%3d发送给客户端%d的数据",myid,ret2);
                                    }
                                }
                            }else{
                                printf("%3d closed cli fin !!! \n",myid);
                                close(newthread->ClientFD);
                                newthread->ClientFD = -1;
                                goto clo;
                            }
                        }else{
                            printf("%d close cli fin \n",myid);
                            close(newthread->ClientFD);
                            newthread->ClientFD = -1;
                            goto clo;
                        }
                    }
                }else{
                    printf("%3d cli & ser closed ,service to new cli\n",myid);
                    close(ServerFD);
                    close(newthread->ClientFD);
                    newthread->ClientFD = -1;
                    ServerFD = -1;
                    goto clo;
                }
                break;
                //printf("%3d 有数据\n", myid);
        }
    }
    clo:
    if(ServerFD > 0) close(ServerFD);
    if(newthread->ClientFD > 0) close(newthread->ClientFD);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    FREE(newthread->stClientAddr);
    FREE(newthread);
    num--;
    return 0;
}

int main(int argc, char *argv[]){
    //获取地址信息
    if (argc < 4) {
        printf("./%s ports addr portc\n", argv[0]);
        exit(1);
    }

    //signal 信号处理
    signal(SIGINT , Stop);
    signal(SIGTERM ,Stop);
    signal(SIGPIPE , SIG_IGN);//防止有连接broken pipe导致程序退出

    //服务端监听端口
    int ports = strtol(argv[1], NULL, 0);
    //转发服务器的地址 和 端口
    addr = argv[2];
    portc = strtol(argv[3], NULL, 0);
    ca_file = argv[5];
    sigCrt = argv[6];
    sigKey = argv[7];
    encCrt = argv[8];
    encKey = argv[9];

//    创建ssl
    int localfd  = ssl_init(ports);

    //一组文件描述符的集合
    fd_set  fda;
    struct  timeval timeout;

    while(isRun){
        //将指定的文件描述符集清空 ,在对文件描述符集合进行设置前
        FD_ZERO(&fda);
        // 在文件描述符集合中增加一个新的文件描述符
        FD_SET(localfd ,&fda);
        //监控时间
        timeout.tv_sec = 10;

        if(select( localfd +1 , &fda , NULL ,NULL ,&timeout) > 0){

            if(FD_ISSET(localfd ,&fda)){
                struct  threadinfo_t* newthread = (struct  threadinfo_t*)malloc(sizeof (struct  threadinfo_t));

                //客户端地址信息
                memset(newthread ,0, sizeof(struct  threadinfo_t));
                newthread->stClientAddr= malloc(sizeof(struct sockaddr_in));//客户端地址信息
                memset(newthread->stClientAddr, 0, sizeof(struct sockaddr_in));

                newthread->socklen = 0;
                //接收连接
                newthread->ClientFD = accept(localfd , (void  *)newthread->stClientAddr ,&newthread->socklen);
                if(newthread->ClientFD < 0){

                    FREE(newthread->stClientAddr);
                    FREE(newthread);
                    printf("接收失败\n");
                    return -1;
                }else {

                    //启动子线程
                    pthread_t pt;
                    pthread_create(&pt, NULL, deal, newthread);
                    pthread_detach(pt);
                }
            }
        }

    }

    close(localfd);
    while(num>0){
        printf("剩余 %d 个\n",num);
        sleep(1);
    }
    return 0;
}

