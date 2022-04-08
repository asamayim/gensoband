/* File: report.c */

#define _GNU_SOURCE
#include "angband.h"

#ifdef WORLD_SCORE

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

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

#include <setjmp.h>
#include <signal.h>
#endif

/*
 * internet resource value
 */
///system
/*:::�X�R�A���M�֌W�@�ۂ��ƃR�����g�A�E�g���Ƃ���*/
#define HTTP_PROXY ""                   /* Default proxy url */
#define HTTP_PROXY_PORT 0               /* Default proxy port */
#define HTTP_TIMEOUT    20              /* Timeout length (second) */
//v1.1.25 �X�R�A�T�[�o�A�h���X
#define SCORE_SERVER "www.miyamasa.net"    /* Default score server url */
#define SCORE_PORT 80                   /* Default score server port */

#ifdef SCORE_SERVER_TEST

//�e�X�g�T�[�o
#define SCORE_PATH "/score_test/receive_httppost.cgi"

#else

#define SCORE_PATH "/score/receive_httppost.cgi"


#endif



#define MULTIPART_BOUNDARY	"---------------------------8721656041911415653955004"

/*
 * simple buffer library
 */

typedef struct {
	size_t max_size;
	size_t size;
	char *data;
} BUF;

#define	BUFSIZE	(65536)

static BUF* buf_new(void)
{
	BUF *p;

	if ((p = malloc(sizeof(BUF))) == NULL)
		return NULL;

	p->size = 0;
	p->max_size = BUFSIZE;
	if ((p->data = malloc(BUFSIZE)) == NULL)
	{
		free(p);
		return NULL;
	}
	return p;
}

static void buf_delete(BUF *b)
{
	free(b->data);
	free(b);
}

static int buf_append(BUF *buf, const char *data, size_t size)
{
	while (buf->size + size > buf->max_size)
	{
		char *tmp;
		if ((tmp = malloc(buf->max_size * 2)) == NULL) return -1;

		memcpy(tmp, buf->data, buf->max_size);
		free(buf->data);

		buf->data = tmp;

		buf->max_size *= 2;
	}
	memcpy(buf->data + buf->size, data, size);
	buf->size += size;

	return buf->size;
}

static int buf_sprintf(BUF *buf, const char *fmt, ...)
{
	int		ret;
	char	tmpbuf[8192];
	va_list	ap;

	va_start(ap, fmt);
#if defined(HAVE_VSNPRINTF)
	ret = vsnprintf(tmpbuf, sizeof(tmpbuf), fmt, ap);
#else
	ret = vsprintf(tmpbuf, fmt, ap);
#endif
	va_end(ap);

	if (ret < 0) return -1;

#if ('\r' == 0x0a && '\n' == 0x0d)
	{
		/*
		 * Originally '\r'= CR (= 0x0d) and '\n'= LF (= 0x0a)
		 * But for MPW (Macintosh Programers Workbench), these
		 * are reversed so that '\r'=LF and '\n'=CR unless the
		 * -noMapCR option is not defined.
		 *
		 * We need to swap back these here since the score
		 * dump text should be written using LF as the end of
		 * line.
		 */
		char *ptr;
		for (ptr = tmpbuf; *ptr; ptr++)
		{
			if (0x0d == *ptr) *ptr = 0x0a;
		}
	}
#endif

	ret = buf_append(buf, tmpbuf, strlen(tmpbuf));

	return ret;
}

#if 0
static int buf_read(BUF *buf, int fd)
{
	int len;
#ifndef MACINTOSH
	char tmp[BUFSIZE];
#else
	char *tmp;
	
	tmp = calloc( BUFSIZE , sizeof(char) );
#endif

	while ((len = read(fd, tmp, BUFSIZE)) > 0)
		buf_append(buf, tmp, len);

	return buf->size;
}
#endif

#if 0
static int buf_write(BUF *buf, int fd)
{
	write(fd, buf->data, buf->size);

	return buf->size;
}

static int buf_search(BUF *buf, const char *str)
{
	char *ret;

	ret = my_strstr(buf->data, str);

	if (!ret) return -1;

	return ret - buf->data;
}

