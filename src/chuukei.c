/* chuukei.c */

#include "angband.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#ifdef WINDOWS
#include <windows.h>
#endif

#ifdef CHUUKEI
#if defined(WINDOWS)
#include <winsock.h>
#elif defined(MACINTOSH)
#include <OpenTransport.h>
#include <OpenTptInternet.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <setjmp.h>
#include <signal.h>
#endif

#define MAX_HOSTNAME 256
#endif

#define RINGBUF_SIZE 1024*1024
#define FRESH_QUEUE_SIZE 4096
#ifdef WINDOWS
#define WAIT 100
#else
#define WAIT 100*1000 /* �u���E�Y���̃E�G�C�g(us�P��) */
#endif
#define DEFAULT_DELAY 50
#define RECVBUF_SIZE 1024

static long epoch_time;  /* �o�b�t�@�J�n���� */
static int browse_delay; /* �\������܂ł̎���(100ms�P��)(���̊ԂɃ��O���z������) */
#ifdef CHUUKEI
static int sd; /* �\�P�b�g�̃t�@�C���f�B�X�N���v�^ */
static long time_diff;   /* �v���C���Ƃ̎��Ԃ̂���(��������Ȃ���f�B���C�𒲐����Ă���) */
static int server_port;
static char server_name[MAX_HOSTNAME];
#endif

static int movie_fd;
static int movie_mode;

#ifdef CHUUKEI
#ifdef WINDOWS
#define close closesocket
#endif

#ifdef MACINTOSH
static InetSvcRef inet_services = nil;
static EndpointRef ep			= kOTInvalidEndpointRef;
#endif
#endif
/* �`�悷�鎞�����o���Ă����L���[�\���� */
static struct
{
	int time[FRESH_QUEUE_SIZE];
	int next;
	int tail;
}fresh_queue;


/* �����O�o�b�t�@�\���� */
static struct
{
	char *buf;
	int wptr;
	int rptr;
	int inlen;
}ring;

#ifdef MACINTOSH
int recv(int s, char *buffer, size_t buflen, int flags)
{
	OTFlags 	junkFlags;
	int n = OTRcv(ep, (void *) buffer, buflen, &junkFlags);
	if( n <= 0 )
		return n;
	return n;
}
#endif

/*
 * Original hooks
 */
static errr (*old_xtra_hook)(int n, int v);
static errr (*old_curs_hook)(int x, int y);
static errr (*old_bigcurs_hook)(int x, int y);
static errr (*old_wipe_hook)(int x, int y, int n);
static errr (*old_text_hook)(int x, int y, int n, byte a, cptr s);

static void disable_chuukei_server(void)
{
	term *t = angband_term[0];
#ifdef CHUUKEI
	chuukei_server = FALSE;
#endif /* CHUUKEI */
	t->xtra_hook = old_xtra_hook;
	t->curs_hook = old_curs_hook;
	t->bigcurs_hook = old_bigcurs_hook;
	t->wipe_hook = old_wipe_hook;
	t->text_hook = old_text_hook;
}

/* ANSI C�ɂ���static�ϐ���0�ŏ���������邪�ꉞ���������� */
static errr init_buffer(void)
{
	fresh_queue.next = fresh_queue.tail = 0;
	ring.wptr = ring.rptr = ring.inlen = 0;
	fresh_queue.time[0] = 0;
	//ring.buf = malloc(RINGBUF_SIZE);
	ring.buf = (char *)malloc(RINGBUF_SIZE);
	if (ring.buf == NULL) return (-1);

	return (0);
}

/* ���݂̎��Ԃ�100ms�P�ʂŎ擾���� */
static long get_current_time(void)
{
#ifdef WINDOWS
	return timeGetTime() / 100;
#elif defined(MACINTOSH)
	return TickCount();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (tv.tv_sec * 10 + tv.tv_usec / 100000);
#endif
}


