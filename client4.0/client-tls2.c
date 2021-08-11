
#include "tlsutil.h"
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
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <linux/netfilter_ipv4.h>
#include <pthread.h>
#include <openssl/err.h>
#include <openssl/ssl.h>



int connect_client(const char *engine_id, const char *ca_path, const char *chain_file, const char *cert_file, const char *key_file, const char *cipher_list,
                   const char *ip_address, uint16_t port_number){

    int err = 0;
    SSL_CTX *ctx = NULL;
    SSL *ssl;
    X509 *server_cert;
    int sd;
    struct sockaddr_in sa;
    char *str;
    int verify = SSL_VERIFY_PEER;
    char *message = "Clent sends Hello to Server!";
    char buf[1024 * 8];

    init_openssl();
    ctx = create_context(0);
    CHK_NULL(ctx);

    err = setup_engine(engine_id);
    if (err == 0) {
        return 9;
    }
    err = SSL_CTX_set_cipher_list(ctx, cipher_list);
    if (err == 0) {
        fprintf(stderr, "SSL_CTX_set_cipher_list() error\n");
        return 10;
    }

    SSL_CTX_set_verify(ctx, verify, verify_callback);

    err = configure_context(ctx, ca_path, chain_file, cert_file);
    if (err == 0) {
        return 11;
    }

    err = load_private_key(engine_id, ctx, key_file);
    if (err == 0) {
        return 13;
    }

    /* Use standard TCP socket first */

    sd = socket(AF_INET, SOCK_STREAM, 0);
    CHK_ERR(sd, "socket");

    memset(&sa, '\0', sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(ip_address); /* server IP address to connect to*/
    sa.sin_port = htons(port_number);           /* server port number */

    err = connect(sd, (struct sockaddr *)&sa, sizeof(sa));
    CHK_ERR(err, "connect");

    fprintf(stderr, "Connected to server %s, port %u\n", inet_ntoa(sa.sin_addr),
            ntohs(sa.sin_port));

    /* TCP connection ready, start TLS1.2 negotiation */

    ssl = SSL_new(ctx);
    CHK_NULL(ssl);

    SSL_set_fd(ssl, sd);
    SSL_set_connect_state(ssl);
    err = SSL_connect(ssl);
    CHK_SSL(err);

    /* Optional section of code, not required for data exchange */
    fprintf(stderr, "Client Version: %s\n", SSL_get_version(ssl));

    /* The cipher negotiated and being used */
    fprintf(stderr, "Using cipher %s\n", SSL_get_cipher(ssl));

    /* Get server's certificate */
    server_cert = SSL_get_peer_certificate(ssl);
    CHK_NULL(server_cert);
    fprintf(stderr, "Server certificate:\n");

    str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
    CHK_NULL(str);
    fprintf(stderr, "\t subject: %s\n", str);
    OPENSSL_free(str);

    str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
    CHK_NULL(str);
    fprintf(stderr, "\t issuer: %s\n", str);
    OPENSSL_free(str);

    /* Deallocate certificate, free memory */
    X509_free(server_cert);

    /* Use TLS1.2 transmit and receive */

    fprintf(stderr, "---\n");

    err = SSL_write(ssl, message, strlen(message));
    CHK_SSL(err);

    err = SSL_read(ssl, buf, sizeof(buf) - 1);
    CHK_SSL(err);
    buf[err] = '\0';
    fprintf(stderr, "\n Received %d characters from server: '%s'\n\n", err, buf);
    SSL_shutdown(ssl);

    close(sd);
    SSL_free(ssl);
    if (ctx) SSL_CTX_free(ctx);
    cleanup_openssl();

    return 0;
}