static BUF * buf_subbuf(BUF *buf, int pos1, size_t sz)
{
	BUF *ret;

	if (pos1 < 0) return NULL;

	ret = buf_new();

	if (sz <= 0) sz = buf->size - pos1;

	buf_append(ret, buf->data + pos1, sz);

	return ret;
}
#endif

static void http_post(int sd, cptr url, BUF *buf)
{
	BUF *output;
	output = buf_new();

/*:::�X�R�A�T�[�o�ɑ��M����HTML�w�b�_���`��*/
	buf_sprintf(output, "POST %s HTTP/1.1\r\n",url);
	buf_sprintf(output, "Connection: Keep-Alive\r\n");//�ǉ�
	buf_sprintf(output, "User-Agent: Hengband_Katte_Ban\r\n");//v1.1.25
	buf_sprintf(output, "Host: %s:%d\r\n",SCORE_SERVER,SCORE_PORT);
	buf_sprintf(output, "Accept: text/html\r\n");//v1.1.25
	buf_sprintf(output, "Content-Type: multipart/form-data; boundary=%s\r\n",MULTIPART_BOUNDARY);
	buf_sprintf(output, "Content-Length: %d\r\n", buf->size);
	buf_sprintf(output, "\r\n");
	buf_append(output, buf->data, buf->size);
	soc_write(sd, output->data, output->size);
}


/* �L�����N�^�_���v������� BUF�ɕۑ� */
static errr make_dump(BUF* dumpbuf)
{
	char		buf[1024];
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff)
	{
#ifdef JP
		msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
		msg_format("Failed to create temporary file %s.", file_name);
#endif
		msg_print(NULL);
		return 1;
	}

	/* ��U�ꎞ�t�@�C�������B�ʏ�̃_���v�o�͂Ƌ��ʉ����邽�߁B */
	(void)make_character_dump(fff);

	/* Close the file */
	my_fclose(fff);

	/* Open for read */
	fff = my_fopen(file_name, "r");

	while (fgets(buf, 1024, fff))
	{
		(void)buf_sprintf(dumpbuf, "%s", buf);
	}

	/* Close the file */
	my_fclose(fff);

	/* Remove the file */
	fd_kill(file_name);

	/* Success */
	return (0);
}

/*
 * Make screen dump to buffer
 */
/*:::�X�N���[���V���b�g��html�ŎB��@�ڍז���*/
cptr make_screen_dump(void)
{
	BUF *screen_buf;
	int y, x, i;
	cptr ret;

	byte a = 0, old_a = 0;
	char c = ' ';

	static cptr html_head[] = {
		"<html>\n<body text=\"#ffffff\" bgcolor=\"#000000\">\n",
		"<pre>",
		0,
	};
	static cptr html_foot[] = {
		"</pre>\n",
		"</body>\n</html>\n",
		0,
	};

	bool old_use_graphics = use_graphics;

	int wid, hgt;


	Term_get_size(&wid, &hgt);

	/* Alloc buffer */
	screen_buf = buf_new();
	if (screen_buf == NULL) return (NULL);

	if (old_use_graphics)
	{
		/* Clear -more- prompt first */
		msg_print(NULL);

		use_graphics = FALSE;
		reset_visuals();

		/* Redraw everything */
		p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

		/* Hack -- update */
		handle_stuff();
	}

	for (i = 0; html_head[i]; i++)
		buf_sprintf(screen_buf, html_head[i]);

	/* Dump the screen */
	for (y = 0; y < hgt; y++)
	{
		/* Start the row */
		if (y != 0)
			buf_sprintf(screen_buf, "\n");

		/* Dump each row */
		for (x = 0; x < wid - 1; x++)
		{
			int rv, gv, bv;
			cptr cc = NULL;
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			switch (c)
			{
			case '&': cc = "&amp;"; break;
			case '<': cc = "&lt;"; break;
			case '>': cc = "&gt;"; break;
#ifdef WINDOWS
			case 0x1f: c = '.'; break;
			case 0x7f: c = (a == 0x09) ? '%' : '#'; break;
#endif
			}

			a = a & 0x0F;
			if ((y == 0 && x == 0) || a != old_a) {
				rv = angband_color_table[a][1];
				gv = angband_color_table[a][2];
				bv = angband_color_table[a][3];
				buf_sprintf(screen_buf, "%s<font color=\"#%02x%02x%02x\">", 
					    ((y == 0 && x == 0) ? "" : "</font>"), rv, gv, bv);
				old_a = a;
			}
			if (cc)
				buf_sprintf(screen_buf, "%s", cc);
			else
				buf_sprintf(screen_buf, "%c", c);
		}
	}
	buf_sprintf(screen_buf, "</font>");

	for (i = 0; html_foot[i]; i++)
		buf_sprintf(screen_buf, html_foot[i]);


	if(cheat_xtra)
		msg_format("screen dump size: %d byte)",screen_buf->size);

	/* Screen dump size is too big ? */
	//v1.1.28 �o�b�t�@�T�C�Y��4�{�ɂ��Ă݂�
	if (screen_buf->size + 1> SCREEN_BUF_SIZE)
	{
		msg_format("(�X�N���[���V���b�g��256KB���z���܂��� %d byte)",screen_buf->size);
		ret = NULL;
	}
	else
	{
		/* Terminate string */
		buf_append(screen_buf, "", 1);

		ret = string_make(screen_buf->data);
	}

	/* Free buffer */
	buf_delete(screen_buf);

	if (old_use_graphics)
	{
		use_graphics = TRUE;
		reset_visuals();

		/* Redraw everything */
		p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

		/* Hack -- update */
		handle_stuff();
	}

	return ret;
}



