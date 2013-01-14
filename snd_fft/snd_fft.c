#include <err.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <asoundlib.h>

#include <sndfile.h>

#include <complex.h>
#include <fftw3.h>

#define WIDTH	80
#define HEIGHT	40

struct holder {
	SNDFILE *infile;
	SF_INFO ininfo;

	snd_pcm_t *alsa_handle;

	fftw_complex *output;
	fftw_plan plan;

	unsigned int samples_count;

	double *samples;

	double max;
};

static void prepare_fftw(struct holder *holder)
{
	unsigned int a;

	holder->samples = fftw_alloc_real(holder->samples_count);
	if (!holder->samples)
		errx(3, "cannot allocate input");
	holder->output = fftw_alloc_complex(holder->samples_count);
	if (!holder->output)
		errx(3, "cannot allocate output");

	for (a = 0; a < holder->samples_count; a++) {
		holder->samples[a] = 0;
		holder->output[a] = 0;
	}

	holder->plan = fftw_plan_dft_r2c_1d(holder->samples_count,
			holder->samples, holder->output, 0);
	if (!holder->plan)
		errx(3, "plan not created");
}

static void destroy_fftw(struct holder *holder)
{
	fftw_destroy_plan(holder->plan);
	fftw_free(holder->output);
	fftw_free(holder->samples);
}

static void show_graph(const struct holder *holder)
{
	unsigned int sum = 0, sum_n = holder->samples_count / 2 / HEIGHT;
	unsigned int a, b, idx = 0;

	for (a = 0; a < holder->samples_count / 2; a++) {
		sum += (unsigned int)(holder->samples[a] * WIDTH / holder->max);
		if (++idx == sum_n) {
			sum /= sum_n;
			for (b = 0; b < sum; b++)
				printf("*");
			printf("\n");
			sum = idx = 0;
		}
	}
}

static void compute_fftw(struct holder *holder)
{
	unsigned int a;

	fftw_execute(holder->plan);

	for (a = 0; a < holder->samples_count; a++) {
		holder->samples[a] = cabs(holder->output[a]);
		if (holder->samples[a] > holder->max)
			holder->max = holder->samples[a];
	}
}

static void write_snd(struct holder *holder, float const *samples,
		unsigned int count)
{
	snd_pcm_sframes_t frames;

	frames = snd_pcm_writei(holder->alsa_handle, samples, count);
	if (frames < 0)
		frames = snd_pcm_recover(holder->alsa_handle, frames, 0);
	if (frames < 0)
		errx(2, "snd_pcm_writei failed: %s", snd_strerror(frames));
}

static void decode(struct holder *holder)
{
	unsigned int channels = holder->ininfo.channels;
	float buf[channels * holder->samples_count];
	int count, a, ch;

	while (1) {
		count = sf_readf_float(holder->infile, buf,
				holder->samples_count);
		if (count == 0)
			break;

		write_snd(holder, buf, count);

		/* this was the last chunk */
		if (count != holder->samples_count)
			break;

		/* avg of all channels */
		for (a = 0; a < count; a++) {
			for (ch = 0; ch < channels; ch++)
				holder->samples[a] += buf[a * channels + ch];
			holder->samples[a] /= channels;
		}
		compute_fftw(holder);
		show_graph(holder);
	}
}

static void open_io(struct holder *holder, const char *filename)
{
	int err;

	if (!strcmp(filename, "-"))
		holder->infile = sf_open_fd(STDIN_FILENO, SFM_READ,
				&holder->ininfo, 1);
	else
		holder->infile = sf_open(filename, SFM_READ, &holder->ininfo);

	if (holder->infile == NULL)
		errx(1, "open in: %s", sf_strerror(NULL));

	err = snd_pcm_open(&holder->alsa_handle, "default",
			SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0)
		errx(1, "alsa open: %s", snd_strerror(err));

	err = snd_pcm_set_params(holder->alsa_handle, SND_PCM_FORMAT_FLOAT,
			SND_PCM_ACCESS_RW_INTERLEAVED, holder->ininfo.channels,
			holder->ininfo.samplerate, 1, 500000);
	if (err < 0)
		errx(1, "alsa set_params: %s", snd_strerror(err));
}

static void close_io(struct holder *holder)
{
	snd_pcm_close(holder->alsa_handle);
	sf_close(holder->infile);
}

int main(int argc, char **argv)
{
	struct holder holder = {};

	if (argc < 2)
		errx(1, "bad arguments");

	open_io(&holder, argv[1]);

	holder.samples_count = holder.ininfo.samplerate / 100;

	prepare_fftw(&holder);

	decode(&holder);

	destroy_fftw(&holder);

	close_io(&holder);

	return 0;
}
