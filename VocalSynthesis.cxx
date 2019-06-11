#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "speak_lib.h"

typedef struct {
  short* data;
  int size;
} short_arr;

static void writeWav(const char* filename, short_arr* samples);
static int synthCallback(short* wav, int numsamples, espeak_EVENT *events);

static int doneGathering = 0;

const char* textToWav(const char* str, const char* filename) {
    const char* res;

    short_arr samples;
    samples.data = NULL;
    samples.size = 0;
   
    // Init espeak and read
    espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, 0);
    doneGathering = 1;

    espeak_SetVoiceByName("french");
    espeak_SetParameter(espeakRATE, 80, 0);
    espeak_SetSynthCallback(synthCallback);
    espeak_Synth((const void*) str, 20, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, &samples);

    res = espeak_TextToPhonemes(
      (const void**) &str,
      espeakCHARS_AUTO,
      0x01
    );
    fprintf(stderr, "res: %s\n", res);

    // Wait for the samples to be done
    while (!doneGathering);
    
    // Write wav file
    writeWav(filename, &samples);

    free(samples.data);

    return res;
}

static int reverse(int n) {
  int a = (n >> 24) & 0xFF;
  int b = (n >> 16) & 0xFF;
  int c = (n >>  8) & 0xFF;
  int d = (n >>  0) & 0xFF;

  return (d << 24) + (c << 16) + (b << 8) + a;
}

static void writeWav(const char* filename, short_arr* samples) {
  int nbChannel = 1;
  int frequency = 22050;
  int bitsPerSample = 16;
  int bytePerBloc = nbChannel * bitsPerSample / 8;
  int bytePerSec = frequency * bytePerBloc;
  int i;

  int from = 0;
  int to = samples->size;
  int realSize;

  // Find beginning and ending of data
  while (samples->data[from] == 0 && from < to) from++;
  while (samples->data[to - 1] == 0 && from < to) to--;
  realSize = to - from;

  // Header
  int header[11] = {
    0x52494646, // "RIFF"
    reverse(samples->size * 2 + 44 - 8),
    0x57415645, // "WAVE"

    0x666D7420, // "fmt "
    0x10000000,

    0x01000100,
    reverse(frequency),
    reverse(bytePerSec),
    reverse((bitsPerSample << 16) + bytePerBloc),

    0x64617461, // "data"
    reverse(realSize * sizeof *samples->data)
  };

  FILE* f = fopen(filename, "w+");
  
  // Write header
  for (i = 0; i < 11; i++) {
    fputc(header[i] >> 24, f);
    fputc(header[i] >> 16, f);
    fputc(header[i] >>  8, f);
    fputc(header[i] >>  0, f);
  }

  // Write data
  for (i = from; i < to; i++) {
    fputc(samples->data[i] >> 0, f);
    fputc(samples->data[i] >> 8, f);
  }

  fclose(f);
}

static int synthCallback(short* wav, int numsamples, espeak_EVENT *events) {
  int i;

  short_arr* samples = (short_arr*) events->user_data;
  size_t newsize = (samples->size + numsamples) * sizeof *(samples->data);

  if (numsamples > 0) {
    samples->data = (short int*) realloc(samples->data, newsize);
    for (i = 0; i < numsamples; i++) {
      samples->data[samples->size + i] = wav[i];
    }
    samples->size += numsamples; 
  }

  for (i = 0; events[i].type != espeakEVENT_LIST_TERMINATED; i++) {
    if (events[i].type == espeakEVENT_MSG_TERMINATED) {
      doneGathering = 1;
    }
  }

  return 0;
}

