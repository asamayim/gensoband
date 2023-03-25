#include "angband.h"
//v1.1.86
//�V������u�A�r���e�B�J�[�h�v�֌W�̊֐�


//�疒��p
//r_idx��n���āu���̃����X�^�[���琶���ł���A�r���e�B�J�[�h�͂��邩�v�𔻒肷��B
//��������Ƃ��͍ŏ���card_idx��Ԃ��B
//�Ȃ����0��Ԃ��B0�́u�󔒂̃J�[�h�v��card_idx�����疒�̏��������J�[�h�Ȃ̂Ŏ��s�l�Ƃ��Ďg���Ă����Ȃ�
int	chimata_can_copy(int r_idx)
{

	int i;
	monster_race *r_ptr;

	if (r_idx < 1 || r_idx >= max_r_idx) { msg_format("ERROR:chimata_can_copy(%d)", r_idx); return 0; }

	r_ptr = &r_info[r_idx];

	//�J�[�hidx�Ń��[�v
	for (i = 1; i<ABILITY_CARD_LIST_LEN; i++)
	{
		//�Ώۃ����X�^�[���ǂ���
		if (r_idx != ability_card_list[i].r_idx) continue;

		//magic_num2[card_idx]��0�ȊO�Ȃ珊�L�ς݂Ȃ̂Ńp�X
		if (p_ptr->magic_num2[i]) continue;

		//��̂݃��C�}���̓��x��90�ȏ����Ƃ���
		if (i == ABL_CARD_YUNOMI_REIMU && r_ptr->level < 90) continue;
		if (i == ABL_CARD_YUNOMI_MARISA && r_ptr->level < 90) continue;

		break;
	}

	if (i == ABILITY_CARD_LIST_LEN) return 0;
	return i;


}

//v2.0.7 �疒���󔒂̃J�[�h���g�p���ăA�r���e�B�J�[�h���쐬����
//������J�[�h��p_ptr->magic_num2[card_idx]��1�ɂ���Ƃ����`�ŋL�^����
//�s����������Ƃ�TRUE
static bool	chimata_use_blank_card(void)
{

	int i;
	monster_type *m_ptr;
	int y, x;
	int m_idx, dir;
	int new_card_idx;
	char m_name[120];
	int prob;

	int old_rank, new_rank;

	if (p_ptr->inside_arena)
	{
		msg_print("�����ł͎g���Ȃ��B");
		return FALSE;
	}

	//�אڃ����X�^�[���w��
	if (!get_rep_dir2(&dir) || dir == 5) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];
	m_idx = cave[y][x].m_idx;
	m_ptr = &m_list[m_idx];
	if (!m_idx || !m_ptr->ml)
	{
		msg_print("�����ɂ͉������Ȃ��B");
		return FALSE;
	}

	//�w�肵�������X�^�[����J�[�h�𐶐��\���H
	new_card_idx = chimata_can_copy(m_ptr->r_idx);
	if (!new_card_idx)
	{
		msg_print("���̎҂ł͍���ӗ~���N���Ȃ��B");
		return FALSE;
	}

	//�������v�Z
	//(�m�\+��p+���x����*2)% �Q�Ă��+50%�A�N���Ăċ��|�����N�O�ǂꂩ�Ȃ�+25%�A�h��ቺ��ԂȂ�+25%�A�Œ�ۏ�5%
	//��ɋN���ĂĈُ�ϐ������ŃN�G�X�g�ɂ����o�Ȃ��t�����ƉB��ނ���Ԃ̋��G���H���ƃ}�~�]�E�Ƃʂ���������ρB
	//lev70�̉B��ނ̏ꍇlev,INT,DEX�S��40�Ő�����20%�A�j�ł̖�Ƃ��������Ėh��ቺ�������45%�܂ŏオ��v�Z
	//lev90���C�}����lev50INTDEX40����INT�Ɍ��E�˔j����Ėh��ቺ��ʂ��Ă悤�₭35%

	prob = (p_ptr->stat_ind[A_INT] + 3) + (p_ptr->stat_ind[A_DEX] + 3) + (p_ptr->lev - r_info[m_ptr->r_idx].level) * 2;
	if (MON_CSLEEP(m_ptr)) prob += 50;
	else if (MON_CONFUSED(m_ptr) || MON_STUNNED(m_ptr) || MON_MONFEAR(m_ptr)) prob += 25;
	if (MON_DEC_DEF(m_ptr)) prob += 25;

	if (prob > 100) prob = 100;
	if (prob < 5) prob = 5;

	if (!get_check_strict(format("���s���܂����H(������%d%%)", prob), CHECK_DEFAULT_Y)) return FALSE;

	monster_desc(m_name, m_ptr, 0);

	msg_format("���Ȃ���%s�̗͂�͕킵�n�߂�...",m_name);

	//����
	if (randint0(100) < prob)
	{
		msg_format("�������I�u%s�v�����������I", ability_card_list[new_card_idx].card_name);

		old_rank = (CHIMATA_CARD_RANK);

		//����ς݃t���O
		p_ptr->magic_num2[new_card_idx] = 1;

		chimata_calc_card_rank();

		new_rank = (CHIMATA_CARD_RANK);

		if (old_rank != new_rank)
		{
			msg_print("�A�r���e�B�J�[�h�̗��ʂ��V���Ȓi�K�ɓ������C������I");
			p_ptr->update |= (PU_BONUS | PU_LITE | PU_HP | PU_MANA);
			update_stuff();
		}

		//���ɂ�����ꍇ�ʒm���Ƃ�
		if (chimata_can_copy(m_ptr->r_idx))
		{
			msg_print("�܂����ɂ���ꂻ�����I");
		}

	}
	//���s
	else
	{
		msg_print("�͂̃R�s�[�Ɏ��s�����I");
	}

	//�N���ēG��
	set_monster_csleep(cave[y][x].m_idx, 0);
	anger_monster(m_ptr);

	return TRUE;

}

//�A�r���e�B�J�[�h��������apply_magic()�̑���ɌĂԁBapply_magic()�ɕ��荞��ł���������Ă΂�邪�����N�Ȃǂ̎w��͂ł��Ȃ��B
//�J�[�h��IDX��o_ptr->pval�ɋL�^�����BIDX��ability_card_list[]�̓Y���Ɉ�v����B
//ability_card_list[]�ɃJ�[�h���A�J�[�h�̔�����x�A�`���[�W���ԂȂǂ��ݒ肳��Ă���B
//card_idx:��������J�[�h��idx idx���L���͈͊O�̂Ƃ������_������
//card_rank_min/max:��������J�[�h�̃����N���w�肵�����Ƃ��B0�Ȃ烉���_������
//�߂�l:�������ꂽ�J�[�h�̃����N(�����e�X�g�p)
int apply_magic_abilitycard(object_type *o_ptr, int card_idx, int card_rank_min, int card_rank_max)
{

	int i;
	int new_rank = -1, new_idx = -1;
	bool flag_extra_bonus = FALSE;

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_print("ERROR:apply_magic_abilitycard()��TV_ABILITY_CARD�ȊO���n���ꂽ");
		return 0;
	}

	//�J�[�hidx���w�肳��Ă���Ƃ�
	if (card_idx >= 0 && card_idx < ABILITY_CARD_LIST_LEN)
	{
		new_idx = card_idx;
	}
	else
	{
		int candi_num = 0;

		//�w�肳��Ă��Ȃ��Ƃ��J�[�h�������_�������B�܂������N�����߂�

		if (card_rank_min < 2 && !one_in_(5)) new_rank = 1;//�R���� 80%
		else if (card_rank_min < 3 && !one_in_(5)) new_rank = 2;//���A 16%
		else if (card_rank_min < 4 && !one_in_(5)) new_rank = 3;//���j�[�N�H�X�[�p�[���A�H 3.2%
		else new_rank = 4; //���W�F���_���[�Ƃ�USSR�Ƃ�����Ȃ� 0.8%

		//weird_luck��ʂ�ƃ����N�A�b�v���邱�Ƃɂ��Ă݂�B�O���t�B�b�N������Ȃ���ꉉ�o���o�������Ƃ��낾�B
		//�ʏ��1/12�A���I�[�����肢�K�^��1/9�A�����d�����邩�K�^�̃��J�j�Y����1/7�Œʂ�B
		//weird_luck���݂̔r�o���́��̂悤�ɂȂ�͂��B
		// 1/12 73%/22%/4.3%/1.1%
		// 1/7  69%/30%/5%/1.25%
		if (weird_luck()) new_rank++;

		if (card_rank_max > 0 && new_rank > card_rank_max) new_rank = card_rank_max;

		//�����N4�ɂ����weird_luck()��ʂ��ă����N5�ɂȂ��Ă��܂�����4�ɖ߂��B
		if (new_rank > 4)
		{
			//�����J�����Ƃ��̂݁A�����N5�ɂȂ�����{�[�i�X�ł����ꖇ�B���̒����甠���o�Ă��Ȃ��悤�ɂ���O���[�o���t���O���ė��p
			if(opening_chest ) flag_extra_bonus = TRUE;
			new_rank = 4;
		}

		//�J�[�h���X�g�̒����烌�A���e�B����v�����J�[�h�������_���ɒ��I
		for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
		{
			if (ability_card_list[i].rarity_rank != new_rank) continue;

			//EXTRA���[�h�ł͋A�җp�J�[�h���o�Ȃ�
			if (EXTRA_MODE && i == ABL_CARD_SANAE1) continue;

			//�����ϗe�J�[�h���o�Ȃ�
			if (EXTRA_MODE && i == ABL_CARD_DOREMY) continue;

			//�s��j��J�[�h���o�Ȃ�
			if (EXTRA_MODE && i == ABL_CARD_NITORI) continue;


			candi_num++;
			if (!one_in_(candi_num)) continue;
			new_idx = i;
		}

		if (new_idx < 0)
		{
			msg_format("ERROR:�J�[�h���ꖇ���I�肳��Ȃ�����(min:%d max:%d)", card_rank_min, card_rank_max);
			return 0;
		}
	}

	//�J�[�h�̎�ނ�pval�Ɋi�[
	o_ptr->pval = new_idx;

	//�����肪�o��������ꖇ
	if (flag_extra_bonus) o_ptr->number++;

	//*�Ӓ�*�ςɂ���
	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	return new_rank;
}


