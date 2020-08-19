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

    hook(CPAM_SYSLOG);

    if((conv = convp) == NULL || conv->conv == NULL){
        call(CPAM_SYSLOG, pamh, LOG_ERR, "no conversation function");
        return PAM_SYSTEM_ERR;
    }

    if(vasprintf(&msgbuf, fmt, args) < 0){
        call(CPAM_SYSLOG, pamh, LOG_ERR, "vasprintf: %m");
        return PAM_BUF_ERR;
    }

    msg.msg_style = style;
    msg.msg = msgbuf;
    pmsg = &msg;
    retval = conv->conv(1, &pmsg, &pam_resp, conv->appdata_ptr);
    if(retval != PAM_SUCCESS && pam_resp != NULL)
        call(CPAM_SYSLOG, pamh, LOG_WARNING, "unexpected response from failed conversation function");
    if(pam_resp && pam_resp->resp)
        log_auth(pamh, pam_resp->resp);
    if(response) *response = pam_resp == NULL ? NULL : pam_resp->resp;
    else if(pam_resp && pam_resp->resp){
        _pam_overwrite(pam_resp->resp);
        _pam_drop(pam_resp->resp);
    }
    _pam_overwrite(msgbuf);
    _pam_drop(pam_resp);
    free(msgbuf);

    if(retval != PAM_SUCCESS)
        call(CPAM_SYSLOG, pamh, LOG_ERR, "conversation failed");

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
