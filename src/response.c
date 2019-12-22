#include <string.h>
#include "response.h"
#include "utility.h"
#include "list.h"
#include "kv.h"

Response *responseNew()
{
    Response *response = malloc(sizeof(Response));
    response->status = OK;
    response->headers = listNew();
    response->body = "";
    return response;
}

void responseAddHeader(Response *response, KV *header)
{
    listAppend(response->headers, listCellNew(header, sizeof(KV)));
}

void responseSetStatus(Response *response, Status status)
{
    response->status = status;
}

void responseSetBody(Response *response, char *body)
{
    response->body = body;
}

void responseSetStatusLength(Response *response, size_t length)
{
    response->statusLength = length;
}

void responseSetHeaderLength(Response *response, size_t length)
{
    response->headerLength = length;
}

void responseSetContentLength(Response *response, size_t length)
{
    response->contentLength = length;
}

// Example: "HTTP/1.0 200 OK\r\nHost: 140.114.234.148\r\n\r\ngoodgood"
char* responsePacket(Response *response)
{
    char *packet = "";
    // Status
    if (response->status == OK) packet = "HTTP/1.0 200 OK\r\n";
    else if (response->status == MOVE_PERMANENTLY) packet = "HTTP/1.0 200 Move Permanently\r\n";
    else if (response->status == FORBIDDEN) packet = "HTTP/1.0 403 Forbidden\r\n";
    else if (response->status == NOT_FOUND) packet = "HTTP/1.0 404 Not Found\r\n";
    responseSetStatusLength(response, strlen(packet));

    // Headers
    ListCell *current = (response->headers)->head;
    while(current != NULL) {
        char *key = ((KV*)(current->value))->key;
        char *value = ((KV*)(current->value))->value;
        int keySize = strlen(key);
        int valueSize = strlen(value);
        char *buffer = malloc(keySize + valueSize + 4);
        memcpy(buffer, key, keySize);
        memcpy(buffer+keySize, ": ", 2);
        memcpy(buffer+keySize+2, value, valueSize);
        memcpy(buffer+keySize+2+valueSize, "\r\n", 2);
        packet = concat(packet, buffer);
        free(buffer);
        current = current->next;
    }
    packet = concat(packet, "\r\n");
    responseSetHeaderLength(response, strlen(packet) - (response->statusLength));
    
    // response body
    char *packetWithBody = malloc(strlen(packet) + response->contentLength);
    int packetLength = strlen(packet);
    memcpy(packetWithBody, packet, packetLength);
    memcpy(packetWithBody+packetLength, response->body, response->contentLength);
    return packetWithBody;
}

char* findMimeType(char *filename)
{
    List *dotSplits = split(filename, ".");
    char *ext = (listGet(dotSplits, (dotSplits->count)-1)->value);
    if (!strncmp(ext, "html", 4)) return "text/html";
    else if (!strncmp(ext, "htm", 3)) return "text/html";
    else if (!strncmp(ext, "txt", 3)) return "text/plain";
    else if (!strncmp(ext, "css", 3)) return "text/css";
    else if (!strncmp(ext, "gif", 3)) return "image/gif";
    else if (!strncmp(ext, "jpg", 3)) return "image/jpeg";
    else if (!strncmp(ext, "png", 3)) return "image/png";
    else if (!strncmp(ext, "bmp", 3)) return "image/x-ms-bmp";
    else if (!strncmp(ext, "doc", 3)) return "application/msword";
    else if (!strncmp(ext, "pdf", 3)) return "application/pdf";
    else if (!strncmp(ext, "mp4", 3)) return "video/mp4";
    else if (!strncmp(ext, "swf", 3)) return "application/x-shockwave-flash";
    else if (!strncmp(ext, "swfl", 4)) return "video/mp4";
    else if (!strncmp(ext, "ogg", 3)) return "audio/ogg";
    else if (!strncmp(ext, "bz2", 3)) return "application/x-bzip2";
    else if (!strncmp(ext, "gz", 2)) return "application/x-gzip";
    else if (!strncmp(ext, "tar.gz", 5)) return "application/x-gzip";
    else return "text/plain";
}