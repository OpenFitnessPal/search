#include "mongoose.h"

#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <time.h>

char *port = "8888";

static struct mg_http_serve_opts s_http_server_opts;

char *exec_command(const char *cmd) {
        FILE *fp;
        char *path = malloc(1024 * 1000);

        /* Open the command for reading. */
        fp = popen(cmd, "r");
        if (fp == NULL) {
                printf("Failed to run command\n");
                exit(1);
        }

        /* Read the output a line at a time - output it. */
        fgets(path, 1000 * 1024, fp);

        /* close */
        pclose(fp);

        return path;
}

void handle_url_req(struct mg_connection *nc, char *query) {
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) p;

        struct mg_str uri = hm->uri;

        printf("%s\n", uri.buf);

        char *ur = malloc(uri.len + 1);
        snprintf(ur, uri.len + 1, "%s", uri.buf);

        char *u = malloc(1024);
        mg_url_decode(ur, uri.len + 1, u, 1024, 0);

        u++;

        char *command = malloc(24 + strlen(u));

        // All syntax, queries, etc. are handled by this script
        snprintf(command, 24 + strlen(u), "python3 search.py %s", u);
        printf("%s\n", command);
        mg_http_reply(nc, 200, "", "%s", exec_command(command));
    }
}

int main(int argc, char *argv[]) {
    struct mg_mgr mgr;

    mg_mgr_init(&mgr);
    printf("Starting web server on port %s\n", port);
    char *str_port = malloc(20);
    sprintf(str_port, "http://0.0.0.0:%s", port);
    mg_http_listen(&mgr, str_port, ev_handler, NULL);

    for (;;) { mg_mgr_poll(&mgr, 1000); }
    mg_mgr_free(&mgr);
    return 0;
}