/* �����O�o�b�t�@�\���̂� buf �̓��e�������� */
static errr insert_ringbuf(char *buf)
{
	int len;
	len = strlen(buf) + 1; /* +1�͏I�[������ */

	if (movie_mode)
	{
		fd_write(movie_fd, buf, len);
#ifdef CHUUKEI
		if (!chuukei_server) return 0;
#else
		return 0;
#endif
	}

	/* �o�b�t�@���I�[�o�[ */
	if (ring.inlen + len >= RINGBUF_SIZE)
	{
#ifdef CHUUKEI
		if (chuukei_server) disable_chuukei_server();
		else chuukei_client = FALSE;

		prt("����M�o�b�t�@�����܂����B�T�[�o�Ƃ̐ڑ���ؒf���܂��B", 0, 0);
		inkey();

		close(sd);
#endif
		return (-1);
	}

	/* �o�b�t�@�̏I�[�܂łɎ��܂� */
	if (ring.wptr + len < RINGBUF_SIZE)
	{
		memcpy(ring.buf + ring.wptr, buf, len);
		ring.wptr += len;
	}
	/* �o�b�t�@�̏I�[�܂łɎ��܂�Ȃ�(�s�b�^�����܂�ꍇ���܂�) */
	else
	{
		int head = RINGBUF_SIZE - ring.wptr;  /* �O�� */
		int tail = len - head;               /* �㔼 */

		memcpy(ring.buf + ring.wptr, buf, head);
		memcpy(ring.buf, buf + head, tail);
		ring.wptr = tail;
	}

	ring.inlen += len;

	/* Success */
	return (0);
}

#ifdef CHUUKEI
void flush_ringbuf(void)
{
#ifndef MACINTOSH
	fd_set fdset;
	struct timeval tv;

	if (!chuukei_server) return;

	if (ring.inlen == 0) return;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&fdset);
	FD_SET(sd, &fdset);

	while (1)
	{
		fd_set tmp_fdset;
		int result;

		tmp_fdset = fdset;

		/* �\�P�b�g�Ƀf�[�^���������߂邩�ǂ������ׂ� */
		select(sd+1, (fd_set *)NULL, &tmp_fdset, (fd_set *)NULL, &tv);

		/* �������߂Ȃ���Ζ߂� */
		if (FD_ISSET(sd, &tmp_fdset) == 0) break;

		result = send(sd, ring.buf + ring.rptr, ((ring.wptr > ring.rptr ) ? ring.wptr : RINGBUF_SIZE) - ring.rptr, 0);

		if (result <= 0)
		{
			/* �T�[�o�Ƃ̐ڑ��f�H */
			if (chuukei_server) disable_chuukei_server();

			prt("�T�[�o�Ƃ̐ڑ����ؒf����܂����B", 0, 0);
			inkey();
			close(sd);

			return;
		}

		ring.rptr += result;
		ring.inlen -= result;

		if (ring.rptr == RINGBUF_SIZE) ring.rptr = 0;
		if (ring.inlen == 0) break;
	}
#else
	if (!chuukei_server) return;

	if (ring.inlen == 0) return;

	while (1)
	{
		int result;

		/* �\�P�b�g�Ƀf�[�^���������߂邩�ǂ������ׂ� */
		result = OTSnd(ep, ring.buf + ring.rptr, ((ring.wptr > ring.rptr ) ? ring.wptr : RINGBUF_SIZE) - ring.rptr, 0);

		if (result <= 0)
		{
			/* �T�[�o�Ƃ̐ڑ��f�H */
			if (chuukei_server) disable_chuukei_server();

			prt("�T�[�o�Ƃ̐ڑ����ؒf����܂����B", 0, 0);
			inkey();
			close(sd);

			return;
		}

		ring.rptr += result;
		ring.inlen -= result;

		if (ring.rptr == RINGBUF_SIZE) ring.rptr = 0;
		if (ring.inlen == 0) break;
	}
#endif
}