//����̃A�r���e�B�J�[�h���U�b�N���ɉ����ۗL���Ă��邩�m�F����B
//�E�ɂ���Ă͒ǉ��C���x���g�����m�F����B
//card_idx��ability_card_list[]�̓Y��
//�߂�l�̓J�[�h�̍��v����(�����ӏ��Ɏ����Ă���Ƃ��͑S�Ă̍��v����)�B�d�l��10�ȏ�̂Ƃ���9���Ԃ�
int	count_ability_card(int card_idx)
{
	int num = 0;
	int i;
	int inven2_num;

	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:count_ability_card()�ɕs����idx(%d)���n���ꂽ", card_idx);
		return 0;

	}

	//�疒��p_otr->magic_num2[card_idx]�ɕۗL�̗L�����A�J�[�h���ʃ����N�Ŗ������L�^
	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		if (!CHECK_CHIMATA_HAVE_CARD(card_idx)) return 0;

		return (CHIMATA_CARD_RANK);

	}

	//character_icky��TRUE�̂Ƃ��́A�X�⎩��ɒu���ꂽ�A�C�e���𒲂ׂĂ�����̂ƌ��Ȃ��Č������1�ɂ���
	//if (character_icky) return 1;
	//v1.1.87 ���X�Ŕ������Ƃ������Ƃ���calc_bonuses()���Ă΂�Ă�炵�����̃p�����[�^���ꎞ�I�ɕςɂȂ�̂ŃO���[�o���ϐ��̃t���O�ɒu��������
	if (hack_flag_store_examine) return 1;


	//�U�b�N��(�C���x���g��0�`22)���`�F�b�N
	for (i = 0; i<INVEN_PACK; i++)
	{
		if (!inventory[i].k_idx) continue;
		if (inventory[i].tval != TV_ABILITY_CARD) continue;
		//�J�[�h�̎�ނ�pval��sval�łȂ�xtra1�ŊǗ����Ă���
		if (inventory[i].pval == card_idx) num += inventory[i].number;
	}

	inven2_num = calc_inven2_num();

	//v1.1.87 �ǉ��C���x���g��������΂�����`�F�b�N 俎q�̂͏��L�Ƃ����邩�ǂ���������Ɖ��������܂��ǂ�
	for (i = 0; i < inven2_num; i++)
	{
		if(!inven_add[i].k_idx) continue;
		if (inven_add[i].tval != TV_ABILITY_CARD) continue;
		if (inven_add[i].pval == card_idx) num += inven_add[i].number;
	}

	//���L�^�̃J�[�h��10���ȏ㎝���Ă����ʂ͏㏸���Ȃ�
	if (num>9) num = 9;

	return num;

}

//�J�[�h�ɂ��HP,SP�㏸�Ȃǂ̃p�����[�^�v�Z
int calc_ability_card_mod_param(int card_idx, int card_num)
{
	int bonus = 0;
	int plev = p_ptr->lev;
	switch (card_idx)
	{
		case ABL_CARD_LIFE:
		{
			bonus = plev;

			if (card_num > 1) bonus += plev * (card_num - 1) /10;
			if (card_num == 9) bonus = plev * 2;

		}
		break;
		case ABL_CARD_SPELL:
		{
			bonus = plev / 2;

			if (card_num > 1) bonus += plev * (card_num - 1) / 20;
			if (card_num == 9) bonus = plev;

		}
		break;
		case ABL_CARD_FUTO:
		{
			bonus = plev;

			if (card_num > 1) bonus += plev * (card_num - 1) / 10;
			if (card_num == 9) bonus = plev * 2;

		}
		break;
		case ABL_CARD_FEAST:
		{
			bonus = plev * 2;

			if (card_num > 1) bonus += plev * (card_num - 1) / 5;
			if (card_num == 9) bonus = plev * 4;

		}
		break;

		case ABL_CARD_PHOENIX:
		{

			if (card_num == 9) 
				bonus = plev * 8;
			else
				bonus = plev * 3 + (card_num - 1) * plev/2;
		}
		break;

		//vault�o����(%)�Ƌ��G�o����(get_mon_num())�ւ̃u�[�X�g
		case ABL_CARD_JYOON:
		case ABL_CARD_SHION:
		{
			bonus = 200;

			if (card_num > 1) bonus += (card_num - 1) * 30;
			if (card_num >= 9) bonus = 500;
		}
		break;
		
		default:
			msg_format("ERROR:calc_ability_card_mod_param()�ɑz��O�̃J�[�h(idx:%d)���n���ꂽ",card_idx);

	}

	return bonus;


}

//�w���̔��ȂǃJ�[�h�Ɋւ���v�Z�l��Ԃ�
//�����h��m���������������ʂɑ��̃p�����[�^�Ɏg��񗝗R���Ȃ�
//prob��mod_param�ɕ������񂾂��炱�̊֐��ɂ͊m���l�̌v�Z����������ׂ��������������Ȃ��Ă��Ȃ��͔̂���
int calc_ability_card_prob(int card_idx, int card_num)
{
	int result_param = 0;
	switch (card_idx)
	{
	case ABL_CARD_BACKDOOR:
	case ABL_CARD_DANMAKUGHOST:
	case ABL_CARD_BASS_DRUM:
	case ABL_CARD_PSYCHOKINESIS:

	{
		result_param = 25;

		if (card_num > 1) result_param += (card_num-1)*5;

		if (card_num >= 9) result_param = 70;

	}
	break;

	case ABL_CARD_UFO:
	{
		result_param = 10;
		if (card_num > 1) result_param += (card_num - 1) * 2;
		if (card_num >= 9) result_param = 30;

	}
	break;

	case ABL_CARD_KAGUYA:
	{
		result_param = 20;

		if (card_num > 1) result_param += (card_num - 1) * 3;
		if (card_num >= 9) result_param = 50;

	}
	break;
	case ABL_CARD_MAGATAMA:
	{
		result_param = 20;

		if (card_num > 1) result_param += (card_num - 1) * 8;
		if (card_num >= 9) result_param = 100;

	}
	break;
	case ABL_CARD_HANIWA:
	{
		result_param = 20;
		if (card_num > 1) result_param += (card_num - 1) * 3;
		if (card_num >= 9) result_param = 50;

	}
	break;

	case ABL_CARD_JIZO:
		result_param = 50;
		if (card_num > 1) result_param += (card_num - 1) * 10;
		if (card_num >= 9) result_param = 150;

		break;

	case ABL_CARD_PATHE_SPELL:
		result_param = 30;
		if (card_num > 1) result_param += (card_num - 1) * 7;
		if (card_num >= 9) result_param = 100;

		break;

	case ABL_CARD_KAMIYAMA:
		result_param = 50;
		if (card_num > 1) result_param += (card_num - 1) * 10;
		if (card_num >= 9) result_param = 150;

		break;

	case ABL_CARD_EIKI:
		result_param = 100;
		if (card_num > 1) result_param -= (card_num - 1) * 5;
		if (card_num >= 9) result_param = 50;
		break;

	case ABL_CARD_PATHE_STUDY:
		result_param = 4 + card_num;
		if (card_num >= 9) result_param = 15;
		break;

	case ABL_CARD_SEIRAN:
		result_param = 20;
		if (card_num > 1) result_param += (card_num - 1) * 5;
		if (card_num >= 9) result_param = 75;

		break;

	case ABL_CARD_JELLYFISH:
		result_param = 30;
		if (card_num > 1) result_param += (card_num - 1) * 5;
		if (card_num >= 9) result_param = 80;
		break;

	case ABL_CARD_100TH_MARKET:
		result_param = 40;
		if (card_num > 1) result_param += (card_num - 1) * 5;
		if (card_num >= 9) result_param = 90;
		break;


	default:
		msg_format("ERROR:calc_ability_card_prob()�ɑz��O�̃J�[�h(idx:%d)���n���ꂽ", card_idx);

	}


	//���e�X�g�p��100%�ɂ���@�\�����Ă������A�ꍇ�ɂ���Ă̓t���[�Y����̂Ŗ��������B�K�v�ɉ����Ė߂�
	//if (p_ptr->wizard && result_param < 100) result_param = 100;

	return result_param;

}



//�V������u�A�r���e�B�J�[�h�v�̃J�[�h����/����������
//�J�[�h�I���␬������͂��łɍς�ł���
//�J�[�h��I�Œ��ׂ�Ƃ���only_info��TRUE�ɂ��Ă�����Ăяo���Ɛ�����������ւ̃A�h���X���Ԃ�B
//only_info�ł̌Ăяo���͔����s�\�ȃJ�[�h�ł��Ă΂��B

