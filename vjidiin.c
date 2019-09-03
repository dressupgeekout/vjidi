/*
 * vjidiin.c
 * Charlotte Koch
 */

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/midiio.h>

#define SEQUENCER_DEVICE "/dev/music"

int
main(void)
{
	int fd;
	int ch;

	fd = open(SEQUENCER_DEVICE, O_RDWR);
	write(fd, &SEQ_MK_TIMING(START), sizeof(seq_event_t));
	write(fd, &SEQ_MK_TIMING(TEMPO, .bpm=120), sizeof(seq_event_t));
	write(fd, &SEQ_MK_TIMING(TIMESIG, .numerator=4, .lg2denom=2, .clks_per_click=24, .dsq_per_24clks=8), sizeof(seq_event_t));

	write(fd, &SEQ_MK_CHN(PGM_CHANGE, .device=1, .channel=0, .program=62), sizeof(seq_event_t));

	bool done = false;

	while (!done) {
		ch = getchar();
		fflush(stdin);

		if (ch == 'Q') {
		  done = true;
		}

		write(fd, &SEQ_MK_CHN(NOTEON, .device=1, .channel=0, .key=ch, .velocity=127), sizeof(seq_event_t));
		write(fd, &SEQ_MK_TIMING(WAIT_REL, .divisions=24), sizeof(seq_event_t));
		write(fd, &SEQ_MK_CHN(NOTEOFF, .device=1, .channel=0, .key=ch, .velocity=127), sizeof(seq_event_t));
	}

	//for (int i = 0; i < 20; i++) {
	//	write(fd, &SEQ_MK_CHN(NOTEON, .device=1, .channel=0, .key=40+i, .velocity=127), sizeof(seq_event_t));
	//	write(fd, &SEQ_MK_TIMING(WAIT_REL, .divisions=24), sizeof(seq_event_t));
	//	write(fd, &SEQ_MK_CHN(NOTEOFF, .device=1, .channel=0, .key=40+i, .velocity=0), sizeof(seq_event_t));
	//	write(fd, &SEQ_MK_TIMING(WAIT_REL, .divisions=24), sizeof(seq_event_t));
	//	write(fd, &SEQ_MK_CHN(PGM_CHANGE, .device=1, .channel=0, .program=40+i), sizeof(seq_event_t));
	//}

	write(fd, &SEQ_MK_TIMING(STOP), sizeof(seq_event_t));

	close(fd);

	return EXIT_SUCCESS;
}
