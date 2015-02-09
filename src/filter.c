/*  filter.c main() implementation source file
 
    Copyright (C) 2014-2015 Michele Sorcinelli
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* standard libs */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fftw3.h>

/* jack libs */
#include <jack/jack.h>
#include <jack/midiport.h>

typedef jack_default_audio_sample_t sample_t;

jack_port_t *in_p, *out_p;
jack_nframes_t sr, bs;
fftw_complex *dft;
fftw_plan dft_p, idft_p;
double *dft_in, *idft;
sample_t *in;

/* function declaration */

int process(jack_nframes_t, void *);
int srate(jack_nframes_t, void *);
int bsize(jack_nframes_t, void *);
void data_init();
void jack_shutdown(void *); 

/* function definition */

/* initialize fft plans and arrays */
void data_init() {
  if (idft != NULL) {
    free(idft);
    fftw_destroy_plan(idft_p);
  }

  if (dft != NULL) {
    fftw_destroy_plan(dft_p);
    fftw_free(dft);
  }

  dft_in = calloc(sizeof(double), bs);
  dft = fftw_malloc(sizeof(fftw_complex) * bs);
  idft = calloc(sizeof(double), bs);
  dft_p = fftw_plan_dft_r2c_1d(bs, dft_in, dft, FFTW_ESTIMATE);
  idft_p = fftw_plan_dft_c2r_1d(bs, dft, idft, FFTW_ESTIMATE);
}

/* called by jack sr/bs times per seconds */
int process(jack_nframes_t nframes, void *arg) {
  in = (sample_t *) jack_port_get_buffer (in_p, nframes);
  int i;
  for(i=0; i < nframes; i++) {
    dft_in[i] = in[i];
  }
  /* calculating dft of the signal */
  fftw_execute(dft_p);

  /* calculating idft of signal spectrum */
  fftw_execute(idft_p);
  sample_t *out = (sample_t *) jack_port_get_buffer (out_p, nframes);
  for(i=0; i < nframes; i++) {
    /* we need to normalize the signal */
    out[i] = idft[i]/bs;
  }
  return 0;  
}

/* called at start and on sr change */
int srate(jack_nframes_t nframes, void *arg) {
  printf("Sample Rate = %d/sec\n", nframes);
  sr = nframes;
  return 0;
}

/* called at start and on bs change */
int bsize(jack_nframes_t nframes, void *arg) {
  printf("Buffer size = %d\n", nframes);
  bs = nframes;
  data_init();
  return 0;
}

/* called on jack shutdown */
void jack_shutdown(void *arg) {
  exit(1);
}

int main(int argc, char **argv) {
  jack_client_t *client;
  char name[11];

  if (argc < 2) {
    printf("Type client name (max 10 char): ");
    scanf("%s", name); 
  }
  else 
    strcpy(name, argv[1]);

  if (!(client = jack_client_open(name, JackNullOption, NULL))) {
    fprintf(stderr, "jack server not running?\n");
    return 1;
  }

  jack_set_process_callback(client, process, 0);
  jack_set_sample_rate_callback(client, srate, 0);
  jack_set_buffer_size_callback(client, bsize, 0);
  jack_on_shutdown(client, jack_shutdown, 0);

  in_p = jack_port_register(client, "in", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  out_p = jack_port_register(client, "out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  if (jack_activate(client)) {
    fprintf(stderr, "cannot activate client");
    return 1;
  }
  while(getchar() != EOF);
  return 0;
}