cptr use_ability_card_aux(object_type *o_ptr, bool only_info)
{
	int dir;
	int card_num; //�J�[�h�̃X�^�b�N���ꂽ�����@�ۗL�J�[�h�̌��ʂɉe���@10���ȏ㎝���ĂĂ��ő�l��9
	int card_idx;
	int card_charge;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_format("ERROR:use_ability_card_aux()�ɃA�r���e�B�J�[�h�ȊO���n���ꂽ(Tval:%d)", o_ptr->tval);
		return NULL;
	}
	card_idx = o_ptr->pval;

	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:use_ability_card()�ɕs���ȃJ�[�hidx(%d)���������A�r���e�B�J�[�h���n���ꂽ", card_idx);
		return NULL;
	}

	card_num = count_ability_card(card_idx);

	if (!ability_card_list[card_idx].activate && !only_info)
	{
		msg_format("ERROR:�����s�\�ȃJ�[�h���������ꂽ(idx:%d)", card_idx);
		return NULL;
	}

	//card_charge = ability_card_list[o_ptr->pval].charge_turn;



	switch (card_idx)
	{

	case ABL_CARD_BLANK:
	{
		if (p_ptr->pclass != CLASS_CHIMATA)
		{
			if (only_info) return format(" ���̃J�[�h�͂Ȃ�̖��ɂ������Ȃ��B");
			msg_print("�g���Ƒ����i���S�đ����ɗ��������ɂȂ����I�Q�Ăđ������������B");

		}
		else
		{
			if (only_info) return format(" ���̃J�[�h���g���ĐV���ȃJ�[�h����邱�Ƃ��ł���B�Ώۂ̃����X�^�[�ɗאڂ��Ďg��Ȃ���΂Ȃ�Ȃ��B�������̓��x�����E�m�\�E��p�Ō��܂�B�����E�����E�N�O�E���|�E�h��ቺ��Ԃ̃����X�^�[�ɂ͐������₷���B���̍s�ׂ̓����X�^�[��{�点��B");

			if (!chimata_use_blank_card()) return NULL;
		}

	}
	break;

	case ABL_CARD_LIFE:
	case ABL_CARD_FEAST:
	case ABL_CARD_PHOENIX:
	{
		int add_hp;

		add_hp = calc_ability_card_mod_param(card_idx, card_num);

		if (only_info) return format(" ���̃J�[�h���������Ă���ƍő�HP��%d��������B�������ʂ̃J�[�h�𕡐���ޏ������Ă���ꍇ��������L���B", add_hp);

	}
	break;
	case ABL_CARD_SPELL:
	case ABL_CARD_FUTO:
	{
		int add_sp;

		add_sp = calc_ability_card_mod_param(card_idx, card_num);

		if (only_info) return format(" ���̃J�[�h���������Ă���ƍő�MP��%d��������BMP�̔��ɒႢ�ꕔ�̐E�ɂ͖����B�������ʂ̃J�[�h�𕡐���ޏ������Ă���ꍇ��������L���B", add_sp);

	}
	break;
	case ABL_CARD_NAZ:
	{

		if (only_info) return format(" ���̃J�[�h�ɂ͉��̎g�������Ȃ����������l�������B");

	}
	break;
	case ABL_CARD_RINGO:
	{
		int v,base = 10;
		{

			if (only_info) return format(" ���̃J�[�h�𔭓������%d+1d%d�^�[���r�́E��p�E�ϋv��3�|�C���g�㏸����B�ʏ�̌��E�l�𒴂��Ȃ��B",base,base);

			v = base + randint1(base);

			set_tim_addstat(A_STR, 3, v, FALSE);
			set_tim_addstat(A_CON, 3, v, FALSE);
			set_tim_addstat(A_DEX, 3, v, FALSE);

		}
		break;

	}
	case ABL_CARD_ONMYOU1:
	{
		int dam = 20 + plev + chr_adj;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�_���[�W�̔j�ב����̃{�[������B", dam);

		if (!get_aim_dir(&dir)) return NULL;

		fire_ball(GF_HOLY_FIRE, dir, dam, 1);

	}
	break;
	case ABL_CARD_ONMYOU2:
	{
		int dam = 40 + plev + chr_adj;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�_���[�W�̔j�ב����̃{���g����B", dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_bolt(GF_HOLY_FIRE, dir, dam);

	}
	break;

	case ABL_CARD_HAKKE1:
	{
		int dam = 20 + plev + chr_adj;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�_���[�W�̑M�������̃r�[������B", dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_beam(GF_LITE, dir, dam);

	}
	break;

	case ABL_CARD_HAKKE2:
	{
		int dam = 65 + plev * 5 / 2 + chr_adj * 2;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�_���[�W�̃��P�b�g����B", dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_rocket(GF_ROCKET, dir, dam, 2);

	}
	break;
	case ABL_CARD_KNIFE1:
	{
		//�����Z�̃y�[�W����ۃR�s�[�����B���������͍̂s�V�������񂾂�
		int tx, ty;
		int quality = plev / 5;
		object_type forge;
		object_type *q_ptr = &forge;
		if (only_info) return format("���̃J�[�h�𔭓������1d3(+%d,+%d)�̃i�C�t�𐶐����ē�������B", quality, quality);

		if (!get_aim_dir(&dir)) return NULL;
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}
		else
		{
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
		}
		object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_KNIFE));
		q_ptr->to_d = quality;
		q_ptr->to_h = quality;
		q_ptr->discount = 99;
		object_known(q_ptr);
		msg_print("���Ȃ��̓i�C�t�𓊂����B");
		do_cmd_throw_aux2(py, px, ty, tx, 1, q_ptr, 0);

		break;
	}
	break;

	case ABL_CARD_KNIFE2:
	{
		int dam = 20 + plev * 2 + chr_adj;

		int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_OPT;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�_���[�W�̔j�Б����{���g����B���̃{���g���ǂɓ�����Ƃ��������x���������_���ȓG�̂ق��֔�ԁB", dam);

		if (!get_aim_dir(&dir)) return NULL;

		project_hook(GF_SHARDS, dir, dam, flg);

	}
	break;

	case ABL_CARD_SANAE1:
	{
		int base = 3;
		if (only_info) return format(" ���̃J�[�h�𔭓������%d+1d%d�^�[����Ƀ_���W��������A�҂ł���B�n�ォ��_���W�����ɖ߂�Ƃ��ɂ͎g���Ȃ��B",base,base);

		if (!dun_level)
		{
			msg_print("�n��ł͎g���Ȃ��B");
			return NULL;
		}

		if (EXTRA_MODE)
		{
			msg_print("���͎g���Ȃ��B");
			return NULL;

		}

		recall_player(base+randint1(base));

	}
	break;

	case ABL_CARD_EIKI:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info) return format(" ���̃J�[�h���������Ă���ƁA��Ⴢ�ӎ��s���ɂȂ����Ƃ��ɑ����ɕ��A�ł���\��������B���������̎��ɂ͗L�����%d%%��D�������B",prob );

	}
	break;

	case ABL_CARD_SANAE2:
	{

		int base = 20;

		if (only_info)	return format(" ���̃J�[�h�𔭓������%d+1d%d�^�[���łɑ΂���ꎞ�I�ȑϐ��𓾂�B", base, base);

		set_oppose_pois(base + randint1(base), FALSE);

	}
	break;


	case ABL_CARD_ICE_FAIRY:
	{
		int dam = 10 + plev * 3 + chr_adj * 3;

		if (only_info) return format(" ���̃J�[�h�𔭓�����Ɨא�%d�_���[�W�̋���ȋɊ������̃{�[���𔭐�������B", dam );

		project(0, 6, py, px, dam*2, GF_ICE, PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);


	}
	break;



	case ABL_CARD_TANUKI:
	{
		int power = plev + p_ptr->stat_ind[A_CHR]+3;

		if (only_info) return format(" ���̃J�[�h�𔭓�����Ǝ��͂̏���m�邱�Ƃ��ł���B���x���Ɩ��͂������قǑ����̏�񂪏W�܂�B");
		msg_print("�q�K�������z�����ď����W�߂Ă����I");

		map_area(DETECT_RAD_DEFAULT);
		if (power>30) detect_monsters_normal(DETECT_RAD_DEFAULT);
		if (power>40) detect_monsters_invis(DETECT_RAD_DEFAULT);
		if (power>50) detect_objects_gold(DETECT_RAD_DEFAULT);
		if (power>60) detect_objects_normal(DETECT_RAD_DEFAULT);
		if (power>70) detect_traps(DETECT_RAD_DEFAULT,TRUE);
		if (power>80) p_ptr->feeling_turn = 0;
	}
	break;



	case ABL_CARD_QUARTER_GHOST:
	case ABL_CARD_SPARE_GHOST:
	{
		int dice = card_num+1;
		int sides = 9;
		if (card_num >= 9) sides=10;

		if (only_info) return format(" ���̃J�[�h���������Ă���ƓG�̗אڍU���ɑ΂��Ēn��������%dd%d�_���[�W�̔������s���B", dice,sides);
	}
	break;


	case ABL_CARD_BACKDOOR:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" ���̃J�[�h���������Ă���ƓG�̃{���g�E�r�[���U����%d%%�̊m���Ŗ���������B", prob);
	}
	break;
	case ABL_CARD_DANMAKUGHOST:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" ���̃J�[�h���������Ă���ƓG�̃{�[���U����%d%%�̊m���Ŗ���������B", prob);
	}
	break;
	case ABL_CARD_BASS_DRUM:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" ���̃J�[�h���������Ă���ƓG�̃��P�b�g�U����%d%%�̊m���Ŗ���������B", prob);
	}
	break;
	case ABL_CARD_PSYCHOKINESIS:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" ���̃J�[�h���������Ă���ƓG�̃u���X�U����%d%%�̊m���Ŗ���������B", prob);
	}
	break;
	case ABL_CARD_UFO:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" ���̃J�[�h���������Ă���ƓG�̓��ˌn�U�����@��u���X�Ȃǂ�%d%%�̊m���Ŗ���������B", prob);
	}
	break;


	case ABL_CARD_MANEKINEKO:
	{
		if (only_info)return format(" ���̃J�[�h���������Ă���ƓX�̏��i������ւ��₷���Ȃ�A���i�̎��������ǂ��Ȃ�B");
	}
	break;

	case ABL_CARD_YAMAWARO:
	{
		if (only_info)return format(" ���̃J�[�h���������Ă���ƓX�ɐV���ɕ��ԏ��i���l��������₷���Ȃ�B");
	}
	break;

	case ABL_CARD_SYUSENDO:
	{
		if (only_info)
		{
			if(card_num == 1)
				return format(" ���̃J�[�h���������Ă���ƓD�_�ɕ��𓐂܂�Ȃ��Ȃ�B");
			else if (card_num < 9)
				return format(" ���̃J�[�h���������Ă���ƓD�_�ɕ��𓐂܂�Ȃ��Ȃ�B����ɓD�_�������o�����Ƃ�%d%%�̊m���ŖW�Q����B", (card_num-1) * 10);
			else
				return format(" ���̃J�[�h���������Ă���ƓD�_�ɕ��𓐂܂�Ȃ��Ȃ�B����ɓD�_�������o�����Ƃ�W�Q����B");
		}
	}
	break;

	case ABL_CARD_HANIWA:
	{
		int sav_bonus = calc_ability_card_prob(card_idx, card_num);
		if (only_info)
			return format(" ���̃J�[�h���������Ă���Ɩ��@�h��\�͂�%d�㏸����B",sav_bonus);

	}
	break;

	case ABL_CARD_MAGATAMA:
	{
		int ac_bonus = calc_ability_card_prob(card_idx, card_num);

		if (only_info)
			return format(" ���̃J�[�h���������Ă����AC��%d�㏸����B",ac_bonus);
	}
	break;

	case ABL_CARD_YACHIE:
	{
		int percen=10;

		if(card_num>1)percen += (card_num-1) * 2;
		if (card_num >= 9) percen = 30;

		if (only_info)
			return format(" ���̃J�[�h���������Ă���ƓG��|�����Ƃ����������A�C�e����%d%%��������B����A�C�e���𗎂Ƃ��Ȃ��G�ɂ͌��ʂ��Ȃ��B",percen);

	}
	break;

	case ABL_CARD_SYANHAI:
	{

		if (only_info)	return format(" ���̃J�[�h�𔭓�����Ɓu�l�`�v����̔z���Ƃ��ď�������B");

		if (summon_named_creature(0, py, px, MON_ALICE_DOLL, PM_FORCE_PET))
			msg_print("�l�`��������B");

	}
	break;

	case ABL_CARD_LILYWHITE:
	{
		bool flag = FALSE;
		if (only_info)	return format(" ���̃J�[�h�𔭓�����ƓG�ΓI�ȁw�����[�z���C�g�x����������B�����[�z���C�g�͌ĂԂ��тɋ����Ȃ�B");

		flag_generate_lilywhite = o_ptr->xtra3+1;
		if (summon_named_creature(0, py, px, MON_RANDOM_UNIQUE_2, (PM_FORCE_ENEMY|PM_NO_ENERGY))) flag=TRUE;
		flag_generate_lilywhite = 0;

		if (flag)
		{
			msg_print("�����[�z���C�g���P���Ă����I");
			if (o_ptr->xtra3 < 99) o_ptr->xtra3++;
		}
		else
		{
			msg_print("�����N����Ȃ������B");
		}

	}
	break;

	case ABL_CARD_EIRIN_DRUG:
	{
		int base = 4;

		if (only_info)	return format(" ���̃J�[�h�𔭓������%d+1d%d�^�[�����G������BMP200�������B",base,base);

		if (p_ptr->csp < 200)
		{
			msg_print("MP������Ȃ��B");
			return NULL;
		}
		p_ptr->csp -= 200;
		p_ptr->redraw |= PR_MANA;

		set_invuln(base + randint1(base), FALSE);


	}
	break;
	case ABL_CARD_INABA_LEG:
	{
		int prob = 25;

		if (card_num > 1) prob += (card_num - 1) * 8;
		if (card_num >= 9) prob = 100;

		if (only_info)	return format(" ���̃J�[�h���������Ă���ƃU�b�N�̒��̃A�C�e����%d%%�̊m���ő����U���ɂ��j�󂩂�����B", prob);

	}
	break;

	case ABL_CARD_SAKI:
	{
		int bonus = 10;
		if (card_num > 1) bonus += (card_num - 1) * 2;
		if (card_num >= 9) bonus = 32;

		if (only_info)	return format(" ���̃J�[�h���������Ă���ƗאڍU����(+%d,+%d)�̃{�[�i�X�𓾂�B", bonus,bonus);


	}
	break;

	case ABL_CARD_KOISHI:
	{
		int base = 20;

		if (only_info)	return format(" ���̃J�[�h�𔭓������%d+1d%d�^�[���G�ɔF������Â炭�Ȃ�B", base, base);

		set_tim_superstealth(base + randint1(20), FALSE, SUPERSTEALTH_TYPE_NORMAL);

	}
	break;

	case ABL_CARD_KISERU:
	{
		int base = 15;
		int v;

		if (only_info)	return format(" ���̃J�[�h�𔭓������%d+1d%d�^�[���m�C���g�A�����A����m������B", base, base);

		v = base + randint1(base);
		set_hero(v, FALSE);
		set_fast(v, FALSE);
		set_shero(v, FALSE);
	}
	break;


	case ABL_CARD_JIZO:
	{
		int percen = calc_ability_card_prob(card_idx, card_num);

		if (only_info)	return format(" ���̃J�[�h���������Ă����HP�̎��R�񕜗ʂ�%d%%��������B�}�񕜂ɗݐς��Č��ʂ��o��B���łɓ��l�̔\�͂������Ă���ꍇ�͌��ʂ��Ȃ��B", percen);
	}

		break;

	case ABL_CARD_PATHE_SPELL:
	{
		int percen = calc_ability_card_prob(card_idx, card_num);

		if (only_info)	return format(" ���̃J�[�h���������Ă����MP�̎��R�񕜗ʂ�%d%%��������B�}�񕜂ɗݐς��Č��ʂ��o��B���łɓ��l�̔\�͂������Ă���ꍇ�͌��ʂ��Ȃ��B", percen);
	}
		break;


	case ABL_CARD_AKAGAERU:
	{
		int mult= 75;
		int rad = 1;

		if (card_num > 1) mult += (card_num - 1) * 7;
		if (card_num >= 9)
		{
			rad = 2;
			mult = 150;
		}
		if (only_info) return format("���̃J�[�h���������Ă���ƃv���C���[���g�p�����{���g�n�U�������a%d�̃��P�b�g�ɕω�����B�З͂�%d%%�ɂȂ�B",rad,mult);
	}
	break;

	case ABL_CARD_TENGU_GETA:
	{
		int len = 3 + plev / 7;

		if (only_info) return format("���̃J�[�h�𔭓������%d�O���b�h����u�ňړ����A�G�ɓ��������炻�̂܂ܒʏ�U�����s���B", len);
		if (!rush_attack(NULL, len, 0)) return NULL;
	}
	break;

	case ABL_CARD_MUKADE:
	{
		int border_lis[10] = { 100,95,92,89,86,86,83,80,77,75 };
		int add_lis[10]		= { 1,1,1,1,1,2,2,2,2,3 };

		if (only_info) return format("���̃J�[�h���������Ă���ƁAHP���ő�l��%d%%�ȏ�̂Ƃ��Ɍ���אڍU���񐔂�%d������B", border_lis[card_num], add_lis[card_num]);

	}
	break;

	case ABL_CARD_SAKI_2:
	{
		int border_lis[10] = { 50,55,58,61,64,64,67,70,73,75 };
		int add_lis[10] = { 1,1,1,1,1,2,2,2,2,3 };

		if (only_info) return format("���̃J�[�h���������Ă���ƁAHP���ő�l��%d%%�����̂Ƃ��Ɍ���אڍU���񐔂�%d������B", border_lis[card_num], add_lis[card_num]);


	}
	break;

	case ABL_CARD_SUKIMA:
	{

		if (only_info) return format("���̃J�[�h�𔭓�����Ɨאڂ����ǂȂǂ̌������֏u�Ԉړ�����B�}�b�v�[�ɓ��B����Ɣ��Α��֏o��B�ʏ�̃e���|�[�g�ňړ��ł��Ȃ��ꏊ�ɂ͏o���Ȃ��B");

		if (!wall_through_telepo(0)) return NULL;

	}
	break;

	case ABL_CARD_KODUCHI:
	{

		if (only_info) return format("���̃J�[�h�𔭓�����ƃA�C�e�����ЂƂI������1/3�̉��l�́��ɕς��邱�Ƃ��ł���B");

		alchemy(0);
	}
	break;

	case ABL_CARD_KEIKI:
	{

		if (only_info) return format("���̃J�[�h�𔭓�����ƍz�΁E��Όn�A�C�e�����ЂƂI������3�{�̉��l�́��ɕς��邱�Ƃ��ł���B");

		alchemy(1);
	}
	break;


	case ABL_CARD_KANAMEISHI:
	{

		int y, x;
		monster_type *m_ptr;
		int damage = plev * 3;

		if (only_info) return format("���̃J�[�h�𔭓�����Ǝ��E���̏���ӏ����u��΁v�n�`�ɕς���B�w��ʒu�Ƀ����X�^�[�������ꍇ%d�̃_���[�W��^����B",damage);


		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx)
		{
			if (!cave_naked_bold(y, x))
			{
				msg_print("�����ɂ͉���������悤���B��߂Ă������B");
				return NULL;
			}
			msg_print("�n�ʂɐΉ��ςݏd�˂��B");
			cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			msg_format("%s�̓���ɗv�΂������Ă����I", m_name);
			project(0, 0, y, x, damage, GF_ARROW, PROJECT_KILL, -1);
		}

		break;



	}
	break;

	case ABL_CARD_LUNATIC:
	{
		int base = 100 + plev * 6;
		int sides = 100 + chr_adj * 10;

		if (only_info) return format("���̃J�[�h�𔭓�����Ǝw�肵���O���b�h��%d+1d%d�_���[�W��覐Α����̃{�[���ōU������B", base,sides);

		if (!get_aim_dir(&dir)) return NULL;
		if(!fire_ball_jump(GF_METEOR, dir, base + randint1(sides), 3, NULL)) return NULL;

	}
	break;

	case ABL_CARD_MIKO:
	{
		int base = 50 + plev + chr_adj * 5;
		int time = 15 + randint1(15);
		if (only_info) return format("���̃J�[�h�𔭓�����Ǝ��E�����ׂĂɑ΂�%d�_���[�W�̑M�������U�����s���B����Ɉꎞ�I�ɑΎ׈����E�Ɛ��Ȃ�I�[���𓾂�B", base);

		project_hack2(GF_LITE, 0, 0, base);
		set_protevil(time, FALSE);
		set_tim_sh_holy(time, FALSE);

	}
	break;

	case ABL_CARD_VAMPIRE_FANG:
	{
		int dice = plev+10;
		int sides = 10 + chr_adj / 5;
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format("���̃J�[�h�𔭓�����Ɨאڂ��������X�^�[��̂�%dd%d�̖������_���[�W��^���A����ɑϐ��𖳎����ĞN�O�Ƃ�����B", dice,sides);

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;
		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
			msg_format("%s�։��˂����Ă��I", m_name);

			project(0, 0, y, x, damroll(dice, sides), GF_MISSILE, PROJECT_KILL | PROJECT_JUMP, -1);
			if (m_ptr->r_idx)
			{
				msg_format("%s�̓t���t���ɂȂ����B", m_name);
				(void)set_monster_stunned(cave[y][x].m_idx, MON_STUNNED(m_ptr) + 4 + randint0(4));
			}
		}

	}
	break;

	case ABL_CARD_SUBTERRANEAN:
	{
		int dam = 500 + plev * 10 + chr_adj * 20;

		if (only_info) return format("���̃J�[�h�𔭓�����Ǝ��E�����ׂĂɑ΂�%d�_���[�W�̊j�M�����U�����s���B", dam);

		project_hack2(GF_NUKE, 0, 0, dam);

	}
	break;

	case ABL_CARD_MUGIMESHI:
	{
		int heal = 400;

		if (only_info) return format("���̃J�[�h�𔭓������HP��%d�񕜂��A�łƐ؂菝�����Â��A�����ɂȂ�A�ቺ���Ă�����̔\�͂𕜊�������B",heal);

		hp_player(400);
		set_poisoned(0);
		set_cut(0);
		set_food(PY_FOOD_MAX - 1);
		do_res_stat(A_STR);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);

	}
	break;

	case ABL_CARD_KAMIYAMA:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info) return format("���̃J�[�h�������Ă���ƁA����̌Œ�A�[�e�B�t�@�N�g���������X�^�[��|�����Ƃ��ɂ��̃A�[�e�B�t�@�N�g�𗎂Ƃ��m����%d.%02d�{�ɂȂ�B",(prob/100+1),(prob%100));

	}
	break;

	case ABL_CARD_TSUKASA:
	{

		if (only_info) return format("���̃J�[�h�𔭓�����ƍő�HP��1/3��HP������ē��ʂ�MP���񕜂���B�ꎞ�I�ɘr�́E��p�E�ϋv���ቺ���邱�Ƃ�����B");

		if (p_ptr->pclass == CLASS_CLOWNPIECE)
		{
			msg_print("���Ȃ��͂��̃J�[�h���g���Ȃ��B");
			return NULL;
		}

		if (p_ptr->chp < p_ptr->mhp/3)
		{
			msg_print("�J�[�h���g���Ȃ��B�̗͂����Ȃ�����I");
			return NULL;
		}
		take_hit(DAMAGE_USELIFE, p_ptr->mhp / 3, "�ǌς̃J�[�h", -1);
		player_gain_mana(p_ptr->mhp/3);

		if (one_in_(2)) do_dec_stat(A_STR);
		if (one_in_(2)) do_dec_stat(A_DEX);
		if (one_in_(2)) do_dec_stat(A_CON);

	}
	break;

	case ABL_CARD_KYOUTEN:
	{
		int percen = CALC_ABL_KYOUTEN_RECHARGE_BONUS(card_num);


		if (only_info) return format("���̃J�[�h���������Ă���Ɩ�����̎��R�[�U��%d%%�����Ȃ�B",percen);
	}
	break;

	case ABL_CARD_KAGUYA:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info) return format("���̃J�[�h���������Ă���ƁA�G�̍U���Ń_���[�W���󂯂��Ƃ�����%d%%��MP���񕜂���B�܂�MP�����Ō��ɑ΂���%d%%�̑ϐ��𓾂�B",prob,prob*2);


	}
	break;

	case ABL_CARD_PATHE_STUDY:
	{

		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info)
			return format("���̃J�[�h���������Ă���Ɩ��@�̎��s����%d%%�ቺ������B�ŒᎸ�s��������Ƃ��͂����%d%%�ቺ������B", prob, prob / 3);

	}
	break;

	case ABL_CARD_SUNNY:
	case ABL_CARD_LUNAR:
	case ABL_CARD_STAR:
	{
		int dam = 40 + plev * 2 + chr_adj * 2;
		int typ;
		int tx, ty;

		if (only_info)
		{
			if(card_idx == ABL_CARD_SUNNY)
				return format(" ���̃J�[�h�𔭓������%d�_���[�W�̑M�������̃r�[������B�������΂ߕ����ɂ������ĂȂ��B", dam);
			else if (card_idx == ABL_CARD_LUNAR)
				return format(" ���̃J�[�h�𔭓������%d�_���[�W�̈Í������̃r�[������B�������㉺�����ɂ������ĂȂ��B", dam);
			else
				return format(" ���̃J�[�h�𔭓������%d�_���[�W��覐Α����̃r�[������B���������E�����ɂ������ĂȂ��B", dam);
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;


		if (card_idx == ABL_CARD_SUNNY)
		{
			typ = GF_LITE;
			if (dir != 1 && dir != 3 && dir != 7 && dir != 9)
			{
				msg_print("���̕����ɂ͌��ĂȂ��B");
				return NULL;
			}
		}
		else if (card_idx == ABL_CARD_LUNAR)
		{
			typ = GF_DARK;
			if (dir != 2 && dir != 8)
			{
				msg_print("���̕����ɂ͌��ĂȂ��B");
				return NULL;
			}
		}
		else
		{
			typ = GF_METEOR;
			if (dir != 4 && dir != 6)
			{
				msg_print("���̕����ɂ͌��ĂȂ��B");
				return NULL;
			}
		}

		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];

		msg_print("�r�[����������I");
		project(0, 0, ty, tx, dam, typ, (PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

	}
	break;
	case ABL_CARD_AUNN:
	{

		//�����~���Ɠ���
		int gain_mana = 3 + plev / 20;

		if (only_info) return format("���̃J�[�h�𔭓������MP���͂��ɉ񕜂���B");

		msg_print("���Ȃ��͌ċz�𐮂���...");

		if(player_gain_mana(gain_mana))
			msg_print("���������n�b�L�������B");

	}
	break;

	case ABL_CARD_FLAN:
	{
		int base = 12, sides = 4;

		if (only_info) return format("���̃J�[�h�𔭓�����Ǝ���%d+1d%d�O���b�h��*�j��*����B",base,sides);

		destroy_area(py, px, base + randint1(sides), FALSE, FALSE, FALSE);

	}
	break;
	case ABL_CARD_LARVA:
	{
		int dam = 50 + plev + chr_adj * 2;

		if (only_info) return format(" ���̃J�[�h�𔭓�����Ǝ��͂̃����X�^�[���ő����%d�ō����E�U���͒ቺ������B", dam);

		msg_print("�ؕ���U��܂����I");
		project(0, 6, py, px, dam * 2, GF_OLD_CONF, PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);
		project(0, 6, py, px, dam * 2, GF_DEC_ATK, PROJECT_HIDE | PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);
	}
	break;

	case ABL_CARD_URUMI:
	{
		int dam = plev / 2 + 5;

		if (only_info) return format(" ���̃J�[�h�𔭓�����Ǝ��͂̃����X�^�[�̈ړ���Z���ԖW�Q����B", dam);

		msg_print("�G�ꂽ�ւ̂悤�Ȃ��̂��ӂ�ɗ��݂���...");
		project(0, 4, py, px, dam * 2, GF_NO_MOVE, PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);
	}
	break;

	case ABL_CARD_JYOON:
	{
		int bonus = calc_ability_card_mod_param(card_idx, card_num);

		if (only_info) return format(" ���̃J�[�h���������Ă����vault�Ȃǂ̓���n�`��%d.%02d�{�o�₷���Ȃ�B", bonus/100,bonus%100);
	}
	break;

	case ABL_CARD_SHION:
	{
		int bonus = calc_ability_card_mod_param(card_idx, card_num);

		if (only_info) return format(" ���̃J�[�h���������Ă���ƃt���A��荂�����x���̃����X�^�[��%d.%02d�{�o�ɂ����Ȃ�B", bonus / 100, bonus % 100);
	}
	break;

	case ABL_CARD_DOREMY:
	{
		if (only_info) return format(" ���̃J�[�h�𔭓�����ƃt���A����u�ōč\������B�n���N�G�X�g�_���W�����ł͌��ʂ��Ȃ��B");
		if (p_ptr->inside_arena)
		{
			msg_print("�����ł͎g���Ȃ��B");
			return NULL;
		}
		//1�^�[���㎞�����ϗe
		msg_print("�����̊��o���B���ɂȂ��Ă���...");
		p_ptr->alter_reality = 1;
		p_ptr->redraw |= (PR_STATUS);

	}
	break;

	case ABL_CARD_LIFE_EXPLODE:
	{
		if (only_info) return format(" ���̃J�[�h�𔭓�����ƓG����̍U������x��������������悤�ɂȂ�B���̂Ƃ�HP/2�̈З͂̋C�����̑唚�����N�������͂̓G�Ƀ_���[�W��^����B");

		msg_print("�̂��琶���͂�瞂�C������I");
		p_ptr->special_defense |= SD_LIFE_EXPLODE;
		p_ptr->redraw |= (PR_STATUS);

	}
	break;

	case ABL_CARD_JUNKO:
	{
		int power = plev * 8;
		if (only_info) return format(" ���̃J�[�h�𔭓�����ƍL�͈͂̓G�ɑ΂��p���[%d�̃t���A�Ǖ������݂�B",power);

		msg_print("���Ȃ��͈��|�I�ȈЈ����������o�����I");
		mass_genocide_2(power, 40, 2);
	}
	break;

	case ABL_CARD_LUNATIC_TORCH:
	{
		if (only_info) return format(" ���̃J�[�h�𔭓�����ƘA���Ŗ��@���r�����邱�Ƃ��ł���B�������g�p���HP��100�����B");

		if (!cp_ptr->realm_aptitude[0])
		{
			msg_print("���Ȃ��͎������g���Ȃ��B");
			return NULL;
		}

		if (!can_do_cmd_cast()) return NULL;
		do_cmd_cast();
		handle_stuff();
		if (can_do_cmd_cast()) do_cmd_cast();

		msg_print("�A���r���̔������̂��P�����I");
		if(!p_ptr->is_dead) take_hit(DAMAGE_USELIFE, 100, "���𕰂�����", -1);

	}
	break;

	case ABL_CARD_SEIRAN:
	{

		int prob = calc_ability_card_prob(card_idx, card_num);
		int dam = plev*2;

		if (only_info) return format(" ���̃J�[�h���������Ă���ƁA�퓬���Ƀ^�[�Q�b�g�ɂ��������X�^�[�ɑ΂�%d%%�̊m���Ń��P�b�g�ɂ��%d�_���[�W�̒ǌ����s����B", prob,dam);
	}
	break;


	case ABL_CARD_TEWI2:
	{
		int x, y;
		if (only_info) return format(" ���̃J�[�h�𔭓�����ƃ^�[�Q�b�g�̑����ɂ���g���b�v�𔭓�������B�g���b�v���Ȃ���΃����_���ȃg���b�v�𐶐����Ĕ��������݂�B�v���C���[���g���b�v�̉e�����󂯂�B");

		if (!get_aim_dir(&dir)) return NULL;

		x = target_col;
		y = target_row;

		if (!projectable(y, x, py, px))
		{
			msg_print("���E���̈�_���w�肵�Ȃ��Ƃ����Ȃ��B");
			return NULL;
		}

		//㩂��Ȃ���΍��
		if (!is_trap(cave[y][x].feat))
		{
				place_trap(y, x);
		}

		project(0, 0, y, x, 0, GF_ACTIV_TRAP, (PROJECT_GRID | PROJECT_JUMP | PROJECT_HIDE), -1);

	}
	break;



	case ABL_CARD_KOMACHI:
	{
		int x, y;
		int dist = 2 + plev / 24;

		if (only_info) return format(" ���̃J�[�h�𔭓�����Ƌ���%d�O���b�h�ȓ��̏��Ɉ�u�ňړ��ł���B�h�A���o�R����Ə��������������Ȃ�B�n�`���s���ȏꏊ�ɂ͍s�����A�ǂ����蔲���邱�Ƃ͂ł��Ȃ��B",dist);

		if (!teleport_walk(dist)) return NULL;

	}
	break;

	case ABL_CARD_WAKASAGI:
	{
		int base = 5;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d+1d%d�^�[���̊ԃ����X�^�[���N�����ɂ����Ȃ�B", base,base);

		set_tim_stealth(randint1(base) + base, FALSE);


	}
	break;

	case ABL_CARD_NEMUNO:
	{
		int base = 100 + plev * 4;
		int sides = chr_adj * 4;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d+1d%d�_���[�W�̎ˌ������̃{���g����B���̃{���g�͔��˂���Ȃ��B", base,sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print("����Ԃ񓊂����I");
		fire_bolt(GF_ARROW, dir, base+randint1(sides));

	}
	break;

	case ABL_CARD_SUIKA:
	{
		int base = 20;
		int v;
		int percen;
		if (only_info) return format(" ���̃J�[�h�𔭓�����ƓD����ԂɂȂ�A�����%d+1d%d�^�[���r�͂Ƒϋv�͂�5�|�C���g�㏸����B���̏㏸�͒ʏ�̌��E�𒴂���B", base, base);

		if (p_ptr->pclass == CLASS_BYAKUREN)
		{
			msg_print("���Ȃ��͉����ɂ��������߂Ȃ��B");
			return NULL;
		}
		msg_print("���Ȃ��͋S�̎����ꑧ�ə�����I");
		percen = p_ptr->chp * 100 / p_ptr->mhp;
		v = base + randint1(base);
		if(p_ptr->alcohol < DRANK_3-1) set_alcohol(DRANK_3-1);

		set_tim_addstat(A_STR, 105, v, FALSE);
		set_tim_addstat(A_CON, 105, v, FALSE);
		p_ptr->chp = p_ptr->mhp * percen / 100;
		p_ptr->redraw |= PR_HP;
		redraw_stuff();

	}
	break;

	case ABL_CARD_HOUTOU:
	{
		int dam = plev * 6 + chr_adj * 6;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�_���[�W�̖������̋��͂ȃ��[�U�[����B���̃��[�U�[�͊�n�`���΂ɕς���B", dam);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print("�󓃂��P�����I");
		fire_spark(GF_HOUTOU, dir, dam, 2);

	}
	break;

	case ABL_CARD_KANAME_MISSILE:
	{

		int dam = plev * 7 + chr_adj * 5;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�_���[�W��覐Α����̃��P�b�g����B�����X�^�[�ɖ������Ȃ������ꍇ�͒��e�n�_����n�`�ɂ���B", dam);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print("�v�΂����ł������I");
		fire_rocket(GF_KANAMEISHI, dir, dam, 1);

	}
	break;

	case ABL_CARD_ROKURO_HEAD:
	{

		int wait = 30;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�^�[����Ƀt���A�̃A�C�e�����ЂƂ����Ɏ����Ă���B�����̃A�C�e�������Ȓn�`�ɒu���ꂽ�A�C�e���͎����Ă���Ȃ��B", wait);

		rokuro_head_search_item(wait, FALSE);

	}
	break;


	case ABL_CARD_KUTAKA:
	{
		int tx, ty;

		if (only_info)
				return format(" ���̃J�[�h�𔭓�����Ɣ��ɒ������̃g���l�������r�[������B������8�����꒼���ɂ������ĂȂ��B");

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;


		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];

		msg_print("���Ȃ��͑O���ɓ���񂢂��I");
		project(0, 0, ty, tx, 0, GF_KILL_WALL, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_DISI | PROJECT_LONG_RANGE), -1);

	}
	break;

	case ABL_CARD_JELLYFISH:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info) return format(" ���̃J�[�h���������Ă���ƓG�̃{���g�U����%d%%�̊m���Ŗ���������B",prob);
	}
	break;


	case ABL_CARD_YUNOMI_REIMU:
	{
		int power = 100 + plev * 3 + chr_adj * 5;

		if (only_info) return format(" ���̃J�[�h�𔭓�����ƃp���[%d�̑S�\�͒ቺ�����̃{�[������B", power);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print("����ȍ��z�c���Ԃ񓊂����I");
		fire_ball(GF_DEC_ALL, dir, power, 1);
	}
	break;

	case ABL_CARD_YUNOMI_MARISA:
	{
		int power = 100 + plev * 3 + chr_adj * 5;

		if (only_info) return format(" ���̃J�[�h�𔭓�����ƃp���[%d�̌��������̃{�[������B", power);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print("����Ȑ��^�ʂ�����݂������t�����I");
		fire_ball(GF_OLD_SLOW, dir, power, 1);
	}
	break;



	case ABL_CARD_100TH_MARKET:
	{

		if (only_info) return format(" ���̃J�[�h��6:00��������18:00�ɏ������Ă���ƃJ�[�h������Ŗ������̃J�[�h���o�₷���Ȃ�B�܂����̃J�[�h�͌������l�����ɍ����B");

	}
	break;

	case ABL_CARD_HIRARI:
	{
		int base = 10;

		if (only_info) return format(" ���̃J�[�h�𔭓������%d�^�[���g���B���ēG�̍U���̃^�[�Q�b�g����O���B����_�����U���Ɋ������܂�邱�Ƃ͂���B�܂����̏�ɗ��܂�A�x�e�A���H�A�����ύX�ȊO�̍s��������ƌ��ʂ��؂��B", base);

		set_hirarinuno_card(base, FALSE);
		break_eibon_flag = FALSE;
	}
	break;

	case ABL_CARD_NITORI:
	{

		if (only_info) return format(" ���̃J�[�h��6:00��������18:00�ɏ������Ă���ƃJ�[�h������ɔ����Ă���J�[�h�̉��i������������B");


	}


	default:
		if (only_info) return "������";

		msg_format("ERROR:��������Ă��Ȃ��A�r���e�B�J�[�h���g��ꂽ (idx:%d)", card_idx);
		return NULL;
	}

	return ""; //�������������NULL�łȂ��K���ȋ󕶎���Ԃ�

}




//�A�r���e�B�J�[�h���g�p����B�J�[�h�͂��łɑI������Ă���B��������A�`���[�W�����������s�����s�����ɓn���B
//�s��������Ȃ��Ƃ��̂�FALSE��Ԃ��B
//v1.1.94 ����������Ƀ^�[�Q�b�g�L�����Z�������ꍇ�s���͂�����Ȃ����̂Ƃ���
bool use_ability_card(object_type *o_ptr)
{
	ability_card_type *ac_ptr;

	int chance = p_ptr->skill_dev;
	int fail;
	bool success;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
		msg_print("�~�܂������̒��ł͂��܂������Ȃ��悤���B");

		sound(SOUND_FAIL);
		return FALSE;
	}

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_format("ERROR:use_ability_card()��tval%d�̃A�C�e�����n���ꂽ", o_ptr->tval);
		return FALSE;
	}

	if (o_ptr->pval < 0 || o_ptr->pval >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:use_ability_card()�ɕs���ȃJ�[�hidx(%d)���������A�r���e�B�J�[�h���n���ꂽ", o_ptr->pval);
		return FALSE;
	}

	ac_ptr = &ability_card_list[o_ptr->pval];


	//��Փx�v�Z�@���b�h�Ɠ����v�Z���g��
	if (p_ptr->confused) chance /= 2;
	fail = ac_ptr->difficulty + 5;
	if (chance > fail) fail -= (chance - fail) * 2;
	else chance -= (fail - chance) * 2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;


	//��������@���b�h�Ɠ����v�Z���g��
	//�J�[�h���l(��)�̂ݐ��i���C�ł��J�[�h���g����悤�ɂ��Ă�����
	if (p_ptr->pseikaku == SEIKAKU_BERSERK) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance * 2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail * 2) < chance) success = TRUE;
		else success = FALSE;
	}

	if (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	if (!success)
	{
		if (flush_failure) flush();
		msg_print("�J�[�h�̔����Ɏ��s�����B");

		sound(SOUND_FAIL);
		return TRUE;
	}

	if (o_ptr->timeout > ac_ptr->charge_turn * (o_ptr->number - 1))
	{
		if (flush_failure) flush();
		msg_print("���̃J�[�h�͂܂��`���[�W���I����Ă��Ȃ��B");

		return FALSE;
	}

	if ((o_ptr->timeout + ac_ptr->charge_turn) > 32767)
	{
		msg_format("ERROR:timeout�l���I�[�o�[�t���[����B");
		return FALSE;
	}

	/* Sound */
	sound(SOUND_ZAP);


	//(���Ӓ胍�b�h�̔����ɑ������鏈���͂Ȃ��B�J�[�h�͐������ɏ��*�Ӓ�*����Ă��邽��)

	
	//msg_format("���Ȃ��́w%s�x�̃A�r���e�B�J�[�h�𔭓������I", ac_ptr->card_name);
	//�����b�Z�[�W���璷�Ȃ̂ŏ���

	//���s�����B�^�[�Q�b�g�L�����Z���Ȃǂ�����NULL���Ԃ�s������Ȃ�
	if (!use_ability_card_aux(o_ptr, FALSE)) return FALSE;

	//�`���[�W�^�C�����Z
	o_ptr->timeout += ac_ptr->charge_turn;
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	return TRUE;

}


//�A�r���e�B�J�[�h�́u�����x�v��ݒ肷��B
//�Q�[���J�n������܂�6:00��18 : 00�ɌĂ΂��B
//�h�ɘA���Ŕ��܂�����h���o��u�ԂɈ�x���������Ă΂�Ȃ����������p��̖��͂Ȃ����낤
void set_abilitycard_price_rate()
{

	int mod = 0;
	int prev_rate = p_ptr->abilitycard_price_rate;

	//�Q�[���J�n���甼���o�߂��ƂɃJ�E���g
	int half_days = turn / ((TURNS_PER_TICK * TOWN_DAWN) / 2);

	//EXTRA���[�h�ł͈ꗥ�K�w/2
	if (EXTRA_MODE)
	{
		p_ptr->abilitycard_price_rate = (dun_level + 1) / 2;
		return;
	}

	//�����x10����Q�[���J�n
	if (!half_days)
	{
		mod = 10;
	}
	//3���ڔӂ܂ł�+1
	else if (half_days < 6)
	{
		mod = 1;
	}
	//7���ڔӂ܂ł�+1+1d2 
	else if (half_days < 14)
	{
		mod = 1+randint1(2);
	}
	//14���ڔӂ܂ł�+15%�`20%(14���I�����_��300���炢�ɂȂ�B���̍��ɂ̓K�`�����������ق������������͂�)
	else if (half_days < 28)
	{
		mod = prev_rate * 14 / 100 + randint1(prev_rate * 6 / 100);
	}
	//���̌�͍����x300�`600���炢�͈͓̔��ŗ������B
	else
	{
		mod = damroll(3, 100) - 150;

		if ((prev_rate + mod) < 300)
		{
			if (mod < 0 && !one_in_(3)) mod *= -1; //���܂艺�ɂ͒���t���ɂ���
			//else mod = prev_rate - 300; �h��Ɍv�Z�Ԉ���Ă�
			else mod = 300 - prev_rate;
		}
		else if ((prev_rate + mod) > 600)
		{
			mod = 600 - prev_rate;
		}

	}



	p_ptr->abilitycard_price_rate += mod;

	if (p_ptr->abilitycard_price_rate < 0)
	{
		p_ptr->abilitycard_price_rate = 300;
		msg_print("ERROR:abilitycard_price_rate���}�C�i�X�ɂȂ���");

	}

	if (CHECK_ABLCARD_DEALER_CLASS && cheat_xtra) 
		msg_format("testmsg:�A�r���e�B�J�[�h�����x�X�V:%d(%d)", p_ptr->abilitycard_price_rate, mod);


}

//�J�[�h�̔����̃��X�g�����
//�Q�[���J�n������܂�6:00��18:00�ɌĂ΂��
//���X�g��p_ptr->magic_num2[0-9]�Ɋi�[
//���E�ɂ���Ă͎g�p�ӏ������炷
void make_ability_card_store_list(void)
{
	object_type forge;
	object_type *o_ptr = &forge;
	int i;

	bool owned_card_list[ABILITY_CARD_LIST_LEN];
	int bm100th_card_num;


	//�J�[�h���l�n�̃N���X�̂�
	if (!CHECK_ABLCARD_DEALER_CLASS) return;

	for (i = 0; i < ABILITY_CARD_LIST_LEN; i++) owned_card_list[i] = FALSE;

	bm100th_card_num = count_ability_card(ABL_CARD_100TH_MARKET);

	//�u100��ڂ̃u���b�N�}�[�P�b�g�v�̃J�[�h������Ƃ��������Ă���J�[�h�̃t���O�𗧂Ă�B
	//�C���x���g���A�ǉ��C���x���g���A����A���オ�ΏہB
	if (bm100th_card_num)
	{
		store_type  *st_ptr;

		//�U�b�N
		for (i = 0; i < INVEN_PACK; i++)
		{
			if (inventory[i].tval == TV_ABILITY_CARD) owned_card_list[inventory[i].pval] = TRUE;
		}
		//�ǉ��C���x���g��
		for (i = 0; i < INVEN_ADD_MAX; i++)
		{
			if (inven_add[i].tval == TV_ABILITY_CARD) owned_card_list[inven_add[i].pval] = TRUE;
		}
		//����
		st_ptr = &town[1].store[STORE_HOME];
		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			if(st_ptr->stock[i].tval == TV_ABILITY_CARD)owned_card_list[st_ptr->stock[i].pval] = TRUE;
		}
		//����
		//�킴�ƓG�ΓI�ȗd���Ƃ����������Ă����ɃJ�[�h���E�킹���珊���Ɋ܂܂�Ȃ��Ȃ邪�A�����ł����΂����Ȃ�ق����������C������
		for (i = 1; i < o_max; i++)
		{
			object_type *o_ptr = &o_list[i];

			/* Skip dead objects */
			if (!o_ptr->k_idx) continue;

			/* Skip held objects */
			if (o_ptr->held_m_idx) continue;

			if (o_ptr->tval == TV_ABILITY_CARD) owned_card_list[o_ptr->pval] = TRUE;
		}

		/* �t���O�e�X�g */
		if (cheat_peek)
		{
			for (i = 0; i < ABILITY_CARD_LIST_LEN; i++)
			{
				//if (ability_card_list[i].rarity_rank < 4) owned_card_list[i] = TRUE;
				if (owned_card_list[i])	msg_format("idx%d:TRUE", i);
			}
		}
	}


	for (i = 0; i < 10; i++)
	{
		int tmp;

		while (1)
		{

			//�_�~�[�J�[�h���� ���̃��x���������قǍ������N�̂��̂��o�₷��
			object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
			tmp = randint1(p_ptr->lev);
			if (tmp >= 45)
				apply_magic_abilitycard(o_ptr, -1, 4, 0);
			else if (tmp >= 30)
				apply_magic_abilitycard(o_ptr, -1, 3, 0);
			else if (tmp >= 15)
				apply_magic_abilitycard(o_ptr, -1, 2, 0);
			else
				apply_magic_abilitycard(o_ptr, -1, 0, 0);

			//�u100��ڂ̃u���b�N�}�[�P�b�g�v�̃J�[�h�������Ă���Ƃ��A���L�ς݂̃J�[�h����������悤�Ƃ�����m���ł�蒼��
			if (bm100th_card_num)
			{
				int prob = calc_ability_card_prob(ABL_CARD_100TH_MARKET, bm100th_card_num);

				if (owned_card_list[o_ptr->pval] && randint0(100) < prob)
				{
					if(cheat_peek) msg_print("continue");
					continue;
				}
			}

			break;
		}
		//�J�[�h�̎��(pval�l)���L�^
		p_ptr->magic_num2[i+ ABLCARD_MAGICNUM_SHIFT] = o_ptr->pval;

	}


}

//�A�r���e�B�J�[�h�̔̔����i���v�Z
int calc_ability_card_price(int card_idx)
{
	int price;
	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:calc_ability_card_price()�ɓn���ꂽcard_idx(%d)���͈͊O", card_idx);
		return 0;
	}

	price = 300 + p_ptr->abilitycard_price_rate * ability_card_list[card_idx].trade_value;

	if (ability_card_list[card_idx].rarity_rank == 4) price *= 30;
	else if (ability_card_list[card_idx].rarity_rank == 3) price *= 10;
	else if (ability_card_list[card_idx].rarity_rank == 2) price *= 3;

	return price;

}



//�אڂ��������X�^�[��̂��w�肵�A���̃����X�^�[�ɂ��Ȃ񂾃J�[�h������Δ������B
//������̂̓J�[�h���ނɂ��ꖇ�܂ŁBp_ptr->magic_num1[]�ɔ���ς݂̃J�[�h���L�^����B
//�s���������Ƃ�TRUE
bool	buy_abilitycard_from_mon(void)
{
	int i;
	int price = 0;
	int dir;
	monster_type *m_ptr;
	int y, x;
	char m_name[160];

	object_type forge;
	object_type *o_ptr = &forge;


	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		msg_print("ERROR:�J�[�h���l�n�ȊO�̃N���X��buy_abilitycard_from_mon()���Ă΂ꂽ");
		return FALSE;
	}
	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print("���͉ו�����t���B");
		return FALSE;
	}

	//�אڃ����X�^�[�w��
	if (!get_rep_dir2(&dir)) return FALSE;
	if (dir == 5) return FALSE;

	y = py + ddy[dir];
	x = px + ddx[dir];
	m_ptr = &m_list[cave[y][x].m_idx];

	if (!cave[y][x].m_idx || !m_ptr->ml)
	{
		msg_format("�����ɂ͉������Ȃ��B");
		return FALSE;
	}

	//�N����
	set_monster_csleep(cave[y][x].m_idx, 0);

	monster_desc(m_name, m_ptr, 0);
	if (p_ptr->inside_arena)
	{
		msg_format("���̐��E��%s�͍D��I���I���ǂ���ł͂Ȃ��I", m_name);
		return TRUE;
	}
	msg_format("���Ȃ���%s�ɃJ�[�h�̔����������|����...", m_name);

	//�J�[�h���X�g���[�v�B�w���\�ȃJ�[�h�������i��ability_card_list[]�̃C���f�b�N�X�������ă��[�v����o��
	for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
	{
		//�J�[�h���X�g�ɓo�^���ꂽ���胂���X�^�[�̂ݗL��
		if (ability_card_list[i].r_idx != m_ptr->r_idx) continue;

		//EXTRA���[�h�ł͋A�җp�J�[�h���o�Ȃ�
		if (EXTRA_MODE && i == ABL_CARD_SANAE1) continue;
		//�����ϗe�J�[�h���o�Ȃ�
		if (EXTRA_MODE && i == ABL_CARD_DOREMY) continue;

		//v2.0.7 ��̂ݗ얲�A�������̓��x��90�ȍ~����Ŕ���\��
		if (i == ABL_CARD_YUNOMI_REIMU && dun_level < 90) continue;
		if (i == ABL_CARD_YUNOMI_MARISA && dun_level < 90) continue;

		//���p�ς݃t���O�������Ă���J�[�h�͂����o�Ȃ��B�����t���ϐ��𖳗����r�b�g�t���O�Ɏg���̂͂Ȃ񂩕s���Ȃ̂ŉ���16bit�����t���O�Ɏg���B
		if ((p_ptr->magic_num1[(i / 16)+ ABLCARD_MAGICNUM_SHIFT] >> (i % 16)) & 1L) continue;

		break;
	}

	if (i >= ABILITY_CARD_LIST_LEN)
	{
		msg_print("�J�[�h�������Ă��Ȃ��悤���B");
		return TRUE;
	}

	//�J�[�h�������Ă���Ƃ��A���i�����肵�Ċm�F�����
	//���i�͓T���甃��2/3
	price = calc_ability_card_price(i) * 2 / 3;

	//v1.1.93 �~�P�����Z�Ńt���A�ɌĂ񂾋q�͉��i�𐁂������Ă��� �����_���ɓ�{����O�{
	if (p_ptr->pclass == CLASS_MIKE && m_ptr->mflag & MFLAG_SPECIAL)
	{
		if (cheat_xtra) msg_print("(���i�㏸)");
		price = price * (200 + m_ptr->mon_random_number % 100) / 100;

	}

	if (!get_check_strict(format("�u%s�v�̃J�[�h��$%d�Ŕ����Ă����悤���B�����܂����H", ability_card_list[i].card_name, price), CHECK_DEFAULT_Y))
	{
		msg_print("���͌��􂵂��I");
		return TRUE;
	}

	if (p_ptr->au < price)
	{
		msg_print("����������Ȃ��I");
		return TRUE;
	}

	//�����������炵�ăJ�[�h����
	p_ptr->au -= price;
	p_ptr->redraw |= (PR_GOLD);
	object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
	apply_magic_abilitycard(o_ptr, i, 0, 0);
	inven_carry(o_ptr);
	msg_format("%s����J�[�h�𔃂�������I", m_name);

	//����ς݃t���O�𗧂Ă�
	p_ptr->magic_num1[(i / 16)+ ABLCARD_MAGICNUM_SHIFT] |= ((1L) << (i % 16));

	//���������������Ƃ������X�^�[��F�D�I�ɂ���B�������N�G�X�g�_���W��������N�G�X�g�����Ώۂ�_���W�����{�X�̏ꍇ�G�΂����܂�
	//����q���������X�^�[���������������r�[�z������^�R����ɂ����̂��Ȃ񂩕ςȂ̂Œ��~
	//if (!(r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR) && !p_ptr->inside_quest)
	//{
	//		set_friendly(m_ptr);
	//}

	return TRUE;
}


//�A�r���e�B�J�[�h�u���{��`�̃W�����}�v�̌���
//�J�[�h�̔����̍݌�10�킻�ꂼ��ɑ΂��A���i�𗐍���������W�����v�Z����
//6:00��18:00��process_world()����Ă΂��
void break_market()
{
	int card_num;
	int i;

	//�J�[�h���l�̂�
	if (!(CHECK_ABLCARD_DEALER_CLASS)) return;

	//�J�[�h�������x��magic_num2[10-19]�ɋL�^�����
	//0�͉e���Ȃ��A1�`250�̃����_����50����l�Ƃ���1�������邲�Ƃɉ��i��2%�㉺����

	//�܂��J�[�h�L���Ɋւ�炸���ʃ��Z�b�g
	for (i = 0; i < 10; i++)
	{
		p_ptr->magic_num2[10 + i + ABLCARD_MAGICNUM_SHIFT]=0;
	}

	//�J�[�h���������Ă��Ȃ��Ȃ���ʃ��Z�b�g�����܂܏I��
	card_num = count_ability_card(ABL_CARD_NITORI);
	if (!card_num) return;

	for (i = 0; i < 10; i++)
	{
		int tmp;

		//���i��������
		if (weird_luck())
		{
			tmp = 50 - randint1(5)*card_num - randint1(10);
			if (tmp < 1) tmp = 1;
		}
		//���i���オ��
		else
		{
			tmp = 50 + randint1(20)*card_num + randint1(40);
			if (tmp > 250) tmp = 250;

		}

		if (cheat_peek) msg_format("mult%d:%d", i, tmp);

		p_ptr->magic_num2[10 + i + ABLCARD_MAGICNUM_SHIFT] = (byte)tmp;

	}


}



//�疒���A�r���e�B�J�[�h�𔭓�����Ƃ��̎��s���v�Z�@������p�t�Ƃقړ���
int calc_chimata_activate_fail_rate(int card_idx)
{
	int level, chance;

	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:calc_chimata_activate_fail_rate(%d)", card_idx);
		return 0;
	}

	//�������̓��b�h�Ɠ����v�Z���g���Bk_info[].level�ɓ�����l�Ƃ���ability_card_list[card_idx].difficulty���g�p����
	level = (ability_card_list[card_idx].difficulty * 5 / 6 - 5);
	chance = level * 4 / 5 + 20;
	chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[A_INT]] - 1);
	level /= 2;
	if (p_ptr->lev > level)
	{
		chance -= 3 * (p_ptr->lev - level);
	}
	chance = mod_spell_chance_1(chance);
	chance = MAX(chance, adj_mag_fail[p_ptr->stat_ind[A_INT]]);
	if (p_ptr->stun > 50) chance += 25;
	else if (p_ptr->stun) chance += 15;
	if (chance > 95) chance = 95;
	chance = mod_spell_chance_2(chance);

	return chance;
}



//�疒���J�[�h���m�F�܂��͔�������
//�J�[�h�̏�����������p_ptr->magic_num2[card_idx]�Ŕ��肳��A
//�����ς݂̃J�[�h�̃`���[�W�^�[����p_ptr->magic_num1[card_idx]�ɋL�^�����
//�^�[�Q�b�g�L�����Z���ȂǍs��������Ȃ������Ƃ�FALSE
bool	chimata_activate_ability_card_power(bool only_info)
{

	int card_idx_list[100];//5�y�[�W��
	int col_max = 20;//1�y�[�W��20���܂ŕ\��
	int page_num = 0;

	int i;
	int fail_rate;
	int card_idx;//�I�����ꂽ�J�[�h
	int card_idx_list_len = 0;//�g�p�\�ȃJ�[�h�̃��X�g
	bool flag_choice = FALSE;//�J�[�h���I�����ꂽ�t���O
	bool flag_need_redraw = TRUE;//�J�[�h���X�g�ĕ`��t���O

	object_type dummyitem;
	object_type *o_ptr = &dummyitem;

	int card_number; //�ۗL�J�[�h�̈��ނ��Ƃ̖����@���ʃ����N�Ɠ����l�ɂȂ�

	if (p_ptr->pclass != CLASS_CHIMATA)
	{
		msg_format("ERROR:chimata_activate_ability_card_power(class:%d)", p_ptr->pclass);
		return FALSE;
	}
	//�Ȃ��Ƃ͎v�����Ǎ��コ��ɃA�r���e�B�J�[�h����ʂɒǉ����ꂽ�Ƃ��ɔ����Ĉꉞ
	if (ABILITY_CARD_LIST_LEN >= 100)
	{
		msg_format("ERROR:chimata_activate_ability_card_power()�̃y�[�W��������Ȃ�");
		return FALSE;
	}

	card_number = (CHIMATA_CARD_RANK);

	//card_idx_list��ability_card_list[]��idx�l���i�[����B
	//���̂܂܎g��Ȃ��͔̂����p�I���̂Ƃ������p�łȂ��J�[�h���΂�����
	for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
	{
		if (!only_info && !ability_card_list[i].activate) continue;
		card_idx_list[card_idx_list_len++] = i;
	}

#ifdef ALLOW_REPEAT
	//���s�[�g�̂Ƃ��J�[�hidx���擾����
	if (repeat_pull(&card_idx))
	{
		flag_choice = TRUE;
		flag_need_redraw = FALSE;
	}
#endif

	if (flag_need_redraw) screen_save();

	//�I����ʂ�\�����ăJ�[�h��I�����郋�[�v
	while (!flag_choice)
	{

		char c;
		int tmp;

		//�J�[�h���X�g��\��
		if (flag_need_redraw)
		{

			for (i = 2; i<25; i++) Term_erase(17, i, 255);

			c_put_str(TERM_WHITE, "�@�@�@�@       ���́@�@�@�@�@�@�@�@����", 3, 20);
			for (i = 0; i<col_max; i++)
			{
				int tmp_card_idx;
				if (page_num * col_max + i >= card_idx_list_len) break;

				tmp_card_idx = card_idx_list[page_num * col_max + i];

				//������
				if (!CHECK_CHIMATA_HAVE_CARD(tmp_card_idx))
				{
					if (tmp_card_idx == ABL_CARD_100TH_MARKET)
						c_put_str(TERM_L_DARK, "(*����*����ƊJ�������)", 4 + i, 20);
					else if (tmp_card_idx == ABL_CARD_YUNOMI_REIMU)
						c_put_str(TERM_L_DARK, "(���x��90�ȏ�́H�H����̂ݓ���ł���)", 4 + i, 20);
					else if (tmp_card_idx == ABL_CARD_YUNOMI_MARISA)
						c_put_str(TERM_L_DARK, "(���x��90�ȏ�́H�H�H����̂ݓ���ł���)", 4 + i, 20);
					else if (tmp_card_idx == ABL_CARD_SHION)
						c_put_str(TERM_L_DARK, "(�{�C���o�����H�H����̂ݓ���ł���)", 4 + i, 20);
					else
						c_put_str(TERM_L_DARK, "�H�H�H�H�H�H�H", 4 + i, 20);
				}
				//�����p�J�[�h
				else if (ability_card_list[tmp_card_idx].activate)
				{
					int valid_card_num;//�`���[�W���łȂ��g�p�\�J�[�h�̖���

					fail_rate = calc_chimata_activate_fail_rate(tmp_card_idx);

					valid_card_num = (card_number * ability_card_list[tmp_card_idx].charge_turn - p_ptr->magic_num1[tmp_card_idx]) / ability_card_list[tmp_card_idx].charge_turn;

					//�[�U���̃J�[�h�͊D�F�ɂ���
					if (valid_card_num)
						c_put_str(TERM_WHITE, format("%c) %25s(%d/%d)  %3d%%", ('a' + i), ability_card_list[tmp_card_idx].card_name, valid_card_num, card_number, fail_rate), 4 + i, 20);
					else
						c_put_str(TERM_L_DARK, format("%c) %25s(%d/%d)  %3d%%", ('a' + i), ability_card_list[tmp_card_idx].card_name, valid_card_num, card_number, fail_rate), 4 + i, 20);


				}
				//���L���ʃJ�[�h
				else
				{
					c_put_str(TERM_WHITE, format("%c) %30s  ---", ('a' + i), ability_card_list[tmp_card_idx].card_name), 4 + i, 20);

				}
			}

			c_put_str(TERM_WHITE, "(�X�y�[�X:���̃y�[�W ESC:�L�����Z��)", 4 + i, 20);

			flag_need_redraw = FALSE;
		}

		//���͂��󂯕t���ă`�F�b�N

		c = inkey();

		//�L�����Z��
		if (c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}

		//�X�y�[�X�̓y�[�W�ԍ��𑝂₵�ĕ`��
		if (c == ' ')
		{
			page_num++;
			//�y�[�W�����X�g���𒴂�����ŏ��̃y�[�W��
			if (page_num * col_max >= card_idx_list_len) page_num = 0;
			flag_need_redraw = TRUE;
			continue;
		}
		//�͈͊O�@���͂�����
		if (c < 'a' || c >= ('a' + col_max))
		{
			continue;
		}

		//�y�[�W�ԍ��Ɠ��͒l���烊�X�g�̃C���f�b�N�X���v�Z
		tmp = page_num * col_max + (c - 'a');

		//�͈͓��������X�g�̏I�[�ȍ~�@���͂�����
		if (tmp >= card_idx_list_len)
		{
			continue;
		}
		card_idx = card_idx_list[tmp];

		//���K���̃J�[�h��I���@���͂�����
		if (!CHECK_CHIMATA_HAVE_CARD(card_idx))
		{
			continue;
		}

#ifdef ALLOW_REPEAT
		repeat_push(card_idx);
#endif

		flag_need_redraw = TRUE; //���[�v���o�Ă���screen_load()�����邽��TRUE�ɂ���
		break;
	}//�J�[�h�I�����[�v�I��

	if (flag_need_redraw) screen_load();

	//�J�[�h�̗͂��g�����߂̃_�~�[�A�C�e������
	object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
	o_ptr->pval = card_idx;
	o_ptr->ident |= (IDENT_MENTAL);
	o_ptr->timeout = p_ptr->magic_num1[card_idx];
	o_ptr->number = card_number;


	//�J�[�h���m�F���邾���̏ꍇ�͍s��������Ȃ��ŏI��
	if (only_info)
	{
		screen_object(o_ptr, SCROBJ_FORCE_DETAIL);
		return FALSE;
	}

	//�[�U���̃J�[�h�̓��b�Z�[�W���o����FALSE�ŏI���@���s�[�g�ŌĂ΂�邱�Ƃ�����̂Ń��[�v�O�ŏ�������
	if (o_ptr->timeout > ability_card_list[card_idx].charge_turn * (o_ptr->number - 1))
	{
		if (flush_failure) flush();
		msg_print("�܂��`���[�W���I����Ă��Ȃ��B");

		return FALSE;
	}

	//�������s����
	fail_rate = calc_chimata_activate_fail_rate(card_idx);
	if (randint0(100) < fail_rate)
	{
		msg_print("�J�[�h�̗͂̔����Ɏ��s�����B");
		return TRUE;
	}

	if (card_idx != ABL_CARD_BLANK)	msg_format("�u%s�v�̗͂��J�������I", ability_card_list[card_idx].card_name);

	//�_�~�[�J�[�h�����s�����ɓn��
	if (!use_ability_card_aux(o_ptr, FALSE)) return FALSE;

	//�`���[�W�l��ݒ�
	p_ptr->magic_num1[card_idx] += ability_card_list[card_idx].charge_turn;

	return TRUE;

}



//�疒���J�[�h�̗��ʃ����N���v�Z����
//�J�[�h�̏����񏊎���p_ptr->magic_num2[card_idx]�ɋL�^����Ă���A
//�������Ă����ނɂ���ė��ʃ����N�����߂��p_ptr->magic_num2[255]�ɕۑ������
//���̃����N�͊e��̃J�[�h�������Ă���u�����v�Ƃ��Ĉ�����
//�߂�l�̓����N�łȂ����L�����B������\�����������Ɏg����悤��
int 	chimata_calc_card_rank(void)
{
	
	int card_num = 0;
	int card_rank = 0;
	int i;

	if (p_ptr->pclass != CLASS_CHIMATA)
	{
		msg_format("ERROR:chimata_calc_card_rank(class:%d)", p_ptr->pclass);
		return 0;
	}


	for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
	{
		if (p_ptr->magic_num2[i]) card_num++;
	}

	if (card_num == ABILITY_CARD_LIST_LEN)
		card_rank = 9;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 2)
		card_rank = 8;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 5)
		card_rank = 7;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 10)
		card_rank = 6;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 20)
		card_rank = 5;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 35)
		card_rank = 4;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 55)
		card_rank = 3;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 75)
		card_rank = 2;
	else
		card_rank = 1;

	p_ptr->magic_num2[255] = card_rank;

	if (p_ptr->wizard) msg_format("card num:%d rank:%d", card_num, card_rank);

	return card_num;
}


