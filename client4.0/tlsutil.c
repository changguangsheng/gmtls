#include "tlsutil.h"

int verify_depth = 0;
int verify_quiet = 0;
int verify_error = X509_V_OK;
int verify_return_error = 0;

/**
 *
 * \brief setup OpenSSL engine by engine ID
 *
 * \param[in] engine_id Engine ID (just return if NULL)
 * \return 1 for success, 0 for error
 */
int setup_engine(const char *engine_id){

#ifndef OPENSSL_NO_ENGINE
    ENGINE *e = NULL;

    if (engine_id == NULL) {
        return 1;
    }
    ENGINE_load_builtin_engines();
    e = ENGINE_by_id(engine_id);
    if (!e) {
        fprintf(stderr, "FAILED to load engine: %s\n", engine_id);
        return 0;
    }
    if (!ENGINE_set_default(e, ENGINE_METHOD_ALL)) {
        fprintf(stderr, "FAILED to set default engine: %s\n", engine_id);
        ENGINE_free(e);
        return 0;
    }
#endif // OPENSSL_NO_ENGINE
    return 1;
}

/**
 *
 * \brief Calls OpenSSL standard initialize methods
 *
 */
void init_openssl(void){
    SSL_library_init();
    //加载SSL错误信息
    SSL_load_error_strings();
}

/**
 *
 * \brief Creates the SSL context for server or
 * client
 *
 * \param[in] is_server 1 - for server, 0 - for client
 * \return a pointer to SSL_CTX for success, NULL for error
 */
SSL_CTX* create_context(uint32_t is_server){

    const SSL_METHOD *method = NULL;
    SSL_CTX *ctx = NULL;

    if (is_server) {
//        method = TLSv1_2_server_method();
        method = GMTLS_server_method();
    } else {
//        method = TLSv1_2_client_method();
        method = GMTLS_client_method();
    }
    if (!method) {
        goto done;
    }

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        fprintf(stderr, "tlsutil:create_context() cannot create SSL context\n");
        goto done;
    }
    done:
    return ctx;
}

/**
 *
 * \brief Configures the SSL context for server or
 * client using provided certificates, chain files, and private
 * keys (ATECCX08 token are encoded into the OpenSSL private key
 * files)
 *
 * \param[in] ctx SSL context
 * \param[in] ca_path Path to CA (Certificate Authority)
 * \param[in] chain_file Chain File Name (Certificate Bundle)
 * \param[in] cert_file Certificate File Name
 * \return 1 for success
 */
int configure_context(SSL_CTX *ctx, const char *ca_path, const char *chain_file,
                      const char *cert_file){
    int rc = 0;

    /* Compression should not be used: there are CRIME and BREACH attacks
       that leverage HTTP compression */
    if (!SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION)) {
        fprintf(stderr, "SSL_CTX_set_options(SSL_OP_NO_COMPRESSION) error\n");
        goto done;
    }
    //证根证书
    if (SSL_CTX_load_verify_locations(ctx, chain_file, ca_path) <= 0) {
        ERR_print_errors_fp(stderr);
        goto done;
    }

    if (SSL_CTX_use_certificate_chain_file(ctx, chain_file) <= 0) {
        ERR_print_errors_fp(stderr);
        goto done;
    }
    /*
     * See SSL_CTX_use_certificate_chain_file() in the ssl/ssl_rsa.c file as
     * a reference if we get the chain from the hardware without saving to
     * a file
     */

    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "SSL_CTX_use_certificate_file() error\n");
        ERR_print_errors_fp(stderr);
        goto done;
    }
    if (!SSL_CTX_set_default_verify_paths(ctx)) {
        ERR_print_errors_fp(stderr);
        goto done;
    }

    rc = 1;
    done:
    return (rc);
}

/**
 *
 * \brief setup OpenSSL engine by engine ID
 *
 * \param[in] engine_id Engine ID
 * \param ctx[in] SSL context
 * \param key_file[in] Private Key File Name
 * \return 1 for success, 0 for error
 */

int load_private_key(const char *engine_id, SSL_CTX *ctx, const char *key_file){
    int rc = 0;
    ENGINE *e = NULL;
    EVP_PKEY *pkey = NULL;

    if (engine_id) {
        e = ENGINE_by_id(engine_id);
        pkey = ENGINE_load_private_key(e, key_file, NULL, NULL);
        if (NULL == pkey) {
            fprintf(stderr, "load_private_key(): pkey is NULL\n");
            goto done;
        }
        rc = SSL_CTX_use_PrivateKey(ctx, pkey);
        EVP_PKEY_free(pkey);
    } else {
        if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
            fprintf(stderr, "SSL_CTX_use_PrivateKey_file() error");
            ERR_print_errors_fp(stderr);
            goto done;
        }
        /*
         * The SSL_CTX_use_PrivateKey_file() calls
         * ret = SSL_CTX_use_PrivateKey(ctx, pkey);
         * Can be used instead if we get the private key from the hardware and use it immediately.
         * Openssl engine structure provides hooks to load_privkey() and load_pubkey() functions.
         * There are no hooks for certificates.
         */
    }

    rc = SSL_CTX_check_private_key(ctx);
    if (!rc) {
        fprintf(stderr, "Private key does not match public key in certificate\n");
        goto done;
    }
    done:
    return (rc);

}

