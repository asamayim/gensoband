#include "angband.h"
//v1.1.86
//�V������u�A�r���e�B�J�[�h�v�֌W�̊֐�




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

//�J�[�h�ɂ��HP,SP�㏸�̌v�Z��
//use_ability_card_axu()�ɂ������������̂ق��Ɏ��ۂ�HP/MP�𑝂₷�Ƃ��ɂ����������g���̂ł܂Ƃ߂邱�Ƃɂ���
int calc_ability_card_add_hp_sp(int card_idx, int card_num)
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
		case ABL_CARD_PHOENIX:
		{

			if (card_num == 9) 
				bonus = plev * 8;
			else
				bonus = plev * 3 + (card_num - 1) * plev/2;
		}
		break;
		
		default:
			msg_format("ERROR:calc_ability_card_add_hp_sp()�ɑz��O�̃J�[�h(idx:%d)���n���ꂽ",card_idx);

	}

	return bonus;


}

//�w���̔��ȂǃJ�[�h�Ɋւ���v�Z�l��Ԃ�
//�����h��m���������������ʂɑ��̃p�����[�^�Ɏg��񗝗R���Ȃ�
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



	default:
		msg_format("ERROR:calc_ability_card_prob()�ɑz��O�̃J�[�h(idx:%d)���n���ꂽ", card_idx);

	}




	if (p_ptr->wizard && result_param < 100) result_param = 100;

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
		if (only_info) return format(" ���̃J�[�h�͂Ȃ�̖��ɂ������Ȃ��B");

	}
	break;

	case ABL_CARD_LIFE:
	case ABL_CARD_PHOENIX:
	{
		int add_hp;

		add_hp = calc_ability_card_add_hp_sp(card_idx, card_num);

		if (only_info) return format(" ���̃J�[�h���������Ă���ƍő�HP��%d��������B�������ʂ̃J�[�h�𕡐���ޏ������Ă���ꍇ��������L���B", add_hp);

	}
	break;
	case ABL_CARD_SPELL:
	{
		int add_sp;

		add_sp = calc_ability_card_add_hp_sp(card_idx, card_num);

		if (only_info) return format(" ���̃J�[�h���������Ă���ƍő�MP��%d��������BMP�̔��ɒႢ�ꕔ�̐E�ɂ͖����B", add_sp);

	}
	break;
	case ABL_CARD_NAZ:
	{

		if (only_info) return format(" ���̃J�[�h�ɂ͉��̎g�������Ȃ��B");

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

	case ABL_CARD_SUKIMA:
	{

		if (only_info) return format("���̃J�[�h�𔭓�����Ɨאڂ����ǂȂǂ̌������֏u�Ԉړ�����B�}�b�v�[�ɓ��B����Ɣ��Α��֏o��B�ʏ�̃e���|�[�g�ňړ��ł��Ȃ��ꏊ�ɂ͏o���Ȃ��B");

		if (!wall_through_telepo(0)) return NULL;

	}
	break;

	case ABL_CARD_KODUCHI:
	{

		if (only_info) return format("���̃J�[�h�𔭓�����ƃA�C�e�����ЂƂI������1/3�̉��l�́��ɕς��邱�Ƃ��ł���B");

		alchemy();
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

	//�J�[�h���l�n�̃N���X�̂�
	if (!CHECK_ABLCARD_DEALER_CLASS) return;

	for (i = 0; i < 10; i++)
	{
		int tmp;

		//�_�~�[�J�[�h���� ���̃��x���������قǍ������N�̂��̂��o�₷��
		object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		tmp = randint1(p_ptr->lev);
		if(tmp >= 45 )
			apply_magic_abilitycard(o_ptr, -1,4, 0);
		else if(tmp >= 30)
			apply_magic_abilitycard(o_ptr, -1, 3, 0);
		else if (tmp >= 15)
			apply_magic_abilitycard(o_ptr, -1, 2, 0);
		else
			apply_magic_abilitycard(o_ptr, -1, 0, 0);


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
