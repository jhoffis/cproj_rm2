#include "wav_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocator.h"
#include "file_util.h"

typedef struct {
    wav_header header;
    const char *filename;
    void *loaded_data;
    u32 loaded_data_size; 
} wav_file;

static const u32 ALLOC_SIZE = 64 * 1024;

static void free_wav_file(wav_file *file) {
    if (!file) return;
    xfree(file->loaded_data);
    xfree(file);
}

static wav_file* load_wav(const char* filename, u32 start_index, u32 max_alloc_size) {
    if (!filename) return NULL;
    
    char *fixed_name = path_name("audio/", filename, ".wav");
    if (!fixed_name) return NULL;

    FILE* file = fopen(fixed_name, "rb");
    xfree(fixed_name);
    
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    wav_header header = {0};
    if (fread(&header, sizeof(wav_header), 1, file) != 1) {
        perror("Error reading header");
        fclose(file);
        return NULL;
    }

    // Validate the header
    if (header.chunk_id != RIFF_ID || header.format != WAVE_ID ||
        header.subchunk1_id != FMT_ID || header.subchunk2_id != DATA_ID) {
        fprintf(stderr, "Invalid WAV file\n");
        fclose(file);
        return NULL;
    }

    // Calculate how much data we can actually read
    u32 remaining_size = header.subchunk2_size - start_index;
    u32 alloc_size = (max_alloc_size < remaining_size) ? max_alloc_size : remaining_size;
    
    void* audio_data = xmalloc(alloc_size);
    if (!audio_data) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    if (fseek(file, start_index, SEEK_CUR) != 0 ||
        fread(audio_data, 1, alloc_size, file) != alloc_size) {
        perror("Error reading audio data");
        xfree(audio_data);
        fclose(file);
        return NULL;
    }

    fclose(file);
    
    wav_file* wav = xmalloc(sizeof(wav_file));
    if (!wav) {
        xfree(audio_data);
        return NULL;
    }
    
    *wav = (wav_file){header, filename, audio_data, alloc_size};
    return wav;
}

static void setup_buffer(void **dest, void *src, u32 size) {
    if (size == 0) {
        *dest = NULL;
        return;
    }
    *dest = xmalloc(size);
    memcpy(*dest, src, size);
}

wav_entity *create_wav_entity(const char *filename) {
    wav_file *wav = load_wav(filename, 0, 2 * ALLOC_SIZE);
    if (!wav) return NULL;

    wav_entity *entity = xmalloc(sizeof(wav_entity));
    if (!entity) {
        free_wav_file(wav);
        return NULL;
    }

    entity->header = wav->header;
    entity->filename = strdup(filename);
    entity->cursor = xmalloc(sizeof(u32));
    *entity->cursor = 0;

    // Setup current buffer (first ALLOC_SIZE bytes or less)
    u32 current_size = (wav->loaded_data_size < ALLOC_SIZE) ? 
                      wav->loaded_data_size : ALLOC_SIZE;
    entity->current_data_size = current_size;
    setup_buffer(&entity->current_data, wav->loaded_data, current_size);

    // Setup next buffer (remaining bytes up to ALLOC_SIZE)
    u32 next_size = (wav->loaded_data_size <= ALLOC_SIZE) ? 0 :
                    ((wav->loaded_data_size - ALLOC_SIZE) < ALLOC_SIZE) ?
                    (wav->loaded_data_size - ALLOC_SIZE) : ALLOC_SIZE;
    
    entity->next_data_size = next_size;
    setup_buffer(&entity->next_data, wav->loaded_data + ALLOC_SIZE, next_size);

    // Set loaded cursor position
    entity->loaded_cursor = (wav->loaded_data_size <= 2 * ALLOC_SIZE) ? 
                           0 : 2 * ALLOC_SIZE;

    free_wav_file(wav);
    return entity;
}

bool load_next_wav_buffer(wav_entity *entity) {
    if (!entity) return false;

    xfree(entity->current_data);
    
    // Move next buffer to current
    entity->current_data = entity->next_data;
    entity->current_data_size = entity->next_data_size;
    entity->next_data = NULL;
    
    // If we've reached the end, no more loading needed
    if (entity->loaded_cursor == 0) {
        entity->next_data_size = 0;
        return entity->current_data_size > 0;
    }

    // Load next chunk
    wav_file *wav = load_wav(entity->filename, entity->loaded_cursor, ALLOC_SIZE);
    if (!wav) {
        entity->next_data_size = 0;
        return entity->current_data_size > 0;
    }

    // Setup next buffer
    entity->next_data_size = wav->loaded_data_size;
    setup_buffer(&entity->next_data, wav->loaded_data, wav->loaded_data_size);
    
    // Update cursor
    entity->loaded_cursor = (wav->loaded_data_size < ALLOC_SIZE) ? 0 : 
                           entity->loaded_cursor + ALLOC_SIZE;

    free_wav_file(wav);
    return true;
}

void free_wav_entity(wav_entity *entity) {
    if (!entity) return;
    xfree(entity->cursor);
    // xfree(entity->filename);
    xfree(entity->current_data);
    xfree(entity->next_data);
    xfree(entity);
}






