
#include "angband.h"





/*:::�E�Ƃ��Ƃ̒ǉ��C���x���g���������߂�*/
/*:::memo�F���x���Ȃǃp�����[�^�ˑ��ɂ���Ƃ��̓��x���_�E���ŃC���x���g��������Ȃ��悤�ɍl�������ق��������B�C���x���g���g���Ȃ��Ȃ��Ă������͂��Ȃ��͂�����*/
int calc_inven2_num(void)
{

	int num = 0;
	int pc = p_ptr->pclass;

	//��t�@���10
	if(pc == CLASS_CHEMIST) num = 10;
	//���� 2-5
	else if(pc == CLASS_ORIN) num = 2 + p_ptr->max_plv / 13;
	//�G���W�j�A 2-10
	else if(pc == CLASS_ENGINEER) num = 2 + p_ptr->max_plv / 6;
	//�Ó���@���5
	else if(pc == CLASS_SH_DEALER) num = 5;
	//�ɂƂ� 1-6
	else if(pc == CLASS_NITORI) num = 1 + p_ptr->max_plv / 10;
	//俎q 1-7
	else if(pc == CLASS_SUMIREKO) num = 1 + (p_ptr->max_plv+3) / 8;
	//�A���X1-7
	else if(pc == CLASS_ALICE) num = MIN(7, (1 + (p_ptr->max_plv-3) / 7));
	//���4-8
	else if(pc == CLASS_SAKUYA) num = 4 + p_ptr->max_plv / 12;
	//���ǂ�(��) 2-5
	else if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE)) num = 2 + p_ptr->max_plv / 13;
	//�����@���8
	else if (pc == CLASS_JYOON) num = JYOON_INVEN2_NUM;
	//�J�[�h���l ���8
	else if (pc == CLASS_CARD_DEALER) num = 8;
	//�����ˁ@���16
	else if (pc == CLASS_TAKANE) num = 16;
	//�R�@�@���8
	else if (pc == CLASS_SANNYO) num = 8;
	//�~�P�@���8
	else if (pc == CLASS_MIKE) num = 8;



	if(num > INVEN_ADD_MAX) num = INVEN_ADD_MAX;
	//���o�^�̐E�Ƃ�0���Ԃ��ĊO�ŃG���[����

	return num;
}

#define SI2_MODE_NONE 0
#define SI2_MODE_ENGINEER 1
#define SI2_MODE_ALICE 2
#define SI2_MODE_JYOON	3
#define SI2_MODE_ACTIVATE 4

/*:::�ǉ��A�C�e�����̈ꗗ�\�� display_list_inven2()�̓R�}���h�Ƃ��Ă̈ꗗ�\���ł�����͂����ȃR�}���h����Ă΂��ꎞ�I�Ȉꗗ�\��*/
/*:::mode:SI2_MODE_ENGINEER*/
void show_inven2(int mode)
{
	int i,l,len,col,wid,hgt,cur_col;
	char            tmp_val[80];
	char            o_name[MAX_NLEN];
	int inven2_num = calc_inven2_num();
	int             out_index[INVEN_ADD_MAX];
	byte            out_color[INVEN_ADD_MAX];
	char            out_desc[INVEN_ADD_MAX][MAX_NLEN];
	object_type *o_ptr;

	if(!inven2_num)
	{
		msg_print("ERROR:���̐E�Ƃ̒ǉ��C���x���g��������`����Ă��Ȃ�");
		return ;
	}
	/* Get size */
	Term_get_size(&wid, &hgt);
	col = wid - 30;
	len = wid - col - 1;

	/*:::�ǉ��C���x���g���\���@��ɂ��ׂĂ̗�*/
	for (i = 0; i < inven2_num; i++)
	{
		o_ptr = &inven_add[i];

		/* Is this item acceptable? */
		/*:::�󗓈ȊO�͒ʂ�͂�*/
		//if (!item_tester_okay(o_ptr)) continue;

		if(!o_ptr->k_idx)
		{
			if(mode == SI2_MODE_ALICE)
			{
				out_color[i] = TERM_L_DARK;
				(void)strcpy(out_desc[i], format("%15s:(������)",alice_doll_name[i]));
				l = 13;
			}
			else if (mode == SI2_MODE_JYOON)
			{
				out_color[i] = TERM_L_DARK;
				(void)strcpy(out_desc[i], format("%15s:(������)", jyoon_inven2_finger_name[i]));
				l = 13;
			}
			else
			{
				out_color[i] = TERM_L_DARK;
				(void)strcpy(out_desc[i], "(��)");
				l = 11;
			}
		}	
		else
		{
			/* Describe the object */
			object_desc(o_name, o_ptr, 0);

			/* Save the object index, color, and description */
			out_color[i] = tval_to_attr[o_ptr->tval % 128];

			/* Grey out charging items */
			if (o_ptr->timeout)
			{
				out_color[i] = TERM_L_DARK;
			}
			//�G���W�j�A�̓A�C�e���ƈꏏ�ɉ񐔂�\�����A�g�p�񐔐؂�̃A�C�e�����Â��\������
			if(mode == SI2_MODE_ENGINEER)
			{
				if(!o_ptr->pval) out_color[i] = TERM_L_DARK;
				else if(o_ptr->xtra4) out_color[i] = TERM_L_DARK;
				else  out_color[i] = TERM_WHITE;
				if(o_ptr->xtra4)
					sprintf(out_desc[i],"%s(�̏ᒆ)",o_name);
				else
					sprintf(out_desc[i],"%s(%d/%d)",o_name,o_ptr->pval,machine_table[o_ptr->sval].charge);
			}
			//�������[�h�@�����s�\�ȃA�C�e���͈Â��\��
			else if (mode == SI2_MODE_ACTIVATE)
			{
				if(!item_tester_hook_activate(o_ptr))
					out_color[i] = TERM_L_DARK;
				if(o_ptr->timeout)
					out_color[i] = TERM_L_DARK;

				(void)strcpy(out_desc[i], o_name);
			}
			else if(mode == SI2_MODE_ALICE)
			{
				sprintf(out_desc[i],"%15s:%s",alice_doll_name[i],o_name);
			}
			else if (mode == SI2_MODE_JYOON)
			{
				sprintf(out_desc[i], "%15s:%s", jyoon_inven2_finger_name[i], o_name);
			}

			else (void)strcpy(out_desc[i], o_name);

			/* Find the predicted "line length" */
			l = strlen(out_desc[i]) + 5;

			/* Be sure to account for the weight */
			if (show_weights) l += 9;
		}
	
		/* Maintain the maximum length */
		if (l > len) len = l;
	}

	/* Find the column to start in */
	col = (len > wid - 4) ? 0 : (wid - len - 1);
	/* Output each entry */
	for (i = 0; i < inven2_num; i++)
	{

		/* Get the item */
		o_ptr = &inven_add[i];

		/* Clear the line */
		prt("", i + 1, col ? col - 2 : col);


		/* Prepare an index --(-- */
		sprintf(tmp_val, "%c)", index_to_label(i));


		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, i + 1, col);

		cur_col = col + 3;

		/* Display graphics for object, if desired */
		if (show_item_graph && mode != SI2_MODE_ALICE)
		{
			byte  a = object_attr(o_ptr);
			char c = object_char(o_ptr);

			if(!o_ptr->k_idx)
			{
				a = TERM_DARK;
				c = ' ';
			}
			//�����͋󗓂̎��ǂ����������H

#ifdef AMIGA
			if (a & 0x80) a |= 0x40;
#endif

			Term_queue_bigchar(cur_col, i + 1, a, c, 0, 0);
			if (use_bigtile) cur_col++;

			cur_col += 2;
		}


		/* Display the entry itself */
		c_put_str(out_color[i], out_desc[i], i + 1, cur_col);

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt=0;
			if(o_ptr->k_idx) wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			(void)sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
#else
			(void)sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			prt(tmp_val, i + 1, wid - 9);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (i) prt("", i + 1, col ? col - 2 : col);

	/* Save the new column */
	command_gap = col;

	return ;
}


/*:::�ǉ��A�C�e�����̕\��*/
//v1.1.87 �A�C�e����I�����AI�R�}���h�Ɠ����m�F�����邱�Ƃɂ���
void display_list_inven2(void)
{
	int inven2_num = calc_inven2_num();
	int item;
	object_type *o_ptr;

	char c;
	char out_val[160];
	int i;


	screen_save();

	while (1)
	{

		item_tester_full = TRUE;
		if (p_ptr->pclass == CLASS_ALICE)
			show_inven2(SI2_MODE_ALICE);
		else if (p_ptr->pclass == CLASS_JYOON)
			show_inven2(SI2_MODE_JYOON);
		else
			show_inven2(0);
		item_tester_full = FALSE;

		prt("�ǂ���m�F���܂����H(ESC:�I��)", 0, 0);
		c = inkey();
		if (c == ESCAPE) break;

		item = c - 'a';
		if (item < 0 || item >= inven2_num) continue;

		o_ptr = &inven_add[item];

		if (!o_ptr->k_idx)
		{
			msg_format("�m�F����A�C�e�����Ȃ��B");
			continue;
		}
		if (!(o_ptr->ident & IDENT_MENTAL))
		{
			msg_print("���̃A�C�e���ɂ��ē��ɒm���Ă��邱�Ƃ͂Ȃ��B");

		}
		else
		{
			if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL)) msg_print("���ɕς�����Ƃ���͂Ȃ��悤���B");

		}



	}

	screen_load();


}


/*:::��t���򔠂ɓ������A�C�e����I�肷��@��A�f�ށA�L�m�R�̂�*/
static bool item_tester_inven2_chemist(object_type *o_ptr)
{
	if (o_ptr->tval == TV_POTION) return (TRUE);
	if (o_ptr->tval == TV_COMPOUND) return (TRUE);
	if (o_ptr->tval == TV_MATERIAL) return (TRUE);
	if (o_ptr->tval == TV_MUSHROOM) return (TRUE);

	return (FALSE);
}

//v1.1.87 �J�[�h���l���J�[�h�P�[�X�ɓ������A�C�e��
static bool item_tester_inven2_card_dealer(object_type *o_ptr)
{
	if (o_ptr->tval == TV_ITEMCARD) return (TRUE);
	if (o_ptr->tval == TV_ABILITY_CARD) return (TRUE);

	return (FALSE);
}

/*:::�A���X���l�`�Ɏ���������́@����Ə� �����@�s��*/
static bool item_tester_inven2_alice(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE]; //�؂ꖡ
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_NO_MAGIC)) return (FALSE);
	
	if (o_ptr->tval == TV_SHIELD) return (TRUE);
	if ((TV_KNIFE <= o_ptr->tval && o_ptr->tval <= TV_POLEARM)) return (TRUE);

	return (FALSE);
}

//�A���X�̐l�`�������\�ȕi �n�[�h�R�[�f�B���O����
bool check_alice_doll_can_wield(object_type *o_ptr, int doll_num)
{
	switch(doll_num)
	{
	case INVEN_DOLL_SHANHAI:
		if(o_ptr->weight > 50) return FALSE;
		if(o_ptr->tval == TV_KNIFE || o_ptr->tval == TV_SWORD) return TRUE;
		break;		
	case INVEN_DOLL_HORAI:
		if(o_ptr->weight > 70) return FALSE;
		if(o_ptr->tval == TV_KNIFE || o_ptr->tval == TV_SWORD || o_ptr->tval == TV_SPEAR) return TRUE;
		break;	
	case INVEN_DOLL_RUSSIA:
		if(o_ptr->weight > 120) return FALSE;
		if(o_ptr->tval == TV_HAMMER || o_ptr->tval == TV_AXE || o_ptr->tval == TV_SWORD) return TRUE;
		break;	
	case INVEN_DOLL_LONDON:
		if(o_ptr->weight > 160) return FALSE;
		if(o_ptr->tval == TV_SWORD || o_ptr->tval == TV_POLEARM || o_ptr->tval == TV_SPEAR || o_ptr->tval == TV_STICK) return TRUE;
		break;	
	case INVEN_DOLL_ORLEANS:
		if(o_ptr->weight > 180) return FALSE;
		if(o_ptr->tval == TV_SWORD || o_ptr->tval == TV_SHIELD || o_ptr->tval == TV_HAMMER) return TRUE;
		break;	
	case INVEN_DOLL_KYOTO:
		if(o_ptr->weight > 200) return FALSE;
		if(o_ptr->tval != TV_SHIELD) return TRUE;
		break;	
	case INVEN_DOLL_GOLIATH:
		if(o_ptr->weight < 200) return FALSE;
		if(o_ptr->tval != TV_SHIELD) return TRUE;
		//if(o_ptr->tval >= TV_KNIFE && o_ptr->tval <= TV_POLEARM) return TRUE;
		break;	

	default:
		msg_format("ERROR:check_alice_doll_can_wield()�ɖ���`��doll_num(%d)������ꂽ",doll_num);
		break;
	}

	return FALSE;
}



/*:::�ǉ��C���x���g���ɃA�C�e��������B�E�ɂ���ē������A�C�e���̎�ށA���A�A�C�e�����܂Ƃ߂邩�ǂ����ȂǏ������Ⴄ*/
/*:::�s���������Ƃ��߂�l��TRUE*/
bool put_item_into_inven2(void)
{
	int inven2_num = calc_inven2_num();
	int item, amt, slot2;
	cptr q, s;
	object_type *o_ptr;
	object_type forge,forge2;
	object_type *q_ptr = &forge;
	object_type *q2_ptr = &forge2;
	int pc = p_ptr->pclass;
	char o_name[MAX_NLEN];
	bool success = FALSE;
	bool changed = FALSE;
	int choose_num;

	if(!inven2_num)
	{
		msg_print("ERROR:���̐E�Ƃ̒ǉ��C���x���g��������`����Ă��Ȃ�");
		return FALSE;
	}

	switch(pc)
	{
	case CLASS_CHEMIST:
		item_tester_hook = item_tester_inven2_chemist;
		q = "�ǂ��򔠂ɓ���܂���? ";
		s = "�򔠂ɓ����ׂ����̂��Ȃ��B";
		break;
	case CLASS_ORIN:
		q = "�ǂ��L�Ԃɓ���܂���? ";
		s = "�L�Ԃɓ����ׂ����̂��Ȃ��B";
		break;
	case CLASS_ENGINEER:
		item_tester_tval = TV_MACHINE;
		q = "�ǂ̋@�B���o�b�N�p�b�N�Ɋi�[���܂���? ";
		s = "�@�B�������Ă��Ȃ��B";
		break;
	case CLASS_NITORI:
		item_tester_tval = TV_MACHINE;
		q = "�ǂ̋@�B�������b�N�ɓ���܂���? ";
		s = "�@�B�������Ă��Ȃ��B";
		break;
	case CLASS_SH_DEALER:
		q = "�ǂ�𓹋�ɓ���܂���? ";
		s = "����ɓ����ׂ����̂��Ȃ��B";
		break;
	case CLASS_ALICE:
		item_tester_hook = item_tester_inven2_alice;
		q = "�ǂ��l�`�Ɏ������܂���? ";
		s = "�l�`�Ɏ���������̂��Ȃ��B";
		break;
	case CLASS_SAKUYA:
		item_tester_tval = TV_KNIFE;
		q = "�ǂ̒Z�����i�C�t�z���_�[�ɍ����܂����H ";
		s = "�ڂڂ����Z������������Ȃ��B";
		break;
	case CLASS_UDONGE:
		q = "�ǂ����Ăɓ���܂���? ";
		s = "���Ăɓ����ׂ����̂��Ȃ��B";
		break;
	case CLASS_JYOON:
		item_tester_tval = TV_RING;
		q = "�ǂ̎w�ւ����܂����H ";
		s = "�w�ւ������Ă��Ȃ��B";
		break;
	case CLASS_TAKANE:
	case CLASS_SANNYO:
	case CLASS_MIKE:
	case CLASS_CARD_DEALER:
		item_tester_hook = item_tester_inven2_card_dealer;

		q = "�ǂ̃J�[�h���P�[�X�ɓ���܂����H ";
		s = "�A�r���e�B�J�[�h�������Ă��Ȃ��B";
		break;

	default:
		msg_print("ERROR:���̐E�Ƃ̒ǉ��C���x���g���ΏۃA�C�e������`����Ă��Ȃ�");
		return FALSE;
	}

	//���̂ݑ������̕i���w��ł���
	if(pc == CLASS_SAKUYA)
	{
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR | USE_EQUIP))) return FALSE;
	}
	else
	{
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;
	}

	if (item >= 0)	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];


	
	amt = 1;
	/*:::���w��@�����Ŏw�肳��Ă��Ȃ��A���X��G���W�j�A�A���Ȃǂ͏�Ɉ����*/
	if (o_ptr->number > 1 && ( pc == CLASS_CHEMIST || pc == CLASS_ORIN || pc == CLASS_SH_DEALER || pc == CLASS_UDONGE || pc == CLASS_MIKE || pc == CLASS_TAKANE || pc == CLASS_SANNYO || pc == CLASS_CARD_DEALER) )
	{
		/*:::���ʂ���́B�A�C�e�����ȏ�ɂ͂Ȃ�Ȃ��悤���������B*/
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return FALSE;
	}

	//(void)show_inven2();

	//�����͈����̎w�ւ���ꂽ�w�ւ𑕔����Ȃ�
	if (pc == CLASS_JYOON)
	{
		if (object_is_cursed(o_ptr))
		{
			msg_print("���Ȃ��͂���ȃP�`�̂������̂𑕔��������Ȃ��B");
			return FALSE;
		}
		/* �A�C�e�����i�����{�c
		if(object_is_cheap_to_jyoon(o_ptr))
		{
			msg_print("���Ȃ��͂���Ȉ����𑕔��������Ȃ��B");
			return FALSE;
		}
		*/

	}
	//v1.1.53 ���̑����i�X���b�g������ꂽ�����i���ړ��ł��Ȃ��悤�C��
	else if (p_ptr->pclass == CLASS_SAKUYA)
	{
		if (item >= INVEN_RARM && object_is_cursed(o_ptr))
		{
			msg_print("�i�C�t������Ă��Ď肩�痣��Ȃ��B");
			return FALSE;
		}


	}

	/*:::�I���A�C�e���̎��̊m��*/
	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;


	/*:::�����I�ɂ܂Ƃ߂��邩���肵�A�C�e���������I�ɋ󂢂Ă�ǉ��C���x���g���ɓ����E�Ɓi��t�A���Ӂj*/
	/*:::�G���W�j�A��1�X���b�g1��������Ȃ����ǂ����@�B�͂܂Ƃ܂�Ȃ��̂ł��̃��[�`���̂܂܂Ŗ��Ȃ��͂�*/
	if( pc == CLASS_CHEMIST || pc == CLASS_ORIN || pc == CLASS_ENGINEER || pc == CLASS_NITORI || pc == CLASS_SH_DEALER || pc == CLASS_UDONGE || pc == CLASS_MIKE || pc == CLASS_TAKANE || pc == CLASS_SANNYO || pc == CLASS_CARD_DEALER)
	{
		int freespace = 99;
		/*:::�܂Ƃ߂��邩����*/
		for(slot2 = 0;slot2 < inven2_num;slot2++)
		{
			object_type *j_ptr;
			j_ptr = &inven_add[slot2];
			/*:::�󂫃X���b�g������΂�����o�^���Ă���*/
			if(!j_ptr->k_idx)
			{
				if(freespace == 99) freespace = slot2;
				 continue;
			}
			//���n���A�C�e�����A�����v99�ȓ����A�܂Ƃ߂���A�C�e�����A�𔻒肵�Ă���
			if(object_similar(j_ptr,q_ptr))
			{
				/*:::���b�h�����@�_�𕪂����Ƃ�pval�ɋL�^����Ă���[�U�󋵂𒲐�����*/
				distribute_charges(o_ptr, q_ptr, amt);

				/*:::�����̃A�C�e���Ƃ܂Ƃ߂���ꍇ�͂܂Ƃߏ��������A���[�v�I��*/
				object_absorb(j_ptr,q_ptr);
				success = TRUE;
				break;

			}
			
		}
		/*:::�܂Ƃ߂��Ȃ��������󂫃X���b�g������Ƃ������ɓ����*/
		if(!success && freespace != 99)
		{
			/*:::���b�h�����@�_�𕪂����Ƃ�pval�ɋL�^����Ă���[�U�󋵂𒲐�����*/
			distribute_charges(o_ptr, q_ptr, amt);

			object_wipe(&inven_add[freespace]);
			object_copy(&inven_add[freespace], q_ptr);
			success = TRUE;
		}
	}
	//�A���X�̓X���b�g���w�肷��K�v������̂ŕʏ���
	else if(pc == CLASS_ALICE)
	{
		int wid,hgt;
		int i;
		char o_name_2[MAX_NLEN];
		char tmp_val[7][160];
		int len = 0, col;
		char c;
		
		screen_save();
		msg_print("�ǂ̐l�`�Ɏ������܂����H(�L�����Z��:ESC)");
		Term_get_size(&wid, &hgt);
		for(i=0;i<inven2_num;i++)
		{
			int tmp_len;

			if(inven_add[i].k_idx)
			{
				object_desc(o_name_2, &inven_add[i], 0);
				sprintf(tmp_val[i],"%c) %15s: %s",'a'+i,alice_doll_name[i],o_name_2);
			}
			else
			{
				sprintf(tmp_val[i],"%c) %15s : (������)",'a'+i,alice_doll_name[i]);
			}
			tmp_len = strlen(tmp_val[i]);
			if(len < tmp_len) len = tmp_len;
		}

		col = (len > wid - 4) ? 0 : (wid - len - 1);
		for(i=0;i<inven2_num;i++)
		{
			prt("", i + 1, col ? col - 2 : col);

			//�����s�̐l�`��e��
			if(!check_alice_doll_can_wield(o_ptr,i)) continue;
			c_put_str(TERM_WHITE, tmp_val[i], i + 1, col);

		}
		while(1)
		{
			c = inkey();
			if(c >= 'a' && c < ('a' + inven2_num) ) 
			{
				choose_num = c - 'a';
				//�����\�Ȑl�`�̎��̂݃��[�v�E�o
				if(check_alice_doll_can_wield(o_ptr, choose_num)) 
					break;	
			}
			else if(c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
		}
		screen_load();

		//�������ɑ������̐l�`��I�������猳�̑����i�����o������

		if(inven_add[choose_num].k_idx)
		{
			changed = TRUE;
			success = TRUE;
			object_copy(q2_ptr, &inven_add[choose_num]);
			object_wipe(&inven_add[choose_num]);
			object_copy(&inven_add[choose_num], q_ptr);

		}
		else
		{
			object_wipe(&inven_add[choose_num]);
			object_copy(&inven_add[choose_num], q_ptr);
			success = TRUE;
		}
	}
	//�����ǉ��@�X���b�g����Ɏw�肷��
	//v1.1.46 �������ǉ�
	else if(pc == CLASS_SAKUYA || pc == CLASS_JYOON)
	{
		int wid,hgt;
		int i;
		char o_name_2[MAX_NLEN];
		char tmp_val[8][160]; //����7�ɂ��Ă������ŕ\�����������Ȃ��Ă�
		int len = 0, col;
		char c;

		screen_save();
		if(pc == CLASS_SAKUYA)
			msg_print("�ǂ��ɍ����܂����H(�L�����Z��:ESC)");
		else
			msg_print("�ǂ��ɂ��܂����H(�L�����Z��:ESC)");

		Term_get_size(&wid, &hgt);
		for (i = 0; i < inven2_num; i++)
		{
			int tmp_len;

			if (pc == CLASS_SAKUYA)
			{
				if (inven_add[i].k_idx)
				{
					object_desc(o_name_2, &inven_add[i], 0);
					sprintf(tmp_val[i], "%c)  %s", 'a' + i, o_name_2);
				}
				else
				{
					sprintf(tmp_val[i], "%c) (������)", 'a' + i);
				}
			}
			else //����
			{
				if (inven_add[i].k_idx)
				{
					object_desc(o_name_2, &inven_add[i], 0);
					sprintf(tmp_val[i], "%c) %15s: %s", 'a' + i, jyoon_inven2_finger_name[i], o_name_2);
				}
				else
				{
					sprintf(tmp_val[i], "%c) %15s : (������)", 'a' + i, jyoon_inven2_finger_name[i]);
				}
			}

			tmp_len = strlen(tmp_val[i]);
			if(len < tmp_len) len = tmp_len;
		}


		col = (len > wid - 4) ? 0 : (wid - len - 1);
		for(i=0;i<inven2_num;i++)
		{
			prt("", i + 1, col ? col - 2 : col);
			c_put_str(TERM_WHITE, tmp_val[i], i + 1, col);
		}

		while(1)
		{
			c = inkey();
			if(c >= 'a' && c < ('a' + inven2_num) ) 
			{
				choose_num = c - 'a';
				break;	
			}
			else if(c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
		}
		screen_load();


		//v1.1.46 �����͑����ɉ��i����������̂œ�x�Ƒ����ł��Ȃ����̂��O���Ƃ��͊m�F����悤�ɂ��Ă���
		/* ...���肾����������σQ�[���̃e���|�������Ȃ肻���Ȃ̂Ń{�c
		if (p_ptr->pclass == CLASS_JYOON)
		{
			if (inven_add[choose_num].k_idx && object_is_cheap_to_jyoon(&inven_add[choose_num]))
			{
				msg_print("���̂悤�Ȉ����͓�x�Ɛg�ɂ��邱�Ƃ͂Ȃ����낤�B");
				if (!get_check_strict("�{���Ɏw�ւ�t���ւ��܂����H", CHECK_OKAY_CANCEL))
				{
					return FALSE;
				}
			}
		}
		*/

		if(inven_add[choose_num].k_idx)
		{
			changed = TRUE;
			success = TRUE;
			object_copy(q2_ptr, &inven_add[choose_num]);
			object_wipe(&inven_add[choose_num]);
			object_copy(&inven_add[choose_num], q_ptr);
		}
		else
		{
			object_wipe(&inven_add[choose_num]);
			object_copy(&inven_add[choose_num], q_ptr);
			success = TRUE;
		}
	}
	else
	{
		msg_format("ERROR:���̐E�Ƃ̒ǉ��C���x���g���X���b�g�I�菈�����L�q����Ă��Ȃ�");
		return FALSE;
	}
	

	object_desc(o_name, q_ptr, 0);

	/*:::�ǉ��C���x���g���ɃA�C�e������ꂽ�ꍇ���A�C�e���̐������炷*/
	if(success)
	{
		/*:::�ǉ��C���x���g���̏d�ʑ��� �G���W�j�A�͒ǉ��C���x���g���̏d�ʂ𖳎�����i�o�b�N�p�b�N�������Ă邩��j*/
		///mod150805 �A���X�ǉ�
		if(p_ptr->pclass != CLASS_ALICE && p_ptr->pclass != CLASS_ENGINEER 
			&& p_ptr->pclass != CLASS_NITORI ) p_ptr->total_weight += q_ptr->weight * q_ptr->number;

		/*:::�U�b�N�̏d�ʌ�����inven_item_increase()���ŏ�������Ă���*/
		if(item >= 0)
		{
			inven_item_increase(item, -amt);
		//	inven_item_describe(item);
			inven_item_optimize(item);
		}
		else
		{
			floor_item_increase(0 - item, -amt);
		//	floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}	

		if(pc == CLASS_CHEMIST) msg_format("%s��򔠂ɓ��ꂽ�B",o_name);
		else if(pc == CLASS_ORIN) msg_format("%s��L�Ԃɓ��ꂽ�B",o_name);
		else if(pc == CLASS_SH_DEALER) msg_format("%s�𓹋�ɓ��ꂽ�B",o_name);
		else if(pc == CLASS_ENGINEER) msg_format("%s���o�b�N�p�b�N�ɑ��������B",o_name);
		else if(pc == CLASS_NITORI) msg_format("%s�������b�N�ɓ��ꂽ�B",o_name);
		else if(pc == CLASS_ALICE) msg_format("%s��%s�Ɏ��������B",o_name,alice_doll_name[choose_num]);
		else if(pc == CLASS_SAKUYA) msg_format("%s���i�C�t�z���_�[�ɍ������B",o_name);
		else if (pc == CLASS_UDONGE) msg_format("%s����Ăɓ��ꂽ�B", o_name);
		else if (pc == CLASS_JYOON) msg_format("%s�𑕔������B", o_name);
		else if (pc == CLASS_TAKANE) msg_format("%s���P�[�X�Ɏ��[�����B", o_name);
		else if (pc == CLASS_MIKE) msg_format("%s���P�[�X�Ɏ��[�����B", o_name);
		else if (pc == CLASS_SANNYO) msg_format("%s���P�[�X�Ɏ��[�����B", o_name);
		else if (pc == CLASS_CARD_DEALER) msg_format("%s���P�[�X�Ɏ��[�����B", o_name);
		else msg_format("ERROR:�ǉ��C���x���g���ɃA�C�e�����ꂽ�Ƃ��̃��b�Z�[�W���Ȃ�");

//inven_cnt��equip_cnt�͋C�ɂ��Ȃ��Ă����񂾂낤���H�A�C�e�����E������U�b�N���܂Ƃ߂�ӂ�ŉ����ςȂ��ƂɂȂ邩���B�Z�[�u�����[�h�Œ���悤�����B

		if(changed)
		{
			//Hack - ��������inven_item_optimize()�ŋl�߂��Ȃ��̂�item�l�����̂܂܎g���T�u�C���x���g���ɂ������A�C�e���ŏ㏑�����Ă����Ȃ��͂�
			if(item > INVEN_PACK)
			{
				object_desc(o_name, q2_ptr, 0);
				msg_format("%s�����ɍ\�����B",o_name);
				object_wipe(&inventory[item]);
				object_copy(&inventory[item], q2_ptr);
			}
			else
			{
				inven_carry(q2_ptr);
			}
		}
		///mod160113 �����i���O���Ēǉ��C���x���g���ɓ��ꉽ���o���Ȃ������ꍇ�O�����������̑�����Ԃ��Čv�Z�@���̂Ƃ�����̃i�C�t���[�̂�
		else if(item > INVEN_PACK)
		{
			kamaenaoshi(item);
		}

		//�����ύX�̂Ƃ��Čv�Z
		if(item > INVEN_PACK)
		{
			//kamaenaoshi�����畐�킪�������B�Ȃ��H
			//��kamaenaoshi()�͑������O�������ɌĂ΂�郋�[�`��������
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= (PR_EQUIPPY);
		}
	
		return TRUE;
	}
	else //�ǉ��C���x���g������t�������Ƃ��̃��b�Z�[�W�@����A���X�Ȃǂ͎w�肵���X���b�g�ɕ��������������ւ���̂ł����ɂ͗��Ȃ��͂�
	{
		if(pc == CLASS_CHEMIST) msg_format("�򔠂ɂ�%s������ꏊ���Ȃ��B",o_name);
		else if(pc == CLASS_ORIN) msg_format("�L�Ԃɂ�%s�����錄�Ԃ��Ȃ��B",o_name);
		else if(pc == CLASS_SH_DEALER) msg_format("����ɂ�%s�����錄�Ԃ��Ȃ��B",o_name);
		else if(pc == CLASS_ENGINEER) msg_format("�o�b�N�p�b�N��%s���i�[����X�y�[�X���Ȃ��B",o_name);
		else if(pc == CLASS_NITORI) msg_format("���������b�N�ɂ͋@�B������Ȃ��B");
		else if (pc == CLASS_UDONGE) msg_format("���Ăɂ�%s�����錄�Ԃ��Ȃ��B", o_name);
		else if (pc == CLASS_TAKANE) msg_format("�P�[�X�ɂ�%s�����錄�Ԃ��Ȃ��B", o_name);
		else if (pc == CLASS_MIKE) msg_format("�P�[�X�ɂ�%s�����錄�Ԃ��Ȃ��B", o_name);
		else if (pc == CLASS_SANNYO) msg_format("�P�[�X�ɂ�%s�����錄�Ԃ��Ȃ��B", o_name);
		else if (pc == CLASS_CARD_DEALER) msg_format("�P�[�X�ɂ�%s�����錄�Ԃ��Ȃ��B", o_name);
		else msg_format("ERROR:�ǉ��C���x���g���ɃA�C�e�������ꏊ���Ȃ��Ƃ��̃��b�Z�[�W���Ȃ�");

		return FALSE;
	}

}


/*:::�ǉ��C���x���g������A�C�e�������o��*/
bool takeout_inven2(void)
{
	int inven2_num = calc_inven2_num();
	int item, amt;
	cptr q, s;
	char c;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int pc = p_ptr->pclass;
	char o_name[MAX_NLEN];

	if(!inven2_num)
	{
		msg_print("ERROR:���̐E�Ƃ̒ǉ��C���x���g��������`����Ă��Ȃ�");
		return FALSE;
	}
	screen_save();

	if( pc == CLASS_ENGINEER || pc == CLASS_NITORI) (void)show_inven2(SI2_MODE_ENGINEER);
	else if( pc == CLASS_ALICE) (void)show_inven2(SI2_MODE_ALICE);
	else if (pc == CLASS_JYOON) (void)show_inven2(SI2_MODE_JYOON);
	else (void)show_inven2(0);

	if( pc == CLASS_ENGINEER || pc == CLASS_NITORI) prt("�ǂ̋@�B���O���܂����H", 0, 0);
	else if( pc == CLASS_ALICE ) prt("�ǂ̑������O���܂����H", 0, 0);
	else if( pc == CLASS_SAKUYA ) prt("�ǂ̕�������܂����H", 0, 0);
	else if (pc == CLASS_JYOON) prt("�ǂ̎w�ւ��O���܂����H", 0, 0);
	else  prt("�ǂ̃A�C�e�������o���܂����H", 0, 0);

	while(1)
	{
		c = inkey();
		if(c >= 'a' && c < ('a' + inven2_num) ) break;
		if(c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
	}
	item = c-'a';
	o_ptr = &inven_add[ item ];

	if(!o_ptr->k_idx)
	{
		if(pc == CLASS_ALICE)
			msg_format("%s�͉��������Ă��Ȃ��B",alice_doll_name[item]);
		else if(pc == CLASS_JYOON)
			msg_format("�����ɂ͉������Ă��Ȃ��B");
		else
			msg_format("�����ɂ͉��������Ă��Ȃ��B");

		screen_load();
		return FALSE;
	}

	//v1.1.46 �����͑����ɉ��i����������̂œ�x�Ƒ����ł��Ȃ����̂��O���Ƃ��͊m�F����悤�ɂ��Ă���
	/* ���{�c
	if (p_ptr->pclass == CLASS_JYOON)
	{
		if (object_is_cheap_to_jyoon(o_ptr))
		{
			msg_print("���̂悤�Ȉ����͓�x�Ɛg�ɂ��邱�Ƃ͂Ȃ����낤�B");
			if (!get_check_strict("�{���Ɏw�ւ��O���܂����H", CHECK_OKAY_CANCEL))
			{
				screen_load();
				return FALSE;
			}
		}
	}
	*/

	amt = 1;
	if(o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0)
		{
			screen_load();
			return FALSE;
		}
	}

	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;
	distribute_charges(o_ptr, q_ptr, amt);
	object_desc(o_name, q_ptr, 0);

	/*:::�ǉ��C���x���g���̐������������A0�ɂȂ����珉��������*/
	/*:::inven_item_increase()�Ǝ�������*/

	//v1.1.73 �A���X�ǉ�
	//�d�ʏ����@�G���W�j�A�A�ɂƂ�A�A���X�͒ǉ��C���x���g���̏d�ʂ𖳎�����̂ł�������󂯎�������͏d�ʑ���
	//(�����ŃA�C�e���������ς��������ꍇprocess_player()��pack_overflow()���ŃA�C�e�����ӂꏈ�����s����̂ł��̎��_�ł͂��̂܂ܓn���Ă悢
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI && p_ptr->pclass != CLASS_ALICE)
		p_ptr->total_weight -= q_ptr->weight * amt; 

	o_ptr->number -= amt;
	if(o_ptr->number <1) object_wipe(o_ptr);

	//if(p_ptr->total_weight < 0) msg_format("ERROR:�ǉ��C���x���g������A�C�e�����o�����Ƃ�total_weight�����ɂȂ���");

	if( pc == CLASS_CHEMIST) msg_format("�򔠂���%s�����o�����B",o_name);
	else if(pc == CLASS_ORIN) msg_format("%s��L�Ԃ���o�����B",o_name);
	else if(pc == CLASS_SH_DEALER) msg_format("�������%s���o�����B",o_name);
	else if(pc == CLASS_ENGINEER) msg_format("�o�b�N�p�b�N����%s���O�����B",o_name);
	else if(pc == CLASS_NITORI) msg_format("�����b�N����%s���o�����B",o_name);
	else if(pc == CLASS_ALICE) msg_format("%s����%s���󂯎�����B",alice_doll_name[item],o_name);
	else if(pc == CLASS_SAKUYA) msg_format("�i�C�t�z���_�[����%s�𔲂�������B",o_name);
	else if (pc == CLASS_UDONGE) msg_format("���Ă���%s���o�����B", o_name);
	else if (pc == CLASS_JYOON) msg_format("%s���w����O�����B", o_name);
	else if (pc == CLASS_TAKANE) msg_format("%s���P�[�X����o�����B", o_name);
	else if (pc == CLASS_MIKE) msg_format("%s���P�[�X����o�����B", o_name);
	else if (pc == CLASS_SANNYO) msg_format("%s���P�[�X����o�����B", o_name);
	else if (pc == CLASS_CARD_DEALER) msg_format("%s���P�[�X����o�����B", o_name);
	else msg_format("ERROR:�ǉ��C���x���g������A�C�e���o�����Ƃ��̃��b�Z�[�W���Ȃ�");
	
	(void)inven_carry(q_ptr);

	screen_load();
	return TRUE;

}



//v1.1.46 inven_add[]���瑕���i�𔭓����鏈���B���̂Ƃ��돗���̎w�֐�p�B
//�s���������Ƃ�TRUE��Ԃ��B�Ȃ������R�}���h�̏ꍇ����������Ƀ^�[�Q�b�g�I���ȂǂŃL�����Z�����Ă��s���������B
bool	activate_inven2(void)
{
	int inven2_num = calc_inven2_num();
	int item, amt;
	cptr q, s;
	char c;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int pc = p_ptr->pclass;
	char o_name[MAX_NLEN];

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)
	{
		msg_print("���̎p�ł̓A�C�e���̔������ł��Ȃ��B");
		return FALSE;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	if (!inven2_num)
	{
		msg_print("ERROR:���̐E�Ƃ̒ǉ��C���x���g��������`����Ă��Ȃ�");
		return FALSE;
	}
	screen_save();

	(void)show_inven2(SI2_MODE_ACTIVATE);

	prt("�ǂ̃A�C�e�����n�������܂����H", 0, 0);

	while (1)
	{
		c = inkey();
		if (c >= 'a' && c < ('a' + inven2_num)) break;
		if (c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
	}
	item = c - 'a';
	o_ptr = &inven_add[item];
	screen_load();

	if (!o_ptr->k_idx)
	{
		if (pc == CLASS_JYOON)
			msg_format("�����ɂ͉������Ă��Ȃ��B");
		else
			msg_format("�����ɂ͉��������Ă��Ȃ��B");

		return FALSE;
	}

	if (!item_tester_hook_activate(o_ptr))
	{
		msg_format("����͎n���ł���A�C�e���ł͂Ȃ��B");

		return FALSE;
	}

	/*::: -Hack- 
	* do_cmd_activate_aux()�ɕ���item�l��n����inven_add[]����A�C�e�������擾���邱�Ƃɂ���B
	* ���̃R�}���h�̏ꍇitem�������Ə���A�C�e���ɑ΂��ď������s�����A�����R�}���h�̏ꍇ�������̕i�����I���ł��Ȃ��̂ŏ���A�C�e�����C�ɂ���K�v���Ȃ��B
	* ������item=0�̂Ƃ�inventory[]��0��ǂݍ��܂Ȃ��悤�����1���Z����
	*/
	do_cmd_activate_aux( -1 -item);

	return TRUE;

}


/*:::�t�r�_�g���̓��Z�u�A�Җ��߁v���g�������̏��� ����t���O�������Ă��鑶�݂�1/3�ŃU�b�N�ɋA�҂���*/
void tsukumo_recall(void)
{
	int i;

	if(p_ptr->pclass != CLASS_TSUKUMO_MASTER) return;

	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		object_type *q_ptr;
		char m_name[80];
		if (m_ptr->r_idx < MON_TSUKUMO_WEAPON1 || m_ptr->r_idx > MON_TSUKUMO_WEAPON5 ) continue;
		if (!(m_ptr->mflag & MFLAG_SPECIAL)) continue;
		if(!one_in_(3)) continue;
		
		q_ptr = &o_list[m_ptr->hold_o_idx];

		if(!q_ptr->k_idx) msg_format("ERROR:�A�Җ��߂̏o���ꂽ�t�r�_IDX:%d���A�C�e���������Ă��Ȃ�",m_ptr->r_idx);

		object_desc(m_name,q_ptr,OD_NAME_ONLY);
		msg_format("%s���A�҂����I",m_name);
		disturb(1,1);

		q_ptr->held_m_idx = 0;
		q_ptr->marked = OM_TOUCHED;
		m_ptr->hold_o_idx = q_ptr->next_o_idx;
		q_ptr->next_o_idx = 0;

		if(inven_carry_okay(q_ptr))
		{
			inven_carry(q_ptr);
		}
		else
		{
			object_type forge;
			object_type *o_ptr = &forge;
			object_copy(o_ptr, q_ptr);
			msg_format("�������U�b�N�ɓ��炸�n�ʂɗ������I",m_name);
			(void)drop_near(o_ptr, -1, py, px);
		}
		delete_monster_idx(i);
	}

}

/*:::�t�r�_�g���̎g���Ă��镐�킪�d�퉻���Ĕ\�͂𓾂鏈��*/
/*:::�g�p���Ă�������A���̊֐����Ă΂ꂽ���U�����Ă����G�̎푰������*/
/*:::o_ptr->xtra1��50��100�̂Ƃ��Ă΂��*/
///mod151219 ���ےǉ�
void make_evilweapon(object_type *o_ptr, monster_race *r_ptr)
{
	int attempt = 10;
	u32b flgs[TR_FLAG_SIZE];
	int new_flag = -1;
	bool make_1st = FALSE;
	char o_name[MAX_NLEN];

	/*:::Paranoiacs*/
	if(object_is_fixed_artifact(o_ptr)) return; 
	if(!object_is_melee_weapon(o_ptr)) return;
	if(p_ptr->pclass != CLASS_TSUKUMO_MASTER && p_ptr->pclass != CLASS_RAIKO) msg_print("ERROR:�t�r�_�g���ȊO��make_evliweapon()���Ă΂�Ă���");

	if(o_ptr->xtra1 == 50) make_1st = TRUE;
	object_flags(o_ptr, flgs);
	object_desc(o_name, o_ptr, OD_NAME_ONLY);
	o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER; //�b��A�C�e�������ɂ���

	for(attempt=10;attempt;attempt--)
	{
		int select = randint1(7);

		//����̗��ۂ�|�d�����t���Ȃ�
		if(p_ptr->pclass == CLASS_RAIKO && make_1st)
		{
			new_flag = TR_BRAND_ELEC;
			break;
		}

		switch(select)
		{
		case 1:	case 2: //�U�����Ă����G�ɉ����ăX���C��t�^����
//msg_print("chk:Enter:newSlay");
			{
				int cnt=0;


				if(r_ptr->flags3 & RF3_KWAI)
				{
					cnt++;
					if(have_flag(flgs,TR_SLAY_KWAI)) new_flag = TR_KILL_KWAI;
					else new_flag = TR_SLAY_KWAI;
				}
				if(r_ptr->flags3 & RF3_DEITY)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_DEITY)) new_flag = TR_KILL_DEITY;
						else new_flag = TR_SLAY_DEITY;
					}
				}
				if(r_ptr->flags3 & RF3_DRAGON)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_DRAGON)) new_flag = TR_KILL_DRAGON;
						else new_flag = TR_SLAY_DRAGON;
					}
				}
				if(r_ptr->flags3 & RF3_DEMON)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_DEMON)) new_flag = TR_KILL_DEMON;
						else new_flag = TR_SLAY_DEMON;
					}
				}
				if(r_ptr->flags3 & RF3_UNDEAD)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_UNDEAD)) new_flag = TR_KILL_UNDEAD;
						else new_flag = TR_SLAY_UNDEAD;
					}
				}
				if(r_ptr->flags3 & RF3_ANIMAL)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_ANIMAL)) new_flag = TR_KILL_ANIMAL;
						else new_flag = TR_SLAY_ANIMAL;
					}
				}
				if(r_ptr->flags3 & RF3_HUMAN)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_HUMAN)) new_flag = TR_KILL_HUMAN;
						else new_flag = TR_SLAY_HUMAN;
					}
				}
				if((r_ptr->flags3 & RF3_ANG_CHAOS) && one_in_(2))
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_EVIL)) new_flag = TR_KILL_EVIL;
						else new_flag = TR_SLAY_EVIL;
					}
				}
				if((r_ptr->flags3 & RF3_ANG_COSMOS) && one_in_(2))
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_GOOD)) new_flag = TR_KILL_GOOD;
						else new_flag = TR_SLAY_GOOD;
					}
				}
			}
			break;

		case 3: //�����_����pval��t�^���� ������\�͕͂t�^����Ȃ�
//msg_print("chk:Enter:newPval");
			{

				while(1)
				{
					new_flag = TR_STR + randint0(13);
					if(new_flag != TR_MAGIC_MASTERY) break;
				}
			}
			break;

		case 4: case 5: //�������U���\�͂�t�^����
//msg_print("chk:Enter:newBrand");
			{
				while(1)
				{
					new_flag = TR_CHAOTIC + randint0(11);
					if(!object_has_a_blade(o_ptr) && (new_flag == TR_EX_VORPAL || new_flag == TR_VORPAL) ) continue;
					if(object_has_a_blade(o_ptr) && (new_flag == TR_IMPACT) ) continue;
					if(new_flag == TR_VORPAL && have_flag(flgs,TR_VORPAL)) new_flag = TR_EX_VORPAL;
					else if(new_flag == TR_EX_VORPAL && !have_flag(flgs,TR_VORPAL)) new_flag = TR_VORPAL;

					break;
				}
			}
			break;

		case 6://���̑��̔\�͂������_���ɕt�^����
//msg_print("chk:Enter:newSkill");
			{
				int chance = randint1(32);
				if(chance < 18) new_flag = TR_ESP_EVIL + randint0(17);
				else if(chance < 26) new_flag = TR_LEVITATION + randint0(8);
				else new_flag = TR_SPEEDSTER + randint0(7);
			}
			break;

		case 7://�ϐ��������_���ɕt�^���� �Ɖu�͕t���ɂ���
//msg_print("chk:Enter:newRes");
			{
				while(1)
				{
					new_flag = TR_IM_ACID + randint0(24);
					if(new_flag >= TR_IM_ACID && new_flag <= TR_IM_COLD && !weird_luck()) continue;
					break;
				}
			}
			break;
		default:
			msg_format("ERROR:make_evilweapon��select�l(%d)����������",select);

		}
		//���Ɏ����Ă���\�͂Ɍ��܂������蒼��
		if(new_flag >= 0 && !have_flag(flgs,new_flag)) break;
		else new_flag = -1;
	}

	if(!attempt) //10�񎎍s���Ĕ\�͂����܂�Ȃ������Ƃ�����Ƀ_�C�X�{�[�i�X
	{
		o_ptr->dd += 3;
	}
	else
	{
		//�I�����ꂽ�\�͂�t�^
		add_flag(o_ptr->art_flags, new_flag);

		//�ǉ��U���Ȃǂ�pval����
		if(new_flag == TR_BLOWS)
		{
			if(make_1st) o_ptr->pval = 1;
			else o_ptr->pval = 2;
		}
		else if(is_pval_flag(new_flag))
		{
			if(have_flag(flgs,TR_BLOWS))
			{
				if(o_ptr->pval > 1) ; //�ǉ��U���G�S�ȂǊ���2�ȏ��pval�̂Ƃ���pval��G��Ȃ�
				else if(make_1st) o_ptr->pval = 1;
				else o_ptr->pval = 2;
			}
			else
			{
				int tmp = 1 + randint1(3);
				o_ptr->pval = MAX(o_ptr->pval, tmp);
			}
		}
		//��x�ڂŒǉ��U�����t���ē�x�ڂ�pval�֌W�Ȃ��̂��t��������pval2�ɂȂ�B�ǉ��U���␹��҃G�S��+1�U�̂Ƃ���+2�ɂȂ��Ă��܂����܂��������B
		else if(have_flag(flgs,TR_BLOWS) && !make_1st && o_ptr->pval == 1)
		{
			o_ptr->pval = 2;
		}
	}
	//���ۏ����ǉ��@�t�r�_�g���قǂɂ͋����Ȃ�Ȃ�
	if(p_ptr->pclass == CLASS_RAIKO)
	{
		if(make_1st)
		{
			o_ptr->to_h += randint1(3);
			o_ptr->to_d += randint1(3);
			msg_format("%s�̖��͂��g�����Ȃ���悤�ɂȂ��Ă����C������B",o_name);
		}
		else
		{
			o_ptr->dd += 1;
			o_ptr->to_h += randint1(5);
			o_ptr->to_d += randint1(5);
			msg_format("%s�̖��͂����S�Ɏg�����Ȃ���悤�ɂȂ����I",o_name);
		}
	}
	else
	{
		if(make_1st)
		{
			o_ptr->to_h += randint1(5);
			o_ptr->to_d += randint1(5);
			msg_format("�����X�^�[�̌����z��������%s�͗d�퉻�����I",o_name);
		}
		else
		{
			o_ptr->dd += 2;
			o_ptr->to_h += randint1(5);
			o_ptr->to_d += randint1(5);
			msg_format("%s�͍X�Ȃ�͂𓾂��I",o_name);
		}
	}

	p_ptr->window |= (PW_INVEN | PW_EQUIP);
	p_ptr->update |= (PU_BONUS);
	calc_android_exp();

}

/*:::��Փx�����`�F�b�N�@FALSE�̂Ƃ����̃����X�^�[�𔭐��}�~����Bcheck_rest��FALSE��RESTRICT������Ă̂����͂����������C�����邪�B
 *FLAG_50F�t���O�̐ݒ肳�ꂽ���G��EASY�ł͏�ɗ}�~����NORMAL�ł͎���|���܂ŗ}�~�����
 */
bool check_rest_f50(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if(!(r_ptr->flags1 & RF1_FLAG_50F)) return TRUE;

	if(difficulty == DIFFICULTY_EASY)
		return FALSE;
	else if(difficulty == DIFFICULTY_NORMAL)
	{
		if(quest[QUEST_YUKARI].status == QUEST_STATUS_FINISHED) return TRUE;
		else return FALSE;
	}
	else return TRUE;

}


/*:::�t���A�̃A�C�e���𑖍����A���[�h�ɍ��v����ԋ߂����̂̋������܂��ɒm�点��B*/
/*:::mode:1.�����i 2.�A�[�e�B�t�@�N�g 3.�f�� 4.���S�Ăƒ��i 5.���ꂽ�A�C�e�� 6.�f�ނƃL�m�R*/
//7:�����������\�ȕi..�̂��肾�������{�c
void search_specific_object(int mode)
{
	int i;
	int count = 0;
	int temp_dist = 999, temp_dist_new;

	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];
		if (!o_ptr->k_idx) continue;
//object_desc(tmp_str, o_ptr, OD_STORE);
//msg_format("%s..", tmp_str);
		if (o_ptr->held_m_idx) continue;
		if (o_ptr->marked & (OM_TOUCHED))continue;

		if(mode == 1)
		{
			if (!object_is_ego(o_ptr)) continue;
		}
		else if(mode == 2)
		{
			if (!object_is_artifact(o_ptr)) continue;
		}
		else if(mode == 3)
		{
			if (o_ptr->tval != TV_MATERIAL) continue;
			if (o_ptr->sval == SV_MATERIAL_STONE || o_ptr->sval == SV_MATERIAL_SCRAP_IRON) continue;
		}
		else if(mode == 4)
		{
			if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr) && o_ptr->tval != TV_MATERIAL  && o_ptr->tval != TV_SOUVENIR) continue;
		}
		else if(mode == 5)
		{
			if (!object_is_cursed(o_ptr)) continue;
		}
		else if(mode == 6)
		{
			if (o_ptr->tval != TV_MATERIAL && o_ptr->tval != TV_MUSHROOM) continue;
			if (o_ptr->tval == TV_MATERIAL && (o_ptr->sval == SV_MATERIAL_STONE || o_ptr->sval == SV_MATERIAL_SCRAP_IRON)) continue;
		}
		/*
		else if (mode == 7)
		{
			if (!object_is_wearable(o_ptr) || !object_is_cheap_to_jyoon(o_ptr)) continue;

		}
		*/
		else
		{
			msg_print("ERROR:search_specific_object()�ɒ�`����Ă��Ȃ�mode�l���n���ꂽ");
			return;

		}

		count++;
		temp_dist_new = distance(py,px,o_ptr->iy,o_ptr->ix);
		if(temp_dist_new < temp_dist)	temp_dist = temp_dist_new;
	}
	if(count < 1)
	{
		if(mode == 1) msg_format("���̊K�ɂ͏��̎d���ƌĂׂ���̂͂Ȃ��悤���B");
		if(mode == 2) msg_format("���̊K�ɂ͓`���̕�ƌĂׂ���̂͂Ȃ��悤���B");
		if(mode == 3) msg_format("���̊K�ɂ͑f�ނȂǂ͂Ȃ������Ȋ������B");
		if(mode == 4) msg_format("�������������䂭���̂͌�������Ȃ������B");
		if(mode == 5) msg_format("���̊K�ɂ͖�ɖ��������i�͂Ȃ��悤���B");
		if(mode == 6) msg_format("���̊K�̐A���ɂ͂Ƃ��ɋ������������̂͂Ȃ��B");
	//	if (mode == 7) msg_format("���̊K�ɂ͋������������̂͂Ȃ��������B");
	}
	else
	{
		char msg_mode[24];
		char msg_dist[16];

		if(mode == 1) my_strcpy(msg_mode,"���̎d���̍���",sizeof(msg_mode)-2);
		if(mode == 2) my_strcpy(msg_mode,"�`���Ƃ���",sizeof(msg_mode)-2);
		if(mode == 3) my_strcpy(msg_mode,"���Y���̓���",sizeof(msg_mode)-2);
		if(mode == 4) my_strcpy(msg_mode,"�����̍���",sizeof(msg_mode)-2);
		if(mode == 5) my_strcpy(msg_mode,"��̋C�z",sizeof(msg_mode)-2);
		if(mode == 6) my_strcpy(msg_mode,"�����I�ȍ���",sizeof(msg_mode)-2);
	//	if (mode == 7) my_strcpy(msg_mode, "�����ȕ����i�̓���", sizeof(msg_mode) - 2);

		if(temp_dist < 15) my_strcpy(msg_dist,"�߂��I",sizeof(msg_dist)-2);
		else if(temp_dist < 30) my_strcpy(msg_dist,"���߂��B",sizeof(msg_dist)-2);
		else if(temp_dist < 60) my_strcpy(msg_dist,"��≓���B",sizeof(msg_dist)-2);
		else if(temp_dist < 100) my_strcpy(msg_dist,"�����B",sizeof(msg_dist)-2);
		else  my_strcpy(msg_dist,"���Ȃ艓���B",sizeof(msg_dist)-2);

		msg_format("%s��������B%s",msg_mode, msg_dist);
	}
}

/*:::��H���@�ψق����łȂ��S�[�������g�����ߕʊ֐��ɂ���*/
bool eat_rock(void)
{
	int dir;
	int x, y;
	cave_type *c_ptr;
	feature_type *f_ptr, *mimic_f_ptr;

	if (!get_rep_dir2(&dir)) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];
	c_ptr = &cave[y][x];
	f_ptr = &f_info[c_ptr->feat];
	mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];

	mutation_stop_mouth();

	if (!have_flag(mimic_f_ptr->flags, FF_HURT_ROCK))
	{
		msg_print("���̒n�`�͐H�ׂ��Ȃ��B");
		return FALSE;
	}
	else if (have_flag(f_ptr->flags, FF_PERMANENT))
	{
		msg_format("���Ă��I����%s�͂��Ȃ��̎����d���I", f_name + mimic_f_ptr->name);
		return FALSE;
	}
	else if (c_ptr->m_idx)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];
		msg_print("�������ז����Ă��܂��I");
		if (!m_ptr->ml || !is_pet(m_ptr))
		{
			py_attack(y, x, 0);
			return TRUE;
		}
		else return FALSE;
	}
	else if (have_flag(f_ptr->flags, FF_TREE))
	{
		msg_print("�؂̖��͍D������Ȃ��I");
		return FALSE;
	}
	else if (have_flag(f_ptr->flags, FF_GLASS))
	{
		msg_print("�K���X�̖��͍D������Ȃ��I");
		return FALSE;
	}
	else if (have_flag(f_ptr->flags, FF_DOOR) || have_flag(f_ptr->flags, FF_CAN_DIG))
	{
		(void)set_food(p_ptr->food + 3000);
	}
	else if (have_flag(f_ptr->flags, FF_MAY_HAVE_GOLD) || have_flag(f_ptr->flags, FF_HAS_GOLD))
	{
		(void)set_food(p_ptr->food + 5000);
	}
	else
	{
		msg_format("����%s�͂ƂĂ����������I", f_name + mimic_f_ptr->name);
		(void)set_food(p_ptr->food + 10000);
	}

	/* Destroy the wall */
	cave_alter_feat(y, x, FF_HURT_ROCK);
	/* Move the player */
	(void)move_player_effect(y, x, MPE_DONT_PICKUP);

	return TRUE;

}

/*:::�����Ԕ؊�̂Ƃ��A�s���J�n���ɓ��̕��g�̎��E�����X�V*/
/*:::�d���̕��g�������悤�ɏ������邱�Ƃɂ����B*/
void update_minion_sight(void)
{
	int x, y, x2, y2,i,j;
	s16b            feat;
	bool flag = FALSE; //�Y���z��������Ƃ��}�b�v�ĕ`�悷��t���O
	feature_type    *f_ptr;

	if(p_ptr->pclass != CLASS_BANKI && p_ptr->pclass != CLASS_YOUMU) return;

	for(i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		if(!is_pet(m_ptr)) continue;
		if (p_ptr->pclass == CLASS_BANKI)
		{
			if(m_ptr->r_idx != MON_BANKI_HEAD_1 && m_ptr->r_idx != MON_BANKI_HEAD_2) continue;
		}
		else if(p_ptr->pclass == CLASS_YOUMU)
		{
			if(m_ptr->r_idx != MON_YOUMU || !(m_ptr->mflag & MFLAG_EPHEMERA)) continue;
		}
		else msg_format("ERROR:�z�����E�A�b�v�f�[�g��������������");
		flag = TRUE;


		x2 = m_ptr->fx;
		y2 = m_ptr->fy;
		/*�n�`���m*/
		for (y = 1; y < cur_hgt - 1; y++)
		{
			for (x = 1; x < cur_wid - 1; x++)
			{
				cave_type *c_ptr = &cave[y][x];
				if(distance(y,x,y2,x2) > MAX_SIGHT) continue;
				if(!los(y,x,y2,x2)) continue;

				/* Memorize terrain of the grid */
				c_ptr->info |= (CAVE_KNOWN);

				/* Feature code (applying "mimic" field) */
				feat = get_feat_mimic(c_ptr);
				f_ptr = &f_info[feat];

				/* All non-walls are "checked" */
				if (!have_flag(f_ptr->flags, FF_WALL))
				{
					/* Memorize normal features */
					if (have_flag(f_ptr->flags, FF_REMEMBER))
					{
						/* Memorize the object */
						c_ptr->info |= (CAVE_MARK);
					}
					/* Memorize known walls */
					for (j = 0; j < 8; j++)
					{
						c_ptr = &cave[y + ddy_ddd[j]][x + ddx_ddd[j]];

						/* Feature code (applying "mimic" field) */
						feat = get_feat_mimic(c_ptr);
						f_ptr = &f_info[feat];

						/* Memorize walls (etc) */
						if (have_flag(f_ptr->flags, FF_REMEMBER))
						{
							/* Memorize the walls */
							c_ptr->info |= (CAVE_MARK);
						}
					}
				}
			}
		}
		/*�A�C�e�����m*/
		for (j = 1; j < o_max; j++)
		{
			object_type *o_ptr = &o_list[j];
			if (!o_ptr->k_idx) continue;
			if (o_ptr->held_m_idx) continue;
			y = o_ptr->iy;
			x = o_ptr->ix;

			if(!los(y,x,y2,x2)) continue;
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
		//	lite_spot(y, x);

		}

	}
	if(flag) prt_map();
	return;
}

/*:::�~�X�e�B�A�̉̂Œ��ڂɂȂ��Ă��邩�ǂ����̔���*/
///mod150308 �ɂƂ�̌��w���ʂ�����Ŕ��肷�邱�Ƃɂ���
///mod150801 �T�j�[�̌��f�U������
//v1.1.22 ���m�̖��Z�\�`�F�b�N�ɂ��g��
bool check_mon_blind(int m_idx)
{
	monster_type    *m_ptr; 
	monster_race    *r_ptr;
	int chance;

	if(m_idx <= 0)
	{
		msg_print("ERROR:check_mon_blind()�ɓn���ꂽm_idx�l����������");
		return FALSE;
	}
	m_ptr = &m_list[m_idx];
	r_ptr = &r_info[m_ptr->r_idx];

	//���E�ɑS�������ĂȂ������ȃV���{����FALSE
	if(my_strchr("bjmsvQV", r_ptr->d_char)) return FALSE;
	if( !isalpha(r_ptr->d_char)) return FALSE;

	//�~�X�e�B�A�鐝�͖̉̂��͂ƕ����̖��邳���傫���e��
	if(p_ptr->pclass == CLASS_MYSTIA && music_singing(MUSIC_NEW_MYSTIA_YAZYAKU))
	{
		chance = p_ptr->lev*3 + (p_ptr->skill_stl+10)*4 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
		if (cave[py][px].info & CAVE_GLOW) chance /= 3;
		else if(p_ptr->cur_lite == 0) chance *= 3;
	}
	else if(p_ptr->pclass == CLASS_SOLDIER)//���m�̃m�N�g�r�W��������@EXTRA���[�h�Ō��w���ʂƂ��g�����Ƃ����������̎��ɂȂ邪�܂�������
	{
		chance = p_ptr->lev*6 + (p_ptr->skill_stl+5)*6;
	}
	else
	{
		chance = p_ptr->lev*3 + (p_ptr->skill_stl+5)*3;
		if(chance < 50) chance=50;
	}

	if(my_strchr("iwEJLSUWXZ", r_ptr->d_char)) chance /= 3;
	else if(r_ptr->flags3 | (RF3_UNDEAD)) chance /= 3;
	else if(my_strchr("acfgklqruyACDGIY", r_ptr->d_char)) chance = chance / 2;
	if (r_ptr->flags2 & RF2_SMART) chance /= 2;
	if (p_ptr->cursed & TRC_AGGRAVATE) chance /= 5;
	if (r_ptr->level > p_ptr->lev * 2) chance /= 3;

	return (randint0(chance) > r_ptr->level);

}

//�[�U�A�C�e���I��̂��߂ɋ@�B��sval��ێ�����O���[�o���ϐ�������Ă��܂����Bextern�͕K�v���H
int item_tester_engineer_supply_num = 0;

/*:::�G���W�j�A�����Օi���[���邽�߂�hook �O���[�o���ϐ��g�p*/
bool item_tester_hook_engineer_supply(object_type *o_ptr)
{
	if(o_ptr->tval == machine_table[item_tester_engineer_supply_num].tval && o_ptr->sval == machine_table[item_tester_engineer_supply_num].sval) return TRUE;
	else return FALSE;
}


/*:::�G���W�j�A�̋@�B�̎ˌ������B��Ďˌ����[�`���Ƌ��p���邽�ߕ����Ă���B�ˌ�����łȂ��ꍇFALSE���Ԃ�*/
bool fire_machine_aux(int sv, int dir)
{

	switch(sv)
	{
	case SV_MACHINE_WATERGUN:
		msg_print("���S�C���������B");
		fire_bolt(GF_WATER, dir, randint1(10));
		break;
	case SV_MACHINE_FLAME: 
		msg_print("�Ή�����˂����I");
		fire_ball(GF_FIRE, dir, (25+randint1(50)), -2);
		break;
	case SV_MACHINE_TORPEDO:
		msg_print("�����𔭎˂����I");
		fire_rocket(GF_ROCKET, dir, 250, 1);
		break;
	case SV_MACHINE_MOTORGUN:
		msg_print("�}�V���K���𗐎˂����I");
		fire_blast(GF_ARROW, dir, 2, 20, 10, 2, 0);
		break;
	case SV_MACHINE_LASERGUN:
		msg_print("���[�U�[���������I");
		fire_beam(GF_LITE, dir, 150);
		break;
	case SV_MACHINE_PLASMAGUN:
		msg_print("�v���Y�}��������I");
		fire_bolt(GF_PLASMA, dir, 300);
		break;
	case SV_MACHINE_GATLINGGUN:
		msg_print("�K�g�����O�C���΂𕬂����I");
		fire_blast(GF_ARROW, dir, 2, 20, 20 + randint0(11), 3, 0);
		break;
	case SV_MACHINE_CANNON:
		msg_print("�����Ƌ��ɖC�e�������o���ꂽ�I");
		fire_ball(GF_ARROW, dir, 400+randint1(200), 2);
		break;
	case SV_MACHINE_ROCKET_6:
		msg_print("���P�b�g�e���y�􂵂��I");
		fire_rocket(GF_ROCKET, dir, 400, 1);
		break;
	case SV_MACHINE_NEUTRINOGUN:
		msg_print("�s���̍��G�l���M�[�̂������ꂽ�I");
		fire_beam(GF_PSY_SPEAR, dir, 180 + randint1(180));
		break;
	case SV_MACHINE_WAVEMOTION:
		msg_print("�����k�^�L�I�����q���_���W�������т����I");
		fire_spark(GF_DISINTEGRATE,dir,damroll(50,50),2);
		break;
	case SV_MACHINE_N_E_P:
		msg_print("���ʗ��̏C�������������I");
		fire_spark(GF_N_E_P,dir,9999,2);
		break;

	default:
		return FALSE;
	}



return TRUE;

}
/*:::�G���W�j�A��p�@�@�B���g�� mode=0�̂Ƃ��̓A�C�e���̐��������邾����1�̂Ƃ��͒e����[����B �s��������Ȃ��Ƃ�FALSE���Ԃ�*/
/*:::�ˌ��p�@�B��fire_machine_aux(slot,dir)�ɓn���B��Ďˌ����[�`���Ƌ��p���邽��*/
bool use_machine(int mode)
{
	int inven2_num = calc_inven2_num();
	int item, amt, i, tmp;
	char c;
	cptr    q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	bool only_browse = FALSE,supply = FALSE, use_machine = FALSE;

	cptr pack = (p_ptr->pclass == CLASS_NITORI)?"�����b�N":"�o�b�N�p�b�N";

	if(mode == 0) only_browse = TRUE;
	else if(mode == 1) supply = TRUE;
	else use_machine = TRUE;

	//Paranoia
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI) return FALSE; 

	screen_save();


	item = 0;
#ifdef ALLOW_REPEAT
	if (!repeat_pull(&item))
#endif
	{
		do
		{
			(void)show_inven2(SI2_MODE_ENGINEER);
			if(only_browse) prt("�ǂ̋@�B���m�F���܂����H", 0, 0);
			else if(supply) prt("�ǂ̋@�B�֕�[���s���܂����H", 0, 0);
			else prt("�ǂ̋@�B���g���܂����H", 0, 0);
			c = inkey();
			if(c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
		}while(c < 'a' || c >= ('a' + inven2_num));
		item = c-'a';
#ifdef ALLOW_REPEAT
		repeat_push(item);
#endif
	}

	o_ptr = &inven_add[ item ];
	if(!o_ptr->k_idx)
	{
		msg_format("�����ɂ͉��������Ă��Ȃ��B");
		screen_load();
		return FALSE;
	}
	if(only_browse)
	{
		screen_object(o_ptr, SCROBJ_FORCE_DETAIL);
		screen_load();
		return FALSE;
	}
	else if(o_ptr->xtra4)
	{
		msg_format("���̋@�B�͍��̏ᒆ���B");
		screen_load();
		return FALSE;
	}

	else if(supply && !machine_table[o_ptr->sval].tval)
	{
		msg_format("���̋@�B�ɂ͕�[���ł��Ȃ��B");
		screen_load();
		return FALSE;
	}
	else if(supply && machine_table[o_ptr->sval].charge == o_ptr->pval)
	{
		msg_format("���̋@�B�͂��łɕ�[���ς�ł���B");
		screen_load();
		return FALSE;
	}
	else if(use_machine && !o_ptr->pval)
	{
		switch(o_ptr->sval)
		{
		case SV_MACHINE_WATERGUN:
			msg_print("�^���N���󂾁B"); break;
		case SV_MACHINE_SHIRIKO:
		case SV_MACHINE_FLAME:
		case SV_MACHINE_J_HAMMER:
		case SV_MACHINE_ROCKETEER:
			msg_print("�R���؂ꂾ�I"); break;
		case SV_MACHINE_DRILLARM:
		case SV_MACHINE_RADAR1:
		case SV_MACHINE_RADAR2:
		case SV_MACHINE_SCOUTER:
			msg_print("�o�b�e���[�؂ꂾ�I"); break;
		case SV_MACHINE_MOTORGUN:
		case SV_MACHINE_GATLINGGUN:
			msg_print("�e�؂ꂾ�I"); break;
		case SV_MACHINE_CANNON:
			msg_print("�܂����e�𑕓U���Ă��Ȃ��B"); break;

		default:
			msg_print("�G�l���M�[�؂ꂾ�I"); break;
		}
		screen_load();
		return FALSE;
	}

	screen_load();

	if(supply) //�@�B�[�U��
	{
		feature_type *f_ptr = &f_info[cave[py][px].feat];
		//���S�C�ɐ��ӂŏ[�U���鏈��
		if( (o_ptr->sval == SV_MACHINE_WATERGUN) && have_flag( f_ptr->flags, FF_WATER))
		{
			msg_print("���̕ӂ̐��𐅓S�C�̃^���N�Ƀu�N�u�N���ꂽ�B");
		}
		else
		{
			item_tester_engineer_supply_num = o_ptr->sval;

			item_tester_hook = item_tester_hook_engineer_supply;

			q = "�ǂ���g���ĕ�[���܂����H ";
			s = "���̋@�B�̕�[�p�A�C�e���������Ă��Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;
			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}
			switch(o_ptr->sval)
			{
			case SV_MACHINE_WATERGUN:
				msg_print("���S�C�ɐ�����ꂽ�B"); break;
			case SV_MACHINE_SHIRIKO:
			case SV_MACHINE_FLAME:
			case SV_MACHINE_J_HAMMER:
			case SV_MACHINE_ROCKETEER:
				msg_print("�R����⋋�����B"); break;
			case SV_MACHINE_DRILLARM:
			case SV_MACHINE_RADAR1:
			case SV_MACHINE_RADAR2:
			case SV_MACHINE_SCOUTER:
				msg_print("�o�b�e���[�����������B"); break;
			case SV_MACHINE_MOTORGUN:
			case SV_MACHINE_GATLINGGUN:
				msg_print("�e�q�����������B"); break;
			case SV_MACHINE_CANNON:
				msg_print("�C�e�����߂��B"); break;

			default:
				msg_print("�G�l���M�[�p�b�N����芷�����B"); break;

			}
		}

		o_ptr->pval = machine_table[o_ptr->sval].charge;

	}
	else //�@�B�g�p��
	{
		int dir,x,y,v;
		int sv = o_ptr->sval;
		switch(sv)
		{
		case  SV_MACHINE_WATERGUN:
		case  SV_MACHINE_FLAME:
		case  SV_MACHINE_TORPEDO:
		case  SV_MACHINE_MOTORGUN:
		case  SV_MACHINE_LASERGUN:
		case SV_MACHINE_PLASMAGUN:
		case SV_MACHINE_GATLINGGUN:
		case SV_MACHINE_CANNON:
		case SV_MACHINE_ROCKET_6:
		case SV_MACHINE_NEUTRINOGUN:
		case SV_MACHINE_WAVEMOTION:
		case SV_MACHINE_N_E_P:

			if (!get_aim_dir(&dir)) return FALSE;
			fire_machine_aux(sv,dir);

			///mod150712 ���i�g���K�[�n�b�s�[�̎ˌ����x�{�[�i�X
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) 
				new_class_power_change_energy_need = 80;

			break;
		case  SV_MACHINE_SHIRIKO:
			msg_format("%s�����ʂ̃A�[���Ɗ�Ȋ���яo���A�󒆂ŋC���̈��������������E�E",pack);
			turn_monsters(50);
			break;
		case  SV_MACHINE_LIGHTBOMB:
			msg_print("�M���e��������I");
			lite_area(50, 5);
			confuse_monsters(100);
			break;
		case SV_MACHINE_DRILLARM:
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir == 5) return FALSE;
			msg_format("%s���猃������]����h��������яo�����I",pack);
			y = py + ddy[dir];
			x = px + ddx[dir];
			project(0, 0, y, x, 0, GF_KILL_WALL, PROJECT_GRID, -1);
			project(0, 0, y, x, damroll(10,10), GF_MISSILE, PROJECT_KILL, -1);
			break;
		case  SV_MACHINE_RADAR1:
			detect_monsters_living(DETECT_RAD_DEFAULT);
			break;
		case SV_MACHINE_J_HAMMER:
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir == 5) return FALSE;
			msg_format("%s����΂𕬂�����ȃn���}�[����яo�����I",pack);
			y = py + ddy[dir];
			x = px + ddx[dir];
			project(0, 0, y, x, 150 + randint1(150), GF_MISSILE, PROJECT_KILL, -1);
			earthquake(y, x, 5);
			break;
		case SV_MACHINE_ROCKETEER:
			if(!rush_attack3(15,"���Ȃ��̓W�F�b�g���˂Ő�����񂾁I",damroll(10,10))) return FALSE;
			break;
		case SV_MACHINE_SCOUTER:
			scouter_use = TRUE;
			probing(); 
			//Hack - �O���[�o���ϐ��g���č����x���ȓG���肾�Ɖ���悤�ɂ���
			if(!scouter_use)
			{
				msg_print("�X�J�E�^�[�͔������ĉ�ꂽ�I");
				object_wipe(o_ptr);
				return TRUE;
			}
			scouter_use = FALSE;

			break;
		case SV_MACHINE_BARRIER1:
			msg_print("���F�̃o���A�𒣂����I");
			v = randint1(20) + 20;
			set_oppose_acid(v, FALSE);
			set_oppose_elec(v, FALSE);
			set_oppose_fire(v, FALSE);
			set_oppose_cold(v, FALSE);
			set_oppose_pois(v, FALSE);
			break;
		case SV_MACHINE_MEGACLASH:
			msg_print("���K�N���b�V�������֎ˏo�����B");
			if(dun_level) destroy_area(py, px, 20 + randint1(10), FALSE,FALSE,FALSE);
			else if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
				msg_print("���ɗ����Q�̂悤�Ȍ��̉Ԃ��炢���B");
			else 
				msg_print("��ɗ����Q�̂悤�Ȍ��̉Ԃ��炢���B");
			break;
		case SV_MACHINE_RADAR2:
			msg_format("%s����Z���T�����{����яo�����B",pack);
			set_radar_sense(20 + randint1(20), FALSE);
			break;
		case SV_MACHINE_TELEPORTER:
				msg_print("�e���|�[�^�[���N�������E�E");
				if (!dimension_door(D_DOOR_MACHINE)) return FALSE;
			break;
		case SV_MACHINE_PHONON_MASER:
			msg_format("%s���J���A�U���̂�����n�߂��E�E�E",pack);
			project_hack2(GF_SOUND, 0,0,200);
			break;

		case SV_MACHINE_ACCELERATOR:
			msg_print("���͂̓�����������肵�Č�����E�E�E");
			v = randint1(25) + 25;
			set_fast(v, FALSE);
			break;

		case SV_MACHINE_BARRIER2:
			msg_print("�����Ɏ���Ă��銴��������E�E");
			v = randint1(20) + 20;
			set_resist_magic(v, FALSE);
			break;

		case SV_MACHINE_BARRIER3:
			msg_print("�˗̓t�B�[���h��W�J�����I");
			v = randint1(20) + 20;
			set_tim_reflect(v, FALSE);
			set_shield(v, FALSE);
			break;

		case SV_MACHINE_MIDAS_HAND:
			if (!alchemy()) return FALSE; 
			break;

		case SV_MACHINE_E_CAN:
			msg_print("����Ă�����E�ʂ��g�����I");
			hp_player(5000);
			set_cut(0);
			break;

		case SV_MACHINE_TWILIGHT_ZONE:
			{
				int percentage;
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				v = randint1(16) + 16;
				msg_print("���������E�E");
				msg_print(NULL);
				msg_print("�����͕s�v�c��ԃg���C���C�g�]�[���I"); 
				msg_print("���Ȃ��̃p���[���O�{�ɂȂ����C������I"); 
				set_lightspeed(v,FALSE);
				for(i=0;i<6;i++)set_tim_addstat(i,120,v,FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= (PR_HP );
				redraw_stuff();

			}
			break;

		default:
			msg_format("ERROR:�@�B(sval:%d)�̎g�p�����ʂ���`����Ă��Ȃ�",o_ptr->sval);
			return FALSE;
		}
		/*:::pval(�g�p��)��1�����B�[�U�A�C�e�����ݒ肳��Ă��Ȃ��g���̂ĕi��pval0�ɂȂ����ꍇ�A�C�e������������B�G���W�j�A�Ȃ̂Ńo�b�N�p�b�N�̏d�ʂ��l�����Ȃ��B*/
		o_ptr->pval--;
		if((!o_ptr->pval) && (!machine_table[o_ptr->sval].tval)) object_wipe(o_ptr);
	}



	return TRUE;
}


/*:::�G���W�j�A�̈�Ďˌ��R�}���h �s���������Ƃ�TRUE*/
bool engineer_triggerhappy(void)
{
	int fire_cnt = 0;
	int inven2_num = calc_inven2_num();
	int dir, i;
	object_type *o_ptr;

	if(p_ptr->pclass != CLASS_ENGINEER)
	{
		msg_print("ERROR:��Ή��̐E��triggerhappy()���Ă΂ꂽ");
		return FALSE;
	}

	if (!get_aim_dir(&dir)) return FALSE;
	screen_save();
	(void)show_inven2(SI2_MODE_ENGINEER);
	if (!get_check_strict("��Ďˌ����܂����H", CHECK_DEFAULT_Y))
	{
		screen_load();
		return FALSE;
	}
	screen_load();

	msg_print("�o�b�N�p�b�N�̑S�������W�J���ꂽ�I");

	for(i=0;i<inven2_num;i++)
	{
		o_ptr = &inven_add[i];

		if(!o_ptr->k_idx) continue; //��
		if(!o_ptr->pval) continue; //�e�؂�
		if(o_ptr->xtra4) continue; //�̏ᒆ
		if(o_ptr->tval != TV_MACHINE) msg_print("ERROR:�o�b�N�p�b�N�ɋ@�B�ȊO����������Ԃ�engineer_triggerhappy()���Ă΂ꂽ");

		/*:::�ˌ����Ȃ��@�B��FALSE���Ԃ�̂Ŏˌ���������̂ݎg�p�񐔌�����J�E���g����*/
		if(fire_machine_aux(o_ptr->sval, dir))
		{
			fire_cnt++;
			o_ptr->pval--;
			//��[�s�̋@�B����0�ɂȂ�������Ł@�G���W�j�A�͏d�ʂ��l�����Ȃ�
			if((!o_ptr->pval) && (!machine_table[o_ptr->sval].tval)) object_wipe(o_ptr); 
		}
		if(dir == 5 && !target_okay()) break; //�^�[�Q�b�g���|�ꂽ��I���
	}

	if(!fire_cnt)
	{
		msg_print("�������ˌ��ł���@�B�𑕔����Ă��Ȃ������E�E");
	}
	else
	{
		//energy_use���Đݒ肷��ϐ�
		new_class_power_change_energy_need = 100 + fire_cnt * 25; 
		///mod150712 �g���K�[�n�b�s�[�̎ˌ��{�[�i�X
		if(p_ptr->pseikaku == SEIKAKU_TRIGGER) 
			new_class_power_change_energy_need = new_class_power_change_energy_need * 4 / 5;

	}

	return TRUE;

}



/*:::�G���W�j�A�̋@�B������d���ňꎞ�g�p�s�\�ɂȂ鏈���B���肪�ʂ��xtra4�����Z����A0�ɖ߂�܂ł͎g�p�E���O���E�⋋�s��*/
/*:::typ=0�ŌĂԂƉ񕜏����B�S�Ă̑������̋@�B��xtra4��dam(��) �l�����Z�����B���Ԍo�߁A�h���ŌĂ΂��*/
void engineer_malfunction(int typ, int dam)
{
	int inven2_num = calc_inven2_num();
	int dir, i;
	int chance;
	object_type *o_ptr;
	char o_name[MAX_NLEN];

	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI) return;
	if(dam <= 0) return;
	if(typ == GF_WATER && (p_ptr->prace == RACE_KAPPA  )) return; //�͓��̋@�B�͖h���@v1.1.86 �R���͊O����
	if(typ == GF_ELEC && (p_ptr->immune_elec || p_ptr->resist_elec && IS_OPPOSE_ELEC() )) return; //�d���̖Ɖu����d�ϐ�������ƌ̏��h����

	for(i=0;i<inven2_num;i++)
	{
		o_ptr = &inven_add[i];
		if(!o_ptr->k_idx) continue;
		if(o_ptr->tval != TV_MACHINE)
		{
			msg_format("ERROR:�@�B�ȊO(Tval:%d)�ɑ΂�engineer_malfunction()���Ă΂�Ă���",o_ptr->tval);
			return;

		}
		//�s������̉񕜏���
		if(!typ)
		{
			if( o_ptr->xtra4)
			{
				o_ptr->xtra4 -= dam;
				if(o_ptr->xtra4 < 0) o_ptr->xtra4 = 0;
				if(!o_ptr->xtra4)
				{
					object_desc(o_name, o_ptr, 0);
					msg_format("%s�͋@�\���񕜂����I",o_name);
				}
			}
		}
		else
		{
			if(typ == GF_WATER) chance = dam * machine_table[o_ptr->sval].hate_water / 100;
			else if(typ == GF_ELEC) chance = dam * machine_table[o_ptr->sval].hate_elec / 100;
			else msg_format("ERROR:����`�̑�����engineer_malfunction()���Ă΂ꂽ");
			if(randint0(100) < chance)
			{
				object_desc(o_name, o_ptr, 0);
				if(!o_ptr->xtra4) msg_format("%s���̏Ⴕ���I",o_name);
				else  msg_format("%s�̒��q������ɂ��������Ȃ����悤���E�E",o_name);
				o_ptr->xtra4 += chance;
				if(o_ptr->xtra4 > 10000) o_ptr->xtra4 = 10000;
			}
		}
	}

}



/*:::�G���W�j�A�M���h�ŏ��Օi���w������Ƃ��̂��߂̉��i�W����ύX����B�Q�[���X�^�[�g��,6:00,18:00�ɌĂ΂��B*/
/*:::p_ptr->magic_num1[0]�`[19]�ɑ΂��A-50�`+50�̒l���i�[����B�͓��̗��ł͂��͈̔͂ŏ��-10�����B*/
void engineer_guild_price_change(void)
{
	int i, tmp;
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI) return;

	for(i=0;i<20;i++) 
	{
		/*:::3/7�ŕω��Ȃ��A3/7�őO�̒l�ց}5����A1/7�őS���V�����l�ɂȂ�B�V�����l�͒��S�ɕ΂�A�����͂Ȃ�ɂ����B*/
		if(one_in_(7))
		{
			tmp = damroll(2,50) - 50;
			if(tmp<0) tmp = 0;
			else if(one_in_(3)) tmp = 0 - tmp;
			p_ptr->magic_num1[i] = tmp;
		}
		else if(one_in_(2))
		{
			tmp = p_ptr->magic_num1[i] + randint0(11) - 5;
			if(tmp < -50) tmp = -50;
			if(tmp > 50) tmp = 50;
			p_ptr->magic_num1[i] = tmp;
		}

	}
}

















/*:::�ːi���[�`��3 �G��e����΂��ă_���[�W��^���ړ����邪����ȓG�ɂ͎~�߂���*/
/*:::length:�˒� msg:�������̃Z���t dam:�_���[�W*/
bool rush_attack3(int length ,cptr msg, int dam)
{
	int dir;
	int tx, ty;
	u16b path_g[32];
	int path_n, i;
	bool moved = FALSE;

	project_length = length;
	if (!get_aim_dir(&dir)) return FALSE;
	tx = px + project_length * ddx[dir];
	ty = py + project_length * ddy[dir];
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	if (in_bounds(ty, tx)) msg_format("%s",msg);

	path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_THRU | PROJECT_KILL);
	project_length = 0;
	if (!path_n) return FALSE;
	ty = py;
	tx = px;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];
		bool stop = FALSE;

		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);
	
		if (cave[ny][nx].m_idx)
		{
			m_ptr = &m_list[cave[ny][nx].m_idx];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			if(d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
				msg_format("�Ȃ���%s�̒��O�ő����~�܂����B",m_name);
				break;
			}						
			else if(r_ptr->flags2 & RF2_GIGANTIC)
			{			
				msg_format("%s�ɓːi���~�߂�ꂽ�I",m_name);
				(void)project(0, 0, ny, nx, p_ptr->lev * 3, GF_MISSILE, PROJECT_HIDE|PROJECT_KILL, -1);
				break;
			}
			else 
			{
				msg_format("%s��e����΂����I",m_name);
				(void)project(0, 0, ny, nx, dam, GF_MISSILE, PROJECT_HIDE|PROJECT_KILL, -1);
				if(cave[ny][nx].m_idx) teleport_away(cave[ny][nx].m_idx, 5,TELEPORT_PASSIVE);
			}
		}
		if (cave_empty_bold(ny, nx) && player_can_enter(cave[ny][nx].feat, 0))
		{
			ty = ny;
			tx = nx;
			if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
		}
		else 
		{
			msg_print("�����ɂ͓���Ȃ��B");
			break;
		}

	}
	return TRUE;
}

/***���̉��A�����X�^�[���@�֘A****************************************/


//�����X�^�[���@�̕��ނ�I������B���̂Ƃ���B��ސ�p
//v1.1.82 �V�E�Ɓu�e�������Ɓv���g��
static int choose_monspell_kind(void)
{

	int choose_kind = 0;
	char choice;
	int menu_line = 1;


	if (repeat_pull(&choose_kind)) return choose_kind;
	screen_save();
	while (!choose_kind)
	{
		//����menu_line�̍s��NEW_MSPELL_TYPE_***����v���Ă��邱�ƁB
		//�񋓌^�Ƃ��g���΂����ƃX�}�[�g�ɏ�����炵�����悭�m��Ȃ�:(
		prt(format(" %s a) �{���g/�r�[��/���P�b�g", (menu_line == 1) ? "�t" : "  "), 2, 14);
		prt(format(" %s b) �{�[��", (menu_line == 2) ? "�t" : "  "), 3, 14);
		prt(format(" %s c) �u���X", (menu_line == 3) ? "�t" : "  "), 4, 14);
		prt(format(" %s d) ���̑��̍U��", (menu_line == 4) ? "�t" : "  "), 5, 14);
		prt(format(" %s e) ����", (menu_line == 5) ? "�t" : "  "), 6, 14);
		prt(format(" %s f) ���̑�", (menu_line == 6) ? "�t" : "  "), 7, 14);
		prt("�ǂ̎�ނ̔\�͂��g���܂����H", 0, 0);

		choice = inkey();
		switch (choice)
		{
		case ESCAPE:
			screen_load();
			return 0;
		case '2':
		case 'j':
		case 'J':
			menu_line++;
			break;
		case '8':
		case 'k':
		case 'K':
			menu_line += 5;
			break;
		case '\r':
		case 'x':
		case 'X':
			choose_kind = menu_line;
			break;
		case 'a':
		case 'A':
			choose_kind = NEW_MSPELL_TYPE_BOLT_BEAM_ROCKET;
			break;
		case 'b':
		case 'B':
			choose_kind = NEW_MSPELL_TYPE_BALL;
			break;
		case 'c':
		case 'C':
			choose_kind = NEW_MSPELL_TYPE_BREATH;
			break;
		case 'd':
		case 'D':
			choose_kind = NEW_MSPELL_TYPE_OTHER_ATTACK;
			break;
		case 'e':
		case 'E':
			choose_kind = NEW_MSPELL_TYPE_SUMMMON;
			break;
		case 'f':
		case 'F':
			choose_kind = NEW_MSPELL_TYPE_OTHER;
			break;

		}
		if (menu_line > 6) menu_line -= 6;
	}
	screen_load();
	repeat_push(choose_kind);

	return choose_kind;
	
}

//�����X�^�[���@���g�����Ƃ��̃��b�Z�[�W�@�N���X�̂ق��U�����@�������łȂ����Ȃǂŕ���
static void msg_cast_monspell_new(int num, cptr msg, int xtra)
{
	if(xtra == CAST_MONSPELL_EXTRA_KYOUKO_YAMABIKO) //�A�C�e���J�[�h�̃��}�r�R�p
		msg_format("���Ȃ��́u%s�v��%s",monspell_list2[num].name,msg);
	else if(p_ptr->pclass == CLASS_YUKARI && monspell_list2[num].attack_spell)
		msg_format("�X�L�}����%s����яo�����I",monspell_list2[num].name,msg);
	else if(p_ptr->pclass == CLASS_YORIHIME)
	{
		cptr mname;
		int r_idx = p_ptr->magic_num1[20];

		//v1.1.30 �����_�����j�[�N�̖��O�����ǉ�
		if(IS_RANDOM_UNIQUE_IDX(r_idx))
			mname = random_unique_name[r_idx - MON_RANDOM_UNIQUE_1];
		else
			mname = r_name + r_info[r_idx].name;

		if(monspell_list2[num].attack_spell)
			msg_format("�u%s��A%s����āI�v",mname,monspell_list2[num].name);
		else
			msg_format("�u%s��A%s�������点�I�v",mname,monspell_list2[num].name);
	}
	else if(p_ptr->pclass == CLASS_OKINA)
	{
		msg_format("���Ȃ��́u���E%s�v��%s", monspell_list2[num].name, msg);
	}
	else
	{
		msg_format("���Ȃ��́u%s�v��%s", monspell_list2[num].name, msg);
	}

}

/*:::�V���������X�^�[���@�g�p���[�`�� ���ɐ��������MP�����͍ς�ł��邱��*/
//�߂�l�Fonly_info�̂Ƃ��З͂Ȃǂ̊ȈՐ����� �s�����"" �s������NULL
//num�Fmonspell_list2[]�̓Y����
//fail�F���s��TRUE �������s�����Ȃ�
cptr cast_monspell_new_aux(int num, bool only_info, bool fail, int xtra)
{
	int cast_lev;
	int cast_hp;
	int plev = p_ptr->lev;
	bool powerful=FALSE;
	cptr msg;
	int dir;
	int dam,base,dice,sides;

	if(xtra == CAST_MONSPELL_EXTRA_KYOUKO_YAMABIKO)
	{
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp;
		msg = "�I�E���Ԃ������I";
	}

	else if(p_ptr->pclass == CLASS_SATORI)
	{
		cast_lev = p_ptr->lev * 3 / 2;
		cast_hp = p_ptr->chp;
		if(p_ptr->lev > 44) powerful = TRUE;
		msg = "�z�N�����I";
	}
	else if (p_ptr->pclass == CLASS_SATONO)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		cast_lev = r_ptr->level;
		cast_hp = m_ptr->hp;
		if (r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
		msg = "�g�킹���I";
	}
	else if(p_ptr->pclass == CLASS_KYOUKO)
	{
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp;
		msg = "�I�E���Ԃ������I";
	}
	else if(p_ptr->pclass == CLASS_KOKORO)
	{
		object_type *o_ptr = &inventory[INVEN_HEAD];

		if(o_ptr->tval != TV_MASK)
		{
			msg_print("ERROR:�ʂ𑕔����Ă��Ȃ���Ԃ�cast_monspell_new_aux()���Ă΂�Ă���");
			return NULL;
		}

		cast_lev = o_ptr->discount;
		cast_hp = MAX(10,(o_ptr->xtra5 / 6));
		if(have_flag(o_ptr->art_flags,TR_SPECIAL_KOKORO)) powerful = TRUE;
		msg = "�ĉ������I";
	}
	else if(p_ptr->pclass == CLASS_RAIKO)
	{
		//xtra�l�ɔ��背�x��������BPOWERFUL�̂Ƃ�+100����Ă���B
		if(xtra > 99) powerful = TRUE;
		cast_lev = xtra % 100;
		cast_hp = p_ptr->chp;
		msg = "���������I";
	}
	else if(p_ptr->pclass == CLASS_YUKARI)
	{
		if(p_ptr->lev > 44) powerful = TRUE;
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp * 2;
		msg = "�s�g�����I";//�U�����@�̂Ƃ����b�Z�[�W��O��������
	}
	else if(IS_METAMORPHOSIS)
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];
		cast_lev = r_ptr->level;
		cast_hp = p_ptr->chp;
		if(r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
		msg = "�g�����I";

	}
	else if(p_ptr->pclass == CLASS_YORIHIME)
	{
		int r_idx = p_ptr->magic_num1[20];
		int mon_hp;
		monster_race *r_ptr;

		if(!r_idx)
		{
			msg_print("ERROR:�˕Pcast_monspell_new_aux��r_idx���ݒ肳��Ă��Ȃ�");
			if(only_info) return format("");
			return NULL;
		}
		r_ptr = &r_info[r_idx];
		if(r_ptr->flags1 & RF1_FORCE_MAXHP)
			cast_hp = r_ptr->hdice * r_ptr->hside;
		else
			cast_hp = r_ptr->hdice * (r_ptr->hside+1)/2;

		cast_lev = r_ptr->level;
		if(r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
		msg = "�g�p�����I";//��O�����ŏ㏑��
	}
	else if (p_ptr->pclass == CLASS_OKINA)
	{
		cast_lev = p_ptr->lev * 2;
		cast_hp = p_ptr->chp;
		if (p_ptr->lev > 44) powerful = TRUE;
		msg = "�s�g�����I";
	}
	else if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		cast_lev = p_ptr->lev *2;
		cast_hp = p_ptr->chp;
		if (p_ptr->concent)
		{
			if (p_ptr->magic_num1[0] == CONCENT_KANA)
				cast_hp = cast_hp * 2;
			else
				powerful = TRUE;
		}

		msg = "�Č������I";

	}
	else
	{
		msg_print("ERROR:���̃N���X�ł̓G���@���s�p�����[�^����`����Ă��Ȃ�");
		if(only_info) return format("");
		return NULL;
	}



	switch(num)
	{

	case 1://RF4_SHRIEK
	case 128: //RF9_ALARM
		{
			if(only_info) return format("");
			if(fail) return "";

			//msg_format("���Ȃ��́u%s�v��%s",monspell_list2[num].name,msg);
			msg_cast_monspell_new(num, msg, xtra);

			if( num==1) aggravate_monsters(0,FALSE);
			else aggravate_monsters(0,TRUE);
		}
		break;
	case 2://RF4_DANMAKU
		{
			dam = cast_lev;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_bolt(GF_MISSILE, dir, dam);
		}
		break;
	case 3: //RF4_DISPEL
		{
			int m_idx;

			if(only_info) return format("");
			if(fail) return "";

			if (!target_set(TARGET_KILL)) return NULL;
			m_idx = cave[target_row][target_col].m_idx;
			if (!m_idx) break;
			if (!player_has_los_bold(target_row, target_col)) break;
			if (!projectable(py, px, target_row, target_col)) break;

			msg_cast_monspell_new(num, msg, xtra);
			dispel_monster_status(m_idx);
		}
		break;

	case 4: //RF4_ROCKET
		{
			dam = cast_hp / 4;
			if(dam>800) dam=800;

			if(only_info) return format("�����F%d",dam);
			if(fail) return "";
			if (!get_aim_dir(&dir)) return NULL;

			msg_cast_monspell_new(num, msg, xtra);
			fire_rocket(GF_ROCKET, dir, dam, 2);
		}
		break;
	case 5://RF4_SHOOT
		{
			if(IS_METAMORPHOSIS)
			{
				dice = r_info[MON_EXTRA_FIELD].blow[0].d_dice;
				sides = r_info[MON_EXTRA_FIELD].blow[0].d_side;
				if(only_info) return format("�����F%dd%d",dice,sides);

				dam = damroll(dice,sides);
			}
			else
			{
				dam = cast_lev * 3 / 2;
				if(only_info) return format("�����F%d",dam);

			}
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_bolt(GF_MISSILE, dir, dam);
		}
		break;
	case 6: //RF4_BR_HOLY
	case 7: //RF4_BR_HELL
		{
			int typ;
			if(num==6) typ = GF_HOLY_FIRE;
			if(num==7) typ = GF_HELL_FIRE;
			dam = cast_hp / 6;
			if(dam>500) dam=500;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 8: //RF4_BR_AQUA
		{
			dam = cast_hp / 8;
			if(dam>350) dam=350;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_WATER, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 9: //RF4_BR_ACID
	case 10: //RF4_BR_ELEC
	case 11: //RF4_BR_FIRE
	case 12: //RF4_BR_COLD
		{
			int typ;
			if(num==9) typ = GF_ACID;
			if(num==10) typ = GF_ELEC;
			if(num==11) typ = GF_FIRE;
			if(num==12) typ = GF_COLD;

			dam = cast_hp / 3;
			if(dam>1600) dam=1600;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 13: //RF4_BR_POIS
		{
			dam = cast_hp / 3;
			if(dam>800) dam=800;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_POIS, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 14: //RF4_BR_NETH
		{
			int typ;
			if(num==14) typ = GF_NETHER;

			dam = cast_hp / 6;
			if(dam>550) dam=550;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;


	case 15: //RF4_BR_LITE
	case 16: //RF4_BR_DARK
		{
			int typ;
			if(num==15) typ = GF_LITE;
			if(num==16) typ = GF_DARK;

			dam = cast_hp / 6;
			if(dam>400) dam=400;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;

	case 17: //RF4_WAVEMOTION
		{
			dam = cast_hp / 6;
			if(dam>555) dam=555;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_spark(GF_DISINTEGRATE,dir,dam,2);
		}
		break;
	case 18: //RF4_BR_SOUN
		{
			int typ;
			if(num==18) typ = GF_SOUND;

			dam = cast_hp / 6;
			if(dam>450) dam=450;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 19: //RF4_BR_CHAO
		{
			int typ;
			if(num==19) typ = GF_CHAOS;

			dam = cast_hp / 6;
			if(dam>600) dam=600;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 20: //RF4_BR_DISE
	case 25: //RF4_BR_SHAR
		{
			int typ;
			if(num==20) typ = GF_DISENCHANT;
			if(num==25) typ = GF_SHARDS;

			dam = cast_hp / 6;
			if(dam>500) dam=500;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 21: //RF4_BR_NEXU
		{
			int typ;
			if(num==21) typ = GF_NEXUS;

			dam = cast_hp / 7;
			if(dam>600) dam=600;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 22: //RF4_BR_TIME
		{
			int typ;
			if(num==22) typ = GF_TIME;

			dam = cast_hp / 3;
			if(dam>150) dam=150;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 23: //RF4_BR_INACT
		{
			int typ;
			if(num==23) typ = GF_INACT;

			dam = cast_hp / 6;
			if(dam>200) dam=200;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 24: //RF4_BR_GRAV
		{
			int typ;
			if(num==24) typ = GF_GRAVITY;

			dam = cast_hp / 3;
			if(dam>200) dam=200;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 26: //RF4_BR_PLAS
		{
			int typ;
			if(num==26) typ = GF_PLASMA;

			dam = cast_hp / 4;
			if(dam>1200) dam=1200;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 27: //RF4_BA_FORCE
		{
			if(powerful)
			{
				dam = cast_hp / 4;
				if(dam>350) dam=350;
			}
			else
			{
				dam = cast_hp / 6;
				if(dam>250) dam=250;
			}

			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_FORCE, dir, dam, 2);
		}
		break;
	case 28: //RF4_BR_MANA
		{
			int typ;
			if(num==28) typ = GF_MANA;

			dam = cast_hp / 3;
			if(dam>250) dam=250;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 29: //RF4_SPECIAL2
		{

			msg_format("ERROR:���̋Z�͌Ă΂�Ȃ��͂��FRF4_SPECIAL2");
			return NULL;
		}
		break;

	case 30: //RF4_BR_NUKE
		{
			int typ;
			if(num==30) typ = GF_NUKE;

			dam = cast_hp / 4;
			if(dam>1000) dam=1000;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 31: //RF4_BA_CHAO
		{
			if(powerful)
			{
				base = cast_lev * 3;
				dice = 10;
				sides = 10;
			}
			else
			{
				base = cast_lev * 2;
				dice = 10;
				sides = 10;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_CHAOS, dir, dam, 4);
		}
		break;
	case 32: //RF4_BR_DISI
		{
			int typ = GF_DISINTEGRATE;

			dam = cast_hp / 6;
			if(dam>150) dam=150;
			if(only_info) return format("�����F%d",dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 33: //RF5_BA_ACID
		{
			int rad;
			if(powerful)
			{
				rad = 4;
				base = cast_lev * 4 + 50;
				dice = 10;
				sides = 10;
			}
			else
			{
				rad = 2;
				base = 15;
				dice = 1;
				sides = cast_lev * 3;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_ACID, dir, dam, 4);
		}
		break;

	case 34: //RF5_BA_ELEC
		{
			int rad;
			if(powerful)
			{
				rad = 4;
				base = cast_lev * 4 + 50;
				dice = 10;
				sides = 10;
			}
			else
			{
				rad = 2;
				base = 8;
				dice = 1;
				sides = cast_lev * 3 / 2;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_ELEC, dir, dam, 4);
		}
		break;

	case 35: //RF5_BA_FIRE
		{
			int rad;
			if(powerful)
			{
				rad = 4;
				base = cast_lev * 4 + 50;
				dice = 10;
				sides = 10;
			}
			else
			{
				rad = 2;
				base = 10;
				dice = 1;
				sides = cast_lev * 7 / 2;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_FIRE, dir, dam, 4);
		}
		break;
	case 36: //RF5_BA_COLD
		{
			int rad;
			if(powerful)
			{
				rad = 4;
				base = cast_lev * 4 + 50;
				dice = 10;
				sides = 10;
			}
			else
			{
				rad = 2;
				base = 10;
				dice = 1;
				sides = cast_lev * 3 / 2;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_COLD, dir, dam, 4);
		}
		break;
	case 37: //RF5_BA_POIS
		{
			int rad = 2;
			if(powerful)
			{
				dice = 24;
				sides = 2;
			}
			else
			{
				dice = 12;
				sides = 2;
			}

			if(only_info) return format("�����F%dd%d",dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_POIS, dir, dam, 4);
		}
		break;
	case 38: //RF5_BA_NETH
		{
			int rad = 2;
			if(powerful)
			{
				base = 50 + cast_lev * 2;
				dice = 10;
				sides = 10;
			}
			else
			{
				base = 50 + cast_lev ;
				dice = 10;
				sides = 10;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_NETHER, dir, dam, 4);
		}
		break;
	case 39: //RF5_BA_WATE
		{
			int rad = 4;
			if(powerful)
			{
				base = 50;
				dice = 1;
				sides = cast_lev * 3;
			}
			else
			{
				base = 50;
				dice = 1;
				sides = cast_lev * 2;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_WATER, dir, dam, 4);
		}
		break;

	case 40: //RF5_BA_MANA
	case 41: //RF5_BA_DARK
	case 53: //RF5_BA_LITE
		{
			int typ;
			int rad = 4;

			base = 50 + cast_lev * 4;
			dice = 10;
			sides = 10;

			if(num==40) typ = GF_MANA;
			else if(num==41) typ = GF_DARK;
			else if(num==53) typ = GF_LITE;
			else {msg_print("ERROR:typ"); return NULL;}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, rad);
		}
		break;
	case 42: //RF5_DRAIN_MANA
		{
		//v1.1.82 �e�������Ǝ������A�������狭���������nerf
		//�E�З͔���
		//�EGF_DRAIN_MANA����GF_PSI_DRAIN��
		//PSI_DRAIN�̒�R�������F�X����

			base = MAX(1,cast_lev/2);
			dice = 1;
			sides = MAX(2,cast_lev/2);

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);

			dam = damroll(dice, sides) + base;
			//fire_ball_hide(GF_DRAIN_MANA, dir, dam, 0);
			fire_ball_hide(GF_PSI_DRAIN, dir, dam, 0);

		}
		break;
	case 43: //RF5_MIND_BLAST
	case 44: //RF5_BRAIN_SMASH
	case 45: //RF5_CAUSE_1
	case 46: //RF5_CAUSE_2
	case 47: //RF5_CAUSE_3
	case 48: //RF5_CAUSE_4
	case 107: //RF9_PUNISH_1
	case 108: //RF9_PUNISH_2
	case 109: //RF9_PUNISH_3
	case 110: //RF9_PUNISH_4
		{
			int typ;
			if(num==43)
			{
				dice = 7;
				sides = 7;
				typ = GF_MIND_BLAST;
			}
			else if(num==44)
			{
				dice = 12;
				sides = 12;
				typ = GF_BRAIN_SMASH;
			}
			else if(num==45)
			{
				dice = 3;
				sides = 8;
				typ = GF_CAUSE_1;
			}
			else if(num==46)
			{
				dice = 8;
				sides = 8;
				typ = GF_CAUSE_2;
			}
			else if(num==47)
			{
				dice = 10;
				sides = 15;
				typ = GF_CAUSE_3;
			}
			else if(num==48)
			{
				dice = 15;
				sides = 15;
				typ = GF_CAUSE_4;
			}
			else if(num==107)
			{
				dice = 3;
				sides = 8;
				typ = GF_PUNISH_1;
			}
			else if(num==108)
			{
				dice = 8;
				sides = 8;
				typ = GF_PUNISH_2;
			}
			else if(num==109)
			{
				dice = 10;
				sides = 15;
				typ = GF_PUNISH_3;
			}
			else if(num==110)
			{
				dice = 15;
				sides = 15;
				typ = GF_PUNISH_4;
			}
			else {msg_print("ERROR:typ"); return NULL;}

			if(only_info) return format("�����F%dd%d",dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);

			fire_ball_hide(typ, dir, dam, 0);
		}
		break;
	case 49: //RF5_BO_ACID
	case 50: //RF5_BO_ELEC
	case 51: //RF5_BO_FIRE
	case 52: //RF5_BO_COLD
	case 54: //RF5_BO_NETH
	case 55: //RF5_BO_WATE
	case 56: //RF5_BO_MANA
	case 57: //RF5_BO_PLAS
	case 58: //RF5_BO_ICEE
	case 59: //RF5_MISSILE
	case 111: //RF9_BO_HOLY
	case 113: //RF9_BO_SOUND
		{
			int typ;

			if(num==49)
			{
				dice = 7;
				sides = 8;
				base = cast_lev / 3;
				if(powerful) base *= 2;
				typ = GF_ACID;
			}
			else if(num==50)
			{
				dice = 4;
				sides = 8;
				base = cast_lev / 3;
				if(powerful) base *= 2;
				typ = GF_ELEC;
			}
			else if(num==51)
			{
				dice = 9;
				sides = 8;
				base = cast_lev / 3;
				if(powerful) base *= 2;
				typ = GF_FIRE;
			}
			else if(num==52)
			{
				dice = 6;
				sides = 8;
				base = cast_lev / 3;
				if(powerful) base *= 2;
				typ = GF_COLD;
			}
			else if(num==54)
			{
				dice = 5;
				sides = 5;
				base = cast_lev * 4;
				if(powerful) base /= 2;
				else base /= 3;
				typ = GF_NETHER;
			}
			else if(num==55)
			{
				dice = 10;
				sides = 10;
				base = cast_lev * 3;
				if(powerful) base /= 2;
				else base /= 3;
				typ = GF_WATER;
			}
			else if(num==56)
			{
				dice = 1;
				sides = cast_lev * 7 / 2;
				base = 50;
				typ = GF_MANA;
			}
			else if(num==57)
			{
				dice = 1;
				sides = 200;
				if(powerful) base = 100 + cast_lev * 6;
				else  base = 100 + cast_lev * 3;
				typ = GF_PLASMA;
			}
			else if(num==58)
			{
				dice = 6;
				sides = 6;
				base = cast_lev * 3;
				if(powerful) base /= 2;
				else  base /= 3;
				typ = GF_ICE;
			}
			else if(num==59)
			{
				dice = 2;
				sides = 6;
				base = cast_lev / 3 + 1;
				typ = GF_MISSILE;
			}
			else if(num==111)
			{
				dice = 10;
				sides = 10;
				if(powerful) base = cast_lev * 3 / 2;
				else base = cast_lev;
				typ = GF_HOLY_FIRE;
			}
			else if(num==113)
			{
				dice = 3;
				if(powerful) dice = 10;
				sides = 10;
				if(powerful) base = cast_lev * 3 / 2;
				else base = cast_lev;
				typ = GF_SOUND;
			}


			else {msg_print("ERROR:typ"); return NULL;}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_bolt(typ, dir, dam);
		}
		break;

	case 60: //RF5_SCARE
	case 61: //RF5_BLIND
	case 62: //RF5_CONF
	case 63: //RF5_SLOW
	case 64: //RF5_HOLD
	case 79: //RF6_FORGET
		{
			int power;

			if(num == 60 || num == 63 || num == 64) power = cast_lev;
			else power = cast_lev * 2;

			if(only_info) return format("���́F%d",power);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);

			if(num==60) fear_monster(dir, power);
			else if(num==61) confuse_monster(dir, power);
			else if(num==62) confuse_monster(dir, power);
			else if(num==63) slow_monster(dir, power);
			else if(num==64) sleep_monster(dir, power);
			else fire_ball_hide(GF_GENOCIDE, dir, power, 0);

		}
		break;
	case 65: //RF6_HASTE
		{
			int time = 100;

			if(only_info) return format("���ԁF%d",time);
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);

			(void)set_fast(time, FALSE);
		}
		break;
	case 66: //RF6_HAND_DOOM
		{
			int power = cast_lev * 3;

			if(only_info) return format("���́F%d",power);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_hide(GF_HAND_DOOM, dir, power, 0);


		}
		break;
	case 67: //RF6_HEAL
		{
			int power = cast_lev * 6;

			if(only_info) return format("�񕜁F%d",power);
			if(fail) return "";

			msg_cast_monspell_new(num, msg, xtra);
			(void)hp_player(power);
			(void)set_stun(0);
			(void)set_cut(0);
		}
		break;
	case 68: //RF6_INVULNER
		{
			base = 4;

			if(only_info) return format("���ԁF%d+1d%d",base,base);
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);

			(void)set_invuln(randint1(base) + base, FALSE);
		}
		break;
	case 69: //RF6_BLINK
	case 70: //RF6_TPORT
		{
			int dist;

			if(num==69) dist = 10;
			else dist = MAX_SIGHT * 2 + 5;

			if(only_info) return format("�����F%d",dist);
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);
			teleport_player(dist, 0L);

		}
		break;
	case 71: //RF6_WORLD
		{

			if(only_info) return format(" ");
			if(fail) return "";
			if (world_player)
			{
				msg_print("���Ɏ��͎~�܂��Ă���B");
				return NULL;
			}
			else
			{

				msg_cast_monspell_new(num, msg, xtra);
				//v1.1.58
				flag_update_floor_music = TRUE;
				world_player = TRUE;
				/* Hack */
				p_ptr->energy_need -= 1000 + (400)*TURNS_PER_TICK/10;
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
				handle_stuff();

			}
		}
		break;
	case 72: //RF6_SPECIAL
		{

			msg_format("ERROR:���̋Z�͌Ă΂�Ȃ��͂��FRF6_SPECIAL");
			return NULL;
		}
		break;
	case 73: //RF6_TELE_TO
		{
			int idx_dummy;
			if(only_info) return format(" ");
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);
			if(!teleport_back(&idx_dummy, 0L)) return NULL;

		}
		break;
	case 74: //RF6_TELE_AWAY
		{
			int dist = 100;
			if(only_info) return format("�����F%d",dist);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return FALSE;
			msg_cast_monspell_new(num, msg, xtra);
			(void)fire_beam(GF_AWAY_ALL, dir, dist);


		}
		break;
	case 75: //RF6_TELE_LEVEL?
		{

			if(only_info) return format("������");
			if(fail) return "";
			msg_format("ERROR:���̋Z�͖������̂͂��FRF6_TELE_LEVEL");
			return NULL;
		}
		break;
	case 76: //RF5_PSY_SPEAR
		{
			if(powerful)
			{
				base = 150;
				dice = 1;
				sides = cast_lev * 2;
			}
			else
			{
				base = 100;
				dice = 1;
				sides = cast_lev * 3 / 2;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			(void)fire_beam(GF_PSY_SPEAR, dir, dam);
		}
		break;
	case 77: //RF6_DARKNESS�@�����C�g�E�G���A�ɂ���
		{
			if(only_info) return format("���ʁF�Ɩ�");
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);
			(void)lite_area(0, 3);
		}
		break;
	case 78: //RF6_TRAP
		{
			if(only_info) return format("");
			if(fail) return "";
			if (!target_set(TARGET_KILL)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			trap_creation(target_row, target_col);
		}
		break;
	case 80: //RF6_COSMIC_HORROR
		{

			base = 50 + cast_lev * 4;
			dice = 10;
			sides = 10;

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_hide(GF_COSMIC_HORROR, dir, dam, 0);
		}
		break;
	case 81: //RF6_S_KIN
	case 82: 
	case 83: 
	case 84: 
	case 85: 
	case 86: 
	case 87: 
	case 88: 
	case 89: 
	case 90: //RF6_S_DEMON
	case 91: 
	case 92: 
	case 93: 
	case 94: 
	case 95: 
	case 96:
	case 114: //RF9_S_ANIMAL
	case 122: //RF9_S_DEITY
	case 123: //RF9_S_HI_DEMON
	case 124: //RF9_S_KWAI
		{
			int mode;
			bool summon = FALSE;
			int cnt;
			if(only_info) return format("");

			if(fail) mode = (PM_NO_PET | PM_ALLOW_GROUP);
			else mode = PM_FORCE_PET;

			if(!fail) msg_cast_monspell_new(num, msg, xtra);

			if(num == 81) summon = summon_kin_player(cast_lev, py, px, mode);
			if(num == 82) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_CYBER, mode);
			if(num == 83) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, 0, mode);
			if(num == 84) for(cnt=0;cnt<6;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, 0, mode)) summon = TRUE;
			if(num == 85) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANT, (mode | PM_ALLOW_GROUP));
			if(num == 86)  for(cnt=0;cnt<6;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_SPIDER, mode | PM_ALLOW_GROUP)) summon = TRUE;
			if(num == 87) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HOUND, (mode | PM_ALLOW_GROUP));
			if(num == 88) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HYDRA, (mode));
			if(num == 89) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANGEL, (mode));
			if(num == 90) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DEMON, (mode));
			if(num == 91) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_UNDEAD, (mode));
			if(num == 92) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DRAGON, (mode));
			if(num == 93) for(cnt=0;cnt<6;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_UNDEAD, mode)) summon = TRUE;
			if(num == 94) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_DRAGON, mode)) summon = TRUE;
			if(num == 95) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_AMBERITES, (mode | PM_ALLOW_UNIQUE))) summon = TRUE;
			if(num == 96) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_UNIQUE, (mode | PM_ALLOW_UNIQUE | PM_ALLOW_GROUP))) summon = TRUE;
			if(num == 114) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANIMAL, (mode | PM_ALLOW_GROUP))) summon = TRUE;
			if(num == 122) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DEITY, (mode | PM_ALLOW_GROUP))) summon = TRUE;
			if(num == 123) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_DEMON, (mode | PM_ALLOW_GROUP))) summon = TRUE;
			if(num == 124) for(cnt=0;cnt<6;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_KWAI, (mode | PM_ALLOW_GROUP))) summon = TRUE;



			if(summon && fail) msg_format("�������������X�^�[���P���������Ă����I"); 
			else if(!summon) msg_format("��������������Ȃ������B");

		}
		break;
	case 97: //RF9_FIRE_STORM
	case 98: //RF9_ICE_STORM
	case 99: //RF9_THUNDER_STORM
	case 100: //RF9_ACID_STORM

		{
			int rad = 4;
			int typ;

			if(num==97) typ = GF_FIRE;
			if(num==98) typ = GF_COLD;
			if(num==99) typ = GF_ELEC;
			if(num==100) typ = GF_ACID;

			if(powerful)
			{
				base = 100 + cast_lev * 10;
				dice = 1;
				sides = 100;
			}
			else
			{
				base = 50 + cast_lev * 7;
				dice = 1;
				sides = 50;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, rad);
		}
		break;
	case 101: //RF9_TOXIC_STORM
		{
			int rad = 4;
			int typ;

			if(powerful)
			{
				base = 50 + cast_lev * 8;
				dice = 1;
				sides = 50;
			}
			else
			{
				base = 25 + cast_lev * 5;
				dice = 1;
				sides = 25;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_POIS, dir, dam, rad);
		}
		break;
	case 102: //RF9_BA_POLLUTE
		{
			int rad = 4;

			if(powerful)
			{
				base = 50 + cast_lev * 4;
				dice = 10;
				sides = 10;
			}
			else
			{
				base = 50 + cast_lev * 3;
				dice = 1;
				sides = 50;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_POLLUTE, dir, dam, rad);
		}
		break;
	case 103: //RF9_BA_DISI
		{
			int rad = 5;

			if(powerful)
			{
				base = 50 + cast_lev * 2;
				dice = 1;
				sides = 50;
			}
			else
			{
				base = 25 + cast_lev ;
				dice = 1;
				sides = 25;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_DISINTEGRATE, dir, dam, rad);
		}
		break;
	case 104: //RF9_BA_HOLY
		{
			int rad = 2;

			if(powerful)
			{
				base = 50 + cast_lev * 2;
				dice = 10;
				sides = 10;
			}
			else
			{
				base = 50 + cast_lev;
				dice = 10;
				sides = 10;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_HOLY_FIRE, dir, dam, rad);
		}
		break;
	case 105: //RF9_METEOR
		{
			int rad;

			if(powerful)
			{
				rad = 5;
				base = cast_lev * 4;
				dice = 1;
				sides = 300;
			}
			else
			{
				rad = 4;
				base = cast_lev * 2;
				dice = 1;
				sides = 150;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_jump(GF_METEOR, dir, dam, rad,NULL);
		}
		break;
	case 106: //RF9_DISTORTION
		{
			int rad = 4;

			if(powerful)
			{
				base = 50 + cast_lev * 3;
				dice = 10;
				sides = 10;
			}
			else
			{
				base = 50 + cast_lev * 3 / 2;
				dice = 10;
				sides = 10;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_jump(GF_DISTORTION, dir, dam, rad,NULL);
		}
		break;
	case 112: //RF9_GIGANTIC_LASER
		{
			int rad;

			if(powerful)
			{
				rad = 2;
				base = cast_lev * 8;
				dice = 10;
				sides = 10;
			}
			else
			{
				rad = 1;
				base = cast_lev * 4;
				dice = 10;
				sides = 10;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_spark(GF_NUKE, dir, dam, rad);
		}
		break;

	case 115: //RF9_BEAM_LITE
		{
			if(powerful)
				base = cast_lev * 4;
			else
				base = cast_lev * 2;

			if(only_info) return format("�����F%d",base);
			if(fail) return "";
			dam =  base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			(void)fire_beam(GF_LITE, dir, dam);
		}
		break;


	case 116: 
		{

			if(only_info) return format("������");
			if(fail) return "";
			msg_format("ERROR:���̋Z�͖������̂͂��FRF9_");
			return NULL;
		}
		break;

	case 117: //RF9_HELL_FIRE
	case 118: //RF9_HOLY_FIRE
		{
			int rad = 5;
			int typ;
			if(num == 117) typ = GF_HELL_FIRE;
			else typ = GF_HOLY_FIRE;

			if(powerful)
			{
				base = 100 + cast_lev * 4;
				dice = 10;
				sides = 10;
			}
			else
			{
				base = 50 + cast_lev * 3;
				dice = 10;
				sides = 5;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, rad);
		}
		break;
	case 119: //RF9_FINAL_SPARK
		{
			base = cast_lev * 2 + 200;
			dice = 1;
			sides = cast_lev;

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_spark(GF_DISINTEGRATE, dir, dam, 2);
		}
		break;
	case 120: //RF9_TORNADO
		{
			int rad = 5;

			if(powerful)
			{
				base = 50 + cast_lev * 3;
				dice = 1;
				sides = 50;
			}
			else
			{
				base = 25 + cast_lev * 2;
				dice = 1;
				sides = 25;
			}

			if(only_info) return format("�����F%d+%dd%d",base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_jump(GF_TORNADO, dir, dam, rad,NULL);
		}
		break;
	case 121: //RF9_DESTRUCTION
		{
			base = 7;
			sides = 5;
			if(only_info) return format("�͈́F%d+1d%d",base,sides);
			if(fail) return "";

			msg_cast_monspell_new(num, msg, xtra);
			destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);

		}
		break;
	case 125: //RF9_TELE_APPROACH
		{
			if(only_info) return format("");
			if(fail) return "";
			if (!target_set(TARGET_KILL)) return NULL;
			if (!target_okay()|| !projectable(target_row,target_col,py,px))
			{
				msg_format("���E���̃^�[�Q�b�g���w�肳��Ă��Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			msg_cast_monspell_new(num, msg, xtra);

			teleport_player_to(target_row, target_col, 0L);
		}
		break;
	case 126: //RF9_TELE_HI_APPROACH
		{
			if(only_info) return format("");
			if(fail) return "";

			if(target_who > 0 && m_list[target_who].r_idx)
			{
				msg_cast_monspell_new(num, msg, xtra);
				teleport_player_to(m_list[target_who].fy, m_list[target_who].fx, 0L);
			}
			else
			{
				msg_format("�^�[�Q�b�g�����X�^�[���w�肳��Ă��Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}

		}
		break;

	case 127: //RF9_MAELSTROM
		{
			int rad;

			if(powerful)
			{
				rad = 8;
				base = cast_lev * 8;
				dice = 1;
				sides = cast_lev * 6;
			}
			else
			{
				rad = 6;
				base = cast_lev * 5;
				dice = 1;
				sides = cast_lev * 3;
			}

			if(only_info) return format("�����F�`%d+%dd%d",base/2,dice,sides/2);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			msg_cast_monspell_new(num, msg, xtra);

			(void)project(0, rad, py, px, dam, GF_WATER, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP), FALSE);
			(void)project(0, rad, py, px, rad, GF_WATER_FLOW, (PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP | PROJECT_HIDE), FALSE);

		}
		break;

	default:
		{
			if(only_info) return format("������");
			msg_format("ERROR:�������Ă��Ȃ��G���@���Ă΂ꂽ num:%d",num);
			return NULL;
		}
	}







	return "";
}





/*:::���̂܂ˁA���n�E�Ƃ̋L���\�Ȗ��@�̐�*/
int calc_monspell_cast_mem(void)
{
	int num=0;
	//�����X�^�[�ϐg�n
	if (IS_METAMORPHOSIS) num = 20;
	//���Ƃ�
	else if(p_ptr->pclass == CLASS_SATORI) num = 3 + p_ptr->lev / 7;
	//������
	else if(p_ptr->pclass == CLASS_KOKORO) num = 2 + (p_ptr->lev-1) / 13;
	//����
	else if(p_ptr->pclass == CLASS_RAIKO) num = 20;
	//�B���
	else if (p_ptr->pclass == CLASS_OKINA) num = NEW_MSPELL_LIST_MAX;
	//�f�t�H20
	else num = 20;

	return num;
}

/*:::���Ƃ�p���̂܂ˏ������[�`�� ���E���̓G�𑖍����Amonspell_list2[]�p�̓Y�������ő�10�܂�p_ptr->magic_num1[]�Ɋi�[����*/
void make_magic_list_satori(bool del)
{
	int i,shift;
	int mlist_num = 0;
	int memory;

	if(p_ptr->pclass != CLASS_SATORI) return;

	memory = calc_monspell_cast_mem();

	for(i=0;i<10;i++) p_ptr->magic_num1[i] = 0; //��ɍő��10���N���A
	if(del) return;

	//�S�����X�^�[����
	for (i = 1; i < m_max; i++)
	{
		u32b rflags;
		int j;
		int new_num;
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		if (!m_ptr->r_idx) continue;
		//���E���̐��_�̂���G�𒊏o

		if(!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;
		if(!m_ptr->ml) continue;
		if(r_ptr->flags2 & RF2_EMPTY_MIND) continue;

		//WEIRD_MIND�̓G��1/2�̊m���ł����ǂ߂Ȃ�
		if((r_ptr->flags2 & RF2_WEIRD_MIND) && (turn % 2)) continue;

		//�G�̎����Ă��閂�@�𒊏o��monspell_list2[]�̓Y���ԍ��ɕϊ�����magic_num1[]�ɒǉ��@�L���\���𒴂�����K���ɓ���ւ���
		for(j=0;j<4;j++)
		{
			int spell;
			if(j==0) rflags = r_ptr->flags4;
			else if(j==1) rflags = r_ptr->flags5;
			else if(j==2) rflags = r_ptr->flags6;
			else rflags = r_ptr->flags9;

			for(shift=0;shift<32;shift++)
			{
				bool exist = FALSE;
				int k;
				if((rflags >> shift) % 2 == 0) continue;
				spell = shift+1 + j * 32;

				if(spell < 1 || spell > 128)
				{
					msg_format("ERROR:make_magic_satori()�ŕs����spell�l(%d)���o��",spell);
					return;
				}
				if(!monspell_list2[spell].level) continue; //���ʂȍs���▢�����̋Z�͔�Ώ�


				//���ɂ��閂�@�̓p�X
				for(k=0;k<10;k++) if(spell == p_ptr->magic_num1[k]) exist = TRUE;
				if(exist) continue;


				if(mlist_num < memory) new_num = mlist_num;
				else
				{
					new_num = randint0(mlist_num+1);
					if(new_num >=memory) continue;
				}
				p_ptr->magic_num1[new_num] = spell;
				mlist_num++;
			}
		}
	}

}



/*:::
*���T�p���̂܂ˏ������[�`���B�R�撆�����X�^�[�̖��@��monspell_list2[]�p�̓Y�����Ƃ��čő�20�܂�p_ptr->magic_num1[]�Ɋi�[����B
*���̃��[�`���͋R�揈���̂��тɌĂ΂��B
*/
void make_magic_list_satono(void)
{
	int i, j, new_num;
	int mlist_num = 0;
	int memory;
	monster_type *m_ptr;
	monster_race *r_ptr;
	u32b rlags;

	if (p_ptr->pclass != CLASS_SATONO) return;

	memory = calc_monspell_cast_mem();
	for (i = 0; i<memory; i++) p_ptr->magic_num1[i] = 0; //���X�g�̃N���A

	if (!p_ptr->riding) return;
	m_ptr = &m_list[p_ptr->riding];
	if (!m_ptr->r_idx) { msg_format("ERROR:riding�̒l����������(%d)", p_ptr->riding); return; }
	r_ptr = &r_info[m_ptr->r_idx];

	//�����X�^�[�̎����Ă��閂�@�𒊏o��monspell_list2[]�̓Y���ԍ��ɕϊ�����magic_num1[]�ɒǉ�����B�L���\���𒴂�����K���ɓ���ւ���
	for (j = 0; j<4; j++)
	{
		u32b rflags;
		int spell, shift;
		//���@�݂̂�Ώۂɂ���
		if (j == 0) rflags = r_ptr->flags4 & ~(RF4_NOMAGIC_MASK);
		else if (j == 1) rflags = r_ptr->flags5 & ~(RF5_NOMAGIC_MASK);
		else if (j == 2) rflags = r_ptr->flags6 & ~(RF6_NOMAGIC_MASK);
		else rflags = r_ptr->flags9 & ~(RF9_NOMAGIC_MASK);

		for (shift = 0; shift<32; shift++)
		{
			int k;
			if ((rflags >> shift) % 2 == 0) continue;
			spell = shift + 1 + j * 32;

			if (spell < 1 || spell > 128)
			{
				msg_format("ERROR:make_magic_satono()�ŕs����spell�l(%d)���o��", spell);
				return;
			}
			if (!monspell_list2[spell].level) continue; //���ʂȍs���▢�����̋Z�͔�Ώ�

			if (mlist_num < memory) new_num = mlist_num;
			else
			{
				new_num = randint0(mlist_num + 1);
				if (new_num >= memory) continue;
			}
			p_ptr->magic_num1[new_num] = spell;
			mlist_num++;
		}
	}

}

/*:::���ۖ��@���X�g�쐬�@����Е�����10�A���v20�܂Ł@���@�����v��Ԃ�*/
//p_ptr->magic_num1[10]-[29]�𖂖@���X�g�Ɏg���B[30]�ɂ́u��ڂ̖��@���炪2�{�ڂ̕��킩�v���L�^�����B
int make_magic_list_aux_raiko(void)
{
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	int hand;
	int power, rank;
	int magic_num = 10;
	int i;

	for(i=10;i<30;i++) p_ptr->magic_num1[i] = 0; //���@���X�g��20�N���A
	p_ptr->magic_num1[30] = 0;

	//���胋�[�v
	for(hand = 0;hand < 2;hand++)
	{
		int magic_num_one = 0; //���̕��킩�瓾�����@�̐� 10�ɂȂ�����I��
		if(hand == 1) p_ptr->magic_num1[30] = (magic_num-10);

		o_ptr = &inventory[INVEN_RARM+hand];

		//�ʏ핐�킩�����_���A�[�e�B�t�@�N�g�̂�
		if(!o_ptr->k_idx || !object_is_melee_weapon(o_ptr) || object_is_fixed_artifact(o_ptr)) continue;
		if(o_ptr->xtra3 < 50) continue;
		object_flags(o_ptr, flgs);

		//����̃����N���ܒi�K�Ɍ��߂�
		power = k_info[o_ptr->k_idx].level;
		if(object_is_artifact(o_ptr)) power += 50;
		else if(object_is_ego(o_ptr)) power += e_info[o_ptr->name2].rarity;//e_info.level�͋@�\���Ă��Ȃ��̂�rarity���g��
		power += flag_cost(o_ptr,o_ptr->pval,FALSE) / 1000;

		if(power < 50) rank=0;
		else if(power < 100) rank=1;
		else if(power < 150) rank = 2;
		else rank = 3;

		if(o_ptr->xtra1 == 100) rank += 1;//�u���S�Ɏg�����Ȃ���v���̂�1�����N�A�b�v

	
		//�Ȍ�A����̃t���O�ƃ����N�ɂ���Ďg�p�\�Ȗ��@�𔻒肵p_ptr->magic_num1[11-29]�Ɋi�[�@����Е����Ƃɍő�10�@
		//�i�[�����l��monspell_list2[]�̓Y��
	
		//�e���͂Ȃ��H
		//�g���C
		if((have_flag(flgs, TR_VORPAL) || have_flag(flgs, TR_EX_VORPAL)) && have_flag(flgs, TR_FORCE_WEAPON) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 17; 
			if(++magic_num_one == 10) continue;
		}

		//���̗͂�
		if((o_ptr->name2 == EGO_WEAPON_HERO || have_flag(flgs, TR_INT) && have_flag(flgs, TR_WIS) || have_flag(flgs, TR_EASY_SPELL)) && rank > 3 
		|| (o_ptr->name2 == EGO_WEAPON_FORCE && rank > 2))
		{
			p_ptr->magic_num1[magic_num++] = 40; 
			if(++magic_num_one == 10) continue;
		}
		//�Í��̗�
		if((o_ptr->name2 == EGO_WEAPON_VAMP || have_flag(flgs, TR_RES_DARK) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) ) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 41; 
			if(++magic_num_one == 10) continue;
		}
		//�M���̗�
		if((o_ptr->name2 == EGO_WEAPON_HAKUREI || have_flag(flgs, TR_RES_LITE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) ) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 53; 
			if(++magic_num_one == 10) continue;
		}
		//�w���t�@�C�A
		if(have_flag(flgs, TR_KILL_GOOD) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 117; 
			if(++magic_num_one == 10) continue;
		}
		//�z�[���[�t�@�C�A
		if(have_flag(flgs, TR_KILL_EVIL) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 118; 
			if(++magic_num_one == 10) continue;
		}

		//�����̋�
		if(have_flag(flgs, TR_RES_DISEN) && have_flag(flgs, TR_RES_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 102; 
			if(++magic_num_one == 10) continue;
		}

		//���q����
		if((have_flag(flgs, TR_VORPAL) && rank > 3 || have_flag(flgs, TR_EX_VORPAL))&& (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) )
		{
			p_ptr->magic_num1[magic_num++] = 103; 
			if(++magic_num_one == 10) continue;
		}
		//�j�ׂ̌���
		if((have_flag(flgs, TR_SLAY_EVIL) &&  (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2 )
		|| o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 104;
			if(++magic_num_one == 10) continue;
		}
		//���e�I�X�g���C�N
		if(have_flag(flgs, TR_IMPACT) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 105; 
			if(++magic_num_one == 10) continue;
		}
		//��Ԙc��
		if((have_flag(flgs, TR_TELEPORT) &&  (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2 )
		|| (have_flag(flgs, TR_RES_TIME) &&  (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1 ))
		{
			p_ptr->magic_num1[magic_num++] = 106; 
			if(++magic_num_one == 10) continue;
		}	
		//���C���V���g����
		if(o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank > 2 ||
		have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 127; 
			if(++magic_num_one == 10) continue;
		}

		//���[�U�[
		if(have_flag(flgs, TR_RES_LITE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1 
		|| have_flag(flgs, TR_VORPAL) && rank > 2 ||  have_flag(flgs, TR_EX_VORPAL))
		{
			p_ptr->magic_num1[magic_num++] = 115; 
			if(++magic_num_one == 10) continue;
		}
		//��^���[�U�[
		if((have_flag(flgs, TR_RES_LITE) && have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2 )
		|| have_flag(flgs, TR_IM_FIRE) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 112; 
			if(++magic_num_one == 10) continue;
		}
		//���̌�
		if(have_flag(flgs, TR_FORCE_WEAPON)  && rank > 2
		 ||have_flag(flgs, TR_EX_VORPAL)  && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 76; 
			if(++magic_num_one == 10) continue;
		}

		//�C�e
		if((have_flag(flgs, TR_IMPACT) && have_flag(flgs, TR_CON) && rank > 2)
		  ||(have_flag(flgs, TR_STR) && have_flag(flgs, TR_CON) && rank > 2)
		  ||(have_flag(flgs, TR_IMPACT) && have_flag(flgs, TR_STR) && rank > 2)
		  ||(o_ptr->name2 == EGO_WEAPON_ONI && rank > 2 ))
		{
			p_ptr->magic_num1[magic_num++] = 27; 
			if(++magic_num_one == 10) continue;
		}


		//���͏���
		if(have_flag(flgs, TR_EX_VORPAL) || have_flag(flgs, TR_VORPAL) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 3; 
			if(++magic_num_one == 10) continue;
		}
		//���͋z��
		if(have_flag(flgs, TR_VAMPIRIC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_CHR)) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 42; 
			if(++magic_num_one == 10) continue;
		}
		//����
		if(o_ptr->name2 == EGO_WEAPON_EXATTACK && rank > 1
		|| o_ptr->name2 == EGO_WEAPON_HUNTER && rank > 2
		|| have_flag(flgs, TR_SPEEDSTER) && rank > 1
		|| have_flag(flgs, TR_SPEED) && o_ptr->pval > 0 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 65; 
			if(++magic_num_one == 10) continue;
		}
		//�j�ł̎�
		if(o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 0
		|| (o_ptr->curse_flags & TRC_PERMA_CURSE) 
		|| (o_ptr->curse_flags & TRC_ADD_H_CURSE ) && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 66; 
			if(++magic_num_one == 10) continue;
		}
		//����
		if((have_flag(flgs, TR_REGEN) && have_flag(flgs, TR_CON) && rank > 2)
		  ||(have_flag(flgs, TR_CON) && have_flag(flgs, TR_WIS) && rank > 3)
		  ||(have_flag(flgs, TR_REGEN) && have_flag(flgs, TR_WIS) && rank > 3)
		  ||(o_ptr->name2 == EGO_WEAPON_DEFENDER && rank > 2)
		  ||(o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 1))
		{
			p_ptr->magic_num1[magic_num++] = 67; 
			if(++magic_num_one == 10) continue;
		}
		//���G��
		if((o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 3)
		|| have_flag(flgs, TR_BLESSED) && o_ptr->to_a > 5 && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 68; 
			if(++magic_num_one == 10) continue;
		}
		//�V���[�g�E�e���|�[�g
		if(have_flag(flgs, TR_TELEPORT) || have_flag(flgs, TR_LEVITATION) || have_flag(flgs, TR_SPEEDSTER))
		{
			p_ptr->magic_num1[magic_num++] = 69; 
			if(++magic_num_one == 10) continue;
		}
		//�e���|�[�g
		if(have_flag(flgs, TR_TELEPORT) && rank > 0
		|| have_flag(flgs, TR_RES_CHAOS)
		|| have_flag(flgs, TR_RES_TIME) )
		{
			p_ptr->magic_num1[magic_num++] = 70; 
			if(++magic_num_one == 10) continue;
		}
		//�U�E���[���h
		if(have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_SPEEDSTER) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 71; 
			if(++magic_num_one == 10) continue;
		}
		//�e���|�o�b�N
		if(have_flag(flgs, TR_NO_TELE) && rank > 0 || have_flag(flgs, TR_RIDING) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 73; 
			if(++magic_num_one == 10) continue;
		}
		//�e���|�A�E�F�C
		if(have_flag(flgs, TR_NO_TELE) && rank > 1 || have_flag(flgs, TR_BLESSED) && rank > 3 || o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 74; 
			if(++magic_num_one == 10) continue;
		}
		//�אڃe���|
		if(o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 1 || have_flag(flgs, TR_SPEEDSTER) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 125; 
			if(++magic_num_one == 10) continue;
		}
		//���E�O�אڃe���|
		if(have_flag(flgs, TR_TELEPORT) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 126; 
			if(++magic_num_one == 10) continue;
		}
		//���C�g�G���A
		if(have_flag(flgs, TR_LITE) )
		{
			p_ptr->magic_num1[magic_num++] = 77; 
			if(++magic_num_one == 10) continue;
		}

		//*�j��*
		if(have_flag(flgs, TR_CHAOTIC) && rank > 3 ||  have_flag(flgs, TR_IMPACT) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 121; 
			if(++magic_num_one == 10) continue;
		}	
		//���̗�
		if(have_flag(flgs, TR_BRAND_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3 
		|| have_flag(flgs, TR_IM_FIRE) &&  rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 97; 
			if(++magic_num_one == 10) continue;
		}
		//�X�̗�
		if(have_flag(flgs, TR_BRAND_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3 
		|| have_flag(flgs, TR_IM_COLD) &&  rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 98; 
			if(++magic_num_one == 10) continue;
		}
		//���̗�
		if(have_flag(flgs, TR_BRAND_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3 
		|| have_flag(flgs, TR_IM_ELEC) &&  rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 99; 
			if(++magic_num_one == 10) continue;
		}
		//�_�̗�
		if(have_flag(flgs, TR_BRAND_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3 
		|| have_flag(flgs, TR_IM_ACID) &&  rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 100; 
			if(++magic_num_one == 10) continue;
		}
		//�őf�̗�
		if(have_flag(flgs, TR_BRAND_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 101; 
			if(++magic_num_one == 10) continue;
		}

		//�n����
		if(o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON && rank > 1 || 
		have_flag(flgs, TR_RES_NETHER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 1 )
		{
			p_ptr->magic_num1[magic_num++] = 38; 
			if(++magic_num_one == 10) continue;
		}
		//�E�H�[�^�[�E�{�[��
		if(o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank == 3 ||  o_ptr->name2 == EGO_WEAPON_RYU_JIN && rank >= 2
		|| have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 39; 
			if(++magic_num_one == 10) continue;
		}
		//�����O���X
		if(o_ptr->name2 == EGO_WEAPON_CHAOS && rank > 1 || 
		have_flag(flgs, TR_RES_CHAOS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 31; 
			if(++magic_num_one == 10) continue;
		}
		//����
		if(o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 120; 
			if(++magic_num_one == 10) continue;
		}

		//�n���̖�
		if(o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON || 
		have_flag(flgs, TR_RES_NETHER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 54; 
			if(++magic_num_one == 10) continue;
		}		
		//�E�H�[�^�[�E�{���g
		if(o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank >= 1 ||  o_ptr->name2 == EGO_WEAPON_RYU_JIN && rank == 1
		|| have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1 )
		{
			p_ptr->magic_num1[magic_num++] = 55; 
			if(++magic_num_one == 10) continue;
		}
		//���̖͂�
		if((o_ptr->name2 == EGO_WEAPON_HERO || o_ptr->name2 == EGO_WEAPON_FORCE || 
		have_flag(flgs, TR_INT) && have_flag(flgs, TR_WIS) || have_flag(flgs, TR_EASY_SPELL)) && rank > 1 ||
		have_flag(flgs, TR_THROW) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 56; 
			if(++magic_num_one == 10) continue;
		}
		//�v���Y�}�{���g
		if(o_ptr->name2 == EGO_WEAPON_BRANDELEM || have_flag(flgs, TR_IM_FIRE) || have_flag(flgs, TR_IM_ELEC) ||
		have_flag(flgs, TR_RES_FIRE) && have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 1 )
		{
			p_ptr->magic_num1[magic_num++] = 57; 
			if(++magic_num_one == 10) continue;
		}		
		//�Ɋ��̖�
		if(o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 1 || have_flag(flgs, TR_IM_COLD) || 
		have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 58; 
			if(++magic_num_one == 10) continue;
		}

		//���Ȃ��
		if(o_ptr->name2 == EGO_WEAPON_HAKUREI  || have_flag(flgs, TR_RES_HOLY) && rank > 2 && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) )
		{
			p_ptr->magic_num1[magic_num++] = 111; 
			if(++magic_num_one == 10) continue;
		}

		//��̃{���g
		if(have_flag(flgs, TR_RES_SOUND) && have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1 
		|| have_flag(flgs, TR_AGGRAVATE) && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 113; 
			if(++magic_num_one == 10) continue;
		}
		//���_�U��
		if(have_flag(flgs, TR_CHAOTIC) && rank > 0 && rank < 3 )
		{
			p_ptr->magic_num1[magic_num++] = 43; 
			if(++magic_num_one == 10) continue;
		}
		//�]�U��
		if(have_flag(flgs, TR_CHAOTIC) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 44; 
			if(++magic_num_one == 10) continue;
		}
		//�y���̎�
		if(o_ptr->name2 == EGO_WEAPON_GHOST)
		{
			p_ptr->magic_num1[magic_num++] = 45; 
			if(++magic_num_one == 10) continue;
		}
		//�d���̎�
		if(o_ptr->name2 == EGO_WEAPON_GHOST)
		{
			p_ptr->magic_num1[magic_num++] = 46; 
			if(++magic_num_one == 10) continue;
		}
		//�v�����̎�
		if((o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 47; 
			if(++magic_num_one == 10) continue;
		}
		//���̌���
		if((o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON || o_ptr->name2 == EGO_WEAPON_MURDERER) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 48; 
			if(++magic_num_one == 10) continue;
		}
		//�R�Y�~�b�N�E�z���[
		if(have_flag(flgs, TR_RES_INSANITY) && rank > 3 || have_flag(flgs, TR_CHR) && o_ptr->pval < -4 )
		{
			p_ptr->magic_num1[magic_num++] = 80; 
			if(++magic_num_one == 10) continue;
		}


		//�������̂܂��Ȃ�
		if((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 1)
		{
			p_ptr->magic_num1[magic_num++] = 107; 
			if(++magic_num_one == 10) continue;
		}
		//���Ȃ錾�t
		if((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 108; 
			if(++magic_num_one == 10) continue;
		}
		//�ޖ��̎���
		if((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 3)
		{
			p_ptr->magic_num1[magic_num++] = 109; 
			if(++magic_num_one == 10) continue;
		}
		//�j�ׂ̈�
		if((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank > 3
		|| o_ptr->name2 == EGO_WEAPON_HAKUREI && rank < 3 || have_flag(flgs, TR_RES_HOLY) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 110; 
			if(++magic_num_one == 10) continue;
		}

		//�A�V�b�h�E�{�[��
		if(o_ptr->name2 == EGO_WEAPON_BRANDACID && rank >= 2  || 
		have_flag(flgs, TR_RES_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2 )
		{
			p_ptr->magic_num1[magic_num++] = 33; 
			if(++magic_num_one == 10) continue;
		}
		//�T���_�[�E�{�[��
		if(o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank >= 2  || 
		have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2 )
		{
			p_ptr->magic_num1[magic_num++] = 34; 
			if(++magic_num_one == 10) continue;
		}
		//�t�@�C�A�E�{�[��
		if(o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank >= 2  || 
		have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2 )
		{
			p_ptr->magic_num1[magic_num++] = 35; 
			if(++magic_num_one == 10) continue;
		}
		//�A�C�X�E�{�[��
		if(o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank >= 2  || 
		have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2 )
		{
			p_ptr->magic_num1[magic_num++] = 36; 
			if(++magic_num_one == 10) continue;
		}
		//�A�V�b�h�E�{���g
		if(o_ptr->name2 == EGO_WEAPON_BRANDACID && rank == 1  || 
		have_flag(flgs, TR_RES_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1 )
		{
			p_ptr->magic_num1[magic_num++] = 49; 
			if(++magic_num_one == 10) continue;
		}
		//�T���_�[�E�{���g
		if(o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank == 1  || 
		have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1 )
		{
			p_ptr->magic_num1[magic_num++] = 50; 
			if(++magic_num_one == 10) continue;
		}
		//�t�@�C�A�E�{���g
		if(o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank == 1 || 
		have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1 )
		{
			p_ptr->magic_num1[magic_num++] = 51; 
			if(++magic_num_one == 10) continue;
		}
		//�A�C�X�E�{���g
		if(o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank == 1  || 
		have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1 )
		{
			p_ptr->magic_num1[magic_num++] = 52; 
			if(++magic_num_one == 10) continue;
		}
		//���L�_
		if(o_ptr->name2 == EGO_WEAPON_BRANDPOIS && rank <= 2  || 
		have_flag(flgs, TR_RES_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank <= 2 )
		{
			p_ptr->magic_num1[magic_num++] = 37; 
			if(++magic_num_one == 10) continue;
		}


		//�~������
		if(have_flag(flgs, TR_CHR) && o_ptr->pval > 3 && rank > 3 || o_ptr->name2 == EGO_WEAPON_HERO && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 81; 
			if(++magic_num_one == 10) continue;
		}
		//�T�C�o�[U
		if(have_flag(flgs, TR_RES_SHARDS) && have_flag(flgs, TR_SLAY_DEMON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 82; 
			if(++magic_num_one == 10) continue;
		}
		//�����X�^�[��̏���
		if(have_flag(flgs, TR_CHR) && o_ptr->pval > 3 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 83; 
			if(++magic_num_one == 10) continue;
		}
		//�����X�^�[��������
		if(have_flag(flgs, TR_TELEPATHY) && have_flag(flgs, TR_TELEPORT))
		{
			p_ptr->magic_num1[magic_num++] = 84; 
			if(++magic_num_one == 10) continue;
		}
		//�A������
		if(o_ptr->name2 == EGO_WEAPON_HUNTER || have_flag(flgs, TR_SLAY_ANIMAL) && rank < 2)
		{
			p_ptr->magic_num1[magic_num++] = 85; 
			if(++magic_num_one == 10) continue;
		}
		//������
		if(o_ptr->name2 == EGO_WEAPON_HUNTER || have_flag(flgs, TR_SLAY_ANIMAL) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 86; 
			if(++magic_num_one == 10) continue;
		}
		//�n�E���h����
		if((have_flag(flgs, TR_SLAY_ANIMAL) || have_flag(flgs, TR_ESP_ANIMAL) )&& rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 87; 
			if(++magic_num_one == 10) continue;
		}
		//�q�h������
		if(have_flag(flgs, TR_KILL_ANIMAL) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 88; 
			if(++magic_num_one == 10) continue;
		}
		//�V�g����
		if(have_flag(flgs, TR_KILL_GOOD) && rank > 2 || have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_GOOD) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 89; 
			if(++magic_num_one == 10) continue;
		}
		//��������
		if(have_flag(flgs, TR_SLAY_DEMON) && rank > 2 || 
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEMON) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 90; 
			if(++magic_num_one == 10) continue;
		}
		//�A���f�b�h����
		if(have_flag(flgs, TR_SLAY_UNDEAD) && rank > 2 || 
		have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_UNDEAD) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 91; 
			if(++magic_num_one == 10) continue;
		}
		//�h���S������
		if(have_flag(flgs, TR_SLAY_DRAGON) && rank > 2 || 
		have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_DRAGON) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 92; 
			if(++magic_num_one == 10) continue;
		}
		//�㋉�A���f�b�h����
		if(have_flag(flgs, TR_KILL_UNDEAD) && rank > 3 || 
		have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_UNDEAD) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 93; 
			if(++magic_num_one == 10) continue;
		}
		//�Ñ�h���S������
		if(have_flag(flgs, TR_KILL_DRAGON) && rank > 3 || 
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DRAGON) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 94; 
			if(++magic_num_one == 10) continue;
		}
		//�A���o���C�g����
		if((have_flag(flgs, TR_KILL_HUMAN) && rank > 3 || 
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_HUMAN) && rank > 3)
		&& have_flag(flgs, TR_RES_TIME) )
		{
			p_ptr->magic_num1[magic_num++] = 95; 
			if(++magic_num_one == 10) continue;
		}
		//���j�[�N�����X�^�[����
		if(have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_UNIQUE) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 96; 
			if(++magic_num_one == 10) continue;
		}

		//��������
		if((have_flag(flgs, TR_SLAY_ANIMAL) || have_flag(flgs, TR_ESP_ANIMAL) )&& rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 114; 
			if(++magic_num_one == 10) continue;
		}
		//�_�i����
		if(have_flag(flgs, TR_KILL_DEITY) && rank > 1 || 
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEITY) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 122; 
			if(++magic_num_one == 10) continue;
		}
		//�㋉��������
		if(have_flag(flgs, TR_KILL_DEMON) && rank > 2 || 
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEMON) && rank > 3
		&& o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 123; 
			if(++magic_num_one == 10) continue;
		}
		//�d������
		if(have_flag(flgs, TR_KILL_KWAI) && rank > 2 || have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_KWAI) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 124; 
			if(++magic_num_one == 10) continue;
		}

		//���Ȃ�u���X
		if(have_flag(flgs, TR_BLESSED)  && rank > 3 || have_flag(flgs, TR_RES_HOLY) && have_flag(flgs, TR_WIS) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 6; 
			if(++magic_num_one == 10) continue;
		}
		//�n���̍��΂̃u���X
		if(o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 3 || have_flag(flgs, TR_KILL_GOOD) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 7; 
			if(++magic_num_one == 10) continue;
		}

		//�A�N�A�u���X
		if(o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank > 2 || have_flag(flgs, TR_RES_WATER) && have_flag(flgs, TR_CON) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 8; 
			if(++magic_num_one == 10) continue;
		}
		//�_�u���X
		if(o_ptr->name2 == EGO_WEAPON_BRANDACID && rank > 2 || have_flag(flgs, TR_IM_ACID) || 
		have_flag(flgs, TR_RES_ACID) && have_flag(flgs, TR_CON) && rank > 3 || have_flag(flgs, TR_BRAND_ACID) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 9; 
			if(++magic_num_one == 10) continue;
		}
		//�d���u���X ������Ɠ��₷������
		if(o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank > 1 || have_flag(flgs, TR_IM_ELEC) || 
		have_flag(flgs, TR_RES_ELEC) && have_flag(flgs, TR_DEX) && rank > 2 || have_flag(flgs, TR_BRAND_ELEC) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 10; 
			if(++magic_num_one == 10) continue;
		}
		//�Ή��u���X
		if(o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank > 2 || have_flag(flgs, TR_IM_FIRE) || 
		have_flag(flgs, TR_RES_FIRE) && have_flag(flgs, TR_STR) && rank > 3 || have_flag(flgs, TR_BRAND_FIRE) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 11; 
			if(++magic_num_one == 10) continue;
		}
		//��C�u���X
		if(o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 2 || have_flag(flgs, TR_IM_COLD) || 
		have_flag(flgs, TR_RES_COLD) && have_flag(flgs, TR_WIS) && rank > 3 || have_flag(flgs, TR_BRAND_COLD) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 12; 
			if(++magic_num_one == 10) continue;
		}

		//�Ńu���X
		if(o_ptr->name2 == EGO_WEAPON_BRANDPOIS && rank > 2 || 
		have_flag(flgs, TR_RES_POIS) && have_flag(flgs, TR_INT) && rank > 3 || have_flag(flgs, TR_BRAND_POIS) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 13; 
			if(++magic_num_one == 10) continue;
		}

		//�n���u���X
		if(o_ptr->name2 == EGO_WEAPON_DEMONLORD  || o_ptr->name2 == EGO_WEAPON_DEMON && rank > 2 || 
		have_flag(flgs, TR_RES_NETHER) && have_flag(flgs, TR_VAMPIRIC) && rank > 2 || have_flag(flgs, TR_KILL_HUMAN) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 14; 
			if(++magic_num_one == 10) continue;
		}

		//�M���u���X
		if(o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 1  || o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER && rank > 2 || 
		have_flag(flgs, TR_RES_LITE) && have_flag(flgs, TR_WIS) && rank > 2 || have_flag(flgs, TR_KILL_UNDEAD) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 15; 
			if(++magic_num_one == 10) continue;
		}
		//�Í��u���X
		if(o_ptr->name2 == EGO_WEAPON_MURDERER && rank > 2 || 
		have_flag(flgs, TR_RES_DARK) && have_flag(flgs, TR_CHR) && rank > 2 || have_flag(flgs, TR_SLAY_GOOD) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 16; 
			if(++magic_num_one == 10) continue;
		}
		//�����u���X
		if(o_ptr->name2 == EGO_WEAPON_DRAGONSLAY && rank > 2 || 
		have_flag(flgs, TR_RES_SOUND) && have_flag(flgs, TR_STR) && rank > 2 || have_flag(flgs, TR_SLAY_DRAGON) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 18; 
			if(++magic_num_one == 10) continue;
		}
		//�J�I�X�u���X
		if(o_ptr->name2 == EGO_WEAPON_CHAOS && rank > 2 || 
		have_flag(flgs, TR_RES_CHAOS) && have_flag(flgs, TR_CON) && rank > 2 || have_flag(flgs, TR_CHAOTIC) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 19; 
			if(++magic_num_one == 10) continue;
		}
		//�򉻃u���X
		if(o_ptr->name2 == EGO_WEAPON_GODEATER && rank > 2 || 
		have_flag(flgs, TR_RES_DISEN) && have_flag(flgs, TR_CON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 20; 
			if(++magic_num_one == 10) continue;
		}
		//���ʍ����u���X
		if(have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_TELEPORT) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 21; 
			if(++magic_num_one == 10) continue;
		}
		//���ԋt�]�u���X
		if(have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_CHR) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 22; 
			if(++magic_num_one == 10) continue;
		}
		//�x�݃u���X
		if(have_flag(flgs,TR_SPEED) && o_ptr->pval < 0 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 23; 
			if(++magic_num_one == 10) continue;
		}
		//�d�̓u���X
		if(o_ptr->name2 == EGO_WEAPON_QUAKE && rank > 1 || have_flag(flgs,TR_IMPACT) && have_flag(flgs, TR_STR) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 24; 
			if(++magic_num_one == 10) continue;
		}
		//�j�Ѓu���X
		if(o_ptr->name2 == EGO_WEAPON_GODEATER && rank > 2 || 
		have_flag(flgs, TR_RES_SHARDS) && have_flag(flgs, TR_DEX) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 25; 
			if(++magic_num_one == 10) continue;
		}
		//�v���Y�}�u���X
		if(have_flag(flgs, TR_BRAND_FIRE) && have_flag(flgs, TR_BRAND_ELEC) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 26; 
			if(++magic_num_one == 10) continue;
		}
		//���̓u���X
		if(have_flag(flgs, TR_FORCE_WEAPON) && have_flag(flgs, TR_CON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 28; 
			if(++magic_num_one == 10) continue;
		}
		//�j�M�u���X
		if(have_flag(flgs, TR_BRAND_FIRE) && have_flag(flgs, TR_RES_LITE) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 30; 
			if(++magic_num_one == 10) continue;
		}
		//�����u���X
		if(have_flag(flgs, TR_EX_VORPAL) && (have_flag(flgs, TR_STR) || have_flag(flgs, TR_CON) ) && rank > 2
		|| have_flag(flgs, TR_TUNNEL) && o_ptr->pval > 2 &&  (have_flag(flgs, TR_STR) || have_flag(flgs, TR_CON) ) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 32; 
			if(++magic_num_one == 10) continue;
		}


		//�}�W�b�N�~�T�C��
		if( rank >0 && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_SUST_INT) || have_flag(flgs, TR_SUST_WIS))
		|| have_flag(flgs, TR_THROW) )
		{
			p_ptr->magic_num1[magic_num++] = 59; 
			if(++magic_num_one == 10) continue;
		}
		//���Q
		if(o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON)
		{
			p_ptr->magic_num1[magic_num++] = 60; 
			if(++magic_num_one == 10) continue;
		}		
		//�Ӗ�
		if(o_ptr->name2 == EGO_WEAPON_MURDERER || o_ptr->name2 == EGO_WEAPON_BRANDACID && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 61; 
			if(++magic_num_one == 10) continue;
		}		
		//����
		if(o_ptr->name2 == EGO_WEAPON_CHAOS || o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 62; 
			if(++magic_num_one == 10) continue;
		}		
		//����
		if(o_ptr->name2 == EGO_WEAPON_QUAKE || o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 63; 
			if(++magic_num_one == 10) continue;
		}		
		//���
		if(o_ptr->name2 == EGO_WEAPON_VAMP || o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 64; 
			if(++magic_num_one == 10) continue;
		}	
		//�g���b�v
		if(have_flag(flgs, TR_AGGRAVATE) || o_ptr->curse_flags & TRC_ADD_H_CURSE || o_ptr->curse_flags & TRC_ADD_L_CURSE)
		{
			p_ptr->magic_num1[magic_num++] = 78; 
			if(++magic_num_one == 10) continue;
		}
		//�L������
		if(have_flag(flgs, TR_STEALTH) && o_ptr->pval > 0)
		{
			p_ptr->magic_num1[magic_num++] = 79; 
			if(++magic_num_one == 10) continue;
		}

		//���сA�x��
		if(have_flag(flgs, TR_AGGRAVATE) || have_flag(flgs, TR_STEALTH) && o_ptr->pval < 0)
		{
			if(rank > 1) p_ptr->magic_num1[magic_num++] = 128; //�x��
			else p_ptr->magic_num1[magic_num++] = 1; //����
			if(++magic_num_one == 10) continue;
		}
	

	}
	return (magic_num-10);

}

//���ۋZ���x�����v�Z����Bi��spell_list[]�Y����p_ptr->magic_num1[30]�ɂ͍��蕐��J�n����i�l�������Ă���͂�
//����POWERFUL�����̂���+100
static int calc_raiko_spell_lev(int i)
{
	int skilllev;
	object_type *o_ptr;
	if(i < p_ptr->magic_num1[30]) o_ptr = &inventory[INVEN_RARM];
	else  o_ptr = &inventory[INVEN_LARM];

	skilllev = k_info[o_ptr->k_idx].level / 2 + p_ptr->lev;
	if(skilllev > 99) skilllev = 99;
	if(object_is_artifact(o_ptr)) skilllev += 100;

	return skilllev;

}


//���T�͓Ǝ��ɍŒᎸ�s���v�Z������
static int satono_spell_minfail(void)
{
	monster_type *m_ptr = &m_list[p_ptr->riding];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	int chance = 25 - r_ptr->level / 4;//�����X�^�[�̖��@�g�p�Ɠ������s��

	//int mult = r_ptr->freq_spell + p_ptr->stat_ind[A_CHR] + 3;//���@�g�p��+���͕␳

	//if (mult > 100) mult = 100;

	//chance = 100 - (100 - chance) * mult / 100;//�����X�^�[���s���Ɩ��@�g�p������g�[�^�����s�����v�Z

	if (MON_STUNNED(m_ptr)) chance += 50;
	if (MON_CONFUSED(m_ptr)) chance += 100;

	if (chance < 0) chance = 0;
	if (chance > 100) chance = 100;

	return chance;
}

/*:::�����X�^�[�̖��@���r�����邽�߂̃��[�`�� �s����������Ƃ�TRUE��Ԃ�*/
bool cast_monspell_new(void)
{

	int spell_list[NEW_MSPELL_LIST_MAX]; //�g�p�\���@���X�g monspell_list2[]�̓Y��
	int memory=0; //�����X�^�[���@�̋L���\��
    int cnt=0; //���ۂ̃��X�g�̒���
	int spell_lev;
	cptr power_desc = "";

	int num; //�Z�ԍ� 0����J�n
	bool  flag_choice, flag_redraw;
    char  out_val[160];
    char  comment[80];
    char  choice;
    int   y = 1;
    int   x = 16;
	int i;
	int chance = 0;
	int cost;
    int minfail = 0;
	bool anti_magic = FALSE;
	int ask = TRUE;
	int xtra = 0; //aux()�ɓn������p�����[�^

	/*:::�e��ϐ��⃊�X�g��ݒ�*/
	for(i=0;i<NEW_MSPELL_LIST_MAX;i++) spell_list[i]=0; //�Ƃ肠����25�ӏ�������
	memory = calc_monspell_cast_mem();
	if(!memory)
	{
		msg_print("ERROR:�����X�^�[���@�̋L���\����0");
		return FALSE;
	}
	//�����X�^�[�ϐg��
	if(IS_METAMORPHOSIS)
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];
		power_desc = "�g�p";

		for(i=0;i<=MAX_MONSPELLS2;i++)
		{
			if(monspell_list2[i].priority == 0) continue;

			if(i <= 32 && !(r_ptr->flags4 >> (i-1) & 1L)) continue;
			if(i > 32 && i <= 64 && !(r_ptr->flags5 >> (i-33) & 1L)) continue;
			if(i > 64 && i <= 96 && !(r_ptr->flags6 >> (i-65) & 1L)) continue;
			if(i > 96 && !(r_ptr->flags9 >> (i-97) & 1L)) continue;


			if(cnt < memory)
			{
				spell_list[cnt] = i;
				cnt++;
			}
			else //�G��20�ȏ�̓��Z�������Ă���Ƃ��Apriority�l���Q�l�Ɉ�ԗD��x���Ⴂ�̂�����
			{
				int change_num = 0;
				int j;

				for(j=memory-1;j>0;j--)
				{
					if(monspell_list2[spell_list[j]].priority > monspell_list2[spell_list[change_num]].priority) change_num = j;
				}
				if(monspell_list2[spell_list[change_num]].priority > monspell_list2[i].priority) spell_list[change_num] = i;

			}
		}
	}
	//���Ƃ�p������
	else if(p_ptr->pclass == CLASS_SATORI)
	{
		power_desc = "�z�N";
		for(i=0;i<memory;i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if(spell_list[i]) cnt++;
		}
	}
	//���T�p������
	else if (p_ptr->pclass == CLASS_SATONO)
	{
		power_desc = "����";
		for (i = 0; i<memory; i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if (spell_list[i]) cnt++;
		}
	}
	//������p������
	else if(p_ptr->pclass == CLASS_KOKORO)
	{
		object_type *o_ptr = &inventory[INVEN_HEAD];

		power_desc = "�ĉ�";
		if(o_ptr->tval != TV_MASK)
		{
			msg_print("�ʂ𑕔����Ă��Ȃ��B");
			return FALSE;
		}
		else if(object_is_cursed(o_ptr))
		{
			msg_print("�ʂ�����Ă��Ďg���Ȃ��B");
			return FALSE;
		}

		spell_list[0] = o_ptr->xtra1;
		spell_list[1] = o_ptr->xtra2;
		spell_list[2] = o_ptr->xtra3;
		spell_list[3] = o_ptr->xtra4 % 200;
		spell_list[4] = o_ptr->xtra4 / 200;
		for(i=0;i<memory;i++) if(spell_list[i]) cnt++;
	}
	//���ۗp������
	else if(p_ptr->pclass == CLASS_RAIKO)
	{
		power_desc = "����";
		cnt = make_magic_list_aux_raiko();
		for(i=0;i<memory;i++)spell_list[i] = p_ptr->magic_num1[10+i];

	}
	//���e�����E
	else if(p_ptr->pclass == CLASS_YUKARI)
	{
		const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;/*:::10*10000*/
		power_desc = "�g�p";

		for(i=1;i<=MAX_MONSPELLS2;i++)
		{
			if(i % 8 != turn / (A_DAY / 24) % 8 ) continue;
			if(!monspell_list2[i].level) continue;
			spell_list[cnt++] = i;
		}
	}
	//�˕P�~�_
	else if(p_ptr->pclass == CLASS_YORIHIME)
	{
		power_desc = "�g�p";
		for(i=0;i<memory;i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if(spell_list[i]) cnt++;
		}
	}
	//�B��ށ@��˂̗�
	//v1.1.82b �e��������
	else if (p_ptr->pclass == CLASS_OKINA || p_ptr->pclass == CLASS_RESEARCHER)
	{
		int monspell_kind;
		monspell_kind = choose_monspell_kind();//�����X�^�[���@����(�{���g�E�{�[���E�u���X�Ȃ�)��I���B���s�[�g�Ȃ�O�̂��g�p

		if (!monspell_kind) return FALSE;

		if(p_ptr->pclass == CLASS_OKINA)
			power_desc = "�s�g";
		else
			power_desc = "�Č�";


		//�w�肵�����ނ̃����X�^�[���@�����ׂēo�^
		for (i = 1; i <= MAX_MONSPELLS2; i++)
		{
			if (monspell_list2[i].monspell_type != monspell_kind) continue;

			if (cnt == NEW_MSPELL_LIST_MAX)
			{
				msg_print("ERROR:�����X�^�[���@���X�g�̓o�^�\���𒴉߂���");
				break;
			}
			spell_list[cnt++] = i;
		}
	}

	else
	{
		msg_print("ERROR:���̐E�Ƃł̃����X�^�[���@�r����������������Ă��Ȃ�");
		return FALSE;
	}

	if(dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
		msg_print("�_���W���������͂��z�������I");
		return FALSE;
	}
	if(p_ptr->anti_magic) 
	{
		msg_print("�����@�o���A�ɖW�Q���ꂽ�I");
		return FALSE;
	}

	screen_save();

	flag_choice = FALSE;
    flag_redraw = FALSE;

      num = (-1);
#ifdef ALLOW_REPEAT
	  //���s�[�g�R�}���h�̂Ƃ��A�O��g�p���̓��Z�ԍ����擾����
	if (repeat_pull(&num))
	{
		//���x��������Ă��Ȃ�(���O�Ɍo���l�����U���󂯂ă��x�������������Ȃ�)�Ƃ��̓��s�[�g���Ă��I���ς݃t���O���������I���������B
		//�������h���~�[�Ɨ��T�̓��x����������
		//v1.1.94 �����X�^�[�ϐg���ɂ����x����������
		if ( monspell_list2[num].level <=  p_ptr->lev || (IS_METAMORPHOSIS) || p_ptr->pclass == CLASS_DOREMY || p_ptr->pclass == CLASS_SATONO) flag_choice = TRUE;

		//�˕P�_�~�낵�͎g���x�Ɍ��ʂ��ς��̂Ń��s�[�g�s��
		if(p_ptr->pclass == CLASS_YORIHIME) flag_choice = FALSE;
	}
#endif

/*:::��������A�Z�̈ꗗ��\�����đI�����󂯎��Z�ԍ��𓾂鏈��*/

	if(cnt == 0)
	{
		msg_format("%s�ł�����Z���Ȃ��B",power_desc);
		screen_load();
		return FALSE;
	}

	(void) strnfmt(out_val, 78, "(%c-%c, '*'�ňꗗ, ESC) �ǂ��%s���܂����H",	I2A(0), I2A(cnt - 1),power_desc);
	
	choice= (always_show_list ) ? ESCAPE:1;

	while (!flag_choice)
	{
		if(choice==ESCAPE) choice = ' '; 
		else if( !get_com(out_val, &choice, TRUE) )break;
		/*:::���j���[�\���p*/

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') )
		{
			/* Show the list */
			if (!flag_redraw)
			{
				char psi_desc[80];

				flag_redraw = TRUE;
				screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str("���O", y, x + 5);

				put_str(format("Lv ����MP�@ �֘A�@���� ����"), y, x + 35);

				/* Dump the spells */
				/*:::�Z�Ȃǂ̈ꗗ��\��*/
				for (i = 0; i < cnt; i++)
				{
					bool	flag_usable = TRUE;

					int use_stat = monspell_list2[spell_list[i]].use_stat;
					//���Ƃ�͏�ɒm�\�ŋZ���g��
					if(p_ptr->pclass == CLASS_SATORI) use_stat = A_INT;
					//���T�͏�ɖ��͂ŋZ���g���B
					if (p_ptr->pclass == CLASS_SATONO) use_stat = A_CHR;

					//���ۂ͂ǂ���̕���̋Z���Ń��x�����肪�Ⴄ
					if(p_ptr->pclass == CLASS_RAIKO)
						xtra = calc_raiko_spell_lev(i);

					cost = monspell_list2[spell_list[i]].smana;
					if(cost > 0)
					{
						/*:::����͌��� mod_need_mana�ɒ�`���ꂽMANA_DIV��DEC_MANA_DIV�̒l���g�p*/
						if (p_ptr->dec_mana) cost = cost * 3 / 4;

						if(cost < 1) cost = 1;
					}

					spell_lev = monspell_list2[spell_list[i]].level;
					chance = monspell_list2[spell_list[i]].fail;


					/*:::���s���v�Z�@���s���ݒ�0�̋Z�̓p�X*/					
					if (chance)
					{
						//v1.1.48 �h���~�[�̕ϐg�Ǝ����̜߈�
						if (IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME))
						{
							//�����X�^�[�̖��@�g�p���Ɠ������s��
							chance = 25 - (r_info[MON_EXTRA_FIELD].level + 3) / 4;
							if (chance < 0) chance = 0;

							if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
							else if (p_ptr->stun) chance += 15;
							/*:::�\����MP���Ȃ��Ƃ��@�ǂ݂̂����s�ł��Ȃ����邪*/
							if (cost > p_ptr->csp) chance = 100;

						}
						else
						{
							/*:::���x�����ɂ�鎸�s������*/
							chance -= 3 * (p_ptr->lev - spell_lev);
							/*:::�p�����[�^�ɂ�鎸�s������*/
							chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[use_stat]] - 1);

							/*:::���i��􂢂ɂ�鐬��������*/
							chance += p_ptr->to_m_chance;
							/*:::����͌����Ȃǂɂ�鎸�s������*/
							chance = mod_spell_chance_1(chance);
							/*:::�ŒᎸ�s������*/
							minfail = adj_mag_fail[p_ptr->stat_ind[use_stat]];
							if (chance < minfail) chance = minfail;

							/*:::�N�O�����s������ */
							if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
							else if (p_ptr->stun) chance += 15;

							/* Always a 5 percent chance of working */
							if (chance > 95) chance = 95;
							/*:::�\����MP���Ȃ��Ƃ��@�ǂ݂̂����s�ł��Ȃ����邪*/
							//v1.1.66 ���T�͊֌W�Ȃ��g����
							if (cost > p_ptr->csp && p_ptr->pclass != CLASS_SATONO)
							{	
								flag_usable = FALSE;
								chance = 100;
							}
							//���x�������̃��x����荂���Ǝg���Ȃ��B�����X�^�[�ϐg�Ɨ��T�͗�O
							if (p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS) && p_ptr->pclass != CLASS_SATONO)
							{
								flag_usable = FALSE;
								chance = 100;
							}
						}
					}


					//���T�̓R�X�g�ƍŒᎸ�s����ʂɌv�Z����
					if (p_ptr->pclass == CLASS_SATONO)
					{
						int s_minfail = satono_spell_minfail();
						if (chance < s_minfail) chance = s_minfail;
						cost = 0;
					}


					/*:::���ʗ��̃R�����g�𓾂�*/
					sprintf(comment, "%s", cast_monspell_new_aux(spell_list[i],TRUE, FALSE, xtra));

					/*:::�J�[�\���܂��̓A���t�@�x�b�g�̕\��*/
					sprintf(psi_desc, "  %c) ",I2A(i));

					//v1.1.82b �e�������Ɓ@���K���̓��Z�͉B��
					if (p_ptr->pclass == CLASS_RESEARCHER && !check_monspell_learned(spell_list[i]))
					{
						flag_usable = FALSE;




						strcat(psi_desc, "(���K��)");

					}
					else
					{
						strcat(psi_desc, format("%-30s%2d  %4d    %s %3d%%  %s",
							monspell_list2[spell_list[i]].name, monspell_list2[spell_list[i]].level,
							cost, stat_desc[use_stat], chance, comment));
					}


					if (cost > p_ptr->csp) 
						flag_usable = FALSE;

					///mod151001 �h���~�[�̓��x���Ɋւ�炸�Z���g����
					//v1.1.45 ���T��
					if (p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME)) && p_ptr->pclass != CLASS_SATONO)
						flag_usable = FALSE;

					if(!flag_usable)
						c_prt(TERM_L_DARK, psi_desc, y + i + 1, x);
					else
						prt(psi_desc, y + i + 1, x);

				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			/* Redo asking */
			continue;
		}

		/* Note verify */
		ask = isupper(choice);

		/* Lowercase */
		if (ask) choice = tolower(choice);

		/* Extract request */
		i = (islower(choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if ((i < 0) || (i >= cnt))
		{
			bell();
			continue;
		}

		/* Verify it */
		//v1.1.82b shift�������Ȃ�����Z��I�ԂƊm�F���o�邪�A���̂Ƃ��Z�����o��̂Œe�������Ƃ̖��K�����Z�ł͊m�F���Ȃ����Ƃɂ���
		if (ask && !(p_ptr->pclass == CLASS_RESEARCHER && !check_monspell_learned(spell_list[i])))
		{
			char tmp_val[160];

			/* Prompt */
			(void) strnfmt(tmp_val, 78, "%s���g���܂����H", monspell_list2[spell_list[i]].name);

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}
		num = spell_list[i];
		/* Stop the loop */
		flag_choice = TRUE;
#ifdef ALLOW_REPEAT /* TNB */
		repeat_push(num);
#endif /* ALLOW_REPEAT -- TNB */

	}

	/* Restore the screen */
	if (flag_redraw ) screen_load();
	/* Show choices */
	p_ptr->window |= (PW_SPELL);
	/* Window stuff */
	window_stuff();
	/* Abort if needed */
	if (!flag_choice) 
	{
		screen_load();
		return FALSE;
	}
/*:::�Z�̑I���I���B���s������ցB*/

	screen_load();

	if(p_ptr->pclass == CLASS_RAIKO)
		xtra = calc_raiko_spell_lev(i);

	/*:::�I�����ꂽ�Z�ɂ��ăR�X�g�⎸�s�����Ăьv�Z����@���s�[�g����������̂ōė��p�͂ł��Ȃ�*/
	chance = monspell_list2[num].fail;

	cost = monspell_list2[num].smana;

	//v1.1.82b
	if (p_ptr->pclass == CLASS_RESEARCHER && !check_monspell_learned(num))
	{
		msg_format("�܂����̓��Z���K�����Ă��Ȃ��B");
		return FALSE;

	}

	if (chance)
	{
		//v1.1.48 �h���~�[�̕ϐg�Ǝ����̜߈�
		if (IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME))
		{
			//�����X�^�[�̖��@�g�p���Ɠ������s��
			chance = 25 - (r_info[MON_EXTRA_FIELD].level + 3) / 4;

			if (chance < 0) chance = 0;

			if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
			else if (p_ptr->stun) chance += 15;
			/*:::�\����MP���Ȃ��Ƃ��@�ǂ݂̂����s�ł��Ȃ����邪*/
			if (cost > p_ptr->csp) chance = 100;

		}
		else
		{

			int use_stat = monspell_list2[num].use_stat;
			spell_lev = monspell_list2[num].level;
			//���Ƃ�͏�ɒm�\�ŋZ���g��
			if (p_ptr->pclass == CLASS_SATORI) use_stat = A_INT;
			//���T�͏�ɖ��͂ŋZ���g��
			if (p_ptr->pclass == CLASS_SATONO) use_stat = A_CHR;


			chance -= 3 * (p_ptr->lev - spell_lev);
			chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[use_stat]] - 1);

			chance += p_ptr->to_m_chance;
			chance = mod_spell_chance_1(chance);
			minfail = adj_mag_fail[p_ptr->stat_ind[use_stat]];
			if (chance < minfail) chance = minfail;
			if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
			else if (p_ptr->stun) chance += 15;
			if (chance > 95) chance = 95;
		}
	}
	if(cost > 0)
	{
		if(p_ptr->dec_mana) cost = cost * 3 / 4;
		if(cost < 1) cost = 1;
	}

	//���T�͓Ǝ��Ɏ��s���v�Z������
	if (p_ptr->pclass == CLASS_SATONO)
	{
		int s_minfail = satono_spell_minfail();
		if (chance < s_minfail) chance = s_minfail;
		cost = 0;
	}


	/*:::MP������Ȃ����͋������f�@�ǂ����I�v�V�����g��Ȃ��������߂�ǂ�*/
	if ( cost > p_ptr->csp)
	{
		msg_print("�l�o������܂���B");
		return FALSE;	
	}
	///mod151001 �h���~�[�ϐg�A���T�_���X�A�����߈˂̓��x���Ɋւ�炸�Z���g����
	if(p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME)) && p_ptr->pclass != CLASS_SATONO)
	{
		msg_format("���Ȃ��̃��x���ł͂܂�%s�ł��Ȃ��悤���B",power_desc);
		return FALSE;	
	}

	stop_raiko_music();

	/*:::���Z���s����*/
	if (randint0(100) < chance)
	{
		if (flush_failure) flush();
		msg_format("%s�Ɏ��s�����I",power_desc);
		sound(SOUND_FAIL);

		/*:::����Ȏ��s�y�i���e�B������ꍇ�����ɏ���*/
		cast_monspell_new_aux(num,FALSE,TRUE,xtra);
	}

	/*:::���Z��������*/
	else
	{
		/*:::���������Ƀ^�[�Q�b�g�I���ŃL�����Z�������Ƃ��Ȃǂɂ�cptr��NULL���Ԃ�A���̂܂܍s����������I������*/
		if(!cast_monspell_new_aux(num,FALSE,FALSE,xtra)) return FALSE;
	}

	p_ptr->csp -= cost;
	if (p_ptr->csp < 0) 
	{
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;
	}

	//���T�͋R�撆�����X�^�[���Z���g�����Ƃ��������Ȃ̂Ń����X�^�[�Ɉ�s�����s���x��������B
	//������������Ώۂ̔����ȂǂŋR�撆�����X�^�[������ł邩�������̂�p_ptr->riding���������ɓ���Ă����B
	if (p_ptr->pclass == CLASS_SATONO && p_ptr->riding)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
		m_ptr->energy_need += ENERGY_NEED();
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);

	return TRUE;

}


/*:::���q�̃��}�r�R���ݒ胋�[�`�� �����X�^�[�̃X�y�����s���[�`������thrown_spell��n���Bdel:TRUE��n������N���A*/
/*:::�X�y���̔ԍ���p_ptr->magic_num1[5]�Ɋi�[�B(0�`2�ӂ�͉̂Ŏg������) �ԍ���monspell_list2[1�`128]�̓Y�����Ƃ��Ďg����B*/
//attack_spell, monst_spell_monst,�L������
///mod160305 �A�C�e���J�[�h����g�����߂ɓƗ��ϐ��ɕۑ����邱�Ƃɂ���
void kyouko_echo(bool del, int thrown_spell)
{
	int num = thrown_spell - 95;

	//if(p_ptr->pclass != CLASS_KYOUKO) return;

	if(del)
	{
//		p_ptr->magic_num1[5]=0;
		monspell_yamabiko = 0;
		return;
	}
	if(num < 0 || num > 128)
	{
		msg_print("ERROR:kyouko_echo()�ŕs����thrown_spell�l���n���ꂽ");
	}

	if(!monspell_list2[num].level || !monspell_list2[num].use_words) return; //�g�p�s�̖��@�A�u���t���g���v�t���O�̂Ȃ����@�͔�Ώ�

	//p_ptr->magic_num1[5]=num;
	monspell_yamabiko = num;
	return;
}

/*:::�L�X�������i���U���̃{�[�i�X�l*/
int calc_kisume_bonus(void)
{
	object_type *o_ptr = &inventory[INVEN_BODY];
	int bonus=0;

	if(!character_generated) return 0;
	if(p_ptr->pclass != CLASS_KISUME){msg_print("ERROR:calc_kisume_bonus()���L�X���ȊO�ŌĂ΂ꂽ"); return 0;}
	if(o_ptr->tval != TV_ARMOR || o_ptr->sval != SV_ARMOR_TUB){msg_print("ERROR:calc_kisume_bonus()�ɂĉ��ȊO�̖h����o���ꂽ"); return 0;}

	bonus += (o_ptr->ac + o_ptr->to_a) * 2 + o_ptr->to_d * 4;

	if(p_ptr->concent) bonus *= (1 + p_ptr->concent);

	return bonus;
	
}

/*:::�W���x���Z�@�L�X���Ȃǁ@�s��������Ȃ���FALSE*/
bool do_cmd_concentrate(int mode)
{
	int max;

	p_ptr->concent++;
	switch(p_ptr->pclass)
	{
	case CLASS_KISUME:
		max = 1 + p_ptr->lev / 15;
		if(p_ptr->concent > max)
		{
			p_ptr->concent = max;
			msg_format("���Ȃ��͕W�I��҂��\���Ă���E�E(���x�F%d)", p_ptr->concent);
		}
		else 
			msg_format("���Ȃ��͏㏸���Ă������E�E(���x�F%d)", p_ptr->concent);
		break;
	case CLASS_YUGI:
		if(p_ptr->concent == 1)
			msg_print("����ڂ𓥂ݏo�����B��C���k�����B");
		else
			msg_print("����ڂ𓥂ݏo�����B�n�ʂ��h�ꂽ�B");

		break;
	case CLASS_SANAE:
		max = MAX(p_ptr->lev, 5);
		if(mode == p_ptr->magic_num1[0])
		{
			if(p_ptr->concent > max) p_ptr->concent = max;
			msg_format("�r���𑱂����E�E");
			break;			
		}
		else
		{
			p_ptr->concent = 1;
			p_ptr->magic_num1[0] = mode;
			if(mode == CONCENT_SOMA) 
				msg_format("���Ȃ��͔�p�̉r�����J�n�����E�E");
			else if(mode == CONCENT_KANA) 
				msg_format("���Ȃ��͊��̉r�����J�n�����E�E");
			else if(mode == CONCENT_SUWA) 
				msg_format("���Ȃ��͍��̉r�����J�n�����E�E");
		}
		break;
	case CLASS_MEIRIN:
		max = 50;
		if(mode == 1) p_ptr->concent += 4;
		if(p_ptr->concent > max) p_ptr->concent = max;
		break;

	case CLASS_BENBEN:
	case CLASS_YATSUHASHI:
			msg_print("����ȋՂƔ��i�̉����ӂ���񂾁E�E");
		break;
	case CLASS_RAIKO:
		if(p_ptr->concent < 5)
			msg_print("���Y���ɏ���đf�����U�������B");
		else if(p_ptr->concent < 10)
			msg_print("�M���I�ȘA����������I");
		else
			msg_print("���Ȃ��̃r�[�g�͍ō������I");

		break;
	case CLASS_YORIHIME:
		msg_print("�_���l�̌���n�ʂɓ˂����Ă��B�n�ʂ��疳���̐n�������Ă����I");
		break;

	case CLASS_SOLDIER:
		max = 50;
		if(p_ptr->concent > max) 
		{
			p_ptr->concent = max;
			msg_print("���Ȃ��͌��E�܂ł̏W�����ێ�����..");
		}
		else
		{
			msg_print("���Ȃ��͏W�����đ_�����߂�..");

		}
		break;
	case CLASS_RESEARCHER:
		max = 1;
		if (p_ptr->concent > max) p_ptr->concent = max;

		p_ptr->magic_num1[0] = mode; //�C������/���͏W���̂ǂ�����g���Ă邩��magic_num1[0]�ŊǗ�

		if (mode == CONCENT_KANA)
		{
			p_ptr->concent = max;
			msg_print("���Ȃ��͑傫�������z�����񂾁I");
		}
		else
		{
			msg_print("���Ȃ��͖��͂��W������..");
		}
		break;

	default:
		msg_print("���̐E�Ƃ�concentrate�l���Z���[�`�����o�^����Ă��Ȃ�");
		return FALSE;
	}

	reset_concent = FALSE;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	p_ptr->window |= (PW_EQUIP);
	p_ptr->redraw |= (PR_STATUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	return (TRUE);
}


/*:::�͂��Ă̔O�ʇU�@�L�[���[�h����͂�����ɍ��v�������̂̎��͂��B�e����*/
bool hatate_psychography(void)
{
	char temp[80] = "";
	int xx;
	char temp2[80];
	int i;
	int tx = -1, ty = -1;
	bool success = FALSE;
	int range = 7;

	if (!get_string("�L�[���[�h����́F",temp, 70)) return FALSE;

	//�t���A�̃����X�^�[�𑖍����L�[���[�h�Ɉ�v����΍��W���i�[
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		m_ptr = &m_list[i];
		if(!m_ptr->r_idx) continue;
		r_ptr = &r_info[m_ptr->r_idx];

		for (xx = 0; temp[xx] && xx < 80; xx++)
		{
			if (iskanji(temp[xx]))
			{
				xx++;
				continue;
			}
			if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
		}
  
		strcpy(temp2, r_name + r_ptr->E_name);
		for (xx = 0; temp2[xx] && xx < 80; xx++)if (isupper(temp2[xx])) temp2[xx] = tolower(temp2[xx]);
		if (my_strstr(temp2, temp) || my_strstr(r_name + r_ptr->name, temp))
		{
			success = TRUE;
			ty = m_ptr->fy;
			tx = m_ptr->fx;
			break;
		}
	}
	//�����X�^�[�ɊY�����Ȃ���΃t���A�̃A�C�e���œ����悤�ɂ���
	if(!success)
	{
		char o_name[MAX_NLEN];
		for (i = 1; i < o_max; i++)
		{
			object_type *o_ptr = &o_list[i];

			if (!o_ptr->k_idx) continue;
			for (xx = 0; temp[xx] && xx < 80; xx++)
			{
				if (iskanji(temp[xx]))
				{
					xx++;
					continue;
				}
				if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
			}
			object_desc(o_name, o_ptr, (OD_NAME_ONLY | OD_STORE));
			if (my_strstr(o_name, temp))
			{
				success = TRUE;

				if(o_ptr->held_m_idx)
				{
					ty = m_list[o_ptr->held_m_idx].fy;
					tx = m_list[o_ptr->held_m_idx].fx;
				}
				else
				{
					ty = o_ptr->iy;
					tx = o_ptr->ix;
				}
				break;
			}
		}
	}


	if(!success)
	{
		msg_print("�������ʁE�E�Y���Ȃ�");
	}
	else if(!in_bounds2(ty,tx))
	{
		msg_print("ERROR:hatate_psychography()��tx��ty����������");
		return FALSE;
	}
	//�����B�Y���ӏ����ӂ̒n�`�A�A�C�e���A�G�����m����
	else
	{
		int x,y;
		cave_type       *c_ptr;
		s16b            feat;
		feature_type    *f_ptr;

		if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;
		//�n�`
		for (y = 1; y < cur_hgt - 1; y++)
		for (x = 1; x < cur_wid - 1; x++)
		{
			if (distance(ty, tx, y, x) > range) continue;
			c_ptr = &cave[y][x];
			c_ptr->info |= (CAVE_KNOWN);

			feat = get_feat_mimic(c_ptr);
			f_ptr = &f_info[feat];

			if (!have_flag(f_ptr->flags, FF_WALL))
			{
				if (have_flag(f_ptr->flags, FF_REMEMBER)) c_ptr->info |= (CAVE_MARK);
				for (i = 0; i < 8; i++)
				{
					c_ptr = &cave[y + ddy_ddd[i]][x + ddx_ddd[i]];
					feat = get_feat_mimic(c_ptr);
					f_ptr = &f_info[feat];
					if (have_flag(f_ptr->flags, FF_REMEMBER)) c_ptr->info |= (CAVE_MARK);
				}
			}
		}

		//�A�C�e���A��
		for (i = 1; i < o_max; i++)
		{
			object_type *o_ptr = &o_list[i];
			if (!o_ptr->k_idx) continue;
			if (o_ptr->held_m_idx) continue;
			y = o_ptr->iy;
			x = o_ptr->ix;
			if (distance(ty, tx, y, x) > range) continue;
			o_ptr->marked |= OM_FOUND;
			lite_spot(y, x);
		}
		//�����X�^�[
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			if (!m_ptr->r_idx) continue;
			y = m_ptr->fy;
			x = m_ptr->fx;

			if (distance(ty, tx, y, x) > range) continue;

			if (!(r_ptr->flags2 & RF2_INVISIBLE) || p_ptr->see_inv)
			{
				repair_monsters = TRUE;
				m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);
				update_mon(i, FALSE);
			}
		}

		msg_format("�u%s�v��O�ʂ����I",temp);
		p_ptr->redraw |= (PR_MAP);
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	}

	return TRUE;
}



/*:::���������Z�ɂ����ꑕ���u�����̖ʁv���쐬����*/
void make_mask(monster_type *m_ptr)
{
	object_type forge;
	object_type *o_ptr = &forge;
	monster_race *r_ptr;
	char m_name[80];
	int gain_skill[5] = {0,0,0,0,0};
	int gain_skill_cnt = 0;
	int i;

	if(p_ptr->pclass != CLASS_KOKORO) { msg_print("ERROR:������ȊO��make_mask()���Ă΂ꂽ"); return;}
	monster_desc(m_name, m_ptr, 0);
	r_ptr = &r_info[m_ptr->r_idx];

	if(r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND) )
	{
		msg_format("%s�͊���������Ȃ��悤���B",m_name);
		return;
	}

	object_prep(o_ptr, (int)lookup_kind(TV_MASK, 0));
	/*:::�����X�^�[�̔\�͂�object_type�̊e�ϐ��֊i�[����B
	 *:::�����_�����j�[�N�ɑΉ����邽�߃����X�^�[���⃌�x���A�X�L���ȂǑS�Ă����Ŋi�[���Ȃ��Ƃ����Ȃ��B
	 *���x����discount
	 *HP��xtra5(dd��ds�͓������Ƃ��̏������ʓ|)
	 *AC��1/10�ɂ��ʂ�AC�։��Z
	 *�����X�^�[����art_name
	 */
	o_ptr->number = 1;
	o_ptr->discount = r_ptr->level;
	o_ptr->xtra5 = MIN(30000,(r_ptr->hdice * r_ptr->hside));
	o_ptr->to_a = MIN(99,(r_ptr->ac / 10));
	o_ptr->art_name = quark_add(format("%s��",m_name));

	/*:::�����X�^�[�̑ϐ���art_flags�ɃR�s�[*/
	/*:::Hack - POWERFUL�t���O��TR120�Ƃ��Ĉ���*/
	/*:::�t���O���̂܂܎�������GHB�Ƃ��̖ʂ��n�������̂Ń��x���Ń����_������ɂ���*/
	if( r_ptr->flagsr & (RFR_IM_ACID) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_RES_ACID);
	if( r_ptr->flagsr & (RFR_IM_ELEC) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_RES_ELEC);
	if( r_ptr->flagsr & (RFR_IM_FIRE) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_RES_FIRE);
	if( r_ptr->flagsr & (RFR_IM_COLD) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_RES_COLD);
	if( r_ptr->flagsr & (RFR_IM_POIS) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_POIS);

	if( r_ptr->flagsr & (RFR_RES_LITE) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_LITE);
	if( r_ptr->flagsr & (RFR_RES_DARK) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_DARK);
	if( r_ptr->flagsr & (RFR_RES_NETH) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_NETHER);
	if( r_ptr->flagsr & (RFR_RES_WATE) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_WATER);
	if( r_ptr->flagsr & (RFR_RES_SHAR) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_SHARDS);
	if( r_ptr->flagsr & (RFR_RES_SOUN) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_SOUND);
	if( r_ptr->flagsr & (RFR_RES_CHAO) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_CHAOS);
	if( r_ptr->flagsr & (RFR_RES_HOLY) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_HOLY);
	if( r_ptr->flagsr & (RFR_RES_DISE) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_DISEN);
	if( r_ptr->flagsr & (RFR_RES_TIME) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_TIME);

	if( r_ptr->flags3 & (RF3_NO_FEAR) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_FEAR);
	if( r_ptr->flags3 & (RF3_NO_CONF) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_CONF);
	if( r_ptr->flags3 & (RF3_NO_SLEEP) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_FREE_ACT);
	if( r_ptr->flags2 & (RF2_POWERFUL) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_SPECIAL_KOKORO);

	/*:::�����X�^�[�̃X�L������ő�5�����_���ɑI�肵��xtra1-4�֊i�[*/
	for(i=0;i<128;i++)
	{
		int shift = i % 32;
		u32b rflg;

		if(i < 32) rflg = r_ptr->flags4;
		else if(i < 64) rflg = r_ptr->flags5;
		else if(i < 96) rflg = r_ptr->flags6;
		else rflg = r_ptr->flags9;

		//�����ĂȂ��Z��continue;
		if(!(rflg & (1L << shift))) continue;
		//���ʂȍs���Ȃǁ����g���Ȃ��Z��continue;
		if(!monspell_list2[i+1].level) continue;


		if(gain_skill_cnt < 5) gain_skill[gain_skill_cnt] = i+1;
		else
		{
			int check = randint0(gain_skill_cnt+1);
			if(check < 5) gain_skill[check] = i+1;
		}
		gain_skill_cnt++;
	}

	o_ptr->xtra1 = gain_skill[0];
	o_ptr->xtra2 = gain_skill[1];
	o_ptr->xtra3 = gain_skill[2];
	//xtra4��s16b�Ȃ̂œ�i�[����
	o_ptr->xtra4 = gain_skill[3] + gain_skill[4] * 200; 

	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	(void)drop_near(o_ptr, -1, py, px);

	return;

}

/*:::�i���ψق������ψق̐��𐔂���*/
int muta_erasable_count(void)
{
	int bits = 0;
	bits += count_bits(p_ptr->muta1 & ~(p_ptr->muta1_perma));
	bits += count_bits(p_ptr->muta2 & ~(p_ptr->muta2_perma));
	bits += count_bits(p_ptr->muta3 & ~(p_ptr->muta3_perma));
	bits += count_bits(p_ptr->muta4 & ~(p_ptr->muta4_perma));

	return (bits);
}

/*:::���݂̗얲�̃p���[�A�b�v�x�����𓾂�*/
int osaisen_rank(void)
{
	if(p_ptr->pclass != CLASS_REIMU)
	{
		//msg_print("ERROR:�얲�ȊO��osaisen_rank()���Ă΂ꂽ");
		return 0;
	}
	if(osaisen >= OSAISEN_MAX) return 9;
	if(osaisen >= 50000000) return 8;
	if(osaisen >= 10000000) return 7;
	if(osaisen >= 5000000) return 6;
	if(osaisen >= 1000000) return 5;
	if(osaisen >= 500000) return 4;
	if(osaisen >= 100000) return 3;
	if(osaisen >= 50000) return 2;
	if(osaisen >= 10000) return 1;
	else return 0;

}

/*:::�얲�����ΑK�𓾂�*/
void gain_osaisen(int amount)
{
	int old_rank = osaisen_rank();
	int new_rank;
	if(p_ptr->pclass != CLASS_REIMU) return;
	if(osaisen >= OSAISEN_MAX) return;
	osaisen += amount;
	if(osaisen > OSAISEN_MAX) osaisen = OSAISEN_MAX;

//msg_format("Osaisen:%d(%d)",amount,osaisen);
	new_rank = osaisen_rank();
	if(old_rank != new_rank)
	{
		msg_print("���������C���o�Ă����I");
		p_ptr->update |= (PU_BONUS);
	
	}
}


/*:::���ΑK������B*/
void throw_osaisen(void)
{
	s32b amount;
	char out_val[160];
	cptr p;
	if(p_ptr->pclass == CLASS_REIMU)
	{
		msg_print("ERROR:�얲��throw_osaisen()���Ă΂ꂽ");
		return ;
	}
	//�E�B�U�[�h�R�}���h�₢�����ܒ����ΑK�������Ȃ� 
	if (p_ptr->noscore || p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		msg_print("���s���̋��𓊂��悤�Ƃ������A�Ȃ����ΑK�����ǂ��ɂ���������Ȃ��B");
		return ;
	}
	if(quest[QUEST_CHAOS_WYRM].status != QUEST_STATUS_FINISHED && !EXTRA_MODE && 
		!(p_ptr->pclass == CLASS_3_FAIRIES || p_ptr->pclass == CLASS_SUNNY || p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_STAR))
	{
		msg_print("���Y�҂̗d���������ΑK���̉����狻���ÁX�̗l�q�ł���������Ă���B��ɂ��Ă������B");
		return ;
	}
	if (osaisen >= OSAISEN_MAX && !EXTRA_MODE)
	{
		msg_print("�M������ƂɁA�ΑK���͊��Ɉ�t���B");
		return ;
	}


	if (p_ptr->au < 1)
	{
		msg_print("���Ȃ��͖��ꕶ���B");
		return ;
	}

	strcpy(out_val, "");	
	if (get_string("���ΑK�����������܂����H", out_val, 32))
	{
		for (p = out_val; *p == ' '; p++);
		amount = atol(p);
		if(amount > p_ptr->au)
		{
			msg_print("����Ȃɂ����������Ă��Ȃ��B");
			return ;
		}
		else if(amount < 0)
		{
			msg_print("�ΑK�D�_���������͂Ȃ��B");
			return;
		}
		else if(amount == 0)
		{
			msg_print("��͂��߂Ă������B");
			return;
		}

		if(osaisen + amount > OSAISEN_MAX && !EXTRA_MODE)
		{
			amount = OSAISEN_MAX - osaisen;
			msg_print("�ΑK���ɓ��肫��Ȃ��̂œ�����邾�����ꂽ�B");

		}
		else
		{
			msg_print("���ΑK����ꂽ�B");
		}

		osaisen += amount;
		if(osaisen > OSAISEN_MAX) osaisen = OSAISEN_MAX; //Paranoia
		p_ptr->au -= amount; 
		p_ptr->redraw |= (PR_GOLD);

		//Hack - Extra���[�h�ł͌����p�����[�^���ΑK���z���L�^����A���̌�̃g���[�h�ɉe����^����
		if(EXTRA_MODE)
		{
			if(ex_buildings_param[f_info[cave[py][px].feat].subtype] != 255)
			{
				int tmp = ex_buildings_param[f_info[cave[py][px].feat].subtype];
				tmp += amount / (dun_level * dun_level / 5 + 100);
				if(tmp > 100) tmp = 100;
				ex_buildings_param[f_info[cave[py][px].feat].subtype] = (byte)tmp;
			}
		}

	}
	else
		msg_print("��͂��߂Ă������B");

}

/*:::�ΑK�����N���m�F����@�얲��p*/
void check_osaisen(void)
{
	if(p_ptr->pclass != CLASS_REIMU)
	{
		msg_print("ERROR:�얲�ȊO��check_osaisen()���Ă΂ꂽ");
		return ;
	}

	prt("�ΑK�����m�F�����E�E", 7, 5); 
	(void)inkey();
	switch(osaisen_rank())
	{
		case 0:
		prt("�قƂ�ǋ���ۂ��B", 8, 5); 
		prt("�S�܂Ŋ��X�����Ȃ�悤���E�E", 9, 5); 
		break;
		case 1:
		prt("���Ƃ��̂������ΑK�̓��肾�E�E", 8, 5); 
		break;
		case 2:
		prt("�����͓����Ă���B", 8, 5); 
		break;
		case 3:
		prt("���ΑK�͂���Ȃ�ɓ����Ă���B", 8, 5); 
		break;
		case 4:
		prt("���ΑK�̓���͏�X���B", 8, 5); 
		break;
		case 5:
		prt("��R�̂��ΑK�������Ă���B", 8, 5); 
		prt("�_�Ђ̍D�]���ւ炵���B", 9, 5); 
		break;
		case 6:
		prt("�т����肷��قǂ��ΑK�������Ă���B", 8, 5); 
		prt("���Ă��邾���ŐS�����܂�悤���B", 9, 5); 
		break;
		case 7:
		prt("�M�����Ȃ��قǂ̂��ΑK�������Ă���B", 8, 5); 
		prt("�l�X�̊��ӂƊ��҂��󂯂Đg���������܂�v�����B", 9, 5); 
		break;
		case 8:
		prt("���ΑK����t�ɋl�܂��Ă���B", 8, 5); 
		prt("�g���S����тɂ͂��؂ꂻ�����I", 9, 5); 
		break;
		case 9:
		prt("�ΑK�������ΑK�ɖ��܂��Ă���I", 8, 5); 
		prt("���ɂ��z�����Ȃ��������i���ڂ̑O�ɂ���I", 9, 5); 
		break;
	}

}


/*:::�����������@�쐬�p�̖��͂𓾂�BMaripo�����▂�͒��o���ɌĂ΂��B*/
//�E�Ɩ������̂Ƃ��͒��o�ʂ�{�ɂ���H����o�͔{�œ���X�L���Ń_���W�������Œ��o������ʏ�H
//mult�͒��o�ʌW��(�P�ʂ�%)
//���͂�marisa_magic_power[8]�Ɋi�[����邪�A�������v���C���Ƀ`�[�g�I�v�V������ON�ɂ���������magic_num[30-37]�Ɋi�[
void marisa_gain_power(object_type *o_ptr, int mult)
{
	int type;
	int tv = o_ptr->tval;
	int sv = o_ptr->sval;
	bool flag = FALSE;
	bool cheat_gain = FALSE;

	///mod160103 �������܈ȊO�̃`�[�g���[�h�������͕ʗ̈�ɖ��͂𓾂�
	if (p_ptr->pclass == CLASS_MARISA && p_ptr->noscore)
	{
		cheat_gain = TRUE;
	}
	//�ق��A���L�����f�o�b�O���[�h�␫�i�������܂̂Ƃ��͒l�𓾂��Ȃ��B
	else if (p_ptr->noscore || p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		if(p_ptr->wizard || cheat_xtra) msg_print("�f�o�b�O���[�h�₢�����܂̎��͖������p���͂𓾂��Ȃ�");
		return;
	}
	for(type=0;type<8;type++)
	{
		int gain = 0;

		switch(type)
		{
			case MARISA_POWER_FIRE:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_STUPIDITY) gain = 3;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_WEAKNESS) gain = 4;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_BLINDNESS) gain = 10;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_RED) gain = 5;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_GARNET) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HEMATITE) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONSCALE) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_RUBY) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) gain = 100;
				if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_PHOENIX_FEATHER) gain = 1000;
				break;

			case MARISA_POWER_COLD:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_RESTORE_STR) gain = 10;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_WHITE) gain = 5;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_AQUAMARINE) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_PERIDOT) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONSCALE) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ICESCALE) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_SAPPHIRE) gain = 300;
				if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) gain = 1000;
				break;
			case MARISA_POWER_ELEC:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_PARALYSIS) gain = 4;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_DISEASE) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_POISON) gain = 10;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_RESTORE_CON) gain = 10;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_BLUE) gain = 5;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_AMETHYST) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_OPAL) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HEMATITE) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONSCALE) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_EMERALD) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) gain = 100;
				break;
			case MARISA_POWER_LITE:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_NAIVETY) gain = 4;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CONFUSION) gain = 2;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_PARANOIA) gain = 8;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIKARIGOKE) gain = 10;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_TOPAZ) gain = 15;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MITHRIL) gain = 50;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_TAKAKUSAGORI) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_EMERALD) gain = 200;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HOPE_FRAGMENT) gain = 300;
				if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_PHOENIX_FEATHER) gain = 500;
				break;
			case MARISA_POWER_DARK:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_SICKNESS) gain = 6;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_POISON) gain = 3;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_UNHEALTH) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_BLACK) gain = 5;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_GREEN) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_CONFUSION) gain = 8;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MOONSTONE) gain = 10;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_COAL) gain = 10;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_LAPISLAZULI) gain = 15;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MAGNETITE) gain = 30;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ISHIZAKURA) gain = 20;
				if (tv == TV_MATERIAL && sv == SV_MATERIAL_SKULL) gain = 25;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_NIGHTMARE_FRAGMENT) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_SAPPHIRE) gain = 200;
				break;
			case MARISA_POWER_ILLU:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_BLINDNESS) gain = 3;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_HALLUCINATION) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_BERSERK) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_GRAY) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_L_BLUE) gain = 15;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_TAKAKUSAGORI) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIKARIGOKE) gain = 10;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ISHIZAKURA) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ARSENIC) gain = 50;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_GELSEMIUM) gain = 50;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ICESCALE) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MYSTERIUM) gain = 500;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_NIGHTMARE_FRAGMENT) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_RUBY) gain = 200;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIHIIROKANE) gain = 1000;
				if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) gain = 300;

				break;
			case MARISA_POWER_STAR:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_PARANOIA) gain = 2;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_SERIOUS) gain = 4;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MANA) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_CLEAR) gain = 5;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_STONE) gain = 1; //���Ƃ�3�Ŋ���
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ADAMANTITE) gain = 30;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DIAMOND) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HOPE_FRAGMENT) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_METEORICIRON) gain = 1000;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MYSTERIUM) gain = 500;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIHIIROKANE) gain = 1000;
				break;
			case MARISA_POWER_SUPER:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_RESTORING) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_GAUDY) gain = 4;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MERCURY) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_BROKEN_NEEDLE) gain = 30;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_SUPER) gain = 500;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONNAIL) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DIAMOND) gain = 200;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MYSTERIUM) gain = 500;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIHIIROKANE) gain = 1000;
				if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_PHOENIX_FEATHER) gain = 500;
				if (tv == TV_MUSHROOM && sv == SV_MUSHROOM_PUFFBALL) gain = 10;
				if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) gain = 300;
				if (tv == TV_MATERIAL && sv == SV_MATERIAL_RYUUZYU) gain = 100;
				break;
			default:
			msg_print("ERROR:marisa_gain_power()");
			return;
		}

		if(tv == TV_MATERIAL && sv == SV_MATERIAL_SCRAP_IRON)
		{
			if(one_in_(3)) gain = randint1(10);
			while(one_in_(3)) gain *= 2;
		}
		gain *= o_ptr->number;
		if(tv == TV_MATERIAL && sv == SV_MATERIAL_STONE) gain /= 3;
		gain = gain * mult / 100;
		if(!gain) continue;
		flag = TRUE;

		//�E�Ɩ������̎����b�Z�[�W�o��
		if(p_ptr->pclass == CLASS_MARISA) 
		{
			msg_format("%s�̖��͂�%d���o�����I",marisa_essence_name[type],gain);
			msg_print(NULL);
		}

		//���͂𓾂鏈���@�������`�[�g�I�v�V����ON�̂Ƃ�magic_num[30-37]���g��
		if(cheat_gain)
		{
			p_ptr->magic_num1[30+type] += gain;
			if(p_ptr->magic_num1[30+type] > MARISA_POWER_MAX) p_ptr->magic_num1[30+type] = MARISA_POWER_MAX;

		}
		else
		{
			marisa_magic_power[type] += gain;
			if(marisa_magic_power[type] > MARISA_POWER_MAX) marisa_magic_power[type] = MARISA_POWER_MAX;
		}
	}

	if(p_ptr->pclass == CLASS_MARISA && !flag) msg_print("���������Ȃ������E�E");
}



/*:::�ʂ������̂����j���Ă����Ԃ��ǂ����`�F�b�N����B*/
/*:::�t���A���ړ���������Adelete_monster_idx()���A�ϐg���@�������ɌĂ΂��B���[�v����*/
void check_nue_undefined(void)
{
	int i;
	bool flag_undefined = TRUE;

	if(p_ptr->pclass != CLASS_NUE) return;
	//if(p_ptr->special_defense & NUE_UNDEFINED) return;
	//���̕s���ȊO�̕ϐg���ɂ͊֌W�Ȃ�
	if(p_ptr->mimic_form && p_ptr->mimic_form != MIMIC_NUE) return;

	//�ʂ��̐��̂����j���Ă���G�����邩
	for (i = 1; i < m_max; i++)
	{
		if(!m_list[i].r_idx) continue;
		if(m_list[i].mflag & MFLAG_SPECIAL)
		{
			flag_undefined = FALSE;
			break;
		}
	}

	/*:::�ʂ��͕ϐg���ĂȂ������̂����j���Ă���G�����Ȃ��̂ŕϐg����*/
	if(flag_undefined && !(p_ptr->special_defense & NUE_UNDEFINED))
	{
		msg_print("�����ɂ��Ȃ��̐��̂�m���Ă���҂͂��Ȃ��B");
		p_ptr->special_defense |= NUE_UNDEFINED;
		set_mimic(1,MIMIC_NUE,FALSE);
	}
	//�ϐg�������t���A�ɐ��̂����j���Ă�����̂����鎞
	else if(!flag_undefined && (p_ptr->special_defense & NUE_UNDEFINED))
	{
		msg_print("�����ɂ͂��Ȃ��̐��̂�m���Ă���҂�����I");
		p_ptr->special_defense &= ~(NUE_UNDEFINED);
		set_mimic(0,0,FALSE);

	}

}


/*:::�ʂ�������̓G�̍s���ɂ�萳�̂����j��ꂽ���ǂ����`�F�b�N����*/
//chance_mult:�s���ɂ�錩�j��₷���{���@�M������3�{�Ȃ� ���̏ꍇ�K�����j��
void check_nue_revealed(int m_idx, int chance_mult)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr;
	int chance=0;
	int power = 0;
	char m_name[80];

	if(!m_ptr->r_idx) return;
	r_ptr = &r_info[m_ptr->r_idx];	
	//�ʂ�����
	if(p_ptr->pclass != CLASS_NUE) return;
	//���̕s���ȊO�̕ϐg���ɂ͌��j���Ȃ�
	if(p_ptr->tim_mimic && p_ptr->mimic_form != MIMIC_NUE) return;
	//���łɌ��j��ꂽ�G�ɂ͊֌W�Ȃ�
	if(m_ptr->mflag & MFLAG_SPECIAL) return;

	if(r_ptr->flags2 & (RF2_STUPID | RF2_EMPTY_MIND | RF2_WEIRD_MIND)) return;
	if(r_ptr->flags3 & RF3_NONLIVING) return;
	if(!is_hostile(m_ptr)) return;

	if(!los(py,px,m_ptr->fy,m_ptr->fx)) return;


	if(chance_mult >= 0)
	{
		chance = 20 + r_ptr->level * 2;

		if (r_ptr->flags2 & RF2_SMART_EX) chance *= 10;
		else if (r_ptr->flags2 & RF2_SMART) chance *= 3;
		if(is_gen_unique(m_ptr->r_idx)) chance *= 4;
		else if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance *= 2;

		if(m_ptr->cdis < 2) chance *= 2;

		if(!p_ptr->tim_mimic) chance *= 5;
		if (cave[py][px].info & CAVE_GLOW) chance *= 5;
		chance = chance * chance_mult / 100;

		power = (50 + p_ptr->lev * 3) * (30 + adj_general[p_ptr->stat_ind[A_CHR]] + adj_general[p_ptr->stat_ind[A_INT]]);
	}

	//���j���Ȃ�������I��
	if(chance_mult >= 0 && chance < randint1(power)) return;

	monster_desc(m_name, m_ptr, 0);
	if(p_ptr->special_defense & NUE_UNDEFINED)
	{
		msg_format("%s�ɐ��̂����j���Ă��܂����I",m_name);
		m_ptr->mflag |= MFLAG_SPECIAL;
		p_ptr->special_defense &= ~(NUE_UNDEFINED);
		if(p_ptr->mimic_form) set_mimic(0,0,FALSE);
	}
	else
	{
		msg_format("%s�ɂ����̂��΂�Ă��܂����I",m_name);
		m_ptr->mflag |= MFLAG_SPECIAL;
	}
}


void restore_seija_item(void)
{
	int i;
	bool flag_mdv = FALSE;
	bool flag_mdv2 = FALSE;

	if(p_ptr->pclass != CLASS_SEIJA && p_ptr->pclass != CLASS_MAGIC_EATER) return;

	for (i = 0; i < EATER_EXT*2; i++)
	{
		if(p_ptr->magic_num1[i] != p_ptr->magic_num2[i]*EATER_CHARGE)
		{	
			flag_mdv = TRUE;
			p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
		}
	}
	for (; i < EATER_EXT*3; i++)
	{
		if(p_ptr->magic_num1[i])
		{

			p_ptr->magic_num1[i] = 0;
			flag_mdv = TRUE;
		}
	}
	if(p_ptr->pclass == CLASS_MAGIC_EATER)
	{
		if(flag_mdv) msg_print("���Ȃ��͖�����̃����e�i���X�������B");
		return;
	}
	for( i = SEIJA_ITEM_START; i <= SEIJA_ITEM_END; i++)
	{
		if(p_ptr->magic_num1[i] != p_ptr->magic_num2[i])
		{
			p_ptr->magic_num1[i] = p_ptr->magic_num2[i];
			flag_mdv2 = TRUE;
		}

	}

	if(flag_mdv) msg_print("���Ȃ��͉B��������������̃����e�i���X�������B");
	if(flag_mdv2) msg_format("%s�s�v�c����̃����e�i���X�������B",flag_mdv?"�����":"���Ȃ���");

}


/*:::�[�U���̑����i��I��*/
bool item_tester_hook_recharging_equipment(object_type *o_ptr)
{
	if(!o_ptr->k_idx) return FALSE;
	if(!item_tester_hook_wear(o_ptr)) return FALSE;
	if(o_ptr->timeout) return TRUE;
	return (FALSE);
}

/*:::�����i��I�����ď[�U����*/
bool do_cmd_recharge_equipment(int power)
{
	object_type *o_ptr;
	int item;
	char o_name[MAX_NLEN];
	cptr q, s;

	item_tester_hook = item_tester_hook_recharging_equipment;
	q = "�ǂ���[�U���܂���? ";
	s = "�[�U���̑����i���Ȃ��B";

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) return FALSE;

	if (item >= 0) 	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	if(o_ptr->tval == TV_GUN) power *= 100;

	msg_format("%s�ɖ��͂𒍓������E�E",o_name);
	
	o_ptr->timeout -= power;
	if(o_ptr->timeout <= 0)
	{
		o_ptr->timeout = 0;
		msg_format("�[�U�����������B");
		p_ptr->window |= (PW_EQUIP);
	}

	return TRUE;
}

//���ׁA������g����p�̎�荞�ݍςݖ�����̏[�U���Z���[�`��
//�s���������Ƃ�TRUE
bool recharge_magic_eater(int power)
{
	int plev = p_ptr->lev;
	int item;
	int k_idx;
	int item_level;

	if(p_ptr->pclass != CLASS_SEIJA && p_ptr->pclass != CLASS_MAGIC_EATER)
		{ msg_print("ERROR:recharge_magic_eater()�����Ή��̐E�ŌĂ΂ꂽ"); return FALSE;}

	item = select_magic_eater(FALSE, TRUE);
	if (item == -1) return FALSE;
	if(item < EATER_EXT ) //��
	{
		if(p_ptr->magic_num1[item] == p_ptr->magic_num2[item]*EATER_CHARGE)
		{
			msg_print("���̏�͊��ɏ[�U����Ă���B");
			return FALSE;
		}

		k_idx = lookup_kind(TV_STAFF,item);
		item_level = k_info[k_idx].level;
		if(cheat_xtra) msg_format("power:%d level:%d",power,item_level);
		power /= (10+item_level);
		power = power * MIN(50,p_ptr->magic_num2[item]) / 10;
		if(item == SV_STAFF_THE_MAGI) power /= 10;//���҂̏���ꏈ��
		p_ptr->magic_num1[item] += power;
		msg_print("��ɖ��͂𒍂����񂾁E�E");
		if(p_ptr->magic_num1[item] >= p_ptr->magic_num2[item]*EATER_CHARGE)
		{
			p_ptr->magic_num1[item] = p_ptr->magic_num2[item]*EATER_CHARGE;
			msg_print("�[�U�����������B");

		}
		if(cheat_xtra) msg_format("num:%d",p_ptr->magic_num1[item] / 65535);

	}
	else if(item < EATER_EXT * 2 ) //���@�_
	{
		if(p_ptr->magic_num1[item] == p_ptr->magic_num2[item]*EATER_CHARGE)
		{
			msg_print("���̖��@�_�͊��ɏ[�U����Ă���B");
			return FALSE;
		}
		k_idx = lookup_kind(TV_WAND,item - EATER_EXT);
		item_level = k_info[k_idx].level;

		if(cheat_xtra) msg_format("power:%d level:%d",power,item_level);

		power /= (10+item_level);
		power = power * MIN(50,p_ptr->magic_num2[item]) / 10;
		p_ptr->magic_num1[item] += power;
		msg_print("���@�_�ɖ��͂𒍂����񂾁E�E");
		if(p_ptr->magic_num1[item] >= p_ptr->magic_num2[item]*EATER_CHARGE)
		{
			p_ptr->magic_num1[item] = p_ptr->magic_num2[item]*EATER_CHARGE;
			msg_print("�[�U�����������B");

		}
		if(cheat_xtra) msg_format("num:%d",p_ptr->magic_num1[item]/ 65535);
	}
	else if(item < EATER_EXT * 3) //���b�h
	{
		if(!p_ptr->magic_num1[item])
		{
			msg_print("���̃��b�h�͊��ɏ[�U����Ă���B");
			return FALSE;
		}
		k_idx = lookup_kind(TV_ROD,item - EATER_EXT*2);

		item_level = k_info[k_idx].level;
		if(cheat_xtra) msg_format("power:%d level:%d",power,item_level);
		power /= (10 + item_level);
		power = power * MIN(50,p_ptr->magic_num2[item]*5) / 10;
		p_ptr->magic_num1[item] -= (power / k_info[k_idx].pval);
		msg_print("���b�h�ɖ��͂𒍂����񂾁E�E");
		if(p_ptr->magic_num1[item] <= 0)
		{
			p_ptr->magic_num1[item] = 0;
			msg_print("�[�U�����������B");

		}

		if(cheat_xtra) msg_format("num:%d",p_ptr->magic_num1[item]);

	}
	else
	{
		msg_format("ERROR:select_magic_eater()�őz��O�̒l(%d)���Ԃ��ꂽ",item);
		return FALSE;
	}

	return TRUE;
}

//�V�q�̗v�΂��n�k���z���ł��邩�̃`�F�b�N TRUE���Ԃ�ƒn�k���N����Ȃ�
bool kanameishi_check(int add)
{
	if(p_ptr->pclass != CLASS_TENSHI)
	{
		return FALSE;
	}
	//�v�ΐݒu����Ă��Ȃ�
	if(!p_ptr->magic_num1[0] || !p_ptr->magic_num1[1]) return FALSE;
	if(!add) return FALSE;
	//�N�G�X�g�_���W�����ł̓J�E���g���i�܂Ȃ�
	if(p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) return FALSE;

	if(EXTRA_QUEST_FLOOR) return FALSE;

	p_ptr->magic_num1[2] += add;
	if(cheat_xtra) msg_format("�n�k:%d",p_ptr->magic_num1[2]);

	if(p_ptr->magic_num1[2] + randint1(50) < p_ptr->lev * 5)
	{
		msg_print("�n���̗v�΂��n�k�𕕂����߂��B");
	}
	else
	{
		msg_print("�n���̗v�΂��n�k�ɑς��؂ꂸ��ꂽ�I");
		kanameishi_break();
	}
	return TRUE;


}

//�V�q�̗v�΂����邩��������ĕK�v�Ȃ�n�k���N��������
//���̃_���[�W�Œn�k�Ō��̓G���|�ꂽ�ꍇ�̏����͖��Ȃ����H����
void kanameishi_break()
{
	int cx;
	int cy;
	int power;

	if(p_ptr->pclass != CLASS_TENSHI)
	{
		msg_format("ERROR:�V�q�ȊO��kanameishi_break()���Ă΂ꂽ");
		return;
	}
	if(p_ptr->is_dead) return;

	if(!p_ptr->magic_num1[0] || !p_ptr->magic_num1[1])
	{
		msg_format("ERROR:�v�΂��ݒu����Ă��Ȃ��̂�kanameish_break()���Ă΂ꂽ");
		return;
	}
	cx = p_ptr->magic_num1[1];
	cy = p_ptr->magic_num1[0];
	power = p_ptr->magic_num1[2] * 2;

	//���W�ƃJ�E���g�����Z�b�g
	p_ptr->magic_num1[0] = 0;
	p_ptr->magic_num1[1] = 0;
	p_ptr->magic_num1[2] = 0;

	//�v�΂���x�ł��n�k���z�����Ă���ꍇ�͒n�k���N����
	if(power)
	{
		int             i, t, y, x, yy, xx, dy, dx;

		int r = power / 8;
		int             sn = 0, sy = 0, sx = 0;
		bool            map[128][128];
		int damage;
		cave_type       *c_ptr;

		if(r > 60) r = 60;

		msg_print("�����������Ă����n�k��������ꂽ�I");

		for (y = 0; y < 128; y++)for (x = 0; x < 128; x++)map[y][x] = FALSE;


		/* Check around the epicenter */
		if(dun_level) for (dy = -r; dy <= r; dy++)
		{
			for (dx = -r; dx <= r; dx++)
			{
				yy = cy + dy;
				xx = cx + dx;

				if (!in_bounds(yy, xx)) continue;
				if (distance(cy, cx, yy, xx) > r) continue;

				c_ptr = &cave[yy][xx];
				///vault���ɂ͌��ʂ��Ȃ�
				if(c_ptr->info & CAVE_ICKY) continue;

				/* Lose room and vault */
				c_ptr->info &= ~(CAVE_ROOM | CAVE_UNSAFE);
				/* Lose light and knowledge */
				c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK | CAVE_KNOWN);
				/* Skip most grids */
				if (randint0(100) < 60) continue;
				if (player_bold(yy, xx)) continue;
				map[64+yy-cy][64+xx-cx] = TRUE;
			}
		}
		/* Examine the quaked region */
		for (dy = -r; dy <= r; dy++)
		{
			for (dx = -r; dx <= r; dx++)
			{
				yy = cy + dy;
				xx = cx + dx;

				if (!in_bounds(yy, xx)) continue;
				if (distance(cy, cx, yy, xx) > r) continue;

				/* Access the grid */
				c_ptr = &cave[yy][xx];

				if (c_ptr->m_idx == p_ptr->riding) continue;
				///vault���ɂ͌��ʂ��Ȃ�
				if(c_ptr->info & CAVE_ICKY) continue;
				/* Process monsters */
				if (c_ptr->m_idx)
				{
					monster_type *m_ptr = &m_list[c_ptr->m_idx];
					monster_race *r_ptr = &r_info[m_ptr->r_idx];


					///�V�q�̒n�k�_���[�W
					if((r_ptr->flags7 & RF7_CAN_FLY)
					|| (r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL))
					|| (r_ptr->flagsr & RFR_RES_ALL))
					{/*�������Ȃ�*/}
					else
					{
						//�n�k�_���[�W�@�|�ꂽ��continue
						if(project_m(0, 0, yy, xx, power + randint1(power), GF_ARROW, 0, TRUE)) continue;
					}
					if(!dun_level) continue;

					//������������Ȃ��n�`�͊֌W�Ȃ�
					if (!map[64+yy-cy][64+xx-cx]) continue;

					/* Quest monsters */
					if (r_ptr->flags1 & RF1_QUESTOR)
					{
						map[64+yy-cy][64+xx-cx] = FALSE;
						continue;
					}

					/* Most monsters cannot co-exist with rock */
					if (!(r_ptr->flags2 & (RF2_KILL_WALL)) &&
					    !(r_ptr->flags2 & (RF2_PASS_WALL)))
					{
						char m_name[80];

						/* Assume not safe */
						sn = 0;

					/* Monster can move to escape the wall */
					if (!(r_ptr->flags1 & (RF1_NEVER_MOVE)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Access the grid */
							y = yy + ddy_ddd[i];
							x = xx + ddx_ddd[i];

							/* Skip non-empty grids */
							if (!cave_empty_bold(y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if (is_glyph_grid(&cave[y][x])) continue;
							if (is_explosive_rune_grid(&cave[y][x])) continue;

							/* ... nor on the Pattern */
							if (pattern_tile(y, x)) continue;

							/* Important -- Skip "quake" grids */
							if (map[64+y-cy][64+x-cx]) continue;

							if (cave[y][x].m_idx) continue;
							if (player_bold(y, x)) continue;

							/* Count "safe" grids */
							sn++;

							/* Randomize choice */
							if (randint0(sn) > 0) continue;

							/* Save the safe grid */
							sy = y; sx = x;
						}
					}

					/* Describe the monster */
					monster_desc(m_name, m_ptr, 0);

					/* Scream in pain */
#ifdef JP
					if (!ignore_unview || is_seen(m_ptr)) msg_format("%^s�͔ߖ��グ���I", m_name);
#else
					if (!ignore_unview || is_seen(m_ptr)) msg_format("%^s wails out in pain!", m_name);
#endif

					/* Take damage from the quake */
					damage = (sn ? damroll(4, 8) : (m_ptr->hp + 1));

					/* Monster is certainly awake */
					(void)set_monster_csleep(c_ptr->m_idx, 0);

					/* Apply damage directly */
					m_ptr->hp -= damage;

					/* Delete (not kill) "dead" monsters */
					if (m_ptr->hp < 0)
					{
						/* Message */
#ifdef JP
						if (!ignore_unview || is_seen(m_ptr)) msg_format("%^s�͊�΂ɖ�����Ă��܂����I", m_name);
#else
						if (!ignore_unview || is_seen(m_ptr)) msg_format("%^s is embedded in the rock!", m_name);
#endif

						if (c_ptr->m_idx)
						{
							if (record_named_pet && is_pet(&m_list[c_ptr->m_idx]) && m_list[c_ptr->m_idx].nickname)
							{
								char m2_name[80];

								monster_desc(m2_name, m_ptr, MD_INDEF_VISIBLE);
								do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_EARTHQUAKE, m2_name);
							}
						}

						/* Delete the monster */
						delete_monster(yy, xx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if (sn)
					{
						int m_idx = cave[yy][xx].m_idx;

						/* Update the old location */
						cave[yy][xx].m_idx = 0;

						/* Update the new location */
						cave[sy][sx].m_idx = m_idx;

						/* Move the monster */
						m_ptr->fy = sy;
						m_ptr->fx = sx;

						/* Update the monster (new location) */
						update_mon(m_idx, TRUE);

						/* Redraw the old grid */
						lite_spot(yy, xx);

						/* Redraw the new grid */
						lite_spot(sy, sx);
					}
				}
			}
		}
	}
	if(!dun_level) return;

	/* Lose monster light */
	clear_mon_lite();

	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[64+yy-cy][64+xx-cx]) continue;

			/* Access the cave grid */
			c_ptr = &cave[yy][xx];

			/* Paranoia -- never affect player */
/*			if (player_bold(yy, xx)) continue; */

			/* Destroy location (if valid) */
			if (cave_valid_bold(yy, xx))
			{
				/* Delete objects */
				delete_object(yy, xx);

				/* Wall (or floor) type */
				t = cave_have_flag_bold(yy, xx, FF_PROJECT) ? randint0(100) : 200;

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					cave_set_feat(yy, xx, feat_granite);
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					cave_set_feat(yy, xx, feat_quartz_vein);
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					cave_set_feat(yy, xx, feat_magma_vein);
				}

				/* Floor */
				else
				{
					/* Create floor */
					cave_set_feat(yy, xx, floor_type[randint0(100)]);
				}
			}
		}
	}


	/* Process "re-glowing" */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if (!in_bounds(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			if (is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
			else if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS))
			{
				int ii, yyy, xxx;
				cave_type *cc_ptr;

				for (ii = 0; ii < 9; ii++)
				{
					yyy = yy + ddy_ddd[ii];
					xxx = xx + ddx_ddd[ii];
					if (!in_bounds2(yyy, xxx)) continue;
					cc_ptr = &cave[yyy][xxx];
					if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
					{
						c_ptr->info |= CAVE_GLOW;
						break;
					}
				}
			}
		}
	}


	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_MONSTERS);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);






	}



}



/*:::�������[�`������ �����J�n�ʒu�A�^�[�Q�b�g�A������A�C�e�������Ɍ��܂��Ă���*/
//mult:�����p���[ ��{1
//y,x: �����J�n���W
//ty,tx: �����^�[�Q�b�g(dir�𑗂�ƕ����񏈗��̂Ƃ��ʓ|)
//o_ptr:������A�C�e�� ���̒��ł͐��ʌ����A�폜�����͍s��Ȃ� ���ʂ͏��1�ł��邱��
//�g�[���n���}�[�Ȃǂ��߂��Ă��Ȃ�
//����������Ȃ������Ƃ�FALSE
//���ǂ񂰃��f�B�X���`�F�X�g�����������������A�C�e���̏d�ʌ��������͍s���Ȃ�
//���[�h 0:�����Ȃ� 1:���i�C�t����(�K��������) 2:���i�C�t�S����(�����̎w�ւ̌��ʔ���)
//���[�h 3:�f�t���[�V�������[���h(�{���啝���A����\���A�K������Ȃ�)
//���[�h4:���́u�����̒�������s�́v�����̎w�ւ̉e�����󂯂Ȃ�
//5:�n���R���l��2�����p�_�~�[�A�C�e�� �K��
//6:��{�����Y��
bool do_cmd_throw_aux2(int y, int x, int ty, int tx, int mult, object_type *o_ptr, int mode)
{
	int item;
	int i, j, prev_y, prev_x;
	int ny[19], nx[19];
	int chance, tdam, tdis;
	int mul, div, dd, ds;
	int cur_dis, visible;

	object_type forge;
	object_type *q_ptr;

	bool hit_body = FALSE;
	bool hit_wall = FALSE;
	bool suitable_item = FALSE;
	bool return_when_thrown = FALSE;

	char o_name[MAX_NLEN];

	int msec = delay_factor * delay_factor * delay_factor;

	u32b flgs[TR_FLAG_SIZE];
	cptr q, s;
	bool do_drop = TRUE;


	q_ptr = &forge;
	object_copy(q_ptr, o_ptr);
	object_flags(q_ptr, flgs);
	torch_flags(q_ptr, flgs);
	if(have_flag(flgs, TR_THROW)) suitable_item = TRUE;
	/* Description */
	object_desc(o_name, q_ptr, OD_OMIT_PREFIX);

	if(o_ptr->number != 1)
	{
		msg_format("ERROR:throw_aux2()�ŃA�C�e��%s�̌���1�ȊO",o_name);
		return FALSE;
	}

	/*:::���͓����̎w�ւ̌���*/
	if(p_ptr->mighty_throw)
	{
			//���̃i�C�t�S�������͋��͓����̌��ʂ�����
		if(mode == 1) mult += 1;
			//俎q�̓����͎w�ւ̌��ʂ𓾂��Ȃ�
		else if ( p_ptr->pclass != CLASS_SUMIREKO) ;
		else if ( mode == 4 || mode == 5 || mode == 6) ;
		else 
			mult += 2;

	}

	/* Extract a "distance multiplier" */
	mul = 10 + 2 * (mult - 1);

	/*:::�Œ�d��10�ɌŒ�@�d��10��1�|���h(0.5kg)����*/
	div = (((q_ptr->weight * 2) > 10) ? (q_ptr->weight * 2) : 10);
	if (have_flag(flgs, TR_THROW)) div /= 2;

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[p_ptr->stat_ind[A_STR]] + 20) * mul / div;

	/* Max distance of 10-18 */
	if (tdis > mul) tdis = mul;

	//俎q�͏d�ʂ𖳎�
	//�f�t���[�V�������[���h��������
	if(p_ptr->pclass == CLASS_SUMIREKO || mode == 3) tdis = 18;
	else if(mode == 5 || mode == 6) tdis = 18;
	else if(tdis > 18) tdis = 18;
	else if(tdis < 1) tdis = 1;

	if ( have_flag(flgs, TR_BOOMERANG))
		return_when_thrown = TRUE;

	//�s���͏�����͂��Ȃ�

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Chance of hitting */
	/*:::���������̕���͖�������ɕ��펩�g�̖������������*/
//俎q�̖������͒m�\�ˑ��ɂ��悤��?
	if (suitable_item) chance = ((p_ptr->skill_tht) +
		((p_ptr->to_h_b + q_ptr->to_h) * BTH_PLUS_ADJ));
	else chance = (p_ptr->skill_tht + (p_ptr->to_h_b * BTH_PLUS_ADJ));

	//�����X�L���ɂ�閽���␳ �ő��{
	if(suitable_item) chance += chance * ref_skill_exp(SKILL_THROWING) / 8000;
	else chance += chance * ref_skill_exp(SKILL_THROWING) / 16000;

	if(mode == 3) chance *= 3;

	//���|��Ԃ��Ɩ���������
	if(p_ptr->afraid) chance = chance * 2 / 3;

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	//�u���̐��E�v
	if(SAKUYA_WORLD)
	{
		sakuya_time_stop(FALSE);
	}

	/* Travel until stopped */
	/*:::���������̂���ԏ���*/
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny[cur_dis] = y;
		nx[cur_dis] = x;
		mmove2(&ny[cur_dis], &nx[cur_dis], py, px, ty, tx);

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_PROJECT))
		{
			hit_wall = TRUE;
			/*:::�ǂɓ������āA�������̂���A�l�`�A���邢�͕ǂ̒��Ƀ����X�^�[�������ꍇ���[�v�𔲂���*/
			if ((q_ptr->tval == TV_FIGURINE) || object_is_potion(q_ptr) || !cave[ny[cur_dis]][nx[cur_dis]].m_idx) break;
		}

		//��{�����Y�����ꏈ��
		if(mode == 6 && !cave[ny[cur_dis]][nx[cur_dis]].m_idx)
		{
			feature_type *f_ptr = &f_info[cave[ny[cur_dis]][nx[cur_dis]].feat];
			if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_LAVA))
			{
				msg_format("%s�͗n��ɒ���ł�����...",o_name);
				return TRUE;
			}
			if (cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_ACID_PUDDLE))
			{
				msg_format("%s�͎_�̏��ɒ���ł�����...", o_name);
				return TRUE;
			}
			if (cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_POISON_PUDDLE))
			{
				msg_format("%s�͓ł̏��ɒ���ł�����...", o_name);
				return TRUE;
			}

			if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_WATER))
			{
				msg_format("%s�͐����ɒ���ł�����...",o_name);
				return TRUE;
			}
			if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_STAIRS))
			{
				if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_MORE))				
					msg_format("%s�͊K�i���삯�~��Ă�����...",o_name);
				else
					msg_format("%s�͊K�i���삯�オ���Ă�����...",o_name);
				return TRUE;
			}
			if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_CAN_FLY) && !cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_MOVE))
			{
				msg_format("%s�͐[�����ɗ����Ă�����...",o_name);
				return TRUE;
			}
			if(have_flag(f_ptr->flags,FF_TRAP))
			{
				disclose_grid(ny[cur_dis], nx[cur_dis]);
				if(f_ptr->subtype == TRAP_TRAPDOOR)
				{
					msg_format("%s�͗��Ƃ��˂ɗ����Ă�����...",o_name);
					return TRUE;
				}
				else if(f_ptr->subtype == TRAP_PIT || f_ptr->subtype == TRAP_SPIKED_PIT || f_ptr->subtype == TRAP_POISON_PIT)
				{
					msg_format("%s�͗��Ƃ����ɗ������I",o_name);
					return TRUE;
				}
				else if(f_ptr->subtype == TRAP_TELEPORT)
				{
					msg_format("%s�͂ǂ����ɏ������B",o_name);
					return TRUE;
				}
				else if(f_ptr->subtype == TRAP_BLIND || f_ptr->subtype == TRAP_CONFUSE || f_ptr->subtype == TRAP_POISON || f_ptr->subtype == TRAP_SLEEP)
				{
					msg_format("%s�ɃK�X����������ꂽ�B",o_name);
				}
				else
				{
					msg_format("%s�̑����ŃJ�`���Ɖ��������������N����Ȃ������B",o_name);
				}
			}



		}

		/* The player can see the (on screen) missile */
		if (panel_contains(ny[cur_dis], nx[cur_dis]) && player_can_see_bold(ny[cur_dis], nx[cur_dis]))
		{
			char c = object_char(q_ptr);
			byte a = object_attr(q_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(c, a, ny[cur_dis], nx[cur_dis]);
			move_cursor_relative(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			if(mode != 3) 
			{
				lite_spot(ny[cur_dis], nx[cur_dis]);
				Term_fresh();
			}
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx[cur_dis];
		y = ny[cur_dis];

		/* Advance the distance */
		cur_dis++;

		/* Monster here, Try to hit it */
		if (cave[y][x].m_idx)
		{
			cave_type *c_ptr = &cave[y][x];

			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			int mon_ac = r_ptr->ac;

			//v1.1.94 �����X�^�[�h��͒ቺ����AC25%�J�b�g
			if (MON_DEC_DEF(m_ptr))
			{
				mon_ac = MONSTER_DECREASED_AC(mon_ac);
			}

			visible = m_ptr->ml;
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			//�n���R���l��2�Ɠ�{�����Y���͕K��
			if (test_hit_fire(chance - cur_dis, r_ptr->ac, m_ptr->ml) || mode == 5 || mode == 6)
			{
				bool fear = FALSE;

				if (!visible)
				{
					msg_format("%s���G��ߑ������B", o_name);
				}
				else
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, 0);
					if(mode == 3)
						msg_format("������%s��%s�Ɍ����Ď��k�����I", o_name, m_name);
					else
						msg_format("%s��%s�ɖ��������B", o_name, m_name);
					if (m_ptr->ml)
					{
						if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);
						health_track(c_ptr->m_idx);
					}
				}
				//v1.1.27 �e���Ԃ񓊂����Ƃ��͕ʏ����ɂ���
				if(q_ptr->tval == TV_GUN)
				{
					gun_throw_effect(y,x,q_ptr);
					//�����Əe�̗����̌o���l�𓾂邱�Ƃɂ��Ă���
					if(mode != 5 && mode != 6)
					{
						gain_skill_exp(SKILL_THROWING, m_ptr);
						gain_skill_exp(TV_GUN, m_ptr);
					}
				}
				else
				{

					dd = q_ptr->dd;
					ds = q_ptr->ds;
					torch_dice(q_ptr, &dd, &ds);

					if(suitable_item && object_is_artifact(q_ptr)) dd *= 2; //���������̃A�[�e�B�t�@�N�g�𓊂������̃{�[�i�X�ǉ�
				
					tdam = damroll(dd, ds);
					tdam = tot_dam_aux(q_ptr, tdam, m_ptr, 0, TRUE);
					tdam = critical_shot(q_ptr->weight, q_ptr->to_h, tdam);
					if (q_ptr->to_d > 0)
						tdam += q_ptr->to_d;
					else
						tdam += -q_ptr->to_d;

					if (suitable_item)
					{
						tdam *= (2+mult);
						tdam += p_ptr->to_d_m;
					}
					else
					{
						tdam *= mult;
					}

					if (tdam < 0) tdam = 0;

					///mod150620 �_���[�W�ɏd�ʃ{�[�i�X�����悤�ɂ��Ă݂�
					///mod160625 ������ƃ_�C�X�C��
					tdam += damroll(3, MAX(q_ptr->weight/10,1));

					tdam = mon_damage_mod(m_ptr, tdam, FALSE);

					if (p_ptr->wizard)
						msg_format("%d/%d�̃_���[�W��^�����B",  tdam, m_ptr->hp);

					//�����X�L���𓾂�
					if(mode != 5 && mode != 6) gain_skill_exp(SKILL_THROWING, m_ptr);

					if (mon_take_hit(c_ptr->m_idx, tdam, &fear, extract_note_dies(real_r_ptr(m_ptr))))
					{
						/* Dead monster */
					}

					/* No death */
					else
					{
						message_pain(c_ptr->m_idx, tdam);
						if ((tdam > 0) && !object_is_potion(q_ptr))
							anger_monster(m_ptr);

						if (fear && m_ptr->ml && mode != 5)
						{
							char m_name[80];
							sound(SOUND_FLEE);
							monster_desc(m_name, m_ptr, 0);
							msg_format("%^s�͋��|���ē����o�����I", m_name);

						}
					}
				}
			}

			break;
		}
	}
	if(mode == 3) 
	{
		p_ptr->redraw |= PR_MAP;
		redraw_stuff();
	}

	if (hit_body) torch_lost_fuel(q_ptr);

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(q_ptr) : 0);

	//�j�󗦐ݒ�
	if(mode == 1 || mode == 5 || mode == 6) j=100;

	if ((q_ptr->tval == TV_FIGURINE) && !(p_ptr->inside_arena))
	{
		j = 100;

		if (!(summon_named_creature(0, y, x, q_ptr->pval,  !(object_is_cursed(q_ptr)) ? PM_FORCE_PET : 0L)))
			msg_print("�l�`�͔P���Ȃ���ӂ��U���Ă��܂����I");
		else if (object_is_cursed(q_ptr))
			msg_print("����͂��܂�ǂ��Ȃ��C������B");
	}

	//v1.1.43 �I�j�t�X�x�͔j�􂵂č����{�[������
	if (q_ptr->tval == TV_MUSHROOM && q_ptr->sval == SV_MUSHROOM_PUFFBALL)
	{
		if (hit_body || hit_wall)
		{
			msg_format("%s�͔j�􂵂��I", o_name);
			project(0, 3, y, x, damroll(8, 12), GF_CONFUSION, (PROJECT_JUMP | PROJECT_KILL), -1);
			do_drop = FALSE;
		}
		else j = 0;
	}

	//���ǂ񂰃��f�B�X���`�F�X�g����
	if (q_ptr->tval == TV_COMPOUND && q_ptr->sval == SV_COMPOUND_MEDICINE_CHEST)
	{
		int potion_count = 0;
		int rad;

		do_drop = FALSE;

		//�ǉ��C���x���g�����[�v �܂��C���x���g�����̖�̐��𐔂���
		for (i = 0; i<INVEN_ADD_MAX; i++)
		{
			object_type *tmp_o_ptr = &inven_add[i];
			if (object_is_potion(tmp_o_ptr)) potion_count += tmp_o_ptr->number;

		}

		//��̐��ɉ����Ė�̃{�[���̎U��΂���ݒ�
		if (potion_count < 3) rad = 0;
		else if (potion_count < 20) rad = 1;
		else if (potion_count < 50) rad = 2;
		else if (potion_count < 100) rad = 3;
		else  rad = 4;

		//�ǉ��C���x���g�����[�v �C���x���g�����̃A�C�e�����������藎�Ƃ����肷��
		for (i = 0; i<INVEN_ADD_MAX; i++)
		{
			int k;

			object_type *tmp_o_ptr = &inven_add[i];

			if (!tmp_o_ptr->k_idx) continue;

			//�����_�~�[�A�C�e�������ɓ����Ă����ꍇ�������Ȃ��B�E�B�U�[�h�R�}���h�ł킴�Ƃ��Ȃ��ƋN���蓾�Ȃ����������[�v�h�~�Ɉꉞ����Ƃ�
			if (tmp_o_ptr->tval == TV_COMPOUND && tmp_o_ptr->sval == SV_COMPOUND_MEDICINE_CHEST) continue;

			//��Ȃ犄���
			if (object_is_potion(tmp_o_ptr))
			{

				int count = tmp_o_ptr->number;
				
				//���̖򂪈������鏈��
				for (k = 0; k<count; k++)
				{
					int cx, cy;

					scatter(&cy, &cx, y, x, rad, 0);
					//��̔��������B*����*�̖�͂�����Ƌ�������̂ŕ��ʂ̔����ɂ���
					if (tmp_o_ptr->tval == TV_COMPOUND && tmp_o_ptr->sval == SV_COMPOUND_STAR_DETORNATION)
						project(0, 7, cy, cx, (800 + randint1(800)), GF_NUKE, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID), -1);
					else
						potion_smash_effect(0, cy, cx, tmp_o_ptr->k_idx);
				}

			}
			//��ȊO�͂��̂܂ܒn�ʂɗ�����
			else
			{
				drop_near(tmp_o_ptr, 0, y, x);

			}
			//�d�ʌ���
			p_ptr->total_weight -= tmp_o_ptr->weight * tmp_o_ptr->number; 

			//�C���x���g���̃A�C�e��������
			object_wipe(&inven_add[i]);
		}
		

	}

	else if (object_is_potion(q_ptr))
	{
		if (hit_body || hit_wall || (randint1(100) < j))
		{
			if(mode != 5) msg_format("%s�͍ӂ��U�����I", o_name);

			if (potion_smash_effect(0, y, x, q_ptr->k_idx))
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];

				/* ToDo (Robert): fix the invulnerability */
				if (cave[y][x].m_idx &&
				    is_friendly(&m_list[cave[y][x].m_idx]) &&
				    !MON_INVULNER(m_ptr) &&
					m_ptr->r_idx != MON_MASTER_KAGUYA					
					)
				{
					char m_name[80];
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
					msg_format("%s�͓{�����I", m_name);
					set_hostile(&m_list[cave[y][x].m_idx]);
				}
			}
			do_drop = FALSE;
		}
		else
		{
			j = 0;
		}
	}
	/* Drop (or break) near that location */
	if (mode == 3)
	{
		msg_format("%s�͏��ł����B", o_name);
	}
	else if (do_drop)
	{
		if (cave_have_flag_bold(y, x, FF_PROJECT))
		{
			/* Drop (or break) near that location */
			(void)drop_near(q_ptr, j, y, x);
		}
		else
		{
			/* Drop (or break) near that location */
			(void)drop_near(q_ptr, j, prev_y, prev_x);
		}
	}

	return TRUE;
}


//�_�R����ւ���锻��
bool unzan_guard(void)
{
	int chance = 15 + p_ptr->lev / 5;
	if(p_ptr->pclass != CLASS_ICHIRIN)	return FALSE;
//�ŏI�Z�̔������͌��ʂ��o�Ȃ��H
	if(p_ptr->chp < p_ptr->mhp / 2) chance += 50 * (p_ptr->mhp - p_ptr->chp)  / p_ptr->mhp ;

	if(p_ptr->afraid || p_ptr->paralyzed || p_ptr->confused || p_ptr->alcohol >= DRANK_3) chance += 30;

	if(chance > 80) chance = 80;

	if(inventory[INVEN_RARM].k_idx) chance /= 2;
	if(inventory[INVEN_LARM].k_idx) chance /= 2;

	if(randint1(100) < chance)
	{
		msg_print("�_�R�����Ȃ���������I");
		return TRUE;
	}
	else	return FALSE;
}
/*:::���g���U���N�V����*/
void mokou_resurrection(void)
{
		int heal_mult = MIN(100,p_ptr->csp * 2);
		int dam = (p_ptr->lev * 8 + adj_general[p_ptr->stat_ind[A_CHR]] * 10 + 300);

		msg_format("�w -���U���N�V����- �x");
		flag_mokou_resurrection = FALSE;
		p_ptr->magic_num1[0] += 1; //�����񐔃J�E���g

		set_poisoned(0);
		set_confused(0);
		set_paralyzed(0);
		set_blind(0);
		set_stun(0);
		set_cut(0);
		set_image(0);
		do_res_stat(A_STR);
		do_res_stat(A_INT);
		do_res_stat(A_WIS);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);
		do_res_stat(A_CHR);
		set_alcohol(0);
		restore_level();
		dispel_player();
		set_asthma(0);

		//���S�����U���N�V������MP�������
		if(p_ptr->chp < 0)
		{
			p_ptr->csp -= 50;
			if(p_ptr->csp < 0) p_ptr->csp = 0;
		}
		p_ptr->chp = MAX(p_ptr->chp,(p_ptr->mhp * heal_mult / 100));
		//msg_format("mult:%d hp:%d mp:%d",heal_mult,p_ptr->chp,p_ptr->csp);
		project(0,(1 + p_ptr->lev / 16),py,px,dam,GF_FIRE, (PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID),-1);
		inven_damage(set_fire_destroy, 5);
		p_ptr->redraw |= (PR_MANA | PR_HP);
		handle_stuff();

}

/*:::���݂̐_�ގq�̃p���[�A�b�v�x�����𓾂�*/
//p_ptr->magic_num1[0]��M�l�Ɏg���B
//p_ptr->magic_num2[0,1]���{�X�œ|�t���O�Ɏg���B
int kanako_rank(void)
{
	if(p_ptr->pclass != CLASS_KANAKO)
	{
		msg_print("ERROR:�_�ގq�ȊO��kanako_rank()���Ă΂ꂽ");
		return 0;
	}

	if(p_ptr->magic_num1[0] >= 15000) return 7;
	if(p_ptr->magic_num1[0] >= 8000) return 6;
	if(p_ptr->magic_num1[0] >= 4500) return 5;
	if(p_ptr->magic_num1[0] >= 2500) return 4;
	if(p_ptr->magic_num1[0] >= 1200) return 3;
	if(p_ptr->magic_num1[0] >= 500) return 2;
	if(p_ptr->magic_num1[0] >= 200) return 1;
	else return 0;
}

/*:::���݂̐_�ގq�̃p���[�A�b�v�x�����̃R�����g�𓾂�*/
cptr kanako_comment(void)
{
	if(p_ptr->pclass != CLASS_KANAKO)
	{
		msg_print("ERROR:�_�ގq�ȊO��kanako_comment()���Ă΂ꂽ");
		return "";
	}
	switch(kanako_rank())
	{
	case 0:
		return "�قƂ�ǐM����Ă��Ȃ��E�E";
	case 1:
		return "������ƐM���W�܂��Ă����C������B";
	case 2:
		return "�����͐M�����悤�ɂȂ��Ă����悤���B";
	case 3:
		return "�R�����łȂ��l���ł���������Ă����炵���B";
	case 4:
		return "�l�X�Ȑl�d����̐M���W�܂��Ă���̂�������E�E";
	case 5:
		return "�������z���ł��Ȃ��̖���m��ʂ��̂͂��Ȃ��B";
	case 6:
		return "���Ȃ��͍��⌶�z���ōł��M���W�߂Ă���_�l���B";
	case 7:
		return "���Ȃ��͂��Ɍ��z���̐M�S�Ă��蒆�ɂ����I";
	default:
		return format("ERROR:�_�ގq���b�Z�[�W(rank:%d)���o�^����Ă��Ȃ�",kanako_rank());

	}
}


void kanako_get_point(monster_type *m_ptr)	
	{
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		int kanako_old_rank = kanako_rank();
		int get_point = 0;
		int need_level_list[8] = {0, 10, 20, 30, 40, 50, 60, 0}; //�e�����N�ŐM�l�𓾂邽�߂ɍŒ���K�v�ȓG���x��
		byte kanako_old_beat1 = p_ptr->magic_num2[0];
		byte kanako_old_beat2 = p_ptr->magic_num2[1];

		if(m_ptr->r_idx < 1)
			{msg_print("ERROR:kanako_get_point()��r_idx����������"); return; }

		if(p_ptr->pclass != CLASS_KANAKO)
			{msg_print("ERROR:�_�ގq�ȊO��kanako_get_point()���Ă΂ꂽ"); return; }

		if(!(r_ptr->flags1 & RF1_UNIQUE || (r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags7 & RF7_VARIABLE)))
			return;


		if(m_ptr->r_idx == MON_BYAKUREN)
		{
			get_point += 300;
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_HIZIRIN;
		}
		if(m_ptr->r_idx == MON_MIKO)
		{
			get_point += 200; //*2
		 	p_ptr->magic_num2[0] |= KANAKO_BEAT_0_MIMIMI;
		}
		if(m_ptr->r_idx == MON_REIMU && r_ptr->level > 89)
		{
			 if(!(p_ptr->magic_num2[0] & KANAKO_BEAT_0_REIMU)) get_point += 500;
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_REIMU;
		}
		if(m_ptr->r_idx == MON_G_CTHULHU)
		{
			get_point += 300; //*2
			p_ptr->magic_num2[0] |= KANAKO_BEAT_0_CTHULHU;
		}

		if(m_ptr->r_idx == MON_OBERON)
		{
			get_point += 250; //*2
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_OBERON;
		}
		if(m_ptr->r_idx == MON_AZATHOTH)
		{
			get_point += 500; //*2
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_AZAT;
		}
		if(m_ptr->r_idx == MON_MORGOTH)
		{
			get_point += 400; //*2
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_MORGOTH;
		}
		if(m_ptr->r_idx == MON_SERPENT) 
		{
			get_point += 3000;
			p_ptr->magic_num2[0] |= KANAKO_BEAT_0_J;
		}

		if(kanako_old_rank == 7) 
		{
			get_point = 0;
		}
		else if(r_ptr->level < need_level_list[kanako_old_rank])
		{
			 if(!get_point) msg_print("�������̃��x���̓G��|���Ă��M�𓾂��Ȃ��悤���B");
		}
		else
		{
			get_point += r_ptr->level;
		}
		if(r_ptr->flags3 & RF3_DEITY) get_point *= 2;

		p_ptr->magic_num1[0] += get_point;
		if(cheat_xtra && get_point) msg_format("�_�ގq�M�l%d(+%d)",p_ptr->magic_num1[0],get_point);

		if(kanako_old_beat1 != p_ptr->magic_num2[0] || kanako_old_beat2 != p_ptr->magic_num2[1] ||  kanako_old_rank != kanako_rank())
		{
			msg_print("�M�������ė͂��������C������I");
			p_ptr->update |= (PU_BONUS | PU_HP);
			p_ptr->redraw |= (PR_STATS | PU_HP | PU_MANA);
		}

	}


///mod150719 �z�s�̎M���菈��
bool futo_break_plate(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	if(!is_plate_grid(c_ptr))
	{
		return FALSE;
	}

	c_ptr->info &= ~(CAVE_MARK);
	c_ptr->info &= ~(CAVE_OBJECT);
	c_ptr->mimic = 0;
	note_spot(y, x);
	lite_spot(y, x);
	return TRUE;

}

///mod150719 �z�s�ꕔ���Z�̃^�[�Q�b�g�����I�菈��
//���E���ɎM������Έ�ԋ߂��M���A�Ȃ���Έ�ԋ߂��G��target�ɃZ�b�g�A������Ȃ����FALSE
bool futo_determine_target(int cy, int cx, int *ty, int *tx)
{
	int x, y, i;
	bool flag_ok = FALSE;
	int dist = 99;
	monster_type *m_ptr;


	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(distance(cy,cx,y,x) > MAX_RANGE) continue;
			if(!projectable(cy,cx,y,x)) continue;
			if(!is_plate_grid(&cave[y][x])) continue;
			if(distance(cy,cx,y,x) > dist) continue;

			dist = distance(cy,cx,y,x);
			flag_ok = TRUE;
			*ty = y;
			*tx = x;
		}
	}

	if(flag_ok) return TRUE;

	for (i = 1; i < m_max; i++)
	{
		m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (is_pet(m_ptr)) continue;
		if (is_friendly(m_ptr)) continue;
		if (!m_ptr->ml) continue;
		if (!projectable(cy, cx, m_ptr->fy,m_ptr->fx)) continue;
		if (!los(cy, cx, m_ptr->fy,m_ptr->fx)) continue;
		if (distance(cy,cx, m_ptr->fy,m_ptr->fx) > dist) continue;
		dist = distance(cy,cx, m_ptr->fy,m_ptr->fx);
		flag_ok = TRUE;
		*ty = m_ptr->fy;
		*tx = m_ptr->fx;

	}

	if(flag_ok) return TRUE;

	return FALSE;

}
//�z�s�p�@�M��z�u�ł���ꏊ
bool futo_can_place_plate(int y, int x)
{
	if(!cave_have_flag_bold((y), (x), FF_MOVE) && !cave_have_flag_bold((y), (x), FF_CAN_FLY) || cave_have_flag_bold((y), (x), FF_TRAP) || cave[y][x].info & CAVE_OBJECT)
		return FALSE;
	else
		return TRUE;

}



///mod150801 �X�^�[�T�t�@�C�A�̔��e�J�E���g�_�E���Ɣ���
//v1.1.14 ���e�̔���+�U������
//project()�̍ċA�̂��߂��Ǝv����t���[�Y���N�������̂ō�蒼�����B
//���e�̃J�E���g�_�E���l���}�C�i�X�ɂȂ�ƌĂ΂��B
//���e�̗U���̓J�E���g���Z�łȂ��K���N���邱�Ƃɂ����B
//x,y:�������锚�e��������W

void bomb_count(int y, int x, int num)
{
	cave_type       *c_ptr;
	//�������锚�e�̏ꏊ���i�[�@������[0]����n�܂�
	byte explode_array_x[MAX_HGT * MAX_WID];
	byte explode_array_y[MAX_HGT * MAX_WID];
	//�������锚�e�����v
	s16b explode_num_total = 1;
	//�������锚�e�̂����A�U�����肪�ς񂾐�
	s16b explode_num_induce_check = 0;

	bool flag_seen = FALSE;
	int xx,yy,cx,cy,i;
	int rad = 3; //���e�̗U�����蔼�a

	c_ptr = &cave[y][x];
	if (!((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_BOMB)))
	{
		return;
	}
	//v1.1.14 �J�E���g�_�E�������łɍς�ł��锚�e�ɂ͉������Ȃ��B���łɗU�����X�g�ɓ����Ă���͂��B
	if(c_ptr->special <= 0) return;

	//�J�E���g�_�E����i�߁A0�ȉ��ɂȂ�������
	c_ptr->special += num;
	if(c_ptr->special > 999) c_ptr->special = 999;

	if(c_ptr->special > 0) return;

	if(player_can_see_bold(y,x)) flag_seen = TRUE;

	explode_array_x[0] = x;
	explode_array_y[0] = y;

	//�U�����薢�����̔��e���ƂɃ��[�v
	while(1)
	{
		int rad_tmp;
		cx = explode_array_x[explode_num_induce_check];
		cy = explode_array_y[explode_num_induce_check];

		for(rad_tmp=1;rad_tmp <= rad;rad_tmp++)
		{
			for(xx = cx - rad_tmp;xx <= cx + rad_tmp; xx++)
			{
				for(yy = cy - rad_tmp;yy <= cy + rad_tmp; yy++)
				{
					//���S����U�����肷�邽�ߏ��X��ȃ��[�v������
					if(cx == xx && cy == yy) continue;
					if(!in_bounds(yy,xx)) continue;
					if(rad_tmp > 1 && distance(yy,xx,cy,cx) != rad_tmp) continue;

					c_ptr = &cave[yy][xx];
					//���`�F�b�N���Ă��锚�e����ː����ʂ�ꏊ�ɔ��e������Δ������͂��ėU��
					if(!(c_ptr->info & CAVE_OBJECT) || !have_flag(f_info[c_ptr->mimic].flags, FF_BOMB))continue;
					if(rad_tmp > 1 && !projectable(yy,xx,cx,cy)) continue;
					//�J�E���g�_�E�����I����Ă��锚�e�͑ΏۊO�B���łɔ������X�g�ɓ����Ă���͂��Ȃ̂ŁB
					if(c_ptr->special <= 0) continue;

					//�������X�g�ɒǉ����J�E���g�_�E����0�ɂ��Ĕ����������Z
					c_ptr->special = 0;
					explode_array_x[explode_num_total] = xx;
					explode_array_y[explode_num_total] = yy;
					explode_num_total++;

					//�v���C���[���猩���Ă���t���O
					if(!flag_seen && player_can_see_bold(yy,xx)) flag_seen = TRUE;
				}
			}
		}

		//�U�����肪�U���܂ޑS�Ă̔��e�ōς񂾂烋�[�v�𔲂���
		explode_num_induce_check++;
		if(explode_num_total == explode_num_induce_check) break;
	}

	disturb(1,1);
	//msg
	if(flag_seen)
	{
		if(explode_num_total < 4) msg_print("���e�����������I");
		else if(explode_num_total < 10) msg_print("�������̔��e���U�������I");
		else  msg_print("���e�̎R���唚�������I");
	}
	//���i�`�����\�͂��g���Ă���Ƃ��͔������������Ȃ�
	else if(!((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] || p_ptr->tim_general[1])))
	{
		if(explode_num_total < 2) msg_print("���������������B");
		else if(explode_num_total < 10) msg_print("�������̔������������I");
		else  msg_print("����Ȕ�������n��h�邪�����I");
	}
	else
	{
		msg_print("�_���W�����̋�C���k�����C������...");
	}

	//���X�g�ɓo�^�������e�̔�������
	for(i=0;i<explode_num_total;i++)
	{
		cx = explode_array_x[i];
		cy = explode_array_y[i];

		if(!in_bounds(cy,cx) || !(cave[cy][cx].info & CAVE_OBJECT) || !have_flag(f_info[cave[cy][cx].mimic].flags, FF_BOMB))
		{
			msg_format("ERROR:���e�������� array[%d],(%d,%d)����������",i,cx,cy);
			return;
		}
		c_ptr = &cave[cy][cx];
		c_ptr->info &= ~(CAVE_MARK);
		c_ptr->info &= ~(CAVE_OBJECT);
		c_ptr->mimic = 0;
		c_ptr->special = 0;
		note_spot(cy, cx);
		lite_spot(cy, cx);

		//v1.1.21 ���m
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_POWERUP_BOMB))
			project(PROJECT_WHO_EXPLODE_BOMB, 4, cy, cx, 150 + p_ptr->lev * 6,GF_ROCKET, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
		else
			project(PROJECT_WHO_EXPLODE_BOMB, 3, cy, cx, 100 + p_ptr->lev * 4,GF_ROCKET, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
	}





}

/*:::�A���X�̐l�`�̍U���񐔂��v�Z����B�Œ�l�͏��1�Ŏ��ۂɂ��̐l�`���g���郌�x���łȂ��ꍇ��0�B*/
/*:::�C���x���g�������ɂ�max_lev���g�����߁A���x���A�b�v�Ől�`�����������ƃ��x���_�E�����Ă����ʂɍU���\�B*/
int calc_doll_blow_num(int doll_num)
{
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	int add_blows = 0;
	int add_blows_tmp = 0;
	int i;
	int num;
	int doll_total_num;
	int id;

	if(p_ptr->pclass != CLASS_ALICE)
	{
		msg_format("ERROR:�A���X�ȊO��calc_doll_blow_num()���Ă΂ꂽ");
		return 0;
	}

	doll_total_num = calc_inven2_num();	

	/*:::�܂��g���Ȃ��l�`��0*/
	if(doll_total_num < doll_num + 1) return 0;

	o_ptr = &inven_add[doll_num];
	/*:::�Y���̐l�`������𑕔����Ă��Ȃ��ꍇ0*/
	if(!o_ptr->k_idx) return 0; 
	if(!object_is_melee_weapon(o_ptr)) return 0;
	//�Őj�͏��1��U��
	if(o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DOKUBARI) return 1;

	/*:::�Y���̐l�`���������Ă��镐��̒ǉ��U���K�p*/
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_BLOWS)) add_blows += o_ptr->pval;
	
	/*:::�A���X�̖h��ƃA�N�Z�T���̒ǉ��U����K�p pval+1�ɂ���́A��C���珇�Ԃɑ�����*/
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];
		if (!o_ptr->k_idx) continue;
		/*:::�ϐg�Ŗ���������Ă��鑕�����ʂ�e���B �֌W�Ȃ��͂������ꉞ*/
		if(check_invalidate_inventory(i)) continue;
		object_flags(o_ptr, flgs);
		if (have_flag(flgs, TR_BLOWS)) add_blows_tmp += o_ptr->pval * 2;
	}
	add_blows += add_blows_tmp / doll_total_num + ((add_blows_tmp % doll_total_num) > doll_num)?1:0;


	/*:::(INT+DEX�l���U���񐔌v�Z��b�l�ɂ���*/
	id = p_ptr->stat_ind[A_INT] + 3 + p_ptr->stat_ind[A_DEX] + 3;
	switch(doll_num)
	{
	//��C id80��4,100��6
	case INVEN_DOLL_SHANHAI:
		if(id > 80) num = (4 + (id-80) / 8);
		else num = (id+10) / 20;
		break;
	//�H�� id80��4,100��6 ������C���オ��̂��x��
	case INVEN_DOLL_HORAI:
		if(id > 80) num = (4 + (id-83) / 8);
		else num = (id+5) / 20;
		break;

	//���V�A id80��3,100��4
	case INVEN_DOLL_RUSSIA:
		if(id > 80) num = (3 + id / 85);
		else num = (id+15) / 25;
		break;
	//�����h�� id80��3,100��4 �t�����X���x��
	case INVEN_DOLL_LONDON:
		if(id > 80) num = (3 + id / 90);
		else num = (id+10)/25;
		break;
	//�I�����A�� id80��4,100��6 �����x��
	case INVEN_DOLL_ORLEANS:
		if(id > 80) num = (3 + id / 95);
		else num = (id+5) / 25;
		break;
		//�� id80��3,100��4 �����h�����x��
	case INVEN_DOLL_KYOTO:
		if(id > 80) num = (3 + id / 100);
		else num = (id) / 25;
		break;
	//�S���A�e id80��2,100��4
	case INVEN_DOLL_GOLIATH:
		if(id > 80) num = 2 + (id-80) / 10;
		else num = (id) / 35;
		break;
	default:
		msg_format("ERROR:calc_doll_blow_num()�ɂ����Ė���`�̐l�`(%d)���Ă΂ꂽ",doll_num);
		return 0;
	}
	num += add_blows;
	//�d�ʒ��߃y�i���e�B
	if(heavy_armor()) num = MAX(1,num * 3 / 4 );
	//�Иr�����y�i���e�B
	if(inventory[INVEN_LARM].k_idx) num = MAX(1,num * 3 / 4 );
	if(num < 1) return 1;

	return num;

}


///mod150811 ������
/*:::�������ɂ��f�ނ���̖��̓G�b�Z���X���o �ƂŒ��o���Ȃ��Ƃ��͗ʂ���������@�s���������Ƃ�TRUE*/
bool marisa_extract_material(bool in_home)
{

	int choice;
	int item, item_pos;
	int amt;
	s32b price, value, dummy;
	object_type forge;
	object_type *q_ptr;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int i, base_point, total_point;

	q = "�����疂�͂𒊏o���悤���H";
	s = "���@�̍ޗ��ɂȂ肻���Ȃ��̂������Ă��Ȃ��B";

	item_tester_hook = marisa_will_buy;
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0) 	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];

	amt = 1;
	if (o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return FALSE;
	}
	q_ptr = &forge;
	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;

	object_desc(o_name, q_ptr, 0);

	if(!get_check(format("%s���璊�o���܂����H",o_name))) return FALSE;

	marisa_gain_power(q_ptr,(in_home)?100:(p_ptr->lev + 30));

	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
	return TRUE;

}

/*:::�������̖��@�̌��ʁA�K�v�G�b�Z���X���m�F����*/
bool check_marisa_recipe(void)
{
	int num;
	char buf[1024];
	int i;

	while(1)
	{
		int j;
		cptr info_ptr;
		num = choose_marisa_magic(CMM_MODE_CHECK);
		if(num < 0) return FALSE;

		screen_save();

		//�������������łȂ����ŉ�ʃN���A����̈��ς��Ă����B���screen_save()����character_icky++�����̂�TRUE/FALSE�ł͔���ł��Ȃ��B
		if(character_icky > 1)
			clear_bldg(4,22);
		else
			for(i=4;i<22;i++) Term_erase(17, i, 255);

		prt(format("%s%s",marisa_magic_table[num].name,marisa_magic_table[num].is_drug?"����":""),4,18);
		sprintf(buf,"%s",marisa_magic_table[num].info);
		prt(format("%s",use_marisa_magic(num,TRUE)), 5, 18);

		//rumor_new�Ɠ���\\�ɂ�鎩�����s
		for(i=0;i < sizeof(buf)-2;i++)
		{
			if(buf[i] == '\0')	break; 
			/*:::�ꕔ���������s��������Ă��܂��̂�}�~*/
			else if(iskanji(buf[i]))
			{
				i++;
				continue;
			}
			else if(buf[i] == '\\') buf[i] = '\0';
		}
		buf[i] = '\0';
		buf[i+1] = '\0';
		info_ptr = buf;
		for (i = 0; i< 4; i++)
		{
			if(!info_ptr[0]) break;
			prt(info_ptr, 7+i, 18);
			info_ptr += strlen(info_ptr) + 1;
		}
		prt("�K�v�Ȗ���:", 12, 18);
		j=13;
		for(i=0;i<8;i++)
		{
			if(!marisa_magic_table[num].use_magic_power[i]) continue;
			prt(format("%s�̖���(%d)",marisa_essence_name[i],marisa_magic_table[num].use_magic_power[i]),j++,18);
		}


		prt("�����L�[�������Ă�������", 0, 0);
		(void)inkey();
		prt("", 0, 0);

		screen_load();
	}


	return TRUE;
}


void msg_marisa_magic(int num)
{
	if(marisa_magic_table[num].is_drug)
	{
		if(one_in_(4)) msg_format("�|�P�b�g����%s�����o���Ĉ��񂾁B",marisa_magic_table[num].name);
		else if(one_in_(3)) msg_format("�X�q�̒�����%s�����o���Ĉ��񂾁B",marisa_magic_table[num].name);
		else if(one_in_(2)) msg_format("������%s�����o���Ĉ��񂾁B",marisa_magic_table[num].name);
		else msg_format("�X�J�[�g�̒�����%s�����o���Ĉ��񂾁B",marisa_magic_table[num].name);
	}
	else 
		msg_format("%s��������I",marisa_magic_table[num].name);
}


/*:::�������̖��@���s��*/
/*:::num��marisa_magic_table[]�̓Y���ƈ�v���邱��*/
cptr use_marisa_magic(int num, bool only_info)
{
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int i;
	int plev = p_ptr->lev;
	int dir;
	
	switch(num)
	{
		case 0: //���T�t�@�C�A 500-750
		{
			int dam = 300 + plev * 6 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_ball(GF_FIRE, dir, dam, -2);
		}
		break;
		case 1: //�|�����X���j�[�N 320-400
		{
			int dam = 200 + plev * 3 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_rocket(GF_TELEKINESIS, dir, dam, 2);
		}
		break;

		case 2: //�i���[�X�p�[�N 450-700
		{
			int dam = 300 + plev * 5 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_beam(GF_ELEC, dir, dam);
		}
		break;

	/*
		case 2: //�~���b�p���T�[ 250-1000?
		{
			int x, y;
			char m_name[160];
			int dam = 30 + MAX(p_ptr->to_d[0],p_ptr->to_d[1]);
			int num = 3 + plev / 12;
			if(only_info) return format("����:%d * %d",dam, num);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
//�g���b�v�E�h�A�E�G�E�p�^�[���Evault���œ���m�F
			msg_marisa_magic(num);
			for(i=0;i<num;i++)
			{
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!in_bounds(y,x))
				{
					break;
				}
				else if(cave[y][x].m_idx)
				{
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
					msg_format("%s�ɑ̓����肵���I",m_name);
					project(0, 0, y, x, dam, GF_DISP_ALL , (PROJECT_JUMP | PROJECT_KILL), -1);
				}
				else if(player_can_enter(cave[y][x].feat, 0))
				{
					teleport_player_to(y, x, TELEPORT_NONMAGICAL);
				}
				else break;
			}
		}
		break;
*/
		case 3: //�X�e���~�T�C�� 400-650
		{
			int dam = 200 + plev * 6 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_ball_jump(GF_METEOR, dir, dam, 1, NULL);
		}
		break;

		case 4: //�E�B�b�`���C���C�� 300-400
		{
			int len = 10 + plev / 10;
			int damage = 150 + plev * 5;
			if(only_info) return format("�˒�:%d ����:%d",len,damage);
			if (!rush_attack2(len,GF_MANA,damage,0)) return NULL;
			break;			
		}
		break;

		case 5: //�O
		{
			int heal = 300;
			
			if(only_info) return format("��:%d",heal);
			msg_marisa_magic(num);
			hp_player(heal);
			set_cut(0);	
			set_poisoned(0);
		}
		break;
		case 6: //���~�l�X�X�g���C�N 200-400
		{
			int sides = 50 + chr_adj * 5;
			int base = 100 + plev * 2 ;

			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_marisa_magic(num);
			fire_rocket(GF_LITE,dir,base + randint1(sides),1);

			break;
		}

		case 7: //�G�X�P�[�v�x���V�e�B300-400
		{
			int dam = 350 + plev * 6 + chr_adj * 8;
			if(only_info) return format("����:%d",dam/2);	
			msg_marisa_magic(num);
			project(0,1,py,px,dam,GF_MANA,(PROJECT_KILL|PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM),-1);
			teleport_player(20,TELEPORT_NONMAGICAL);
			break;
		}

		case 8: //�R�[���h�C���t�F���m 500-750
		{
			int dam = 300 + plev * 6 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_ball(GF_COLD, dir, dam, -2);
		}
		break;

		case 9: //�f�r���_���g�[�` 600-800
		{
			int len = 5 + plev / 16;
			int damage = 400 + plev * 5 + chr_adj * 5;
			if(only_info) return format("�˒�:%d ����:%d",len,damage);
	
			project_length = len;
			if (!get_aim_dir(&dir)) return NULL;
			msg_marisa_magic(num);
			if(!fire_ball_jump(GF_NETHER,dir,damage,2,NULL)) return NULL;			
			break;			
		}
		break;

		case 10: //�}�W�b�N�h���C���~�T�C�� 300-500
		{
			int dam = 200 + plev * 3 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			drain_life(dir,dam);
		}
		break;

		case 11: //�p���t���h���b�O 400-600
		{
			int len = 5 + plev / 16;
			int damage = 250 + plev * 4 + chr_adj * 5;
			if(only_info) return format("�˒�:%d ����:%d",len,damage);
			project_length = len;

			if (!get_aim_dir(&dir)) return NULL;
			msg_marisa_magic(num);
			if(!fire_ball_jump(GF_ACID,dir,damage,3,NULL)) return NULL;			
			break;	
		}
		break;



		case 12: //�h���S�����e�I 1200-1700
		{
			int len = 2 + plev / 24;
			int dice = 35 + p_ptr->lev / 2;
			int sides = 40 + chr_adj / 2;
			if(only_info) return format("����:%dd%d",dice,sides);
	
			if (!get_aim_dir(&dir)) return NULL;
			msg_marisa_magic(num);
			if(!fire_ball_jump(GF_NUKE,dir,damroll(dice,sides),4,NULL)) return NULL;			
			break;	
		}
		break;
		case 13: //�I�[�v�����j�o�[�X650-1000
		{
			int dam = 400 + plev * 6 + chr_adj * 10;
			if(only_info) return format("����:%d",dam);

			msg_marisa_magic(num);
			project_hack2(GF_METEOR,0,0,dam);
			break;
		}
		case 14: //�}�W�b�N�A�u�\�[�o�[ general[0]�̃J�E���^���g��
		{
			int time = 10 + plev / 5;
			if(only_info) return format("����:%d",time);
			msg_marisa_magic(num);
			set_tim_general(time,FALSE,0,0);

			break;
		}
		case 15: //�C�x���g�z���C�Y�� 1000-1500
		{
			int dam = 1500 + plev * 20 + chr_adj * 50 / 3;
			int rad = 3 + plev / 9;
			if(only_info) return format("���a:%d ����:�`%d",rad,dam/2);	
			msg_marisa_magic(num);
			project(0,rad,py,px,dam,GF_GRAVITY,(PROJECT_KILL|PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM),-1);
			break;
		}
		case 16: //���͒O
		{
			int mana = 9999;
			
			if(only_info) return format("");
			msg_marisa_magic(num);
			p_ptr->csp += mana;
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
		}
		break;
		case 17: //���͒O
		{
			int heal = 9999;
			
			if(only_info) return format("");
			msg_marisa_magic(num);
			hp_player(heal);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			set_image(0);
			restore_level();
			do_res_stat(A_STR);
			do_res_stat(A_INT);
			do_res_stat(A_WIS);
			do_res_stat(A_DEX);
			do_res_stat(A_CON);
			do_res_stat(A_CHR);
			set_alcohol(0);
			set_asthma(0);
		}
		break;
		case 18: //�X�^�[�_�X�g�����@���G1000-2000?
		{
			int rad = 2 + plev / 20;
			int damage = 170 + plev * 2 + chr_adj;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_marisa_magic(num);
			cast_meteor(damage, rad, GF_LITE);

			break;
		}


		case 19: //�I�[�����[�Y�\�[���[�V�X�e��
		{
			int time = 15 + plev / 2;
			if(only_info) return format("����:%d",time);
			msg_marisa_magic(num);
			set_tim_sh_fire(time,FALSE);
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);

			break;
		}


		case 20: //�r�b�O�N�����`
		{
			int rad = plev;
			if(only_info) return format("�͈�:%d",rad);

			msg_marisa_magic(num);
			destroy_area(py,px,rad,FALSE,FALSE,FALSE);
		}
		break;

		case 21: //�_�[�N�X�p�[�N 600-1000
		{
			int dice = 25 + p_ptr->lev / 2;
			int sides = 25 + chr_adj / 2;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			fire_spark(GF_DARK,dir, damroll(dice,sides),1);

		}
		break;

		case 22: //�I�J���e�B�V����
		{
			int power = 200 + plev * 4;
			if(only_info) return format("����:%d",power);
			mass_genocide(power,TRUE);

			break;
		}
		case 23: //�f�B�[�v�G�R���W�J���{�� 1200-1600
		{
			int rad = 4 + plev / 16;
			int tx,ty;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP;
			int len = 6 + plev / 16;
			int damage = 500 + plev * 16 + chr_adj * 10;
			if(only_info) return format("�˒�:%d ���a:%d ����:%d",len,rad, damage);
			project_length = len;

			if (!get_aim_dir(&dir)) return NULL;

			if ((dir == 5) && target_okay())
			{
				flg &= ~(PROJECT_STOP);
				tx = target_col;
				ty = target_row;
			}
			if(dir != 5 || (dir == 5) && !target_okay() || !in_bounds(ty,tx) || !projectable(py,px,ty,tx))
			{
				msg_print("���E���̈�_��_��Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}

			if(distance(py,px,ty,tx) <= rad && !get_check_strict("�^�[�Q�b�g���߂�����B�{���Ɏg���܂����H", CHECK_OKAY_CANCEL)) return NULL;

			msg_marisa_magic(num);
			project(PROJECT_WHO_DEEPECO_BOMB, 5, ty, tx, damage, GF_CHAOS, flg, -1);


			break;	
		}
		break;

		case 24: //�t�@�C�i���X�p�[�N 1600�Œ�
		{
			int dam = 1600;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_spark(GF_DISINTEGRATE,dir, dam,2);

		}
		break;

		case 25: //�I�[�����[�Y���j�o�[�X
		{
			int time = 14 + plev / 3;
			if(only_info) return format("����:%d",time);
			msg_marisa_magic(num);
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);
			set_ultimate_res(time, FALSE);
			set_magicdef(time,FALSE);


			break;
		}

		case 26: //�A���e�B���b�g�V���[�g�E�F�[�u 1500
		{
			int y,x;
			int typ = GF_PLASMA;
			int dice = plev;
			int sides = 20 + chr_adj;
			if(only_info) return format("����:%dd%d",dice,sides);

			msg_marisa_magic(num);
			for (y = 1; y < cur_hgt-1; y++)	for (x = 1; x < cur_wid-1; x++)
			{
				if(cave_have_flag_bold(y,x,FF_PERMANENT)) continue;
				print_rel('#', (0x30 + spell_color(typ)), y, x);
			}
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, delay_factor * delay_factor * delay_factor * 2);

			for (y = 1; y < cur_hgt-1; y++)	for (x = 1; x < cur_wid-1; x++)
			{
				int dam = damroll(dice,sides);
				if(cave_have_flag_bold(y,x,FF_PERMANENT)) continue;
				project_f(0,0,y,x,dam,typ);
				project_o(0,0,y,x,dam,typ);
				project_m(0,0,y,x,dam,typ,(PROJECT_KILL),FALSE);

			}
//			project_hack2(GF_PLASMA,dice,sides,0);
			break;
		}

		case 27: //�u���C�W���O�X�^�[ 3200
		{
			int tx, ty;
			int dam = 3200;
			int flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_THRU | PROJECT_FAST | PROJECT_MASTERSPARK | PROJECT_DISI | PROJECT_FINAL);
			if(only_info) return format("����:%d",dam);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			msg_marisa_magic(num);

			tx = px + 255 * ddx[dir];
			ty = py + 255 * ddy[dir];
			project(0, 2, ty, tx, dam, GF_BLAZINGSTAR, flg, -1);

		}
		break;


		default:
		if(only_info) return format("������");
		msg_format("ERROR:num%d�̖��@������������",num);
		return NULL;
	}
	return "";
}


//���s���ۓW��㩂ŏo�Ă���G��IDX�ƍ��W
int moon_vault_mon_list[][3] =
{
	//idx,y,x
	{MON_KAGUYA,5,1},
	{MON_EIRIN,5,3},
	{MON_UDONGE,3,5},
	{MON_TEWI,7,5},

	{MON_G_USAGI,3,7},
	{MON_G_USAGI,4,7},
	{MON_G_USAGI,5,7},
	{MON_G_USAGI,6,7},
	{MON_G_USAGI,7,7},
	{MON_G_USAGI,3,8},
	{MON_G_USAGI,4,8},
	{MON_G_USAGI,5,8},
	{MON_G_USAGI,6,8},
	{MON_G_USAGI,7,8},

	{MON_FAIRY_EX,3,12},
	{MON_FAIRY_EX,4,12},
	{MON_FAIRY_EX,5,12},
	{MON_FAIRY_EX,6,12},
	{MON_FAIRY_EX,7,12},
	{MON_FAIRY_EX,3,13},
	{MON_FAIRY_EX,4,13},
	{MON_FAIRY_EX,5,13},
	{MON_FAIRY_EX,6,13},
	{MON_FAIRY_EX,7,13},

	{MON_GETTOMECA,5,85},



	{0,0,0}

};

//�����������s���ۓW�ŋN�������
void activate_moon_vault_trap(void)
{
	int i, x, y;

	if(p_ptr->pclass != CLASS_MARISA || p_ptr->inside_quest != QUEST_MOON_VAULT)
	{
		msg_print("activate_moon_vault_trap()���\�����Ȃ��Ă΂��������");
		return;
	}
	if(quest[QUEST_MOON_VAULT].flags & QUEST_FLAG_SPECIAL)
		return;
	quest[QUEST_MOON_VAULT].flags |= QUEST_FLAG_SPECIAL;

	msg_format("�˔@�A�ْ��Ɍx�񉹂��苿�����I");
	world_monster = -1;
	do_cmd_redraw();

	/*�����X�^�[�S���E*/
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		if (!m_ptr->r_idx) continue;
		if (i == p_ptr->riding) continue;

		if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
		{
			char m_name[80];

			monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_MOVED, m_name);
		}
		delete_monster_idx(i);
	}
	/*���͈͂̊��X�A���[���ȂǑS�j��*/
	for(y=3;y<8;y++)for(x=2;x<87;x++)
	{
		remove_mirror(y,x);
		cave_set_feat(y,x,feat_floor);
	}
	/*:::�����X�^�[�z�u*/
	for(i=0;;i++)
	{
		int r_idx = moon_vault_mon_list[i][0];
		y = moon_vault_mon_list[i][1];
		x = moon_vault_mon_list[i][2];
		if(!r_idx) break;

		if (r_info[r_idx].flags1 & RF1_UNIQUE)
		{
			r_info[r_idx].cur_num = 0;
			r_info[r_idx].max_num = 1;
		}
		place_monster_aux(0, y, x, r_idx, (PM_NO_KAGE));
	}
	
	Term_xtra(TERM_XTRA_DELAY, 1000);
	world_monster = 0;
	p_ptr->redraw |= PR_MAP;
	redraw_stuff();
	msg_format("�ӂ�̕��͋C���ς�����C������E�E");

}



/*:::�v���Y�����o�[��p �������A�������̓L�������C�N���ɂǂ̃L�����ɂȂ邩�I������*/
int select_prism_class(void)
{

	int n,choose=0;
	char c;
	bool selected = FALSE;
	cptr namelist[3] = {"���i�T","��������","�����J"};

	screen_save();	

	put_str("�N�ƌ�サ�܂����H", 10, 10);

	while(1)
	{
		for(n=0;n<3;n++)
		{
			if(p_ptr->pclass == (CLASS_LUNASA+n)) continue;
			else put_str( format("%c)%s",'a'+n,namelist[n]), 12 + n, 10);
		}
		c = inkey();

		if (c == 'a')
		{
			if(p_ptr->pclass == CLASS_LUNASA) continue;
			choose = CLASS_LUNASA;
			break;
		}
		else if (c == 'b')
		{
			if(p_ptr->pclass == CLASS_MERLIN) continue;
			choose = CLASS_MERLIN;
			break;
		}
		else if (c == 'c')
		{
			if(p_ptr->pclass == CLASS_LYRICA) continue;
			choose = CLASS_LYRICA;
			break;
		}
		else if(c == ESCAPE)
		{
			choose = p_ptr->pclass;				
			break;
		}
	}

	screen_load();
	return choose;
}

/*::: Mega Hack - �v���Y�����o�[�o����p�]�E�R�}���h*/		
void  prism_change_class(bool urgency)
{
	int new_class, i;


	if(!CLASS_IS_PRISM_SISTERS)
	{
		msg_print("ERROR:prism_change_class()���O�o���ȊO�ŌĂ΂ꂽ");
		return;
	}

	if(urgency)
	{
		do
		{
			new_class = CLASS_LUNASA + randint0(3);
		}while(p_ptr->pclass == new_class);
	}
	else
	{
		new_class = select_prism_class();
		if(p_ptr->pclass == new_class) return;
	}

	if(urgency)
		msg_format("���Ȃ��̃s���`��%s�������ɗ����I",new_class < p_ptr->pclass?"�o":"��");

	msg_format("���Ȃ���%s�ƌ�サ���B",new_class < p_ptr->pclass?"�o":"��");

	//magic_num1/2��[0�`1]�͉̌n���Z�Ɏg�� �O�̂���[2]���󂯂Ƃ�

	//�e�L�����̋Z�\�n���x��p_ptr->magic_num1[3-66]�ɕێ�����
	for(i=0;i<SKILL_EXP_MAX;i++)
	{
		p_ptr->magic_num1[3 + i + SKILL_EXP_MAX * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->skill_exp[i];
		p_ptr->skill_exp[i] = p_ptr->magic_num1[3 + i + SKILL_EXP_MAX * (new_class - CLASS_LUNASA)];
	}

	//�ˑR�ψق��ێ� magic_num2[3-74]
	for(i=0;i<4;i++)
	{
		p_ptr->magic_num2[3+i      + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta1>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 4  + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta2>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 8  + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta3>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 12 + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta1_perma>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 16 + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta2_perma>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 20 + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta3_perma>>(i*8) & (0xFF);
	}
		p_ptr->muta1 = 0L;
		p_ptr->muta2 = 0L;
		p_ptr->muta3 = 0L;
		p_ptr->muta1_perma = 0L;
		p_ptr->muta2_perma = 0L;
		p_ptr->muta3_perma = 0L;
	for(i=0;i<4;i++)
	{
		p_ptr->muta1       |= (u32b)(p_ptr->magic_num2[3+i      + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta2       |= (u32b)(p_ptr->magic_num2[3+i +  4 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta3       |= (u32b)(p_ptr->magic_num2[3+i +  8 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta1_perma |= (u32b)(p_ptr->magic_num2[3+i + 12 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta2_perma |= (u32b)(p_ptr->magic_num2[3+i + 16 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta3_perma |= (u32b)(p_ptr->magic_num2[3+i + 20 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
	}

	p_ptr->magic_num2[75] = 1; 
	//��x�ł���サ����num2[75]�Ƀt���OON

	//�p�����[�^��
	for(i=0;i<6;i++) p_ptr->stat_cur[i] = p_ptr->stat_max[i];

	p_ptr->pclass = new_class;
	cp_ptr = &class_info[p_ptr->pclass];


	reset_tim_flags();
	set_food(PY_FOOD_FULL - 1);
	p_ptr->exp = p_ptr->max_exp;
	check_experience();

	p_ptr->update |= PU_BONUS | PU_HP | PU_MANA | PU_SPELLS;
	update_stuff();

	p_ptr->chp = p_ptr->mhp;
	p_ptr->chp_frac = 0;
	p_ptr->csp = p_ptr->msp;
	p_ptr->csp_frac = 0;

	//���O�A�E�ƕ\�L��������
	strcpy(player_class_name, class_info[p_ptr->pclass].f_title);
	if(p_ptr->pclass == CLASS_LUNASA)
		strcpy(player_name, "���i�T�E�v���Y�����o�[");
	else if(p_ptr->pclass == CLASS_MERLIN)
		strcpy(player_name, "���������E�v���Y�����o�[");
	else
		strcpy(player_name, "�����J�E�v���Y�����o�[");


}

/*:::�ʂ̃����X�^�[�ɕϐg����B�h���~�[�A�}�~�]�E�̓��Z*/
//v1.1.47�������ǉ� time�����ŌĂ΂ꂽ�ꍇ�͎��Ԑ����𖳎�����t���O�������Ƃɂ���
void metamorphose_to_monster(int r_idx , int time)
{
	int percen;
	monster_race *r_ptr = &r_info[r_idx];



	//paranoia
	if(r_info[r_idx].flagsr & RFR_RES_ALL)
	{
		msg_print("�ϐg�Ɏ��s�����B");
		return;
	}

	//v1.1.47 �m�F��߂�
	//if(!get_check_strict("�ϐg���܂����H", CHECK_OKAY_CANCEL)) return;


	//�ϐg���̃����X�^�[�̖��O���p�̈�Ɋi�[�@�����_�����j�[�N�l��
	//�ϐg���̃����X�^�[idx��p_ptr->metamor_r_idx�Ɋi�[����悤�ɂ������A�����_�����j�[�N���l������ƈˑR����͕K�v
	if(IS_RANDOM_UNIQUE_IDX(r_idx))
	{
		sprintf(extra_mon_name,"%s",random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]);
	}
	else
	{
		sprintf(extra_mon_name,"%s",r_name + r_ptr->name);
	}

	//�����X�^�[�̏���S�ē���
	lore_do_probe(r_idx);

	(void)COPY(&r_info[MON_EXTRA_FIELD],r_ptr,monster_race);

	percen = p_ptr->chp * 100 / p_ptr->mhp;
	//����t���O�𗧂Ă�
	p_ptr->special_defense |= SD_METAMORPHOSIS;
	//v1.1.47 time�����ŌĂ΂ꂽ�ꍇ���Ԑ����𖳎�����t���O�𗧂Ă�
	if (time < 0)
	{
		time = -time;
		p_ptr->special_flags |= SPF_IGNORE_METAMOR_TIME;

	}

	if(r_ptr->flags2 & RF2_GIGANTIC)
	{
		if(!set_mimic(time,MIMIC_METAMORPHOSE_GIGANTIC,FALSE)) return;
		msg_print("���̎p�ł͖��@���▂����A����Ɋ������g���Ȃ��B");
	}
	else if(is_gen_unique(r_idx) || my_strchr("hknopstuzAFGKLOPSTUVWY", r_ptr->d_char) || r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN))
	{
		if(!set_mimic(time,MIMIC_METAMORPHOSE_NORMAL,FALSE)) return;
	}
	else
	{
		if(!set_mimic(time,MIMIC_METAMORPHOSE_MONST,FALSE)) return;
		msg_print("���̎p�ł͖��@���▂����A����Ɋ������g���Ȃ��B");
	}

	p_ptr->chp = p_ptr->mhp * percen / 100;

	p_ptr->metamor_r_idx = r_idx; //v1.1.85 �ϐg���̃����X�^�[��r_idx��ێ�����
	

	p_ptr->update |= (PU_HP | PU_BONUS | PU_MANA | PU_TORCH);
	p_ptr->redraw |= (PR_HP | PR_MANA | PR_STATS | PR_ARMOR);
	update_stuff();




}

///mod150922
/*:::�{���ɍ��X�����AMP���񕜂��郋�[�`����Ɨ�����B
 *�C����������ߋ��̋L�q���u�������悤�B���p�ƂȂ�MP����˔j����E�ɒ��ӁB
 *����MP���ő�̏ꍇFALSE��Ԃ�
 */
bool player_gain_mana(int mana)
{
	if(p_ptr->csp >= p_ptr->msp) return FALSE;
	p_ptr->csp += mana;
	if(p_ptr->csp >= p_ptr->msp)
	{
		p_ptr->csp = p_ptr->msp;
		p_ptr->csp_frac = 0;

	}
	p_ptr->redraw |= (PR_MANA);
	return TRUE;
}




/*:::���̖�������@��̕ێ��ɂ�magic_num1[0]���g���B*/
void hina_gain_yaku(int yaku)
{
	int old_rank,new_rank;

	if(p_ptr->pclass != CLASS_HINA) return;
	if(!yaku) return;

	if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT1) old_rank = 0;
	else if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT2) old_rank = 1;
	else if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT3) old_rank = 2;
	else old_rank = 3;

	p_ptr->magic_num1[0] += yaku;
	if(p_ptr->magic_num1[0] < 0) p_ptr->magic_num1[0] = 0;
	if(p_ptr->magic_num1[0] > 99999) p_ptr->magic_num1[0] = 99999;

	if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT1) new_rank = 0;
	else if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT2) new_rank = 1;
	else if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT3) new_rank = 2;
	else new_rank = 3;


	if(old_rank != new_rank)
	{
		if(new_rank == 0)
		{
			if(!p_ptr->magic_num1[0])
				msg_print("��S�ė������B");
			else
				msg_print("���̒��x�̖�Ȃ�y���}�����߂�B");
		}
		else if(new_rank == 1 && old_rank > new_rank)
		{
			msg_print("������Ĉ����₷���Ȃ����B");
		}
		else if(new_rank == 1 && old_rank < new_rank)
		{
			msg_print("��������܂��Ă����悤���B");
		}
		else if(new_rank == 2 && old_rank > new_rank)
		{
			msg_print("��̕��S�����������܂����ė]���C�����B");
		}
		else if(new_rank == 2 && old_rank < new_rank)
		{
			msg_print("��d���B���ė]��������ӂ�n�߂��c");
		}
		else if(new_rank == 3)
		{
			msg_print("��ʂ̖����������Ȃ��I���̂܂܂ł͑�ςȂ��ƂɂȂ�I");
		}
	}

	p_ptr->update |= PU_BONUS;
	p_ptr->redraw |= PR_ARMOR;
}

//���̐��E�@open=TRUE�ŊJ�n
void sakuya_time_stop(bool open)
{
	if(open && !SAKUYA_WORLD)
	{
		msg_print("���Ȃ��͎��Ԃ̗��ꂩ��O�ꂽ�B");
		p_ptr->special_defense |= SD_SAKUYA_WORLD;
	}
	else if(!open && SAKUYA_WORLD)
	{
		msg_print("�Ăю��Ԃ̗���ɏ�����B");
		p_ptr->special_defense &= ~(SD_SAKUYA_WORLD);
	}
	else return;

	p_ptr->redraw |= (PR_MAP | PR_STATUS);
	p_ptr->update |= (PU_MONSTERS);
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
	handle_stuff();

}



//�x��2�@�x���ɏ]���ăL�����Z������ꍇFALSE���Ԃ�@�x���Ɏ�������͊O�ŏ�������
bool process_warning2(void)
{
	object_type *o_ptr = choose_warning_item();
	char o_name[MAX_NLEN];

	//�x����ԂłȂ��Ƃ��������Ȃ�
	if(!p_ptr->warning) return TRUE;

	if (o_ptr)
	{
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		msg_format("%s���s���k�����I", o_name);
		}
	else
	{
		if(p_ptr->pclass == CLASS_WRIGGLE)
			msg_format("���͂��щ��峂��x���𔭂����I");
		else if(p_ptr->pclass == CLASS_ICHIRIN)
			msg_format("�_�R���x���̈ӎv�𔭂����I");
		else if (p_ptr->pclass == CLASS_KUTAKA)
			msg_format("���Ȃ��̓���̃q���R���s�������I");
		else
			msg_format("���Ȃ��̑�Z�����x���𔭂����I");
	}
	disturb(0, 1);


	return get_check("�{���ɑ����܂����H");
}




//�����􂢃t���O�t���̃A�C�e���𑕔����Đ���Ă���Ǝ􂢂������鏈��
//�ʏ�̉���\�Ȏ􂢂łȂ��Aart_flag,a_info,e_info�̂����ꂩ�Ƀt���O������􂢁A�������̓}�C�i�Xpval,ac,to_d,to_h���Ώ�
//�o���l���邽�тɌĂ΂��̂ł�����Ɨv���v�Z�������邩���B�Q�[�����d���Ȃ�悤�Ȃ��Ɍv�Z�l����Č��Z����d�g�݂ɂ��悤
void hina_yakuotoshi(int gain_exp)
{
	int need_exp;
	int i,j;
	char o_name[MAX_NLEN];
	u32b flgs[TR_FLAG_SIZE];
	int tmp_xtra5;

	if(p_ptr->pclass != CLASS_HINA) return;
	if(gain_exp < 10) return;
	
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		need_exp = 0;
		if (!o_ptr->k_idx) continue;
		if(o_ptr->xtra3 == SPECIAL_ESSENCE_OTHER) continue;//���łɔ��]�������ꂽ���̂͂����������Ȃ�
		if(o_ptr->tval == TV_CAPTURE) continue;

		object_flags(o_ptr, flgs);
		//�������̂ɕK�v�Ȍo���l(/1000)���Z�o xtra5��16bit�Ȃ̂�30000�𒴂��Ȃ�����
		if(object_is_ego(o_ptr) && o_ptr->name2 == EGO_WEAPON_DEMONLORD) need_exp += 500; //�א_�G�S 50��
		else if(object_is_ego(o_ptr) && e_info[o_ptr->name2].rating == 0) need_exp += 100; //�ق��􂢃G�S 10��
		//�􂢃t���O�@����Ŏ��Ȃ��i���I�Ȃ��݂̂̂��Ώ�
		if(o_ptr->curse_flags & TRC_PERMA_CURSE) need_exp += 1000;//�i���̎�100��
		if(have_flag(flgs,TR_AGGRAVATE)) need_exp += 500;//����50��
		if(have_flag(flgs,TR_TY_CURSE)) need_exp += 500;//���Â̎�50��
		if(have_flag(flgs,TR_DRAIN_EXP)) need_exp += 300;//�o���l�z��30��
		if(have_flag(flgs,TR_ADD_L_CURSE)) need_exp += 50;//�ア�􂢂𑝂₷ 5��
		if(have_flag(flgs,TR_ADD_H_CURSE)) need_exp += 300;//�����􂢂𑝂₷ 30��
		if(have_flag(flgs,TR_NO_TELE)) need_exp += 100;//���e���| 10��
		if(have_flag(flgs,TR_NO_MAGIC)) need_exp += 100; //�����@ 10��

		//�}�C�i�Xpval
		if(o_ptr->pval < 0)
		{
			if(have_flag(flgs,TR_BLOWS)) need_exp += -(o_ptr->pval) * -(o_ptr->pval) * 50; //���}�C�i�X�C���̒ǉ��U���ɑ啝�y�i���e�B
			if(have_flag(flgs,TR_SPEED)) need_exp += -(o_ptr->pval) * -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_MAGIC_MASTERY)) need_exp += -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_STR)) need_exp += -(o_ptr->pval) * 15;
			if(have_flag(flgs,TR_INT)) need_exp += -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_WIS)) need_exp += -(o_ptr->pval) * 20;
			if(have_flag(flgs,TR_CON)) need_exp += -(o_ptr->pval) * 20;
			if(have_flag(flgs,TR_DEX)) need_exp += -(o_ptr->pval) * 15;
			if(have_flag(flgs,TR_CHR)) need_exp += -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_STEALTH)) need_exp += -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_SEARCH)) need_exp += -(o_ptr->pval) * 10;
		}

		//�}�C�i�X�C���l 1���Ƃ�2000
		if(o_ptr->ac + o_ptr->to_a < 0) need_exp += -(o_ptr->to_a) * 2;
		if(o_ptr->to_d < 0) need_exp += -(o_ptr->to_d) * 2;
		if(o_ptr->to_h < 0 && o_ptr->tval != TV_ARMOR && o_ptr->tval != TV_DRAG_ARMOR) need_exp += -(o_ptr->to_h) * 2; //�Z�̖����}�C�i�X�͌v�Z���Ȃ�

		if(!need_exp) continue;

		if(object_is_artifact(o_ptr))
		{
			if(o_ptr->name1 == ART_POWER) need_exp = 10000;//��̎w�ւ�1000���Œ�
			else if(o_ptr->name1 == ART_TWILIGHT) need_exp = 5000;//�ق��ꕔ�A�[�e�B�t�@�N�g�ɑ啝�y�i���e�B
			else if(o_ptr->name1 == ART_THANOS) need_exp = 4000;
			else if(o_ptr->name1 == ART_ANUBIS) need_exp = 3000;
			else if(o_ptr->name1 == ART_HAKABUSA) need_exp = 3000;
			else if(o_ptr->name1 == ART_STORMBRINGER) need_exp = 3000;
			else if(o_ptr->name1 == ART_BERUTHIEL) need_exp = 3000;
			else need_exp += 500; //�ق��A�[�e�B�t�@�N�g+50��
		}

		if(need_exp > 10000) need_exp = 10000;//1000���𒴂��Ȃ�

		//xtra5�Ɍo���l�����Z�@16bit�Ȃ̂ŃI�[�o�[�t���[�΍􂵂Ƃ�
		tmp_xtra5 = o_ptr->xtra5;
		tmp_xtra5 += gain_exp / 1000;
		if(randint1(1000) < (gain_exp % 1000)) tmp_xtra5++;
		if(tmp_xtra5 > 32000) tmp_xtra5 = 32000;
		o_ptr->xtra5 = tmp_xtra5;

		if(p_ptr->wizard) msg_format("INVEN:%d %d/%d",i,tmp_xtra5,need_exp);

		if(o_ptr->xtra5 > need_exp)
		{
			bool flag_bonus = FALSE;
			object_desc(o_name,o_ptr,OD_OMIT_INSCRIPTION);
			msg_format("%s�̖�S�ė������I",o_name);

			o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER; //�b��A�C�e�������ɂ���
			if(o_ptr->curse_flags & TRC_PERMA_CURSE) flag_bonus = TRUE;
			o_ptr->curse_flags = 0L; //�i���̎􂢂�������
			//�p�����[�^���]
			if(o_ptr->to_a < 0) o_ptr->to_a = -(o_ptr->to_a);
			if(o_ptr->to_d < 0) o_ptr->to_d = -(o_ptr->to_d);
			if(o_ptr->to_h < 0) o_ptr->to_h = -(o_ptr->to_h);
			if(o_ptr->pval < 0) o_ptr->pval = -(o_ptr->pval);


			add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
			add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
			add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
			add_flag(o_ptr->art_flags, TR_IGNORE_COLD);
			//�i���̎􂢂��������Ƃ��{�[�i�X
			if(flag_bonus)
			{
				if(object_is_melee_weapon(o_ptr))
				{
					gain_random_slay_2(o_ptr);
					o_ptr->dd += 1;
				}
				else
				{
					one_high_resistance(o_ptr);
					one_ability(o_ptr);

				}
			}

			p_ptr->update |= PU_BONUS;
			p_ptr->window |= (PW_INVEN | PW_EQUIP);

		}
	}

}


//����́�������/�������Ă��邩�ǂ������`�F�b�N����B�O�����ɑΉ����邽�߃X���b�g�����肹���ɂ���
//v1.1.59 �������Ă��Ȃ��Ă��������Ă����OK�ɂ��A��������ɂ��邽�߂̃t���O������
bool check_equip_specific_fixed_art(int a_idx, bool only_equip)
{
	object_type *o_ptr;
	int i;

	if (only_equip) i = INVEN_RARM;
	else i = 0;

	for(;i<INVEN_TOTAL;i++)
	{
		o_ptr = &inventory[i];
		if(!o_ptr->k_idx) continue;
		if(check_invalidate_inventory(i)) continue;
		if(o_ptr->name1 == a_idx) return TRUE;
	}

	return FALSE;
}

//�����X�^�[�������N�A�b�v������B��t�̉h�{�܁A���ς̏����A�u���[�_�[�̓��P�H
//rank�̐����������N�A�b�v����B0�Ȃ�ő�܂ŁB
//v1.1.15 �����N�A�b�v������TRUE��Ԃ��悤�ɂ���
bool rankup_monster(int m_idx, int rank)
{
	monster_type *m_ptr;
	monster_race *r_ptr;
	int i;
	int perc;
	char m_name_old[80],m_name_new[80];

	if (m_idx <= 0) return FALSE;
	m_ptr = &m_list[m_idx];
	r_ptr = &r_info[m_ptr->r_idx];
	monster_desc(m_name_old, m_ptr, 0);
	perc = m_ptr->hp * 100 / m_ptr->maxhp;

	if (m_ptr->mflag2 & MFLAG2_CHAMELEON) return FALSE;
	if (r_ptr->flags7 & RF7_TANUKI) return FALSE;

	//�����N�A�b�v�����@monster_gain_exp()����R�s�[
	for(i=0;(rank<=0||i<rank) && r_ptr->next_exp;i++)
	{
		byte old_sub_align = m_ptr->sub_align;

		real_r_ptr(m_ptr)->cur_num--;
		m_ptr->r_idx = r_ptr->next_r_idx;
		real_r_ptr(m_ptr)->cur_num++;
		m_ptr->ap_r_idx = m_ptr->r_idx;
		r_ptr = &r_info[m_ptr->r_idx];
		if (r_ptr->flags1 & RF1_FORCE_MAXHP)
			m_ptr->max_maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
		else
			m_ptr->max_maxhp = damroll(r_ptr->hdice, r_ptr->hside);
		if (ironman_nightmare)
		{
			u32b hp = m_ptr->max_maxhp * 2L;
			m_ptr->max_maxhp = (s16b)MIN(30000, hp);
		}
		else if(difficulty == DIFFICULTY_EASY) m_ptr->max_maxhp = MAX(1,m_ptr->max_maxhp/2);
		else if(difficulty == DIFFICULTY_NORMAL) m_ptr->max_maxhp = MAX(1,m_ptr->max_maxhp/2 + m_ptr->max_maxhp/4);
		m_ptr->maxhp = m_ptr->max_maxhp;
		//v1.1.64 �����N�A�b�v���ɂ������HP��
		if (m_ptr->hp < m_ptr->maxhp)
			m_ptr->hp += (m_ptr->maxhp - m_ptr->hp) / (2 + randint0(4));

		m_ptr->mspeed = get_mspeed(r_ptr);
		if (!is_pet(m_ptr) && !(r_ptr->flags3 & (RF3_ANG_CHAOS | RF3_ANG_COSMOS)))
			m_ptr->sub_align = old_sub_align;
		else
		{
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
			if (r_ptr->flags3 & RF3_ANG_CHAOS) m_ptr->sub_align |= SUB_ALIGN_EVIL;
			if (r_ptr->flags3 & RF3_ANG_COSMOS) m_ptr->sub_align |= SUB_ALIGN_GOOD;
		}

		m_ptr->exp = 0;
	}

	if(!i) return FALSE; //�����N�A�b�v���Ȃ������ꍇ�I��

	m_ptr->hp = m_ptr->maxhp * perc / 100;//HP����
	(void)set_monster_csleep(m_idx, 0);//�N����
	msg_format("%s�̓p���[�A�b�v�����I", m_name_old);
	update_mon(m_idx, FALSE);
	lite_spot(m_ptr->fy, m_ptr->fx);
	if (m_idx == p_ptr->riding) p_ptr->update |= PU_BONUS;

	return TRUE;

}






//��t�����ܔ�`����ǂރR�}���h�̎��̕� do_cmd_browse()��check_drug_recipe()����Ă΂��
void check_drug_recipe_aux(object_type *o_ptr)
{
	int i,j;
	object_type forge;
	object_type *q_ptr = &forge;
	char temp[70 * 20];
	char *t;
	int shift = o_ptr->sval * 8;

	if(o_ptr->tval != TV_BOOK_MEDICINE || o_ptr->sval > 3)
	{
		msg_print("ERROR:check_drug_recipe()�ɓn���ꂽ�A�C�e������������");
		return;
	}

	screen_save();

	//�������ƊO�ŉ�ʃN���A�͈̔͂�ς���
	if(character_icky > 1)	clear_bldg(4,22);
	else		for(i=4;i<22;i++) Term_erase(17, i, 255);

	while(1)
	{
		char c;
		char o_name[80];
		int idx;

		if(character_icky > 1)	clear_bldg(4,22);
		else		for(i=4;i<22;i++) Term_erase(17, i, 255);

		prt("�ǂ̖�ɂ��Ċm�F���܂����H�v(ESC:�L�����Z��)",4,20);
		for(i=0;i<8;i++)
		{
			char sym = 'a'+i;
			object_prep(q_ptr, lookup_kind(drug_recipe_table[shift+i].tval,drug_recipe_table[shift+i].sval ));
			object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,format("%c)%s", sym,o_name),7+i,20);
		}

		c = inkey();
		if(c == ESCAPE) break;
		if(c < 'a' || c >= 'a'+8) continue;

		if(character_icky > 1)	clear_bldg(4,22);
		else		for(i=4;i<22;i++) Term_erase(17, i, 255);

		idx = shift+(c-'a');
		//�����x�[�X�A�C�e���Ƃ���k_info��������\��
		object_prep(q_ptr, lookup_kind(drug_recipe_table[idx].tval,drug_recipe_table[idx].sval ));
		object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
		roff_to_buf((k_text + k_info[q_ptr->k_idx].text),77 - 15, temp, sizeof(temp));

		c_put_str(TERM_WHITE,o_name,5,20);
		t = temp;
		for (i = 0; i < 4; i++)
		{
			char buf[160];
			if (t[0] == 0) break;
			else
			{
				strcpy(buf, t);
				c_put_str(TERM_WHITE,buf,7+i,20);
				t += strlen(t) + 1;
			}
		}

/* �Ȃ�Ł��̓o�O��̂��킩��Ȃ�
		for (j = 0; temp[j]; j += 1 + strlen(&temp[j]))
		{
			msg_format("%s",temp[j]);
			//c_put_str(TERM_WHITE,&temp[j],7+j,20);
		}
*/
		c_put_str(TERM_WHITE,"�ޗ���:",12,20);
		if(drug_recipe_table[idx].mat1_tv)
		{
			object_prep(q_ptr, lookup_kind(drug_recipe_table[idx].mat1_tv,drug_recipe_table[idx].mat1_sv ));
			object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,o_name,13,20);
		}
		if(drug_recipe_table[idx].mat2_tv)
		{
			object_prep(q_ptr, lookup_kind(drug_recipe_table[idx].mat2_tv,drug_recipe_table[idx].mat2_sv ));
			object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,o_name,14,20);
		}
		if(drug_recipe_table[idx].mat3_tv)
		{
			object_prep(q_ptr, lookup_kind(drug_recipe_table[idx].mat3_tv,drug_recipe_table[idx].mat3_sv ));
			object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,o_name,15,20);
		}



		prt("�����L�[�������Ă�������", 0, 0);
		(void)inkey();
		prt("", 0, 0);


	}
	screen_load();
}


//��t���V�s�m�F�@browse�R�}���h���o�R���Ȃ���
void check_drug_recipe(void)
{
	int		item;

	object_type	*o_ptr;

	cptr q, s;

	if (!CHECK_MAKEDRUG_CLASS)
	{
		msg_print("���Ȃ��͒��܃��V�s��ǂ߂Ȃ��B");
		return;
	}
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_CAST | MIMIC_GIGANTIC))
	{
		msg_print("���͖{��ǂ߂Ȃ��B");
		return ;
	}
	item_tester_tval = TV_BOOK_MEDICINE;

	q = "�ǂ̒��܃��V�s���m�F���܂���? ";
	s = "���܃��V�s���Ȃ��B";

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
	{
		return;
	}
		
	if (item >= 0)	o_ptr = &inventory[item];
	else			o_ptr = &o_list[0 - item];

	check_drug_recipe_aux(o_ptr);

}





/*:::�������̑����i�̂Ƃ�TRUE ���P�̕��틭��*/
bool object_is_metal(object_type *o_ptr)
{
	int sv = o_ptr->sval;

	//���Ӓ�i��false�ɂ���H

	switch(o_ptr->tval)
	{
	//��́u�|�S�́v�̂�OK
	case TV_ARROW:
	case TV_BOLT:
		if(sv == SV_AMMO_NORMAL) return TRUE;
		break;
	//�j�ǉ�
	case TV_OTHERWEAPON:
		if(sv == SV_OTHERWEAPON_NEEDLE) return TRUE;
		break;
	//�܂ꂽ�j
	case TV_MATERIAL:
		if(sv == SV_MATERIAL_BROKEN_NEEDLE) return TRUE;
		break;

	case TV_KNIFE:
		if(sv != SV_WEAPON_DRAGON_DAGGER && sv != SV_WEAPON_DOKUBARI) return TRUE;
		break;
	case TV_SWORD:
	case TV_KATANA:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:
		return TRUE;
	case TV_HAMMER:
		if(sv != SV_WEAPON_KINE) return TRUE;
		break;
	case TV_STICK:
		if(sv == SV_WEAPON_SYAKUJYOU || sv == SV_WEAPON_IRONSTICK || sv == SV_WEAPON_GROND) return TRUE;
		break;
	case TV_SHIELD:
		if(sv == SV_SMALL_METAL_SHIELD || sv == SV_LARGE_METAL_SHIELD || sv == SV_KNIGHT_SHIELD || sv == SV_MIRROR_SHIELD) return TRUE;
		break;
	case TV_ARMOR:
		if(sv == SV_ARMOR_CHAIN || sv == SV_ARMOR_BRIGANDINE || sv == SV_ARMOR_PARTIAL_PLATE || sv == SV_ARMOR_OOYOROI
		|| sv == SV_ARMOR_FULL_PLATE || sv == SV_ARMOR_MITHRIL_CHAIN || sv == SV_ARMOR_MITHRIL_PLATE) return TRUE;
		break;
	case TV_HEAD:
		if(sv == SV_HEAD_METALCAP || sv == SV_HEAD_STARHELM || sv == SV_HEAD_STEELHELM || sv == SV_HEAD_GOLDCROWN
		|| sv == SV_HEAD_JEWELCROWN || sv == SV_HEAD_IRONCROWN) return TRUE;
		break;
	case TV_GLOVES:
		if(sv == SV_HAND_ARMGUARDS || sv == SV_HAND_GAUNTLETS) return TRUE;
		break;
	case TV_BOOTS:
		if(sv == SV_LEG_METALSHODBOOTS || sv == SV_LEG_SABATON) return TRUE;
		break;

	}
	return FALSE;

}


//���P�b��\��
bool kogasa_smith_aux(object_type *o_ptr)
{
	int new_val = 5 + p_ptr->lev / 5;

	if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_BROKEN_NEEDLE)
	{
		int new_num = damroll((2*o_ptr->number),6) + 8 * o_ptr->number;
		if(new_num > 99) new_num = 99;

		object_wipe(o_ptr);
		object_prep(o_ptr, lookup_kind(TV_OTHERWEAPON, SV_OTHERWEAPON_NEEDLE));	
		o_ptr->number = new_num;
		o_ptr->name2 = EGO_ARROW_HOLY;
		o_ptr->to_h = new_val;
		o_ptr->to_d = new_val;
		identify_item(o_ptr);
	}
	else if(object_is_weapon_ammo(o_ptr))
	{
		if(o_ptr->to_h < new_val) o_ptr->to_h = new_val;
		if(o_ptr->to_d < new_val) o_ptr->to_d = new_val;
	}
	else if(object_is_armour(o_ptr))
	{
		if(o_ptr->to_a < new_val) o_ptr->to_a = new_val;
	}
	else
	{
		msg_print("ERROR:kogasa_smith_aux()�ɑz�肵�Ȃ��A�C�e�����n���ꂽ");
		return FALSE;
	}

	if (object_is_cursed(o_ptr) && !(o_ptr->curse_flags & TRC_PERMA_CURSE) && !(o_ptr->curse_flags & TRC_HEAVY_CURSE) )
	{
		o_ptr->curse_flags = 0L;
		o_ptr->ident |= (IDENT_SENSE);
		o_ptr->feeling = FEEL_NONE;
	}
	return TRUE;
}



//�A�[�e�B�t�@�N�g��������̌������̑�^�r�[�����{�[������B�s��������Ȃ��Ƃ�FALSE
bool final_strike(int item_idx, bool remain_broken_weapon)
{
	object_type *o_ptr;
	int item, dam, dir;
	char o_name[MAX_NLEN];

	cptr q = "�A�C�e����I�����Ă�������: ";
	cptr s = "�K�����A�C�e��������܂���B";

	//Paranoia
	if (item_idx < 0) return FALSE;

	if(item_idx) 
		item = item_idx;
	else
	{
		item_tester_hook = object_is_artifact;
		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return FALSE;
		if( !get_check_strict("��낵���ł���? ", CHECK_NO_HISTORY)) return FALSE;	
	}

	if (item >= 0)	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	dam =  object_value_real(o_ptr) / (15+randint1(15));

	if(dam > 9999) dam = 9999;

	if(dam > 0)
	{
		msg_format("%s��ῂ����𔭂����I",o_name);
		if(object_is_melee_weapon(o_ptr))
		{
			(void)get_hack_dir(&dir);	
			fire_spark(GF_PSY_SPEAR,dir,dam,1);
		}
		else
		{
			project(0,6,py,px,dam*2,GF_PSY_SPEAR,(PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID|PROJECT_ITEM),-1);
		}
	}
	else
	{
		msg_format("%s�͕��X�ɍӂ��ď������I",o_name);
	}	

	//����(���̂�)��܂�
	if(object_is_melee_weapon(o_ptr) && remain_broken_weapon && object_is_random_artifact(o_ptr))
	{
		int new_tv = o_ptr->tval;
		int new_sv = o_ptr->sval;

		if(o_ptr->tval == TV_SWORD) new_sv = SV_WEAPON_BROKEN_SWORD;
		else if(o_ptr->tval == TV_KNIFE) new_sv = SV_WEAPON_BROKEN_DAGGER;
		else if(o_ptr->tval == TV_KATANA) new_sv = SV_WEAPON_BROKEN_KATANA;
		else
		{
			new_tv = TV_STICK;
			new_sv = SV_WEAPON_BROKEN_STICK;
		}

		//����̃p�����[�^�ύX�Ȃ�
		o_ptr->k_idx = lookup_kind(new_tv,new_sv);
		//v1.1.12 Tval��Sval���X�V����ĂȂ������̂Œǉ�
		o_ptr->tval = new_tv;
		o_ptr->sval = new_sv;

		o_ptr->dd = randint1(o_ptr->dd / 2 + 1);
		o_ptr->ds = randint0(o_ptr->ds) +1 ;
		if(o_ptr->ds < k_info[o_ptr->k_idx].ds) o_ptr->ds = k_info[o_ptr->k_idx].ds;
		o_ptr->to_a /= 3;
		o_ptr->to_d /= 2;
		o_ptr->to_h /= 2;
		o_ptr->weight = k_info[o_ptr->k_idx].weight;
		if(o_ptr->pval > 1 && !one_in_(4)) o_ptr->pval -= 1;

		//�����C���f�b�N�X���N���A ���������ꁚ��ǉ�����Ȃ甭���C���f�b�N�X�̈������Ⴄ�̂œ��ꏈ�����K�v���낤
		o_ptr->xtra2 = 0; 
		remove_flag(o_ptr->art_flags,TR_ACTIVATE);
	}
	else if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	if (item >= INVEN_RARM)
	{
		kamaenaoshi(item);
		calc_android_exp();
	}
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
	p_ptr->update |= (PU_BONUS);
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	return TRUE;

}


/*:::�������̏e�̎ˌ��������������v�Z����*/
int	calc_gun_fire_chance(int slot)
{
	int chance,lev;
	object_type *o_ptr;

	if(slot != INVEN_RARM && slot != INVEN_LARM)
	{ msg_print("ERROR:calc_gun_fire_cance()����Ɏ����ĂȂ����ɑ΂��ČĂ΂ꂽ");return 0;}
	o_ptr = &inventory[slot];
	if(o_ptr->tval != TV_GUN)
	{ msg_print("ERROR:calc_gun_fire_cance()���e�ȊO�ɑ΂��ČĂ΂ꂽ");return 0;}

	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_ENCHANT && p_ptr->lev > 39) return 100;

	if (object_is_fixed_artifact(o_ptr))
	{	//�ő�100
		lev = a_info[o_ptr->name1].level;
	}
	else if (object_is_random_artifact(o_ptr))
	{	//�ő�127�@�U���͂����܂��Ƀ��x���v�Z
		int need_turn = gun_base_param_table[o_ptr->sval].charge_turn;
		lev = (o_ptr->dd * o_ptr->ds + o_ptr->to_h + o_ptr->to_d)/ need_turn;
		switch(o_ptr->xtra1)
		{
			case GF_FIRE: case GF_ELEC: case GF_ACID:case GF_COLD:case GF_POIS:
			case GF_PLASMA:
				lev /= 2;break;
			case GF_NETHER: case GF_POLLUTE: case GF_NUKE: case GF_CHAOS:
				lev = lev * 2/ 3;break;
		}
		if(o_ptr->xtra4 == GUN_FIRE_MODE_BEAM || o_ptr->xtra4 == GUN_FIRE_MODE_SPARK ||
			o_ptr->xtra4 == GUN_FIRE_MODE_BREATH ||o_ptr->xtra4 == GUN_FIRE_MODE_ROCKET)
			lev += 10;
		if(lev < k_info[o_ptr->k_idx].level) lev = k_info[o_ptr->k_idx].level;
		if(lev > 80) lev = 80 + (lev - 80) / 2;
	}
	else if(object_is_ego(o_ptr))
	{	//�ő�Ŋ�{���x��+25 ���P������90
		lev = k_info[o_ptr->k_idx].level;
		if(lev > 50) lev = 50 + (lev-50)/2;
		lev += e_info[o_ptr->name2].level;
	}
	else
	{	//��{���x��
		lev = k_info[o_ptr->k_idx].level;
		if(lev > 50) lev = 50 + (lev-50)/2;
	}

	if(lev > 127) lev = 127;
	if(lev < 5) lev = 5;

	//������ˌ��̋Z�\�̍��������g���B������p_ptr->to_h_b�����Z����B�@
	//to_h_b��(�j��+10,�q�[���[+12,��p��40��+16,����ɖh��⑕���i�̖����C���Ȃ�)*3
	//Lev1��50-100���x�ALev50��200-300,������g��400,�g���K�[�n�b�s�[�ˎ�450���炢?
	chance = MAX(p_ptr->skill_dev,p_ptr->skill_thb) + p_ptr->to_h_b * BTH_PLUS_ADJ;

	chance += 50 + p_ptr->lev * 2;
	//�Z�\�l�␳ 0-200
	chance += ref_skill_exp(TV_GUN) / 32;
	//�����C���l
	chance += o_ptr->to_h * BTH_PLUS_ADJ;

	//�r�h��y�i���e�B
	if(p_ptr->cumber_glove)
	{
		chance -= chance * (100 - p_ptr->stat_ind[A_DEX] * 2) / 100;
	}
	//�񒚌��e�y�i���e�B �O�d���ɂ͊֌W�Ȃ�
	if(inventory[INVEN_RARM].tval == TV_GUN && inventory[INVEN_LARM].tval == TV_GUN && p_ptr->pclass != CLASS_3_FAIRIES)
	{
		u32b flgs[TR_FLAG_SIZE];
		int i;
		bool flag_genzi = FALSE;
		//���������t���O�`�F�b�N
		object_flags(&inventory[INVEN_HANDS], flgs);
		if (have_flag(flgs, TR_GENZI))
		{
			flag_genzi = TRUE;
		}
		//���N�U�V��̏e�̓񒚌��e�{�[�i�X
		//v1.1.21 ���m�u�E�F�|���}�X�^���[�v���K�������Ƃ��������ɗ���悤�ɂ���
		if((inventory[INVEN_RARM].name1 == ART_YAKUZAGUN_1 && inventory[INVEN_LARM].name1 == ART_YAKUZAGUN_2)
			|| (inventory[INVEN_RARM].name1 == ART_YAKUZAGUN_2 && inventory[INVEN_LARM].name1 == ART_YAKUZAGUN_1)
			|| HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_WEAPON_MASTERY)			)
		{
			if(!flag_genzi) chance = chance * 3 / 4;
		}
		else
		{
			if(flag_genzi) chance = chance * 3 / 4;
			else chance /= 2;
		}

	}
	//�e�������Ȃ���ɕ���⏂��d���{�Ȃǂ𑕔����Ă��鎞�̃y�i���e�B�@�O�d���ɂ͊֌W�Ȃ�
	else if((slot == INVEN_RARM && inventory[INVEN_LARM].k_idx || slot == INVEN_LARM && inventory[INVEN_RARM].k_idx) && p_ptr->pclass != CLASS_3_FAIRIES)
	{
		chance = chance * 4 / 5;
	}
	//���l���Ȃ��Ǝv�����R�撆�y�i���e�B�����Ƃ���
	if(p_ptr->riding )
	{
		chance = chance * (ref_skill_exp(SKILL_RIDING)+2000) / 10000;
	}

	if (p_ptr->confused) chance /= 2;
	else if (p_ptr->shero && p_ptr->pseikaku != SEIKAKU_BERSERK) chance /= 2;
	if(p_ptr->afraid) chance = chance * 2 / 3;
	if(p_ptr->stun) chance = chance * 2 / 3;

	//�␳��
	chance = chance * 5 / lev + chance / 5 - lev * 2 / 3;


	if(p_ptr->pclass == CLASS_SOLDIER && p_ptr->concent)
	{
			chance += p_ptr->concent;
	}

//	msg_format("slot:%d lev:%d chance:%d",slot,lev,chance);

	if(p_ptr->special_attack & ATTACK_HEAD_SHOT)	chance = chance * 3 / 4;

	if(special_shooting_mode == SSM_SEIRAN4) chance = chance * 4 / 5;
	//if(special_shooting_mode == SSM_HEAD_SHOT) chance = chance * 4 / 5;
	if (special_shooting_mode == SSM_RAPID_FIRE)
	{
		//v1.1.44 ���ǂ񂰃��i�e�B�b�N�_�u���͂ނ��떽�������オ�邱�Ƃɂ���B
		if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
		{
			chance += chance / 5;
		}
		else
		{
			chance = chance * 7 / 10;
		}
	}


	if(chance > 100) chance = 100;
	if(chance < USE_DEVICE) chance = USE_DEVICE;

	return chance;

}

///mod160508 �e�̏[�U���Ԃɂ�����{��(%)���v�Z����
int calc_gun_charge_mult(int slot)
{
	int charge_bonus = 0;
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	int mult;

//v1.1.21 ���m�u�����e�i���X�v�Z�\�ɂ�藼��Ɏ����Ȃ��e�ɂ��[�U�K�p����
//	if(slot != INVEN_RARM && slot != INVEN_LARM)
//	{ msg_print("ERROR:calc_gun_charge_bonus()����Ɏ����ĂȂ����ɑ΂��ČĂ΂ꂽ");return 0;}
	o_ptr = &inventory[slot];
	if(o_ptr->tval != TV_GUN)
	{ msg_print("ERROR:calc_gun_charge_bonus()���e�ȊO�ɑ΂��ČĂ΂ꂽ");return 1;}


	//�[�U�Z�k�{�[�i�X�v�Z
	charge_bonus = ref_skill_exp(TV_GUN) / 2000;//�n���x�{�[�i�X�ő�+4
	if(p_ptr->pseikaku == SEIKAKU_TRIGGER) charge_bonus++; //�g���K�[�n�b�s�[
	object_flags(o_ptr, flgs);
	if(have_flag(flgs, TR_XTRA_SHOTS)) charge_bonus++; //�e�̍����ˌ�

	//�O�����̒ǉ��ˌ��̎w�ւ̏����@�����ӏ��Ɋւ�炸�J�E���g����邪�e1������+1�܂�
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		int i,cnt=0;

		for(i=INVEN_RIBBON;i<INVEN_TOTAL;i++)
		{
			if(inventory[i].tval == TV_RING && inventory[i].sval == SV_RING_SHOTS) cnt++;
		}
		if(inventory[INVEN_RARM].tval == TV_GUN && inventory[INVEN_LARM].tval == TV_GUN)
		{
			if(slot == INVEN_RARM && cnt > 0) charge_bonus++;
			if(slot == INVEN_LARM && cnt > 1) charge_bonus++;
		}
		else if(cnt > 0) charge_bonus++;

	}
	//�ʏ�̒ǉ��ˌ��̎w�ւ̏���
	else
	{
		//�E��̏e�̂Ƃ��A���w�̎w�ւ͍���ɉ��������Ă��Ȃ����̂ݗL��
		if(slot == INVEN_RARM)
		{
			if(inventory[INVEN_RIGHT].sval == SV_RING_SHOTS) charge_bonus++;
			if(inventory[INVEN_LEFT].sval == SV_RING_SHOTS && !inventory[INVEN_LARM].k_idx) charge_bonus++;
		}
		else if(slot == INVEN_LARM)
		{
			if(inventory[INVEN_LEFT].sval == SV_RING_SHOTS) charge_bonus++;
			if(inventory[INVEN_RIGHT].sval == SV_RING_SHOTS && !inventory[INVEN_RARM].k_idx) charge_bonus++;
		}
	}

	//���˃{�[�i�X�l1���Ƃ�20%�[�U�Z�k  100��80��64��51..
	mult = 100;
	while(charge_bonus)
	{
		mult = mult * 80 / 100;
		charge_bonus--;
	}
	//�����}���A�[�~�[�̓`���[�W2�{
	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_ONE_MAN_ARMY)) 
		mult /= 2;

	return mult;

}

//�e�̑��e���@��{�l��gun_base_param_table[]�Ł��̓n�[�h�R�[�f�B���O�ŏ㏑��
int calc_gun_load_num(object_type *o_ptr)
{
	if(o_ptr->tval != TV_GUN)
	{
		msg_format("ERROR:calc_gun_load_num()�ɏe�ȊO�̃A�C�e�����n���ꂽ");
		return 1;
	}

	switch(o_ptr->name1)
	{
	case ART_3DPRINTER_GUN:
		return 1;
	case ART_YAKUZAGUN_1:
	case ART_YAKUZAGUN_2:
		return 4;
	case ART_BFG9000:
		return 1;
	default:
		return gun_base_param_table[o_ptr->sval].bullets;
	}

}

//����̃A�C�e���������Ă��邩�ǂ����m�F����B�{���ɍ��X�����B
//v1.1.69 �����Ă���Ƃ��C���x���g��idx���A�����Ă��Ȃ��Ƃ�-1��Ԃ����Ƃɂ���
int	check_have_specific_item(int tval, int sval)
{
	int i;
	object_type *o_ptr;

	for(i=0;i<INVEN_PACK;i++)
	{
		o_ptr = &inventory[i];
		if(!o_ptr->k_idx) continue;
		if(o_ptr->tval == tval && o_ptr->sval == sval) return i;
	}

	return -1;

}


//v1.1.15
/*::: Mega Hack - �w�J�[�e�B�A�̑̕ύX�R�}���h*/		
//magic_num2[0]:���̑̂̔ԍ�(�يE0,�n��1,��2)���i�[
//magic_num2[1]:�ǉ��C���x���g���O��(inven_add[0-11])�ɓ����Ă���̂͂ǂ̑̂̑����i�Ȃ̂��i�[
//magic_num2[4-75]:�ˑR�ψق�ێ�

//magic_num1[0-2]���ꂼ��̑̂�HP�c��(%*100)���L�^
//magic_num1[3-5]���ꂼ��̑̂�MP�c��(%*100)���L�^
//magic_num1[6-8]���ꂼ��̑̂̐��i�l���L�^
//magic_num1[9-11]���ꂼ��̑̂̍ő�MP���L�^

/*:::�w�J�[�e�B�A��p �ǂ̐g�̂ƌ�シ�邩�I������*/
//HP��MP�c�ʂ�䗦�ŕ\��
static int select_hecatia_body(void)
{

	int n,choose=0;
	char c;
	bool selected = FALSE;
	int i;
	cptr namelist[3] = {"�يE","�n��","��"};

	screen_save();	


	while(1)
	{
		for(i=9;i<16;i++) Term_erase(17, i, 255);
		put_str("�ǂ̑̂ƌ�サ�܂����H", 10, 30);

		for(n=0;n<3;n++)
		{
			//���̑̂͑I���ł��Ȃ�
			if(n == p_ptr->magic_num2[0]) continue;
			else put_str( format("%c)%4s�̑�(HP:%.1f%% MP:%.1f%%)",'a'+n,namelist[n],((float)p_ptr->magic_num1[0+n]/100.0),((float)p_ptr->magic_num1[3+n]/100.0)), 12 + n, 30);
		}
		c = inkey();

		if (c == 'a')
		{
			if(p_ptr->magic_num2[0] == HECATE_BODY_OTHER) continue;
			choose = HECATE_BODY_OTHER;
			break;
		}
		else if (c == 'b')
		{
			if(p_ptr->magic_num2[0] == HECATE_BODY_EARTH) continue;
			choose = HECATE_BODY_EARTH;
			break;
		}
		else if (c == 'c')
		{
			if(p_ptr->magic_num2[0] == HECATE_BODY_MOON) continue;
			choose = HECATE_BODY_MOON;
			break;
		}
		else if(c == ESCAPE)
		{
			choose = p_ptr->magic_num2[0];				
			break;
		}
	}

	screen_load();
	return choose;
}

//�w�J�[�e�B�A�̑̌��R�}���h�@�s�����������TRUE
bool  hecatia_change_body(void)
{
	int old_body, new_body, i;
	int shift;
	int hp_perc, sp_perc;
	if(p_ptr->pclass != CLASS_HECATIA)
	{
		msg_print("ERROR:hecatia_change_body()���w�J�[�e�B�A�ȊO�ŌĂ΂ꂽ");
		return FALSE;
	}

	old_body = p_ptr->magic_num2[0];
	new_body = select_hecatia_body();
	if( old_body == new_body) return FALSE;


	if(new_body == HECATE_BODY_OTHER)
		msg_format("���Ȃ��͈يE�̑̂ƌ�サ���B");
	else if(new_body == HECATE_BODY_EARTH)
		msg_format("���Ȃ��͒n���̑̂ƌ�サ���B");
	else
		msg_format("���Ȃ��͌��̑̂ƌ�サ���B");

	//MP����@���Z�I�����V�����̂ł������ ����͌����Ȃǂ�������ɂ͌����Ȃ����܂�������
	p_ptr->csp -= 30;
	if(p_ptr->csp < 0) p_ptr->csp = 0; //paranoia


	//�V�����g�̂̑����i���ǉ��C���x���g���̑O���㔼�ǂ��瑤�ɓ����Ă��邩�ǂ݂����Ĕz��Y�����V�t�g�ʂ𓾂�
	if(p_ptr->magic_num2[1] == new_body)
	{
		shift=0;
		p_ptr->magic_num2[1] = old_body;
	}
	else 
		shift=12;

	//�ǉ��C���x���g���ɕۊǂ��ꂽ�����i�ƍ��̑����i�����ւ���
	for(i=INVEN_RARM;i<=INVEN_FEET;i++)
	{
		object_type forge;
		object_type *o_ptr_swap = &forge;
		object_type *o_ptr_old;
		object_type *o_ptr_new;

		o_ptr_old = &inventory[i];
		o_ptr_new = &inven_add[i-INVEN_RARM+shift];

		object_copy(o_ptr_swap,o_ptr_old);
		object_copy(o_ptr_old,o_ptr_new);
		object_copy(o_ptr_new,o_ptr_swap);
	}

	//�ˑR�ψق��ێ� magic_num2[4-75]
	for(i=0;i<4;i++)
	{
		p_ptr->magic_num2[4+i      + 24 * (old_body)] = p_ptr->muta1>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 4  + 24 * (old_body)] = p_ptr->muta2>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 8  + 24 * (old_body)] = p_ptr->muta3>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 12 + 24 * (old_body)] = p_ptr->muta1_perma>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 16 + 24 * (old_body)] = p_ptr->muta2_perma>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 20 + 24 * (old_body)] = p_ptr->muta3_perma>>(i*8) & (0xFF);
	}
	p_ptr->muta1 = 0L;
	p_ptr->muta2 = 0L;
	p_ptr->muta3 = 0L;
	p_ptr->muta1_perma = 0L;
	p_ptr->muta2_perma = 0L;
	p_ptr->muta3_perma = 0L;
	for(i=0;i<4;i++)
	{
		p_ptr->muta1       |= (u32b)(p_ptr->magic_num2[4+i      + 24 * (new_body)]) <<(i*8);
		p_ptr->muta2       |= (u32b)(p_ptr->magic_num2[4+i +  4 + 24 * (new_body)]) <<(i*8);
		p_ptr->muta3       |= (u32b)(p_ptr->magic_num2[4+i +  8 + 24 * (new_body)]) <<(i*8);
		p_ptr->muta1_perma |= (u32b)(p_ptr->magic_num2[4+i + 12 + 24 * (new_body)]) <<(i*8);
		p_ptr->muta2_perma |= (u32b)(p_ptr->magic_num2[4+i + 16 + 24 * (new_body)]) <<(i*8);
		p_ptr->muta3_perma |= (u32b)(p_ptr->magic_num2[4+i + 20 + 24 * (new_body)]) <<(i*8);
	}

	//�p�����[�^��
	reset_tim_flags();
	for(i=0;i<6;i++) p_ptr->stat_cur[i] = p_ptr->stat_max[i];
	p_ptr->exp = p_ptr->max_exp;
	check_experience();

	//�����x�����Ă�Ή�
	if(p_ptr->food < 5000) p_ptr->food = 5000;

	//���̑̂̍ő�MP���L�^
	p_ptr->magic_num1[9+old_body] = p_ptr->msp;

	//����HP��MP�̊���(0.01%�P��)���v�Z���ێ��A����ɐV�����g�̂̊����l�������Ă���
	hp_perc = p_ptr->chp * 10000 / p_ptr->mhp;
	sp_perc = p_ptr->csp * 10000 / p_ptr->msp;
	p_ptr->magic_num1[0+old_body] = hp_perc;
	p_ptr->magic_num1[3+old_body] = sp_perc;
	hp_perc = p_ptr->magic_num1[0+new_body];
	sp_perc = p_ptr->magic_num1[3+new_body];



	//���i�ύX����
	p_ptr->pseikaku = p_ptr->magic_num1[6+new_body];
	ap_ptr = &seikaku_info[p_ptr->pseikaku];
	//�u���������Ă�́v�l��ύX
	p_ptr->magic_num2[0] = new_body;

	p_ptr->update |= PU_BONUS | PU_HP | PU_MANA | PU_SPELLS;
	update_stuff();

	//�V���ȑ̂�HP��MP���v�Z����
	p_ptr->chp = p_ptr->mhp * hp_perc / 10000;
	p_ptr->csp = p_ptr->msp * sp_perc / 10000;
	p_ptr->chp_frac = 0;
	p_ptr->csp_frac = 0;


	//�ĕ`��
	do_cmd_redraw();

	return TRUE;
}

//v1.1.15
/*:::�w�J�[�e�B�A�̒n���ƌ��̑̂̐��i(p_ptr->magic_num1[6,7,8])��ݒ肷��*/
void get_hecatia_seikaku(void)
{
	int i,j;

	//���ʂ̐��i���X�g
	byte seikaku_list[HECATE_SEIKAKU_CHOOSE] = 
		{SEIKAKU_FUTUU,SEIKAKU_CHIKARA,SEIKAKU_KIREMONO,SEIKAKU_INOCHI,SEIKAKU_SUBASI,SEIKAKU_SHIAWASE,
		SEIKAKU_TRIGGER,SEIKAKU_ELEGANT,SEIKAKU_GAMAN,SEIKAKU_LUCKY};
	byte flag_need_choose = FALSE;

	p_ptr->magic_num1[6] = p_ptr->pseikaku;//�يE�̑̂̐��i�l�i�[��

	for(i=0;i<HECATE_SEIKAKU_CHOOSE;i++)
	{
		if(p_ptr->pseikaku == seikaku_list[i]) flag_need_choose = TRUE;
	}

	if(flag_need_choose)
	{
		char c;

		screen_save();	
		//�يE�̐��i�����ʂ̂��̂��Ƒ��̑̂̐��i���I��
		for(j=0;j<2;j++)
		{
			
			while(1)
			{
				for(i=9;i<HECATE_SEIKAKU_CHOOSE+14;i++) Term_erase(17, i, 255);
				if(j==0) put_str("���Ȃ��́u�n���v�̑̂͂ǂ�Ȑ��i�ł����H", 10, 30);
				else put_str("���Ȃ��́u���v�̑̂͂ǂ�Ȑ��i�ł����H", 10, 30);

				for(i=0;i<HECATE_SEIKAKU_CHOOSE;i++)
				{
					put_str( format("%c)%20s",'a'+i,seikaku_info[seikaku_list[i]].f_title),12+i,25);
				}
				c = inkey();

				if(c >= 'a' && c < 'a'+HECATE_SEIKAKU_CHOOSE)
				{
					if(j==0) p_ptr->magic_num1[7] = seikaku_list[c - 'a'];
					else p_ptr->magic_num1[8] = seikaku_list[c - 'a'];
					break;
				}
			}

		}

		//���i�Ɂu���b�L�[�K�[���v���������Ă���Ɣ����I�[���i���ψق�ݒ�@�n�[�h�R�[�f�B���O
		if(p_ptr->magic_num1[7] == SEIKAKU_LUCKY)
		{
				p_ptr->magic_num2[39] = 0x80;
				p_ptr->magic_num2[51] = 0x80;
		}

		if(p_ptr->magic_num1[8] == SEIKAKU_LUCKY)
		{
				p_ptr->magic_num2[63] = 0x80;
				p_ptr->magic_num2[75] = 0x80;
		}


		screen_load();
	}
	else
	{
		//�يE�̐��i������ȊO���Ƌ����I�ɓ������i�ɂ���
		p_ptr->magic_num1[7] = p_ptr->pseikaku;
		p_ptr->magic_num1[8] = p_ptr->pseikaku;

	}


}


//v1.1.21
/*:::���m�̎擾�Z�\���m�F����*/
void check_soldier_skill(void)
{

	char buf[1024];
	int i;
	int total_skill_num = 0;
	cptr info_ptr;

	if(p_ptr->pclass != CLASS_SOLDIER){msg_print("ERROR:check_soldier_skill()�����Ή��̃N���X�ŌĂ΂ꂽ"); return;}

	for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++) 
		total_skill_num += p_ptr->magic_num2[i];

	if(!total_skill_num)
	{
		msg_print("�܂����̋Z�\�����Ă��Ȃ��B");
		return;
	}

	screen_save();

	while(1)
	{
		int line_num = 0;
		int j;
		char c;
		int s_array=0,num=0;

		for(i=0;i<total_skill_num + 5;i++) Term_erase(17, i, 255);

		prt("�ǂ̋Z�\�̐��������܂����H(ESC:�I��)",2,18);
		//�Z�\�ꗗ��\��
		for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++)
		{
			for(j=1;j<=p_ptr->magic_num2[i];j++)
			{
				sprintf(buf,"  %c) %s",'a' + line_num, soldier_skill_table[i][j].name);
				prt(buf,4+line_num,18);
				line_num++;
			}
		}

		while(1)
		{
			c = inkey();
			if(c == ESCAPE || c >= 'a' && c < 'a'+total_skill_num) break;
		}

		if(c == ESCAPE) break;

		num = c - 'a' + 1;
		
		//�I�������������Z�\�e�[�u���̂ǂ�ɓ����邩array��num���v�Z
		for(s_array = 0; p_ptr->magic_num2[s_array] < num;s_array++) 
			num -= p_ptr->magic_num2[s_array];


		//�������\��
		//��ʃN���A
		for(i=0;i<total_skill_num + 5;i++) Term_erase(17, i, 255);
		//�Z�\�^�C�g���\��
		sprintf(buf,("[%s]"),soldier_skill_table[s_array][num].name);
		prt(buf,3,18);
		//�������\��
		sprintf(buf,"%s",soldier_skill_table[s_array][num].info);
		//rumor_new�Ɠ���\\�ɂ�鎩�����s
		for(i=0;i < sizeof(buf)-2;i++)
		{
			if(buf[i] == '\0')	break; 
			/*:::�ꕔ���������s��������Ă��܂��̂�}�~*/
			else if(iskanji(buf[i]))
			{
				i++;
				continue;
			}
			else if(buf[i] == '\\') buf[i] = '\0';
		}
		buf[i] = '\0';
		buf[i+1] = '\0';
		info_ptr = buf;
		for (i = 0; i< 4; i++)
		{
			if(!info_ptr[0]) break;
			prt(info_ptr, 4+i, 18);
			info_ptr += strlen(info_ptr) + 1;
		}

		prt("�����L�[�������Ă�������", 0, 0);
		(void)inkey();
		prt("", 0, 0);

	}

	screen_load();
	return;

}

//v1.1.21
//���m���Z�\�|�C���g�𓾂�Blevel_reward���[�`������Ă΂��B
//p_ptr->magic_num2[10]���|�C���g�ۑ��̈�Ɏg��
void get_soldier_skillpoint(void)
{
	if(p_ptr->pclass != CLASS_SOLDIER){msg_print("ERROR:get_soldier_skillpoint()�����Ή��̃N���X�ŌĂ΂ꂽ"); return;}

	//���背�x���ł̂݃|�C���g�𓾂�
	switch(p_ptr->lev)
	{
		case 1:	case 5:	case 10:case 15:case 20:case 24:case 28:
		case 32:case 35:case 38:case 41:case 44:case 46:case 48:
		case 50:
			msg_print("�Z�\�K���|�C���g�𓾂��I");
			p_ptr->magic_num2[10]++;
			break;

		default:
			break;
	}

	//v1.1.40 �d���l�`���m�̓��x��1�̃|�C���g�𓾂�O�ɏ��������i�Ń��x��9�ɂȂ�̂Ń��x��1�̃|�C���g�𓾂��Ȃ��Ƃ����o�O�ւ̖�D��
	if (p_ptr->prace == RACE_ANDROID && p_ptr->lev == 5)
	{
		p_ptr->magic_num2[10]++;

	}



}

//v1.1.21
//���m���|�C���g������ċZ�\����K������
//p_ptr->magic_num2[0-3]���Z�\���x���̕ێ��Ɏg���B
bool gain_soldier_skill(void)
{

	char buf[1024];
	int i;
	int s_array=0,num=0;
	cptr info_ptr;
	int skillpoint = p_ptr->magic_num2[10];

	//�f�o�b�O���p�t���[�Y����
	for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++) num += p_ptr->magic_num2[i];
	if(num >= SOLDIER_SKILL_ARRAY_MAX * SOLDIER_SKILL_NUM_MAX){msg_print("ERROR:���ɂ��ׂĂ̋Z�\���K���ς݂�"); return FALSE;}
	num=0;

	
	screen_save();

	while(1)
	{
		int j;
		char c;

		for(i=0;i<10;i++) Term_erase(17, i, 255);

		prt(format("�ǂ̋Z�\���K�����܂����H(ESC:�L�����Z��)"),2,18);
		prt(format("(�c��Z�\�K���|�C���g:%d)",skillpoint),3,28);
		//�Z�\�ꗗ��\��
		for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++)
		{
			j = p_ptr->magic_num2[i]+1;
			if(j > SOLDIER_SKILL_NUM_MAX)
			{
				sprintf(buf,"---�K������---");
			}
			else if(soldier_skill_table[i][j].lev > p_ptr->lev)
			{
				sprintf(buf,"(���̃��x���ł͏K���ł��Ȃ�)");
			}
			else
			{
				if(j == SOLDIER_SKILL_NUM_MAX)
					sprintf(buf,"  %c) %s(2�|�C���g�K�v)",('a' + i), soldier_skill_table[i][j].name);
				else
					sprintf(buf,"  %c) %s",('a' + i), soldier_skill_table[i][j].name);
			}
			prt(buf,4+i,18);
		}

		while(1)
		{
			c = inkey();

			if(c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}

			if(c >= 'a' && c < 'a'+SOLDIER_SKILL_ARRAY_MAX)
			{
				s_array = c - 'a';
				num = p_ptr->magic_num2[s_array]+1;
				if(num > SOLDIER_SKILL_NUM_MAX) continue; //�K�������̂Ƃ���̃L�[���������Ƃ�
				else if(!skillpoint) continue;
				else if(skillpoint < 2 && num == SOLDIER_SKILL_NUM_MAX) continue;
				else if(soldier_skill_table[s_array][num].lev > p_ptr->lev) continue;
				break;
			}
		}

		//�������\��
		//��ʃN���A
		for(i=0;i<10;i++) Term_erase(17, i, 255);
		//���O
		sprintf(buf,("[%s]"),soldier_skill_table[s_array][num].name);
		prt(buf,3,18);
		//�������\��
		sprintf(buf,"%s",soldier_skill_table[s_array][num].info);
		//rumor_new�Ɠ���\\�ɂ�鎩�����s
		for(i=0;i < sizeof(buf)-2;i++)
		{
			if(buf[i] == '\0')	break; 
			/*:::�ꕔ���������s��������Ă��܂��̂�}�~*/
			else if(iskanji(buf[i]))
			{
				i++;
				continue;
			}
			else if(buf[i] == '\\') buf[i] = '\0';
		}
		buf[i] = '\0';
		buf[i+1] = '\0';
		info_ptr = buf;
		for (i = 0; i< 4; i++)
		{
			if(!info_ptr[0]) break;
			prt(info_ptr, 4+i, 18);
			info_ptr += strlen(info_ptr) + 1;
		}

		if(get_check_strict("���̋Z�\���K�����܂����H", CHECK_OKAY_CANCEL)) break;

	}
	msg_format("�u%s�v�̋Z�\���K�������I",soldier_skill_table[s_array][num].name);
	//�K�����x���A�b�v
	p_ptr->magic_num2[s_array]++;
	//�X�L���|�C���g����
	if(num == SOLDIER_SKILL_NUM_MAX)
		p_ptr->magic_num2[10] -= 2;
	else
		p_ptr->magic_num2[10] -= 1;


	screen_load();
	p_ptr->update |= (PU_BONUS|PU_HP|PU_MANA);
	return TRUE;

}

//v1.1.21 �|�[�^����ʂ蔲����
void pass_through_portal(void)
{
	bool flag_ok = FALSE;
	int x,y;
	if(!cave_have_flag_bold(py,px,FF_PORTAL)) return;

	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(cave_have_flag_bold(y,x,FF_PORTAL))
			{
				if(!player_bold(y,x))
				{
					//��������ȊO�̃|�[�^�����������烋�[�v�𔲂���
					flag_ok = TRUE;
					break;
				}
			}
		}
		if(flag_ok) break;
	}
	//�s�悪�Ȃ��Ƃ��I���@energy����Ȃ��̂͂�
	if(!flag_ok)
	{
		msg_print("�܂��s����̃|�[�^�����Ȃ��B");
		return;
	}

	if(!get_check_strict("�����Е��̃|�[�^���Ɉړ����܂����H", CHECK_OKAY_CANCEL)) return;

	//���Α��̃|�[�^���Ɉړ��B���������Ƀ����X�^�[�����������ւ��͂�
	move_player_effect(y,x,MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
	energy_use = 100;



}


//v1.1.22���m�̏e�e�ύX
//SSM_**�̔ԍ���Ԃ�
//�c�e������R�X�g������
int soldier_change_bullet(void)
{
	int i;
	int num_list[100];
	int num,total_num;
	bool flag_only_info = FALSE;
	bool flag_choose = FALSE;
	char buf[1000];

	//��ʃN���A
	screen_save();


	//ALLOW_REPEAT���s�[�g����
	if (repeat_pull(&num))
	{
		flag_choose = TRUE;
	}

	while(!flag_choose)
	{
		int j;
		char c;
		num=0;
		total_num=0;

		for(i=0;i<21;i++) Term_erase(17, i, 255);

		if(flag_only_info)
			prt(format("�ǂ̏e�e�̐��������܂����H(ESC:�L�����Z�� ?:�m�F���I����)"),2,18);
		else
			prt(format("�ǂ̏e�e���g�p���܂����H(ESC:�L�����Z�� ?:�e�e�̌��ʂ��m�F����)"),2,18);

			prt(format("�@�e��@�@�@�@�@�@�@�@ MP �c�e�@�@�e��@�@�@�@�@�@�@�@ MP �c�e"),4,18);

		//�e�e�ꗗ��\��
		for(i=1;soldier_bullet_table[i].cost >= 0;i++)
		{
			int col,row;
			byte char_color;
			bool use_ammo = FALSE;
			bool flag_unable = FALSE;
			//���e�Z�\���x�����ݒ肳��Ă��Ȃ��A�܂��͑���Ȃ��Ƃ����O
			if(!soldier_bullet_table[i].magic_bullet_lev)
				continue;
			if(soldier_bullet_table[i].magic_bullet_lev > p_ptr->magic_num2[SOLDIER_SKILL_MAGICBULLET])
				continue;
			//��␔�J�E���g
			total_num++;
			//���X�g�ɏe�eindex���L�^
			num_list[total_num] = i;

			if(soldier_bullet_table[i].magic_bullet_lev == SS_M_MATERIAL_BULLET)
				use_ammo = TRUE;

			//�c�e�s���A�R�X�g�s���̂Ƃ��g�p�s�t���O
			if(soldier_bullet_table[i].magic_bullet_lev == SS_M_MATERIAL_BULLET && !p_ptr->magic_num1[i])
				flag_unable = TRUE;
			else if(soldier_bullet_table[i].cost > p_ptr->csp)
				flag_unable = TRUE;

			if(use_ammo)
				sprintf(buf,"%c)%-20s %2d  %d",('a'+total_num-1),soldier_bullet_table[i].name,soldier_bullet_table[i].cost,
				p_ptr->magic_num1[i]);
			else
				sprintf(buf,"%c)%-20s %2d  --",('a'+total_num-1),soldier_bullet_table[i].name,soldier_bullet_table[i].cost);

			//�g�p�s�\�̒e�ۂ͊D��
			if(flag_unable) char_color = TERM_L_DARK;
			else char_color = TERM_WHITE;

			col = (total_num-1) % 2;
			row = (total_num-1) / 2;

			c_put_str(char_color,buf,row+5,18+col*32);

		}

		while(1)
		{
			c = inkey();

			if(c == '?')//?�ŏ��m�F�t���O���]
			{
				for(i=21;i<25;i++) Term_erase(17, i, 255);
				flag_only_info = !flag_only_info;
				break;
			}

			if(c == ESCAPE)
			{
				screen_load();
				return 0;
			}
			if(c >= 'a' && c <= ('a'+total_num-1))
			{
				num = num_list[c - 'a' + 1]; //num�����X�g����ݒ�(num_list��1����i�[)

				break;
			}
		}

		if(!num) continue;//?���������Ƃ�

		//��������\�����Ăу��[�v�̍ŏ���
		if(flag_only_info)
		{
			cptr info_ptr;
			for(i=21;i<28;i++) Term_erase(17, i, 255);
			sprintf(buf,("[%s]"),soldier_bullet_table[num].name);
			prt(buf,21,18);
			//�������\��
			sprintf(buf,"%s",soldier_bullet_table[num].info);
			//rumor_new�Ɠ���\\�ɂ�鎩�����s
			for(i=0;i < sizeof(buf)-2;i++)
			{
				if(buf[i] == '\0')	break; 
				/*:::�ꕔ���������s��������Ă��܂��̂�}�~*/
				else if(iskanji(buf[i]))
				{
					i++;
					continue;
				}
				else if(buf[i] == '\\') buf[i] = '\0';
			}
			buf[i] = '\0';
			buf[i+1] = '\0';
			info_ptr = buf;
			for (i = 0; i< 4; i++)
			{
				if(!info_ptr[0]) break;
				prt(info_ptr, 22+i, 18);
				info_ptr += strlen(info_ptr) + 1;
			}
		}
		//�I�����ς񂾂烋�[�v����o��
		else
		{
			repeat_push(num);
			flag_choose = TRUE;
		}
	}

	screen_load();

	//�e�؂��MP�s�������B�񒚌��e�̂���do_cmd_fire_bun_aux()�ł��s���B���璷�����e�e�I�����^�[�Q�b�g���e�؂�msg�݂����Ȃ̂�������ƕςȂ̂�
	if(soldier_bullet_table[num].magic_bullet_lev == SS_M_MATERIAL_BULLET && !p_ptr->magic_num1[num])
	{
		msg_print("���̏e�e�������Ă��Ȃ��B");
		return 0;
	}
	else if(soldier_bullet_table[num].cost > p_ptr->csp)
	{
		msg_print("MP������Ȃ��B");
		return 0;
	}

	//num(p_ptr->magic_num1[]�Y���ASSM_***�ɓ�����)��Ԃ��ďI��
	return num;
}


//v1.1.27 �e���u�������ă����X�^�[�ɖ��������Ƃ��ɋN������ʁB�e�̈З͂ɓ������{�[�����������A�e�̎c�e��1����B
void gun_throw_effect(int y,int x,object_type *q_ptr)
{
	int dam,typ,mode,rad;

	int timeout_base,timeout_max;
	const activation_type* const act_ptr = find_activation_info(q_ptr);

	if(q_ptr->tval != TV_GUN ) return;

	timeout_base = calc_gun_timeout(q_ptr) * 1000;
	timeout_max = timeout_base * (calc_gun_load_num(q_ptr) - 1); 
	//timeout�l��max�𒴂��Ă���Ǝc�e0�ŉ����N����Ȃ�
	if(q_ptr->timeout > timeout_max) return;

	//do_cmd_fire_gun_aux()�݂����ȃn�[�h�R�[�f�B���O��������ɂ��ǉ��B��@���z���܂���B
	switch(act_ptr->index)
	{
	case ACT_CRIMSON:
		rad = 3;
		dam = p_ptr->lev * p_ptr->lev * 6 / 50;
		typ = GF_ROCKET;
		if(one_in_(3)) msg_print("�u�ォ�痈�邼�A�C������I�v");
		break;

	case ACT_BFG9000:
		rad = 5;
		dam = 800 + damroll(8,100);
		typ = GF_DISP_ALL;
		break;
	case ACT_BAROQUE:
		rad = 4;
		dam = 500;
		typ = GF_GENOCIDE;
		if(one_in_(3)) msg_print("�s���킠�t");
		break;
	case ACT_DER_FREISCHUTZ:
		dam = 100;
		while(!one_in_(7)) dam += 100;
		rad = 1;
		typ = GF_SHARDS;
		break;

	case ACT_HARKONNEN2:
		rad = 7;
		dam = 1600;
		typ = GF_NUKE;
		break;

	case ACT_YAKUZA_GUN:
		//�e�Ƀ��b�N���������Ă��Č��ĂȂ��B����������Ȃ烄�N�U�V��łȂ��̂Ɏ����Ă�Ό��Ă�̂��Ƃ����b�ɂȂ邪
		msg_print("�u�Z�[�t�e�B����ȁB�{�l�F�؂ł��܂���h�X�G�v");
		return;

	//��{�I�ȏe
	case ACT_GUN_VARIABLE:
		{
			int dice = q_ptr->dd;
			int sides = q_ptr->ds;
			int base = q_ptr->to_d;
			typ = q_ptr->xtra1;
			mode = q_ptr->xtra4;

			dam = base;
			if(dice && sides) dam += damroll(dice,sides);

			switch(mode)
			{
			case  GUN_FIRE_MODE_BALL:
			case  GUN_FIRE_MODE_ROCKET:
			case  GUN_FIRE_MODE_BREATH:
			case  GUN_FIRE_MODE_SPARK:
				rad = 2 + p_ptr->lev / 15;
				break;
			default:
				rad = 0;
				break;
			}
			break;
		}
	default:
		msg_print("ERROR:gun_throw_effect()�ɂĂ��̏e�̎ˌ���b�p�����[�^����������Ă��Ȃ�");
		return;
	}

	//������������
	project(0, rad, y, x, dam, typ,(PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);

	//�c�e����
	if(q_ptr->timeout + timeout_base > 32000L)
	{
		msg_print("ERROR:���̏e�̃^�C���A�E�g�l��32000�𒴂���");
		return;
	}
	q_ptr->timeout += timeout_base;



}

//v1.1.27 �V�V�X�e���@���E�K�[�h
//set:���ɂ�鑀��
//guard_break:set=FALSE�̂Ƃ��K�[�h�u���C�N�Ȃ�TRUE,��ჂȂǂɂ�鋭��������FALSE 
bool set_mana_shield(bool set, bool guard_break)
{

	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	if (set)
	{
		if(!cp_ptr->magicmaster)
		{
			msg_print("���Ȃ��͌��E�̒������S���Ă��Ȃ��B");
			return FALSE;
		}

		if ((p_ptr->special_defense & SD_MANA_SHIELD))
		{
			msg_print("���Ȃ��͖h����������B");
			p_ptr->special_defense &= ~(SD_MANA_SHIELD);
		}
		else
		{
			if(p_ptr->confused || p_ptr->stun > 50)
			{
				msg_print("���E�ɏW���ł��Ȃ��I");
				return FALSE;
			}
			if(p_ptr->anti_magic || dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
			{
				msg_print("���E�𒣂�Ȃ��B");
				return FALSE;
			}

			msg_print("���Ȃ��͌��E�𒣂��Đg�\�����B");
			p_ptr->special_defense |= SD_MANA_SHIELD;
		}

		notice = TRUE;
	}
	//�U�����󂯂ĉ��������Ƃ��N�O
	else
	{
		if (p_ptr->special_defense & SD_MANA_SHIELD)
		{
			if(guard_break)
			{
				msg_print("�K�[�h�u���C�N���ꂽ�I");
				set_stun(p_ptr->stun + 30 + randint1(30));
			}
			else
			{
				msg_print("���E���������I");
			}

			notice = TRUE;
			/* Use the value */
			p_ptr->special_defense &= ~(SD_MANA_SHIELD);
		}
	}

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Result */
	return (TRUE);
}



///v1.1.27 ���E�K�[�h�ɂ��_���[�W�y����MP����
void check_mana_shield(int *dam, int damage_type)
{
	int reduce_dam, use_mana;

	int shield_power;

	if (!(p_ptr->special_defense & SD_MANA_SHIELD)) return;

	//�ꕔ�̃_���[�W�͔�Ώ�
	//DAMAGE_NOESCAPE�͂ǂ������������B���n��I�[����㩂�*�j��*�͌y���ł��Ă����C�����邪�����̖�����񂾂�M���̂͌y�����ꂽ��ςȋC������B�Ƃ肠�����y���\�ɁB
	if(damage_type == DAMAGE_USELIFE || damage_type == DAMAGE_LOSELIFE || damage_type == DAMAGE_GENO)
		return;

	count_damage_guard_break += *dam;

	if(damage_type == DAMAGE_FORCE)
	{
		msg_print("���E���ђʂ��ꂽ�I");
		reduce_dam = 0;
		use_mana = 0;
	}
	else
	{
		//�y���_���[�W�v�Z
		reduce_dam = (*dam + 1) / 2;

		//����MP�v�Z ����͌����̂Ƃ�-25%
		use_mana = reduce_dam;
		if (p_ptr->dec_mana) use_mana = use_mana * 3 / 4;

		//MP������Ȃ��Ԃ��HP�_���[�W�ɋt��(����͌����̌��ʂ͔������ꂽ�܂�)
		if (p_ptr->csp < use_mana)
		{
			reduce_dam -= use_mana - p_ptr->csp;
			use_mana = p_ptr->csp;
			//�K�[�h�u���C�N
			set_mana_shield(FALSE, TRUE);
		}

	}

	shield_power = MIN(p_ptr->msp, 500);
	if (p_ptr->easy_spell) shield_power += shield_power / 2;

	//���̍s���܂łɍő�MP�ȏ�̃_���[�W��H�������K�[�h�u���C�N
	if(count_damage_guard_break > shield_power)
		set_mana_shield(FALSE,TRUE);

	//paranoia
	if (reduce_dam < 0) reduce_dam = 0;

	//�_���[�W��MP����
	*dam -= reduce_dam;
	p_ptr->csp -= use_mana;
	p_ptr->redraw |= PR_MANA;

}






//�l���m�꒣��쐬1�O���b�h�`�F�b�N 
//temp_x[],temp_y[],temp_n[]���g���B

static bool make_nemuno_sanctuary_aux2(byte y, byte x)
{
	cave_type *c_ptr = &cave[y][x];

	//��ʊO�͖���
	if (!in_bounds2(y, x))
		return TRUE;

	//ROOM�O�͔�Ώ�
	if (!(c_ptr->info & CAVE_ROOM))
		return TRUE;

	//�ǂ�h�A�͔�Ώ� �؂��΂����邽�߂�TUNNEL���`�F�b�N
	if (cave_have_flag_bold(y, x, FF_WALL)
		|| cave_have_flag_bold(y, x, FF_DOOR)
		|| cave_have_flag_bold(y, x, FF_TUNNEL)
		)
		return TRUE;

	//���łɃ`�F�b�N�����ꏊ�͉������Ȃ��B�����Ƃ�������[�v���Ȃ��悤�ɁB
	if(c_ptr->info & CAVE_TEMP)
		return TRUE;

	//�����X�^�[��������FALSE��Ԃ��B�꒣�蔻��Ɏ��s����B
	if(c_ptr->m_idx && is_hostile(&m_list[c_ptr->m_idx]))
		return FALSE;

	if (temp_n == TEMP_MAX) return TRUE;

	/* Mark the grid as "seen" */
	c_ptr->info |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;

	//���̃��[���Ŏl�p���͂���ROOM�łȂ��Ă��꒣��ɂł���Ɩʔ�������

	return TRUE;

}

//v1.1.35 �l���m���꒣������Blite_room()���Q�l�ɂ����B
void make_nemuno_sanctuary(void)
{
	bool flag_success = TRUE;
	int i;
	byte x,y;

	if(p_ptr->pclass != CLASS_NEMUNO)
	{
		msg_print("ERROR:�l���m�ȊO��make_nemuno_sanctuary()���Ă΂ꂽ");
		return;

	}

	if(!(cave[py][px].info & CAVE_ROOM))
	{
		msg_print("�����łȂ��ꏊ��꒣��ɂ��邱�Ƃ͂ł��Ȃ��B");
		return;
	}

	//v1.1.36 temp_n�J�E���g��CAVE_TEMP�t���O�����Z�b�g����
	//�ǂ𒴂��ĕ��������ɂ킽��꒣�肪���������Ƃ����񍐂��󂯂��̂ŁA�Ђ���Ƃ���temp_n�����Z�b�g����Ă��Ȃ��������ǂ����ɂ���̂ł͂Ȃ����Ǝv���ǉ����Ă���
	for (i = 0; i < temp_n; i++)
	{
		y = temp_y[i];
		x = temp_x[i];
		cave[y][x].info &= ~(CAVE_TEMP);
	}
	temp_n = 0;



	if(!make_nemuno_sanctuary_aux2(py,px)) flag_success = FALSE;

	//�ׂ̃O���b�h���`�F�b�N���Ȃ���꒣��ɂł��邩�ǂ����̔���𑱂���
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		//�ǂ�h�A�͓꒣��ɂȂ邪�A���������܂ł͓꒣�蔻�肪�����Ȃ����Ƃɂ���B
		//����ς�꒣��ɂ��Ȃ�Ȃ����Ƃɂ����B���F�h�~
		//if(cave_have_flag_bold(y,x,FF_WALL)
		//	|| cave_have_flag_bold(y,x,FF_DOOR))
		//	continue;

		/* Spread adjacent */
		if(!make_nemuno_sanctuary_aux2(y + 1, x)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y - 1, x)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y , x + 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y , x - 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y + 1, x + 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y - 1, x - 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y - 1, x + 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y + 1, x - 1)) flag_success = FALSE;

		if(!flag_success)
			break;

		if (temp_n == TEMP_MAX)
			break;

	}

	if(cheat_xtra)
		msg_format("�F���O���b�h��:%d",temp_n);

	if(!flag_success)
	{
		msg_print("���̕����ɂ͓G�ΓI�ȋC�z������I������꒣��ɂ��邱�Ƃ͂ł��Ȃ��B");
	}
	else if(temp_n < 4)
	{
		msg_print("�����͋������ē꒣��Ɍ����Ȃ��B");
	}
	else
	{
		//�Â��꒣������
		erase_nemuno_sanctuary(FALSE,TRUE);

		//�꒣��t���O�t�^����
		for (i = 0; i < temp_n; i++)
		{
			y = temp_y[i];
			x = temp_x[i];
			cave[y][x].cave_xtra_flag |= CAVE_XTRA_FLAG_NEMUNO;
			lite_spot(y,x);
		}

		msg_print("���Ȃ��͂��̕�����V���ȓ꒣��ɂ����I");
		p_ptr->update |= (PU_BONUS | PU_HP);

	}

	//temp_n�J�E���g��CAVE_TEMP�t���O�����Z�b�g����
	for (i = 0; i < temp_n; i++)
	{
		y = temp_y[i];
		x = temp_x[i];
		cave[y][x].info &= ~(CAVE_TEMP);
	}
	temp_n = 0;


}

//v1.1.35 �l���m���꒣���������鏈��
void erase_nemuno_sanctuary(bool force,bool redraw)
{
	int x,y;
	bool flag_erase = FALSE;

	if(p_ptr->pclass != CLASS_NEMUNO)
	{
		msg_print("ERROR:�l���m�ȊO��erase_nemuno_sanctuary()���Ă΂ꂽ");
		return;
	}

	//�Â��꒣��t���O������
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(cave[y][x].cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO)
			{
				flag_erase = TRUE;
				cave[y][x].cave_xtra_flag &=  ~(CAVE_XTRA_FLAG_NEMUNO);
				if(redraw) lite_spot(y,x);
			}
		}
	}
	if(flag_erase)
	{
		if(force)
		{
			msg_print("�꒣���D���Ă��܂����I");
		}
		else
		{
			msg_print("���Ȃ��͌Â��꒣�����������B");
		}
	}
	//v1.1.54 �l���m�����j�Q���Z�̉���
	p_ptr->special_defense &= ~(SD_UNIQUE_CLASS_POWER);
	p_ptr->redraw |= (PR_STATUS);


}



//v1.1.46 �������Q������z��p_ptr->magic_num1[0][1]�ɋL�^����B$999,999,999,999�����
//v1.1.92 ��p���i�̂Ƃ��ɂ͒n�`�ω����ĐΖ��n�`�ɂ����O���b�h�����J�E���g���邱�Ƃɂ���
void	jyoon_record_money_waste(int sum)
{
	int dig1, dig2;

	if (p_ptr->pclass != CLASS_JYOON) return;

	dig1 = p_ptr->magic_num1[0];
	dig2 = p_ptr->magic_num1[1];

	dig1 += sum % 1000000;

	if (dig1 > 1000000)
	{
		dig1 -= 1000000;
		dig2 += 1;
	}
	dig2 += sum / 1000000;

	if (dig2 > 1000000)
	{
		dig1 = 999999;
		dig2 = 999999;
	}
	p_ptr->magic_num1[0] = dig1;
	p_ptr->magic_num1[1] = dig2;


}


//v1.1.48 �����������i�ɍ�����������H�炤�B
//process_world����50�Q�[���^�[�����ƂɌĂ΂�Ao_ptr->xtra5�ŃJ�E���g���A100�ɂȂ�����v���B��12���ԁB
//�������O������J�E���g���i�܂Ȃ��Ȃ邪�C���x���g����ƂȂǑS���̃J�E���g���`�F�b�N�����葕���O��������ɂ����肷��̂��ʓ|�Ȃ̂ŕ��u
void shion_seizure(int slot)
{
	char o_name[160];
	object_type *o_ptr = &inventory[slot];
	int value;
	u32b flgs[TR_FLAG_SIZE];

	if (p_ptr->pclass != CLASS_SHION) return;
	if (!o_ptr->k_idx) return;


	object_flags(o_ptr, flgs);

	//�؋����Ȃ��Ȃ�����J�E���g�X�g�b�v
	if (p_ptr->au >= 0)
	{
		o_ptr->xtra5 = 0;
		return;
	}

	//���ꂽ�A�C�e���̓p�X�@TR_�̃t���O�͕ʏ����炵���̂ŋL�q�ǉ�
	if (object_is_cursed(o_ptr) || have_flag(flgs, TR_TY_CURSE) || have_flag(flgs, TR_AGGRAVATE)
		|| have_flag(flgs, TR_ADD_L_CURSE) || have_flag(flgs, TR_ADD_H_CURSE)) return;

	value = object_value_real(o_ptr);
	if ( value < 1) return;


	if (!o_ptr->xtra5)
	{
		object_desc(o_name, o_ptr, 0);
		msg_format("%s�͍������������󂯂��I",o_name);
		p_ptr->window |= PW_EQUIP;
		disturb(1, 0);
	}

	o_ptr->xtra5++;

	//�߈˒��͉�����󂯂Ȃ�
	if (check_invalidate_inventory(slot)) return;

	if (o_ptr->xtra5 >= 100)
	{
		object_desc(o_name, o_ptr, 0);
		msg_format("%s���������Ă��܂����I", o_name);

		inven_item_increase(slot, -1);
		inven_item_describe(slot);
		inven_item_optimize(slot);

		p_ptr->au += value / 2;
		p_ptr->update |= PU_BONUS;
		p_ptr->redraw |= (PR_EQUIPPY | PR_GOLD);
		p_ptr->window |= PW_EQUIP;
		disturb(1, 0);

	}


}


//v1.1.48 �����̕���R�}���h�@�s���������Ƃ���TRUE��Ԃ�
bool shion_begging(void)
{
	int y, x;
	int dir;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int power = 20 + chr_adj;

	monster_type *m_ptr;
	monster_race *r_ptr;

	if (p_ptr->inside_quest || p_ptr->inside_arena)
	{
		msg_print("�_���W�����̕��͋C�͎E���Ƃ��Ă���B�H�����˂���ǂ���ł͂Ȃ��悤���B");
		return FALSE;
	}

	if (p_ptr->food < PY_FOOD_ALERT) power *= 2;
	if (p_ptr->food < PY_FOOD_WEAK) power *= 2;
	if (p_ptr->food < PY_FOOD_STARVE) power *= 2;

	if (!get_rep_dir2(&dir)) return FALSE;
	if (dir == 5) return FALSE;

	y = py + ddy[dir];
	x = px + ddx[dir];
	m_ptr = &m_list[cave[y][x].m_idx];

	if (cave[y][x].m_idx && (m_ptr->ml))
	{
		char m_name[80];
		r_ptr = &r_info[m_ptr->r_idx];
		monster_desc(m_name, m_ptr, 0);


		//�N����
		set_monster_csleep(cave[y][x].m_idx, 0);

		if (is_pet(m_ptr))
		{
			msg_format("���Ȃ��̔z���݂͂Ȉꕶ�������B");
			return TRUE;
		}

		if (p_ptr->food < PY_FOOD_WEAK)
		{
			if (one_in_(2))
				msg_format("���Ȃ��͒p���O�����Ȃ�%s���v������I", m_name);
			else
				msg_format("���Ȃ��͌`�U��\�킸%s�ɋ��������I", m_name);
		}
		else
		{
			if (one_in_(4))
				msg_format("�u�����b��ł�[�B�Ђ�������[�B�v", m_name);
			else if (one_in_(3))
				msg_format("���Ȃ���%s�ɐH�����˂������B", m_name);
			else if (one_in_(2))
				msg_format("���Ȃ��͌��������������%s�𕨗~�����Ɍ��߂��c", m_name);
			else
				msg_format("���Ȃ���%s�ɐH�ו��������񂾁B", m_name);
		}

		if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND) || r_ptr->flagsr & RFR_RES_ALL)
		{
			msg_format("%s�͖��������B", m_name);
			return TRUE;
		}

		if (r_ptr->flags1 & RF1_QUESTOR)
		{
			msg_format("%s�Ƃ͊��S�ɓG�΂��Ă���B���������邵���Ȃ��悤���B", m_name);
			return TRUE;
		}

		if (m_ptr->mflag & MFLAG_SPECIAL)
		{
			msg_format("%s�͂������Ȃ��Ɏ�荇���Ă���Ȃ��B", m_name);
			return TRUE;
		}

		if (r_ptr->flags3 & RF3_GEN_MASK)
		{
			if (r_ptr->flags3 & (RF3_GEN_HUMAN)) power = power * 3 / 2;
			else if (r_ptr->flags3 & RF3_GEN_KWAI) power /= 2;
			else if (r_ptr->flags3 & RF3_GEN_MOON) power /= 3;
			else if (r_ptr->flags3 & RF3_GEN_WARLIKE) power /= 2;
		}
		else if (r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD)) power /= 3;
		else if (r_ptr->flags3 & RF3_ANG_CHAOS) power /= 2;
		else if (r_ptr->flags3 & RF3_HUMAN) power = power * 3 / 2;

		if (p_ptr->cursed & TRC_AGGRAVATE) power /= 2;
		else if (is_friendly(m_ptr)) power *= 2;

		//�V�q�͌����ł��ł�̂Œ��ǂ����Ƃ�
		if (m_ptr->r_idx == MON_TENSHI) power = 100;

		if (randint1(100) < power) //����
		{
			object_type forge, *q_ptr = &forge;

			if (m_ptr->r_idx == MON_TENSHI)
			{
				msg_format("%s�͓V�E�̓����킯�Ă��ꂽ�B", m_name);
			}
			else if(is_gen_unique(m_ptr->r_idx) || (r_ptr->flags3 & RF3_HUMAN)|| (r_ptr->flags3 & RF3_DEMIHUMAN)
				|| my_strchr("hknoptuzAFKLOPSTUVWY", r_ptr->d_char))
			{
				if (power > 50)
					msg_format("%s�͋C���킵���ɉ����������o���Ă����B", m_name);
				else if (power > 30)
					msg_format("%s�͕��ꂽ������������𕪂��Ă��ꂽ�B", m_name);
				else if (power > 15)
					msg_format("%s�͂����ɂ����X�����ɉ������b��ł��ꂽ�B", m_name);
				else
					msg_format("%s�͂��Ȃ���}�΂������𑫌��ɕ������B", m_name);

			}
			else
			{
				msg_format("%s�͂ǂ����炩�A�C�e�����o�����B", m_name);
			}


			if (m_ptr->r_idx == MON_MINORIKO)
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEET_POTATO));
			else if (m_ptr->r_idx == MON_SANAE)
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_MIRACLE_FRUIT));
			else if (m_ptr->r_idx == MON_RINGO)
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_DANGO));
			else if (m_ptr->r_idx == MON_TENSHI)
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_PEACH));
			else if (r_ptr->d_char == 'K')
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_CUCUMBER));

			else if (is_friendly(m_ptr) && weird_luck())
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_CURING));
			else if( (r_ptr->flags2 & RF2_SMART) && weird_luck())
				object_prep(q_ptr, lookup_kind(TV_STAFF, SV_STAFF_IDENTIFY));
			else if (power > 50 && weird_luck())
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_SENTAN));
			else if (power < 30 && weird_luck())
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_SLIMEMOLD));
			else if (one_in_(6))
				object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH));
			else if (one_in_(3))
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_VENISON));
			else
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

			apply_magic(q_ptr, dun_level, AM_NO_FIXED_ART);
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, y, x);

			msg_format("%s�͋����Ă������B", m_name);

			check_quest_completion(m_ptr);
			delete_monster_idx(cave[y][x].m_idx);

		}
		else  //���s
		{
			//�T�O���͖���
			if (m_ptr->r_idx == MON_SAGUME)
			{
				msg_format("%s�͂��Ȃ���̂ނ悤���ɂ񂾁B", m_name);
			}

			else if (is_gen_unique(m_ptr->r_idx) || (r_ptr->flags3 & RF3_HUMAN) || (r_ptr->flags3 & RF3_DEMIHUMAN)
				|| my_strchr("hknoptuzAFKLOPSTUVWY", r_ptr->d_char))
			{
				if (power > 50)
					msg_format("%s�͍ς܂Ȃ����Ɏ��U�����B", m_name);
				else if (power > 30)
					msg_format("%s�͂��Ȃ��̑i����َE�����B", m_name);
				else if (power > 15)
					msg_format("%s�͂��Ȃ��Ɏ{���C�Ȃǂ��炳��Ȃ��悤���B", m_name);
				else
					msg_format("%s�͂��Ȃ��ɕ��̓I�Ȍ��t�𗁂т����B", m_name);

			}
			else if (r_ptr->flags2 & RF2_SMART)
			{
				if (power > 50)
					msg_format("%s�͉�������Ȃ������B", m_name);
				else
					msg_format("%s�͈Ј��I�ɋ��₵���B", m_name);
			}
			else
			{
				if (power > 30)
					msg_format("%s�͂��Ȃ��̑i���𗝉��ł��Ȃ��悤���B", m_name);
				else
					msg_format("%s�͂��Ȃ��̂��Ƃ��a���Ƃ����v���Ă��Ȃ��I", m_name);

			}

			//����ɔ�������s����ƁA����t���O�𗧂ĂĂ��������Ă���Ȃ�����
			if (randint1(50) > power || one_in_(6))
			{
				msg_format("%s�͂�������݂��Ă��ꂻ���ɂȂ��I", m_name);
				m_ptr->mflag |= MFLAG_SPECIAL;

			}
		}

	}
	else
	{
		msg_format("�����ɂ͉������Ȃ��B");
		return FALSE;
	}
	return TRUE;

}

//v1.1.76 �H�X�q�́u���s�����]���ρv����
//�g�p���ƃ^�[���o�ߎ�(process_upkeep_with_speed)�ɌĂ΂��
void yuyuko_nehan(void)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	if (one_in_(3))
	{
		msg_print("�₽�����񂾌��������ꂽ�I");
		deathray_monsters();
		project_hack(GF_HAND_DOOM, plev * 5 + chr_adj * 5);
	}

	if (one_in_(2))
	{
		msg_print("���̉ԕق��~�蒍�����I");
		project_hack(GF_REDEYE, plev * 10 + chr_adj * 10);
		confuse_monsters(plev * 5);
		stun_monsters(plev * 5);
		turn_monsters(plev * 5);

	}

	if (one_in_(2))
	{
		msg_print("�����̒����ӂ�𖄂ߐs�������I");
		project_hack2(GF_DISP_ALL, plev, chr_adj+10 , 0);
	}

	msg_print("���̗͂����͂��ݍ���...");
	project_hack(GF_NETHER, damroll(plev, chr_adj + 20));


}


//���͕s���y�i���e�B�ɂ��C�⏈��
//���@�g�p(�v�I�v�V�����ݒ�)���������̎��ł����ɗ���
//sleep_turn_base:�C��^�[�����@��{�I��MP�s����
void process_over_exert(int sleep_turn_base)
{
	int sleep_turn;

	p_ptr->csp = 0;
	p_ptr->csp_frac = 0;
	p_ptr->redraw |= PR_MANA;

	if (sleep_turn_base < 1) return;

	if (RACE_NOT_MANA_DRIVEN)
	{
		sleep_turn = randint1(sleep_turn_base / 5 + 1);
	}
	else
	{
		sleep_turn = (sleep_turn_base + 1) / 2 + randint1(sleep_turn_base);
	}


	if (sleep_turn < 5 && !one_in_(6))
	{
		set_stun(sleep_turn * 20);
	}
	else
	{
		if (RACE_NOT_MANA_DRIVEN)
			msg_print("���Ȃ��͋}���Ȗ��͑r���ɂ���ċC�₵���I");
		else
			msg_print("���Ȃ��͖��͌��R�̂��ߍ�����ԂɊׂ����I");

		//�ϐ��𖳎����Ė��
		(void)set_paralyzed(p_ptr->paralyzed + sleep_turn);
	}

}