static int read_chuukei_prf(cptr prf_name)
{
	char buf[1024];
	FILE *fp;

	path_build(buf, sizeof(buf), ANGBAND_DIR_XTRA, prf_name);
	fp = my_fopen(buf, "r");

	if (!fp) return (-1);

	/* ������ */
	server_port = -1;
	server_name[0] = 0;
	browse_delay = DEFAULT_DELAY;

	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* �T�[�o�� */
		if (!strncmp(buf, "server:", 7))
		{
			strncpy(server_name, buf + 7, MAX_HOSTNAME - 1);
			server_name[MAX_HOSTNAME - 1] = '\0';
		}

		/* �|�[�g�ԍ� */
		if (!strncmp(buf, "port:", 5))
		{
			server_port = atoi(buf + 5);
		}

		/* �f�B���C */
		if (!strncmp(buf, "delay:", 6))
		{
			browse_delay = atoi(buf + 6);
		}
	}

	my_fclose(fp);

	/* prf�t�@�C�������S�łȂ� */
	if (server_port == -1 || server_name[0] == 0) return (-1);

	return (0);
}

int connect_chuukei_server(char *prf_name)
{
#ifndef MACINTOSH

#ifdef WINDOWS
	WSADATA wsaData;
	WORD wVersionRequested = (WORD) (( 1) |  ( 1 << 8));
#endif

	struct sockaddr_in ask;
	struct hostent *hp;

	if (read_chuukei_prf(prf_name) < 0)
	{
		printf("Wrong prf file\n");
		return (-1);
	}

	if (init_buffer() < 0)
	{
		printf("Malloc error\n");
		return (-1);
	}

#ifdef WINDOWS
	if (WSAStartup(wVersionRequested, &wsaData))
	{
		msg_print("Report: WSAStartup failed.");
		return (-1);
	}
#endif

	printf("server = %s\nport = %d\n", server_name, server_port);

	if ((hp = gethostbyname(server_name)) != NULL)
	{
		memset(&ask, 0, sizeof(ask));
		memcpy(&ask.sin_addr, hp->h_addr_list[0], hp->h_length);
	}
	else
	{
		if ((ask.sin_addr.s_addr=inet_addr(server_name)) == 0)
		{
			printf("Bad hostname\n");
			return (-1);
		}
	}

	ask.sin_family = AF_INET;
	ask.sin_port = htons((unsigned short)server_port);

#ifndef WINDOWS
	if ((sd=socket(PF_INET,SOCK_STREAM, 0)) < 0)
#else
	if ((sd=socket(PF_INET,SOCK_STREAM, 0)) == INVALID_SOCKET)
#endif
	{
		printf("Can't create socket\n");
		return (-1);
	}

	if (connect(sd, (struct sockaddr *)&ask, sizeof(ask)) < 0)
	{
		close(sd);
		printf("Can't connect %s port %d\n", server_name, server_port);
		return (-1);
	}

	return (0);
#else	/* MACINTOSH */
	OSStatus err;
	InetHostInfo 	response;
	InetHost 		host_addr;
	InetAddress 	inAddr;
	TCall 			sndCall;
	Boolean			bind	= false;
	OSStatus 	junk;

	if (read_chuukei_prf(prf_name) < 0){
		printf("Wrong prf file\n");
		return (-1);
	}
	
	init_buffer();
	
	printf("server = %s\nport = %d\n", server_name, server_port);


#if TARGET_API_MAC_CARBON
	err = InitOpenTransportInContext(kInitOTForApplicationMask, NULL);
#else
	err = InitOpenTransport();
#endif

	memset(&response, 0, sizeof(response));


#if TARGET_API_MAC_CARBON
	inet_services = OTOpenInternetServicesInContext(kDefaultInternetServicesPath, 0, &err, NULL);
#else
	inet_services = OTOpenInternetServices(kDefaultInternetServicesPath, 0, &err);
#endif
	
	if (err == noErr) {
		err = OTInetStringToAddress(inet_services, (char *)server_name, &response);
		
		if (err == noErr) {
			host_addr = response.addrs[0];
		} else {
			printf("Bad hostname\n");
		}
		
#if TARGET_API_MAC_CARBON
		ep = (void *)OTOpenEndpointInContext(OTCreateConfiguration(kTCPName), 0, nil, &err, NULL);
#else
		ep = (void *)OTOpenEndpoint(OTCreateConfiguration(kTCPName), 0, nil, &err);
#endif

		if (err == noErr) {
			err = OTBind(ep, nil, nil);
			bind = (err == noErr);
	    }
	    if (err == noErr){
		OTInitInetAddress(&inAddr, server_port, host_addr);
			
			sndCall.addr.len 	= sizeof(InetAddress);				
			sndCall.addr.buf	= (unsigned char*) &inAddr;
			sndCall.opt.buf 	= nil;		/* no connection options */
			sndCall.opt.len 	= 0;
			sndCall.udata.buf 	= nil;		/* no connection data */
			sndCall.udata.len 	= 0;
			sndCall.sequence 	= 0;		/* ignored by OTConnect */
			
			err = OTConnect(ep, &sndCall, NULL);
			
			if( err != noErr ){
				printf("Can't connect %s port %d\n", server_name, server_port);
			}
		}
		
		err = OTSetSynchronous(ep);
		if (err == noErr)		
			err = OTSetBlocking(ep);
		
	}
	
	if( err != noErr ){
		if( bind ){
			OTUnbind(ep);
		}
		/* Clean up. */
		if (ep != kOTInvalidEndpointRef) {
			OTCloseProvider(ep);
			ep = nil;
		}
		if (inet_services != nil) {
			OTCloseProvider(inet_services);
			inet_services = nil;
		}
	
		return -1;
	}
	
	return 0;

#endif
}
#endif /* CHUUKEI */