//�疒���J�[�h�̗��ʃ����N�ɂ���ă��b�Z�[�W��\������
void chimata_comment_card_rank(void)
{

	int card_rank, card_num;
	int i;

	if (p_ptr->pclass != CLASS_CHIMATA)
	{
		msg_format("ERROR:chimata_comment_card_rank(class:%d)", p_ptr->pclass);
		return;
	}


	//�J�[�h�𐔂��ă����N���v�Z
	card_num = chimata_calc_card_rank();

	//�v�Z���������N��p_ptr->magic_num[255]����擾
	card_rank = CHIMATA_CARD_RANK;

	screen_save();

	for (i = 10; i<16; i++) Term_erase(17, i, 255);

	prt(format("���ʃ����N:%d/9", card_rank), 11, 20);
	switch (card_rank)
	{
	case 1:

		prt("�܂��قƂ�ǒN���A�r���e�B�J�[�h�̂��Ƃ�m��Ȃ��B", 12, 20);
		prt("���̑̂������Ă��܂��O�ɂ����ƃJ�[�h�𗬒ʂ����Ȃ��Ƃ����Ȃ��B", 13, 20);
		prt("�u�󔒂̃J�[�h�v���g���V���ȃJ�[�h������čL�߂悤�B", 14, 20);
		break;

	case 2:
		prt("�܂��܂��J�[�h�̒m���x�͒Ⴂ�B", 12, 20);
		prt("�C�𔲂��ƕ��ɔ�΂��ꂻ���ɂȂ�...", 13, 20);
		break;

	case 3:
		prt("�R�t�����������ɃJ�[�h�𔃂������Ă���悤���B", 12, 20);
		prt("�܂������̂������ʂ��Ă���B", 13, 20);
		break;

	case 4:
		prt("�J�[�h���m��l���m��Ő�[�z�r�[�Ƃ��ĔF�m����Ă����B", 12, 20);
		prt("���A��Ԃ���邱�Ƃ������Ă����悤�Ɋ�����B", 13, 20);
		break;

	case 5:
		prt("�J�[�h���L�����ʂ��n�߂��B", 12, 20);
		prt("���U���ɗ͋������߂��Ă����B", 13, 20);
		break;

	case 6:
		prt("�d���̎R�ŃJ�[�h�W�߂����u�[�����I", 12, 20);
		prt("�s�������l�X���݂ȍD�ӓI�Ȏ����������Ă���B", 13, 20);
		break;

	case 7:
		prt("�d���������݂�ȃJ�[�h�ŗV��ł���I", 12, 20);
		prt("�Ȃ񂾂����肪���邢�Ǝv�����玩���̌���������B", 13, 20);
		break;

	case 8:
		prt("�l���̏Z�l����𑈂��ăJ�[�h�𔃂����߂Ă���I", 12, 20);
		prt("�c��ȐM�̗͂��̂𖞂����̂�������I", 13, 20);
		break;

	case 9:
		prt("���܂⌶�z���̂�����l�d���A�r���e�B�J�[�h�ɖ������I", 12, 20);
		prt("���ɂ��Ă̋P���������X���߂��Ă����I", 13, 20);
		break;

	default:
		msg_format("ERROR:shimata_comment_card_rank(%d)", card_rank);
		break;
	}

	inkey();

	screen_load();

}

