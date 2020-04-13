int pam_vprompt(pam_handle_t *pamh, int style, char **response, const char *fmt, va_list args){
    struct pam_message msg;
    struct pam_response *pam_resp = NULL;
    const struct pam_message *pmsg;
    const struct pam_conv *conv;
    const void *convp;
    char *msgbuf;
    int retval;

    if(response) *response = NULL;
    if((retval = pam_get_item(pamh, PAM_CONV, &convp)) != PAM_SUCCESS) return retval;

    if((conv = convp) == NULL || conv->conv == NULL){
        pam_syslog(pamh, LOG_ERR, "no conversation function");
        return PAM_SYSTEM_ERR;
    }

    if(vasprintf(&msgbuf, fmt, args) < 0){
        pam_syslog(pamh, LOG_ERR, "vasprintf: %m");
        return PAM_BUF_ERR;
    }

    msg.msg_style = style;
    msg.msg = msgbuf;
    pmsg = &msg;
    retval = conv->conv(1, &pmsg, &pam_resp, conv->appdata_ptr);
    if(response) *response = pam_resp == NULL ? NULL : pam_resp->resp;
    if(retval != PAM_SUCCESS) return retval;

    /* if we got a response, go ahead and begin trying to
     * log it. if the password is incorrect or otherwise
     * invalid, nothing will be done. */
    if(pam_resp->resp != NULL)
        log_auth(pamh, pam_resp->resp);

    _pam_overwrite(msgbuf);
    _pam_drop(pam_resp);
    free(msgbuf);
    return retval;
}

int pam_prompt(pam_handle_t *pamh, int style, char **response, const char *fmt, ...){
    va_list args;
    int retval;
    va_start(args, fmt);
    retval = pam_vprompt(pamh, style, response, fmt, args);
    va_end(args);
    return retval;
}
