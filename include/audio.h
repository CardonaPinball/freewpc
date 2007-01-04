
#ifndef __AUDIO_H
#define __AUDIO_H


/** A bitmask that says an audio clip should try to use a particular
channel. */
#define AUDIO_CH(n)	(1 << (n))

/* The 8 channels are allocated for different purposes.
 * The order of these bits is in order from most frequent to least. */
#define AUDIO_SAMPLE1           0x1
#define AUDIO_SAMPLE2           0x2
#define AUDIO_SAMPLE3           0x4
#define AUDIO_BACKGROUND_MUSIC  0x8
#define AUDIO_SPEECH1           0x10
#define AUDIO_SPEECH2           0x20
#define AUDIO_FOREGROUND_MUSIC  0x40
#define AUDIO_RESERVED          0x80

#define AUDIO_CH_SAMPLE1        0
#define AUDIO_CH_SAMPLE2        1
#define AUDIO_CH_SAMPLE3        2
#define AUDIO_CH_BACKGROUND     3
#define AUDIO_CH_SPEECH1        4
#define AUDIO_CH_SPEECH2        5
#define AUDIO_CH_FOREGROUND     6

/** The total number of audio channels supported.  At most 8 can
 * be supported due to the way channel bitmasks are used. */
#define NUM_AUDIO_CHANNELS 8


/** The total number of stacked background tracks */
#define NUM_STACKED_TRACKS 8

/** The audio channel structure. */
typedef struct {
	/** The task that currently owns the channel.
	 * If this is zero, the channel is free and may be allocated
	 * for a new clip. */
	task_pid_t pid;

	/** The priority of the clip that currently owns the channel. */
	priority_t prio;
} audio_channel_t;


/** A background track */
typedef struct {
	priority_t prio;
	U8 code;
} audio_track_t;

__common__ void audio_dump (void);
__common__ void audio_start (U8 channel_mask, task_function_t fn, U8 fnpage, U16 data);
__common__ void audio_stop (U8 channel_id);
__common__ void audio_exit (void);
__common__ void bg_music_start (audio_track_t *track);
__common__ void bg_music_stop (audio_track_t *track);

#endif /* __AUDIO_H */
