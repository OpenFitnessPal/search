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

// You must free the result if result is non-NULL.
char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

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
        struct mg_str query = hm->query;

        char *q = malloc(query.len + 1);
        snprintf(q, query.len + 1, "%s", query.buf);

        char *newQ = str_replace(q, "&", "\\&");

        char *ur = malloc(uri.len + 1);
        snprintf(ur, uri.len + 1, "%s", uri.buf);

        char *u = malloc(1024);
        mg_url_decode(ur, uri.len + 1, u, 1024, 0);

        u++;

        if (strncmp(u, "favicon", 7) == 0) {
            mg_http_reply(nc, 404, "", "Not Found");
            free(ur);
            u--;
            free(u);
            free(q);
            free(newQ);
            return;
        }

        char *command = malloc(20 + strlen(u) + strlen(newQ));

        // All syntax, queries, etc. are handled by this script
        snprintf(command, 20 + strlen(u) + strlen(newQ), "python3 search.py %s?%s", u, newQ);
        printf("%s\n", command);
        char *res = exec_command(command);
        mg_http_reply(nc, 200, "", "%s", res);

        free(newQ);
        free(res);
        u--;
        free(u);
        free(ur);
        free(q);
        free(command);
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