/* str�����������̌J��Ԃ����ǂ������ׂ� */
static bool string_is_repeat(char *str, int len)
{
	char c = str[0];
	int i;

	if (len < 2) return (FALSE);
#ifdef JP
	if (iskanji(c)) return (FALSE);
#endif

	for (i = 1; i < len; i++)
	{
#ifdef JP
		if(c != str[i] || iskanji(str[i])) return (FALSE);
#else
		if(c != str[i]) return (FALSE);
#endif
	}

	return (TRUE);
}

static errr send_text_to_chuukei_server(int x, int y, int len, byte col, cptr str)
{
	char buf[1024];
	char buf2[1024];

	strncpy(buf2, str, len);
	buf2[len] = '\0';

	if (len == 1)
	{
		sprintf(buf, "s%c%c%c%c", x+1, y+1, col, buf2[0]);
	}
	else if(string_is_repeat(buf2, len))
	{
		int i;
		for (i = len; i > 0; i -= 127)
		{
			sprintf(buf, "n%c%c%c%c%c", x+1, y+1, MIN(i, 127), col, buf2[0]);
		}
	}
	else
	{
#ifdef SJIS
		sjis2euc(buf2);
#endif
		sprintf(buf, "t%c%c%c%c%s", x+1, y+1, len, col, buf2);
	}

	insert_ringbuf(buf);

	return (*old_text_hook)(x, y, len, col, str);
}

static errr send_wipe_to_chuukei_server(int x, int y, int len)
{
	char buf[1024];

	sprintf(buf, "w%c%c%c", x+1, y+1, len);

	insert_ringbuf(buf);

	return (*old_wipe_hook)(x, y, len);
}

static errr send_xtra_to_chuukei_server(int n, int v)
{
	char buf[1024];

	if (n == TERM_XTRA_CLEAR || n == TERM_XTRA_FRESH || n == TERM_XTRA_SHAPE)
	{
		sprintf(buf, "x%c", n+1);
		
		insert_ringbuf(buf);
		
		if (n == TERM_XTRA_FRESH)
		{
			sprintf(buf, "d%ld", get_current_time() - epoch_time);
			insert_ringbuf(buf);
		}
	}

	/* Verify the hook */
	if (!old_xtra_hook) return -1;

	return (*old_xtra_hook)(n, v);
}

static errr send_curs_to_chuukei_server(int x, int y)
{
	char buf[1024];

	sprintf(buf, "c%c%c", x+1, y+1);

	insert_ringbuf(buf);

	return (*old_curs_hook)(x, y);
}

