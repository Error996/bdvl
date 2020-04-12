int gen_cert(X509 **cert, EVP_PKEY **key){
    RSA *rsa;
    X509_NAME *subj;
    X509_EXTENSION *ext;
    X509V3_CTX ctx;
    BIGNUM *bn;
    const char *commonName = "localhost";
    char dNSName[128];
    int rc;

    *cert = NULL;
    *key = NULL;

    /* Generate a private key. */
    if((*key = EVP_PKEY_new()) == NULL) exit(0);

    bn = BN_new();
    BN_set_word(bn, RSA_F4);

    do{
        rsa = RSA_new();
        RSA_generate_key_ex(rsa, 2048, bn, NULL);
        if(rsa == NULL) exit(0);
        if((rc = RSA_check_key(rsa)) < 0) exit(0);
    }while(rc == 0);

    if(EVP_PKEY_assign_RSA(*key, rsa) == 0){
        RSA_free(rsa);
        exit(0);
    }

    /* Generate a certificate. */
    if((*cert = X509_new()) == NULL) exit(0);
    if(X509_set_version(*cert, 2) == 0) exit(0);

    /* Set the commonName. */
    subj = X509_get_subject_name(*cert);
    if(X509_NAME_add_entry_by_txt(subj, "commonName",
                                  MBSTRING_ASC, (unsigned char *)commonName,
                                  -1, -1, 0) == 0) exit(0);

    /* Set the dNSName. */
    rc = snprintf(dNSName, sizeof(dNSName) - 1, "DNS:%s", commonName);
    if(rc < 0 || rc >= sizeof(dNSName)) exit(0);

    X509V3_set_ctx(&ctx, *cert, *cert, NULL, NULL, 0);
    ext = X509V3_EXT_conf(NULL, &ctx, "subjectAltName", dNSName);
    if(ext == NULL) exit(0);
    if(X509_add_ext(*cert, ext, -1) == 0) exit(0);

    /* Set a comment. */
    ext = X509V3_EXT_conf(NULL, &ctx, "nsComment", CERTIFICATE_COMMENT);
    if(ext == NULL) exit(0);

    if(X509_add_ext(*cert, ext, -1) == 0) exit(0);

    X509_set_issuer_name(*cert, X509_get_subject_name(*cert));
    X509_gmtime_adj(X509_get_notBefore(*cert), 0);
    X509_gmtime_adj(X509_get_notAfter(*cert), DEFAULT_CERT_DURATION);
    X509_set_pubkey(*cert, *key);

    /* Sign it. */
    if(X509_sign(*cert, *key, EVP_sha1()) == 0) exit(0);

    return 1;
}

SSL_CTX *InitCTX(void){
    const SSL_METHOD *method = SSLv23_method();
    X509 *cert;
    EVP_PKEY *key;

    SSL_library_init();
    OpenSSL_add_all_algorithms();    /* Load cryptos, et.al. */
    SSL_load_error_strings();    /* Bring in and register error messages */
    if((ctx = SSL_CTX_new(method)) == NULL) abort();  /* Create new context */

    SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2);
    SSL_CTX_set_cipher_list(ctx, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

    if(gen_cert(&cert, &key) == 0) exit(0);
    if(SSL_CTX_use_certificate(ctx, cert) != 1) exit(0);
    if(SSL_CTX_use_PrivateKey(ctx, key) != 1) exit(0);

    X509_free(cert);
    EVP_PKEY_free(key);

    return ctx;
}