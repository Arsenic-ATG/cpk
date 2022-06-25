
#include <curl/curl.h>
#include <string>

void print_curl_protocols()
{
    printf("curl version: %s\n", curl_version());
    const char *const *proto;
    curl_version_info_data *curlinfo = curl_version_info(CURLVERSION_NOW);
    for(proto = curlinfo->protocols; *proto; proto++)
    {
        printf("curl support protocol: %s\n", *proto);
    }
}

size_t FileWriteData(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void DownloadFile(const char* url, const char* output_file)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        printf("curl inited\n");
        fp = fopen(output_file, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FileWriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        else
            printf("downloaded %s\n", output_file);

        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

size_t WriteStringData(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string SendPostRequest(const char* url, const std::string& jsonString = "{\"username\":\"bob\",\"password\":\"12345\"}")
{
    CURLcode ret;
    CURL *curl;
    struct curl_slist *slist1;
    std::string readBuffer;

    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");

    curl = curl_easy_init();
    if (!curl) {
        return readBuffer;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.38.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStringData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    ret = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_slist_free_all(slist1);
    return readBuffer;
}