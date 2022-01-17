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

#include <ncurses.h>
#include <alsa/asoundlib.h>
#include <sndfile.h>

#include <complex.h>
#include <fftw3.h>

struct holder {
	SNDFILE *infile;
	SF_INFO ininfo;

	snd_pcm_t *alsa_handle;

	fftw_complex *output;
	fftw_plan plan;

	int height, width;

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

/* compute avg of all channels */
static void compute_avg(struct holder *holder, float *buf, unsigned int count)
{
	unsigned int channels = holder->ininfo.channels;
	unsigned int a, ch;

	for (a = 0; a < count; a++) {
		holder->samples[a] = 0;
		for (ch = 0; ch < channels; ch++)
			holder->samples[a] += buf[a * channels + ch];
		holder->samples[a] /= channels;
	}
}

static void compute_fftw(struct holder *holder)
{
	unsigned int a;

	fftw_execute(holder->plan);

	for (a = 0; a < holder->samples_count / 2; a++) {
		holder->samples[a] = cabs(holder->output[a]);
		if (holder->samples[a] > holder->max)
			holder->max = holder->samples[a];
	}
}

static void show_graph(const struct holder *holder)
{
	unsigned int usable_count = holder->samples_count / 2;
	unsigned int height = holder->height;
	unsigned int max = 0, sum = 0, sum_n = usable_count / holder->width;
	unsigned int a, b, idx = 0, x = 0;

	erase();
	for (a = 0; a < usable_count; a++) {
		unsigned int sample = holder->samples[a] * height / holder->max;
		if (sample > max)
			max = sample;
		sum += sample;
		if (++idx == sum_n) {
			sum /= sum_n;
			color_set(1, NULL);
			for (b = height - max; b < height - sum; b++)
				mvaddch(b, x, '|');
			color_set(2, NULL);
			for (b = height - sum; b < height; b++)
				mvaddch(b, x, '*');
			max = sum = idx = 0;
			x++;
		}
	}
	move(0, 0);
	refresh();
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
	int count, short_read;

	do {
		count = sf_readf_float(holder->infile, buf,
				holder->samples_count);
		if (count <= 0)
			break;

		/* the last chunk? */
		short_read = count != holder->samples_count;
		if (!short_read) {
			compute_avg(holder, buf, count);
			compute_fftw(holder);
			show_graph(holder);
		}

		write_snd(holder, buf, count);
	} while (!short_read);
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

	initscr();
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	getmaxyx(stdscr, holder->height, holder->width);
}

static void close_io(struct holder *holder)
{
	endwin();
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

	/* do we have enough data? no = clamp the graph */
	if (holder.width > holder.samples_count / 2)
		holder.width = holder.samples_count / 2;

	prepare_fftw(&holder);

	decode(&holder);

	destroy_fftw(&holder);

	close_io(&holder);

	return 0;
}
