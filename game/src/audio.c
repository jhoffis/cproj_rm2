#include "audio.h"
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#include <audioclient.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#elif __linux__
#include <alsa/asoundlib.h>
#include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define BITS_PER_SAMPLE 16
#define BUFFER_MS 50  // Buffer duration in milliseconds

typedef void (*audio_callback)(int16_t* buffer, size_t buffer_size);

typedef struct {
    audio_callback user_callback;
    bool is_running;
    void* platform_specific;
} AudioContext;

#ifdef _WIN32
typedef struct {
    IAudioClient* audio_client;
    IAudioRenderClient* render_client;
    HANDLE event_handle;
    HANDLE thread_handle;
    DWORD thread_id;
} WindowsAudioData;
#elif __linux__
typedef struct {
    snd_pcm_t* pcm_handle;
    pthread_t thread;
    pthread_mutex_t mutex;
} LinuxAudioData;
#endif

AudioContext* global_audio_context = NULL;

#ifdef _WIN32
DWORD WINAPI windows_audio_thread(LPVOID param) {
    AudioContext* context = (AudioContext*)param;
    WindowsAudioData* win_data = (WindowsAudioData*)context->platform_specific;
    IAudioClient* audio_client = win_data->audio_client;
    IAudioRenderClient* render_client = win_data->render_client;

    UINT32 buffer_frame_count;
    audio_client->lpVtbl->GetBufferSize(audio_client, &buffer_frame_count);

    while (context->is_running) {
        DWORD wait_result = WaitForSingleObject(win_data->event_handle, INFINITE);
        if (wait_result != WAIT_OBJECT_0) break;

        UINT32 padding_frames;
        audio_client->lpVtbl->GetCurrentPadding(audio_client, &padding_frames);

        UINT32 frames_to_render = buffer_frame_count - padding_frames;
        if (frames_to_render > 0) {
            BYTE* render_buffer;
            render_client->lpVtbl->GetBuffer(render_client, frames_to_render, &render_buffer);

            context->user_callback((int16_t*)render_buffer, frames_to_render * CHANNELS);

            render_client->lpVtbl->ReleaseBuffer(render_client, frames_to_render, 0);
        }
    }

    return 0;
}
#elif __linux__
void* linux_audio_thread(void* param) {
    AudioContext* context = (AudioContext*)param;
    LinuxAudioData* linux_data = (LinuxAudioData*)context->platform_specific;

    int buffer_size = SAMPLE_RATE * BUFFER_MS / 1000;
    int16_t* buffer = malloc(buffer_size * CHANNELS * sizeof(int16_t));

    while (context->is_running) {
        pthread_mutex_lock(&linux_data->mutex);

        context->user_callback(buffer, buffer_size * CHANNELS);

        snd_pcm_sframes_t frames = snd_pcm_writei(linux_data->pcm_handle, buffer, buffer_size);
        if (frames < 0) {
            frames = snd_pcm_recover(linux_data->pcm_handle, frames, 0);
        }

        pthread_mutex_unlock(&linux_data->mutex);
    }

    free(buffer);
    return NULL;
}
#endif

// Example test sound generation callback
static void test_sound_callback(int16_t* buffer, size_t sample_count) {
    static float phase = 0.0f;
    const float frequency = 440.0f;  // A4 note
    const float sample_rate = 48000.0f;
    const float amplitude = 32767.0f * 0.1f;  // 10% volume

    for (size_t i = 0; i < sample_count; i += CHANNELS) {
        int16_t sample = (int16_t)(amplitude * sinf(phase));
        for (int ch = 0; ch < CHANNELS; ch++) {
            buffer[i + ch] = sample;
        }

        phase += 2.0f * M_PI * frequency / sample_rate;
        if (phase > 2.0f * M_PI) {
            phase -= 2.0f * M_PI;
        }
    }
}

