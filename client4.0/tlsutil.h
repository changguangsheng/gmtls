//
// Created by cgs on 2021/8/8.
//

#ifndef CLIENT_4_0_TLSUTIL_H
#define CLIENT_4_0_TLSUTIL_H

#endif //CLIENT_4_0_TLSUTIL_H
#ifndef SSLUTIL_H_
#define SSLUTIL_H_

#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/engine.h>

#include <engine_meth/ecc_meth.h>

#define EXCHANGE_VERSION         "1.1.0"
#define PORT_NUMBER_DEFAULT      (49917)

#define CHK_NULL(x) if ((x)==NULL) { sleep(1); exit (1); }
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); sleep(1); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); sleep(2); exit(2); }

int setup_engine(const char *engine_id);
void init_openssl(void);
SSL_CTX* create_context(uint32_t is_server);
int config_args_ssl_call(SSL_CTX *ctx, SSL_CONF_CTX *cctx);
int configure_context(SSL_CTX *ctx, const char *ca_path, const char *chain_file,
                      const char *cert_file);
int verify_callback(int ok, X509_STORE_CTX *ctx);
int load_private_key(const char *engine_id, SSL_CTX *ctx, const char *key_file);
void cleanup_openssl(void);

int connect_client(const char *engine_id, const char *ca_path, const char *chain_file,
                   const char *cert_file, const char *key_file, const char *cipher_list,
                   const char *ip_address, uint16_t port_number);
int connect_server(const char *engine_id, const char *ca_path, const char *chain_file,
                   const char *cert_file, const char *key_file,
                   const char *ip_address, uint16_t port_number);

int save_private_key(EVP_PKEY *pkey, const char *privkey_fname);
int save_x509_certificate(X509 *x509, const char *cert_fname);
int run_engine_cmds(const char *engine_id, int cmd, char *buffer, int len);


#endif /* SSLUTIL_H_ */