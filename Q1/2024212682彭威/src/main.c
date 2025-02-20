#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include <curl/curl.h>
#include <time.h>
typedef struct {
    char* image;
    char* container_name;
    char** volumes;
    int volumes_count;
    char** ports;
    int ports_count;
    char** environment;
    int environment_count;
    int auto_update;
} ContainerConfig;

void parse_yaml(const char* filename, ContainerConfig* config) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    yaml_parser_t parser;
    yaml_document_t document;

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize parser\n");
        fclose(file);
        return;
    }
    yaml_parser_set_input_file(&parser, file);

    if (!yaml_parser_load(&parser, &document)) {
        fprintf(stderr, "Failed to parse YAML document\n");
        yaml_parser_delete(&parser);
        fclose(file);
        return;
    }

    yaml_node_t* root = yaml_document_get_root_node(&document);
    if (root->type == YAML_MAPPING_NODE) {
        yaml_node_pair_t* pair = root->data.mapping.pairs.start;
        for (; pair < root->data.mapping.pairs.top; pair++) {
            yaml_node_t* key = yaml_document_get_node(&document, pair->key);
            yaml_node_t* value = yaml_document_get_node(&document, pair->value);
            if (key->type == YAML_SCALAR_NODE) {
                if (strcmp((char*)key->data.scalar.value, "image") == 0) {
                    config->image = strdup((char*)value->data.scalar.value);
                }
                else if (strcmp((char*)key->data.scalar.value, "container_name") == 0) {
                    config->container_name = strdup((char*)value->data.scalar.value);
                }
                else if (strcmp((char*)key->data.scalar.value, "volumes") == 0) {
                    if (value->type == YAML_SEQUENCE_NODE) {
                        config->volumes_count = value->data.sequence.items.top - value->data.sequence.items.start;
                        config->volumes = (char**)malloc(config->volumes_count * sizeof(char*));
                        yaml_node_item_t* item = value->data.sequence.items.start;
                        for (int i = 0; i < config->volumes_count; i++, item++) {
                            yaml_node_t* vol = yaml_document_get_node(&document, *item);
                            config->volumes[i] = strdup((char*)vol->data.scalar.value);
                        }
                    }
                }
                else if (strcmp((char*)key->data.scalar.value, "ports") == 0) {
                    if (value->type == YAML_SEQUENCE_NODE) {
                        config->ports_count = value->data.sequence.items.top - value->data.sequence.items.start;
                        config->ports = (char**)malloc(config->ports_count * sizeof(char*));
                        yaml_node_item_t* item = value->data.sequence.items.start;
                        for (int i = 0; i < config->ports_count; i++, item++) {
                            yaml_node_t* port = yaml_document_get_node(&document, *item);
                            config->ports[i] = strdup((char*)port->data.scalar.value);
                        }
                    }
                }
                else if (strcmp((char*)key->data.scalar.value, "environment") == 0) {
                    if (value->type == YAML_SEQUENCE_NODE) {
                        config->environment_count = value->data.sequence.items.top - value->data.sequence.items.start;
                        config->environment = (char**)malloc(config->environment_count * sizeof(char*));
                        yaml_node_item_t* item = value->data.sequence.items.start;
                        for (int i = 0; i < config->environment_count; i++, item++) {
                            yaml_node_t* env = yaml_document_get_node(&document, *item);
                            config->environment[i] = strdup((char*)env->data.scalar.value);
                        }
                    }
                }
                else if (strcmp((char*)key->data.scalar.value, "auto_update") == 0) {
                    config->auto_update = strcmp((char*)value->data.scalar.value, "true") == 0;
                }
            }
        }
    }

    yaml_document_delete(&document);
    yaml_parser_delete(&parser);
    fclose(file);
}