static errr send_bigcurs_to_chuukei_server(int x, int y)
{
	char buf[1024];

	sprintf(buf, "C%c%c", x+1, y+1);

	insert_ringbuf(buf);

	return (*old_bigcurs_hook)(x, y);
}


/*
 * Prepare z-term hooks to call send_*_to_chuukei_server()'s
 */
void prepare_chuukei_hooks(void)
{
	term *t0 = angband_term[0];

	/* Save original z-term hooks */
	old_xtra_hook = t0->xtra_hook;
	old_curs_hook = t0->curs_hook;
	old_bigcurs_hook = t0->bigcurs_hook;
	old_wipe_hook = t0->wipe_hook;
	old_text_hook = t0->text_hook;

	/* Prepare z-term hooks */
	t0->xtra_hook = send_xtra_to_chuukei_server;
	t0->curs_hook = send_curs_to_chuukei_server;
	t0->bigcurs_hook = send_bigcurs_to_chuukei_server;
	t0->wipe_hook = send_wipe_to_chuukei_server;
	t0->text_hook = send_text_to_chuukei_server;
}


/*
 * Prepare z-term hooks to call send_*_to_chuukei_server()'s
 */
/*:::���[�r�[���L�^����炵��*/
void prepare_movie_hooks(void)
{
	char buf[1024];
	char tmp[80];

	if (movie_mode)
	{
		movie_mode = 0;
#ifdef CHUUKEI
		if (!chuukei_server) disable_chuukei_server();
#else
		disable_chuukei_server();
#endif
		fd_close(movie_fd);
#ifdef JP
		msg_print("�^����I�����܂����B");
#else
		msg_print("Stopped recording.");
#endif
	}
	else
	{
		sprintf(tmp, "%s.amv", player_base);
#ifdef JP
		if (get_string("���[�r�[�L�^�t�@�C��: ", tmp, 80))
#else
		if (get_string("Movie file name: ", tmp, 80))
#endif
		{
			int fd;

			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			fd = fd_open(buf, O_RDONLY);

			/* Existing file */
			if (fd >= 0)
			{
				char out_val[160];

				/* Close the file */
				(void)fd_close(fd);

				/* Build query */
#ifdef JP
				(void)sprintf(out_val, "��������t�@�C���ɏ㏑�����܂���? (%s)", buf);
#else
				(void)sprintf(out_val, "Replace existing file %s? ", buf);
#endif

				/* Ask */
				if (!get_check(out_val)) return;

				movie_fd = fd_open(buf, O_WRONLY | O_TRUNC);
			}
			else
			{
				movie_fd = fd_make(buf, 0644);
			}

			if (!movie_fd)
			{
#ifdef JP
				msg_print("�t�@�C�����J���܂���I");
#else
				msg_print("Can not open file.");
#endif
				return;
			}

			movie_mode = 1;
#ifdef CHUUKEI
			if (!chuukei_server) prepare_chuukei_hooks();
#else
			prepare_chuukei_hooks();
#endif
			do_cmd_redraw();
		}
	}
}

#ifdef CHUUKEI
static int handle_timestamp_data(int timestamp)
{
	long current_time = get_current_time();

	/* �`��L���[�͋󂩂ǂ����H */
	if (fresh_queue.tail == fresh_queue.next)
	{
		/* �o�b�t�@�����O���n�߂̎��Ԃ�ۑ����Ă��� */
		epoch_time = current_time;
		epoch_time += browse_delay;
		epoch_time -= timestamp;
		time_diff = current_time - timestamp;
	}

	/* �`��L���[�ɕۑ����A�ۑ��ʒu��i�߂� */
	fresh_queue.time[fresh_queue.tail] = timestamp;
	fresh_queue.tail ++;

	/* �L���[�̍Ō���ɓ��B������擪�ɖ߂� */
	fresh_queue.tail %= FRESH_QUEUE_SIZE;

	if (fresh_queue.tail == fresh_queue.next)
	{
		/* �`��L���[��� */
		prt("�`��^�C�~���O�L���[�����܂����B�T�[�o�Ƃ̐ڑ���ؒf���܂��B", 0, 0);
		inkey();
		close(sd);

		return -1;
	}

	/* �v���C���Ƃ̃f�B���C�𒲐� */
	if (time_diff != current_time - timestamp)
	{
		long old_time_diff = time_diff;
		time_diff = current_time - timestamp;
		epoch_time -= (old_time_diff - time_diff);
	}

	/* Success */
	return 0;
}
#endif /* CHUUKEI */

