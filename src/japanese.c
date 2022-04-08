/* File: japanese.c */


#include "angband.h"

#ifdef JP

typedef struct convert_key convert_key;

struct convert_key
{
	cptr key1;
	cptr key2;
};

static const convert_key s2j_table[] = {
	{"mb","nb"}, {"mp","np"}, {"mv","nv"}, {"mm","nm"},
	{"x","ks"},
	/* sindar:�V���_�[��  parantir:�p�����e�B�A  feanor:�t�F�A�m�[�� */
	{"ar$","a-ru$"}, {"ir$","ia$"}, {"or$","o-ru$"},
	{"ra","��"}, {"ri","��"}, {"ru","��"}, {"re","��"}, {"ro","��"},
	{"ir","ia"}, {"ur","ua"}, {"er","ea"}, {"ar","a��"},
	{"sha","�V��"}, {"shi","�V"}, {"shu","�V��"}, {"she","�V�F"}, {"sho","�V��"},
	{"tha","�T"}, {"thi","�V"}, {"thu","�X"}, {"the","�Z"}, {"tho","�\"},
	{"cha","�n"}, {"chi","�q"}, {"chu","�t"}, {"che","�w"}, {"cho","�z"},
	{"dha","�U"}, {"dhi","�W"}, {"dhu","�Y"}, {"dhe","�["}, {"dho","�]"},
	{"ba","�o"}, {"bi","�r"}, {"bu","�u"}, {"be","�x"}, {"bo","�{"},
	{"ca","�J"}, {"ci","�L"}, {"cu","�N"}, {"ce","�P"}, {"co","�R"},
	{"da","�_"}, {"di","�f�B"}, {"du","�h�D"}, {"de","�f"}, {"do","�h"},
	{"fa","�t�@"}, {"fi","�t�B"}, {"fu","�t"}, {"fe","�t�F"}, {"fo","�t�H"},
	{"ga","�K"}, {"gi","�M"}, {"gu","�O"}, {"ge","�Q"}, {"go","�S"},
	{"ha","�n"}, {"hi","�q"}, {"hu","�t"}, {"he","�w"}, {"ho","�z"},
	{"ja","�W��"}, {"ji","�W"}, {"ju","�W��"}, {"je","�W�F"}, {"jo","�W��"},
	{"ka","�J"}, {"ki","�L"}, {"ku","�N"}, {"ke","�P"}, {"ko","�R"},
	{"la","��"}, {"li","��"}, {"lu","��"}, {"le","��"}, {"lo","��"},
	{"ma","�}"}, {"mi","�~"}, {"mu","��"}, {"me","��"}, {"mo","��"},
	{"na","�i"}, {"ni","�j"}, {"nu","�k"}, {"ne","�l"}, {"no","�m"},
	{"pa","�p"}, {"pi","�s"}, {"pu","�v"}, {"pe","�y"}, {"po","�|"},
	{"qu","�N"},
	{"sa","�T"}, {"si","�V"}, {"su","�X"}, {"se","�Z"}, {"so","�\"},
	{"ta","�^"}, {"ti","�e�B"}, {"tu","�g�D"}, {"te","�e"}, {"to","�g"},
	{"va","���@"}, {"vi","���B"}, {"vu","��"}, {"ve","���F"}, {"vo","���H"},
	{"wa","��"}, {"wi","�E�B"}, {"wu","�E"}, {"we","�E�F"}, {"wo","�E�H"},
	{"ya","��"}, {"yu","��"}, {"yo","��"},
	{"za","�U"}, {"zi","�W"}, {"zu","�Y"}, {"ze","�["}, {"zo","�]"},
	{"dh","�Y"}, {"ch","�t"}, {"th","�X"},
	{"b","�u"}, {"c","�N"}, {"d","�h"}, {"f","�t"}, {"g","�O"},
	{"h","�t"}, {"j","�W��"}, {"k","�N"}, {"l","��"}, {"m","��"},
	{"n","��"}, {"p","�v"}, {"q","�N"}, {"r","��"}, {"s","�X"},
	{"t","�g"}, {"v","��"}, {"w","�E"}, {"y","�C"},
	{"a","�A"}, {"i","�C"}, {"u","�E"}, {"e","�G"}, {"o","�I"},
	{"-","�["},
	{NULL,NULL}
};

/* �V���_��������{��̓ǂ݂ɕϊ����� */
void sindarin_to_kana(char *kana, const char *sindarin)
{
	char buf[256];
	int idx;

	sprintf(kana, "%s$", sindarin);
	for (idx = 0; kana[idx]; idx++)
		if (isupper(kana[idx])) kana[idx] = tolower(kana[idx]);

	for (idx = 0; s2j_table[idx].key1 != NULL; idx++)
	{
		cptr pat1 = s2j_table[idx].key1;
		cptr pat2 = s2j_table[idx].key2;
		int len = strlen(pat1);
		char *src = kana;
		char *dest = buf;

		while (*src)
		{
			if (strncmp(src, pat1, len) == 0)
			{
				strcpy(dest, pat2);
				src += len;
				dest += strlen(pat2);
			}
			else
			{
				if (iskanji(*src))
				{
					*dest = *src;
					src++;
					dest++;
				}
				*dest = *src;
				src++;
				dest++;
			}
		}

		*dest = 0;
		strcpy(kana, buf);
	}

	idx = 0;

	while (kana[idx] != '$') idx++;

	kana[idx] = '\0';
}


/*���{�ꓮ�����p (�ł��ł���,�ł� etc) */

#define CMPTAIL(y) strncmp(&in[l-(int)strlen(y)],y,strlen(y))

/* ����,�R�遄����,�R�� */
void jverb1( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�Ă�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�ł�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�˂�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�ւ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�ׂ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�߂�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"��");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else

  sprintf(&out[l],"������");}

/* ����,�R��> �����ďR�� */
void jverb2( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("�Ă�")==0) sprintf(&out[l-4],"�Ă���");else
if( CMPTAIL("�ł�")==0) sprintf(&out[l-4],"�ł�");else
if( CMPTAIL("�˂�")==0) sprintf(&out[l-4],"�˂�");else
if( CMPTAIL("�ւ�")==0) sprintf(&out[l-4],"�ւ�");else
if( CMPTAIL("�ׂ�")==0) sprintf(&out[l-4],"�ׂ�");else
if( CMPTAIL("�߂�")==0) sprintf(&out[l-4],"�߂�");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"���");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�˂�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ւ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
  sprintf(&out[l],"���Ƃɂ��");}