/**
 *
 * \brief A modification of the args_ssl_call()
 * function from the openssl aps/s_cb.c file
 *
 * \param[in] ctx SSL context
 * \param[in, out] cctx SSL_CONF_CTX
 * \return 0 for success
 */
int config_args_ssl_call(SSL_CTX *ctx, SSL_CONF_CTX *cctx){
    SSL_CONF_CTX_set_ssl_ctx(cctx, ctx);
    /*
     * This is a special case to keep existing s_server functionality: if we
     * don't have any curve specified *and* we haven't disabled ECDHE then
     * use P-256.
     */
    if (SSL_CONF_cmd(cctx, "-named_curve", "P-256") <= 0) {
        fprintf(stderr, "Error setting EC curve\n");
        return 0;
    }
    if (!SSL_CONF_CTX_finish(cctx)) {
        fprintf(stderr, "Error finishing context\n");
        return 0;
    }
    return 1;
}

/**
 *
 * \brief A modification of the verify_callback()
 * function from the openssl aps/s_cb.c file
 *
 * \param[in] ok a parameter allowint to exit from the callback
 *       immediately
 * \param[in, out] ctx X509_STORE_CTX
 * \return 0 for success
 */
int verify_callback(int ok, X509_STORE_CTX *ctx){
    X509 *err_cert;
    int err;
    int depth;
    char *str;

    err_cert = X509_STORE_CTX_get_current_cert(ctx);
    err = X509_STORE_CTX_get_error(ctx);
    depth = X509_STORE_CTX_get_error_depth(ctx);

    if (!ok) {
        fprintf(stderr, "depth=%d ", depth);
        if (err_cert) {
            str = X509_NAME_oneline(X509_get_subject_name(err_cert), 0, 0);
            CHK_NULL(str);
            fprintf(stderr, "\t Subject: %s\n", str);
            OPENSSL_free(str);
        } else fprintf(stderr, "<no cert>\n");
    }
    if (!ok) {
        fprintf(stderr, "verify error:num=%d:%s\n", err,
                X509_verify_cert_error_string(err));
        if (verify_depth >= depth) {
            if (!verify_return_error) ok = 1;
            verify_error = X509_V_OK;
        } else {
            ok = 0;
            verify_error = X509_V_ERR_CERT_CHAIN_TOO_LONG;
        }
    }
    switch (err) {
        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
            str = X509_NAME_oneline(X509_get_issuer_name(err_cert), 0, 0);
            CHK_NULL(str);
            fprintf(stderr, "\t Issuer: %s\n", str);
            OPENSSL_free(str);
            break;
        case X509_V_ERR_CERT_NOT_YET_VALID:
        case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
            fprintf(stderr, "notBefore invalid\n");
            break;
        case X509_V_ERR_CERT_HAS_EXPIRED:
        case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
            fprintf(stderr, "notAfter invalid\n");
            break;
        case X509_V_ERR_NO_EXPLICIT_POLICY:
            if (!verify_quiet) {
                fprintf(stderr, "No explicit policy\n\n");
            }
            break;
    }
    if (ok && !verify_quiet) {
        fprintf(stderr, "verify return:%d\n", ok);
    }
    return (ok);
}


/**
 *
 * \brief Call OpenSSL standard cleanup methods
 *
 */
void cleanup_openssl()
{
    EVP_cleanup();
}

/**
 *
 * Saves unencrypted private key in the PEM format. There is no
 * reason to use password/encryption for keys tht are already in
 * the hardware. Here we expect a pointer to the key, not a real
 * key
 *
 * \param[in] pkey pointer to public/private key structure
 *             (private key may be just a tocken, pointing to
 *             the hardware)
 * \param[in] privkey_fname Private Key File Name
 * \return 1 for success
 */
int save_private_key(EVP_PKEY *pkey, const char *privkey_fname)
{
    int rc = 0;
    FILE *fd = NULL;

    fd = fopen(privkey_fname, "wb");
    if (fd == NULL) {
        goto done;
    }

    rc = PEM_write_PrivateKey(fd, pkey, NULL, NULL, 0, NULL, NULL);
    done:
    if (fd) {
        fclose(fd);
    }
    return (rc);
}

/**
 *
 * Saves a certificate in the PEM format
 *
 * \param[in] x509 pointer to X509 structure with certificate
 * \param[in] cert_fname Certificate file name
 * \return 1 for success
 */
int save_x509_certificate(X509 *x509, const char *cert_fname)
{
    int rc = 0;
    FILE *fd = NULL;

    fd = fopen(cert_fname, "wb");
    if (fd == NULL) {
        goto done;
    }

    rc = PEM_write_X509(fd, x509);
    done:
    if (fd) {
        fclose(fd);
    }
    return (rc);
}

/**
 *
 * Calls different engine commands by CMD ID
 *
 * \param[in] engine_id Engine ID (just return if NULL)
 * \param[in] cmd a command to pass to the engine library (for
 *            the list of commands see ecc_meth.h file
 * \param[in, out] buffer an optional buffer to pass into the
 *       command
 * \param[in, out] len the buffer size
 * \return 1 for success, 0 for error
 */
int run_engine_cmds(const char *engine_id, int cmd, char *buffer, int len)
{
    ENGINE *e = NULL;

    if (engine_id == NULL) {
        return 1;
    }
    e = ENGINE_by_id(engine_id);

    ENGINE_ctrl(e, cmd, len, buffer, 0);
    return 1;
}