static int handle_movie_timestamp_data(int timestamp)
{
	static int initialized = FALSE;

	/* �`��L���[�͋󂩂ǂ����H */
	if (!initialized)
	{
		/* �o�b�t�@�����O���n�߂̎��Ԃ�ۑ����Ă��� */
		epoch_time = get_current_time();
		epoch_time += browse_delay;
		epoch_time -= timestamp;
		//time_diff = current_time - timestamp;
		initialized = TRUE;
	}

	/* �`��L���[�ɕۑ����A�ۑ��ʒu��i�߂� */
	fresh_queue.time[fresh_queue.tail] = timestamp;
	fresh_queue.tail ++;

	/* �L���[�̍Ō���ɓ��B������擪�ɖ߂� */
	fresh_queue.tail %= FRESH_QUEUE_SIZE;

	/* Success */
	return 0;
}

#ifdef CHUUKEI
static int read_sock(void)
{
	static char recv_buf[RECVBUF_SIZE];
	static int remain_bytes = 0;
	int recv_bytes;
	int i;

	/* �O��c�����f�[�^�̌�ɂÂ��Ĕz�M�T�[�o����f�[�^��M */
	recv_bytes = recv(sd, recv_buf + remain_bytes, RECVBUF_SIZE - remain_bytes, 0);

	if (recv_bytes <= 0)
		return -1;

	/* �O��c�����f�[�^�ʂɍ���ǂ񂾃f�[�^�ʂ�ǉ� */
	remain_bytes += recv_bytes;

	for (i = 0; i < remain_bytes; i ++)
	{
		/* �f�[�^�̂�����('\0')��T�� */
		if (recv_buf[i] == '\0')
		{
			/* 'd'�Ŏn�܂�f�[�^(�^�C���X�^���v)�̏ꍇ��
			   �`��L���[�ɕۑ����鏈�����Ă� */
			if ((recv_buf[0] == 'd') &&
			    (handle_timestamp_data(atoi(recv_buf + 1)) < 0))
				return -1;

			/* ��M�f�[�^��ۑ� */
			if (insert_ringbuf(recv_buf) < 0) 
				return -1;

			/* ���̃f�[�^�ڍs��recv_buf�̐擪�Ɉړ� */
			memmove(recv_buf, recv_buf + i + 1, remain_bytes - i - 1);

			remain_bytes -= (i+1);
			i = 0;
		}
	}

	return 0;
}
#endif

static int read_movie_file(void)
{
	static char recv_buf[RECVBUF_SIZE];
	static int remain_bytes = 0;
	int recv_bytes;
	int i;

	recv_bytes = read(movie_fd, recv_buf + remain_bytes, RECVBUF_SIZE - remain_bytes);

	if (recv_bytes <= 0)
		return -1;

	/* �O��c�����f�[�^�ʂɍ���ǂ񂾃f�[�^�ʂ�ǉ� */
	remain_bytes += recv_bytes;

	for (i = 0; i < remain_bytes; i ++)
	{
		/* �f�[�^�̂�����('\0')��T�� */
		if (recv_buf[i] == '\0')
		{
			/* 'd'�Ŏn�܂�f�[�^(�^�C���X�^���v)�̏ꍇ��
			   �`��L���[�ɕۑ����鏈�����Ă� */
			if ((recv_buf[0] == 'd') &&
			    (handle_movie_timestamp_data(atoi(recv_buf + 1)) < 0))
				return -1;

			/* ��M�f�[�^��ۑ� */
			if (insert_ringbuf(recv_buf) < 0) 
				return -1;

			/* ���̃f�[�^�ڍs��recv_buf�̐擪�Ɉړ� */
			memmove(recv_buf, recv_buf + i + 1, remain_bytes - i - 1);

			remain_bytes -= (i+1);
			i = 0;
		}
	}

	return 0;
}