void start_container(ContainerConfig* config) {
    char command[1024] = "docker run -d ";
    if (config->container_name) {
        strncat(command, "--name ", sizeof(command) - strlen(command) - 1);
        strncat(command, config->container_name, sizeof(command) - strlen(command) - 1);
        strncat(command, " ", sizeof(command) - strlen(command) - 1);
    }
    for (int i = 0; i < config->volumes_count; i++) {
        strncat(command, "-v ", sizeof(command) - strlen(command) - 1);
        strncat(command, config->volumes[i], sizeof(command) - strlen(command) - 1);
        strncat(command, " ", sizeof(command) - strlen(command) - 1);
    }
    for (int i = 0; i < config->ports_count; i++) {
        strncat(command, "-p ", sizeof(command) - strlen(command) - 1);
        strncat(command, config->ports[i], sizeof(command) - strlen(command) - 1);
        strncat(command, " ", sizeof(command) - strlen(command) - 1);
    }
    for (int i = 0; i < config->environment_count; i++) {
        strncat(command, "-e ", sizeof(command) - strlen(command) - 1);
        strncat(command, config->environment[i], sizeof(command) - strlen(command) - 1);
        strncat(command, " ", sizeof(command) - strlen(command) - 1);
    }
    strncat(command, config->image, sizeof(command) - strlen(command) - 1);

    printf("Starting container with command: %s\n", command);
    system(command);
}

void stop_container(const char* container_name) {
    char command[256];
    snprintf(command, sizeof(command), "docker stop %s", container_name);
    system(command);
}

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    char** response = (char**)userp;
    *response = realloc(*response, strlen(*response) + realsize + 1);
    if (*response == NULL) {
        fprintf(stderr, "Failed to allocate memory for response\n");
        return 0;
    }
    strncat(*response, (char*)contents, realsize);
    return realsize;
}

int check_image_update(const char* image) {
    CURL* curl;
    CURLcode res;
    char url[256];
    char namespace[128], repository[128], tag[128];
    sscanf(image, "%[^/]/%[^:]:%s", namespace, repository, tag);
    snprintf(url, sizeof(url), "https://registry.hub.docker.com/v2/repositories/%s/%s/tags", namespace, repository);

    char* response = strdup("");
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(response);
            curl_easy_cleanup(curl);
            return 0;
        }

        if (strstr(response, "latest") != NULL) {
            free(response);
            curl_easy_cleanup(curl);
            return 1;
        }
        free(response);
        curl_easy_cleanup(curl);
    }
    return 0;
}

void schedule_image_update(ContainerConfig* config, const char* filename) {
    time_t last_check = time(NULL);
    while (1) {
        time_t now = time(NULL);
        if (now - last_check >= 12 * 60 * 60) {
            if (config->auto_update && check_image_update(config->image)) {
                FILE* file = fopen(filename, "r");
                if (file == NULL) {
                    perror("Failed to open file");
                    continue;
                }
                char line[1024];
                char temp_filename[256];
                snprintf(temp_filename, sizeof(temp_filename), "%s.tmp", filename);
                FILE* temp_file = fopen(temp_filename, "w");
                if (temp_file == NULL) {
                    perror("Failed to open temp file");
                    fclose(file);
                    continue;
                }
                while (fgets(line, sizeof(line), file) != NULL) {
                    if (strstr(line, "image:") != NULL) {
                        char new_line[1024];
                        snprintf(new_line, sizeof(new_line), "image: %s/latest\n", config->image);
                        fputs(new_line, temp_file);
                    }
                    else {
                        fputs(line, temp_file);
                    }
                }
                fclose(file);
                fclose(temp_file);
                remove(filename);
                rename(temp_filename, filename);
                free(config->image);
                config->image = strdup("new_image_tag");
            }
            last_check = now;
        }
        sleep(60);
    }
}

int main() {
    ContainerConfig config = { 0 };
    const char* filename = "docker-compose.yml";
    parse_yaml(filename, &config);

    start_container(&config);

    if (config.auto_update) {
        schedule_image_update(&config, filename);
    }

    stop_container(config.container_name);

    free(config.image);
    free(config.container_name);
    for (int i = 0; i < config.volumes_count; i++) {
        free(config.volumes[i]);
    }
    free(config.volumes);
    for (int i = 0; i < config.ports_count; i++) {
        free(config.ports[i]);
    }
    free(config.ports);
    for (int i = 0; i < config.environment_count; i++) {
        free(config.environment[i]);
    }
    free(config.environment);

    return 0;
}