/* ����,�R�� > ��������R������ */
void jverb3( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("�Ă�")==0) sprintf(&out[l-4],"�Ă���");else
if( CMPTAIL("�ł�")==0) sprintf(&out[l-4],"�ł�");else
if( CMPTAIL("�˂�")==0) sprintf(&out[l-4],"�˂�");else
if( CMPTAIL("�ւ�")==0) sprintf(&out[l-4],"�ւ�");else
if( CMPTAIL("�ׂ�")==0) sprintf(&out[l-4],"�ׂ�");else
if( CMPTAIL("�߂�")==0) sprintf(&out[l-4],"�߂�");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"�ꂽ");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�˂�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ւ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
  sprintf(&out[l],"���Ƃ�");}


void jverb( const char *in , char *out , int flag){
  switch (flag){
  case JVERB_AND:jverb1(in , out);break;
  case JVERB_TO :jverb2(in , out);break;
  case JVERB_OR :jverb3(in , out);break;
  }
}


/*
 * Convert SJIS string to EUC string
 */
void sjis2euc(char *str)
{
	int i;
	unsigned char c1, c2;
	unsigned char *tmp;

	int len = strlen(str);

	C_MAKE(tmp, len+1, byte);

	for (i = 0; i < len; i++)
	{
		c1 = str[i];
		if (c1 & 0x80)
		{
			i++;
			c2 = str[i];
			if (c2 >= 0x9f)
			{
				c1 = c1 * 2 - (c1 >= 0xe0 ? 0xe0 : 0x60);
				c2 += 2;
			}
			else
			{
				c1 = c1 * 2 - (c1 >= 0xe0 ? 0xe1 : 0x61);
				c2 += 0x60 + (c2 < 0x7f);
			}
			tmp[i - 1] = c1;
			tmp[i] = c2;
		}
		else
			tmp[i] = c1;
	}
	tmp[len] = 0;
	strcpy(str, (char *)tmp);

	C_KILL(tmp, len+1, byte);
}  


