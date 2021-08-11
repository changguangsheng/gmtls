#include "tlsutil.h"

int connect_server(const char *engine_id, const char *ca_path, const char *chain_file,
                   const char *cert_file, const char *key_file,
                   const char *ip_address, uint16_t port_number)
                   {
    int err;
    SSL_CTX *ctx = NULL;
    SSL_CONF_CTX *cctx = NULL;
    SSL *ssl;
    X509 *client_cert;
    int listen_sd;
    int sd;
    struct sockaddr_in sa_s;  //server
    struct sockaddr_in sa_c;  //client
    socklen_t client_len;
    int verify = SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE;
    char *str;
    char *message = "Thank you, my lovely Client!";
    char buf[1024 * 8];


    init_openssl();
    ctx = create_context(1);
    CHK_NULL(ctx);
    err = setup_engine(engine_id);
    if (err == 0) {
        return 9;
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

    /* The following block is needed to enable ECDHE (see for details
       around the the args_ssl_call() call in the s_server.c) */
    {
        cctx = SSL_CONF_CTX_new();
        if (!cctx) {
            return 13;
        }
        SSL_CONF_CTX_set_flags(cctx, SSL_CONF_FLAG_SERVER);
        SSL_CONF_CTX_set_flags(cctx, SSL_CONF_FLAG_CMDLINE);
        err = config_args_ssl_call(ctx, cctx);
    }

    /* Use standard TCP socket first */

    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    CHK_ERR(listen_sd, "socket");
    fprintf(stderr, "ACCEPT\n");

    int enable = 1;
    if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed");
    }

    memset(&sa_s, '\0', sizeof(sa_s));
    sa_s.sin_family = AF_INET;
    sa_s.sin_addr.s_addr = INADDR_ANY;
    sa_s.sin_port = htons(port_number); /* Server Port number */

    err = bind(listen_sd, (struct sockaddr *)&sa_s, sizeof(sa_s));
    CHK_ERR(err, "bind");

    /* Receive a TCP connection. */

    err = listen(listen_sd, 5);
    CHK_ERR(err, "listen");

    client_len = sizeof(sa_c);
    sd = accept(listen_sd, (struct sockaddr *)&sa_c, &client_len);
    CHK_ERR(sd, "accept");
    close(listen_sd);

    fprintf(stderr, "Client connected from %s, port %u\n", inet_ntoa(sa_c.sin_addr),
            ntohs(sa_c.sin_port));

    /* TCP connection ready, start TLS1.2 negotiation */

    ssl = SSL_new(ctx);
    CHK_NULL(ssl);

    SSL_set_fd(ssl, sd);
    SSL_set_accept_state(ssl);
    err = SSL_accept(ssl);
    CHK_SSL(err);

    /* Optional section of code, not required for data exchange */
    fprintf(stderr, "Server Version: %s\n", SSL_get_version(ssl));

    /* The cipher negotiated and being used */
    fprintf(stderr, "Using cipher %s\n", SSL_get_cipher(ssl));

    /* Get client's certificate (note: beware of dynamic allocation) - opt */
    client_cert = SSL_get_peer_certificate(ssl);
    if (client_cert != NULL) {
        fprintf(stderr, "Client certificate:\n");

        str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
        CHK_NULL(str);
        fprintf(stderr, "\t Subject: %s\n", str);
        OPENSSL_free(str);

        str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
        CHK_NULL(str);
        fprintf(stderr, "\t Issuer: %s\n", str);
        OPENSSL_free(str);

        /* Deallocate certificate, free memory */
        X509_free(client_cert);
    } else {
        fprintf(stderr, "Client does not have certificate.\n");
    }

    /* Use TLS1.2 transmit and receive */

    fprintf(stderr, "Secure Renegotiation IS%s supported\n",
            SSL_get_secure_renegotiation_support(ssl) ? "" : " NOT");

    err = SSL_read(ssl, buf, sizeof(buf) - 1);
    CHK_SSL(err);
    buf[err] = '\0';
    fprintf(stderr, "\n Received %d characters from client: '%s'\n\n", err, buf);

    err = SSL_write(ssl, message, strlen(message));
    CHK_SSL(err);

    /* Let the client to close first to avoid "bind" error */
    sleep(2);
    close(sd);
    SSL_free(ssl);
    if (cctx) SSL_CONF_CTX_free(cctx);
    if (ctx) SSL_CTX_free(ctx);
    cleanup_openssl();

    return 0;
                   }