#ifndef WINDOWS
/* Win�ł̏��̒��_�ƕǂ̓������s���I�h�ƃV���[�v�ɂ���B*/
static void win2unix(int col, char *buf)
{
	char kabe;
	if ( col == 9 ) kabe = '%';
	else            kabe = '#';

	while (*buf)
	{
#ifdef JP
		if (iskanji(*buf))
		{
			buf += 2;
			continue;
		}
#endif
		if (*buf == 127) *buf = kabe;
		else if(*buf == 31) *buf = '.';
		buf++;
	}
}
#endif

static bool get_nextbuf(char *buf)
{
	char *ptr = buf;

	while (1)
	{
		*ptr = ring.buf[ring.rptr ++];
		ring.inlen --;
		if (ring.rptr == RINGBUF_SIZE) ring.rptr = 0;
		if (*ptr++ == '\0') break;
	}

	if (buf[0] == 'd') return (FALSE);

	return (TRUE);
}

/* �v���C�z�X�g�̃}�b�v���傫���Ƃ��N���C�A���g�̃}�b�v�����T�C�Y���� */
static void update_term_size(int x, int y, int len)
{
	int ox, oy;
	int nx, ny;
	Term_get_size(&ox, &oy);
	nx = ox;
	ny = oy;

	/* �������̃`�F�b�N */
	if (x + len > ox) nx = x + len;
	/* �c�����̃`�F�b�N */
	if (y + 1 > oy) ny = y + 1;

	if (nx != ox || ny != oy) Term_resize(nx, ny);
}

static bool flush_ringbuf_client(void)
{
	char buf[1024];

	/* �����f�[�^�Ȃ� */
	if (fresh_queue.next == fresh_queue.tail) return (FALSE);

	/* �܂������ׂ����łȂ� */
	if (fresh_queue.time[fresh_queue.next] > get_current_time() - epoch_time) return (FALSE);

	/* ���ԏ��(��؂�)��������܂ŏ��� */
	while (get_nextbuf(buf))
	{
		char id;
		int x, y, len, col;
		int i;
		unsigned char tmp1, tmp2, tmp3, tmp4;
		char *mesg;

		sscanf(buf, "%c%c%c%c%c", &id, &tmp1, &tmp2, &tmp3, &tmp4);
		x = tmp1-1; y = tmp2-1; len = tmp3; col = tmp4;
		if (id == 's')
		{
			col = tmp3;
			mesg = &buf[4];
		}
		else mesg = &buf[5];
#ifndef WINDOWS
		win2unix(col, mesg);
#endif

		switch (id)
		{
		case 't': /* �ʏ� */
#ifdef SJIS
			euc2sjis(mesg);
#endif
			update_term_size(x, y, len);
			(void)((*angband_term[0]->text_hook)(x, y, len, (byte)col, mesg));
			strncpy(&Term->scr->c[y][x], mesg, len);
			for (i = x; i < x+len; i++)
			{
				Term->scr->a[y][i] = col;
			}
			break;

		case 'n': /* �J��Ԃ� */
			for (i = 1; i < len; i++)
			{
				mesg[i] = mesg[0];
			}
			mesg[i] = '\0';
			update_term_size(x, y, len);
			(void)((*angband_term[0]->text_hook)(x, y, len, (byte)col, mesg));
			strncpy(&Term->scr->c[y][x], mesg, len);
			for (i = x; i < x+len; i++)
			{
				Term->scr->a[y][i] = col;
			}
			break;

		case 's': /* �ꕶ�� */
			update_term_size(x, y, 1);
			(void)((*angband_term[0]->text_hook)(x, y, 1, (byte)col, mesg));
			strncpy(&Term->scr->c[y][x], mesg, 1);
			Term->scr->a[y][x] = col;
			break;

		case 'w':
			update_term_size(x, y, len);
			(void)((*angband_term[0]->wipe_hook)(x, y, len));
			break;

		case 'x':
			if (x == TERM_XTRA_CLEAR) Term_clear();
			(void)((*angband_term[0]->xtra_hook)(x, 0));
			break;

		case 'c':
			update_term_size(x, y, 1);
			(void)((*angband_term[0]->curs_hook)(x, y));
			break;
		case 'C':
			update_term_size(x, y, 1);
			(void)((*angband_term[0]->bigcurs_hook)(x, y));
			break;
		}
	}

	fresh_queue.next++;
	if (fresh_queue.next == FRESH_QUEUE_SIZE) fresh_queue.next = 0;
	return (TRUE);
}