/*
 * Convert EUC string to SJIS string
 */
void euc2sjis(char *str)
{
	int i;
	unsigned char c1, c2;
	unsigned char *tmp;
	
	int len = strlen(str);

	C_MAKE(tmp, len+1, byte);

	for (i = 0; i < len; i++)
	{
		c1 = str[i];
		if (c1 & 0x80)
		{
			i++;
			c2 = str[i];
			if (c1 % 2)
			{
				c1 = (c1 >> 1) + (c1 < 0xdf ? 0x31 : 0x71);
				c2 -= 0x60 + (c2 < 0xe0);
			}
			else
			{
				c1 = (c1 >> 1) + (c1 < 0xdf ? 0x30 : 0x70);
				c2 -= 2;
			}

			tmp[i - 1] = c1;
			tmp[i] = c2;
		}
		else
			tmp[i] = c1;
	}
	tmp[len] = 0;
	strcpy(str, (char *)tmp);

	C_KILL(tmp, len+1, byte);
}  


/*
 * str�����ɍ����������R�[�h�ɕϊ����A�ϊ��O�̕����R�[�h��Ԃ��B
 * str�̒����ɐ����͂Ȃ��B
 *
 * 0: Unknown
 * 1: ASCII (Never known to be ASCII in this function.)
 * 2: EUC
 * 3: SJIS
 */
byte codeconv(char *str)
{
	byte code = 0;
	int i;

	for (i = 0; str[i]; i++)
	{
		unsigned char c1;
		unsigned char c2;

		/* First byte */
		c1 = str[i];

		/* ASCII? */
		if (!(c1 & 0x80)) continue;

		/* Second byte */
		i++;
		c2 = str[i];

		if (((0xa1 <= c1 && c1 <= 0xdf) || (0xfd <= c1 && c1 <= 0xfe)) &&
		    (0xa1 <= c2 && c2 <= 0xfe))
		{
			/* Only EUC is allowed */
			if (!code)
			{
				/* EUC */
				code = 2;
			}

			/* Broken string? */
			else if (code != 2)
			{
				/* No conversion */
				return 0;
			}
		}

		else if (((0x81 <= c1 && c1 <= 0x9f) &&
			  ((0x40 <= c2 && c2 <= 0x7e) || (0x80 <= c2 && c2 <= 0xfc))) ||
			 ((0xe0 <= c1 && c1 <= 0xfc) &&
			  (0x40 <= c2 && c2 <= 0x7e)))
		{
			/* Only SJIS is allowed */
			if (!code)
			{
				/* SJIS */
				code = 3;
			}

			/* Broken string? */
			else if (code != 3)
			{
				/* No conversion */
				return 0;
			}
		}
	}


	switch (code)
	{
#ifdef EUC
	case 3:
		/* SJIS -> EUC */
		sjis2euc(str);
		break;
#endif

#ifdef SJIS
	case 2:
		/* EUC -> SJIS */
		euc2sjis(str);

		break;
#endif
	}

	/* Return kanji code */
	return code;
}

/* ������s��x�o�C�g�ڂ�������1�o�C�g�ڂ��ǂ������肷�� */
bool iskanji2(cptr s, int x)
{
	int i;

	for (i = 0; i < x; i++)
	{
		if (iskanji(s[i])) i++;
	}
	if ((x == i) && iskanji(s[x])) return TRUE;

	return FALSE;
}

#endif /* JP */