bool audio_init() {
    if (global_audio_context) return false;

    global_audio_context = malloc(sizeof(AudioContext));
    global_audio_context->user_callback = test_sound_callback;
    global_audio_context->is_running = true;

#ifdef _WIN32
    {
        WindowsAudioData* win_data = malloc(sizeof(WindowsAudioData));
        global_audio_context->platform_specific = win_data;

        HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if (FAILED(hr)) {
            // Handle initialization error
            fprintf(stderr, "Failed to initialize COM library\n");
            return false;
        } 

        IMMDeviceEnumerator* device_enum = NULL;
        CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void**)&device_enum);

        IMMDevice* device = NULL;
        device_enum->lpVtbl->GetDefaultAudioEndpoint(device_enum, eRender, eConsoleRole, &device);

        IAudioClient* audio_client = NULL;
        device->lpVtbl->Activate(device, &IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&audio_client);

        WAVEFORMATEX wfx = {0};
        wfx.wFormatTag = WAVE_FORMAT_PCM;
        wfx.nChannels = CHANNELS;
        wfx.nSamplesPerSec = SAMPLE_RATE;
        wfx.wBitsPerSample = BITS_PER_SAMPLE;
        wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

        audio_client->lpVtbl->Initialize(audio_client, AUDCLNT_SHAREMODE_SHARED, 
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 
                BUFFER_MS * 10000, 0, &wfx, NULL);

        win_data->event_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
        audio_client->lpVtbl->SetEventHandle(audio_client, win_data->event_handle);

        IAudioRenderClient* render_client = NULL;
        audio_client->lpVtbl->GetService(audio_client, &IID_IAudioRenderClient, (void**)&render_client);

        win_data->audio_client = audio_client;
        win_data->render_client = render_client;

        win_data->thread_handle = CreateThread(NULL, 0, windows_audio_thread, global_audio_context, 0, &win_data->thread_id);

        audio_client->lpVtbl->Start(audio_client);
    }
#elif __linux__
    {
        LinuxAudioData* linux_data = malloc(sizeof(LinuxAudioData));
        global_audio_context->platform_specific = linux_data;

        int err;
        snd_pcm_hw_params_t* hw_params;

        err = snd_pcm_open(&linux_data->pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
        if (err < 0) {
            fprintf(stderr, "Cannot open audio device: %s\n", snd_strerror(err));
            return false;
        }

        snd_pcm_hw_params_malloc(&hw_params);
        snd_pcm_hw_params_any(linux_data->pcm_handle, hw_params);

        snd_pcm_hw_params_set_access(linux_data->pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(linux_data->pcm_handle, hw_params, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(linux_data->pcm_handle, hw_params, CHANNELS);

        unsigned int rate = SAMPLE_RATE;
        snd_pcm_hw_params_set_rate_near(linux_data->pcm_handle, hw_params, &rate, 0);

        snd_pcm_uframes_t buffer_size = SAMPLE_RATE * BUFFER_MS / 1000;
        snd_pcm_hw_params_set_buffer_size_near(linux_data->pcm_handle, hw_params, &buffer_size);

        err = snd_pcm_hw_params(linux_data->pcm_handle, hw_params);
        snd_pcm_hw_params_free(hw_params);

        pthread_mutex_init(&linux_data->mutex, NULL);
        pthread_create(&linux_data->thread, NULL, linux_audio_thread, global_audio_context);
    }
#endif

    return true;
}

void audio_close() {
    if (!global_audio_context) return;

    global_audio_context->is_running = false;

#ifdef _WIN32
    {
        WindowsAudioData* win_data = (WindowsAudioData*)global_audio_context->platform_specific;

        if (win_data->audio_client) {
            win_data->audio_client->lpVtbl->Stop(win_data->audio_client);
        }

        WaitForSingleObject(win_data->thread_handle, INFINITE);
        CloseHandle(win_data->thread_handle);
        CloseHandle(win_data->event_handle);

        if (win_data->render_client) win_data->render_client->lpVtbl->Release(win_data->render_client);
        if (win_data->audio_client) win_data->audio_client->lpVtbl->Release(win_data->audio_client);

        CoUninitialize();
        free(win_data);
    }
#elif __linux__
    {
        LinuxAudioData* linux_data = (LinuxAudioData*)global_audio_context->platform_specific;

        pthread_join(linux_data->thread, NULL);
        pthread_mutex_destroy(&linux_data->mutex);

        snd_pcm_close(linux_data->pcm_handle);
        free(linux_data);
    }
#endif

    free(global_audio_context);
    global_audio_context = NULL;
}