#ifdef CHUUKEI
void browse_chuukei()
{
#ifndef MACINTOSH
	fd_set fdset;
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = WAIT;

	FD_ZERO(&fdset);
	FD_SET(sd, &fdset);

	Term_clear();
	Term_fresh();
	Term_xtra(TERM_XTRA_REACT, 0);

	while (1)
	{
		fd_set tmp_fdset;
		struct timeval tmp_tv;

		if (flush_ringbuf_client()) continue;

		tmp_fdset = fdset;
		tmp_tv = tv;

		/* �\�P�b�g�Ƀf�[�^�����Ă��邩�ǂ������ׂ� */
		select(sd+1, &tmp_fdset, (fd_set *)NULL, (fd_set *)NULL, &tmp_tv);
		if (FD_ISSET(sd, &tmp_fdset) == 0)
		{
			Term_xtra(TERM_XTRA_FLUSH, 0);
			continue;
		}

		if (read_sock() < 0)
		{
			chuukei_client = FALSE;
		}

		/* �ڑ����؂ꂽ��Ԃŏ����ׂ��f�[�^���Ȃ��Ȃ��Ă�����I�� */
		if (!chuukei_client && fresh_queue.next == fresh_queue.tail ) break;
	}
#else
	Term_clear();
	Term_fresh();
	Term_xtra(TERM_XTRA_REACT, 0);

	while (1)
	{
		UInt32	unreadData = 0;
		int n;

		if (flush_ringbuf_client()) continue;

		/* �\�P�b�g�Ƀf�[�^�����Ă��邩�ǂ������ׂ� */

		OTCountDataBytes(ep, &unreadData);
		if(unreadData <= 0 ){
			Term_xtra(TERM_XTRA_FLUSH, 0);
			continue;
		}
		if (read_sock() < 0)
		{
			chuukei_client = FALSE;
		}

		/* �ڑ����؂ꂽ��Ԃŏ����ׂ��f�[�^���Ȃ��Ȃ��Ă�����I�� */
		if (!chuukei_client && fresh_queue.next == fresh_queue.tail ) break;
	}
#endif /*MACINTOSH*/
}
#endif /* CHUUKEI */

void prepare_browse_movie_aux(cptr filename)
{
	movie_fd = fd_open(filename, O_RDONLY);
	
	browsing_movie = TRUE;

	init_buffer();
}

void prepare_browse_movie(cptr filename)
{
	char buf[1024];
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, filename);

	prepare_browse_movie_aux(buf);
}

void browse_movie(void)
{
	Term_clear();
	Term_fresh();
	Term_xtra(TERM_XTRA_REACT, 0);

	while (read_movie_file() == 0)
	{
		while (fresh_queue.next != fresh_queue.tail)
		{
			if (!flush_ringbuf_client())
			{
				Term_xtra(TERM_XTRA_FLUSH, 0);

				/* �\�P�b�g�Ƀf�[�^�����Ă��邩�ǂ������ׂ� */
#ifdef WINDOWS
				Sleep(WAIT);
#else
				usleep(WAIT);
#endif
			}
		}
	}
}