//v1.1.25 �X�R�A�𑗐M�����Ƃ�CGI�T�[�o����̃��b�Z�[�W��\�����Ă݂�
static bool http_get_msg(int sd)
{
	char buf[2048];
	int read_size;
	FILE *fff;
	char file_name[1024];
	bool success = FALSE;
	bool use_tmpfile = FALSE;
	byte err_code = 0L;

	path_build(file_name, sizeof(buf), ANGBAND_DIR_USER, "http_post_result.log");

	/*:::�T�[�o����̉��������O�t�@�C���ɕۑ� */
	fff = my_fopen(file_name, "w");
	if (!fff)
	{
		msg_format("ERROR:�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
		msg_print(NULL);
		//�w�肵���t�@�C�����J���Ȃ������ꍇ�ꎞ�t�@�C�����g��
		fff = my_fopen_temp(file_name, 1024);
		if(!fff)
		{
			msg_format("ERROR:�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
			msg_format("�t�@�C�����M�ɐ����������ǂ����m�F�ł��܂���I");
			msg_print(NULL);
			return TRUE;
		}
		use_tmpfile = TRUE;
	}


	while (1)
	{
        memset(buf, 0, sizeof(buf));
        read_size = recv(sd, buf, 2048, 0);
        if (read_size > 0)
		{

			if(my_strstr(buf,"RECEIVE_SAVEFILE_SUCCESSFULLY"))
				success = TRUE;

			if(my_strstr(buf,"ERROR:MAINTENANCE"))
				err_code |= 0x01;
			if(my_strstr(buf,"ERROR:CROWDED"))
				err_code |= 0x02;
			if(my_strstr(buf,"ERROR:FILE_OPEN"))
				err_code |= 0x04;
			if(my_strstr(buf,"FILE_PARSE_ERROR"))
				err_code |= 0x08;
			if (my_strstr(buf, "GET_NUM_ERROR"))
				err_code |= 0x10;


			fprintf(fff,"%s",buf);
        }
        else 
		{
            break;
        }
    }

	/* Close the file */
	my_fclose(fff);

	//���s�̂Ƃ��̓T�[�o����Ԃ��Ă������b�Z�[�W����ʂɕ\������
	if(err_code )
	{

		//v1.1.93 �G���[���b�Z�[�W(html�t�@�C��)����ʂɕ\������̂���߂�
#if 0
			/* Open for read */
			fff = my_fopen(file_name, "r");
			while (fgets(buf, 1024, fff))
			{
				msg_format("%s", buf);

			}
			/* Close the file */
			my_fclose(fff);
			if (use_tmpfile)	fd_kill(file_name);

#endif

		if(err_code & 0x01)
			msg_print("CGI�T�[�o�������e�i���X���ł��B");
		else if(err_code & 0x02)
			msg_print("CGI�T�[�o�����ݍ����Ă��܂��A�������̓T�[�o�g���u���ł��B");
		else if(err_code & 0x04)
			msg_print("CGI�T�[�o���Ńt�@�C���̕ۑ��Ɏ��s���܂����B");
		else if(err_code & 0x08)
			msg_print("�f�[�^�̉�͂Ɏ��s���܂����B");
		else if (err_code & 0x10)
			msg_print("�T�[�o�g���u���ɂ��t�@�C���ԍ��̎擾�Ɏ��s���܂����B");
		else
			msg_print("ERROR:CGI�T�[�o����̉������s���ł�");

		if(!use_tmpfile)
			msg_format("�ڍׂ�user�t�H���_�̃��O�t�@�C�����Q�Ƃ��Ă�������");


	}


	return success;
}

/*:::�X�R�A�𑗐M����*/
//v1.1.25�@�X�R�A���M�����B
//HTTP�}���`�p�[�g�t�H�[�����g���ă_���v�e�L�X�g�AHTML�X�N���[���V���b�g�A�Z�[�u�t�@�C���𑗂�悤�ɂ���
errr report_score(void)
{
#ifdef MACINTOSH
	OSStatus err;
#else
	errr err = 0;
#endif

#ifdef WINDOWS
	WSADATA wsaData;
	WORD wVersionRequested =(WORD) (( 1) |  ( 1 << 8));
#endif

	BUF *score;
	int sd;
	char seikakutmp[128];
	char savefilename[1024];
	bool send_savefile = TRUE;

	FILE *fp_tmp;

	score = buf_new();
/*
#ifdef JP
	sprintf(seikakutmp, "%s%s", ap_ptr->title, (ap_ptr->no ? "��" : ""));
#else
	sprintf(seikakutmp, "%s ", ap_ptr->title);
#endif

	buf_sprintf(score, "name: %s\r\n", player_name);
#ifdef JP
	buf_sprintf(score, "version: �ϋ�ؓ{(�����) %d.%d.%d\r\n",
		    H_VER_MAJOR-10, H_VER_MINOR, H_VER_PATCH);
#else
	buf_sprintf(score, "version: Hengband %d.%d.%d\n",
		    FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#endif
	buf_sprintf(score, "score: %d\r\n", total_points_new(FALSE));
	buf_sprintf(score, "level: %d\r\n", p_ptr->lev);
	buf_sprintf(score, "depth: %d\r\n", dun_level);
	buf_sprintf(score, "maxlv: %d\r\n", p_ptr->max_plv);
	buf_sprintf(score, "maxdp: %d\r\n", max_dlv[DUNGEON_ANGBAND]);
	buf_sprintf(score, "au: %d\r\n", p_ptr->au);
	buf_sprintf(score, "turns: %d\r\n", turn_real(turn));
	buf_sprintf(score, "sex: %d\r\n", p_ptr->psex);
	buf_sprintf(score, "race: %s\r\n", rp_ptr->title);
	buf_sprintf(score, "class: %s\r\n", cp_ptr->title);
	buf_sprintf(score, "seikaku: %s\r\n", seikakutmp);
	buf_sprintf(score, "realm1: %s\r\n", realm_names[p_ptr->realm1]);
	buf_sprintf(score, "realm2: %s\r\n", realm_names[p_ptr->realm2]);
	buf_sprintf(score, "killer: %s\r\n", p_ptr->died_from);
//	buf_sprintf(score, "-----charcter dump-----\r\n");
//		buf_sprintf(score, "-----screen shot-----\r\n");
*/


/*:::�X�R�A�T�[�o�ɑ��M����}���`�p�[�g�t�H�[���f�[�^���`��*/

	//�X�N���[���V���b�g�𑗐M(�{�X�œ|���A���邢�̓Q�[���I�[�o�[���ɍ����)
	if(!p_ptr->is_dead) //�e�X�g�p
	{
		screen_dump = make_screen_dump();
	}
	if (screen_dump)
	{
		//���ۂɋ�؂������Ƃ���boundary=..�̂Ƃ����-��2�����v��
		buf_sprintf(score, "--%s\r\n", MULTIPART_BOUNDARY);
		buf_sprintf(score, "Content-Disposition: form-data; name=\"screen_shot\"\r\n");
		buf_sprintf(score, "Content-Type: text/html\r\n\r\n");

		buf_append(score, screen_dump, strlen(screen_dump));
	}

	//�X�R�A�_���v�𑗐M
	{
		buf_sprintf(score, "--%s\r\n", MULTIPART_BOUNDARY);
		buf_sprintf(score, "Content-Disposition: form-data; name=\"dump_file\"\r\n");
		buf_sprintf(score, "Content-Type: text/html\r\n\r\n");
		make_dump(score);

	}

	//�Z�[�u�t�@�C���𑗐M
	{

		fp_tmp = my_fopen(savefile,"rb");
		if(!fp_tmp)
		{
			msg_format("ERROR:�Z�[�u�t�@�C��%s�̎擾�Ɏ��s",savefile);
			err = 1;
		}
		else
		{
			int tmp_size;
			int n;
			char *tmp_buf;

			buf_sprintf(score, "--%s\r\n", MULTIPART_BOUNDARY);
			buf_sprintf(score, "Content-Disposition: form-data; name=\"save_file\"\r\n");
			buf_sprintf(score, "Content-Type: application/octet-stream\r\n");
			buf_sprintf(score, "Content-Transfer-Encoding: binary\r\n\r\n");

			fseek(fp_tmp,0,SEEK_END);
			tmp_size = ftell(fp_tmp);
			tmp_buf = (char *)malloc(tmp_size);
			if(tmp_buf == NULL)
			{
				msg_print("ERROR:malloc�ł̗̈�m�ۂɎ��s�H");
				err = 1;
			}
			else
			{
				rewind(fp_tmp);
				n = fread(tmp_buf,1,tmp_size,fp_tmp);
				buf_append(score,tmp_buf,tmp_size);
			}
			free(tmp_buf);

			my_fclose(fp_tmp);
		}
	}

	buf_sprintf(score, "--%s--\r\n", MULTIPART_BOUNDARY);//�Ō�̃o�E���_���̌��ɂ͂����--���v��

	
#ifdef WINDOWS
	if (WSAStartup(wVersionRequested, &wsaData))
	{
		msg_print("Report: WSAStartup failed.");
		err = 1;
		goto report_end;
	}
#endif

#ifdef MACINTOSH
#if TARGET_API_MAC_CARBON
	err = InitOpenTransportInContext(kInitOTForApplicationMask, NULL);
#else
	err = InitOpenTransport();
#endif
	if (err != noErr)
	{
		msg_print("Report: OpenTransport failed.");
		return 1;
	}
#endif

	Term_clear();

	if(!err) while (1)
	{
		char buff[160];
#ifdef JP
		prt("�ڑ���...", 0, 0);
#else
		prt("connecting...", 0, 0);
#endif
		Term_fresh();
		
		/* �v���L�V��ݒ肷�� */
		set_proxy(HTTP_PROXY, HTTP_PROXY_PORT);

		/* Connect to the score server */
		sd = connect_server(HTTP_TIMEOUT, SCORE_SERVER, SCORE_PORT);


		if (!(sd < 0)) break;
#ifdef JP
		sprintf(buff, "�X�R�A�E�T�[�o�ւ̐ڑ��Ɏ��s���܂����B(%s)", soc_err());
#else
		sprintf(buff, "Failed to connect to the score server.(%s)", soc_err());
#endif
		prt(buff, 0, 0);
		(void)inkey();
		
#ifdef JP
		if (!get_check_strict("������x�ڑ������݂܂���? ", CHECK_NO_HISTORY))
#else
		if (!get_check_strict("Try again? ", CHECK_NO_HISTORY))
#endif
		{
			err = 1;
			goto report_end;
		}
	}




	if(!err) 
	{
	#ifdef JP
		prt("�X�R�A���M��...", 0, 0);
	#else
		prt("Sending the score...", 0, 0);
	#endif
		Term_fresh();
		http_post(sd, SCORE_PATH, score);
		//http_post(sd, SCORE_TEST_PATH, score);

		//�T�[�o����̃��b�Z�[�W����M����B�����̃��b�Z�[�W���Ȃ�������G���[�t���O
		if(!http_get_msg(sd))
		{
			err = 1;
		}

		disconnect_server(sd);

	}

 report_end:
#ifdef WINDOWS
	WSACleanup();
#endif

#ifdef MACINTOSH
#if TARGET_API_MAC_CARBON
	CloseOpenTransportInContext(NULL);
#else
	CloseOpenTransport();
#endif
#endif

	return err;
}

#endif /* WORLD_SCORE */
