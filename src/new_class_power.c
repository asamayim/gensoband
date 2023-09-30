#include "angband.h"

//�A�C�e���J�[�h�g�p����TRUE
static bool use_itemcard = FALSE;

/*:::�E�Ƃ��Ƃ̋Z��m�R�}���h�Ŏg���̂�U�R�}���h�Ŏg���̂𓝍�����J�R�}���h�Ŏg���悤�ɂ���B*/
/*:::class_power_type�^�̔z��Ŏ��s���Ȃǂ̏����L�q���E�Ƃ��ƂɓƎ��̊֐��Ŏ��s��������B*/

/*:::
	class_power_type: �K�����x�����ɂ��A�Ō�Ƀ��x��50���傫���_�~�[���ڂ�����
	lev,cost,fail,use_hp,is_magic, stat,
	heavy_armor_fail,lev_inc_cost,name,
	tips
*/


//v2.0.11 ���V���Z
class_power_type class_power_biten[] =
{

	{ 7,10,25,FALSE,FALSE,A_INT,0,0,"�E�C���m",
	"���͂̐��_���������X�^�[�����m����B" },

	{ 16,20,35,FALSE,FALSE,A_DEX,0,0,"�_����",
	"�_�n�̕���𓊝�����B�ʏ�̓������򋗗��������Ȃ�₷���B���̓��Z�œ������_�̓u�[�����������ɂȂ荂�m���Ŗ߂��Ă���B�u�������V�̃N�H�[�^�[�X�^�b�t�v�𓊂���ƕK���߂��Ă���B" },

	{ 24,30,45,FALSE,TRUE,A_CHR,0,0,"�T������",
	"���n�̃����X�^�[���̂�z���Ƃ��ď�������B" },

	{ 30,0,50,FALSE,FALSE,A_CHR,0,0,"�ڑ�",
	"�����X�^�[��̂����݂̃t���A����Ǖ�����B�u�������V�̃N�H�[�^�[�X�^�b�t�v���������Ă��Ȃ��Ǝg���Ȃ��B�F�D�I�ȃ����X�^�[�ɂ͌����₷�����j�[�N�����X�^�[�ɂ͌����Â炢�B�N�G�X�g�œ|�Ώۂ̃����X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 37,74,60,TRUE,FALSE,A_STR,0,20,"�͈͍U��",
	"�אڂ����O���b�h�S�ĂɍU������B�_�n����𑕔����Ă��Ȃ��Ǝg���Ȃ��B" },

	{ 44,96,90,FALSE,TRUE,A_DEX,0,0,"�����L�[�}�W�b�N",
		"���E���̑S�Ăɑ΂��_���[�W��^����B�З͉͂E��ɑ��������_����𓊂����Ƃ��̈З͂ɂ���Č��܂�B���푮����X���C�͍l������Ȃ��B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};


cptr do_cmd_class_power_aux_biten(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:// �E�C���m
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("�͈�:%d", rad);
		detect_monsters_mind(rad);
		break;
	}
	case 1: //�_����
	{
		if (only_info) return format("");

		//shuriken��-3�����A�K���u�[�������ɂȂ����ɖ_�����������Ȃ��悤�ɂ���
		if (!do_cmd_throw_aux(1, FALSE, -3)) return NULL;

		//cmd_throw_aux()�œ����n���x�ɂ��ቺ��������s���͂���Z����s���͂̒l�ɔ��f..���悤�Ǝv�����������w��+�O�����h��������������̂ł�߂�
		//new_class_power_change_energy_need = energy_use;
	}
	break;



	case 2://������
	{
		int i;
		bool flag = FALSE;
		if (only_info) return "";

		for (i = 0; i<(2 + p_ptr->lev / 20); i++)
		{
			if ((summon_specific(-1, py, px, p_ptr->lev, SUMMON_MONKEYS, (PM_ALLOW_GROUP | PM_FORCE_PET))))flag = TRUE;
		}
		if (flag)
			msg_print("���B���Ăяo�����I");
		else
			msg_print("��������Ȃ�����...");
		break;

	}

	//�ڑ���㩂̔\���I�Ȕ���
	case 3:
	{

		int y, x;
		monster_type *m_ptr;

		int power = plev + chr_adj * 5;

		if (only_info) return format("����:%d", power);

		if (!check_equip_specific_fixed_art(ART_BITEN, FALSE))
		{
			msg_print("㩂̎d�|����ꂽ�_���Ȃ��I");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return FALSE;
		if (dir == 5) return FALSE;
		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[120];

			if (is_friendly(m_ptr)) power *= 2;

			monster_desc(m_name, m_ptr, 0);

			msg_format("���Ȃ���%s���x���Ĉڑ���㩂ɂ����悤�Ƃ���...",m_name);

			if (check_transportation_trap(m_ptr, power))
			{
				msg_format("%s�͂��̃t���A����������B", m_name);

				delete_monster_idx(cave[y][x].m_idx);

				break;
			}

			msg_print("���s�I");

			if (is_friendly(m_ptr))
			{
				msg_format("%s�͓{�����I",m_name);
				set_hostile(m_ptr);
			}

		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;



	case 4: //���͍U��
	{
		if (only_info) return format("");

		if (inventory[INVEN_RARM].tval != TV_STICK)
		{
			msg_print("�_�𑕔����Ă��Ȃ��Ƃ��̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		msg_print("���Ȃ��͖_���Ԃ�񂵂��I");
		whirlwind_attack(0);
	}
	break;

	case 5: //�����L�[�}�W�b�N
	{

		int dam=0;
		object_type *o_ptr = &inventory[INVEN_RARM];

		if (o_ptr->tval != TV_STICK)
		{
			dam = 0;
		}
		else
		{
			//�������_���[�W�T�Z�@�����������{�[�i�X�Ƃ��d�ʃ{�[�i�X�Ƃ��ׂ͍����v�Z�����G�ɎQ�����Ă���
			int mult = 3;
			u32b flgs[TR_FLAG_SIZE];

			if (p_ptr->mighty_throw) mult++;
			object_flags(o_ptr, flgs);
			if (have_flag(flgs, TR_THROW)) mult ++;

			dam = o_ptr->dd * (o_ptr->ds + 1) / 2 + (o_ptr->to_d > 0 ? o_ptr->to_d : -1 * (o_ptr->to_d))+p_ptr->to_d_m;

			dam *= mult;

		}

		if (only_info) return format("����:%d", dam);

		if (o_ptr->tval != TV_STICK)
		{
			msg_print("�_�𑕔����Ă��Ȃ��Ƃ��̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		project_hack2(GF_ARROW, 0, 0, dam);
		msg_print("���Ȃ��̓������_�͎��͂̑S�Ă̓G��ガ�|���Ė߂��Ă����I");

		break;
	}

	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}




//v2.09 ����
class_power_type class_power_miyoi[] =
{

	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"�Ŕ��̂���",
	"�אڂ��������X�^�[��̂�F�D�I�ɂ��A����Ɏ������܂��ēD���x���㏸������B�D���x���ő�HP�𒴂��������X�^�[�͐����ׂ��ē|���������ɂȂ�B���͂������قǐ������₷���B�l�ԂɌ����₷���A�������E�����E�őϐ��E����E�͋��������X�^�[�ɂ͌����Â炢�B������������ɗאڍU�����悤�Ƃ���Ƃ��̓��Z����������B" },

	{ 10,20,30,FALSE,TRUE,A_CHR,0,0,"�L�������T",
	"�אڂ��������X�^�[���t���A�����������B�����Ă��郂���X�^�[�ɂ͔��Ɍ����₷���B���j�[�N�����X�^�[�ƒʏ�̐��_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 15,30,40,FALSE,FALSE,A_DEX,0,0,"����",
	"�ꕔ�̃����X�^�[���ޗ��ɗ��������A�����̎�ނɉ������ꎞ���ʂ𓾂�B�ޗ��ƂȂ郂���X�^�[�����O�ɓ|���Ă����K�v������B" },

	{ 20,20,50,FALSE,FALSE,A_INT,0,0,"���Ӓ���",
	"���͂̃����X�^�[�ƃg���b�v�����m����B���x��30�ȏ�ŃA�C�e���A���x��40�ȏ�Œn�`�����m����B" },

	{ 25,30,45,FALSE,FALSE,A_CHR,0,0,"��i�̎ϕ�",
	"�אڂ��������X�^�[�S�Ăɂ��������߂�B" },

	{ 30,50,60,FALSE,TRUE,A_CHR,0,0,"�L�������U",
	"��莞�Ԏ��E�O�̃����X�^�[����F������Ȃ��Ȃ�B�������␸�_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 35,80,70,FALSE,FALSE,A_DEX,0,0,"�X�������킹����",
	"���E���̑S�Ẵ����X�^�[�ɋ����I�Ɏ���ۂ܂���B" },

	{ 40,0,0,FALSE,FALSE,A_CHR,0,0,"�����߂̈�{",
	"�אڂ��������X�^�[��̂����m���ŗF�D�I�ɂ��A����ɓD���x��啝�ɏ㏸������B������{����A���̉��i�Ƌ����ɂ���Č��͂��ς��B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_miyoi(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0: //�Ŕ��̂���
	{

		int y, x;
		monster_type *m_ptr;

		int power = 10 + plev + chr_adj * 3;
		int alcohol = plev * 3 + chr_adj * 5;

		if (only_info) return format("����:%d ��:%d", power, alcohol);

		if (!get_rep_dir2(&dir)) return FALSE;
		if (dir == 5) return FALSE;
		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{

			msg_print("���Ȃ��͈��z�悭���������߂��B");
			miyoi_serve_alcohol(m_ptr, alcohol, power);

		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;



	case 1://�L�������T
	{

		int rad = 1;
		int power = p_ptr->lev * 3 + chr_adj * 5;
		if (only_info) return format("�͈�:%d ����:%d", rad, power);

		msg_format("���Ȃ��͂ɂ��ɂ��΂��Ȃ��痼���U�����B");
		if (mass_genocide_2(power, rad, 3))
			msg_print("���͂̎҂͕s�v�c�����Ȋ�����Ȃ��狎���Ă�����...");

		break;
	}

	case 2:
	{

		if (only_info) return format("");

		if (!geidontei_cooking(TRUE)) return NULL;

	}
	break;


	case 3://���Ӓ���
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("�͈�:%d", rad);

		msg_print("���Ȃ��͎��͂Ɋ댯���Ȃ�����T����...");

		detect_monsters_normal(rad);
		detect_monsters_invis(rad);
		detect_traps(rad, TRUE);
		if (plev > 29)
		{
			detect_objects_gold(rad);
			detect_objects_normal(rad);
		}
		if (plev > 39)
		{
			map_area(rad);
		}

		break;
	}


	case 4: //��i�̎ϕ�
	{

		int y, x, i;
		monster_type *m_ptr;
		bool msg_flag = FALSE;

		int power = 10 + plev + chr_adj * 2;
		int alcohol = plev * 2 + chr_adj * 2;

		if (only_info) return format("����:%d ��:%d", power, alcohol);

		//�אڃO���b�h�̃����X�^�[�S�Ă�ڋq
		for (i = 1; i <= 9; i++)
		{
			y = py + ddy[i];
			x = px + ddx[i];

			if (!in_bounds(y, x)) continue;
			if (player_bold(y, x)) continue;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml) continue;

			if (!msg_flag) msg_print("���Ȃ��͎����̎ϕ���U�镑�����I");
			msg_flag = TRUE;

			miyoi_serve_alcohol(m_ptr, alcohol, power);

		}

		if (!msg_flag)
		{
			msg_print("�ӂ�ɂ͒N�����Ȃ��B");
			return NULL;
		}

	}
	break;

	case 5:
	{
		base = p_ptr->lev;
		if (only_info) return format("����:%d + 1d%d", base, base);
		set_tim_general(base + randint1(base), TRUE, 0, 0);
		break;
	}


	case 6: //���E���A���R�[���U��
	{

		int alcohol = plev * 5 + chr_adj * 5;

		if (only_info) return format("��:%d", alcohol);

		msg_print("���Ȃ��͑��ɑ�ʂ̎��𓊓������B�ӂ�͖җ�Ȏ��̍���ɕ�܂ꂽ�I");
		project_hack2(GF_ALCOHOL, 0, 0, alcohol);

	}
	break;

	case 7: //�����߂̈�{
	{

		int y, x;
		monster_type *m_ptr;
		object_type *o_ptr;
		cptr q, s;
		int item;
		int power = 20 + plev + chr_adj;
		int alcohol = plev * 3 + chr_adj * 5;

		if (only_info) return format("����:�s��");

		item_tester_tval = TV_ALCOHOL;

		q = "�ǂ̂�����񋟂��܂���? ";
		if (use_itemcard)
			s = "���������Ă��Ȃ��B";
		else
			s = "�����̕Z�\���������Ă��Ȃ��B";

		if (!get_item(&item, q, s, (USE_INVEN))) return NULL;

		o_ptr = &inventory[item];

		if (!get_rep_dir2(&dir)) return FALSE;
		if (dir == 5) return FALSE;
		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		//���ꏈ���@�ꕔ�̎���power��-1��ݒ肵�ꌂ�Ő����ׂ������ɂ���
		if (m_ptr->r_idx == MON_OROCHI && o_ptr->sval == SV_ALCOHOL_KUSHINADA)
		{
			msg_print("�����ւɓ����̓Ŏ���������I");
			power = -1;
		}
		else if (o_ptr->name1 == ART_HANGOKU_SAKE)
		{
			msg_print("�������Ȃ�ʕ��͋C�̂�����U�镑����...");
			power = -1;
		}
		//���̒l�i�������قǖ��͔���l�A���������قǃA���R�[���x���ɉ��Z
		else
		{
			int value = object_value_real(o_ptr);

			if (value >= 50000) power += 300;
			else if (value >= 10000) power += 100;
			else if (value >= 1000) power += 30;
			else if (!value) power -= 50;

			if (o_ptr->pval > 10000) alcohol += 5000 + (o_ptr->pval-5000) / 5;
			else if (o_ptr->pval > 3000) alcohol += 2000 + (o_ptr->pval-3000) / 4;
			else alcohol += o_ptr->pval / 3;
		}

		if (cave[y][x].m_idx && (m_ptr->ml))
		{

			if(power > 0) msg_print("���Ȃ��͉�����鑠�̈�{�����o�����I");

			//�������܂����Ƃ��A�C�e����������
			if (miyoi_serve_alcohol(m_ptr, alcohol, power))
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}

		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}


	}
	break;



	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}









//v2.07 �疒
//magic_num2[card_idx]�ɃA�r���e�B�J�[�h�ۗL�t���O�A
//magic_num1[card_idx]�ɔ����^�C�v�J�[�h�̃^�C���A�E�g�l�A
//magic_num2[255]�Ɍ��݂̃J�[�h���ʃ����N���L�^
class_power_type class_power_chimata[] =
{
	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"���ʃ����N�̊m�F",
	"���������A�r���e�B�J�[�h�̐��ƃA�r���e�B�J�[�h�̐l�C���m�F�ł���B" },
	{ 1,0,0,FALSE,FALSE,A_INT,0,0,"�o���b�g�}�[�P�b�g",
	"���������A�r���e�B�J�[�h�̈ꗗ��\�����A�I�������J�[�h�̐�����ǂށB�s����������Ȃ��B��ނ��Ƃ̃J�[�h�ۗL�����͗��ʃ����N�ɔ�����������B" },
	{ 1,0,0,FALSE,TRUE,A_INT,0,0,"�o���b�g�h�~�j�I��",
	"���������A�r���e�B�J�[�h��I�����Ĕ�������B�����������͒m�\�Ŕ��肷��B���������J�[�h�͍Ďg�p�܂łɈ�莞�Ԃ̃`���[�W���K�v�B" },

	{ 15,20,50,FALSE,FALSE,A_INT,0,0,"�ӎ�",
	"�A�C�e�����ЂƂӒ肷��B���x��35�ȍ~�̓A�C�e���̊��S�ȏ��𓾂�B" },

	{ 25,40,50,FALSE,TRUE,A_CHR,0,0,"���l��",
	"�������̃{�[������B�A�r���e�B�J�[�h�̗��ʃ����N���オ��ƈЗ͂��オ��B" },

	{ 30,30,55,FALSE,FALSE,A_INT,0,0,"�e�����N�Ƃ̖ώ�",
	"���݂̃t���A�ɃA�r���e�B�J�[�h�����郂���X�^�[�����݂��邩�ǂ����m�F���A���̑�܂��ȋ�����m�邱�Ƃ��ł���" },

	{ 35,50,65,FALSE,TRUE,A_WIS,0,0,"����ւ̋���",
	"�����X�^�[��̂����m���ōU���E�h��E���@�͒ቺ��Ԃɂ���B����ɂ��̃����X�^�[������̕��ނ̗אڍU�����g�p�ł��Ȃ��Ȃ�B���������̓��Z���󂯂������X�^�[�͓|���Ă��A�C�e���𗎂Ƃ��Ȃ��Ȃ�B" },

	{ 45,160,85,FALSE,TRUE,A_CHR,0,0,"�e���̃A�W�[��",
	"���E���̂��ׂĂ�������ōU������B�A�r���e�B�J�[�h�̗��ʃ����N���オ��ƈЗ͂��オ��B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_chimata(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	int card_rank = (CHIMATA_CARD_RANK);

	switch (num)
	{
	//�����N�m�F
	case 0:
	{
		if (only_info) return format("");

		chimata_comment_card_rank();

		return NULL;
		break;

	}
	//�J�[�h�m�F
	case 1:
	{
		if (only_info) return format("");
		chimata_activate_ability_card_power(TRUE);
		return NULL; //���邾���Ȃ̂ōs��������Ȃ�

	}
	//�J�[�h����
	case 2:
	{
		if (only_info) return format("");
		if(!chimata_activate_ability_card_power(FALSE)) return NULL;

		break;
	}

	//�ӎ�
	case 3:
	{
		if (only_info) return format("");

		if (plev < 35)
		{
			if (!ident_spell(FALSE)) return NULL;
		}
		else
		{
			if (!identify_fully(FALSE)) return NULL;
		}

		break;
	}

	//���l��
	case 4:
	{
		int rad;

		//EXTRA�̃A�C�e���J�[�h�ł͊K�w*3.5�Ƃ���(EXTRA�ł̓J�[�h�����x��7�{�ɑ���)
		//�ŏ�p_ptr->ability_card_price_rate*7�ɂ��悤�Ƃ������Ȃ񂩏������I���Ńg���u���̌��ɂȂ肻���Ȃ̂ŃV���v���ɂ���
		if (p_ptr->pclass != CLASS_CHIMATA && use_itemcard)
		{
			damage = dun_level * 7 / 2;
			if (damage > 500) damage = 500;
			if (damage < 1) damage = 1;
			rad = 1 + dun_level / 40;
		}
		else
		{
			damage = (1 + card_rank) * (10 + plev / 2 + chr_adj / 2);
			rad = 1 + card_rank / 2;
		}

		if (only_info) return format("���a:%d ����:%d", rad, damage);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print("���F�̌����������ʂ�`�����B");
		fire_ball(GF_RAINBOW, dir, damage, rad);
		break;
	}

	//�J�[�h�ɂł��郂���X�^�[�T�m
	case 5:
	{
		if (only_info) return format("");
		msg_print("���Ȃ��̓t���A�̕��͋C��T��n�߂�...");
		search_specific_monster(1);
		break;
	}

	//����ւ̋���
	case 6:
	{
		int power = plev * 4 + chr_adj * 4;
		monster_type *m_ptr;
		char m_name[120];

		if (only_info) return format("����:%d", power);

		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !projectable(target_row, target_col, py, px) || !cave[target_row][target_col].m_idx)
		{
			msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
			return NULL;
		}

		m_ptr = &m_list[cave[target_row][target_col].m_idx];

		monster_desc(m_name, m_ptr, 0);

		if(one_in_(4))
			msg_format("���Ȃ���%s�Ɍ������ā��̂悤�Ȋ�ȃ|�[�Y���Ƃ����B", m_name);
		else
			msg_format("���Ȃ���%s�������̂̏��L���𖳂ɕԂ����I", m_name);

		project(0, 0, m_ptr->fy, m_ptr->fx, power, GF_DEC_ALL, PROJECT_KILL, -1);

		//�ꕔ�̗אڍU�����ł��Ȃ��Ȃ�h���b�v�������Ȃ�t���O
		m_ptr->mflag |= (MFLAG_SPECIAL);


	}
	break;

	case 7:
	{

		damage = (1 + card_rank) * (10 + plev / 2 + chr_adj / 2) * 2;

		if (only_info) return format("����:%d", damage);

		msg_print("���F�̌��̗�������Ԃ���ߏグ���I");
		project_hack2(GF_RAINBOW, 0, 0, damage);
		break;
	}



	default:
		if (only_info) return format("������");

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}




//v2.0.6 �ޖ�
//p_ptr->magic_num1[0-130?]�ɐ��n�t���O���Amagic_num2[0-130?]�ɕ���h��t���O���Amagic_num2[200]�ɖ����x�X�g�b�N���L�^
class_power_type class_power_yuma[] =
{


	{10,5,20,FALSE,FALSE,A_INT,0,0,"���Ӓ���",
	"���͂̃����X�^�[�ƃg���b�v�����m����B����Ƀ��x��20�ŃA�C�e���A���x��30�Œn�`�����m����B" },

	{ 15,20,30,TRUE,FALSE,A_DEX,50,0,"�l�ߊ��",
	"�^�[�Q�b�g�Ɍ������Ĉ�u�Ő��O���b�h���ړ�����B���x��30�ȍ~�̓^�[�Q�b�g�ɓ��������Ƃ����̂܂܍U������B�����i���d���Ƃ����s���₷���Ȃ�B" },

	{ 20,16,30,FALSE,TRUE,A_STR,0,0,"���E�݂̍����ς��鍕����",
	"�Ζ������̃r�[��������A�n�`��Ζ��ɕς��ă����X�^�[���U���͒ቺ��Ԃɂ���B�u���b�W�̃X�v�[���v���K�v�B" },

	{25,30,50,FALSE,TRUE,A_STR,0,0,"���̐��ɑ��݂��Ă͂Ȃ�Ȃ��\�H",
	"���͂̃A�C�e���ƒn�`��H�ׂĉh�{�ɂ���B�H�ׂ�����̑ϐ���\�͂𔼓��̊Ԋl������B�܂����͂̃����X�^�[�������񂹂ėאڂ�����_���[�W��^���h�{�ɂ���B���x��30�ȍ~�͂��̋Z�œ|���������X�^�[�̓��Z�𔼓��Ԏg�p�ł���B" },

	{30,0,0,FALSE,TRUE,A_INT,0,0,"���ꂽ���t�̒ǉ�",
	"���Z�u���̐��ɑ��݂��Ă͂Ȃ�Ȃ��\�H�v�œ|���������X�^�[�̓��Z���g���B���Z���Ƃɒǉ���MP�Ɛ������肪�K�v�B" },

	{ 33,30,60,FALSE,TRUE,A_CON,0,0,"�S�[�W���C�U�[",
	"���ӂ�*�j��*����B���a�͌��݂̖����x�ɂ���ĕω�����B�����x���ʂɏ����B" },

	{36,45,65,FALSE,TRUE,A_STR,0,0,"�����Ŗ��n�ȏ������u",
	"�����̈ʒu�𒆐S�ɋ���ȃ{�[���𕡐��񔭐�������B�{�[���̑����͐Ζ��E���C�E�Ή��E�������烉���_���B" },

	{40,60,75,FALSE,TRUE,A_CON,0,0,"�K�C�A�̌��t",
	"�̗͂�S�񕜂��A�؂菝�Ɣ\�͒ቺ���������A����ɘr�͊�p�ϋv��Z���ԏ㏸������B�����x���ʂɏ����B" },

	{ 45,120,80,FALSE,TRUE,A_CHR,0,0,"���~�ȏb�_�g�E�e�c�̗[�M",
	"���E�����ׂĂ𐅑����Ɖ��������ōU�����A���͂�Ζ��n�`�ɂ���B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_yuma(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int digestion = 200 + plev * 6 + (3+num) * 200;

	switch (num)
	{
	case 0://���Ӓ���
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("�͈�:%d", rad);

		msg_print("���Ȃ��͎��͂̏���𚐂����B");

		detect_monsters_normal(rad);
		detect_monsters_invis(rad);
			detect_traps(rad, TRUE);
		if (plev > 19)
		{
			detect_objects_gold(rad);
			detect_objects_normal(rad);
		}
		if (plev > 29)
		{
			map_area(rad);
		}
		set_food(p_ptr->food - digestion);


		break;
	}

	case 1:
	{
		int len = 3 + plev / 8;
		if (only_info) return format("�˒�:%d", len);

		if (plev < 30)
		{
			if (rush_attack2(len, 0, 0, 0)) return NULL;
		}
		else
		{
			if (!rush_attack(NULL, len, 0)) return NULL;

		}
		//�����x����
		set_food(p_ptr->food - digestion);

	}
	break;

	case 2:
	{
		int base = 50 + plev * 3 + chr_adj * 2;

		if (only_info) return format("����:%d", base);

		if (!check_equip_specific_fixed_art(ART_TOUTETSU, FALSE))
		{
			msg_print("�n�ʂ��@�铹��Ȃ��B");
			return NULL;
		}

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("�X�v�[����n�ʂɓ˂����Ă��I");
		fire_beam(GF_DIG_OIL, dir, base);
		break;
	}

	case 3:
	{
		int rad = 3 + plev / 7;

		if (only_info) return format("�͈�:%d",rad);

		//�����x����
		set_food(p_ptr->food - digestion);
		yuma_swallow(rad);

	}
	break;

	case 4:
	{
		if (only_info) return "";

		if (!cast_monspell_new()) return NULL;

		//�����x����
		set_food(p_ptr->food - digestion);

	}
	break;


	case 5:
	{
		int food = CHECK_YUMA_FOOD_STOCK;
		int rad = 3 * food;
		if (only_info) return format("���a:%d", rad);

		digestion = 13000;

		if (rad<1)
		{
			msg_print("�h�{������Ȃ��B");
			return NULL;
		}

		msg_print("�̓��̃G�l���M�[���J�������I");
		destroy_area(py, px, rad, FALSE, FALSE, FALSE);

		set_food(p_ptr->food - digestion);


	}
	break;



	case 6:
	{
		int rad = 3 + plev / 24;
		damage = 200 + plev * 3 + chr_adj * 5;
		int count = plev / 10 - 1;

		if (only_info) return format("����:�`%d * %d", damage / 2, count);

		for (; count > 0; count--)
		{
			switch (randint1(4))
			{
			case 1:
				msg_format("�n�ʂ���Ζ��������o�����I");
				project(0, rad, py, px, damage, GF_DIG_OIL, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				break;
			case 2:
				msg_format("�n�ʂ�����C�������o�����I");
				project(0, rad, py, px, damage, GF_STEAM, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				break;
			case 3:
				msg_format("�n�ʂ��牘�������������o�����I");
				project(0, rad, py, px, damage, GF_POLLUTE, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				break;
			default:
				msg_format("�n�ʂ��΂𕬂����I");
				project(0, rad, py, px, damage*3/2, GF_FIRE, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				break;
			}
		}
		//�����x����
		set_food(p_ptr->food - digestion);


	}
	break;

	case 7:
	{
		base = 10;
		if (only_info) return format("����:%d",base);

		digestion = 13000;

		if(use_itemcard && (p_ptr->food - PY_FOOD_WEAK) < digestion || !use_itemcard && !CHECK_YUMA_FOOD_STOCK)
		{
			msg_print("�h�{������Ȃ��B");
			return NULL;
		}

		msg_print("��n�̗͂����̐g�Ɏ�荞�񂾁I");

		set_stun(0);
		set_cut(0);
		do_res_stat(A_STR);
		do_res_stat(A_INT);
		do_res_stat(A_WIS);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);
		do_res_stat(A_CHR);
		set_tim_addstat(A_STR, 105, base, FALSE);
		set_tim_addstat(A_DEX, 105, base, FALSE);
		set_tim_addstat(A_CON, 105, base, FALSE);
		hp_player(5000);

		set_food(p_ptr->food - digestion);


	}
	break;

	case 8:
	{
		damage = 200 + plev * 3 + chr_adj * 5;

		if (only_info) return format("����:%d * 2", damage);

		msg_print("��n���瑾�Ẩ��������o����...");

		project_hack2(GF_WATER,0,0, damage);
		project_hack2(GF_POLLUTE, 0,0,damage);
		project_hack2(GF_DIG_OIL, 0,0,damage);

		//�����x����
		set_food(p_ptr->food - digestion);


	}
	break;



	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}









//v2.0.4 ���{�ۓ��Z
class_power_type class_power_misumaru[] =
{

	{ 1,25,30,FALSE,TRUE,A_DEX,0,0,"���ʐ���",
	"�u���ʁv�𐧍삷��B���삵�����ʂ͑������Ĕ������邱�Ƃ��ł���B��������Ƃ��̌��ʂ͑f�ނɂ���Ă�����x�ω�����B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"���ʑ���",
	"���삵�����ʂ��p�X���b�g�ɑ�������B���x�����オ��Ƒ����\�X���b�g����������B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"���ʊm�F",
	"�����������ʂ̔������̌��ʂ��m�F����B" },
	{ 1,0,0,FALSE,TRUE,A_INT,0,0,"���ʔ���",
	"�����������ʂ𔭓�����B�����̂��тɓ��Z���Ƃɐݒ肳�ꂽ�ǉ���MP�������B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"���ʎ��O��",
	"��p�X���b�g�ɑ����������ʂ���������B" },

	{ 20,20,30,FALSE,TRUE,A_INT,0,0,"�����X�^�[����",
	"���͂̃����X�^�[�̔\�͂�ϐ��Ȃǂ�m��B" },

	{ 25,32,50,FALSE,TRUE,A_CHR,0,0,"�����A�z��",
	"�������̃{�[������B" },

	{ 30,1,60,FALSE,TRUE,A_INT,0,0,"���͐H��(���)",
	"��΂��疂�͂��z�����MP���񕜂���B�z�����閂�̗͂ʂ͕�΂̎�ނŕς��B" },

	{ 40,96,85,FALSE,TRUE,A_CHR,0,0,"�A�z�T�t�H�P�C�V����",
	"���E���̑S�Ăɑ΂��Ė������̍U�����s���B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_misumaru(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{

		if (only_info) return format("");

		if (!make_magatama()) return NULL;
	}
	break;


	//���ʑ��� inven2�g�p
	case 1:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;
	}

	//���ʊm�F inven2�ɓ��������ʂ�����Z���X�g�𐶐����A�I�����ꂽ�Z�̃p�����[�^��\������
	case 2:
	{
		if (only_info) return format("");

		activate_magatama(TRUE);
		return NULL; //���邾���Ȃ̂ōs��������Ȃ� ���ς�俎q�������ɂ���
	}
	break;
	//���ʔ���
	case 3:
	{
		if (only_info) return format("");

		activate_magatama(FALSE);

	}
	break;

	//���ʎ��O��
	case 4:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}

	//����
	case 5:
	{
		if (only_info) return format("");
		msg_print("���ʂ��g���Ď��͂̃����X�^�[�̏����W�߂�...");
		probing();
		break;
	}



	case 6:
	{
		int rad = 2 + plev / 40;
		int dice = 7;
		int sides = plev / 2;
		int base = plev + chr_adj * 2;

		if (only_info) return format("���a:%d ����:%d+%dd%d", rad, base, dice, sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print("���F�ɋP���A�z�ʂ�������I");
		fire_ball(GF_RAINBOW, dir, base + damroll(dice, sides), rad);
		break;
	}


	case 7:
	{

		if (only_info) return format("");

		if (!eat_jewel()) return NULL;
	}
	break;

	case 8:
	{
		int dam = plev * 4 + chr_adj * 6;
		if (only_info) return format("����:%d", dam);
		stop_raiko_music();
		msg_print("��ʂ̉A�z�ʂ������𖄂ߐs�������I");
		project_hack2(GF_DISP_ALL, 0, 0, dam);
		break;
	}


	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



/*v2.0.3 �эj�ۗ���p�Z*/
class_power_type class_power_megumu[] =
{
	{ 15,45,35,FALSE,TRUE,A_CHR,0,0,"���������̕�",
	"���E���̑S�Ăɑ΂��ĉΉ������̍U�����s���A���ӂ𖾂邭����B" },

	{ 20,30,40,FALSE,FALSE,A_CHR,0,0,"�z������",
	"�z���̓V��𐔑̌Ăяo���B" },//��V��͏o���Ȃ��悤�ɂ���

	{ 25,80,60,FALSE,TRUE,A_CHR,0,0,"���ʗ����̕�",
	"���E���̑S�Ăɑ΂��đM�������̍U�����s���A����Ƀ����X�^�[�����������悤�Ǝ��݂�B" },

	{ 30,0,80,FALSE,FALSE,A_WIS,0,0,"�����ǌ�",
	"HP��MP���킸���ɉ񕜂�����B�܂��o�b�h�X�e�[�^�X����̉񕜂𑁂߂�B" },

	{ 35,120,70,FALSE,TRUE,A_DEX,0,0,"�V�n�s��̕�",
	"���E���̑S�Ă̓G�ΓI�ȃ����X�^�[�ɑ΂��Ēʏ�U�����s���B���̍U���̓I�[���ɂ�锽�����󂯂Ȃ����U���񐔂������ɂȂ�B" },

	{ 40,200,75,FALSE,TRUE,A_WIS,0,0,"�V�^ࣖ��̐�",
	"�t���A�S�Ẵ����X�^�[��������ɂ��悤�Ǝ��݂�B���j�[�N�����X�^�[�E�͋��������X�^�[�E�_�i�ɂ͌��ʂ������B" },

	{ 45,160,85,FALSE,TRUE,A_CHR,0,0,"��X��X�̕�",
	"���E���̑S�Ăɑ΂��Ė������̍U�����s���B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },

};

cptr do_cmd_class_power_aux_megumu(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		base = plev + chr_adj * 5;
		if (only_info) return format("����:%d", base);
		msg_print("�ӂ��ʂ��R���オ�����I");
		project_hack2(GF_FIRE, 0, 0, base);
		(void)lite_room(py, px);
		break;
	}

	case 1:
	{
		int level;
		int num = 1 + plev / 10;
		level = plev + chr_adj;
		bool flag = FALSE;
		if (only_info) return format("�������x��:%d", level);
		for (; num>0; num--)
		{
			if (summon_specific(-1, py, px, level, SUMMON_TENGU_MINION, (PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;
		}
		if (flag) msg_format("�V��B���Ăяo�����I");
		else msg_format("��������Ȃ�����...");

	}
	break;


	case 2:
	{
		base = plev * 2 + chr_adj * 5;
		if (only_info) return format("����:%d", base);
		msg_print("ῂ������_���W�������ʂ����I");
		project_hack2(GF_LITE, 0, 0, base);
		confuse_monsters(base);
		break;
	}


	case 3:
	{
		if (only_info) return format("");

		msg_print("���Ȃ����ґz���n�߂�...");

		hp_player(randint1(10 + p_ptr->lev / 5));
		player_gain_mana(randint1(5 + p_ptr->lev / 10));
		set_poisoned(p_ptr->poisoned - 10);
		set_stun(p_ptr->stun - 10);
		set_cut(p_ptr->cut - 10);
		set_image(p_ptr->image - 10);

	}
	break;

	case 4:
	{
		if (only_info) return format("");
		msg_format("���Ȃ��͕��̂悤�ɋ삯�������I");
		project_hack2(GF_SOULSCULPTURE, 0, 0, 100);
	}
	break;

	case 5:
	{
		int base = plev * 7;
		if (only_info) return format("����:%d", base);

		msg_print("�_���W���������̌��Ŗ��������...");
		floor_attack(GF_STASIS, 0,0, base, 0);
	}
	break;


	case 6:
	{
		int dam = plev * 7 + chr_adj * 10;
		if (only_info) return format("����:%d", dam);
		stop_raiko_music();
		msg_print("���ݓn���������_���W�����𐁂��������I");
		project_hack2(GF_DISP_ALL, 0, 0, dam);
		break;
	}


	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;


	}
	return "";
}


//v2.0.2 �T
class_power_type class_power_tsukasa[] =
{

	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"��",
	"�z����̂Ɋ񐶂���B�񐶒��͂��̃����X�^�[�̈ʒu�ɏd�Ȃ�ꏏ�Ɉړ�����B���̃����X�^�[�̍U���Ńv���C���[���o���l��A�C�e���𓾂邱�Ƃ��ł���B�z���R�}���h�̋R�悩������̓��Z�Ɠ������Ƃ��ł���B" },

	{ 10,0,20,FALSE,FALSE,A_CHR,0,0,"�����Ś���",
	"�אڂ��郂���X�^�[��̂���������߂Ĕz���ɂ��悤�Ǝ��݂�B�����X�^�[������Ă���Ɛ������₷���B���j�[�N�����X�^�[�ƌ��������X�^�[�ɂ͌����Â炭�A���ꃆ�j�[�N�����X�^�[�A�N�G�X�g�œ|�Ώۃ����X�^�[�A���_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 20,30,40,FALSE,TRUE,A_INT,0,0,"�x�����̊ǌϒe",
	"�j�Б����̔��a0�̃��P�b�g�ōU������B�����X�^�[�ɓ�����Ƃ��̃����X�^�[���s�����邽�тɃ_���[�W��^����B" },

	{ 25,10,40,FALSE,TRUE,A_DEX,0,0,"�ÎE",
	"�m���Ń����X�^�[���ꌂ�œ|�������s�����1�_���[�W�ɂȂ�U�����s���B�z���Ɋ񐶒��͎g���Ȃ��B����������Ă��Ȃ��Ƃ����Ȃ��B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 30,40,50,FALSE,FALSE,A_CHR,0,0,"�t�H�b�N�X���C���_�[",
	"�񐶒��̔z��������������B" },

	{ 34,30,55,FALSE,FALSE,A_INT,0,0,"���i�T��",
	"���݂̃t���A�Ɂu���i�v�ɕ��ނ����A�C�e�������邩�ǂ����Ƃ��̑�܂��ȋ����𒲂ׂ�B" },

	{ 37,55,65,FALSE,TRUE,A_INT,0,0,"�ǂ̒��̎׈�",
	"�Í������̃r�[����אڂ����ǂ����������_���ȕ����ɕ�������B" },

	{ 40,1,70,FALSE,TRUE,A_CHR,0,0,"�V�����_�[�t�H�b�N�X",
	"�񐶒��̔z���ɍő�HP��1/5�̃_���[�W��^���A����1/3��MP���z������B�񐶖��̃����X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 45,80,80,FALSE,TRUE,A_CHR,0,0,"�V�ϗ����̕�",
	"���E���̑S�Ă�M�������ōU�����A����Ɏ��͂̃����X�^�[�������E�����E���@�͒ቺ�����悤�Ǝ��݂�B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_tsukasa(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{

		if (only_info)	return "";

		if (!do_riding(FALSE)) return NULL;

	}
	break;
	case 1://�X�J�E�g
	{
		int y, x;
		int power = 20 + plev + chr_adj;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format("����:%d", power);

		if (p_ptr->inside_arena)
		{
			msg_print("�����̓��Z�͎g���Ȃ��B");
			return NULL;
		}


		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

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

			if (m_ptr->r_idx == MON_MEGUMU)
			{
				msg_format("%s�͂��Ȃ��̎�l���B", m_name);
				return NULL;
			}

			msg_format("���Ȃ���%s�Ɍ����Ě���������...", m_name);

			//SMART�Ɍ����Â炭����Ă�G�Ɍ����₷���悤��
			if (r_ptr->flags2 & RF2_SMART) power /= 3;
			else if (r_ptr->flags2 & RF2_STUPID) power *= 2;

			if (m_ptr->hp < m_ptr->max_maxhp / 2) power *= 2;
			if (m_ptr->hp < m_ptr->max_maxhp / 8) power *= 2;

			if (MON_SLOW(m_ptr))    power = power * 3 / 2;
			if (MON_STUNNED(m_ptr)) power = power * 3 / 2;

			if (MON_CONFUSED(m_ptr)) power = power * 3;
			if (MON_MONFEAR(m_ptr)) power = power * 3;

			if (MON_DEC_ATK(m_ptr)) power = power * 3 / 2;
			if (MON_DEC_DEF(m_ptr)) power = power * 3 / 2;
			if (MON_DEC_MAG(m_ptr)) power = power * 3 / 2;
			if (MON_NO_MOVE(m_ptr)) power = power * 3 / 2;

			if (MON_BERSERK(m_ptr)) power = 0;

			if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				msg_format("%s�͖��������B", m_name);
			}
			else if (is_pet(m_ptr))
			{
				msg_format("%s�͂��łɂ��Ȃ��̈ӂ̂܂܂��B", m_name);
			}
			else if (r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags1 & (RF1_QUESTOR) || r_ptr->flags7 & RF7_VARIABLE || power < r_ptr->level)
			{
				msg_format("%s�͂��Ȃ��̗U���Ɏ���݂��Ȃ��I", m_name);
			}
			//���j�[�N�����X�^�[�𒇊Ԃɂ���ɂ�weird_luck�v
			else if (randint1(power) < r_ptr->level || (r_ptr->flags1 & RF1_UNIQUE) && !weird_luck())
			{
				msg_format("%s�͂��Ȃ��̗U���ɏ��Ȃ������B", m_name);
			}
			else
			{
				msg_format("%s�͂��Ȃ��ɏ]�����B", m_name);
				set_pet(m_ptr);
			}
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;

	case 2:
	{
		int dam = 5 + plev / 2;

		if (only_info) return format("����:%d*10", dam);
		if (!get_aim_dir(&dir)) return NULL;

		//project_m()���ł�����󂯂������X�^�[�ɃJ�E���g���J�n���Aprocess_monster()�̃����X�^�[�s�����ɃJ�E���g����������_���[�W��^����
		fire_rocket(GF_TIMED_SHARD, dir, dam, 0);

		break;
	}

	case 3:
	{
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format("");

		if (p_ptr->riding)
		{
			msg_print("���̓��Z�͈�l�łȂ��Ǝg���Ȃ��B");
			return NULL;
		}
		else if (p_ptr->do_martialarts)
		{
			msg_format("����������Ă��Ȃ��B");
			return NULL;
		}

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

			msg_format("���Ȃ��͕�����\����%s�̌�납��E�ъ�����c", m_name);

			py_attack(y, x, HISSATSU_KYUSHO);
		}

		break;
	}





	case 4:
	{
		int v = 20;

		if (only_info) return format("����:%d", v);

		if (!p_ptr->riding)
		{
			msg_print("�񐶒��̔z�������Ȃ��B");
			return NULL;
		}
		else
		{

			char m_name[80];
			monster_type *m_ptr = &m_list[p_ptr->riding];
			monster_desc(m_name, m_ptr, 0);

			msg_format("%s�����E�ȏ�Ɋ撣�点���I",m_name);

			if (MON_FAST(m_ptr)) v -= MON_FAST(m_ptr);

			if(v>0)	set_monster_fast(p_ptr->riding, MON_FAST(m_ptr) + v);

		}


	}
	break;


	case 5:
	{
		if (only_info) return format("");
		msg_print("���Ȃ��͔z���̊ǌς����ɒ��������̑{���𖽂����B");
		search_specific_object(7);
		break;
	}


	//�ǂ̒��̎׈�
	//�X�^�[���C�g�̏�̈Í������o�[�W���� �񐔂�5d3����6�Œ��
	//����dam lev30:300 lev40:360 lev50:400
	case 6:
	{
		int beam_num = 6;
		int dice = 6 + plev / 8;
		int sides = 10;
		int k;

		int y, x;
		int attempts;

		if (only_info) return format("����:%dd%d * %d", dice, sides, beam_num);

		for (k = 0; k < beam_num; k++)
		{
			attempts = 1000;

			while (attempts--)
			{
				scatter(&y, &x, py, px, 4, 0);

				if (!cave_have_flag_bold(y, x, FF_PROJECT)) continue;

				if (!player_bold(y, x)) break;
			}

			project(0, 0, y, x, damroll(dice, sides), GF_DARK,
				(PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
		}
	}
	break;

	case 7:
	{

		monster_type 	*m_ptr;
		monster_race	*r_ptr;
		int 	dam = 0, gain_mana = 0;
		char m_name[80];

		if (p_ptr->riding)
		{
			m_ptr = &m_list[p_ptr->riding];
			r_ptr = &r_info[m_ptr->r_idx];

			dam = m_ptr->max_maxhp / 5;

			if (!monster_living(r_ptr)) dam = 0;

			gain_mana = dam / 3;

		}

		if (only_info) return format("����:%d ��:%d", dam, gain_mana);

		if(!p_ptr->riding)
		{
			msg_print("�񐶒��̔z�������Ȃ��B");
			return NULL;
		}


		monster_desc(m_name, m_ptr, 0);

		if (!dam)
		{
			msg_format("%s����͗͂��z���Ȃ������B", m_name);
		}
		else
		{
			bool dummy;
			//�O�̂��߂Ƀ��j�[�N��|���Ȃ��悤�ɂ��Ă���
			if (r_ptr->flags1 & RF1_UNIQUE && dam > m_ptr->hp)dam = m_ptr->hp;

			msg_format("%s���琶���͂�e�͂Ȃ��z���グ���I", m_name);
			mon_take_hit(p_ptr->riding, dam, &dummy, "�͓|�ꂽ");
			player_gain_mana(gain_mana);
		}

	}
	break;

	//�V�ϗ����̕�
	case 8:
	{
		int base = plev * 5 + chr_adj * 5;

		if (only_info) return format("����:%d", base);
		msg_format("���f�I�Ȍ��̗������ӂ�𖄂ߐs�������I");
		project_hack2(GF_LITE, 0, 0, base);
		confuse_monsters(base);
		charm_monsters(base);
		project_hack(GF_DEC_MAG, base);

		break;
	}
	break;



	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.93 �~�P
//�J�[�h�̔����̃��X�g��magic_num2[0-9]���A�J�[�h����̔��p�ς݃t���O��magic_num1[0-3]���g���Ă���B
//����Ɂu���̃t���A�ł��łɒe�������̓��Z���g�����v�t���O��magic_num2[100]�ɋL�^
class_power_type class_power_mike[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h���[",
	"�J�[�h�P�[�X�ɃA�r���e�B�J�[�h��8��ނ܂Ŏ��[����B�J�[�h�P�[�X�ɓ��ꂽ�J�[�h�𔭓����邱�Ƃ͂ł��Ȃ����A�����^�C�v�̃J�[�h�̓P�[�X�̒��ł����ʂ𔭊�����B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h�m�F",
	"�J�[�h�P�[�X�̒����m�F����B�s����������Ȃ��B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h��o��",
	"�J�[�h�P�[�X����J�[�h�����o���B" },

	{ 20,20,70,FALSE,FALSE,A_INT,0,0,"�ӎ�",
	"�A�C�e�����ЂƂӒ肷��B" },

	//(�����ĂԂ��q��������)
	{ 25,16,60,FALSE,TRUE,A_CHR,0,0,"�e�Џ���",
	"�ꎞ�I�Ƀh���b�v�A�C�e���̎��Ȃǂ��ǂ��Ȃ�K�^��ԂɂȂ邪�A�����Ɏ��͂̃����X�^�[�������N���ēG�΂��锽����ԂɂȂ�B" },

	{ 30,0,75,FALSE,FALSE,A_CHR,0,0,"�J�[�h�������",
	"�אڂ��������X�^�[��̂���J�[�h�𔃂����B�J�[�h�������Ă��郂���X�^�[�͂��̃J�[�h�̑�ނƂȂ������z���̏Z�l�̂݁B�܂������J�[�h�𔃂���͈̂�x�̂݁B" },

	{ 35,64,70,FALSE,FALSE,A_CHR,0,0,"�O�єL�ɏ���",
	"���������Ă���G�ΓI�ȃ����X�^�[����������B" },

	//(�q���ĂԂ�����������)
	{ 40,120,90,FALSE,FALSE,A_CHR,0,0,"�e������",
	"�A�r���e�B�J�[�h�������Ă��錶�z���̏Z�l��F�D�I�ȏ�Ԃŏ�������B���������̓��Z�ŌĂ΂ꂽ�҂���J�[�h�𔃂����Ƃ���Ɖ��i��{�ȏ�ɐ����|���Ă���B��t���A�Ɉ�x�����ĂׂȂ��B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_mike(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //���邾���Ȃ̂ōs��������Ȃ�

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}


	case 3:
	{
		if (only_info) return format("");

		if (!ident_spell(FALSE)) return NULL;

		break;
	}

	case 4:
	{
		int base = 25;
		int v;

		if (only_info) return format("����:%d+1d%d", base, base);

		v = base + randint1(base);

		set_tim_lucky(v, FALSE);
		set_tim_aggravation(v, FALSE);

	}
	break;


	case 5:
	{
		if (only_info) return format("");

		if (!buy_abilitycard_from_mon()) return NULL;

	}
	break;

	case 6: //v2.0.2�ǉ� �O�єL�ɏ��� ONLY_GOLD�t���O�̓G���Ă�
	{
		bool flag = FALSE;

		if (only_info) return format("");

		for (i = 0; i<p_ptr->lev / 10; i++)
		{
			if ((summon_specific(-1, py, px, p_ptr->lev, SUMMON_ONLY_GOLD, (PM_ALLOW_GROUP | PM_FORCE_ENEMY | PM_ALLOW_UNIQUE))))flag = TRUE;
		}
		if (flag)
			msg_print("���̓�����������I");
		else
			msg_print("�����N����Ȃ������B");

		break;


	}

	case 7:
	{


		int candi_count = 0;
		int summon_r_idx = 0;
		int i;
		bool flag_ng = FALSE;
		bool flag_summon_success = FALSE;

		if (only_info) return format("");


		if (p_ptr->inside_arena)
		{
			msg_format("�����ł͎g���Ȃ��B");
			return NULL;
		}

		//��t���A�Ɉ�x�����ĂׂȂ��悤�ɂ��Ă����B�얲�������X�g�Ɏc���ĉ��x���Ă�ŃT�[�y���g��m���ɂł����肵�Ȃ��悤�ɁB
		if (p_ptr->magic_num2[100])
		{
			msg_format("���̃t���A�ł͂��łɂ��q���Ă񂾁B");
			return NULL;

		}


		for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
		{
			monster_race *tmp_r_ptr;

			if (!ability_card_list[i].r_idx) continue;

			//���p�ς݃t���O�������Ă���J�[�h�̎�����͏o�ė��Ȃ�
			if ((p_ptr->magic_num1[(i / 16) + ABLCARD_MAGICNUM_SHIFT] >> (i % 16)) & 1L) continue;

			tmp_r_ptr = &r_info[ability_card_list[i].r_idx];

			//�œ|�ς�,�������͊��Ƀt���A�ɂ���ƃp�X
			if (tmp_r_ptr->cur_num >= tmp_r_ptr->max_num)
			{
				if (cheat_hear) msg_format("TESTMSG:idx%d�͑œ|�ς݂������͂��łɃt���A�ɂ���", ability_card_list[i].r_idx);
				continue;
			}

			//�_���W�����{�X��N�G�X�g�œ|�Ώۂ̓p�X
			if (tmp_r_ptr->flags1 & RF1_QUESTOR) continue;

			//���̊K�w���[�������X�^�[�̓p�X ���C�}�����ȑO�o���t���A���󂢃t���A�ŏo�Ă��Ȃ��Ȃ邩������Ȃ����܂��債���e���͂Ȃ����낤
			if (tmp_r_ptr->level > dun_level) continue;

			candi_count++;

			if (!one_in_(candi_count)) continue;

			summon_r_idx = ability_card_list[i].r_idx;

		}

		//����t���O�𗧂Ăď���
		if (summon_r_idx)
		{

			if (summon_named_creature(0, py, px, summon_r_idx, (PM_FORCE_FRIENDLY|PM_ALLOW_SP_UNIQUE|PM_SET_MFLAG_SP))) flag_summon_success = TRUE;

		}

		if (flag_summon_success)
		{
			msg_print("�ǂ�����Ƃ��Ȃ����q���񂪌��ꂽ�I");
			//���̃t���A�ł��łɋq���Ă񂾃t���O�@�t���A����o�鎞0�ɂ���
			p_ptr->magic_num2[100] = 1;
		}
		else
			msg_print("���q����͗��Ȃ�����...");


	}
	break;


	default:
		if (only_info) return format("������");

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}




//v1.1.92 �������ꐫ�i��p�Z*/
//magic_num1[0],[1]�ɂ΂�T�����Ζ����L�^���Ă���
//magic_num1[4]�Ƀc�P�����Ŕ��������z���L�^����
//magic_num1[6]�Ɏ������L����܂ł̃_���[�W�J�E���g
class_power_type class_power_jyoon_2[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�w�֑���",
	"��p�X���b�g�Ɏw�ւ�8�܂ő����ł���B���������������w�ւ̃p�����[�^�͔���(�[���؂�̂�)�Ƃ��Ĉ�����B���ꂽ�w�ւ͑����ł��Ȃ��B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�w�։���",
	"��p�X���b�g�ɑ������Ă���w�ւ��O���B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�w�֔���",
	"��p�X���b�g�ɑ������Ă���w�ւ𔭓�����B" },

	{ 10,5,50,FALSE,FALSE,A_INT,0,0,"�ڗ���",
	"�A�C�e����̉��l��m�邱�Ƃ��ł���B�Ӓ�┻�ʂ͂ł��Ȃ��B" },
	{ 15,10,45,FALSE,FALSE,A_INT,0,0,"���݊��m",
	"�t���A�̃A�C�e���ƍ�������m����B���x��30�ȍ~�̓A�C�e�������������Ă��郂���X�^�[�����m����B" },

	{ 20,20,50,FALSE,FALSE,A_DEX,50,0,"�����",
	"��s���Ő��O���b�h���ړ�����B�h�A��ʉ߂���Ƃ��ړ��\�������Z���Ȃ�B�����i���d���Ƃ����s���₷���Ȃ�B�n�`�̂킩��Ȃ��ꏊ���^�[�Q�b�g�ɂ��邱�Ƃ͂ł��Ȃ��B" },

	{ 25,50,55,TRUE,FALSE,A_STR,50,20,"�ˌ�",
	"�^�[�Q�b�g�ɓˌ����Ή������̃_���[�W��^����B�i�����ɂ����g�p�ł����A�З͂͊i���U���͂Ō��܂�B�����i���d���Ƃ����s���₷���Ȃ�B" },

	{ 32,45,60,FALSE,TRUE,A_CON,0,0,"�t���C�}�u�����C��",
	"���͂̃����_���ȃO���b�h�ɉΉ������̃{�[���U����A�ł���B" },

	{ 38,50,70,FALSE,TRUE,A_STR,0,0,"�u�a�_�I�ȓV��@��@",
	"���E���̎w��ʒu�ɗ��������_���[�W��^���A����ɂ��̎��͂�Ζ��n�`�ɂ���B" },

	{ 43,76,85,FALSE,TRUE,A_CHR,0,0,"�n�R�_�I�ȐΖ����o����",
	"���͂̃����_���ȕ����ɐ��E�����E�Ζ��n�`���������ꂩ�̑����̃r�[����A���ŕ��B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_jyoon_2(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;

		p_ptr->update |= PU_BONUS;
		update_stuff();
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		p_ptr->update |= PU_BONUS;
		update_stuff();
		break;
	}
	case 2:
	{
		if (only_info) return format("");
		if (!activate_inven2()) return NULL;
		break;
	}

	case 3:
	{
		cptr q, s;
		object_type *o_ptr;
		int item;
		int value;
		if (only_info) return "";

		q = "�ǂ̃A�C�e�����m�F���܂����H";
		s = "�A�C�e�����Ȃ��B";
		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return NULL;
		if (item >= 0)
		{
			o_ptr = &inventory[item];
		}
		else
		{
			o_ptr = &o_list[0 - item];
		}
		value = object_value_real(o_ptr);

		if (!value)
		{
			msg_format("���̃A�C�e���͖����l���B");
			if (object_is_cursed(o_ptr))
				msg_format("���̏����Ă���B");
		}
		else
		{
			msg_format("���̃A�C�e���ɂ�%s$%d�̉��l������悤���B", ((o_ptr->number>1) ? "�������" : ""), value);
			if (object_is_cursed(o_ptr))
				msg_format("����������Ă���B");

		}


		break;
	}


	case 4:
	{
		int range = 25 + plev / 2;
		if (only_info) return format("�͈�:%d", range);

		detect_objects_gold(range);
		detect_objects_normal(range);

		if (plev > 29)
			detect_monsters_rich(range);

		break;
	}


	case 5://�U����������
	{
		int x, y;
		int cost;
		int dist = plev / 3;
		if (only_info) return format("�ړ��R�X�g:%d", dist - 1);
		if (!tgt_pt(&x, &y)) return NULL;

		if (!player_can_enter(cave[y][x].feat, 0) || !(cave[y][x].info & CAVE_KNOWN))
		{
			msg_print("�����ɂ͍s���Ȃ��B");
			return NULL;
		}
		forget_travel_flow();
		travel_flow(y, x);
		if (dist < travel.cost[py][px])
		{
			if (travel.cost[py][px] >= 9999)
				msg_print("�����ɂ͓����ʂ��Ă��Ȃ��B");
			else
				msg_print("�����͉�������B");
			return NULL;
		}

		msg_print("���Ȃ��͎o�ɒ͂܂��Ĕ�񂾁B");
		teleport_player_to(y, x, TELEPORT_NONMAGICAL);

		//�����ړ������鎞�ړ��Ɠ����悤�ɏ���s���͂���������
		if (p_ptr->speedster)
			new_class_power_change_energy_need = (75 - p_ptr->lev / 2);

		break;
	}
	break;

	case 6://�ːi�@�Ή������ɕύX
	{
		int len = p_ptr->lev / 5;
		if (len < 5) len = 5;

		if (p_ptr->do_martialarts)
		{
			damage = calc_martialarts_dam_x100(0) * p_ptr->num_blow[0] / 100;
		}
		else
		{
			damage = 0;
		}

		if (only_info) return format("�˒�:%d ����:���悻%d", len, damage);

		if (!damage)
		{
			msg_print("�����̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		if (!rush_attack2(len, GF_FIRE, damage,0)) return NULL;
		break;
	}


	case 7://�X���C�u���o�[���t���C�}�u�����C��		
	{
		int rad = 2 + plev / 24;
		int damage = plev * 2 + chr_adj * 2;
		if (only_info) return format("����:%d*(10+1d10)", damage);
		msg_format("�R��������T���U�炵���I");
		cast_meteor(damage, rad, GF_FIRE);

		//�ꉞ�����T���̂Ŗ��΂�܂��J�E���g��ǉ����Ă����B�����Ȃ̂Ŏ��ۂɔ������鐔�Ƃ͈�v���Ȃ����܂��[���ȉe���͂Ȃ��B
		jyoon_record_money_waste(10+randint1(10));

		break;
	}

	case 8: //�v���b�N�s�W�������Ζ��@��@�@�L�X���u���ň�̒��v�𗬗p
	{
		int range = 5 + plev / 10;
		int base;
		int tx, ty, dam;

		base = plev * 5;

		if (use_itemcard && base < 100) base = 100;

		if (only_info) return format("����:%d + 1d%d", base, base);

		project_length = range;
		if (!get_aim_dir(&dir)) return NULL;
		dam = base + randint1(base);

		ty = target_row;
		tx = target_col;
		//����project()���Ⴂ����̂��H��������������
		if (!fire_ball_jump(GF_DIG_OIL, dir, 3, 3, "���Ȃ��͓V������сA�n�ʂɐ[�X�ƌ���ł����񂾁I")) return NULL;
		project(0, 4, ty, tx, dam, GF_WATER, (PROJECT_HIDE | PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID), -1);
		teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

	}

	break;

	case 9://�A�u�\�����[�g���[�U�[���n�R�_�I�ȐΖ����o����
	{
		int num = damroll(5, 3);
		int y, x,k;
		int attempts;

		int dice = 10 + plev / 5;
		int sides = 10 + chr_adj / 5;

		//�X�^�[���C�g�C�Ɠ����v�̂Ő��򔲂���750dam,�Ő��򔲂���1130dam���炢�̊��Ғl
		if (only_info) return format("����:%dd%d * �s��", dice, sides);

		msg_print("�n�ʂ��琅�Ɩ��Ɖ��������������o�����I");

		for (k = 0; k < num; k++)
		{
			attempts = 100;
			int typ;

			if (one_in_(3)) typ = GF_POLLUTE;
			else if (one_in_(2)) typ = GF_WATER;
			else typ = GF_DIG_OIL;

			while (attempts--)
			{
				scatter(&y, &x, py, px, 4, 0);

				if (!cave_have_flag_bold(y, x, FF_PROJECT)) continue;

				if (!player_bold(y, x)) break;
			}

			project(0, 0, y, x, damroll(dice, sides), typ,
				(PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
		}
		break;
	}



	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.90 �R�@
//�J�[�h�̔����̃��X�g��magic_num2[32-41]���A�J�[�h����̔��p�ς݃t���O��magic_num1[32-35]���g���Ă���.
//��V���l�n���Z�Ɣ��Ȃ��悤�g���C���f�b�N�X��+32���Ă���B

class_power_type class_power_sannyo[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h���[",
	"�J�[�h�P�[�X�ɃA�r���e�B�J�[�h��8��ނ܂Ŏ��[����B�J�[�h�P�[�X�ɓ��ꂽ�J�[�h�𔭓����邱�Ƃ͂ł��Ȃ����A�����^�C�v�̃J�[�h�̓P�[�X�̒��ł����ʂ𔭊�����B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h�m�F",
	"�J�[�h�P�[�X�̒����m�F����B�s����������Ȃ��B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h��o��",
	"�J�[�h�P�[�X����J�[�h�����o���B" },

	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"����������",
	"�������z���Ă���Ƃ�������~�߂�B�s���͂�����Ȃ��B" },

	{ 5,0,0,FALSE,TRUE,A_CHR,0,0,"�ꕞ",
	"�������z���n�߂�B�z���Ă���Ԃ�MP�̉񕜂����܂�B" },

	{ 10,6,25,FALSE,TRUE,A_CHR,0,0,"���V�̉_�ԑ�",
	"�������z���n�߂�B�z���Ă���Ԃ͖��^�[���߂��̃����X�^�[�����|������B�����z���قǌ��ʂ��㏸����(�ő�400)" },

	{ 18,12,40,FALSE,TRUE,A_CHR,0,0,"�����̉_�ԑ�",
	"�������z���n�߂�B�z���Ă���Ԃ͖��^�[���߂��̃����X�^�[������������B�����z���قǌ��ʂ��㏸����(�ő�400)" },

	{ 24,20,50,FALSE,TRUE,A_CHR,0,0,"�d���P�����ᑐ",
	"�������z���n�߂�B�z���Ă���Ԃ͖��^�[���߂��̃����X�^�[��N�O�Ƃ�����B�����z���قǌ��ʂ��㏸����(�ő�400)" },

	{ 30,0,75,FALSE,FALSE,A_CHR,0,0,"�J�[�h�������",
	"�אڂ��������X�^�[��̂���J�[�h�𔃂����B�J�[�h�������Ă��郂���X�^�[�͂��̃J�[�h�̑�ނƂȂ������z���̏Z�l�̂݁B�܂������J�[�h�𔃂���͈̂�x�̂݁B" },

	{ 35,30,60,FALSE,TRUE,A_CHR,0,0,"�d��ം߂����ᑐ",
	"�������z���n�߂�B�z���Ă���Ԃ͖��^�[���߂��̃����X�^�[������������B�����z���قǌ��ʂ��㏸����(�ő�400)" },

	{ 40,48,75,FALSE,TRUE,A_CHR,0,0,"�E�C�̋",
	"�������z���n�߂�B�z���Ă���Ԃ͖��^�[���߂��̃����X�^�[�ɋ��͂Ȑ��_�U�����s���B�����z���قǌ��ʂ��㏸����(�ő�400)" },

	{ 45,80,85,FALSE,TRUE,A_CHR,0,0,"�E�C�̎R�̏���",
	"�������z���n�߂�B�z���Ă���Ԃ͋���m��ԂɂȂ�A����ɐg�̔\�͂��啝�ɏ㏸����B�z���n�߂̂Ƃ��Ɍ���HP���啝�ɉ񕜂���B" },




	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_sannyo(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;

	int power_add = 5 + plev / 2;

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //���邾���Ȃ̂ōs��������Ȃ�

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}

	//�������~�߂�@�s��������Ȃ�
	case 3:
	{
		if (only_info) return "";
		stop_singing();
		return NULL;

	}
	break;
	case 4://�ꕞ

	{
		if (only_info) return format("�񕜋���");
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_SMOKE, SPELL_CAST);

		break;
	}
	break;

	case 5:
	{
		int power = 20 + plev * 2;
		if (only_info) return format("����:%d+%d/turn",power,power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_FEAR, SPELL_CAST);

		break;
	}
	break;

	case 6:
	{
		int power = 30 + plev * 2;
		if (only_info) return format("����:%d+%d/turn", power, power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_CONF, SPELL_CAST);

		break;
	}
	break;
	case 7:
	{
		int power = 40 + plev * 2;
		if (only_info) return format("����:%d+%d/turn", power, power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_STUN, SPELL_CAST);

		break;
	}
	break;




	case 8:
	{
		if (only_info) return format("");

		if (!buy_abilitycard_from_mon()) return NULL;

	}
	break;


	case 9:
	{
		int power = 40 + plev * 2;
		if (only_info) return format("����:%d+%d/turn", power, power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_SLOW, SPELL_CAST);

		break;
	}
	break;
	case 10:
	{
		int power = 50 + plev * 2;
		if (only_info) return format("����:%d+%d/turn", power, power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_MINDBLAST, SPELL_CAST);

		break;
	}
	break;
	case 11:
	{
		if (only_info) return format("");
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_BERSERK, SPELL_CAST);

		break;
	}
	break;


	default:
		if (only_info) return format("������");

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}





//v1.1.89 �S�X��
//�d���B�̃V�[���h���\�b�h��tim_general[0],�X�l�[�N�C�[�^�[��[1],�h���S���C�[�^�[��[2],���ł̃O������[3]���g�p

class_power_type class_power_momoyo[] =
{


	{ 10,20,30,FALSE,TRUE,A_CHR,0,0,"�P�C�u�X�E�H�[�}�[",
	"��ʂ̒���F�D�I�ȏ�Ԃŏ�������B" },

	{ 15,24,50,FALSE,TRUE,A_DEX,0,0,"�X�l�[�N�C�[�^�[",
	"�ꎞ�I�Ƀw�r�n�����X�^�[�ɑ΂�������\�͂��l�����A����Ƀ����X�^�[�̊��݂��E���ߕt���U�����������悤�ɂȂ�B���n���̓��Z�ƕ��p�ł��Ȃ��B" },

	{ 20,40,45,TRUE,FALSE,A_CON,0,20,"�}�C���u���X�g",
	"�����̎��͂ɑ΂����������U�����s���B�З͂�HP�ɔ�Ⴗ��B���x��40�ȏ�Ŏ��E���S�Ă��ΏۂɂȂ�B" },

	{ 25,40,60,FALSE,TRUE,A_STR,0,0,"�h���S���C�[�^�[",
	"�ꎞ�I�Ƀh���S���n�����X�^�[�ɑ΂�������\�͂��l�����A����Ƀ����X�^�[����̃I�[���_���[�W��MP�Ƃ��ċz������悤�ɂȂ�B���n���̓��Z�ƕ��p�ł��Ȃ��B" },

	{ 30,64,70,FALSE,TRUE,A_DEX,0,0,"�d���B�̃V�[���h���\�b�h",
	"�ꎞ�I�ɕǂ��@��Ȃ���ړ����A����Ɏ������󂯂�_���[�W��2/3�Ɍy������悤�ɂȂ�B" },

	{ 35,80,75,FALSE,TRUE,A_CON,0,0,"���ł̃O����",
	"�ꎞ�I�Ƀ����X�^�[��|�����Ƃ�HP�Ɩ����x��啝�ɋz�����A����Ɍo���l�𑽂�������悤�ɂȂ�B���n���̓��Z�ƕ��p�ł��Ȃ��B" },

	{ 40,120,85,FALSE,TRUE,A_STR,0,0,"�ς��葱����}�C���_���v",
	"���͂̒n�`����΂ɂ���B" },

	{ 45,-100,80,FALSE,TRUE,A_CHR,0,0,"峕P���܂̋P���������������Ȃ�����",
	"MP��S��(�Œ�100)����A���E�����ׂĂɋ��͂Ȗ��͑����U�����s���B" },



	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_momoyo(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{


	case 0:
	{
		bool flag = FALSE;
		int count = 2 + plev / 6;
		if (only_info) return "";
		

		for (i = 0; i<count; i++)
		{
			int s_mode;
			s_mode = one_in_(4) ? SUMMON_ANT : SUMMON_SPIDER;

			if ((summon_specific(-1, py, px, p_ptr->lev, s_mode, (PM_ALLOW_GROUP | PM_FORCE_FRIENDLY))))flag = TRUE;
		}
		if (flag)
			msg_print("��ʂ�峒B���Ăяo�����I");
		else
			msg_print("��������Ȃ������B");

		break;
	}

	case 1:
	case 3:
	case 5:
	{
		int v;
		int base = plev / 3;
		if (only_info) return format("����:%d+1d%d", base, base);

		v = base + randint1(base);

		if (num == 1)
		{
			set_tim_general(0, TRUE, 2, 0);
			set_tim_general(0, TRUE, 3, 0);
			set_tim_general(v, FALSE, 1, 0);
		}
		else if(num == 3)
		{
			set_tim_general(0, TRUE, 1, 0);
			set_tim_general(0, TRUE, 3, 0);
			set_tim_general(v, FALSE, 2, 0);
		}
		else
		{
			set_tim_general(0, TRUE, 1, 0);
			set_tim_general(0, TRUE, 2, 0);
			set_tim_general(v, FALSE, 3, 0);

		}

		break;
	}


	case 2:
	{
		int damage;

		if (p_ptr->lev < 40 )
		{
			damage = p_ptr->chp;

			if (damage > 1600) damage = 1600;

			if (only_info) return format("����:�`%d", damage/2);

			msg_format("���Ȃ��̑̂���őf�������o�����I");
			project(0, 6, py, px, damage, GF_POLLUTE, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);

		}
		else
		{
			damage = p_ptr->chp / 3;
			if (only_info) return format("����:%d", damage);

			msg_print("���J�f�̓ł���n��N�����I");
			project_hack2(GF_POLLUTE, 0, 0, damage);
		}

	}
	break;


	case 4:
	{
		int v;
		int base = 20;
		if (only_info) return format("����:%d+1d%d", base, base);

		v = base + randint1(base);

		set_tim_general(v, FALSE, 0, 0);
		set_magicdef(v, FALSE);

		break;
	}


	case 6:
	{
		int rad = 2;
		if (only_info) return format("���a:%d", rad);

		msg_print("��ʂ̊�΂��ςݏオ�����I");
		project(0, rad, py, px, 10, GF_STONE_WALL, (PROJECT_GRID), -1);
		break;
	}

	case 7:
	{
		int dam = p_ptr->csp * 2;
		if (only_info) return format("����:%d", dam);

		msg_print("�c��Ȗ��͂������������I");
		project_hack2(GF_MANA, 0, 0, dam);

		break;
	}


	default:
		if (only_info) return format("������");

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.88 ������
//�J�[�h�̔����̃��X�g��magic_num2[0-9]���A�J�[�h����̔��p�ς݃t���O��magic_num1[0-3]���g���Ă���.
class_power_type class_power_takane[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h���[",
	"�J�[�h�P�[�X�ɃA�r���e�B�J�[�h��16��ނ܂Ŏ��[����B�J�[�h�P�[�X�ɓ��ꂽ�J�[�h�𔭓����邱�Ƃ͂ł��Ȃ����A�����^�C�v�̃J�[�h�̓P�[�X�̒��ł����ʂ𔭊�����B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h�m�F",
	"�J�[�h�P�[�X�̒����m�F����B�s����������Ȃ��B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h��o��",
	"�J�[�h�P�[�X����J�[�h�����o���B" },

	{ 15,20,50,FALSE,FALSE,A_INT,0,0,"�ӎ�",
	"�A�C�e�����ЂƂӒ肷��B" },

	{ 20,0,50,FALSE,FALSE,A_CHR,0,0,"�J�[�h�������",
	"�אڂ��������X�^�[��̂���J�[�h�𔃂����B�J�[�h�������Ă��郂���X�^�[�͂��̃J�[�h�̑�ނƂȂ������z���̏Z�l�̂݁B�܂������J�[�h�𔃂���͈̂�x�̂݁B" },

	{ 25,20,55,FALSE,FALSE,A_DEX,0,0,"�؉B��̋Z�p",
	"�ꎞ�I�ɉB���\�͂������オ��B�X�̒��ɂ���Ƒ啝�ɏオ��B" },

	{ 30,30,65,FALSE,TRUE,A_INT,0,0,"�ŉ��̐X��",
	"�����̂���O���b�h�̒n�`��X�ɂ���B" },

	{ 35,30,60,FALSE,TRUE,A_CHR,0,0,"�O���[���X�p�C����",
	"�אڂ��������X�^�[�S�Ăɒx�ݑ����̍U�����s���B�������X�̒��ɂ��Ȃ��Ǝg���Ȃ��B" },

	{ 40,75,75,FALSE,FALSE,A_INT,0,0,"�^�E�ŉ��̐X��",
	"�����𒆐S�Ƃ����L�͈͂̒n�`��X�ɂ���B" },

	{ 45,90,85,FALSE,FALSE,A_DEX,0,0,"�^�E�؉B��̋Z�p",
	"�ꎞ�I�Ɏ��͂̓G����F������Â炢���B����ԂɂȂ�B�������X�̒��ɂ��Ȃ��ƌ��ʂ��Ȃ��B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_takane(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //���邾���Ȃ̂ōs��������Ȃ�

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}


	case 3:
	{
		if (only_info) return format("");

		if (!ident_spell(FALSE)) return NULL;

		break;
	}

	case 4:
	{
		if (only_info) return format("");

		if (!buy_abilitycard_from_mon()) return NULL;

	}
	break;



	case 5://�؉B��̋Z�p tim_general[0]���g��
	{
		int base = plev;
		if (only_info) return format("����:%d+1d%d", base,base);

		set_tim_general(base+randint1(base), FALSE, 0, 0);

	}
	break;



	case 6:
	case 8:
	{
		int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
		int rad=0;

		if (num == 8) rad = 2;
		if (only_info) return format("���a:%d", rad);

		//GF_MAKE_TREE�͂Ȃ����m��񂪁��̂���ꏊ�ɖ؂𐶂₳�Ȃ��̂ł����Ő��₷
		if (cave_clean_bold(py, px))
		{
			cave_set_feat(py, px, feat_tree);
			msg_print("�ӂ�͐��X�����X�̋�C�ɕ�܂ꂽ�B");
		}

		if(num == 8) project(0, rad, py, px, 0, GF_MAKE_TREE, flg, -1);

		//�X�n�`�ɓ��������Ƃɂ��p�����[�^�Čv�Z�p
		p_ptr->update |= PU_BONUS;

	}
	break;


	//�O���[���X�p�C����
	case 7:
	{
		int base = p_ptr->lev * 2 + chr_adj * 10 / 3;

		if (only_info) return format("����:%d", base / 2);

		//�Z�g�p�����֐��ɂ��������Ə������͂��Ȃ񂾂��A�����Ȃ����Ƃ�����H�E�B�U�[�h���[�h�ł��Ȃ��̂ɂ悭�킩���
		if (!cave_have_flag_bold((py), (px), FF_TREE))
		{
			msg_print("���̓��Z�͐X�̒��łȂ��Ǝg���Ȃ��B");
			return NULL;
		}

		msg_format("�؂̗t�����̂悤�ɉQ�������I");
		project(0, 1, py, px, base, GF_INACT, PROJECT_KILL, -1);

	}
	break;


	case 9:
	{
		int base = 25;

		if (only_info) return format("����:%d+1d%d", base, base);

		set_tim_superstealth(base + randint1(base), FALSE, SUPERSTEALTH_TYPE_FOREST);

	}
	break;


	default:
		if (only_info) return format("������");

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.87 �J�[�h���l
//�J�[�h�̔����̃��X�g��magic_num2[0-9]���A�J�[�h����̔��p�ς݃t���O��magic_num1[0-3]���g���Ă���.
//�܂��u���{��`�̃W�����}�v�ɂ�鉿�i�������W����magic_num2[10-19]�ɋL�^

class_power_type class_power_card_dealer[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h���[",
	"�J�[�h�P�[�X�ɃA�r���e�B�J�[�h��8��ނ܂Ŏ��[����B�J�[�h�P�[�X�ɓ��ꂽ�J�[�h�𔭓����邱�Ƃ͂ł��Ȃ����A�����^�C�v�̃J�[�h�̓P�[�X�̒��ł����ʂ𔭊�����B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h�m�F",
	"�J�[�h�P�[�X�̒����m�F����B�s����������Ȃ��B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�J�[�h��o��",
	"�J�[�h�P�[�X����J�[�h�����o���B" },

	{ 20,20,70,FALSE,FALSE,A_INT,0,0,"�ӎ�",
	"�A�C�e�����ЂƂӒ肷��B" },

	{ 30,0,75,FALSE,FALSE,A_CHR,0,0,"�J�[�h�������",
	"�אڂ��������X�^�[��̂���J�[�h�𔃂����B�J�[�h�������Ă��郂���X�^�[�͂��̃J�[�h�̑�ނƂȂ������z���̏Z�l�̂݁B�܂������J�[�h�𔃂���͈̂�x�̂݁B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_card_dealer(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //���邾���Ȃ̂ōs��������Ȃ�

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}


	case 3:
	{
		if (only_info) return format("");

		if (!ident_spell(FALSE)) return NULL;

		break;
	}

	case 4:
	{
		if (only_info) return format("");

		if (!buy_abilitycard_from_mon()) return NULL;

	}
	break;

	default:
		if (only_info) return format("������");

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}






/*:::v1.1.82 �e�������Ɛ�p�Z*/
//magic_num1[]��1-96��magic_num2[]��1-32����Z�K���x�ɁA
//magic_num1[0]���W���̃��[�h�Ɏg�p����B

class_power_type class_power_researcher[] =
{

	{ 1,0,0,FALSE,TRUE,A_INT,0,0,"���Z",
	"�����X�^�[����K���������Z���g�p����B���Z���K������ɂ̓����X�^�[�����Z���g���Ƃ�������x���߂��Ō���΂悢�B�����������X�^�[���������^�[�Q�b�g�����Ɏg�������Z�ł͏K�n�x�����ȏ�ɏオ��Ȃ��B" },

	{ 20,20,30,FALSE,FALSE,A_INT,0,0,"�����X�^�[����",
	"���͂̃����X�^�[�̔\�͂�ϐ��Ȃǂ�m��B" },

	{ 30,30,60,TRUE,FALSE,A_CON,0,10,"�C��������",
	"���̃^�[���Ɏg�p���ꂽHP�ˑ��̓��Z�̈З͂�2�{�ɂȂ�B" },

	{ 40,30,80,FALSE,TRUE,A_WIS,0,0,"���͏W��",
	"���̃^�[���Ɏg�p���ꂽ�ꕔ�̓��Z���͋����Ȃ�_���[�W���㏸����B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_researcher(int num, bool only_info)
{

	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
	{
		if (only_info) return "";

		if (!cast_monspell_new()) return NULL;
		break;
	}

	case 1:
	{
		if (only_info) return format("");
		msg_print("���Ȃ��͎��͂̃����X�^�[�̒������n�߂�...");
		probing();
		break;
	}
	case 2:
	{

		if (only_info) return format("");

		do_cmd_concentrate(CONCENT_KANA);//���[�h2���u���X�����A���[�h3��POWERFUL�t���O�t�^�ɂ���
		break;
	}
	case 3:
	{

		if (only_info) return format("");

		do_cmd_concentrate(CONCENT_SUWA);
		break;
	}


	default:
		{
			if (only_info) return "������";

			msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
			return NULL;
		}

	}

return "";

}



/*:::v1.1.77 ����(��p���i)��p�Z*/
//�ǐՃ����X�^�[��m_idx���i�[���邽�߂�p_ptr->magic_num1[0]���g�p����
class_power_type class_power_orin2[] =
{
	{ 5,10,20,FALSE,FALSE,A_INT,0,1,"��������",
	"���͂̃����X�^�[�����m����B���x��15�Ńg���b�v�A���x��25�ŃA�C�e���A���x��35�Œn�`�����m����B���ʔ͈͂̓��x���A�b�v�ɔ����L����B" },

	{ 10,0,0,FALSE,TRUE,A_CON,0,0,"�L��",
	"�L�̎p�ɂȂ�B�ϐg���͈ړ����x�ƉB���\�͂�����I�ɏ㏸����B�������퓬�\�͂��ቺ���A�قƂ�ǂ̑����i���ꎞ����������A�����E������E���@�E���Z���g���Ȃ��Ȃ�BU�R�}���h����l�^�ɖ߂邱�Ƃ��ł���B" },

	{ 20,20,40,FALSE,FALSE,A_DEX,0,1,"�ǐ�",
	"�אڂ��������X�^�[��̂��w�肵�A���̃����X�^�[����Ɋ��m��������B���x��30�ȏ�̂Ƃ����̃����X�^�[����̍U�����󂯂钼�O�Ɏ����I�ɒZ�����e���|�[�g�����݂�B�e���|�[�g�ɂ�MP30������A�����X�^�[�̃��x���������ƃe���|�[�g�Ɏ��s���₷���B" },




	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};



cptr do_cmd_class_power_aux_orin2(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
		{
			int rad = 15;
			if (plev > 24) rad += 10;
			if (plev > 34) rad += 15;
			if (only_info) return format("�͈�:%d", rad);

			if(plev < 25)
				msg_print("���Ȃ��͎��͂ɕY���H��ɕ������݂��s�����B");
			else if(plev < 35)
				msg_print("���Ȃ��͔z���̗삽�����@�ɏo�����B");
			else
				msg_print("�����̗삽�������Ȃ��̌��ɏW������͂����I");

			detect_monsters_normal(rad);
			detect_monsters_invis(rad);
			if (plev > 14) detect_traps(rad, TRUE);
			if (plev > 24) detect_objects_normal(rad);
			if (plev > 34) map_area(rad);
		}
		break;


	case 1:
	{
		if (only_info) return format("����:�Œ�1000�^�[��");
		if (!set_mimic(1000, MIMIC_CAT, FALSE)) return NULL;

		break;
	}

	//�ǐ�
	case 2:
	{
		int i;
		monster_type *m_ptr;
		int y, x;

		if (only_info) return  format("");

		//�אڃ����X�^�[�w��
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			int damage;
			int o_idx;
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			monster_desc(m_name, m_ptr, 0);

			if (cave[y][x].m_idx == p_ptr->magic_num1[0])
			{
				msg_format("���łɒǐՒ����B");
				return NULL;
			}
			else
			{
				msg_format("���Ȃ���%s�̒ǐՂ��J�n�����B", m_name);
				p_ptr->magic_num1[0] = cave[y][x].m_idx;
			}

		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}


	}
	break;



	default:
	{
		if (only_info) return "������";

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}

	}
	return "";
}




/*:::v1.1.74 �ݕP���Z*/
class_power_type class_power_keiki[] =
{

	{ 5,0,0,FALSE,TRUE,A_DEX,0,0,"���`�p",
	"�����i���ЂƂ���A�t�^���ꂽ�\�͂��G�b�Z���X�Ƃ��Ē��o����B�܂����o�����G�b�Z���X��ʂ̑����i�ɂЂƂt�^����B" },
	{ 10,20,40,FALSE,FALSE,A_INT,0,0,"�ӎ�",
	"�A�C�e�����ЂƂӒ肷��B���x��30�ȍ~�̓A�C�e���̊��S�ȏ��𓾂�B" },
	{ 15,30,45,FALSE,TRUE,A_CHR,0,0,"����", 
	"�������̎��ꂽ�����i�����􂷂�B���x��35�ŋ��͂Ȏ􂢂����􂷂�悤�ɂȂ�B" },
	{ 20,0,60,FALSE,FALSE,A_DEX,0,0,"�����l�n���`�p",
	"���͂ɏ��ւ̔z������������B�h��G�b�Z���X��500�����B" },

	{ 24,0,65,FALSE,FALSE,A_DEX,0,0,"�^�~���`�p",
	"�����𒆐S�Ƃ���8�O���b�h�ɗאڍU�����s���B�I�[���ɂ�锽�����󂯂Ȃ��B�U���G�b�Z���X��500�����B" },

	{ 28,20,70,FALSE,FALSE,A_INT,0,0,"���`�_�̖�",
	"���ӂ̒n�`�ƃA�C�e���E�����X�^�[�E�g���b�v�����m���A����Ɏ��E���̃����X�^�[�𒲍�����B" },

	{ 32,0,70,FALSE,FALSE,A_DEX,0,0,"���`���`�p",
	"�����𒆐S�Ƃ���8�O���b�h�Ɂu��̕ǁv�𐶐�����B�h��G�b�Z���X��1000�����B" },

	{ 36,0,75,FALSE,FALSE,A_DEX,0,0,"���^���`�p",
	"������̃����X�^�[�Ƀ_���[�W��^����B�З͂͗אڍU���͂Ɠ����B�U���G�b�Z���X��1000�����B�n�̂�������𑕔����Ă��Ȃ��Ǝg���Ȃ��B" },

	{ 40,0,80,FALSE,TRUE,A_DEX,0,0,"�S�`���`�p",
	"�ϐ��G�b�Z���X�������_����50����A���̃t���A����̓��ʂȔz���𐶐�����B�z���̔\�͂͏�����G�b�Z���X�ɂ�茈�܂�B" },

	{ 45,0,80,FALSE,TRUE,A_DEX,0,0,"�C�h���f�B�A�{���X",
	"��莞�ԑS�\�͂��������A����Ɏ푰���ꎞ�I�Ɂu�����v�ɂȂ�B�\�̓G�b�Z���X6��ނ�50�������B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_keiki(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		if (only_info) return "";
		screen_save();
		clear_bldg(2, 22);
		do_cmd_kaji(FALSE);

		screen_load();

	}
	break;
	case 1:
	{
		if (only_info) return format("");

		if (plev < 30)
		{
			if (!ident_spell(FALSE)) return NULL;
		}
		else
		{
			if (!identify_fully(FALSE)) return NULL;
		}
		break;
	}
	case 2:
	{
		bool success = FALSE;
		if (only_info) return format("");

		if (plev < 35)
		{
			if (remove_curse()) success = TRUE;
		}
		else
		{
			if (remove_all_curse())  success = TRUE;
		}
		if (success) msg_print("�����i�̎����������������B");
		else msg_print("�����N����Ȃ������B");

		break;
	}

	case 3:
	{
		bool flag = FALSE;
		int i;
		int count = 2 + plev / 8;
		int mode;

		if (only_info) return "";

		if (p_ptr->magic_num1[TR_ES_AC] < 500)
		{
			msg_print("���̓��Z���g�������������Ă��Ȃ��B");
			return NULL;
		}
		p_ptr->magic_num1[TR_ES_AC] -= 500;

		for (i = 0; i<count; i++)
		{
			int r_idx;

			switch (randint1(3))
			{
			case 1:
				r_idx = plev > 39 ? MON_HANIWA_F2 : MON_HANIWA_F1;
				break;
			case 2:
				r_idx = plev > 39 ? MON_HANIWA_A2 : MON_HANIWA_A1;
				break;
			default:
				r_idx = plev > 39 ? MON_HANIWA_C2 : MON_HANIWA_C1;
				break;
			}
			
			if (summon_named_creature(0, py, px, r_idx, PM_FORCE_PET)) flag = TRUE;

		}
		if (flag)
			msg_print("���ւ�����n��o�����B");
		else
			msg_print("���ւ�����ꏊ���Ȃ��B");

		break;
	}

	case 4:
	{
		int rad = 1;
		if (only_info) return format("");

		if (p_ptr->magic_num1[TR_ES_ATTACK] < 500)
		{
			msg_print("���̓��Z���g�������������Ă��Ȃ��B");
			return NULL;
		}
		p_ptr->magic_num1[TR_ES_ATTACK] -= 500;

		flag_friendly_attack = TRUE;

		if(p_ptr->do_martialarts)
			msg_format("���Ȃ��̘r�͔������ʂ�`�����B");
		else
			msg_format("���Ȃ��̕��킪�������ʂ�`�����B");


		project(0, rad, py, px, 100, GF_SOULSCULPTURE, PROJECT_KILL, -1);
		flag_friendly_attack = FALSE;
	}
	break;

	case 5:
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return "";

		msg_format("���Ȃ��͎��͂����񂵂��B");
		map_area(rad);
		detect_traps(rad, TRUE);
		detect_objects_normal(rad);
		detect_monsters_normal(rad);
		detect_monsters_invis(rad);
		probing();
		break;
	}

	case 6:
	{
		if (only_info) return format("");

		if (!use_itemcard)
		{
			if (p_ptr->magic_num1[TR_ES_AC] < 1000)
			{
				msg_print("���̓��Z���g�������������Ă��Ȃ��B");
				return NULL;
			}
			p_ptr->magic_num1[TR_ES_AC] -= 1000;
		}

		msg_format("���Ȃ��͏u���Ԃɕǂ�z�����B");
		wall_stone();
	}
	break;


	case 7:
	{
		int damage = 0;
		int range = 5 + plev / 10;

		if (object_has_a_blade(&inventory[INVEN_RARM])) damage += calc_weapon_dam(0);
		if (object_has_a_blade(&inventory[INVEN_LARM])) damage += calc_weapon_dam(1);

		if (only_info) return format("�˒�:%d ����:%d", range, damage);

		if (!damage)
		{
			msg_print("�n�̂�������𑕔����Ă��Ȃ��B");
			return NULL;
		}
		if (p_ptr->magic_num1[TR_ES_ATTACK] < 1000)
		{
			msg_print("���̓��Z���g�������������Ă��Ȃ��B");
			return NULL;
		}

		project_length = range;
		if (!get_aim_dir(&dir)) return NULL;

		p_ptr->magic_num1[TR_ES_ATTACK] -= 1000;


		msg_print("��n���꒼���ɐ؂�􂢂��I");
		fire_beam(GF_MISSILE, dir, damage);
		break;
	}

	case 8: //�����_�����j�[�N�z������
			//�t���A���܂����ŘA�������悤�ɂ������������A���ꂾ�ƃt���A�ړ��̂Ƃ��Ƀ����_�����j�[�N���������ꂽ��㏑������ď�����炵���B�Ή���������Ƃ�₱�������Ȃ̂Ńp�X
	{
		int r_idx;
		int i;
		bool flag1 = TRUE;
		bool flag2 = TRUE;
		bool flag3 = TRUE;
		monster_type *m_ptr;
		monster_race *r_ptr;
		u32b mode = PM_EPHEMERA;
		int cand_num = 0;

		if (only_info) return "";

		//�g���Ă��Ȃ������_�����j�[�NIDX��T��
		for (i = 1; i < m_max; i++)
		{
			if (m_list[i].r_idx == MON_RANDOM_UNIQUE_1) flag1 = FALSE;
			if (m_list[i].r_idx == MON_RANDOM_UNIQUE_2) flag2 = FALSE;
			if (m_list[i].r_idx == MON_RANDOM_UNIQUE_3) flag3 = FALSE;

		}
		if (flag1)
			r_idx = MON_RANDOM_UNIQUE_1;
		else if (flag2)
			r_idx = MON_RANDOM_UNIQUE_2;
		else if (flag3)
			r_idx = MON_RANDOM_UNIQUE_3;
		else
			r_idx = 0;

		//�����G�b�Z���X�����肵�A�G�b�Z���X������ă����_�����j�[�N�����^�C�v����
		if (r_idx)
		{
			int tmp_sum = 0;


			for (i = TR_RES_ACID; i <= TR_RES_TIME; i++)
			{
				if (p_ptr->magic_num1[i] < 50) continue;
				tmp_sum += p_ptr->magic_num1[i];
				cand_num++;
			}
			tmp_sum = randint1(tmp_sum);
			for (i = TR_RES_ACID; i < TR_RES_TIME; i++)
			{
				if (p_ptr->magic_num1[i] < 50) continue;
				tmp_sum -= p_ptr->magic_num1[i];

				if (tmp_sum <= 0) break;
			}

			if(cand_num)special_idol_generate_type = i;

		}

		if(!cand_num)
		{
			msg_print("���̓��Z���g�������������Ă��Ȃ��B");
			return NULL;
		}

		msg_print("���Ȃ��͐V��̐���Ɏ��|������...");

		if (r_idx && special_idol_generate_type && summon_named_creature(0, py, px, r_idx, mode))
		{
			cptr adj;
			char name_total[256];

			switch (special_idol_generate_type)
			{
			case TR_RES_ACID:
				adj = "ࣂꂽ"; break;
			case TR_RES_ELEC:
				adj = "�ΉԂ��U�炷"; break;
			case TR_RES_FIRE:
				adj = "�R������"; break;
			case TR_RES_COLD:
				adj = "����t����"; break;
			case TR_RES_POIS:
				adj = "�ŁX����"; break;
			case TR_RES_LITE:
				adj = "ῂ�"; break;
			case TR_RES_DARK:
				adj = "������"; break;
			case TR_RES_SHARDS:
				adj = "������"; break;
			case TR_RES_SOUND:
				adj = "�k����"; break;
			case TR_RES_NETHER:
				adj = "�s�g��"; break;
			case TR_RES_WATER:
				adj = "������"; break;
			case TR_RES_CHAOS:
				adj = "�˂��ꂽ"; break;
			case TR_RES_DISEN:
				adj = "�����Ȃ�"; break;
			case TR_RES_HOLY:
				adj = "�_�X����"; break;
			default:
				adj = "���";
			}
			msg_format("���Ȃ���%s�����𑢂�グ���I",adj);
			//�����_�����j�[�N�̖��O�ύX
			sprintf(name_total, "%s����", adj);
			my_strcpy(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1], name_total, sizeof(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]) - 2);

			p_ptr->magic_num1[special_idol_generate_type] -= 50;
		}
		else
		{
			msg_print("�����̐���Ɏ��s�����B");
		}
		special_idol_generate_type = 0;

		break;

	}




	case 9:
	{
		int i;
		int percentage;
		int base = 20;
		int time;
		bool flag_ok = TRUE;
		if (only_info) return format("����:%d+1d%d", base, base);


		//�r�́`���͂̃G�b�Z���X�̕K�v�����`�F�b�N
		for (i = 0; i < 6; i++)
		{
			if (p_ptr->magic_num1[i] < 50)flag_ok = FALSE;
		}
		if (!flag_ok)
		{
			msg_print("���̓��Z���g�������������Ă��Ȃ��B");
			return NULL;
		}
		for (i = 0; i < 6; i++)
		{
			p_ptr->magic_num1[i] -= 50;
		}

		msg_print("���Ȃ��͐킢�̏����𐮂����B");

		percentage = p_ptr->chp * 100 / p_ptr->mhp;
		time = base + randint1(base);
		set_tim_addstat(A_STR, 107, time, FALSE);
		set_tim_addstat(A_INT, 107, time, FALSE);
		set_tim_addstat(A_WIS, 107, time, FALSE);
		set_tim_addstat(A_DEX, 107, time, FALSE);
		set_tim_addstat(A_CON, 107, time, FALSE);
		set_tim_addstat(A_CHR, 107, time, FALSE);
		set_mimic(time, MIMIC_DEMON_LORD, FALSE);

		p_ptr->chp = p_ptr->mhp * percentage / 100;
		p_ptr->redraw |= (PR_HP);
		redraw_stuff();


		break;
	}
	default:
	{
		if (only_info) return "������";

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}

	}
	return "";
}



/*:::v1.1.73 ����d���Z*/
class_power_type class_power_yachie[] =
{

	{ 1,5,20,FALSE,FALSE,A_DEX,0,0,"�E�C���m",
	"���_�������͂̃����X�^�[�����m����B" },

	{ 7,10,20,FALSE,TRUE,A_CHR,0,0,"���U",
	"�אڂ��������X�^�[��̂����U���Ĕz���ɂ���B���x�����s����ƃ��j�[�N�����X�^�[�ɂ������\�������邪�N�G�X�g�̑œ|�Ώۃ����X�^�[�ɂ͖����B" },

	{ 15,20,30,FALSE,FALSE,A_INT,0,0,"����̒{��",
	"�w�肵�������X�^�[�𑼂̃����X�^�[����G�΂����悤�Ɏd������B���j�[�N�����X�^�[�⍂���x���ȃ����X�^�[�ɂ͌����ɂ����B" },

	{ 20,20,40,FALSE,FALSE,A_CHR,0,0,"�z������",
	"�z���̓��������������B���x��35�ȏ�ŕ����̏�������B" },

	{ 25,20,50,FALSE,FALSE,A_CON,0,5,"�Ή��̃u���X",
	"����HP��1/4�̈З͂̉Ή������̃u���X��f���B" },

	{ 30,45,50,FALSE,FALSE,A_INT,0,0,"�T�b�n��",
	"�����𒆐S�ɒx�ݑ����̃{�[���𔭐������A����Ɉړ��֎~��Ԃɂ���B" },

	{ 35,160,80,FALSE,FALSE,A_INT,0,0,"����̌��{��",
	"���̃t���A�Ŏ����ɑ΂��ēG�Ώ�������Ă��郂���X�^�[�𔃎����悤�Ǝ��݂�B�����ɐ�������Ə���������������B" },

	{ 39,50,70,FALSE,TRUE,A_STR,0,0,"����e",
	"�������̋��͂ȃ{�[������B" },

	{ 44,66,75,FALSE,TRUE,A_INT,0,0,"����̋S�{��",
	"�t���A�ɂ��镡���̔z�����w�肵�������X�^�[�ׂ̗Ƀe���|�[�g�����A����ɂ��̔z���������ɍs���ł���悤�ɂ���B" },

	{ 48,120,80,FALSE,TRUE,A_CHR,0,0,"�g���匋�E",
	"�����𒆐S�Ƃ����L�͈͂Ɏ��̃��[����~���l�߁A����ɋ߂��̃����X�^�[��\�͒ቺ��Ԃɂ��悤�Ǝ��݂�B" },



	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_yachie(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("�͈�:%d", rad);
		detect_monsters_mind(rad);
	}
	break;


	case 1: //���U
	{
		int y, x;
		int power = plev + chr_adj;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format("����:%d", power);

		if (p_ptr->inside_arena)
		{
			msg_print("�����̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

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

			msg_format("���Ȃ���%s�����U�����B", m_name);

			if (r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flags1 & RF1_QUESTOR)
			{
				msg_format("%s�͕������������Ȃ��I", m_name);
			}
			else if (!(m_ptr->mflag & MFLAG_SPECIAL) && (randint1(power) < r_ptr->level || r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2))
			{
				msg_format("%s�ɒf��ꂽ�B", m_name);

				//�m���Ŏ��̊��U���m�萬��
				if (randint1((r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2) ? r_ptr->level * 3 : r_ptr->level) < plev)
				{
					msg_format("������%s�̈ӎu���キ�Ȃ��Ă����̂�������B", m_name);
					m_ptr->mflag |= MFLAG_SPECIAL;
				}
			}
			else
			{
				msg_format("%s�͂��Ȃ��ɏ]�����B", m_name);
				set_pet(m_ptr);
			}
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;

	case 2://����̒{��
	{
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format("");


		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			int chance = plev * 2 + chr_adj * 4;
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			if (r_ptr->flags1 & RF1_QUESTOR) chance /= 2;
			else if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance = chance * 2 / 3;

			if (m_ptr->mflag & MFLAG_ISOLATION)
			{
				msg_format("���Ɍ����Ă���B");

			}
			else if (chance <= r_ptr->level)
			{
				msg_format("%s�ɑ΂��ĕt�����ޗ]�n�͂Ȃ��悤���B", m_name);
			}
			else if (randint1(chance) < r_ptr->level)
			{
				msg_format("%s�ɑ΂���H��Ɏ��s�����I", m_name);
			}
			else
			{
				if(chance > 200)
					msg_format("%s������ȉA�d�̍����Ɏd���ďグ���I", m_name);
				else if(chance > 100)
					msg_format("%s�̎��������̃X�L�����_���𗬕z�����I", m_name);
				else
					msg_format("�t���A��%s�̈��]�𗬂����I", m_name);

				m_ptr->mflag |= MFLAG_ISOLATION;
			}
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}

		break;
	}


	case 3:
	{
		int i;
		int summon_num = 1;
		if (plev > 34) summon_num += 1 + (plev - 35) / 2;
		if (only_info) return format("������:%d", summon_num);
		int mode = PM_FORCE_PET;

		msg_print("���Ȃ��͔z���̓���������W�����I");

		for (i = 0; i < summon_num; i++)
		{
			int attempt = 10;
			int my, mx;
			int r_idx = MON_ANIMAL_G_KIKETSU;

			while (attempt--)
			{
				scatter(&my, &mx, py, px, 3, 0);

				/* Require empty grids */
				if (cave_empty_bold2(my, mx)) break;
			}
			if (attempt < 0) continue;
			summon_named_creature(0, my, mx, r_idx, (mode));
		}

	}
	break;

	case 4:
	{
		int dam;
		dam = p_ptr->chp / 4;
		if (dam<1) dam = 1;
		if (dam > 1600) dam = 1600;

		if (only_info) return format("����:%d", dam);
		if (!get_aim_dir(&dir)) return NULL;
		msg_print("���Ȃ��͉���f�����I");
		fire_ball(GF_FIRE, dir, dam, (p_ptr->lev > 35 ? -3 : -2));
		break;
	}

	case 5:
	{
		int rad = plev / 7 - 1;
		int base = p_ptr->lev * 3 + chr_adj * 3;

		if (use_itemcard && base < 100) base = 200;

		if (only_info) return format("����:%d", base / 2);
		msg_format("���Ȃ��͎��͂̓����𕕂��悤�Ǝ��݂��B");
		project(0, rad, py, px, base, GF_INACT, PROJECT_KILL, -1);
		project(0, rad, py, px, base/4, GF_NO_MOVE, PROJECT_HIDE|PROJECT_KILL, -1);

	}
	break;

	case 6:
	{
		if (only_info) return format("");

		if (p_ptr->magic_num1[0])
		{
			msg_print("�����H����~�߂��B");
			p_ptr->magic_num1[0] = 0;
		}
		else
		{
			msg_print("���Ȃ��͔z���̓����삽���ɔ����H��𖽂���...");
			p_ptr->magic_num1[0] = 1;
		}
		p_ptr->redraw |= PR_STATUS;
	}
	break;

	case 7: 
	{
		int dice = 5 + chr_adj / 7;
		int sides = plev * 2;

		if (only_info) return format("����:%dd%d", dice,sides);
		if (!get_aim_dir(&dir)) return NULL;

		msg_print("����Ȑ��e��@�������I");

		fire_ball(GF_WATER, dir, damroll(dice,sides), 4);

		break;
	}

	case 8:
	{
		int mon_max = 6 ;//�ő�6��
		int pick_num = 0;
		int i;
		int cnt = 0;
		int idx[6]; //�ő�6��
		if (only_info) return format("�Ώ�:%d��",mon_max);

		//do_new_spell_necromancy()�́u�g�t�v�̖��@���R�s�y���ď�������
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (!is_pet(m_ptr)) continue;
			pick_num++;
			//�Ăяo����ȏ�̐��̔z�����������₩��K���ɓ���ւ���
			if (cnt<mon_max)
				idx[cnt++] = i;
			else if(randint1(pick_num) > cnt) 
				idx[randint0(mon_max)] = i;
		}
		if (!pick_num)
		{
			msg_format("�t���A�ɔz�������Ȃ��B");
			return NULL;
		}
		if (!get_aim_dir(&dir)) return NULL;

		msg_format("���Ȃ��͔z�������Ɋ�P�𖽂����I");
		for (i = 0; i<cnt&&i<mon_max; i++)
		{
			monster_type *m_ptr = &m_list[idx[i]];
			teleport_monster_to(idx[i], target_row, target_col, 100, TELEPORT_PASSIVE);
			m_ptr->energy_need = 0;
			
		}

	}
	break;

	case 9:
	{
		int rad = 6;
		int power = plev * 6 + chr_adj * 10;
		if (only_info) return format("����:�`%d", power/2);

		project(0, rad, py, px, 0, GF_MAKE_GLYPH, PROJECT_GRID | PROJECT_JUMP | PROJECT_HIDE, -1);
		msg_print("����Ȍ��E�����������I");
		project(0, rad, py, px, power, GF_DEC_ALL, PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE, -1);

	}
	break;


	default:
	{
		if (only_info) return "������";

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}

	}
	return "";
}


/*:::v1.1.71 ���S���Z*/
class_power_type class_power_saki[] =
{

	{ 1,5,20,FALSE,FALSE,A_DEX,0,0,"�E�C���m",
	"���_�������͂̃����X�^�[�����m����B" },

	{ 8,10,22,FALSE,FALSE,A_CHR,0,0,"�t�H���~�[�A���A�t���C�h",
	"�אڂ��������X�^�[��̂����U���Ĕz���ɂ���B���|��Ԃ̃����X�^�[�Ɍ��ʂ������B���j�[�N�����X�^�[�ɂ͖����B" },

	{ 14,18,35,FALSE,TRUE,A_DEX,0,0,"���C�g�j���O�l�C",
	"���E���̃����_���ȃ����X�^�[�֑S�e�ˌ����s���B�ˌ��͓d�������r�[���ɕω�����B�ꕔ�̓���ȏe��ɂ͎g�p�ł��Ȃ��B" },

	{ 20,30,45,FALSE,TRUE,A_CON,0,0,"�f���X�N���E�h",
	"�ꎞ�I�Ɍ��f�ϐ��𓾂�B���x��40�ȏ�ł�AC���㏸����B" },

	{ 25,30,50,FALSE,FALSE,A_CHR,0,0,"�S�`�̃z�C�|���C",
	"�z���̓�����𕡐��̏�������B���x��40�ȏ�ł͉������ꂽ��ԂŌĂяo���B" },

	{ 30,70,60,FALSE,FALSE,A_STR,0,0,"���ړ�",
	"�s�������Ƃ̂��钬�Ɉړ�����B�n��ł����g���Ȃ��B" },

	{ 35,50,70,FALSE,FALSE,A_DEX,0,0,"�X�������O�V���b�g",
	"���E���̃����_���ȃ����X�^�[�֑S�e�ˌ����s���A����ɋ��|������B�ꕔ�̓���ȏe��ɂ͎g�p�ł��Ȃ��B" },

	//�ĂƂ��Ă͋��N����l���Ă����̂����ꉞ���l
	//{ 35,66,70,FALSE,TRUE,A_CON,0,0,"�r�[�X�g�G�s�f�~�V�e�B",
	//"���E���̃����X�^�[�ɑ΂��ő����̍U�����s������ɍ����E�N�O������B���E���̃����X�^�[�����W���Ă���قǈЗ͂������Ȃ�B" },

	{ 40,80,75,TRUE,FALSE,A_DEX,50,20,"�g���C�A���O���`�F�C�X",
	"�w�肵�������X�^�[�ׂ̗܂ň�u�ňړ����A����ɍU�����ė��E����B���E�Ɏ��s���邱�Ƃ�����B�����i���d���Ɠ��Z�̎g�p�Ɏ��s���₷���B" },

	{ 43,120,80,FALSE,FALSE,A_STR,0,0,"�}�b�X���G�N�X�v���[�W����",
	"���E���̑S�Ăɑ΂��C�����ōU������B����ɘr�͂��ꎞ�I�Ɍ��E�ȏ�ɏ㏸������B�З͂͌���HP�̔����ɂȂ�B" },

	{ 47,160,90,FALSE,TRUE,A_CHR,0,0,"�u���b�N�y�K�T�X�����e",
	"�З�2�{�̎ˌ����s���B�ˌ���覐Α������P�b�g�ɕω�����B�ꕔ�̓���ȏe��ł͌��ʂ��Ȃ��B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_saki(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("�͈�:%d", rad);
		detect_monsters_mind(rad);
	}
	break;

	case 1: //���U
	{
		int y, x;
		int power = plev * 2 + chr_adj * 2;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (use_itemcard && power < 100) power = 100;

		if (only_info) return format("����:%d", power);

		if (p_ptr->inside_arena)
		{
			msg_print("�����̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			//���|���Ă��郂���X�^�[�Ɍ��ʑ�
			if (MON_MONFEAR(m_ptr)) power *= 3;

			//�N����
			set_monster_csleep(cave[y][x].m_idx, 0);

			if(!one_in_(3) || use_itemcard)
				msg_format("���Ȃ���%s�����U�����B", m_name);
			else 
				msg_print("�ǂ����A����g�ɓ����ĖႦ�Ȃ����낤���H");

			if (r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & RF7_UNIQUE2)
			{
				msg_format("%s�͕������������Ȃ��I", m_name);
			}
			else if (randint1(power) < r_ptr->level)
			{
				msg_format("%s�ɒf��ꂽ�B", m_name);
			}
			else
			{
				msg_format("%s�͂��Ȃ��ɏ]�����B", m_name);
				set_pet(m_ptr);
			}
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;

	//���C�g�j���O�l�C
	case 2:
	{
		if (only_info) return format("");

		if (!rapid_fire(99,1))
		{
			return NULL;
		}

	}
	break;

	case 3: //�f���X�N���E�h
	{
		int base = 15;
		int v;
		if (only_info) return format("����:%d+1d%d�^�[��", base, base);

		msg_format("�Z���ȉ_������A���Ȃ����ݍ���...");
		v = base + randint1(base);
		set_oppose_acid(v, FALSE);
		set_oppose_elec(v, FALSE);
		set_oppose_fire(v, FALSE);
		set_oppose_cold(v, FALSE);
		set_oppose_pois(v, FALSE);
		if (plev > 39) set_shield(v, FALSE);

		break;
	}

	case 4:
	{
		int i;
		int summon_num = plev / 6;
		if (only_info) return format("������:%d", summon_num);
		int mode = PM_FORCE_PET;

		if (plev > 39) mode |= PM_HASTE;

		msg_print("���Ȃ��͔z���̓���������W�����I");

		for (i = 0; i < summon_num; i++)
		{
			int attempt = 10;
			int my, mx;
			int r_idx = MON_ANIMAL_G_KEIGA;

			while (attempt--)
			{
				scatter(&my, &mx, py, px, 3, 0);

				/* Require empty grids */
				if (cave_empty_bold2(my, mx)) break;
			}
			if (attempt < 0) continue;
			summon_named_creature(0, my, mx, r_idx, (mode));
		}

	}
	break;


	case 5: //���ړ�
	{
		if (only_info) return format("");




		if (!dun_level)
		{
			msg_print("���Ȃ��͓V������яオ�����I");

			if (!tele_town(TRUE)) return NULL;

		}
		else
		{
			msg_print("���̋Z�͒n��ł����g���Ȃ��B");
			return NULL;

		}

	}
	break;

	//�X�������O�V���b�g�@�����I�ɂ̓��s�b�h�t�@�C�A�Ɠ����ɂ���
	case 6:
	{
		int power = plev * 3 + chr_adj * 5;
		if (only_info) return format("����:%d",power);

		msg_print("���Ȃ��͈Ј��I�ɏe���\����...");

		if (!rapid_fire(99, 0))
		{
			return NULL;
		}
		turn_monsters(power);

	}
	break;
	//�g���C�A���O���`�F�C�X
	case 7:
	{
		int len = plev / 6;
		if (only_info) return format("�˒�:%d", len);
		if (!rush_attack(NULL, len, HISSATSU_TRIANGLE)) return NULL;
		break;
	}
	break;

	//�}�b�X���G�N�X�v���[�W����
	case 8:
	{
		int dam = p_ptr->chp / 2;
		if (only_info) return format("����:%d",dam);

		msg_print("�K�E�̋C��������n���ӂ����I");
		project_hack2(GF_FORCE, 0, 0, dam);
		set_tim_addstat(A_STR, 120, 10, FALSE);

		break;
	}


	case 9:
	{
		if (only_info) return "�{��:2.0";

		special_shooting_mode = SSM_BLACK_PEGASUS;

		if (!do_cmd_fire())
		{
			special_shooting_mode = 0L;
			return NULL;
		}
	}
	break;



	default:
	{
		if (only_info) return "������";

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}

	}
	return "";
}


/*:::v1.1.69 �������Z*/
class_power_type class_power_urumi[] =
{

	{ 7,7,30,FALSE,TRUE,A_DEX,0,0,"�d�ʑ���T",
	"���͂ȓ������s���B" },

	{ 13,20,40,FALSE,TRUE,A_CHR,0,0,"�X�g�[���x�C�r�[",
	"�אڂ��������X�^�[��̂𐅖v�����悤�Ǝ��݂�B�u�΂̐Ԏq�v���g�p����B���͂ɐ��ӂ��Ȃ��Ǝg�����A���j�[�N�����X�^�[�Ɛ��ϐ����������X�^�[�ɂ͖����B" },

	{ 20,50,60,FALSE,FALSE,A_DEX,0,0,"�΂̐Ԏq����",
	"�A�C�e���u�΂̐Ԏq�v�����B�Ή���ЂƂ����B" },

	{ 25,30,50,FALSE,TRUE,A_STR,0,0,"�d�ʑ���U",
	"��莞�ԁA����U�������������Ƃ��̏d�ʃ{�[�i�X����������S�̈ꌂ���o�₷���Ȃ�B" },

	{ 30,45,55,FALSE,TRUE,A_DEX,0,0,"�����̂����g",
	"���͂̃����X�^�[�̈ړ���Z���ԖW�Q����B" },

	{ 35,40,60,FALSE,TRUE,A_CHR,0,0,"�f�[�����V�[�W",
	"�w�肵���^�[�Q�b�g���ӂɐ����n�̔z�������X�^�[�𕡐���������B���̂Ȃ��Ƃ���ɂ͏����ł��Ȃ��B" },

	{ 40,64,75,FALSE,TRUE,A_STR,0,0,"�d�ʑ���V",
	"�אڂ��������X�^�[�S�Ăɋ��͂Ȓx�ݑ����U�����s���A����ɍU���͂�ቺ������B" },

	{ 48,120,90,FALSE,TRUE,A_CHR,0,0,"�w�r�[�X�g�[���x�C�r�[",
	"�אڂ��������X�^�[��̂��A�ϐ��𖳎����đ啝�Ɍ���������B�u�΂̐Ԏq�v���g�p����B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

//�΂̐Ԏq�쐬�p�Ή�w�胋�[�`��
static bool item_tester_hook_stone(object_type *o_ptr)
{
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_STONE) return TRUE;
	else return FALSE;
}

cptr do_cmd_class_power_aux_urumi(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
		{
			int mult = 1 + (plev + 10) / 20;
			if (only_info) return format("�{��:%d", mult);
			if (!do_cmd_throw_aux(mult, FALSE, -2)) return NULL;
		}
		break;
	case 1:
		{
			int i;
			int inven_idx;
			bool flag_ok = FALSE;
			monster_type *m_ptr;
			int y, x;
			int power = plev * 3 + chr_adj * 3 + 10;

			if (use_itemcard && power < 150) power = 150;

			if (only_info) return  format("����:%d", power);

			//���͂ɐ������邩�ǂ����m�F
			for (i = 1; i <= 9; i++)
			{
				y = py + ddy[i];
				x = px + ddx[i];

				if (!in_bounds(y, x)) continue;
				if (cave_have_flag_bold(y, x, FF_WATER)) flag_ok = TRUE;
			}

			if (!flag_ok)
			{
				msg_print("���̋Z�͐��ӂłȂ��Ǝg���Ȃ��B");
				return NULL;
			}

			//�΂̐Ԏq�������Ă��邩�m�F
			inven_idx = check_have_specific_item(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY);
			if (!use_itemcard && inven_idx < 0)
			{
				msg_print("�΂̐Ԏq���K�v���B");
				return NULL;
			}

			//�אڃ����X�^�[�w��
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			//�����X�^�[�𒾂߂�
			if (cave[y][x].m_idx && (m_ptr->ml))
			{
				int damage;
				int o_idx;
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				monster_desc(m_name, m_ptr, 0);

				if (use_itemcard || !one_in_(4))
					msg_format("%s�ɐ΂̐Ԏq�������t�����B",m_name);
				else
					msg_format("�u�ӂ��ӂ��ӁA���̑���ɂ��̎q�����Ƃ��Ă���Ȃ����H�v");

				//�����X�^�[���΂̐Ԏq�������Ă��鈵���ɂ���(�A�C�e���J�[�h�g�p��������)
				o_idx = o_pop();
				if (o_idx)
				{
					object_type *o_ptr = &o_list[o_idx];

					object_prep(o_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY));

					/* Memorize monster */
					o_ptr->held_m_idx = cave[y][x].m_idx;
					/* Build a stack */
					o_ptr->next_o_idx = m_ptr->hold_o_idx;
					/* Carry object */
					m_ptr->hold_o_idx = o_idx;
				}

				//���v����
				if ((r_ptr->flags7 & RF7_AQUATIC) || (r_ptr->flagsr & RFR_RES_WATE) ||
					(r_ptr->flags1 & RF1_QUESTOR) || (r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
				{
					power = -1;
				}
				else
				{
					if (r_ptr->flags7 & RF7_CAN_SWIM) power /= 2;
					if (r_ptr->flags2 & RF2_POWERFUL) power /= 2;
					if (r_ptr->flags2 & RF2_GIGANTIC) power /= 2;
				}
				if (randint1(power) > r_ptr->level)
				{
					msg_format("%s�𐅒�֒��߂��B", m_name);
					damage = m_ptr->hp + 1;
					project(0, 0, m_ptr->fy, m_ptr->fx, damage, GF_WATER, PROJECT_KILL, -1);
				}
				else
				{
					msg_format("%s�͐��ɒ��܂Ȃ������I", m_name);
				}
				anger_monster(m_ptr);
				set_monster_csleep(cave[y][x].m_idx, 0);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			//�A�C�e���J�[�h�g�p���łȂ��Ƃ��A�΂̐Ԏq����
			if (!use_itemcard)
			{
				inven_item_increase(inven_idx, -1);
				inven_item_describe(inven_idx);
				inven_item_optimize(inven_idx);
			}

		}
		break;

		//�΂̐Ԏq����
		case 2:
		{
			cptr q, s;
			int item;
			object_type forge;
			object_type *q_ptr = &forge;

			if (only_info) return format("");
			item_tester_hook = item_tester_hook_stone;

			q = "�ǂ�����H���܂���? ";
			s = "�Ή򂪕K�v���B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			msg_print("���Ȃ��͐΂����n�߂�..");
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY));
			drop_near(q_ptr, -1, py, px);

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
		}
		break;

		case 3://����d�ʑ��� tim_general[0]���g��
		{
			int mult = plev * 8 / 5;
			int time = 10 + plev / 5;
			if (only_info) return format("����:%d �{��:x%d.%d", time,mult/10,mult%10);

			set_tim_general(time, FALSE, 0, 0);

		}
		break;

		//v2.0.2 �����̂����g
		case 4:
		{

			int dam = plev / 2 + 5;

			if (only_info) return format("����:%d", dam);

			msg_print("�G�ꂽ�ւ̂悤�Ȃ��̂��ӂ�ɗ��݂���...");
			project(0, 4, py, px, dam * 2, GF_NO_MOVE, PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);

		}
		break;

		case 5: //�f�[�����V�[�W(���������X�^�[����)
		{
			int i;
			int y, x;
			int summon_num = 2 + plev / 10;
			int type = SUMMON_AQUATIC;
			bool flag_ok = FALSE;
			u32b mode = PM_FORCE_PET;

			if (only_info) return format("������:%d", summon_num);

			if (!get_aim_dir(&dir)) return NULL;
			if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			for (i = 0; i < summon_num; i++)
			{
				if (summon_specific(-1, y, x, plev+20, type, mode)) flag_ok = TRUE;
			}

			if (flag_ok)
				msg_print("������炵����������t�����I");
			else
				msg_print("��������Ȃ�����...");

		}
		break;

		case 6:
		{
			int rad = 1;
			int base = p_ptr->lev * 10 + chr_adj * 10;
			if (only_info) return format("����:%d", base / 2);
			msg_format("�߂��̑S�Ă̂��̂𒴏d�ʂɂ����I");
			project(0, rad, py, px, base, GF_INACT, PROJECT_KILL, -1);
			project(0, rad, py, px, base, GF_DEC_ATK, PROJECT_KILL|PROJECT_HIDE, -1);

		}
		break;

		//�w�r�[�X�g�[���x�C�r�[
		case 7:
		{
			int i;
			int inven_idx;
			int dec_speed = 15;
			monster_type *m_ptr;
			int y, x;

			if (only_info) return  format("����:%d",dec_speed);

			//�΂̐Ԏq�������Ă��邩�m�F
			inven_idx = check_have_specific_item(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY);
			if (!use_itemcard && inven_idx < 0)
			{
				msg_print("�΂̐Ԏq���K�v���B");
				return NULL;
			}

			//�אڃ����X�^�[�w��
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			//�����X�^�[������������
			if (cave[y][x].m_idx && (m_ptr->ml))
			{
				int damage;
				int o_idx;
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				monster_desc(m_name, m_ptr, 0);

				if (m_ptr->mflag & MFLAG_SPECIAL)
				{
					msg_format("���łɌ����Ă���B");
					return NULL;
				}
				else
				{
					msg_format("%s�ɐ΂̐Ԏq���\��t���A�ǂ�ǂ�d���Ȃ��Ă����I", m_name);
				}

				//�����X�^�[���΂̐Ԏq�������Ă��鈵���ɂ���
				o_idx = o_pop();
				if (o_idx)
				{
					object_type *o_ptr = &o_list[o_idx];

					object_prep(o_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY));

					/* Memorize monster */
					o_ptr->held_m_idx = cave[y][x].m_idx;
					/* Build a stack */
					o_ptr->next_o_idx = m_ptr->hold_o_idx;
					/* Carry object */
					m_ptr->hold_o_idx = o_idx;
				}

				m_ptr->mflag |= MFLAG_SPECIAL; //����t���O�𗧂Ăē�x�ڈȍ~�𖳌���
				if(m_ptr->mspeed > 20) m_ptr->mspeed -= dec_speed;

				anger_monster(m_ptr);
				set_monster_csleep(cave[y][x].m_idx, 0);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			//�A�C�e���J�[�h�g�p���łȂ��Ƃ��A�΂̐Ԏq����
			if (!use_itemcard)
			{
				inven_item_increase(inven_idx, -1);
				inven_item_describe(inven_idx);
				inven_item_optimize(inven_idx);
			}
			
		}
		break;


	default:
	{
		if (only_info) return "������";

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}

	}
	return "";
}


/*:::�O������p���i*/
//�Î�1��tim_general[0]�A�Î�2��tim_general[1]���g��
//�T�j�[�̓����|�C���g��magic_num1[0]���g��
class_power_type class_power_sangetsu_2[] =
{
	{ 1,2,3,FALSE,TRUE,A_DEX,0,5,"�A�C�V�N���V���[�g",
	"��C�����̃{���g����B���x�����オ��ƈЗ͂Ɩ{�����オ�邪����MP���オ��B" },

	{ 5,5,20,FALSE,FALSE,A_CHR,0,0,"���C�̏���",
	"�אڂ��������X�^�[��̂����C�ɖ`�����Ǝ��݂�B���C�ɖ`���ꂽ�����X�^�[�͓G�����̋�ʂȂ����͂̃����X�^�[�Ɛ킢�n�߂�B���j�[�N�����X�^�[�ɂ͌����Â炭�A�ʏ�̐��_�������Ȃ������X�^�[�ɂ͌����Ȃ��B�u���N���E���s�[�X�̏����v���������Ă��Ȃ��Ǝg���Ȃ��B" },

	{ 8,6,20,FALSE,TRUE,A_STR,0,3,"�A�C�X�f�B�]���o�[",
	"�Ή������̃r�[������B" },
	{ 10,10,20,FALSE,FALSE,A_DEX,0,2,"�Z�����ؕ�",
	"�����𒆐S�ɒx�ݑ����̃{�[���𔭐�������B" },
	{ 15,5,25,FALSE,TRUE,A_INT,0,0,"���f�T",
	"�G��̂�����������B�����G�ɂ͌����Ȃ��B��Ԃ͌��ʂ�������B" },
	{ 18,16,40,FALSE,TRUE,A_INT,0,0,"�Î�",
	"��莞�ԁA���������Ƌ߂��̃����X�^�[�����@�ƈꕔ�̍s�����g���Ȃ��Ȃ�B������ǂނƌ��ʂ��������ꏭ������s�����Ԃ�������B" },
	{ 18,0,0,FALSE,TRUE,A_INT,0,0,"�Î����",
	"���Z�u�Î�v�̌��ʂ���������B���̍s���ɂ��^�[������͒ʏ�̔����B" },
	{ 20,10,40,FALSE,TRUE,A_INT,0,0,"���`���t���N�V����",
	"�������܂����ė��ꂽ�ꏊ�����F����B�����������h�A��J�[�e���ŎՂ�ꂽ�������邱�Ƃ͂ł��Ȃ��B" },
	{ 24,25,40,FALSE,TRUE,A_STR,0,0,"�X�򐶐�",
	"�אڂ������ꂩ���̒n�`���u�X��v�ɂ���B�X��͒ʂ蔲�����Ȃ����@���󂵂���Ή��ŗn��������ł���B�܂��M�������̍U�������������ꍇ�唚�����邱�Ƃ�����B���̔����̓v���C���[���_���[�W���󂯂�B" },

	{ 25,0,0,FALSE,FALSE,A_INT,0,0,"����c",
	"MP���킸���ɉ񕜂��m�C���g��ԂɂȂ�B�ʏ펞��2�{�̍s���͂������B" },

	{ 27,20,45,FALSE,TRUE,A_DEX,0,0,"�G���t�B���L���m�s�[",
	"��莞�Ԕ��˔\�͂𓾂�B" },
	{ 30,20,0,FALSE,FALSE,A_DEX,0,0,"���e�ݒu",
	"�����ɔ��e��ݒu����B���e�́u�ʏ�́v10�^�[���Ŕ��������a3�̔j�Б����̋��͂ȃ{�[���𔭐�������B���̔����ł̓v���C���[���_���[�W���󂯂�B���e�͈ꕔ�̑����U���ɎN�����ƗU�������蔚���^�C�~���O���ς�邱�Ƃ�����B" },
	{ 32,30,50,FALSE,TRUE,A_INT,0,0,"���f�U",
	"��莞�ԁA���͂̓G��f�킹��B���E�ɗ���Ȃ��G�ɑ΂��Ă͌��ʂ������B��Ԃ͌��ʎ��Ԃ��Z���Ȃ�B" },
	{ 33,30,60,FALSE,TRUE,A_DEX,0,0,"�A�Q�n�̗ؕ�",
	"�����𒆐S�ɃJ�I�X�����̃{�[���𔭐�������B" },
	{ 37,20,60,FALSE,TRUE,A_CON,0,0,"�I���I���x���g",
	"��莞�Ԏm�C���g�Ɛg�̔\�͏㏸�𓾂�B" },
	{ 40,33,75,FALSE,TRUE,A_INT,0,0,"�u���C�N�L���m�s�[",
	"�^�[�Q�b�g�����X�^�[�ɂ������Ă��閂�@���ʂ���������B" },

	{ 40,120,45,TRUE,TRUE,A_CON,0,30,"�O���C�Y�C���t�F���m",
	"���E���̑S�Ăɑ΂��Ή������U�����s���B�З͂�HP��1/3�ɂȂ�B" },

	{ 45,80,80,FALSE,TRUE,A_WIS,0,0,"�Î�U",
	"��莞�ԁA�t���A�S��̃����X�^�[�����@�ƈꕔ�̍s�����g���Ȃ��Ȃ�B���������͐������󂯂Ȃ��B" },
	{ 45,50,80,FALSE,TRUE,A_CHR,0,0,"�p�[�t�F�N�g�t���[�Y",
	"���E���̑S�Ăɑ΂����͂ȋɊ������U������B" },
	{ 48,72,80,FALSE,TRUE,A_CHR,0,0,"���ɂ̕K�E�Z",
	"�M�������̋��͂ȃ��[�U�[����B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_sangetsu_2(int num, bool only_info)
{
	int dir, dice, sides, base, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

		//�A�C�V�N���V���[�g
	case 0:
	{
		int dice = 2 + plev / 7;
		int dir;
		int sides = 3 + chr_adj / 15;
		int num = MIN(10, (1 + plev / 4));
		if (only_info) return format("����:(%dd%d) * %d", dice, sides, num);
		if (!get_aim_dir(&dir)) return NULL;
		msg_print("�`���m���X�̒e��������I");
		for (i = 0; i < num; i++) fire_bolt(GF_COLD, dir, damroll(dice, sides));

	}
	break;
	//���C�̏���
	case 1:
	{
		monster_type *m_ptr;
		int y, x;
		int power = plev * 2 + chr_adj * 2 + 20;
		bool torch = FALSE;

		if (only_info) return  format("����:%d", power);

		for (i = 0; i<INVEN_TOTAL; i++) if (inventory[i].name1 == ART_CLOWNPIECE) torch = TRUE;
		if (!torch)
		{
			msg_print("���̋Z�͒n���̏������������Ă��Ȃ��Ǝg���Ȃ��B");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);

			if (use_itemcard)
				msg_format("⾉΂����C�̌���������I");
			else
				msg_format("�s�[�X��%s�̑O�ɏ������߂Â����c�c", m_name);

			lunatic_torch(cave[y][x].m_idx, power);
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;
	//�A�C�X�f�B�\���o�[
	case 2:
	{
		int dice = 1 + plev / 4;
		int sides = 7;
		int base = plev / 2 + chr_adj / 2;

		if (only_info) return format("����:%d+%dd%d", base, dice, sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("�M����������B");
		fire_beam(GF_FIRE, dir, base + damroll(dice, sides));
		break;
	}
	//�Z�����ؕ�
	case 3:
	{
		int dam = 10 + plev + chr_adj;
		int rad = 1 + plev / 30;
		if (only_info) return format("����:�`%d", dam / 2);

		msg_print("�����o���ؕ����܂��U�炵���B");
		project(0, rad, py, px, dam, GF_INACT, (PROJECT_KILL | PROJECT_JUMP), -1);
	}
	break;
	//���o1
	case 4:
	{
		int x, y;
		int dist = 2 + plev / 5;
		monster_type *m_ptr;
		int power = 20 + plev;
		if (is_daytime()) power += 30;

		if (only_info) return format("�˒�:%d", dist);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_print("�����ɂ͉������Ȃ��B");
			return NULL;
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			msg_format("�T�j�[��%s�̎��͂̌i�F��c�߂��E�E", m_name);

			if (r_ptr->flags2 & RF2_SMART || r_ptr->flags3 & RF3_NO_CONF ||
				r_ptr->flagsr & RFR_RES_ALL || randint1(power) < r_ptr->level)
			{
				msg_format("���������ʂ��Ȃ������B");
			}
			else
			{
				msg_format("%s�͍��������I", m_name);
				(void)set_monster_confused(cave[y][x].m_idx, MON_CONFUSED(m_ptr) + 10 + randint1(plev / 3));
			}
			set_monster_csleep(cave[y][x].m_idx, 0);
			anger_monster(m_ptr);
		}

		break;
	}

	case 5://�Î�1 tim_general[0]���g��
	{
		int time = 10 + plev / 5;
		int rad = plev / 5;
		if (only_info) return format("����:%d �͈�:%d", time, rad);
		if (p_ptr->tim_general[0] || p_ptr->tim_general[1])
		{
			msg_print("���łɔ\�͂��g���Ă���B");
			return NULL;
		}

		msg_print("���i�͎w��炵���B���͂̉����������B");
		set_tim_general(time, FALSE, 0, 0);

	}
	break;
	case 6://�Î����
	{
		if (only_info) return format("");
		if (!p_ptr->tim_general[0] && !p_ptr->tim_general[1])
		{
			msg_print("�\�͂��g���Ă��Ȃ��B");
			return NULL;
		}

		msg_print("���i�͔\�͂����������B");
		set_tim_general(0, TRUE, 0, 0);
		set_tim_general(0, TRUE, 1, 0);

	}
	break;
	case 7:
	{
		//v1.1.34 �T�j�[�́u�������܂����ė��ꂽ�ꏊ�̌��i������v�\�͂�ǉ��B
		//�܂�����t���O�𗧂ĂĂ���flying_dist�l������h�A��J�[�e���ɎՂ���悤�ɂ��čČv�Z����B
		//���ɂ���flying_dist�l���ݒ肳�ꂽ�O���b�h�݂̂��[�ցA�����X�^�[���m����B
		//�Ō�Ƀt���O��߂���flying_dist�l���Čv�Z����B
		if (only_info) return format("�͈�:%d�ړ��O���b�h", MONSTER_FLOW_DEPTH);

		if (p_ptr->blind)
		{
			msg_print("�T�j�[�͖ڂ������Ȃ��B");
			return NULL;
		}

		msg_print("���ꂽ�ꏊ�̉f�����T�j�[�̖ڂ̑O�ɍL�������c�c");
		flag_sunny_refraction = TRUE;
		update_flying_dist();
		wiz_lite(FALSE);
		detect_monsters_normal(255);
		flag_sunny_refraction = FALSE;
		update_flying_dist();

	}
	break;

	//v1.1.68 �X�򐶐�
	case 8:
	{
		int y, x, dir;

		if (only_info) return format("");
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (!cave_naked_bold(y, x))
		{
			msg_print("���̏ꏊ�ɂ͓��Z���g���Ȃ��B");
			return NULL;
		}
		msg_print("�`���m�����łȕX�����o�����B");
		cave_set_feat(y, x, f_tag_to_index_in_init("ICE_WALL"));

		break;
	}
	//����c
	case 9:
	{
		int mana = plev / 5;
		if (only_info) return format("��:%d", mana);

		if (!one_in_(5))
			msg_print("�݂�Ȃō���c���n�߂�..");
		else if (p_ptr->prace == RACE_NINJA)
		{
			if(one_in_(3))	msg_print("�u�͂�Ŗ_�Œ@���I�v");
			else if (one_in_(2))	msg_print("�u�������\�͂ŉ�������̂���Ԃ��v");
			else msg_print("�u�K���o���]�[�I�v");

		}
		else
		{
			if (one_in_(3))	msg_print("�`���m�ƃs�[�X�����܂��n�߂��B");
			else if (one_in_(2))	msg_print("�X�^�[�̓e�B�[�^�C���ɓ������B");
			else msg_print("�u�݂�Ȉꏏ�Ɏv�������肩����Ε����锤���Ȃ��I�v");


		}

		player_gain_mana(mana);
		set_hero(10, FALSE);
		//����s����2�{
		new_class_power_change_energy_need = 200;
		break;
	}



	case 10:
	{
		int time;
		int base = 15 + p_ptr->lev / 5;

		if (only_info) return format("����:%d+1d%d", base, base);
		time = base + randint1(base);
		msg_format("�T�j�[�͐g�\�����B");
		set_tim_reflect(time, FALSE);

		break;
	}

	case 11:
	{
		int dam = 100 + plev * 4;
		if (only_info) return format("����:%d", dam);
		if (!futo_can_place_plate(py, px) || cave[py][px].special)
		{
			msg_print("�����ɂ͔��e��u���Ȃ��B");
			return NULL;
		}
		msg_print("�X�^�[�͔��e���d�|�����B");
		/*:::Mega Hack - cave_type.special�𔚒e�̃J�E���g�Ɏg�p����B*/
		cave[py][px].info |= CAVE_OBJECT;
		cave[py][px].mimic = feat_bomb;
		cave[py][px].special = 10;
		while (one_in_(5)) cave[py][px].special += randint1(20);
		note_spot(py, px);
		lite_spot(py, px);
	}
	break;
	case 12:
	{
		int base = 5 + plev / 5;
		if (is_daytime()) base *= 2;
		if (only_info) return format("����:%d+1d%d", base, base);
		msg_format("�T�j�[�͌��𑀂��ĊF�̎p���������B");
		set_tim_superstealth(base + randint1(base), FALSE, SUPERSTEALTH_TYPE_OPTICAL);
		break;
	}

	case 13://�A�Q�n�̗ؕ�
	{
		int dam = plev * 4 + chr_adj * 4;
		int rad = 1 + plev / 15;
		if (only_info) return format("����:�`%d", dam / 2);

		msg_print("�����o���d�����P���ؕ����܂��U�炵��...");
		project(0, rad, py, px, dam, GF_CHAOS, (PROJECT_KILL | PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM), -1);

	}
	break;

	//�I���I���x���g
	case 14:
	{
		int base = 10 + plev / 5;
		int time;
		if (only_info) return format("����:%d+1d%d", base, base);
		time = base + randint1(base);
		msg_format("�X�^�[���F�����サ���I");
		set_hero(time, FALSE);
		set_tim_addstat(A_STR, 4, time, FALSE);
		set_tim_addstat(A_CON, 4, time, FALSE);
		set_tim_addstat(A_DEX, 4, time, FALSE);

		break;
	}
	//�u���C�N�L���m�s�[
	case 15:
	{

		int m_idx;

		if (only_info) return format("");
		if (!target_set(TARGET_KILL)) return NULL;
		m_idx = cave[target_row][target_col].m_idx;
		if (!m_idx) return NULL;
		if (!player_has_los_bold(target_row, target_col)) return NULL;
		if (!projectable(py, px, target_row, target_col)) return NULL;
		msg_print("���i�͎w��炵���B");
		dispel_monster_status(m_idx);

		break;
	}

	case 16:
	{
		int base = p_ptr->chp / 3;

		if (base < 1) base = 1;

		if (only_info) return format("����:�`%d", base);

		msg_print("�s�[�X���������M�g�����͂��Ă��������I");
		project_hack2(GF_FIRE, 0, 0, base);

	}
	break;



	case 17://�Î�2 tim_general[1]���g��
	{
		int time = 30;
		if (only_info) return format("����:%d", time);
		if (p_ptr->tim_general[1])
		{
			msg_print("���łɔ\�͂��g���Ă���B");
			return NULL;
		}

		msg_print("���i�̓t���A�S�̂̉��Ɋ������B");
		set_tim_general(time, FALSE, 1, 0);
		p_ptr->tim_general[0] = 0;
	}
	break;

	//�p�[�t�F�N�g�t���[�Y
	case 18:
	{
		int dam = plev * 4 + chr_adj * 10;
		if (only_info) return format("����:%d", dam);
		msg_print("�`���m���S�͂�������ĕӂ�𓀂点���I");
		project_hack2(GF_ICE, 0, 0, dam);
	}
	break;
	//���͋Z
	case 19:
	{
		int dice = 6 + chr_adj / 5;
		int sides = plev;
		int base = plev * 2 + chr_adj * 5;

		if (only_info) return format("����:%d+%dd%d", base, dice, sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("�F�����[�U�[������A�T�j�[�������������������I");
		fire_beam(GF_LITE, dir, base + damroll(dice, sides));
		break;
	}



	default:
		if (only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}






/*:::v1.1.67 �v�̉̓��Z*/
class_power_type class_power_kutaka[] =
{
	{ 5,3,20,FALSE,FALSE,A_INT,0,0,"���Ӓ���",
	"���͂̃����X�^�[�����m����B����Ƀ��x��15�Ńg���b�v�A���x��25�ŃA�C�e���A���x��35�Œn�`�����m����B" },

	{ 10,0,20,FALSE,FALSE,A_INT,0,0,"���������T",
	"���|��Ԃ��������A�m�\�⌫�����ቺ���Ă���ꍇ�񕜂���B���x��30�ȍ~�ł�MP���킸���ɉ񕜂���B�����������i����͂̒n�`�Ɋւ���L�����������Ƃ�����B" },

	{ 15,24,40,FALSE,TRUE,A_INT,0,0,"���z��̎���",
	"�n�ʂ𐅒n�`�ɕω�������˒��̒Z���r�[������B" },

	{ 20,20,40,FALSE,TRUE,A_INT,0,0,"���n���̎���",
	"�����𖾂邭���A����Ɏ��E���̃����X�^�[�����������悤�Ǝ��݂�B" },

	{ 24,36,50,FALSE,TRUE,A_INT,0,0,"�S�n�̎���",
	"���E���̃����X�^�[���e���|�[�g�����悤�Ǝ��݂�B" },

	{ 28,56,50,FALSE,TRUE,A_CON,0,20,"�ڊo�߂�A����ꂽ�H��B��",
	"���E���S�Ăɍ��������U�����s���B���͂̓G���N����B����ɗF�D�I�ȗH��n�̃����X�^�[�����̌����B" },

	{ 32,50,75,FALSE,FALSE,A_INT,0,0,"���������U",
	"�I�������K���ς݂̖��@����Y�p���A�����w�K�\��������₷�B�Y�p�������@�̏n���x�������Ɠ���₷�B" },

	{ 36,40,60,FALSE,TRUE,A_WIS,0,0,"���̕�����",
	"�ꎞ�I�Ɏm�C���g�E���̋����𓾂�B�܂������Ƌ߂��̃����X�^�[�̃e���|�[�g��j�Q����B" },

	{ 38,55,70,FALSE,TRUE,A_CHR,0,0,"���E����",
	"���E���̃����X�^�[�S�Ăɋ��͂Ȑ��_�U�����s���A����ɋ�����ɂ��悤�Ǝ��݂�B" },

	{ 43,60,75,FALSE,TRUE,A_WIS,0,0,"���̕�����",
	"�����𒆐S�Ƃ����������̃{�[���𔭐�������B���̍U���͐������Ɣj�ב����̗����̐��������B" },

	{ 45,96,80,FALSE,TRUE,A_CHR,0,0,"�S��S�n��",
	"�����̎��͂ɂ��錶�z���̑��݂ł͂Ȃ������X�^�[���t���A����Ǖ����悤�Ǝ��݂�B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};


cptr do_cmd_class_power_aux_kutaka(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0://���Ӓ���
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("�͈�:%d", rad);

		msg_print("���Ȃ��͓G�̋C�z��T����...");
		detect_monsters_normal(rad);
		detect_monsters_invis(rad);
		if (plev > 14)
		{
			detect_traps(rad, TRUE);
		}
		if (plev > 24)
		{
			detect_objects_gold(rad);
			detect_objects_normal(rad);
		}
		if (plev > 34)
		{
			map_area(rad);
		}


		break;
	}
	case 1://����1
	{
		if (only_info) return "";


		if (p_ptr->afraid || p_ptr->stat_cur[A_INT] != p_ptr->stat_max[A_INT] || p_ptr->stat_cur[A_WIS] != p_ptr->stat_max[A_WIS])
		{
			msg_format("���Ȃ��͌��Ȃ��Ƃ�Y��Ă��܂����Ƃɂ����B");
			set_afraid(0);
			do_res_stat(A_INT);
			do_res_stat(A_WIS);

		}
		else if(plev > 29)
		{
			msg_format("���Ȃ��͓�������ۂɂ���...");
			if (player_gain_mana(randint1(3 + plev / 7))) msg_print("�����X�b�L�������B");
		}

		if (randint1(plev+5) > plev)
		{
			if (lose_all_info()) msg_print("...�H�@��������F�X�Y�ꂷ�����悤���B");
		}

		break;
	}


	case 2://���z��
	{
		int dist = 2 + plev / 5;
		if (only_info) return format(" �˒�:%d", dist);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;
		msg_format("���Ȃ��͂��̒n�ɐ��H���������B");
		fire_beam(GF_WATER_FLOW, dir, 10);
		break;
	}
	case 3://���n��
	{
		int power = 10 + p_ptr->lev * 3;
		if (only_info) return format("����:%d", power);

		msg_format("���Ȃ��͌��f�I�Ȍ���������B");
		lite_area(damroll(2,15), 3);
		confuse_monsters(power);
		break;
	}

	case 4://�S�n��
	{
		int power = p_ptr->lev * 4;
		if (only_info) return format("����:%d", power);

		msg_format("���Ȃ��͖��@�҂�����ǂ��������B");
		banish_monsters(power);

	}
	break;

	case 5:
	{
		int dam = plev * 5;
		int summon_num = randint1(3);
		if (only_info) return format("����:%d", dam);

		if (one_in_(2))	
			msg_print("�u�R �P �R �b �R �[ �I�v");
		else			
			msg_print("���Ȃ��͎��҂����ыN����悤�ȑ剹����������I");

		project_hack2(GF_SOUND, 0, 0, dam);
		aggravate_monsters(0, FALSE);
		for (; summon_num; summon_num--)
		{
			summon_specific(0, py, px, dun_level, SUMMON_GHOST, (PM_FORCE_FRIENDLY | PM_ALLOW_GROUP));
		}

	}
	break;

	case 6://�I��I�L������
	{
		if (only_info) return "";

		msg_print("���Ȃ��͕��G�ȖY�p�̂��߂ɏW�����n�߂�...");
		flag_spell_forget = TRUE;

		do_cmd_cast();

		flag_spell_forget = FALSE;

	}
	break;


	case 7: //���̕�����
	{
		int v;
		bool base = 10;
		int percentage = p_ptr->chp * 100 / p_ptr->mhp;
		if (only_info) return format("����:%d+1d%d", base, base);

		v = base + randint1(base);
		if(one_in_(3))
			msg_print("���Ȃ��͕s�ޓ]�̊o������߂��I");
		else if (one_in_(2))
			msg_print("���Ȃ��͂��������炷�錈�ӂ��ł߂��I");
		else 
			msg_print("�u�R�P�[�I�v�u�s���I�v");

		set_hero(v, FALSE);
		set_tim_addstat(A_STR, 104, v, FALSE);
		set_tim_addstat(A_CON, 104, v, FALSE);
		set_tim_addstat(A_DEX, 104, v, FALSE);
		set_nennbaku(v, FALSE);
		p_ptr->chp = p_ptr->mhp * percentage / 100;
		p_ptr->redraw |= (PR_HP);
		redraw_stuff();

		break;
	}

	case 8: //���E����
	{

		int power = p_ptr->lev * 2 + chr_adj * 5;
		if (only_info) return format("����:%d", power);

		msg_format("���Ȃ��̉s����������͂���␂߂��I");
		project_hack(GF_REDEYE, power);
		stasis_monsters(power);

		break;
	}

	case 9:
	{
		int dam = plev * 6 + chr_adj * 6;
		if (only_info) return format("����:�`%d", dam / 2);

		msg_print("���Ȃ��͗H���̋���N���҂𔱂����I");
		project(0, 5, py, px, dam, GF_HOLY_WATER, (PROJECT_KILL | PROJECT_JUMP), -1);

	}
	break;

	case 10://�S��S�n��
	{

		int rad = plev;
		int power = p_ptr->lev * 2 + chr_adj * 5;
		if (only_info) return format("�͈�:%d ����:%d",rad, power);

		msg_format("���Ȃ��͏����ꂴ��҂��������̒n������������I");
		mass_genocide_2(power, rad, 1);

		break;
	}

	default:
	{
		if (only_info) return "������";

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}

	}
	return "";
}



/*:::v1.1.66 ���|�p���Z*/
class_power_type class_power_mayumi[] =
{
	{ 8,5,20,FALSE,FALSE,A_INT,0,0,"���G",
	"���͂̃����X�^�[�����m����B" },

	{ 15,20,40,FALSE,FALSE,A_INT,0,0,"�����X�^�[����",
	"���E���̃����X�^�[�̏��𓾂�B" },

	{ 20,30,50,FALSE,TRUE,A_CHR,0,0,"���m����",
	"�����X�^�[�u���m���ցv����������B���x��40�ȍ~�́u�n�����m���ցv����������B" },
	{ 25,30,50,FALSE,TRUE,A_CHR,0,0,"�|������",
	"�����X�^�[�u�|�����ցv����������B���x��40�ȍ~�́u�n���|�����ցv����������B" },
	{ 30,30,50,FALSE,TRUE,A_CHR,0,0,"�R�n������",
	"�����X�^�[�u�R�n�����ցv����������B���x��40�ȍ~�́u�n���R�n�����ցv����������B" },

	{ 45,80,70,FALSE,TRUE,A_CHR,0,0,"�s�s�̖��s���c",
	"�����̗̑͂Ƒ������������A���E���̓G�΃����X�^�[�����|�����A����ɑ����̉������ꂽ���ւ��������A�q�[���[�A�j���A�����A�Ύ׈����E�𓾂�B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};


cptr do_cmd_class_power_aux_mayumi(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("�͈�:%d", rad);

		msg_print("���Ȃ��͓G�̋C�z��T����...");
		detect_monsters_normal(rad);
		detect_monsters_invis(rad);

		break;
	}
	case 1:
	{
		if (only_info) return "";

		msg_format("���Ȃ��͊�O�̓G�𒍈Ӑ[������߂�...");
		probing();
		break;
	}
	case 2:
	case 3:
	case 4:
	{
		bool flag = FALSE;
		int i;
		int count = 2 + plev / 8;
		int mode;


		if (only_info) return "";
		for (i = 0; i<count; i++)
		{
			int r_idx;

			if (num == 2) r_idx = plev > 39 ? MON_HANIWA_F2 : MON_HANIWA_F1;
			else if (num == 3) r_idx = plev > 39 ? MON_HANIWA_A2 : MON_HANIWA_A1;
			else r_idx = plev > 39 ? MON_HANIWA_C2 : MON_HANIWA_C1;

			if (summon_named_creature(0, py, px, r_idx, PM_FORCE_PET)) flag = TRUE;

		}
		if (flag)
			msg_print("�����B�����W�����B");
		else
			msg_print("�N������Ȃ�����...");



		break;
	}

	case 5:
	{
		int i,v;
		if (only_info) return "";

		v = 100 + randint1(100);

		msg_print("���Ȃ��͕��c�ɓˌ����߂��������I");
		//���͂̔z���̏��ւ������A����ȊO�����|�ɂ���
		crusade();
		//���͂ɏ��֌R�c����������
		for (i = 0; i < 30; i++)
		{
			int attempt = 10;
			int my, mx;
			int r_idx;

			if ((i % 3) == 0) r_idx = MON_HANIWA_F2;
			else if ((i % 3) == 1) r_idx = MON_HANIWA_A2;
			else r_idx = MON_HANIWA_C2;

			while (attempt--)
			{
				scatter(&my, &mx, py, px, 5, 0);

				/* Require empty grids */
				if (cave_empty_bold2(my, mx)) break;
			}
			if (attempt < 0) continue;
			summon_named_creature(0, my, mx, r_idx, (PM_FORCE_PET | PM_HASTE));
		}
		set_hero(v, FALSE);
		set_blessed(v, FALSE);
		set_fast(v, FALSE);
		set_protevil(v, FALSE);
		set_afraid(0);
		set_broken(-10000);
		hp_player(5000);

	}
	break;

	default:
	{
		if (only_info) return "������";

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}

	}
	return "";
}






/*:::�ؐ��p���i��p�Z*/
class_power_type class_power_oni_kasen[] =
{

	{ 5,10,20,TRUE,FALSE,A_STR,0,0,"�Ⓤ��",
	"�A�C�e���u�Ή�v�𓊂�����B" },

	{ 10,10,30,TRUE,FALSE,A_DEX,0,0,"���͓���",
	"�A�C�e��������͂ɓ�������B" },

	{ 20,20,40,FALSE,TRUE,A_INT,0,0,"�X�P���g������",
	"�X�P���g���n�̔z���𕡐��̏�������B" },

	{ 25,50,50,TRUE,FALSE,A_STR,0,20,"�Ռ��g",
	"�����̎��͂ɑ΂����������U�����s���B�З͂͗אڍU���̃^�[���_���[�W�Ɠ����B���x��45�𒴂���Ǝ��E���S�Ă��ΏۂɂȂ�B���͂̃����X�^�[���N����B" },

	{ 30,30,50,FALSE,TRUE,A_INT,0,0,"�A���f�b�h�x�z",
	"���E���̃A���f�b�h�S�Ă�z���ɂ��悤�Ǝ��݂�B" },

	{ 35,66,70,FALSE,TRUE,A_INT,0,0,"�[���̗�",
	"�n�������̋���ȃ{�[������B" },

	{ 40,96,80,FALSE,TRUE,A_INT,0,0,"���Ԓn�����",
	"�߂��̃����X�^�[��S�ē|���A���̌���HP�ɓ������ʂ̗̑͂��񕜂���B��R�����Ɩ����B�����̂Ȃ������X�^�[�ɂ������B�܂��͋����G�⌫���G�ɂ͌����ɂ����B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};
cptr do_cmd_class_power_aux_oni_kasen(int num, bool only_info)
{
	int dir, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	switch (num)
	{
	case 0:
		{
			object_type forge;
			object_type *tmp_o_ptr = &forge;
			int ty, tx;

			if (only_info) return "";

			if (!get_aim_dir(&dir)) return NULL;
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			msg_print("�n�ʂ���Ή���@��o���ē��������I");
			object_prep(tmp_o_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_STONE));
			do_cmd_throw_aux2(py, px, ty, tx, 3, tmp_o_ptr, 0);

		}
		break;
	case 1: //���͓���
		{
			int mult = 2 + plev / 15;
			if (only_info) return format("�{��:%d", mult);
			if (!do_cmd_throw_aux(mult, FALSE, -1)) return NULL;
		}
		break;
	case 2: //�X�P���g������
		{
			int i;
			int summon_num = 2 + plev / 10;
			int type = SUMMON_SKELETON;
			bool flag_ok = FALSE;
			u32b mode = PM_FORCE_PET;
			if (plev > 34) mode |= PM_ALLOW_GROUP;

			if (only_info) return format("������:%d", summon_num);


			for (i = 0; i < summon_num; i++)
			{
				if (summon_specific(-1, py, px, plev, type, mode)) flag_ok = TRUE;
			}

			if(flag_ok)
				msg_print("�n�ʂ���[���������o���A���Ȃ��ɏ]�����B");
			else
				msg_print("�n�ʂ������������C�����邪�A�����N����Ȃ������B");

		}
		break;
	case 3:
		{
			int damage;

			int str_adj = adj_general[p_ptr->stat_ind[A_STR]];


			if (p_ptr->do_martialarts)
			{
				damage = (calc_martialarts_dam_x100(0) * p_ptr->num_blow[0] + calc_martialarts_dam_x100(1) * p_ptr->num_blow[1]) / 100;
			}
			else
			{
				damage = (calc_weapon_dam(0) + calc_weapon_dam(1));
			}

			if (p_ptr->lev < 45)
			{
				if (only_info) return format("����:���悻�`%d", damage);

				msg_format("���Ȃ��͒n�ʂ�%s��ł��t�����I", (p_ptr->do_martialarts ? "��":"����"));
				project(0, 3, py, px, damage, GF_SOUND, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);

			}
			else
			{
				if (only_info) return format("����:���悻%d", damage);
				msg_print("���r�̈ꌂ����n��h�邪�����I");
				project_hack2(GF_SOUND, 0, 0, damage);
			}
			aggravate_monsters(0,FALSE);
		}
		break;
	case 4:
		{
			int power = plev + chr_adj * 5;

			if (only_info) return format("����:%d", power);

			msg_print("���Ȃ��͗₽���ڂŎ��͂��ɂ݂���...");
			charm_undead(power);

		}
		break;
	case 5:
	{

		int dir,damage;
		int rad = 6;

		damage = plev * 4 + chr_adj * 10;

		if (only_info) return format("����:%d", damage);

		if (!get_aim_dir(&dir)) return NULL;
		if (!fire_ball_jump(GF_NETHER, dir, damage, rad, "�R�̂悤�Ȋ[�����~�蒍�����I")) return NULL;

	}
	break;
	case 6:
	{
		int power = plev * 7 + chr_adj * 5;
		if (power < 300) power = 300;
		if (only_info) return format("����:%d", power / 2);


		msg_print("���Ԓn���ւ̖傪�J�����I");

		project(0, 1, py, px, power, GF_SOULSTEAL, PROJECT_KILL, -1);


	}
	break;


	default:
		if (only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



//���ԗp���Z
class_power_type class_power_eika[] =
{
	{ 10,5,25,FALSE,TRUE,A_DEX,0,0,"�ΐς݇T",
	"�����̂���O���b�h�Ɂu���̃��[���v��z�u����B" },

	{ 20,20,30,FALSE,TRUE,A_WIS,0,0,"�X�g�[���E�b�Y",
	"�_���W�������ɒn�k���N�����Ċ�΂��~�点��B" },

	{ 30,30,50,TRUE,FALSE,A_DEX,0,0,"�ΐς݇U",
	"�w������̗אڃO���b�h���u��΁v�n�`�ɂ���B" },

	{ 40,50,80,FALSE,FALSE,A_WIS,0,0,"�`���h�����Y�����{",
	"�����𒆐S�Ƃ����L�͈͂Ɂu���̃��[���v��z�u����B" },

	{ 45,72,90,FALSE,TRUE,A_CHR,0,0,"�G�C�W���̐ϐ�",
	"����5�O���b�h�ȓ��ɂ�����̃��[������1/3�̊m���Ń����_���ȃ^�[�Q�b�g�Ɋj�M�����̃r�[�����������B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};



/*:::���ԗp���Z*/
cptr do_cmd_class_power_aux_eika(int num, bool only_info)
{
	int dir, dice, sides, base, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
	{
		if (only_info) return "";
		msg_print("���Ȃ��͐S�����߂Ĉ��΂�ς�...");
		warding_glyph();
	}
	break;
	case 1:
	{
		int rad = plev / 5;
		int time;
		if (only_info) return format("���a:%d", rad);
		msg_format("�_���W�������h�ꂽ�I");
		earthquake(py, px, rad);

		break;
	}

	case 2:
	{
		int y, x;

		if (only_info) return format("");
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (!cave_naked_bold(y, x))
		{
			msg_print("�����ɂ͐΂�ς߂Ȃ��B");
			return NULL;
		}
		msg_print("���Ȃ��͏u���ԂɐΕǂ�ςݏグ���I");
		cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));

		break;
	}


	case 3:
	{
		int rad = plev / 15;

		if (only_info) return format("���a:%d", rad);
		msg_format("���Ȃ��͎q���삽���Ɛΐς݃R���e�X�g���n�߂�...");
		project(0, rad, py, px, rad*2, GF_MAKE_GLYPH, PROJECT_GRID, -1);
		break;
	}
	break;


	case 4:
	{
		int dam = (plev + chr_adj)/2 ;
		cave_type       *c_ptr;
		bool flag = FALSE;
		int x, y;
		int x2, y2;
		int test_count = 0;
		if (only_info) return format("����:%d * �s��", dam);

		msg_print("���͂̐ϐ΂��P���n�߂��I");
		//���[�����̓`���h�����Y�����{�ꔭ��35(�Ή��ϐ������M���ϐ��������Ғl250)�A�~���l�߂܂�����90(��650)���炢
		for (y = 1; y < cur_hgt - 1; y++)
		{
			for (x = 1; x < cur_wid - 1; x++)
			{
				int tmp_idx_cnt = 0;
				int target_who_tmp=0;
				c_ptr = &cave[y][x];
				if (!(c_ptr->info & CAVE_OBJECT) || !have_flag(f_info[c_ptr->mimic].flags, FF_GLYPH))
					continue;

				//�U���񐔂������Ȃ肷����̂Ŕ����m����1/3�ɂ��Ă��̂Ԃ�З͂Œ�������
				if (!one_in_(3)) continue;

				if (distance(py, px, y, x) > 5) continue;

				if (!los(py, px, y, x)) continue;

				test_count++;

				//���[���̂���O���b�h���炳��Ƀ����X�^�[���[�v
				tmp_idx_cnt = 0;
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];

					if (!m_ptr->r_idx) continue;
					if (is_pet(m_ptr)) continue;
					if (is_friendly(m_ptr)) continue;
					if (!m_ptr->ml) continue;
					if (!projectable(y, x, m_ptr->fy, m_ptr->fx)) continue;

					tmp_idx_cnt++;
					if (one_in_(tmp_idx_cnt)) target_who_tmp = i;
				}

				if (!tmp_idx_cnt)continue;
				//�^�[�Q�b�g�����܂�����r�[��
				hack_project_start_x = x;
				hack_project_start_y = y;


				(void)project(0, 0, m_list[target_who_tmp].fy, m_list[target_who_tmp].fx, dam, GF_NUKE, (PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU), -1);

				flag = TRUE;

			}
		}
		if (!flag) msg_print("...�C�����������������B");

		hack_project_start_x = 0;
		hack_project_start_y = 0;

		if (cheat_xtra) msg_format("glyph_count:%d", test_count);

	}
	break;


	default:
		if (only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}




/*:::�B��ޓ��Z*/
class_power_type class_power_okina[] =
{

	{ 1,0,0,FALSE,TRUE,A_INT,0,0,"��˂̗�",
	"��˂�ʂ��ă����X�^�[�B����D�������Z���g���B���x���A�b�v�ɂ��V���Ȗ��@����Z���K������B" },

	{ 5,5,10,FALSE,FALSE,A_INT,0,0,"��˂�`��",
	"���͂̔������m����B���x���A�b�v�ɔ������m�ł�����̂̎�ނƊ��m�͈͂��L����B" },

	{ 10,20,25,FALSE,TRUE,A_DEX,0,0,"��˂̋���",
	"�����̎��͂̒n�ʂɃh�A�𐶐�����B���x��40�ȍ~�͎��E���S�Ă̒n�ʂ��ΏۂɂȂ�B" },

	{ 12,25,30,FALSE,TRUE,A_INT,0,0,"��ˋA��",
	"��˂̍���ʂ��Ēn��֋A�҂���B���̋A�҂́u�قځv�����ɔ�������BEXTRA���[�h�ł͉��̊K�փ��x���e���|�[�g���s���B" },

	{ 15,30,30,FALSE,FALSE,A_CHR,0,0,"��_�̌��",
	"���͂̃����X�^�[�𖣗����z���ɂ��悤�Ǝ��݂�B�������߂��قǌ��ʂ������B" },

	{ 18,50,50,FALSE,TRUE,A_INT,0,0,"��˂����������",
	"�����̎��͂̓G�΃����X�^�[���t���A����Ǖ�����B�N�G�X�g�œ|�Ώۃ����X�^�[�̓t���A�̕ʂ̏ꏊ�ɋ����e���|�[�g����B�Ǖ����������X�^�[�̐��ɉ�����HP���񕜂���B�N�G�X�g�_���W�����ł͎g���Ȃ��B" },

	{ 20,180,70,FALSE,FALSE,A_DEX,0,0,"��_�̈Ö��e��",
	"�u�p�^�[���v�Ɋ����ē���̐��͂ɑ΂���U�����s���B����ȍ~�ɐV���ȃt���A�ɓ���Ƃ��̐��͂̃����X�^�[�̗͑͂��������čŏ�����N���Ă����ԂɂȂ�B���x�����㏸����ƍU���ł��鐨�͂̎�ނ�������B���̓��Z�́u�p�^�[���v�̒��S�ɂ��Ȃ��Ǝg�p�ł��Ȃ��B" },

	{ 23,40,50,FALSE,TRUE,A_WIS,0,0,"�_��̋ʖ�",
	"�ꎞ�I�Ɍ��f�U���ɑ΂���ϐ��𓾂�B���x��45�ȍ~�͖��@�̊Z���ʂ������ɓ���B" },

	{ 27,56,50,FALSE,TRUE,A_DEX,0,0,"�����̌�",
	"�ꎞ�I�Ɂu�������v�𐶐����đ�������B�ǂ��炩�̘r���󂢂Ă���Ƃ��ɂ����g���Ȃ��B���̕���𑕔�����O�����Ƃ͂ł��Ȃ��B" },

	{ 30,48,60,FALSE,TRUE,A_WIS,0,0,"�}�^�[���X�b�J",
	"�ꎞ�I�ɒm�\�E�����E���͂���������B" },
	
	{ 35,50,80,FALSE,TRUE,A_INT,0,0,"�^�E��ˈړ�",
	"��˂̍���ʂ��Ďw�肵�������X�^�[�ׂ̗Ƀe���|�[�g����B���̂Ƃ��t���A�S�̂̃����X�^�[�̈ʒu�����m���邱�Ƃ��ł���B�ʏ��1.5�{�̍s���͂������B�e���|�[�g�s�\�Ȓn�`�ɂ͏o���Ȃ��B�n��Ŏg���ƍs�������Ƃ̂���X�ֈړ��ł���B" },

	{ 38,65,70,FALSE,TRUE,A_INT,0,0,"���o�[�X�C�����H�[�J�[",
	"��莞�ԁA�����̎��͂ōs��ꂽ�G�ΓI�ȏ������@�����m���őj�Q����B" },

	{ 40,128,80,FALSE,FALSE,A_DEX,0,0,"��˂ւ̔����J������",
	"�T�����_�ւ̓���������o���B���͂ɂ��L���X�y�[�X���Ȃ��Ǝg���Ȃ��B�N�G�X�g���s���̃t���A�ł͎g���Ȃ��B" },

	{ 43,50,80,FALSE,FALSE,A_CHR,0,0,"�����̌|�\��",
	"�ꎞ�I�ɂ�����Z�\���x�������݂̖��͒l�Ɠ����Ƃ��Ĉ�����B" },

	{ 47,100,80,FALSE,TRUE,A_CHR,0,0,"�w�ʂ̈Í����y",
	"���E���̑S�Ăɑ΂��A�ɂ߂ċ��͂ȈÍ������U���Ƌ��C�����U�����s���B�Â����ł����g���Ȃ��B" },


{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};



cptr do_cmd_class_power_aux_okina(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0: //��˂̗�
	{
		if (only_info) return "";

		if (!cast_monspell_new()) return NULL;
		break;
	}

	case 1: //��˂�`��
	{
		int rad = 10 + plev;
		if (only_info) return format("�͈�:%d", rad);

		msg_print("���Ȃ��͌�˂�`������...");
		if (plev < 15)
			detect_doors(rad);
		else
			map_area(rad);

		if (plev > 9)
		{
			if( plev < 20)
				detect_monsters_normal(rad);
			else 
				detect_all(rad);
		}

		break;
	}

	case 2://��˂̋���
	{
		if (plev < 40)
		{
			int rad = plev / 10 + 1;
			if (only_info)				return format("���a:%d", rad);

			project(0, rad, py, px, 100, GF_MAKE_DOOR, (PROJECT_JUMP | PROJECT_GRID), -1);

		}
		else
		{
			if (only_info)
				return format(" ");

			project_hack2(GF_MAKE_DOOR, 0, 0, 100);
		}
	}
	break;

	case 3: //��ˋA��
	{
		if (only_info) return "";
		int m_idx;

		msg_print("��˂̍��ւ̔����J�����B");

		if (EXTRA_MODE)
		{
			teleport_level(0);
		}
		else
		{
			recall_player(1);
		}
	}
	break;

	case 4://��_�̌��
	{
		int power = plev * 4 + chr_adj * 10;
		int rad = plev / 7;
		if (only_info) return format("����:�`%d", power / 2);

		if (one_in_(3))
			msg_print("���Ȃ��͑���Ȓ��q�Ō��n�߂�...");
		else if (one_in_(2))
			msg_print("���Ȃ���ῂ�����������I");
		else
			msg_print("���Ȃ��͈Ј��I�ȕ��͋C�������o�����B");

		project(0, rad, py, px, power, GF_CHARM, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);

	}
	break;


	case 5: //��˂����������
	{
		int rad = plev / 2;
		int geno_count;
		if (only_info) return format("�͈�:%d", rad);

		geno_count = mass_genocide_3(rad, FALSE, TRUE);

		if (geno_count)
		{
			msg_format("%d�̂̃����X�^�[��w���̔�����ǂ��������B", geno_count);
			hp_player(damroll(geno_count, plev));
		}
		break;
	}

	case 6: //��_�̈Ö��e��
	{
		if (only_info) return "";

		if (!pattern_attack()) return NULL;

		break;
	}


	case 7: //�_��̋ʖ�
	{
		int time;
		int base = 25;
		if (only_info) return format("����:%d+1d%d", base, base);

		msg_print("�����P���������Ȃ�����...");
		time = base + randint1(base);
		set_oppose_acid(time, FALSE);
		set_oppose_elec(time, FALSE);
		set_oppose_fire(time, FALSE);
		set_oppose_cold(time, FALSE);
		set_oppose_pois(time, FALSE);

		if (plev > 44)
			set_magicdef(time, FALSE);

		break;
	}

	case 8: //�����̌�
	{
		if (only_info) return format("����:25");
		if (!generate_seven_star_sword()) return NULL;
		break;
	}

	case 9: //�}�^�[���X�b�J
	{
		int v;
		bool base = 10;
		if (only_info) return format("����:%d+1d%d", base, base);

		v = base + randint1(base);
		msg_print("���Ȃ��̊�͉b�q�Ǝ����݂ɖ�����ꂽ...");
		set_tim_addstat(A_INT, 100 + plev / 5, v, FALSE);
		set_tim_addstat(A_WIS, 100 + plev / 5, v, FALSE);
		set_tim_addstat(A_CHR, 100 + plev / 5, v, FALSE);

		break;
	}

	case 10: //��ˈړ�
	{
		if (only_info) return format("����:�Ȃ�");

		msg_print("���Ȃ��͌�˂̍���`������...");

		if (!dun_level)
		{
			if (!tele_town(TRUE)) return NULL;

		}
		else
		{

			detect_monsters_normal(255);

			if (p_ptr->paralyzed || p_ptr->confused || p_ptr->image)
			{
				msg_print("���Ȃ��͈ړ��Ɏ��s�����I");
			}
			else
			{
				//�e���|�[�g�ɐ���������TRUE���A���čs����1.5�{
				if (dimension_door(D_DOOR_BACKDOOR))
					new_class_power_change_energy_need = 150;
			}
		}

	}
	break;

	case 11: //���o�[�X�C�����H�[�J�[
	{
		int base = 25;
		if (only_info) return format("����:%d+1d%d", base, base);

		set_deportation(randint1(base) + base, FALSE);
		break;
	}

	case 12: //��˂̍��ւ̔����J�� ���́u���鏊�ɐR����v����R�s�[
	{
		int dx, dy, i;
		bool flag_ok = TRUE;
		if (only_info) return format("");

		if (p_ptr->inside_quest
			|| quest[QUEST_TAISAI].status == QUEST_STATUS_TAKEN && dungeon_type == DUNGEON_ANGBAND && dun_level == 100
			|| quest[QUEST_SERPENT].status == QUEST_STATUS_TAKEN && dungeon_type == DUNGEON_CHAOS && dun_level == 127)
		{
			msg_print("���͎g���Ȃ��B");
			return NULL;
		}

		for (i = 1; i<10; i++)
		{
			dx = px + ddx[i];
			dy = py + ddy[i];
			if (!in_bounds(dy, dx) || !cave_clean_bold(dy, dx)) flag_ok = FALSE;
		}

		if (!flag_ok)
		{

			msg_print("�����ł͎g���Ȃ��B");
			return NULL;
		}

		msg_print("��˂̍��ւ̔����J�����I");

		cave_set_feat(py, px, f_tag_to_index_in_init("HOME"));

	}
	break;


	case 13: //�����̌|�\��
	{
		int base = 20;
		int time;

		if (only_info) return format("����:%d+1d%d", base, base);
		time = base + randint1(base);
		set_tim_general(time, FALSE, 0, 0);

	}
	break;

	case 14://�w�ʂ̈Í����y
	{
		int dam = plev * 5 + chr_adj * 10;
		if (only_info) return format("����:%d * 2", dam);


		if (p_ptr->cur_lite > 0 || cave[py][px].info & CAVE_GLOW)
		{
			msg_print("���̋Z�͖��邢�ꏊ�ł͎g���Ȃ��B");
			return NULL;
		}
		msg_print("���Ȃ��͈ł̒��œƂ�x��n�߂�...");
		project_hack2(GF_DARK, 0, 0, dam);
		project_hack2(GF_COSMIC_HORROR, 0, 0, dam);

		break;
	}


	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}


//v1.1.52 ���̐��E��俎q
class_power_type class_power_sumireko_d[] =
{

	{ 1, 0,0,FALSE,FALSE,A_INT,0,0,"�V���ȗ͂̎g�p",
	"��_�ɗ^����ꂽ�V���ȗ͂��g�p����B�Z���Ƃɒǉ���MP�������B" },

	{ 1, 0,0,FALSE,FALSE,A_INT,0,0,"�V���ȗ͂ɂ��Ē��ׂ�",
	"��_�ɗ^����ꂽ�V���ȗ͂̏ڍׂ��m�F����B" },


	{ 6,4,25,FALSE,TRUE,A_WIS,0,2,"�p�C���L�l�V�X",
	"���˒����Z���Ή������̃r�[������B" },
	{ 16,10,40,FALSE,TRUE,A_INT,0,0,"�w�����e���|�[�g",
	"�w�肵�������֒Z�����̃e���|�[�g���s���B�ʏ�̔����̍s���͂�������Ȃ��B�e���|�[�g�ł��Ȃ��ꏊ�Ƀe���|�[�g���悤�Ƃ����ꍇ�A���̎��ӂ̃����_���ȏꏊ�֏o�邩���̏ꏊ�ɖ߂�B" },
	{ 25,0,75,FALSE,TRUE,A_WIS,0,0,"���_�W��",
	"MP���킸���ɉ񕜂���B" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_sumireko_d(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!activate_nameless_art(FALSE)) return NULL;
	}
	break;
	case 1:
	{
		if (only_info) return format("");
		activate_nameless_art(TRUE);
		return NULL;//�m�F�݂̂Ȃ̂ōs��������Ȃ�
	}
	break;

	case 2:
	{
		int dist = 4 + plev / 8;
		int dice = 8 + plev / 2;
		int sides = 5;
		if (only_info) return format("����:%dd%d �˒�:%d", dice, sides, dist);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;
		msg_format("�w��炷�Ƌ�C���R���オ�����B");
		fire_beam(GF_FIRE, dir, damroll(dice, sides));
		break;
	}
	case 3:
	{
		int dist = 5 + plev / 10;
		if (only_info) return format("����:%d", dist);

		if(!teleport_to_specific_dir(dist, 7, 0)) return NULL;

		new_class_power_change_energy_need = 50;
		break;
	}
	case 4:
	{
		if (only_info) return format("");
		msg_print("���Ȃ��̓X�v�[�������o���A���_���W�������E�E");
		p_ptr->csp += (4 + randint1(plev / 10));
		if (p_ptr->csp >= p_ptr->msp)
		{
			p_ptr->csp = p_ptr->msp;
			p_ptr->csp_frac = 0;
		}
		p_ptr->redraw |= (PR_MANA);
	}
	break;


	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}





/*:::�ː_������p�Z*/
class_power_type class_power_shion[] =
{
	{ 1,0,0,FALSE,TRUE,A_INT,0,0,"�������S�߈�",
	"�����X�^�[�ɜ߈˂��đ���B���j�[�N�����X�^�[�ɂ͐������ɂ����A�����Ă��郂���X�^�[�ɂ͐������₷���B�߈˂��ꂽ�����X�^�[�͑œ|�ς݈����ɂȂ�B�߈˂Ɏ��Ԑ����͂Ȃ����͏����ł���������Ȃ��B" },

	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"���",
	"�����X�^�[����H���╨���𓾂悤�Ǝ��݂�B����������A�C�e�������ꂽ�����X�^�[�̓t���A���狎��B�N�G�X�g�_���W�����ł͎g���Ȃ��B" },

	{ 6,20,30,FALSE,TRUE,A_DEX,0,0,"�A��",
	"�_���W�����ƒn��̊Ԃ��s��������B�����܂łɂ̓^�C�����O������B" },

	{ 13,20,40,TRUE,FALSE,A_DEX,0,0,"����",
	"75%�̊m����1d200�̋����̃e���|�[�g�����A25%�̊m���Ń��x���e���|�[�g����B" },

	{ 19,30,50,FALSE,TRUE,A_INT,0,0,"������",
	"�ꎞ�I�ɉB���\�͂����߂�B���x��40�ȍ~�͕ǔ����\�͂��l������B" },

	{ 33,0,0,FALSE,FALSE,A_CHR,0,0,"�ŋ��ň��̋ɕn�s�K�_",
	"�����ȃX�[�p�[�n�R�_�ɕω�����B�ϐg�\�ȃ^�[�����͒~�ς����u�s�^�p���[�v�ɂ�葝���A4�^�[���ȉ��ł͕ϐg�ł��Ȃ��B���܂�^�[���������܂肷����Ə���ɕϐg���邱�Ƃ�����B" },

	{ 37,36,55,FALSE,TRUE,A_CON,0,0,"���n�R��",
	"�~�ς����u�s�^�p���[�v�̈ꕔ������A���͂ȈÍ������̃{�[������B" },

	{ 42,96,60,FALSE,TRUE,A_CHR,0,0,"�~�X�`�����X�X�L���b�^�[",
	"���E���̃����X�^�[�S�Ăɑ΂��A����HP��40%�`60%�̃_���[�W��^����B��R�����Ɩ����B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B�X�[�p�[�n�R�_�̎��ɂ����g���Ȃ��B" },



	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

// �X�[�p�[�n�R�_�ւ̕ϐg�J�E���^��tim_general[0]���g�p����
// �O��ϐg�����Ƃ��̃^�[������magic_num1[0]�ɋL�^����
// �O��ϐg���Ă�����͂œ|�ꂽ�����X�^�[�̌o���l��magic_num1[1]�ɋL�^����
// �����X�^�[�ɜ߈˂����񐔂�magic_num1[2]�ɋL�^����
// �X�[�p�[�n�R�_�̏�Ԃ�J��|�����t���O��magic_num2[0]���g��
cptr do_cmd_class_power_aux_shion(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:

	{
		int x, y;
		int dist = 6 + plev / 8;
	//	int time = 50 + plev;
		monster_type *m_ptr;
		char m_name[80];


		if (only_info) return format("�˒�:%d", dist);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;

		x = target_col;
		y = target_row;

		if (dir != 5 || !target_okay() || !projectable(y, x, py, px))
		{
			msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
			return NULL;
		}
		project_length = 0;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_print("�����ɂ͉������Ȃ��B");
			return NULL;
		}
		else
		{
			int r_idx = m_ptr->r_idx;
			int chance;
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);


			chance = (plev + chr_adj) * 100 / MAX(r_ptr->level,1);
			if (MON_CSLEEP(m_ptr)) chance = chance * 2;
			if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance /= 2;

			if (p_ptr->wizard) chance = 100;
			if (chance > 100) chance = 100;
			if (r_ptr->flags1 & RF1_QUESTOR) chance = 0;
			if (r_ptr->flagsr & RFR_RES_ALL) chance = 0;

			if (m_ptr->r_idx == MON_TENSHI) chance = 0; //�V�q�͐S�Ɍ����Ȃ������Ȃ̂Ő�����0�ɂ��Ƃ��B����Ȃ瑼�̓V�l���l�_�l�A���͂ǂ��Ȃ̂Ǝv�����C�ɂ��Ȃ����Ƃɂ���

			if (!get_check_strict(format("���߂��܂����H(������:%d%%)", chance), CHECK_DEFAULT_Y)) return NULL;
			//��������ƃ����X�^�[���폜�����̃����X�^�[�ɕϐg
			if (randint0(100) < chance)
			{
				int mon_old_hp = m_ptr->hp;

				check_quest_completion(m_ptr);
				delete_monster_idx(cave[y][x].m_idx);
				move_player_effect(y, x, MPE_DONT_PICKUP);
				metamorphose_to_monster(r_idx,-1);

				if (p_ptr->chp < mon_old_hp)
				{
					p_ptr->chp = MIN(p_ptr->mhp,mon_old_hp);
					p_ptr->redraw |= PR_HP;
					redraw_stuff();
				}

				//�߈ː���������œ|�ς݂ɂ���
				if (r_ptr->r_akills < MAX_SHORT) r_ptr->r_akills++;
				if (r_ptr->r_pkills < MAX_SHORT) r_ptr->r_pkills++;
				if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 0;


				gain_exp(r_ptr->mexp);
				//�^�C���z����������̂Ƃ��A�s�^�p���[��~�ς���
				p_ptr->magic_num1[1] += r_ptr->mexp * 3;
				//�߈ˉ񐔃J�E���g
				p_ptr->magic_num1[2]++;



			}
			else
			{
				if (chance < 1)
					msg_format("%s�̐S�ɂ͑S�������Ȃ��I", m_name);
				else
					msg_print("�߈˂Ɏ��s�����I");
				//�N����
				set_monster_csleep(cave[y][x].m_idx, 0);
				if (is_friendly(m_ptr))
				{
					msg_format("%s�͓{����!",m_name);
					set_hostile(m_ptr);
				}

			}
		}
	}
	break;

	case 1:
	{
		if (only_info) return "";
		if (!shion_begging()) return NULL;
		break;
	}
	case 2:
	{
		if (only_info) return "";
		if (!word_of_recall()) return NULL;
		break;
	}

	case 3:
	{
		if (only_info) return format("����:�s��");

		if (one_in_(4) && !p_ptr->inside_quest && dun_level)
		{
			teleport_level(0);
		}
		else
		{
			teleport_player(randint1(200), 0L);
		}

		break;
	}

	case 4:
	{
		base = plev / 5;
		int v;
		if (only_info) return format("����:%d+1d%d", base,base);

		msg_print("���Ȃ��̎p���ڂ₯��...");
		v = randint1(base) + base;
		set_tim_stealth(v, FALSE);
		if (plev > 39) set_kabenuke(v, FALSE);

	}
	break;

	case 5:
	{
		int v = CALC_SUPER_SHION_TURNS;


		if (only_info) return format("����:%d turn", v);

		if(SUPER_SHION)
		{
			msg_print("���łɕϐg���Ă���B");
			return NULL;
		}

		if (v < 5)
		{
			msg_print("�܂��\���ȕs�^�p���[�����܂��Ă��Ȃ��B");
			return NULL;
		}

		set_tim_general(v, FALSE, 0, 0);

		//�ϐg����magic_num1[0]�����݃^�[�����ɂ�[1]�����Z�b�g
		p_ptr->magic_num1[0] = turn;
		p_ptr->magic_num1[1] = 0;

		//HP�S��
		hp_player(9999);


		break;
	}
	case 6:
	{
		int pows = p_ptr->magic_num1[1] / SHION_BINBOUDAMA;
		int dam = 250 + plev * 5;


		if (only_info) return format("����:%d �g�p��:%d", dam,pows);

		if (!pows)
		{
			msg_print("�\���ȕs�K�p���[�����܂��Ă��Ȃ��B");
			return NULL;
		}

		if (!get_aim_dir(&dir)) return NULL;

		msg_format("�F����W�߂��s�K�p���[��@���t�����I");
		fire_ball(GF_DARK, dir, dam, 4);

		p_ptr->magic_num1[1] -= SHION_BINBOUDAMA;

	}
	break;

	case 7:
	{
		int power = p_ptr->lev * 10;
		if (power < 300) power = 300;

		if (only_info) return format("����:%d", power);

		if (use_itemcard) msg_print("�J�[�h�̒��̔�����j�ł̃I�[���������o�����I");
		else msg_print("���Ȃ��̎�������j�ł̃I�[���������o�����I");

		project_hack(GF_HAND_DOOM, power);

		break;
	}





	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



/*:::�ː_������p�Z*/
//magic_num1[0],[1]���U���ɏ�������Ƃ��ċL�^���Ă���
class_power_type class_power_jyoon[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�w�֑���",
	"��p�X���b�g�Ɏw�ւ�8�܂ő����ł���B���������������w�ւ̃p�����[�^�͔���(�[���؂�̂�)�Ƃ��Ĉ�����B���ꂽ�w�ւ͑����ł��Ȃ��B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�w�։���",
	"��p�X���b�g�ɑ������Ă���w�ւ��O���B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"�w�֔���",
	"��p�X���b�g�ɑ������Ă���w�ւ𔭓�����B" },

	{ 10,5,50,FALSE,FALSE,A_INT,0,0,"�ڗ���",
	"�A�C�e����̉��l��m�邱�Ƃ��ł���B�Ӓ�┻�ʂ͂ł��Ȃ��B" },
	{ 15,10,45,FALSE,FALSE,A_INT,0,0,"���݊��m",
	"�t���A�̃A�C�e���ƍ�������m����B���x��30�ȍ~�̓A�C�e�������������Ă��郂���X�^�[�����m����B" },
	{ 20,0,0,FALSE,FALSE,A_CHR,0,0,"�U��",
	"�U�����n�߂�B��������$1000�ȏ�K�v�B�U�����͊i���U���̈З͂��啝�ɏ㏸���邪�U���̂��тɏ���������������B������x���̃R�}���h�����s����ƌ��ɖ߂�B���̃R�}���h�̎��s�ɂ͍s����������Ȃ��B" },
	{ 25,25,0,TRUE,FALSE,A_DEX,30,10,"�ːi",
	"���O���b�h����u�ňړ�����B�r���Ƀ����X�^�[��������C�����_���[�W��^���Ď~�܂�B" },
	{ 32,20,60,FALSE,TRUE,A_STR,0,0,"�X���C�u���o�[",
	"�אڂ��������X�^�[��̂��������Ă���Ƃ����̌��ʂ�D������Č���������B���x��45�ȍ~�͂���ɖ��G���̌��ʂ�D������Ă킸���ɍs���x��������B" },
	{ 38,80,70,FALSE,TRUE,A_CHR,0,0,"�v���b�N�s�W����",
	"��莞�ԁA���͂̃����X�^�[�̖��@��������ቺ������B�A�C�e�������𑽂��������X�^�[�قǌ��ʂ������B" },
	{ 43,120,80,FALSE,TRUE,A_INT,0,0,"�A�u�\�����[�g���[�U�[",
	"�����X�^�[��̂ɕn�R�_�̎o�������I�ɜ߈˂�����B�o�ɜ߈˂��ꂽ�����X�^�[�͂��̃t���A����̈ꎞ�I�Ȕz���Ƃ��Ă��Ȃ��̎x�z���󂯂�B�N�G�X�g�œ|�Ώۂ̃����X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_jyoon(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;

		p_ptr->update |= PU_BONUS;
		update_stuff();
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		p_ptr->update |= PU_BONUS;
		update_stuff();
		break;
	}
	case 2:
	{
		if (only_info) return format("");
		if (!activate_inven2()) return NULL;
		break;
	}

	case 3:
	{
		cptr q, s;
		object_type *o_ptr;
		int item;
		int value;
		if (only_info) return "";

		q = "�ǂ̃A�C�e�����m�F���܂����H";
		s = "�A�C�e�����Ȃ��B";
		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return NULL;
		if (item >= 0)
		{
			o_ptr = &inventory[item];
		}
		else
		{
			o_ptr = &o_list[0 - item];
		}
		value = object_value_real(o_ptr);

		if (!value)
		{
			msg_format("���̃A�C�e���͖����l���B");
			if (object_is_cursed(o_ptr))
				msg_format("���̏����Ă���B");
		}
		else
		{
			msg_format("���̃A�C�e���ɂ�%s$%d�̉��l������悤���B", ((o_ptr->number>1) ? "�������" : ""), value);
			if (object_is_cursed(o_ptr))
				msg_format("����������Ă���B");

		}

		//if (object_is_cheap_to_jyoon(o_ptr))
		//	msg_format("���Ȃ��ɂƂ��Ă͐g�ɂ���ɒl���Ȃ��������B");

		break;
	}


	case 4:
	{
		int range = 25 + plev / 2;
		if (only_info) return format("�͈�:%d", range);

		detect_objects_gold(range);
		detect_objects_normal(range);

		if (plev > 29)
			detect_monsters_rich(range);

		break;
	}


	case 5:
	{
		if (only_info) return format("");

		if (p_ptr->special_attack & ATTACK_WASTE_MONEY)
		{
			msg_print("�U�����~�߂��B");
			p_ptr->special_attack &= ~(ATTACK_WASTE_MONEY);
		}
		else if (p_ptr->au < 1000) //v1.1.47 �����ǉ�
		{
			msg_print("�旧���̂��Ȃ��B�����Ɖ҂��Ȃ��ƁB");

		}
		else
		{
			msg_print("���Ȃ��͉����F�̃I�[�����܂Ƃ����I");
			p_ptr->special_attack |= ATTACK_WASTE_MONEY;
		}
		p_ptr->redraw |= PR_STATUS;

		return NULL; //�s��������Ȃ�
	}
	break;

	case 6:
	{
		int len = p_ptr->lev / 5;
		if (len < 5) len = 5;
		damage = p_ptr->lev * 3;
		if (damage < 50) damage = 50;
		if (only_info) return format("�˒�:%d ����:%d", len, damage);
		if (!rush_attack2(len, GF_FORCE, damage,0)) return NULL;
		break;
	}


	case 7://�X���C�u���o�[
	{
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format(" ");
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			int tim;
			bool flag_success = FALSE;
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);

			//�N����
			set_monster_csleep(cave[y][x].m_idx, 0);

			msg_format("���Ȃ���%s�֔�ъ|�������I", m_name);

			tim = MON_FAST(m_ptr);

			if (tim)
			{
				msg_print("�������ʂ�D��������I");
				set_monster_fast(cave[y][x].m_idx, 0);
				set_monster_slow(cave[y][x].m_idx, tim);
				set_fast(tim, FALSE);
				flag_success = TRUE;

			}

			tim = MON_INVULNER(m_ptr);
			if (tim && (use_itemcard || plev > 44))
			{
				msg_print("�����̋���D��������I");
				set_monster_invulner(cave[y][x].m_idx, 0,TRUE);
				set_invuln(tim, FALSE);
				flag_success = TRUE;
			}

			if (!flag_success) msg_print("...�����������D���Ȃ������B");

		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;

	case 8: //�v���b�N�s�W����
	{
		int base = 10;
		int time;

		if (only_info) return format("����:%d+1d%d", base, base);
		time = base + randint1(base);
		set_tim_general(time, FALSE, 0, 0);

	}
	break;

	case 9://�A�u�\�����[�g���[�U�[
	{
		int y, x;
		int i;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format(" ");

		for (i = 1; i < m_max; i++)
		{
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (m_ptr->mflag & MFLAG_POSSESSED_BY_SHION)
			{
				msg_print("���o�͑��̃����X�^�[�ɜ߂���Ă���B");
				return NULL;
			}
		}
		if (p_ptr->inside_arena)
		{
			msg_print("�����̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			int tim;
			bool flag_success = FALSE;
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			//�N����
			set_monster_csleep(cave[y][x].m_idx, 0);

			if ((r_ptr->flags1 & RF1_QUESTOR) || (r_ptr->flagsr & RFR_RES_ALL))
			{
				msg_format("�o��%s�ɜ߈˂ł��Ȃ������I", m_name);
			}
			else
			{
				msg_format("�o��%s�ɜ߈˂��x�z�����I", m_name);
				set_pet(m_ptr);
				m_ptr->mflag |= (MFLAG_EPHEMERA | MFLAG_POSSESSED_BY_SHION);
			}
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;



	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.44 ���ǂ�(��)
/*:::���̐��E�̂��ǂ񂰐�p�Z*/
class_power_type class_power_udonge_d[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"���f�B�X���`�F�X�g(���[)",
	"�ǂ����ɉB�������Ă���傫�Ȗ��ĂɃA�C�e��������B�A�C�e�����̐��̓��x���A�b�v�ŏ㏸����B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"���f�B�X���`�F�X�g(�m�F)",
	"���Ă̒�������B" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"���f�B�X���`�F�X�g(��o��)",
	"���Ă���A�C�e�����o���B" },

	{ 5,10,25,FALSE,FALSE,A_STR,0,0,"���i�e�B�b�N�N�����v",
	"�����r�ɑ������́u���i�e�B�b�N�K���v�̎c�e��1����A�e�Ɠ����З͂̒�˒��̍��������u���X����B" },
	{ 12,20,55,FALSE,FALSE,A_INT,0,0,"�g������",
	"���͂ɂ�����̂����m����B���x�����オ��Ɗ��m�ł�����̂̎�ނƔ͈͂�������B" },
	{ 16,18,30,FALSE,TRUE,A_WIS,0,0,"�}�C���h�E�F�[�u",
	"�����������_�U���̃��[�U�[����B���_�̊󔖂ȓG�⋶�C�������炷�G�A���j�[�N�����X�^�[�ɂ͌��ʂ������B" },
	{ 20,30,75,FALSE,FALSE,A_INT,0,0,"�g���f�f",
	"���E���̃����X�^�[�̔g����ǂݎ���Ĕ\�͂�p�����[�^�Ȃǂ𒲍�����B" },

	{ 25,40,60,FALSE,FALSE,A_STR,0,0,"���f�B�X���`�F�X�g(����)",
	"���Ă������ɓ�������B���Ă̒��ɂ���A�C�e���͑S�Ă΂�T����Ė�͊����B�u*����*�̖�v�͎��E���_���[�W�łȂ��ʏ�̔����ɂȂ�B" },

	{ 30,72,80,FALSE,TRUE,A_DEX,0,0,"���i�e�B�b�N�_�u��",
	"���E���̃����_���ȃ^�[�Q�b�g�ɘA�˂���B�A�ˉ\���̓��x���A�b�v�ő�������B�e�̖��������㏸����B�ꕔ�̓���ȏe��ł͎g�p�ł��Ȃ��B" },

	{ 35,40,70,FALSE,TRUE,A_INT,0,0,"�f�B�X�I�[�_�[�A�C",
	"�ߋ������E���̍D���ȏꏊ�ɏu�Ԉړ�����B���̂Ƃ����̍s���܂łɂ����鎞�Ԃ�������������B" },

	{ 40,80,75,FALSE,TRUE,A_CHR,0,0,"���i�e�B�b�N���b�h�A�C�Y",
	"���E���̑S�Ăɑ΂��ċ��͂Ȑ��_�U�����s���B" },

	{ 43,64,70,FALSE,TRUE,A_STR,0,0,"���i�e�B�b�N�G�R�[",
	"�����r�ɑ������́u���i�e�B�b�N�K���v�̎c�e��S�ď���A�e�З�*�c�e�̈З͂̋��͂ȍ��������r�[������B" },
	{ 48,180,90,FALSE,TRUE,A_CHR,0,0,"�C�r���A���W�����[�V����",
	"�G����̍U�����O�񖳌�������o���A�𒣂�B�������A�C�e���̔j�����̌������̍U���͖h���Ȃ��B�ʏ�̖��G���ƈႢ�������ɍs���x�����Ȃ��B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};
cptr do_cmd_class_power_aux_udonge_d(int num, bool only_info)
{
	int dir, dice, sides, base, damage;
	int plev = p_ptr->lev;
	switch (num)
	{


	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //���邾���Ȃ̂ōs��������Ȃ�

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}

	case 3:
	case 11:
	//���i�e�B�b�N�N�����v�ƃ��i�e�B�b�N�G�R�[
	{
		int hand;
		int dam = 0;
		int range = 2 + plev / 16;

		//����̃��i�e�B�b�N�K���̃_���[�W���v�l���v�Z ���悤�Ǝv��������͂�Иr�����ɂ���
		for (hand = 0; hand <= 0; hand++)
		{
			int mult;
			object_type *o_ptr = &inventory[INVEN_RARM + hand];
			int timeout_base, bullets;

//			if (o_ptr->tval != TV_GUN || o_ptr->sval != SV_FIRE_GUN_LUNATIC) continue;

			//v1.1.98 ���i�e�B�b�N�K�����������
			if (!(o_ptr->tval == TV_GUN && ( o_ptr->sval == SV_FIRE_GUN_LUNATIC || o_ptr->sval == SV_FIRE_GUN_LUNATIC_2))) continue;

			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			bullets = calc_gun_load_num(o_ptr);

			mult = (timeout_base * bullets - o_ptr->timeout) / timeout_base; //�e�̎c�e��
			if (num == 3) mult = MIN(1,mult); //���i�e�B�b�N�N�����v�͈ꔭ�����g��Ȃ�

			if (!mult) continue;

			dam += (o_ptr->dd * (o_ptr->ds + 1) / 2 + o_ptr->to_d) * mult;

		}
		if (dam > 9999) dam = 9999;


		if (only_info)
		{
			if (num == 3)
				return format("�˒�:%d ����:%d",range, dam);
			else
				return format("����:%d", dam);
		}

		if (dam <= 0)
		{
			msg_print("���̑����ł͋Z���g���Ȃ��B");
			return NULL;
		}


		if (num == 3)
		{
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���i�e�B�b�N�K���������ɍ\���đ吺���o�����I");
			fire_ball(GF_SOUND, dir, dam, -2);
		}
		else
		{
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���C�̋��т��_���W������k�킹���I");
			fire_spark(GF_SOUND, dir, dam, 2);
		}

		//�c�e��������
		for (hand = 0; hand <= 0; hand++)
		{
			int mult;
			object_type *o_ptr = &inventory[INVEN_RARM + hand];
			int timeout_base, bullets, timeout_max;

			//if (o_ptr->tval != TV_GUN || o_ptr->sval != SV_FIRE_GUN_LUNATIC) continue;

			//v1.1.98 ���i�e�B�b�N�K�����������
			if (!(o_ptr->tval == TV_GUN && (o_ptr->sval == SV_FIRE_GUN_LUNATIC || o_ptr->sval == SV_FIRE_GUN_LUNATIC_2))) continue;

			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			bullets = calc_gun_load_num(o_ptr);
			timeout_max = timeout_base * (bullets - 1);

			mult = (timeout_base * bullets - o_ptr->timeout) / timeout_base; //�e�̎c�e��
			if (num == 3) mult = MIN(1, mult); //���i�e�B�b�N�N�����v�͈ꔭ�����g��Ȃ�

			if (!mult) continue;

			o_ptr->timeout += timeout_base * mult;
			p_ptr->window |= PW_EQUIP;
			//�K���J�^�c�e����̂��ߒe�؂�`�F�b�N
			if (o_ptr->timeout > timeout_max)
				p_ptr->update |= PU_BONUS;


		}


	}
	break;
	case 4:
	{
		int rad = p_ptr->lev / 2 + 10;
		if (only_info) return format("�͈�:%d", rad);

		if (use_itemcard)
			msg_format("���͂̐F�X�Ȃ��Ƃ����������C������...");
		else
			msg_format("���͂̔g���𕪐͂����E�E");
		detect_doors(rad);
		if (p_ptr->lev > 4) detect_monsters_normal(rad);
		if (p_ptr->lev > 9) detect_traps(rad, TRUE);
		if (p_ptr->lev < 15)detect_stairs(rad);
		else map_area(rad);
		if (p_ptr->lev > 19) detect_objects_normal(rad);
		break;
	}
	case 5:
	{
		dice = p_ptr->lev / 5;
		sides = 5 + p_ptr->lev / 10;
		base = p_ptr->lev + 10;
		if (only_info) return format("����:%d+%dd%d", base, dice, sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("���Ȃ��͐��_�g��������B");
		fire_beam(GF_REDEYE, dir, base + damroll(dice, sides));
		break;
	}
	case 6:
	{
		if (only_info) return "";

		msg_format("���͂̑��݂���������g����ǂݎ�����B");
		probing();
		break;
	}
	case 7:
	{
		object_type forge;
		object_type *tmp_o_ptr = &forge;
		int ty, tx;
		if (only_info) return "";

		if (!get_aim_dir(&dir)) return NULL;
		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}
		//�_�~�[�A�C�e���𐶐������꓊�����[�`���֓n��
		msg_print("���Ȃ��͖��Ă��Ԃ񓊂����I");
		object_prep(tmp_o_ptr, lookup_kind(TV_COMPOUND, SV_COMPOUND_MEDICINE_CHEST));

		do_cmd_throw_aux2(py, px, ty, tx, 2, tmp_o_ptr, 0);

		break;
	}


	case 8: //���i�e�B�b�N�_�u��
	{
		int shoot_num = (plev -1) / 7;
		if (only_info) return format("�A�ː�:�ő�%d", shoot_num);

		if (!rapid_fire(shoot_num,0))
		{
			return NULL;
		}

	}
	break;
	case 9:
	{
		int x, y;
		int range = plev / 6 + 2;
		if (only_info) return format("����:%d",range);

		project_length = range;
		if (!get_aim_dir(&dir)) return NULL;
		x = px + range * ddx[dir];
		y = py + range * ddy[dir];
		if ((dir == 5) && target_okay())
		{
			x = target_col;
			y = target_row;
		}

		project_length = 0;
		if (!cave_player_teleportable_bold(y, x, 0L) ||
			(distance(y, x, py, px) > range) ||
			!projectable(py, px, y, x))
		{
			msg_print("�����ւ͈ړ��ł��Ȃ��B");
			return NULL;
		}
		else if (p_ptr->anti_tele)
		{
			msg_print("�s�v�c�ȗ͂��e���|�[�g��h�����I");
		}
		else
		{
			msg_print("���Ȃ��͗��ꂽ�ꏊ�Ɉ�u�Ō��ꂽ�B");
			teleport_player_to(y, x, TELEPORT_NONMAGICAL);
			new_class_power_change_energy_need = 100 - plev;
		}
		break;
	}
	case 10:
	{
		damage = p_ptr->lev * 4 + adj_general[p_ptr->stat_ind[A_WIS]] * 5 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
		if (only_info) return format("����:%d", damage);
		msg_format("���Ȃ��̎��E�͐Ԃ����܂����I");
		project_hack2(GF_REDEYE, 0, 0, damage);

		break;
	}
	case 12:
	{
		if (only_info) return format("����:3��");

		evil_undulation(TRUE, FALSE);

	}
	break;

	default:
		if (only_info) return format("");

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}




/*:::�N���E���s�[�X(�V�O������)��p�Z*/
class_power_type class_power_vfs_clownpiece[] =
{

	{ 5,5,20,FALSE,FALSE,A_CHR,0,0,"���C�̏����T",
	"�אڂ��������X�^�[��̂����C�ɖ`�����Ǝ��݂�B���C�ɖ`���ꂽ�����X�^�[�͓G�����̋�ʂȂ����͂̃����X�^�[�Ɛ킢�n�߂�B���j�[�N�����X�^�[�ɂ͌����Â炭�A�ʏ�̐��_�������Ȃ������X�^�[�ɂ͌����Ȃ��B�u���N���E���s�[�X�̏����v���������Ă��Ȃ��Ǝg���Ȃ��B" },

	{ 16,20,30,FALSE,TRUE,A_DEX,0,3,"�w���G�N���v�X",
	"�����镔���𖾂邭���A���E���̓G������������B���x�����オ��ƞN�O�Ƌ��|�Ƌ���m�����ǉ������B�u���N���E���s�[�X�̏����v���������Ă��Ȃ��Ǝg���Ȃ��B" },

	{ 24,72,45,TRUE,TRUE,A_CON,0,30,"�O���C�Y�C���t�F���m",
	"�����𒆐S�ɉΉ������̃{�[���𔭐�������B�З͂�HP��1/3�ɂȂ�B���x��40�ȍ~�͎��E���U���ɂȂ�B" },

	{ 33,30,70,FALSE,TRUE,A_WIS,0,0,"�X�g���C�v�h�A�r�X",
	"�n���̍��Α����̃r�[������B" },

	{ 40,45,80,FALSE,TRUE,A_CHR,0,0,"���C�̏����U",
	"���E���̃����X�^�[�S�Ă����C�ɖ`�����Ǝ��݂�B���̎d�l�́u���C�̏����v�Ɠ����B" },

	{ 46,66,85,FALSE,TRUE,A_CHR,0,0,"�t�F�C�N�A�|��",
	"���E���̃^�[�Q�b�g�̈ʒu��覐Α����̃{�[���𔭐�������B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_vfs_clownpiece(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{
		monster_type *m_ptr;
		int y, x;
		int power = plev * 2 + chr_adj * 2 + 20;

		if (use_itemcard) power += 60;

		if (only_info) return  format("����:%d", power);
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);

			if(use_itemcard)
				msg_format("⾉΂����C�̌���������I");
			else
				msg_format("���Ȃ���%s�̑O�ɏ������߂Â����c�c", m_name);

			lunatic_torch(cave[y][x].m_idx, power);
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;
	case 1:
	{
		int power = 10 + p_ptr->lev * 3;
		if (only_info) return format("����:%d", power);
		msg_format("���������邳�𑝂����B");

		lite_area(randint1(power), 3);
		confuse_monsters(power);
		if (plev > 24) stun_monsters(power);
		if (plev > 34) turn_monsters(power);
		if (plev > 44) project_hack(GF_BERSERK,power);

	}
	break;


	case 2:
	{
		int base = p_ptr->chp / 3;

		if (base < 1) base = 1;

		if (only_info) return format("����:�`%d", base);

		if (plev < 40)
		{
			msg_format("���͂ɉ΂�������B");
			project(0, 4, py, px, base * 2, GF_FIRE, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
		}
		else
		{
			msg_print("�M�g�����͂��Ă��������I");
			project_hack2(GF_FIRE, 0, 0, base);
		}

	}
	break;
	case 3:
	{
		int base = plev * 2 + chr_adj * 2 - 10;

		if (only_info) return format("����:%d+1d%d", base, base);

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("�n���̃p���[��������I");
		fire_beam(GF_HELL_FIRE, dir, base + randint1(base));
		break;
	}


	case 4:
	{
		int y, x;
		int power = plev * 2 + chr_adj * 2;

		if (only_info) return  format("����:%d", power);

		msg_format("�n���̏��������X�ƋP�����I");
		project_hack2(GF_LUNATIC_TORCH, 0, 0, power);

	}
	break;

	case 5:
	{
		int x, y;
		int dist = 7;
		int rad = 5;
		monster_type *m_ptr;
		int dice = 5 + chr_adj / 10;
		int sides = plev * 2;

		if (only_info) return format("�˒�:%d ����:%dd%d", dist, dice, sides);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("������_�����Ƃ͂ł��Ȃ��B");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_print("�����ɂ͉������Ȃ��B");
			return NULL;
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			msg_format("%s��ڊ|����覐΂��������I", m_name);
			project(0, rad, y, x, damroll(dice, sides), GF_METEOR, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		}

		break;
	}




	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}










//v1.1.45 ���T�@���s���͕��̂Ƌ��L����
class_power_type class_power_satono[] =
{

	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"�x��",
	"�z����̂̔w��ŗx��B�w��ŗx���Ă���Ԃ͂��̃����X�^�[�̈ʒu�ɏd�Ȃ�ꏏ�Ɉړ�����B�����X�^�[�̖��@�g�p�������͂ɉ����ċ�������A�����X�^�[�̍U���Ńv���C���[���o���l��A�C�e���𓾂邱�Ƃ��ł���B�������d���Ƃ��ɗx���Ă���ƃv���C���[�̎��R�񕜑��x���ቺ����B" },

	{ 10,0,20,FALSE,FALSE,A_CHR,0,0,"�X�J�E�g",
	"�אڂ��郂���X�^�[��̂�z���ɂ��悤�Ǝ��݂�B�������̓��x�����Ɩ��͂Ō��܂�B���j�[�N�����X�^�[�ɂ͂��Ȃ�����Â炭�A���ꃆ�j�[�N�����X�^�[�A�N�G�X�g�œ|�Ώۃ����X�^�[�A���_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 15,23,30,FALSE,FALSE,A_INT,0,2,"�t�H�Q�b�g���A�l�[��",
	"�אڂ��郂���X�^�[�S�Ă����m���Ńt���A����Ǖ�����B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B�n���N�G�X�g�_���W�����ł͌��ʂ��Ȃ��B" },

	{ 20,30,40,FALSE,TRUE,A_INT,0,0,"��ˋA��",
	"��˂̍���ʂ��Ēn��֋A�҂���B���̋A�҂́u�قځv�����ɔ�������BEXTRA���[�h�ł͉��̊K�փ��x���e���|�[�g���s���B" },

	{ 25,0,50,FALSE,TRUE,A_INT,0,0,"�r�n�C���h���[",
	"���Ȃ����w��ŗx���Ă��郂���X�^�[�ɖ��@���g�킹��B�u���X�⃍�P�b�g�Ȃǂ͎w��ł��Ȃ��B���@�̐������̓v���C���[�̖��͂ƃ����X�^�[�̃��x���Ō��܂�B�ǉ��R�X�g�̏���͂Ȃ��B�����E�����E�e���|�[�g�Ȃǂ̕⏕���@�̓v���C���[�ɑ΂��Ďg����B�v���C���[�ƃ����X�^�[�̗������s���������̂Ƃ݂Ȃ����B" },

	{ 30,48,60,FALSE,TRUE,A_CHR,0,0,"�e���O�I�h�V",
	"���͂̃A���f�b�h�E�����E�d���ɑ�_���[�W��^���ĞN�O�Ƃ�����B" },

	{ 34,50,70,FALSE,TRUE,A_CHR,0,0,"���X�����Y���_���X",
	"���͂̃����X�^�[�ɞN�O�A�����A�����������炷�B�z���̔w��ŗx���Ă���Ƃ��ɂ͎g���Ȃ��B" },

	{ 37,60,75,FALSE,TRUE,A_CHR,0,0,"�p���t���`�A�[�Y",
	"�ꎞ�I�ɖ��͂��啝�ɏ㏸����B�ʏ�̌��E�l�𒴂���B" },

	{ 40,50,85,FALSE,TRUE,A_INT,0,0,"��ˈړ�",
	"��˂̍���ʂ��Ďw�肵�������X�^�[�ׂ̗Ƀe���|�[�g����B���̂Ƃ��t���A�S�̂̃����X�^�[�̈ʒu�����m���邱�Ƃ��ł���B�ʏ�̔{�̍s���͂������B�e���|�[�g�s�\�Ȓn�`�ɂ͏o���Ȃ��B" },

	{ 45,75,90,FALSE,TRUE,A_CHR,0,0,"�N���C�W�[�o�b�N�_���X",
	"�������w��ŗx���Ă��郂���X�^�[���ꎞ�I�ɖ��G������B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

//v1.1.41 ��
class_power_type class_power_mai[] =
{

	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"�x��",
	"�z����̂̔w��ŗx��B�w��ŗx���Ă���Ԃ͂��̃����X�^�[�̈ʒu�ɏd�Ȃ�ꏏ�Ɉړ�����B�����X�^�[�̗אڍU���AAC�AHP�񕜑��x�����͂ɉ����ċ�������A�����X�^�[�̍U���Ńv���C���[���o���l��A�C�e���𓾂邱�Ƃ��ł���B�������d���Ƃ��ɗx���Ă���ƃv���C���[�̎��R�񕜑��x���ቺ����B" },

	{ 10,0,20,FALSE,FALSE,A_CHR,0,0,"�X�J�E�g",
	"�אڂ��郂���X�^�[��̂�z���ɂ��悤�Ǝ��݂�B�������̓��x�����Ɩ��͂Ō��܂�B���j�[�N�����X�^�[�ɂ͂��Ȃ�����Â炭�A���ꃆ�j�[�N�����X�^�[�A�N�G�X�g�œ|�Ώۃ����X�^�[�A���_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B" },

	{ 15,5,30,TRUE,FALSE,A_DEX,0,3,"�o���u�[�X�s�A�_���X",
	"�ˌ������̃r�[������B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B�З͂͑������̑��ɂ���ĕϓ�����B" },

	{ 20,30,40,FALSE,TRUE,A_INT,0,0,"��ˋA��",
	"��˂̍���ʂ��Ēn��֋A�҂���B���̋A�҂́u�قځv�����ɔ�������BEXTRA���[�h�ł͉��̊K�փ��x���e���|�[�g���s���B" },

	{ 25,30,50,FALSE,TRUE,A_CHR,0,0,"�^�i�o�^�X�^�[�t�F�X�e�B�o��",
	"�������g�Ǝ������w��ŗx���Ă��郂���X�^�[�̗̑͂��񕜂��X�e�[�^�X�ُ�����Â���B" },

	{ 30,48,60,FALSE,TRUE,A_CHR,0,0,"�e���O�I�h�V",
	"���͂̃A���f�b�h�E�����E�d���ɑ�_���[�W��^���ĞN�O�Ƃ�����B" },

	{ 34,50,70,FALSE,TRUE,A_DEX,0,0,"�o���u�[���r�����X",
	"���͂ɐX�n�`�𐶐�����B�z���̔w��ŗx���Ă���Ƃ��ɂ͎g���Ȃ��B" },

	{ 37,60,75,FALSE,TRUE,A_CHR,0,0,"�p���t���`�A�[�Y",
	"�ꎞ�I�ɖ��͂��啝�ɏ㏸����B�ʏ�̌��E�l�𒴂���B" },

	{ 40,50,85,FALSE,TRUE,A_INT,0,0,"��ˈړ�",
	"��˂̍���ʂ��Ďw�肵�������X�^�[�ׂ̗Ƀe���|�[�g����B���̂Ƃ��t���A�S�̂̃����X�^�[�̈ʒu�����m���邱�Ƃ��ł���B�ʏ�̔{�̍s���͂������B�e���|�[�g�s�\�Ȓn�`�ɂ͏o���Ȃ��B" },

	{ 45,75,90,FALSE,TRUE,A_CHR,0,0,"�N���C�W�[�o�b�N�_���X",
	"�������w��ŗx���Ă��郂���X�^�[���ꎞ�I�ɖ��G������B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_mai(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{
		int chr = p_ptr->stat_ind[A_CHR] + 3;
		int mult = 100 + MAX(0, 100 * (chr - 5) / 15);
		if (only_info)
		{
			if(p_ptr->pclass == CLASS_MAI)
				return format("�ߐڋ���:x%d.%02d", mult / 100, mult % 100);
			else
				return format("���@�g�p��:+%d", chr);
		}

		if(!do_riding(FALSE)) return NULL;


	}
	break;
	case 1://�X�J�E�g
	{
		int y, x;
		int power = 20 + plev + chr_adj;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format("����:%d", power);

		if (p_ptr->inside_arena)
		{
			msg_print("�����̓��Z�͎g���Ȃ��B");
			return NULL;
		}


		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

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

			if (one_in_(64))
			{
				msg_format("���Ȃ��́A���邮��Ƃ��ǂ����B");
				power *= 3;
			}
			else if (one_in_(4))
			{
				if(p_ptr->pclass == CLASS_MAI)
					msg_format("�u�N�̂悤�ȃA�O���b�V�u�Ȑl�ނ�T���Ă����񂾁I�v");
				else 
					msg_format("�u�悤������˂̍��ցB�v");
			}
			else
				msg_format("���Ȃ���%s����˂̍��փX�J�E�g�����B", m_name);

			if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				msg_format("%s�͖��������B", m_name);
			}
			else if (is_pet(m_ptr))
			{
				msg_format("%s�͂��łɃX�J�E�g�ς݂��B", m_name);
			}
			else if (r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags1 & (RF1_QUESTOR) || r_ptr->flags7 & RF7_VARIABLE || power < r_ptr->level)
			{
				msg_format("%s�͂��Ȃ��̗U���Ɏ���݂��Ȃ��I", m_name);
			}
			//���j�[�N�����X�^�[�𒇊Ԃɂ���ɂ�weird_luck�v
			else if (randint1(power) < r_ptr->level || (r_ptr->flags1 & RF1_UNIQUE) && !weird_luck())
			{
				msg_format("%s�͂��Ȃ��̗U���ɏ��Ȃ������B", m_name);
			}
			else
			{
				msg_format("%s�͂��Ȃ��ɏ]�����B", m_name);
				set_pet(m_ptr);
			}
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;
	case 2:
	if(!use_itemcard && p_ptr->pclass == CLASS_MAI)//�o���u�[�X�s�A�_���X
	{
		int damage = 0;
		int range = 2 + plev / 15;

		if(inventory[INVEN_RARM].tval == TV_SPEAR)
			damage = calc_weapon_dam(0) / p_ptr->num_blow[0];
		else if (inventory[INVEN_LARM].tval == TV_SPEAR)
			damage = calc_weapon_dam(1) / p_ptr->num_blow[1];

		if (only_info) return format("�˒�:%d ����:%d", range, damage);

		if (damage < 1)
		{
			msg_print("���𑕔����Ă��Ȃ��Ƃ��̋Z���g�����Ƃ͂ł��Ȃ��B");
			return NULL;
		}

		project_length = range;
		if (!get_aim_dir(&dir)) return NULL;
		//msg_print("�˂����������I");
		fire_beam(GF_ARROW, dir, damage);
	}
	else//�t�H�Q�b�g���A�l�[���A�������̓A�C�e���J�[�h��䪉�
	{
		int power = 100 + plev * 4 + chr_adj * 10;
		if (only_info) return format("����:%d",power/2);

		msg_print("���Ȃ���䪉ׂ�h�炵�ėx����...");
		project(0, 1, py, px, power, GF_GENOCIDE, PROJECT_HIDE | PROJECT_JUMP | PROJECT_KILL, -1);

	}
	break;
	case 3:
	{
		if (only_info) return "";
		int m_idx;

		msg_print("��˂̍��ւ̔����J�����B");

		if (EXTRA_MODE)
		{
			teleport_level(0);
		}
		else
		{
			recall_player(1);
		}
	}
	break;
	case 4:
	if(use_itemcard || p_ptr->pclass == CLASS_MAI) //���̃^�i�o�^�X�^�[�A���邢�̓A�C�e���J�[�h�̍�
	{
		int heal = 50 + chr_adj * 5;
		int heal_percen = 10 + chr_adj / 5;

		if (use_itemcard && heal < 100) heal = 100;

		if (only_info) return format("��:%d/%d%%",heal,heal_percen);

		msg_print("���_�̗͂����Ȃ�����₷...");

		hp_player(heal);
		set_stun(0);
		set_cut(0);
		set_poisoned(0);
		set_image(0);

		if (p_ptr->riding)
		{
			monster_type *m_ptr = &m_list[p_ptr->riding];
			int heal2;
			char m_name[80];

			set_monster_stunned(p_ptr->riding, 0);
			set_monster_confused(p_ptr->riding, 0);
			set_monster_slow(p_ptr->riding, 0);
			if (m_ptr->hp < m_ptr->maxhp)
			{
				monster_desc(m_name, m_ptr, 0);
				heal2 = m_ptr->maxhp * heal_percen / 100;
				if (!heal2) heal2 = 1;
				m_ptr->hp += heal2;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				msg_format("%^s�̏��������������B", m_name);
				p_ptr->redraw |= (PR_HEALTH);
				p_ptr->redraw |= (PR_UHEALTH);
			}
		}

	}
	else //���T�̃r�n�C���h���[
	{
		if (only_info) return format("�@");

		if (!p_ptr->riding)
		{
			msg_print("���͗x���Ă��Ȃ��B");
			return NULL;
		}

		if (!cast_monspell_new()) return NULL;

	}
	break;
	case 5:
	{
		int dam = plev * 3 + chr_adj * 3;
		if (only_info) return format("����:%d", dam);

		msg_print("���Ȃ��͋����������˗x�����I");
		dispel_undead(dam);
		dispel_demons(dam);
		dispel_kwai(dam);

	}
	break;

	case 6:
	{
		if (p_ptr->pclass == CLASS_MAI)
		{
			if (only_info) return "";

			msg_print("���Ȃ��͒n�ʂ𓥂ݖ炵���B�|�����X�ɐ����Ă��Đ��_�ɂȂ����I");

			tree_creation();
		}
		else
		{
			int power = plev + chr_adj * 5;
			if (only_info) return format("����:%d", power);

			msg_print("���Ȃ��͌��f�I�ȗx����I�����B");
			confuse_monsters(power);
			stun_monsters(power);
			charm_monsters(power);

		}



	}
	break;

	case 7:
	{
		int v;
		bool base = 10;
		if (only_info) return format("����:%d+1d%d", base, base);

		v = base + randint1(base);

		msg_print("���Ȃ��̗x��͌������𑝂����I");
		set_tim_addstat(A_CHR, 100 + plev / 5, v, FALSE);

		break;
	}

	case 8:
	{
		if (only_info) return format("����:�Ȃ�");

		msg_print("���Ȃ��͌�˂̍���`������...");
		detect_monsters_normal(255);

		if (p_ptr->paralyzed || p_ptr->confused || p_ptr->image)
		{
			msg_print("���Ȃ��͈ړ��Ɏ��s�����I");
		}
		else
		{
			//�e���|�[�g�ɐ���������TRUE���A���čs���͓�{
			if (dimension_door(D_DOOR_BACKDOOR))
				new_class_power_change_energy_need = 200;
		}



	}
	break;


	case 9:
	{
		int v;
		bool base = 6;
		if (only_info) return format("����:%d+1d%d", base, base);
		v = base + randint1(base);

		if (p_ptr->riding)
		{
			char m_name[80];
			monster_desc(m_name, &m_list[p_ptr->riding], 0);
			msg_format("%s�͐��ݔ\�͂��J������Ė��G�ɂȂ����I",m_name);
			set_monster_invulner(p_ptr->riding, v, FALSE);
		}

	}
	break;

	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}





//v1.1.39 �����o
class_power_type class_power_larva[] =
{
	{6,7,10,FALSE,FALSE,A_CON,0,3,"���L�U��",
	"�����𒆐S�ɓő����̃{�[���𔭐�������B�̗͂������ȉ��ɂȂ�ƈЗ͂��啝�ɏオ��A����Ɏ��͂̓G�������E�N�O�E���|������B"},
	{10,10,20,FALSE,FALSE,A_DEX,0,2,"�Z�����ؕ�",
	"�����𒆐S�ɒx�ݑ����̃{�[���𔭐�������B"},
	{15, 0,20,TRUE,FALSE,A_WIS,0,0,"�Đ�",
	"HP�Ə�Ԉُ���킸���ɉ񕜂���B�����x��������������B" },
	{ 24,24,30,TRUE,FALSE,A_DEX,0,10,"�~�j�b�g�X�P�[���X",
	"�����𒆐S�ɍ��������̃{�[���𔭐�������B" },
	{ 30,20,50,FALSE,TRUE,A_CON,0,0,"�ψً��͔���",
	"�ψقɂ�郌�C�V�����p���[���{�̈З͂Ŕ�������B" },
	{ 33,30,60,FALSE,TRUE,A_DEX,0,0,"�A�Q�n�̗ؕ�",
	"�����𒆐S�ɃJ�I�X�����̃{�[���𔭐�������B" },
	{ 36,50,50,FALSE,TRUE,A_INT,0,0,"�o�^�t���C�h���[��",
	"��莞�Ԍo�ߌ�Ɍ��݂̃t���A���č\������B" },
	{ 40,150,90,FALSE,TRUE,A_CON,0,0,"�ψ�",
	"�����_���ȓˑR�ψق𓾂�B" },
	{ 44,40,75,FALSE,TRUE,A_DEX,0,0,"�f�b�h���[�o�^�t���C",
	"�����𒆐S�ɒn�������Ɩh��͒ቺ�����̃{�[���𔭐�������B" },
	{ 48,64,80,FALSE,TRUE,A_CHR,0,0,"�^�ẲH�΂���",
	"�ꎞ�I�Ɏ푰���ς��啝�Ƀp���[�A�b�v����B" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_larva(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{
		bool flag = FALSE;
		int power = 5 + plev/2;

		if (p_ptr->chp <= p_ptr->mhp / 2)
		{
			power *= 4;
			flag = TRUE;
		}

		if (only_info) return format("����:�`%d", power); 

		if (flag)
		{
			msg_print("���Ȃ��͕@���Ȃ��肻���Ȉ��L��������I");
			project(0, 5, py, px, power*2, GF_POIS, (PROJECT_KILL | PROJECT_JUMP), -1);
			confuse_monsters(power);
			stun_monsters(power);
			turn_monsters(power);
		}
		else
		{
			msg_print("���Ȃ��͓��̏L�p���猙�ȓ������o����...");
			project(0, 3, py, px, power*2, GF_POIS, (PROJECT_KILL | PROJECT_JUMP), -1);
		}

	}
	break;
	case 1:
	{
		int dam = 10 + plev + chr_adj ;
		int rad = 1 + plev / 30;
		if (only_info) return format("����:�`%d", dam / 2);

		msg_print("�ؕ����܂��U�炵���B");
		project(0, rad, py, px, dam, GF_INACT, (PROJECT_KILL | PROJECT_JUMP), -1);

	}
	break;

	case 2://�Đ�
	{
		int base = p_ptr->lev / 5;
		if (only_info) return format("��:%d+1d%d", base, base);

		if (p_ptr->food < PY_FOOD_WEAK)
		{
			msg_print("�������󂢂ďW���ł��Ȃ��B");
			return NULL;
		}

		msg_print("���Ȃ��͎����̑̂Ɉӎ����W������..");
		hp_player(base + randint1(base));
		set_poisoned(p_ptr->poisoned - base);
		set_stun(p_ptr->stun - base);
		set_cut(p_ptr->cut - base);
		set_image(p_ptr->image - base);
		set_food(p_ptr->food - 50);

	}
	break;
	case 3://�~�j�b�g�X�P�[���X
	{
		int dam = plev * 2 + chr_adj * 2;
		int rad = plev / 15;
		if (only_info) return format("����:�`%d", dam / 2);

		msg_print("�ؕ����܂��U�炵���B");
		project(0, rad, py, px, dam, GF_CONFUSION, (PROJECT_KILL | PROJECT_JUMP), -1);

	}
	break;

	case 4://�ψً��͔���
	{
		if (only_info) return "";

		msg_print("���Ȃ��͗͋����I�[�����܂Ƃ���..");

		hack_flag_powerup_mutation = TRUE;
		do_cmd_racial_power(FALSE);
		hack_flag_powerup_mutation = FALSE;

		//-hack- ���C�V�����������L�����Z���������ǂ������s���͏���Ŕ���
		if (!energy_use) return NULL;

	}
	break;

	case 5://�A�Q�n�̗ؕ�
	{
		int dam = plev * 4 + chr_adj * 4;
		int rad = 1 + plev / 15;
		if (only_info) return format("����:�`%d", dam / 2);

		msg_print("�d�����P���ؕ����܂��U�炵��...");
		project(0, rad, py, px, dam, GF_CHAOS, (PROJECT_KILL | PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM), -1);

	}
	break;

	case 6:
	{
		if (only_info) return "";

		if (p_ptr->inside_arena)
		{
			msg_print("�����̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		msg_print("���Ȃ��͎����̑��݂ɋ^���������...");

		alter_reality();
	}
	break;


	case 7:
	{
		if (only_info) return "";

		msg_print("���Ȃ��͑̂��ۂ߂Ėڂ����...");
		gain_random_mutation(0);

	}
	break;


	case 8://�f�b�h���[�o�^�t���C
	{
		int dam = plev * 5 + chr_adj * 5;
		int rad = 2 + plev / 15;
		if (only_info) return format("����:�`%d", dam / 2);

		msg_print("�ЁX�����P���ؕ����܂��U�炵��...");
		project(0, rad, py, px, dam, GF_NETHER, (PROJECT_KILL | PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM), -1);
		project(0, rad, py, px, dam, GF_DEC_DEF, (PROJECT_KILL | PROJECT_JUMP | PROJECT_GRID | PROJECT_HIDE), -1);

	}
	break;

	case 9:
	{
		int base = 7;
		int time;
		if (only_info) return format("����:%d+1d%d", base, base);

		time = randint1(base) + base;

		set_mimic(time, MIMIC_GOD_OF_NEW_WORLD, FALSE);
		set_oppose_fire(time, FALSE);
		set_hero(time, FALSE);
	}
	break;

	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.37 �׋S�ɏ�����ꂽ�Ƃ��̏���
class_power_type class_power_kosuzu_hyakki[] =
{

	{10,10,30,FALSE,TRUE,A_WIS,0,0,"�S�S��s",
		"�����̕t�r�_����������B"},
	{20,20,40,FALSE,TRUE,A_INT,0,0,"�S�S�̗�",
		"���͂̕t�r�_(��)(��)(��)���z�����AHP��啝�ɉ񕜂���B"},
	{30,20,50,FALSE,TRUE,A_STR,0,0,"�t�r�_�u���X",
		"����HP��1/6�̈З͂̔j�Б����̃u���X��f���B"},





	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_kosuzu_hyakki(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0: //�t�r�_����
		{
			int i;
			bool flag = FALSE;
			int base = plev / 10;
			if (only_info) return "";

			i = base + randint1(base);

			for(;i>0;i--)
			{
				if(summon_specific(0,py,px,(dun_level/2+plev/2),SUMMON_TSUKUMO,(PM_ALLOW_GROUP | PM_FORCE_FRIENDLY))) flag = TRUE;
			}
			if(flag)
				msg_print("�ǂ�����Ƃ��Ȃ��t�r�_���������ꂽ�B");
			else
				msg_print("���ɉ����N����Ȃ������B");

		}
		break;
	case 1://�t�r�_�z��
		{
			if(only_info) return "";

			msg_print("�e�̋S��������J���Ėi�����I");
			absorb_tsukumo(0);
		}
		break;
	case 2:
		{
			int dam = p_ptr->chp / 6;
			if(dam<1) dam = 1;
			if(dam > 600) dam=600;
			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;

			msg_print("�t�r�_�����𐁂������I");

			fire_ball(GF_SHARDS, dir, dam, -2);
			break;
		}



	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

//v1.1.37 ����ɏ�����ꂽ�Ƃ��̏���
class_power_type class_power_kosuzu_ghost[] =
{

	{10,20,30,FALSE,TRUE,A_CHR,0,0,"����",
		"�אڂ��������X�^�[��̂𖣗����Ĕz���ɂ��悤�Ǝ��݂�B���ɒj���▰���Ă��郂���X�^�[�Ɍ��ʂ������B"},
	{20,10,40,FALSE,TRUE,A_INT,0,0,"�u�Ԉړ�",
		"���������e���|�[�g����B"},
	{30,20,50,FALSE,TRUE,A_CHR,0,0,"�����͋z��",
		"�אڂ���������̂���HP���z�������𖞂����B"},
	{35,30,60,FALSE,TRUE,A_DEX,0,0,"�ς��g",
		"�U�����󂯂��Ƃ��������e���|�[�g�œ����悤�ɂȂ�B���s���邱�Ƃ�����B"},
	{40,50,70,FALSE,TRUE,A_DEX,0,0,"�ǔ���",
		"��莞�ԁA�ǂ𔲂�����悤�ɂȂ�B"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_kosuzu_ghost(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0: //����
		{
			int y, x;
			int power = 10 + plev + chr_adj * 2;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (only_info) return format("����:%d",power);

			if (p_ptr->inside_arena)
			{
				msg_print("�����̓��Z�͎g���Ȃ��B");
				return NULL;
			}

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flags1 & RF1_MALE) power *= 2;
				if(MON_CSLEEP(m_ptr)) power *= 2;
				if(r_ptr->flags3 & RF3_NO_CONF) power /= 2;

				msg_format("���Ȃ���%s�Ɍ����Ĕ��΂�Ŏ菵������...",m_name);

				if(r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & RF7_UNIQUE2)
				{
					msg_format("%s�ɂ͌��ʂ��Ȃ��I",m_name);
				}
				else if(randint1(power) < r_ptr->level) 
				{
					msg_format("%s�͒�R�����B",m_name);
				}
				else
				{
					msg_format("%s�͂��Ȃ��ɏ]�����B",m_name);
					set_pet(m_ptr);
				}
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
		}
		break;
	case 1://�e���|�[�g
		{
			int dist = 25 + plev / 2;
			if(only_info) return format("����:%d",dist);
			msg_print("���Ȃ��͏u���ɏ������B");
			teleport_player(dist, 0L);
		}
		break;

	case 2: //�����͋z��
		{
			int y, x;
			int dam = plev + chr_adj * 5;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (only_info) return format("����:%d",dam);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int food = 0;
				char m_name[80];

				r_ptr = &r_info[m_ptr->r_idx];

				monster_desc(m_name, m_ptr, 0);
				if(!monster_living(r_ptr))
				{
					msg_format("%s����͐����͂�D���Ȃ������B",m_name);
				}
				else
				{
					msg_format("���Ȃ���%s�֐G��A�����͂��z�������I",m_name);

					if(m_ptr->hp < dam) dam = m_ptr->hp+1;

					if(p_ptr->food < PY_FOOD_MAX) set_food(MIN(PY_FOOD_MAX - 1, (p_ptr->food + dam * 10)));
					hp_player(dam);
					project(0,0,y,x,dam,GF_DISP_ALL,PROJECT_KILL,0);
				}
				touch_zap_player(m_ptr);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
		}
		break;
	case 3:
		{
			if(only_info) return "";
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("���Ȃ��̎��͂ɗ����������n�߂�...");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
			}
			break;
		}
	case 4:
		{
			int base = 20;
			if(only_info) return format("����:%d+1d%d",base,base);


			set_kabenuke(base + randint1(base), FALSE);

			break;
		}
	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


//v1.1.37 ����
class_power_type class_power_kosuzu[] =
{

	{1,0,0,FALSE,FALSE,A_INT,0,0,"�����Ӓ�",
		"�������Ӓ肷��B"},
	{7,0,25,FALSE,TRUE,A_INT,0,0,"�������ӉZ��",
		"�����x���񕜂���B"},
	{14,10,50,FALSE,TRUE,A_INT,0,0,"���ҕs���̈Տ�",
		"�t���A�̕��͋C�����m����B���x��20�ŋ߂��̃g���b�v�A���x��30�ŋ߂��̃����X�^�[�����m����B"},
	{21,0,50,FALSE,TRUE,A_INT,0,0,"���̕S�S�E��B�c�ʖ{",
		"���̃t���A����Ń����X�^�[�u���X���v��z���Ƃ��ď�������B"},
	{28,0,0,FALSE,TRUE,A_INT,0,0,"����̉���",
		"����Ɏ��߂����B���߂���Ă���Ԃ͎푰����Z���ω�����B"},
	{35,20,60,FALSE,TRUE,A_CHR,0,0,"�ϕ����̃m�[�g",
		"���̃t���A����Ń����X�^�[�u�d���ρv��z���Ƃ��Ĉ�̏�������B"},
	{42,80,70,FALSE,TRUE,A_INT,0,0,"���ƔŕS�S��s�G���ŏI�͕��",
		"�G�����̎׋S�Ɏ��߂����B���߂���Ă���Ԃ͎푰����Z���ω�����B�����ƔŕS�S��s�G���ŏI�͕��𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_kosuzu(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0: //�����Ӓ�
		{
			if (only_info) return format("");
			if (!ident_spell_2(4)) return NULL;
		}
		break;

	case 1: //
		{
			if(only_info) return "";
			if(p_ptr->food >= PY_FOOD_FULL && p_ptr->pclass != CLASS_YUMA)
			{
				msg_print("���͖������B");
				return NULL;
			}

			msg_print("�ǂ�����Ƃ��Ȃ��u��i��؃R���b�P�v���o�������I");
			set_food(PY_FOOD_MAX - 1);

			break;
		}
	case 2:
		{
			int rad = DETECT_RAD_DEFAULT;
			if(only_info) return format("�͈�:%d",rad);

			msg_print("���Ȃ���⬒|�����o���A�L���𗊂�ɌT�𗧂Ďn�߂�...");
			p_ptr->feeling_turn = 0;

			if(plev > 19) detect_traps(rad,TRUE);
			if(plev > 29) detect_monsters_normal(rad);
			if(plev > 29) detect_monsters_invis(rad);

			break;
		}
	case 3:
		{
			int max_num = (plev-10) / 10;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("�ő�:%d",max_num);
	
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_ENENRA) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("�����{�͋󔒂��B",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_ENENRA, PM_EPHEMERA))
			{
				msg_print("�{�̒�����N�X�Ɖ����N���o����...");

			}

		}
		break;

	case 4:
		{
			int base = 1000;
			if(only_info) return format("����:%d+1d%d",base,base);

			set_mimic(base + randint1(base), MIMIC_KOSUZU_GHOST, FALSE);

			break;
		}
	case 5:
		{
			int max_num = 1;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("�ő�:%d",max_num);
	
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				//�J�����I���t���O�ƈꎞ�I�t���O���������X�^�[�ŏ��ς𔻒�B�����A�C�e���J�[�h�g���Α��̃��[�g�ł����̃����X�^�[���o�邩������Ȃ����܂����ߏ���͂Ȃ����낤
				if( (m_ptr->mflag2 & MFLAG2_CHAMELEON) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("�������ς��Ă�ł���B",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_CHAMELEON, PM_EPHEMERA))
			{
				msg_print("����݂̏��ς��ϐg���Ă��Ȃ��������ɗ����I");
			}

		}
		break;

	case 6:
		{
			int base = 100;
			if(only_info) return format("����:%d+1d%d",base,base);

			set_mimic(base + randint1(base), MIMIC_KOSUZU_HYAKKI, FALSE);

			break;
		}




	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


//�����u���R�Ȃ��D��I�ȁv�̂Ƃ��̓��Z�e�[�u��
class_power_type class_power_narumi2[] =
{

	{7,3,20,FALSE,TRUE,A_WIS,0,0,"�������m",
		"�L�͈͂̐������������X�^�[�����m����B"},

	{15,5,25,FALSE,TRUE,A_INT,0,0,"���͊��m",
		"�߂��̖��@�����������A�C�e��(������A�㎿�ȏ�̑����i�Ȃ�)�����m����B"},

	{20,12,45,FALSE,TRUE,A_INT,0,4,"�������샿",
		"�������������X�^�[��̂Ƀ_���[�W��^���A�_���[�W�̈ꕔ��MP�Ƃ��ċz������B"},

	{25,20,40,FALSE,TRUE,A_WIS,0,6,"���ȕ��",
		"���͂ɔj�ב����U�����s���A�N�O�A���|�����悤�Ǝ��݂�B��R�����Ɩ����B�j�׎�_�łȂ������X�^�[�ɂ͌��ʂ��Ȃ��B"},

	{30,40,50,FALSE,TRUE,A_WIS,0,0,"���������",
		"�������������X�^�[��̗̂̑͂𔼕��ɂ���B�d���̏ꍇ�ꌂ�œ|���B��R�����Ɩ����B"},

	{ 33,33,40,TRUE,FALSE,A_STR,30,10,"��F�X�g���v",
		"�߂��̎w�肵���O���b�h�ɗ������A�����Ƀ����X�^�[������΃_���[�W��^����B�З͎͂�����AC�Ɉˑ�����B" },

	{37,35,60,FALSE,TRUE,A_INT,0,10,"�y�b�g�̋���e������",
		"�߂��̃����_���ȃ����X�^�[�ɑ΂��A����ȋC�����̃{�[���𐔉���B"},

	{40,72,75,FALSE,TRUE,A_WIS,0,0,"���������",
		"���E���̐������������X�^�[�ɑ�_���[�W��^����B"},

	{45,100,90,FALSE,TRUE,A_CHR,0,0,"�Ɖ΋~��",
		"�^�[�Q�b�g�̈ʒu�ɋ��͂Ȕj�ב����̃{�[���𔭐�������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_narumi2(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			int rad = 25 + plev / 2;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_print("���Ȃ��͂��̏�ɘȂ�Ő����̋C�z��T����..");
			detect_monsters_living(rad);

			break;
		}
	case 1:
		{
			int rad = DETECT_RAD_DEFAULT;
			if(only_info) return format("�͈�:%d",rad);
	
			msg_print("���Ȃ��͎��̖͂��͂�T����..");
			detect_objects_magic(rad);
			break;
		}
	case 2:
		{
			int y, x;
			int dam = plev * 3;
			int gain_mana;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
	
			/* v1.1.36 fire_ball_jump�̖߂�l��project()�̖߂�l�ł͂Ȃ��A���ۂɂ�����炸MP���񕜂��Ă����̂ŏC��
			if(fire_ball_jump(GF_OLD_DRAIN,dir,dam,0,"���Ȃ��͓G���琶���͂��z����낤�Ǝ��݂�.."))
			{
				if(player_gain_mana(dam/3))
					msg_print("���Ȃ��̖��͂��񕜂����I");
			}
			*/
			x = target_col;
			y = target_row;

			if (dir != 5 || !target_okay() || !projectable(y, x, py, px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			if (!cave[y][x].m_idx)
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}


			//GF_OLD_DRAIN�͌��ʂ��Ȃ������Ƃ�obvious=FALSE�ƂȂ�project_hook��FALSE��Ԃ�
			if (project_hook(GF_OLD_DRAIN, dir, dam, (PROJECT_KILL | PROJECT_HIDE | PROJECT_JUMP)))
			{
				if (player_gain_mana(dam/3))
					msg_print("���Ȃ��̖��͂��񕜂����I");

			}


			break;
		}
	case 3:
		{
			int dam = 160 + plev * 4 + chr_adj * 3; 

			if(only_info) return format("����:�ő�%d",dam);
			msg_print("���Ȃ��͔O���������n�߂�...");
			project(0,6,py,px,dam,GF_PUNISH_4,(PROJECT_KILL|PROJECT_JUMP|PROJECT_HIDE),-1);

			break;
		}

	case 4:
		{
			int y, x, base;
			int m_idx;
			monster_type *m_ptr;
			monster_race *r_ptr;
			char m_name[80];
			int dam=0;

			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;

			x = target_col;
			y = target_row;

			if(dir != 5 || !target_okay() || !projectable(y,x,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			m_idx = cave[y][x].m_idx;
			if(!m_idx)
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			m_ptr = &m_list[m_idx];
			r_ptr = &r_info[m_ptr->r_idx];

			monster_desc(m_name, m_ptr, 0);
			msg_format("���Ȃ���%s��{�C�Ŗ��͉����悤�Ƃ���..",m_name);

			if(r_ptr->flagsr & RFR_RES_ALL)
			{
				msg_format("%s�ɂ͊��S�ȑϐ�������I",m_name);
				break;
			}

			//�d���ɂ͖������Ō���
			if(r_ptr->flags3 & RF3_FAIRY)
			{

				dam = m_ptr->hp +1;
			}
			//���j�[�N�Ɩ������ɂ͌����Ȃ�
			else if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2 || !monster_living(r_ptr))
			{
				;
			}
			//�j�ł̎�Ɠ�������ɂ��Ă���
			else if(((plev*2 ) + randint1(plev*2)) > (r_ptr->level  + randint1(120)))
			{
				dam = m_ptr->hp / 2 + 1;
			}

			if(dam)
			{
				project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_MISSILE,(PROJECT_JUMP|PROJECT_KILL),-1);
			}
			else
			{
				msg_format("%s�͒�R�����I",m_name);
				//�N����
				set_monster_csleep(m_idx, 0);
				anger_monster(m_ptr);
			}

			break;
		}

	case 5:

		{
			int range = 4 + plev / 40;
			int base = p_ptr->ac + p_ptr->to_a;
			int tx, ty, dam;

			if (base < 0) base = 0;

			if (only_info) return format("����:%d+1d%d", base / 2, base / 2);

			if (base < 1)
			{
				msg_print("���̂��Ȃ��̏_�炩���ł͒���C�ׂ��Ȃ����낤�B");
				return NULL;
			}

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			dam = base / 2 + randint1(base / 2);
			ty = target_row;
			tx = target_col;
			if (!fire_ball_jump(GF_ARROW, dir, dam, 0, "���Ȃ��͔�яオ��A�W�I�ڊ|���ė��������I")) return NULL;
			teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
			//���肪�݂�����G�̂Ƃ��ǂ��Ȃ�̂��`�F�b�N�v
			break;
		}


	case 6:
		{
			bool flag = FALSE;
			int i;
			int dice = 4 + plev / 6;
			int sides = 11 + chr_adj/ 6;
			int num = (plev+12) / 10;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			msg_print("����Ȓe���̃S�[�������ӂ�̓G���U�������I");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_FORCE, damroll(dice,sides),4, 2,3)) flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������߂��ɓG�����Ȃ������B");
					break;
				}
			}

		}
		break;

	case 7:
		{
			int base = (plev * 5 + chr_adj * 5) / 2;
			if(only_info) return format("����:%d+1d%d",base,base);

			msg_print("���Ȃ��͎��d�����ɔ\�͂��J�������I");
			dispel_living(base+randint1(base));

		}
		break;
	case 8:
		{

			int base = plev * 6;
			int sides = chr_adj * 12;
			if(only_info) return format("����:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			fire_ball_jump(GF_HOLY_FIRE,dir,base+randint1(sides),5,(randint0(4)?"���Ȃ��͗L��]��͂̑S�Ă��Ԃ����I":"�u�c�O���O�A�܂������[�I�v"));

		}
		break;

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




class_power_type class_power_narumi[] =
{

	{7,3,20,FALSE,TRUE,A_WIS,0,0,"�������m",
		"�L�͈͂̐������������X�^�[�����m����B"},

	{15,5,25,FALSE,TRUE,A_INT,0,0,"���͊��m",
		"�߂��̖��@�����������A�C�e��(������A�㎿�ȏ�̑����i�Ȃ�)�����m����B"},

	{20,4,45,TRUE,TRUE,A_INT,0,2,"��������T",
		"HP����������ē��ʂ�MP���񕜂���B"},

	{25,20,40,FALSE,TRUE,A_WIS,0,0,"�C���X�^���g�{�[�f�B",
		"�ꎞ�I�ɒm�\�ƌ������㏸����B���x��40�ȍ~�͏j���̌��ʂ��ǉ������B"},

	{30,24,50,FALSE,TRUE,A_WIS,0,0,"��������U",
		"���͂̐����������A�������Ԃɂ��悤�Ǝ��݂�B�d���ɂ͓��Ɍ����₷���B"},

	{33,33,40,TRUE,TRUE,A_CHR,0,10,"�����̒n��",
		"���͂̃����X�^�[�̗̑͂��񕜂���Ԉُ�����Â���B�����ɋ߂��قǉ񕜗ʂ������B���ʂ͖��͂ɑ傫���ˑ�����B" },

	{37,30,60,FALSE,TRUE,A_INT,0,0,"�o���b�g�S�[����",
		"�����X�^�[�u�o���b�g�S�[�����v�����̃t���A����ŌĂяo���B"},

	{40,50,75,FALSE,TRUE,A_WIS,0,0,"��������V",
		"������HP�ƌo���l��S�񕜂���B"},

	{45,80,85,FALSE,TRUE,A_CHR,0,0,"�N���~�i���T�����F�C�V����",
		"��x�����n���̍��Α����̍U���𖳌�������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_narumi(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			int rad = 25 + plev / 2;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_print("���Ȃ��͂��̏�ɘȂ�Ő����̋C�z��T����..");
			detect_monsters_living(rad);

			break;
		}
	case 1:
		{
			int rad = DETECT_RAD_DEFAULT;
			if(only_info) return format("�͈�:%d",rad);
	
			msg_print("���Ȃ��͎��̖͂��͂�T����..");
			detect_objects_magic(rad);
			break;
		}
	case 2:
		{
			int mana = plev / 5;
			if(only_info) return format("��:%d",mana);
			msg_print("���Ȃ��͐����͂𖂗͂ɕϊ������B");
			player_gain_mana(mana);
			break;
		}
	case 3:
		{
			int v;
			bool base = 20;
			if(only_info) return format("����:%d+1d%d",base,base);

			v = base + randint1(base);

			msg_print("���Ȃ����Җڍ�������..");
			set_tim_addstat(A_INT,102+plev/40,v,FALSE);
			set_tim_addstat(A_WIS,102+plev/40,v,FALSE);
			if(plev > 39) set_blessed(v,FALSE);

			break;
		}

	case 4:
		{
			int power = 20 + plev * 2;
			if(only_info) return format("");

			msg_print("���Ȃ��͐���������̂𖳗͉����悤�Ǝ��݂�...");
			project_hack(GF_STASIS_LIVING,power);

		}
		break;

	case 5:
		{
			int rad = 5;
			int heal = plev * 4 + chr_adj * 10;

			if (only_info) return format("��:�`%d", heal / 2);

			msg_print("���Ȃ��̎��߂̐S���ӂ��D������񂾁c");
			project(0, rad, py, px, heal, GF_STAR_HEAL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE),-1);
			break;

		}
		break;

	case 6:
		{
			int max_num = 1 + (plev-35)/5;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(plev > 40) max_num++;
			if(only_info) return format("�ő�:%d",max_num);
	
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_BULLET_GOLEM) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("����ȏ���o���Ȃ��B",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_BULLET_GOLEM, PM_EPHEMERA))
			{
				msg_print("���Ȃ��̓S�[�����̂悤�Ȓe�������o�����B");

			}

		}
		break;

	case 7:
		{
			if(only_info) return format("");

			msg_print("�����͂��݂Ȃ����Ă����I");
			hp_player(5000);
			restore_level();

		}
		break;
	case 8:
		{

			if(only_info) return format("");

			msg_print("�n����F�̗͂����Ȃ�������Ă���C������c");

			p_ptr->special_defense |= SD_HELLFIRE_BARRIER;
			p_ptr->redraw |= (PR_STATUS);

		}
		break;

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



class_power_type class_power_aunn[] =
{
	{5,5,15,FALSE,TRUE,A_WIS,0,0,"*�_�i���m*",
		"�t���A�ɂ���_�i���������X�^�[��S�Ċ��m����B"},

	{12,10,20,FALSE,TRUE,A_WIS,0,0,"�d���E�׈����m",
		"�L�͈̗͂d���ƍ��א��̓����X�^�[�����m����B"},

	{20,20,30,TRUE,FALSE,A_DEX,20,0,"�R��̎U��",
		"�^�[�Q�b�g�Ɍ����ĒZ�������ړ����A���̂܂܍U������B�������d���Ǝ��s���₷���B"},

	{30,25,40,FALSE,FALSE,A_CON,0,0,"�Α���",
		"�����X�^�[����F������Ȃ��Ȃ�B���E���Ƀ����X�^�[�������Ԃł͎g�p�ł��Ȃ��B�_���[�W���󂯂���ҋ@�A�x�e�A���H�ȂǈȊO�̍s��������ƌ��ʂ��؂��B"},

	{35,35,60,TRUE,FALSE,A_STR,50,10,"�R�}����",
		"���͂̃����X�^�[�S�ĂɍU������B�������d���Ǝ��s���₷���B"},

	{40,50,70,FALSE,FALSE,A_CHR,0,0,"�P�O�P�C�̖�ǌ�",
		"�F�D�I�Ȍ��n�����X�^�[���ʂɏ�������B" },

	{43,72,85,FALSE,TRUE,A_CHR,0,0,"��l���݂̌ċz",
		"�_�Ђɂ��������l�̎�����Z���ԌĂяo�������X�^�[����̍U���̔������������B����ɗאڍU�����ɕ�����̒ǌ�����������悤�ɂȂ�B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_aunn(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			int rad = 255;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_print("���Ȃ��͂��̏�ɘȂ�Ŋ��o���������܂���..");
			detect_monsters_xxx(rad,RF3_DEITY);

			break;
		}
	case 1:
		{
			int rad = 25 + plev / 2;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_print("���Ȃ��͓G�̑��݂�T����..");
			detect_monsters_xxx(rad,RF3_KWAI);
			detect_monsters_evil(rad);
			break;
		}
	case 2:
		{
			int len = plev / 12;
			if(len < 2) len = 2;
			if(only_info) return format("�˒�:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
		//�Α����@�I�J���g���@�̃G�C�{���̏����ɕ֏悷��B�A�C�e���J�[�h������g����B
	case 3:
		{
			int i;
			bool flag_ng = FALSE;
			if(only_info) return format("����:--");

			for (i = m_max - 1; i >= 1; i--)
			{
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (!projectable(py,px,m_ptr->fy,m_ptr->fx)) continue;
				if (!is_hostile(m_ptr)) continue;
				if(MON_CSLEEP(m_ptr)) continue;
				if(MON_CONFUSED(m_ptr)) continue;

				flag_ng = TRUE;
				break;
			}

			if(flag_ng)
			{
				msg_print("�G�Ɍ����Ă���̂ŐΑ����ł��Ȃ��B");
				return NULL;
			}

			msg_print("���Ȃ��͐Α��ɕϐg�����I");
			p_ptr->special_defense |= (SD_EIBON_WHEEL | SD_STATUE_FORM);
			break_eibon_flag = FALSE;
			p_ptr->redraw |= (PR_STATUS);
			break;
		}

	case 4:
		{
			if(only_info) return format("");

			msg_print("���Ȃ��͖ڂ܂��邵�����ˉ�����I");
			whirlwind_attack(0);
		}
		break;
		case 5:
		{
			int level;
			int num = 10;
			level = plev / 2 + chr_adj / 2;
			bool flag = FALSE;
			if (only_info) return format("�������x��:%d", level);
			for (; num>0; num--)
			{
				if (summon_specific(-1, py, px, level, SUMMON_HOUND, (PM_FORCE_FRIENDLY | PM_ALLOW_GROUP))) flag = TRUE;
			}
			if (flag) msg_format("��ǌ����������ꂽ�I");
			else msg_format("��������Ȃ������E�E");

		}
		break;

	case 6:
		{
			int base = 6;
			if(only_info) return format("����:%d+1d%d",base,base);

			set_multishadow(base + randint1(base),FALSE);
			break;
		}


	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


class_power_type class_power_nemuno[] =
{

	{7,5,10,FALSE,FALSE,A_DEX,0,0,"�H������",
		"�H���𒲒B����B"},

	{15,30,30,TRUE,FALSE,A_CHR,0,20,"����쐬�T",
		"�N�����Ȃ��������u�꒣��v�ɂ���B�꒣��̒��ł͔\�͂��啝�ɏ㏸���A���|�ϐ��A��ბϐ��A�}�񕜂𓾂�B�܂��꒣��ɓ����Ă��������X�^�[����Ɋ��m����B�꒣�肩�痣��Ă���Ԃɑ��̃����X�^�[���꒣��ɓ���Ɠ꒣���D���邱�Ƃ�����B"},

	{20,30,40,FALSE,TRUE,A_WIS,0,0,"�����̏H�J",
		"�꒣��̒��S�Ăɒ�З͂̒x�ݑ����U�����s���B" },

	{25,25,50,FALSE,FALSE,A_STR,0,0,"�R�W�̕����",
		"�������̕���Ɉꎞ�I�Ɂu�؂ꖡ�v��t������B���łɐ؂ꖡ�̂��镐��́u*�؂ꖡ*�v�ɂȂ�B�n�̂��镐��𑕔����Ă��Ȃ��Ǝg���Ȃ��B������ύX����ƌ��ʂ�������B"},

	{30,30,60,FALSE,TRUE,A_CHR,0,0,"����쐬�U",
	"�����Ɏ��̃��[����ݒu����B" },

	{35,35,55,TRUE,FALSE,A_STR,0,10,"�n���ڂꂷ��܂Ő؂荏��",
	"�אڂ��������X�^�[��̂ɒʏ�̔{�̉񐔂̍U�����s���B�������U����ɕ��킪�򉻂���B�n�̂��镐��𑕔����Ă��Ȃ��Ǝg�p�ł��Ȃ��B" },

	{37,47,70,FALSE,TRUE,A_WIS,0,10,"���ꂽ�Ğ҉J",
		"�꒣��̒��S�Ăɋ��͂Ȑ������U�����s���B" },

	{40,30,70,FALSE,TRUE,A_INT,0,0,"���Ƃ̎R�W",
		"�꒣��̒��œG���g���������@�����m���őj�Q����B�꒣�肩��o�邩�꒣�肪���͂������ƌ��ʂ��؂��B" },

	{43,60,80,FALSE,FALSE,A_STR,0,0,"�}�E���e���}�[�_�[",
		"���͂̍L�͈͂̃����X�^�[�ɑ΂��Ė����ʍU�����s���B�ΏۂƂ̋����ɂ���čU���񐔂���������B�꒣����ɂ���Ƃ��U���񐔂�����ɑ�����B���̍U���ł̓I�[���_���[�W���󂯂Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_nemuno(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "�H���𐶐�";
			msg_print("���Ȃ��͎�ۂ悭�H�����W�ߎn�߂�..");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}

	case 1:
		{
			if(only_info) return "";

			if (IS_NEMUNO_IN_SANCTUARY)
			{
				msg_print("�����͂��łɂ��Ȃ��̓꒣�肾�B");
				return NULL;
			}
			if (!(cave[py][px].info & CAVE_ROOM))
			{
				msg_print("�����łȂ��ꏊ��꒣��ɂ��邱�Ƃ͂ł��Ȃ��B");
				return NULL;
			}
			make_nemuno_sanctuary();

			break;
		}
	case 2:
		{
			int x, y;
			bool flag_ok = FALSE;
			int base = (plev + chr_adj) / 2;
			int sides = (plev + chr_adj) / 2;
			if (only_info) return format("����:%d+1d%d", base, sides);


			//�꒣����ɂ������ʂ��Ȃ�
			msg_print("�₽���J���~�蒍����...");
			floor_attack(GF_INACT, 1, sides, base, 1);
		}
		break;

	case 3:
		{
			int base = 4 + p_ptr->lev / 4;
			int sides = 4 + p_ptr->lev / 4;
			if (only_info) return format("����:%d+1d%d", base, sides);

			if (!object_has_a_blade(&inventory[INVEN_RARM]) && !object_has_a_blade(&inventory[INVEN_LARM]))
			{
				msg_print("�n���������Ă��Ȃ��Ǝg���Ȃ��B");
				return NULL;
			}
			msg_print("���Ȃ��͈�S�s���ɕ���������n�߂�...");
			set_ele_attack(ATTACK_VORPAL, base + randint1(sides));

			break;

		}
	case 4: //����U
	{
		if (only_info) return "";
		msg_print("���Ȃ��͊���Ȏ��������n�߂�..");
		warding_glyph();
		break;
	}

	//v2.0.2 �n���ڂꂷ��܂Ő؂荏��
	case 5:
	{
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format("");

		if (!object_has_a_blade(&inventory[INVEN_RARM]) && !object_has_a_blade(&inventory[INVEN_LARM]))
		{
			msg_print("�n�̂�������������Ă��Ȃ��B");
			return NULL;
		}

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

			msg_format("%s�֖ґR�Ǝa�肩�������I", m_name);
			py_attack(y, x, HISSATSU_HAKOBORE);
		}

		break;
	}


	case 6:
	{
		int base = plev * 4 + chr_adj * 5;
		if (only_info) return format("����:%d", base);

		//�꒣����ɂ������ʂ��Ȃ�
		msg_print("���J�����̗̈���O�E����������B");
		floor_attack(GF_WATER, 0, 0, base, 1);
	}
	break;

	case 7:
	{
		if (only_info) return "";

		if (!IS_NEMUNO_IN_SANCTUARY)
		{
			msg_print("�꒣��̒��łȂ��Ǝg���Ȃ��B");
			return NULL;
		}

		msg_print("�ӂ�͐l���񂹕t���Ȃ����͋C�ɖ������B");

		p_ptr->special_defense |= SD_UNIQUE_CLASS_POWER;
		p_ptr->redraw |= (PR_STATUS);


	}
	break;


	case 8:
	{
		int rad = 4;
		if (only_info) return format("");
		sakuya_time_stop(FALSE);
		flag_friendly_attack = TRUE;

		if(one_in_(3))
			msg_format("���������悤�ɓ�����U��񂵂��I");
		else if (one_in_(2))
			msg_format("�R�W�̂����ł悻�҂��}�����I");
		else
			msg_format("�蓖���莟��ɑ~���J�����I");


		project(0,rad,py,px,100,GF_SOULSCULPTURE, PROJECT_KILL,-1);
		flag_friendly_attack = FALSE;
	}
	break;

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�e�ۍ쐬�p*/
static bool item_tester_hook_make_bullet(object_type *o_ptr)
{
	int i;

	for(i=0;soldier_bullet_making_table[i].shooting_mode;i++)
	{
		if(soldier_bullet_making_table[i].mat_tval == o_ptr->tval
			&& soldier_bullet_making_table[i].mat_sval == o_ptr->sval)
		{
			return TRUE;
		}
	}

	return FALSE;
}



/*:::���m�p���Z*/
class_power_type class_power_soldier[] =
{
	{1,0,0,FALSE,FALSE,A_INT,0,0,"�Z�\�̏K��",
		"�Z�\�K���|�C���g���g�p���ĐV�����Z�\�𓾂�B�Z�\�K���|�C���g�͈�背�x���ɓ��B���邲�Ƃ�1�|�C���g����B(�ő�15)"},
	{1,0,0,FALSE,FALSE,A_INT,0,0,"�Z�\�̊m�F",
		"�K���ς݂̋Z�\���m�F����B�s���͂�����Ȃ��B"},
	{5,5,0,FALSE,FALSE,A_DEX,0,0,"�����ˌ�",
		"�_���[�W�͂Ȃ����^�[�Q�b�g��Z���Ԍ���������ˌ����s���B���x��20�ȍ~�͍������ǉ�����B"},
	{5,5,20,FALSE,FALSE,A_DEX,0,0,"�g���b�v�E�h�A�j��",
		"���͂̃g���b�v��h�A��j�󂷂�B"},
	{5,16,30,FALSE,FALSE,A_INT,0,0,"�X�L�����E�����X�^�[",
		"���͂̃����X�^�[�𒲍�����B"},
	{8,5,30,FALSE,FALSE,A_INT,0,0,"���G",
		"���͂̓G�̈ʒu�����m����B"},
	{10,0,0,FALSE,FALSE,A_DEX,0,0,"�w�b�h�V���b�g",
		"�e�̎ˌ��œ��Ȃǂ̎�_��_���B������x�g���Ɖ����B�ˌ��̐�������25%�ቺ���邪�N���e�B�J���q�b�g����������悤�ɂȂ�B���肪�������̏ꍇ���ʂ������B���̎ˌ����[�h�̕ύX�ɂ͍s���͂�����Ȃ��B"},
	{10,10,30,FALSE,FALSE,A_INT,0,0,"���Ӓ���",
		"���͂̃g���b�v�����m����B���x��20�Œn�`�A���x��25�ŃA�C�e���A���x��30�Ń����X�^�[�����m����B"},
	{10,15,50,FALSE,FALSE,A_DEX,0,0,"�o���P�[�h�ݒu",
		"�אڂ������ꂩ������Βn�`�ɂ���B"},
	{10,0,40,FALSE,FALSE,A_INT,0,0,"�e�e�ύX",
		"�e�̑������ނ�ύX���Ďˌ����s���B�ύX����e�e�̎�ނɂ����MP�����c�e����ʓr��������B�e�Ɂu�ˌ��v�ȊO�̑��������Ă���Əe�̈З͂����f1/2�A����ȊO2/3�ɒቺ����B���P�b�g�����`���[��ꕔ�̓���ȏe�͂��̓��Z�̑ΏۂɂȂ�Ȃ��B"},
	{12,16,40,FALSE,FALSE,A_INT,0,0,"�H�����B",
		"�A�C�e���u�H���v�𐶐�����B"},
	{15,20,50,TRUE,FALSE,A_DEX,50,0,"�q�b�g���A�E�F�C",
		"�G�ɍU�����A���̌��u�ŒZ�����e���|�[�g������B�����i���d���Ɠ�Փx���オ��B�U����Ƀe���|�[�g�Ɏ��s���邱�Ƃ�����B"},
	{15,24,50,FALSE,FALSE,A_INT,0,0,"�|�[�^���ݒu",
		"�����鏰�Ƀ|�[�^����ݒu����B'>'�R�}���h�œ�ӏ��̃|�[�^���̊Ԃ��ړ��ł���B���̈ړ��̓e���|�[�g�W�Q�̉e�����󂯂Ȃ��B�O�ȏ�̃|�[�^����ݒu�������ԌÂ��|�[�^����������B�����ړ���Ƀ����X�^�[��������e���|�[�g�ϐ��𖳎����ċ����I�ɏꏊ������ւ��B"},
	{20,1,0,FALSE,FALSE,A_INT,0,0,"���_�W��",
		"���_���W������B�W���񐔂ɉ����Ė������ƃN���e�B�J���q�b�g�����㏸����B�ˌ���{���g�ȊO�𔭎˂���e�ł̓N���e�B�J���q�b�g���������ɂ����B"},
	{20,20,50,FALSE,FALSE,A_DEX,0,0,"���e�ݒu",
		"�����ɔ��e��ݒu����B���e��10�^�[���Ŕ��������a3�̔j�Б����̋��͂ȃ{�[���𔭐�������B���̔����ł̓v���C���[���_���[�W���󂯂�B���e�͈ꕔ�̑����U���ɎN�����ƗU�������蔚���^�C�~���O���ς�邱�Ƃ�����B"},
	{24,30,70,FALSE,FALSE,A_DEX,0,0,"���s�b�h�t�@�C�A",
		"���E���̃����_���ȃ^�[�Q�b�g�ɘA�˂���B�A�ˉ\���̓��x���A�b�v�ő�������B�ˌ��ꔭ���Ƃ̖�������30%�ቺ����B�ꕔ�̓���ȏe��ł͎g�p�ł��Ȃ��B"},
	{25,25,60,FALSE,FALSE,A_INT,0,0,"���Ȏ���",
		"HP�ƈꕔ�̃X�e�[�^�X�ُ���񕜂���B"},
	{28,40,60,FALSE,FALSE,A_DEX,0,0,"���K�N���b�V��",
		"���͂�*�j��*����B"},
	{35,20,60,FALSE,FALSE,A_DEX,0,0,"�e�ۉ��H",
		"�f�ތn�A�C�e����e�ۂɉ��H����B���H�����e�ۂ́u�e�e�ύX�v����g�p�ł���B"},
	{35,40,65,FALSE,FALSE,A_DEX,0,0,"�S�ϐ�",
		"�ꎞ�I�Ɍ��f�ϐ��𓾂�B"	},
	{40,65,75,FALSE,FALSE,A_STR,0,0,"�A�h���i�����E�R���g���[��",
		"�ꎞ�I�ɉ����A���̋����A����m����ԂɂȂ�B"	},

	{45,0,50,FALSE,FALSE,A_INT,0,0,"�N��",
		"�t���A�ɐݒu���ꂽ���e��S�Ĕ���������B"	},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_soldier(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			if(only_info) return "";
			if(!gain_soldier_skill()) return NULL;
		}
		break;
	case 1:
		{
			if(only_info) return "";
			check_soldier_skill();
			return NULL; //�s���͂�����Ȃ�
		}
	case 3:
		{
			int range = 1 + plev / 35;
			if(only_info) return format("�͈�:%d",range);
			msg_print("���Ȃ��͍H������o����...");
			project(0, range, py, px, 0, GF_KILL_DOOR, (PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE), -1);

		}
		break;
	case 2:
		{
			if(only_info) return "";

			if(num == 2)
				special_shooting_mode = SSM_CONFUSE;
			//else 
			//	special_shooting_mode = SSM_HEAD_SHOT;

			if(!do_cmd_fire())
			{
				special_shooting_mode = 0L;
				return NULL;
			}

			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
				new_class_power_change_energy_need = 75;

		}
		break;
	case 4:
		{
			if(only_info) return "";
			probing();
		}
		break;

	case 5:
		{
			int range = DETECT_RAD_DEFAULT;
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_SCAN_MONSTER)) range += range / 2;

			if(only_info) return format("�͈�:%d",range);
			msg_format("�T�d�ɕӂ���M����..");
			detect_monsters_normal(DETECT_RAD_DEFAULT);
			detect_monsters_invis(DETECT_RAD_DEFAULT);
			break;
		}
	case 6:
		{
			if(only_info) return "";

			if(p_ptr->special_attack & ATTACK_HEAD_SHOT)
			{
				msg_print("��_��_���̂��~�߂��B");
				p_ptr->special_attack &= ~(ATTACK_HEAD_SHOT);
			}
			else
			{
				msg_print("���Ȃ��͓G�̎�_��_�����Ƃɂ����B");
				p_ptr->special_attack |= ATTACK_HEAD_SHOT;
			}
			p_ptr->redraw |= PR_STATUS;
			return NULL; //�s���͂�����Ȃ�

		}
		break;	
	case 7:
		{
			int rad = 15 + plev / 2;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_print("���ӂ̏󋵂𒲍�����...");
			detect_traps(rad, TRUE);
			if(plev > 19) map_area(rad);
			if(plev > 24) detect_objects_normal(rad);
			if(plev > 29) detect_monsters_normal(rad);
			if(plev > 29) detect_monsters_invis(rad);
		}
		break;
	case 8:
		{
			int y,x,dir;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (!cave_naked_bold(y, x))
			{
				msg_print("�����ɂ̓o���P�[�h�����Ȃ��B");
				return NULL;
			}
			msg_print("�o���P�[�h��ݒu�����I");
			cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));

			break;

		}
		//�e�e�ύX
	case 9:
		{
			int tmp;
			if(only_info) return "";

			//�g�p�\�Ȓ�����e�e��I��
			tmp = soldier_change_bullet();
			if(!tmp) return NULL;

			//�I�񂾎ˌ����[�h���Z�b�g���e�ˌ����[�`����
			special_shooting_mode = tmp;
			if(!do_cmd_fire())
			{
				special_shooting_mode = 0L;
				return NULL;
			}

			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
				new_class_power_change_energy_need = 75;


			break;
		}

	case 10:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "�H���𐶐�";
			msg_print("�H�ׂ���쑐�Ȃǂ��W�߂��B");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}


	case 11:
		{
			if(only_info) return format("");	
			if(!hit_and_away()) return NULL;
		}
		break;
	case 12:
		{
			int x,y,old_x=0,old_y=0;
			int num=0,max=0;
			if(only_info) return "";
			if(!cave_clean_bold(py,px))
			{
				msg_print("�����ɂ͐ݒu�ł��Ȃ��B");
				return NULL;
			}
			//cave:special���g���ĐV���Ǘ�
			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					if(cave_have_flag_bold(y,x,FF_PORTAL))
					{
						if(cheat_xtra) msg_format("1 y:%d x:%d num:%d old_x:%d old_y:%d max:%d",y,x,num,old_x,old_y,max);
						num++;
						if(!max || cave[y][x].special < max)
						{
							old_x = x;
							old_y = y;
						}
						if(cave[y][x].special > max) 
						{
							max = cave[y][x].special;
						}
						if(cheat_xtra) msg_format("2 y:%d x:%d num:%d old_x:%d old_y:%d max:%d",y,x,num,old_x,old_y,max);

					}
				}
			}
			cave_set_feat(py, px, f_tag_to_index_in_init("PORTAL"));
			cave[py][px].special = max+1;
			msg_print("�|�[�^����ݒu�����I");
			//���X2�������ꍇ�A�Â��̂������
			if(num > 1)
			{
				cave_set_feat(old_y,old_x,feat_floor);
				note_spot(old_y,old_x);
				lite_spot(old_y,old_x);
				msg_print("�Â��|�[�^�����������B");
			}
		}
		break;

	case 13:
		{
			if(only_info) return format("");
			do_cmd_concentrate(0);
			break;
		}

	case 14: //���e�ݒu�@�z�s�̎M�Ɠ��������ɂ���
		{
			int dam = 100+plev*4;
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_POWERUP_BOMB)) dam += dam / 2;

			if(only_info) return format("����:%d",dam);
			if(!futo_can_place_plate(py,px) || cave[py][px].special)
			{
				msg_print("�����ɂ͔��e��u���Ȃ��B");
				return NULL;
			}
			msg_print("���e���d�|�����B");
			/*:::Mega Hack - cave_type.special�𔚒e�̃J�E���g�Ɏg�p����B*/
			cave[py][px].info |= CAVE_OBJECT;
			cave[py][px].mimic = feat_bomb;
			cave[py][px].special = 10;

			note_spot(py, px);
			lite_spot(py, px);

		}
		break;
	//���s�b�h�t�@�C�A
	//v1.1.44 �ʊ֐��ɕ�����
	case 15:
		{
			int shoot_num = (plev + 4) / 7;
			if (only_info) return format("�A�ː�:�ő�%d", shoot_num);

			if (rapid_fire(shoot_num,0))
			{
				if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT, SS_S_QUICK_DRAW))
					new_class_power_change_energy_need = 75;
			}
			else
			{
				return NULL;

			}

		}
			/*
			{
				int dir=5;
				int timeout_base, timeout_max;
				int shoot_num = (plev+4) / 7;
				object_type *o_ptr;
				bool flag_ammo = FALSE;
				bool flag_mon = FALSE;
				if(only_info) return format("�A�ː�:�ő�%d",shoot_num);

				special_shooting_mode = SSM_RAPID_FIRE;
				for(;shoot_num;shoot_num--)
				{
					bool righthand = FALSE;
					bool lefthand = FALSE;

					if(inventory[INVEN_RARM].tval == TV_GUN)
					{
						o_ptr = &inventory[INVEN_RARM];
						timeout_base = calc_gun_timeout(o_ptr) * 1000;
						timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1); 
						if(o_ptr->timeout <= timeout_max) righthand = TRUE;
					}
					if(!get_random_target(0,0)) break;
					flag_mon = TRUE;
					if(righthand)do_cmd_fire_gun_aux(INVEN_RARM,dir);

					if(inventory[INVEN_LARM].tval == TV_GUN)
					{
						o_ptr = &inventory[INVEN_LARM];
						timeout_base = calc_gun_timeout(o_ptr) * 1000;
						timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1); 
						if(o_ptr->timeout <= timeout_max) lefthand = TRUE;
					}
					if(!righthand && !lefthand) break;
					else flag_ammo = TRUE;

					if(!get_random_target(0,0)) break;
					if(lefthand)do_cmd_fire_gun_aux(INVEN_LARM,dir);
				}

				if(!flag_mon) msg_print("..���������͂ɕW�I�����Ȃ��B");
				else if(!flag_ammo) msg_print("..���������X�e���Ȃ������B");

				if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
					new_class_power_change_energy_need = 75;
			}
			*/
		break;
	case 16:
		{
			int base = 100;
			if(only_info) return format("��:%d",base);
			msg_print("���Ȃ��͌g�ш�ÃL�b�g�����o����..");
			set_cut(0);
			set_stun(0);
			set_poisoned(0);
			hp_player(base);
			break;
		}

	case 17: 
		{
			int sides = 5;
			int base = 12;
			if(only_info) return format("�͈�:%d+1d%d",base,sides);
			destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			break;
		}
	case 18:
		{
			int         item,amt,i,bullet_index=0;
			object_type *o_ptr;
			cptr        q, s;
			char            o_name[MAX_NLEN];
			int makenum;

			if(only_info) return format("");

			item_tester_hook = item_tester_hook_make_bullet;
			q = "�ǂ��e�ۂɉ��H���܂���? ";
			s = "�e�ۂɉ��H�ł������Ȃ��̂���������Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else		o_ptr = &o_list[0 - item];

			for(i=0;soldier_bullet_making_table[i].shooting_mode;i++)
			{
				if(soldier_bullet_making_table[i].mat_tval == o_ptr->tval
					&& soldier_bullet_making_table[i].mat_sval == o_ptr->sval)
				{
					bullet_index = soldier_bullet_making_table[i].shooting_mode;
					break;
				}
			}

			if(bullet_index<=0 || bullet_index>100){msg_print("ERROR:�e�ۉ��H���[�`�����Ȃ񂩕�");return NULL;}

			if(p_ptr->magic_num1[bullet_index] >= 99)
			{
				msg_print("�������̒e�ۂ͏\���Ɏ����Ă���B");
				return NULL;
			}
			object_desc(o_name, o_ptr, OD_NAME_ONLY);

			if (!get_check_strict(format("�u%s�v�����H���܂��B��낵���ł����H",o_name), CHECK_DEFAULT_Y))
				return NULL;


			msg_format("���Ȃ��͑f�ނ̉��H�Ɏ��|������..");

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
			makenum = soldier_bullet_making_table[i].prod_num;
			if(weird_luck()) makenum += 2;
			else if(one_in_(3)) makenum+=1;

			msg_format("�u%s�v��%d�����������I",soldier_bullet_table[bullet_index].name,makenum);
			p_ptr->magic_num1[bullet_index] += makenum;
			if(p_ptr->magic_num1[bullet_index]>99) p_ptr->magic_num1[bullet_index]=99;
		}
		break;
	case 19:
		{
			int base = 15;
			int time;
			if(only_info) return format("����:%d+1d%d",base,base);

			time = base + randint1(base);
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);
		}
		break;
	case 20:
		{
			int base = 7;
			int time;
			int percentage = p_ptr->chp * 100 / p_ptr->mhp;
			if(only_info) return format("����:%d+1d%d",base,base);

			time = randint1(base) + base;
			msg_format("�l�X�ȋ������������̒����삯�������I");
			set_tim_addstat(A_STR,104,time,FALSE);
			set_tim_addstat(A_DEX,104,time,FALSE);
			set_tim_addstat(A_CON,104,time,FALSE);
			set_fast(time,FALSE);
			set_shero(time,FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= (PR_HP );
			redraw_stuff();

		}
		break;
	case 21:
		{
			cave_type       *c_ptr;
			bool flag = FALSE;
			int x,y;
			if(only_info) return "";
			

			msg_print("�N���M���𔭐M�����I");
			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					c_ptr = &cave[y][x];
					if(!(c_ptr->info & CAVE_OBJECT) || !have_flag(f_info[c_ptr->mimic].flags, FF_BOMB))
						continue;

					bomb_count(y,x,-999);
					flag = TRUE;
				}
			}
			if(!flag) msg_print("...�������������Ȃ������B");

		}
		break;




	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}

	special_shooting_mode = 0L;

	return "";
}



/*:::���ϓ��Z*/
class_power_type class_power_junko[] =
{
	{ 1,20,0,FALSE,FALSE,A_INT,0,0,"���������Z����",
		"���炪�ێ����鏃���ȗ͂��g���đ����̓��Z�����B�t���A���ړ����邽�тɂ��̋Z�Ɠ������ʂ��������ċZ������ւ��B"},

	{ 1, 0,0,FALSE,FALSE,A_INT,0,0,"���������Z�g�p",
		"���グ���u�����Ȃ��Z�v���g�p����B�Z���Ƃɒǉ���MP�������B"},

	{ 1, 0,0,FALSE,FALSE,A_INT,0,0,"���������Z����",
		"���グ���u�����Ȃ��Z�v�̌��ʂ��m�F����B" },

	{ 7,11,25,FALSE,FALSE,A_WIS,0,5,"���̏���",
		"�˒��̒Z���u�����v�����̃r�[������B�����������������������X�^�[�����炩�̎�_�������Ă���΂���ɍ�p���ă_���[�W��1.5�{�ɂȂ�B"},

	{12,20,30,FALSE,FALSE,A_INT,0,0,"���n�̐_��E",
		"�אڂ��������X�^�[��̂������N�A�b�v������B"},

	{18, 0,45,FALSE,FALSE,A_DEX,0,0,"�����T",
		"�A�C�e�������ł����A���̉��l��1/100��MP�ɕϊ�����B������A�C�e���͖����Ȃ��Z�̐����ɉe����^���邱�Ƃ�����B"},

	{25,30,40,FALSE,FALSE,A_CHR,0,0,"���R����",
		"���̃t���A����Œn���̗d���𑽐��Ăяo���B"},

	{30,100,60,FALSE,FALSE,A_DEX,0,0,"�����U",
		"�����i���ނɑ΂��A���̃x�[�X�A�C�e���ɂȂ��S�Ă̔\�͂𖕏�����AC���U���C�����p�����[�^�㏸�l�ɕϊ�����B"},

	{34,34,55,FALSE,FALSE,A_CHR,0,10,"�����Ȃ鋶�C",
		"���E���̑S�Ăɋ��͂Ȑ��_�U�����s���A����ɖ������Ĕz���ɂ��悤�Ǝ��݂�B����m�����Ă��Ȃ��Ǝg���Ȃ��B"},

	{41,80,75,FALSE,FALSE,A_INT,0,0,"����̐_��E",
		"���E���̃����X�^�[�S�Ă��ŏI�i�K�܂Ń����N�A�b�v������B"},

	{44,88,80,FALSE,FALSE,A_CHR,0,20,"�E�ӂ̕S��",
		"�ɂ߂ċ��͂ȏ��������r�[������B����m�����Ă��Ȃ��Ǝg���Ȃ��B"},

	{47,160,85,FALSE,FALSE,A_WIS,0,0,"�n���q�̏���",
		"���E���̃����X�^�[��HP��S�񕜂�����B������HP�������ȉ��̃����X�^�[�͈ꌂ�œ|���B�N�G�X�g�{�X�ɂ͌��ʂ��Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_junko(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!junko_make_nameless_arts()) return NULL;
			msg_print("���Ȃ��͖����Ȃ������ȗ͂�l�X�ɑ�����..");

		}
		break;
	case 1:
		{
			if(only_info) return format("");
			if(!activate_nameless_art(FALSE)) return NULL;
		}
		break;
	case 2:
		{
			if (only_info) return format("");
			activate_nameless_art(TRUE);
			return NULL;//�m�F�݂̂Ȃ̂ōs��������Ȃ�
		}
		break;

	case 3:
		{
			int dice = 2 + plev / 3;
			int sides = 9;
			int base = plev + chr_adj * 2;
			int range = 2 + plev / 6;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���Ȃ��̏�����ῂ����������ꂽ�B");
			fire_beam(GF_PURIFY, dir, base + damroll(dice,sides));
			break;
		}
	case 4:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];

				if(!rankup_monster(cave[y][x].m_idx,1))
					msg_format("�����N����Ȃ������B");
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	
	case 5:
		{
			if(only_info) return format(" ");
			if(!convert_item_to_mana()) return NULL;
		}
		break;
	case 6:
		{
			int i;
			bool flag = FALSE;
			int max = 1 + p_ptr->lev / 7;
			if(only_info) return format("");

			for(i=0;i<max;i++) if(summon_named_creature(0, py, px, MON_HELL_FAIRY, (PM_EPHEMERA))) flag = TRUE;
			if(flag) msg_print("�F�l�̔z�������ꂽ�B");
			else msg_print("��������Ȃ�����..");

			break;
		}

	case 7:
		{
			if(only_info) return format(" ");
			if(!purify_equipment()) return NULL;
		}
		break;

	case 8:
		{
			int base = plev * 3 + chr_adj * 5;

			if(only_info) return format("����:%d+1d%d",base,base);
			msg_format("�����ȋ��C�������������B");
			project_hack2(GF_COSMIC_HORROR,1,base,base);
			charm_monsters(base);

			break;
		}
		break;

	case 9:
		{
			int i;
			bool flag = FALSE;
			if(only_info) return format("");

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (!m_ptr->r_idx) continue;

				//v1.1.38 ���E���łȂ������̂ŏC��
				if(!projectable(m_ptr->fy,m_ptr->fx,py,px)) continue;

				if(rankup_monster(i,0))	flag = TRUE;
			}

			if(!flag)
				msg_print("�����N����Ȃ�����..");
		}
		break;

	case 10:
		{
			int dam = plev * 6 + chr_adj * 10;

			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�������_���W�����𖄂ߐs�������I");
			fire_spark(GF_PURIFY, dir, dam, 2);
			break;
		}

	case 11:
		{

			int i;
			bool flag = FALSE;
			if(only_info) return format("");
			msg_print("���Ȃ��͎��͂̃����X�^�[�������������悤�Ǝ��݂�...");

			for (i = 1; i < m_max; i++)
			{
				char m_name[80];
				
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if(!m_ptr->r_idx) continue;
				if(!projectable(m_ptr->fy,m_ptr->fx,py,px)) continue;
				if(r_ptr->flags1 & RF1_QUESTOR) continue;

				if(m_ptr->hp > m_ptr->maxhp / 2)
				{
					monster_desc(m_name, m_ptr, 0);
					msg_format("%s�͊��S�Ɏ������I",m_name);
					m_ptr->hp = m_ptr->maxhp;
					if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
					if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);
					redraw_stuff();
				}
				else
				{
					bool dummy;
					mon_take_hit(i,32767,&dummy,"�͓|�ꂽ�B");
				}
				flag = TRUE;

			}

			if(!flag)
				msg_print("�����N����Ȃ������B");

			break;
		}



	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}





//v1.1.15
/*:::�w�J�[�e�B�A���Z*/
//�g���ϐ��Ȃǂ�cmd7.c��select_hecatia_body()�̂Ƃ��Ƀ�������
//�M���Ɖu��tim_gen1���g��
class_power_type class_power_hecatia[] =
{

	{9,10,15,FALSE,FALSE,A_INT,0,0,"�����X�^�[����",
		"���͂̃����X�^�[�̏��𓾂�B"},
	{15,16,25,FALSE,TRUE,A_CHR,0,10,"�z������",
		"�n���̏Z�l��z���Ƃ��ČĂяo���B"},
	//�c��̑̂�T�V���c���������x��20�ōs����̂Ń��x���ύX������
	{21,30,0,FALSE,FALSE,A_DEX,0,0,"�̂���シ��",
		"�ʂ̑̂ƌ�シ��B��シ��Ƃ��ɃX�e�[�^�X�ُ��ꎞ���ʂ͂��ׂď�����B���ꂼ��̑̂͐��i�E�����i�EHP/MP�E�g�p�\���@�̈悪�ʁX�ɂȂ��Ă���B�����̑̂�MP�������B"},
	{24,60,60,FALSE,TRUE,A_INT,0,0,"�����K��",
		"���̏�ŒT�����_�ɃA�N�Z�X����B��������̏\���H���O���H�łȂ��Ǝg���Ȃ��B"},
	//�A�C�e���J�[�h�u���_�̎�ցv
	{27,45,45,FALSE,TRUE,A_WIS,0,0,"���q�ݐg",
		"���͂̃A���f�b�h���ɂ߂č��m���Ŗ������z���ɂ���B"},
	{30,43,55,FALSE,TRUE,A_DEX,0,0,"�A�|�����ˋ�",
		"��莞�ԁA���ˁA���@�h��㏸�A�M���Ɖu�𓾂�B"},
	{36,60,70,FALSE,TRUE,A_INT,0,0,"�n���̃m���C�f�A���e��",
		"���E���̑S�Ăɑ΂��A�n���̋ƉΑ����̍U�����s���B"},
	{39,66,70,FALSE,TRUE,A_DEX,0,0,"�n���ɍ~��J",
		"���E���̃����_���ȓG�ɑ΂��Đ������̃r�[���𐔉���B"},
	//�A�C�e���J�[�h�u���v
	{42,90,75,FALSE,TRUE,A_STR,0,0,"���i�e�B�b�N�C���p�N�g",
		"�^�[�Q�b�g�ɑ΂��ɂ߂ċ��͂�覐΍U�����s���B"},
	{48,-100,80,FALSE,TRUE,A_CHR,0,0,"�g���j�^���A�����v�\�f�B",
		"�S�Ă̑̂��SMP(�Œ�100)���g�p���A�^�[�Q�b�g�ɑ΂��l�X�ȍU�����d�|����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_hecatia(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:
		{
			if(only_info) return "";
			msg_print("���Ȃ��͎��͂𖳉������ɂ߉񂵂��B");
			probing();
		}
		break;
	case 1:
		{
			int level = plev * 2;
			int num = 2 + plev / 12;
			bool flag = FALSE;
			if(only_info) return format("�������x��:%d",level);
			ignore_summon_align = TRUE;
			if(one_in_(3))
			{
				if(summon_named_creature(-1,py,px,MON_CLOWNPIECE,(PM_FORCE_PET))) flag = TRUE;
			}
			for(;num>0;num--)
			{
				if(summon_specific(-1, py, px, level, SUMMON_DEATH, (PM_ALLOW_UNIQUE | PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;

			}
			if(flag) msg_format("�n���̏Z�l���Ăяo�����B");
			else msg_format("��������Ȃ������E�E");
			ignore_summon_align = FALSE;

		}
		break;
	case 2://�̌��
		{
			if(only_info) return format(" ");
			if(!hecatia_change_body()) return NULL;
		}
		break;
	case 3:
		{
			int dir;
			bool flag_ok = TRUE;
			int count_roads=0;
			if(only_info) return format(" ");

			if(is_daytime())
			{
				msg_print("���Ԃɂ͎g���Ȃ��B");
				return NULL;
			}
			//�\���H�ɂ��邩�ǂ�������
			for(dir = 1 ; dir <= 9 ; dir++)
			{
				int x,y;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!in_bounds(y,x))
				{
					flag_ok = FALSE;
				}
				else if(y==py && x==px)
				{
					if(!cave_have_flag_bold(y,x,FF_FLOOR)) flag_ok = FALSE;
				}
				else if(!ddx[dir] || !ddy[dir])
				{
					if(cave_have_flag_bold(y,x,FF_FLOOR)) count_roads++;
				}
				else
				{
					if(!cave_have_flag_bold(y,x,FF_WALL)) flag_ok = FALSE;
				}
			}
			if(count_roads < 3) flag_ok = FALSE;

			if(!flag_ok)
			{
				msg_print("�\���H���O���H�łȂ��Ǝg���Ȃ��B");
				return NULL;
			}

			msg_print("���Ȃ��͎����̋�������͂��s�g����..");
			hack_flag_access_home = TRUE;
			do_cmd_store();
			hack_flag_access_home = FALSE;
		}
		break;
	case 4:
		{
			int power = plev * 16;
			if(power < 200) power = 200;
			if(only_info) return format("����:%d",power);

			msg_print("���Ȃ��͈Ј��I�ȕ��͋C�𔭂����B");
			charm_undead(power);

		}
		break;

	case 5: 
		{
			int base = 9 + plev / 3;
			int time;

			if(only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			set_tim_general(time,FALSE,0,0);
			set_tim_reflect(time,FALSE);
			set_resist_magic(time,FALSE);

		}
		break;

	case 6:
		{
			int dice = plev/2;
			int sides = 15 + chr_adj / 2;
			if(only_info) return format("����:%dd%d",dice,sides);
			msg_format("�n���̋Ɖ΂����͂��Ă��������I");
			project_hack2(GF_HELL_FIRE,dice,sides,0);
			break;
		}

	case 7:
		{
			bool flag = FALSE;
			int i;
			int dice = 6 + plev / 12;
			int sides = 3 + chr_adj / 10;
			int num = 8 + plev / 6;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			sakuya_time_stop(FALSE);
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_WATER, damroll(dice,sides),2, 0,0)) flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("���͂ɓG����������Ȃ������B");
					break;
				}
			}

		}
		break;

	case 8:
		{
			int base = plev * 10;
			int sides = chr_adj * 20;
			if(only_info) return format("����:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			fire_ball_jump(GF_METEOR,dir,base+randint1(sides),6,"����覐΂���n��������I");


		}
		break;
	case 9:
		{
			int i;
			int sp[3];
			int x, y;
			monster_type *m_ptr;
			char m_name[80];

			if(only_info) return format("����:�s��");
			//���ׂĂ̑̂̍���MP�𓾂�
			for(i=0;i<3;i++)
				sp[i] = p_ptr->magic_num1[9+i] * p_ptr->magic_num1[3+i] / 10000;
			sp[p_ptr->magic_num2[0]] = p_ptr->csp;

			if(sp[0] < 100 || sp[1] < 100 || sp[2] < 100)
			{
				msg_print("���̑̂�MP������Ȃ��B");
				return NULL;
			}

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}

			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			monster_desc(m_name, m_ptr, 0);
			msg_format("���Ȃ��͎O�̑S�Ă��g����%s��n���ɗ��Ƃ����Ƃɂ����I",m_name);

			for(i=0;;i++)
			{
				int typ,dam;
				if(!m_ptr->r_idx) break;
				if(sp[i%3] < (i/3+1) * 100) break;//�U����*100��MP�������̂��o����I���B�ő�18���o��͂�

				switch(randint1(4))
				{
				case 1://����{�[������
					{
						int rad = 5 + randint1(3);
						
						dam = 200+chr_adj*5+randint1(100);

						if(one_in_(2)) typ = GF_METEOR;
						else if(one_in_(2)) typ = GF_DISINTEGRATE;
						else if(one_in_(4)) typ = GF_DARK;
						else if(one_in_(3)) typ = GF_WATER;
						else if(one_in_(2)) typ = GF_HELL_FIRE;
						else typ = GF_TIME;

						if(typ == GF_WATER)
							project(0,4+randint1(3),y,x,10,GF_WATER_FLOW,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM),-1);

						project(0,rad,y,x,dam,typ,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL),-1);
					}
					break;
				case 2: //���僌�[�U�[
					{
						dam = 200+chr_adj*5+randint1(100);
						if(one_in_(7))		typ = GF_PSY_SPEAR;
						else if(one_in_(6))	typ = GF_LITE;
						else if(one_in_(5)) typ = GF_DISINTEGRATE;
						else if(one_in_(4)) typ = GF_DISENCHANT;
						else if(one_in_(3)) typ = GF_MANA;
						else if(one_in_(2)) typ = GF_NETHER;
						else  typ = GF_CHAOS;

						fire_spark(typ,dir,dam,randint1(2));

					}
					break;
				case 3: //���^�{�[�����͘A������
					{
						int num = 5 + randint1(3);
						int j;

						if(one_in_(2)) typ = GF_METEOR;
						else if(one_in_(2)) typ = GF_FIRE;
						else if(one_in_(3)) typ = GF_SHARDS;
						else if(one_in_(2)) typ = GF_CHAOS;
						else typ = GF_DARK;

						for(j=num;j>0;j--)
						{
							int rad = 1+randint1(2);
							int tx,ty;

							dam = 100 + chr_adj*2+ randint1(50);

							tx = rand_spread(x,4);
							ty = rand_spread(y,4);
							if(!in_bounds(ty,tx) || !cave_have_flag_bold(ty,tx,FF_PROJECT)) continue;
							if(!projectable(py,px,ty,tx)) continue;

							if(typ == GF_FIRE)
								project(0,randint1(3),ty,tx,5,GF_LAVA_FLOW,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM),-1);

							project(0,rad,ty,tx,dam,typ,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL),-1);
	
						}
					}
					break;
				case 4: //�A�����P�b�g
					{
						int num = 2 + randint1(3);

						int j;

						if(one_in_(6)) typ = GF_METEOR;
						else if(one_in_(5)) typ = GF_DARK;
						else if(one_in_(4)) typ = GF_NETHER;
						else if(one_in_(3)) typ = GF_HELL_FIRE;
						else if(one_in_(2)) typ = GF_NUKE;
						else typ = GF_SOUND;

						for(j=num;j>0;j--)
						{
							dam = 50 + chr_adj + randint1(50);
							fire_rocket(typ,dir,dam,2);
						}
					}
					break;
				}
			}

			//�S�Ă̑̂�MP��0�ɂ���
			for(i=0;i<3;i++)p_ptr->magic_num1[3+i] = 0;

		}
		break;

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�˕P��p�Z*/
/*:::magic_num1[0-19]�𔪕S���̐_�X�~�_���̖��@���X�g�Ƃ��Ďg�p����*/
/*:::magic_num1[20]�𔪕S���̐_�X�~�_����r_idx�Ƃ��Ďg�p����*/
//�_���l�̌��g�p�t���O��concent���g��
class_power_type class_power_yorihime[] =
{
	{7,0,25,FALSE,TRUE,A_CHR,0,0,"���S���̐_�X",
		"�����_���Ȑ_�i�n�����X�^�[�����̐g�ɍ~�낵�A���̐_�i�������Z���s�g����B���Z�̎�ނɉ������ǉ���MP���K�v�ɂȂ�B�L�����Z�������Ƃ��ɂ��s���͂������B"},
	{12,24,30,FALSE,TRUE,A_STR,0,3,"�_���l�̌�",
		"������߂��ňړ����אڍU�������悤�Ƃ��������X�^�[�ɃJ�E���^�[�U�����s���A��m���ōs���I��������B�������𑕔����Ă��Ȃ��Ǝg���Ȃ��B���V�H�X�a�𑕔����Ă���ƌ��ʔ͈͂��L����B�v���C���[���ҋ@�Ɛ_�~�낵�ȊO�̍s��������Ɖ��������B"},

//�A�C�e���J�[�h�F�p���W�E�������̒�
	{18,30,35,FALSE,FALSE,A_INT,0,0,"���R�F��",
		"��莞�ԁA�����X�^�[�ɂ�镐��U���Ɠ��Z�u�ˌ��v�����m���Ŗ���������B"},
	{23,20,45,FALSE,TRUE,A_CON,0,6,"�Η��_",
		"������HP��1/3�̈З͂̃v���Y�}�����̃u���X����B"},
	{27,45,50,FALSE,TRUE,A_WIS,0,0,"�����l�̉�",
		"��莞�ԁA���ɑ΂��銮�S�ȖƉu�Ƌ��͂ȉΉ��̃I�[���𓾂ĉΉ������U�����ł���悤�ɂȂ�B"},
	{31,30,60,FALSE,TRUE,A_DEX,0,10,"�΋ÉW��",
		"��莞�ԁA�M���U���ɑ΂��銮�S�ȖƉu�Ɣ��˔\�͂𓾂�B"},
//�A�C�e���J�[�h:�����Ē�
	{35,40,65,FALSE,FALSE,A_WIS,0,0,"�V���P��",
		"���E���̑S�Ẵ����X�^�[���u���̌����v�����ōU������B"},
	{38,55,70,FALSE,TRUE,A_WIS,0,0,"�ɓ��\��",
		"���E���S�Ăɑ΂��ċ��͂Ȕj�ב����U�����s���A����Ɏ��ꂽ���������􂷂�B"},
	{42,80,80,FALSE,TRUE,A_CHR,0,0,"�V�F��",
		"��莞�ԁA�Í��E�n���E�n���̍��Α����̍U���ւ̑ϐ��AAC�㏸�A���@�h��㏸�A���͑啝�㏸�𓾂�B"},
	{45,96,90,FALSE,TRUE,A_WIS,0,0,"�V�Ƒ��_",
		"���͂̓G�ɑ�Ō���^����B�߂��ɂ���قǌ��ʂ��傫���B�V�F�󔄖��̍~�_���ɍs���ƌ��ʂ����������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


/*:::�˕P�́u���S���̐_�X�~�_�v�ŏo�Ă���_�i*/
static bool monster_hook_yorihime(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//�����Ǝo�͌Ă΂Ȃ�
	if(r_idx == MON_YORIHIME) return FALSE;
	if(r_idx == MON_TOYOHIME) return FALSE;
	//���ςƃw�J�[�e�B�A���~�߂Ƃ�
	if(r_idx == MON_JUNKO) return FALSE;
	if(r_idx == MON_HECATIA1) return FALSE;
	if(r_idx == MON_HECATIA2) return FALSE;
	if(r_idx == MON_HECATIA3) return FALSE;

	//�_�i�t���O�������Ȃ����̂͑ΏۊO
	if(!(r_ptr->flags3 & RF3_DEITY)) return FALSE;


	//���Z�������Ȃ��_�i��FALSE
	if(!(r_ptr->flags4 | r_ptr->flags5 | r_ptr->flags6 | r_ptr->flags9)) return FALSE;

	return TRUE;

}

cptr do_cmd_class_power_aux_yorihime(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0://���S���̐_�X�~�_
		{
			int r_idx = 0;
			if(only_info) return format("");

			//�_�i��r_idx��I��
			get_mon_num_prep(monster_hook_yorihime,NULL);//�œ|�ςݐ_�i�������Əo�邩�`�F�b�N
			r_idx = get_mon_num(MAX(dun_level,p_ptr->lev));
			get_mon_num_prep(NULL, NULL);

			if(!r_idx)
			{
				msg_print("�����~��Ă��Ȃ�����...");
				break;
			}
			else
			{
				u32b rflags;
				int j,new_num;
				int mlist_num = 0;
				int memory = 20; //���@�L�����@���20
				int shift;
				//������
				for(j=0;j<20;j++)p_ptr->magic_num1[j]=0;

				//�I�肳�ꂽ�_�ir_idx�̂����Z���ꗗ�ɂ���magic_num1[0-19]�ɓo�^
				for(j=0;j<4;j++)
				{
					int spell;
					monster_race *r_ptr = &r_info[r_idx];

					if(j==0) rflags = r_ptr->flags4;
					else if(j==1) rflags = r_ptr->flags5;
					else if(j==2) rflags = r_ptr->flags6;
					else rflags = r_ptr->flags9;

					for(shift=0;shift<32;shift++)
					{
						int k;
						if((rflags >> shift) % 2 == 0) continue;
						spell = shift+1 + j * 32;

						if(spell < 1 || spell > 128)
						{
							msg_format("ERROR:�˕P�_�i���Z�ݒ胋�[�`���ŕs����spell�l(%d)���o��",spell);
							return NULL;
						}
						if(!monspell_list2[spell].level) continue; //���ʂȍs���▢�����̋Z�͔�Ώ�

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
			p_ptr->magic_num1[20] = r_idx;
			set_kamioroshi(KAMIOROSHI_YAOYOROZU, 0);

			 //�L�����Z�����Ă��s�����������
			if(!cast_monspell_new())
			{
				cptr mname;

				//v1.1.30 �����_�����j�[�N�̖��O�����ǉ�
				if(IS_RANDOM_UNIQUE_IDX(r_idx))
					mname = random_unique_name[r_idx - MON_RANDOM_UNIQUE_1];
				else
					mname = r_name + r_info[r_idx].name;

				//�u���_��̐_��́`�v�Ƃ����b�Z�[�W�o����ςȂ̂�streq�ŕ��򂵂悤�Ƃ��������܂������Ȃ��B�S�p�ɑΉ����Ă��Ȃ��̂��B
				//if(streq(r_name + r_info[r_idx].name, "�_��"))
				if(r_idx == MON_C_SHINREI || r_idx == MON_D_SHINREI)
					msg_format("%s�͉��������ɋA���Ă������B",mname);
				else
					msg_format("%s�̐_��͉��������ɋA���Ă������B",mname);
			}

		}
		break;


	
	case 1: //�_���l�̌�
		{
			int time = 25 + plev / 2;
			int range = plev/12;
			if(inventory[INVEN_RARM].name1 == ART_AMENOHABAKIRI ||inventory[INVEN_LARM].name1 == ART_AMENOHABAKIRI)
				range += 4;

			if(only_info) return format("����:%d �͈�:%d",time,range);
			do_cmd_concentrate(0);
			set_kamioroshi(KAMIOROSHI_GION, time);

		}
		break;
	//SLASH,SPEAR�̍U���S�Ă�STING�̍U���̂���HUMAN,DEMIHUMAN,is_gen_unique()�̃����X�^�[�ɂ��U���𖳌���
	//ARROW�n�̃{���g�𖳌���
	case 2: //���R�F
		{
			int time = 10 + plev / 5;

			if(only_info) return format("����:%d+1d%d",time,time);

			if(use_itemcard)
				msg_print("�����������Ȑ_�C�����Ȃ������̂�������..");
			else
				msg_print("���R�F�������Ȃ��̐g�ɍ~��Ă���...");
			set_kamioroshi(KAMIOROSHI_KANAYAMAHIKO, time+randint1(time));

		}
		break;

	case 3: //�Η��_
		{
			int dam = p_ptr->chp / 3;

			if(dam < 1) dam = 1;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			set_kamioroshi(KAMIOROSHI_HONOIKAZUTI, 0);

			msg_print("�Η��_���҉΂Ɨ����������N�������I");
			fire_ball(GF_PLASMA, dir, dam, -2);
		}
		break;
	case 4: //�����l�̉��@�j�ז��@�̂Ɠ���
		{
			int base = 5 + plev / 5;

			if(only_info) return format("����:%d+1d%d",base,base);

			msg_print("�����l�̐_�΂��g�ɏh�����I");
			set_kamioroshi(KAMIOROSHI_ATAGO, randint1(base) + base);

		}
		break;
	case 5: //�΋ÉW���@�j�ז��@�̂Ɠ���
		{
			int base = 15 + plev / 5;

			if(only_info) return format("����:%d+1d%d",base,base);
			msg_print("�΋ÉW��������A�_�����f�����I");
			set_kamioroshi(KAMIOROSHI_ISHIKORIDOME, randint1(base) + base);
		}
		break;
	case 6: //�V���P��
		{
			int dam = plev * 200;
			if(dam > 9999) dam = 9999;
			if(only_info) return format("����:%d",dam);

			set_kamioroshi(KAMIOROSHI_AMATSUMIKABOSHI, 0);
			msg_print("�V���P���������̋P����������I");

			//�g���ĂȂ��֐����������̂ł����g����������:)
			deathray_monsters();

		}
		break;
	case 7: //�ɓ��\���@�j�ז��@�̂Ɠ���
		{
			int dice = 1;
			int sides = plev * 3;
			int base = plev * 3;
			if (only_info) return format("����:%d+1d%d",dice, sides, base);

			set_kamioroshi(KAMIOROSHI_IZUNOME, 0);
			msg_print("��̉��Ƌ��Ƀ_���W�����ɐ���ȋ�C���������E�E");		
			project_hack2(GF_HOLY_FIRE, dice,sides,base);
			if (remove_all_curse())
			{
				msg_print("�����i�ɂ��������������������B");
			}
		}
		break;
	case 8: //�V�F�󔄖��@�j�ז��@�̂Ɠ���
		{
			int base = 10 + plev / 5;

			if(only_info) return format("����:%d+1d%d",base,base);
			msg_print("���Ȃ��͌y�₩�ɕ����n�߂��B");
			set_kamioroshi(KAMIOROSHI_AMENOUZUME, randint1(base) + base);
		}
		break;

	case 9: //�V�Ƒ��_�@�j�ז��@�̂Ɠ���
		{

			if(only_info) return format("����:�s��");

			call_amaterasu();
		}
		break;



	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}





/*:::�L�P���Z*/
class_power_type class_power_toyohime[] =
{

	{5,8,25,FALSE,FALSE,A_INT,0,0,"�e���|�[�g",
		"�������̃����_���e���|�[�g���s���B"},
	{9,8,25,FALSE,FALSE,A_INT,0,0,"�q�g���m",
		"���͂̍L�͈͂̐��������m����B"},
	{15,25,40,FALSE,FALSE,A_CHR,0,5,"���̌R������",
		"���ʐ��͂̃����X�^�[��z���Ƃ��đ�����������B"},
	{20,18,50,FALSE,FALSE,A_DEX,0,0,"�e���|�[�g�E���x��",
		"�ʂ̃t���A�փe���|�[�g����B"},
	{25,25,60,FALSE,TRUE,A_INT,0,0,"�����̔�",
		"�w�肵���O���b�h�փe���|�[�g����B��������������Ǝ��s���邱�Ƃ�����B"},
	{30,30,55,FALSE,TRUE,A_WIS,0,0,"*�Ǖ�*",
		"�w�肵���G��̂����m���Ō��݂̃t���A����ǂ������B���x��40�ȍ~�͎��͂̑S�Ă̓G���ΏۂɂȂ�B�N�G�X�g�_���W�����ł͎g�����A���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
//�A�C�e���J�[�h�F���҂̕���
	{35,20,65,FALSE,TRUE,A_INT,0,0,"�����̔��U",
		"���E���̎w�肵�������X�^�[���w�肵���O���b�h�փe���|�[�g������B��������������Ǝ��s���邱�Ƃ�����B�܂��e���|�[�g�ϐ��̂���G�ΓI�ȃ����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	{40,72,80,FALSE,TRUE,A_INT,0,0,"�����̔��V",
		"�t���A���̎w�肵���ʒu�̎��ӂɑS�Ă̔z�������X�^�[���e���|�[�g������B"},
//�A�C�e���J�[�h�F���҂̕���
	{45,240,90,FALSE,TRUE,A_CHR,0,0,"���̌��]�������",
		"��������ƁA�Ȍケ�̃t���A�Ńe���|�[�g�Ȃǂ��g���������X�^�[���t���A��������A����Ƀ����X�^�[���g���������@���j�Q�����悤�ɂȂ�B�N�G�X�g�����Ώۃ����X�^�[�ɂ͌��ʂ��Ȃ��N�G�X�g�_���W�����ł͎g���Ȃ��B"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_toyohime(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0://�e���|�[�g
		{
			int dist = 25 + plev / 2;
			if(only_info) return format("����:%d",dist);
			msg_print("���Ȃ��͏u���ɏ������B");
			teleport_player(dist, 0L);
		}
		break;

	case 1: //�q�g���m
		{
			int rad = 25 + plev * 3 / 2;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_print("���Ȃ����q��̒���������������E�E");
			detect_monsters_living(rad);
		}
		break;
	case 2: //�R�c����
		{
			bool flag = FALSE;
			int i;
			int count = 2 + plev / 5;
			int mode;


			if(only_info) return "";
			for(i=0;i<count;i++)
			{
				int r_idx;
				if(randint1(p_ptr->lev) > 29)
					r_idx = MON_MOON_SENTINEL;
				else if(randint1(p_ptr->lev) > 19)
					r_idx = MON_GYOKUTO;
				else
					r_idx = MON_EAGLE_RABBIT;

				if(summon_named_creature(0,py,px,r_idx,PM_FORCE_PET)) flag = TRUE;

			}
			if(flag)
				msg_print("�����B�����W�����B");
			else 
				msg_print("�N������Ȃ�����...");

		}
		break;

	case 3:
		{
			if(only_info) return format("");

			if (!get_check("�{���ɑ��̊K�Ƀe���|�[�g���܂����H")) return NULL;
			teleport_level(0);
		}
		break;
	case 4:
		{
			int range = plev;
			if(only_info) return format("����:%d",range);
			if (!dimension_door(D_DOOR_TOYOHIME)) return NULL;
		}
		break;

	case 5: //�Ǖ�
		{
			int power = 100 + plev * 3;

			if(only_info) return format("����:%d",power);

			if(plev < 40)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_GENOCIDE, dir, power, 0);
			}
			else
			{

				if(one_in_(7)) 
					msg_print("�u�q�y�ɐ��܂�A���S�ɑ���ꂵ�q�g�A���O�̏�y�͂����ł͂Ȃ��I�v");

				mass_genocide(power, TRUE);
			}


		}
		break;
	case 6: //�����̔��U
		{
			int x, y;
			monster_type *m_ptr;
			int range = plev * 2;

			if(only_info) return format("����:%d",range);
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				int tx,ty;
				int dist;
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if (!tgt_pt(&tx, &ty)) return NULL;

				dist = distance(px,py,tx,ty);

				if(is_hostile(m_ptr) && r_ptr->flagsr & RFR_RES_TELE)
				{
					msg_format("%s�ɂ͑ϐ�������I",m_name);
				}
				else if( randint1(dist) > range)
				{
					msg_print("��ԑ���Ɏ��s�����I");
					teleport_away(cave[y][x].m_idx, randint1(250), TELEPORT_PASSIVE);
				}
				else
				{
					
					msg_format("%s�𗣂ꂽ�ꏊ�֑��荞�񂾁B",m_name);
					teleport_monster_to(cave[y][x].m_idx, ty, tx, range,TELEPORT_PASSIVE);
				}
			}
		}
		break;


	case 7:
		{
			int tx,ty,i;
			bool flag = FALSE;

			if(only_info) return format("����:������");

			if (!tgt_pt(&tx, &ty)) return NULL;

			msg_print("���Ȃ��͔ޕ��̌i�F��ڂ̑O�̂��̂Ƃ��A�z�������𑗂荞�񂾁I");
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (!m_ptr->r_idx) continue;
				if (!is_pet(m_ptr)) continue;
				teleport_monster_to(i, ty, tx, 100,0L);
				flag = TRUE;

			}

			if(!flag)
				msg_print("...�������A��Ă����z�������Ȃ������B");

		}
		break;
	case 8: //���]�������
		{
			if(only_info) return format("");

			if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || EXTRA_QUEST_FLOOR)
			{
				msg_print("�����ł͎g���Ȃ��B");
				return NULL;
			}

			if(use_itemcard)
				msg_print("�t���A�̉������͂��Ɍ������C������...");
			else
				msg_print("���Ȃ��͎t�������Ɏd�|�����g���b�v���N�������B");

			p_ptr->special_flags |= SPF_ORBITAL_TRAP;
			p_ptr->redraw |= (PR_STATUS);
			redraw_stuff();

		}
		break;

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}







/*:::���C�Z��2��p�Z*/
class_power_type class_power_reisen2[] =
{

	{ 5,10,25,FALSE,FALSE,A_CHR,0,0,"�݂�",
		"�A�C�e���u�c�q�v�����B�������n�𑕔����Ă��Ȃ��Ǝ��s�ł��Ȃ��B"},
	{ 15,5,40,FALSE,FALSE,A_DEX,0,5,"�A�C�e���D��",
		"�אڂ��������X�^�[���E���Ď����Ă���A�C�e���������_���Ɉ�D���B"},
	{ 24,12,55,FALSE,TRUE,A_DEX,0,0,"����",
		"�Z���ԉ�������B"},

	{ 30,20,60,FALSE,FALSE,A_DEX,0,3,"�q�b�g���A�E�F�C",
		"�אڂ����G�ɍU�����A���̂܂܈�u�ŗ��E����B���E�Ɏ��s���邱�Ƃ�����B"},

	{ 40,30,70,FALSE,FALSE,A_DEX,0,0,"�t�F���g�t�@�C�o�[",
		"�ɂ߂ď�v�ȕR�Ń����X�^�[�𔛂�グ�ړ���W�Q���s���𒆊m���ŖW�Q����B�������̓v���C���[�ƃ����X�^�[�̑��x���Ɉˑ����A�������f�����G��ߔ����邱�Ƃ͓���B�����Ă����荬�����Ă���G�ɂ͌����₷���A����ȓG�E�͋����G�E���j�[�N�����X�^�[�ɂ͌����ɂ����B"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_reisen2(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "��:1";
			msg_print("���Ȃ��͊��ꂽ����Ŗ݂𝑂��n�߂�...");
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_DANGO));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
		}
		break;
	case 1:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);

				if(m_ptr->hold_o_idx)
				{
					object_type *q_ptr = &o_list[m_ptr->hold_o_idx];
					char o_name[MAX_NLEN];

					object_desc(o_name, q_ptr, OD_NAME_ONLY);
					q_ptr->held_m_idx = 0;
					q_ptr->marked = OM_TOUCHED;
					m_ptr->hold_o_idx = q_ptr->next_o_idx;
					q_ptr->next_o_idx = 0;
					msg_format("%s����%s��D�����I",m_name,o_name);
					inven_carry(q_ptr);

				}
				else
				{
					msg_format("%s�͉��������Ă��Ȃ��悤���B",m_name);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	

	case 2:
		{
			int time = 5 + plev / 10;
			if(only_info) return format("����:%d",time);
			set_fast(time, FALSE);

		}
		break;
	case 3:
		{
			if(only_info) return format("");	
			if(!hit_and_away()) return NULL;
		}
		break;

	case 4:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
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
				int chance;
				int spd = p_ptr->pspeed;
				int mspd = m_ptr->mspeed;
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if(r_ptr->flags2 & RF2_POWERFUL) mspd += 5;
				if(r_ptr->flags2 & RF2_GIGANTIC) mspd += 5;
		
				if(MON_CSLEEP(m_ptr)) mspd -= 10;
				if(MON_CONFUSED(m_ptr)) mspd -= 5;

				chance = plev/2;
				chance += (spd - mspd) * 3;	

				if(use_itemcard) chance += 75;
				if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance = chance * 4 / 5;

				monster_desc(m_name, m_ptr, 0);
				if(m_ptr->mflag & MFLAG_FEMTO_FIBER)
				{
					msg_format("%s�͂��łɕߊl�ς݂��B",m_name);
					return NULL;
				}

				else if(randint0(100) < chance)
				{
					msg_format("%s���t�F���g�t�@�C�o�[�ŕߔ������I",m_name);
					m_ptr->mflag |= MFLAG_FEMTO_FIBER;

				}
				else
				{
					msg_format("%s�̕ߔ��Ɏ��s�����I",m_name);
				}
				anger_monster(m_ptr);


			}

		}
		break;

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�T�O����p�Z*/
//�_�X�̒e����tim_general1�g�p
class_power_type class_power_sagume[] =
{

	{12,32,30,FALSE,FALSE,A_INT,0,0,"�G���̎�",
		"���͂̓G��z�����邢�͗F�D�I�ɂ���B�������G�̍U���Ńs���`�ɂȂ�\���̂���󋵂Ŏg��Ȃ��ƌ��ʂ����������t�Ɏ��͂̓G���N���ĉ�������B�N�G�X�g�̓����Ώۂ�_���W�����{�X�ɂ͌��ʂ��Ȃ��B"},

	{24,30,40,FALSE,FALSE,A_DEX,0,0,"�q�g�T�m�^�@��",
		"�@�����Ăяo���B�@���͕��􂵂Ȃ�����s�K���ɓ����j�ב����̔����ōU������B�t���A���ړ�����Ə�����B"},

	{30,50,65,FALSE,TRUE,A_WIS,0,0,"�_�X�̒e��",
		"�ꎞ�I�ɓG����̍U����HP�łȂ�MP�Ŏ󂯂�悤�ɂȂ�BMP���s����ƌ��ʂ�������B���̌������̍U����G����̍U���łȂ��_���[�W�ɂ͌��ʂ��Ȃ��B"},

	{35,64,70,FALSE,TRUE,A_INT,0,0,"�G���̓�d��",
		"���E���̃����X�^�[�S�Ă����m���ŋ���m�����A����Ɉړ��֎~��Ԃɂ���B�������ړ����邢�͗אڍU���̔\�͂��ŏ���������Ȃ��G����̂ł��܂܂�Ă���Ǝ��s���A����Ƀv���C���[�Ɍ����Č��̎􂢂���������B" },

	{ 40,80,75,FALSE,TRUE,A_INT,0,0,"����S�ǂ̉A�z��",
		"�ꎞ�I�ɕ����h��㏸�A���@�h��㏸�A���˂𓾂�B" },

	{45,64,75,FALSE,TRUE,A_CHR,0,0,"�З��̔���",
		"�����̂���ꏊ�ɋ��͂ȑM�������̃{�[���𔭐������Ă��̌㒆�����e���|�[�g����B���̃e���|�[�g�͔��e���|�[�g�ɖW�Q����Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_sagume(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			if(only_info) return "����:�s��";

			if (p_ptr->inside_arena)
			{
				msg_print("�����ł͎g���Ȃ��B");
				return NULL;
			}

			if(one_in_(10)) msg_print("�u...�����ł͖����B�v");
			else if(one_in_(9)) msg_print("�u�^���͋t�]���n�߂��B�v");
			else if(one_in_(8)) msg_print("�u����͂���Β�����B�v");
			else if (one_in_(7)) msg_print("�u�q��ɖ����������𑱂��邪�ǂ��I�v");
			else msg_print("���Ȃ��͓G�Ɍ����Č��n�߂�..");

			if(process_warning(px,py,TRUE))
			{
				char m_name[80];
				charm_monsters(999);
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					m_ptr = &m_list[i];
					if(!m_ptr->r_idx) continue;
					if (!is_hostile(m_ptr)) continue;
					if(!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
					monster_desc(m_name,m_ptr,0);

					if(    (r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR)
						|| (r_info[m_ptr->r_idx].flagsr & RFR_RES_ALL))
					{
						msg_format("%s�͓G�ΓI�Ȃ܂܂��I",m_name);
					}
					else
					{
						msg_format("%s�͗F�D�I�ɂȂ����悤���B",m_name);
						set_friendly(m_ptr);
					}
				}

			}
			else
			{
				msg_print("���Ԃ͂��Ȃ��̖]�܂Ȃ������֐i�ݎn�߂��悤���I");
				aggravate_monsters(0,TRUE);
			}

			break;
		}

	case 1:
		{
			bool flag = FALSE;
			int num = 1;
			if(plev > 44) num = 3;
			if(only_info) return format("��:%d",num);

			for(;num>0;num--)
			{
				if(summon_named_creature(0,py,px,MON_SAGUME_MINE,PM_EPHEMERA)) flag = TRUE;
			}

			if(flag)
				msg_print("�@����ݒu�����B");
			else
				msg_print("�ݒu�Ɏ��s�����B");

		}
		break;

	case 2:
		{
			int base = 25;
			if(only_info) return format("����:%d+1d%d",base,base);

			set_tim_general(base+randint1(base),FALSE,0,0);

		}
		break;


	case 3:
		{
			int i,power;
			bool flag_fail = FALSE;

			power = plev * 5 + chr_adj * 5;

			if (only_info) return format("����:%d", power);

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_list[i].r_idx];

				if (!m_ptr->r_idx) continue;
				if (!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;

				if (r_ptr->flags1 & (RF1_NEVER_BLOW | RF1_NEVER_MOVE))flag_fail = TRUE;
			}

			msg_print("���Ȃ��͕��G�Ȗ��߂����n�߂�...");

			if (flag_fail)
			{
				bool stop_ty = FALSE;
				int count = 0;
				int curses = 1 + randint1(3);
				msg_format("���Ȃ��͏����̐ݒ�Ɏ��s�����I�􂢂��t�]����I");
				curse_equipment(100, 50);
				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				} while (--curses);

			}
			else
			{
				project_hack(GF_BERSERK, power);
				project_hack(GF_NO_MOVE, power / 10);

			}
		}
		break;



	case 4:
		{
			int v;
			int base = plev / 4;

			if (only_info) return format("����:%d+1d%d", base,base);
			v = base + randint1(base);

			msg_print("���łȖh�ǂ𒣂菄�点���B");
			set_shield(v, FALSE);
			set_resist_magic(v, FALSE);
			set_tim_reflect(v, FALSE);

		}
		break;


	case 5:
		{
			int dam = plev * 16 + chr_adj * 16;
			
			if(only_info) return format("����:�ő�%d",dam/2);
			msg_print("���Ȃ��͌��̗����͂��߂����Ĕ��Ă����I");
			project(0,6,py,px,dam,GF_LITE,(PROJECT_KILL|PROJECT_JUMP),-1);
			teleport_player(50+randint1(50),TELEPORT_NONMAGICAL);
		}
		break;

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�P���p�Z*/
//p_ptr->magic_num1[0-4]��_��ۗL�����X�^�[���X�g(�V���b�t���ς�)�i�[�̈�Ƃ��Ďg��
class_power_type class_power_kaguya[] =
{
	{ 8, 8,20,FALSE,TRUE,A_STR,0,3,"�u�����A���g�h���S���o���b�^(��)",
		"�Ή������̏����ȋ�����B�u��������̋ʁv�𑕔����Ă���ꍇ���͂ȃu���X����B"},
	{11, 9,20,FALSE,TRUE,A_STR,0,3,"�u�����A���g�h���S���o���b�^(��)",
		"��C�����̏����ȋ�����B�u��������̋ʁv�𑕔����Ă���ꍇ���͂ȃu���X����B"},
	{14,10,20,FALSE,TRUE,A_STR,0,3,"�u�����A���g�h���S���o���b�^(��)",
		"�d�������̏����ȋ�����B�u��������̋ʁv�𑕔����Ă���ꍇ���͂ȃu���X����B"},
	{17,11,20,FALSE,TRUE,A_STR,0,3,"�u�����A���g�h���S���o���b�^(��)",
		"�ő����̏����ȋ�����B�u��������̋ʁv�𑕔����Ă���ꍇ���͂ȃu���X����B"},
	{20,11,20,FALSE,TRUE,A_STR,0,3,"�u�����A���g�h���S���o���b�^(��)",
		"�_�����̏����ȋ�����B�u��������̋ʁv�𑕔����Ă���ꍇ���͂ȃu���X����B"},

	{23,30,30,FALSE,TRUE,A_WIS,0,0,"�u�f�B�X�g�_�C�A�����h",
		"�ꎞ�I�ɖ��@�h��㏸�𓾂�B�u�����̌�΂̔��v�𑕔����Ă���ꍇ���@�̊Z��g�ɂ܂Ƃ��B"},
	{25,25,40,FALSE,TRUE,A_DEX,0,0,"�T���}���_�[�V�[���h",
		"�ꎞ�I�ɉΉ��U���ւ̑ϐ��ƉΉ��̃I�[���𓾂�B�u���Αl�̔�߁v�𑕔����Ă���ꍇ���E���S�ĂɉΉ��U������B"},
	{28,29,50,FALSE,TRUE,A_DEX,0,5,"���̃C�����i�C�g",
		"�^�[�Q�b�g��覐Α����̋�����������B�u���̃C�����i�C�g�v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{30,30,60,FALSE,TRUE,A_CON,0,0,"���C�t�X�v�����O�C���t�B�j�e�B",
		"�̗͂𒆒��x�񕜂����łƐ؂菝����������B�u�����̎q���L�v�𑕔����Ă���ꍇ�̗͂�啝�ɉ񕜂����S�����Ɠ��̋����̌��ʂ𓾂�B"},
	{32,0,80,FALSE,TRUE,A_WIS,0,0,"�~�X�e���E��",
		"MP�������񕜂���B�u�~�X�e���E���v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{35,35,65,FALSE,TRUE,A_CHR,0,10,"�H���̋ʂ̎}",
		"�M�����邢�͈Í������̏����ȋ�����B�u���H���̋ʂ̎}�v�𑕔����Ă���ꍇ�l�X�ȑ����̋���ȋ�����B"},
	{36,72,65,FALSE,TRUE,A_INT,0,0,"�i���̏p�T",
		"�אڂ����ǂ��i�v�ǂɕω�������B�N�G�X�g�_���W�����ł͎g���Ȃ��B"},
	{40,40,75,FALSE,TRUE,A_INT,0,0,"�{�k�̏p�T",
		"�w�肵���ꏊ�Ɉ�u�Ō����B�e���|�[�g�W�Q�𖳎����邪���m�̏ꏊ��ʘH���ʂ��Ă��Ȃ��ꏊ�ɂ͈ړ��ł��Ȃ��B"},

	{43,80,80,FALSE,TRUE,A_CHR,0,0,"�G�C�W���̐Ԑ�",
		"�j�M�����̋��͂ȃ��[�U�[����B�u�G�C�W���̐Ԑ΁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
//project_hack2�ɓ���I�v�V����������΂�����
	{45,128,85,FALSE,TRUE,A_INT,0,0,"�i���̏p�U",
		"5�^�[���̊Ԗ��G������B"},
	{48,160,85,FALSE,TRUE,A_STR,0,0,"���t���̈ꖇ�V��",
		"�v���C���[�𒆐S�Ƃ���c��7�O���b�h�ɑ΂��ɂ߂ċ��͂ȕ��𑮐��U�����s���B�u���t���̈ꖇ�V��v���������Ă��Ȃ��Ǝg���Ȃ��B�܂��͈͓��ɉi�v�ǂ�}�b�v�[����K�i�Ȃǂ�����Ǝ��s����B"},
	{50,255,90,FALSE,TRUE,A_INT,0,0,"�{�k�̏p�U",
		"5��A���ōs������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_kaguya(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:	case 1:
	case 2:	case 3:
	case 4:
		{
			bool nandai = FALSE;
			int dam;
			//num��z��Y�����Ɏg���Ă���̂ŕύX������
			int gf_list[5] = {GF_FIRE,GF_COLD,GF_ELEC,GF_POIS,GF_ACID};

			if(inventory[INVEN_LITE].name1 == ART_KAGUYA_DRAGON) nandai = TRUE;

			dam = 25 + plev + chr_adj * 3/2  ;
			if(nandai) dam *= 3;

			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;

			if(nandai)
			{
				msg_format("������̋ʂ���%s�̃u���X�������ꂽ�I",gf_desc_name[gf_list[num]]);
				fire_ball(gf_list[num], dir, dam,-2);
			}
			else
			{
				msg_format("%s�̋ʂ��������B",gf_desc_name[gf_list[num]]);
				fire_ball(gf_list[num], dir, dam,1);
			}



		}
		break;
	case 5:
		{
			bool nandai = FALSE;
			int base = 10 + p_ptr->lev/5;
			if(inventory[INVEN_RARM].name1 == ART_KAGUYA_HOTOKE || inventory[INVEN_LARM].name1 == ART_KAGUYA_HOTOKE) nandai = TRUE;

			if(only_info) return format("����:%d+1d%d",base,base);
			if(nandai)
			{
				msg_print("���̌�΂̔������łȖh�ǂ𐶂ݏo�����I");
				set_magicdef(randint1(base) + base, FALSE);
			}
			else
			{
				msg_print("���Ȃ��͐S�𗎂���������..");
				set_resist_magic(randint1(base) + base,FALSE);
			}

		}
		break;
	case 6:
		{
			bool nandai = FALSE;
			int base,dam;
			if(inventory[INVEN_OUTER].name1 == ART_KAGUYA_HINEZUMI) nandai = TRUE;

			if(nandai)
			{
				int dam = plev * 3 + chr_adj * 5;
				if(only_info) return format("����:%d",dam);
				msg_print("�Αl�̔�߂���҉΂������ꂽ�I");
				project_hack2(GF_FIRE,0,0,dam);
			}
			else
			{
				int base = 20;
				int time;
				if(only_info) return format("����:%d+1d%d",base,base);
				time = base + randint1(base);
				set_tim_sh_fire(time, FALSE);
				set_oppose_fire(time, FALSE);
			}
		}
		break;
	
	case 7:	//�V���@�����Ă邩�ǂ����͂ق��Ŕ��肷��
		{
			int base = plev * 6;
			int sides = chr_adj * 6;

			if(only_info) return format("����:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_METEOR, dir, base + randint1(sides), 2,"���̂����炪�~���Ă����B")) return NULL;
		}	
		break;
	case 8:
		{
			bool nandai = FALSE;
			int heal;
			if(inventory[INVEN_NECK].name1 == ART_KAGUYA_TSUBAME) nandai = TRUE;

			if(nandai)
			{
				int v = 10 + randint1(10);
				heal = 300+plev*2+chr_adj*4;
				if(only_info) return format("��:%d",heal);
				set_tim_addstat(A_STR,105,v,FALSE);
				set_tim_addstat(A_DEX,105,v,FALSE);
				set_tim_addstat(A_CON,105,v,FALSE);
				restore_level();
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				msg_print("���̎q���L���琶���̗͂��Ƃ߂ǂȂ��N���o���Ă���I");
			}
			else
			{
				heal = 100+plev + chr_adj*2;
				if(only_info) return format("��:%d",heal);
				msg_print("�D�����������Ȃ����񂾁B");
				
			}
			hp_player(heal);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
		}
		break;
	case 9:	//�V���@�����Ă邩�ǂ����͂ق��Ŕ��肷��
		{
			int base = plev/5;

			if(only_info) return format("��:%d+1d%d",base,base);

			msg_print("�s�v�c�Ȗ��͂��̂ɖ�����..");
			player_gain_mana(base + randint1(base));

		}	
		break;
	case 10:
		{
			bool nandai = FALSE;
			int dice,sides,dam;
			int typ_list[7] = {GF_LITE,GF_DARK,GF_MANA,GF_WATER,GF_SHARDS,GF_FORCE,GF_DISP_ALL};

			if(inventory[INVEN_RARM].name1 == ART_KAGUYA_HOURAI || inventory[INVEN_LARM].name1 == ART_KAGUYA_HOURAI) nandai = TRUE;

			if(use_itemcard) nandai = TRUE;

			dice = 4 + plev / 4;
			sides = 25 + chr_adj / 2;

			if(nandai) sides *= 2;

			if(only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;

			dam = damroll(dice,sides);

			if(nandai)
			{
				msg_format("�H���̋ʂ̎}�����F�̌���������I");
				fire_ball(typ_list[randint0(7)], dir, dam,5);
			}
			else
			{
				msg_format("����߂�����������B");
				fire_ball(typ_list[randint0(2)], dir, dam,1);
			}
		}
		break;
	case 11:
		{
			int xx, yy,i;
			bool flag = FALSE;
			if(only_info) return "";

			if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle)
			{
				msg_print("�����ł͎g���Ȃ��B");
				return NULL;
			}
			for(i=0;i<8;i++)
			{
				yy = py + ddy_cdd[i];
				xx = px + ddx_cdd[i];
				if(!in_bounds(yy,xx)) continue;
				if(!cave_have_flag_bold((yy), (xx), FF_WALL)) continue;
				if(cave_have_flag_bold((yy), (xx), FF_PERMANENT)) continue;
				if(cave[yy][xx].m_idx)continue;
				cave_set_feat(yy, xx, feat_permanent);
				flag = TRUE;
			}
			if(flag)
				msg_print("���͂̕ǂɉi���̏p���������B");
			else 
				msg_print("�����N����Ȃ������B");
		}
		break;
	case 12:
		{
			int x, y;
			int cost;
			int dist = plev * 2;
			if(dist < 30) dist = 30;
			if(only_info) return format("�ړ��R�X�g:%d",dist);

			if (!teleport_walk(dist))return NULL;

		}
		break;
	case 13:
		{
			int dice = p_ptr->lev;
			int sides = chr_adj;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�G�C�W���̐Ԑ΂����z�̋P����������I");
			fire_spark(GF_NUKE,dir, damroll(dice,sides),1);

			break;
		}
	case 14:
		{
			int time = 5;

			if(only_info) return format("����:%d",time);
			msg_format("�i���̖��@�����Ȃ���������B");
			set_invuln(time,FALSE);

		}
		break;
	case 15:
		{
			int dam = 1600;
			bool flag_ok = TRUE;
			int i,j,tx,ty;

			if(only_info) return format("����:%d",dam);

			for (i = -3; i < 3; i++) for (j = -3; j < 3; j++)
			{
				ty = py + i;
				tx = px + j;
				if(!in_bounds2(ty,tx) || !cave_have_flag_bold(ty,tx,FF_HURT_DISI) && !cave_have_flag_bold(ty,tx,FF_PROJECT) || cave_have_flag_bold(ty,tx,FF_LESS)) 
				{
					flag_ok = FALSE;
					break;
				}
			}
			msg_print("���t���̈ꖇ�V�䂪�ォ��~���Ă����I");
			if(!flag_ok)
			{
				msg_format("...�����������ɂ������Ď~�܂����B");
				break;
			}

			project_hack2(GF_KINKAKUJI,0,0,dam);
		}
		break;
	case 16:
		{
			if(only_info) return format("�s��:5��");
			if (world_player)
			{
				msg_print("���Ɏ��͎~�܂��Ă���B");
				return NULL;
			}
			msg_print("���Ȃ��͐{�k�̏p�𔭓������B");
			world_player = TRUE;
			msg_print(NULL);
			//v1.1.58
			flag_update_floor_music = TRUE;
			p_ptr->energy_need -= 1500;
			p_ptr->redraw |= (PR_MAP);
			p_ptr->update |= (PU_MONSTERS);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			handle_stuff();
			msg_print("�����~�܂��Č�����I");
			break;
		}



	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}






//p_ptr->magic_num1[0]:�P�邪�|���ꂽ�Ƃ���r_idx�l
/*:::�i�Ԑ�p�Z*/
class_power_type class_power_eirin[] =
{

	{8,10,25,FALSE,TRUE,A_DEX,0,0,"�Ӓ�����",
		"�אڂ����G��̂����m���Ŗ��点��B�ʏ햰��Ȃ��G�ɂ����ʂ����邪���j�[�N�E�����X�^�[�ɂ͌��ʂ������B"},
	{12,25,30,FALSE,TRUE,A_INT,0,0,"�十�ʓe",
		"���E���̃����X�^�[�����m���ō����A�N�O������B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	{20,60,35,FALSE,TRUE,A_INT,0,0,"�⒆�̓V�n",
		"�t���A���č\������B�؂�ւ��́u�قځv��u�ōs����BEXTRA���[�h�ł̓e���|�[�g�E���x�����s���B"},
	{23,36,50,FALSE,TRUE,A_INT,0,0,"�_��̋L��",
		"�����i�S�Ă��Ӓ肵�A����Ɏ��E���̃����X�^�[�̏��𓾂�B"},
	{26,28,45,FALSE,TRUE,A_DEX,0,5,"�A�|���P�R",
		"覐Α����̃r�[������B�З͂͋|�̐��\�ŕς��B"},
	{30,40,60,FALSE,TRUE,A_CHR,0,0,"���C�t�Q�[��",
		"�אڂ���������̂�HP���ő�܂ŉ񕜂�����B"},
	{35,0,70,FALSE,TRUE,A_WIS,0,0,"�I���C�J�l�u���C��",
		"�Z���Ԓm�\�ƌ������㏸�����A�����MP�������񕜂���B"},
	{37,80,70,FALSE,TRUE,A_CON,0,0,"����̊C",
		"���E���S�Ăɑ΂����͂Ȗ������U�����d�|����B"},
	{40,40,80,FALSE,TRUE,A_INT,0,0,"�V�������@",
		"��̋P��Ǝ����̏ꏊ�����ւ���B��l���t���A�̂ǂ��ɂ��Ă����ʂ���������B"},
	{44,120,90,FALSE,TRUE,A_INT,0,0,"�V�Ԓw�ԕߒ��̖@",
		"�^�[�Q�b�g�̈ʒu���ӂɁu���̃��[���v��~���l�߂�B"},
	{48,250,95,FALSE,TRUE,A_CHR,0,0,"���C�W���O�Q�[��",
		"�t���A�̑S�Ă̐��������􂷂�B���j�[�N�E�����X�^�[�ɂ͌��ʂ��Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};



cptr do_cmd_class_power_aux_eirin(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	//�Ӓ�����
	case 0:
		{
			int x,y;
			char m_name[80];
			monster_type *m_ptr;
			monster_race *r_ptr;
			int power = 90 + plev + chr_adj * 2;
			if(only_info) return format("��{����:%d",power);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];
			if (!cave[y][x].m_idx || !m_ptr->ml)
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			r_ptr = &r_info[m_ptr->r_idx];
			
			monster_desc(m_name, m_ptr, 0);
			if((r_ptr->flagsr & RFR_RES_ALL) || (r_ptr->flags1 & RF1_QUESTOR) | (r_ptr->flags2 & RF2_EMPTY_MIND) )
			{
				msg_format("%s�ɂ͖�ɑ΂��銮�S�ȑϐ�������I",m_name);
				break;
			}
			else if(MON_CSLEEP(m_ptr)) power *= 2;
			
			if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) power /= 2;
			if(r_ptr->flags3 & RF3_NO_SLEEP) power /= 2;

			msg_print("�����̊ۖ�𓊗^�����B");
			if(r_ptr->level >= power)
			{
				msg_format("%s�ɂ͑S���򂪌����l�q���Ȃ��I",m_name);
			}
			else if(r_ptr->level > randint1(power))
			{
				msg_format("%s�ɂ͖�̌����������悤���B",m_name);
			}
			else
			{
				if(MON_CSLEEP(m_ptr)) msg_format("%s�͂���ɐ[���������悤��...",m_name);
				else msg_format("%s�͖������B",m_name);
				(void)set_monster_csleep(cave[y][x].m_idx, power);
			}

		}
		break;
	case 1:
		{
			int power = 50 + plev * 4;
			if(only_info) return format("����:%d",power);

			msg_print("���o�������炷���𕰂���...");
			stun_monsters(power);
			confuse_monsters(power);
		}
		break;
	case 2:
		{
			if(only_info) return "";

			if (p_ptr->inside_arena)
			{
				msg_print("�����̓��Z�͎g���Ȃ��B");
				return NULL;
			}

			msg_print("���Ȃ��͖��ւƔ�э��񂾁I");

			if(EXTRA_MODE)
			{
				teleport_level(0);
			}
			else
			{
				p_ptr->alter_reality = 1;
				p_ptr->redraw |= (PR_STATUS);
			}
		}
		break;
	case 3:
		{
			if(only_info) return "";
			msg_print("�ߋ��̋L����T����...");
			identify_pack();
			probing();
		}
		break;
	//�A�|��13
	case 4: 
		{
			object_type *o_ptr;
			int mult;
			int dam;
			int ty,tx;
			bool dummy;

			if(inventory[INVEN_LARM].tval == TV_BOW || inventory[INVEN_RARM].tval == TV_BOW) 
			{
				if(inventory[INVEN_LARM].tval == TV_BOW) 
					o_ptr = &inventory[INVEN_LARM];
				else 
					o_ptr = &inventory[INVEN_RARM];

				mult = bow_tmul(o_ptr->tval,o_ptr->sval);
				if (p_ptr->xtra_might) mult++;

				dam = (plev + o_ptr->to_d + chr_adj) * mult;

				if(dam < 1) dam=1;
			}
			else dam = 0;

			if(only_info) return  format("����:%d",dam);
			if(!dam)
			{
				msg_print("�|�𑕔����Ă��Ȃ��B");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_print("�_�����܂����ꌂ��������B");
			fire_beam(GF_METEOR, dir, dam);

		}
		break;		

			//���C�t�Q�[��
	case 5:
		{
			int y, x;
			int dist;
			int damage;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int heal;
			char m_name[80];
			int target_m_idx;

			if(only_info) return "";
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];

			target_m_idx = cave[y][x].m_idx;
			if(!target_m_idx || !m_list[target_m_idx].ml)
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			m_ptr = &m_list[target_m_idx];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
			if(monster_living(r_ptr))
			{
				msg_format("���Ȃ���%^s�̎��Â��n�߂�...", m_name);
				m_ptr->hp = m_ptr->maxhp;
				(void)set_monster_slow(target_m_idx, 0);
				(void)set_monster_stunned(target_m_idx, 0);
				(void)set_monster_confused(target_m_idx, 0);
				(void)set_monster_monfear(target_m_idx, 0);
				if (p_ptr->health_who == target_m_idx) p_ptr->redraw |= (PR_HEALTH);
				if (p_ptr->riding == target_m_idx) p_ptr->redraw |= (PR_UHEALTH);
				redraw_stuff();
				msg_format("%^s�͊��S�Ɏ������I", m_name);
			}
			else
				msg_format("%^s�����Â��邱�Ƃ͂ł��Ȃ��B", m_name);
			break;
		}

	case 6:
		{
			int base = plev / 10;
			int time;

			if(base < 3) base=3;
			if(only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);

			msg_print("���_���W�������I");
			set_tim_addstat(A_INT,100+base,time,FALSE);
			set_tim_addstat(A_WIS,100+base,time,FALSE);
			calc_bonuses();
			player_gain_mana(plev/4 + randint1(plev/4));

		}
		break;

	case 7://����̊C DISP_ALL
		{
			int dam = plev * 3 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);
			project_hack2(GF_DISP_ALL,0,0,dam);

			break;
		}

	case 8://�V�������@
		{
			monster_type *m_ptr;
			if(only_info) return "";

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (m_ptr->r_idx == MON_MASTER_KAGUYA) break;
			}
			if(i >= m_max)
			{
				msg_print("���͋P�邪���Ȃ��B");
				return NULL;
			}
			if(i == p_ptr->riding)
			{
				msg_print("WARNING:�����܂ł�I");
				return NULL;
			}
			msg_print("���Ȃ��͋P��̐g����ɂȂ����I");
			//move_player_effect�Ƀ����X�^�[�̂���ꏊ���w�肷��ΕW���ňʒu��������
			(void)move_player_effect(m_ptr->fy, m_ptr->fx, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
			break;
		}
	case 9:
		{
			int rad = 3;
			if(only_info) return format("���a:%d",rad);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_MAKE_GLYPH, dir, 0, rad,"�W�I�����͂Ȍ��E�ɕ����߂��I")) return NULL;

		}
		break;
	//���C�W���O�Q�[���@�S�Ă̐����Ɍ���
	case 10:
		{
			if(only_info) return format("");
		
			msg_print("�t���A���ٗl�ȃG�l���M�[�ɖ������ꂽ...");

			raising_game();
		}
		break;

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::������p�Z*/
class_power_type class_power_seiran[] =
{

	{6,6,25,FALSE,FALSE,A_DEX,0,0,"�c�q����",
		"�A�C�e���u�c�q�v�����B�n�𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},

	{12,10,30,FALSE,TRUE,A_INT,0,0,"�َ����ˌ��T",
		"���E���̃^�[�Q�b�g�̈ʒu�ɒ��ڏe�e���o������ˌ����s���B�O��邱�Ƃ��Ȃ����˂���Ȃ��B�ꕔ�̓���ȏe��͂��̔\�͂̑ΏۂɂȂ�Ȃ��B"},

	{16,20,30,FALSE,FALSE,A_CHR,0,0,"�~��M��",
		"�u�C�[�O�������B�v��F�D�I�ȏ�ԂŐ��̌Ăяo���B�K�w���[���Ǝ��s���₷���B"},

	{20,15,35,FALSE,TRUE,A_INT,0,0,"�َ����ˌ��U",
		"�ˌ��̓��������G��Z�`�������e���|�[�g������B�e���|�[�g�ϐ��⎞��U���ϐ������G�ɂ͌��ʂ��Ȃ��B�ꕔ�̓���ȏe��͂��̔\�͂̑ΏۂɂȂ�Ȃ��B"},

	{25,30,50,FALSE,TRUE,A_DEX,0,0,"���i�e�B�b�N�h���[���V���b�g",
		"�l�Ԃɑ�_���[�W��^����ˌ�����B���P�b�g�����`���[��ꕔ�̓���ȏe��͂��̔\�͂̑ΏۂɂȂ�Ȃ��B" },

	{30,25,45,FALSE,TRUE,A_INT,0,0,"�َ����ˌ��V",
		"�ǂɓ�����ƈ�x���������_���ȓG�֌������Ē��˕Ԃ�ˌ�����B�r�[�����{���g����e��łȂ��Ǝg���Ȃ��B"},

	{36,40,60,FALSE,TRUE,A_DEX,0,0,"�X�s�[�h�X�g���C�N",
		"���E���̃^�[�Q�b�g��̂Ɍ����A��s���Ŏc�e�S�Ă𔭎˂���B�ˌ��̐���������≺����B"},

	{ 40,50,75,FALSE,TRUE,A_DEX,0,0,"�C�[�O���V���[�e�B���O",
		"����ԃ����X�^�[�ɑ�_���[�W��^����ˌ�����B���P�b�g�����`���[��ꕔ�̓���ȏe��͂��̔\�͂̑ΏۂɂȂ�Ȃ��B" },

	{47,60,70,FALSE,TRUE,A_CHR,0,0,"��͌���������",
		"�ǂ��ђʂ��鋭�͂Ȏ��󑮐��r�[���̎ˌ�����B�i�v�ǂ�R���͊ђʂ��Ȃ��B���P�b�g�����`���[��ꕔ�̓���ȏe��͂��̔\�͂̑ΏۂɂȂ�Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_seiran(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:
		{
			int dice = 1 + plev / 16;
			int sides = 3;
			object_type forge, *q_ptr = &forge;
			if(only_info) return format("��:%dd%d",dice,sides);
			msg_print("���Ȃ��͒c�q�𝑂��n�߂�...");
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_DANGO));
			q_ptr->discount = 99;
			q_ptr->number = damroll(dice,sides);
			drop_near(q_ptr, -1, py, px);
		}
		break;
	case 1:
	case 3:
	case 4:
	case 5:
	case 7:
	case 8:
	{
			if(only_info) return "";

			if(num == 1)
				special_shooting_mode = SSM_SEIRAN1;
			else if(num == 3)
				special_shooting_mode = SSM_SEIRAN2;
			else if (num == 4) //v1.1.53�ǉ�
				special_shooting_mode = SSM_SLAY_HUMAN;
			else if(num == 5)
				special_shooting_mode = SSM_SEIRAN3;
			else if (num == 7) //v1.1.53�ǉ�
				special_shooting_mode = SSM_SLAY_FLYER;
			else if (num == 8)
				special_shooting_mode = SSM_SEIRAN5;

			if(!do_cmd_fire())
			{
				special_shooting_mode = 0L;
				return NULL;
			}
		}
		break;
	case 2:
		{
			int i;
			bool flag = FALSE;
			int num;
			if(only_info) return "";

			msg_print("���Ȃ��̓��[�f�[�𔭐M�����I");
			num = randint1(6) + p_ptr->lev / 5 - dun_level / 3;
			if(p_ptr->inside_quest) num /= 3;

			for(i=0;i<num;i++)
			{
				if (summon_named_creature(0, py, px, MON_EAGLE_RABBIT, PM_FORCE_FRIENDLY)) flag = TRUE;
			}
			if(flag)
				msg_print("���Ԃ������Ɍ��ꂽ�I");
			else 
				msg_print("�������N�����Ȃ�����...");

		}
		break;
	case 6:
		{
			int timeout_base, timeout_max;
			object_type *o_ptr;
			bool flag = FALSE;
			if(only_info) return "";

			if (!get_aim_dir(&dir)) return NULL;

			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}

			if(one_in_(3))
				msg_print("�u�����Ă���A���ʍő��K���i�[�̖��Z���I�v");
			else
				msg_print("�c�e�S�Ă𗐎˂����I");

			special_shooting_mode = SSM_SEIRAN4;
			while(1)
			{
				bool righthand = FALSE;
				bool lefthand = FALSE;

				if(inventory[INVEN_RARM].tval == TV_GUN)
				{
					o_ptr = &inventory[INVEN_RARM];
					timeout_base = calc_gun_timeout(o_ptr) * 1000;
					timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1); 
					if(o_ptr->timeout <= timeout_max) righthand = TRUE;
				}
				if(inventory[INVEN_LARM].tval == TV_GUN)
				{
					o_ptr = &inventory[INVEN_LARM];
					timeout_base = calc_gun_timeout(o_ptr) * 1000;
					timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1); 
					if(o_ptr->timeout <= timeout_max) lefthand = TRUE;
				}
				if(!righthand && !lefthand) break;
				else flag = TRUE;

				if(righthand)do_cmd_fire_gun_aux(INVEN_RARM,dir);
				if(!target_okay()) break;
				if(lefthand)do_cmd_fire_gun_aux(INVEN_LARM,dir);
				if(!target_okay()) break;
			}

			if(!flag) msg_print("..���������X�e���Ȃ������B");
		}
		break;

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}

	//����ˌ��t���O�����Z�b�g
	special_shooting_mode = 0L;

	return "";
}


//�����i����w����̒c�q�����炷�B����Ȃ��Ƃ���������FALSE��Ԃ��B
bool ringo_dec_dango(int num)
{
	int i,cnt=0;
	object_type *o_ptr;

	//�܂��U�b�N�S�Ă̒c�q�̐��𐔂���B���⊄���ōׂ���������Ă���\�����l���B
	for(i=0;i<INVEN_PACK;i++)
	{
		o_ptr = &inventory[i];
		if(!o_ptr->k_idx) continue;
		if(o_ptr->tval != TV_SWEETS || o_ptr->sval != SV_SWEETS_DANGO) continue;
		cnt += o_ptr->number;
	}

	if(cnt < num)
	{
		msg_print("�c�q������Ȃ�...");
		return FALSE;
	}
	if(num == 1)
		msg_format("���Ȃ��͒c�q���ЂƂH�ׂ��B");
	else if(num < 6)
		msg_format("%d�̒c�q��j�������I",num);
	else
		msg_format("%d�̒c�q����C�ɓۂݍ��񂾁I",num);

	//����܂ŐH�ׂ��c�q�̌��J�E���g
	p_ptr->magic_num1[0] += num;
	if(p_ptr->magic_num1[0] > 10000) p_ptr->magic_num1[0] = 10000;


	//�c�q���w������炷
	for(i=0;i<INVEN_PACK;i++)
	{
		o_ptr = &inventory[i];
		if(!o_ptr->k_idx) continue;
		if(o_ptr->tval != TV_SWEETS || o_ptr->sval != SV_SWEETS_DANGO) continue;
		if(o_ptr->number < num)
		{
			num -= o_ptr->number;
			inven_item_increase(i,-(o_ptr->number));
		}
		else
		{
			inven_item_increase(i,-num);
			num = 0;
		}

		if(!num) break;
	}
	//optimize�͂܂Ƃ߂čs���B��̃��[�v�Ō��炷���тɏ�������Ə��������ׂ��C���x���g��������邽�߁B
	for(i=INVEN_PACK-1;i>=0;i--) inven_item_optimize(i);	
	p_ptr->window |= (PW_INVEN);


	return TRUE;
}

//�����Z
//tim_general[0] �c�q��H�ׂ������J�E���^�@20turn �V���ɐH�ׂ��20�ɐݒ肵����
//tim_general[1] �_���S�C���t���[�G���X�̖Ɖu�J�E���^
//magic_num1[0] ����܂ŐH�ׂ��c�q�̌��J�E���g
//magic_num1[1] ���݂̋����i�K�@���l���r�͊�p�ϋv+ tim_general[0]�̃J�E���^���؂ꂽ��0�ɖ߂�
//magic_num1[2] �Ō�Ɏ󂯂������U����GF�l���L�^���Ă���
//magic_num1[3] �_���S�C���t���[�G���X�ŖƉu�𓾂Ă���GF�l�Btim_general[1]�̃J�E���^���؂ꂽ��0�ɖ߂�
//magic_num1[4] �x�������̔��̐����J�E���g�B���Ԍo�߂��Ő[���B�K�w���Ƃɉ��Z����A���l�ȏ�Ńt���A�������Ɂw�u�x�������v�Ə����ꂽ���x�𐶐���0�ɖ߂�
//magic_num2[1�`] �e�_���W�����̍Ő[���B�K�w�B�~���������̐����J�E���g�Ɏg���B�����̕ϐ��̓��Z�b�g�\�Ȃ̂ŕʊǗ�����B�e�Y����DUNGEON_*�l�ɑΉ�

class_power_type class_power_ringo[] =
{

	{5,6,20,FALSE,FALSE,A_INT,0,1,"���Ӓ���",
		"���͂̃����X�^�[�����m����B���x��10�Ńg���b�v�A���x��20�ŃA�C�e���A���x��30�Œn�`�����m����B���ʔ͈͂̓��x���A�b�v�ɔ����L����B"},
	{18,6,25,FALSE,TRUE,A_STR,0,2,"�X�g���x���[�_���S",
		"�c�q��1����A�C�����̋�����B"},
	{23,12,30,FALSE,FALSE,A_INT,0,0,"�����X�^�[����",
		"���͂̃����X�^�[�̏��𓾂�B"},

	{27,24,40,FALSE,TRUE,A_CON,0,0,"�_���S�C���t�����[�G���X",
		"�c�q��3����A�ꎞ�I�Ɂu���̋Z���g�����O�Ɏ󂯂��U���̑����v�ɑ΂��銮�S�ȖƉu�𓾂�B�������u��C�v�Ɓu�Ɋ��v�A�u�Ή��v�Ɓu�j�M�v�Ȃǂ͑S���ʂ̑����Ƃ��Ĉ�����B���̌������̖Ɖu�͓����Ȃ��B"},
	{30,25,60,FALSE,FALSE,A_INT,0,0,"�ӎ�",
		"�A�C�e�����ЂƂӒ肷��B���x��45�ȍ~�̓A�C�e���̊��S�ȏ��𓾂�B"},
	{32,33,33,FALSE,TRUE,A_DEX,0,0,"�_���S�O�o��",
		"�c�q��3����A�ꎞ�I�ɉΉ��A��C�A�d���U���ւ̑ϐ��𓾂�B" },
	{35,40,65,FALSE,FALSE,A_INT,0,5,"�򉻍��",
		"���͂ɂ���d���A�����A�b�����݂̃t���A���獂�m���ŏ��ł�����B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	{40,24,75,FALSE,TRUE,A_STR,0,0,"�x���[�x���[�_���S",
		"�c�q��5����A�ɂ߂ċ��͂ȋC�����̋�����B"},

	{43,60,80,FALSE,TRUE,A_CHR,0,0,"�_���S�t�����[",
		"�c�q��7����A�����̋���ꏊ�𒆐S�ɋ���ȋC�����̋��𔭐�������B" },

	{45,30,85,FALSE,TRUE,A_CON,0,0,"�Z�v�e���o�[�t�����[��",
		"�c�q��9����A�Z���Ԗ��G�̃o���A�𒣂�B���ʂ��؂��Ƃ��ɏ����s�����Ԃ������̂Œ��ӁB"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};
cptr do_cmd_class_power_aux_ringo(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0://���Ӓ���
		{
			int rad = 15 + plev / 2;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_print("���ӂ̏������W��͂���...");
			detect_monsters_normal(rad);
			if(plev < 10) detect_monsters_invis(rad);
			if(plev >  9) detect_traps(rad, TRUE);
			if(plev > 19) detect_objects_normal(rad);
			if(plev > 29) map_area(rad);
		}
		break;
	case 1: //�X�g���x���[�_���S
		{

			int dam = 50 + chr_adj * 10 / 3 + plev * 2;
			int rad = 1;

			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(!ringo_dec_dango(1)) return NULL;

			msg_format("�c�q�̃p���[���G�l���M�[�ɂ��ē��������B");
			fire_ball(GF_FORCE, dir, dam,rad);

			break;
		}
		break;
	case 2:
		{
			if(only_info) return "";

			msg_format("���͂̓G�𒲍�����...");
			probing();
			break;
		}
	case 3:
		{
			int base = 5 + plev/5;
			int get_typ = p_ptr->magic_num1[2];
			if(only_info) return format("����:%d ����:%s����",base,gf_desc_name[get_typ]);
			if(!ringo_dec_dango(3)) return NULL;

			if(get_typ == GF_PSY_SPEAR)
			{
				msg_print("���̍U���ւ̖Ɖu�͓���ꂻ���ɂȂ��B");
				return NULL;
			}

			set_tim_general(base,FALSE,1,get_typ);
			p_ptr->magic_num1[2] = 0;

			break;
		}
	case 4:
		{
			if(only_info) return format("");

			if(plev < 45)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
			else
			{
				if (!identify_fully(FALSE)) return NULL;
			}
			break;
		}
	case 5: //v1.1.53 �_���S�O�o��
	{
		int base = 20;
		int v;
		if (only_info) return format("����:%d+1d%d", base, base);
		if (!ringo_dec_dango(3)) return NULL;
		v = base + randint1(base);
		set_oppose_elec(v, FALSE);
		set_oppose_fire(v, FALSE);
		set_oppose_cold(v, FALSE);


	}
	break;

	case 6://�򉻍��
		{
			int power = 50 + plev * 4;
			if(only_info) return format("����:%d",power);
			msg_format("�w偌^�T���@�����͂֎U����...");
			mass_genocide_2(power, MAX_SIGHT, 0);
		}
		break;
	case 7: //�x���[�x���[�_���S
		{

			int dam = 180 + chr_adj * 8 + plev * 6;
			int rad = 4;

			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(!ringo_dec_dango(5)) return NULL;

			msg_format("����ȃG�l���M�[�̉��@���t�����I");
			fire_ball(GF_FORCE, dir, dam,rad);
		}
		break;
	case 8: //�_���S�t�����[
	{

		int dam = 400 + chr_adj * 16 + plev * 16;
		int rad = 8;

		if (only_info) return format("����:%d", dam/2);
		if (!ringo_dec_dango(7)) return NULL;

		msg_format("���̒c�q�͖��̂悤�ɔ������I");
		project(0, rad, py, px, dam, GF_FORCE, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID, -1);
	}
	break;

	case 9: //�Z�v�e���o�[�t�����[��
		{
			int time = 9;

			if(only_info) return format("����:%d",time);
			if(!use_itemcard && !ringo_dec_dango(9)) return NULL;
			msg_format("�����̋������Ȃ��𕢂����I");
			set_invuln(time,FALSE);

		}
		break;


	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



//Extra���[�h�̃A�C�e���J�[�h�p�̓�����Z���@�����̓��x�����ɕ���łȂ��Ă����Ȃ��͂��H
class_power_type class_power_extra[] =
{
	{50,100,80,FALSE,TRUE,A_CHR,0,0,"�t�@�C�i���X�p�[�N",
		"���𑮐��̋ɂ߂ċ��͂ȃr�[������B"},
	{40,100,80,FALSE,TRUE,A_CHR,0,0,"�V���Ȃ��]�̖�",
		"�̗͑啝�񕜁A�S����"},
	{40,100,80,FALSE,TRUE,A_CHR,0,0,"�i�Ԃ̒O",
		"�̗͉񕜁A�S����" },

	{ 40,100,80,FALSE,TRUE,A_CHR,0,0,"�j���^���_�̌䗘�v",
		"�ŉ񕜁A�ꎞ�őϐ��A�b������" },

	{ 30,100,80,FALSE,TRUE,A_CHR,0,0,"�R�@�̉���",
		"���Ӄf�o�t" },


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_extra(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];


	switch(num)
	{


	case 0: //�t�@�C�i���}�X�p1600
		{
			int dam = 1600;
			if(only_info) return format("����:%d",dam);
			
			if (!get_aim_dir(&dir)) return NULL;
			fire_spark(GF_DISINTEGRATE,dir, dam,2);

			break;
		}
	case 1:
		{
			if(only_info) return format("��:500+��");
			msg_print("��]�̖ʂ�ῂ�����������I");
			hp_player(500);
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
			break;
		}
		break;

		//�i�Ԃ̒O
	case 2:
	{
		if (only_info) return format("��:100+��");
		msg_print("�����ۖ�����ݍ��񂾁B");
		hp_player(100);
		set_stun(0);
		set_cut(0);
		set_poisoned(0);
		set_image(0);
		do_res_stat(A_STR);
		do_res_stat(A_INT);
		do_res_stat(A_WIS);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);
		do_res_stat(A_CHR);
		set_asthma(0);
		break;
	}
	break;

	case 3://�{�̕`���ꂽ�G�n
	{

		if (only_info) return format(" ");

		if(p_ptr->pclass != CLASS_KUTAKA)
			msg_print("�j���^���_�̗͂�������...");

		if (p_ptr->muta2 & MUT2_ASTHMA)
		{
			//�p�`�����[���ꏈ���@�v�̉̂̔\�͂ŉi�v�ψق̚b��������
			//�����ɂ͚b���͍A�̕a�C�ł͂Ȃ����ǃj���^���_�̌䗘�v�͊P�~�߂������Ԃ�M���M���Z�[�t���Ǝv��:)
			if (p_ptr->pclass == CLASS_PATCHOULI)
			{
				msg_print("�Ȃ�ƁA���N�ꂵ�߂��Ă����b�������������I");
				p_ptr->muta2_perma &= ~(MUT2_ASTHMA);
				p_ptr->muta2 &= ~(MUT2_ASTHMA);
				p_ptr->magic_num2[0] = 1; //�푰�ύX�Ƃ������Ƃ��ɍĂњb���ɂȂ�Ȃ��t���O

			}
			else
			{
				lose_mutation(116);

			}
			p_ptr->asthma = 0;


		}
		else
		{
			msg_print("�Ȃ񂾂��A����v�ɂȂ����C������B");
		}

		set_oppose_pois(5000, FALSE);


		break;

	}

	case 4:
	{
		int power = plev * 6;
		u32b flg = (PROJECT_JUMP | PROJECT_KILL);
		if (power < 100) power = 100;
		if (only_info) return format("����:%d",power);

		msg_print("���͔͂Z���ȉ����̉��ɖ������ꂽ...");
		project(0, 7, py, px, power, GF_OLD_CONF, flg, -1);
		project(0, 7, py, px, power, GF_STUN, flg, -1);
		project(0, 7, py, px, power, GF_OLD_SLOW, flg, -1);


	}
	break;


	default:
		if(only_info) return format("");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}






/*����p�Z*/
//�u���ƐÂ̋ύt�v��tim_general[0]
class_power_type class_power_yukari[] =
{
	{4,8,15,FALSE,FALSE,A_INT,0,0,"��펯�̗���",
		"�w������̕ǂȂǈړ��s�n�`�̌������֏u�Ԉړ�����B�}�b�v�[�ɓ��B����Ɣ��Α��֏o��B�ʏ�̃e���|�[�g�ňړ��ł��Ȃ��ꏊ�ɂ͏o���Ȃ��B"},
	{8,16,24,FALSE,FALSE,A_INT,0,4,"�������_��",
		"���͂̃����_���ȓG��̂ɖ������_���[�W��^����B���̍U����6/7�̊m���ōĔ�������B�t���A�Ɂu���_�@���v������Ǝg���Ȃ��B"},
	{10,0,0,FALSE,FALSE,A_INT,0,0,"�e�����E",
		"�l�X�Ȗ��@����Z���g���B�g���閂�@����Z�͈ꎞ�Ԃ��Ƃɓ���ւ��B"},
	{12,24,30,FALSE,FALSE,A_DEX,0,0,"���ƐÂ̋ύt",
		"��莞�ԁA�߂��̃����X�^�[�̈ړ���}�~����B"},
	{14,24,40,FALSE,FALSE,A_DEX,0,5,"�����̒�������s��",
		"�w�肵���^�[�Q�b�g�ɑ΂��A����ɒu���ꂽ�ߐڕ��킪���{���������B"},
	{16,32,30,FALSE,FALSE,A_WIS,0,0,"���ƌ��̎�",
		"��u�̃^�C�����O�̌�A���݂̃t���A���č\�z�����B�n���N�G�X�g�_���W�����ł͎g���Ȃ��B"},
	{18,24,30,FALSE,FALSE,A_DEX,0,0,"���_�̑�",
		"���E���̃A�C�e���������_���Ɏ���ɕ��荞�ށB"},
	{20,56,40,FALSE,FALSE,A_INT,0,0,"�l�d���E",
		"�ꎞ�I�Ɍ��f�U���ւ̑ϐ��𓾂�B���x��30��AC�㏸�A���x��40�Ŗ��@�̊Z���ǉ������B"},
	{22,24,30,FALSE,FALSE,A_INT,0,0,"�g�Ɨ��̋��E",
		"��莞�ԁA�v���C���[���g�������U���̑����l��1�����B"},
	{25,16,50,FALSE,FALSE,A_INT,0,0,"�񎟌��ƎO�����̋��E",
		"���@�����������A����ɋL���ꂽ���@���ЂƂ���������B�L�����Z�����Ă��s���͂͏����B"},
	{28,48,50,FALSE,FALSE,A_INT,0,8,"���ƈł̖Ԗ�",
		"���E���ɑM���U�����Í��U�����s���B"},
	{30,48,60,FALSE,FALSE,A_INT,0,0,"�F�Ƌ�̋��E",
		"HP��MP�����ւ���BHP50�ȉ��̎��͎g�p�ł��Ȃ��B"},
	{32,64,70,FALSE,FALSE,A_INT,0,0,"���z���z��",
		"�w�肵���ꏊ�ɏu�Ԉړ�����B�e���|�[�g�֎~�̒n�`�ɂ͏o���Ȃ��B���̓��Z�͍L��}�b�v�ł��g�����Ƃ��ł���B"},
	{34,32,70,FALSE,FALSE,A_INT,0,0,"�Ԃ��p�w���Ԃ̗�",
		"�u�d�ԁv�����̃r�[������B�������_���[�W��^���G�𐁂���΂�������ȓG�ɂ͌��ʂ������B"},
	{36,48,65,FALSE,FALSE,A_CHR,0,0,"���_�u���_���v",
		"�����X�^�[�u���_�@���v��z���Ƃ��ď�������B�t���A���ړ�����Ə�����B"},
	{38,48,50,FALSE,FALSE,A_INT,0,0,"�l�ԂƗd���̋��E",
		"��莞�ԁA�����X�^�[�ɕ���X���C��K�p����Ƃ��̎푰�t���O��1�O�ɂ����B"},

	{40,64,80,FALSE,FALSE,A_DEX,0,0,"���鏈�ɐR����",
		"�ǂ�����ł�����ɃA�N�Z�X����B���͂ɂ��L���X�y�[�X���Ȃ��Ǝg���Ȃ��B�N�G�X�g���s���̃t���A�ł͎g���Ȃ��B"},

	{43,64,80,FALSE,FALSE,A_INT,0,0,"���_���̐_�B��",
		"���͂̓G�����m���Ńt���A������ł���B"},
//	{43,72,73,FALSE,FALSE,A_INT,0,0,"���Ǝ��̋��E",
//		"��莞�ԁA�߂��̓G�̑ϐ��t���O�����]����B�u�S�Ă̍U���ɑ΂���ϐ��v��푰�R���̑ϐ��͔��]����Ȃ��B"},
	{45,120,85,FALSE,FALSE,A_INT,0,0,"�q�ό��E",
		"�t���A�̕ǂ��������ւ���B�n���N�G�X�g�_���W�����ł͎g���Ȃ��B"},
	{48,96,80,FALSE,FALSE,A_INT,0,0,"���v���X�̖�",
		"�t���A�̑S�Ă����m����B����Ɉ�莞�Ԏ��͂̑S�Ă����m�������m�\�ƌ������啝�ɏ㏸��AC�啝�㏸�ƌx���𓾂�B"},
//	{49,128,85,FALSE,FALSE,A_INT,0,0,"�[�e�����E",
//		"���͂̃����_���ȃ^�[�Q�b�g�ɑ΂��A���Ń����_���ȍU�����@����B"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_yukari(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: 
		{
			if(only_info) return format("");

			if (!wall_through_telepo(0)) return NULL;

			break;
		}
	case 1:
		//v1.1.44 ���[�`��������
		{
			int tx, ty;
			int dam = 10 + plev / 2 + chr_adj / 2;
			int rad = 0;
			monster_type *m_ptr;

			if (only_info) return format("����:%d * �s��", dam);

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (m_ptr->r_idx == MON_RAN)
				{
					if (m_ptr->mflag & MFLAG_EPHEMERA) msg_print("���łɗ����g�𒆂��B");
					else msg_print("���͌Ăяo���ɉ����Ȃ��B");
					return NULL;
				}
			}

			msg_format("������]���Ȃ���X�L�}�����яo�����I");
			if(!execute_restartable_project(0,0,7,0,0,dam,GF_ARROW,rad))
				msg_format("���u���l�A�W�I�����܂��񂪁B�v");

		}
	/*
		{
			int tx,ty;
			int dam = 10 + plev/2 + chr_adj/2 ; 
			int rad = 0;
			int flg = (PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN);
			monster_type *m_ptr;

			if(only_info) return format("����:%d * �s��",dam);

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if(m_ptr->r_idx == MON_RAN)
				{
					if(m_ptr->mflag & MFLAG_EPHEMERA) msg_print("���łɗ����g�𒆂��B");
					else msg_print("���͌Ăяo���ɉ����Ȃ��B");
					return NULL;
				}
			}

			if (!get_aim_dir(&dir)) return NULL;

			oonusa_dam = dam;
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			msg_format("������]���Ȃ����񂾁I");
			project(0,  rad, ty, tx, oonusa_dam, GF_ARROW, flg, -1);
			oonusa_dam = 0;
		}
		*/
		break;
	case 2:
		{
			if(only_info) return "";

			if(!cast_monspell_new()) return NULL;
			break;
		}
	case 3:
		{
			int rad = 2 + plev / 16;
			int time = 10+plev/5;
			if(only_info) return format("����:%d �͈�:%d",time,rad);

			if(p_ptr->tim_general[0])
			{
				msg_print("���łɔ\�͂��g���Ă���B");
				return NULL;
			}

			msg_print("���Ȃ��͋��E�𑀍삵���B���͂Ɋ�ȏꂪ�`�����ꂽ�E�E");
			set_tim_general(time,FALSE,0,0);


		}
		break;
	case 4:
		{
			int num = 2 + plev / 8;
			int     item;
			int ty,tx;
			int cnt;
			store_type *st_ptr = &town[TOWN_HITOZATO].store[STORE_HOME];
			if(only_info) return format("��:%d",num);

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

			for(;num>0;num--)
			{
				object_type *o_ptr;
				object_type forge;
				object_type *q_ptr = &forge;
				char		o_name[MAX_NLEN];

				cnt = 0;
				for(i=0;i<st_ptr->stock_num;i++)
				{
					o_ptr = &st_ptr->stock[i];
					if(!object_is_melee_weapon(o_ptr)) continue;
					cnt += o_ptr->number;
				}

				if(!cnt)
				{
					msg_print("�X�L�}���J�������A�����o�Ă��Ȃ������B");
					break;
				}

				cnt = randint1(cnt);
				for(item=0;item<st_ptr->stock_num;item++)
				{
					o_ptr = &st_ptr->stock[item];
					if(!object_is_melee_weapon(o_ptr)) continue;
					cnt -= o_ptr->number;
					if(cnt <= 0) break; //���̂Ƃ���item��o_ptr�̕���𓊝�����
				}
				if(!object_is_melee_weapon(o_ptr))
				{
					msg_print("ERROR:�u�����̒�������s�́v�őI�肳�ꂽ�A�C�e�����ߐڕ���ł͂Ȃ�");
					break;
				}

				object_copy(q_ptr,o_ptr);
				q_ptr->number = 1;
				object_desc(o_name,q_ptr,OD_NAME_ONLY);
				msg_format("�X�L�}����%s�������o���ꂽ�I",o_name);
				if(do_cmd_throw_aux2(py,px,ty,tx,1,q_ptr,4))
				{
					yukari_dec_home_item(item);
				}

			}
		}
		break;
	case 5:
		{
			if(only_info) return format("����:�����ϗe");

			if (p_ptr->inside_arena)
			{
				msg_print("�����̓��Z�͎g���Ȃ��B");
				return NULL;
			}

			msg_print("��������߂�悤�Ɋ��o���B���ɂȂ��Ă����E�E");
			/*:::0�ł͂܂����͂�*/
			p_ptr->alter_reality = 1;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	case 6:
		{
			int j;
			int num = 2 + plev / 8;
			bool flag_pick = FALSE;
			object_type forge;
			object_type *o_ptr;
			object_type *q_ptr;
			char o_name[MAX_NLEN];

			if(only_info) return format("�ő�:%d��",num);
			if(use_itemcard)	
				msg_print("���͂̂��������ɃX�L�}���J����!");
			else
				msg_print("���Ȃ��͂��������ɃX�L�}���J�����E�E");
			//�t���A�̃A�C�e����S�Ē��ׂ�
			for(i=0;i<num;i++)
			{
				int item = 0;
				int item_count_temp = 0;
				//���E���̃A�C�e���������_���ɑI��
				for(j=0;j<o_max;j++)
				{
					o_ptr = &o_list[j];
					if (!o_ptr->k_idx) continue;
					if (o_ptr->tval == TV_GOLD) continue;
					if (!projectable(o_ptr->iy,o_ptr->ix,py,px)) continue;

					item_count_temp++;
					if(one_in_(item_count_temp)) item = j;
				}
				if(!item) break;
				flag_pick = TRUE;
				o_ptr = &o_list[item];
				q_ptr = &forge;
				object_copy(q_ptr, o_ptr);
				object_desc(o_name, q_ptr, 0);
				if(!yukari_send_item_to_home(q_ptr))
				{
					msg_print("�X�L�}�����܂��J���Ȃ��B���_�������A�C�e���ň�t���B");
					break;
				}

				msg_format("%s���X�L�}�̒��ɗ������B",o_name);
				floor_item_increase(item, -255);
				floor_item_describe(item);
				floor_item_optimize(item);

			}
			if(!flag_pick)
			{
				msg_print("�ڂڂ������͌�������Ȃ��B");
				break;
			}

		}
		break;


	case 7:
		{
			int time = 5 + plev / 2;

			if(only_info) return format("����:%d",time);

			msg_print("���łȌ��E�𒣂菄�点���B");
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);
		
			if(plev > 29) set_shield(time,FALSE);
			if(plev > 39) set_magicdef(time, FALSE);
		}
		break;
	case 8:
		{
			int time = 10+plev/5;
			if(only_info) return format("����:%d",time);

			if(p_ptr->tim_general[1])
			{
				msg_print("���łɔ\�͂��g���Ă���B");
				return NULL;
			}

			msg_print("���Ȃ��͋��E�𑀍삵���B");
			set_tim_general(time,FALSE,1,0);


		}
		break;
	case 9:
		{
			/*:::Mega Hack - ���@��������ďK�����Ă��Ȃ����@�𔭓��������t���O*/
			if(only_info) return format("");
			flag_spell_consume_book = TRUE;
			do_cmd_cast();
			flag_spell_consume_book = FALSE;

		}
		break;


	case 10:
		{
			int base = plev * 3 + chr_adj * 5;

			if(only_info) return format("����:%d+1d%d",base,base);
			msg_format("��Ԃ𖄂ߐs�����悤�ȃ��[�U�[�������ꂽ�I");
			project_hack2(GF_YUKARI_STORM,1,base,base);			
			break;
		}

	case 11:
		{
			int tmp;
			if(only_info) return format("");

			if(!use_itemcard && p_ptr->chp < 50)
			{
				msg_print("�̗͂����Ȃ����Ďg���Ȃ��B");
				return NULL;
			}
			tmp = p_ptr->csp;
			p_ptr->csp = p_ptr->chp;
			p_ptr->chp = tmp;
			if(p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;
			if(p_ptr->chp > p_ptr->mhp) p_ptr->chp = p_ptr->mhp;
			p_ptr->csp_frac = 0;
			p_ptr->chp_frac = 0;
			p_ptr->redraw |= (PR_HP | PR_MANA);
			redraw_stuff();
			msg_print("�̗͂Ɩ��͂�����ւ�����B");
		}
		break;
	case 12:
		{
			if(only_info) return format("����:������");
			if (!dimension_door(D_DOOR_YUKARI)) return NULL;
		}
		break;
	case 13:
		{
			int base = 100 + plev * 3;

			if(only_info) return format("����:%d",base);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�傫���J�����X�L�}����d�Ԃ���яo���Ă����I");
			fire_beam(GF_TRAIN, dir, base);
			break;
		}
	case 14:
		{
			int i;
			int flag_ran = TRUE;
			int flag_chen = TRUE;
			monster_type *m_ptr;
			monster_race *r_ptr;
			u32b mode = PM_EPHEMERA;

			if(only_info) return "";

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_RAN) ) flag_ran = FALSE;
				if((m_ptr->r_idx == MON_CHEN) ) flag_chen = FALSE;
			}

			if(flag_ran && summon_named_creature(0, py, px, MON_RAN, PM_EPHEMERA))
			{
				msg_print("�����Ăяo�����B");
			}
			else if(flag_chen && summon_named_creature(0, py, px, MON_CHEN, PM_EPHEMERA))
			{
				msg_print("����Ăяo�����B");
			}
			else
				msg_print("���̏����Ɏ��s�����B");

			break;		

		}
	case 15:
		{
			int time = 10+plev/5;
			if(only_info) return format("����:%d",time);

			if(p_ptr->tim_general[2])
			{
				msg_print("���łɔ\�͂��g���Ă���B");
				return NULL;
			}

			msg_print("���Ȃ��͋��E�𑀍삵���B�t���A�̋C�z���ς�����E�E");
			set_tim_general(time,FALSE,2,0);
		}
		break;
	case 16:
		{
			int dx,dy;
			bool flag_ok = TRUE;
			if(only_info) return format("");

			if(p_ptr->inside_quest 
				|| quest[QUEST_TAISAI].status == QUEST_STATUS_TAKEN && dungeon_type == DUNGEON_ANGBAND && dun_level == 100
				|| quest[QUEST_SERPENT].status == QUEST_STATUS_TAKEN && dungeon_type == DUNGEON_CHAOS && dun_level == 127)
			{
				msg_print("���͎g���Ȃ��B");
				return NULL;
			}

			for(i=1;i<10;i++)
			{
				dx = px + ddx[i];
				dy = py + ddy[i];
				if(!in_bounds(dy,dx) || !cave_clean_bold(dy,dx)) flag_ok = FALSE;
			}
			
			if(!flag_ok)
			{

				msg_print("�����ł͎g���Ȃ��B");
				return NULL;
			}
			if(use_itemcard)
				msg_print("�����ɒT�����_�ւ̓�������J�����I");
			else
				msg_print("���Ȃ��͑����ɃX�L�}���J�����I");
			cave_set_feat(py, px, f_tag_to_index_in_init("HOME"));

		}
		break;

	case 17:
		{
			int power = 200 + plev * 4;
			if(only_info) return format("����:%d",power);
			mass_genocide(power, TRUE);
		}
		break;
	case 18:
		{
			int y,x,feat;
			feature_type *f_ptr;
			if(only_info) return format("");

			if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
			{
				msg_print("�����ł͎g���Ȃ��B");
				return NULL;
			}

			msg_print("���Ȃ��̓_���W�����̋��E�𑀍삵���B");
			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					if (cave_clean_bold(y,x))
						cave_set_feat(y, x, feat_granite);
					else if(cave_have_flag_bold(y,x,FF_WALL) && !cave_have_flag_bold(y,x,FF_PERMANENT))
						cave_set_feat(y, x, feat_floor);
				}
			}

			p_ptr->redraw |= (PR_MAP);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			handle_stuff();

		}
		break;
	case 19:
		{
			int base = 30;
			int time;
			if (only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("���Ȃ��̓t���A�̑S�Ă��v�����ĉ��Z�����I");
			set_tim_addstat(A_INT,120,time, FALSE);
			set_tim_addstat(A_WIS,120,time, FALSE);
			wiz_lite(FALSE);
			set_foresight(time, FALSE);
			set_radar_sense(time, FALSE);

			break;
		}








	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}







/*:::�}�~�]�E��p�Z*/
/*:::�u��b��Ƃ̍ق��v��magic_num1[0]��tim_general[0]���g�p����*/
class_power_type class_power_mamizou[] =
{

	{5,5,25,FALSE,TRUE,A_WIS,0,3,"�e���ω�",
		"�^�[�Q�b�g���ӂɌ����Ė������̃{���g�𐔔����B"},
	{12,16,30,FALSE,FALSE,A_INT,0,0,"�Ŕj",
		"��z�������@�͂Ŏ��͂̃����X�^�[�̔\�͂�ϐ��Ȃǂ�m��B"},
	{20,24,35,FALSE,FALSE,A_WIS,0,0,"���b���i�@�t",
		"�����n�̃����X�^�[��z���Ƃ��ď�������B���̏����ŏo�Ă��������X�^�[�͈ێ��R�X�g���Ⴂ���d���{�ɕ߂炦��ꂸ�t���A�ړ��ŏ�����B"},
	{24,30,45,FALSE,TRUE,A_INT,0,0,"�ω�",
		"�߂��̎w�肵�������X�^�[�Ɉ�莞�ԕϐg����B�ϐg���̓��{���E�A�~�����b�g�ȊO�̑���������������A�ϐg���������X�^�[�̔\�͂�'U'�R�}���h�Ŏg�����Ƃ��ł���B�ϐg�̐������̓��x���E�m�\�E�����E�Ώۃ����X�^�[�̃��x���ŕς��B���j�[�N�����X�^�[�ɂ͏����ϐg���ɂ����B"},

	//v1.1.36 �ǉ�
	{28,60,65,FALSE,TRUE,A_INT,0,0,"���z�ω�",
		"�����𒆐S�ɋ���Ȋj�M�����̃{�[���𔭐�������B����(15+30n����)�̔ӂɎg���ƈЗ͂���������B"},

	{32,32,50,FALSE,TRUE,A_CHR,0,10,"�����M�����C",
		"�u���C�v�����̃u���X����B�З͖͂��͂ɑ傫���e�����󂯂�B�Ή��ϐ������ϐ������G�ɂ͂������Â炢�B�n�`�⏰��̃A�C�e���ɂقډe�����y�ڂ��Ȃ��B"},
	{36,45,60,FALSE,TRUE,A_CHR,0,0,"�S�S�d�E�̖�",
		"�z���̒K�B����������B"},

	{40,50,70,FALSE,TRUE,A_INT,0,0,"��b��Ƃ̍ق�",
		"�G��̂�Z���ԕϐg������̉�������B�ϐg������ꂽ�G�͗אڍU���Ɠ���U�����g�p�s�\�ɂȂ�B�t���A���ړ�������ʂ̓G�ɋZ��������Ɖ��������B�����x���ȓG�⃆�j�[�N�����X�^�[�ɂ͌����Â炭�A�J�I�X�ϐ������G�ɂ͌����Ȃ��B"},
	{45,120,80,FALSE,TRUE,A_CHR,0,0,"�����̃|���|�R����",
		"���E���̑S�Ăɑ΂��Í��E�����U�����d�|����B����(15+30n����)�̔ӂɎg���ƈЗ͂���������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_mamizou(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:
		{
			int num = 3 + plev / 10;
			int dice = 3 + p_ptr->lev / 12;
			int sides = 5 + chr_adj / 8;

			if(only_info) return format("����:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�΂�T�����؂̗t������^�̌`���Ƃ��Ĕ�񂾁B");
			fire_blast(GF_MISSILE, dir, dice, sides, num, 2,(PROJECT_STOP | PROJECT_KILL | PROJECT_GRID));
		}
		break;
	case 1: 
		{
			if(only_info) return format("");
			msg_print("���Ȃ��͕ӂ���ɂ߂����c");
			probing();
			break;
		}
	case 2:
		{
			int level = 5 + plev / 2;
			int num = 2 + plev / 12;
			bool flag = FALSE;
			if(only_info) return format("�������x��:%d",level);
			for(;num>0;num--)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_ANIMAL, (PM_EPHEMERA))) flag = TRUE;
			}
			if(flag) msg_format("���b�̌����x�����B");
			else msg_format("��������Ȃ������E�E");

		}
		break;

	case 3:
		{
			int x, y;
			int dist = 1 + plev/16;
			int time = 50+plev;
			monster_type *m_ptr;
			char m_name[80];


			if(only_info) return format("�˒�:%d ���ʎ���:%d�^�[��",dist,time);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("�߂��̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				project_length = 0; //v1.1.76 �ϐg��̓��Z�̎˒����Z���s��Ή��@�����ɕK�v����̂��͕s��
				return NULL;
			}

			project_length = 0; //v1.1.76 �ϐg��̓��Z�̎˒����Z���s��Ή�

			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				int r_idx = m_ptr->r_idx;
				int chance;
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				chance = plev + p_ptr->stat_ind[A_INT]+3+p_ptr->stat_ind[A_WIS]+3 - r_ptr->level;
				if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance -= 20;
				if(r_ptr->flagsr & RFR_RES_ALL) chance = 0;

				msg_format("���Ȃ���%s�ɉ����悤�Ǝ��݂��E�E",m_name);
				if(p_ptr->wizard) msg_format("(chance:%d)",chance);
				if(randint0(100) < chance)
				{
					metamorphose_to_monster(r_idx,time);
				}
				else if(chance < 1)
					msg_format("%s�ɂ͂ƂĂ�������ꂻ���ɂȂ��B",m_name);
				else
					msg_print("�ω��Ɏ��s�����I");

			}
		}
		break;


	case 4:
		{
			int rad = 3;
			int base = plev * 10 + chr_adj * 10;

			if(FULL_MOON)
			{
				rad *= 2;
				base *= 2;
			}

			if(only_info) return format("����:�`%d",base / 2);	
			msg_format("���Ȃ��͑��z�ɕϐg�����I");
			project(0, rad, py, px, base, GF_NUKE, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			break;
		}
		break;


	case 5:
		{
			int dam = 50 + plev * 2 + chr_adj * 10;
			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("����Ȓ���������C���f���o���ꂽ�I");
		
			fire_ball(GF_STEAM, dir, dam, -2);
			break;
		}

	case 6:
		{
			int level = plev / 2 + chr_adj;
			int num = 1 + chr_adj / 4;
			bool flag = FALSE;
			if(only_info) return format("�������x��:%d",level);
			for(;num>0;num--)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_TANUKI, (PM_FORCE_PET))) flag = TRUE;
			}
			if(flag) msg_format("�z���̒K���Ăяo�����I");
			else msg_format("�N������Ȃ������E�E");

		}
		break;

	case 7://��b��Ƃ̍ق�
		{
			int y, x;
			monster_type *m_ptr;
			int time = 3;

			if(only_info) return format("����:%d+1d%d",time,time);

			if(p_ptr->tim_general[0])
			{
				msg_print("���łɏp�����s�����B");
				return NULL;
			}

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					char m_name[80];
					int chance = plev + (p_ptr->stat_ind[A_INT]+3) * 2;
					monster_race *r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);

					msg_format("���Ȃ���%s�ɉ��𐁂��t�����B",m_name);
					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance = chance*2/3;

					if(r_ptr->flagsr & (RFR_RES_CHAO | RFR_RES_ALL))
					{
						msg_format("%s�ɂ͊��S�ȑϐ�������I",m_name);
					}
					else if(p_ptr->magic_num1[0] == cave[y][x].m_idx)
					{
						msg_format("���Ɍ����Ă���B");

					}
					else if( chance < r_ptr->level)
					{
						msg_format("%s�ɂ͌��������ɂȂ��B",m_name);
					}
					else if( randint1(chance) < r_ptr->level)
					{
						msg_format("%s�ɂ͌����Ȃ������B",m_name);
					}
					else
					{
						p_ptr->magic_num1[0] = cave[y][x].m_idx;
						set_tim_general(time+randint1(time),FALSE,0,0);
						lite_spot(m_ptr->fy,m_ptr->fx);
						msg_format("%s�̓R�~�J���Ȏp�ɕϐg�����I",m_name);
						(void)set_monster_csleep(cave[y][x].m_idx, 0);
						anger_monster(m_ptr);
					}
				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}

			break;
		}
	case 8:
		{
			int dice = plev / 2;
			int sides = 5 + chr_adj / 2;

			if(FULL_MOON) sides *= 2;
			if(only_info) return format("����:%dd%d*2",dice,sides);
			msg_print("�s��ȒK���q���n�܂����I");
			project_hack2(GF_DARK,dice,sides,0);
			project_hack2(GF_SOUND,dice,sides,0);

		}
		break;


	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::��t��p�Z*/
class_power_type class_power_chemist[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�򔠎��[",
		"�A�C�e����򔠂ɓ����B���f�ތn�A�C�e������������Ȃ��B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�򔠊m�F",
		"�򔠂̒����m�F����B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�򔠎�o��",
		"�򔠂���A�C�e�����o���B"},
	{5,5,20,FALSE,FALSE,A_INT,0,0,"���}�蓖",
		"HP�������񕜂��łƐ؂菝�����Â���B���x�����オ��ƞN�O�ƌ��o�����Â���B"},
	{10,4,35,FALSE,FALSE,A_INT,0,0,"��������",
		"�L�m�R�����Ӓ肷��B"},
	{16,6,40,FALSE,FALSE,A_INT,0,0,"�H������",
		"�H���𒲒B����B"},
	{ 24,18,70,FALSE,FALSE,A_INT,0,0,"�A������",
		"�t���A�ɂ���L�m�R��f�ތn�A�C�e���̑��݂��܂��Ɋ��m����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_chemist(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			display_list_inven2();
			return NULL; //���邾���Ȃ̂ōs��������Ȃ�

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			int dice = 4 + plev / 4;
			int sides = 7;
			if(only_info) return format("��:%dd%d",dice,sides);
			set_cut(0);
			if(plev > 9)set_stun(0);
			set_poisoned(0);
			if(plev > 19)set_image(0);
			hp_player(damroll(dice,sides));
			break;
		}
	case 4: //��������
		{
			if (only_info) return format("");
			if (!ident_spell_2(2)) return NULL;
		}
		break;

	case 5:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "�H���𐶐�";
			msg_print("�H�ׂ���쑐�Ȃǂ��W�߂��B");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}

	//�����i���m
	case 6:
		{
			if(only_info) return format("");
			msg_print("���Ȃ��͎��͂̒������n�߂��E�E");
			search_specific_object(6);
			break;
		}




	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::���ۗp���Z*/
class_power_type class_power_raiko[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"���t���~�߂�",
		"�����t���Ȃ炻����~�߂�B�s���͂�����Ȃ��B"},
	{7,6,20,FALSE,TRUE,A_STR,0,2,"�\��{����",
		"���������̃{�[������B"},
	{12,6,25,FALSE,TRUE,A_INT,0,0,"����A���m�c�d�~",
		"���t���͎��͂̃����X�^�[�����m����B���x�����オ��Ɗ��m�ł�����̂�������B"},
	{16,12,30,FALSE,TRUE,A_STR,0,0,"�����h�p�[�J�X",
		"�n�k���N�����ă_���W����������A�ꎞ�I�Ɏm�C���g�𓾂�B"},
	{20,25,45,FALSE,TRUE,A_WIS,0,7,"���_�̓{��",
		"�����𒆐S�ɓd�������̋��͂ȃ{�[���𔭐�������B���t���̉��y������Ύ~�܂�B"},
	{25,0,0,FALSE,TRUE,A_DEX,0,0,"�u���[���f�B�[�V���[",
		"�������̕���ɉ���������\�͂��g�p����B������x�g�����񂾕���łȂ��Ɣ\�͂𔭊��ł��Ȃ��B�Œ�A�[�e�B�t�@�N�g�͂��̔\�͂̑ΏۂɂȂ�Ȃ��B���t���̉��y������Ύ~�܂�B"},
	{30,30,55,TRUE,FALSE,A_STR,0,10,"�I���^�l�C�g�X�e�B�b�L���O",
		"�����𒆐S�ɍ��������̋��͂ȃ{�[���𔭐�������B�������ǂɗאڂ��Ă��Ȃ��Ǝg���Ȃ��B���t���̉��y������Ύ~�܂�B"},
	{35,32,60,FALSE,TRUE,A_CHR,0,0,"�{��̃f���f������",
		"���t���A���E���̃����_���ȓG�֓d���������̃r�[�������������B"},
	{40,40,60,FALSE,TRUE,A_CON,0,10,"�����a���ۃ��P�b�g",
		"�j�Б����̃��P�b�g����B�З͂͌���HP��1/3�ɂȂ�B"},
	{43,88,70,FALSE,TRUE,A_STR,0,0,"�t�@���^�W�b�N�E�[�t�@�[",
		"���E���̑S�Ăɑ΂����͂ȍ��������U�����s���B���t���̉��y������Ύ~�܂�B"},
	{47,48,70,FALSE,TRUE,A_CHR,0,0,"�v���X�e�B���r�[�g",
		"���t���͉����Ǝm�C���g�𓾂Ēʏ�U���̏���s�����Ԃ���������B�A���ōU�����s���΍s���قǌ��������傫���Ȃ�A�ō��l�{���ōU������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::���ۓ��Z p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]���̂Ɏg����*/
//p_ptr->magic_num1[10�`29]�����ꖂ�@�Ɏg����H
cptr do_cmd_class_power_aux_raiko(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�̂��~�߂�@�s��������Ȃ�
	case 0:
		{
			if(only_info) return "";
			stop_raiko_music();
			return NULL;

		}
		break;
	case 1:
		{
			int dist = 2 + plev / 6;
			int rad = 2 + plev / 20;
			int dice = 4 + plev / 4;
			int sides = 7 + chr_adj / 2;
	
			if(only_info) return  format("�˒�:%d ����:%dd%d",dist,dice,sides);
			project_length = dist;
			stop_tsukumo_music();
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���ۂ𓊂������I");
			fire_ball(GF_SOUND, dir, damroll(dice,sides),rad);

		}
		break;
	case 2://�A���m�c�d�~
		{
			if(only_info) return format("���m�p��");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_RAIKO_AYANOTUDUMI, SPELL_CAST);

			break;
		}
		break;
	case 3:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("��n�����������ݖ炵���I");
			earthquake(py,px,5);
			set_afraid(0);
			set_hero(time,FALSE);

			break;
		}
		//v1.1.85 �ǉ��ŗ��_��������
	case 4:
		{
			int rad = 3 + plev / 12;
			int base = plev * 12 + chr_adj * 10;
			if(only_info) return format("����:�`%d",base / 2);	
			stop_raiko_music();
			msg_format("���_�������N����A�����������I");
			project(0, rad, py, px, base, GF_ELEC, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			project(0, rad/2, py, px, rad, GF_MAKE_STORM, (PROJECT_GRID | PROJECT_HIDE), -1);
			break;
		}
	case 5:
		{
			if(only_info) return "";

			if(!cast_monspell_new()) return NULL;
			break;
		}
	case 6:
		{
			int rad = 4 + plev / 16;
			int base = plev * 8 + adj_general[p_ptr->stat_ind[A_STR]] * 8;
			if(only_info) return format("����:�`%d",base / 2);	
			stop_raiko_music();
			msg_format("���Ȃ��͕ǂɌ������Ďv���؂�̓��肵���I");
			project(0, rad, py, px, base, GF_SOUND, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			break;
		}
	case 7://�{��̃f���f������
		{
			int base = plev * 3;
			int sides = chr_adj * 5;
			if(only_info) return format("����:%d+1d%d",base,sides);

			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_RAIKO_DENDEN, SPELL_CAST);

			break;
		}
		break;
	case 8:
		{
			int dam = p_ptr->chp / 3;
			if(dam<1) dam = 1;
			if (dam > 800) dam = 800;

			if(only_info) return format("����:%d",dam);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���ۂ��΂𕬂��Ĕ�񂾁I");
			fire_rocket(GF_ROCKET, dir, dam,2);
			break;
		}
	case 9:
		{
			int dam = plev * 5 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);
			stop_raiko_music();
			msg_print("�d�ቹ���_���W������h�炵���I");
			project_hack2(GF_SOUND,0,0,dam);
			break;
		}
		break;
	case 10://�v���X�e�B���r�[�g
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_RAIKO_PRISTINE, SPELL_CAST);

			break;
		}
		break;

	default:
		if(only_info) return  format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�O������p�Z*/
//�Î�1��tim_general[0]�A�Î�2��tim_general[1]���g��
class_power_type class_power_sangetsu[] =
{
	{8,6,20,FALSE,TRUE,A_STR,0,3,"�A�C�X�f�B�]���o�[",
		"�Ή������̃r�[������B"},
	{15,5,25,FALSE,TRUE,A_INT,0,0,"���f�T",
		"�G��̂�����������B�����G�ɂ͌����Ȃ��B��Ԃ͌��ʂ�������B"},
	{18,16,40,FALSE,TRUE,A_INT,0,0,"�Î�",
		"��莞�ԁA���������Ƌ߂��̃����X�^�[�����@�ƈꕔ�̍s�����g���Ȃ��Ȃ�B������ǂނƌ��ʂ��������ꏭ������s�����Ԃ�������B"},
	{18,0,0,FALSE,TRUE,A_INT,0,0,"�Î����",
		"���Z�u�Î�v�̌��ʂ���������B���̍s���ɂ��^�[������͒ʏ�̔����B"},
	{ 20,10,40,FALSE,TRUE,A_INT,0,0,"���`���t���N�V����",
		"�������܂����ė��ꂽ�ꏊ�����F����B�����������h�A��J�[�e���ŎՂ�ꂽ�������邱�Ƃ͂ł��Ȃ��B" },
	{23,20,45,FALSE,TRUE,A_WIS,0,5,"�g���v�����e�I",
		"���͂̃����_���Ȉʒu��覐Α����̃{�[����A���Ŕ���������B"},
	{27,20,45,FALSE,TRUE,A_DEX,0,0,"�G���t�B���L���m�s�[",
		"��莞�Ԕ��˔\�͂𓾂�B"},
	{30,20,0,FALSE,FALSE,A_DEX,0,0,"���e�ݒu",
		"�����ɔ��e��ݒu����B���e�́u�ʏ�́v10�^�[���Ŕ��������a3�̔j�Б����̋��͂ȃ{�[���𔭐�������B���̔����ł̓v���C���[���_���[�W���󂯂�B���e�͈ꕔ�̑����U���ɎN�����ƗU�������蔚���^�C�~���O���ς�邱�Ƃ�����B"},
	{32,30,50,FALSE,TRUE,A_INT,0,0,"���f�U",
		"��莞�ԁA���͂̓G��f�킹��B���E�ɗ���Ȃ��G�ɑ΂��Ă͌��ʂ������B��Ԃ͌��ʎ��Ԃ��Z���Ȃ�B"},
	{34,24,60,FALSE,TRUE,A_CHR,0,8,"�g���v�����C�g",
		"�w��ʒu�ɑM�������̋���ȃ{�[���𔭐�������B"},
	{37,20,60,FALSE,TRUE,A_CON,0,0,"�I���I���x���g",
		"��莞�Ԏm�C���g�Ɛg�̔\�͏㏸�𓾂�B"},
	{40,33,75,FALSE,TRUE,A_INT,0,0,"�u���C�N�L���m�s�[",
		"�^�[�Q�b�g�����X�^�[�ɂ������Ă��閂�@���ʂ���������B"},
	{43,60,70,FALSE,TRUE,A_STR,0,0,"�T���o�[�X�g",
		"���E���̑S�Ăɑ΂��A�M�������̍U�����s���B�T�j�[��������~�ς��Ă���ƈЗ͂����������B"},
	{45,80,80,FALSE,TRUE,A_WIS,0,0,"�Î�U",
		"��莞�ԁA�t���A�S��̃����X�^�[�����@�ƈꕔ�̍s�����g���Ȃ��Ȃ�B���������͐������󂯂Ȃ��B"},
	{48,99,80,FALSE,TRUE,A_CHR,0,0,"�X���[�t�F�A���[�Y",
		"���������𒆐S�Ƃ����M���A�Í��A覐Α����̋���ȃ{�[����A���Ŕ���������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_sangetsu(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dice = 1 + plev / 4;
			int sides = 7;
			int base = plev/2 + chr_adj/2 ;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�M����������B");
			fire_beam(GF_FIRE, dir, base + damroll(dice,sides));
			break;
		}
	case 1: 
		{
			int x, y;
			int dist = 2 + plev / 5;
			monster_type *m_ptr;
			int power = 20 + plev;
			if(is_daytime()) power += 30;

			if(only_info) return format("�˒�:%d",dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("�T�j�[��%s�̎��͂̌i�F��c�߂��E�E",m_name);

				if(r_ptr->flags2 & RF2_SMART || r_ptr->flags3 & RF3_NO_CONF || 
					r_ptr->flagsr & RFR_RES_ALL || randint1(power) < r_ptr->level)
				{
					msg_format("���������ʂ��Ȃ������B");
				}
				else
				{
					msg_format("%s�͍��������I",m_name);
					(void)set_monster_confused(cave[y][x].m_idx,  MON_CONFUSED(m_ptr)+10 + randint1(plev/3));
				}
				set_monster_csleep(cave[y][x].m_idx, 0);
				anger_monster(m_ptr);
			}

			break;
		}

	case 2://�Î�1 tim_general[0]���g��
		{
			int time = 10+plev/5;
			int rad = plev / 5;
			if(only_info) return format("����:%d �͈�:%d",time,rad);
			if(p_ptr->tim_general[0] || p_ptr->tim_general[1])
			{
				msg_print("���łɔ\�͂��g���Ă���B");
				return NULL;
			}

			msg_print("���i�͎w��炵���B���͂̉����������B");
			set_tim_general(time,FALSE,0,0);

		}
		break;
	case 3://�Î����
		{
			if(only_info) return format("");
			if(!p_ptr->tim_general[0] && !p_ptr->tim_general[1])
			{
				msg_print("�\�͂��g���Ă��Ȃ��B");
				return NULL;
			}

			msg_print("���i�͔\�͂����������B");
			set_tim_general(0,TRUE,0,0);
			set_tim_general(0,TRUE,1,0);

		}
		break;
	case 4:
		{
			//v1.1.34 �T�j�[�́u�������܂����ė��ꂽ�ꏊ�̌��i������v�\�͂�ǉ��B
			//�܂�����t���O�𗧂ĂĂ���flying_dist�l������h�A��J�[�e���ɎՂ���悤�ɂ��čČv�Z����B
			//���ɂ���flying_dist�l���ݒ肳�ꂽ�O���b�h�݂̂��[�ցA�����X�^�[���m����B
			//�Ō�Ƀt���O��߂���flying_dist�l���Čv�Z����B
			if (only_info) return format("�͈�:%d�ړ��O���b�h", MONSTER_FLOW_DEPTH);

			if (p_ptr->blind)
			{
				msg_print("�T�j�[�͖ڂ������Ȃ��B");
				return NULL;
			}

			msg_print("���ꂽ�ꏊ�̉f�����T�j�[�̖ڂ̑O�ɍL�������c�c");
			flag_sunny_refraction = TRUE;
			update_flying_dist();
			wiz_lite(FALSE);
			detect_monsters_normal(255);
			flag_sunny_refraction = FALSE;
			update_flying_dist();

		}
		break;

	case 5:
		{
			int rad = 2 + plev / 24;
			int damage = plev  + chr_adj / 2 ;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("�O�l�Œe�����~�点���I");
			cast_meteor(damage, rad, GF_METEOR);
			break;
		}	
	case 6:
		{
			int time;
			int base = 15 + p_ptr->lev/5;
			
			if(only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("�T�j�[�͐g�\�����B");
			set_tim_reflect(time, FALSE);

			break;
		}	

	case 7:
		{
			int dam = 100+plev*4;
			if(only_info) return format("����:%d",dam);
			if(!futo_can_place_plate(py,px) || cave[py][px].special)
			{
				msg_print("�����ɂ͔��e��u���Ȃ��B");
				return NULL;
			}
			msg_print("�X�^�[�͔��e���d�|�����B");
			/*:::Mega Hack - cave_type.special�𔚒e�̃J�E���g�Ɏg�p����B*/
			cave[py][px].info |= CAVE_OBJECT;
			cave[py][px].mimic = feat_bomb;
			cave[py][px].special = 10;
			while(one_in_(5)) cave[py][px].special += randint1(20);
			note_spot(py, px);
			lite_spot(py, px);
		}
		break;
	case 8:
		{
			int base = 5+plev/5;
			if(is_daytime()) base *= 2;
			if(only_info) return format("����:%d+1d%d",base,base);
			msg_format("�T�j�[�͌��𑀂��ĎO�l�̎p���������B");
			set_tim_superstealth(base + randint1(base),FALSE, SUPERSTEALTH_TYPE_OPTICAL);
			break;
		}
	case 9:
		{
			int dice = plev/2 ;
			int sides = 10 + chr_adj / 3;
			int rad = 5;

			if(only_info) return format("����:%dd%d",dice,sides);
		
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_LITE,dir,damroll(dice,sides),rad,"�O�l�ŋ���Ȍ��������o�����I")) return NULL;

			break;
		}
	case 10:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("�X�^�[�̓T�j�[�ƃ��i�����サ���I");
			set_hero(time,FALSE);
			set_tim_addstat(A_STR,4,time,FALSE);
			set_tim_addstat(A_CON,4,time,FALSE);
			set_tim_addstat(A_DEX,4,time,FALSE);

			break;
		}
	case 11:
		{

			int m_idx;

			if(only_info) return format("");
			if (!target_set(TARGET_KILL)) return NULL;
			m_idx = cave[target_row][target_col].m_idx;
			if (!m_idx) return NULL;
			if (!player_has_los_bold(target_row, target_col)) return NULL;
			if (!projectable(py, px, target_row, target_col)) return NULL;
			msg_print("���i�͎w��炵���B");
			dispel_monster_status(m_idx);

			break;
		}
	case 12:
		{
			int dam = 40 + plev*3 + chr_adj*2;

			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) dam += 100;

			if(only_info) return format("����:%d",dam);
			msg_format("�T�j�[������ῂ��������I");
			project_hack2(GF_LITE,0,0,dam);
			lite_room(py,px);
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) sunny_charge_sunlight(-500);
			break;
		}

	case 13://�Î�2 tim_general[1]���g��
		{
			int time = 30;
			if(only_info) return format("����:%d",time);
			if(p_ptr->tim_general[1])
			{
				msg_print("���łɔ\�͂��g���Ă���B");
				return NULL;
			}

			msg_print("���i�̓t���A�S�̂̉��Ɋ������B");
			set_tim_general(time,FALSE,1,0);
			p_ptr->tim_general[0]=0; 
		}
		break;
	case 14:
		{
			int dam = plev * 4 + chr_adj * 10;
			int rad = 6;
			int i;
			if(only_info) return format("����:�ő�%d*3",dam/2);
			msg_print("�O�l�őS�͂̈ꌂ��������I");
			project(0, rad, py, px, dam, GF_LITE, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			if(p_ptr->chp>=0) project(0, rad, py, px, dam, GF_DARK, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			if(p_ptr->chp>=0) project(0, rad, py, px, dam, GF_METEOR, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
		}
		break;


	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}





/*:::�����p���Z*/
class_power_type class_power_yatsuhashi[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"���t���~�߂�",
		"�����t���Ȃ炻����~�߂�B�s���͂�����Ȃ��B"},
	{5,4,20,FALSE,TRUE,A_WIS,0,2,"���s����̋Ղ̉�",
		"�G��̂������A����������B���x��30�ȍ~�͎��E�����ׂĂ̓G�ɑ΂��Č��ʂ����������B"},
	{15,8,30,FALSE,TRUE,A_CHR,0,0,"�����̎c��",
		"���t���ɂ͔��˔\�͂𓾂�B���t����MP�����������B"},
	{20,12,35,FALSE,TRUE,A_WIS,0,0,"�V�ً̏�",
		"���t���ɗאڍU�����Ă����G�ɑ΂��J�E���^�[�ō��������̑�_���[�W��^����B���t����MP�����������B"},
	{27,24,45,FALSE,TRUE,A_CHR,0,0,"���̃A���T���u��",
		"���t���Ɏ��E���̑S�Ăɑ΂��A�d���A�����A���A�����̂����ꂩ�̑����̍U�����s����B���t����MP�����������B"},
	{30,0,0,FALSE,TRUE,A_WIS,0,0,"��ڗ����E",
		"�\��񑱂��ĉ��t�����HP��MP�������x�񕜂��X�e�[�^�X�ُ킪��������B"},
	{33,40,55,FALSE,FALSE,A_CHR,0,0,"�����ヌ�N�C�G��",
		"�����X�^�[��̂�S�\�͒ቺ�����悤�Ǝ��݂�B��R�����Ɩ����B������1.5�{�ȏ㍂�����x���̃����X�^�[�ɂ������ʂ��Ȃ��B" },
	{36,24,65,FALSE,FALSE,A_WIS,0,0,"�l�ՂƂ��ɖS��",
		"���t���Ƀ_���[�W���󂯂�Ƃ��̃_���[�W�𑊎�ɕԂ��B���t����MP�����������B���얂�@�u���Q�̌_��v�Ƃ͕ʏ����B"},
	{40,32,55,FALSE,FALSE,A_INT,0,0,"�G�R�[�`�F���o�[",
		"���t���Ɏ����̎g�����@���������ꃌ�x������l��1.5�{�����B���t����MP�����������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::�������Z p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]���̂Ɏg����*/
cptr do_cmd_class_power_aux_yatsuhashi(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�̂��~�߂�@�s��������Ȃ�
	case 0:
		{
			if(only_info) return "";
			stop_tsukumo_music();
			return NULL;

		}
		break;
	case 1:
		{
			int power = 50 + plev * 3;
			if(only_info) return format("����:%d",power);

			stop_tsukumo_music();

			if(plev < 30)
			{
				if (!get_aim_dir(&dir)) return NULL;
				msg_print("���ׂ��Ղ̉����������B");
				slow_monster(dir,power);
				confuse_monster(dir,power);
			}
			else
			{
				msg_print("���߂����Ղ̉��������n�����E�E");
				slow_monsters(power);
				confuse_monsters(power);
			}
			break;
		}
	case 2://
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_ECHO2, SPELL_CAST);
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= (PR_STATUS);
			break;
		}
	case 3://�V�ً̏�
		{
			int dice = 8 + plev / 4;
			int sides = 10 + chr_adj / 3;
			if(only_info) return format("����:%dd%d",dice,sides);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_NORIGOTO, SPELL_CAST);
			break;
		}
	case 4://���y�u���̃A���T���u���v
		{
			int base = plev * 2 + chr_adj * 2;
			if(only_info) return format("����:1d%d�p��",base);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_STORM, SPELL_CAST);

			break;
		}
		break;
	case 5://��ڗ����E concent���g��
		{
			if(only_info) return format("");
			stop_tsukumo_music();
			do_cmd_concentrate(0);
			if(p_ptr->concent == 12)
			{
				msg_print("���Ȃ������͎��F�̌��ɕ�܂ꂽ�I");
				hp_player(plev * 6);
				player_gain_mana(plev * 2);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
				set_image(0);
				restore_level();
				set_alcohol(0);
				set_asthma(0);

				reset_concentration(FALSE);
			}
			break;

		}
		break;

	case 6:
	{
		int x, y;
		int power = plev * 3 + chr_adj * 5;
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];

		if (only_info) return format("����:%d", power);

		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("�^�[�Q�b�g�����X�^�[�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_print("�����ɂ͉������Ȃ��B");
			return NULL;
		}
		r_ptr = &r_info[m_ptr->r_idx];

		stop_tsukumo_music();

		monster_desc(m_name, m_ptr, 0);
		if (r_ptr->level < plev * 3 / 2)
		{
			msg_format("%s������ł͍��ЂƂ��t�ɐg������Ȃ������B", m_name);
			break;
		}

		msg_format("%s�֌��������R�̑����Ȃ�t�ł��I", m_name);
		project(0, 0, y, x, power, GF_DEC_ALL, (PROJECT_KILL | PROJECT_JUMP), -1);
	}
	break;

	case 7://�l�ՂƂ��ɖS��
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_JINKIN, SPELL_CAST);
			break;
		}
	case 8://�G�R�[�`�F���o�[
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_ECHO, SPELL_CAST);
			break;
		}

	default:
		if(only_info) return  format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::�فX�p���Z*/
class_power_type class_power_benben[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"���t���~�߂�",
		"�����t���Ȃ炻����~�߂�B�s���͂�����Ȃ��B"},
	{5,3,20,FALSE,TRUE,A_WIS,0,2,"�_�����ɂ̏��̉�",
		"�˒��̒Z�����������̃{�[������B"},
	{12,9,30,FALSE,TRUE,A_CHR,0,0,"���Ƃ̑剅��",
		"���͂̃����_���ȓG�ɑ΂��􂢂ōU�����鉉�t���s���B���t����MP�����������B"},
	{20,15,35,FALSE,TRUE,A_WIS,0,3,"�׈��Ȍܐ���",
		"�M�������̃r�[������B"},
	{27,24,45,FALSE,TRUE,A_CHR,0,0,"���̃A���T���u��",
		"���t���Ɏ��E���̑S�Ăɑ΂��A�d���A�����A���A�����̂����ꂩ�̑����̍U�����s����B���t����MP�����������B"},
	{30,0,0,FALSE,TRUE,A_WIS,0,0,"��ڗ����E",
		"�\��񑱂��ĉ��t�����HP��MP�������x�񕜂��X�e�[�^�X�ُ킪��������B"},
	{34,25,55,FALSE,FALSE,A_CON,0,7,"��M�����i",
		"�����𒆐S�Ƃ��鋐��ȍ��������̃{�[���𔭐�������B���͂̓G���N����B"},
	{40,36,65,FALSE,FALSE,A_CHR,0,0,"�X�R�A�E�F�u",
		"���t���A���E���̃����_���ȓG�ɑ΂��M�������r�[���𐔔����B���t����MP�����������B"},
	{44,48,70,FALSE,TRUE,A_CHR,0,0,"�_�u���X�R�A",
		"���t���͊i���U�������������B�U���񐔂▽�����Ƀ{�[�i�X�����A���|��Ԃł��U���ł��A�I�[���_���[�W���󂯂Ȃ��B�܂��G����̗אڍU���ɑ΂��J�E���^�[�U�����s���B���̊i���U���̂ݗאڍU�������Ă����t���r�؂�Ȃ��B���t����MP�����������B���푕�����ɂ͎g���Ȃ��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::�فX���Z p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]���̂Ɏg����*/
cptr do_cmd_class_power_aux_benben(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�̂��~�߂�@�s��������Ȃ�
	case 0:
		{
			if(only_info) return "";
			stop_tsukumo_music();
			return NULL;

		}
		break;
	case 1:
		{
			int dist = 2 + plev / 16;
			int rad = 2 + plev / 20;
			int dice = 4 + plev / 6;
			int sides = 6 + chr_adj / 5;
	
			if(only_info) return  format("�˒�:%d ����:%dd%d",dist,dice,sides);
			project_length = dist;
			stop_tsukumo_music();
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�d���ȉ����������E�E");
			fire_ball(GF_SOUND, dir, damroll(dice,sides),rad);

		}
		break;
	case 2://���Ƃ̑剅��
		{
			int dice = 7 + plev / 6;
			if(only_info) return format("����:%dd%d�p��",dice,dice);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_HEIKE, SPELL_CAST);

			break;
		}
	case 3:
		{
			int dice = 2 + plev / 3;
			int sides = 8;
			int base = plev + chr_adj * 2;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			stop_tsukumo_music();
			msg_format("�܏��̌�����������B");
			fire_beam(GF_LITE, dir, base + damroll(dice,sides));
			break;
		}
	case 4://���y�u���̃A���T���u���v
		{
			int base = plev * 2 + chr_adj * 2;
			if(only_info) return format("����:1d%d�p��",base);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_STORM, SPELL_CAST);

			break;
		}
		break;
	case 5://��ڗ����E concent���g��
		{
			if(only_info) return format("");
			stop_tsukumo_music();
			do_cmd_concentrate(0);
			if(p_ptr->concent == 12)
			{
				msg_print("���Ȃ������͎��F�̌��ɕ�܂ꂽ�I");
				hp_player(plev * 6);
				player_gain_mana(plev * 2);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
				set_image(0);
				restore_level();
				set_alcohol(0);
				set_asthma(0);

				reset_concentration(FALSE);
			}
			break;

		}
	case 6:
		{
			int dam = 300 + plev * 4 + chr_adj * 6;
			int rad = 4 + plev / 12;
			int i;
			if(only_info) return format("���a:%d ����:�ő�%d",rad,dam/2);
			msg_print("��������n��k�킹���I");
			project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			aggravate_monsters(0,FALSE);
		}
		break;
	case 7://�X�R�A�E�F�u
		{
			int dam = plev/2 + chr_adj / 2;
			int num = 5;
			if(only_info) return format("����:%d*%d�p��",dam,num);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_SCOREWEB, SPELL_CAST);

			break;
		}
	case 8://�_�u���X�R�A
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_DOUBLESCORE, SPELL_CAST);

			break;
		}

	default:
		if(only_info) return  format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::����p�Z*/
class_power_type class_power_sakuya[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�i�C�t���[",
		"�i�C�t�z���_�[�֒Z���n�A�C�e������{���[����B���[�ς݂̃X���b�g��I�����邱�Ƃő������̕���Ɠ���ւ��邱�Ƃ��ł���B�s�����Ԃ��قƂ�Ǐ���Ȃ��B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�i�C�t��o��",
		"�i�C�t�z���_�[�����{���o���B�s�����Ԃ��قƂ�Ǐ���Ȃ��B"},
	{1,0,20,FALSE,FALSE,A_DEX,0,0,"�i�C�t���",
		"�����ɗ����Ă���Z���n�A�C�e����K���ɏE���邾���E���ăi�C�t�z���_�[�Ɏ��[����B�u���̐��E�v���s���͋߂��̃i�C�t���E���B�i�C�t�z���_�[�̗e�ʂ̓��x���A�b�v�ő�����B"},
	{5,3,15,FALSE,FALSE,A_DEX,0,0,"�i�C�t�����T",
		"�ǂ����ɉB�������Ă����i�C�t�𓊂���B���̃i�C�t�͓�����ƕK��������B�ʏ�̓����Ɠ��l�ɓ����n���x�ŏ���Ԃ�����B�u���̐��E�v���s���Ȃ玞�Ԓ�~����������B"},

	{8,6,20,FALSE,TRUE,A_INT,0,0,"�o�j�V���O�G�u���V���O",
		"�������̃����_���Ȉʒu�փe���|�[�g����B"},
	{12,16,30,FALSE,TRUE,A_WIS,0,0,"�v���C�x�[�g�X�N�E�F�A",
		"��莞�ԉ�������B"},
	{16,32,40,FALSE,FALSE,A_CHR,0,0,"���̐��E",
		"���Ԃ��~����B���̎��Ԓ�~�Ɋ����͂Ȃ����A���Ԓ�~���ɂ͓G�֍U�������蓹��Ȃǂ��g�����Ƃ��ł��Ȃ��B�ˌ��Ⓤ�����s���Ǝ��Ԓ�~�����������B�܂��x�e�R�}���h��HP��X�e�[�^�X�ُ킪�񕜂��邪MP�͉񕜂��Ȃ��B"},
	{16,0,0,FALSE,FALSE,A_CHR,0,0,"���̐��E(����)",
		"���Ԓ�~����������B�s�����Ԃ��قƂ�Ǐ���Ȃ��B"},

	{20,24,35,FALSE,TRUE,A_DEX,0,7,"�E�l�h�[��",
		"���͂̃����_���ȓG�ɑ΂���ʂ̔j�Б����{���g����B�u���̐��E�v���s���Ȃ玞�Ԓ�~����������B"},
	{23,12,45,FALSE,TRUE,A_DEX,0,0,"�`�F���W�����O�}�W�b�N",
		"�w�肵���G�ƈʒu����������B�e���|�[�g������ɓ����Ȃ��ꏊ�ł͎��s����B"},
	{27,32,55,FALSE,TRUE,A_DEX,0,0,"�p�[�t�F�N�g���C�h",
		"���s��A��x�����G����̍U����������ĒZ�������e���|�[�g������Ɍ������ꏊ���ӂɔj�Б����U�����s���B�e���|�[�g�����Ɏ��s���邱�Ƃ�����B"},
	{30,50,60,FALSE,FALSE,A_DEX,0,0,"�i�C�t�����U",
		"�i�C�t�z���_�[�ɓ����Ă���Z���ނ�S�Ĉ�x�ɓ�������B�u���̐��E�v���s���Ȃ玞�Ԓ�~����������B��������߂��Ă���\�͂�������͖߂��Ă��Ȃ��B�����̎w�ւ̌��ʂ͔�������B"},

	{32,40,85,FALSE,TRUE,A_INT,0,0,"�g���l���G�t�F�N�g",
		"�w������̕ǂȂǈړ��s�n�`�̌������֏u�Ԉړ�����B�}�b�v�[�ɓ��B����Ɣ��Α��֏o��B�ʏ�̃e���|�[�g�ňړ��ł��Ȃ��ꏊ�ɂ͏o���Ȃ��B"},

	{35,44,65,FALSE,TRUE,A_CHR,0,0,"��F�̃A�i�U�[�f�B�����W����",
		"��莞�Ԕ��˂�AC�㏸�𓾂�B"},
	{38,128,70,FALSE,FALSE,A_STR,0,0,"�\�E���X�J���v�`���A",
		"���E���̓G�S�Ăɒʏ�U�����d�|����B���̍U���ł͓G����̃I�[���_���[�W���󂯂Ȃ��B�G����̋����ɂ���čU���񐔂���������B�Z���ނ𑕔����Ă��Ȃ��Ƃ��̋Z�͎g���Ȃ��B�u���̐��E�v���s���Ȃ玞�Ԓ�~����������B"},
	{41,64,75,FALSE,TRUE,A_INT,0,0,"�C���t���[�V�����X�N�E�F�A",
		"���E���̑S�Ăɑ΂���Ԙc�ȑ����̍U�����d�|����B�u���̐��E�v���s���Ȃ玞�Ԓ�~����������B"},
	{43,64,75,FALSE,TRUE,A_CHR,0,0,"�f�t���[�V�������[���h",
		"�ɂ߂ċ��͂ȓ������s���B�����ł���͕̂���݂̂Ŏg��������͕K�����ł���B�u���̐��E�v���s���Ȃ玞�Ԓ�~����������B"},
	{46,255,80,FALSE,TRUE,A_WIS,0,0,"�U�E���[���h",
		"�����~�߂�B���̂Ƃ��͓G�ւ̍U�����ł��邪���ʎ��Ԃ͋ɂ߂ĒZ���B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_sakuya(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int inven2_num = calc_inven2_num();

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("�ő�:%d��",inven2_num);
			put_item_into_inven2();
			new_class_power_change_energy_need = 20;
			break;
		}
	case 1:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			new_class_power_change_energy_need = 20;
			break;
		}

	case 2:
		{
			int j;
			bool flag_pick = FALSE;
			bool flag_max = TRUE;
			object_type forge;
			object_type *o_ptr;
			object_type *q_ptr;
			char o_name[MAX_NLEN];

			if(only_info) return format("");

			for(i=0;i<inven2_num;i++)
				if(!inven_add[i].k_idx)flag_max = FALSE;

			if(flag_max)
			{
				msg_print("����ȏ㎝�ĂȂ��B");
				return NULL;
			}

			//�t���A�̃A�C�e����S�Ē��ׂ�
			for(i=0;i<inven2_num;i++)
			{
				int item = 0;
				int item_count_temp = 0;
				if(inven_add[i].k_idx) continue;
				flag_max = FALSE;
				//���E���̃A�C�e���������_���ɑI��
				for(j=0;j<o_max;j++)
				{
					o_ptr = &o_list[j];
					if (!o_ptr->k_idx) continue;
					if (o_ptr->tval != TV_KNIFE) continue;

					if(SAKUYA_WORLD)
					{
						if(distance(py,px,o_ptr->iy,o_ptr->ix) > 2) continue;
						if (!projectable(o_ptr->iy,o_ptr->ix,py,px)) continue;
					}
					else
					{
						if(py != o_ptr->iy || px != o_ptr->ix) continue;
					}

					item_count_temp++;
					if(one_in_(item_count_temp)) item = j;
				}
				if(!item) break;
				//�I�΂ꂽ�A�C�e����ǉ��C���x���g����1�����i�[������A�C�e�������炷�B�����i�d�ʂ͍l�����Ȃ��B
				flag_pick = TRUE;
				o_ptr = &o_list[item];
				q_ptr = &forge;
				object_copy(q_ptr, o_ptr);
				q_ptr->number = 1;
				distribute_charges(o_ptr, q_ptr, 1);
				object_desc(o_name, q_ptr, 0);
				msg_format("%s���E���ăi�C�t�z���_�[�ɍ������B",o_name);
				p_ptr->total_weight += q_ptr->weight * q_ptr->number;
				object_wipe(&inven_add[i]);
				object_copy(&inven_add[i], q_ptr);
				floor_item_increase(item, -1);
				floor_item_describe(item);
				floor_item_optimize(item);

			}
			if(!flag_pick)
			{
				msg_print("�߂��Ƀi�C�t�͗����Ă��Ȃ��B");
				break;
			}


		}
		break;

	case 3:
		{
			int tx,ty;
			int quality = plev / 5;
			object_type forge;
			object_type *q_ptr = &forge;
			if(only_info) return format("���\:1d3(+%d,+%d)",quality,quality);

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
			object_known(q_ptr);
			msg_print("�i�C�t�𓊂����B");
			do_cmd_throw_aux2(py,px,ty,tx,1,q_ptr,1);
			if(ref_skill_exp(SKILL_THROWING) > 1600)
			{
				new_class_power_change_energy_need = 100 - (ref_skill_exp(SKILL_THROWING) - 1600) / 100;
				if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need -= new_class_power_change_energy_need / 5;
			}

			break;
		}

	case 4:
		{
			int dist = 15 + plev / 2;
			if(only_info) return format("����:%d",dist);
			msg_print("���Ȃ��͏u���ɏ������B");
			teleport_player(dist, 0L);
		}
		break;

	case 5:
		{
			int time = 20 + plev / 5;
			if(only_info) return format("����:%d",time);
			msg_print("���͂̎��ԂɊ������E�E");
			set_fast(time, FALSE);

			break;
		}

	case 6:
		{
			if(only_info) return format("");

			if(p_ptr->riding)
			{
				msg_print("�R�撆�͎��s�ł��Ȃ��B");
				return NULL;
			}
			if (world_player)
			{
				msg_print("���Ɏ��͎~�܂��Ă���B");
				return NULL;
			}
			sakuya_time_stop(TRUE);
			break;
		}
	case 7:
		{
			if(only_info) return format("");
			sakuya_time_stop(FALSE);
			new_class_power_change_energy_need = 20;
			break;
		}


	case 8:
		{
			bool flag = FALSE;
			int i;
			int dice = 2 + plev / 9;
			int sides = 3 + chr_adj/ 12;
			int num = 9 + plev / 3;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			msg_print("��ʂ̃i�C�t����񂾁I");
			sakuya_time_stop(FALSE);
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_SHARDS, damroll(dice,sides),1, 0,0)) flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}

		}
		break;
	case 9:
		{
			bool result;
			if(only_info) return "";

			/* HACK -- No range limit */
			project_length = -1;
			result = get_aim_dir(&dir);
			project_length = 0;
			if (!result) return NULL;
			teleport_swap(dir);
			break;
		}
	case 10:
		{
			int dam = 50 + plev * 3;
			if(only_info) return format("����:�`%d",dam/2);
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("���Ȃ��͎��͂ɋC��z��n�߂��E�E");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
			}
			break;
		}
	case 11:
		{
			int ty,tx;
			int thrown_num = 0;
			object_type *o_ptr;
			if(only_info) return format("");

			for(i=0;i<inven2_num;i++)
				if(inven_add[i].k_idx) thrown_num++;
			if(!thrown_num)
			{
				msg_print("�i�C�t�z���_�[�ɕ��킪�������Ă��Ȃ��B");
				return NULL;
			}

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

			if(thrown_num > 1)
				msg_print("����Ƀi�C�t��͂�ň�Ăɓ������I");
			else
				msg_print("�i�C�t�𓊂����B");

			for(i=0;i<inven2_num;i++)
			{
				if(!inven_add[i].k_idx) continue;
				o_ptr = &inven_add[i];

				if (do_cmd_throw_aux2(py, px, ty, tx, 1, o_ptr, 0))
				{
					//v1.1.46 �d�ʌ��������������Ă����̂Œǉ�
					p_ptr->total_weight -= inven_add[i].weight * inven_add[i].number;
					object_wipe(&inven_add[i]);
				}
			}
		}
		break;

	case 12: 
		{
			int x,y;
			int attempt = 500;
			if(only_info) return format("");

			if (p_ptr->anti_tele)
			{
				msg_format("�������e���|�[�g��j�Q���Ă���B");
				return NULL;
			}

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];

			if(cave_have_flag_bold(y,x,FF_MOVE) || cave_have_flag_bold(y,x,FF_CAN_FLY))
			{
				msg_format("�����ɂ͕ǂ��Ȃ��B");
				return NULL;
			}
			while(1)
			{
				attempt--;
				y += ddy[dir];
				x += ddx[dir];

				if(y >= cur_hgt) y = 1;
				if(y < 1) y = cur_hgt - 1;
				if(x >= cur_wid) x = 1;
				if(x < 1) x = cur_wid - 1;
				if(attempt<0)
				{
					msg_format("�g���l�����ʂ̔����Ɏ��s�����I");
					teleport_player(200,0L);
					break;
				}
				if(!cave_player_teleportable_bold(y, x, 0L)) continue;
				msg_format("���Ȃ��̓|�e���V������ǂ�˂��������I");
				teleport_player_to(y,x,0L);
				break;
			}
			break;
		}
	case 13:
		{
			int time;
			int base = p_ptr->lev/2-5;
			
			if(only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("��F�̌������Ȃ����񂾁E�E");
			set_tim_reflect(time, FALSE);
			set_shield(time, FALSE);
			break;
		}
	case 14:
		{
			if(only_info) return format("");
			sakuya_time_stop(FALSE);
			flag_friendly_attack = TRUE;
			msg_format("���Ȃ��̖ڂ��Ԃ��������E�E");
			project_hack2(GF_SOULSCULPTURE,0,0,100);
			flag_friendly_attack = FALSE;
		}
		break;
	case 15:
		{
			int base = plev * 5;
			int sides = 100 + chr_adj * 5;
			if (only_info) return format("����:%d+1d%d",base,sides);
			sakuya_time_stop(FALSE);
			msg_format("�u�ԓI�Ɏ��͂̋�Ԃ��g�傳�����I");
			project_hack2(GF_DISTORTION,1,sides,base);
		}
	break;
	case 16:
		{
			int     item;
			cptr    q, s;
			object_type *o_ptr;
			object_type forge;
			object_type *q_ptr = &forge;
			int ty,tx;
			int mult = 16;

			if(only_info) return format("�{��:%d",mult);

			item_tester_hook = object_is_melee_weapon;

			q = "�ǂ̕���𓊂��܂���? ";
			s = "����������Ă��Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

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
			object_copy(q_ptr,o_ptr);
			q_ptr->number = 1;
			if(do_cmd_throw_aux2(py,px,ty,tx,mult,q_ptr,3))
			{
				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_optimize(0 - item);
				}
			}
			break;
		}
	case 17:
		{
			if(only_info) return format("����:500");
			if (world_player)
			{
				msg_print("���Ɏ��͎~�܂��Ă���B");
				return NULL;
			}
			sakuya_time_stop(FALSE);
			world_player = TRUE;
			msg_print("�u���Ȃ��̎��Ԃ����̂��́c�v");
			msg_print(NULL);
			//v1.1.58
			flag_update_floor_music = TRUE;
			p_ptr->energy_need -= 1500;
			p_ptr->redraw |= (PR_MAP);
			p_ptr->update |= (PU_MONSTERS);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			handle_stuff();
			msg_print("�����~�܂����I");
			break;
		}


	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�u���[�N���A�~�����b�g�p*/
static bool item_tester_hook_broken_amulet(object_type *o_ptr)
{
	if(o_ptr->tval != TV_AMULET) return FALSE;

	if(object_is_cursed(o_ptr) && (object_is_known(o_ptr) || (o_ptr->ident & IDENT_SENSE))) return TRUE;

	return FALSE;
}

/*:::�����Z*/
class_power_type class_power_hina[] =
{
	{1,0,0,FALSE,FALSE,A_INT,0,0,"����m�F����",
		"���ǂꂭ�炢����܂��Ă��Ăǂ̂悤�Ȍ��ʂ��������Ă��邩���m�F����B"},
	{1,0,0,FALSE,FALSE,A_WIS,0,0,"��_�l�̃o�C�I���Y��",
		"����h�{�Ƃ��ď���󕠂𖞂����B��500�̏���ŋQ���Ԃ��疞���ɂȂ�B"},
	{1,0,20,FALSE,TRUE,A_WIS,0,0,"����z�����",
		"�������̎��ꂽ�i�̎􂢂������A��Ƃ��ċz������B"},
	{7,7,25,FALSE,TRUE,A_WIS,0,0,"�o�b�h�t�H�[�`����",
		"�^�[�Q�b�g��̂ɖ���΂��ă_���[�W��^����B��R�����Ɩ����B�_���[�W��^�����Ƃ�1/10�̖�������B"},
	{15,5,40,FALSE,TRUE,A_INT,0,0,"��m",
		"���͂̃g���b�v�����m�����ꂽ�A�C�e�����t���A�ɂ���Α�܂��Ɋ��m����B���x��30�ȍ~�͍��א��͂̃����X�^�[�����m����B"},
	{23,12,30,FALSE,TRUE,A_DEX,0,0,"�u���[�N���A�~�����b�g",
		"���ꂽ�A�~�����b�g���ЂƂ������Ĕj�Б����{�[���𔭐�������B�З͎͂􂢂̋����ŕς��B�������A�~�����b�g�͕K�����ł���B"},
	{30,28,50,FALSE,TRUE,A_WIS,0,0,"�~�X�t�H�[�`�����Y�z�C�[��",
		"�������S�̋���Ȗ�̃{�[���𔭐�������B��R�����Ɩ����B�_���[�W��^�����Ƃ�1/10�̖�������B"},
	{35,22,60,FALSE,TRUE,A_CON,0,0,"�y�C���t���[",
		"��莞�ԁA�_���[�W���󂯂����ɓ����_���[�W�𑊎�ɕԂ��悤�ɂȂ�B�^�����_���[�W��1/5�̖�������B"},
	{40,24,70,FALSE,TRUE,A_CON,0,0,"���Y�l�`",
		"�G��̂̈ړ��ƃe���|�[�g����莞�ԖW�Q����B���300�����B����ȓG��͋����G�ɂ͌��ʂ������B"},
	{47,66,70,FALSE,TRUE,A_CHR,0,0,"�􂢂̐��l�`",
		"����T���U�炵�A���E���̃����X�^�[�̖��@�𒆊m���ŖW�Q����B���^�[�����50+1d100�����B������x���s���邩�t���A���ړ�����ƋZ�����������B�n��ł͎g���Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::�����Z p_ptr->magic_num1[0]����l�Ƃ��Ďg����*/
//magic_num1[1]�𐗐l�`���l�̔����̉҂��Ƃ��Ďg��
//magic_num1[2]�𗬌Y�l�`��IDX�Ƃ��Ďg��
cptr do_cmd_class_power_aux_hina(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int yaku = p_ptr->magic_num1[0];
	int yaku_max = HINA_YAKU_MAX;

	switch(num)
	{
	case 0:
		{
			int i,x,y;
			if(only_info) return format("��:%d",yaku);
			screen_save();

			for(i=1;i<20;i++) Term_erase(16, i, 255);
			y = 1;
			x = 18;

			if(yaku > HINA_YAKU_LIMIT3)
			{
				c_put_str(TERM_RED,format("���݂̖�:%d/%d",yaku,yaku_max), y++, x);
				c_put_str(TERM_RED,format("����ȏ�̖����������Ȃ��I���̂܂܂ł͑�ςȂ��ƂɂȂ�I"), y++, x);
			}
			else if(yaku > HINA_YAKU_LIMIT2)
			{
				c_put_str(TERM_ORANGE,format("���݂̖�:%d/%d",yaku,yaku_max), y++, x);
				c_put_str(TERM_ORANGE,format("��R��o���A���͂Ɉ��o�Ă���c"), y++, x);
			}
			else if(yaku > HINA_YAKU_LIMIT1)
			{
				c_put_str(TERM_YELLOW,format("���݂̖�:%d/%d",yaku,yaku_max), y++, x);
				c_put_str(TERM_YELLOW,format("���Ȃ��͖���������ė]���C�����B"), y++, x);
			}
			else
				c_put_str(TERM_WHITE,format("���݂̖�:%d/%d",yaku,yaku_max), y++, x);
			y++;

			if(yaku > HINA_YAKU_LIMIT3)
			{
				c_put_str(TERM_ORANGE,format("��̕��S���̂�I��ł���B"), y++, x);
				c_put_str(TERM_WHITE,format("���͂̂��̂ɖ����ʂɍЂ����~�肩�����Ă���B"), y++, x);
				c_put_str(TERM_ORANGE,format("���̂��Ȃ��̏�Ԃ͐l���Łu��_�ٕρv�ƌĂ΂�Ă���炵���B"), y++, x);
			}

			if(yaku > HINA_YAKU_LIMIT2+5000)
			{
				c_put_str(TERM_ORANGE,format("���Ȃ��͌����ɓ���Ă��炦�Ȃ��B"), y++, x);
			}

			if(yaku > HINA_YAKU_LIMIT2)
				c_put_str(TERM_YELLOW,format("���Ȃ��͔������󂯂Ă���B"), y++, x);
			else if(yaku > HINA_YAKU_LIMIT1 && yaku >= 1500)
				c_put_str(TERM_WHITE,format("���Ȃ��͉B���\�͂��ቺ���Ă���B(-%d)",yaku/1500), y++, x);

			if(yaku > 9999)
			{
				c_put_str(TERM_L_GREEN,format("���Ȃ��͈Í��E�n���U�������S�ɖ���������B"), y++, x);
			}
			if(yaku > 14999)
			{
				c_put_str(TERM_L_GREEN,format("���Ȃ��͈يE�̜��܂������C�ւ̑ϐ������B"), y++, x);
			}
			if(yaku > 19999)
			{
				c_put_str(TERM_L_GREEN,format("���Ȃ��͓G����󂯂��{���g���@�𒵂˕Ԃ��B"), y++, x);
			}

			if(p_ptr->do_martialarts)
			{
				if(yaku > 443) c_put_str(TERM_WHITE,format("���Ȃ��͗אڍU����%d�̃_�C�X�{�[�i�X��(+%d,+%d)�̏C���𓾂�B",yaku/999,yaku/444,yaku/666), y++, x);
			}
			else
			{
				if(yaku > 1200) c_put_str(TERM_WHITE,format("���Ȃ��͗אڍU����%d�̃_�C�X�{�[�i�X��(+%d,+%d)�̏C���𓾂�B",yaku/4800,yaku/1200,yaku/1800), y++, x);
			}

			if(yaku > 200) c_put_str(TERM_WHITE,format("���Ȃ���AC��%d�̏C���𓾂�B",yaku/200), y++, x);

			if(yaku > 99) c_put_str(TERM_WHITE,format("���Ȃ��ɐG�ꂽ�҂�1d%d�̃_���[�W���󂯂�B",yaku/100), y++, x);
			if(yaku > 10000) c_put_str(TERM_L_GREEN,format("���Ȃ��ɐG�ꂽ�҂͍������邱�Ƃ�����B"), y++, x);
			if(yaku > 20000) c_put_str(TERM_L_GREEN,format("���Ȃ��ɐG�ꂽ�҂͞N�O�Ƃ��邱�Ƃ�����B"), y++, x);

			if(yaku > 27000) c_put_str(TERM_L_BLUE,format("���Ȃ��͒����̐��͂̓V�G���B"), y++, x);
			else if(yaku > 18000) c_put_str(TERM_L_GREEN,format("���Ȃ��͒����̐��͂ɑ΂��ē��ɋ���ׂ��͂𔭊�����B"), y++, x);
			if(yaku > 18000) c_put_str(TERM_L_BLUE,format("���Ȃ��͐l�Ԃ̓V�G���B"), y++, x);
			else if(yaku > 12000) c_put_str(TERM_L_GREEN,format("���Ȃ��͐l�Ԃɑ΂��ē��ɋ���ׂ��͂𔭊�����B"), y++, x);

			inkey();
			screen_load();

			return NULL; //�s����������Ȃ�
		}
		break;

	case 1:
		{
			int food;
			if(only_info) return format("");

			if(use_itemcard)
			{
				if(remove_all_curse())
				{
					msg_print("�J�[�h������z��������B");
					set_food(PY_FOOD_MAX - 1);
				}
			}
			else
			{
				if(p_ptr->food >= PY_FOOD_FULL)
				{
					msg_print("���͂͏\�����B");
					return NULL;
				}
				if(!yaku)
				{
					msg_print("���͂ɂ����S���Ȃ��c");
					return NULL;
				}

				food = PY_FOOD_MAX - 1 - p_ptr->food;
				if(food > yaku * 30) food = yaku * 30;

				set_food(p_ptr->food + food);
	
				hina_gain_yaku(-(food/30));
			}
		}
		break;


	case 2:
		{
			char o_name[MAX_NLEN];
			int i;
			int gain = 0;
			if(only_info) return format("");

			for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
			{
				object_type *o_ptr = &inventory[i];
				if (!o_ptr->k_idx) continue;
				if (!object_is_cursed(o_ptr)) continue;
				object_desc(o_name, o_ptr, OD_OMIT_INSCRIPTION);

				if (o_ptr->curse_flags & TRC_PERMA_CURSE)
				{
					msg_format("%s�̎􂢂͂��܂�ɂ������I�􂢂��z������Ȃ������I",o_name);
					o_ptr->curse_flags &= (TRC_CURSED | TRC_HEAVY_CURSE | TRC_PERMA_CURSE);
					gain += 1000 + randint1(1000);
				}
				else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
				{
					msg_format("%s�̏d���􂢂��z��������I",o_name);
					gain += 500 + randint1(500);
					o_ptr->curse_flags = 0L;
				}
				else
				{
					msg_format("%s�̎􂢂��z��������B",o_name);
					gain += 200 + randint1(200);
					o_ptr->curse_flags = 0L;
				}
				//���Ӓ�̏ꍇ�ȈՊӒ�
				if(!object_is_known(o_ptr))
				{
					o_ptr->ident |= (IDENT_SENSE);
					o_ptr->feeling = value_check_aux1(o_ptr);
				}
				p_ptr->update |= PU_BONUS;
				p_ptr->window |= (PW_INVEN | PW_EQUIP);


			}
			if(gain) 
				hina_gain_yaku(gain);
			else 
				msg_print("����z���ł�����̂𑕔����Ă��Ȃ��B");

		}
		break;

	case 3:
		{
			int dice = 12;
			int sides = 10 + plev / 5;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���������B");
			fire_ball_hide(GF_YAKU, dir, damroll(dice,sides),0);
			break;
		}
	//��m
	case 4:
		{
			int rad = 20 + plev / 5;
			if(only_info) return format("�͈�:%d+��",rad);	
			msg_print("���Ȃ��͖��T�����E�E");
			search_specific_object(5);

			detect_traps(rad,TRUE);
			if(plev>29) detect_monsters_evil(rad);

			break;
		}

	case 5:
		{
			cptr q,s;
			int item;
			object_type *o_ptr;		
			int mag;
			int dam;

			if(only_info) return format("");
			item_tester_hook = item_tester_hook_broken_amulet;

			q = "�ǂ̃A�~�����b�g�𓊂��܂���? ";
			s = "��ꂽ�A�~�����b�g���Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0) o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

			dam = count_bits(o_ptr->curse_flags) * 100;
			if(o_ptr->curse_flags & TRC_PERMA_CURSE) dam += 800;
			else if(o_ptr->curse_flags & TRC_HEAVY_CURSE) dam += 200;
			if(object_is_artifact(o_ptr)) dam += 800;
			else if(object_is_ego(o_ptr)) dam += 200;
			else dam += 100;

			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���������A�~�����b�g���e�����I");
			fire_ball(GF_SHARDS, dir, dam,3);

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

		}
		break;

	case 6:
		{
			int dam = 200 + plev * 6;
			int rad = 4 + plev / 15;
			if(only_info) return format("����:�`%d",dam/2);
			
			msg_print("����Ȃ��̎�����Q�������I");
			project(0,rad,py,px,dam,GF_YAKU,PROJECT_KILL,-1);
			break;
		}
	case 7:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("����:%d+1d%d",base,base);

			time = base + randint1(base);
			set_tim_eyeeye(time,FALSE);
			break;
		}

	case 8:
		{
			int y, x, base;
			monster_type *m_ptr;

			base = 10 + p_ptr->lev / 5;
			if(only_info) return format("����:%d",base);

			if(yaku < 300) 
			{
				msg_print("���̓��Z���g�������Ȃ��B");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			x = target_col;
			y = target_row;

			if(dir != 5 || !target_okay() || !projectable(y,x,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("��Q������%s��߂����I",m_name);

				project(0, 0, m_ptr->fy, m_ptr->fx, base, GF_NO_MOVE, PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE, -1);
				hina_gain_yaku(-300);

				//p_ptr->magic_num1[2] = cave[y][x].m_idx;
				//set_tim_general(base,FALSE,0,0);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			break;
		}

	case 9:
		{

			if(only_info) return format("");

			if (!(p_ptr->special_defense & SD_HINA_NINGYOU))
			{
				msg_print("���Ȃ��͖���T���U�炵�n�߂��c");

				p_ptr->special_defense |= SD_HINA_NINGYOU;
				p_ptr->redraw |= (PR_STATUS);
			}
			else
			{
				msg_print("���Ȃ��͖���T���U�炷�̂��~�߂��B");

				p_ptr->special_defense &= ~(SD_HINA_NINGYOU);
				p_ptr->redraw |= (PR_STATUS);
			}

			break;
		}



	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�h���~�[�E�X�C�[�g��p�Z*/
class_power_type class_power_doremy[] =
{

	{1,10,0,FALSE,FALSE,A_CHR,0,0,"����H�ׂ�",
		"�אڂ��������Ă���G�̖���H�ו��𖞂����B�_���W�����u���̐��E�v�ł͖����ԂłȂ��G�ɂ����ʂ����������̓G�͏��ł���B���̏ꍇ��R����邱�Ƃ�����A���j�[�N�����X�^�[�ɂ͌����Ȃ��B"},
	{7,7,30,FALSE,TRUE,A_WIS,0,2,"��F�̈���",
		"�w��^�[�Q�b�g��̂����m���Ŗ��点��B���x��25�ȍ~�͐����ϐ������G�ɂ������\��������B�_���W�����u���̐��E�v�ł͐��_�U�����s���B"},
	{16,25,35,FALSE,FALSE,A_INT,0,0,"���f�f",
		"�L�͈͂̒n�`�A�A�C�e���A�g���b�v�A�����X�^�[�����m���A����Ɏ��E���̓G�̏��𓾂�B�_���W�����u���̐��E�v�ł����g���Ȃ��B"},
	{20,18,45,FALSE,TRUE,A_WIS,0,4,"���F�̏D��",
		"�w��^�[�Q�b�g�Ɉ����������čU������B���̍U���ł͖������G���N�����ɂ����B�G�������Ă��邩�_���W�����u���̐��E�v�Ŏg�����Ƃ��͈З͂������B"},
	{24,27,50,FALSE,TRUE,A_INT,0,5,"�h���[���L���b�`���[",
		"�G��̂ɖ������_���[�W��^����B���̍U���œ|�����G�ɕϐg���邱�Ƃ��ł���B�ϐg���̓��{���E�A�~�����b�g�ȊO�̑���������������A�ϐg���������X�^�[�̔\�͂�'U'�R�}���h�Ŏg�����Ƃ��ł���B�_���W�����u���̐��E�v�łȂ��Ǝg���Ȃ��B"},
	{27,0,80,FALSE,FALSE,A_WIS,0,0,"������",
		"HP��MP���͂��ɉ񕜂���B�_���W�����u���̐��E�v�ł͉񕜗ʂ�������B"},
	{30,27,60,FALSE,TRUE,A_WIS,0,5,"�����F�̖���",
		"���E���̓G�𖰂点��B�����ϐ��𖳎����邪���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B�_���W�����u���̐��E�v�ł͍��З͂̍����U���ɂȂ�B"},
	{32,40,65,FALSE,TRUE,A_INT,0,0,"�h���[���G�N�X�v���X",
		"�t���A�̔C�ӂ̏ꏊ�ֈړ�����B�ʏ�̃e���|�[�g�ŐN���ł��Ȃ��ꏊ�ɂ͈ړ��ł��Ȃ��B�_���W�����u���̐��E�v�ł����g���Ȃ��B"},
	{35,50,65,FALSE,TRUE,A_WIS,0,0,"�����F�̓�����",
		"���͂Ȉ��������̃{�[������B�_���W�����u���̐��E�v�ł͂���ɈЗ͂������B" },
	{38,48,60,FALSE,TRUE,A_INT,0,0,"�o�^�t���C�T�v�����e�[�V����",
		"������t���A���Đ��������B�t���A�̕ω��̑O�ɂ͈�u�̒x������������B�_���W�����u���̐��E�v�ł����g���Ȃ��B"},
	{42,60,70,FALSE,TRUE,A_WIS,0,0,"���F�̋���",
		"�G��̂ɋ��͂Ȑ��_�U�����d�|����B��R�����Ɩ����B���j�[�N�����X�^�[�ɂ͂���R����₷���A�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ������B�_���W�����u���̐��E�v�ł͂���ɈЗ͂������B"},
	{44,80,75,FALSE,TRUE,A_INT,0,0,"���g",
		"�Z���ԕ��g���ēG����̍U���ɓ�����Â炭�Ȃ�B" },
	{47,90,80,FALSE,TRUE,A_CHR,0,0,"�����̉F����",
		"�����𒆐S�ɋ��͂ȈÍ������̃{�[���𔭐�������B�_���W�����u���̐��E�v�ł͎��E���U���ɂȂ�B" },

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_doremy(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;
	bool in_dream = (IN_DREAM_WORLD);

	switch(num)
	{

	case 0:
		{
			int x, y;
			char m_name[160];
			monster_type *m_ptr;
			int food = 5000;
			int power = 50 + plev;
			if(only_info) return format("����:%d",power);

			if(p_ptr->food >= PY_FOOD_MAX) food = 0;
			else if(p_ptr->food + food >= PY_FOOD_MAX) food = PY_FOOD_MAX - 1 - p_ptr->food;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (!cave[y][x].m_idx || !(m_ptr->ml) )
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			monster_desc(m_name,m_ptr,0);
			if(in_dream)
			{
				if(genocide_aux(cave[y][x].m_idx,power,TRUE,0,""))
				{
					msg_format("%s�̖�����������������B",m_name);
					if(food) set_food(p_ptr->food + food);
				}
			}
			else
			{
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if (!MON_CSLEEP(m_ptr))
				{
					msg_format("%s�͋N���Ă���B",m_name);
					return NULL;
				}
				else if(r_ptr->flags2 & (RF2_EMPTY_MIND))
				{
					msg_format("%s�̖��͂��Ȃ��̏ܖ��ł���悤�Ȃ��̂ł͂Ȃ������B",m_name);
					break;
				}
				else if(m_ptr->mflag & MFLAG_SPECIAL)
				{
					msg_format("%s�̖��͂����ܖ��ς݂��B",m_name);
					break;
				}
				else if(r_ptr->flags2 & (RF2_WEIRD_MIND))
				{
					msg_format("%s�̖��͂قƂ�ǖ������Ȃ��E�E",m_name);
					food /= 10;
				}
				else if(r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN) || is_gen_unique(m_ptr->r_idx))
				{
					msg_format("%s�̖�����������������B",m_name);
				}
				else
				{
					msg_format("%s�̖��͂�����Ɣ����Ŕ��������Ȃ��B",m_name);
					food /= 3;
				}
				if(food) set_food(p_ptr->food + food);
				m_ptr->mflag |= MFLAG_SPECIAL; //����t���O�𗧂Ăē�x�ڈȍ~�𖳌���
				(void)set_monster_csleep(cave[y][x].m_idx, 0);

			}
		}
		break;

	case 1:
	case 3:
	case 10:
		{
			int x, y;
			int dice,sides;
			int power;
			int typ;
			monster_type *m_ptr;

			if(num == 1 && !in_dream)
			{
				power = 50 + plev * 4;
				if(only_info) return format("����:%d",power);
			}
			else if(num == 1)
			{
				typ = GF_REDEYE;
				dice = 9 + plev / 3;
				sides = 8 + chr_adj / 6;

			}
			else if(num == 3)
			{
				typ = GF_NIGHTMARE;
				dice = 6 + plev / 4;
				sides = 13 + chr_adj / 4;

				if(in_dream) dice *= 2;

			}
			else
			{
				typ = GF_COSMIC_HORROR;

				dice = plev;
				sides = 20 + chr_adj / 3;

				if(in_dream) dice = dice * 3 / 2;


			}
		
			if(only_info) return format("����:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flags2 & RF2_EMPTY_MIND)
				{
					msg_format("%s�͐��_�������Ȃ��悤���B",m_name);
					break;
				}

				if(num == 1 && !in_dream)
				{
					msg_format("%s�𖲂̐��E�֗U�����E�E",m_name);
					if(plev > 24) project(0,0,y,x,power,GF_STASIS,(PROJECT_KILL|PROJECT_JUMP),-1);
					else	project(0,0,y,x,power,GF_OLD_SLEEP,(PROJECT_KILL|PROJECT_JUMP),-1);
				}
				else
				{
					if(num == 1) msg_format("%s�ֈ������������B",m_name);
					else if(num == 3) msg_format("%s�֏D�����������B",m_name);
					else msg_format("%s�֋������������B",m_name);

					project(0,0,y,x,damroll(dice,sides),typ,(PROJECT_KILL|PROJECT_JUMP),-1);
				}
			}
		}
		break;

	case 2:
		{
			int rad = 25 + plev / 2;
			if (only_info) return format("�͈�:%d",rad);
			msg_print("�ӂ�̗l�q�𕪐͂����E�E");
			map_area(rad);
			detect_all(rad);
			detect_monsters_normal(rad);
			probing();
			break;
		}

	case 4:
		{
			int x, y;
			int dist = 7;
			int dam = plev * 2 + 100;
			monster_type *m_ptr;
			char m_name[80];

			if(only_info) return format("�˒�:%d ����:%d",dist,dam);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("�߂��̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			monster_desc(m_name, m_ptr, 0);

			if(!in_dream && !MON_CSLEEP(m_ptr))
			{
				msg_format("%s�͖����Ă��Ȃ��B",m_name);
				return NULL;
			}
			else
			{
				int r_idx = m_ptr->r_idx;
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if(r_ptr->flags2 & RF2_EMPTY_MIND)
				{
					msg_format("%s�͐��_�������Ȃ��悤���B",m_name);
					break;
				}

				msg_format("%s�̖�����荞�����Ƃ����E�E",m_name);
				project(0,0,y,x,dam,GF_MISSILE,(PROJECT_KILL|PROJECT_JUMP),-1);
				if(!m_ptr->r_idx)
				{
					metamorphose_to_monster(r_idx,-1);
				}
			}
		}
		break;

	case 5:
		{
			if(only_info) return format("");
			if(in_dream)
			{
				msg_print("���Ȃ��͎����̖��̏C�������݂��E�E");
				player_gain_mana(plev/10+randint1(plev/10));
				hp_player(plev/3+randint1(plev/3));
			}
			else
			{
				msg_print("���Ȃ��͐Q���𗧂Ďn�߂��E�E");
				player_gain_mana(1+randint1(plev/12));
				hp_player(plev/7+randint1(plev/7));
			}
		}
		break;



	case 6:
		{
			int power = 50 + plev * 2;
			if(in_dream) power += 90 + chr_adj * 2;
			if(only_info) return format("����:%d",power);

			if(in_dream)
			{
				msg_print("���͂ɍ����������炵���B");
				project_hack2(GF_CONFUSION,0,0,power);
			}
			else
			{
				msg_print("�����������o�𐶂ݏo�����B");
				stasis_monsters(power);
			}

		}
		break;

	case 7:
		{
			if(only_info) return format("����:������");
			if (!dimension_door(D_DOOR_DREAM)) return NULL;
		}
		break;

	case 8:
	{
		int dice = 5 + plev / 2;
		int sides = 10 + chr_adj / 5;

		if (in_dream) dice *=2;

		if (only_info) return format("����:%dd%d", dice, sides);
		if (!get_aim_dir(&dir)) return NULL;

		msg_print("�d�ꂵ�����������グ��...");
		fire_ball(GF_NIGHTMARE, dir, damroll(dice, sides), 3);

		break;
	}


	case 9:
		{
			if(only_info) return format("����:�����ϗe");
			if (p_ptr->inside_arena)
			{
				msg_print("�����̓��Z�͎g���Ȃ��B");
				return NULL;
			}

			msg_print("���͂̌i�F���ڂ₯���E�E");
			/*:::0�ł͂܂����͂�*/
			p_ptr->alter_reality = 1;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;

	case 11:
		{
			int dice = 8;
			int base = 8;
			if (only_info) return format("����:%d+1d%d", base, dice);
			set_multishadow(base + randint1(dice), FALSE);
			break;
		}
	case 12:
		{
			int rad = 2;
			int base = p_ptr->lev * 15 + chr_adj * 25;
			if (only_info) return format("����:�`%d", base / 2);

			msg_format("���Ȃ��͂��̏���Í��̉F����Ԃւƕς����I");
			if (in_dream)
			{
				project_hack2(GF_DARK, 0, 0, base / 2);
			}
			else
			{
				project(0, rad, py, px, base, GF_DARK, PROJECT_KILL, -1);
			}
			break;
		}




	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�N���E���s�[�X��p�Z*/
class_power_type class_power_clownpiece[] =
{

	{6,8,30,FALSE,FALSE,A_CHR,0,5,"���Ԃ��Ă�",
		"�d����z���Ƃ��ČĂяo���B"},
	{12,20,35,TRUE,TRUE,A_WIS,0,0,"���[�U�[",
		"���͑����̃r�[������B"},
	{20,25,30,FALSE,FALSE,A_CHR,0,5,"�w���G�N���v�X",
		"�����镔���𖾂邭���A���E���̓G�ɐ��_�U�����s���A����ɍ����E���|�����悤�Ƃ���B����Ƀ��x��30�ŞN�O�A���x��40�ŋ���m�����ǉ��B�u���N���E���s�[�X�̏����v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{25,80,40,TRUE,TRUE,A_WIS,0,0,"�t���b�V���A���h�X�g���C�v",
		"��^�̑M�������r�[������B"},
	{30,120,50,TRUE,TRUE,A_WIS,0,0,"�O���C�Y�C���t�F���m",
		"�����̂���ꏊ�ɋ���ȉΉ������̃{�[���𕡐��񔭐�������B"},
	{35,66,55,FALSE,TRUE,A_CHR,0,0,"�o�[�X�e�B���O�O���b�W",
		"�����𒆐S�ɋ���Ȓn�������{�[���𔭐�������B�З͂͂���܂Ŏ󂯂��_���[�W�̗ݐϒl�̔����B�g�����тɃ_���[�W�ݐϒl�����Z�b�g�����B" },

	{40,240,60,TRUE,TRUE,A_WIS,0,0,"�X�g���C�v�h�A�r�X",
		"���E���̑S�Ăɑ΂��n���̍��Α����̍U�����s���B"},
	{45,320,70,TRUE,TRUE,A_CHR,0,0,"�t�F�C�N�A�|��",
		"���E���̃^�[�Q�b�g�̈ʒu��覐Α����̋���ȋ���3�񔭐�������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_clownpiece(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			int level = plev + 10;
			int num = 1 + plev / 16;
			bool flag = FALSE;
			if(only_info) return format("�������x��:%d",level);
			for(;num>0;num--)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_FAIRY, (PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;
			}
			if(flag) msg_format("���Ԃ��Ă񂾁I");
			else msg_format("�N������Ȃ������E�E");

		}
		break;
	case 1:
		{
			int dice = 3 + plev / 4;
			int sides = 8;
			int base = 20 + plev * 2 + chr_adj * 2;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			fire_beam(GF_MANA, dir, base + damroll(dice,sides));
			break;
		}
	case 2:
		{
			int power = 50 + p_ptr->lev * 4;
			if(power < 100) power = 100;
			if (only_info) return format("����:%d",power);
			msg_format("���C�̌������͂���ߏグ���I");
			project_hack2(GF_REDEYE,0,0,power);
			confuse_monsters(power);
			turn_monsters(power);
			if (plev > 29) stun_monsters(power);
			if (plev > 39) project_hack(GF_BERSERK, power);
			lite_room(py, px);
		}
		break;
	case 3:
		{
			int damage = 50 + p_ptr->lev * 6 + chr_adj * 5;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�����̌������O����ガ�������I");
			fire_spark(GF_LITE,dir,damage,2);
		}
		break;
	case 4:
		{
			int num = 4 + (plev-3) / 11;
			int dam = 200 + plev * 2 + chr_adj * 2;
			int rad = 2 + plev / 12;
			int i;
			if(only_info) return format("����:�ő�%d * %d",dam/2,num);
			msg_print("�M�g�����͂��Ă��������I");
			for (i = 0; i < num; i++) project(0, rad, py, px, dam, GF_FIRE, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			break;
		}
		break;
	case 5:
		{
			int rad, dam;

			dam = p_ptr->magic_num1[0] - p_ptr->magic_num1[1];

			if (dam > 9999) dam = 9999;

			rad = 1 + dam / 1200;

			if (only_info) return format("����:�ő�%d",dam / 2);

			if (dam< 1000)
			{
				msg_print("���̋Z�͂����ƃ_���[�W���󂯂Ȃ��Ǝg���Ȃ��B");
				return NULL;
			}

			msg_format("���݂̃p���[�������������I");
			project(0, rad, py, px, dam, GF_NETHER, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);

			p_ptr->magic_num1[1] = p_ptr->magic_num1[0];

			break;
		}


	case 6:
		{
			int dam = plev * 10 + chr_adj * 10;
			if(dam < 400) dam = 400;

			if(only_info) return format("����:%d",dam);
			msg_format("�n���̍��΂��n�ʂ��畬���o�����I");
			project_hack2(GF_HELL_FIRE,0,0,dam);	
			break;
		}
	case 7:
		{
			int x, y;
			int dist = 7;
			int num = 3;
			int rad = 5;
			monster_type *m_ptr;
			int dice = 10;
			int sides = plev * 2;
		
			if(only_info) return format("�˒�:%d ����:%dd%d*%d",dist,dice,sides,num);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("������_�����Ƃ͂ł��Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s��ڊ|���ċ���覐΂��������I",m_name);
				for(;num>0;num--)
				{
					project(0,rad,y,x,damroll(dice,sides),GF_METEOR,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM),-1);
					if(!m_ptr->r_idx) break;
				}
			}

			break;
		}




	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�����J�p���Z*/
class_power_type class_power_lyrica[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�����~�߂�",
		"���t�łĂ��鉹������Ύ~�߂�B�s���͂�����Ȃ��B"},
	{8,8,20,FALSE,TRUE,A_CHR,0,2,"�t�@�c�B�I�[�����t",
		"�����𒆐S�Ƃ������������̃{�[���𔭐�������B"},
	{17,12,35,FALSE,TRUE,A_DEX,0,0,"�t�@���g���m�C�Y",
		"�G�̍U����������ĒZ�����e���|�[�g����悤�ɂȂ�B����Ɏ��s���邱�Ƃ�����B"},
	{24,24,50,FALSE,TRUE,A_INT,0,0,"�\�E���m�C�Y�t���[",
		"���͂̃����X�^�[�A���A㩁A�A�C�e�������m����B"},
	{30,60,65,FALSE,FALSE,A_CHR,0,10,"���C�u�|���^�[�K�C�X�g",
		"���E���S�Ăɑ΂����͂ȍ��������U�����s���B�s�����Ԃ�1.5�{����A���̖͂����Ă���G���N����B"},

	{35,8,70,FALSE,FALSE,A_WIS,50,0,"�����J�E�\�����C�u",
		"���@�h��㏸�ƌ��f�U���ւ̑ϐ��𓾂鉹�y��t�ł�B�^�[�����Ƃ�MP��4�����B"},
	{ 40,60,75,FALSE,FALSE,A_CHR,0,10,"�v���Y���R���`�F���g",
		"���E���S�Ăɑ΂��M���Ɛ��̕��������U�����s���B����ɑM���E�����ϐ��̂Ȃ��G���m���ɍ���������B�s�����Ԃ�1.5�{�����B" },

	{44,99,80,FALSE,TRUE,A_CHR,0,0,"��ԃR���`�F���g�O���b�\",
		"�߂��̎w�肵���^�[�Q�b�g�̈ʒu�ɋ��͂Ȗ������̃{�[���𔭐�������B�З͖͂��͂ɑ傫���ˑ�����B�s�����Ԃ�1.5�{�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::�����J���Z p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]���̂Ɏg����*/
cptr do_cmd_class_power_aux_lyrica(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�̂��~�߂�@�s��������Ȃ�
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1:
		{
			int base = 30 + plev * 3 + chr_adj * 4;
			int rad = 2 + plev / 12;

			if(only_info) return format("�͈�:%d ����:�`%d",rad,base/2);
			msg_format("��ȉ����ӂ���񂾁E�E");
			project(0,rad,py,px,base,GF_SOUND,(PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID|PROJECT_ITEM),-1);

			break;
		}
	case 2:
		{

			if(only_info) return format("");
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("���Ȃ��͂�������Ǝd���݂��ς܂����E�E");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
			}
			break;
		}
	case 3:
		{
			int rad = plev / 2;
			if(rad < 12) rad = 12;
			if (only_info) return format("�͈�:%d",rad);
			detect_all(rad);

			break;
		}

	case 4:
		{
			int dam = plev * 4 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);
			msg_print("�O�o�����W�܂�A�j��I�ȑ������N�������I");
			project_hack2(GF_SOUND,0,0,dam);
			aggravate_monsters(0,FALSE);
			new_class_power_change_energy_need = 150;
			msg_print("�o�B�͋A���čs�����B");
			break;
		}

		break;
	case 5:
		{
			if(only_info) return  format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_LYRICA_SOLO, SPELL_CAST);

			break;
		}

	case 6:
	{
		int dam = plev * 3 + chr_adj * 3;
		if (only_info) return format("����:%d", dam);
		msg_print("�O�o�����W�܂�A���f�I�Ȍ���������I");
		project_hack2(GF_RAINBOW, 0, 0, dam);
		new_class_power_change_energy_need = 150;
		msg_print("�o�B�͋A���čs�����B");

		break;
	}

	case 7: 
		{
			int x, y;
			int dist = 5;
			int rad = 6;
			int dice = 15+chr_adj/2;
			int sides = plev;
			monster_type *m_ptr;

			if(only_info) return format("����:%dd%d �˒�:%d",dice,sides,dist);

			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("�O�o�����W�܂�A%s�̂��߂̑����Ȃ�t�ł��I",m_name);
				project(0,rad,y,x,damroll(dice,sides),GF_DISP_ALL,(PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL),-1);
				new_class_power_change_energy_need = 150;
				msg_print("�o�B�͋A���čs�����B");
			}
			break;
		}

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���������p���Z*/
class_power_type class_power_merlin[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�����~�߂�",
		"���t�łĂ��鉹������Ύ~�߂�B�s���͂�����Ȃ��B"},
	{8,10,20,FALSE,TRUE,A_CHR,0,2,"�q�m�t�@���^�Y��",
		"���������̃{�[������B"},
	{17,20,35,FALSE,TRUE,A_STR,0,0,"�g�����y�b�g�\�E��",
		"��莞�Ԏm�C���g�ƕ����h��㏸�𓾂�B���@�h��\�͂��啝�ɒቺ����B���X����m������B"},
	{24,30,50,FALSE,TRUE,A_WIS,0,0,"�\�E���S�[�n�b�s�[",
		"��莞�ԉ����Ɠ��̋����𓾂�B���������͂̓G���N�����E���̓G����������B"},
	{30,60,65,FALSE,FALSE,A_CHR,0,10,"���C�u�|���^�[�K�C�X�g",
		"���E���S�Ăɑ΂����͂ȍ��������U�����s���B�s�����Ԃ�1.5�{����A���̖͂����Ă���G���N����B"},
	{35,36,70,FALSE,FALSE,A_WIS,50,0,"���������E�n�b�s�[���C�u",
		"���͂̓G�̖��@��������ቺ�����A�U���s�����m���ŖW�Q���鉹��炵������B�����x���ȓG�ɂ͌����ɂ����B�^�[�����Ƃ�MP��18�����B"},
	{ 40,60,75,FALSE,FALSE,A_CHR,0,10,"�v���Y���R���`�F���g",
		"���E���S�Ăɑ΂��M�������U�����s���B����ɑM���E�����ϐ��̂Ȃ��G���m���ɍ���������B�s�����Ԃ�1.5�{�����B" },

	{44,99,80,FALSE,TRUE,A_CHR,0,0,"��ԃR���`�F���g�O���b�\",
		"�߂��̎w�肵���^�[�Q�b�g�̈ʒu�ɋ��͂Ȗ������̃{�[���𔭐�������B�З͖͂��͂ɑ傫���ˑ�����B�s�����Ԃ�1.5�{�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::�����������Z p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]���̂Ɏg����*/
cptr do_cmd_class_power_aux_merlin(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�̂��~�߂�@�s��������Ȃ�
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1:
		{
			int rad = 2 + plev / 16;
			int dice = 3 + plev / 4;
			int sides = 9;
			int base = plev + chr_adj * 2;

			if(only_info) return format("���a:%d ����:%d+%dd%d",rad, base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�g�����y�b�g���甚���������ꂽ�I");
			fire_ball(GF_SOUND, dir, base + damroll(dice,sides),rad);
			break;
		}
	case 2:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("����:%d+1d%d",base,base);
			msg_format("���Ȃ��̓g�����y�b�g�����炩�ɐ����炵���I");
			time = base + randint1(base);

			if(one_in_(4))
				set_shero(time,FALSE);
			else
				set_hero(time,FALSE);
			set_shield(time,FALSE);
			break;
		}	
	case 3:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("����:%d+1d%d",base,base);
			msg_format("�z�C�ȉ��y���_���W�����ɋ����n�����I");
			time = base + randint1(base);

			set_tim_addstat(A_STR,1+plev/12,time,FALSE);
			set_tim_addstat(A_CON,1+plev/12,time,FALSE);
			set_tim_addstat(A_DEX,1+plev/12,time,FALSE);
			set_fast(time,FALSE);

			aggravate_monsters(0,TRUE);

			break;
		}

	case 4:
		{
			int dam = plev * 4 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);
			msg_print("�O�o�����W�܂�A�j��I�ȑ������N�������I");
			project_hack2(GF_SOUND,0,0,dam);
			aggravate_monsters(0,FALSE);
			new_class_power_change_energy_need = 150;
			msg_print("�o�Ɩ��͋A���čs�����B");
			break;
		}

		break;
	case 5:
		{
			if(only_info) return  format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MERLIN_SOLO, SPELL_CAST);

			break;
		}
	case 6:
	{
		int dam = plev * 3 + chr_adj * 3;
		if (only_info) return format("����:%d", dam);
		msg_print("�O�o�����W�܂�A���f�I�Ȍ���������I");
		project_hack2(GF_RAINBOW, 0, 0, dam);
		new_class_power_change_energy_need = 150;
		msg_print("�o�Ɩ��͋A���čs�����B");

		break;
	}


	case 7: 
		{
			int x, y;
			int dist = 5;
			int rad = 6;
			int dice = 15+chr_adj/2;
			int sides = plev;
			monster_type *m_ptr;

			if(only_info) return format("����:%dd%d �˒�:%d",dice,sides,dist);

			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("�O�o�����W�܂�A%s�̂��߂̑����Ȃ�t�ł��I",m_name);
				project(0,rad,y,x,damroll(dice,sides),GF_DISP_ALL,(PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL),-1);
				new_class_power_change_energy_need = 150;
				msg_print("�o�Ɩ��͋A���čs�����B");
			}
			break;
		}

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::���i�T�p���Z*/
class_power_type class_power_lunasa[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�����~�߂�",
		"���t�łĂ��鉹������Ύ~�߂�B�s���͂�����Ȃ��B"},
	{8,9,20,FALSE,TRUE,A_CHR,0,2,"�O�@���l���E�f���E�W�F�X",
		"���������̃r�[������B"},
	{17,16,35,FALSE,TRUE,A_DEX,0,4,"�X���[�T�E���h",
		"�w�肵���ꏊ�ɒx�ݑ����̏��^�̃{�[���𔭐�������B"},
	{24,32,50,FALSE,TRUE,A_WIS,0,0,"�m�C�Y�������R���[",
		"���E���̓G�������A�����A���|������B���x��35�ŞN�O���ʂ��ǉ�����B"},
	{30,60,65,FALSE,FALSE,A_CHR,0,10,"���C�u�|���^�[�K�C�X�g",
		"���E���S�Ăɑ΂����͂ȍ��������U�����s���B�s�����Ԃ�1.5�{����A���̖͂����Ă���G���N����B"},

	{35,24,70,FALSE,FALSE,A_WIS,50,0,"���i�T�E�\�����C�u",
		"�߂��̓G�̍s�������킸���ɒx�点�鉹��炵������B�����x���ȓG�ɂ͌����ɂ����B�^�[�����Ƃ�MP��12�����B"},

	{40,60,75,FALSE,FALSE,A_CHR,0,10,"�v���Y���R���`�F���g",
		"���E���S�Ăɑ΂��M�������U�����s���B����ɑM���E�����ϐ��̂Ȃ��G���m���ɍ���������B�s�����Ԃ�1.5�{�����B" },

	{44,99,80,FALSE,TRUE,A_CHR,0,0,"��ԃR���`�F���g�O���b�\",
		"�߂��̎w�肵���^�[�Q�b�g�̈ʒu�ɋ��͂Ȗ������̃{�[���𔭐�������B�З͖͂��͂ɑ傫���ˑ�����B�s�����Ԃ�1.5�{�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::���i�T���Z p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]���̂Ɏg����*/
cptr do_cmd_class_power_aux_lunasa(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�̂��~�߂�@�s��������Ȃ�
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1:
		{
			int dice = 2 + plev / 4;
			int sides = 6;
			int base = plev + chr_adj;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���@�C�I��������d���ȉ������ꂾ�����B");
			fire_beam(GF_SOUND, dir, base + damroll(dice,sides));
			break;
		}
	case 2:
		{
			int base = p_ptr->lev * 3 + chr_adj * 3;
			int rad = plev / 15;

			if(only_info) return format("���a:%d ����:%d",rad,base);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_INACT, dir, base, rad,"�d�ꂵ�������ӂ���񂾁E�E")) return NULL;

			break;
		}
	case 3:
		{
			int power = p_ptr->lev * 4;
			if(power < 100) power = 100;

			if (only_info) return format("����:%d",power);
			msg_format("�A�C�ȉ��������n�����E�E");
			if(plev > 34) stun_monsters(power);
			confuse_monsters(power);
			slow_monsters(power);
			turn_monsters(power);
			break;
		}

	case 4:
		{
			int dam = plev * 4 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);
			msg_print("�O�o�����W�܂�A�j��I�ȑ������N�������I");
			project_hack2(GF_SOUND,0,0,dam);
			aggravate_monsters(0,FALSE);
			new_class_power_change_energy_need = 150;
			msg_print("���B�͋A���čs�����B");
			break;
		}

		break;
	case 5:
		{
			if(only_info) return  format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_LUNASA_SOLO, SPELL_CAST);

			break;
		}

	case 6:
	{
		int dam = plev * 3 + chr_adj * 3;
		if (only_info) return format("����:%d", dam);
		msg_print("�O�o�����W�܂�A���f�I�Ȍ���������I");
		project_hack2(GF_RAINBOW, 0, 0, dam);
		new_class_power_change_energy_need = 150;
		msg_print("���B�͋A���čs�����B");
		break;
	}

	break;
	case 7: 
		{
			int x, y;
			int dist = 5;
			int rad = 6;
			int dice = 15+chr_adj/2;
			int sides = plev;
			monster_type *m_ptr;

			if(only_info) return format("����:%dd%d �˒�:%d",dice,sides,dist);

			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("�O�o�����W�܂�A%s�̂��߂̑����Ȃ�t�ł��I",m_name);
				project(0,rad,y,x,damroll(dice,sides),GF_DISP_ALL,(PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL),-1);
				new_class_power_change_energy_need = 150;
				msg_print("���B�͋A���čs�����B");
			}
			break;
		}

	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�A���X��p�Z*/
class_power_type class_power_alice[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�l�`����",
		"�l�`�ɕ������������B�l�`�ɂ���đ����ł��镐��̎�ނƏd�ʂ��ς��A�������Ă�l�`������B����̏C���l�E�X���C�E�����E�؂ꖡ�ȊO�̔\�͂͌��ʂ𔭊����Ȃ��B���̏ꍇAC�E�ϐ��ȊO�̔\�͂͌��ʂ𔭊����Ȃ��B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�l�`��������",
		"�l�`�̑������Ă���A�C�e�����O���B"},
	{8,15,30,FALSE,TRUE,A_INT,0,5,"���ʌ��̏�C�l�`",
		"�M�������̃r�[������B"},
	{ 12,5,40,FALSE,FALSE,A_DEX,0,0,"�g���b�v����",
		"�g���b�v�𔭓�������r�[������B���������g���b�v�Ƀ����X�^�[���������ނ��Ƃ��ł���B�v���C���[���͈͓��ɂ���ƃ_���[�W���󂯂�B" },
	{16,20,40,FALSE,TRUE,A_DEX,0,0,"�l�`�쐬",
		"�����X�^�[�u�l�`�v��z���Ƃ��Đ��̏�������B"},
	{20,20,45,FALSE,TRUE,A_WIS,0,0,"�g���b�v���C���[",
		"�߂��̃����X�^�[��̂��ړ��֎~��Ԃɂ���B����ȃ����X�^�[��͋��������X�^�[�ɂ͒E�o����₷���B" },

	{24,30,50,FALSE,TRUE,A_DEX,0,0,"�A�[�e�B�t���T�N���t�@�C�X",
		"�A�C�e���u�l�`�v�𓊝����Ή������̔����𔭐�������B�З͂͐l�`��HP��1/2(�ő�1600)�ƂȂ�B"},
	{28,40,60,FALSE,TRUE,A_INT,0,0,"�X�[�T�C�h�p�N�g",
		"�z���̃����X�^�[�u�l�`�v��S�Ă��̏�Ŕ���������B"},
	{32,45,75,FALSE,TRUE,A_WIS,0,0,"�h�[���Y�E�H�[",
		"���ꂽ�ꏊ�ɑ΂��Đl�`�ɂ��ʏ�U�����s���B"},
	{36,50,80,FALSE,TRUE,A_INT,0,10,"��݂�H���l�`",
		"�Í������̋��͂ȃr�[������B"},
	{40,50,85,FALSE,TRUE,A_DEX,0,0,"���^�[���C�i�j���g�l�X",
		"�A�C�e���u�l�`�v�𓊝������͑����̑唚���𔭐�������B�З͂͐l�`��HP��1/3(�ő�3200)�ƂȂ�B"},
	{45,70,85,FALSE,TRUE,A_CHR,0,0,"�O�����M�j�������̉��l",
		"���E���̓G��̂��w�肵�A���̓G�Ɏː����ʂ����z���̐l�`�S�Ă��疳�����r�[�����������B�r�[���ꔭ���Ƃ�MP3�������B�z���̐l�`�����̃r�[���ŏ������Ƃ͂Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_alice(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;

			p_ptr->update |= PU_BONUS;
			update_stuff();
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			p_ptr->update |= PU_BONUS;
			update_stuff();
			break;
		}


	case 2:
		{
			dice = 2 + plev / 3;
			sides = 8;
			base = plev + chr_adj * 2;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("��C�l�`���r�[����������I");
			fire_beam(GF_LITE, dir, base + damroll(dice,sides));
			break;
		}


	case 3:
	{
		int range = 6 + p_ptr->lev / 4;
		if (only_info) return format("�͈�:%d", range);
		if (!get_aim_dir(&dir)) return NULL;

		msg_print("���Ȃ��͖��������ɔ��킹��...");
		fire_beam(GF_ACTIV_TRAP, dir, 0);


		break;
	}

	case 4:
		{
			bool flag = FALSE;
			int max = 1 + p_ptr->lev / 10;
			if(only_info) return format("�ő�:%d��",max);

			for(i=0;i<max;i++) if(summon_named_creature(0, py, px, MON_ALICE_DOLL, PM_FORCE_PET)) flag = TRUE;
			if(flag) msg_print("�l�`��������B");
			else msg_print("�l�`�̍쐬�Ɏ��s�����B");

			break;
		}

	case 5:
	{
		int power = plev / 2;
		int range = 3 + plev / 12;
		int x, y;
		monster_type *m_ptr;

		if (only_info) return format("�˒�:%d ����:%d", range,power);

		project_length = range;
		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_print("�����ɂ͉������Ȃ��B");
			return NULL;
		}
		else
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);
			msg_format("%s�̎���ɖ��͂ŋ����������𒣂菄�点���B", m_name);
			project(0, 0, m_ptr->fy, m_ptr->fx, power, GF_NO_MOVE, PROJECT_JUMP | PROJECT_KILL, -1);
		}
	}
	break;


	case 6:
	case 10:
		{
			int     item;
			cptr    q, s;
			object_type *o_ptr;
			monster_race *r_ptr;
			int typ;
			int rad;

			if(only_info) return format("");

			item_tester_tval = TV_FIGURINE;


			q = "�ǂ̐l�`�𓊂��܂���? ";
			s = "�l�`�������Ă��Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];
	
			r_ptr = &r_info[o_ptr->pval];

			if(r_ptr->flags1 & RF1_FORCE_MAXHP)
				damage = r_ptr->hdice * r_ptr->hside;
			else
				damage = damroll(r_ptr->hdice,r_ptr->hside);
			if(num == 4)
			{
				damage /= 2;			
				if(damage > 1600) damage = 1600;
				if(!damage) damage = 1;
				typ = GF_FIRE;
				rad = 3;
			}
			else
			{
				damage /= 3;			
				if(damage > 3200) damage = 3200;
				if(!damage) damage = 1;
				typ = GF_MANA;
				rad = 6;

			}

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�l�`�𓊂������I");
			fire_ball(typ, dir, damage,rad);
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

			break;
		}
	case 7:
		{
			monster_type *m_ptr;
			int xx,yy;
			int damage = 30 + plev * 2 + chr_adj;
			bool flag = FALSE;
			if(only_info) return format("����:��̂ɂ�%d",damage);
			msg_format("���Ȃ��͐l�`�����ɋN���w�߂��o�����I");

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (m_ptr->r_idx != MON_ALICE_DOLL) continue;
				if (!is_pet(m_ptr)) continue;
				flag = TRUE;
				xx = m_ptr->fx;
				yy = m_ptr->fy;
				delete_monster_idx(i);
				project(0,3,yy,xx,damage,GF_FIRE,PROJECT_KILL | PROJECT_GRID | PROJECT_JUMP | PROJECT_ITEM,-1);
				
			}
			if(!flag) msg_print("����������������l�`�����Ȃ������E�E");

		}
		break;

	case 8:
		{
			int range = 2 + (plev-32) / 8;
			int x, y;
			monster_type *m_ptr, *m2_ptr;

			if(range < 2) range = 2; //paranoia

			if(only_info) return format("�˒�:%d",range);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("�l�`������%s�ڊ|���Ĕ�т��������I",m_name);
				py_attack(m_ptr->fy,m_ptr->fx,0);

			}
		}
		break;



	case 9:
		{
			dice = plev / 2;
			sides = 10 + chr_adj / 10;
			base = plev * 3 + chr_adj * 3;

			if(dice < 15) dice = 15;
			if(base < 100) base = 100;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�H���l�`���r�[����������I");
			fire_spark(GF_DARK, dir, base + damroll(dice,sides),1);
			break;
		}
	case 11:
		{
			int dam = plev + chr_adj;
			int x, y;
			monster_type *m_ptr, *m2_ptr;
			bool flag = FALSE;

			if(only_info) return format("����:%d * �s��",dam);

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				msg_print("�S���̕���̖����オ�����I");
				for (i = 1; (i < m_max && m_ptr->r_idx); i++)
				{
					m2_ptr = &m_list[i];
					if (m2_ptr->r_idx != MON_ALICE_DOLL) continue;
					if (!is_pet(m2_ptr)) continue;
					if(!projectable(m2_ptr->fy,m_ptr->fx,y,x)) continue;
					flag = TRUE;
					project(i,0,y,x,dam,GF_BANKI_BEAM2,(PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU),-1);
					p_ptr->csp -= 3;
					if(p_ptr->csp <= 70) break;
				}
				if(!flag) msg_print("�E�E�C�������B");
			}
		}
		break;
	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}





/*:::�T�j�[��p�Z*/
class_power_type class_power_sunny[] =
{
	{8,6,20,FALSE,TRUE,A_STR,0,2,"�A�O���b�V�u���C�g",
		"�M�������̃r�[������B��Ԃ͈З͂�������B"},
	{15,5,25,FALSE,TRUE,A_INT,0,0,"���f�T",
		"�G��̂�����������B�����G�ɂ͌����Ȃ��B��Ԃ͌��ʂ�������B"},
	{20,10,40,FALSE,TRUE,A_INT,0,0,"���`���t���N�V����",
		"�������܂����ė��ꂽ�ꏊ�����F����B�����������h�A��J�[�e���ŎՂ�ꂽ�������邱�Ƃ͂ł��Ȃ��B" },

	{24,20,35,FALSE,TRUE,A_CON,0,4,"�T���V���C���u���X�g",
		"�����镔���𖾂邭���A���E���S�ĂɑM�������U�����s���B�����������̒~�ς��s�\���ȏꍇ�͌��Ɏア�����X�^�[�ɂ����_���[�W��^�����Ȃ��B��Ԃ͈З͂�������B"},
	{32,30,50,FALSE,TRUE,A_INT,0,0,"���f�U",
		"��莞�ԁA���͂̓G��f�킹��B���E�ɗ���Ȃ��G�ɑ΂��Ă͌��ʂ������B��Ԃ͌��ʎ��Ԃ��Z���Ȃ�B"},
	{40,45,70,FALSE,TRUE,A_STR,0,7,"�t�F�C�^���t���b�V��",
		"�����𖾂邭���A�אڂ����G��̂ɑM�������_���[�W��^���A�N�O�A����������B��Ԃ͌��ʂ�������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_sunny(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			dice = 3 + p_ptr->lev / 5;
			sides = 5;
			base = p_ptr->lev / 2 + chr_adj / 2;

			if(is_daytime())
			{
				sides *= 2;
				base *= 2;
			}
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) base += p_ptr->lev;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�r�[����������I");
			fire_beam(GF_LITE, dir, base + damroll(dice,sides));
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) sunny_charge_sunlight(-100);

			break;
		}
	case 1: 
		{
			int x, y;
			int dist = 2 + plev / 5;
			monster_type *m_ptr;
			int power = 20 + plev;
			if(is_daytime()) power += 30;

			if(only_info) return format("�˒�:%d",dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("���Ȃ���%s�̎��͂̌i�F��c�߂��E�E",m_name);

				if(r_ptr->flags2 & RF2_SMART || r_ptr->flags3 & RF3_NO_CONF || 
					r_ptr->flagsr & RFR_RES_ALL || randint1(power) < r_ptr->level)
				{
					msg_format("���������ʂ��Ȃ������B");
				}
				else
				{
					msg_format("%s�͍��������I",m_name);
					(void)set_monster_confused(cave[y][x].m_idx,  MON_CONFUSED(m_ptr)+10 + randint1(plev/3));
				}
				set_monster_csleep(cave[y][x].m_idx, 0);
				anger_monster(m_ptr);
			}

			break;
		}
	case 2:
		{
			//v1.1.34 �T�j�[�́u�������܂����ė��ꂽ�ꏊ�̌��i������v�\�͂�ǉ��B
			//�܂�����t���O�𗧂ĂĂ���flying_dist�l������h�A��J�[�e���ɎՂ���悤�ɂ��čČv�Z����B
			//���ɂ���flying_dist�l���ݒ肳�ꂽ�O���b�h�݂̂��[�ցA�����X�^�[���m����B
			//�Ō�Ƀt���O��߂���flying_dist�l���Čv�Z����B
			if (only_info) return format("�͈�:%d�ړ��O���b�h", MONSTER_FLOW_DEPTH);

			if (p_ptr->blind)
			{
				msg_print("�ڂ������Ȃ��I");
				return NULL;
			}

			msg_print("���ꂽ�ꏊ�̉f�������Ȃ��̖ڂ̑O�ɍL�������c�c");
			flag_sunny_refraction = TRUE;
			update_flying_dist();
			wiz_lite(FALSE);
			detect_monsters_normal(255);
			flag_sunny_refraction = FALSE;
			update_flying_dist();

		}
		break;



	case 3:
		{
			int	typ = GF_LITE_WEAK;
			int dam = 40 + plev + chr_adj;
			if(!is_daytime()) dam /= 2;
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1)
			{
				typ = GF_LITE;
				dam += plev * 3 / 5;
			}

			if(only_info) return format("����:%d",dam);
			msg_format("���Ȃ���ῂ��������I");
			project_hack2(typ,0,0,dam);
			lite_room(py,px);
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) sunny_charge_sunlight(-250);

			break;
		}
	case 4:
		{
			int base = 5+plev/5;
			if(is_daytime()) base *= 2;
			if(only_info) return format("����:%d+1d%d",base,base);
			msg_format("���Ȃ��͌��𑀂��Ďp���������I");
			set_tim_superstealth(base + randint1(base),FALSE, SUPERSTEALTH_TYPE_OPTICAL);
			break;
		}
	case 5:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 5 + chr_adj * 5;
			if(!is_daytime()) damage /= 2;
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) damage = damage * 3 / 2;

			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�ɋߊ��A�ˑR���z�̂悤�Ɍ������I",m_name);
				lite_room(py,px);

				if(!(r_ptr->flagsr & (RFR_RES_LITE | RFR_RES_ALL)))
				{
					if(!(r_ptr->flags3 & RF3_NO_CONF))
					{
						msg_format("%s�͍��������I",m_name);
						(void)set_monster_confused(cave[y][x].m_idx,  MON_CONFUSED(m_ptr)+10 + randint1(plev/2));
					}
					if(!(r_ptr->flags3 & RF3_NO_STUN))
					{
						msg_format("%s�͞N�O�Ƃ����I",m_name);
						(void)set_monster_stunned(cave[y][x].m_idx,  MON_STUNNED(m_ptr)+10 + randint1(plev/2));
					}
				}
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_LITE,PROJECT_KILL,-1);

				if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) sunny_charge_sunlight(-1000);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	



	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���i��p�Z*/
class_power_type class_power_lunar[] =
{

	{9,6,20,FALSE,TRUE,A_INT,0,4,"���i�e�B�b�N���C��",
		"�אڂ����G�S�ĂɈÍ������U�����s���B���Ԃ͈З͂�������B"},
	{18,16,40,FALSE,TRUE,A_INT,0,0,"�Î�T",
		"��莞�ԁA�������g�Ƌ߂��̃����X�^�[�����@�ƈꕔ�̍s�����g���Ȃ��Ȃ�B������ǂނƌ��ʂ��������ꏭ������s�����Ԃ�������B"},
	{18,0,0,FALSE,TRUE,A_INT,0,0,"�Î����",
		"���Z�u�Î�v�̌��ʂ���������B���̍s���ɂ��^�[������͒ʏ�̔����B"},
	{27,35,50,FALSE,TRUE,A_WIS,0,5,"���[�����C�g�E�H�[��",
		"���̍L���Í������̃��[�U�[�U������B���Ԃ͈З͂�������B"},
	{36,44,75,FALSE,TRUE,A_INT,0,7,"���i�T�C�N����",
		"�^�[�Q�b�g�̈ʒu�ɈÍ������̃{�[���𔭐�������B���Ԃ͈З͂�������B"},
	{45,80,80,FALSE,TRUE,A_WIS,0,0,"�Î�U",
		"��莞�ԁA�t���A�S��̃����X�^�[�����@�ƈꕔ�̍s�����g���Ȃ��Ȃ�B�����͐������󂯂Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_lunar(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dam = 10 + plev * 4 + chr_adj * 3;
			if(is_daytime()) dam /= 2;

			if(only_info) return format("����:%d ",dam/2);
			msg_print("�������~�蒍�����E�E");
			project(0, 1, py, px, dam,GF_DARK, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);

			break;
		}
	case 1://�Î�1 tim_general[0]���g��
		{
			int time = 10+plev/5;
			int rad = plev / 5;
			if(only_info) return format("����:%d �͈�:%d",time,rad);
			if(p_ptr->tim_general[0] || p_ptr->tim_general[1])
			{
				msg_print("���łɔ\�͂��g���Ă���B");
				return NULL;
			}

			msg_print("���Ȃ��͎w��炵���B���͂̉����������B");
			set_tim_general(time,FALSE,0,0);

		}
		break;
	case 2://�Î����
		{
			if(only_info) return format("");
			if(!p_ptr->tim_general[0] && !p_ptr->tim_general[1])
			{
				msg_print("�\�͂��g���Ă��Ȃ��B");
				return NULL;
			}

			msg_print("���Ȃ��͔\�͂����������B");
			set_tim_general(0,TRUE,0,0);
			set_tim_general(0,TRUE,1,0);

		}
		break;

	case 3:
		{
			int damage = 30 + p_ptr->lev * 2 + chr_adj;
			if(is_daytime()) damage /= 2;

			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�����̕ǂ��O����ガ�|�����B");
			fire_spark(GF_DARK,dir,damage,2);

			break;
		}	
	case 4:
		{
			int base = p_ptr->lev * 5;
			int sides = chr_adj * 6;
			if(is_daytime()) base /= 2;

			if(only_info) return format("����:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_DARK, dir, base + randint1(sides), 3,"���������ɂȂ��ĉQ���������I")) return NULL;
			break;
		}	

	case 5://�Î�2 tim_general[1]���g��
		{
			int time = 30;
			if(only_info) return format("����:%d",time);
			if(p_ptr->tim_general[1])
			{
				msg_print("���łɔ\�͂��g���Ă���B");
				return NULL;
			}

			msg_print("���Ȃ��̓t���A�S�̂̉��Ɋ������B");
			set_tim_general(time,FALSE,1,0);
			p_ptr->tim_general[0]=0; 
		}
		break;


	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�X�^�[��p�Z*/
class_power_type class_power_star[] =
{
	{7,6,15,FALSE,TRUE,A_WIS,0,2,"�v�`�R���b�g",
		"�^�[�Q�b�g�̈ʒu�ɔ��a0��覐Α����̃{�[���𔭐�������B"},
	{13,12,25,FALSE,TRUE,A_INT,0,3,"�X�^�[���C�g���C��",
		"�^�[�Q�b�g�t�߂ɑM�������̃{���g��A�˂���B"},
	{21,16,50,FALSE,TRUE,A_INT,0,5,"�X�^�[���[�U�[",
		"�M�������̃r�[������B"},
	{30,20,0,FALSE,FALSE,A_DEX,0,0,"���e�ݒu",
		"�����ɔ��e��ݒu����B���e�́u�ʏ�́v10�^�[���Ŕ��������a3�̔j�Б����̋��͂ȃ{�[���𔭐�������B���̔����ł̓v���C���[���_���[�W���󂯂�B���e�͈ꕔ�̑����U���ɎN�����ƗU�������蔚���^�C�~���O���ς�邱�Ƃ�����B"},
	{42,60,80,FALSE,TRUE,A_WIS,0,0,"�V���[�e�B���O�T�t�@�C�A",
		"���\�����̃��P�b�g�U������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_star(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int base = 20+p_ptr->lev;
			int sides = chr_adj * 2;

			if(only_info) return format("����:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_METEOR, dir, base + randint1(sides), 0,"���̂����炪�������B")) return NULL;
			break;
		}	
	case 1:
		{
			int dice = 4 + plev / 10;
			int sides = 2 + chr_adj / 20;
			int num = 8 + plev / 9 ;
			if(only_info) return format("����:%dd%d * %d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�������~�蒍�����B");
			fire_blast(GF_LITE, dir, dice, sides, num, 3, 0);
			break;
		}
	case 2:
		{
			int damage = plev * 3 + chr_adj * 5 / 3;

			if(only_info) return format("����:%d",damage);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���̌������ꂽ�B");
			fire_beam(GF_LITE, dir, damage);
			break;
		}
	case 3: //���e�ݒu�@�z�s�̎M�Ɠ��������ɂ���
		{
			int dam = 100+plev*4;
			if(only_info) return format("����:%d",dam);
			if(!futo_can_place_plate(py,px) || cave[py][px].special)
			{
				msg_print("�����ɂ͔��e��u���Ȃ��B");
				return NULL;
			}
			msg_print("���e���d�|�����B");
			/*:::Mega Hack - cave_type.special�𔚒e�̃J�E���g�Ɏg�p����B*/
			cave[py][px].info |= CAVE_OBJECT;
			cave[py][px].mimic = feat_bomb;
			cave[py][px].special = 10;
			while(one_in_(5)) cave[py][px].special += randint1(20);

			note_spot(py, px);
			lite_spot(py, px);

		}
		break;
	case 4:
		{
			base = plev * 5;
			sides = chr_adj * 6;

			if(only_info) return format("����:%d+1d%d",base,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���Ȃ���ῂ�������������I");
			fire_rocket(GF_PSY_SPEAR, dir, base+randint1(sides),2);
			break;
		}


	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�z�s��p�Z*/
class_power_type class_power_futo[] =
{
	{5,2,0,FALSE,TRUE,A_DEX,0,0,"�M�ݒu",
		"���̏�ɎM��ݒu����B�M�͍U����e���|�[�g�̊�_�Ƃ��Ďg����B�M��Ƃ���$20�����B"},
	{5,7,20,FALSE,TRUE,A_INT,0,3,"�Z�p�_��",
		"�Ή������̃r�[������B�r�[���̓^�[�Q�b�g���w�肷�邱�Ƃ��ł����A���͂ɎM������ΎM�֔�сA���͂ɎM���Ȃ���΋߂��̓G�ւƔ�ԁB"},
	{12,5,30,FALSE,TRUE,A_DEX,0,0,"�M����",
		"�M�𓊂��Ĕj�Б����̃_���[�W��^����B�������M�͂��̏�Ɏc��B�M��Ƃ���$50�����B"},
	{16,50,60,FALSE,TRUE,A_DEX,0,0,"�G���V�F���g�V�b�v",
		"���ꃂ���X�^�[�u�G���V�F���g�V�b�v�v���Ăяo���B����ɋR�悷�邱�Ƃ��ł��A�R��n���x���Ⴍ�Ă����n�⌸�������ɂ����B" },
	{20,20,40,FALSE,TRUE,A_INT,0,0,"���̋���",
		"�߂��̎M�̂���ꏊ��I�уe���|�[�g����B�e���|�[�g������ŗ��������̃{�[���𔭐������A���̎M�͔j�󂳂��B"},
	{24,22,40,FALSE,TRUE,A_WIS,0,5,"�����M",
		"���͂̎M��j�󂵁A�M�̂������ꏊ�ɐ������̏����ȃ{�[���𔭐�������B"},
	{30,24,50,FALSE,TRUE,A_DEX,0,0,"�O�ւ̎M��",
		"���̐n�Ŏ��͂��U�����A����Ɏ��͂̃����_���ȏꏊ�ɎM�𐔖��z�u����B�M��Ƃ���$500�����B"},
	{36,36,65,FALSE,TRUE,A_WIS,0,10,"�M���̖�",
		"�������̃r�[������B�r�[�����M�ɓ�����ƎM�͓���Ȕ��������Ĕj�󂳂�A���̃r�[���͋߂��̎M�֌������M���Ȃ���ΓG�Ɍ������B�|�𑕔����Ă��Ȃ��Ǝg���Ȃ������������A�|�����͂ȂقǈЗ͂��オ��B"},
	{40,80,75,FALSE,TRUE,A_DEX,0,0,"�����̔��\���D",
		"���E���ɔj�Б����U�����s���A��ʂ̎M��z�u����B�M��Ƃ���$5000�����B"},
	{44,96,80,FALSE,TRUE,A_INT,0,0,"�j�ǂ̊J��",
		"�߂��̎M���S�Ĕj�󂳂�A�߂��̃����_���ȓG��̂��s�K�Ɍ�������B�j�󂳂ꂽ�M�̐��������قǑ�ςȕs�K���~�肩����B"},
	{47,160,85,FALSE,TRUE,A_CHR,0,0,"��΂̉��V",
		"���E���̑S�Ăɑ΂����͂ȉΉ��U�����s���A����ɏ���n��ɂ���B���E���ɎM������ƑS�Ĕj�󂳂ꂻ�̕����̈З͂������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

//-Mega Hack-
//�Z�p�_�΂ƋM���̖�̎~�܂�r�[���Ȃǂ̕\���̂���PROJECT_FUTO���g���B
cptr do_cmd_class_power_aux_futo(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: 
		{
			int charge = 20;
			if(only_info) return "";
			if(!futo_can_place_plate(py,px))
			{
				msg_print("�����ɂ͎M��u���Ȃ��B");
				return NULL;
			}

			if(p_ptr->au < charge)
			{
				msg_print("�M�オ�Ȃ��E�E");
				return NULL;
			}
			else
			{
				p_ptr->au -= charge;
				p_ptr->redraw |= PR_GOLD;
			}
			msg_print("�M��u�����B");
			/* Create a glyph */
			cave[py][px].info |= CAVE_OBJECT;
			cave[py][px].mimic = feat_plate;
			note_spot(py, px);
			lite_spot(py, px);

		}
		break;
	case 1:
		{
			int dam = 15 + plev*3/2 + chr_adj;
			//int ty,tx;

			if(only_info) return  format("����:%d",dam);


			//v1.1.44 �ċA�����̂������Ńt���[�Y�����̂ŏ�������������
			msg_print("�΋���������I");
			if (!execute_restartable_project(1, 0, 0, 0, 0, dam, GF_FIRE, 0))
				msg_print("�������^�[�Q�b�g�����Ȃ������B");
			
			/*
			if(futo_determine_target(py,px,&ty,&tx))
			{
				msg_print("�΋���������I");
				project(0,0,ty,tx,dam,GF_FIRE,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
			}
			else
			{
				msg_print("�΋����o�������^�[�Q�b�g�����Ȃ������B");
			}
			*/

		}
		break;
	case 2:
		{
			int dam = 10 + plev * 3 / 2 + chr_adj / 2;
			int charge = 50;

			if(use_itemcard) charge = 0;

			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;

			if(p_ptr->au < charge)
			{
				msg_print("�M�オ�Ȃ��E�E");
				return NULL;
			}
			else
			{
				p_ptr->au -= charge;
				p_ptr->redraw |= PR_GOLD;
			}

			msg_print("�M�𓊂����I");
			fire_rocket(GF_THROW_PLATE, dir, dam,0);
		}
		break;
	case 3:
	{
		int i;
		u32b mode = PM_FORCE_PET;
		bool flag_exist = FALSE;
		if (only_info) return "";

		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if ((m_ptr->r_idx == MON_ANCIENT_SHIP)) flag_exist = TRUE;
		}
		if (flag_exist)
		{
			msg_print("���łɏM���Ăяo���Ă���B");
			return NULL;
		}
		if (summon_named_creature(0, py, px, MON_ANCIENT_SHIP, mode))
			msg_print("���ɕ����M�����ꂽ�B");
		else
			msg_print("�M�͏o�Ă��Ȃ������B");

		break;

	}

	case 4:
		{
			int range = plev;
			int dam = plev * 4;
			if(only_info) return format("����:%d ����:�`%d",range,dam);
			if (p_ptr->anti_tele)
			{
				msg_format("���͂��܂��ړ��ł��Ȃ��悤���B");
				return NULL;
			}
			if (!dimension_door(D_DOOR_FUTO)) return NULL;
			break;
		}
	case 5:
		{
			int x,y;
			int dam = plev * 2 + chr_adj * 5 / 3;
			int rad = plev / 6;
			bool flag = FALSE;
			if(only_info) return format("���ʔ͈�:%d ����:%d",rad,dam);

			for(y=py-rad;y<=py+rad;y++)
			{
				for(x=px-rad;x<=px+rad;x++)
				{
					if (!in_bounds(y, x)) continue;
					if(distance(py, px, y, x) > rad) continue;
					if(!is_plate_grid(&cave[y][x])) continue;
					if(!projectable(py,px,y,x)) continue;
					futo_break_plate(y,x);
					flag = TRUE;
					project(0,1,y,x,dam,GF_WATER,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP),-1);
				}
			}
			if(!flag) msg_print("�߂��ɎM���Ȃ��̂ŉ����N����Ȃ������B");
		}
		break;

	case 6:
	{
		int charge = 500;
		int dam = 110 + plev * 4 + chr_adj * 3;
		int rad = 2 + plev / 40;
		int plate_num = 1 + (plev+5)/10;
		int j;

		if(only_info) return format("����:�`%d",dam/2);

		if(p_ptr->au < charge)
		{
			msg_print("�M�オ�Ȃ��E�E");
			return NULL;
		}
		else
		{
			p_ptr->au -= charge;
			p_ptr->redraw |= PR_GOLD;
		}

		msg_print("�����̎M�������������N�������I");
		project(0,rad,py,px,dam,GF_WINDCUTTER,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP),-1);
		for(i=0;i<plate_num;i++)
		{
			int attempt = 100;
			int dy,dx;

			for(j=0;j<attempt;j++)
			{
				scatter(&dy, &dx, py, px, rad, 0);
				if(!futo_can_place_plate(dy,dx)) continue;

				cave[dy][dx].info |= CAVE_OBJECT;
				cave[dy][dx].mimic = feat_plate;
				note_spot(dy, dx);
				lite_spot(dy, dx);

				break;
			}
		}
	}
	break;
	case 7:
		{
			object_type *o_ptr;
			int mult;
			int dam;
			int ty,tx;
			int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_FUTO;
			bool dummy;

			if(inventory[INVEN_LARM].tval == TV_BOW || inventory[INVEN_RARM].tval == TV_BOW) 
			{
				if(inventory[INVEN_LARM].tval == TV_BOW) 
					o_ptr = &inventory[INVEN_LARM];
				else 
					o_ptr = &inventory[INVEN_RARM];

				mult = bow_tmul(o_ptr->tval,o_ptr->sval);
				if (p_ptr->xtra_might) mult++;
				dam = (plev/3 + o_ptr->to_d + chr_adj / 5) * mult;
				if(dam < 1) dam=1;
			}
			else dam = 0;

			if(only_info) return  format("����:%d",dam);
			if(!dam)
			{
				msg_print("�|�𑕔����Ă��Ȃ��B");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			//v1.1.44 �ċA�����̂������Ńt���[�Y�����̂ŏ�������������
			msg_print("���̖��������B");
			execute_restartable_project(1, dir, 0, 0, 0, dam, GF_DISP_ALL, 5);

/*

			msg_print("���̖��������B");
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			project(0, 5, ty, tx, dam, GF_DISP_ALL, flg, -1);
*/
		}
		break;
	case 8:
	{
		int x,y;
		int charge = 5000;
		int dam = plev * 2 + chr_adj *2;
		if(only_info) return format("����:1d%d",dam);

		if(p_ptr->au < charge)
		{
			msg_print("�M�オ�Ȃ��E�E");
			return NULL;
		}
		else
		{
			p_ptr->au -= charge;
			p_ptr->redraw |= PR_GOLD;
		}

		msg_print("���Ȃ��͑�ʂ̎M�𓊂��������I");
		project_hack2(GF_SHARDS,1,dam,0);

		for(y=py-MAX_RANGE;y<=py+MAX_RANGE;y++)
		{
			for(x=px-MAX_RANGE;x<=px+MAX_RANGE;x++)
			{
				if (!in_bounds(y, x)) continue;
				if(!one_in_(6)) continue;
				if(!projectable(py,px,y,x)) continue;
				if(distance(py, px, y, x) > MAX_RANGE) continue;
				if(!futo_can_place_plate(y,x)) continue;

				cave[y][x].info |= CAVE_OBJECT;
				cave[y][x].mimic = feat_plate;
				note_spot(y, x);
				lite_spot(y, x);

			}
		}
	}
	break;


	case 9:
	{
		int range = 9;
		int x, y, tx, ty;
		int count_plate = 0;
		int count_enemy = 0;

		if(only_info) return format("���ʔ͈�:%d",range);

		for(y=py-range;y<=py+range;y++)
		{
			for(x=px-range;x<=px+range;x++)
			{
				if (!in_bounds(y, x)) continue;
				if(!projectable(py,px,y,x)) continue;
				if(distance(py, px, y, x) > range) continue;

				if(cave[y][x].m_idx)
				{
					count_enemy++;

					if(one_in_(count_enemy))
					{
						ty = y;
						tx = x;
					}
				}				

				if(is_plate_grid(&cave[y][x]))
				{
					count_plate++;
					futo_break_plate(y,x);
				}
			}
		}

		if(!count_enemy)
		{
			msg_print("���������͂ɓG�����Ȃ������B");
		}
		else
		{
			int dam,typ,rad;
			cptr desc;
			char m_name[80];
			monster_type *m_ptr;
			m_ptr = &m_list[cave[ty][tx].m_idx];
			monster_desc(m_name, m_ptr, 0);

			if(count_plate < 5)
			{
				if(one_in_(3)) desc = "�^���C";
				else if(one_in_(2)) desc = "�ԕr";
				else desc = "�I�^�}";
				dam = damroll(3,3);
				rad = 0;
				typ = GF_MISSILE;
			}
			else if(count_plate < 10)
			{
				if(one_in_(3)) desc = "�A�ؔ�";
				else if(one_in_(2)) desc = "�S��";
				else desc = "�ꏡ�r";
				dam = damroll(7,7);
				rad = 0;
				typ = GF_MISSILE;
			}
			else if(count_plate < 20)
			{
				if(one_in_(2)) desc = "�^���X";
				else desc = "�M�y�Ă̒K";
				dam = damroll(10,10);
				rad = 0;
				typ = GF_MISSILE;
			}
			else if(count_plate < 30)
			{
				if(one_in_(2))desc = "�v��";
				else  desc = "���";
				dam = damroll(16,16);
				rad = 0;
				typ = GF_ARROW;
			}
			else if(count_plate < 60)
			{
				desc = "�p�d��";
				dam = damroll(33,33);
				rad = 2;
				typ = GF_MISSILE;
			}
			else if(count_plate < 100)
			{
				desc = "覐�";
				dam = damroll(50,50);
				rad = 3;
				typ = GF_METEOR;
			}
			else if(count_plate < 200)
			{
				desc = "���m�F��s����";
				dam = damroll(66,66);
				rad = 4;
				typ = GF_METEOR;
			}
			else
			{
				desc = "���r�D";
				dam = damroll(88,88);
				rad = 7;
				typ = GF_DISINTEGRATE;
			}
			msg_format("%s�̓��ォ��%s���~���Ă����I",m_name,desc);
			project(0,rad,ty,tx,dam,typ,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL),-1);
			if(count_plate >= 60)
				project(0,rad,ty,tx,dam/8, GF_DEC_ALL, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);


		}

	}
	break;



	case 10:
	{
		int x, y;
		int count_plate = 0;
		int dam = plev * 6 + chr_adj * 10;

		if(only_info) return format("����:%d+��",dam);

		for(y=py-MAX_RANGE;y<=py+MAX_RANGE;y++)
		{
			for(x=px-MAX_RANGE;x<=px+MAX_RANGE;x++)
			{
				if (!in_bounds(y, x)) continue;
				if(!projectable(py,px,y,x)) continue;
				if(distance(py, px, y, x) > MAX_RANGE) continue;
				if(!is_plate_grid(&cave[y][x])) continue;
				count_plate++;
				futo_break_plate(y,x);
			}
		}

		dam += count_plate * 7;
		if(dam > 9999) dam = 9999;
		msg_print("�ӂ��ʂ��΂̊C�ƂȂ����I");
		project_hack2(GF_LAVA_FLOW,1,4,0);
		project_hack2(GF_FIRE,0,0,dam);


	}
	break;

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�_�ގq��p�Z*/
class_power_type class_power_kanako[] =
{
	{1,0,0,FALSE,TRUE,A_WIS,0,0,"�M���`�F�b�N����",
		"���Ȃ��������z���łǂ̒��x�̐M���W�߂Ă��邩�m�F����B"},
	{8,6,20,FALSE,TRUE,A_CHR,0,1,"�֏���",
		"�֌^�̃����X�^�[��z���Ƃ��ď�������B"},
	{16,8,30,FALSE,TRUE,A_STR,0,6,"�G�N�X�p���f�b�h�E�I���o�V��",
		"�G�𐁂���΂��������̃r�[������B�e���|�[�g�ϐ��𖳎����邪����ȓG�͐�����΂Ȃ��B�M���W�܂�ƈЗ͂Ǝ˒����㏸����B"},
	{20,16,40,FALSE,TRUE,A_INT,0,0,"�_�̊�",
		"�肢�ɂ����͂̃A�C�e���̏ꏊ��m��B�M���W�܂�Ɗ��m�ł�����̂�������B�܂��󕠂̂Ƃ��͏��������x���񕜂���B"},
	{24,12,50,FALSE,TRUE,A_CHR,0,0,"�_�������ꂽ��_�n��",
		"�g���b�v�ƃh�A��j�󂷂�r�[������B�M�����Ȃ�W�܂�ƃr�[���ŕǂ��@��悤�ɂȂ�B"},
	{28,24,60,FALSE,TRUE,A_DEX,0,8,"��ˎR���_��",
		"���͂̃����_���ȓG�Ɍ��������̖������{���g�U�����s���B�M���W�܂�ƒe����������B"},

	{ 32,50,65,FALSE,TRUE,A_CHR,0,0,"�_�̌�Ќ�",
		"���E���S�Ẵ����X�^�[���U���͒ቺ�E���@�͒ቺ��Ԃɂ��悤�Ǝ��݂�B�i�s���W�܂�ƌ��͂������Ȃ�B" },

	{36,48,65,FALSE,TRUE,A_WIS,0,0,"�}�E���e���E�I�u�E�t�F�C�X",
		"��莞�ԃp���[�A�b�v����B�M���W�܂�Ƃ���ɋ��͂ȃp���[�A�b�v������悤�ɂȂ�B"},

	{40,72,70,FALSE,TRUE,A_CHR,0,1,"�_�i����",
		"�z���Ƃ��Đ_�i����������B"},

	{45,96,80,FALSE,TRUE,A_STR,0,0,"���e�I���b�N�I���o�V��",
		"���͂̃����_���ȏꏊ��覐Α����̋��͂ȃ{�[����A���Ŕ���������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_kanako(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int rank;
	
	if(use_itemcard) rank = 5;
	else rank = kanako_rank();

	switch(num)
	{
	case 0: 
		{
			if(only_info) return "";
			msg_format("%s",kanako_comment());
			return NULL; //�s��������Ȃ�
		}
		break;
	case 1:
		{
			int num = 1 + plev / 5 + rank * 2;
			bool flag = FALSE;
			if(only_info) return "";
			if(num > 20) num = 20;
			for(i=0;i<num ;i++)
			{
				if ((summon_specific(-1, py, px, plev, SUMMON_SNAKE, (PM_FORCE_PET))))flag = TRUE;
			}
			if(flag)
				msg_print("�ւ��Ăяo�����I");
			else 
				msg_print("�ւ͌���Ȃ������B");
		}
		break;

	case 2:
		{
			int dist = MIN(15,(3 + rank * 2));

			dice = 4 + plev / 6;
			sides = 6 + chr_adj / 5;
			base = 30 + plev * rank / 2;
	
			if(only_info) return  format("�˒�:%d ����:%d+%dd%d",dist,base,dice,sides);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_format("�J�[�h����傫�Ȓ�����яo�����I");
			else
				msg_format("�䒌�𓊂��������I");
			fire_beam(GF_TRAIN, dir, base + damroll(dice,sides));

		}
		break;
	case 3:
		{
			int rad = 10 + plev / 3;

			if(only_info) return  format("");
			if(use_itemcard)
				msg_format("������H�ׂ��B");
			else
				msg_format("���Ȃ��͓����𐆂��n�߂��E�E");
	
			if(rank > 6)
			{
				wiz_lite(FALSE);
			}
			else
			{
				if(rank > 0) detect_objects_normal(rad);
				if(rank > 4) map_area(rad);

			}
			detect_objects_gold(rad);
			if(rank > 1) detect_traps(rad, TRUE);
			if(rank > 2) detect_monsters_normal(rad);
//�����_�����ĐH�ׂ�񂾂����H���ג����Ƃ�
			if (p_ptr->food < PY_FOOD_FULL) set_food(p_ptr->food + 3000);
		}
		break;
	case 4:
		{
			int dist = MIN(15,(6 + rank * 2));
			if(only_info) return  format("�˒�:%d",dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("��O�ɓ����J���ꂽ�B");
			if(rank > 4) project_hook(GF_KILL_WALL, dir, 0, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_DISI));
			project_hook(GF_KILL_TRAP, dir, 0, (PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE));
		}
		break;

	case 5:
		{
			bool flag = FALSE;
			int num;
			int i;
			dice = 1;
			sides = p_ptr->lev + chr_adj / 2;
			num = rank * rank / 3 + 4;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);

			msg_format("�J�̂悤�ɖ���~�点���I");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_MISSILE,damroll(dice,sides),1, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
		}
		break;

	case 6:
		{
			int power = 10 + rank * rank * 10;
			if (only_info) return format("����:%d", power);

			if (rank > 5) msg_print("���Ȃ��ׂ͍��A���̖������������I");
			else if (rank > 3) msg_print("���Ȃ��͐_�X�����������I");
			else msg_print("���Ȃ��͈̂����Ɍ�����؂����I");

			project_hack2(GF_DEC_ATK, 0, 0, power);
			project_hack(GF_DEC_MAG, power);

			break;
		}



	case 7:
		{
			int time = 30;
			int dec_hp;

			if(only_info) return format("����:%d",time);

			if(one_in_(3)) msg_print("�䒌���򗈂��w�ɑ������ꂽ�I");
			else if(one_in_(2)) msg_print("�F�̐M�̗͂����ꍞ��ł���I");
			else msg_print("���Ȃ��̑̂͐M�̗͂Ŏ��F�ɋP�����I");
		
			if(rank > 0)
			{
				set_afraid(0);
				set_hero(time,FALSE);
			}
			if(rank > 1 && rank < 7)
			{
				set_tim_addstat(A_STR,5,time,FALSE);
			}
			if(rank > 2)
			{
				set_protevil(time,FALSE);
			}
			if(rank > 3)
			{
				set_shield(time,FALSE);
			}
			if(rank > 4)
			{
				set_oppose_acid(time, FALSE);
				set_oppose_elec(time, FALSE);
				set_oppose_fire(time, FALSE);
				set_oppose_cold(time, FALSE);
				set_oppose_pois(time, FALSE);
			}
			if(rank > 5)
			{
				set_magicdef(time, FALSE);
			}
			if(rank > 6)
			{
				dec_hp = p_ptr->mhp - p_ptr->chp;
				set_tim_addstat(A_STR,107,time,FALSE);
				set_tim_addstat(A_DEX,107,time,FALSE);
				set_tim_addstat(A_CON,107,time,FALSE);
				p_ptr->chp = p_ptr->mhp - dec_hp;
				p_ptr->redraw |= PR_HP;
				handle_stuff();		
			}
		}
		break;

	case 8:
		{
			int num = 1 + rank / 2;
			bool flag = FALSE;
			if(only_info) return "";
			if(rank == 7) num += 2;
			for(i=0;i<num ;i++)
			{
				if ((summon_specific(-1, py, px, plev * 3 / 2, SUMMON_DEITY, (PM_FORCE_PET)))) flag = TRUE;
			}
			if(flag)
				msg_print("�_�X�����Ȃ��̌Ăт����ɉ������I");
			else 
				msg_print("�N������Ȃ������B");
		}
		break;


	case 9:
		{
			int rad = 1 + (rank + 1) / 2;
			int dam = 1 + rank * rank * 5;
			if(only_info) return format("����:%d*�s��",dam);
			msg_print("����Ȍ䒌�������̂悤�ɓV����~�蒍�����I");
			cast_meteor(dam, rad,GF_METEOR);

		}
		break;

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}





/*:::���g��p�Z*/
//magic_num1[0]�ɕ����񐔁Amagic_num2[0]�ɃC���y���V���u���V���[�e�B���O�p�t���O
class_power_type class_power_mokou[] =
{
	{1,50,0,FALSE,TRUE,A_CON,0,0,"���U���N�V����",
		"���̏�ɋ���ȉΉ������̃{�[���𔭐������AHP�ƑS�Ă̏�Ԉُ���񕜂���B�X�R�A��������A���Ɏア�����i���R���邱�Ƃ�����B���̓��Z��HP��0�����ɂȂ����Ƃ��ɂ������I�ɔ������邪�AMP���s�����Ă����HP�̉񕜗���������AMP25�ȉ����Ɣ����ł��Ȃ��B"},
	{5,20,40,TRUE,TRUE,A_DEX,0,0,"�Ή���",
		"������ꏊ�𖾂邭�Ƃ炵�A�ꎞ�I�ɕ��V����B"},
	{12,11,40,FALSE,TRUE,A_DEX,0,5,"�P���V��",
		"�G���Q���ɓ�����Ɣ�������Ή������̋�����B���x��40�ȍ~�͊j�M�����ɂȂ�B"},
	{16,12,30,FALSE,TRUE,A_WIS,0,0,"���̂��͂����̎�",
		"���͂̓G�̓������~�߂�B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	{20,20,40,FALSE,TRUE,A_CON,0,10,"���ŉΉ������",
		"�����𒆐S�ɋ��͂ȉΉ������̃{�[���𔭐�������B�������_���[�W���󂯂�B���Ɏア�����i���R���邱�Ƃ�����B"},
	{24,30,45,FALSE,TRUE,A_INT,0,4,"�����ʔ��΂̕�",
		"���͂̃����_���ȏꏊ�ɔ����̃��[����z�u����B"},
	{28,50,50,FALSE,TRUE,A_CHR,0,0,"�t�F�j�b�N�X�̔�",
		"�אڂ����G��̂ɉΉ������U�����s���A�񃆃j�[�N�̃A���f�b�h�����m���ňꌂ�œ|���B"},
	{33,100,60,FALSE,TRUE,A_INT,0,0,"�p�[�X�g�o�C�t�F�j�b�N�X",
		"�t���A�̍D���ȏꏊ�Ɉړ����ă��U���N�V��������B�ړ���ɓG�������ꍇ�j�M�����̑�_���[�W��^����B�e���|�[�g�œ���Ȃ��ꏊ�ɂ͐N���ł��Ȃ��B�X�R�A��������B"},
	{40,400,70,TRUE,TRUE,A_STR,0,100,"�t�W���}���H���P�C�m",
		"���E���̑S�Ăɑ΂����͂ȉΉ������U�����s���BHP���ʂɏ����B"},
	{44,160,80,FALSE,TRUE,A_CON,0,0,"�t�F�j�b�N�X�Ēa",
		"���U���N�V�����Ɠ������ʂ̌�A�ꎞ�I�ɑS�\�͂��啝�ɏ㏸����B�X�R�A��������B"},
	{47,0,80,FALSE,TRUE,A_CHR,0,0,"�C���y���V���u���V���[�e�B���O",
		"�߂��̓G�ɋ��͂Ȗ������U������������B�U���̂��т�MP30������AMP���s���邩�t���A���ړ�����܂ŉ����ł��Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_mokou(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dam = (p_ptr->lev * 8 + chr_adj * 10 + 300);
			if(only_info) return format("����:�ő�%d",dam/2);

			mokou_resurrection();
			
			break;
		}
	case 1:
		{
			int time;
			base = 15 + plev / 2;
			if(only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_print("�R�����鉊�̗����w�Ɍ��ꂽ�B");
			lite_room(py,px);
			set_tim_levitation(time,FALSE);

		}
		break;
	case 2:
		{
			int dam = 30 + plev * 4 + chr_adj * 3 ;
			int rad = 1 + plev / 24;

			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("�J�[�h����΂̒�������Ĕ��ōs�����I");
			else
				msg_print("�΂̒�����񂾁I");
			fire_rocket((plev > 39)?GF_FIRE:GF_NUKE, dir, dam, rad);
		}
		break;
	case 3:
		{
			int power = 50 + plev * 2;
			if(only_info) return format("����:%d",power);
			msg_print("�_���W�������َ��ȕ��͋C�ɕ�܂ꂽ�B");
			stasis_monsters(power);
			break;

		}
	case 4:
		{
			int dam = plev * 15 + chr_adj * 15;
			int rad = plev / 10;

			if(only_info) return format("�͈�:%d ����:���S�n��%d ",rad,dam);

			msg_print("�Ή��̑�����������N�������I");
			project(PROJECT_WHO_SUICIDAL_FIRE, rad, py, px, dam,GF_FIRE, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
			inven_damage(set_fire_destroy, 2);

			break;
		}
	case 5:
		{
			int xx,yy;
			int num = plev / 5;
			int rad = plev / 10;
			if(num < 3) num = 3;
			if(rad < 2) rad = 2;
			if(only_info) return format("��:�ő�%d ",num);

			msg_print("���΂̕����T���U�炵���I");
			for(i=0;i<num;i++)
			{
				scatter(&yy,&xx,py,px,rad,0);
				if(!cave_clean_bold(yy,xx)) continue;
				cave[yy][xx].info |= CAVE_OBJECT;
				cave[yy][xx].mimic = feat_explosive_rune;
				note_spot(yy, xx);
				lite_spot(yy, xx);
			}
		}
		break;
	case 6:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 3 + chr_adj * 5;
			if(damage < 100) damage = 100;
			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�ɕs�����̉H���˂��h�������B",m_name);

				if(!(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags3 & RF3_UNDEAD) 
					&& (randint1(r_ptr->level) + 20 < plev))
				{
					damage = m_ptr->hp + 1;
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_MISSILE,PROJECT_KILL,-1);
				}
				else
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_FIRE,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	
	case 7:
		{
			int damage = (p_ptr->lev * 6 + chr_adj * 10);
			if(only_info) return format("����:%d",damage);
			if (p_ptr->anti_tele)
			{
				msg_format("���͂��܂��ړ��ł��Ȃ��悤���B");
				return NULL;
			}
			if (!dimension_door(D_DOOR_MOKOU)) return NULL;
			break;
		}

	case 8:
		{
			int dam = plev * 12 + chr_adj * 20;
			if(dam < 600) dam = 600;
			if(only_info) return format("����:%d",dam);
			msg_format("�����Ƌ��Ɏ��͂����ɕ�܂ꂽ�I");
			project_hack2(GF_FIRE,0,0,dam);	
			break;
		}
	case 9:
		{
			int time,percentage;
			base = 25 + plev / 2;
			if(only_info) return format("����:%d+1d%d",base,base);

			mokou_resurrection();
			time = base + randint1(base);
			msg_print("���Ȃ��͂�苭���Ȃ��đh�����I");
			percentage = p_ptr->chp * 100 / p_ptr->mhp;
			time = base + randint1(base);
			set_tim_addstat(A_STR,110,time,FALSE);
			set_tim_addstat(A_INT,110,time,FALSE);
			set_tim_addstat(A_WIS,110,time,FALSE);
			set_tim_addstat(A_DEX,110,time,FALSE);
			set_tim_addstat(A_CON,110,time,FALSE);
			set_tim_addstat(A_CHR,110,time,FALSE);
			set_fast(time, FALSE);
			set_hero(time,FALSE);
			set_tim_speedster(time, FALSE);				
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= (PR_HP | PR_MANA);
			redraw_stuff();

		}
		break;
	case 10:
		{
			int dam = plev * 5 + chr_adj * 5;

			if(only_info) return format("����:�����ő�%d+1d%d ",dam/2,dam/2);
			p_ptr->magic_num2[0] = 1;//�t���O

			break;
		}



	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::��֐�p�Z*/
class_power_type class_power_ichirin[] =
{
	{5,8,10,TRUE,TRUE,A_STR,0,5,"�ⓚ���p�̗d����",
		"�������ꂽ�ꏊ�ɍU������B�ʏ�U���̈З͂��オ��ƃ_���[�W���オ��B"},
	{12,8,20,FALSE,TRUE,A_WIS,0,4,"����e����ڋ�",
		"�M�������̃r�[������B"},
	{18,20,30,FALSE,TRUE,A_DEX,0,3,"���E�̗��_",
		"�����ׂ̗ɗ��������̃{�[���𔭐�������B����ȓG�����ԓG�ɂ͌��ʂ������B"},
	{25,35,45,TRUE,TRUE,A_STR,0,10,"�_��n���˂�",
		"�G���Q���ɓ�����Ɣ�������C�����̃{�[������B"},
	{31,35,60,FALSE,TRUE,A_WIS,0,2,"���z�����_",
		"�אڂ����G�S�Ă��U����������΂��B�e���|�[�g�ϐ��𖳎����邪����ȓG�͐�����΂Ȃ��B"},
	{36,50,70,FALSE,TRUE,A_WIS,0,10,"�V��S�Ɨ��Ƃ�",
		"覐Α����̃{�[���𔭐�������B�ʏ�U���̈З͂��オ��ƃ_���[�W���オ��B"},
	{40,200,75,TRUE,TRUE,A_STR,0,50,"�_�C�N���[�P������",
		"�U����2�`3�{�̒ʏ�U�����s���BHP��啝�ɏ����B"},
	{45,72,85,FALSE,TRUE,A_CON,0,0,"�ؗ�Ȃ�e������",
		"�Z�g�p��A�S�Ă̑ϐ����l�����A�S�Ă̍U���̃_���[�W�𔼌����A�G����̗אڍU���ɑ΂��������s���悤�ɂȂ�B�������^�[���o�ߎ��Ɣ�������MP������A�ҋ@�ȊO�̍s��������ƌ��ʂ��؂��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};



cptr do_cmd_class_power_aux_ichirin(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: 
		{
			int x, y;
			int dist = 2 + plev / 15;
			monster_type *m_ptr;

			dice = plev / 3;
			sides = 10 + chr_adj / 10;
			base = MAX(p_ptr->to_d[0],p_ptr->to_d[1]);
			if(only_info) return format("�˒�:%d ����:%d+%dd%d",dist,base,dice,sides);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�̏�ɉ_�R�̌����������I",m_name);
				project(0,0,y,x,damroll(dice,sides)+base,GF_ARROW,PROJECT_KILL,-1);
			}

			break;
		}


	case 1:
		{
			dice = 3 + p_ptr->lev / 5;
			sides = 10;
			base = p_ptr->lev + chr_adj;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�_�R���s�������������I");
			fire_beam(GF_LITE, dir, base + damroll(dice,sides));
			break;
		}

	case 2:
		{
			int y, x;
			int rad = 3 + plev / 16;

			base = plev * 3 + chr_adj * 2 - 10;
			if(base < 50) base = 50;

			if(only_info) return format("����:�ő�%d",base);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			if(in_bounds(y,x) && cave_have_flag_bold(y, x, FF_PROJECT))
			{
				if(use_itemcard)
					msg_format("�\�����N�������I");
				else
					msg_format("�_�R���\�����N�������I");
				project(0, rad, y, x, base, GF_TORNADO, PROJECT_KILL , -1);

			}
			else
			{	
				msg_format("�����ł͎g���Ȃ��B");
				return NULL;
			}
			break;
		}
	case 3:
		{
			int dam = 30 + plev * 3 + chr_adj * 2 ;
			int rad = 1 + plev / 40;

			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;

			if(use_itemcard)
				msg_format("�J�[�h���猝�̂悤�ȉ_��������ł������I");
			else
				msg_print("�_�R�̌�����񂾁I");
			fire_rocket(GF_FORCE, dir, dam, rad);
		}
		break;
	case 4:
		{
			int dam = plev * 7 + chr_adj * 5;
			int rad = 1;

			if(only_info) return format("����:%d ",dam/2);

			msg_print("�_�R�Ƌ��Ɍ�������]�����I");
			project(0, rad, py, px, dam, GF_TRAIN, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

			break;
		}
	case 5: 
		{
			int x, y;
			int dist = plev / 4;
			int rad = 2;
			monster_type *m_ptr;

			dice = plev / 2;
			sides = 10;
			base = MAX(p_ptr->to_d[0],p_ptr->to_d[1]) * 3 + chr_adj * 3;
			if(only_info) return format("�˒�:%d ����:%d+%dd%d",dist,base,dice,sides);
			project_length = dist;

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				msg_format("�_�R�̋���Ȍ�����n��������I");
				project(0,rad,y,x,damroll(dice,sides)+base,GF_METEOR,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM),-1);
			}

			break;
		}
	case 6:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
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

				msg_format("���Ȃ��Ɖ_�R�͋C���𗭂߂��c",m_name);
				py_attack(y,x,HISSATSU_UNZAN);
			}

			break;
		}
	case 7:
		{
			if(only_info) return format("");
			set_action(ACTION_KATA);
			p_ptr->special_defense |= KATA_MUSOU;
			p_ptr->update |= (PU_BONUS);
			p_ptr->update |= (PU_MONSTERS);
			msg_format("���Ȃ��͐m�������œG�̍U����҂��\�����I");

		}
		break;


	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::俎q��p�Z*/
class_power_type class_power_sumireko[] =
{
	{1,3,10,FALSE,TRUE,A_INT,0,2,"�e���L�l�V�X(����)",
		"�A�C�e���𓊝�����B�˒������̓A�C�e���̏d�ʂɊւ�炸�ő�ƂȂ�B���x�����オ��ƈЗ͂��㏸���A���x��35�ȍ~�͓��������킪�߂��Ă���B"},
	{6,4,25,FALSE,TRUE,A_WIS,0,2,"�p�C���L�l�V�X",
		"���˒����Z���Ή������̃r�[������B"},
	{13,5,30,FALSE,TRUE,A_DEX,0,3,"���u�U��",
		"�������ꂽ�ꏊ�֒ʏ�U�����s���B�@����𑕔����Ă���Ǝg���Ȃ����G�̃I�[���ɂ��_���[�W���󂯂Ȃ��B"},
	{16,10,40,FALSE,TRUE,A_INT,0,0,"�w�����e���|�[�g",
		"�w�肵�������֒Z�����̃e���|�[�g���s���B�ʏ�̔����̍s���͂�������Ȃ��B�e���|�[�g�ł��Ȃ��ꏊ�Ƀe���|�[�g���悤�Ƃ����ꍇ�A���̎��ӂ̃����_���ȏꏊ�֏o�邩���̏ꏊ�ɖ߂�B"},
	{20,12,40,FALSE,TRUE,A_INT,0,4,"�A�[�o���T�C�R�L�l�V�X(�z��)",
		"���E���̃A�C�e�����������񂹎����̎���𕂗V������B���V���Ă���A�C�e���̎�ނɂ���ēG�ɃI�[���_���[�W��^������AC���㏸����B"},
	{20,0,0,FALSE,TRUE,A_INT,0,0,"�A�[�o���T�C�R�L�l�V�X(�m�F)",
		"���͂𕂗V���Ă���A�C�e���̈ꗗ������B�s���͂�����Ȃ��B"},
	{20,0,0,FALSE,TRUE,A_INT,0,0,"�A�[�o���T�C�R�L�l�V�X(���f)",
		"���͂𕂗V���Ă���A�C�e����S�Ēn�ʂɍ~�낷�B"},
	{20,15,50,FALSE,TRUE,A_INT,0,5,"�A�[�o���T�C�R�L�l�V�X(���o)",
		"���͂𕂗V���Ă���A�C�e�����^�[�Q�b�g�֌����đS�ē�������B��������߂��Ă���\�͂����A�C�e���͖߂��Ă��Ȃ��B"},
	{25,0,75,FALSE,TRUE,A_WIS,0,0,"���_�W��",
		"MP���킸���ɉ񕜂���B"},
	{30,22,65,FALSE,TRUE,A_WIS,0,6,"�T�C�R�v���\�W����",
		"�����̎��͂̃����_���Ȉʒu�֑������܂ɔ������N�����A�G��N�O�Ƃ�������e���|�[�g������B"},
	{36,32,60,FALSE,TRUE,A_INT,0,0,"�e���L�l�V�X(�s�@����)",
		"�w�肵���ʒu�ɂ���G�ɖ������U�����s���B�G�����Ȃ��ꍇ��𐶐�����B"},
	{40,64,80,FALSE,TRUE,A_INT,0,0,"�T�C�R�L�l�V�X�A�v��",
		"���E���̑S�ĂɎ��󑮐��U�����s���ǂ����֐�����΂��B�X�}�[�g�t�H�����������Ă��Ȃ��Ǝg���Ȃ����A�X�}�[�g�t�H���͌��z���̗d���B�ɒǂ��|���񂳂ꂽ�Ƃ��ɗ��Ƃ��Ă��܂����B"},
	{45,96,90,FALSE,TRUE,A_CHR,0,0,"�������̃I�J���e�B�V����",
		"���E���̃����_���ȓG�Ɍ����A���͂Ȗ��͑����̃r�[���𐔔����B"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_sumireko(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int inven2_num = calc_inven2_num();

	switch(num)
	{

	case 0:
		{
			int mult = 1 + (plev + 10)/ 20;
			if(only_info) return format("�{��:%d",mult);
			if (!do_cmd_throw_aux(mult, FALSE, -2)) return NULL;
		}
		break;
	case 1:
		{
			int dist = 4 + plev / 8;
			int dice = 8 + plev / 2;
			int sides = 5;
			if(only_info) return format("����:%dd%d �˒�:%d",dice,sides,dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�w��炷�Ƌ�C���R���オ�����B");
			fire_beam(GF_FIRE, dir, damroll(dice,sides));			
			break;
		}
	case 2:
		{
			if(only_info) return format("");
			project_length = 2 + plev / 24;
			if (!get_aim_dir(&dir)) return NULL;
			project_hook(GF_ATTACK, dir, HISSATSU_NO_AURA, PROJECT_STOP | PROJECT_KILL);
			break;
		}
	case 3:
	{
		int dist = 5 + plev / 10;
		if (only_info) return format("����:%d", dist);

		if (!teleport_to_specific_dir(dist, 7, 0)) return NULL;

		new_class_power_change_energy_need = 50;
		break;
	}

	case 4:
		{
			int j;
			bool flag_pick = FALSE;
			bool flag_max = TRUE;
			object_type forge;
			object_type *o_ptr;
			object_type *q_ptr;
			char o_name[MAX_NLEN];

			if(only_info) return format("�ő�:%d��",inven2_num);
			//�t���A�̃A�C�e����S�Ē��ׂ�
			for(i=0;i<inven2_num;i++)
			{
				int item = 0;
				int item_count_temp = 0;
				if(inven_add[i].k_idx) continue;
				flag_max = FALSE;
				//���E���̃A�C�e���������_���ɑI��
				for(j=0;j<o_max;j++)
				{
					o_ptr = &o_list[j];
					if (!o_ptr->k_idx) continue;
					if (!projectable(o_ptr->iy,o_ptr->ix,py,px)) continue;

					item_count_temp++;
					if(one_in_(item_count_temp)) item = j;
				}
				if(!item) break;
				//�I�΂ꂽ�A�C�e����ǉ��C���x���g����1�����i�[������A�C�e�������炷�B�����i�d�ʂ͍l�����Ȃ��B
				flag_pick = TRUE;
				o_ptr = &o_list[item];
				q_ptr = &forge;
				object_copy(q_ptr, o_ptr);
				q_ptr->number = 1;
				distribute_charges(o_ptr, q_ptr, 1);
				object_desc(o_name, q_ptr, 0);
				msg_format("%s���z���񂹂��B",o_name);
				object_wipe(&inven_add[i]);
				object_copy(&inven_add[i], q_ptr);
				floor_item_increase(item, -1);
				floor_item_describe(item);
				floor_item_optimize(item);

				p_ptr->update |= PU_BONUS;
			}
			if(!flag_pick)
			{
				msg_print("�߂��ɂ͓������镨���Ȃ��悤���E�E");
				break;
			}
			else if(flag_max)
			{
				msg_print("����ȏ�̐��͈�������Ȃ��B");
				return NULL;
			}


		}
		break;
	case 5:
		{
			if(only_info) return format("");
			display_list_inven2();
			return NULL; //���邾���Ȃ̂ōs��������Ȃ�
		}
		break;
	case 6:
		{
			object_type *o_ptr;
			bool flag_drop = FALSE;
			char o_name[MAX_NLEN];
			if(only_info) return format("");
			for(i=0;i<inven2_num;i++)
			{
				if(!inven_add[i].k_idx) continue;
				flag_drop = TRUE;
				o_ptr = &inven_add[i];
				object_desc(o_name, o_ptr, 0);
				msg_format("%s���������B",o_name);
				drop_near(o_ptr,0,py,px);
				object_wipe(&inven_add[i]);
				p_ptr->update |= PU_BONUS;
			}
			if(!flag_drop)
			{
				msg_print("�~�낷���̂��Ȃ��B");
				return NULL;
			}
		}
		break;
	case 7:
		{
			int ty,tx;
			bool thrown = FALSE;
			object_type *o_ptr;
			char o_name[MAX_NLEN];
			if(only_info) return format("");

			for(i=0;i<inven2_num;i++)
				if(inven_add[i].k_idx) thrown = TRUE;
			if(!thrown)
			{
				msg_print("��΂����̂��Ȃ��B");
				return NULL;
			}

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
			msg_print("���͂̃A�C�e����W�I�ڊ|���Ĉ�Ăɔ�΂����I");

			for(i=0;i<inven2_num;i++)
			{
				if(!inven_add[i].k_idx) continue;
				thrown = TRUE;
				o_ptr = &inven_add[i];
				p_ptr->update |= PU_BONUS;

				if(do_cmd_throw_aux2(py,px,ty,tx,1,o_ptr,0))
					object_wipe(&inven_add[i]);
			}
		}
		break;
	case 8:
		{
			if(only_info) return format("");
			msg_print("���Ȃ��̓X�v�[�������o���A���_���W�������E�E");
			p_ptr->csp += (4 + randint1(plev/10));
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
		}
		break;

	case 9: 
		{
			int rad = 2 + plev / 40;
			int damage = plev / 2 + 35 ;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("�O���͂̔������N�������I");
			cast_meteor(damage, rad, GF_TELEKINESIS);

			break;
		}
	case 10:
		{
			int y, x;
			monster_type *m_ptr;
			int damage = plev * 5;

			if(only_info) return format("����:%d",damage);

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
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
				msg_print("�O���͂Ŋ���y���ςݏd�˂��B");
				cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�֌����Ċ��I���~�蒍�����I",m_name);
				project(0,0,y,x,damage,GF_ARROW,PROJECT_KILL,-1);
			}

			break;
		}


	case 11:
		{
			int base = plev * 4;
			if(base < 150) base = 150;
			if(only_info) return format("����:%d+1d%d",base,base);
			if(use_itemcard)
				msg_format("�J�[�h���Ȃ���Ƃ���ɏ]���Ď��͂̂��̂�������񂾁I");
			else
				msg_format("����̈�؍����������񂹂đ~���񂵂Đ�����΂����I");
			project_hack2(GF_DISTORTION,1,base,base);	
			break;
		}
	case 12:
		{
			bool flag = FALSE;
			int i;
			int dice = plev / 5;
			int sides = 10 + chr_adj / 4;
			int num = 7;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			msg_print("�S�g�S��̒��\�͂�������I");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_MANA, damroll(dice,sides),2, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
			break;
		}


	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}

	return "";
}












/*:::�j����2��p�Z*/
class_power_type class_power_shinmyou2[] =
{
	{8,4,20,FALSE,FALSE,A_DEX,0,3,"���ߓ˂�",
		"�˒��̒Z���r�[������B�З͂͗����r�̒ʏ�U���͂�1/3���x�B�����r�ɒZ���E���E���E�j�𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},
	{14,8,35,FALSE,FALSE,A_STR,0,0,"���l�̈�{�ނ�",
		"���E���̎w�肵���G�Ƀ_���[�W��^���ċ߂��̂ǂ����֔�΂��B�e���|�[�g�ϐ��𖳎����邪����ȓG�͔�΂��Ȃ��B�ނ�����邱�Ƃ��ł��镐��𑕔��܂��͏������Ă��Ȃ��Ǝg���Ȃ��B"},
	{20,11,40,FALSE,FALSE,A_DEX,0,3,"�G�H�D����",
		"�אڂ����^�[�Q�b�g�ɍU�����A����ɓG�̎��̍s�����킸���ɒx�点��B����ȓG�A�͋����G�A���j�[�N�����X�^�[�ɂ͌��ʂ������B�����r�ɒZ���E���E���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},
	{24,27,50,FALSE,TRUE,A_CHR,0,0,"���������]",
		"�����̂���ꏊ�ɐ������̃{�[���𔭐������A����ɒn�`�𐅂ɂ���B�ނ�����邱�Ƃ��ł��镐��𑕔��܂��͏������Ă��Ȃ��Ǝg���Ȃ��B"},

	{27,40,60,FALSE,TRUE,A_CHR,0,0,"��ѓ���Q��",
		"���z���̗F�D�I�ȃ����X�^�[�����̏�ɏ�������B����̐��i�ł̂ݎg�p�\�B������Ԃł͎g���Ȃ��B" },

	{30,20,60,FALSE,FALSE,A_DEX,0,6,"�S���낵���ړ˂��̐j",
		"�߂��̃����_���ȃ����X�^�[�Ɍ����č��������̃r�[����A�˂���B�r�[���̈З͂Ɩ{���͒ʏ�U���ɏ�����B�����r�ɒZ���E���E���E�j�𑕔����Ă��Ȃ��Ǝg���Ȃ��B" },

	{35,35,60,FALSE,FALSE,A_DEX,0,10,"�V�ߕS�D",
		"�אڂ����G�ɁA�U���񐔂�1.5�{�ɂȂ�I�[���_���[�W���󂯂Ȃ��ʏ�U�����s���B�����r�ɒZ���E���E���E�j�𑕔����Ă��Ȃ��Ǝg�����A�����r�ȊO�̍U���͍s���Ȃ��B"},
	{40,75,75,FALSE,TRUE,A_WIS,0,0,"�P�j��",
		"�^�[�Q�b�g���ӂɌ����Ėh��s�\�ȃr�[���U����A���ŕ��B"},
	{45,80,80,FALSE,TRUE,A_CON,0,0,"�ꐡ�@�t�ɂ��ܕ��̍�",
		"���Ƃ̗͂ŒZ���ԏ��^������B���^�����Ă���Ƃ��͈ړ����x��AC���ቺ���邪�B���\�͂��啝�ɏ㏸���A���@��u���X�Ȃǂ̓���U���𒆊m���ŉ�����A����ɋ���ȓG����F������ɂ����Ȃ�B����ނ͒ʏ�ʂ�g�p�ł���B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_shinmyou2(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int str_adj = adj_general[p_ptr->stat_ind[A_STR]];

	switch(num)
	{
	case 0:
		{
			int damage = 0;
			int range = 2 + plev / 20;

			damage = calc_weapon_dam(0) / 3;
			if (only_info) return format("�˒�:%d ����:%d",range,damage);

			if(!p_ptr->num_blow[0])
			{
				msg_print("ERROR:num_blow[0]��0");
				return NULL;
			}

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�s���˂���������I");
			fire_beam(GF_MISSILE, dir, damage);
			break;
		}
	case 1:
		{
			int y, x;
			monster_type *m_ptr;
			int damage = 10 + plev + str_adj * 2;

			if(only_info) return format("����:%d",damage);

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}

			m_ptr = &m_list[cave[target_row][target_col].m_idx];

			if (!m_ptr->r_idx)
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�ɐj�������|�����I",m_name);
				project(0,0,target_row,target_col,damage,GF_TRAIN,PROJECT_KILL,-1);
			}

			break;
		}



	case 3:
		{
			int dam = 40 + plev * 4 + chr_adj * 2;
			int rad = 1 + plev / 16;

			if(only_info) return format("����:�`%d ",dam/2);

			msg_print("�������琅�Ƌ��������o�����I");

			project(0, rad, py, px, rad, GF_WATER_FLOW, PROJECT_GRID, -1);
			project(0, rad, py, px, dam, GF_WATER, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

			break;
		}
		//v1.1.59�@��p���i�ɂĒǉ�
	case 4:
		{
			int snum = plev / 7;
			int i;
			bool flag = FALSE;

			if (only_info) return format("");


			for (i = 0; i<snum; i++)
			{
				if (summon_specific(-1, py, px, plev+20, SUMMON_GENSOUKYOU, (PM_FORCE_FRIENDLY | PM_ALLOW_UNIQUE))) flag = TRUE;
			}
			if (flag) msg_format("���Ȃ��̌Ăт����Ɍ��z���̏Z�l�������������I");
			else msg_format("�N�����Ȃ��̌Ăт����ɉ����Ȃ�����...");

		}
		break;

	case 5:
	{
		int i;
		int damage = 0;
		int num_blow = p_ptr->num_blow[0];
		int range = 2 + plev / 20;
		int flag_attack_done = FALSE;

		if (num_blow < 1) num_blow = 1;

		damage = calc_weapon_dam(0) / num_blow;
		if (only_info) return format("�˒�:%d ����:%d*%d", range, damage,num_blow);

		msg_print("�ڂ�_���ē˂����������I");
		for (i = 0; i < num_blow; i++)
		{
			if(fire_random_target(GF_CONFUSION, damage, 2, 0, range)) flag_attack_done = TRUE;
		}
		if (!flag_attack_done)
			msg_print("�������W�I�����Ȃ������B");
		break;
	}


	case 6:
	case 2:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
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
			else if(num == 2)
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�֖ڂɂ����܂�ʘA����������I",m_name);
				py_attack(y,x,0);
				if(m_ptr->r_idx)
				{
					int delay = plev + randint1(plev);
					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) delay /= 2;
					if(r_ptr->flags2 & RF2_GIGANTIC) delay -= 30;
					if(r_ptr->flags2 & RF2_POWERFUL) delay -= 20;
					if(delay < 0) delay = 0;
					m_ptr->energy_need += delay;

					if(!delay)
						msg_format("%s�͑S������ł��Ȃ��I",m_name);
					else if(delay < 30)
						msg_format("%s�̓�������u�~�܂����I",m_name);
					else
						msg_format("%s�͍U���ɋ���ł���I",m_name);
				}
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�֖ґR�Ǝa�肩�������I",m_name);
				py_attack(y,x,HISSATSU_SHINMYOU);
			}

			break;
		}
	case 7: //�P�j��
		{
			int dice = p_ptr->lev / 5;
			int sides = 3 + chr_adj / 5;
			int num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 6;
			if(p_ptr->mimic_form == MIMIC_GIGANTIC) dice *= 2;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("ῂ��P��%s�n��������I",(p_ptr->mimic_form==MIMIC_GIGANTIC)?"�����":"");
			fire_blast(GF_PSY_SPEAR, dir, dice, sides, num, 2,(PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM));

			break;
		}

	case 8:
		{
			int base = 12;
			int sides = 12;
	
			if(only_info) return format("����:%d+1d%d",base,sides);
			msg_print("���Ȃ��͏��ƂŎ����̓���@�����B");
			if(!set_mimic(base+randint1(base), MIMIC_SHINMYOU, FALSE)) return NULL;
			break;
		}



	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�f�P��p�Z*/
class_power_type class_power_eiki[] =
{
	{5,3,0,FALSE,TRUE,A_INT,0,0,"���ޗ��̋�",
		"���͂̓G�𒲍������𓾂�B"},
	{12,5,0,FALSE,TRUE,A_INT,0,0,"腖��̖�",
		"���͂̃g���b�v�ƃ����X�^�[�����m����B"},
	{18,13,40,FALSE,TRUE,A_WIS,0,7,"�f�r������",
		"���E���̃����_���ȓG�ɑ΂��j�ב����̃{���g�𐔉���B"},
	{23,30,80,FALSE,TRUE,A_INT,0,0,"*�ӎ�*",
		"�w�肵���A�C�e���̐��\�����S�ɒm��B"},
	{30,45,65,FALSE,TRUE,A_DEX,0,0,"�^���E�I�u�E�E���t",
		"�אڂ����G��̖̂��@�𕕂���B�l�ԁE�A���f�b�h�E���א��͂̓G�E�D�_�Ɍ����₷���A�����x���ȓG�E�͋����G�E���j�[�N�����X�^�[�ɂ͌����ɂ����B�A���f�b�h�ȊO�̖������ɂ͌��ʂ��Ȃ��B�Ȃ��A���@�𕕂���ꂽ�G���u���X�⃍�P�b�g�͎g����B"},
	{36,56,70,FALSE,TRUE,A_WIS,0,20,"�\���ٔ�",
		"�^�[�Q�b�g�̎��ӂɕ��𑮐��̃{�[����A���Ŕ���������B"},
	{42,80,75,FALSE,TRUE,A_INT,0,0,"�򐜗��R��",
		"�Ώۂ̓G�̌��e��z���Ƃ��ď�������B���e��1�t���A�Ɉ�̂����Ăׂ��A���߂Ɋւ�炸���ɂȂ����G�݂̂�_���A�v���C���[�̎��E����o���茳�̓G���|���Ə�����B�A���f�b�h�ȊO�̖������ɂ͌��ʂ��Ȃ��B"},
/* ����ώ~�߂�
	{45,180,80,FALSE,TRUE,A_WIS,0,0,"�M���e�B�E�I���E�m�b�g�M���e�B",
		"�אڂ����G��̂����m���œ|���B���j�[�N�����X�^�[��HP�𔼕��ɂ���B���א��͂̓G�Ɍ����₷���A�����x���ȓG�⒁�����͂̓G�ɂ͌����ɂ����A�A���f�b�h�ȊO�̖������ɂ͑S�����ʂ��Ȃ��B�����t���A�œ����G�ɓ�x�ȏ�g�����Ƃ͂ł��Ȃ��B"},
*/
	{47,160,80,FALSE,TRUE,A_CHR,0,0,"���X�g�W���b�W�����g",
		"�ɂ߂ċ��͂Ȕj�ב����̃r�[������B"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_eiki(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: 
		{
			if(only_info) return format("");
			msg_print("���Ȃ��͎苾�����o�����B");
			probing();
			break;
		}
	case 1: 
		{
			if(only_info) return format("");
			msg_print("���Ȃ��͎��͂����n�����B");
			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			break;
		}
	case 2:
		{
			bool flag = FALSE;
			int i;
			int dice = 2 + plev / 10;
			int sides = 3 + chr_adj / 10;
			int num = 3 + plev / 7;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			msg_print("��Ԃ��c�݁A����̖_�����{����яo�����B");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_HOLY_FIRE, damroll(dice,sides),1, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if (!identify_fully(FALSE)) return NULL;
			break;
		}
	case 4:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx)
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				int chance = plev * 2;
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance /= 3;
				if(r_ptr->flags3 & (RF3_HUMAN | RF3_UNDEAD | RF3_DEMON | RF3_KWAI)) chance = chance * 3 / 2;
				if(r_ptr->flags3 & RF3_ANG_COSMOS) chance /= 2;
				if(r_ptr->flags2 & RF2_POWERFUL) chance /= 2;
				if(r_ptr->flags3 & RF3_ANG_CHAOS) chance = chance * 3 / 2;
				if(r_ptr->level > plev) chance = chance * 2 / 3;
				if(r_ptr->level > plev * 2) chance /= 2;

				if ((r_ptr->blow[0].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[0].effect == RBE_EAT_GOLD)||
					(r_ptr->blow[1].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[1].effect == RBE_EAT_GOLD)||
					(r_ptr->blow[2].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[2].effect == RBE_EAT_GOLD)||
					(r_ptr->blow[3].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[3].effect == RBE_EAT_GOLD))
					chance *= 3;

				if(r_ptr->flagsr & (RFR_RES_ALL) || !(r_ptr->flags3 & RF3_UNDEAD) && !monster_living(r_ptr))
				{
					msg_format("%s�ɂ͌��ʂ��Ȃ������B",m_name);
				}
				else if( randint1(chance) < randint1(r_ptr->level) || m_ptr->r_idx == MON_SUIKA || m_ptr->r_idx == MON_YUGI || m_ptr->r_idx == MON_EIKI)
				{
					msg_format("%s�̐�𕕂���̂Ɏ��s�����B",m_name);
				}
				else
				{
					msg_format("%s�̐�𕕂����I",m_name);
					m_ptr->mflag |= MFLAG_NO_SPELL;
				}
			}

			break;
		}

	case 5:
		{
			int dam = plev * 2 + chr_adj * 2;
			int rad = 2 + plev / 45;
			if(only_info) return format("����:%d * �s��",dam);

			if (!cast_wrath_of_the_god(dam, rad,TRUE)) return NULL;
			break;
		}
//magic_num1[0]�ɏp���s���̓G��m_idx��ێ�����Bm_list���k��m_idx���ς�邱�Ƃ�����̂��H
	case 6:
		{
			monster_type *m_ptr;
			int i;

			if(only_info) return format("");

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				//magic_num[0]�ɃZ�b�g���ꂽm_idx�̓G�Ɠ���r_idx��EPHEMERA������Ί��ɏp���s���Ƃ݂Ȃ��B
				//��������EPHEMERA�t���O�t���Ń����X�^�[�����������p��A�C�e�������������炱�̕ӂŃg���u���N���邩��
				if(m_list[p_ptr->magic_num1[0]].r_idx == m_ptr->r_idx && (m_ptr->mflag & MFLAG_EPHEMERA) )
				{
					msg_print("���łɏp�����s�����B");
					return NULL;
				}
			}

			if (!get_aim_dir(&dir)) return NULL;

			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}

			m_ptr = &m_list[cave[target_row][target_col].m_idx];

			if (!m_ptr->r_idx)
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flagsr & (RFR_RES_ALL) || !(r_ptr->flags3 & RF3_UNDEAD) && !monster_living(r_ptr))
				{
					msg_format("%s�ɂ͌��ʂ��Ȃ������B",m_name);
				}
				else
				{
					p_ptr->magic_num1[0] = cave[target_row][target_col].m_idx;
//					if(cheat_xtra) msg_format("�򐜗��R��r_idx:%d",m_ptr->r_idx);
					if(summon_named_creature(0, py, px, m_ptr->r_idx, PM_EPHEMERA))
						msg_format("%s�̌��e���o�������I",m_name);
					else
						msg_print("�p�Ɏ��s�����B");
	
				}
			}

			break;
		}


	case 7:
		{
			int damage = p_ptr->lev * 7 + chr_adj * 10;
			if(damage < 300) damage = 300;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)	
				msg_format("�J�[�h����ῂ����������ꂽ�I");
			else
				msg_format("�f�߂̌��������ꂽ�I");
			fire_spark(GF_HOLY_FIRE,dir,damage,2);

			break;
		}	

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}






/*:::�p���X�B��p�Z*/
class_power_type class_power_parsee[] =
{
	{5,5,10,FALSE,FALSE,A_WIS,0,0,"����",
		"�G��̂��ɂݕt���č����A���|������B�����x���ȓG�Ɍ����₷�����A�������჌�x���ȓG�⃆�j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	{12,16,35,FALSE,TRUE,A_DEX,0,2,"�؂₩�Ȃ�m�҂ւ̎��i",
		"���E���̓G������������B���͂ɓG�������قǌ��ʂ������Ȃ�B"},
	{18,20,40,FALSE,TRUE,A_CHR,0,0,"���i�S����T",
		"�����X�^�[��̂̎��i�S�����B���i�S�����ꂽ�����X�^�[�͎�����荂���x���ȃ����X�^�[�S�ĂɓG�΂����ԂɂȂ�B���j�[�N�����X�^�[�ƒʏ�̐��_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B" },
	{23,32,50,FALSE,TRUE,A_INT,0,0,"�����Ȃ�x�҂ւ̕Ѝ�",
		"���g�������B���g�͖��߂Ɋւ�炸�����ɂ��ė��悤�Ƃ��A�����̊ԋ߂ɂ��Ȃ��Ə��ł���B�܂��G�ɓ|���ꂽ�Ƃ��ɖ������̑唚�����N�����B(�����͂��̔����̃_���[�W���󂯂Ȃ�)"},
	{27,24,60,FALSE,TRUE,A_CHR,0,6,"�W�F���V�[�{���o�[",
		"���_�U�������̃{�[������B�З͖͂��͂ɑ傫���ˑ����A�܂������̗̑͂��Ⴂ�قǈЗ͂��オ��B���_�������Ȃ��G�⃆�j�[�N�����X�^�[�ɂ͌��ʂ������B"},
	{31,60,75,FALSE,TRUE,A_CHR,0,0,"���i�S����U",
		"���̃t���A�Ń����X�^�[�����B���Ȃ��Ȃ�B" },
	{35,6,65,FALSE,TRUE,A_WIS,0,0,"�N�̍��Q��",
		"��ʂ�HP�o�[���\������Ă��郂���X�^�[�ɑ΂��Ď􂢂̋V�����s���B���񑱂��ē��������X�^�[�Ɏ􂢂�������Ƃ��̃����X�^�[�����m���őS�\�͒ቺ������B���������̎􂢂̋V���͒N�ɂ������Ă͂����Ȃ��B" },
	{39,50,70,FALSE,TRUE,A_WIS,0,0,"�O���[���A�C�h�����X�^�[",
		"���i�̉������Ăяo���B�����͕W�I�������قǋ����Ȃ�A���߂Ɋւ�炸�W�I��ǂ������A�W�I���|���Ə�����B�����̕��g�Ɠ����ɂ͌ĂׂȂ��B"},
	{43,120,80,FALSE,TRUE,A_CHR,0,0,"���i�S����V",
		"�Z���ԁA�����̎��͂̑S�Ẵ����X�^�[���m���G�΂���悤�ɂȂ�B�ʏ�̐��_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B" },
	{48,160,75,FALSE,TRUE,A_WIS,0,0,"�ω��Ԃ�",
		"�����̂���ꏊ�ɒn���̍��Α����̋���ȃ{�[���𔭐�������B�З͂͂��̃t���A�Ŏ������󂯂��_���[�W�̍��v�ɂȂ�B��x�g���ƃ_���[�W�̍��v�̓��Z�b�g�����B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

// p_ptr->magic_num1[0]���O���[���A�C�h�����X�^�[�̃^�[�Q�b�gIDX�Ɏg�p
// p_ptr->magic_num1[1]��ω��Ԃ��̃_���[�W�J�E���g�Ɏg�p�@��parsee_damage_count���g�����Ƃɂ���
// p_ptr->magic_num1[4]���N�̍��Q��̃^�[�Q�b�g�����X�^�[m_idx�ɁAmagic_num2[4]���􂢃J�E���g�Ɏg��
cptr do_cmd_class_power_aux_parsee(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int power;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("����:�s��");

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !projectable(target_row,target_col,py,px) || !cave[target_row][target_col].m_idx)
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			m_ptr = &m_list[cave[target_row][target_col].m_idx];
			r_ptr = &r_info[m_ptr->r_idx];

			power = (r_ptr->level - plev);
			if(power < 1)
			{
				msg_print("�������i�ދC�����ɂȂ�Ȃ��B");
				return NULL;
			}
			power = power * 10 + plev * 2;
			msg_print("���Ȃ��̖ڂ��ΐF��ࣁX�ƋP����...");
			fire_ball_hide(GF_OLD_CONF,dir,power,0);
			fire_ball_hide(GF_TURN_ALL,dir,power,0);

			break;
		}
	case 1: 
		{
			int power, target_num=0;
			if(only_info) return format("����:�s��");
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr;
				m_ptr = &m_list[i];
				if(!m_ptr->r_idx) continue;
				if(!m_ptr->ml || !projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
				if (!is_hostile(m_ptr)) continue;
				target_num++;
			}
			if(!target_num)
			{
				msg_format("�N�����Ȃ��E�E");
				return NULL;
			}
			power = 20 + target_num * 20;
			msg_format("���i�̔O�����͂̎҂����킹���E�E");
			confuse_monsters(power);
			break;
		}

	case 2:
	{
		int power = p_ptr->lev * 2 + chr_adj * 2;
		int y, x;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format("����:%d", power);
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];

			monster_desc(m_name, m_ptr, 0);
			msg_format("���Ȃ���%s�ɋߊ���Ě����������c", m_name);
			(void)set_monster_csleep(cave[y][x].m_idx, 0);

			if (r_ptr->flags3 & RF3_NO_CONF) power /= 2;
			if (MON_CONFUSED(m_ptr) || MON_MONFEAR(m_ptr)) power *= 2;

			if (r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flagsr & RFR_RES_ALL)
				msg_format("%s�͖��������B", m_name);
			else if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR))
				msg_format("%s�͂��Ȃ��̐��Ɏ���݂��Ȃ��I", m_name);
			else if (m_ptr->mflag & MFLAG_SPECIAL)
				msg_format("%s�ɂ͂��łɌ����Ă���B", m_name);
			else if (r_ptr->level > randint1(power) || is_pet(m_ptr))
				msg_format("%s�͘f�킳��Ȃ������B", m_name);
			else
			{
				msg_format("%s�̖ڂɗΐF�̌����h�����I", m_name);
				m_ptr->mflag |= MFLAG_SPECIAL;
			}
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
		break;
	}
	case 3:
		{
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			bool flg_g_mon = FALSE;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_GREEN_EYED)) flg_g_mon = TRUE;
				if((m_ptr->r_idx == MON_PARSEE) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt)
			{
				msg_format("���łɏp�����s���Ă���B",num);
				return NULL;
			}
			if(flg_g_mon)
			{
				msg_format("���Ƀt���A�ɉ������Ă�ł���B");
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_PARSEE, PM_EPHEMERA))
				msg_print("���Ȃ��̌��e�����ꂽ�B");
			else
				msg_print("�p�Ɏ��s�����B");


			break;
		}
	case 4:
		{
			int rad = 2;
			if(p_ptr->lev > 39) rad = 3;
			dice = (5 + p_ptr->lev / 10) * (100 + (p_ptr->mhp - p_ptr->chp) * 600 / p_ptr->mhp) / 100;
			sides = 20 + chr_adj/ 2;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�S����􂯂�ꌂ��������B");
			fire_ball(GF_REDEYE, dir, damroll(dice,sides), rad);
			break;
		}

	case 5:
	{
		if (only_info) return format("");

		msg_print("���Ȃ��̉��l�̔O����n�ɖ������E�E");
		num_repro += MAX_REPRO;
		break;
	}
	case 6:

	{
		int power = plev * 7;
		int target_m_idx;
		monster_type *m_ptr;
		char m_name[160];
		if (only_info) return format("����:%d", power);

		target_m_idx = p_ptr->magic_num1[4];

		if (p_ptr->health_who && m_list[p_ptr->health_who].ml)
		{
			int tmp_m_idx;
			//�Z���g���Ƃ���̎��E���ɒN��������ƋV���厸�s�ő��Ẩ��O����
			tmp_m_idx = check_player_is_seen();
			if (tmp_m_idx)
			{
				int count = 0;
				int i;
				monster_desc(m_name, &m_list[tmp_m_idx], 0);

				msg_format("�􂢂̋V����%s�Ɍ����Ă��܂����I�􂢂��t�������I",m_name);

				activate_ty_curse(FALSE, &count);

				//�J�E���g���Z�b�g
				p_ptr->magic_num1[4] = 0;
				p_ptr->magic_num2[4] = 0;
				break;

			}

			m_ptr = &m_list[p_ptr->health_who];
			monster_desc(m_name, m_ptr, 0);

			if (p_ptr->health_who == target_m_idx)
			{

				msg_print("���Ȃ��͈�S�s���ɘm�l�`�ɓB��ł��t���Ă���...");
				p_ptr->magic_num2[4] += 1;

				if (p_ptr->magic_num2[4] == 7)
				{
					monster_desc(m_name, m_ptr, 0);

					msg_format("%s�ɑ΂���􂢂����A�����I", m_name);
					project(0, 0, m_ptr->fy, m_ptr->fx, power, GF_DEC_ALL,  PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE, -1);
					anger_monster(m_ptr);
					set_monster_csleep(target_m_idx, 0);

					//�J�E���g���Z�b�g
					p_ptr->magic_num1[4] = 0;
					p_ptr->magic_num2[4] = 0;
				}
				else
				{
					//�Ƃ��ǂ��N����
					if (one_in_(6))
					{
						if (set_monster_csleep(target_m_idx, 0))
							msg_format("%s�͖ڂ��o�܂����B",m_name);
					}

				}

			}
			else
			{
				msg_format("���Ȃ���%s�̖��O���������m�l�`�ɓB��ł��t���n�߂��B",m_name);
				p_ptr->magic_num1[4] = p_ptr->health_who;
				p_ptr->magic_num2[4] = 1;

			}
		}
		else
		{
			msg_print("�􂢂̕W�I�����Ȃ��B");
			return NULL;
		}

		break;
	}

	case 7:
		{
			int i;
			bool flg_success = FALSE;
			bool flg_g_mon = FALSE;
			bool flg_parsee = FALSE;
			monster_type *m_ptr;
			if(only_info) return format("");

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_GREEN_EYED)) flg_g_mon = TRUE;
				if((m_ptr->r_idx == MON_PARSEE) && m_ptr->mflag & MFLAG_EPHEMERA) flg_parsee = TRUE;
			}
			if(flg_g_mon)
			{
				msg_format("���Ƀt���A�ɉ������Ă�ł���B");
				return NULL;
			}
			else if(flg_parsee)
			{
				msg_format("���͉������ĂׂȂ��B");
				return NULL;
			}

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !projectable(target_row,target_col,py,px) || !cave[target_row][target_col].m_idx)
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			p_ptr->magic_num1[0] = cave[target_row][target_col].m_idx;

			if(summon_named_creature(0, py, px, MON_GREEN_EYED, PM_EPHEMERA)) flg_success = TRUE;

			if(flg_success)	msg_print("�Ζڂ̉��������ꂽ�I");
			else msg_print("�����͌���Ȃ������B�B");

		break;
		}

	case 8:
	{
		int base = 5;
		if (only_info) return format("�͈�:%d ����:%d+1d%d",MAX_SIGHT,base,base);

		set_tim_general(base+randint1(base), FALSE, 0, 0);

	}
	break;

	///mod160319 �ω��Ԃ��̃_���[�W�J�E���g���O���ϐ��ɂ���
	case 9: 
		{
			int damage = parsee_damage_count;
			int rad;
			if(damage > 9999) damage = 9999;
			rad = 4 + damage / 2000;
			
			if(only_info) return format("����:�ő�%d",damage / 2);

			if(damage < 1)
			{
				msg_print("�܂����݂��ς����Ă��Ȃ��B");
				return NULL;
			}
			if(damage > 1000) msg_format("�ς���ɐς��������݂̔O���唚�����N�������I");
			else msg_format("���݂̔O�����ƂȂ��ĔR���オ�����I");
			project(0, rad, py, px, damage, GF_HELL_FIRE, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			parsee_damage_count = 0L ; //Be happy
			break;
		}

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�����p�Z*/
class_power_type class_power_meirin[] =
{
	{5,3,30,FALSE,FALSE,A_WIS,0,0,"�C�z�@�m",
		"���͂̐��������m����B���x��20�ɂȂ�ƒn�`�����m���A���x��30�ɂȂ�ƑS�Ẵ����X�^�[�����m����悤�ɂȂ�B"},
	{11,4,40,FALSE,FALSE,A_WIS,0,1,"�ՙ�",
		"�̓��ŋC�����A�͂��ɑ̗͂��񕜂������Z���Ԏm�C���g�Ƙr�͏㏸�𓾂�B"},
	{17,14,50,FALSE,TRUE,A_CHR,0,3,"�،���",
		"�C�����̋�����B���͓G���Q���ɓ�����Ɣ�������B�З͖͂��͂ɑ傫���ˑ�����B"},
	{22,23,55,FALSE,TRUE,A_DEX,0,6,"�ɍ��E��",
		"�߂��̓G�ɕ�����̃_���[�W��^����BAC�������G�ɂ͓�����ɂ����B"},
	{27,20,50,FALSE,FALSE,A_DEX,30,0,"������",
		"�Z��������u�ňړ����G������΍U�����ď��_���[�W��^����B��Q���ɓ�����Ǝ~�܂�B���x�����オ��Ǝ˒��������L�т�B���̍U���͘A���̃J�E���g�Ɋ܂܂�A����s���͂����l�Ɍ�������B"},
	{33,36,80,FALSE,TRUE,A_CON,0,0,"���F���Ɍ�",
		"�ꎞ�I�ɓd���E�Ή��E��C�ɑ΂���ϐ��𓾂ĕ����h��͂��㏸����B"},
	{37,60,65,FALSE,FALSE,A_DEX,0,0,"�n���V���r",
		"�אڂ����G�ɑ�_���[�W��^�����ɐ�����΂��B�ʏ�͋���ȓG�͐�����΂Ȃ��B����܂ł̘A���̉񐔂ɉ����ĈЗ͂��㏸����B"},
	{40,-80,70,FALSE,TRUE,A_CHR,0,0,"�،�����",
		"MP��S��(�Œ�80)����ċC�����̋���ȋ�����B���͓G���Q���ɓ�����Ɣ�������B�����MP�ɂ��З͂��㏸����B"},
	{43,72,75,FALSE,FALSE,A_WIS,0,0,"�ҌՓ���",
		"�ቺ�����r�͂𕜊������A�Z���ԑ啝�Șr�͏㏸�𓾂āA�m�C�����g���A����ɘA���̃J�E���g��5�������������ɂ���B���������̋Z�̏���s���͂͒ʏ�ʂ�ł���B"},
	{47,-120,80,FALSE,TRUE,A_CHR,0,0,"�ʌ��@�؏�",
		"MP��S��(�Œ�120)����A�אڂ����G�𒆐S�ɋ���ȋC�����̔������N�����B�����MP�Ƃ���܂ł̘A���̉񐔂ɉ����ĈЗ͂��㏸����B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};



//�A���J�E���g�Ƃ���concent�g�p
//p_ptr->magic_num1[0]������s���͂Ƃ��Ďg�p

cptr do_cmd_class_power_aux_meirin(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int cons = p_ptr->concent;
	int use_energy = p_ptr->magic_num1[0];

	switch(num)
	{

	case 0://�C�z�@�m
		{
			int rad = 15 + plev / 5;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_print("�C�̗��������������E�E");
			if(plev > 19) map_area(rad);
			if(plev > 29) detect_monsters_normal(rad);
			else detect_monsters_living(rad);
		}
		break;
	case 1:
		{
			int time;
			int heal;
			heal = plev / 5 + 5;
			if(only_info) return format("��:%d+1d%d",heal,heal);
			time = 2 + randint1(3);
			msg_print("�C�����߂��B");
			hp_player(heal + randint1(heal));
			set_stun(p_ptr->stun - 10);
			set_cut(p_ptr->cut - 10);
			set_afraid(0);
			set_hero(time,FALSE);
			set_tim_addstat(A_STR,(2 + plev / 10),time,FALSE);

		}
		break;
	case 2:
		{
			int rad = 1 + plev / 30;
			int dice = 1;
			int sides = 50 + chr_adj * 5;
			int base = plev * 2;
			if(base < 30) base = 30;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_format("�J�[�h������F�̒e�������ꂽ�I");
			else
				msg_format("�C�e��������I");
			fire_rocket(GF_FORCE, dir, base + damroll(dice,sides), rad);
			break;
		}

	case 3:
		{
			int base = plev + chr_adj;
			int num = 3 + plev / 24 + chr_adj / 12;
			int rad = 2 + plev / 15;
			int i;
			if(only_info) return format("����:�ő�%d * %d",base/2,num);
			msg_print("���F�̗����������N�������I");
			for (i = 0; i < num; i++) project(0, rad, py, px, base, GF_WINDCUTTER, PROJECT_KILL | PROJECT_ITEM, -1);
			break;

		}
		break;
	case 4:
		{
			int len = 3 + (plev-25) / 6;
			int damage = 50 + plev * 2;
			if(only_info) return format("�˒�:%d ����:%d",len,damage);
			if (!rush_attack2(len,GF_MISSILE,damage,0)) return NULL;

			//�A���J�E���g�����A�s�������
			new_class_power_change_energy_need = use_energy;
			do_cmd_concentrate(0);
			
			break;
		}

	case 5:
		{
			int time;
			int base = 15;
			if(only_info) return format("����:1d%d + %d",base,base);
			time = base + randint1(base);
			msg_print("���F�̋C��Z�����I");
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_shield(time,FALSE);
		}
		break;
	case 6:
		{
			int y, x;
			int dam = (plev + chr_adj + p_ptr->to_d[0]) * (7 + cons) / 7;
			if(only_info) return format("����:%d",dam);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
				msg_print("�Ȃ����U���ł��Ȃ��B");
				break;
			}
			else if (cave[y][x].m_idx)
			{
				msg_print("�V���삯��悤�Ȕ�яR���������I");
				project(0,0,y,x,dam,GF_ARROW,PROJECT_KILL,-1);
			}
			else
			{
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
				return NULL;
			}

			if (cave[y][x].m_idx)
			{
				int i;
				int ty = y, tx = x;
				int oy = y, ox = x;
				int m_idx = cave[y][x].m_idx;
				monster_type *m_ptr = &m_list[m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				char m_name[80];
	
				if(!(r_ptr->flags2 & RF2_GIGANTIC) || cons > 9)
				{


					monster_desc(m_name, m_ptr, 0);
	
					for (i = 0; i < 5; i++)
					{
						y += ddy[dir];
						x += ddx[dir];
						if (cave_empty_bold(y, x))
						{
							ty = y;
							tx = x;
						}
						else break;
					}
					if ((ty != oy) || (tx != ox))
					{
						msg_format("%s�𐁂���΂����I", m_name);
						cave[oy][ox].m_idx = 0;
						cave[ty][tx].m_idx = m_idx;
						m_ptr->fy = ty;
						m_ptr->fx = tx;
		
						update_mon(m_idx, TRUE);
						lite_spot(oy, ox);
						lite_spot(ty, tx);
	
						if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
							p_ptr->update |= (PU_MON_LITE);
					}
				}
			}
		}
		break;

	case 7:
		{
			int rad = 5;
			int dice = 1;
			int sides = p_ptr->csp * 2;
			int base = p_ptr->csp;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("����ȋC�e��������I");
			fire_rocket(GF_FORCE, dir, base + damroll(dice,sides), rad);
			break;
		}

	case 8:
		{
			int time;
			int base = 5;
			if(only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			if(use_itemcard)
				msg_print("�͂����Ă���I");
			else
				msg_print("�C�𔚔��I�ɍ��߂��I");
			do_res_stat(A_STR);
			set_tim_addstat(A_STR,110,time,FALSE);
			set_afraid(0);
			set_hero(time,FALSE);
			if(!use_itemcard) do_cmd_concentrate(1);

		}
		break;
	case 9:
		{
			int y, x;
			int dam = (plev + chr_adj * 2 + p_ptr->to_d[0]) * (10 + cons) / 10 * p_ptr->csp / 120;
			if(only_info) return format("����:%d",dam);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
			{
				int m_idx = cave[y][x].m_idx;
				monster_type *m_ptr = &m_list[m_idx];
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�̑̂�����F�̌����R��o���A�唚�����N�������I",m_name);
				project(0,7,y,x,dam,GF_FORCE,PROJECT_KILL,-1);
			}
			else
			{
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
				return NULL;
			}

		}
		break;


	default:
		if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}

	if(!use_itemcard && (use_energy < 20 || use_energy > 100))
	{
		msg_format("ERROR:use_energy�v�Z�l����������(%d)",use_energy);
		new_class_power_change_energy_need = 100;

	}

	return "";
}



/*:::�V�q��p�Z*/
class_power_type class_power_tenshi[] =
{
	{ 6,10,30,FALSE,TRUE,A_STR,0,0,"���V�̌�",
		"�n�k���N�����B"},
	{ 11,11,45,FALSE,TRUE,A_DEX,0,4,"��z�̈Ќ�",
		"�M�������̃r�[������B"},
	{ 16,25,45,FALSE,TRUE,A_INT,0,0,"�V���̐Β�",
		"�אڂ����G��̂Ƀ_���[�W��^����B���x��30�ȍ~�ɓG�̂��Ȃ��ꏊ�Ɏg���Ɗ�Βn�`�𐶐�����B"},
	{ 20,20,50,FALSE,TRUE,A_WIS,0,0,"�v�ΐݒu",
		"���̃t���A�Œn�k���N����Ȃ��Ȃ�B�������h�����n�k�͗v�΂ɒ~�ς���A���܂葽���̒n�k���~�ς����Ɨv�΂��j�󂳂��B�v�΂��j�󂳂ꂽ�Ƃ���n�k���N�����Ď��͂Ƀ_���[�W��^����B�t���A���ړ�����Ɨv�΂͏����A�~�ς��ꂽ�n�k�������������B"},
	{ 20,0,0,FALSE,TRUE,A_WIS,0,0,"�v�Ώ���",
		"�t���A�ɐݒu�����v�΂���苎��B�~�ς��ꂽ�n�k������ƒn�k���N����B"},
	{ 26,20,70,FALSE,FALSE,A_STR,0,5,"�E�C�z�X�̌�",
		"�v���Y�}�����̋��͂ȃr�[������B"},
	{ 30,30,60,FALSE,TRUE,A_WIS,0,0,"���O���z�̋��n",
		"��莞�ԕ����h��͂Ɩ��@�h��͂��㏸����B"},
	{ 33,60,60,FALSE,TRUE,A_INT,0,0,"�J�i���t�@���l��",
		"���𕑂��v�΂���������B�v�΂͕ߊl�Ȃǈꕔ�s���̑ΏۂɂȂ炸�t���A�ړ��ŏ�����B"},
	{ 38,50,70,FALSE,TRUE,A_INT,0,0,"�������C",
		"覐Α����̃��P�b�g�𔭎˂���B�����X�^�[�̂��Ȃ��ꏊ�ɓ��Ă�Ƃ����Ɋ�Βn�`�𐶐�����B" },

	{ 40,66,80,FALSE,TRUE,A_CON,0,0,"�r�X��������Ȃ��n��",
		"���E���̑S�Ăɑ΂��d�͑����̍U�����s���B"},
	{ 44,60,70,FALSE,TRUE,A_STR,0,0,"�s���y��̌�",
		"���͂̋����͈͂�*�j��*����B"},
	{ 48,160,80,FALSE,TRUE,A_CHR,0,0,"�S�l�ނ̔�z�V",
		"�ɂ߂ċ��͂ȋC�����̃��[�U�[����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

//�v�ΐݒu���W�ƒn�k�z���񐔂�p_ptr->magic_num1[0-3]���g��
cptr do_cmd_class_power_aux_tenshi(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int rad = 5 + plev / 3;
			if(only_info) return format("�͈�:%d",rad);

			msg_print("����n�ʂɓ˂����Ă��B");
			earthquake(py,px,rad);

			break;
		}
	case 1:
		{
			int damage = (20 + plev * 2  + chr_adj * 2);
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���[�U�[��������B");
			fire_beam(GF_LITE, dir, damage);

			break;
		}
	case 2:
	{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = 20 + plev * 3;
			if(plev > 29) damage += plev * 2;
			if(only_info) return format("����:%d",damage);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if(!cave_have_flag_bold(y,x,FF_MOVE) && !cave_have_flag_bold(y,x,FF_CAN_FLY)) 
			{
				msg_print("���̏ꏊ�ɑ΂��Ă͎g���Ȃ��B");
				return NULL;
			}
			if(plev < 30) msg_print("�Β����󂩂�~���Ă����B");
			else msg_print("��₪�󂩂�~���Ă����B");

			if (cave[y][x].m_idx)
			{
				project(0,0,y,x,damage,GF_ARROW,PROJECT_KILL,-1);
			}
			else if(plev > 29 && cave_naked_bold(y, x))
			{
				cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));
			}

			break;
		}	
	case 3:
		{
			if(only_info) return format("");

			if(p_ptr->magic_num1[0])
			{
				msg_format("���̃t���A�ɂ͂��łɗv�΂�ݒu���Ă���B");
				return NULL;
			}
			else if(!cave_have_flag_bold(py,px,FF_FLOOR))
			{
				msg_format("�����ɗv�΂�ݒu���邱�Ƃ͂ł��Ȃ��B");
				return NULL;
			}

			msg_format("���Ȃ��͗v�΂�ݒu�����B�v�΂͒n�ʂɒ��ݍ���ł������E�E");
			p_ptr->magic_num1[0] = py;
			p_ptr->magic_num1[1] = px;

		}
		break;
	case 4:
		{
			if(only_info) return format("");
			if(!p_ptr->magic_num1[0])
			{
				msg_format("���̃t���A�ɂ͂܂��v�΂�ݒu���Ă��Ȃ��B");
				return NULL;
			}

			msg_print("���̃t���A�̗v�΂��n����������������I");
			kanameishi_break();

		}
		break;

	case 5:
		{
			dice = p_ptr->lev / 3;
			sides = 20 + chr_adj / 2;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("������M���������ꂽ�I");
			fire_spark(GF_PLASMA,dir, damroll(dice,sides),1);

			break;
		}
	case 6:
		{
			int time;
			base = plev/2;
			sides = plev/2;

			if(base < 15) base = 15;
			if(sides < 15) sides = 15;
			
			if(only_info) return format("����:%d+1d%d",base,sides);
			time = base + randint1(sides);
			if(use_itemcard)
				msg_format("�̂��S�̂悤�ɏ�v�ɂȂ����C������I");
			else
				msg_format("���Ȃ��͋C������ꂽ�I");
			set_shield(time, FALSE);
			set_resist_magic(time,FALSE);
			break;
		}
		break;
	case 7:
		{
			int max_kaname = (plev - 20) / 3;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if(max_kaname < 3) max_kaname = 3;

			if(only_info) return format("�ő�:%d",max_kaname);
	
			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_KANAME) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_kaname)
			{
				msg_format("����ȏ�z�u�ł��Ȃ��B",num);
				return NULL;
			}

			for(i=0;i<(max_kaname / 3);i++)(void)summon_named_creature(0, py, px, MON_KANAME, PM_EPHEMERA);

		}
		break;

	case 8:
		{

		int dam = plev * 5 + chr_adj * 5;

		if (only_info) return format("����:%d", dam);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print("�v�΂����ł������I");
		fire_rocket(GF_KANAMEISHI, dir, dam, 1);

		}
		break;

	case 9: 
		{
			int dam = plev * 3 + chr_adj * 5;

			if(only_info) return format("����:%d",dam);
			msg_format("��n���L�͈͂ɗ��N�����I");
			project_hack2(GF_GRAVITY,0,0,dam);			
			break;
		}

	case 10:
		{
			int rad = 3;
			if(only_info) return format("�͈�:%d",rad);
			if(!dun_level)
			{
				msg_format("�����ł͎g���Ȃ��B");
				return NULL;
			}

			msg_print("�n�k�ϓ����N�������I");
			destroy_area(py,px,rad,FALSE,FALSE,TRUE);
		}
		break;
	case 11:
		{
			int damage = p_ptr->lev * 9 + chr_adj * 10;
			if(damage < 400) damage = 400;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("��z�̌�����Ïk���ꂽ�C���������ꂽ�I");
			fire_spark(GF_FORCE,dir,damage,2);

			break;
		}
		break;

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::����p�Z*/
class_power_type class_power_ran[] =
{
	{ 10,8,40,FALSE,TRUE,A_CHR,0,0,"�ϋ�K����̌_��",
		"�����E�d���E�A���f�b�h��̂�z���Ƃ��ČĂяo���B"},
	{ 15,16,25,FALSE,TRUE,A_WIS,0,5,"�A���e�B���b�g�u�f�B�X�g",
		"�����̎��͂ɕ�����̑M�������U�����s���B���x�����オ��Ɣ͈͂��L����B"},
	{ 20,20,35,FALSE,TRUE,A_INT,0,0,"���_��",
		"���z���Ƃ��ď�������B�������ꂽ��͕ߊl�Ȃǂ��s�\�ŊK�ړ�����Ə�����B���x��30�ȏ�ɂȂ�Ɖ���������ԂŌĂяo���B"},
	{ 25,24,45,FALSE,TRUE,A_INT,0,0,"�߈�䶋g��V",
		"��莞�ԁA���V�E�����𓾂�B���x��35�ȏ�ɂȂ�ƍ����ړ����ǉ������B"},
	{ 30,40,40,FALSE,TRUE,A_INT,0,0,"���[�j���^���R���^�N�g",
		"�G��̂����m���Ŗ������z���ɂ���B���j�[�N�����X�^�[�ɂ͌����Ȃ��B"},
	{ 35,32,70,FALSE,FALSE,A_DEX,0,0,"�v�����Z�X�V��",
		"�^�[�Q�b�g�ׂ̗Ɉ�u�ňړ����Ă��̂܂܍U������B���E�O�̓G�ɂ��L���B"},
	{ 40,64,80,FALSE,TRUE,A_WIS,0,0,"���疜���얀",
		"���E���̑S�Ăɑ΂��Ĕj�ב����U�����s���B"},
	{ 45,128,80,FALSE,TRUE,A_INT,0,0,"���Ė����p",
		"�Z���ԁA�g�̔\�͂��啝�ɏ㏸���������q�[���[������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_ran(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int typ;
			if(only_info) return format("");

			if(one_in_(3)) typ = SUMMON_DEMON;
			else if(one_in_(2)) typ = SUMMON_UNDEAD;
			else typ = SUMMON_KWAI;

			if(summon_specific(-1, py, px, plev, typ, (PM_FORCE_PET)))
				msg_format("������������Ă����I");
			else 
				msg_format("��������Ȃ������E�E");

			break;
		}
	case 1:
		{
			int rad = 1 + plev / 16;
			int num = (plev+5) / 6;
			int damage = (10 + plev  + chr_adj) / 2 ;
			int flg = (PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL);

			if(only_info) return format("����:�ő�%d*%d",damage,num);
			msg_format("�̈󂪍�����]�����I");
			for(;num>0;num--) project(0,rad,py,px,damage,GF_LITE,flg,-1);

			break;
		}
	case 2:
		{
			int i;
			bool flag = FALSE;
			monster_type *m_ptr;
			monster_race *r_ptr;
			u32b mode = PM_EPHEMERA;

			if(only_info) return "";
			if(plev > 29) mode |= PM_HASTE;

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_CHEN) ) flag = TRUE;
			}
			if(flag)
			{
				msg_print("��͂��łɂ��̃t���A�ɂ���B");
				return NULL;
			}
			if(summon_named_creature(0, py, px, MON_CHEN, PM_EPHEMERA))
				msg_print("����Ăяo�����B");
			else
				msg_print("���̏����Ɏ��s�����B");

			break;		

		}
	case 3:
		{
			int time;
			base = 25;
			if(only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			set_fast(time,FALSE);
			set_tim_levitation(time,FALSE);
			if(plev > 34) set_tim_speedster(time,FALSE);

		}
		break;
	case 4:
		{
			int power = 50 + plev * 2 + chr_adj * 5;
			if(power < 120) power = 120;

			if(only_info) return format("����:%d",power);
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay())
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			msg_print("���Ȃ��͓G���x�z���悤�Ǝ��݂��E�E");
			fire_ball_hide(GF_CHARM,dir,power,0);
			break;
		}
	case 5:
		{
			int x, y;
			int dist = plev / 2;
			if(only_info) return format("�͈�:%d",dist);
			if (!tgt_pt(&x, &y)) return NULL;

			if(!cave[y][x].m_idx || !m_list[cave[y][x].m_idx].ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			else if (distance(y, x, py, px) > dist)
			{
				teleport_player(30, 0L);
				msg_print("�u�Ԉړ��Ɏ��s�����B");
			}
			else
			{
				teleport_player_to(y, x, 0L);
				if(distance(py,px,y,x) > 1)
				{
					msg_print("���܂��W�I�̏ꏊ�ɍs���Ȃ������B");
				}
				else
				{
					msg_print("���Ȃ��͈�u�ŕW�I�ׂ̗Ɍ��ꂽ�I");
					py_attack(y,x,0L);
				}
			}
			break;
		}
	case 6:
		{
			int dice = 5 + p_ptr->lev / 2;
			int sides = 10 + chr_adj / 5;
			if(dice < 20) dice = 20;
			if(only_info) return format("����:%dd%d",dice,sides);
			if(use_itemcard)
				msg_format("�J�[�h�����ʂ̎D�������o���Ď��͂𖄂ߐs������!");
			else
				msg_format("��ʂ̎D�Ŏ��͂𖄂ߐs�������I");
			project_hack2(GF_HOLY_FIRE,dice,sides,0);
			break;
		}
		break;
	case 7:
		{
			int percentage;
			int base = 7;
			int time;
			if(only_info) return format("����:%d+1d%d",base,base);
			msg_format("�`���̒��l�̗͂��g�ɏh�����C������I");
			percentage = p_ptr->chp * 100 / p_ptr->mhp;
			time = base + randint1(base);
			set_tim_addstat(A_STR,110,time,FALSE);
			set_tim_addstat(A_DEX,110,time,FALSE);
			set_tim_addstat(A_CON,110,time,FALSE);
			set_fast(time, FALSE);
			set_hero(time,FALSE);
			set_tim_speedster(time, FALSE);				
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= (PR_HP );
			redraw_stuff();


			break;
		}

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�z�K�q��p�Z*/
class_power_type class_power_suwako[] =
{
	{6,4,20,FALSE,TRUE,A_DEX,0,1,"���^�_",
		"�˒��̒Z���������̃{�[������B"},
	{12,15,35,FALSE,FALSE,A_WIS,0,4,"�k��̓S�̗�",
		"�������̃{�[������B���̍U����7/8�̊m���ōĔ�������B"},
	{16,32,40,TRUE,FALSE,A_CON,0,20,"�z�K����",
		"�������̃u���X��f���B�З͂�HP��1/4�ɂȂ�B"},
	{20,30,60,TRUE,FALSE,A_DEX,0,0,"�^�ΐ_",
		"�w�肵�������̗אڃO���b�h�Ɋ�𐶐�����B"},
	{24,27,65,FALSE,TRUE,A_WIS,0,5,"�蒷��������",
		"����ȋO���̃��[�U�[�Ŏ��͂̃����_���ȓG���U������B"},
	{28,0,0,FALSE,FALSE,A_CHR,0,0,"�I�[���E�F�C�Y�~���ł��܂�",
		"��莞�ԓ~�����A���̊�HP��MP���\�{�̑����ŉ񕜂���B�~���J�n���ɑS�Ă̈ꎞ���ʂ����������B�~�����͂قƂ�ǂ̍U�����疳�G�����_���[�W���󂯂�Ɠr���ŋN�������B�~������ڊo�߂鎞�ɍő�Œʏ�s�����̔{���x�̌����ł���B"},
	{32,43,60,FALSE,TRUE,A_CON,0,6,"�P������񕗉J�ɕ�����",
		"���͂̃����_���ȏꏊ�ɐ������̃{�[����A���Ŕ���������B"},
	{35,35,70,FALSE,TRUE,A_DEX,0,0,"��n�̌�",
		"��莞�ԕǂ𔲂�����悤�ɂȂ�B"},
	{40,80,70,FALSE,TRUE,A_WIS,0,0,"�M��ꂽ��n",
		"���E���̑S�Ă����������ōU������B"},
	{44,90,80,FALSE,TRUE,A_CHR,0,0,"�M��_����",
		"�z�����M��_����̌Ăяo���B"},
	{48,-100,90,FALSE,TRUE,A_CHR,0,0,"�Ԍ�����",
		"MP��S��(�Œ�100)����A�אڂ����G�ɖ������̑�_���[�W��^���A���m���őS�\�͂�ቺ������B����MP�ʂƌ��݂�HP�̒ቺ�x�ɂ��З͂��ω�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


/*:::�z�K�q�p���Z*/
cptr do_cmd_class_power_aux_suwako(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int point = p_ptr->magic_num1[0] / 100; //�ő�999

	switch(num)
	{
	case 0:
		{
			int damage = 20 + plev + chr_adj;
			int dist = 2 + plev / 10;
			if(only_info) return  format("�˒�:%d ����:%d",dist,damage);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�����ʂ����^����т��������B");
			fire_ball(GF_WATER, dir, damage,1);
			break;
		}
	case 1://�k��̓S�̗�
		//v1.1.44 ���[�`��������
		{
			int tx,ty;
			int dam = 10 + plev/2 + chr_adj/2 ; 
			int rad = 0;

			if(only_info) return format("����:%d * �s��",dam);

			if (!get_aim_dir(&dir)) return NULL;

			msg_format("�S�ւ𓊝������I");
			execute_restartable_project(0, dir, 8, 0, 0, dam, GF_ARROW, rad);

		}
		break;
	case 2:
		{
			int dam = p_ptr->chp / 4;
			if(dam<1) dam = 1;
			if(dam > 800) dam=800;
			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("�J�[�h��������������o�����I");
			else
				msg_print("���Ȃ��͐�����f�����I");
		
			fire_ball(GF_WATER, dir, dam, -2);
			break;
		}
	case 3:
		{
			int y,x;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (!cave_naked_bold(y, x))
			{
				msg_print("�����ɂ͊�����Ȃ��B");
				return NULL;
			}
			msg_print("�n�ʂ���^�̂悤�Ȋ₪�o�������I");
			cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));

			break;

		}
	case 4:
		{
			int dam = plev + chr_adj/2 ;
			int cnt;

			if(only_info) return format("����:%d*�s��",dam);
			msg_format("�蒷�����̗͂��؂肽�B");
			for(cnt=0;cnt<4;cnt++) fire_random_target(GF_MISSILE,dam,6,0,0);

		}
		break;
	case 5:
		{
			base = 10;
			if(only_info) return format("����:%d",base);

			if (p_ptr->inside_arena)
			{
				msg_print("���Ȃ��͂��łɖ��̒��ɂ���B");
				return NULL;
			}

			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			set_image(0);
			dispel_player();
			reset_tim_flags();
			set_tim_general(base,FALSE,0,0);
			do_cmd_redraw();

		}
		break;
	case 6:
		{
			int rad = 2 + plev / 24;
			int damage = plev*2  + chr_adj*2 ;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("�V���瑽���̐��򂪍~�蒍�����B");
			cast_meteor(damage, rad, GF_WATER);

			break;
		}
	case 7:
		{
			int base = plev / 2;
			if(base < 10) base = 10;
			if(only_info) return format("����:%d+1d%d",base,base);

			set_kabenuke(base + randint1(base), FALSE);

			break;
		}
	case 8:
		{
			int dam = plev * 3 + chr_adj * 5;
			if(only_info) return format("����:%d",dam);
			msg_print("��n���������܂�AᏋC�𕬂��o�����I");
			project_hack2(GF_POLLUTE,0,0,dam);

			break;
		}
	case 9:
		{
			if(only_info) return format("");
			if(summon_named_creature(0, py, px, MON_TATARI, PM_FORCE_PET))
				msg_print("�M��_���Ăяo�����I");
			else
				msg_print("�M��_�͌���Ȃ������B");

		}
		break;
	case 10:
		{
			int dam = 5 * (p_ptr->csp+1) + chr_adj * 10;
			dam = dam * (p_ptr->mhp + (p_ptr->mhp - p_ptr->chp) * 2) / p_ptr->mhp;
			if(only_info) return format("����:%d",dam/2);
			//Hack ��ʂ����ɂ��邽��world_monster���g��
			world_monster = -1;
			do_cmd_redraw();
			Term_xtra(TERM_XTRA_DELAY, 1500);
			project(0,1,py,px,dam/5,GF_DEC_ALL, PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE, -1);
			project(0,1,py,px,dam,GF_DISP_ALL,PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE,-1);
			world_monster = 0;
			p_ptr->redraw |= (PR_MAP);
			handle_stuff();

			break;
		}

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::������g����p�Z*/
//p_ptr->magic_num1(2)[0-95?]��������Ɏg����B
class_power_type class_power_magic_eater[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"������z��",
		"������i��E���@�_�E���b�h�j��̂Ɏ�荞�ށB��荞�񂾖������m�R�}���h����g�p�ł���B"},

	{10,15,30,FALSE,TRUE,A_INT,0,0,"���͏[�U�T",
		"�̂Ɏ�荞��ł��Ȃ�������̎g�p�񐔂��񕜂�����B�[�U�Ɏ��s���Ė�������邱�Ƃ�����B������̉񐔂���ɋ߂��قǎ��s���ɂ����B"},

	{20,20,45,FALSE,TRUE,A_INT,0,0,"���͏[�U�U",
		"�̂Ɏ�荞�񂾏�E���@�_�E���b�h�̎g�p�񐔂��[�U����B�[�U�ʂɂ̓��x���E�m�\�E�A�C�e�����x���E�ő�g�p�\�񐔂��e�����A����x�Ȗ�����͈�x�̏[�U�ł͎g�p�񐔂������Ȃ����Ƃ�����B"},

	{30,30,60,FALSE,TRUE,A_INT,0,0,"���͏[�U�V",
		"�����ς݂̑����i�̏[�U���Ԃ�Z�k����B"},

	{40,40,80,FALSE,TRUE,A_INT,0,0,"���͔���",
		"�̂Ɏ�荞�񂾖�����̈З͂���ʂ����߂Ďg�p����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_magic_eater(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return "";
			gain_magic();
			break;
		}
	case 1:
		{
			int power = 50 + plev * 3;
			if(only_info)return format("����:%d",power);
			if (!recharge(power)) return NULL;
			break;
		}
	case 2:
		{
			int power = (50+plev*2) * (20+adj_general[p_ptr->stat_ind[A_INT]]*3) * (150+randint1(100));
			if(only_info) return "";
			if(!recharge_magic_eater(power)) return NULL;
			break;
		}
	case 3:
		{
			int base = plev*2;
			if(only_info)return format("�Z�k�^�[��:%d+1d%d",base,base);
			if(!do_cmd_recharge_equipment(base+randint1(base))) return NULL;
			break;
		}

	case 4:
		{
			if(only_info) return "";
			if (!do_cmd_magic_eater(FALSE, TRUE)) return NULL;
			break;
		}

	default:
		if(only_info) return "������";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::�i���Ɨp���Z*/
class_power_type class_power_martial_artist[] =
{
	{1,0,0,FALSE,FALSE,A_WIS,0,0,"�C��������",
		"�ꎞ�I��MP��啝�ɑ���������BMP���ʏ��葝�����Ă����Ԃ��Ɗi���̒ʏ�U���⑼�̋Z�̈З͂������Ȃ�B"},
	{4,12,20,FALSE,FALSE,A_DEX,0,0,"���z��",
		"���͂𖾂邭�Ƃ炵�A���Ɏア�G�Ƀ_���[�W��^����B�p���[���オ��Ǝ��E���̓G������������悤�ɂȂ�B"},
	{8,17,25,FALSE,FALSE,A_STR,0,0,"�g����",
		"�C�����̃{���g����B�p���[���オ��ƃr�[���ɂȂ�B"},
	{12,12,50,FALSE,FALSE,A_DEX,30,0,"��d�̋ɂ�",
		"�אڂ����ꏊ�ɕ��𑮐��̍U�����s���B�ǂȂǂ��@�邱�Ƃ��ł��邪�n�ʂ̃A�C�e��������B�p���[���オ��ƕ����̔��a���L����B"},
	{15,24,35,FALSE,FALSE,A_WIS,60,0,"����p",
		"�ꎞ�I�ɕ��V����B���x��40�ȍ~�͍����ړ��ł���悤�ɂȂ�B�������d���Ǝ��s���₷���B"},

	{18,30,50,FALSE,TRUE,A_WIS,0,0,"���q���K�e",
		"�˒��̒Z���C�����̃r�[������BHP�̍ő�l����̌����ʂɉ����ĈЗ͂��㏸���AHP�������ȉ��̂Ƃ��ɂ͎����𒆐S�Ƃ����^�̃{�[���ɕω�����B"},
	{20,20,75,FALSE,TRUE,A_INT,0,4,"�~",
		"���͂̒n�`�E�A�C�e���E�g���b�v�E�����X�^�[��S�Ċ��m����B�p���[���オ��Ɣ͈͂��L����B"},
	{22,66,75,FALSE,TRUE,A_CHR,0,0,"�h���C���^�b�`",
		"�אڂ�����������HP��啝�ɋz������B�������̎푰�ɂ����g���Ȃ��B"},
	{24,-30,55,FALSE,FALSE,A_STR,30,0,"�^��g",
		"MP��S��(�Œ�30)�g�p���A���͂̑S�Ăɑ΂����̐n�ōU�����s���BAC�������G�ɂ͓�����ɂ����B"},
	{27,25,50,FALSE,FALSE,A_DEX,80,0,"�k�n",
		"�^�[�Q�b�g�ׂ̗܂ň�u�ňړ������̂܂܍U������B��Q���ɓ�����Ǝ~�܂�B�������d���Ǝ��s���₷���B"},
	{30,45,60,FALSE,TRUE,A_WIS,20,3,"���C�E�K��",
		"�G���Q���ɓ�����Ɣ������鋭�͂ȋC�����̋�����B�j�ׂɎア�푰�̏ꍇ�͈Í������́u�u���b�N�E���C�K���v�ɂȂ�B"},

	{32,50,65,FALSE,TRUE,A_CON,0,0,"����ٍb",
		"�Z���Ԙr�͂Ɗ�p���𑝉������������邪�A50+1d100�̃_���[�W���󂯂�B"},
	{35,-70,65,FALSE,TRUE,A_DEX,0,0,"�R���F�̔g�䎾��",
		"MP��S��(�Œ�70)����A�אڂ����G�ɑM�������̃_���[�W��^����BMP�̗ʂɉ����ă_���[�W���啝�ɏ㏸���A�Ƃ��ɃA���f�b�h�ɂ͑�_���[�W��^����B�������̎푰�ɂ͎g���Ȃ��B"},

	{37,-50,80,FALSE,TRUE,A_CON,0,0,"�W�C�@",
		"MP��S��(�Œ�50)����A�����MP�̕���HP���񕜂��Đ؂菝�������B"},
	{40,-100,85,FALSE,TRUE,A_STR,0,0,"�J���n���g",
		"MP��S��(�Œ�100)����A�C�����̋��͂ȃr�[������B"},
	{44,-60,70,FALSE,TRUE,A_STR,0,0,"�n��̔ӎ`",
		"MP��S��(�Œ�60)����A���͂�*�j��*����B"},
	{48,-150,75,FALSE,TRUE,A_WIS,0,0,"�E����",
		"MP��S��(�Œ�150)����A�����Z���ԃX�s�[�h�E�g�̔\�́E�U���͂�啝�ɏ㏸������B���������ʂ��؂ꂽ�Ƃ������ƞN�O��ԂɂȂ�A��x�g���Ƃ��΂炭�͎g���Ȃ��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};
//�E������tim_general[0,1]���g��
cptr do_cmd_class_power_aux_martial_artist(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int ex_power = (p_ptr->csp > p_ptr->msp)?(p_ptr->csp - p_ptr->msp):0;
	int dir;

	if(p_ptr->tim_general[0]) ex_power += 200;

	switch(num)
	{
	case 0:
		{
			//int max_csp = MAX(p_ptr->msp*3, p_ptr->lev*5+5);
			int max_csp = MIN(p_ptr->msp*4, 255);
			if(only_info) return format("�ő�:%d",max_csp);

			if (total_friends)
			{
				msg_print("���͔z���̂��ƂɏW�����Ă��Ȃ��ƁB");
				return NULL;
			}
			if(p_ptr->tim_general[0]) 
			{
				msg_print("�C�𔚔��I�ɍ��߂��I");
				p_ptr->csp += p_ptr->msp;
			}
			else
			{
				msg_print("�C�����߂��I");
				p_ptr->csp += p_ptr->msp / 2;
			}
			if (p_ptr->csp >= max_csp)
			{
				p_ptr->csp = max_csp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
			p_ptr->update |= (PU_BONUS);

			break;
		}
	case 1:
		{
			int power = plev + ex_power;
			int rad = MIN(10,(3 + power / 20));
			if(only_info) return format("");
			msg_format("���Ȃ��͓ˑRῂ��������I");
			lite_area(damroll(2, power / 5), rad);
			if(power >= 50) confuse_monsters(power);
		}
		break;
	case 2:
		{
			int dice = 3 + ex_power / 7;
			int sides = 4 + plev / 8;
			if(only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			if(dice > 9)
			{
				msg_format("�C�C��������I");
				fire_beam(GF_FORCE,dir,damroll(dice,sides));
			}
			else
			{
				msg_format("�C�e��������I");
				fire_bolt(GF_FORCE,dir,damroll(dice,sides));
			}
		}
		break;
	case 3:
		{
			int y,x;
			int rad = plev / 30 + ex_power / 50;
			int damage = 20 + plev + ex_power / 3;

			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			msg_format("��d�̏Ռ���^�����I");
			project(0,rad,y,x,damage,GF_DISINTEGRATE,PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM,-1);
			break;

		}
	case 4:
		{
			int time = 15 + plev / 2;
			if(only_info) return format("����:%d",time);

			if(plev > 39)
			{
				set_tim_speedster(time, FALSE);
			}
			else
			{
				msg_format("�C�̗͂Őg�̂��������I");
				set_tim_levitation(time, FALSE);
			}

			break;
		}
	case 5:
		{
			int dist = 2 + ex_power / 60 + plev / 35;
			int damage=(plev + ex_power / 2) * (p_ptr->mhp - p_ptr->chp) / 100;

			if(dist > 7) dist = 7;
			if(p_ptr->hero) damage /= 3;

			if(!damage)
			{
				if(only_info) return  format("����:%d",damage);
				msg_format("�C����Ƃ��Ƃ������C���y�������B");

			}
			else if(p_ptr->chp > p_ptr->mhp / 2)
			{
				if(only_info) return  format("�˒�:%d ����:%d",dist,damage);
				project_length = dist;
				if (!get_aim_dir(&dir)) return NULL;
				msg_format("���Ȃ��͏d���C��������I");
				fire_beam(GF_FORCE, dir, damage);
			}
			else
			{
				damage *= 4;
				if(only_info) return  format("����:�ő�%d",damage/2);
				msg_format("����ȋC�����ݒn�ʂɑ匊���J�����I");
				project(0,dist,py,px,damage,GF_FORCE,PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM,-1);
			}

			break;
		}
	case 6:
		{
			int rad = 5 + plev / 10 + ex_power / 5;
			if(rad > 50) rad = 50;
			if(only_info) return  format("�͈�:%d",rad);
			msg_format("���͂�Z���ȔO�Ŗ��������E�E");
			map_area(rad);
			detect_all(rad);
			break;

		}
	case 7: //�h���C���^�b�`
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 2 + ex_power;
			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("���Ȃ��͗₽�����%s�ɐG�ꂽ�E�E",m_name);
				if(r_ptr->flagsr & RFR_RES_ALL || !monster_living(r_ptr))
				{
					msg_format("%s�ɂ͑S�����ʂ��Ȃ��悤���B",m_name);
					break;
				}
				if(r_ptr->flagsr & RFR_RES_NETH)
				{
					msg_format("%s�͂����炩�̑ϐ����������B",m_name);
					damage /= 2;
				}
			
				if(m_ptr->hp < damage) damage = m_ptr->hp + 1;

				if(damage>0) 
				{
					msg_format("%s����̗͂��z��������I",m_name);
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_DISP_ALL,PROJECT_KILL,-1);
					hp_player(damage);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	
	case 8:
		{
			int dam = plev * 3 + ex_power ;
			if(only_info) return format("����:%d",dam);
			msg_print("���Ȃ��͌�������]���n�߂��I");
			project_hack2(GF_WINDCUTTER,0,0,dam);

			break;
		}
	case 9:
		{
			int len = plev / 7 + ex_power / 20;
			if(len > 16) len = 16;
			if(only_info) return format("�˒�:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 10://���C�E�K��
		{
			int dam = 50 + plev + ex_power;
			int rad = 1 + plev / 35 + ex_power / 50;
			int dist = MIN(15,5 + ex_power / 20);
			if(rad > 5) rad = 5;
			if(only_info) return format("����:%d",dam);

			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(is_hurt_holy() > 0)
			{
				msg_print("���Ȃ��̌����獕�����������ꂽ�I");
				fire_rocket(GF_DARK, dir, dam, rad);
			}
			else
			{
				msg_print("���Ȃ��̌�����ῂ����������ꂽ�I");
				fire_rocket(GF_FORCE, dir, dam, rad);
			}
			break;
		}
	case 11:
		{
			int time = plev / 2;

			if(only_info) return format("����:%d",time);
			if(p_ptr->chp < 150 && !get_check_strict("�̗͂����Ȃ�����B�{���Ɏg���܂����H", CHECK_OKAY_CANCEL)) return NULL;

			msg_print("�g�̂̐��ݔ\�͂������o�����I");
			set_tim_addstat(A_STR,105,time,FALSE);
			set_tim_addstat(A_DEX,105,time,FALSE);
			set_fast(time,FALSE);
			take_hit(DAMAGE_USELIFE, 50 + randint1(100), "����ٍb�̕���", -1);

			break;
		}
	case 12: //�R���F�̔g�䎾��
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 4 + ex_power * 2;
			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flagsr & RFR_RES_ALL)
				{
					msg_format("%s�ɂ͑S�����ʂ��Ȃ��悤���B",m_name);
					break;
				}
				if(r_ptr->flags3 & RF3_UNDEAD)
				{
					msg_format("�u�ӂ邦�邼�n�[�g�I�R���s����قǃq�[�g�I���ނ����t�̃r�[�g�I�v");
					damage *= 2;
				}
				msg_format("%s�ɔg��G�l���M�[��@�����񂾁I",m_name);
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_LITE,PROJECT_KILL,-1);

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	
	case 13:
		{
			int heal = p_ptr->csp;
			if(p_ptr->tim_general[0]) heal += 200;
			if(only_info) return format("��:%d",heal);
			msg_format("�C��g�̂ɏ��点���E�E");
			hp_player(heal);
			set_cut(0);
			break;

		}
	case 14:
		{
			int dice = 10 + ex_power / 3;
			int sides = plev / 3;
			int rad = 1;
			if(p_ptr->tim_general[0]) rad = 2;

			if(only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�u�g�@�\�\�\�\�I�v");
			fire_spark(GF_FORCE,dir,damroll(dice,sides),rad);
		}
		break;
	case 15:
		{
			int rad = 7 + ex_power / 15;
			if(only_info) return format("�͈�:%d",rad);
			msg_print("�n�ʂɋT�􂪓�������R�ꂾ�����E�E");
			destroy_area(py,px,rad,FALSE,FALSE,FALSE);
		}
		break;
	case 16://�E�����@�Z���̂̃^�C�}�[�ƋZ�̍Ďg�p�֎~�^�C�}�[�̓��tim_general���g��
		{
			int time = p_ptr->csp / 50;
			int percentage = p_ptr->chp * 100 / p_ptr->mhp;
			if(only_info) return format("����:%d",time);

			msg_print("�u�ԓI�ɐ퓬�͂𐔔{�ɍ��߂��I");
			set_tim_general(time,FALSE,0,0);
			set_tim_general(100+randint1(200),FALSE,1,0);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			p_ptr->update |= PU_BONUS;
			handle_stuff();	
		}
		break;


	default:
		if(only_info) return format("������");
		//msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::���}����p�Z*/
class_power_type class_power_yamame[] =
{

	{1,5,0,TRUE,FALSE,A_DEX,0,0,"���𒣂�",
		"�n�`�u�w偂̑��v�����B���̏�ɂ����AC�{�[�i�X�𓾂���B���x��30�ȍ~�͂���ɍ����ړ��\�͂𓾂��A�܂����̏�ɂ��郂���X�^�[�����m����B"},
	{12,10,35,FALSE,TRUE,A_STR,0,4,"�L���v�`���[�E�F�u",
		"���͂ɒw偂̑��𒣂菄�点��B���x��25�ȏ�ɂȂ�Ƃ���Ɏ��͂̓G�Ƀ_���[�W��^�����������悤�Ƃ���B"},
	{19,22,40,FALSE,FALSE,A_CON,0,0,"�����s���̔M�a",
		"���E���̓G�������A�N�O������B"},
	{23,16,55,FALSE,FALSE,A_DEX,50,0,"�J���_�^���[�v",
		"�߂��̒w偂̑����w�肵�A�����ֈ�u�ňړ�����B���e���|�[�g��Ԃł��g�p�ł���B�������̒w偂̑��̏ꏊ�Ƀ����X�^�[�������ꍇ�����͈ړ��������̃����X�^�[�������̂Ƃ���܂ň����񂹂�B����������ȓG�ɂ͌��ʂ��Ȃ��B�܂������i���d���Ǝ��s���₷���B"},
	{28,28,60,FALSE,FALSE,A_WIS,0,10,"�t�B���h�~�A�Y�}",
		"���E���̂��ׂĂɉ��������̍U�����d�|����B"},
	{33,30,65,TRUE,FALSE,A_CON,0,0,"��������",
		"�אڂ����G�Ɋ��݂��ă_���[�W��^����B�őϐ��������Ȃ��G�ɂ͎O�{�̃_���[�W��^���A�U���͂�ቺ�����A��m���ňꌂ�œ|���B"},
	{37,40,70,FALSE,FALSE,A_STR,0,0,"�K�i����",
		"������Ƃ���ɊK�i�����B���K�i�ɂȂ邱�Ƃ�����B�n���N�G�X�g�_���W�����ł͎g���Ȃ��B" },
	//v1.1.91
	{40,40,70,FALSE,FALSE,A_DEX,50,0,"���F�m���E�F�u",
		"�߂��̒w偂̑����w�肵�A�����ֈ�u�ňړ�����B���e���|�[�g��Ԃł��g�p�ł���B����ɂ��̒n�_�ɋ��͂ȓő����̃{�[���𔭐�������B���̒n�_�Ƀ����X�^�[�������ꍇ�߂��ɒ��n����B�܂������i���d���Ǝ��s���₷���B" },

	{44,48,80,FALSE,FALSE,A_CHR,0,0,"�ΌA�̒w偂̑�",
		"���E���̏��ɒw偂̑��𒣂菄�点��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_yamame(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: //���𒣂�
		{
			if(only_info) return format("");

			if(cave_have_flag_bold(py,px,FF_SPIDER_NEST_1))
			{
				msg_print("�����ɂ͊��ɑ��𒣂��Ă���B");
				return NULL;
			}

			project(0, 0, py, px, 0, GF_MAKE_SPIDER_NEST, PROJECT_GRID, -1);
			if (cave_have_flag_bold(py, px, FF_SPIDER_NEST_1))
			{
				msg_print("���𒣂����B");

				//v1.1.75 �����Y��ǉ�
				p_ptr->redraw |= (PR_STATE); 
				p_ptr->update |= (PU_BONUS);
			}
			else
				msg_print("�����ɂ͑��𒣂�Ȃ��B");




			break;
		}
	case 1: //�L���v�`���[�E�F�u
		{
			int rad =  1 + (plev-10) / 15;
			if(plev < 25)
			{
				if(only_info) return format("");
				msg_print("�w偂̎���ԏ�ɍL�����B");
				project(0, rad, py, px, 0, GF_MAKE_SPIDER_NEST, PROJECT_GRID, -1);

			}
			else
			{
				int dam = plev * 2 + chr_adj * 2;
				if(only_info) return format("����:�`%d",dam/2);
				msg_print("����Ԃ�������I");
				project(0, rad, py, px, 0, GF_MAKE_SPIDER_NEST, PROJECT_GRID | PROJECT_HIDE, -1);
				project(0, rad, py, px, dam, GF_INACT, PROJECT_STOP | PROJECT_GRID | PROJECT_KILL, -1);

			}

			break;
		}

	case 2: //�����s���̔M�a
		{
			int power = plev * 3;
			if(power < 50) power = 50;
			if(only_info) return format("����:%d",power);
			if(use_itemcard)
				msg_print("�J�[�h���������T���U�炵�Ă���...");
			else
				msg_print("���Ȃ��͕a���ۂ��T���U�炵���B");
			confuse_monsters(power);
			stun_monsters(power);

			break;
		}
	case 3: //�J���_�^���[�v
	case 7: //v1.1.91 ���F�m���E�F�u
		{
			int range;
			int x = 0, y = 0;
			int dam = 0;

			if (num == 3)
			{
				range = plev / 2;
				if (only_info) return format("�͈�:%d", range);
			}
			else
			{
				range = plev / 3;
				dam = plev * 7 + chr_adj * 5;
				if (only_info) return format("�͈�:%d ����:%d", range,dam);
			}

			if (!tgt_pt(&x, &y)) return NULL;

			if(!cave_have_flag_bold((y), (x), FF_SPIDER_NEST_1))
			{
				msg_print("�����ɂ͑����Ȃ��B");
				return NULL;
			}
			else if(distance(py,px,y,x) > range)
			{
				msg_print("�����͉�������B");
				return NULL;
			}

			if (num == 3)
			{
				if (cave[y][x].m_idx)
				{
					monster_type *m_ptr = &m_list[cave[y][x].m_idx];
					monster_race *r_ptr = &r_info[m_ptr->r_idx];
					char m_name[80];
					monster_desc(m_name, m_ptr, 0);

					if (r_ptr->flags2 & RF2_GIGANTIC)
					{
						msg_format("%s�͑傫�����Ĉ����񂹂��Ȃ������I", m_name);
						break;
					}
					else
					{
						msg_format("%s�Ɏ��𗍂߁A��������񂹂��I", m_name);
						teleport_monster_to(cave[y][x].m_idx, py, px, 100, TELEPORT_PASSIVE);
					}
				}

				else
				{
					msg_format("���Ȃ��͎�����J���Ĕ�񂾁I");
					teleport_player_to(y, x, TELEPORT_NONMAGICAL);
				}
			}
			else
			{
				teleport_player_to(y, x, TELEPORT_NONMAGICAL);
				msg_format("���Ȃ���%s����}�~�����Ėғł��T���U�炵���I",(dun_level?"�V��":"��"));
				project(0, 4, y, x, dam, GF_POIS, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID), -1);

			}

			break;
		}


	case 4: //�t�B���h�~�A�Y�}
		{
			int dam = plev * 3 + chr_adj * 3;
			if(only_info) return format("����:%d",dam);
			msg_print("���Ȃ���ᏋC���T���U�炵���B");
			project_hack2(GF_POLLUTE,0,0,dam);

			break;
		}

	case 5: //��������
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			damage = 10 + plev + chr_adj * 2;
			if(damage < 50) damage = 50;
			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(use_itemcard) msg_format("%s�ɉ��˂����Ă��I",m_name);
				else msg_format("���Ȃ���%s�Ɋ��ݕt�����I",m_name);

				if(r_ptr->flagsr & RFR_RES_ALL)
				{
					msg_format("%s�ɂ͑S�����ʂ��Ȃ��悤���B",m_name);
					break;
				}
				
				if(!(r_ptr->flagsr & RFR_IM_POIS))
				{
					damage *= 3;
					if(!((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
						&& (randint1(randint1(r_ptr->level/7)+3) == 1))
					{
						msg_format("%s�̑S�g�ɓł�������I",m_name);
						damage = m_ptr->hp + 1;
					}
					else if(set_monster_timed_status_add(MTIMED2_DEC_ATK, cave[y][x].m_idx, 16+randint1(16)))
						msg_format("%s�̍U���͂����������悤���B", m_name);


				}

				if(damage>0) 
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_MISSILE,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	

	case 6: //�K�i����
	{
		if (only_info) return format("");

		stair_creation();

		break;
	}




	case 8: //�ΌA�̒w偂̑�
		{
			if(only_info) return format("");
			msg_print("���͂�����Ԃɖ��ߐs�����ꂽ�I");
			project_hack2(GF_MAKE_SPIDER_NEST,0,0,0);

			//v1.1.75 �����Y��ǉ�
			p_ptr->redraw |= (PR_STATE);
			p_ptr->update |= (PU_BONUS);

			break;
		}


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::���p�Z*/
class_power_type class_power_utsuho[] =
{

	{1,0,0,FALSE,FALSE,A_WIS,0,0,"�j�Z��",
		"�̓��Ŗ��s���ɃG�l���M�[�𐶂ݏo��MP��啝�ɉ񕜂�����B�܂������̎��͂𖾂邭�Ƃ炷�B"},
	{1,8,20,FALSE,FALSE,A_DEX,0,0,"���[�U�[",
		"�M�������̃r�[������B"},
	{7,12,30,FALSE,FALSE,A_STR,0,0,"�v�`�t���A",
		"�Ή������̃{�[������B�˒��͂��Z���B"},
	{12,18,35,FALSE,FALSE,A_DEX,0,0,"�n�C�e���V�����u���[�h",
		"�v���Y�}�����̃r�[������B"},
	{16,21,50,TRUE,FALSE,A_STR,0,15,"���P�b�g�_�C�u",
		"�꒼���ɓːi���A�����ȓG�͐�����΂��B���x�����オ��Ǝ˒����L�т�B"},
	{20,24,50,FALSE,FALSE,A_STR,0,0,"���K�t���A",
		"�j�M�����̑傫�ȃ{�[������B"},
	{25,30,65,FALSE,FALSE,A_DEX,0,0,"�j�M�o�C�U�[",
		"�_�E�d���E�Ή��E��C�ւ̈ꎞ�I�ȑϐ��𓾂�B"},

	{27,38,60,FALSE,FALSE,A_DEX,0,0,"�n���g���C",
		"��Q���ɓ�������y�􂷂镪�𑮐��̋�����B"},
	{30,50,60,FALSE,FALSE,A_STR,0,0,"�j���[�N���A�t���[�W����",
		"���E���̂��ׂĂɑ΂��Ċj�M�����U������B"},
	{34,65,70,FALSE,FALSE,A_WIS,0,0,"�\����",
		"���͂̃����_���Ȉʒu�ɋ��͂ȉΉ������{�[����A���Ŕ���������B"},
	{36,80,55,FALSE,FALSE,A_CON,0,0,"�w���Y�g�J�}�N",
		"���͂�*�j��*����B���x�����オ��Ɣ͈͂��L����B"},
	{40,120,75,FALSE,FALSE,A_INT,0,0,"�t�B�N�X�g�X�^�[",
		"�M�������̋�����B����͒��e�n�_����7/8�̊m���Ń����_���ȃ^�[�Q�b�g�֍Ĕ�������B"},
	{44,240,85,FALSE,FALSE,A_STR,0,0,"�M�K�t���A",
		"�ɂ߂ċ��͂Ȋj�M�����̃��[�U�[����B"},

//	{48,450,80,FALSE,TRUE,A_STR,0,0,"�A�r�X�m���@",
//		"���͂̑S�Ăɑ΂��ċɂ߂ċ��͂Ȋj�M�����U������B�������g�p���甭���܂ł�15�s�����x�̃^�C�����O������A�����܂ő��̋Z���g���Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


/*:::���p���Z*/
cptr do_cmd_class_power_aux_utsuho(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: //�j�Z��
		{
			if(only_info) return format("");

			if(one_in_(10)) msg_print("���C�Ȑ��f����^�Z�������I");
			else if(one_in_(9)) msg_print("���ɂ̃G�l���M�[���g�̂ɖ�����I");
			else msg_print("�̓��Ŋj�����������������I");

			p_ptr->csp += p_ptr->msp / 8 + randint1(p_ptr->msp / 8);
			if(p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			lite_room(py, px);
			p_ptr->redraw |= (PR_MANA);
			p_ptr->window |= (PW_PLAYER);
			break;
		}
	case 1: //���[�U�[
		{
			int dam = 20 + plev + chr_adj;
			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�r�[�����������B");
			fire_beam(GF_LITE,dir, dam);
			break;
		}

	case 2: //�v�`�t���A
		{
			int range = 5 + plev / 10;
			int dice = 7 + plev / 3;
			int sides = 10 + chr_adj / 6;

			if(only_info) return format("�˒�:%d ����:%dd%d",range,dice,sides);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�΋��������o�����B");
			fire_ball(GF_FIRE,dir,damroll(dice,sides),1);

			break;
		}
	case 3: //�n�C�e���V�����u���[�h
		{
			int dam = 30 + plev * 2 + chr_adj * 3;
			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���G�l���M�[�דd���q�𔭐��������I");
			fire_beam(GF_PLASMA,dir, dam);
			break;
		}


	case 4: //���P�b�g�_�C�u
		{
			int len = 5 + p_ptr->lev / 10;
			damage = plev + adj_general[p_ptr->stat_ind[A_STR]] *5;
			if(only_info) return format("�˒�:%d ����:%d",len,damage);
			if (!rush_attack3(len,"���Ȃ��͖C�e�̂悤�ɔ�񂾁I",damage)) return NULL;
			break;
		}

	case 5: //���K�t���A
		{
			int dice = 15 + plev / 2;
			int sides = 10 + chr_adj / 4;

			if(only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("�J�[�h���狐��ȉ΋��������ꂽ�I");
			else
				msg_print("����ȉ΋���������I");
			fire_ball(GF_NUKE,dir,damroll(dice,sides),4);

			break;
		}
	case 6: //�j�M�o�C�U�[
		{
			int base = 5 + plev / 5 ;
			int v;
			if(only_info) return format("����:%d+1d%d�^�[��",base,base);
			msg_format("�G�l���M�[�̏�ǂ𒣂����I");
			v = base + randint1(base);
			set_oppose_acid(v, FALSE);
			set_oppose_elec(v, FALSE);
			set_oppose_fire(v, FALSE);
			set_oppose_cold(v, FALSE);

			break;
		}


	case 7: //�n���g���C
		{
			int base = plev*3 + chr_adj * 3;

			if(only_info) return format("����:%d",base);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���q�����̋���������I");
			fire_ball(GF_DISINTEGRATE,dir,base,3);

			break;
		}
	case 8: //�j���[�N���A�t���[�W����
		{
			int damage = 50 + plev*6 + chr_adj * 5;
			if(only_info) return format("����:%d",damage);
			msg_print("�M���ƌ��M�����͂����W�����I");
			project_hack2(GF_NUKE,0,0,damage);
			break;
		}
	case 9: //�\����
		{
			int rad = 2 + plev / 16;
			int damage = plev*3  + chr_adj*3 ;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("�V�������̋���ȉ΋����~�蒍�����B");
			cast_meteor(damage, rad, GF_FIRE);

			break;
		}
	case 10: //�w���Y�g�J�}�N
		{
			int sides = plev / 5;
			int base = 12;
			if(only_info) return format("�͈�:%d+1d%d",base,sides);
			destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			break;
		}

	case 11://�t�B�N�X�g�X�^�[
		//v1.1.44 ���[�`��������
		{

			int tx,ty;
			int dam = plev * 2 + chr_adj * 2; 
			int rad = 3;

			if(only_info) return format("����:%d * �s��",dam);

			if (!get_aim_dir(&dir)) return NULL;

			msg_format("ῂ��P��������������I");
			execute_restartable_project(0, dir, 8, 0, 0, dam, GF_LITE, rad);

		}
		break;
	case 12://�M�K�t���A
		{
			dice = p_ptr->lev;
			sides = 35 + chr_adj / 2;
			if(dice < 30) dice = 30;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�_�̉΂��O���̑S�Ă��Ă��������I");
			fire_spark(GF_NUKE,dir, damroll(dice,sides),2);

			break;
		}







	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���א�p�Z*/
//�����̃��x���⏇�Ԃ�G������calc_mana()�̓��ꏈ�������ύX�v
//p_ptr->magic_num1(2)[97�`105]���g���B�\���ȑO�̕����͖�����p�t�X�L���p�Ɋ���U��
class_power_type class_power_seija[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"������B��",
		"��E���@�_�E���b�h���ǂ����ɉB�����B�B���������������m�R�}���h����g�p�ł���B���b�h�݂͎̂��Ԍo�߂Ŏg�p�񐔂��񕜂���B"},
	{3,2,30,FALSE,TRUE,A_INT,0,5,"���͐H�킹",
		"�B����������E���@�_�E���b�h�̎g�p�񐔂��[�U����B��x�̏[�U�ʂ̓��x���E�m�\�E�A�C�e�����x���E�ő�ۗL�����e�����A�����x���Ȗ�����͈�x�̏[�U�ł͎g�p�񐔂������Ȃ����Ƃ�����B"},
	{5,0,0,FALSE,FALSE,A_DEX,0,0,"�Ђ��z",
		"�s�v�c����̈���g���B�G����U�����󂯂��Ƃ������I��6/7�̊m���Ń_���[�W��0�ɂ���B�X�e�[�^�X�ُ�Ȃǂ͎󂯂�B"},
	{7,12,30,FALSE,TRUE,A_INT,0,0,"�C�r���C���U�~���[",
		"���͂̓G�����m���ō���������B"},
	{9,0,0,FALSE,FALSE,A_DEX,0,0,"���Ԃ̐܂肽���ݎP",
		"�s�v�c����̈���g���B�ǂɌ����Ďg���ƕǂ̌������փe���|�[�g����B�}�b�v�[�֌������ꍇ�}�b�v�̔��Α��֏o��B�ʏ�̃e���|�[�g�ŐN���ł��Ȃ��n�`�ɂ͏o�Ă���Ȃ��B"},
	{13,0,0,FALSE,FALSE,A_DEX,0,0,"�􂢂̃f�R�C�l�`",
		"�s�v�c����̈���g���B������Ɉ�x�����G����̍U�����f�R�C�l�`���󂯂��̂Ƃ������̓e���|�[�g����B"},
	{16,0,0,FALSE,FALSE,A_DEX,0,0,"�l�ڃ}�W�b�N�{��",
		"�s�v�c����̈���g���B�߂��ɋ���ȕ��𑮐��̃{�[���𔭐�������B"},
	{20,16,40,FALSE,TRUE,A_DEX,0,0,"���o�[�X�q�G�����L�[",
		"�G��̂ƈʒu����������B���E�O�̓G�ɂ����ʂ�����B"},
	{24,0,0,FALSE,FALSE,A_DEX,0,0,"�V��̃g�C�J����",
		"�s�v�c����̈���g���B�G����{���g���@���󂯂��Ƃ������I��9/10�̊m���ŉ������B"},
	{28,0,0,FALSE,FALSE,A_DEX,0,0,"���ɋQ�����A�z��",
		"�s�v�c����̈���g���B���E���̃^�[�Q�b�g�̈ʒu�փe���|�[�g����B"},
	{32,0,0,FALSE,FALSE,A_DEX,0,0,"�ŏo�̏���(���v���J)",
		"�s�v�c����̈���g���B�אڂ����G��̂ɑ�_���[�W��^����B"},
	{36,0,0,FALSE,FALSE,A_DEX,0,0,"�g����n��",
		"�s�v�c����̈���g���B�G����̍U���ő̗͂�����������������Ɏ����I�ɔ������A�̗͂����S�ɉ񕜂�����B�̗͂�0�����ɂȂ����Ƃ��ɂ͔������Ȃ��B"},
	{40,0,0,FALSE,FALSE,A_DEX,0,0,"�S��̑����",
		"�s�v�c����̈���g���B��莞�ԗH�̉����A�ǂ𔲂��邱�Ƃ��\�ɂȂ�G����̍U���̂قƂ�ǂ̈З͂���������B�M����j�ׂ̍U�����󂯂�ƗH�̉������������B"},
	{45,72,70,FALSE,TRUE,A_CHR,0,0,"�V���]��",
		"���E���̑S�Ăɋ��͂ȏd�͑����U�����s���B���V���Ă��Ȃ�����ȓG�Ɍ��ʂ��傫���B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_seija(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: 
		{
			if(only_info) return "";
			gain_magic();
			break;
		}
	case 1: //������[�U
		{
			int power = (50+plev*2) * (10+adj_general[p_ptr->stat_ind[A_INT]]*3) * (100+randint1(200));

			if(only_info) return "";
			if(!recharge_magic_eater(power)) return NULL;

			break;
		}
	case 2: 
		{
			if(only_info) return format("�c���:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_HIRARI],p_ptr->magic_num2[SEIJA_ITEM_HIRARI]);
			msg_print("���̓���͎�����������");
			return NULL;
			break;
		}
	case 3: 
		{
			int power = 50 + plev * 4 + chr_adj * 5;
			if(only_info) return format("����:%d",power);
			msg_format("���Ȃ��͎��͂̓G�̎��E�����킹���I");
			confuse_monsters(power);
			break;
		}
	case 4: 
		{
			int x,y;
			int attempt = 500;
			if(only_info)
			{
				if(use_itemcard)
					return "";
				else
					return format("�c���:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_SUKIMA],p_ptr->magic_num2[SEIJA_ITEM_SUKIMA]);
			}

			if (p_ptr->anti_tele)
			{
				msg_format("���̓X�L�}�ɂ��ړ����ł��Ȃ��B");
				return NULL;
			}


			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if(cave_have_flag_bold(y,x,FF_MOVE) || cave_have_flag_bold(y,x,FF_CAN_FLY))
			{
				msg_format("�����ɂ͕ǂ��Ȃ��B");
				return NULL;
			}
			while(1)
			{
				attempt--;
				y += ddy[dir];
				x += ddx[dir];

				if(y >= cur_hgt) y = 1;
				if(y < 1) y = cur_hgt - 1;
				if(x >= cur_wid) x = 1;
				if(x < 1) x = cur_wid - 1;

				if(attempt<0)
				{
					msg_format("�X�L�}�ړ��Ɏ��s�����I");
					teleport_player(200,0L);
					break;
				}
				if(!cave_player_teleportable_bold(y, x, 0L)) continue;
				msg_format("���Ȃ��̓X�L�}���J���Ĕ��Α��ɔ������I");
				teleport_player_to(y,x,0L);
				break;
			}

			if(!use_itemcard) p_ptr->magic_num1[SEIJA_ITEM_SUKIMA] -= 1;

			break;
		}
	case 5: 
		{
			if(only_info) return format("�g�p��:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_DECOY],p_ptr->magic_num2[SEIJA_ITEM_DECOY]);
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("���Ȃ��͂�������Ǝd���݂��ς܂����E�E");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
				p_ptr->magic_num1[SEIJA_ITEM_DECOY] -= 1;
			}
			else
			{
				msg_print("���Ƀf�R�C�l�`�̏����͍ς�ł���B");
				return NULL;
			}
			break;
		}
	case 6:
		{
			int rad = 3 + plev / 15;
			base = 100 + plev * 3;
			if(only_info) return format("�g�p��:%d/%d ����:%d",p_ptr->magic_num1[SEIJA_ITEM_BOMB],p_ptr->magic_num2[SEIJA_ITEM_BOMB],base);

			project_length = 4;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���Ȃ��͔��e�ɓ_�΂��A���������I");
			fire_ball(GF_DISINTEGRATE,dir,base,rad);
			p_ptr->magic_num1[SEIJA_ITEM_BOMB] -= 1;

			break;
		}
	case 7:
		{
			bool result;
			if(only_info) return "";

			/* HACK -- No range limit */
			project_length = -1;
			result = get_aim_dir(&dir);
			project_length = 0;
			if (!result) return NULL;
			teleport_swap(dir);
			break;
		}
	case 8: 
		{
			if(only_info) return format("�c���:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_CAMERA],p_ptr->magic_num2[SEIJA_ITEM_CAMERA]);
			msg_print("���̓���͎�����������");
			return NULL;
			break;
		}
	case 9: 
		{
			int x, y;
			if(only_info) return format("�c���:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_ONMYOU],p_ptr->magic_num2[SEIJA_ITEM_ONMYOU]);
			//if (!tgt_pt(&x, &y)) return NULL;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			if (p_ptr->anti_tele)
			{
				msg_print("�s�v�c�ȗ͂��e���|�[�g��h�����I");
			}
			else
			{
				msg_print("���Ȃ��͗��ꂽ�ꏊ�Ɉ�u�Ō��ꂽ�B");
				teleport_player_to(target_row, target_col, 0L);
			}
			p_ptr->magic_num1[SEIJA_ITEM_ONMYOU] -= 1;
			break;
		}
	case 10://���ƃ��v���J
		{
			int y, x;
			monster_type *m_ptr;
			base = plev * 6;
			sides = plev * 6;

			if(base < 150) base = 150;
			if(sides < 150) sides = 150;

			if(only_info)
			{
				if(use_itemcard)
					return format("����:1d%d + %d",base,sides);
				else
					return format("�c���:%d/%d ����:1d%d + %d",p_ptr->magic_num1[SEIJA_ITEM_KODUCHI],(int)p_ptr->magic_num2[SEIJA_ITEM_KODUCHI],base,sides);

			}

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];
			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("���Ȃ��͏��Ƃ�U�肩�Ԃ�A%s�Ɍ����Ē@���t�����I",m_name);
				project(0, 0, y, x, randint1(sides) + base, GF_DISP_ALL, flg, -1);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			if(!use_itemcard) p_ptr->magic_num1[SEIJA_ITEM_KODUCHI] -= 1;

			break;
		}
	case 11: 
		{
			if(only_info) return format("�c���:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_JIZO],p_ptr->magic_num2[SEIJA_ITEM_JIZO]);
			msg_print("���̓���͎�����������");
			return NULL;
			break;
		}
	case 12: 
		{
			base = 15;
			if(only_info) return format("�c���:%d/%d ����:%d + 1d%d",p_ptr->magic_num1[SEIJA_ITEM_CHOCHIN],p_ptr->magic_num2[SEIJA_ITEM_CHOCHIN],base,base);
			set_wraith_form(randint1(base) + base, FALSE);
			p_ptr->magic_num1[SEIJA_ITEM_CHOCHIN] -= 1;
			break;
		}
	case 13: 
		{
			int dam = plev * 5 + chr_adj * 5;

			if(only_info) return format("����:%d",dam);
			msg_format("���Ȃ��͓V�n���Ђ�����Ԃ����I");
			project_hack2(GF_GRAVITY,0,0,dam);			
			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�ʂ���p�Z*/
class_power_type class_power_nue[] =
{
	{3,3,10,FALSE,TRUE,A_CON,0,0,"�A���f�B�t�@�C���h�_�[�N�l�X",
		"�����镔�����Â�����B"},
	{12,12,30,FALSE,TRUE,A_INT,0,3,"�O���s���̋S��",
		"���͂̃����_���Ȉʒu�ɉΉ������̃{�[����A���Ŕ���������B"},
	{20,30,50,FALSE,TRUE,A_CHR,0,0,"���̕s���̎�",
		"�G��̂ɐ��̕s���̎���d���ށB���̕s���ɂȂ����G�͑��̓G����U�������悤�ɂȂ�B�J�I�X�ϐ������G�ɂ͌��ʂ��Ȃ��A���j�[�N�����X�^�[�⍂���x���ȓG�ɂ͌����ɂ����B"},
	{28,36,60,FALSE,TRUE,A_INT,0,2,"�����s���̗d����",
		"�w�肵���ʒu�Ƀv���Y�}�����̋���ȃ{�[���𔭐�������B"},
	{34,45,65,FALSE,TRUE,A_CON,0,0,"�����̃_�[�N�N���E�h",
		"���E���̑S�Ăɑ΂��Í������U��������B"},
	{42,70,75,FALSE,TRUE,A_CHR,0,0,"����",
		"��莞�ԕ��g�����o���U���̔������������B"},
	{47,90,75,FALSE,TRUE,A_CHR,0,0,"�V�����̒e���w",
		"���E���̑S�Ăɑ΂��A���S�Ƀ����_���ȑ����̋��͂ȍU�����s���B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_nue(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: 
		{
			if(only_info) return "";
			msg_format("���Ȃ��̐g�̂��獕�������N���o�����B");
			unlite_room(py,px);
			break;
		}
	case 1:
		{
			int rad = 2 + plev / 24;
			int damage = 10 + plev  + chr_adj / 2 ;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("���Ȃ��̎�����΋�����щ�����B");
			cast_meteor(damage, rad, GF_FIRE);

			break;
		}

	case 2://���̕s���̎�
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					char m_name[80];
					int chance = 40 + plev * 2 + chr_adj * 2;
					monster_race *r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);

					if(use_itemcard && chance < 100) chance=100;

					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance /= 2;

					if(r_ptr->flagsr & (RFR_RES_CHAO | RFR_RES_ALL))
					{
						msg_format("%s�ɂ͌��ʂ��Ȃ��I",m_name);
					}
					else if(m_ptr->mflag & MFLAG_NUE_UNKNOWN)
					{
						msg_format("���Ɍ����Ă���B");

					}
					else if( randint1(chance) < r_ptr->level)
					{
						msg_format("%s�ɐ��̕s���̎���d���ނ̂Ɏ��s�����I",m_name);
					}
					else
					{

						msg_format("%s�ɐ��̕s���̎���d���񂾁I",m_name);
						m_ptr->mflag |= MFLAG_NUE_UNKNOWN;
						lite_spot(m_ptr->fy,m_ptr->fx);

					}
				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}

			break;
		}
	case 3://�����s���̗d����
		{
			int rad = 2 + plev / 15;
			dice = 8 + p_ptr->lev / 3;
			sides = 12 + chr_adj / 2 ;
			base = p_ptr->lev*4;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_PLASMA, dir, base + damroll(dice,sides), rad,"�����̌Q�ꂪ�P�����������I")) return NULL;
			break;
		}
	case 4:
		{
			int dam = plev * 3 + chr_adj * 5;

			if(only_info) return format("����:%d",dam);
			msg_format("�ӂ�͔Z���Ȉłɕ�܂ꂽ�E�E");
			project_hack2(GF_DARK,0,0,dam);			
			break;
		}
	case 5:
		{
			dice = base = 8;
			if(only_info) return format("����:%d+1d%d",base,dice);
			set_multishadow(base + randint1(dice),FALSE);
			break;
		}
	case 6:
		{
			int base = plev * 5 + chr_adj * 5;

			if(base < 300) base=300;
			if(only_info) return format("����:%d+1d%d",base,base);
			msg_format("���Ȃ��͈يE�̐��̕s���̃p���[���Ăяo�����I");
			project_hack2(0,1,base,base);			
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���@��p�Z*/
class_power_type class_power_byakuren[] =
{

	{3,6,50,FALSE,TRUE,A_INT,0,20,"�]��",
		"���̋������@�̋L���ꂽ�����Ǐグ�����𔭓�������B�g�̔\�͂Ɗi���Z�\���㏸���A���x���ɂ���ėl�X�Ȕ\�͂�ϐ��𓾂�B"},
	{10,10,35,FALSE,TRUE,A_WIS,0,3,"�C���h���̗�",
		"�^�[�Q�b�g�̈ʒu�ɗ��𗎂Ƃ��ēd���U��������B"},
	{15,0,0,FALSE,TRUE,A_WIS,0,0,"�ґz",
		"���_���������܂��AMP���͂��ɉ񕜂���B"},
	{18,20,50,FALSE,TRUE,A_INT,0,0,"�n���̐���",
		"���͂̓G��N�O�A������ɂ��悤�Ǝ��݂�B"},
	{23,30,60,TRUE,FALSE,A_DEX,0,0,"�X�J���_�̋r",
		"���ꂽ�G�Ɉ�u�ŋߊ���čU������B�]�ǂɂ�鋭�����ɂ����g���Ȃ��B"},
	{27,18,55,FALSE,TRUE,A_INT,0,3,"�X�^�[���C���V���g����",
		"�߂��̓G�ɑM�������U�����s���B���x�����オ��ƈЗ͂��オ����ʔ͈͂��L����B"},
	{30,30,45,FALSE,TRUE,A_INT,0,0,"�X�^�[�\�[�h�̌�@",
		"�Z���Ԃ̊ԁA���ڍU�������Ă����G�ɔj�Б����_���[�W��^����悤�ɂȂ�B"},
	{36,48,70,FALSE,TRUE,A_INT,0,0,"�u���t�}�[�̓�",
		"�L�͈͂̒n�`�A�A�C�e���A�����X�^�[�A�g���b�v�����m����B"},
	{40,90,80,FALSE,TRUE,A_INT,0,0,"�A�[���M���T���F�[�_",
		"�M�������̋��͂ȃr�[������B"},
	{45,120,80,FALSE,TRUE,A_INT,0,0,"���l�����@",
		"�]�ǂɂ�鋭�����ɂ����g���Ȃ��B�Z���ԑ啝�ɉ������U���񐔂��㏸����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},

};


cptr do_cmd_class_power_aux_byakuren(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int wis_adj = adj_general[p_ptr->stat_ind[A_WIS]];

	switch(num)
	{

	case 0:
		{
			int time = 10 + plev / 5;
			int dec_hp = p_ptr->mhp - p_ptr->chp;
			if(only_info) return format("����:%d",time);

			if(one_in_(3))msg_print("���镶�����󒆂������ŗ��ꂽ�B");
			else if(one_in_(2))msg_print("�G�A�������N���������B");
			else msg_print("�u�����A�얳�O�[�I�v");
			if(plev > 29) set_fast(time,FALSE);
			if(plev > 34)
			{
				set_oppose_acid(time, FALSE);
				set_oppose_elec(time, FALSE);
				set_oppose_fire(time, FALSE);
				set_oppose_cold(time, FALSE);
				set_oppose_pois(time, FALSE);
			}
			if(!use_itemcard) set_tim_general(time,FALSE,0,0); 
			set_tim_addstat(A_STR,105+plev/10,time,FALSE);
			set_tim_addstat(A_DEX,105+plev/10,time,FALSE);
			set_tim_addstat(A_CON,105+plev/10,time,FALSE);
			p_ptr->chp = p_ptr->mhp - dec_hp;
			p_ptr->redraw |= PR_HP;
			handle_stuff();		
			break;
		}

	case 1:
		{
			int rad = 2;
			if(plev > 39) rad = 3;
			dice = 2 + plev / 7;
			sides = 10 + chr_adj / 6;
			base = 20 + plev + chr_adj ;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_ELEC, dir, base + damroll(dice,sides), rad,"�����������I")) return NULL;
			break;

		}
	case 2:
		{
			if(only_info) return format("");	
			msg_print("���_���������܂����E�E");
			p_ptr->csp += (3 + plev/15);
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
		}
		break;
	case 3:
		{
			int power = plev * 2;
			if(power < 30) power=30;
			if(only_info) return format("����:%d",power);
			msg_print("�����~�蒍���A���͂̎҂̓�����D���~�߂��B");
			stun_monsters(power);
			stasis_monsters(power);
			break;

		}
	case 4:
		{
			int len = 5 + p_ptr->lev / 7;
			if(only_info) return format("�˒�:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 5:
		{
			int base = 20 + plev + chr_adj;
			int num = 3 + plev / 15 + chr_adj / 20;
			int rad = 2;
			int i;
			if(only_info) return format("����:�ő�%d * %d",base/2,num);
			msg_print("���Ȃ��̎���Ɍ������Q���������I");
			for (i = 0; i < num; i++)
			{
				project(0, rad, py, px, base, GF_LITE, PROJECT_KILL, -1);
				rad = MIN(7,rad+1);
			}
			break;
		}
	case 6:
		{
			int time = 20;
			if(only_info) return format("����:%d",time);
			msg_format("���Ȃ��̎���ɖ����̌아�������񂾁B");	
			set_dustrobe(time,FALSE);
		}
		break;
	case 7:
		{
			int range = plev;
			if(only_info) return format("�͈�:%d",range);
			map_area(plev);
			detect_all(plev);

		}
		break;
	case 8:
		{
			int damage = p_ptr->lev * 6 + chr_adj * 5 + wis_adj * 5;
			if(damage < 450) damage = 450;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�����̌����O�����Ă��������I");
			fire_spark(GF_LITE,dir,damage,2);

			break;
		}
	case 9:
		{
			int time = 5;
			if(only_info) return format("����:%d",time);
			msg_print("�����̖��@���Ɍ��܂ō��߂��I");
			if(p_ptr->tim_general[0] < time) p_ptr->tim_general[0] = time;
			set_lightspeed(time, FALSE);
			redraw_stuff();
		}
		break;


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�ɂƂ��p�Z*/
class_power_type class_power_nitori[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�@�B�i�[",
		"�����b�N�ɋ@�B���i�[����B���̐��̓��x���A�b�v�ŏ㏸����B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�@�B�m�F",
		"�����b�N�̋@�B���m�F���������������B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"��������",
		"�����b�N����@�B���o���B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�@�B�g�p",
		"�����b�N�Ɋi�[�����@�B���g�p����B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�e���[",
		"�����b�N�Ɋi�[�����@�B�ɒe��Ȃǂ��[���g�p�񐔂��񕜂�����B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"���d�G�l���M�[��[",
		"�ꕔ���Z�ŏ����G�l���M�[���[����B����ɂ��Ȃ��Ǝg���Ȃ��B"},
	{5,0,0,FALSE,FALSE,A_DEX,0,0,"��^���S�C",
		"�^�[�Q�b�g���ӂɐ������̃{���g�𗐎˂���B���x�����オ��Ɛ���������B�ˌ������̐��d�G�l���M�[�������B"},
	{12,16,30,FALSE,TRUE,A_WIS,0,3,"�͓��̃|�����b�J",
		"�����𒆐S�Ƃ�������Ȑ������̃{�[���𔭐�������B����ɂ��Ȃ��Ǝg���Ȃ��B"},
	{16,0,0,FALSE,FALSE,A_DEX,0,0,"�N���~�i���M�A",
		"�Z���ԁA���ڍU�����Ă����G�ɔj�Б����_���[�W��^����悤�ɂȂ�B���d�G�l���M�[50�������B"},
	{21,0,0,FALSE,FALSE,A_DEX,0,0,"�L���[���T�E���h�V�X�e��",
		"���������̃r�[���𔭎˂���B���d�G�l���M�[30�������B"},
	{26,0,0,FALSE,FALSE,A_DEX,0,0,"�e�ꕶ���R���v���b�T�[",
		"����Ȍ`��ɔ������鐅�����U������B�З͍͂������˒��͒Z���B���d�G�l���M�[200�������B"},

	{30,0,0,FALSE,FALSE,A_DEX,0,0,"�E�H�[�^�[�Ή����ˊ�",
		"���C�����̃u���X����B�З͂�HP��1/3�B���d�G�l���M�[300�������B" },

	{34,40,60,FALSE,TRUE,A_WIS,0,5,"�V���C�g�t�H�[��",
		"�����𒆐S�Ƃ��������_���Ȉʒu�ɐ������̋���A���Ŕ���������B"},
	{38,0,0,FALSE,TRUE,A_INT,0,0,"�I�v�e�B�J���J���t���[�W��",
		"��莞�ԁA�����ɂȂ��ēG����F������ɂ����Ȃ�B���������o�ɗ���Ȃ��G�ɂ͌��ʂ������B���d�G�l���M�[300�������B"},
	{44,0,0,FALSE,TRUE,A_INT,0,0,"�P�S���K��",
		"�G���Q���ɓ�����Ɣ������鐅�����̒e��A�˂���B���d�G�l���M�[400�������B"},
	{48,96,80,FALSE,TRUE,A_WIS,0,0,"����P������̃g���E�}",
		"���E���S�Ăɋ��͂Ȑ������U�����s���A�n�`�𐅂ɂ���B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};
//�ɂƂ���Z�̕K�v���d�G�l���M�[�@�Y����class_power_nitori�ƈ�v���A�G�l���M�[(p_ptr->magic_num1[20])�����̒l�����̋Z�͎g���Ȃ�
int nitori_energy[] =
{
	0,0,0,0,0,0,
	1,0,50,30,200,300,
	0,300,400,0

};

cptr do_cmd_class_power_aux_nitori(int skillnum, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(skillnum)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			(void) use_machine(0);
			return NULL; //���邾���Ȃ̂ōs��������Ȃ�

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if( !use_machine(2)) return NULL;

			break;
		}
	case 4: //�e���[
		{
			if(only_info) return format("");
			if( !use_machine(1)) return NULL;
			break;
		}
	case 5:
		{
			if(only_info) return format("���݃G�l���M�[:%d",p_ptr->magic_num1[20]);
			if(p_ptr->magic_num1[20] == NITORI_ENERGY_MAX)
			{
				msg_print("���ɏ\���ɐ������Ă���B");
				return NULL;
			}
			msg_print("���Ȃ��̓����b�N�ɐ������߂邾�����񂾁I");
			p_ptr->magic_num1[20] = NITORI_ENERGY_MAX;
			break;
		}
	case 6:
		{
			int num = 2 + plev / 5;
			int dice = 1;
			int sides = 3 + plev / 8 + chr_adj / 10;
			if(!use_itemcard && p_ptr->magic_num1[20] < num) num = p_ptr->magic_num1[20];

			if(only_info) return format("����:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���S�C�𗐎˂����I");
			fire_blast(GF_WATER,dir,dice,sides,num,3,( PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE | PROJECT_GRID));
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;
			if(!use_itemcard) p_ptr->magic_num1[20] -= num;

			break;
		}
	case 7:
		{
			int base = 50 + plev * 4 + chr_adj * 3;
			int rad = 4 + plev / 12;

			if(only_info) return format("����:�ő�%d",base/2);
			msg_print("�ˑR��������オ�蕬���o�����I");
			
			project(0, rad, py, px, base, GF_WATER, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
		}
		break;
	case 8:
		{
			int time = 20;
			if(only_info) return format("����:%d",time);
			msg_format("�����b�N����M�A����яo����]���n�߂��B");	
			set_dustrobe(time,FALSE);
			p_ptr->magic_num1[20] -= nitori_energy[skillnum];
		}
		break;
	case 9:
		{
			int dice = 1;
			int sides = plev*2;

			if(only_info) return format("����:1d%d",sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�����b�N�̉�����X�s�[�J�[����яo���A���g��������I");
			fire_beam(GF_SOUND,dir,randint1(sides));
			p_ptr->magic_num1[20] -= nitori_energy[skillnum];
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;
		}
		break;
	case 10:
		{
			int range = 5;
			int dam = 50 + plev * 3;
			int tx, ty;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_COMPRESS;

			if(dam < 120) dam = 120;
			if(only_info) return format("�˒�:%d ����:%d",range,dam);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�O�����k�@���獂���̐������˂��ꂽ�I");
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				flg &= ~(PROJECT_STOP);
				tx = target_col;
				ty = target_row;
			}
			project(0, 5, ty, tx, dam, GF_WATER, flg, -1);
			if(!use_itemcard) p_ptr->magic_num1[20] -= nitori_energy[skillnum];
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;

			break;
		}

	//v2.0.2
	case 11:
	{
		int dam = p_ptr->chp / 3;
		if (!dam) dam = 1;

		if (only_info) return format("����:%d", dam);
		if (!get_aim_dir(&dir)) return NULL;

		msg_print("���Ȃ��͍����̏��C�𐁂��t�����I");
		fire_ball(GF_STEAM, dir, dam, -1);

		p_ptr->magic_num1[20] -= nitori_energy[skillnum];
		if (p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;
	}
	break;

	case 12:
		{
			int dam = 20 + plev + chr_adj;
			if(only_info) return format("����:%d*�s��",dam);
			msg_print("���ʂ̐��򂪑�̂悤�ɍ~�蒍�����I");
			cast_meteor(dam, 3,GF_WATER);
		}
		break;
	case 13:
		{
			int base = 20;
			if(only_info) return format("����:%d+1d%d",base,base);
			msg_format("���w���ʂ��N�������I");
			set_tim_superstealth(base + randint1(base),FALSE, SUPERSTEALTH_TYPE_OPTICAL);
			if(!use_itemcard) p_ptr->magic_num1[20] -= nitori_energy[skillnum];
			break;
		}
	case 14:
		{
			int num = 8;
			int dice = 3;
			int sides = plev/2;
			if(only_info) return format("����:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���������������I");
			for (i = 0; i < num; i++) fire_rocket(GF_WATER, dir, damroll(dice, sides),3);
			p_ptr->magic_num1[20] -= nitori_energy[skillnum];
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;
			break;

		}


	case 15:
		{
			int damage = plev * 5 + chr_adj * 5;
			if(only_info) return format("�����F%d",damage);
			msg_format("�ӂ�͈�u�ɂ��Đ[�����̒�ɂȂ����B");
			project_hack2(GF_WATER_FLOW,0,0,2);
			project_hack2(GF_WATER,0,0,damage);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();

		}
		break;
	}

	return "";
}


/*:::���~���A�p���Z*/
/*:::�~�[���u���t�F�C�g��magic_num1[0]��tim_general[0]���g�p����*/
//v1.1.95 ���g��Ȃ��Ȃ���
class_power_type class_power_remy[] =
{
	{10,10,30,FALSE,TRUE,A_INT,0,6,"�X�^�[�I�u�_�r�f",
		"�����̎��ӂ̃����_���ȏꏊ�ɕ�����̖������U�����s���B"},
	{15,15,30,FALSE,FALSE,A_DEX,30,0,"�}�C�n�[�g�u���C�N",
		"����Ȃǂ����͂ɓ�������B"},
	{20,16,40,FALSE,FALSE,A_STR,0,7,"�X�J�[���b�g�V���[�g",
		"�G���Q���ɓ�������y�􂷂鋅�𓊂�����B����ȑ̂����G�ɑ傫�ȃ_���[�W��^����B"},
	{23,36,50,FALSE,TRUE,A_CON,0,0,"���@���s���b�V���i�C�g",
		"��ʂ̋z���啂��o��������B�z���啂̍U���ɂ��HP���񕜂��邱�Ƃ��ł���B�啂̓t���A�ړ��ŏ��ł��ߊl�Ȃǂ��s�\�B"},
	{27,20,45,TRUE,FALSE,A_DEX,0,10,"�o�b�h���f�B�X�N�����u��",
		"�w�肵���G�֓ːi���_���[�W��^���N�O�Ƃ�����B��Q���ɓ�����Ǝ~�܂�B"},
	{30,40,65,FALSE,TRUE,A_STR,0,0,"�~�[���u���t�F�C�g",
		"�G��̂������̋߂��Ɉ����񂹁A����ɒZ���Ԉړ��ƃe���|�[�g��j�Q����B��������E�o����邱�Ƃ�����A����ȓG��͋����G�ɂ͌����ɂ����B"},
	{33,50,75,FALSE,TRUE,A_CHR,0,0,"�~���j�A���̋z���S",
		"�אڂ����G����啝��HP���z�����A�����x���񕜂�����B�l�Ԍ^�̓G�ɂ����g���Ȃ��B"},
	{37,80,80,FALSE,TRUE,A_STR,0,0,"�X�s�A�E�U�E�O���O�j��",
		"�o���A���ђʂ��鋭�͂ȃr�[���U������B�w�O���O�j���x�𑕔����Ă���ƈЗ͂��オ��B"},
	{40,60,70,FALSE,TRUE,A_DEX,0,0,"���イ���S������",
		"�߂��̃����_���ȓG�̏��Ɉړ������͑����̋��𔭐�������B6/7�̊m���ōĔ������A���̂��тɏ����Â��������͂ɂȂ�B"},
	{44,75,75,FALSE,TRUE,A_CHR,0,0,"�w���J�^�X�g���t�B",
		"�ׂɓG������Ƃ������𒆐S�ɂ����j�Б����{�[���A���͂ɓG�������Ƃ����E���򉻑����U���A�ǂ���ł��Ȃ��Ƃ������_���ȃ^�[�Q�b�g�ɑ΂��鐸�_�U�������{�[���A�˂��s���B" },
	{48,180,85,FALSE,TRUE,A_CHR,0,0,"���b�h�}�W�b�N",
		"���͂Ȏ��E���U�����s���B�З͖͂��͂ɑ傫���ˑ����A����ȑ̂����G�ɑ傫�ȃ_���[�W��^����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_remy(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: //�X�^�[�I�u�_�r�f
		{
			int dam = plev*2 + chr_adj + 20;
			if(only_info) return format("����:%d*�s��",dam);
			cast_meteor(dam, 2,GF_DISP_ALL);
		}
		break;

	case 1: //�}�C�n�[�g�u���C�N
		{
			int mult = 2 + plev / 15;
			if(only_info) return format("�{��:%d",mult);
			if (!do_cmd_throw_aux(mult, FALSE, -1)) return NULL;
		}
		break;

	case 2://�X�J�[���b�g�V���[�g
		{
			int dam = 50 + plev * 2 + chr_adj * 10 / 3;
			int rad = 1 + plev / 40;

			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("�J�[�h�����P�b�g�̂悤�ɔ��ł������I");
			else
				msg_print("�^�g�̖��͒e��������I");

			fire_rocket(GF_REDMAGIC, dir, dam, rad);
		}
		break;

	case 3: //���@���s���b�V���i�C�g
		{
			int num = plev / 3;
			int i;
			bool flag = FALSE;

			if(only_info) return format("");
			for(i=0;i<num;i++)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_REMY_BAT, (PM_FORCE_PET | PM_EPHEMERA))) flag = TRUE;
			}
			if(flag) msg_format("�̂̈ꕔ���ʂ��啂ɕς����I");
			else msg_format("�啂��o���̂Ɏ��s�����B");

		}
		break;
	case 4: //�o�b�h���f�B�X�N�����u��
		{
			int len = 5 + plev / 10;
			int damage = 50 + plev * 2;
			if(only_info) return format("�˒�:%d ����:%d",len,damage);
			if (!rush_attack2(len,GF_FORCE,damage,0)) return NULL;
			break;			
		}
		break;

	case 5: //�~�[���u���t�F�C�g
		{
			int idx;
			monster_type *m_ptr;
			char m_name[80];
			int base = 10;

			if (only_info) return format("����:%d", base);

			if(!teleport_back(&idx, TELEPORT_FORCE_NEXT)) return NULL;
			m_ptr = &m_list[idx];
			monster_desc(m_name, m_ptr, 0);
			if(distance(py,px,m_ptr->fy,m_ptr->fx) > 1) msg_format("%s�������񂹂�̂Ɏ��s�����B",m_name);
			else 
			{
				msg_format("�g��������сA%s�𑩔������I",m_name);
				project(0, 0, m_ptr->fy, m_ptr->fx, base, GF_NO_MOVE, PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE, -1);
//				p_ptr->magic_num1[0] = idx;
//				set_tim_general(base,FALSE,0,0);
			}
		}
		break;
	case 6: //�~���j�A���̋z���S
		{
			int y, x;
			int dam = plev * 4 + chr_adj * 5;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (only_info) return format("����:%d",dam);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int food = 0;
				char m_name[80];

				r_ptr = &r_info[m_ptr->r_idx];

				monster_desc(m_name, m_ptr, 0);
				if(!monster_living(r_ptr) || r_ptr->flags2 & (RF2_COLD_BLOOD) || !(r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN)) )
				{
					msg_format("����͔��������Ȃ��������B");
					return NULL;
				}
				else
				{
					msg_format("���Ȃ���%s�֕������A��؂֊��ݕt�����I",m_name);

					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2)
					{
						if(r_ptr->flags3 & RF3_HUMAN)
							msg_format("���̌��͍ō��ɔ������I");
						else
							msg_format("�ӊO�Ɉ����Ȃ������B",m_name);
					}
					else
					{
						cptr msg[12] = {
							"�D����B�^���I",
							"�r�^�[�X�C�[�g�ȑ�l�̖��킢���B",
							"�����������B�ӊO�ɋ֗~�I�Ȑ����̂悤���B",
							"���n���C�����B",
							"�܂�����Ƃ��Ă���ł��Ă������Ȃ��B",
							"�l���̖��������������镡�G�Ȗ��킢���B",
							"�����C���X�����Z�x�����߂��B",
							"��⍂�����C�����B",
							"���L���B��t������΂���炵���B",
							"�������������������Ȃ��B",
							"�A�z���u�₩���B�R�N������̂ɃL��������B",
							"�E�E�s�����B���̎҂ɂ͐H�����̉��P���K�v���B"
						};
						msg_format("%s",msg[cave[y][x].m_idx % 12]);
					}

					if(m_ptr->hp < dam) dam = m_ptr->hp+1;

					if(p_ptr->food < PY_FOOD_MAX) set_food(MIN(PY_FOOD_MAX - 1, (p_ptr->food + dam * 10)));
					hp_player(dam);
					project(0,0,y,x,dam,GF_DISP_ALL,PROJECT_KILL,0);
				}
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
		}
		break;

	case 7: //�X�s�A�U�O���O�j��
		{
			int base = plev*3 + chr_adj * 3;
			if(base < 150) base = 150;
			if(inventory[INVEN_RARM].name1 == ART_GUNGNIR || inventory[INVEN_LARM].name1 == ART_GUNGNIR) base *= 2;
			if(only_info) return format("����:%d+1d%d",base,base);
			
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_format("�J�[�h�����̑��ɂȂ��Ĕ��ōs�����I");
			else
				msg_format("���̑��𓊂��������I");
			fire_beam(GF_GUNGNIR,dir, base+randint1(base));

			break;
		}

	case 8: //���イ���S������ �X�^�[�t�@���O���炳��ɉ���
		{
			int basedam = (plev + chr_adj) / 2;
			int baserad = 1;
			int range = 12;
			bool flag_msg = FALSE;
			if(only_info) return format("����:%d*�s��*�s��",basedam);

			msg_print("���Ȃ����啂ɕϐg���Ĕ�񂾁E�E");
			do
			{
				int i, path_n;
				int tx,ty;
				int target_m_idx = 0;
				int target_num = 0;
				u16b path_g[32];
				bool moved = FALSE;
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					m_ptr = &m_list[i];
					if(!m_ptr->r_idx) continue;
					if(m_ptr->cdis > range) continue;
					if(!m_ptr->ml || !projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
					if (!is_hostile(m_ptr)) continue;
					target_num++;
					if(!one_in_(target_num)) continue;
 					target_m_idx = i;
				}
				if(!target_m_idx) break;
				tx = m_list[target_m_idx].fx;
				ty = m_list[target_m_idx].fy;
				project_length = 14;
				path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_STOP | PROJECT_KILL);
				project_length = 0;
				if (!path_n) break;
				ty = py;
				tx = px;

				/* Project along the path */
				for (i = 0; i < path_n; i++)
				{
					monster_type *m_ptr;
					int ny = GRID_Y(path_g[i]);
					int nx = GRID_X(path_g[i]);

					if (!cave[ny][nx].m_idx && player_can_enter(cave[ny][nx].feat, 0))
					{
						ty = ny;
						tx = nx;
						continue;
					}
					if (!cave[ny][nx].m_idx)
					{
						msg_print("�ǂɌ��˂����B");
						break;
					}

					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
					update_mon(cave[ny][nx].m_idx, TRUE);

					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
					moved = TRUE;
					project(0, baserad, ny, nx, basedam, GF_MANA, PROJECT_KILL | PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM, -1);
					flag_msg = TRUE;
					break;
				}
				if (!moved && !player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
				basedam = basedam * 11 / 10;
				if(one_in_(2) && baserad < 6) baserad++;

				if(p_ptr->confused || p_ptr->chp < 0 || p_ptr->paralyzed ) break;
			}while(!one_in_(7));

			if(!flag_msg) msg_print("�E�E�������߂��ɒN����������Ȃ������B");

		}
		break;


	case 9:
	{

		int i,dam,base;
		int target_num = 0;
		int near_target_num = 0;
		int attack_type;
		int attack_count;

		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr;
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (!m_ptr->ml) continue;
			if (m_ptr->cdis > MAX_SIGHT) continue;
			if (!is_hostile(m_ptr)) continue;
			if (!projectable(m_ptr->fy, m_ptr->fx, py, px)) continue;
			target_num++;
			if (m_ptr->cdis == 1) near_target_num++;
		}

		if (near_target_num) attack_type = 1;
		else if (target_num >= 7) attack_type = 2;
		else attack_type = 3;

		switch (attack_type)
		{
		case 1://�j�Б����{�[��

			dam = plev * 9 + chr_adj * 15;

			if (only_info) return format("����:�`%d", dam / 2);

			msg_print("���͂̐j�����̂悤�Ɍ����o���ꂽ�I");
			project(0, 5, py, px, dam, GF_SHARDS, (PROJECT_JUMP | PROJECT_KILL | PROJECT_ITEM), -1);
			break;

		case 2://���E����

			base = plev + chr_adj * 5;

			if (only_info) return format("����:%d+1d%d", base, base);

			msg_print("���͂̉傪��������H���r�炵���I");
			project_hack2(GF_DISENCHANT, 1, base, base);
			break;

		default:

			dam = plev * 3 + chr_adj * 5;
			if (only_info) return format("����:%d * 3", dam);

			msg_print("���͂̉��@�������I");

			for (i = 0; i<3; i++)
			{
				if (!fire_random_target(GF_REDEYE, dam, 3, 4, 0))
				{
					if (i == 0)msg_format("�������G����������Ȃ������B");
					break;
				}
			}
			break;

		}

	}
	break;





	case 10: //���b�h�}�W�b�N
		{
			int dice = plev;
			int sides =  chr_adj;
			if(sides < 10) sides = 10;
			if(only_info) return format("����:%dd%d",dice,sides);
			if(use_itemcard)
				msg_format("�J�[�h����Z���Ȑ^�g�̖��������o����...");
			else
				msg_format("���Ȃ��͋��̑O�ŉ������ݍ��ނ悤�Ȏd���������E�E");
			project_hack2(GF_REDMAGIC,dice,sides,0);
			break;
		}
		break;



	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}













/*:::���q���Z*/
class_power_type class_power_minoriko[] =
{
	{	10,10,35,FALSE,TRUE,A_CHR,0,2,"�I�[�^���X�J�C",
		"�����𒆐S�Ƃ����傫�ȃ{�[���𔭐�������B�З͖͂��͂ɑ傫���ˑ����A�����͐��A���A��C�A�d���̂����ꂩ�����_���ɂȂ�B"	},
	{	24,24,50,TRUE,FALSE,A_DEX,0,10,"�I���g�V�n�[�x�X�^�[",
		"�אڂ��������X�^�[�S�ĂɍU������B�劙�𑕔����Ă��Ȃ��Ǝg�����A�З͂͑劙�ɂ��ʏ�U���͂ɏ�����B"},
	{	30,30,70,TRUE,FALSE,A_WIS,0,0,"�X�C�[�g�|�e�g���[��",
		"�u�Ă����v�𐶐�����B�H�ׂ�Ɩ����x��MP���񕜂���B"	},
	//v2.0.2
	{ 35,45,70,FALSE,TRUE,A_CON,0,0,"�܍��L���g",
		"�X�ѐ��������̃r�[������B" },

	{	40,48,80,FALSE,TRUE,A_CHR,0,0,"�E�H�[���J���[�n�[���F�X�g",
		"���͂ɋC�����̃{�[���𑽐�����������B�З͖͂��͂ɑ傫���ˑ�����B"},
	{	99,0,0,FALSE,FALSE,0,0,0,"dummy",	""	},
};

cptr do_cmd_class_power_aux_minoriko(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	switch(num)
	{
	case 0: 
		{
			int rad = 2 + plev / 20 + chr_adj / 13;
			base = plev * 2 + chr_adj * 10;
			if(only_info) return format("����:�`%d",base/2);
			if(one_in_(4))
			{
				msg_format("�ǒn�I���J�����������I");
				project(0, rad, py, px, base, GF_WATER, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			}
			else if(one_in_(3))
			{
				msg_format("����Ȑ��������������I");
				project(0, rad, py, px, base, GF_TORNADO, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			}
			else if(one_in_(2))
			{
				msg_format("�₽�����������r�ꂽ�I");
				project(0, rad, py, px, base, GF_COLD, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			}
			else
			{
				msg_format("�����������I");
				project(0, rad, py, px, base, GF_ELEC, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			}
		}
		break;
	case 1:
		{
			int damage=0;
			//���������Ă���肵���_���[�W�ɎZ�����Ȃ��B
			if(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING))
				damage += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) 
				damage += calc_weapon_dam(1);
			if(only_info) return  format("����:%d",damage);
			if(damage==0)
			{
				msg_format("ERROR:damage0");
				return NULL;
			}
			msg_format("���Ȃ��͊���傫���U�����I");
			project(0, 1, py, px, damage * 2, GF_MISSILE, PROJECT_KILL, -1);

			break;
		}
	case 2:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "";
			msg_print("�Â������Ƌ��ɏĂ������o�������I");
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEET_POTATO));
			drop_near(q_ptr, -1, py, px);
			break;
		}


	case 3:
	{
		if (only_info) return "";
		if (!get_aim_dir(&dir)) return NULL;
		msg_print("���Ȃ��͖L���̃p���[��������I");
		fire_beam(GF_MAKE_TREE, dir, 10);
		break;
	}


	case 4:
		{
			int rad = 2 + chr_adj / 15;
			int damage = 10 + chr_adj * 3;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("�j�Ղ��I�r�X�����x�苶�����I");
			cast_meteor(damage, rad, GF_FORCE);

			break;
		}



	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;

	}
	return "";
}


/*:::���c��p�Z*/
class_power_type class_power_sanae[] =
{
	{ 1,0,0,FALSE,FALSE,A_INT,0,0,"�r��(��p)",
		"��q���`�̔�p���s�g���邽�߂̉r�����s���B"},
	{ 1,1,0,FALSE,FALSE,A_WIS,0,0,"�r��(�_�ގq)",
		"�_�ގq�l�̗͂��؂�邽�߂̉r�����s���B"},
	{ 1,2,0,FALSE,FALSE,A_WIS,0,0,"�r��(�z�K�q)",
		"�z�K�q�l�̗͂��؂�邽�߂̉r�����s���B"},

	{ 2,2,20,FALSE,TRUE,A_INT,0,0,"�E�B���h�u���X�g",
		"�������̃{���g�œG���U������BAC�̍����G�ɂ͓�����ɂ����B�r���͕s�v�����r��(��p)�������ƈЗ͂����オ��B"},
	{ 5,4,20,FALSE,TRUE,A_WIS,0,1,"�X�J�C�T�[�y���g",
		"�A���Ŗ������̃{���g����B�r��(�_�ގq)��3�i�K�ȏ�K�v�B�r���������قǐ���������B"},
	{ 7,5,20,FALSE,TRUE,A_WIS,0,1,"�R�o���g�X�v���b�h",
		"��Q���ɓ�����Ɣ������鉘�������̊^�̒e����B�r��(�z�K�q)��3�i�K�ȏ�K�v�B�r���������قǈЗ͂Ɣ������a���オ��B"},
	{ 10,3,45,FALSE,FALSE,A_INT,0,0,"��I�m�o",
		"���͂̃����X�^�[�����m����B���x�����オ��Ɣ͈͂��L����B���̋Z�ɂ���ĉr�����r�؂�邱�Ƃ͂Ȃ��B(���s����Ɠr�؂��)"},
	{ 12,8,50,FALSE,TRUE,A_INT,0,3,"�O���C�\�[�}�^�[�W",
		"�����̎���̃����_���Ȉʒu�Ƀ����_���Ȍ��f�����̃{�[����A���Ŕ���������B�r��(��p)��5�i�K�ȏ�K�v�B�r���������قǈЗ͂Ɣ������a���オ��B"},
	{ 16,10,40,FALSE,TRUE,A_WIS,0,0,"�t�@�t���b�L�[�Y�̊��",
		"���E���̓G�̓���ɗl�X�Ȃ��̂��~�点��B�r��(�z�K�q)��3�i�K�ȏ�K�v�B"},
	{ 18,32,55,FALSE,TRUE,A_CHR,0,5,"���ꂨ�݂����A������",
		"�����̋߂��̃����_���ȃ^�[�Q�b�g�Ɍ����đM��(�g50%)�A�Í�(��40%)�A�j��(��g3�{9%)�A�n���̍���(�勥5�{1%)�̂����ꂩ�̃{�[���𐔔����B�勥�͎������󂯂邱�Ƃ�����A��x�ɓ�ȏ�o�邱�Ƃ͂Ȃ��B"},
	{ 20,23,70,FALSE,TRUE,A_INT,0,3,"�����̋q��",
		"�^�[�Q�b�g�̏ꏊ�ɑM�������̃{�[���𔭐�������B�r��(��p)��5�i�K�ȏ�K�v�B�r���������قǈЗ͂Ɣ������a���オ��B"},
	{ 23,18,50,FALSE,TRUE,A_WIS,0,0,"�O���̐_��",
		"���������̋��͂ȃr�[������B����ȓG�����ԓG�ɂ͌��ʂ������B�r��(�_�ގq)��4�i�K�ȏ�K�v�B�r���������قǈЗ͂��オ��B"},
	{ 25,5,80,FALSE,TRUE,A_INT,0,0,"�~���N���t���[�c",
		"�H�ׂ��MP���񕜂���u�~���N���t���[�c�v�𐶐�����B�r��(��p)��25�i�K�ȏ�K�v�B"},

	{ 27,12,45,FALSE,TRUE,A_WIS,0,0,"�Y�p�̍ՋV",
		"��ǂ�W�J���A�ꎞ�I�Ɍ��f�ւ̑ϐ��𓾂�B�r��(�z�K�q)��5�i�K�ȏ�K�v�B�r���������قǌ��ʎ��Ԃ����сA10�i�K��AC�A20�i�K�Ŗ��@�h����㏸����B"},
	{ 29,36,70,FALSE,TRUE,A_INT,0,5,"�C��������",
		"�����̓G�ɑ�_���[�W��^���A���̑��̓G�ɂ͐������_���[�W��^����r�[������B�r��(��p)��7�i�K�ȏ�K�v�B�r���������ƈЗ͂��オ��B"},

	{ 32,30,60,FALSE,TRUE,A_DEX,0,0,"���E�p",
		"�����̂���ꏊ�Ɂu���̃��[���v��~�݂���B" },

	{ 34,18,60,FALSE,TRUE,A_INT,0,0,"�㎚�h��",
		"�U���I�Ȍ��E��W�J���A��莞�Ԓ��ڍU�������Ă����G�ɔj�Б����_���[�W��^����B�r��(��p)��9�i�K�ȏ�K�v�B�r���������قǌ��ʎ��Ԃ��L�т�B"},
	{ 36,24,50,FALSE,TRUE,A_WIS,0,0,"�_����",
		"��Ԙc�ȑ����̃r�[������B�r��(�_�ގq)��6�i�K�ȏ�K�v�B�r���������قǈЗ͂����オ��B"},

	{ 39,36,55,FALSE,TRUE,A_WIS,0,0,"��ǂ̉��",
		"�J�I�X�����̋��͂ȃu���X����B�r��(�z�K�q)��12�i�K�ȏ�K�v�B�r���������ƈЗ͂��オ��B"},
	{ 42,60,80,FALSE,TRUE,A_INT,0,0,"�d�̓X�|�C���[",
		"���͂̃����X�^�[�����͑����̔������N�����B���͂ȓG�قǑ唚�����N����A�d���̏ꍇ���Ɍ��ʂ������B�r��(��p)��1�i�K�ȏ�K�v�B�r���������ƈЗ͂��オ��L���͈͂��L����B"},
	{ 48,50,90,FALSE,TRUE,A_INT,0,0,"�_�������Ԑ��̋V��",
		"���̃t���A�Ŋ�Ձu�_�̕��v���g����悤�ɂȂ�B�r��(��p)��48�i�K�ȏ�K�v�B"},
	{ 48,64,75,FALSE,TRUE,A_WIS,0,0,"�_�̕�",
		"�ɂ߂ċ��͂Ȗ��͑����̑�^���[�U�[����B�r��(�_�ގq)��16�i�K�ȏ�K�v�B�u�_�������Ԑ��̋V���v���������Ă��Ȃ��Ǝg���Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


static byte sanae_minimum_cons[22] ={
	0,0,0,
	0,3,3,
	0,5,3,0,
	5,4,25,5,
	7,0,9,6,12,
	1,48,16
};

	//p_ptr->magic_num1[0]�ɉr�������[�h�A[1]�ɐ_�̕��g�p�\�t���O���i�[
cptr do_cmd_class_power_aux_sanae(int num, bool only_info)
{
	int dir,i;
	int dice = 0, sides = 0, base = 0, rad = 0;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int cons_soma = (p_ptr->magic_num1[0] == CONCENT_SOMA) ? p_ptr->concent : 0;
	int cons_kana = (p_ptr->magic_num1[0] == CONCENT_KANA) ? p_ptr->concent : 0;
	int cons_suwa = (p_ptr->magic_num1[0] == CONCENT_SUWA) ? p_ptr->concent : 0;
	int min_cons = sanae_minimum_cons[num];

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("�r��:%d�i�K",cons_soma);
			do_cmd_concentrate(CONCENT_SOMA);
		}
		break;
	case 1:
		{
			if(only_info) return format("�r��:%d�i�K",cons_kana);
			do_cmd_concentrate(CONCENT_KANA);
		}
		break;
	case 2:
		{
			if(only_info) return format("�r��:%d�i�K",cons_suwa);
			do_cmd_concentrate(CONCENT_SUWA);
		}
		break;
	case 3://�E�B���h�u���X�g
		{
			dice = 2 + plev / 10 + cons_soma / 5;
			sides = 5 + chr_adj / 10;
			base = 5 + cons_soma * 3;
			if(only_info) return format("����:%dd%d+%d",dice,sides,base);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�s���Q��������������B");		
			fire_bolt(GF_WINDCUTTER, dir, damroll(dice, sides) + base);
		}
		break;

	case 4://�X�J�C�T�[�y���g
		{
			num = MIN(16,(1 + cons_kana / 2));
			if(cons_kana >= min_cons)
			{
				dice = 3 + plev / 15 + cons_kana / 15;
				sides = 5 + chr_adj / 10;
			}
			if(only_info) return format("����:%dd%d * %d",dice,sides,num);
			if(cons_kana < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("�����̎ւ����𕑂����I");	
			for (i = 0; i < num; i++) fire_bolt(GF_MISSILE, dir, damroll(dice, sides) + base);
			
		}
		break;

	case 5://�R�o���g�X�v���b�h
		{
			if(cons_suwa >= min_cons)
			{
				rad = MIN(5, 1 + plev / 24 + cons_suwa / 7);
				dice = 5 + plev / 5;
				sides = 4 + chr_adj / 5 + cons_suwa / 2;
				base = 10 + plev + cons_suwa * 5;
			}
			if(only_info) return format("����:%dd%d + %d",dice,sides,base);
			if(cons_suwa < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("�d��������^����т��������I");	
			fire_rocket(GF_POLLUTE, dir, damroll(dice, sides)+base,rad);
			
		}
		break;
	case 6: //��I�m�o
		{
			rad = 5 + plev / 2;
			if(only_info) return format("�͈�:%d",rad);
			msg_format("���_���������܂����E�E");				
			detect_monsters_normal(rad);
			detect_monsters_invis(rad);
			reset_concent = FALSE; //�r���͓r�؂�Ȃ�
		}
		break;
	case 7://�O���C�\�[�}�^�[�W
		{
			rad = 2  + cons_soma / 15;
			if(cons_soma >= min_cons)
			{
				base = plev / 2 + chr_adj + cons_soma * 2 - 5;
			}
			if(only_info) return format("����:%d*(10+1d10)",base);
			if(cons_soma <  min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			msg_format("�F�Ƃ�ǂ�̐��������U�����I");	
			cast_meteor(base, rad, 0);
			
		}
		break;

	case 8://�t�@�t���b�L�[�Y�̊�� �T�����}�e���A���̃��[�`���𕪗��Ɨ����L����B
		{
			if(only_info) return format("����:�s��");
			if(cons_suwa <  min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			msg_format("�l�X�ȎG���ȕ����~���Ă����I");
			for (i = 1; i < m_max; i++)
			{
				int x,y;
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr;
				if (!m_ptr->r_idx) continue;
				r_ptr = &r_info[m_ptr->r_idx];
				y = m_ptr->fy;
				x = m_ptr->fx;
				if(player_bold(y,x)) continue;
				if (!player_has_los_bold(y, x) || !projectable(py, px, y, x)) continue;
				summon_material(i);
			}

		}
		break;

	case 9://���ꂨ�݂����A������
		{
			int basedam = plev * 2;
			int typ,rad;
			int num = 4 + plev / 40;
			int range = 5 + plev / 10;
			bool nasty_flag = FALSE;
			if(only_info) return format("��{�З�:%d*%d",basedam,num);
			msg_format("�����炨�݂�������͂ݎ�o���ē����������I");
			while(num > 0 && p_ptr->chp >= 0)
			{
				int roll = randint0(100);

				if(!roll && !nasty_flag)
				{
					nasty_flag = TRUE; //�勥�͈��ɓ�x�͏o�Ȃ�
					typ = GF_HELL_FIRE;
					rad = 7;
					if(one_in_(2))
					{
						msg_format("***�勥***");
						project(PROJECT_WHO_OMIKUJI_HELL, rad, py, px, basedam*5,typ, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
					}

					else
					{
						if(fire_random_target(typ, basedam*5, 5, rad, range))
							msg_format("***�勥***");
					}
				}
				else if(roll < 10)
				{
					typ = GF_HOLY_FIRE;
					rad = 4;
					if(fire_random_target(typ, basedam*3, 3, rad, range))
						msg_format("*��g*");
				}
				else if(roll < 50)
				{
					typ = GF_DARK;
					rad = 3;
					if(fire_random_target(typ, basedam, 3, rad, range))
						msg_format("-��-");
				}
				else
				{
					typ = GF_LITE;
					rad = 3;
					if(fire_random_target(typ, basedam, 3, rad, range))
						msg_format("-�g-");
				}
				num--;
			}



		}
		break;
	case 10://�����̋q��
		{
			if(use_itemcard) cons_soma = 5 + plev / 10;

			rad = 1 + plev / 20 + cons_soma / 10;
			if(cons_soma >= min_cons)
			{
				base = (plev * 3 + chr_adj * 2) * (cons_soma + 10) / 10;

			}
			if(only_info) return format("����:%d",base);
			if(cons_soma <  min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_LITE, dir, base, rad,"�˔@�Aῂ��������o�������I")) return NULL;
		}
		break;
	case 11://�O���̐_��
		{
			if(cons_kana >= min_cons)
			{
				dice = 2 + plev / 10 + chr_adj / 9;
				sides = (cons_kana + 15) * 2;
			}
			if(only_info) return format("����:%dd%d",dice,sides);
			if(cons_kana < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�˔@�A�\���������N�������I");
			fire_spark(GF_TORNADO,dir,damroll(dice,sides),1);	//v1.1.83 base��dice �����ƋC�Â��Ȃ�����
		}
		break;
	case 12://�~���N���t���[�c
		{
			object_type forge, *q_ptr = &forge;
			if(use_itemcard) cons_soma = 30;

			if(only_info) return format("");
			if(cons_soma <  min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if(use_itemcard)
				msg_print("���̒��ɉ��������Ă���B");
			else
				msg_print("��Ղ̗͂Ńt���[�c���~���Ă����I");

			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_MIRACLE_FRUIT));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);

		}
		break;
	case 13: //�Y�p�̍ՋV
		{
			int time;
			if(cons_suwa >= min_cons)
			{
				base = 15 + cons_suwa;
			}
			if(only_info) return format("����:1d%d + %d",base,base);
			if(cons_suwa < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			time = base + randint1(base);
			msg_format("�h�ǂ����ꂽ�I");	
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);		
			if(cons_suwa > 9) set_shield(time, FALSE);
			if(cons_suwa > 19) set_resist_magic(time, FALSE);
		}
		break;
	case 14://�C��������
		{
			if(use_itemcard) cons_soma = 7 + plev / 10;

			if(cons_soma >= min_cons)
			{
				dice = 1;
				sides = cons_soma * (15 + cons_soma / 2);
				base = plev * 3 + chr_adj * 5;
				if(base < 100) base = 100;
			}
			if(only_info) return format("����:%dd%d+%d",dice,sides,base);
			if(cons_soma <  min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�C��������ꌂ�������ꂽ�I");		
			fire_beam(GF_MOSES, dir, damroll(dice, sides) + base);
		}
		break;

		case 15: //���E�p
		{
			if (only_info) return "";
			msg_print("���Ȃ��͓���b�����͂ɒ��菄�点���B");
			warding_glyph();
			break;
		}


	case 16: //�㎚�h��
		{
			int time;
			if(cons_soma >= min_cons)
			{
				base = 10 + cons_soma;
			}
			if(only_info) return format("����:1d%d + %d",base,base);
			if(cons_soma < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			time = base + randint1(base);
			msg_format("�󒆂ɗ͋����㎚��؂����B");	
			set_dustrobe(time,FALSE);
		}
		break;
	case 17://�_����
		{
			if(use_itemcard) cons_kana = 15 + plev / 5;
			if(cons_kana >= min_cons)
			{
				dice = 3 + plev / 4 + chr_adj / 6;
				sides = (cons_kana + 5) * 2;
			}
			if(only_info) return format("����:%dd%d",dice,sides);
			if(cons_kana < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("�����傢�Ȃ���̂���������̂��������I");	
			fire_spark(GF_DISTORTION, dir, damroll(dice, sides),1);
			
		}
		break;

	case 18://��ǂ̉��
		{
			if(use_itemcard) cons_suwa = 15 + plev / 10;
			if(cons_suwa >= min_cons)
			{
				base = (12 + cons_suwa) * (30 + chr_adj) * plev / 100;
			}
			if(only_info) return format("����:%d",base);
			if(cons_suwa < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("�w��ɋ���ȉ��������ꂽ�E�E");	
			fire_ball(GF_CHAOS, dir, base,-3);

			//v1.1.59 ���̋Z���g���ƃJ�G�����~���Ă���炵��
			for (i = 10; i>0; i--)
			{
				int r_idx, tmp_x, tmp_y;

				r_idx = one_in_(3) ? MON_G_FROG : one_in_(2) ? MON_GAMAGAERU : MON_G_PINK_FROG;

				scatter(&tmp_y, &tmp_x, py, px, 20, 0);
				//�n�`����́��̃��[�`�����ōs����
				summon_named_creature(0, tmp_y, tmp_x, r_idx, 0L);


			}
			
		}
		break;
	case 19://�d�̓X�|�C���[
		{
			int range = 0;
			int flag_msg = FALSE;
			if(cons_soma >= min_cons)
			{
				range = MIN(MAX_SIGHT,cons_soma * 2);
				
			}
			if(only_info) return format("�͈�:%d",range);
			if(cons_soma <  min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			for (i = 1; i < m_max; i++)
			{
				int x,y;
				int dam,rad;
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr;
				if (!m_ptr->r_idx) continue;
				r_ptr = &r_info[m_ptr->r_idx];
				y = m_ptr->fy;
				x = m_ptr->fx;
				if(player_bold(y,x)) continue;
				if (!player_has_los_bold(y, x) || !projectable(py, px, y, x)) continue;
				if(m_ptr->cdis > range) continue;
				if(i == p_ptr->riding) continue;

				rad = 1 + r_ptr->level / 20;
				dam = plev + r_ptr->level * 2;
				if(cons_soma > 5) dam = dam * (cons_soma+15) / 20;
				if(r_ptr->flags3 & RF3_KWAI) dam *= 2;
				if(r_ptr->flags2 & RF2_POWERFUL) dam = dam * 3 / 2;

				if(!flag_msg)
				{
					msg_format("���͂̓G�̗d�͂��z���o���A�U���ɓ]�p�����I");
					flag_msg = TRUE;
				}
				project(0,rad,y,x,dam,GF_MANA,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP),-1);

			}
			if(!flag_msg) msg_format("���͂ɒ��x�������肪���Ȃ��悤���B");
		}
		break;
	case 20: //�_�������Ԑ��̋V��
		{
			if(only_info) return format("");
			if(cons_soma < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			else if(p_ptr->magic_num1[1])
			{
				msg_format("���ɏ����͊������Ă���B");		
				return NULL;
			}
			msg_format("�������������������B�_�̗͂����̗̈�ɖ����Ă���̂�������I");
			p_ptr->magic_num1[1] = 1;
		}
		break;

	case 21://�_�̕�
		{
			if(cons_kana >= min_cons)
			{
				dice = plev + chr_adj * 2 / 3;
				sides = cons_kana + 50;
			}
			if(only_info) return format("����:%dd%d",dice,sides);
			if(cons_kana < min_cons)
			{
				msg_format("�͂�����Ȃ��B�r�����������Ȃ��ƁB");		
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("�_�̕����S�Ă𐁂���΂����I");
			fire_spark(GF_MANA,dir,damroll(dice, sides),2);
				
		}
		break;

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}

	if(reset_concent) reset_concentration(FALSE);

	return "";
}

/*:::�×t��p�Z*/
class_power_type class_power_shizuha[] =
{
	{7,5,30,FALSE,TRUE,A_DEX,0,0,"���X�g�E�B���h���E",
		"�g���b�v����������r�[������B"},
	{20,12,50,FALSE,TRUE,A_STR,0,0,"�t�H�[�����u���X�g",
		"�˕��̃r�[��������G�𐁂���΂��B����ȓG�����ԓG�ɂ͌��ʂ������B"},
	{40,55,90,FALSE,TRUE,A_CHR,0,0,"�����̗��t",
		"�Z���ԑ��݊����󔖂ɂȂ�A�G�Ɍ������₷���Ȃ�B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_shizuha(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int plev = p_ptr->lev;
	switch(num)
	{

	case 0:
		{
			if (only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("��w�̕��������������B");
			disarm_trap(dir);
			break;
		}
	case 1:
		{
			damage = plev + chr_adj;
			if(damage < 30) damage = 30;
			if (only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�������˕��������r�ꂽ�I");
			fire_spark(GF_TORNADO, dir,damage,1);
			break;
		}
	case 2:
		{
			dice = base = 10;
			if(only_info) return format("����:%d+1d%d",base,dice);
			msg_format("���Ȃ��̐F�ʂ������Ă������E�E");
			set_tim_superstealth(base + randint1(dice),FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}



	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�e�T�p���Z*/
class_power_type class_power_kagerou[] =
{
	{5,10,0,FALSE,TRUE,A_CON,0,0,"�T�ϐg",
		"����ȘT�ɕϐg����B�����̔ӂɂ����g���Ȃ��B�ϐg���͊i���\�͂�����I�ɏ㏸���邪�قƂ�ǂ̑��������������ꖂ�@�⊪���Ȃǂ��g�p�s�\�ɂȂ�BU�R�}���h����ϐg�������ł���B"},
	{15,25,40,FALSE,TRUE,A_STR,0,0,"�t�����[�����A",
		"���i���Ŏ��E���̓G�����|�����A�������g���m�C���g������B���͂̓G���N����B�������ɂ͂���Ɏ��E���̓G�ɍ����_���[�W��^����B"},

	{24,18,60,TRUE,FALSE,A_DEX,50,5,"�n�C�X�s�[�h�p�E���X",
		"���ꂽ�G�Ɉ�u�Ŕ�т�����U������B���x�����オ��Ǝ˒����������т�B��Q��������Ǝ~�܂�B�����i���d���Ǝ��s���₷���B"},

	{30,30,75,FALSE,FALSE,A_DEX,50,10,"�X�^�[�t�@���O",
		"�߂��̃����_���ȓG�ɐ���̓ːi�U�����d�|����B���x�����オ��Ǝ˒��Ɖ񐔂�������B�����i���d���Ǝ��s���₷���B"},

	{42,100,85,TRUE,FALSE,A_STR,0,0,"���ɉ삦���E���t�t�@���O",
		"����m�����w������ɎO��A���U������B�s���͂�1.5�{�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


/*:::�e�T���Z*/
cptr do_cmd_class_power_aux_kagerou(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�T�ϐg
	case 0:
		{
			int time = 0;
			if(only_info) return format("����:�閾���܂�");
			time = ((turn / (TURNS_PER_TICK * TOWN_DAWN) + 1) * (TURNS_PER_TICK * TOWN_DAWN) - turn) / TURNS_PER_TICK;
			if(time < 5)
			{
				msg_print("�ϐg�ł��Ȃ��B�閾�����߂��悤���B");
				return NULL;
			}

			if(!set_mimic(time, MIMIC_BEAST, FALSE)) return NULL;

		}
		break;

	case 1: //�t�����[�����A
		{
			if(FULL_MOON)
			{
				int power = plev * 3 + 50;
				if(p_ptr->mimic_form == MIMIC_BEAST) power += plev * 2;
				if(only_info) return format("����:%d",power);
				msg_print("���Ȃ��͌��Ɍ����đ���K��������I");
				project_hack2(GF_SOUND,0,0,power);
				turn_monsters(power);
				set_afraid(0);
				set_hero(30 + randint1(30),FALSE);
				aggravate_monsters(0,FALSE);

			}
			else
			{
				int power = plev * 3;
				if(p_ptr->mimic_form == MIMIC_BEAST) power += plev;
				if(only_info) return format("����:%d",power);
				msg_print("���Ȃ��͋��낵���ɉ��i�������B");
				turn_monsters(power);
				set_afraid(0);
				set_hero(20 + randint1(10),FALSE);
				aggravate_monsters(0,FALSE);
			}
			break;
		}
	case 2: //�n�C�X�s�[�h�o�E���X
		{
			int len = p_ptr->lev / 5 + 5;
			if(len < 5) len = 5;
			if(only_info) return format("�˒�:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}

	case 3://�X�^�[�t�@���O rush_attack2()������
		{
			int num = plev / 9;
			int cnt;
			int range = plev / 10;
			int dam = plev  + chr_adj * 5 / 3;
			if(p_ptr->mimic_form == MIMIC_BEAST)
			{
				range += 2;
				dam += plev;
			}
			if(only_info) return  format("�˒�:%d ����:%d*%d",range,dam,num);

			msg_print("���Ȃ��͒n���R���Ē��񂾁I");
			for(cnt=0;cnt < num;cnt++)
			{
				int i, path_n;
				int tx,ty;
				int target_m_idx = 0;
				int target_num = 0;
				u16b path_g[32];
				bool moved = FALSE;
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					m_ptr = &m_list[i];
					if(!m_ptr->r_idx) continue;
					if(m_ptr->cdis > range) continue;
					if(!m_ptr->ml || !projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
					if (!is_hostile(m_ptr)) continue;
					target_num++;
					if(!one_in_(target_num)) continue;
 					target_m_idx = i;
				}
				if(!target_m_idx) break;
				tx = m_list[target_m_idx].fx;
				ty = m_list[target_m_idx].fy;
				project_length = 10;
				path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_STOP | PROJECT_KILL);
				project_length = 0;
				if (!path_n) break;
				ty = py;
				tx = px;

				/* Project along the path */
				for (i = 0; i < path_n; i++)
				{
					monster_type *m_ptr;
					int ny = GRID_Y(path_g[i]);
					int nx = GRID_X(path_g[i]);

					if (!cave[ny][nx].m_idx && player_can_enter(cave[ny][nx].feat, 0))
					{
						ty = ny;
						tx = nx;
						continue;
					}
					if (!cave[ny][nx].m_idx)
					{
						msg_print("�ǂɌ��˂����B");
						break;
					}

					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
					update_mon(cave[ny][nx].m_idx, TRUE);
					msg_format("�u���I�[���I�v");

					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
					moved = TRUE;
					project(0, 0, ny, nx, dam, GF_ARROW, PROJECT_KILL | PROJECT_JUMP, -1);

					break;
				}
				if (!moved && !player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

			}
			if(!cnt) msg_print("�E�E���������͂ɓG�����Ȃ������B");

			break;
		}
	case 4://���ɉ삦���E���t�t�@���O
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int num = 3;
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				set_afraid(0);
				set_shero(randint1(20) + 20, FALSE);
				msg_format("���Ȃ���%s�ɔ�т��������I",m_name);
				for(num=3; (num>1 && cave[y][x].m_idx);num--) py_attack(y, x, 0);
				new_class_power_change_energy_need = 150;
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			break;
		}	

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�얲�p���Z*/
class_power_type class_power_reimu[] =
{
	{1,2,15,FALSE,FALSE,A_DEX,0,3,"�p�X�E�F�C�W�����j�[�h��",
		"�������̃{���g����B���x�����オ��Ɩ{����������B"},
	{4,4,25,FALSE,TRUE,A_DEX,0,0,"�������", 
		"�אڂ���㩂┠�֊J����㩉������s���A�����ꂽ�h�A��j�󂷂�B"},
	{8,16,35,FALSE,TRUE,A_WIS,0,0,"����", //rank2
		"�������̎��ꂽ���������􂷂�B���x��30�ŋ��͂Ȏ􂢂����􂷂�悤�ɂȂ�B"},
	{10,5,30,FALSE,FALSE,A_WIS,0,4,"�C�z�T�m", //rank4
		"���͂̃����X�^�[�����m����B"},
	{14,10,40,FALSE,TRUE,A_WIS,0,4,"�A�z��", //rank2
		"�j�ב����̋�����B���x�����オ��Ɣ������a���L����B"},
	{18,24,60,FALSE,TRUE,A_INT,0,0,"����", //rank5
		"���E���̎w�肵���ʒu�Ƀe���|�[�g����B���̍s���ɂ͒ʏ��1/2�̍s���͂����g���Ȃ��B�w�肵���ꏊ�ɏo��̂Ɏ��s����ƃ����_���ȏꏊ�ɔ�ԁB"},
	{24,32,65,FALSE,TRUE,A_CHR,0,12,"���z����",//rank4
		"���͂̃����_���ȓG�ɖ������̃{�[����A���ŕ��B���x�����オ��ƈЗ́A���A�������a���㏸����B�ǂׂ̗ɂ���Ǝg���Ȃ��B"},
	{27,24,55,FALSE,TRUE,A_WIS,0,0,"����", //rank7
		"���x��*5�̗̑͂��񕜂����A�؂菝�����Â���B"},
	{30,20,50,FALSE,TRUE,A_INT,0,0,"��d���E", //rank3
		"�ꎞ�I�Ɍ��f�U���ւ̑ϐ��𓾂�B���x��40�ɂȂ��AC���㏸����B"},
	{33,1,65,FALSE,FALSE,A_WIS,0,0,"�ґz", //rank6
		"���_���������܂��AMP���킸���ɉ񕜂���B"},
	{35,45,75,FALSE,TRUE,A_WIS,0,0,"�����S���w", //rank4
		"�אڂ����^�[�Q�b�g�Ƃ��̎��ӂɔj�ב����_���[�W��^���A����Ƀ^�[�Q�b�g�̈ړ��ƃe���|�[�g��Z���ԑj�Q����B����ȓG��͋����G�ɂ͒E�o����₷���B"},
	{38,72,85,FALSE,TRUE,A_CHR,0,0,"�悭�΂�啼", //rank7
		"�啼�������ɂ̂ݎg�p�\�B�啼������ɔ��ōs���ēG��ガ�|���B"},
	{40,64,85,FALSE,TRUE,A_STR,0,0,"�A�z�򒹈�", //rank5
		"���З͂̔j�ב������P�b�g����B"},
	{47,250,90,FALSE,TRUE,A_DEX,0,0,"���팶�e", //rank8
		"�����̕��g���o��������B���g�͕ߊl�s�\�ŊK�ړ�������|�����Ə��ł��A�����͌��݂���K�̃��x���ŕς��B"},
	{50,100,80,FALSE,TRUE,A_CHR,0,0,"���z�V��", //rank9
		"�����鑩������J�����ꖳ�G������B���̖��G���͖��͏�������Ȃ����A���G�����Ă���Ԃ͖��^�[��MP50�������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_reimu(int num, bool only_info)
{
	int dir, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int rank;

	if(use_itemcard) rank = 5;
	else rank = osaisen_rank();

	switch(num)
	{

	case 0:
		{
			int num = 3 + plev / 7;
			int dice = 3 + plev / 20;
			int sides = 3 + chr_adj / 10;

			if(rank > 6) num += rank - 4;
			if(only_info) return format("����:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�j�𓊝������I");
			for (i = 0; i < num; i++) fire_bolt(GF_MISSILE, dir, damroll(dice, sides));
			break;
		}
	case 1:
		{

			if(only_info) return format("");
			msg_format("���_���W�����A�傫�����U�����B");
			(void) destroy_doors_touch();

			break;
		}
	case 2:
		{
			bool success = FALSE;
			if(only_info) return format("");
			msg_format("���̏�ɍ���A������킹�ďj�����������B");
			if(plev < 30)
			{
				if(remove_curse()) success = TRUE;
			}
			else
			{
				if(remove_all_curse())  success = TRUE;
			}
			if(success) msg_print("�����i�̎������������B");
			else msg_print("�����N����Ȃ������B");

			break;
		}
	case 3:
		{
			if(only_info) return format("");
			
			msg_print("���͂̋C�z��T�����E�E");
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			(void)detect_monsters_invis(DETECT_RAD_DEFAULT);	
			break;
		}
	case 4:
		{
			int dam = (plev * 3 + chr_adj * 5) / 2;
			int rad =  plev / 20 + rank / 3;
			if(dam < 25) dam = 25;

			if(only_info) return format("����:%d ���a:%d",dam,rad);

			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�A�z�ʂ����o���A���������I");
			fire_ball(GF_HOLY_FIRE,dir,dam,rad);		

			break;
		}

	case 5:
		{
			int x, y;
			if(only_info) return format("");
			if (!tgt_pt(&x, &y)) return NULL;

			if (p_ptr->anti_tele)
			{
				msg_print("�s�v�c�ȗ͂��e���|�[�g��h�����I");
			}
			else if (!cave_player_teleportable_bold(y, x, 0L) ||
			    (distance(y, x, py, px) > MAX_SIGHT ) ||
			    !projectable(py, px, y, x))
			{
				teleport_player(30, 0L);
				msg_print("������ƈړ��Ɏ��s�����B");
			}
			else
			{
				msg_print("���Ȃ��͗��ꂽ�ꏊ�Ɉ�u�Ō��ꂽ�B");
				teleport_player_to(y, x, 0L);
				new_class_power_change_energy_need = 50;
			}
			break;
		}
	case 6:
		{
			int rad = 1 + plev / 40 + rank / 4;
			bool flag = FALSE;
			int num = 3 + plev / 15 + rank / 2;
			int dam = plev + chr_adj;

			if(dam < 30) dam = 30;
		
			if(only_info) return format("����:%d * %d",dam,num);

			if (!in_bounds2(py + 1,px) || !cave_have_flag_grid( &cave[py + 1][px],FF_PROJECT)
				||  !in_bounds2(py - 1,px) || !cave_have_flag_grid( &cave[py - 1][px],FF_PROJECT)
				||  !in_bounds2(py ,px + 1) || !cave_have_flag_grid( &cave[py ][px + 1],FF_PROJECT)
				||  !in_bounds2(py ,px - 1) || !cave_have_flag_grid( &cave[py ][px - 1],FF_PROJECT))
				{
					msg_print("�ǂ��ז����I");
					return NULL;
				}

			msg_format("���Ȃ��̎���Ɋ���̌��������ꂽ�E�E");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_DISP_ALL, dam, 3, rad,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
			break;		

		}

	case 7:
		{
			int heal = plev * 5;
			if(only_info) return format("��:%d",heal);
			msg_format("�����̎D�����o���ē\�����B");
			hp_player(heal);
			set_cut(0);

			break;
		}
	case 8:
		{
			int base = 15 + plev / 5 + rank * 2;
			int v;
			if(only_info) return format("����:%d+1d%d�^�[��",base,base);
			msg_format("�h�ǂ𒣂菄�点���I");
			v = base + randint1(base);
			set_oppose_acid(v, FALSE);
			set_oppose_elec(v, FALSE);
			set_oppose_fire(v, FALSE);
			set_oppose_cold(v, FALSE);
			set_oppose_pois(v, FALSE);		
			if(plev > 39) set_shield(v,FALSE);

			break;
		}
	case 9:
		{
			if(only_info) return format("");	

			if (p_ptr->csp < p_ptr->msp) msg_print("���������n�b�L�������B");

			p_ptr->csp += (3 + randint1(1 + plev/10));
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}

			p_ptr->redraw |= (PR_MANA);
		}
		break;
	/*::: -Hack- �����S���w�@magic_num1[0]��tim_general[0]���g�p����*/
	//v1.1.95 GF_NO_MOVE�ɒu��������
	case 10:
		{
			int y, x;
			int dam = plev * 2 + chr_adj * 10 / 3 + (rank+1) * plev / 5;
			monster_type *m_ptr;

			int base = 7 + p_ptr->lev / 10 + rank * 2;
			if(only_info) return format("����:%d ����:%d",dam, base);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				monster_desc(m_name, m_ptr, 0);
				msg_format("�n�ʂ̐w������������オ�����I",m_name);
		
				project(0,1,m_ptr->fy,m_ptr->fx,dam,GF_HOLY_FIRE,PROJECT_KILL,-1);
				if(!m_ptr->r_idx) break;

				msg_format("%s�͌��̒��Ɏ���ꂽ�I",m_name);
				project(0, 0, m_ptr->fy, m_ptr->fx, base, GF_NO_MOVE, PROJECT_KILL|PROJECT_JUMP|PROJECT_HIDE, -1);

				//p_ptr->magic_num1[0] = cave[y][x].m_idx;
				//set_tim_general(base,FALSE,0,0);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}
	
	/*::: -Hack - �O���[�o���ϐ�oonusa_dam=0�Ƀ_���[�W�l��ݒ肵�Aproject()���ł��ꂪ0�łȂ���΂�����_���[�W�l�Ƃ��čĎˌ�����*/
	case 11: //�悭�΂�啼
		//v1.1.44 ���[�`��������
		{

			int tx,ty;
			int dam = 0;
			int restart_chance = osaisen_rank();

			if (restart_chance < 7) restart_chance = 7; //�e�X�g�p�̍Ĕ������Œ�l

			if(inventory[INVEN_RARM].tval == TV_STICK && inventory[INVEN_RARM].sval == SV_WEAPON_OONUSA ) 
				dam += calc_weapon_dam(0) / p_ptr->num_blow[0];	
			if(inventory[INVEN_LARM].tval == TV_STICK && inventory[INVEN_LARM].sval == SV_WEAPON_OONUSA ) 
				dam += calc_weapon_dam(1) / p_ptr->num_blow[1];	
			if(only_info) return format("����:%d * �s��",dam);

			if (dam <= 0)
			{
				msg_print("����Ȃ��̂𓊂��Ă����ʂ��B");
				return NULL;

			}

			msg_format("�啼���肩�痣��Ĕ��ōs�����I");
			execute_restartable_project(0, 0, restart_chance, 0, 0, dam, GF_ARROW, 0);
			msg_format("�啼�͔��ŋA���Ă��čĂю�̒��ɔ[�܂����B");

		}
		break;
	case 12:
		{
			int rad = 2 + rank / 3;
			int dice = p_ptr->lev / 4 + chr_adj / 3;
			int sides = p_ptr->lev * rank / 10;
			int base = p_ptr->lev * (1 + rank / 3);

			if(dice < 10) dice = 10;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("����ȉA�z�ʂ��X����グ�Ĕ��ł������I");
			fire_rocket(GF_HOLY_FIRE, dir, base + damroll(dice,sides), rad);
			break;
		}
	case 13: //���팶�e
		{
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_REIMU) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt)
			{
				msg_format("���łɏp�����s���Ă���B",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_REIMU, PM_EPHEMERA))
			{
				if(use_itemcard)
					msg_print("����̛ޏ��̌��e���o�������I");
				else
					msg_print("�����̌��e���o�������I");

			}
			else
				msg_print("�p�Ɏ��s�����B");


			break;
		}
	case 14:
		{
			if(only_info) return "";

			msg_print("���Ȃ��͑S�Ă̂��̂�����������ꂽ�I");
			p_ptr->special_defense |= MUSOU_TENSEI;
			p_ptr->redraw |= (PR_MAP | PR_STATUS);
			p_ptr->update |= (PU_MONSTERS | PU_BONUS);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			break;
		}




	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�E�V��p�Z*/
class_power_type class_power_yugi[] =
{

	{ 5,10,25,TRUE,FALSE,A_STR,0,0,"�n�k",
		"�_���W�����̒n�`������B"},
	{ 12,20,40,TRUE,FALSE,A_CON,0,5,"��ł̙��K",
		"�吺�Ŏ��͂ɍ��������U�����s���B���ӂ̓G���N����B"},
	{ 16,20,45,FALSE,TRUE,A_DEX,0,0,"�n���̋��",
		"�Z���Ԃ̊ԁA�����Ǝ��͂̓G�̃e���|�[�g��j�Q����B"},
	{ 21,27,85,FALSE,TRUE,A_STR,0,3,"��]�R��",
		"���E���̓G�ɕ������_���[�W��^����B�З͂͘r�͂Ɉˑ�����B"},
	{ 25,50,80,TRUE,FALSE,A_STR,0,0,"�S�C���p",
		"��莞�ԋ���m������������B"},
	{ 32,32,80,FALSE,FALSE,A_STR,0,0,"���͗��_",
		"���͂�*�j��*����B���x�����オ��Ɣ͈͂��L����B"},
	{ 39,48,90,FALSE,TRUE,A_STR,0,0,"��������",
		"���E���̓G�Ɋj�M�����_���[�W��^����B�З͂͘r�͂Ɉˑ�����B"},
	{ 45,33,80,FALSE,FALSE,A_STR,0,0,"�O���K�E",
		"���͂̓G�Ƀ_���[�W��^����B���s���邽�тɑ����ƈЗ͂Ɣ͈͂��ω�����B�f�莞�̂ݎg�p�\�B"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_yugi(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int str_adj = adj_general[p_ptr->stat_ind[A_STR]];
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int rad = 5 + plev / 10;
			if(only_info) return format("�͈�:%d",rad);
			if(use_itemcard)
				msg_print("�J�[�h���n�ʂ��������h�炵���I");
			else
				msg_print("�n�ʂ𓥂ݖ炵���B");
			earthquake(py, px, rad);

			break;
		}
	case 1:
		{
			int dam = plev * 3 + str_adj * 5;
			int rad = 3 + plev / 16 + str_adj / 20;
			if(only_info) return format("����:�`%d",dam/2);
			msg_format("�傫�������z�����݁A�剹����������I");
			project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL|PROJECT_GRID , -1);
			aggravate_monsters(0,FALSE);
			break;

		}
	case 2:
		{
			int base = 10;
			if(only_info) return format("����:%d+1d%d",base,base);
			set_nennbaku(randint1(base) + base, FALSE);
			break;
		}
	case 3:
		{
			int sides = plev * 3 + str_adj * 5;

			if(only_info) return format("����:1d%d",sides);
			msg_format("�₽�����������r�ꂽ�I");
			project_hack2(GF_TORNADO,1,sides,0);			
			break;
		}
	case 4:
		{
			int base = 25;
			int v;
			if(only_info) return format("����:%d+1d%d",base,base);
			v = base + randint1(base);
			set_shero(v, FALSE);
			hp_player(30);
			set_afraid(0);
			set_fast(v, FALSE);
			break;
		}
	case 5:
		{
			int rad = plev / 5 + str_adj / 5;
			if(only_info) return format("�͈�:%d",rad);
			if(!destroy_area(py, px, rad, FALSE,FALSE,FALSE)) msg_print("�����ł͎g���Ȃ��B");

			break;
		}

	case 6:
		{
			int base = plev * 3 + str_adj * 5;

			if(only_info) return format("����:%d+1d%d",base/2,base/2);
			msg_format("������M���ƔM���������r�ꂽ�I");
			project_hack2(GF_NUKE,1,base/2,base/2);
			break;
		}
	case 7:
		{
			int dam = 0;

			if(!p_ptr->concent) dam = plev * 3 + str_adj * 3;
			else if(p_ptr->concent == 1) dam = plev * 5 + str_adj * 5;
			else dam = plev * 10 + str_adj * 10 + chr_adj * 10;

			if(only_info)
			{
				if(p_ptr->concent < 2)
					return format("����:�`%d",dam/2);
				else
					return format("����:%d",dam);
			}
			if(p_ptr->concent == 0)
			{
				do_cmd_concentrate(0);
				project(0, 3, py, px, dam, GF_DISP_ALL, PROJECT_KILL|PROJECT_GRID , -1);

			}
			else if(p_ptr->concent == 1)
			{
				do_cmd_concentrate(0);	
				project(0, 6, py, px, dam, GF_FORCE, PROJECT_KILL|PROJECT_GRID , -1);
			}
			else
			{
				msg_format("���Ȃ����͋����O���ڂ𓥂ݏo���ƁA���͂̑S�Ă̂��̂�������񂾁I");
				project_hack2(GF_DISINTEGRATE,0,0,dam);
			}


			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�d���p���Z(�ʏ펞)*/
class_power_type class_power_keine1[] =
{
	{8,10,30,FALSE,TRUE,A_INT,0,2,"�O��̐_��@��",
		"�������̃{���g����B�u����V���v�𑕔����Ă���ƍ��З͂̌��̌��ɂȂ�B"},
	{13,16,40,FALSE,TRUE,A_INT,0,3,"�O��̐_��@��",
		"�M�������̋�����B�u���������ʁv�𑕔����Ă���ƍ��З͂̔j�ב����̋��ɂȂ�B"},
	{20,40,55,FALSE,TRUE,A_INT,0,0,"�O��̐_��@��",
		"�ꎞ�I�ɔ��˔\�͂𓾂�B�u���@���v�𑕔����Ă���ƈꎞ�I�ɋ��ɂ̑ϐ��𓾂�B"},
	{26,10,30,FALSE,TRUE,A_WIS,0,0,"���j�B���T",
		"HP�������񕜂��A�łƐ؂菝�����Â���B"},
	{30,20,45,FALSE,TRUE,A_WIS,0,0,"���j�B���U",
		"���͂̃g���b�v�����ł�����B"},
	{35,52,70,FALSE,TRUE,A_WIS,0,0,"���j�B���V",
		"�w�肵���V���{���̓G���t���A�����������B������G�̃��x���ɉ����ă_���[�W���󂯂�B��R����邱�Ƃ�����B"},
	{42,65,80,FALSE,TRUE,A_WIS,0,0,"���j�B���W",
		"�Z���Ԏ���̎p���B���A�G�ɔF������ɂ����Ȃ�B"},
	{45,90,85,FALSE,TRUE,A_CHR,0,0,"���o���̓V�q",
		"���E���̑S�Ăɑ΂����З͂̑M�������U�����s���B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

/*:::�d���p���Z(�n�N�^�N��)*/
class_power_type class_power_keine2[] =
{
	{8,10,30,FALSE,TRUE,A_INT,0,2,"�O��̐_��@��",
		"�������̃{���g����B�u����V���v�𑕔����Ă���ƍ��З͂̌��̌��ɂȂ�B"},
	{13,16,40,FALSE,TRUE,A_INT,0,3,"�O��̐_��@��",
		"�M�������̋�����B�u���������ʁv�𑕔����Ă���ƍ��З͂̔j�ב����̋��ɂȂ�B"},
	{20,40,55,FALSE,TRUE,A_INT,0,0,"�O��̐_��@��",
		"�ꎞ�I�ɔ��˔\�͂𓾂�B�u���@���v�𑕔����Ă���ƈꎞ�I�ɑS�Ă̑ϐ��𓾂�B"},
	{25,40,60,FALSE,TRUE,A_INT,0,0,"�I�[���h�q�X�g���[",
		"������t���A�̒n�`�ƃA�C�e����S�Ċ��m���A���E���̓G�̏���m��B"},
	{30,55,75,FALSE,TRUE,A_INT,0,0,"���߂苴",
		"HP�����S�ɉ񕜂��A�S�Ă̏�Ԉُ�A�\�͒ቺ�A�o���l�����A�ꎞ���ʁA�ˑR�ψق������B"},
	{35,48,75,FALSE,TRUE,A_INT,0,0,"�l�N�X�g�q�X�g���[",
		"�t���A���č\������B�č\���̔����܂łɂ͂����͂��ȃ^�C�����O������B"},
	{43,180,90,FALSE,TRUE,A_INT,0,0,"�����L��",
		"�����̋߂��ɂ��錶�z���̑��݂ł͂Ȃ��G�ɑ΂��A�ɂ߂č��З͂̎��󑮐��_���[�W��^����B"},
	{45,90,85,FALSE,TRUE,A_CHR,0,0,"���o���̓V�q",
		"���E���̑S�Ăɑ΂����З͂̑M�������U�����s���B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};


/*:::�d�����Z*/
cptr do_cmd_class_power_aux_keine(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	bool hakutaku = FALSE;

	if(use_itemcard)
	{
		if(num != 5) hakutaku = TRUE;

	}
	else if(p_ptr->magic_num1[0]) hakutaku = TRUE;

	switch(num)
	{
	case 0:
		{
			int dice = 5 + plev / 5;
			int sides = 6 + chr_adj / 6;
			bool hyper = FALSE;
			if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].name1 == ART_KUSANAGI
				|| buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].name1 == ART_KUSANAGI)
				hyper = TRUE;

			if(hyper) dice *= 3;

			if(only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			if(hyper)
			{
				msg_print("�_�����s�����߂����I");
				fire_beam(GF_PSY_SPEAR, dir,damroll(dice,sides));
			}
			else
			{
				msg_print("���^�̒e��������B");
				fire_bolt(GF_MISSILE,dir,damroll(dice,sides));
			}
			break;
		}
	case 1:
		{
			int dam = 10 + plev + chr_adj * 2;
			int rad = 2;
			bool hyper = FALSE;

			if(inventory[INVEN_NECK].name1 == ART_MAGATAMA)	hyper = TRUE;

			if(hyper)
			{
				dam *= 2;
				rad = 5;
			}
			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(hyper)
			{
				msg_print("�_�󂩂������ꂾ�����I");
				fire_ball(GF_HOLY_FIRE, dir, dam, rad);
			}
			else
			{
				msg_print("���e��������B");
				fire_ball(GF_LITE, dir, dam, rad);
			}
			break;

		}
	case 2:
		{
			int base = 10;
			int sides = 10;
			int v;
			if(only_info) return format("����:%d+1d%d",base,sides);
			v = randint1(sides) + base;

			if(inventory[INVEN_RARM].name1 == ART_KAGAMI || inventory[INVEN_LARM].name1 == ART_KAGAMI )
			{
				msg_format("�_����ῂ��������I");
				set_oppose_acid(v, FALSE);
				set_oppose_elec(v, FALSE);
				set_oppose_fire(v, FALSE);
				set_oppose_cold(v, FALSE);
				set_oppose_pois(v, FALSE);
				set_ultimate_res(v, FALSE);
			}
			else
			{
				msg_format("�ڂ̑O�Ɍ��鋾�����ꂽ�B");
				set_tim_reflect(v, FALSE);
			}

			break;
		}
	case 3://���j�B��1�A�I�[���h�q�X�g���[
		if(hakutaku)
		{
			if(only_info) return format("");
			msg_print("�邳�ꂽ���j�����̒��ɗ��ꍞ��ł���E�E");
			wiz_lite(FALSE);
			probing();

		}
		else
		{
			int heal = (plev + 20 + chr_adj) / 2;
			if(only_info) return format("����:%d",heal);

			msg_print("�����󂯂����j���B�����B");
			hp_player(heal);
			set_cut(0);
			set_poisoned(0);

		}
		break;
	case 4:
		if(hakutaku)
		{
			if(only_info) return format("");
			msg_print("�����j�̍č\�z���J�n�����E�E");
			hp_player(5000);
			set_poisoned(0);
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

			if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
			{
				if(muta_erasable_count()) msg_print("�S�Ă̓ˑR�ψق��������B");
				p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
				p_ptr->muta1 = p_ptr->muta1_perma;
				p_ptr->muta2 = p_ptr->muta2_perma;
				p_ptr->muta3 = p_ptr->muta3_perma;
				p_ptr->muta4 = p_ptr->muta4_perma;
				p_ptr->update |= PU_BONUS;
				p_ptr->bydo = 0;
				handle_stuff();
				mutant_regenerate_mod = calc_mutant_regenerate_mod();
			}
			msg_print("�č\�z�����������B");

		}
		else
		{
			int rad = 2 + plev / 10;
			if(only_info) return format("�͈�:%d",rad);
			msg_print("���͂�㩂��Ȃ��������Ƃɂ����I");
			//v1.1.70 ����㩂��Ώۂ��甲���Ă����̂ŏC��
			project(0, rad, py, px, 0, GF_KILL_TRAP, (PROJECT_GRID|PROJECT_ITEM), -1);

		}
		break;

	case 5:
		if(hakutaku)
		{
				if(only_info) return format("");
				if (!get_check("���̃t���A�̗��j�̍č\�z�����݂܂����H")) return NULL;

				if (p_ptr->inside_arena || ironman_downward)
				{
					msg_print("�����N����Ȃ������B");
				}
				else
				{
					/*:::0�ł͂܂����͂�*/
					p_ptr->alter_reality = 1;
					p_ptr->redraw |= (PR_STATUS);
				}

		}
		else
		{
			int power = plev * 4;
			if(power < 120) power = 120;
			if(only_info) return format("����:%d",power);
			symbol_genocide(power, TRUE,0);

		}
		break;


	case 6:
		if(hakutaku)
		{
			int dam = plev * plev;
			if(dam < 1000) dam = 1000;
			if(only_info) return format("����:�`%d",dam/2);
			msg_print("���ʗ��̏C�������������I");
			project(0, 4, py, px, dam, GF_N_E_P, PROJECT_KILL, -1);

		}
		else
		{
			int base = 10;
			int sides = 5;
			if(only_info) return format("����:%d+1d%d",base,sides);

			set_tim_superstealth(randint1(sides) + base, FALSE, SUPERSTEALTH_TYPE_NORMAL);

		}
		break;

	case 7:
		{
			int dice = 5 + p_ptr->lev / 2;
			int sides = 15 + chr_adj / 2;
			if(sides < 1) sides = 1;
			if(only_info) return format("����:%dd%d",dice,sides);
			msg_format("���Ȃ��͑��z�̂悤�ɋP���n�߂��I");
			project_hack2(GF_LITE,dice,sides,0);
			break;
		}
		break;

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}








/*:::���їp���Z*/
class_power_type class_power_murasa[] =
{
	{3,3,20,FALSE,TRUE,A_WIS,0,1,"���e",
		"��З͂̐������{���g����B"},
	{14,21,40,TRUE,TRUE,A_STR,0,15,"�����A���J�[",
		"�d�𓊂����A���������G�Ƃ��̎��ӂɖ������_���[�W��^����B�r�͂������ƈЗ͂��オ��B"},
	{20,20,55,FALSE,TRUE,A_WIS,0,0,"�f�B�[�v���H�[�e�b�N�X",
		"�����̎��ӂɐ������U�����s���A����ɒn�`�𐅂ɂ���B���łɐ��n�`��̏ꍇ�З͂��オ��B"},
	{32,30,65,FALSE,TRUE,A_DEX,0,0,"�V���J�[�S�[�X�g",
		"�w�肵�����n�`�փe���|�[�g����B�ʏ��1/3�̍s�����Ԃ����g��Ȃ��B���n�`�ɂ��Ȃ��Ǝg���Ȃ��B"},
	{40,100,70,FALSE,TRUE,A_CHR,0,0,"�D��̃��B�[�i�X",
		"���E���S�Ă̏��𐅖v�����A�G�ɐ������_���[�W��^����B"},
	{46,72,75,FALSE,TRUE,A_CHR,0,0,"�f�B�[�v�V���J�[",
		"�אڂ����G�ɋ��͂Ȑ������U�����s���A�񃆃j�[�N�Ő��ϐ��̂Ȃ����������m���ňꌂ�œ|���B�������[�����ɂ��Ȃ��Ǝg�����A�����ł���G�E�͋����G�E����ȓG�ɂ͌����ɂ����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

/*:::���ѓ��Z*/
cptr do_cmd_class_power_aux_murasa(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dice = 3 + plev / 5 + chr_adj / 7;
			int sides = 4;

			if(only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���ۂ��琅���΂����B");
			fire_bolt(GF_WATER,dir,damroll(dice,sides));
			break;
		}
	case 1:
		{
			int sides = plev * 3;
			int base = 30 + (adj_general[p_ptr->stat_ind[A_STR]] * 4);

			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�d�𓊂������I");
			fire_rocket(GF_ARROW,dir,base + randint1(sides),1);

			break;
		}
	case 2:
		{
			int dam = 20 + plev + chr_adj;
			int rad = 1 + plev / 24;
			if(cave_have_flag_bold((py), (px), FF_WATER)) dam *= 4;

			if(only_info) return format("����:�`%d ",dam/2);

			if(cave_have_flag_bold((py), (px), FF_WATER))
			{
				msg_print("��Q�����������I");
			}
			else
				msg_print("�n�ʂ��琅�������o�����I");

			project(0, rad, py, px, rad+1, GF_WATER_FLOW, PROJECT_GRID, -1);
			project(0, rad, py, px, dam, GF_WATER, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();


			break;
		}
	case 3:
		{
			if(only_info) return format("");

			msg_print("���Ȃ��͐��̒��ɏ������E�E");
			if (!dimension_door(D_DOOR_MURASA)) return NULL;
			new_class_power_change_energy_need = 33;
			break;
		}
	case 4:
		{
			int damage = plev + chr_adj * 5;
			if(damage < 80) damage = 80;
			if(only_info) return format("�����F%d",damage);
			msg_format("�ǂ�����Ƃ��Ȃ������N���o���A���ʂ��݂�݂�オ���Ă����I");
			project_hack2(GF_WATER_FLOW,0,0,2);
			project_hack2(GF_WATER,0,0,damage);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();

			break;
		}
	case 5:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 3 + chr_adj * 15;
			if(only_info) return format("�����F%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int chance = 100;
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("���Ȃ���%s�Ƌ��ɐ���ւƒ���ł������E�E",m_name);
				if((r_ptr->flags7 & RF7_AQUATIC) || (r_ptr->flagsr & RFR_RES_WATE) 
					|| !monster_living(r_ptr) || (r_ptr->flags1 & RF1_QUESTOR)
					|| (r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
				{
					chance = 0;
				}
				else
				{
					if(r_ptr->flags7 & RF7_CAN_SWIM) chance = chance * 2 / 3;
					if(r_ptr->flags7 & RF7_CAN_FLY) chance /= 2;
					if(r_ptr->flags2 & RF2_POWERFUL) chance /= 2;
					if(r_ptr->flags2 & RF2_GIGANTIC) chance /= 2;
				}
				if(randint1(r_ptr->level) < chance)
				{
					msg_format("%s�𐅒�֒��߂��B", m_name);
					damage = m_ptr->hp + 1;
				}
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_WATER,PROJECT_KILL,-1);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	

		break;

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::��p���Z �G��Ă���(p_ptr->magic_num1[0]��0�łȂ�)�Ƃ���ڈȊO�g�p�s��*/
class_power_type class_power_chen[] =
{
	{7,10,20,FALSE,TRUE,A_INT,0,0,"�P����",
		"�������ʂ̂���{�[������B"},
	{15,20,40,TRUE,TRUE,A_DEX,50,0,"���L�u��v",
		"���ꂽ�ꏊ�̓G�ֈ�u�ŋ삯���U������B��Q���ɓ�����Ǝ~�܂�B�������d���Ǝ��s���₷���B"},
	{20,10,50,FALSE,TRUE,A_CHR,0,5,"�V���",
		"���E���S�Ăɑ΂���З̖͂������U�����s���B"},
	{27,20,30,FALSE,FALSE,A_DEX,0,0,"��яd�˗�",
		"�אڂ����G�ɍU�����A���̂܂܈�u�ŗ��E����B���E�Ɏ��s���邱�Ƃ�����B"},
	{33,30,50,FALSE,TRUE,A_INT,0,0,"������ʓV",
		"�ꎞ�I�ɕ��V�Ɖ����𓾂�B"},
	{40,36,72,FALSE,TRUE,A_INT,0,0,"���ٍb",
		"�ꎞ�I�Ɏ_�E�d���E�Ή��E��C�ɑ΂���ϐ��𓾂�B"},
	{45,72,85,FALSE,TRUE,A_STR,0,0,"���Ĕ�����V",
		"���͂ɖ��͑����̃{�[�����T���U�炵�A���̌��u�ŗ��E����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::����Z*/
cptr do_cmd_class_power_aux_chen(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int range = 10 + plev / 10;
			int dam = 20 + plev + chr_adj;

			if(only_info) return format("�˒�:%d ����:%d",range,dam);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�d���ݗ��𓊂������B");
			fire_ball(GF_CONFUSION,dir,dam,2);

			break;
		}
	case 1:
		{
			int len = 3 + p_ptr->lev / 10;
			if(only_info) return format("�˒�:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 2:
		{
			int base = p_ptr->lev;
			int dice = 1;
			int sides = chr_adj * 2;
			if(only_info) return format("����:%dd%d+%d",dice,sides,base);
			if(one_in_(2))
				msg_format("���Ȃ��͘r��V�ɓ˂��グ����グ���B");
			else
				msg_format("���͂ɏՌ����������I");
			project_hack2(GF_MISSILE,dice,sides,base);
			break;
		}
	case 3:
		{
			if(only_info) return format("");	
			if(!hit_and_away()) return NULL;
			break;
		}
	case 4:
		{
			int base = 25;
			int sides = 25;
			int v;
			if(only_info) return format("����:%d+1d%d",base,sides);
			if(!use_itemcard) msg_format("���Ȃ��͗d�p�Œ��𕑂����I");
			v = randint1(sides) + base;

			set_tim_levitation(v, FALSE);
			set_fast(v, FALSE);
			break;
		}
	case 5:
		{
			int base = 25;
			int sides = 25;
			int v;
			if(only_info) return format("����:%d+1d%d",base,sides);
			if(!use_itemcard) msg_format("���Ȃ��͌��������G�ȓ����ŋ삯������B");
			v = randint1(sides) + base;
			set_oppose_acid(v, FALSE);
			set_oppose_elec(v, FALSE);
			set_oppose_fire(v, FALSE);
			set_oppose_cold(v, FALSE);
			break;
		}
	case 6:
		{
			int rad = 2;
			int len = 8;
			int damage = 10 + plev + chr_adj * 3 ;
			if(damage < 75) damage = 75;

			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("�S�_�̗͂����͂����W�����I");
			cast_meteor(damage, rad, GF_MANA);
			teleport_player(20,0L);

			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�H����p�Z*/
class_power_type class_power_yuuka[] =
{

	{20,25,50,FALSE,TRUE,A_CHR,0,0,"���z���̊J��",
		"���͂ɉԂ��炩����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},

};


cptr do_cmd_class_power_aux_yuuka(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			msg_print("���͂ɉԂ��炫���ꂽ�I");
			project(0, 3 + (plev / 10), py, px, 0, GF_MAKE_FLOWER, (PROJECT_GRID | PROJECT_HIDE), -1);
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::�E�Ґ�p�Z*/
class_power_type class_power_ninja[] =
{
	{ 3,3,25,FALSE,FALSE,A_WIS,0,0,"�E�C���m",
		"���͂̐��_���������X�^�[�����m����B"},
	{ 5,3,30,FALSE,FALSE,A_INT,0,0,"�H������",
		"�H���𒲒B����B"},
	{ 8,5,30,FALSE,TRUE,A_DEX,0,0,"������̏p",
		"�G��̂̓������~�߂�B�����ϐ��̂���G�⃆�j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B���x��30�ɂȂ�Ǝ��E���S�Ăɋ�������d�|����B"},
	{ 12,25,55,FALSE,TRUE,A_DEX,0,0,"�ŉt�h�z",
		"�������̕����邢�͏㎿�̕���ɓő�����t�^����B����̉��i��99%���ɂȂ�B"},
	{ 15,15,50,TRUE,FALSE,A_DEX,50,0,"�ꌂ���E",
		"�G�ɍU���������ƁA��u�ŒZ�����̃e���|�[�g������B���s���邱�Ƃ�����B"},
	{ 18,20,50,FALSE,FALSE,A_INT,0,0,"����p",
		"���͂̒n�`�A�A�C�e���A�g���b�v�A�����X�^�[�����m����B���x�����オ��ƌ��ʔ͈͂��L����B"},
	{ 20,10,50,TRUE,FALSE,A_DEX,0,0,"�����p",
		"�A�C�e�������З͂œ�������B(���F�{���̓_�C�X�����ɂ̂݊|����)"},
	{ 23,10,45,FALSE,FALSE,A_DEX,0,0,"㩁E���j��",
		"�꒼����̃g���b�v�ƕ����h�A��j�󂷂�B"},
	{ 25,15,55,FALSE,TRUE,A_DEX,0,0,"�Γ�",
		"�����̂���ꏊ�ɉΉ��̋��𔭐������A�Z�����̃e���|�[�g�����A�ꎞ�I�ɉΉ��ɑ΂���ϐ��𓾂�B"},
	{ 27,16,75,FALSE,FALSE,A_INT,0,0,"�ӎ�",
		"�A�C�e�����Ӓ肷��B"},
	{ 30,25,60,FALSE,FALSE,A_DEX,50,0,"�ς��g�̏p",
		"�U�����󂯂�����x�����_���[�W���󂯂��Ƀe���|�[�g����悤�ɂȂ�B���s���邱�Ƃ�����B"},
	{ 33,20,70,FALSE,FALSE,A_INT,0,0,"�Ŕj",
		"���E���̓G�̃p�����[�^��\�͂�m��B"},
	{ 36,8,50,FALSE,FALSE,A_DEX,0,0,"����",
		"�����̂��镔�����Â�����B"},
	{ 40,20,65,TRUE,FALSE,A_DEX,50,0,"�k�n�̏p",
		"�G�Ɉ�u�ŋߊ��A���̂܂܍U������B��Q���ɓ�����Ƃ����Ŏ~�܂�B"},
	{ 44,50,70,FALSE,TRUE,A_INT,50,0,"���g�̏p",
		"��莞�ԁA�G����̍U���̔����𖳌�������B�u�B�`�̏p�v�Ɠ����ɂ͎g���Ȃ��B"},
	{ 48,72,75,FALSE,FALSE,A_DEX,50,0,"�B�`�̏p",
		"��莞�ԁA���邢�ꏊ�ł��G���甭������ɂ����Ȃ�B�u���g�̏p�v�Ɠ����ɂ͎g���Ȃ��B"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_ninja(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			int rad = DETECT_RAD_DEFAULT;
			if(only_info) return format("�͈�:%d",rad);
			detect_monsters_mind(rad);
			break;
		}
	case 1:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "�H���𐶐�";
			msg_print("�H���𒲒B�����B");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}
	case 2:
		{
			int power = plev * 2;
			if(only_info) return format("����:%d",power);

			if(plev < 30)
			{
				if (!get_aim_dir(&dir)) return NULL;
				msg_print("���Ȃ��͕W�I���w�����p���������B");
				(void)stasis_monster(dir);
				break;
			}
			else
			{
				msg_print("�K�E�̔O��������I");
				stasis_monsters(power);
			}
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			brand_weapon(4); //�闖�G�S
			break;
		}
	case 4:
		{
			if(only_info) return format("");	
			if(!hit_and_away()) return NULL;
			break;
		}

	case 5: 
		{
			int rad = 5 + plev / 2;
			if(only_info) return format("�͈�:%d",rad);
			msg_print("���ӂ𒲍������E�E");
			map_area(rad);
			detect_all(rad);
			break;

		}
	case 6:
		{
			int mult = 2 + plev / 15;
			if (only_info) return format("�{���F%d",mult);
			msg_print("���Ȃ��͑S�g�𝚂߂��E�E");
			if (!do_cmd_throw_aux(mult, FALSE, -1)) return NULL;
			break;
		}
	case 7:
		{
			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			destroy_door(dir);
			break;

		}

	case 8:
		{
			base = 10 + plev / 5;
			if(only_info) return format("");
			fire_ball(GF_FIRE, 0, 50+plev, plev/10+2);
			teleport_player(30, 0L);
			set_oppose_fire(base + randint1(base), FALSE);
			break;

		}
	case 9:
		{
			if(only_info) return format("");
			if (!ident_spell(FALSE)) return NULL;
			break;
		}
	case 10:
		{
			if(only_info) return format("");
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("���Ȃ��͂�������Ǝd���݂��ς܂����E�E");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
			}

			break;
		}
	case 11:
		{
			if(only_info) return format("");
			probing();
			break;
		}
	case 12:
		{
			if(only_info) return format("");
			msg_print("���������ɕ�܂ꂽ�I");
			unlite_room(py,px);
			break;
		}
	case 13:
		{
			int len = p_ptr->lev / 5;
			if(only_info) return format("�˒�:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 14:
		{
			dice = base = 6;
			if(only_info) return format("����:%d+1d%d",base,dice);
			set_tim_superstealth(0,TRUE,0);
			set_multishadow(base+randint1(dice), FALSE);
			break;
		}
	case 15:
		{
			dice = base = 15;
			if(only_info) return format("����:%d+1d%d",base,dice);
			set_multishadow(0, TRUE);
			msg_format("���Ȃ��̎p�͔��Β��ɗn�����E�E");
			set_tim_superstealth(base + randint1(dice),FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���O����p�Z*/
class_power_type class_power_wriggle[] =
{

	{ 5,5,25,FALSE,TRUE,A_INT,0,0,"���̒m�点",
		"���͂̃����X�^�[�̈ʒu�����m����B"},
	{ 10,8,40,FALSE,TRUE,A_CHR,0,3,"峏���",
		"���͂�峂̃����X�^�[����������B"},
	{ 16,7,45,FALSE,TRUE,A_CHR,0,4,"�n��̗���",
		"���E���̃����_���ȓG�ɖ������̃{���g����B���x�����オ��Ɛ���������B"},
	{ 22,20,50,FALSE,FALSE,A_CHR,0,0,"峉��_",
		"���E���̒��n�̓G��z�����F�D�ɂ��悤�Ǝ��݂�B���j�[�N�����X�^�[�⍂���x���ȓG�ɂ͌����ɂ����B"},
	{ 28,18,55,FALSE,TRUE,A_STR,0,2,"�t�@�C���t���C�t�F�m���m��",
		"���͂̓G�ɑM���_���[�W��^���A����Ɏ��E���̓G������������B"},
	{ 36,50,70,FALSE,TRUE,A_CHR,0,0,"�c�c�K���V����",
		"���͂Ɂu�����̌Q��v����������B"},
	{ 45,54,80,FALSE,TRUE,A_WIS,0,0,"�i�C�g�o�O�g���l�[�h",
		"�^�[�Q�b�g�̈ʒu�ɋ��͂ȑM�������̃{�[���𔭐�������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_wriggle(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int rad = 9 + plev / 3;
			if(only_info) return format("�͈�:%d",rad);
			msg_print("峂���������̗l�q��m�点�Ă��ꂽ�B");
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			(void)detect_monsters_invis(DETECT_RAD_DEFAULT);

			break;
		}
	case 1:
		{
			bool flag = FALSE;
			if(only_info) return "";
			for(i=0;i<(2 + p_ptr->lev / 15) ;i++)
			{
				if ((summon_specific(-1, py, px, p_ptr->lev, SUMMON_SPIDER, (PM_ALLOW_GROUP | PM_FORCE_PET))))flag = TRUE;
			}
			if(flag)
				msg_print("峒B���Ăяo�����I");
			else 
				msg_print("峂͌���Ȃ������B");
			break;

		}
	case 2:
		{
			bool flag = FALSE;
			int i;
			int dice = 2 + plev / 10;
			int sides = 3 + chr_adj / 10;
			int num = 4 + plev / 8;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			msg_print("峂�����������I");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_ARROW, damroll(dice,sides),1, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
			break;
		}
	case 3:
		{
			int j;
			int chance = plev * 2 + chr_adj * 5;
			if(only_info) return format("");

			if (p_ptr->inside_arena)
			{
				msg_print("�����̓��Z�͎g���Ȃ��B");
				return NULL;
			}


			if(one_in_(4))msg_print("���Ȃ��͓���ȍ����������E�E");
			else if(one_in_(3))msg_print("���Ȃ���峂̌��t�Řb���������E�E");
			else if(one_in_(2))msg_print("���Ȃ��͗d��������������E�E");
			else msg_print("���Ȃ��͐G�p��U�f�I�ɓ��������E�E");

			for (j = 1; j < m_max; j++)
			{
				char m_name[80];
				monster_type *m_ptr;
				monster_race *r_ptr;
				bool success = FALSE;

				//���E���̒��n�̓G���Ώ�
				m_ptr = &m_list[j];
				if(!m_ptr->r_idx) continue;
				if(r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR) continue;
				if(!projectable(py,px,m_ptr->fy,m_ptr->fx)) continue;
				if(is_pet(m_ptr) || is_friendly(m_ptr)) continue;
				if(!m_ptr->ml) continue;

				r_ptr = &r_info[m_ptr->r_idx];
				if(r_ptr->d_char != 'I' && r_ptr->d_char != 'c' && r_ptr->d_char != 'w') continue;

				monster_desc(m_name, m_ptr, 0);
				(void)set_monster_csleep(j, 0);
				if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || r_ptr->level > plev)
				{
					chance /= 2;
					if(r_ptr->level < randint1(chance))
					{
						msg_format("%s�ƒ��ǂ��Ȃ����I",m_name);
						set_friendly(m_ptr);
						success = TRUE;
					}
				}
				else
				{
					if(r_ptr->level < randint1(chance))
					{
						msg_format("%s�͂��Ȃ��ɖ�����ꂽ�I",m_name);
						set_pet(m_ptr);
						success = TRUE;
					}
				}
				if(!success) msg_format("%s�͂��Ȃ��ɓG�΂��Ă���B",m_name);

			}
			
			break;
		}
	case 4:
		{
			int rad = 2 + plev / 12;
			base = p_ptr->lev * 3 + chr_adj * 5;
			if(only_info) return format("����:�`%d",base / 2);
			msg_format("���Ȃ���ῂ�����������I");
			project(0, rad, py, px, base, GF_LITE, PROJECT_KILL | PROJECT_ITEM, -1);
			confuse_monsters(plev * 3);
			break;
		}
	case 5:
		{
			bool flag = FALSE;
			int max = p_ptr->lev / 10;
			if(max < 2) max = 2;
			if(only_info) return format("�ő�:%d��",max);

			for(i=0;i<max;i++) if(summon_named_creature(0, py, px, MON_TUTUGAMUSHI, PM_FORCE_PET)) flag = TRUE;
			if(flag) msg_print("�c�c�K���V���Ăяo�����I");
			else msg_print("峂͌���Ȃ������B");

			break;
		}
	case 6:
		{
			int dice = plev/2 ;
			int sides = 15 + chr_adj / 2;
			int rad = 4;

			if(only_info) return format("����:%dd%d",dice,sides);
		
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_LITE,dir,damroll(dice,sides),rad,"���̌Q�ꂪ�W�I�ւƏP���|�������I")) return NULL;

			break;
		}


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�������p�Z*/
//�J�Q�̒��ǖʂ�tim_gen[0]���g�p
class_power_type class_power_kokoro[] =
{
	{ 3,4,30,FALSE,TRUE,A_WIS,0,0,"����T�m",
		"���͂̐��_���������X�^�[�����m����B"},
	{ 5,10,0,FALSE,TRUE,A_DEX,0,0,"�ʐ���",
		"�אڂ����G��̂ɍU�����s���B�U���̖������͊i���U���ɏ�����B���̍U���œ|�����G�͖ʂ𗎂Ƃ��A��������Ƃ��̓G�̔\�͂��Č��ł���B���_�������Ȃ��G�ɂ͌����Ȃ��B"},
	{ 5,0,0,FALSE,TRUE,A_DEX,0,0,"�ĉ�",
		"�������Ă���ʂ̓G�������Ă����Z�▂�@���Č�����B��Փx�����MP�̔���͌ʂɍs����B�З͖͂ʂ̌��ƂȂ����G�̔\�͂Ɉˑ����邪�u���X�Ȃǂ�HP��1/6�Ƃ��Čv�Z�����B"},

	{ 12,16,35,FALSE,TRUE,A_CHR,0,3,"�X�l�n��J��",
		"�����̎��͂̃����X�^�[�����|�����悤�Ƃ���B���x��20�ō����A���x��30�ȏ�ŞN�O�A���x��40�ōU���͒ቺ���ʂ��ǉ������B��R�����Ɩ����B" },
		
	{ 18,24,35,FALSE,TRUE,A_WIS,0,5,"��{���y�|�[�b�V����",
		"�אڂ��������X�^�[��̂Ɏ����̋��|�E�N�O�E���o�E����m���E�悵�X�y�V�����̏�Ԉُ�̂����ǂꂩ�ЂƂ�ϐ��𖳎����ĉ����t����B���̏�Ԉُ���Ȃ���Ύ��s����B�N�G�X�g�œ|�Ώۃ����X�^�[�Ɛ��_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B���̓��Z�͋���m����Ԃł��g�����Ƃ��ł���B"},
	{ 20,16,40,FALSE,TRUE,A_DEX,0,0,"������̃��[���b�g",
		"���|�A�j���A�q�[���[�A����m���̂����ꂩ�̏�ԂɂȂ�B���x��30�őS�ϐ��A���x��45�ő̗͉�+�S�����̌��ʂ����ɉ����B"},
	{ 24,20,50,FALSE,TRUE,A_CHR,0,2,"����̉Βj�� ",
		"����̕��ƂƂ��ɉԉ΂��y�􂵁A���͂ɕ�����̉Ή������U�����s���B"},
	{ 28,25,50,FALSE,TRUE,A_DEX,0,0,"�{�����T�̖�",
		"�Z������ːi���A�G�ɓ�����ƋC�����_���[�W��^���Ď~�܂�B���x�����オ��Ƌ��������т�B"},

	{ 32,40,60,FALSE,TRUE,A_CHR,0,0,"�J�Q�̒��ǖ�",
		"��莞�ԁA�אڍU�����d�|���Ă��������X�^�[�ɃJ�E���^�[�ŋ��|�E�����E�N�O��Ԃ�t�^����悤�ɂȂ�B" },

	{ 36,36,70,FALSE,TRUE,A_STR,0,0,"�V�g�̐_�y���q",
		"�v���Y�}�����̋��͂ȃ��[�U�[����B"},

	{ 40,48,75,FALSE,TRUE,A_CHR,0,0,"�J�����͗J���̏���",
		"���E���S�Ẵ����X�^�[�����|�A�����A�N�O�A�U���͒ቺ�����悤�Ǝ��݂�B��R�����Ɩ����B" },

	{ 45,96,85,FALSE,TRUE,A_CHR,0,0,"�����L�[�|�[�b�V����",
		"���E���S�Ăɖ��́E�Í��E�M���E�C�����̋��͂ȍU�����s���B"},
	

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_kokoro(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int rad = 10 + plev / 2;
			if(only_info) return format("�͈�:%d",rad);

			detect_monsters_mind(DETECT_RAD_DEFAULT);
			break;
		}

	case 1:
		//�ʂɉ�����
		{

			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int dir, damage;

			damage= plev * 4;

			if(only_info) return  format("����:1d%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if(dir==5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				int chance;
				int mon_ac;

				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				mon_ac = r_ptr->ac;

				//v1.1.94 �����X�^�[�h��͒ቺ����AC25%�J�b�g
				if (MON_DEC_DEF(m_ptr))
				{
					mon_ac = MONSTER_DECREASED_AC(mon_ac);
				}

				chance = (p_ptr->skill_thn + (p_ptr->to_h[0] + p_ptr->lev * ref_skill_exp(SKILL_MARTIALARTS) / 8000)* BTH_PLUS_ADJ);
				msg_format("���Ȃ���%s�֔�т��������I",m_name);

				if(MON_CSLEEP(m_ptr) || test_hit_norm( chance, mon_ac, m_ptr->ml))
				{
					project(0,0,y,x,randint1(damage),GF_KOKORO,(PROJECT_KILL | PROJECT_HIDE),0);
				}
				else
				{
					msg_format("�������]���ꂽ�B",m_name);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
		}
		break;

	case 2:
		{
			if(only_info) return "";

			if(!cast_monspell_new()) return NULL;
			break;
		}

	case 3: //v1.1.95 ���ʏ����ύX
	{
		int power = plev * 5 + chr_adj * 5;
		if (only_info) return format("����:�`%d", power / 2);

		msg_print("��]�̃I�[����������I");

		project(0, 8, py, px, power, GF_TURN_ALL, PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL, -1);
		if (plev > 19)
			project(0, 8, py, px, power, GF_OLD_CONF, PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL, -1);
		if (plev > 29)
			project(0, 8, py, px, power, GF_STUN, PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL, -1);
		if (plev > 39)
			project(0, 8, py, px, power, GF_DEC_ATK, PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL, -1);

		break;
	}
	
	case 4: //v1.1.95 ���\�ύX

		{
			monster_type *m_ptr;
			monster_race *r_ptr;
			int m_idx;

			if (only_info) return format("����:�s��");

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			m_idx = cave[py + ddy[dir]][px + ddx[dir]].m_idx;
			m_ptr = &m_list[m_idx];

			if (m_idx && (m_ptr->ml))
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�Ɍ����Ċ���̔g����������I", m_name);

				//QUESTOR�ɂ͌����Ȃ�����B����m�����10�����炢�K�u���݂��Ă���g���΃{�X����������n���E�������Ȃ̂�
				if (r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags1 & RF1_QUESTOR)
				{
					msg_format("%s�ɂ͌��ʂ��Ȃ������I", m_name);
					break;
				}
				if (p_ptr->afraid)
				{
					if (set_monster_monfear(m_idx, p_ptr->afraid))
						msg_format("%s�͋��|���ē����o�����I", m_name);
					set_afraid(0);
				}
				else if (p_ptr->stun)
				{
					if (set_monster_stunned(m_idx, p_ptr->stun))
						msg_format("%s�͞N�O�Ƃ����B", m_name);
					set_stun(0);
				}
				else if (p_ptr->shero)
				{
					if (set_monster_timed_status_add(MTIMED2_BERSERK, m_idx, p_ptr->shero))
						msg_format("%s�͓ˑR�\��o�����I", m_name);
					set_shero(0, TRUE);
				}
				//�悵�X�y�V�����@�U���ቺ+����
				else if (p_ptr->tsuyoshi)
				{
					if (set_monster_confused(m_idx, p_ptr->tsuyoshi + p_ptr->image))
					{
						if (is_gen_unique(m_ptr->r_idx) || my_strchr("hknopstuzAFGKLOPSTUVWY", r_ptr->d_char) || r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN))
							msg_format("%s�u�I�N���Z����I�I�v", m_name);
						else 
							msg_format("%s�͉����L�}�b������Ă�悤���I", m_name);

					}
					set_monster_timed_status_add(MTIMED2_DEC_ATK, m_idx, p_ptr->tsuyoshi + p_ptr->image);

						//�ʏ�̏�����ʂ����Ɍ��ʂ���������B����p�̘r�ώ㉻�𖳎����邽��
						p_ptr->tsuyoshi = 0;
					p_ptr->redraw |= (PR_STATUS);
					p_ptr->update |= (PU_BONUS | PU_HP);
					//���o�������Ă���
					set_image(0);
				}
				//���o�͍��������ɂ���
				else if (p_ptr->image)
				{
					if (set_monster_confused(m_idx, p_ptr->image))
						msg_format("%s�͍��������B", m_name);
					set_image(0);
				}
				else
				{
					msg_print("�����������t����悤�Ȋ��������Ă��Ȃ������B");
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}



	case 5:
		{
			int time;
			int base = 20;
			int choice;
			if(only_info) return format("����:%d + 1d%d",base,base);

			if(plev > 44) choice = 6;
			else if(plev > 29) choice = 5;
			else choice = 4;
			time = base + randint1(base);

			switch(randint1(choice))
			{
				case 1:
					msg_print("�Q���V�k�̖ʂ��������I");
					set_hero(0,TRUE);
					set_shero(0,TRUE);
					set_blessed(0,TRUE);
					if (!p_ptr->resist_fear) set_afraid(p_ptr->afraid + time);
					break;
				case 2:
					msg_print("�΂��V��̖ʂ��������I");
					set_hero(time,FALSE);
					set_shero(0,TRUE);
					set_blessed(0,TRUE);
					set_afraid(0);
					break;
				case 3:
					msg_print("�������̖ʂ��������I");
					set_hero(0,TRUE);
					set_shero(0,TRUE);
					set_blessed(time,FALSE);
					set_afraid(0);
					break;

				case 4:
					msg_print("�ʎ�̖ʂ��������I");
					set_hero(0,TRUE);
					set_shero(time,FALSE);
					set_blessed(0,TRUE);
					set_afraid(0);
					break;
				case 5:
					msg_print("�V��̖ʂ��������I");
					set_hero(0,TRUE);
					set_shero(0,TRUE);
					set_blessed(0,TRUE);
					set_afraid(0);
					set_oppose_acid(time, FALSE);
					set_oppose_elec(time, FALSE);
					set_oppose_fire(time, FALSE);
					set_oppose_cold(time, FALSE);
					set_oppose_pois(time, FALSE);
					break;
				case 6:
					msg_print("�V���Ȋ�]�̖ʂ��������I");
					hp_player(500);
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
					break;
				default:
					msg_print("ERROR:�����냋�[���b�g�Ŗ���`�̐��l���o��");
					return NULL;
			}



		}
		break;
	case 6:
		{
			int rad = 2 + plev / 24;
			int damage = plev  + chr_adj;
			if(damage < 20) damage = 20;

			if(only_info) return format("����:%d*(10+1d10)",damage);
			if(use_itemcard)
				msg_format("�J�[�h����ԉ΂������U�����I");
			else
				msg_format("����̕��𕑂����I");
			cast_meteor(damage, rad, GF_FIRE);

			break;
		}
	case 7:
		{
			int len = plev / 7;
			int damage = 50 + plev * 2;
			if(len < 4) len = 4;
			
			if(only_info) return format("�˒�:%d ����:%d",len,damage);
			if (!rush_attack2(len,GF_FORCE,damage,0)) return NULL;
			break;
		}

	case 8:
	{
		int base = 20;
		if (only_info) return format("����:%d+1d%d", base, base);

		set_tim_general(base + randint1(base), FALSE, 0, 0);

	}
	break;

	case 9:
		{
			dice = p_ptr->lev / 3;
			sides = 10 + chr_adj;
			if(dice < 10) dice = 10;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���q�̖ʂ̌�����M���������ꂽ�I");
			fire_spark(GF_PLASMA,dir, damroll(dice,sides),1);

			break;
		}


	case 10: //v1.1.95
	{
		int power = plev * 3 + chr_adj * 5;
		if (only_info) return format("����:%d", power);

		msg_print("�����̖ʂ����͂ɔ��...");

		turn_monsters(power);
		confuse_monsters(power);
		stun_monsters(power);
		project_hack(GF_DEC_ATK, power);

		break;
	}


	case 11:
		{
			base = p_ptr->lev;
			dice = 1;
			sides = chr_adj * 5;
			if(only_info) return format("����:(%dd%d+%d) * 4",dice,sides,base);
			msg_format("�߂��ꂽ�悤�ɗx�苶�����I");
			project_hack2(GF_MANA,dice,sides,base);
			project_hack2(GF_DARK,dice,sides,base);
			project_hack2(GF_LITE,dice,sides,base);
			project_hack2(GF_FORCE,dice,sides,base);
		}
		break;

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�_�q��p�Z*/
class_power_type class_power_miko[] =
{
	{ 1,0,0,FALSE,FALSE,A_INT,0,0,"�����X�^�[���m",
		"��z�������o�Ŏ��͂̃����X�^�[�����m����B���x�����オ��Ɣ͈͂��L����B"},
	{ 7,12,30,FALSE,TRUE,A_WIS,0,5,"���o���鏈�̓V�q",
		"�����̎��͂ɑM�������̃{�[���𕡐�����������B"},
	{ 12,16,45,FALSE,FALSE,A_INT,0,0,"�Ŕj",
		"��z������͂Ŏ��E���̃����X�^�[�̃p�����[�^����Z�Ȃǂ�m��B"},
	{ 18,18,50,FALSE,TRUE,A_WIS,0,3,"�\�����̃��[�U�[",
		"�j�ב����̃r�[������B"},
	{ 24,30,85,FALSE,TRUE,A_DEX,0,0,"�k�n�̃}���g",
		"���E���̔C�ӂ̏ꏊ�Ƀe���|�[�g����B���̓��Z��(100-���x��)%�̍s�����Ԃ�������Ȃ��B"},
	{ 29,32,70,FALSE,TRUE,A_CHR,0,8,"�O�Z�t���b�V��",
		"���E���̑S�Ăɑ΂��M�������̍U�����s���B"},
	{ 36,100,80,FALSE,TRUE,A_CHR,0,0,"��������",
		"�u�����z�s�v�u�h��j���Áv����������B�������ꂽ��l�͕ߊl�Ȃǂ̑ΏۂɂȂ炸�K�ړ�����Ə�����B"},
	{ 41,170,85,FALSE,TRUE,A_STR,0,0,"�\�����̌��@���e",
		"�����̎��͂ɔj�ב����̋��͂ȃ{�[���𕡐�����������B"},
	{ 48,240,80,FALSE,TRUE,A_CHR,0,0,"�ق������Ă͕K���T��",
		"�w�肵�������֋��͂ȍU�����s���B�З͖͂��͂ɑ傫���ˑ�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_miko(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int rad = 5 + plev * 2 / 5;
			if(only_info) return format("�͈�:%d",rad);
			
			msg_format("���Ȃ��͎����Ă��͂������B");
			detect_monsters_normal(rad);
			detect_monsters_invis(rad);
			msg_format("�����Ă������B");
			break;
		}
	case 1:
		{
			int rad = 2 + plev / 20;
			int damage = plev  + chr_adj ;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("�����̌��e��������I");
			cast_meteor(damage, rad, GF_LITE);

			break;
		}
	case 2:
		{
			if(only_info) return format("");
			msg_format("���͂����n�����E�E");
			probing();
			break;
		}
	case 3:
		{
			dice = p_ptr->lev / 3;
			sides = chr_adj / 3 + p_ptr->lev / 10;
			base = p_ptr->lev + chr_adj * 3;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�\�����̌����������ꂽ�I");
			fire_beam(GF_HOLY_FIRE, dir, base + damroll(dice,sides));
			break;
		}
	case 4:
		{
			int x, y;
			if(only_info) return format("");
			if (!tgt_pt(&x, &y)) return NULL;
			if (!cave_player_teleportable_bold(y, x, 0L) ||
			    (distance(y, x, py, px) > MAX_SIGHT ) ||
			    !projectable(py, px, y, x))
			{
				msg_print("�����ւ͈ړ��ł��Ȃ��B");
				return NULL;
			}
			else if (p_ptr->anti_tele)
			{
				msg_print("�s�v�c�ȗ͂��e���|�[�g��h�����I");
			}
			else
			{
				msg_print("���Ȃ��͗��ꂽ�ꏊ�Ɉ�u�Ō��ꂽ�B");
				teleport_player_to(y, x, 0L);
				new_class_power_change_energy_need = 100 - plev;
			}
			break;
		}
	case 5: 
		{
			dice = p_ptr->lev / 2;
			sides = 10 + chr_adj / 2;
			if(dice < 15) dice=15;
			if(sides < 1) sides = 1;
			if(only_info) return format("����:%dd%d",dice,sides);
			msg_format("���Ȃ���ῂ�����������I");
			project_hack2(GF_LITE,dice,sides,0);
			break;
		}
	case 6:
		{
			int i;
			bool huto = FALSE, toziko = FALSE;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_FUTO) ) huto = TRUE;
				if((m_ptr->r_idx == MON_TOZIKO)) toziko = TRUE;
			}
			if(!huto && summon_named_creature(0, py, px, MON_FUTO, PM_EPHEMERA))
			{
				if(one_in_(3))msg_print("�u���ɂ��܂������I�v");
				else if(one_in_(2))msg_print("�u�͂Ȃ��ɂ��Ȃ�ʂ�I�v");
				else msg_print("�u�܂��܂����I�v");
			}
			if(!toziko && summon_named_creature(0, py, px, MON_TOZIKO, PM_EPHEMERA))
			{
				if(one_in_(3))msg_print("�u����Ă���I�v");
				else if(one_in_(2))msg_print("�u���납���̂߂��I�v");
				else msg_print("�u�܂��ł����H�v");
			}

			if(huto && toziko) msg_print("���łɓ�l�Ƃ����̃t���A�ɂ���B");
			break;		

		}

	case 7:
		{
			int rad = 4;
			int damage = plev * 2 + chr_adj * 2;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_format("���̔��e���~�蒍�����I");
			cast_meteor(damage, rad, GF_HOLY_FIRE);

			break;
		}
	case 8:
		{
			dice = p_ptr->lev / 2;
			sides = chr_adj * 3;
			if(dice < 15) dice = 15;
			if(sides < 30) sides = 30;

			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("����Ȍ��̌�����n��؂�􂢂��I");
			fire_spark(GF_PSY_SPEAR,dir, damroll(dice,sides),1);

			break;
		}



	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�͂��Đ�p�Z*/
class_power_type class_power_hatate[] =
{
	{ 5,6,35,FALSE,TRUE,A_INT,0,2,"�O�ʇT",
		"���͂̒n�`�E�A�C�e���E�����X�^�[�����m����B�g���b�v�͊��m�ł��Ȃ��B���x�����オ��Ɗ��m���a���L����B"},
	{ 12,18,45,FALSE,TRUE,A_DEX,0,3,"���s�b�h�V���b�g",
		"�����̋߂��̃����_���Ȉʒu�ɑM�������U�����s���B"},
	{ 20,24,50,FALSE,TRUE,A_INT,0,0,"�O�ʇU",
		"�L�[���[�h����͂��A����Ɉ�v�����A�C�e���⃂���X�^�[���t���A�ɑ��݂���΂��̎��ӂ�O�ʂ���B�����̌�₪�������ꍇ�K���Ȉ�ӏ����I�΂��B"},
	{ 27,10,60,FALSE,TRUE,A_DEX,0,0,"�Ă���p�p���b�`",
		"�w�肵���G�̎ʐ^���B��B�ː����ʂ��Ă��Ȃ��G�ɂ������ł���B"},
	{ 36,45,70,FALSE,TRUE,A_DEX,0,0,"�t���p�m���}�V���b�g",
		"���E���S�ĂɑM�������U�����s���B"},
	{ 48,100,70,FALSE,TRUE,A_INT,0,0,"�O�ʇV",
		"�t���A�S�Ă̒n�`�E�A�C�e���E�����X�^�[�����m����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_hatate(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int cons = p_ptr->concent;

	switch(num)
	{

	case 0:
		{
			int rad = 9 + plev / 3;
			if(only_info) return format("�͈�:%d",rad);
			if(use_itemcard)
				msg_print("�J�����Ɏ��͂̌��i���ʂ����I");
			else
				msg_print("���Ȃ��̓J�����ɔO�����߁A�V���b�^�[���������B");

			map_area(rad);
			detect_objects_normal(rad);
			detect_objects_gold(rad);
			detect_monsters_normal(rad);
			break;
		}
	case 1:
		{
			int rad = 2 + plev / 30;
			int damage = 10 + plev + chr_adj * 2;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_print("���͂𖳍��ʂɎB�e�����I");
			cast_meteor(damage, rad, GF_LITE);

			break;
		}
	case 2:
		{
			if(only_info) return format("");
			if(!hatate_psychography()) return NULL;
			break;

		}
	case 3:
		{
			if(only_info) return format("");

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !cave[target_row][target_col].m_idx || !m_list[cave[target_row][target_col].m_idx].ml)
			{
				msg_print("�^�[�Q�b�g���w�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}

			msg_print("���Ȃ��͔�ʑ̂��v�������ׁA�V���b�^�[���������B");
			project(0, 0, target_row, target_col, 1, GF_PHOTO,  PROJECT_JUMP | PROJECT_KILL, -1);

			break;
		}
	case 4:
		{
			int damage = plev*2 + chr_adj * 2;
			if(only_info) return format("����:%d",damage);
			msg_print("���͂��L�p���[�h�ŘA���B�e�����I");
			project_hack2(GF_LITE,0,0,damage);
			break;
		}

	case 5:
		{
			if(only_info) return format("");
			msg_print("�S�͂ŃJ�����ɏW�������E�E");
			wiz_lite(FALSE);
			(void)detect_monsters_normal(255);

			break;

		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�L�X����p�Z*/
class_power_type class_power_kisume[] =
{
	{ 5,0,0,FALSE,FALSE,A_DEX,0,0,"��яオ��",
		"������яオ��B��яオ���Ă��鎞�͎��̍U�����󒆂���̗����U���ɂȂ�A�U���񐔂�1�ɂȂ邪�啝�ȃ_�C�X�{�[�i�X���t���B�܂����x�ɉ����ēG����̍U����������ɂ����Ȃ邪�A����ȓG�E����ԓG�E�����G�E�͋����G�ɂ͌��ʂ������B"},
	{ 10,3,25,FALSE,TRUE,A_CHR,0,3,"�S��",
		"�󒆂���^�[�Q�b�g�̈ʒu�ɉΉ������̋��𗎂Ƃ��B�˒������͒Z����������яオ���Ă���Ǝ˒����L�т�B���̋Z�̎g�p���ɂ͍��x��������Ȃ��B"},
	{ 18,18,35,TRUE,FALSE,A_STR,0,10,"�ޕr���Ƃ��̉�",
		"�w��ʒu�ɗ����������ɂ���G�Ƀ_���[�W��^����B������яオ���Ă���Ǝ˒����L�шЗ͂��オ��B�܂�������v���ƈЗ͂��オ��B"},
	{ 27,16,50,FALSE,TRUE,A_DEX,0,0,"���ň�̒�",
		"�w��ʒu�ɗ����������U���_���[�W�Ɛ��_���[�W��^���A����Ɏ��͂𐅒n�`�ɂ��A�ꎞ�I�ȉΉ��ϐ��𓾂�B������яオ���Ă���Ǝ˒����L�шЗ͂��オ��B�܂�������v���ƈЗ͂��オ��B"},
	{ 40,80,70,FALSE,TRUE,A_CON,0,0,"�E�F���f�B�X�g���N�^�[",
		"�w��ʒu�ɗ�����覐Α����̋���ȋ��𔭐������A����ɒn�k���N�����B������яオ���Ă���Ǝ˒����L�шЗ͂��オ��B�܂�������v���ƈЗ͂��オ��B���������������͂̓G���N����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_kisume(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int cons = p_ptr->concent;

	if(use_itemcard) cons = 3 + plev / 20;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");

			do_cmd_concentrate(0);
			break;
		}
	case 1:
		{
			int range = 2 + cons * 2;
			int dam = 20 + plev * 2 + chr_adj;

			if(only_info) return format("�˒�:%d ����:%d",range,dam);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�󒆂���S�΂𗎂Ƃ����B");
			fire_ball(GF_FIRE,dir,dam,2);

			reset_concent = FALSE; //���x���ێ�����
			break;
		}
	case 2:
		{
			int range = 4 + cons * 2;
			int base = (plev) * (cons+1) + calc_kisume_bonus();
			int tx,ty,dam;
			
			if(only_info) return format("����:%d + 1d%d",base/2,base/2);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			dam = base / 2 + randint1(base/2);
			ty = target_row;
			tx = target_col;
			if(!fire_ball_jump(GF_ARROW,dir,dam,0,"���Ȃ��͔�яオ��A�W�I�ڊ|���ė��������I")) return NULL;
			teleport_player_to(ty,tx,TELEPORT_NONMAGICAL);
			//���肪�݂�����G�̂Ƃ��ǂ��Ȃ�̂��`�F�b�N�v
			break;
		}
	case 3:
		{
			int range = 3 + cons * 2;
			int base;
			int tx,ty,dam;

			if(use_itemcard)
				base = plev * (cons+1) / 2 + 20 * cons;
			else
				base = plev * (cons+1) / 2 + calc_kisume_bonus();
			
			if(only_info) return format("����:%d + 1d%d",base/2,base/2);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			dam = base / 2 + randint1(base/2);
			ty = target_row;
			tx = target_col;
			if(!fire_ball_jump(GF_WATER_FLOW,dir,3,3,"�n�ʂɃq�r������A���������o�����I")) return NULL;
			fire_ball_jump(GF_WATER,dir,dam,3,NULL);
			teleport_player_to(ty,tx,TELEPORT_NONMAGICAL);
			set_oppose_fire(randint1(20) + 20, FALSE);

			break;
		}
	case 4:
		{
			int range = 1 + cons;
			int base = plev * (1 + cons*2) + calc_kisume_bonus() * 3;
			int tx,ty,dam;

			if(only_info) return format("����:%d + 1d%d",base/2,base/2);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			dam = base / 2 + randint1(base/2);
			ty = target_row;
			tx = target_col;
			if(!fire_ball_jump(GF_METEOR,dir,dam,5,"���Ȃ���覐΂̂悤�ɑ�n��������I")) return NULL;
			teleport_player_to(ty,tx,TELEPORT_NONMAGICAL);
			earthquake(ty, tx, 5);
			aggravate_monsters(0,FALSE);
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�����[�z���C�g��p�Z*/
class_power_type class_power_lilywhite[] =
{
	{1,1,0,FALSE,TRUE,A_CHR,0,0,"�e��",
		"�������̃{���g����B���͂������ƈЗ͂��オ��B"},

	{25,10,45,FALSE,TRUE,A_CHR,0,4,"�T�v���C�Y�X�v�����O",
		"�^�[�Q�b�g�̈ʒu���ӂɌ�����ʂ̖������{���g����B���͂������ƈЗ͂��オ��B"},

	{40,40,60,FALSE,TRUE,A_CHR,0,0,"�t�̖K��",
		"���͂ɏt���K���B"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},

};


cptr do_cmd_class_power_aux_lilywhite(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int dam = plev + chr_adj;
			if(only_info) return format("����:%d",dam);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�e����������B");
			fire_bolt(GF_MISSILE, dir, dam);
			break;
		}
	case 1:
		{
			int num = plev / 2;
			int sides = (plev + chr_adj)/3;
			if(only_info) return format("����:1d%d * %d",sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("��ʂ̉Ԃт炪�U�����I");

			fire_blast(GF_MISSILE, dir, 1, sides, num, 3, 0);
			break;
		}
	case 2:
		{
			int rad = plev / 7;
			if(rad < 4) rad = 4;
			if(only_info) return format("");
			msg_print("���Ȃ��͍��炩�ɏt�̓������������I");
			project(0, rad, py, px, 0, GF_MAKE_FLOWER, (PROJECT_GRID | PROJECT_HIDE), -1);
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::�����Ð�p�Z*/
class_power_type class_power_toziko[] =
{
	{5,2,25,FALSE,TRUE,A_STR,0,3,"���̖�",
		"�d�������̃{���g����B���x�����オ��ƒe����������B"},

	{20,20,50,FALSE,TRUE,A_WIS,0,5,"�K�S�E�W�T�C�N����",
		"�����̂���ʒu�ɋ��͂ȓd�������̃{�[���𔭐�������B"	},

	{25,50,80,FALSE,TRUE,A_DEX,0,0,"���",
		"�u�d���̖�v�𐔖{��������B"	},

	{36,54,65,FALSE,TRUE,A_WIS,0,15,"�K�S�E�W�g���l�[�h",
		"���E���̑S�Ăɑ΂��d���U�����s���B"	},

	{45,88,75,FALSE,TRUE,A_CHR,0,0,"�����̗�",
		"�^�[�Q�b�g�̏ꏊ�ɋ��͂ȓd���̃{�[���𔭐�������BHP���Ⴂ�قǈЗ͂��オ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},

};


cptr do_cmd_class_power_aux_toziko(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int num = 1 + plev / 8;

			dice = 3;
			sides = 6 + plev / 15 + chr_adj / 10 ;
			if(only_info) return format("����:%dd%d * %d",dice,sides,num);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���̖��������B");
			for (i = 0; i < num; i++) fire_bolt(GF_ELEC, dir, damroll(dice, sides));
			break;
		}
	case 1:
		{
			int rad = 2 + plev / 15;
			base = plev * 8 + chr_adj * 10;
			if(base < 200) base = 200;
			if(only_info) return format("����:�`%d",base / 2);	
			if(use_itemcard)
				msg_format("�J�[�h�����������d�����I");
			else
				msg_format("���Ȃ��͌��������d�����B");
			project(0, rad, py, px, base, GF_ELEC, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}
	case 2:
		{
			int xx,yy;
			object_type forge;
			object_type *o_ptr = &forge;

			if(only_info) return format("");

			object_prep(o_ptr, (int)lookup_kind(TV_ARROW, SV_AMMO_LIGHT));
			o_ptr->number = plev / 10 + randint1(plev/10);
			o_ptr->to_d = p_ptr->lev / 5;
			o_ptr->to_h = p_ptr->lev / 5;
			o_ptr->name2 = EGO_ARROW_ELEC;
			object_aware(o_ptr);
			object_known(o_ptr);
			o_ptr->discount = 99;
			msg_format("��Ȃ��̌`�ɂ����B");
			(void)drop_near(o_ptr, -1, py, px);

			break;
		}
	case 3:
		{
			dice = p_ptr->lev / 2;
			sides = 15 + chr_adj / 2;
			if(only_info) return format("����:%dd%d",dice,sides);
			msg_format("��Ȃ̗������͂��Ă��������I");
			project_hack2(GF_ELEC,dice,sides,0);
			break;
		}

	case 4:
		{
			int dice = plev/2 + chr_adj/2;
			int sides = plev/2;
			int rad ;
			int mult = 100 + 300 * (p_ptr->mhp - p_ptr->chp) / p_ptr->mhp;

			if(dice < 20) dice = 20;
			if(sides < 16) sides = 16;
			sides = sides * mult / 100;
			rad = mult / 50;
			if(only_info) return format("����:%dd%d",dice,sides);
		
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_ELEC,dir,damroll(dice,sides),rad,"���̓S�Ƃ������ꂽ�I")) return NULL;

			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���q�p���Z*/
class_power_type class_power_kyouko[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�ق�",
		"�����Ă���O����̂��Ă���̂�����Ύ~�߂�B�s���͂�����Ȃ��B"},
	{6,12,25,TRUE,FALSE,A_STR,0,15,"�`���[�W�h�N���C",
		"�����̎��͂ɍ��������̍U�����s���B�З͂͌���HP�Ɉˑ�����B���͂̐Q�Ă���G���N����B"},
	{10,20,0,FALSE,FALSE,A_DEX,0,0,"�}�E���e���G�R�[",
		"���̃t���A�ł��Ȃ����Ō�ɕ������������J��Ԃ��B�З͂���ʂ͂��Ȃ��̔\�͂Ɉˑ�����B"},
	{18,12,50,FALSE,TRUE,A_DEX,0,2,"�����O�����W�G�R�[",
		"�^�[�Q�b�g�̈ʒu�ɍ��������̋��𔭐�������B"},
	{24,32,60,FALSE,TRUE,A_DEX,0,7,"�p���[���]�i���X",
		"���E���S�Ăɑ΂������ōU������B���͂̐Q�Ă���G���N����B"},
	{30,24,65,FALSE,TRUE,A_WIS,0,0,"�����O��",
		"�Ђ�����吺�ŔO�������������A�����Ă���Ԏ��͂̓G�Ƀ_���[�W��^���N�O�A���|������B�j�ׂ���_�Ƃ���G�ɂ����������A�����x���ȓG�ɂ͌����ɂ����B"},
	{38,36,75,FALSE,FALSE,A_STR,0,0,"�v���C�}���X�N���[��",
		"�⋩�̂悤�ȉ̂��̂��A���͂̓G�ɍ��������U������������B���͂̐Q�Ă���G���N����B"},
	{44,60,80,FALSE,FALSE,A_CON,0,0,"�`���[�W�h���b�z�[",
		"���������̋��͂ȃr�[������B���͂̐Q�Ă���G���N����B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

/*:::���q���Z*/
/*::: p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]���̂Ɏg����*/
/*::: p_ptr->magic_num1[5]���I�E���Ԃ����̎����ԍ��i�[�Ɏg����*/
///mod160305 �I�E���Ԃ������ԍ���num1[5]�łȂ�monspell_yamabiko�Ɋi�[���邱�Ƃɂ���
cptr do_cmd_class_power_aux_kyouko(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�ق�@�s��������Ȃ�
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1: //�`���[�W�h�N���C
		{
			int rad = 1 + plev / 10;
			int dam = p_ptr->chp * 2 / 3;
			if(dam > 1600) dam=1600;
			if(only_info) return format("����:�`%d",dam / 2);

			stop_singing();
			if(one_in_(2)) msg_format("�u���͂�[�������܂��I�v");
			else msg_format("�uYAH��O�I�v");
			project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL|PROJECT_GRID , -1);

			aggravate_monsters(0,FALSE);
			break;
		}
	case 2://�}�E���e���G�R�[
		{
			cptr mname;
			if(monspell_yamabiko) mname = monspell_list2[monspell_yamabiko].name;
			else mname = "�Ȃ�";

			if(only_info) return mname;

			if(!monspell_yamabiko)
			{
				msg_format("�Ԃ����t���Ȃ��B");
				return NULL;
			}

			if(!use_itemcard) stop_singing();
			if(!cast_monspell_new_aux(monspell_yamabiko,FALSE,FALSE,CAST_MONSPELL_EXTRA_KYOUKO_YAMABIKO)) return NULL;

			kyouko_echo(TRUE,0);//��x�I�E���Ԃ����������
			break;
		}
	case 3://�����O�����W�G�R�[
		{
			int dir;
			int rad = 3;
			int dice = p_ptr->lev / 4 + chr_adj / 3;
			int sides = 5 + p_ptr->lev / 8;
			int base = p_ptr->lev ;

			if(dice < 5) dice = 5;

			if(p_ptr->lev > 39) rad = 4;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_SOUND, dir, base + damroll(dice,sides), rad, "�ؗ삪��_�Ɏ��������I")) return NULL;
			break;
		}


	case 4://�p���[���]�i���X
		{
			int base = 50 + plev * 2;

			if(only_info) return format("����:%d",base);
			stop_singing();
			if(dun_level) msg_format("���Ȃ��̑吺���_���W�����ɋ����n�����I");
			else msg_format("���Ȃ��̑吺����R�ɋ����n�����I");
			project_hack2(GF_SOUND,0,0,base);
			aggravate_monsters(0,FALSE);
			break;
		}
	case 5://�����O��
		{
			int dice = plev/5 +5;
			if(only_info) return  format("����:%dd%d/�^�[��",dice,dice);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_KYOUKO_NENBUTSU, SPELL_CAST);

		}
		break;
	case 6://�v���C�}���X�N���[��
		{
			int dice = plev * 2;
			if(only_info) return  format("����:1d%d/�^�[��",dice);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_KYOUKO_SCREAM, SPELL_CAST);
		}
		break;
	case 7://�`���[�W�h���b�z�[
		{
			int dice = p_ptr->lev / 2;
			int sides = 20 + chr_adj / 2;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			stop_singing();
			msg_format("�S�g�S��̋��т�������I");
			fire_spark(GF_SOUND,dir, damroll(dice,sides),1);
			aggravate_monsters(0,FALSE);

			break;
		}



	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���Ƃ�p���Z*/
class_power_type class_power_satori[] =
{
	{1,0,0,FALSE,TRUE,A_INT,0,0,"�ǐS",
		"���E���̓G�ɂ��Ē������A����ɓG�̎����Z���L������B�L�����Ă�����Z�͓ǐS�����邽�тɏ����������A�t���A���o��ƃ��Z�b�g�����B�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ��Ȃ��B"},
	{1,0,0,FALSE,TRUE,A_INT,0,0,"�z�N",
		"�ǐS�ŋL�����������X�^�[�̖��@��Z���Č�����B�З͂̓v���C���[�̔\�͂Ɉˑ����A�Č�������̂ɂ���Փx�����MP���ω�����B"},
	{16,20,50,FALSE,TRUE,A_WIS,0,10,"�e���u���X�[���j�[��",
		"�^�[�Q�b�g�̃g���E�}��P��o���ċ��͂Ȑ��_�U�����s���B�A���f�b�h�Ɍ����₷���A���j�[�N�����X�^�[��͋����G�ɂ͌����ɂ����A�ʏ�̐��_�������Ȃ��G�ɂ͌����Ȃ��B���x��35�ȍ~�͎��E���U���ɂȂ�B"},

	{20,10,55,FALSE,TRUE,A_INT,0,0,"�c���v�O���",
		"�A�C�e���̑�܂��ȉ��l�𔻒肷��B���x��30�ȏ�Œʏ�̊Ӓ�Ɠ������ʂɂȂ�B" },

	{32,50,70,FALSE,TRUE,A_INT,0,0,"���|�Ö��p",
		"���|���Ă���אڃ����X�^�[��̂������I�ɔz���ɂ���B�ʏ�̐��_�������Ȃ������X�^�[�ƃN�G�X�g�œ|�Ώۃ����X�^�[�ɂ͌��ʂ��Ȃ��B���j�[�N�����X�^�[��p�����[�^������ȃ����X�^�[�͂��̃t���A�݂̂̔z���ɂȂ�B" },

	{40,72,100,FALSE,TRUE,A_CHR,0,0,"�u���C���t�B���K�[�v�����g",
		"���E���S�Ăɐ��_�U�����s���B����̗̑͂������Ă���قǌ��ʂ������B�A���f�b�h�Ɍ����₷���A���j�[�N�����X�^�[��͋����G�ɂ͌����ɂ����A�ʏ�̐��_�������Ȃ��G�ɂ͌����Ȃ��B" },

		{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_satori(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("");

			satori_reading = TRUE;
			probing(); 
			satori_reading = FALSE;
			make_magic_list_satori(FALSE);


			break;
		}
	case 1:
		{
			if(only_info) return "";

			if(!cast_monspell_new()) return NULL;
			break;
		}
	case 2:
		{
			int dam = plev * 3 + chr_adj * 5;
			if(dam < 100) dam = 100;
			if(only_info) return format("����:%d",dam);

			if(plev < 35 && !use_itemcard)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
					return NULL;
				}
				msg_print("���Ȃ��̑�O�̊Ⴊ�d��������������E�E");
				fire_ball_hide(GF_SATORI,dir,dam,0);
			}
			else
			{
				if(!use_itemcard) 
					msg_print("���Ȃ��̑�O�̊Ⴊ�d��������������E�E");
				else 
					msg_print("�J�[�h�ɕ`���ꂽ�ڂ��d��������������E�E");
				project_hack2(GF_SATORI,0,0,dam);
			}


			break;
		}

	case 3: //v1.1.84 �c���v�O���
	{

		if (only_info) return format(" ");

		if (plev < 30)
		{
			if (!psychometry()) return NULL;
		}
		else
		{
			if (!ident_spell(FALSE)) return NULL;
		}

	}
	break;


	case 4://���|�Ö��p(�A�u�\�����[�g���[�U�[���班������)
	{
		int y, x;
		int i;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format(" ");

		if (p_ptr->inside_arena)
		{
			msg_print("�����̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			int tim;
			bool flag_success = FALSE;
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			//�N����
			set_monster_csleep(cave[y][x].m_idx, 0);

			if (is_pet(m_ptr))
			{
				msg_format("%s�͂��łɂ��Ȃ��̉��l���B", m_name);
			}
			else if ((r_ptr->flags1 & RF1_QUESTOR) || (r_ptr->flagsr & RFR_RES_ALL) || r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				msg_format("%s�ɂ͌��ʂ��Ȃ������I", m_name);
			}
			else if (!MON_MONFEAR(m_ptr))
			{
				msg_format("%s�̐S�ɂ͌����Ȃ��B�����Ƌ��|��^����K�v������B", m_name);
			}
			else
			{
				msg_format("��O�̖ڂ�%s�������ƌ��߂�...", m_name);
				set_pet(m_ptr);

				//���j�[�N��σp�����[�^�����X�^�[�̓t���A����o��Ə����邱�Ƃɂ��Ă���
				if (r_ptr->flags7 & RF7_VARIABLE || r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) 
					m_ptr->mflag |= (MFLAG_EPHEMERA);
				msg_format("���Ȃ���%s�̐��_���x�z�����B", m_name);
			}
		}
		else
		{
			msg_format("�����ɂ͉������Ȃ��B");
			return NULL;
		}
	}
	break;

	case 5:
	{
		if (only_info) return format("����:�s��");

		msg_print("���Ȃ��̑�O�̖ڂ����X�ƋP�����B");
		project_hack2(GF_BRAIN_FINGER_PRINT, 0, 0, 1);

		break;
	}



	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}





/*:::����t�̖��͐H��*/
class_power_type class_power_jeweler[] =
{
	{25,1,60,FALSE,TRUE,A_INT,0,0,"���͐H��(���)",
		"��΂��疂�͂��z�����MP���񕜂���B�z�����閂�̗͂ʂ͕�΂̎�ނŕς��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_jeweler(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{

			if(only_info) return format("");

			if (!eat_jewel()) return NULL;
		}
		break;

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}






/*:::�Ó����p�Z*/
class_power_type class_power_sh_dealer[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"������[",
		"���̓���ɃA�C�e��������B5��ނ܂ł̃A�C�e����������B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"����m�F",
		"����̒����m�F����B�s����������Ȃ��B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�����o��",
		"�������A�C�e�����o���B"},
	{5,7,40,FALSE,FALSE,A_INT,0,0,"�ڗ���",
		"�A�C�e�����Ӓ肷��B���x��30�Ŋ��S�ȊӒ���s����B"},
	{20,30,60,FALSE,FALSE,A_INT,0,0,"�@��o�������m",
		"���̃t���A�ɃG�S�A�C�e���E�A�[�e�B�t�@�N�g�E�f�ށE���i�Ȃǂ������Ă����炻����܂��Ɏ@�m�ł���B"},
	{40,50,75,FALSE,FALSE,A_INT,0,0,"�^�E�ڗ���",
		"�߂��ɗ����Ă���A�C�e����S�ĊӒ肵�A�����ȃA�C�e������ʂȑ����i�����|�[�g����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_sh_dealer(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			display_list_inven2();
			return NULL; //���邾���Ȃ̂ōs��������Ȃ�

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if(plev < 30)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
			else
			{
				if (!identify_fully(FALSE)) return NULL;
			}

		}
		break;

	case 4:
		{
			if(only_info) return format("");
			msg_print("���Ȃ��͎��͂�O�O�ɒ��׎n�߂��E�E");
			search_specific_object(4);
		}
		break;

	case 5:
		{
			int i;
			int count_ego = 0;
			int count_art = 0;
			int count_valuable = 0;
			bool no_use = TRUE;
			int value_thre = 100000;

			int rad = 4;
			if(plev > 44) rad = 5;
			if(only_info) return format("�͈�:%d",rad);
			msg_print("���Ȃ��̊Ⴊ�L�����ƌ������I");
			for (i = 1; i < o_max; i++)
			{
				object_type *o_ptr = &o_list[i];
				if (!o_ptr->k_idx) continue;
				if (o_ptr->held_m_idx) continue;
				if(object_is_known(o_ptr) && object_value_real(o_ptr) < value_thre) continue;
				if(distance(py,px,o_ptr->iy,o_ptr->ix) > rad || !los(py,px,o_ptr->iy,o_ptr->ix)) continue;
				identify_item(o_ptr);
				no_use = FALSE;
				if(object_is_ego(o_ptr)) count_ego++;
				else if(object_is_artifact(o_ptr)) count_art++;
				else if(object_value_real(o_ptr) >= value_thre) count_valuable++;
			}

			if(no_use)	msg_print("�E�E�������ڐV�������̂͂Ȃ������B");

			if(count_art)
			{
				msg_format("����͂܂������`���̕i���I");
				if(count_art > 1) msg_format("������%d������I",count_art);
			}
			if(count_ego)
			{
				if(count_art) msg_format("����ɁA");
				if(count_ego > 1) msg_format("�����ȕi��%d�قǂ���悤���B",count_ego);
				else msg_format("�����ȕi������悤���B");
			}
			if(count_valuable)
			{
				if(count_art || count_ego) msg_format("���ɂ��A");
				if(count_valuable > 1) msg_format("�ɂ߂ċM�d�ȕi��%d�قǂ���悤���B",count_valuable);
				else msg_format("�ɂ߂ċM�d�ȕi������悤���B");
			}

			fix_floor_item_list(py,px);

		}
		break;


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::���җp���Z*/
class_power_type class_power_paladin[] =
{
	{5,5,15,FALSE,FALSE,A_DEX,0,0,"���}�蓖",
		"�łƐ؂菝�����Â��AHP��������x�񕜂���B"},
	{10,10,36,FALSE,FALSE,A_INT,0,0,"���G",
		"���͂̃����X�^�[�����m����B"},
	{17,18,35,FALSE,FALSE,A_CHR,0,0,"����",
		"�G��������Ĕz���ɂ���B���א��͂̓G�A�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ��Ȃ��B"},
	{24,25,40,FALSE,TRUE,A_WIS,0,0,"����",
		"��莞�ԏj���𓾂�B���x�����㏸����Ƒω΁A�ϗ�A�ώ׈����E���t�������B"},
	{30,30,60,FALSE,TRUE,A_WIS,0,0,"�z�[���[�����X",
		"�j�ב����̃r�[������B"},
	{34,24,55,FALSE,FALSE,A_INT,0,0,"����",
		"�אڂ��������̂��郂���X�^�[��HP��10�`15%�񕜂�����B"},
	{40,40,70,FALSE,TRUE,A_INT,0,0,"���E",
		"�����̂���ꏊ�ɁA�����X�^�[���U�����ė����菢�����ꂽ�肵�ɂ����Ȃ郋�[����`���B"},
	{47,80,75,FALSE,TRUE,A_WIS,0,0,"�h��",
		"��莞��AC�Ɩ��@�h�䂪�㏸���A����ɂ��Ȃ����󂯂�قڑS�Ẵ_���[�W��2/3�ɂ���B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_paladin(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dice = 3 + plev / 5;
			int sides = 6;
			if(only_info) return format("��:%dd%d",dice,sides);
			set_cut((p_ptr->cut / 2) - 50);
			set_poisoned(0);
			hp_player(damroll(dice,sides));
			break;
		}
	case 1:
		{
			if(only_info) return "";
			msg_format("�ӂ�̋C�z�ɋC��z�����E�E");
			detect_monsters_normal(DETECT_RAD_DEFAULT);
			detect_monsters_invis(DETECT_RAD_DEFAULT);
			break;
		}
	case 2: //����
		{
			int dam = plev * 2 / 3;
			int i;
			bool flag = FALSE;

			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			if(buki_motteruka(INVEN_RARM) || buki_motteruka(INVEN_LARM)) msg_format("���Ȃ��͕�������낵�A�b���|�����E�E");
			else msg_format("���Ȃ��͍\���������A�b���|�����E�E");
			fire_ball_hide(GF_CHARM_PALADIN,dir,dam,0);
			break;

		}
	case 3: //����
		{
			int base = p_ptr->lev/2;
			int sides = p_ptr->lev/2;
			int v;
			if(only_info) return format("����:%d+1d%d",base,sides);
			msg_print("���_���W�������E�E");
			v = damroll(base,sides);

			set_blessed(v, FALSE);
			if(plev > 29) set_oppose_fire(v, FALSE);
			if(plev > 34) set_oppose_cold(v, FALSE);
			if(plev > 39) set_protevil(v, FALSE);

			break;
		}

	case 4: //�z�[���[�����X
		{
			int dam = plev * 3;
			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���̑��𐶂ݏo���A���������I");
			fire_beam(GF_HOLY_FIRE,dir, dam);
			break;
		}

	case 5: //����
		{
			int y, x;
			int dist;
			int damage;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int heal;
			char m_name[80];	

			if(only_info) return "";
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];

			if(!cave[y][x].m_idx || !m_list[cave[y][x].m_idx].ml)
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			m_ptr = &m_list[cave[y][x].m_idx];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
			heal = m_ptr->maxhp * (10+randint0(6)) / 100;
			if(heal < 1) heal = 1;
			if(monster_living(r_ptr))
			{
				m_ptr->hp += heal;
				if(m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				set_monster_stunned(cave[y][x].m_idx,0);
				msg_format("%^s�̏������Â����B", m_name);
				p_ptr->redraw |= (PR_HEALTH);
			}
			else
				msg_format("%^s�̏������Â��邱�Ƃ͂ł��Ȃ������B", m_name);
			break;
		}
	case 6: //���E
		{
			if(only_info) return "";
			msg_print("���Ȃ��͑����ɖ�l��`���n�߂��E�E");
			warding_glyph();
			break;
		}
	case 7: //�h��
		{
			int base = p_ptr->lev/3;
			if(only_info) return format("����:%d+1d%d",base,base);
			msg_print("�h�ǂ𒣂����I");
			set_magicdef(randint1(base) + base, FALSE);

			break;
		}


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}









/*:::�H�X�q�p���Z*/
class_power_type class_power_yuyuko[] =
{
	{6,7,20,FALSE,FALSE,A_CHR,0,2,"�U��̊Ö�",
		"�w�肵���A���f�b�h�����m���Ŗ������Ĕz���ɂ���B"},
	{11,12,30,FALSE,TRUE,A_CHR,0,7,"�S�[�X�g�o�^�t���C",
		"�����𒆐S�Ƃ���n�������̋���ȃ{�[������B���x�����オ��ƌ��ʔ͈͂��L����B"},
	{17,30,35,FALSE,TRUE,A_CHR,0,5,"�S�[�X�g�X�|�b�g",
		"�w�肵���^�[�Q�b�g�̎��ӂɑ�ʂ̗H��n�����X�^�[���Ăяo���B"},
	{20,24,40,FALSE,TRUE,A_CHR,0,3,"�M���X�g���h���[��",
		"�אڂ����G��̂ɑ΂��A����Ȑ��_�U�����d�|����B���j�[�N�����X�^�[�E�f�[�����E�A���f�b�h�ɂ͌����Â炭���C�������炷�G�ɂ͌����Ȃ��B"},
	{23,27,50,FALSE,TRUE,A_CHR,0,0,"�َ����ւ̖񑩎�`",
		"�G��̂����m���Ō��݂̃t���A����ǂ������B�ǂ��������G�̃��x���ɂ�菭���_���[�W���󂯂�B���j�[�N�����X�^�[�ɂ͌����Ȃ��B"},
	{28,24,55,FALSE,TRUE,A_CHR,0,5,"�P����̎���",
		"�n�������̋��͂ȃr�[������B"},
	{32,48,60,FALSE,TRUE,A_CHR,0,3,"���ҕK�ł̗�",
		"�G��̂����m���őS�\�͒ቺ������B��R�����Ɩ����B" },
	{36,50,65,FALSE,TRUE,A_CHR,0,0,"����̉i��",
		"���͂̓G���t���A�����������B��R����邱�Ƃ�����A���j�[�N�����X�^�[�ɂ͌����Ȃ��B�ǂ��������G�̃��x���ɂ�菭���_���[�W���󂯂�B"},
	{40,60,75,FALSE,TRUE,A_CHR,0,0,"������n��", //v1.1.76
		"�����𒆐S�ɖ������̋���ȃ{�[���𔭐�������B" },
	{43,64,80,FALSE,TRUE,A_CHR,0,0,"���o�̗U�铔",
		"�Z���ԁA���E���̓G�̖��@�g�p���m���ŖW�Q����B���j�[�N�����X�^�[�⍂���x���ȓG�ɂ͌����ɂ����B"},

	{48,100,75,FALSE,TRUE,A_CHR,0,0,"���s�����]����",
		"�Z�g�p��A�S�Ă̑ϐ����l�����A�S�Ă̍U���̃_���[�W�𔼌����A�G����̗אڍU���ɑ΂��n�������̋��͂Ȕ������s���A���^�[�����E���ɗl�X�ȑ����̋��͂ȍU�����s���B�^�[���o�ߎ���MP��50�����B�g�p��ɑҋ@�ȊO�̍s��������ƌ��ʂ��؂��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};


cptr do_cmd_class_power_aux_yuyuko(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int power = 50 + plev * 2 + chr_adj*5;
			if(only_info) return format("����:%d",power);
			if (!get_aim_dir(&dir)) return NULL;
			control_one_undead(dir, power);
		}
		break;

	case 1: //�S�[�X�g�o�^�t���C
		{
			int rad = 3 + plev / 10;
			damage = 50 + plev*4 + chr_adj * 5;
			if(only_info) return format("����:�ő�%d �͈�:%d",damage/2,rad);
			msg_print("�d�������钱�����������x�����B");
			project(0, rad, py, px, damage, GF_NETHER, PROJECT_KILL, -1);

			break;
		}
	case 2: //�S�[�X�g�X�|�b�g
		{
			int num = 5 + plev / 10;
			int i;
			bool flag = FALSE;

			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			for(i=0;i<num;i++)
			{
				if(summon_specific(-1, target_row, target_col, plev, SUMMON_GHOST, (PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;
			}
			if(flag) msg_format("�H����Ă񂾁B");
			else msg_format("�H��͌���Ȃ������E�E");

		}
		break;


	case 3: //�M���X�g���h���[�� �ϐ��Ƃ���R�����Ƃ��������H
		{
			int x,y;
			dice = 1;
			sides = chr_adj * 20 + plev*3 +50;
			if(sides < 200) sides = 200;
			if(only_info) return format("����:1d%d",sides);

			if (!get_rep_dir2(&dir)) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];
			if(!in_bounds(y,x)) return NULL;

			if (cave[y][x].m_idx)
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				if(!use_itemcard) msg_format("���Ȃ���%s�Ɍ����Ďw�����邭��񂵂��B",m_name);
				project(0, 0, y, x, damroll(dice,sides) , GF_COSMIC_HORROR, flg, -1);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			break;

		}

	case 4://�َ����ւ̖񑩎�`
		{
			int power = 50 + plev * 4 + chr_adj*5;
			if(only_info) return format("����:%d",power);
			if (!get_aim_dir(&dir)) return NULL;
			fire_ball_hide(GF_GENOCIDE, dir, power, 0);

		}
		break;
	case 5: //�P����̎���
		{
			int num = 2;
			dice = 10 + p_ptr->lev / 5;
			sides = 5 + chr_adj / 2;

			if(dice < 15) dice = 15;

			if(only_info) return format("����:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("����Ȑ�����̌����������ꂽ�I");
			for (i = 0; i < num; i++) if(dir != 5 || target_okay()) fire_beam(GF_NETHER, dir, damroll(dice, sides));
			break;
		}

	case 6:
		{
			int power = plev * 5 + chr_adj * 5;

			if (only_info) return format("����:%d", power);

			if (!get_aim_dir(&dir)) return NULL;
			if (!fire_ball_jump(GF_DEC_ALL, dir, power, 0, "���Ȃ��̎w������ɑ�ʂ̎����ƗH�삪�����񂹂��I")) return NULL;

			break;
		}



	case 7://����̉i��
		{
			int power = 50 + plev * 2 + chr_adj*5;
			if(power < 200) power = 200;

			if(only_info) return format("����:%d",power);
			msg_format("�����������͂֎U�����E�E");
			mass_genocide(power, TRUE);
		}
		break;

	case 8: //������n�� v1.1.76
		{

		int rad = 7;
		base = plev * 10 + chr_adj * 20;
		if (only_info) return format("����:�`%d", base / 2);

		msg_format("���̂悤�ȍ����Ⴊ�����N�������B");
		project(0, rad, py, px, base, GF_DISP_ALL, PROJECT_KILL | PROJECT_JUMP, -1);

		}
		break;

	case 9://���o�̗U�铔 v1.1.76 �d�l�ύX
	{
		int time;
		int base = plev / 8;

		if (only_info) return format("����:%d+1d%d", base, base);
		time = base + randint1(base);
		set_tim_general(time, FALSE, 0, 0);

	}
	break;


/*
	case 8://���o�̗U�铔 �G��NICE�t���O��t�^����
		{
			int j;
			if(only_info) return format("");

			msg_format("�d�������������ꂽ�E�E");
			for (j = 1; j < m_max; j++)
			{
				int tx,ty;
				int chance;
				char m_name[80];
				monster_type *m_ptr;
				monster_race *r_ptr;

				m_ptr = &m_list[j];
				if(!m_ptr->r_idx) continue;
				if(!los(py,px,m_ptr->fy,m_ptr->fx)) continue;
				if(is_pet(m_ptr)) continue;
				monster_desc(m_name, m_ptr, 0);
				r_ptr = &r_info[m_ptr->r_idx];
				chance = plev * 3 + chr_adj * 5;
				if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) chance /= 2;

				if(r_ptr->level < randint1(chance))
				{
					msg_format("%s�͌��ɗU��ꂽ�B",m_name);
					m_ptr->mflag |= (MFLAG_SPECIAL);
				}
				else if(!(m_ptr->mflag & MFLAG_SPECIAL)) msg_format("%s�͘f�킳��Ȃ������B",m_name);
			}
			new_class_power_change_energy_need = 25;
		}
		break;
		*/


	//���s�����]���� v1.1.76�Ŏd�l�ύX
	//���z�̌^+���^�[�����E���U��
	case 10:
	{
		if (only_info) return format("");
		set_action(ACTION_KATA);
		p_ptr->special_defense |= KATA_MUSOU;
		p_ptr->update |= (PU_BONUS);
		p_ptr->update |= (PU_MONSTERS);
		msg_format("���Ȃ��̔w��ɋ���Ȑ�J����...");
		yuyuko_nehan();

	}
	break;
	/*
	case 9:
		{
			base = (plev * 4 + chr_adj * 10) * p_ptr->csp / 100;

			if(only_info) return format("����:%d",base);
			msg_format("���Ȃ��͑S�Ă̗͂������������I");
			project_hack2(GF_DISP_ALL,0,0,base);
			new_class_power_change_energy_need = 200;
			p_ptr->csp = 1;
			break;
		}
		*/

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}












/*:::�R���p���Z*/
class_power_type class_power_cavalry[] =
{
	{10,0,25,FALSE,FALSE,A_STR,0,0,"�r�n��炵",
		"�G�����X�^�[�ɔ�я���Ė����������A�z���ɂ���B��n�s�\�ȃ����X�^�[�ɂ͖����B�GHP�������Ă���قǌ����₷���B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_cavalry(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			char m_name[80];
			monster_type *m_ptr;
			monster_race *r_ptr;
			int rlev;

			if(only_info) return format("");
			if (p_ptr->inside_arena)
			{
				msg_print("�����̓��Z�͎g���Ȃ��B");
				return NULL;
			}

			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("���͏�n�����B");
#else
				msg_print("You ARE riding.");
#endif
				return NULL;
			}
			if (!do_riding(TRUE)) return "";
			m_ptr = &m_list[p_ptr->riding];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%s�ɏ�����B",m_name);
#else
			msg_format("You ride on %s.",m_name);
#endif
			if (is_pet(m_ptr)) break;
			rlev = r_ptr->level;
			if (r_ptr->flags1 & RF1_UNIQUE) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			if ((randint1(ref_skill_exp(SKILL_RIDING) / 120 + p_ptr->lev * 2 / 3) > rlev)
				&& one_in_(2) && !p_ptr->inside_arena && !p_ptr->inside_battle
			    && !(r_ptr->flags7 & (RF7_GUARDIAN)) && !(r_ptr->flags1 & (RF1_QUESTOR))
//			    && (rlev < p_ptr->lev * 3 / 2 + randint0(p_ptr->lev / 5)))
			    && (m_ptr->hp < (ref_skill_exp(SKILL_RIDING) / 100 * p_ptr->lev) ))
			{
#ifdef JP
				msg_format("%s����Ȃ������B",m_name);
#else
				msg_format("You tame %s.",m_name);
#endif
				if(rlev > randint1(plev)) set_deity_bias(DBIAS_WARLIKE, -2);
				set_pet(m_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%s�ɐU�藎�Ƃ��ꂽ�I",m_name);
#else
				msg_format("You have thrown off by %s.",m_name);
#endif
				rakuba(1,TRUE);

				/* Paranoia */
				/* ���n�����Ɏ��s���Ă��Ƃɂ�����n���� */
				p_ptr->riding = 0;
			}
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::���p�Ɨp���Z*/
class_power_type class_power_samurai[] =
{
	{1,0,0,FALSE,FALSE,A_WIS,0,0,"�C��������",
		"�ꎞ�I��MP��啝�ɑ���������B"},
	{25,0,0,FALSE,FALSE,A_DEX,0,0,"�^���\����",
		"����ȍ\�����Ƃ��Đ키�B�ϐ���X�e�[�^�X�Ȃǂ��ω�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_samurai(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			int max_csp = MAX(p_ptr->msp*4, p_ptr->lev*5+5);
			if (max_csp > 255) max_csp = 255;
			if(only_info) return format("�ő�:%d",max_csp);

			if (total_friends)
			{
				msg_print("���͔z���̂��ƂɏW�����Ă��Ȃ��ƁB");
				return NULL;
			}
			if (p_ptr->special_defense & KATA_MASK)
			{
				msg_print("���͍\���ɏW�����Ă���B");
				return NULL;
			}
			msg_print("���_���W�����ċC�����𗭂߂��B");

			p_ptr->csp += p_ptr->msp / 2;
			if (p_ptr->csp >= max_csp)
			{
				p_ptr->csp = max_csp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);

			break;
		}
	case 1:
		{
			if(only_info) return format("");
			if (!choose_samurai_kata()) return NULL;
			p_ptr->update |= (PU_BONUS);
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���f�B�X���p���Z*/
class_power_type class_power_medi[] =
{
	{4,10,20,FALSE,TRUE,A_DEX,0,0,"�Ŗ򐶐�",
		"�ł̖�����B�Ԕ����ł̏��łȂ��Ǝg���Ȃ��B"},
	{8,8,30,FALSE,TRUE,A_INT,0,0,"�_�o�̓�",
		"�אڂ����G�����m���Ŗ��点�čU���͒ቺ��Ԃɂ���B�����ϐ��̂���G�A�őϐ��̂���G�A�������ɂ͌����Ȃ��B���j�[�N�����X�^�[��͋����G�ɂ͌��ʂ������B"},
	{12,12,45,TRUE,FALSE,A_CON,0,10,"�|�C�Y���u���X",
		"����HP��1/3�̈З͂̓ł̃u���X��f���B�����x����������B"},
	{16,12,50,FALSE,TRUE,A_INT,0,0,"�J�T�̓�",
		"�אڂ����G�����m���Ō����������@�͒ቺ��Ԃɂ���B�őϐ��̂���G�A�ʏ�̐��_�������Ȃ��G�A�������ɂ͌����Ȃ��B���j�[�N�����X�^�[��͋����G�ɂ͌��ʂ������B"},
	{20,30,65,FALSE,TRUE,A_CHR,0,0,"枖ς̓�",
		"�אڂ����G�����m���ő����Ĕz���ɂ���B�������A�őϐ��̂���G�A�ʏ�̐��_�������Ȃ��G�A���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	{25,25,45,FALSE,FALSE,A_WIS,0,10,"�K�V���O�K�[�f��",
		"���E���̑S�Ăɑ΂��ő����̍U������B"},
	{32,50,75,FALSE,TRUE,A_DEX,0,0,"�ғŐ���",
		"�ғł̖�����B�ғł̕�����������B"},
	{40,80,70,FALSE,FALSE,A_CHR,0,0,"�C���g�D�f�����E��",
		"���E���̑S�Ăɑ΂����͂Ȑ��_�U�������̍U�����s���N�O�E�����E����������B���_�̊󔖂ȓG�A���C�������炷�G�A���j�[�N�����X�^�[�ɂ͌��ʂ������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

/*:::���f�B�X���ғŐ����p*/
bool item_tester_hook_make_venom(object_type *o_ptr)
{
	if(o_ptr->tval == TV_MATERIAL && (o_ptr->sval == SV_MATERIAL_ARSENIC || o_ptr->sval == SV_MATERIAL_MERCURY || o_ptr->sval == SV_MATERIAL_GELSEMIUM)) return TRUE;
	if(o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DOKUBARI && !object_is_artifact(o_ptr)) return TRUE;
	if(object_is_melee_weapon(o_ptr) && o_ptr->name2 == EGO_WEAPON_BRANDPOIS) return TRUE;
	return FALSE;
}


cptr do_cmd_class_power_aux_medi(int num, bool only_info)
{
	int dir, i, dam;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return format("");			
			if(cave[py][px].feat != 93 && !cave_have_flag_bold(py,px,FF_POISON_PUDDLE)) //�Ԓn�` v1.1.85 �ŏ����ǉ�
			{
				msg_format("�����ɂ͓łɂȂ肻���Ȃ��̂���������Ȃ��B");
				break;
			}
			msg_format("�u�R���p���R���p���A�ł�W�܂�`�v");
			object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_POISON ));
			drop_near(q_ptr, -1, py, px);

		}
		break;
	case 1:
	case 3:
	case 4:
		{

			int y, x;
			int dist;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int power;
			if(num == 4) 
				power = plev * 3 + 30;
			else 
				power = plev * 4 + 50;

			if(only_info) return  format("����:%d",power);

			if (num == 4 && p_ptr->inside_arena)
			{
				msg_print("�����̓��Z�͎g���Ȃ��B");
				return NULL;
			}


			if (!get_rep_dir2(&dir)) return NULL;
			if(dir==5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(num == 1)
				{
					msg_format("%s�Ɍ������Đ_�o�ł�U��܂����I",m_name);
					if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) power /= 3;
					if((r_ptr->flags2 & RF2_POWERFUL)) power /= 2;

					if((r_ptr->flagsr & RFR_IM_POIS)  || (r_ptr->flagsr & RFR_RES_ALL))
					{
						msg_format("���������ʂ��Ȃ������B");
						if (r_ptr->flagsr & RFR_IM_POIS) r_ptr->r_flagsr |= (RFR_IM_POIS);
					}
					else if(!monster_living(r_ptr)) msg_format("%s�͖���Ȃ��悤���B",m_name);
					else if(randint1(power) < r_ptr->level) msg_format("%s�͓łɒ�R�����I",m_name);
					else
					{
						if (r_ptr->flags3 & RF3_NO_SLEEP)
						{
							r_ptr->r_flags3 |= (RF3_NO_SLEEP);
							msg_format("%s�͖��炳��Ȃ������B", m_name);
						}
						else
						{
							msg_format("%s�͖��荞�񂾁B", m_name);
							(void)set_monster_csleep(cave[y][x].m_idx, power);
						}

						if (set_monster_timed_status_add(MTIMED2_DEC_ATK, cave[y][x].m_idx, MON_DEC_ATK(m_ptr) + 8 + randint1(8)))	
							msg_format("%^s�͍U���͂����������悤���B", m_name);

					}
				}
				else if(num == 3)
				{
					msg_format("%s�Ɍ������ėJ�T�̓ł�U��܂����I",m_name);
					if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) power /= 3;
					if((r_ptr->flags2 & RF2_POWERFUL)) power /= 2;

					if((r_ptr->flagsr & RFR_IM_POIS) || (r_ptr->flags2 & RF2_EMPTY_MIND) || (r_ptr->flags2 & RF2_WEIRD_MIND) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						msg_format("���������ʂ��Ȃ������B");
						if (r_ptr->flagsr & RFR_IM_POIS) r_ptr->r_flagsr |= (RFR_IM_POIS);
					}
					else if(!monster_living(r_ptr)) msg_format("%s�͓����Ȃ��悤���B",m_name);
					else if(randint1(power) < r_ptr->level) msg_format("%s�͓łɒ�R�����I",m_name);
					else
					{
						if (set_monster_slow(cave[y][x].m_idx, MON_SLOW(m_ptr) + 8 + randint1(8))) msg_format("%s�̓������x���Ȃ����B",m_name);

						if (set_monster_timed_status_add(MTIMED2_DEC_MAG, cave[y][x].m_idx, MON_DEC_MAG(m_ptr) + 8 + randint1(8)))	
							msg_format("%^s�͖��@�͂����������悤���B", m_name);

					}
					anger_monster(m_ptr);
				}
				else
				{
					if(is_pet(m_ptr))
					{
						msg_format("%s�͂��łɔz�����B",m_name);
						break;
					}
					msg_format("%s�Ɍ�������枖ς̓ł�U��܂����I",m_name);
					if((r_ptr->flags2 & RF2_POWERFUL)) power = power * 2 / 3;

					if((r_ptr->flagsr & RFR_IM_POIS) || (r_ptr->flags2 & RF2_EMPTY_MIND) || (r_ptr->flags2 & RF2_WEIRD_MIND) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						msg_format("���������ʂ��Ȃ������B");
						if (r_ptr->flags3 & RF3_NO_SLEEP) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
						if (r_ptr->flagsr & RFR_IM_POIS) r_ptr->r_flagsr |= (RFR_IM_POIS);
					}
					else if(!monster_living(r_ptr) || (r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) ) 
						msg_format("%s�͓����Ȃ��悤���B",m_name);
					else if(randint1(power) < r_ptr->level) msg_format("%s�͓łɒ�R�����I",m_name);
					else
					{
						msg_format("%s�͂��Ȃ��ɏ]�����B",m_name);
						set_pet(m_ptr);
					}
					(void)set_monster_csleep(cave[y][x].m_idx, 0);
					anger_monster(m_ptr);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
		}
		break;
	case 2:
		{
			dam = p_ptr->chp / 3;
			if(dam<1) dam = 1;
			if(dam > 1600) dam=1600;

			if(only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;

			if(use_itemcard) 
				msg_print("�J�[�h����ŃK�X�������o�����B");
			else 
				msg_print("���Ȃ��͓ł̃u���X��f�����B");
		
			fire_ball(GF_POIS, dir, dam, -2);
			if(!use_itemcard) (void)set_food(p_ptr->food -1500);
			break;
		}


	case 5:
		{
			int dam = 10 + plev * 3 + chr_adj * 8;
			if(dam < 100) dam = 100;
			if(only_info) return format("����:%d",dam);
			msg_print("�����ɓŃK�X���[�������I");
			project_hack2(GF_POIS, 0,0,dam);
		}
		break;
	case 6:
		{
			int         item;
			object_type forge, *q_ptr = &forge;
			cptr        q, s;
			if(only_info) return format("");

			item_tester_hook = item_tester_hook_make_venom;
			q = "�ǂꂩ��ł𒊏o���܂���? ";
			s = "�ł𒊏o�ł������Ȃ��̂���������Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;
			msg_format("�u�R���p���R���p���A�ł�W�܂�`�v");
			object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_POISON2 ));
			drop_near(q_ptr, -1, py, px);

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
		}
		break;
	case 7:
		{
			int dam = plev * 4 + chr_adj * 15;
			if(only_info) return format("����:%d",dam);
			msg_print("�����̓ł��[�������I");
			project_hack2(GF_REDEYE, 0,0,dam);
		}
		break;


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::����p�Z*/
class_power_type class_power_shou[] =
{
	{5,9,20,FALSE,TRUE,A_WIS,0,2,"���[�U�[",
		"�^�[�Q�b�g���ӂɑM�������̃r�[���𐔔����B�u������V�̕󓃁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{14,20,40,TRUE,FALSE,A_STR,0,5,"�n���O���[�^�C�K�[",
		"���O���b�h���꒼���ɓːi���A�����ȓG�͐�����΂��B���x�����オ��Ǝ˒����L�т�B"},
	{21,24,50,FALSE,TRUE,A_WIS,0,2,"���C�f�B�A���g�g���W���[",
		"��΂𐶂ݏo�����������[�U�[����B�u������V�̕󓃁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{26,30,55,FALSE,TRUE,A_CHR,0,0,"�����̐k�",
		"�����𖾂邭���A���E���̃����X�^�[�������E�N�O�����A����Ƀ��x��40�ȍ~�͖������悤�Ǝ��݂�B�u������V�̕󓃁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{32,30,60,FALSE,TRUE,A_WIS,0,10,"�A�u�\�����[�g�W���X�e�B�X",
		"�j�ב����̋��͂ȃ��[�U�[����B�u������V�̕󓃁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{36,50,70,FALSE,TRUE,A_CHR,0,0,"���`�̈Ќ�",
		"�ꎞ�I�ɏj���E���Ȃ�I�[���E�Ύ׈����E�𓾂�B�u������V�̕󓃁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{40,75,75,FALSE,TRUE,A_WIS,0,0,"���C�f�B�A���g�g���W���[�K��",
		"��΂𐶂ݏo�����͂Ȗ�������^���[�U�[����B�u������V�̕󓃁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{47,160,80,FALSE,TRUE,A_WIS,0,0,"�򉻂̖�",
		"���E���S�Ăɋ��͂Ȕj�ב����U�����s���B�u������V�̕󓃁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

/*:::����p���Z*/
cptr do_cmd_class_power_aux_shou(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int wis_adj = adj_general[p_ptr->stat_ind[A_WIS]];

	switch(num)
	{
	case 0: //�ւɂ�背�[�U�[
		{
			int num = 3 + p_ptr->lev / 15;
			dice = 4 + p_ptr->lev / 20;
			sides = 5 + chr_adj / 10;
			if(only_info) return format("����:%dd%d * %d",dice,sides, num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�󓃂�������������ꂽ�I");
			fire_blast(GF_LITE, dir, dice, sides, num, 3,(PROJECT_BEAM | PROJECT_KILL));
			break;
		}
	case 1: //�n���O���[�^�C�K�[
		{
			int len = 2 + p_ptr->lev / 8;
			damage = plev + adj_general[p_ptr->stat_ind[A_STR]] *5;
			if(damage < 50) damage = 50;

			if(only_info) return format("�˒�:%d ����:%d",len,damage);
			if (!rush_attack3(len,"���Ȃ��͌������ːi�����I",damage)) return NULL;
			break;
		}
	case 2: //���C�f�B�A���g�g���W���[
		{
			damage = p_ptr->lev * 2 + chr_adj * 3 + wis_adj * 2;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�󓃂�ῂ��������I");
			fire_beam(GF_HOUTOU,dir,damage);

			break;
		}
	case 3: //v1.1.30�ǉ��@�����̐k�
		{
			int power = p_ptr->lev * 4;
			if (only_info) return format("����:%d",power);
			msg_format("�󓃂������̋P���������...");
			lite_room(py, px);
			stun_monsters(power);
			confuse_monsters(power);
			if(plev > 39)
				charm_monsters(power);
		}
		break;

	case 4://�A�u�\�����[�g�W���X�e�B�X
		{
			dice = p_ptr->lev / 3;
			sides = 5 + wis_adj / 5 + chr_adj / 5 ;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�󓃂���򉻂̌��������ꂽ�I");
			fire_spark(GF_HOLY_FIRE,dir, damroll(dice,sides),1);

			break;
		}

	case 5: //v1.1.30�ǉ��@���`�̈Ќ�
		{
			int t;
			base = 15;
			if(only_info) return format("����:%d+1d%d",base,base);
			t = base + randint1(base);
			msg_format("������V�̉����������I");
			set_tim_sh_holy(t,FALSE);
			set_protevil(t,FALSE);
			set_blessed(t,FALSE);
			break;
		}


	case 6: //���C�f�B�A���g�g���W���[�K��
		{
			damage = p_ptr->lev * 4 + chr_adj * 5 + wis_adj * 5;
			if(damage < 300) damage = 300;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("����̋P�������o�����I");
			fire_spark(GF_HOUTOU,dir,damage,2);

			break;
		}
	case 7: //�򉻂̖�
		{
			dice = 7 + p_ptr->lev / 2;
			sides = p_ptr->lev / 5 + chr_adj / 2 + wis_adj / 2;
			if(sides < 1) sides = 1;
			if(only_info) return format("����:%dd%d",dice,sides);
			msg_format("�򉻂̌������͂��Ă��s�������I");
			project_hack2(GF_HOLY_FIRE,dice,sides,0);
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


//�t�H�[�r�h�D���t���[�c�w��p�֐�
static bool item_tester_hook_forbidden_fruit(object_type *o_ptr)
{
	if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_FORBIDDEN_FRUIT) return TRUE;
	else return FALSE;
}



/*:::�t�����h�[���p���Z*/
class_power_type class_power_flan[] =
{

	{ 5,10,25,FALSE,FALSE,A_INT,0,0,"�E�C���m",
		"���͂̐��_���������X�^�[�����m����B" },
	{ 12,12,30,TRUE,FALSE,A_STR,0,10,"�P���x���X�N���b�p�[",
		"�����̗אڃO���b�h�S�ĂɍU������B�ߐڕ���𑕔����Ă��Ȃ��Ǝg���Ȃ��B" },
	{16,12,30,FALSE,TRUE,A_INT,0,6,"�X�^�[�{�E�u���C�N",
		"�����̎��ӂ̃����_���ȏꏊ�ɕ������覐Α����U�����s���B"},
	{ 20,30,30,FALSE,FALSE,A_DEX,0,0,"�啈ړ�",
		"��s���Ő��O���b�h���ړ�����B���E�O�ɂ��ړ��ł��邪�h�A��ʉ߂���Ƃ��ړ��\�������Z���Ȃ�B" },
	{24,18,50,FALSE,FALSE,A_DEX,0,0,"������Ƃ��ăh�J�[��",
		"�^�[�Q�b�g�Ƃ��̎���3+1d3�O���b�h��*�j��*����B�^�[�Q�b�g�͖����I�Ɏw�肵�Ȃ���΂Ȃ�Ȃ��B(25-���x��/2)%�̊m���őz��O�̑�j�󂪋N����B�n���N�G�X�g�_���W�����ł͎g���Ȃ��B"},
	{29,32,60,FALSE,TRUE,A_INT,0,8,"�J�^�f�B�I�v�g���b�N",
		"����ȋO���̃r�[����5�����B"},
	{32,64,70,FALSE,TRUE,A_DEX,0,0,"�t�H�[�I�u�A�J�C���h",
		"���g���O�̏o��������B���������g�͖��߂Ɋւ�炸���Ȃ���͈͍U���Ɋ������ށB"},
	{36,80,70,FALSE,TRUE,A_CHR,0,0,"�J�S���J�S��",
		"���E���S�Ăɖ������_���[�W��^���A����ɒZ���Ԉړ��֎~�ɂ���B" },
	{40,72,70,FALSE,TRUE,A_STR,0,0,"���[���@�e�C��",
		"�����̎��͂�n���̍��Α����ōU������B�З͕͂���U���͂Ɉˑ�����B"},
	{45,99,75,FALSE,TRUE,A_CHR,0,0,"495�N�̔g��",
		"�����̎��ӂ̃����_���ȏꏊ�ɋ��͂ȕ��𑮐��{�[���𕡐�����������B�̗͂����Ȃ��قǈЗ͂��オ��B"},
	{50,-100,95,FALSE,TRUE,A_WIS,0,0,"�X�J�[���b�g�j�q���e�B",
		"MP�S��(�Œ�100)�ƃA�C�e���u�t�H�[�r�h�D���t���[�c�v������A�ɂ߂ċ��͂Ȗ��\�����̃{�[���U�����s���B�u�t�H�[�r�h�D���t���[�c�v���g�킸�ɂ��̓��Z���g�p����Ɓu���������v�̌��ʂ���������B" },

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

/*:::�t�����h�[�����Z*/
cptr do_cmd_class_power_aux_flan(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0://v1.1.77 �E�C���m
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("�͈�:%d", rad);
		detect_monsters_mind(rad);
		break;
	}

	case 1: //�P���x���X�N���b�p�[
	{
		if (only_info) return format("");

		if (p_ptr->do_martialarts)
		{
			msg_print("�ߐڕ���𑕔����Ă��Ȃ��Ƃ��̓��Z�͎g���Ȃ��B");
			return NULL;
		}

		msg_print("���Ȃ��͑傫�������U��񂵂��I");
		whirlwind_attack(0);
	}
	break;

	case 2: //�X�^�[�{�E�u���C�N
		{
			int dam = plev*2 + chr_adj + 20;
			if(only_info) return format("����:%d*�s��",dam);
			cast_meteor(dam, 2,GF_METEOR);
		}
		break;

	case 3: //�啈ړ�
	{
		int x, y;
		int cost;
		int dist = 2 + plev / 8;
		if (only_info) return format("�ړ��R�X�g:%d", dist-1);
		if (!tgt_pt(&x, &y)) return NULL;

		if (!player_can_enter(cave[y][x].feat, 0) || !(cave[y][x].info & CAVE_KNOWN))
		{
			msg_print("�����ɂ͍s���Ȃ��B");
			return NULL;
		}
		forget_travel_flow();
		travel_flow(y, x);
		if (dist < travel.cost[py][px])
		{
			if (travel.cost[py][px] >= 9999)
				msg_print("�����ɂ͓����ʂ��Ă��Ȃ��B");
			else
				msg_print("�����͉�������B");
			return NULL;
		}

		msg_print("���Ȃ����啂ɕϐg���Ĕ�񂾁B");
		teleport_player_to(y, x, TELEPORT_NONMAGICAL);

		//�����ړ������鎞�ړ��Ɠ����悤�ɏ���s���͂���������
		if (p_ptr->speedster)
			new_class_power_change_energy_need = (75 - p_ptr->lev / 2);

		break;
	}

	case 4://������Ƃ��Ăǂ��[��
		{
			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			if(dir !=5 || !los(py,px,target_row,target_col))
			{
				msg_format("���E���̈�_���w�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}

			if(!use_itemcard)
				msg_format("���Ȃ��͕W�I���ɂ�Ō����������E�E");

			if( (25-plev/2) > randint0(100))
			{
				if (destroy_area(py, px, 15 + p_ptr->lev + randint0(11), FALSE,FALSE,FALSE))
				{
					msg_print("��������_���W�������̂��̂��󂵂Ă��܂����I");
				}
				else msg_print("�n�ʂ��傫���h�ꂽ�B");
			}
			else
			{
				if(destroy_area(target_row, target_col, 3+randint1(3),FALSE,FALSE,TRUE))
				{
					msg_print("�������N�������I");
				}
				else msg_print("�����ł͎g���Ȃ��B");

			}

		}
		break;

	case 5: //�J�^�f�B�I�v�g���b�N
		{
			int dam = plev * 2 + chr_adj * 2 ;
			int table[] = {1,2,3,6,9,8,7,4,1,2,3,6,9,8};
			int ty,tx,i,p;
			int flg = PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_OPT;

			if(only_info) return format("����:%d*5",dam);
			msg_format("���Ȃ��͌��e��������I");

			p = randint0(8);
			for(i=0;i<5;i++)
			{
				dir = table[p+i];
				tx = px + 99 * ddx[dir];
				ty = py + 99 * ddy[dir];

				project(0, 0, ty, tx, dam, GF_LITE, flg, -1);
			}
		}
		break;
	case 6: //�t�H�[�I�u�A�J�C���h
		{
			int max = 3;
			bool flag = FALSE;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("�ő�:%d",max);
	
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_FLAN_4) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max)
			{
				msg_format("����ȏ�Ăяo���Ȃ��B",num);
				return NULL;
			}
			for(i=0;i<(max-cnt);i++) if(summon_named_creature(0, py, px, MON_FLAN_4, PM_EPHEMERA)) flag = TRUE;

			if(flag && p_ptr->pclass != CLASS_FLAN)
				msg_print("��΂̂悤�ȉH�����z���S�̌��e�����ꂽ�E�E");
			else if(flag) 
				msg_print("��������Ƃ��Ȃ����Ȃ��̕��g�����ꂽ�B");
			else  
				msg_print("��������Ȃ������B");
		}
		break;

	case 7:
		{
			int base = plev + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if (only_info) return format("����:%d+1d%d", base, base);

			msg_print("���Ȃ��̖��͂��B�̂悤�ɕ����𖄂ߐs�������I");
			project_hack2(GF_MISSILE, 1, base, base);
			project_hack(GF_NO_MOVE, base / 20);

			break;
		}
	case 8: //���[���@�e�C��
		{

			int dam =  calc_weapon_dam(0) + calc_weapon_dam(1);
			int rad = 4;
			dam = dam * (250 + chr_adj * 5 ) / 100;

			if(use_itemcard) dam = 500 + plev*20;

			if(only_info) return format("����:�ő�%d",dam/2);
			msg_print("�Ж�̉������͂��Ă��������I");
			project(0, rad, py, px, dam, GF_HELL_FIRE, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}
		break;
	case 9: //495�N�̔g��
		{
			int dam = plev * 3 + chr_adj * 5;
			int rad = 2;
			int mult = 50 + 450 * (p_ptr->mhp - p_ptr->chp) / p_ptr->mhp;
			dam = dam * mult / 100;
			rad = rad * mult / 100;
			if(rad > 7) rad = 7;
			if(rad < 2) rad = 2;
			if(only_info) return format("������{�l:%d",dam);

			if (!cast_wrath_of_the_god(dam, rad, FALSE)) return NULL;
			break;
		}

	case 10: //�X�J�[���b�g�j�q���e�B
		{
			cptr q, s;
			int item;
			int dam = plev * chr_adj * p_ptr->csp / 100;
			bool have_fruit = FALSE;

			if (dam > 9999) dam = 9999;

			if (only_info) return format("����:%d", dam);

			q = "�G�}���w�肵�Ă�������:";
			s = "�G�}�������Ă��Ȃ��B";
			item_tester_hook = item_tester_hook_forbidden_fruit;
			if (get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) have_fruit = TRUE;

			if (have_fruit)
			{
				if (!get_aim_dir(&dir)) return NULL;


				if (one_in_(3))
					msg_print("�u����������Đ����c������͂��Ȃ��I�v");
				else
					msg_print("���Ȃ��͔j��̗͂��֒f�̎��ɍ��߂ē��������I");

				fire_ball(GF_PSY_SPEAR, dir, dam, 8);

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

			}
			else
			{
				if (!get_check_strict("�{���ɐG�}�Ȃ��ł��̓��Z���g���܂����H", CHECK_OKAY_CANCEL)) return NULL;
				if (dun_level && !p_ptr->inside_quest && !p_ptr->inside_arena) msg_print("�\�������j��̗͂��_���W�����������􂢂��I");

				//������������ �N���l�����̂��m��Ȃ������̊֐����͎��ɃN�[�����Ǝv��
				call_the_();
			}

		}
		break;


	default:
		if (only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�G���W�j�A��p�Z*/
class_power_type class_power_engineer[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�@�B�i�[",
		"�o�b�N�p�b�N�ɋ@�B���i�[����B���̐��̓��x���A�b�v�ŏ㏸����B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�@�B�m�F",
		"�o�b�N�p�b�N�̋@�B���m�F���������������B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"��������",
		"�o�b�N�p�b�N����@�B���O���B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�@�B�g�p",
		"�o�b�N�p�b�N�Ɋi�[�����@�B���g�p����B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�e���[",
		"�o�b�N�p�b�N�Ɋi�[�����@�B�ɒe��Ȃǂ��[���g�p�񐔂��񕜂�����B"},

	{30,20,60,FALSE,FALSE,A_DEX,0,0,"��Ďˌ�",
		"�o�b�N�p�b�N�ɑ������ꂽ�ˌ������S�Ă���񂸂��˂���B(1+���ˉ�*0.25)�{�̍s�����Ԃ�������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_engineer(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			(void) use_machine(0);
			return NULL; //���邾���Ȃ̂ōs��������Ȃ�

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if( !use_machine(2)) return NULL;

			break;
		}
	case 4: //�e���[
		{
			if(only_info) return format("");
			if( !use_machine(1)) return NULL;
			break;
		}
	case 5: //��Ďˌ�
		{
			if(only_info) return format("");
			if(!engineer_triggerhappy()) return NULL;
			break;
		}

	}

	return "";
}


/*:::�~�X�e�B�A�p���Z*/
class_power_type class_power_mystia[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�̂��~�߂�",
		"���̂��Ă���̂�����Ύ~�߂�B�s���͂�����Ȃ��B"},
	{3,3,20,FALSE,TRUE,A_CHR,0,0,"���̖��",
		"���Ȃ��������镔�����Â�����B"},
	{5,8,30,FALSE,TRUE,A_CHR,0,0,"�鐝�̉�",
		"���͂̃����X�^�[�𒹖ڂɂ���̂��̂��B�̂��Ă���Ԃ�MP�����������B���ڂɂȂ����G�͍��m���ł��Ȃ��̈ʒu�����������h���ɂ��Ȃ��̍U�����󂯂邪�A���������邩������G�����o�ɗ���Ȃ����݂��ƌ��ʂ������B"},
	{10,10,30,FALSE,TRUE,A_DEX,0,0,"�ŉ�̗ؕ�",
		"���ӂ̓G������������B"},
	{18,32,50,FALSE,FALSE,A_CHR,0,0,"���g���o�^���I��",
		"�z���̒����Ăъ񂹂�B"},
	{25,30,50,FALSE,FALSE,A_DEX,50,0,"�C���X�^�[�h�_�C�u",
		"���ꂽ�G�ɓˌ�����B�������d���Ǝ��s���₷���B"},
	{30,24,60,FALSE,TRUE,A_CHR,0,0,"�~�X�e���A�X�\���O",
		"���͂̓G�ɐ��_�U�����s���N�O�E�����E�����E���@�͒ቺ������̂��̂��B�̂��Ă���Ԃ�MP�����������B���j�[�N�����X�^�[��ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ������B"},
	{35,30,65,FALSE,TRUE,A_CHR,0,0,"�q���[�}���P�[�W",
		"���͂̐l�Ԃ̈ړ��𕕂���̂��̂��B�܂��̂��Ȃ���l�ԂɍU������Ɛl�ԃX���C�𓾂�B�̂��Ă���Ԃ�MP�����������B" },
	{40,36,75,FALSE,TRUE,A_CHR,0,0,"�^�钆�̃R�[���X�}�X�^�[",
		"���͂̓G�𖣗��E�U���͒ቺ��Ԃɂ���̂��̂��B�̂��Ă���Ԃ�MP�����������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::�~�X�e�B�A���Z p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]���̂Ɏg����*/
cptr do_cmd_class_power_aux_mystia(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//�̂��~�߂�@�s��������Ȃ�
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1: //���̖��
		{
			if(only_info) return "";
			msg_format("���Ȃ����ꐺ���ƁA�����̖����肪�������B");
			unlite_room(py,px);
			break;
		}
	case 2://�鐝�̉�
		{
			if(only_info) return "";
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MYSTIA_YAZYAKU, SPELL_CAST);

			break;
		}
	case 3://�ŉ�̗ؕ�
		{
			int power = 50 + plev * 4;
			int rad = 2 + plev / 10;

			if(only_info) return format("����:%d �͈�:%d",power/2,rad);
			msg_format("�h�����̕���U��T�����I");
			project(0, rad, py, px, power, GF_OLD_CONF, PROJECT_KILL , -1);
			
		}
		break;
	case 4://���g���o�^���I��
		{
			int i;
			int num = 2 + randint0(plev / 10);
			bool flag = FALSE;
			if(only_info) return format("�������x��:%d",plev);
			for(i=0;i<num;i++)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_BIRD, (PM_FORCE_PET))) flag = TRUE;
			}
			if(use_itemcard)
			{
				if(flag) msg_format("�����������ꂽ�I");
				else msg_format("��������Ȃ������E�E");
			}
			else
			{
				if(flag) msg_format("�z���̒����Ă񂾁I");
				else msg_format("�z���͌���Ȃ������E�E");
			}

		}
		break;
	case 5:
		{
			int len = p_ptr->lev / 7;
			if(only_info) return format("�˒�:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 6://�~�X�e���A�X�\���O
		{
			int power = plev * 2 + chr_adj * 2;
			if(only_info) return  format("����:%d",power);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MYSTIA_MYSTERIOUS, SPELL_CAST);

			break;
		}
	case 7://�q���[�}���P�[�W
	{
		int range = plev / 3;
		if (only_info) return  format("�͈�:%d",range);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MYSTIA_HUMAN_CAGE, SPELL_CAST);

		break;
	}

	case 8://�^�钆�̃R�[���X�}�X�^�[
		{
			int power = plev * 3 + chr_adj * 5;
			if(only_info) return  format("����:%d",power);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MYSTIA_CHORUSMASTER, SPELL_CAST);

			break;
		}


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}





/*:::�Ԕ؊��p�Z*/
class_power_type class_power_banki[] =
{
	{ 5,16,25,FALSE,TRUE,A_DEX,0,0,"�t���C���O�w�b�h",
		"���̕��g���o��������B���͓G��ǂ������đ̓�������ɂݍU��������B"},

	{ 10,10,20,FALSE,FALSE,A_INT,0,0,"�U�J�w��",
		"���̕��g�S�Ă��Z�����̃e���|�[�g���s���B" },
	{ 15,10,20,FALSE,FALSE,A_INT,0,0,"�T���w��",
		"���̕��g�S�Ă���-�������̃e���|�[�g���s���B" },
	{ 20,10,20,FALSE,FALSE,A_INT,0,0,"�W���w��",
		"���̕��g�S�Ă������̋߂��Ƀe���|�[�g���Ă���B" },
	{ 25,40,65,FALSE,TRUE,A_DEX,0,0,"�}���`�v���P�C�e�B�u�w�b�h",
		"���̕��g�𕡐��o��������B���͂��̏�𓮂����������̃��[�U�[�U��������B"},
	{ 32,32,70,FALSE,TRUE,A_CON,0,0,"�c�C�����N���w�b�h",
		"���͂̓��̕��g��̂����ł����A�������̋��͂ȃr�[���U������B���̍U���͑��̓��Ƀ_���[�W��^���Ȃ��B" },
	{ 38,88,70,FALSE,TRUE,A_CHR,0,0,"�f�����n���i�C�g",
		"���E���S�Ăɑ΂��u���̌����v�����̍U���𕡐���s���B�U���񐔂͎��E���̂��Ȃ��̎�̐��ɂ���đ�����B" },

	{ 44,60,75,FALSE,TRUE,A_DEX,0,0,"�w���Y���C",
		"�w�肵���^�[�Q�b�g���ӂɌ����Ė������̃r�[���𐔔����B���̕��g�����Ă��̃^�[�Q�b�g�Ɏ������ʂ��Ă���ꍇ���̕��g����������悤�Ƀr�[������B���̍U���ł��Ȃ��ƕ��g���_���[�W���󂯂邱�Ƃ͂Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};



cptr do_cmd_class_power_aux_banki(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0://�t���C���O�w�b�h
	case 4://�}���`�v���P�C�e�B�u�w�b�h
		{
			int summon_num;
			int max_num = MIN(9,(p_ptr->max_plv / 5));
			int i,cnt=0;
			int summon_idx;
			bool flg_success = FALSE;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(max_num<1)max_num=1;

			if(only_info) return format("�ő叢���\��:%d",max_num);
			if(num == 0) 
			{
				summon_idx = MON_BANKI_HEAD_1;
				summon_num = 1;
			}
			else
			{
				summon_idx = MON_BANKI_HEAD_2;
				summon_num = 1 + randint1(plev / 10);
			}

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_BANKI_HEAD_1) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
				if((m_ptr->r_idx == MON_BANKI_HEAD_2) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("����ȏ㕪�g���o���Ȃ��B");
				return NULL;
			}
			if(cnt + summon_num > max_num) summon_num = max_num - cnt;
			for(i=0;i<summon_num;i++)
			{
				if(summon_named_creature(0, py, px, summon_idx, PM_EPHEMERA)) flg_success = TRUE;
			}

			if(flg_success)	msg_print("���̕��g���o�����I");
			else msg_print("���̕��g�Ɏ��s�����B");

		break;
		}

	case 1: //�U�J
	case 2: //�T��
	case 3: //�W��
		{
			int i;
			monster_type *m_ptr;

			if (only_info) return "";

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (!is_pet(m_ptr)) continue;
				if (!(m_ptr->r_idx == MON_BANKI_HEAD_1 || (m_ptr->r_idx == MON_BANKI_HEAD_2)))continue;
				if (teleport_barrier(i)) continue;

				if (num == 1)
				{
					teleport_away(i, 10+randint1(5), 0L);
				}
				else if (num == 2)
				{
					teleport_away(i, 30+randint1(30), 0L);
				}
				else
				{
					teleport_monster_to(i, py, px, 100, TELEPORT_FORCE_NEXT);
				}
			}
		}
		break;

	case 5: //�c�C�����N���w�b�h
		{	
			int dir;
			int i;
			int m_idx1 = 0;
			int m_idx2 = 0;
			int damage = plev * 3 + chr_adj * 3;
			if (only_info) return format("����:%d", damage);

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (!is_pet(m_ptr)) continue;
				if (!(m_ptr->r_idx == MON_BANKI_HEAD_1 || (m_ptr->r_idx == MON_BANKI_HEAD_2)))continue;
				if (m_ptr->cdis > 1) continue;
				if(m_idx1) m_idx2 = i;
				else m_idx1 = i;
			}
			
			if (!m_idx1 || !m_idx2)
			{
				msg_print("�߂��ɓ�����ȏ�Ȃ��Ƃ��̋Z�͎g���Ȃ��B");
				return NULL;
			}

			if (!get_aim_dir(&dir)) return NULL;

			msg_print("���Ȃ��̓��͗�����`���Ĕ�񂾁I");
			delete_monster_idx(m_idx1);
			delete_monster_idx(m_idx2);
			fire_spark(GF_BANKI_BEAM2, dir, damage, 1);

		}
		break;

	case 6:
	{
		int i;
		int dam = plev * 200;
		int cnt=0;
		if (dam > 9999) dam = 9999;
		if (only_info) return format("����:%d", dam);

		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (!m_ptr->cdis) continue;
			if (!projectable(m_ptr->fy, m_ptr->fx, py, px)) continue;
			if ((m_ptr->r_idx == MON_BANKI_HEAD_1) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			if ((m_ptr->r_idx == MON_BANKI_HEAD_2) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
		}


		msg_print("�j�ł̐鍐���_���W�����ɋ����n�����B");
		for (i = 1 + cnt / 3; i > 0; i--)
		{
			deathray_monsters();
		}

	}
	break;



	case 7://�w���Y���C
		{
			int damage =  10 + plev/5 + chr_adj;
			int num = 2;
			int dir;
			if(plev > 47) num++;
			if(only_info) return format("����:%d*%d",damage,num);
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return NULL;
			}
			msg_print("���낵������������ꂽ�I");
			for(;num>0;num--)
			{
				int j;
				fire_beam(GF_BANKI_BEAM2,dir,damage);
				for (j = 1; j < m_max; j++)
				{
					int tx,ty;
					monster_type *m_ptr;
					m_ptr = &m_list[j];
					if(!target_okay()) break;
					if((m_ptr->r_idx != MON_BANKI_HEAD_1) && (m_ptr->r_idx != MON_BANKI_HEAD_2)) continue;
					if(!projectable(m_ptr->fy,m_ptr->fx,target_row,target_col)) continue;
					do
					{
						tx = target_col + randint0(3) - 1;
						ty = target_row + randint0(3) - 1;
					}while(!in_bounds2(ty,tx));

					project(j,0,ty,tx,damage,GF_BANKI_BEAM2,(PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU),-1);
				}
				if(!target_okay()) break;


			}

		}
		break;

	default:
		if (only_info) return "";
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::���p���Z*/
class_power_type class_power_aya[] =
{
	{12,10,20,FALSE,TRUE,A_DEX,0,4,"��ꌃx�[�����O",
		"�אڂ��Ă���G�ɑ΂����̐n�ōU������BAC�̍����G�ɂ͓�����ɂ����B���x��30�Ŕ͈͂��L����B"},
	{20,16,30,FALSE,TRUE,A_STR,0,4,"�V��̃}�N���o�[�X�g",
		"�v���[���[���ӂ̃����_���Ȉʒu�ɗ��������̋���A�����ĕ��B"},
	{27,30,50,FALSE,TRUE,A_STR,0,5,"�V�瓹�̊J��",
		"���������̑�^�̃��[�U�[�U������B"},
	{35,25,60,FALSE,TRUE,A_DEX,50,0,"���c�F�̐擱",
		"����Z���ēːi����B�꒼����ɗ��������̍U�����s���G�𐁂���΂��ړ����A�ǂ�������΂��Ȃ��G�ɓ�����Ǝ~�܂�B�������d���Ɠ�x���オ��B"},
	{40,45,60,FALSE,TRUE,A_CHR,0,0,"�V���~�Ղ̓������",
		"�����𒆐S�ɂ�������ȗ����𔭐�������B"},
	{45,80,80,FALSE,TRUE,A_DEX,0,0,"���z���r",
		"�Z���Ԃ̊ԁA���E�܂ŉ������U���񐔂Ɗi���\�͂��㏸����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

cptr do_cmd_class_power_aux_aya(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int dam = 50 + plev * 2 + chr_adj * 5;
			int rad = 1;
			if(plev > 29) rad = 2;

			if(only_info) return format("����:%d �͈�:%d",dam/2,rad);
			msg_print("��ꌂ��N�������I");
			project(0, rad, py, px, dam, GF_WINDCUTTER, PROJECT_KILL|PROJECT_GRID , -1);

		}
		break;
	case 1:
		{
			int rad = 2 + plev / 20;
			int damage = plev + adj_general[p_ptr->stat_ind[A_CHR]] * 3;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			msg_print("��󂩂������������I");
			cast_meteor(damage, rad, GF_TORNADO);

			break;
		}
	case 2:
		{
			int damage = 50 + p_ptr->lev * 2 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�������O����ガ�|�����I");
			fire_spark(GF_TORNADO,dir,damage,2);
			break;
		}
	case 3://���C�V�����́u�ːi�v�̃��[�`������������
		{
			int dir;
			int tx, ty;
			u16b path_g[32];
			int path_n, i;
			bool moved = FALSE;
			int damage = plev * 3;
			project_length = MAX(5,plev / 3);
			if(damage < 100) damage=100;
			if(only_info) return format("����:%d �˒�:%d",damage/2,project_length);
			
			if (!get_aim_dir(&dir)) return NULL;
			tx = px + project_length * ddx[dir];
			ty = py + project_length * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_THRU | PROJECT_KILL);
			project_length = 0;
			if (!path_n) return NULL;
			if (in_bounds(ty, tx)) msg_format("���Ȃ��͕���Z���ēːi�����I");
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

					if(r_ptr->flags2 & RF2_GIGANTIC)
					{			
						msg_format("%s�ɓːi���~�߂�ꂽ�I",m_name);
						(void)project(0, 1, ny, nx, damage, GF_TORNADO, PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID, -1);
						break;
					}
					else 
					{
						(void)project(0, 1, ny, nx, damage, GF_TORNADO, PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID, -1);
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
		}
		break;
	case 4:
		{
			int dam = 100 + plev * 6 + chr_adj * 20;
			int rad = 4 + plev / 15;

			if(only_info) return format("����:%d �͈�:%d",dam/2,rad);
			msg_print("����ȗ������N�������I");
			project(0, rad, py, px, dam, GF_TORNADO, PROJECT_KILL , -1);
		}
		break;
	case 5:
		{
			if(only_info) return format("����:10�^�[��");
			set_lightspeed(10,FALSE);
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�i���p���Z*/
class_power_type class_power_librarian[] =
{
	{40,0,50,FALSE,TRUE,A_DEX,0,0,"�����r��",
		"�f�������@���r������B����������MP��30%�������A���@�r��������ɃL�����Z�����Ă��s�����������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

cptr do_cmd_class_power_aux_librarian(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			quick_cast = TRUE;
			do_cmd_cast();
			quick_cast = FALSE;
			new_class_power_change_energy_need = 100 - p_ptr->lev;
		}
		break;
	
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



//p_ptr->magic_num2[0]���u�b�����������v�t���O�Ɏg��
/*:::�p�`�����[��p�Z*/
class_power_type class_power_patchouli[] =
{
	{ 3,4,15,FALSE,TRUE,A_INT,0,2,"�h���[�X�s�A",
		"�^�[�Q�b�g�ɕ�����̖������U��������B�˒��͒Z���B"},
	{ 15,10,30,FALSE,TRUE,A_INT,0,4,"�v�����Z�X�E���f�B�l",
		"�������̃{���g��A�˂���B"},
	{ 20,20,35,FALSE,TRUE,A_INT,0,0,"�G�������^���n�[�x�X�^�[",
		"�����̎���ɉ�]����n�𐶂ݏo���A�ߊ�����G�ɑ΂��Ĕ�������B�j�Б����I�[�������ɂȂ�B"},
	{ 24,28,45,FALSE,TRUE,A_INT,0,0,"�Z���g�G�����s���[",
		"���͂ȃv���Y�}�����̋�����B"},
	{ 28,36,50,FALSE,TRUE,A_INT,0,0,"���^���t�@�e�B�[�O",//v1.1.95
		"���͂̃����X�^�[���U���͒ቺ�A�h��͒ቺ��Ԃɂ���B" },
	{ 32,45,55,FALSE,TRUE,A_INT,0,0,"���[���@�N�������N",
		"�w�肵���ʒu�ɉΉ������̋���ȃ{�[���𔭐������A�n�ʂ�n��ɂ���B"},
	{ 35,40,65,FALSE,TRUE,A_INT,0,0,"�X�e�B�b�L�[�o�u��",//v1.1.95
		"�����X�^�[��Z���Ԉړ��֎~�ɂ���u���X��̍U�����s���B" },

	{ 37,48,50,FALSE,TRUE,A_INT,0,0,"�T�C�����g�Z���i",
		"�אڂ����G�S�ĂɈÍ������̋��͂ȍU�����s���B"},

	{ 40,40,80,FALSE,TRUE,A_INT,0,0,"�����r��",	//v1.1.75
	"���@���ɂ�閂�@���A���Ŏg�p����B�b���̒��q�������Ƃ��ɂ͎g���Ȃ��B" },

	{ 45,128,75,FALSE,TRUE,A_INT,0,0,"���C�����t���A",
		"�����𒆐S�Ƃ������������̃{�[���𔭐������A����Ɏ��E���S�Ă��j�M�����ōU������B"},
	{ 48,96,80,FALSE,TRUE,A_INT,0,0,"���҂̐�",
		"���͂Ȍ��f�U�����s���u���҂̐΁v��z���Ƃ��ď�������B�K�ړ�����Ə�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};



cptr do_cmd_class_power_aux_patchouli(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0: //�h���[�X�s�A
		{
			int dist = 3 + plev / 15;
			dice = 8 + plev / 3;
			sides = 5 + chr_adj / 5;
			if(only_info) return format("����:%dd%d �˒�:%d",dice,sides,dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_MISSILE, dir, damroll(dice, sides),0,"�n�ʂ���ΐ�����яo�����I")) return NULL;
			break;
		}
	case 1: //�v�����Z�X�E���f�B�l
		{
			int num = 3 + plev / 20;
			dice = 3 + plev / 6;
			sides = 5 + chr_adj / 5;
			if(only_info) return format("����:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���̖��������I");
			for (i = 0; i < num; i++) fire_bolt(GF_WATER, dir, damroll(dice, sides));
			break;

		}
	case 2://�G�������^���n�[�x�X�^�[
		{
			if(only_info) return format("����:20+1d20");
			msg_format("��]����n�����ꂽ�B");
			set_dustrobe(20+randint1(20),FALSE);
			break;
		}
	case 3://�Z���g�G�����s���[
		{
			int rad = 3;
			if(p_ptr->lev > 39) rad = 4;
			dice = 8 + p_ptr->lev / 4;
			sides = 10 + chr_adj / 3 ;
			base = p_ptr->lev*4;
			if(base < 100) base = 100;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_PLASMA, dir, base + damroll(dice,sides), rad,"�ΉԂ��U�炷���̒��������������I")) return NULL;
			break;
		}

	case 4: //���^���t�@�e�B�[�O
		{
			int power = 150 + plev * 10 + chr_adj * 5;
			if (only_info) return format("����:�`%d", power / 2);

			msg_print("���͂̕������}���ɕ��I�������I");

			project(0, 8, py, px, power, GF_DEC_ATK, PROJECT_JUMP | PROJECT_KILL, -1);
			project(0, 8, py, px, power, GF_DEC_DEF, PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL, -1);

			break;
		}



	case 5: //���[���@�N�������N
		{
			int rad = 3;
			base = p_ptr->lev*5 + chr_adj * 5;
			if(only_info) return format("����:%d",base);
			
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_LAVA_FLOW, dir, rad, rad,"�n�₪���o�����I")) return NULL;
			fire_ball_jump(GF_FIRE, dir, base, rad,NULL);
			break;

		}
		break;

	case 6: //�X�e�B�b�L�[�o�u��
		{
			int power = plev / 4;

			if (only_info) return format("����:%d", power);

			project_length = 5;

			if (!get_aim_dir(&dir)) return NULL;

			msg_print("�S���A�𐁂��t�����B");
			fire_ball(GF_NO_MOVE, dir, power, -2);
			break;
		}

	case 7: //�T�C�����g�Z���i
		{
			int rad = 1;
			base = p_ptr->lev * 10 + chr_adj * 20;
			if(base < 600) base = 600;
			if(only_info) return format("����:%d",base / 2);	
			msg_format("���̌����~�蒍�����B");
			project(0, rad, py, px, base, GF_DARK, PROJECT_KILL , -1);
			break;
		}

	case 8: //v1.1.75 �����r�� 
		{
			if (only_info) return format("");

			if (!can_do_cmd_cast()) return NULL;
			do_cmd_cast();
			handle_stuff();
			if (can_do_cmd_cast())
				do_cmd_cast();

			break;
		}


	case 9://���C�����t���A MAX500+1000 ����47��570+1350�ӂ�
		{
			base = p_ptr->lev * 14 + chr_adj * 10;
			dice = p_ptr->lev ;
			sides = 10 + chr_adj ;

			if(only_info) return format("����:�`%d+%dd%d ",base/2 ,dice,sides);
			msg_format("����ȑM���ƏՌ������͂����W�����I");
			project(0, 7, py, px, base, GF_SOUND, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM , -1);
			project_hack2(GF_NUKE,dice,sides,0);

			break;
		}
	case 10://���҂̐�
		{
			int max_num = 5;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("�ő叢���\��:%d",max_num);
	
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_PHILOSOPHER_STONE) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("����ȏ�z�u�ł��Ȃ��B",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_PHILOSOPHER_STONE, PM_EPHEMERA)) msg_print("���҂̐΂��Ăяo�����I");
			else msg_print("���@�Ɏ��s�����B");

		break;
		}
	default:


		if (only_info) return format("������");

		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�F���p���Z*/
class_power_type class_power_yoshika[] =
{
	{15,16,40,FALSE,TRUE,A_CHR,0,8,"�q�[���o�C�f�U�C�A",
		"�F�D�I�ȏ��_�삪��ʂɌ����B���_��͈��m���ł��Ȃ��ɋz�������HP���񕜂���B"},

	{24,30,50,FALSE,TRUE,A_CON,0,0,"�]�E�t�H���D���H",
		"HP�ƃX�e�[�^�X�ُ���񕜂�����B"},

	{32,64,65,TRUE,FALSE,A_STR,0,20,"���ȂȂ��E�l�S",
		"�����̎��͑S�Ăɑ΂��אڍU�����s���B���̍U���ɂ�萶�҂���̗͂��z�����邱�Ƃ��ł���B����g�p���ɂ͎g���Ȃ��B"},

	{45,80,80,FALSE,TRUE,A_CON,0,0,"�ߏ�]�E�t�H���D���H",
		"�ꎞ�I�ɉ����E����m���E���E���z�����g�̔\�͏㏸�𓾂�B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

cptr do_cmd_class_power_aux_yoshika(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			int i;
			bool flag = FALSE;
			int num = p_ptr->lev / 14;
			if(only_info) return "";
			for(i=0;i<num;i++)
			{
				if (summon_specific(-1, py, px, p_ptr->lev, SUMMON_DESIRE, PM_ALLOW_GROUP | PM_FORCE_FRIENDLY)) flag = TRUE;
			}
			if(flag)
				msg_print("���_�삪���ꂽ�I");
			else 
				msg_print("���_��͌���Ȃ������B");

		}
		break;
	case 1:
		{
			int heal = 100 + plev * 3;
			if(only_info) return format("��:%d",heal);

			msg_print("�傪����A���Ȃ��̑̂̏C�����n�߂��E�E");
			hp_player(heal);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			set_image(0);
			restore_level();
			set_alcohol(0);
			set_asthma(0);
			msg_print("��͍Ăтǂ����֍s�����B");

		}
		break;
	case 2:
		{
			int x,y;
			cave_type       *c_ptr;
			monster_type    *m_ptr;
			monster_race	*r_ptr;
			if(only_info) return format("");

			if(!p_ptr->do_martialarts)
			{
				msg_print("�f��łȂ��Ƃ��̋Z�͎g���Ȃ��B");
				return NULL;
			}

			for (dir = 0; dir < 8; dir++)
			{
				y = py + ddy_ddd[dir];
				x = px + ddx_ddd[dir];
				c_ptr = &cave[y][x];

				m_ptr = &m_list[c_ptr->m_idx];
				/*:::�s���̃����X�^�[���U�����邪�A�ǂ̒��ɂ���s���̃����X�^�[�͍U�����Ȃ�*/
				if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
				{
					py_attack(y, x, HISSATSU_YOSHIKA);

				}
			}
		}
		break;
	case 3:
		{
			int base = 10;
			int time;
			if(only_info) return format("����:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_print("�傪����A���Ȃ��̑̂ɉ���猕�ۂȏp���{�����I");
			set_fast(time,FALSE);
			set_shero(time,FALSE);
			set_tim_addstat(A_STR,105,time,FALSE);
			set_tim_addstat(A_CON,105,time,FALSE);
			msg_print("��͖������ɋA���Ă������B");

			break;
		}

	
	default:
			if(only_info) return format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::���e�B�p���Z*/
class_power_type class_power_letty[] =
{
	{3,2,10,FALSE,TRUE,A_WIS,0,2,"��C�̖�",
		"�^�[�Q�b�g�Ɍ����ė�C�����̃{���g���r�[������B���x�����オ��ƈЗ͂��傫���オ��r�[�����o�₷���Ȃ邪����MP���オ��B"},
	{8,10,30,FALSE,TRUE,A_WIS,0,0,"�����K�����O�R�[���h",
		"���E���̖����Ă���G������ɐ[�����点��B���������C�ϐ������G�ɂ͌��ʂ������B"},
	{14,15,35,FALSE,TRUE,A_WIS,0,7,"�t�����[�E�B�U���E�F�C",
		"���E���̑S�Ăɗ�C�U������B���x�����オ��ƈЗ͂��傫���オ�邪����MP���オ��B"},
	{19,16,40,FALSE,TRUE,A_INT,0,0,"�A���f�����C�V�������C",
		"�G�����m���Ŗ��点��r�[������B�����ϐ������G�ɂ͌����Ȃ������x��35�ȍ~�͌����悤�ɂȂ�B���j�[�N�����X�^�[�ɂ͌����Ȃ��B"},
	{25,16,30,FALSE,TRUE,A_CHR,0,6,"�m�[�U���E�B�i�[",
		"�אڂ�����̂̓G�ɓ���ȗ�C�U�����s���B�X�ϐ��Ōy������ɂ��������Ă���G�ɂ͑�_���[�W��^������ɐ����̏ꍇ�͋N�����ɂ����B���j�[�N�����X�^�[�△�����ɂ͌��ʂ������B"},
	{32,24,50,FALSE,TRUE,A_WIS,0,2,"�R�[���h�X�i�b�v",
		"�����𒆐S�ɂ����Ɋ������̋���ȋ�����B"},
	{40,54,80,FALSE,TRUE,A_CHR,0,0,"�e�[�u���^�[�j���O",
		"�אڂ����^�[�Q�b�g��̂ɋ��͂Ȍ����U�����s���B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

cptr do_cmd_class_power_aux_letty(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];


	switch(num)
	{

	case 0:
		{
			int dice = 3 + plev / 4;
			int sides = 6 + chr_adj / 5;
			if (WINTER_LETTY1) sides = sides * 3 / 2;
			if(only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("���Ȃ��͗�C�̉��������I");
			fire_bolt_or_beam(plev * 2, GF_COLD, dir, damroll(dice, sides));
		}
		break;
	case 1: //�����K�����O�R�[���h�@���E���̐Q�Ă��郂���X�^�[�����[�����点��
		{
			int i;
			if(only_info) return format("");
			msg_print("�����Ɋ��C�����ݓn�����E�E");
			for (i = 1; i < m_max; i++)
			{
				char m_name[80];

				int pow = plev * 3  + 50;
				int sleep;
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (WINTER_LETTY1) pow += 100;

				if (!m_ptr->r_idx) continue;
				if(!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
				sleep = MON_CSLEEP(m_ptr);
				if(!sleep) continue;

				monster_desc(m_name, m_ptr, 0);
				if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) pow /= 2;
				if(!monster_living(r_ptr)) pow /= 2;
				if(!(WINTER_LETTY2) && r_ptr->flagsr & RFR_IM_COLD ) pow /= 3; //v1.1.85 ������őϐ��ђ�
				if(r_ptr->flags2 & RF2_POWERFUL) pow /= 2;
				(void)set_monster_csleep(i, sleep + pow);
				msg_format("%^s�̖��肪�[���Ȃ����悤���B", m_name);
			}


		}
		break;
	case 2://�t�����[�E�B�U���E�F�C
		{
			int dam = plev * 2 + chr_adj * 5;
			if(dam < 50) dam = 50;

			if (WINTER_LETTY1) dam += plev * 2;

			if(only_info) return format("����:%d",dam);
			if(use_itemcard)
				msg_print("���Ⴊ�����N�������I");
			else
				msg_print("���Ȃ��͐���������N�������I");

			project_hack2(GF_COLD, 0,0,dam);
		}

		break;

	case 3://�A���f�����C�V�������C
		{
			int power = plev * 3 + chr_adj * 5;
			int typ;
			if(power < 80) power = 80;

			if(only_info) return format("����:%d",power);
			
			if (!get_aim_dir(&dir)) return NULL;
			if(plev > 34 || use_itemcard)
			{
				typ = GF_STASIS;
				msg_format("�����P�������������ꂽ�I");
			}
			else
			{
				typ = GF_OLD_SLEEP;
				msg_format("�����P�������������ꂽ�I");
			}
			fire_beam(typ, dir, power);
			break;
		}
	case 4://�m�[�U���E�B�i�[
		{
			int base = chr_adj * 5;
			int dice = plev / 2;
			int sides = 10;
			int y, x;
			monster_type *m_ptr;
			if (WINTER_LETTY1) base *= 2;
			if(only_info) return format("����:%dd%d + %d",dice,sides,base);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];
			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("���Ȃ���%s�ɐG��A�������Ɗ��C�𐁂����񂾁E�E",m_name);
				project(0, 0, y, x, damroll(dice,sides) + base, GF_NORTHERN, flg, -1);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			break;
		}
	case 5://�R�[���h�X�i�b�v
		{
			if (WINTER_LETTY1)
			{
				int base = 50 + plev * 2 + chr_adj * 5;
				if (only_info) return format("����:%d", base);
				msg_print("���Ⴊ�җ�Ɍ������𑝂����I");
				project_hack2(GF_ICE, 0, 0, base);
				break;

			}
			else
			{
				int base = 100 + plev * 3 + chr_adj * 5;
				int rad = 2 + plev / 10;
				if (only_info) return format("����:�ő�%d", base / 2);
				msg_print("���͂̉��x���}���ɒቺ�����I");
				project(0, rad, py, px, base, GF_ICE, PROJECT_KILL | PROJECT_ITEM, -1);
				break;

			}
		}

	case 6://�e�[�u���^�[�j���O
		{
			int base = plev*3 + chr_adj * 10;
			int y, x;
			monster_type *m_ptr;
			if (WINTER_LETTY1) base = base * 3 / 2;
			if(only_info) return format("����:%d",base);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];
			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("��̏d����%s�������ׂ����I",m_name);
				project(0, 0, y, x, base, GF_INACT, flg, -1);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::���@���m�i���f)�p���Z*/
class_power_type class_power_magic_knight_elem[] =
{
	{8,16,30,FALSE,TRUE,A_INT,0,0,"���̖��@��",
		"�Z���Ԃ̊ԁA�Ή������ōU���ł���悤�ɂȂ�B"},
	{16,16,30,FALSE,TRUE,A_INT,0,0,"�X�̖��@��",
		"�Z���Ԃ̊ԁA��C�����ōU���ł���悤�ɂȂ�B"},
	{24,16,30,FALSE,TRUE,A_INT,0,0,"���̖��@��",
		"�Z���Ԃ̊ԁA�d�������ōU���ł���悤�ɂȂ�B"},
	{32,16,30,FALSE,TRUE,A_INT,0,0,"�_�̖��@��",
		"�Z���Ԃ̊ԁA�_�����ōU���ł���悤�ɂȂ�B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};


cptr do_cmd_class_power_aux_magic_knight_elem(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			msg_format("��ɂ��Ă��镐�킪���ɕ���ꂽ�B");
			set_ele_attack(ATTACK_FIRE, base + randint1(sides));
			break;
		}
		break;
	case 1:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			msg_format("��ɂ��Ă��镐�킪��C�ɕ���ꂽ�B");
			set_ele_attack(ATTACK_COLD, base + randint1(sides));
			break;
		}
		break;
	case 2:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			msg_format("��ɂ��Ă��镐�킪�ΉԂɕ���ꂽ�B");
			set_ele_attack(ATTACK_ELEC, base + randint1(sides));
			break;
		}
		break;
	case 3:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			msg_format("��ɂ��Ă��镐�킪���ɕ���ꂽ�B");
			set_ele_attack(ATTACK_ACID, base + randint1(sides));
			break;
		}
		break;

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���@���m�i�\��)�p���Z*/
class_power_type class_power_magic_knight_fore[] =
{
	{20,24,50,FALSE,TRUE,A_INT,0,0,"�\���̖��@��",
		"�Z���Ԃ̊ԁA����U���̖����������x���ɉ����đ啝�ɏ㏸����B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_magic_knight_fore(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_FORESIGHT, base + randint1(sides));
			break;
		}
		break;
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���@���m�i����)�p���Z*/
class_power_type class_power_magic_knight_summon[] =
{
	{20,24,50,FALSE,TRUE,A_INT,0,0,"�c�Ȃ̖��@��",
		"�Z���Ԃ̊ԁA����ɐ؂ꖡ���t�������B�݊�ɂ����B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};
//QUESTOR��N�G�X�g�_���W�����̓G�����ł��Ȃ��悤�ɋC��t����

cptr do_cmd_class_power_aux_magic_knight_summon(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_UNSUMMON, base + randint1(sides));
			break;
		}
		break;
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::���@���m�i�t�^)�p���Z*/
class_power_type class_power_magic_knight_enchant[] =
{
	{25,30,50,FALSE,TRUE,A_INT,0,0,"�����̖��@��",
		"�Z���Ԃ̊ԁA����̃_�C�X�l����������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_magic_knight_enchant(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_INC_DICES, base + randint1(sides));
			break;
		}
		break;
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::���@���m�i�Í�)�p���Z*/
class_power_type class_power_magic_knight_darkness[] =
{
	{16,20,50,FALSE,TRUE,A_INT,0,0,"����",
		"�Z���Ԃ̊ԁA�������̕��킪�������͂̓G�ɑ傫�ȃ_���[�W��^����悤�ɂȂ�B���x�����㏸����Ƌz�����ʂ��ǉ������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_magic_knight_darkness(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_DARK, base + randint1(base));
			break;
		}
		break;
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���@���m�i����)�p���Z*/
class_power_type class_power_magic_knight_chaos[] =
{
	{20,20,50,FALSE,TRUE,A_INT,0,0,"���ׂ̖��@��",
		"�Z���Ԃ̊ԁA�������̕���ōU�������Ƃ��ɓG�������A�N�O�����邱�Ƃ�����B�J�I�X�ϐ����������X�^�[�ɂ͌��ʂ��Ȃ��B���j�[�N�����X�^�[�E����ȓG�E�͋����G�ɂ͌��ʂ������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_magic_knight_chaos(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_CHAOS, base + randint1(base));
			break;
		}
		break;
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::���\�͎җp���Z*/
class_power_type class_power_mind[] =
{
	{15,0,70,FALSE,FALSE,A_WIS,0,0,"�����~��",
		"���_���W�����Ė��͂������񕜂���B�z��������Ǝg���Ȃ��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

/*:::�C���җp���Z*/
class_power_type class_power_syugen[] =
{
	{30,1,70,FALSE,FALSE,A_WIS,0,0,"�����~��",
		"���_���W�����Ė��͂������񕜂���B�z��������Ǝg���Ȃ��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_syugen(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("");	

			msg_print("���������n�b�L�������B");

			p_ptr->csp += (3 + plev/20);
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}

			p_ptr->redraw |= (PR_MANA);
		}
		break;
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�i�Y�[������p�Z*/
class_power_type class_power_nazrin[] =
{
	{3,1,10,FALSE,FALSE,A_INT,0,0,"����T�m",
		"���͂̋���z���𔭌�����B"},
	{5,3,20,FALSE,FALSE,A_INT,0,0,"�A�C�e���T�m",
		"���͂̃A�C�e���𔭌�����B"},
	{7,6,25,FALSE,FALSE,A_INT,0,0,"㩒T�m",
		"���͂ɉB���ꂽ㩂𔭌�����B"},
	{9,8,30,FALSE,FALSE,A_INT,0,0,"�����X�^�[�T�m",
		"���͂̓G�̋��ꏊ��m��B"},
	{12,10,15,FALSE,TRUE,A_CHR,0,6,"�l����",
		"�z���̃l�Y�~�B���Ăяo���B���x�����オ��Ə���MP�ƌĂяo������������B"},
	{15,20,50,FALSE,TRUE,A_INT,0,0,"�����I���̋P��",
		"���̃t���A�Ɂu���̂���A�C�e���v������΂����m�邱�Ƃ��ł���B�u�`���̃A�C�e���v�̂��Ƃ͕�����Ȃ��B"},
	{16,10,30,FALSE,FALSE,A_INT,0,0,"�n�`�T�m",
		"���͂̒n�`��m��B"},
	{18,25,20,TRUE,FALSE,A_DEX,40,0,"�ꌂ���E",
		"�G�ɍU�����A���̌��u�ŒZ�����e���|�[�g������B�����i���d���Ɠ�Փx���オ��B�U����Ƀe���|�[�g�Ɏ��s���邱�Ƃ�����B"},
	{20,24,36,FALSE,FALSE,A_INT,0,0,"�n��̓��Y���T�K",
		"���̃t���A�Ɂu�f�ށv�J�e�S���̃A�C�e��������΂����m�邱�Ƃ��ł���B"},
	{23,12,60,FALSE,FALSE,A_INT,0,0,"�ӎ�",
		"�A�C�e�����Ӓ肷��B���S�ȏ���m�邱�Ƃ͂ł��Ȃ��B"},
	{25,20,40,FALSE,FALSE,A_INT,0,0,"�S���m",
		"���͂̃A�C�e���A㩁A�����X�^�[�̏ꏊ��m��B"},
	{30,30,50,FALSE,FALSE,A_WIS,0,0,"�y���f�������K�[�h",
		"�t���A�̕��͋C���@�m���A���͋C�����Ɉ����Ƃ��Ɂu���@�̊Z�v���ʂ𓾂�B�A�C�e���u�i�Y�[�����y���f�������v���������Ă��Ȃ��Ǝg���Ȃ��B" },
	{32,36,60,FALSE,TRUE,A_INT,0,0,"�`���̕i����",
		"���̃t���A�Ɂu�`���̃A�C�e���v������΂����m�邱�Ƃ��ł���B"},
	{36,40,70,FALSE,TRUE,A_INT,0,0,"�S���m�E��",
		"��莞�ԁA���͂̒n�`�E�A�C�e���E�g���b�v�E�����X�^�[��m�o��������B"},
	{42,60,80,FALSE,TRUE,A_INT,0,0,"�t���A�T�m",
		"���̃t���A�̒n�`�ƃA�C�e����S�ĒT�m����B"},
	{45,80,80,FALSE,TRUE,A_WIS,0,0,"�O���C�e�X�g�g���W���[",
		"�j�ב����̋��͂ȃr�[������B�u������V�̕󓃁v���������Ă��Ȃ��Ǝg���Ȃ��B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_nazrin(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			if(only_info) return "";
			msg_print("���Ȃ��̓_�E�W���O���n�߂��E�E");
			detect_treasure(DETECT_RAD_DEFAULT);
			detect_objects_gold(DETECT_RAD_DEFAULT);
			break;
		}
	case 1:
		{
			if(only_info) return "";
			msg_print("���Ȃ��̓_�E�W���O���n�߂��E�E");
			detect_objects_normal(DETECT_RAD_DEFAULT);
			break;
		}

	case 2:
		{
			if(only_info) return "";
			msg_print("���Ȃ��̓_�E�W���O���n�߂��E�E");
			detect_traps(DETECT_RAD_DEFAULT, TRUE);
			break;
		}
	case 3:
		{
			if(only_info) return "";
			msg_print("���Ȃ��̓_�E�W���O���n�߂��E�E");
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			(void)detect_monsters_invis(DETECT_RAD_DEFAULT);
			break;
		}
	case 4:
		{
			int max = 2 + p_ptr->lev / 6;
			if(only_info) return format("�ő�:%d��",max);

			if(use_itemcard)
				msg_print("�ǂ�����Ƃ��Ȃ��d���l���������ꂽ�B");
			else
				msg_print("���Ȃ��͎q�l�Y�~���Ăяo�����B");

			for(i=0;i<max;i++) (void)summon_named_creature(0, py, px, MON_YOUKAI_MOUSE, PM_FORCE_PET);
			break;
		}
	//�����i���m
	case 5:
		{
			if(only_info) return format("");
			msg_print("���Ȃ��̓_�E�W���O���n�߂��E�E");
			search_specific_object(1);
			break;
		}
	case 6:
		{
			if(only_info) return "";
			msg_print("���Ȃ��̓_�E�W���O���n�߂��E�E");
			map_area(DETECT_RAD_MAP);
			break;
		}
	case 7:
		{
			if(only_info) return format("");	
			if(!hit_and_away()) return NULL;
			break;
		}
	//�f�ފ��m
	case 8:
		{
			if(only_info) return format("");	
			msg_print("���Ȃ��̓_�E�W���O���n�߂��E�E");
			search_specific_object(3);
			break;
		}
	case 9: //�ӎ�
		{
			if(only_info) return format("");
			if (!ident_spell(FALSE)) return NULL;
			break;
		}
	case 10://�S���m
		{
			if(only_info) return  format("");
			msg_print("���Ȃ��̓_�E�W���O���n�߂��E�E");
			detect_all(DETECT_RAD_DEFAULT);
			break;
		}
	case 11: //v1.1.59 �y���f�������K�[�h
		{
			int base = 20;
			if (only_info) return format("����:%d+1d%d", base, base);

			msg_print("���Ȃ��̓y���f���������񂵎n�߂��E�E");
			update_dungeon_feeling(TRUE);
			//do_cmd_feeling_text[]�Q��
			if (p_ptr->feeling >= 2 && p_ptr->feeling <= 4)
			{
				msg_print("�y���f���������d�C�ɔ������Ėh�ǂɂȂ����I");
				set_magicdef((base + randint1(base)), FALSE);
			}

		}
		break;
	//�`���̕i���m
	case 12:
		{
			if(only_info) return format("");
			msg_print("���Ȃ��͓`���I�ȑ��݂𓪂Ɏv�������ׂă_�E�W���O���n�߂��E�E");
			search_specific_object(2);
			break;
		}
	//�S���m�E��
	case 13:
		{
			int base = p_ptr->lev/2;
			if(only_info) return format("����:%d+1d%d",base,base);

			msg_format("���Ȃ��͑�ʂ̎q�l�����͂ɕ������E�E");
			set_radar_sense(randint1(base) + base, FALSE);
			break;
		}
	case 14: //�t���A�T�m
		{
			if(only_info) return format("");
			msg_format("���Ȃ��͒n���ɏZ�ރl�Y�~�B��������W�߂��E�E");
			wiz_lite(FALSE);
			break;
		}
	case 15:
		{
			damage = p_ptr->lev * 4 + adj_general[p_ptr->stat_ind[A_WIS]] * 5 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("������V�̕󓃂���ῂ����������ꂽ�I");
			fire_spark(GF_HOLY_FIRE,dir,damage,1);

			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�t�r�_�g���p���Z*/
class_power_type class_power_tsukumo_master[] =
{
	{3,20,25,FALSE,TRUE,A_INT,0,0,"�t�r�_��",
		"�����t�r�_�ɂ��Đ�킹��B�t�r�_�͓|���ꂽ�������ꂽ��t���A���ړ�����ƌ��̕���ɖ߂�B�|���ꂽ�Ƃ����̕���͑啝�ɗ򉻂���B�t�r�_���|�����G�̌o���l�ƃA�C�e������ɓ���B�����_���A�[�e�B�t�@�N�g�̕t�r�_�͗򉻂��ɂ����n�k��j��ŏ��ł��Ȃ��B"},
	{9,12,30,FALSE,TRUE,A_INT,0,0,"�A�C�e���E�g���b�v���m",
		"���͂̒n�ʂ̃A�C�e���ƃg���b�v�����m����B"},
	{15,18,40,FALSE,TRUE,A_INT,0,0,"�g���b�v�j��",
		"���͂̃g���b�v�Ɋ�������������B���x�����オ��Ɣ͈͂��L����B"},
	{24,24,55,FALSE,FALSE,A_INT,0,0,"�ӎ�",
		"�w�肵���A�C�e���̓�����m��B���x��45�ȍ~�ŃA�C�e���̓��������S�ɒm��B"},
	{30,48,70,FALSE,TRUE,A_CHR,0,0,"�A�Җ���",
		"�z���̕t�r�_�����Ȃ��̌��ɋA���Ă��Č��̕���ɖ߂��ăU�b�N�ɓ���B�U�b�N�̗e�ʂ�����Ȃ��Ƃ��̕��͑����ɗ�����B�t�r�_���߂��Ă���̂ɂ͂�⎞�Ԃ�������B���̕��킪����Ă���ƋA�Җ��߂ɉ����Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

/*:::�t�r�_�g�����t�r�_�ɂł��镐��@���ȊO�̒��ڐ퓬����*/
///mod150925 �������ɂ����ăt���A�ړ����Ă������Ȃ��Ȃ����̂Ł����Ώۂɂ���
//v1.1.93 �֐��𕪂���K�v�������Ȃ��̂ō폜�Bobject_is_melee_weapon_except_strange_kind���g��
/*
bool item_tester_hook_make_tsukumo(object_type *o_ptr)
{
	//if(object_is_fixed_artifact(o_ptr)) return FALSE;
	return object_is_melee_weapon_except_strange_kind(o_ptr);

}
*/


cptr do_cmd_class_power_aux_tsukumo_master(int num, bool only_info)
{
	int dir,dice,sides,base;
	switch(num)
	{
		//����̕t�r�_��
	case 0:
		{
			int         item;
			object_type *o_ptr;
			cptr        q, s;
			int			r_idx=0;
			int		new_o_idx;
			int i;
			int cnt=0,max;
			u32b empty = 0x1F;

			max = 1 + (p_ptr->lev -5) / 10;

			if(only_info) return format("�ő�:%d��",max);

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (m_ptr->r_idx < MON_TSUKUMO_WEAPON1 || m_ptr->r_idx > MON_TSUKUMO_WEAPON5 ) continue;
				empty -= (1L << (m_ptr->r_idx - MON_TSUKUMO_WEAPON1));
				cnt++;
			}

			if(cnt >= max)
			{
				msg_print("����ȏ�̕t�r�_�������Ȃ��B");
				return NULL;
			}

			for(i=0;i<5;i++)
			{
				if((empty >> i) & 1L)
				{
					r_idx = MON_TSUKUMO_WEAPON1 + i;
					break;
				}
				if(i==4) msg_print("ERROR:�t�r�_idx�Ǘ�����������");
			}

			item_tester_hook = object_is_melee_weapon_except_strange_kind;

			q = "�ǂ̕����t�r�_�����܂���? ";
			s = "�t�r�_���ł��镐�킪�Ȃ��B";

			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0) o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

			apply_mon_race_tsukumoweapon(o_ptr,r_idx);

			new_o_idx = o_pop();

			if (!new_o_idx || !summon_named_creature(0, py, px, r_idx, PM_EPHEMERA))
			{
				msg_print("�t�r�_���Ɏ��s�����B");
			}
			
			else //�����ɓ��鎞summon_named_creature()��TRUE�ŏI���A�t�r�_�̃p�����[�^�����Ɛ������ς�ł���
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[r_idx];
				object_copy(&o_list[new_o_idx], o_ptr);
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					if (m_ptr->r_idx == r_idx ) 
					{
						m_ptr->hold_o_idx = new_o_idx;
						break;
					}
				}
				/*:::�V���ȕt�r�_�����A�C�e���������Ă��鈵���ɂ���*/
				o_list[new_o_idx].held_m_idx = i;
				o_list[new_o_idx].ix = 0;
				o_list[new_o_idx].iy = 0;
				o_list[new_o_idx].number = 1;

				/*:::�v���o����S�ē��đœ|���J�E���g�����Z�b�g*/
				lore_do_probe(r_idx);
				r_ptr->r_sights = 0;
				r_ptr->r_deaths = 0;
				r_ptr->r_pkills = 0;
				r_ptr->r_akills = 0;
				r_ptr->r_tkills = 0;

				object_desc(m_name,&o_list[new_o_idx],OD_NAME_ONLY);

				msg_format("%s�͕t�r�_�ƂȂ��Ē��ɕ������I",m_name);

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
			}
			break;
		}
		//�A�C�e���E�g���b�v���m
	case 1:
		{
			if(only_info) return format("�͈�:%d",DETECT_RAD_DEFAULT);
			detect_traps(DETECT_RAD_DEFAULT, TRUE);
			detect_objects_normal(DETECT_RAD_DEFAULT);
			break;
		}
		//�g���b�v�j��
	case 2:
		{
			int rad = 1 + p_ptr->lev / 7;
			if(only_info) return format("�͈�:%d",rad);
			(void)project(0, rad, py, px, 0, GF_KILL_TRAP, (PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE), -1);
			break;
		}
		//�Ӓ�
	case 3:
		{
			if(only_info) return "";
			if ( p_ptr->lev < 45)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
			else
			{
				if (!identify_fully(FALSE)) return NULL;
			}
			break;

		}
		//�A�Җ���
		//�t���A�̕t�r�_��MFLAG_SPECIAL��ݒ肷��B�t���O���ݒ肳�ꂽ�t�r�_��process_monster�̂Ƃ�1/7�̊m���ŏ��ł��U�b�N�ɓ���B
	case 4:
		{
			int i;
			bool flag = FALSE;
			if(only_info) return "";

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (m_ptr->r_idx < MON_TSUKUMO_WEAPON1 || m_ptr->r_idx > MON_TSUKUMO_WEAPON5 ) continue;
				if(object_is_cursed(&o_list[m_ptr->hold_o_idx])) continue;
				m_ptr->mflag |= MFLAG_SPECIAL;
				flag = TRUE;
			}

			msg_print("�t�r�_�Ɍ����ċA�Җ��߂��o�����B");
			if(!flag) msg_print("�������������Ȃ��B");

			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�j���ې�p�Z*/
class_power_type class_power_shinmyoumaru[] =
{
	{1,0,0,FALSE,FALSE,A_WIS,0,0,"�H�ו��o�Ă����I",
		"�H�ו����o�Ă���B�㏞�Ƃ��ā��L�m�R�^�����X�^�[�Ɉ͂܂��(40%)�E�b�ɏP����(30%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{3,3,0,FALSE,FALSE,A_WIS,0,0,"�傫���Ȃ���I",
		"30�^�[���̊ԑ̂��傫���Ȃ�A�r�͂Ƒϋv�͂���������B�㏞�Ƃ��ā������x����(40%)�E����(25%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{6,6,0,FALSE,FALSE,A_WIS,0,0,"���������Ȃ��Ȃ�I",
		"30�^�[���̊ԉΉ��Ɨ�C�ɑ΂���ϐ��𓾂�B�㏞�Ƃ��ā���p���ꎞ����(30%)�E���͈ꎞ����(20%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{9,9,0,FALSE,FALSE,A_WIS,0,0,"�f�����Ȃ���I",
		"30�^�[���̊ԉ�������B�㏞�Ƃ��ā������x����(50%)�E�r�͈ꎞ����(20%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{12,12,0,FALSE,FALSE,A_WIS,0,0,"���䎡��I",
		"HP���ő��1/3�񕜂��؂菝������B�㏞�Ƃ��ā�MP����(40%)�E�����x����(30%)�E�o���l�ꎞ����(20%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{15,15,0,FALSE,FALSE,A_WIS,0,0,"�z���o�Ă����I",
		"�����X�^�[��z���Ƃ��Ĉ�̏�������B�㏞�Ƃ��ā��T���������X�^�[(30%)�E����(20%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{18,18,0,FALSE,FALSE,A_WIS,0,0,"�����Ȃ����̌�����I",
		"���͂̒n�`�E�A�C�e���E�g���b�v�E�����X�^�[�����m����莞�ԓ������F�ƃe���p�V�[�𓾂�B�㏞�Ƃ��ā��m�\�ꎞ����(30%)�E�����ꎞ����(20%)�E���o(10%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{20,20,0,FALSE,FALSE,A_WIS,0,0,"�������o�Ă����I",
		"$300000�𓾂���B�㏞�Ƃ��Čo���l���i�v�I��300000�����B�}�C�i�X�ɂ͂Ȃ�Ȃ��B"},
	{23,23,0,FALSE,FALSE,A_WIS,0,0,"�E�ё���肭�Ȃ���I",
		"��莞�ԉB���\�͂��啝�ɏオ��B�㏞�Ƃ��ā��ϋv�͈ꎞ����(40%)�E����(30%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{25,25,0,FALSE,FALSE,A_WIS,0,0,"�����Ƒ傫���Ȃ���I",
		"��莞�ԑ̂��{���炢�ɑ傫���Ȃ�A�r�͂Ƒϋv�͂��啝�ɑ�������B�㏞�Ƃ��ā��G�L�T�C�g�����X�^�[(30%)�E����(20%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{28,28,0,FALSE,FALSE,A_WIS,0,0,"����S������I",
		"12+1d5�͈̔͂�*�j��*����B�㏞�Ƃ��āA���z��O�̑�j���100+1d150�̃_���[�W(5%)�����N���邱�Ƃ�����B"},
	{30,30,0,FALSE,FALSE,A_WIS,0,0,"�Ƃɂ�����v�ɂȂ���I",
		"��莞�ԁAAC�Ɩ��@�h�䂪�㏸���A���f�ϐ����l�����A�v���[���[���󂯂�قڑS�Ẵ_���[�W��2/3�Ɍ���������B�㏞�Ƃ��ā�MP����(50%)�E�o���l�ꎞ����(30%)�E����(20%)���̂����ꂩ���N����B"},
	{33,33,0,FALSE,FALSE,A_WIS,0,0,"���O�ɂ����������@������I",
		"�^�[�Q�b�g�ɂ��������ꎞ�I���ʂ�S�Ė���������B�㏞�Ƃ���50%�̊m���Ŏ����ɂ��������ꎞ���ʂ��S�ď�����B"},
	{35,35,0,FALSE,FALSE,A_WIS,0,0,"����Ƃ��F�X�S������I",
		"HP�����S�ɉ񕜂��A�o���l�Ɣ\�͒l���񕜂��A�S�ẴX�e�[�^�X�ُ킪������B�㏞�Ƃ��ā��L���r��(50%)�E������(20%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{37,37,0,FALSE,FALSE,A_WIS,0,0,"���̊K�S��������I",
		"�}�b�v�𖾂邭�Ƃ炵�A�S�Ă̒n�`�E�A�C�e���E�g���b�v�E�����X�^�[�����m����B�㏞�Ƃ��ā����o(30%)�E�o���l�啝�ꎞ����(20%)�E�_���W�����̕ǂ������ă����X�^�[���N����(10%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{40,75,80,FALSE,TRUE,A_WIS,0,0,"�P�j��",
		"�^�[�Q�b�g���ӂɌ����Ėh��s�\�ȃr�[���U����A���ŕ��B"},
	{43,43,0,FALSE,FALSE,A_WIS,0,0,"�����Ƃ����Ƃ����Ƒ傫���Ȃ���I",
		"��莞�ԋ��剻����B���剻���͐g�̔\�͂Ɣ�����\�͂������I�ɏ㏸���ꕔ�̋Z�����͂ɂȂ邪�����▂����g�p�s�\�ɂȂ�BU�R�}���h�Ől�ԑ�ɖ߂邱�Ƃ��ł���B�㏞�Ƃ��ā��G�L�T�C�g�����X�^�[(50%)�E�o���l�啝�ꎞ����(20%)�E�����ˑR�ψ�(10%)���̂����ꂩ���N���邱�Ƃ�����B"},

	{45,140,85,FALSE,TRUE,A_CHR,0,0,"���l�̈ꐡ�@�t",
		"�����Z���ԕ��g���A�G����̍U����6/7���������B" },

	{47,47,0,FALSE,FALSE,A_WIS,0,0,"���͂ȑ����i�o�Ă����I",
		"�����ȃA�C�e�������������B�㏞�Ƃ��ā����͂ȃT���������X�^�[(30%)�E���Ẩ��O����(20%)�E�o���l1�`3%�i�v����(10%)���̂����ꂩ���N���邱�Ƃ�����B"},
	{50,50,0,FALSE,FALSE,A_WIS,0,0,"���X�{�X�|���I",
		"���X�{�X��ⓚ���p�œ|���B�S��100�K�ł����g���Ȃ��B�㏞�Ƃ��ā����͏����E�_���W�����̕ǂ������ă����X�^�[���N����E�h��s�\�̃����_���e���|�[�g�E���Ẩ��O�E���͂ȃT���������X�^�[�����S�Ĕ������A���̃Q�[�����ɏ��Ƃ��g�p�s�\�ɂȂ�A����ɏ��Ƃ̌��ʂ�***�S��***������B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


void koduchi_payment(int kind)
{

	switch(kind)
	{
	case 0: //�����Ȃ�
		break;

	case 1: //�L�m�R�Ɉ͂܂��
		{
			int num = randint1(3) + 2;
			bool flag = FALSE;

			for(;num>0;num--)
			{
				if (summon_specific(0, py, px, dun_level, SUMMON_MUSHROOM, (PM_NO_PET | PM_ALLOW_GROUP)) && !flag)
				{
					flag = TRUE;
					msg_print("�H�ו��̒��ɉ������ȕ�������Ă���B");
				}
			}
		}
		break;

	case 2: //�����Ɉ͂܂��
		if (summon_specific(0, py, px, dun_level, SUMMON_ANIMAL, (PM_NO_PET | PM_ALLOW_GROUP | PM_ALLOW_UNIQUE)))
		{
			msg_print("�ǂ�����Ƃ��Ȃ��b������Ă����I");
		}

		break;

	case 3: //�����x����
		msg_print("�������󂢂��E�E");
		(void)set_food(p_ptr->food * (25+randint0(26)) / 100);

		break;

	case 4: //����
		msg_print("���ɃR�~�J���ȉ����o���I");
		aggravate_monsters(0,FALSE);
		break;
	case 5: //���͈ꎞ����
		msg_print("�������r�ꂽ�B");
		if (!p_ptr->sustain_chr) (void)do_dec_stat(A_CHR);
		break;
	case 6: //��p�ꎞ����
		msg_print("�����̂̓������������Ȃ��B");
		if (!p_ptr->sustain_dex) (void)do_dec_stat(A_DEX);
		break;
	case 7: //�r�͈ꎞ����
		msg_print("�����͂�������B");
		if (!p_ptr->sustain_str) (void)do_dec_stat(A_STR);
		break;
	case 8: //MP����
		msg_print("��J���ɏP��ꂽ�E�E");
		p_ptr->csp /= 2;
		p_ptr->redraw |= (PR_MANA);
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);
		break;
	case 9: //�o���l�ꎞ����
		drain_exp(200 + (p_ptr->exp / 100), 200 + (p_ptr->exp / 1000), 75);
		break;
	case 10: //�T���������X�^�[
		if( summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			msg_print("�������P���|�����Ă����I");
		break;
	case 11: //�m�\�ꎞ����
		msg_print("���������ɂ��B");
		if (!p_ptr->sustain_int) (void)do_dec_stat(A_INT);
		break;
	case 12: //�����ꎞ����
		msg_print("���������{�[���Ƃ���B");
		if (!p_ptr->sustain_wis) (void)do_dec_stat(A_WIS);
		break;
	case 13: //���o
		if (!p_ptr->resist_chaos) (void)set_image(p_ptr->image + randint0(50) + 50);
		break;
	case 14: //����
		msg_print("�}�ɑ����d���Ȃ����E�E");
		(void)set_slow(randint1(15) + 15, FALSE);
		break;
	case 15: //�ϋv�ꎞ����
		msg_print("�����t���t������B");
		if (!p_ptr->sustain_con) (void)do_dec_stat(A_CON);
		break;
	case 16: //�o���l30������
		msg_print("���܂Őς񂾌o���������Ă����C������I");
		p_ptr->max_exp = MAX(0,p_ptr->max_exp - 300000);
		if(p_ptr->exp > p_ptr->max_exp ) p_ptr->exp = p_ptr->max_exp;
		check_experience();
		p_ptr->redraw |= (PR_GOLD | PR_EXP);
		redraw_stuff();

		break;
	case 17: //�G�L�T�C�g�����X�^�[
		msg_print("���낵�������_���W�����ɖ苿�����I");
		aggravate_monsters(0,TRUE);
		break;

	case 18: //���͏���
		dispel_player();
		break;
	case 19: //�L���r��
		msg_print("�������s���R�ȂقǓ����X�b�L�������I");
		lose_all_info();
		break;
	case 20: //������
		msg_print("�����q�r������悤�ȉ������������B");
		apply_disenchant(0,0);
		break;
	case 21: //�o���l�啝�ꎞ����
		msg_print("�̂��琶���͂������������C������B");
		lose_exp(p_ptr->exp / 16);
		break;
	case 22: //�ǂ����ׂď�����
		vanish_dungeon();
		break;
	case 23: //�����ˑR�ψ� ���́A���_�֌W
		{
			int muta_lis[] = {75,76,77,78,79,89,109,123,129,136,143,145,151,154,178,185,188};
			gain_random_mutation(muta_lis[randint0(sizeof(muta_lis) / sizeof(int))]);
		}
		break;
	case 24: //���͂ȃ����X�^�[
		activate_hi_summon(py,px,FALSE);
		msg_print("�˔@�A����ȃ����X�^�[�������P���|�����Ă����I");
		break;
	case 25: //���Ẩ��O
		{
			int count = 0;
			msg_print("���Ẩ��O�����������I");
			activate_ty_curse(FALSE, &count);
		}
		break;
	case 26: //�o���l1-3%�i�v����
		msg_print("���܂Őς񂾌o���������Ă����C������I");
		p_ptr->max_exp -= (p_ptr->max_exp / 100 * randint1(3));
		if(p_ptr->exp > p_ptr->max_exp ) p_ptr->exp = p_ptr->max_exp;
		check_experience();
		break;


	default:
		msg_format("ERROR:���Ƒ㏞(%d)����`����Ă��Ȃ�",kind);
	}


}
	
/*:::�j���ۗp���Z�@���Ƃ̏[�U�J�E���g�p�Ƃ���tim_general[0]���g�p����B�܂��ŏI�Z�ŏ��Ƃ��g�p�s�\�ɂȂ��magic_num1[0]��1������B*/
/*:::�㏞�̔����͕ʊ֐��ŏ�������B*/
cptr do_cmd_class_power_aux_shinmyoumaru(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int payment = 0;
	int charge = 0;
	int i;
	int rnd = randint0(100);

	switch(num)
	{

	case 0: //�H���o�Ă���
		{
			int num = 3;
			object_type forge, *q_ptr = &forge;
			int sval[6] = {SV_FOOD_RATION,SV_FOOD_HARDBUSC,SV_FOOD_VENISON,SV_FOOD_CUCUMBER,SV_FOOD_ABURAAGE,SV_FOOD_SLIMEMOLD};

			charge = 1;
			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��Ƌ󂩂�H�ו����~���Ă����B");
			for(i=0;i<num;i++)
			{
				object_prep(q_ptr, lookup_kind(TV_FOOD, sval[randint0(6)] ));
				drop_near(q_ptr, -1, py, px);
			}
			if(rnd < 30) payment = 1;
			else if(rnd < 70) payment = 2;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);
			break;
		}
	case 1://�傫���Ȃ���
		{
			int base = 30;
			charge = 5;

			if(only_info) 
			{
				if(use_itemcard)
					return format("����:%d�^�[��",base);
				else
					return format("�[�U:%d�^�[��",charge);
			}

			msg_print("���Ƃ�U��Ƒ̂�����傫���Ȃ����I");
			set_tim_addstat(A_STR,5,base,FALSE);
			set_tim_addstat(A_CON,5,base,FALSE);

			if(use_itemcard) break;//�A�C�e���J�[�h�g�p���͑㏞�Ȃ�

			if(rnd < 40) payment = 3;
			else if(rnd < 65) payment = 4;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 2://���������Ȃ��Ȃ�
		{
			int base = 30;
			charge = 3;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��ƐS�n�悢��C�����Ȃ����񂾁I");
			set_oppose_cold(base, FALSE);
			set_oppose_fire(base, FALSE);

			if(rnd < 30) payment = 6;
			else if(rnd < 50) payment = 5;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 3://�f�����Ȃ���
		{
			int base = 30;
			charge = 5;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��Ƒ̂��y���Ȃ����I");
			set_fast(base, FALSE);
			if(rnd < 50) payment = 3;
			else if(rnd < 70) payment = 7;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 4://���䎡��
		{
			charge = 7;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��Ƒ̂����ɕ�܂ꂽ�I");
			hp_player(p_ptr->mhp / 3);
			set_stun(0);
			set_cut(0);
			if(rnd < 40) payment = 8;
			else if(rnd < 70) payment = 3;
			else if(rnd < 90) payment = 9;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 5://�z���o�Ă���
		{
			charge = 10;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��Ɖ����̋C�z���������E�E");
			if (!summon_specific(-1, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_FORCE_PET)))
			{
				msg_print("��������������Ȃ������B");
				set_tim_general(charge,FALSE,0,0);
				return ""; //�������s�����̂ő㏞�Ȃ������[�U�͂���
			}
			if(rnd < 30) payment = 10;
			else if(rnd < 50) payment = 4;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 6://�����Ȃ����̌�����
		{
			int base = 30;
			charge = 10;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��Ɗ��o���������܂��ꂽ�E�E");

			map_area(DETECT_RAD_MAP);
			detect_all(DETECT_RAD_DEFAULT);
			set_tim_invis(base, FALSE);
			set_tim_esp(base, FALSE);

			if(rnd < 30) payment = 11;
			else if(rnd < 50) payment = 12;
			else if(rnd < 60) payment = 13;
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 7://�������o�Ă���
		{
			charge = 10000;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���̊肢�̑㏞�͊댯��������Ȃ��B");
			if (!get_check_strict("�{���Ɋ肢�܂����H ", CHECK_OKAY_CANCEL)) return NULL;

			msg_print("���Ƃ�U��ƓV���������󂪍~���Ă����I");
			p_ptr->au += 300000L;
			payment = 16;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 8://�E�ё���肭�Ȃ���
		{
			int base = 30;
			charge = 15;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("�������菬�Ƃ�U�����B");
			set_tim_stealth(randint1(base) + base, FALSE);

			if(rnd < 40) payment = 15;
			else if(rnd < 70) payment = 14;
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 9://�����Ƒ傫���Ȃ���
		{
			int percentage;
			int base = 30;
			charge = 15;

			if(only_info) 
			{
				if(use_itemcard)
					return format("����:%d�^�[��",base);
				else
					return format("�[�U:%d�^�[��",charge);
			}

			percentage = p_ptr->chp * 100 / p_ptr->mhp;

			msg_print("���Ƃ�U��Ƒ̂��{�قǂɑ傫���Ȃ����I");
			set_tim_addstat(A_STR,110,base,FALSE);
			set_tim_addstat(A_CON,110,base,FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			redraw_stuff();		

			if(use_itemcard) break;//�A�C�e���J�[�h�g�p���͑㏞�Ȃ�

			if(rnd < 30) payment = 17;
			else if(rnd < 20) payment = 14;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 10://����S������
		{
			charge = 7;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ȃ��͎v���؂菬�Ƃ�U�����I");

			if(rnd < 95)
			{
				int base = 12;
				int sides = 4;
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
			else if((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
			{
				msg_print("�������n�ʂ������h�ꂽ�����������B");
			}
			else
			{
				if (destroy_area(py, px, 15 + p_ptr->lev + randint0(11), FALSE,FALSE,FALSE))
				{
					msg_print("����ȗ͂��_���W�������Ђ�����Ԃ����I");
					take_hit(DAMAGE_NOESCAPE, 100 + randint1(150), "���Ƃ̊����N��������j��", -1);
				}
				else msg_print("�n�ʂ��傫���h�ꂽ�B");

			}
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 11://�Ƃɂ�����v�ɂȂ�
		{
			int base = 30;
			charge = 30;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��Ƌ��x�Ȗh�ǂ����Ȃ����񂾁I");
			set_oppose_acid(base, FALSE);
			set_oppose_elec(base, FALSE);
			set_oppose_fire(base, FALSE);
			set_oppose_cold(base, FALSE);
			set_oppose_pois(base, FALSE);
			set_magicdef(base, FALSE);

			if(rnd < 50) payment = 8;
			else if(rnd < 80) payment = 9;
			else payment = 14;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}

	case 12://���O�̖��@������
		{

			int m_idx;

			if(only_info) return format("�[�U:%d�^�[��",charge);
			charge = 1;
			if (!target_set(TARGET_KILL)) return NULL;
			m_idx = cave[target_row][target_col].m_idx;
			if (!m_idx) return NULL;
			if (!player_has_los_bold(target_row, target_col)) return NULL;
			if (!projectable(py, px, target_row, target_col)) return NULL;
			dispel_monster_status(m_idx);
			if(rnd < 50) payment = 18;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}

	case 13://����Ƃ��F�X�S������
		{
			charge = 16;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��Ƒ̂��܂΂䂢���ɕ�܂ꂽ�I");
			hp_player(p_ptr->mhp);
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

			if(rnd < 50) payment = 19;
			else if(rnd < 70) payment = 20;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}


	case 14://���̊K�S��������
		{
			charge = 30;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U��Ɛ痢��������ʂ���悤�ɂȂ����I");

			wiz_lite(FALSE);
			(void)detect_traps(255, TRUE);
			(void)detect_monsters_invis(255);
			(void)detect_monsters_normal(255);

			if(rnd < 30) payment = 13;
			else if(rnd < 50) payment = 21;
			else if(rnd < 60) payment = 22;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 15: //�P�j��
		{
			int dice = p_ptr->lev / 5;
			int dir;
			int sides = 3 + adj_general[p_ptr->stat_ind[A_CHR]] / 5;
			int num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 6;
			if(p_ptr->mimic_form == MIMIC_GIGANTIC) dice *= 2;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("ῂ��P��%s�n��������I",(p_ptr->mimic_form==MIMIC_GIGANTIC)?"�����":"");
			fire_blast(GF_PSY_SPEAR, dir, dice, sides, num, 2,(PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM));

			break;
		}

	case 16://�����Ƃ����Ƃ����Ƒ傫���Ȃ���
		{
			int percentage;
			int base = 30;
			charge = 15;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			percentage = p_ptr->chp * 100 / p_ptr->mhp;

			msg_print("���Ƃ�U�����I");
			if(!set_mimic(base+randint1(base), MIMIC_GIGANTIC, FALSE)) return NULL;
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			redraw_stuff();

			if(rnd < 50) payment = 17;
			else if(rnd < 70) payment = 21;
			else if(rnd < 80) payment = 23;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 17:
		{
			int base = 3;
			if (only_info) return format("����:%d+1d%d", base, base);
			set_multishadow(base + randint1(base), FALSE);
			break;
		}
	case 18://���͂ȑ����i�o�Ă���
		{
			charge = 100;

			if(only_info) return format("�[�U:%d�^�[��",charge);

			msg_print("���Ƃ�U�����I");
			acquirement(py, px, randint1(2) + 1, TRUE, FALSE);

			if(rnd < 30) payment = 24;
			else if(rnd < 50) payment = 25;
			else if(rnd < 60) payment = 26;
		
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 19://���X�{�X�|���I
		{

			bool dummy;
			int oberon_idx = 0;
			if(only_info) return format("�[�U:-----");
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (m_ptr->r_idx == MON_TAISAI ) oberon_idx = i;
			}
			
			if(!oberon_idx)
			{
				msg_print("�|���ׂ��҂͂����ɂ͂��Ȃ��B");
				return NULL;
			}

			msg_print("���炭���̊肢�̑㏞�͐��܂������傾�B");
			if (!get_check_strict("�{���Ɋ肢�܂����H ", CHECK_OKAY_CANCEL)) return NULL;

			koduchi_payment(18);
			koduchi_payment(22);
			teleport_player(200,TELEPORT_NONMAGICAL);
			mon_take_hit(oberon_idx,30001,&dummy,"�͕����������Œn���ɒ���ł������I");
			koduchi_payment(25);
			koduchi_payment(24);

			p_ptr->magic_num1[0] = 1;
			msg_print("���ƂɃq�r���������I");
			//�����Ŏ푰�����l�ɖ߂�̂͌��݂̎푰���u���l(�l�ԑ�)�v�̂Ƃ��݂̂ɂ���
			if(p_ptr->prace == RACE_GREATER_LILLIPUT) 
				change_race(RACE_KOBITO,"");

			break;
		}




	default:
		//msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::���Ӑ�p�Z*/
class_power_type class_power_orin[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�L�Ԏ��[",
		"���p�̔L�ԂɃA�C�e��������B�A�C�e�����̐��̓��x���A�b�v�ŏ㏸����B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�L�Ԋm�F",
		"���p�̔L�Ԃ̒�������B"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"�L�Ԏ�o��",
		"���p�̔L�Ԃ���A�C�e�����o���B"},
	{10,0,0,FALSE,TRUE,A_CON,0,0,"�L���b�c�E�H�[�N",
		"�L�̎p�ɂȂ�B�ϐg���͈ړ����x�ƉB���\�͂�����I�ɏ㏸����B�������퓬�\�͂��ቺ���A�قƂ�ǂ̑����i���ꎞ����������A�����E������E���@�E���Z���g���Ȃ��Ȃ�BU�R�}���h����l�^�ɖ߂邱�Ƃ��ł���B"},
	{18,10,0,FALSE,TRUE,A_CON,0,12,"���n���̐j�R",
		"���E���̃����_���ȓG�ɒn�������̃{���g�𐔔����B���x�����オ��Ɩ{���Ə���MP��������B"},
	{24,20,25,FALSE,TRUE,A_CHR,0,0,"�]���r�t�F�A���[",
		"�]���r�t�F�A���[����������B"},
	{32,64,50,FALSE,TRUE,A_CHR,0,0,"�H�l����",
		"���E���̃����X�^�[�S�Ă�����m�������悤�Ǝ��݂�B�d���Ɍ����₷���B����m�����������X�^�[�͖��@����Z���g���Ȃ��Ȃ邪�אڍU���͂��啝�ɏ㏸����B"},
	{45,70,50,FALSE,TRUE,A_CHR,0,0,"�Ή��̎ԗ�",
		"���E���S�Ăɑ΂��Ēn���̋ƉΑ����̍U�����s���B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_orin(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			display_list_inven2();
			return NULL; //���邾���Ȃ̂ōs��������Ȃ�

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("����:�Œ�1000�^�[��");
			if(!set_mimic(1000, MIMIC_CAT, FALSE)) return NULL;

			break;
		}

	case 4:
		{
			bool flag = FALSE;
			int i;
			int dice = 2 + plev / 5;
			int sides = 4 + adj_general[p_ptr->stat_ind[A_CHR]] / 10;
			int num = 3 + plev / 4;
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			msg_print("�n���̐j��������I");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_NETHER, damroll(dice,sides),1, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
		}
		break;

	case 5:
		{
			int slev = p_ptr->lev;
			int k;
			if(slev < 20) slev=20;
			if(only_info) return format("");
			if (summon_specific(-1, py, px, slev, SUMMON_ZOMBIE_FAIRY, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				msg_print("�]���r�p�̗d���������Ăяo�����I");

			break;
		}

	case 6:
		{
			int rad = 0;
			bool flag = FALSE;
			//int num;
			int i;
			int power = p_ptr->lev * 2 + adj_general[p_ptr->stat_ind[A_CHR]] * 2;
			//num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 5;

			if(power < 50) power = 50;
			//if(num < 4) num = 4;

			if(only_info) return format("����:%d",power);

			if(use_itemcard)
				msg_format("�J�[�h�����ʂ̉��삪�N�������Ă����I");
			else 
				msg_format("���Ȃ��͑�ʂ̉����������I");

			project_hack2(GF_POSSESSION, 0, 0, power);
			/*
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_POSSESSION, power,3, rad,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
			*/
			break;
		}
	case 7:
		{
			dice = 16;
			sides = 20 + adj_general[p_ptr->stat_ind[A_CHR]] ;
			if(only_info) return format("����:%dd%d",dice,sides);
			msg_format("�n���̉Ή��������ꂽ�I");
			project_hack2(GF_HELL_FIRE,dice,sides,0);
			break;
		}


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�`���m��p�Z*/
class_power_type class_power_cirno[] =
{
	{1,2,3,FALSE,TRUE,A_DEX,0,5,"�A�C�V�N���V���[�g",
		"��C�����̃{���g����B���x�����オ��ƈЗ͂Ɩ{�����オ�邪����MP���オ��B"},
	{4,4,10,FALSE,TRUE,A_CON,0,2,"�t���[�Y���Ⓚ�@",
		"�אڂ����G�𓀂点�A�Ɋ������̃_���[�W��^����B"},
	{8,8,15,FALSE,TRUE,A_WIS,0,0,"�Ⓚ����",
		"�^�[�Q�b�g���ӂɗ�C�����̃r�[�����O�����B������Ȃ����Ƃ�����B"},
	{12,12,20,FALSE,TRUE,A_DEX,0,5,"�t�F�A���[�X�s��",
		"���͂ɑ΂��Đ����̗�C�����U�����d�|����B�����ɋ߂��قǃ_���[�W���傫���B�������d���Ǝ��s���₷���B"},
	{15,14,30,FALSE,TRUE,A_INT,0,2,"�t���X�g�s���[�Y",
		"�G���ʉ߂��悤�Ƃ���Ɨ�C�����̔������N����㩂��d�|����B���ӏ��ł��d�|������B"},
	{18,20,30,FALSE,TRUE,A_DEX,0,0,"�\�[�h�t���[�U�[",
		"�������̕���ɑ΂��ꎞ�I�ɗ�C������t�^����B"},
	{22,15,35,FALSE,TRUE,A_WIS,0,7,"�A�C�V�N���{��",
		"�Ɋ������̋���ȃ{�[������B�{�[���͉����ɓ�����Ƃ��̏�Ŕ�������B"},

	{24,25,40,FALSE,TRUE,A_STR,0,0,"�X�򐶐�",
		"�אڂ������ꂩ���̒n�`���u�X��v�ɂ���B�X��͒ʂ蔲�����Ȃ����@���󂵂���Ή��ŗn��������ł���B�܂��M�������̍U�������������ꍇ�唚�����邱�Ƃ�����B���̔����̓v���C���[���_���[�W���󂯂�B" },

	{27,30,45,FALSE,TRUE,A_CON,0,0,"�u�ԗⓀ�r�[��",
		"�Ɋ������̃��[�U�[���L�͈͂ɕ��B�З͂�HP��1/3�ɂȂ�B"},
	{31,32,70,FALSE,TRUE,A_DEX,0,8,"�A�C�V�N���}�V���K��",
		"��C�����̃{���g���ʂɔ��˂���B���������e�_�͑_�����኱�΂���B"},
	{36,27,50,FALSE,TRUE,A_STR,50,8,"�O���[�g�N���b�V���[",
		"�אڂ����G������ȕX�̃n���}�[�ŉ�����ĞN�O�Ƃ�����B��C�ϐ��̂Ȃ��G�ɂ�1.5�{�̃_���[�W��^����B�������d���Ǝ��s���₷���B"},
	{40,75,80,FALSE,TRUE,A_INT,0,0,"�t���X�g�L���O",
		"���@�̕X�̌������o��������B�����͔z�������ƂȂ�A�ړ������ɏ�ɗ�C�����U�����s���B�܂������͗d���{�ɂ͓��炸�K�ړ��ŏ�����B"},
	{45,50,80,FALSE,TRUE,A_CHR,0,0,"�p�[�t�F�N�g�t���[�Y",
		"���E���̑S�Ăɑ΂����͂ȋɊ������U������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_cirno(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	//v1.1.32 ���Ă������`���m�̓��Z�p���[�A�b�v
	if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO))
	{
		plev += 10;
		chr_adj += 10;
		if(chr_adj > 50) chr_adj = 50;
	}

	switch(num)
	{

	//�A�C�V�N���V���[�g
	case 0:
		{
			int i;
			int dice = 2 + plev / 7;
			int dir;
			int sides = 3 + chr_adj / 15;
			int num = MIN(10,(1 + plev / 4));
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�X�̒e��������I");
			for (i = 0; i < num; i++) fire_bolt(GF_COLD, dir, damroll(dice, sides));

		}
		break;

	//�t���[�Y���Ⓚ�@
	case 1:
		{

			int y, x;
			int dist;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int dir, damage;

			damage=plev * 2 + chr_adj * 3 ;
			if(damage < 15) damage=15;

			if(only_info) return  format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if(dir==5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(use_itemcard)
					msg_format("%s�ɐG���ĕX�Ђ��ɂ����I",m_name);
				else
					msg_format("%s�͓ˑR�X�Ђ��ɂȂ����I",m_name);
				project(0,0,y,x,damage,GF_ICE,PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID,0);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
		}
		break;
	//�Ⓚ����
	case 2:
		{
			int dir;
			int dice = 2 + p_ptr->lev / 5;
			int sides = 10 + chr_adj / 5;
			if(only_info) return format("����:(%dd%d) * 3",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("��C�̃r�[����������I");
			fire_blast(GF_COLD, dir, dice, sides, 3, 4,(PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM));
			break;

		}
		break;
	//�t�F�A���[�X�s��
	case 3:
		{
			int base = 25 + plev + chr_adj * 2;
			int num = 2 + plev / 30 + chr_adj / 20;
			int rad = 2 + plev / 30;
			int i;
			if(only_info) return format("����:�ő�%d * %d",base/2,num);
			msg_print("�X�̒e������Ȃ����]�����B");
			for (i = 0; i < num; i++) project(0, rad, py, px, base, GF_COLD, PROJECT_KILL | PROJECT_ITEM, -1);
			break;

		}
		break;

	//�t���X�g�s���[�Y
	case 4:
		{
			if(only_info) return format("����:%d + 7d7",plev);
			msg_print("�����ɕX�����d���񂾁E�E");
			explosive_rune();
		}
		break;

	//�\�[�h�t���[�U�[
	case 5:
		{
			int base = p_ptr->lev/2;

			if(base < 10) base = 10;

			if(only_info) return format("����:%d+1d%d",base,base);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			{
				msg_print("����������Ă��Ȃ��Ƃ�����g���Ȃ��B");
				return NULL;
			}

			msg_format("��ɂ��Ă��镐�킪�X�ɕ���ꂽ�B");
			set_ele_attack(ATTACK_COLD, base + randint1(base));
			break;
		}
		break;
	//�A�C�V�N���{��
	case 6:
		{
			int dir;
			int rad = 3;
			int dice = p_ptr->lev / 4 + chr_adj / 3;
			int sides = 10 + p_ptr->lev / 5;
			int base = p_ptr->lev * 2;
			if(p_ptr->lev > 39) rad = 4;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("����ȕX���������I");
			fire_rocket(GF_ICE, dir, base + damroll(dice,sides), rad);
			break;
		}
		//v1.1.68 �X�򐶐�
	case 7:
		{
		int y, x,dir;

		if (only_info) return format("");
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (!cave_naked_bold(y, x))
		{
			msg_print("���̏ꏊ�ɂ͓��Z���g���Ȃ��B");
			return NULL;
		}
		msg_print("���Ȃ��͋��łȕX�����o�����B");
		cave_set_feat(y, x, f_tag_to_index_in_init("ICE_WALL"));

		break;
		}

	//�u�ԗⓀ�r�[��
	case 8:
		{
			int dir;
			int base = p_ptr->chp / 3;
			if(base<1) base = 1;
			if(base > 1600) base=1600;
			if(only_info) return format("����:%d",base);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("�J�[�h���狭��ȗ�C�������ꂽ�I");
			else
				msg_print("�S�̗͂Ⓚ�r�[����������I");
			fire_spark(GF_ICE,dir,base,1);
			break;

		}
	//�A�C�V�N���}�V���K��
	case 9:
		{
			int i;
			int dice = 2 + plev / 8;
			int dir;
			int sides = 3  + chr_adj / 20;
			int num = MIN(30,(10 + plev / 5 + chr_adj / 3));
			if(only_info) return format("����:(%dd%d) * %d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("�J�[�h�����ʂ̕X�򂪕����ꂽ�I");
			else
				msg_print("��ʂ̕X�e����������I");
			fire_blast(GF_COLD, dir, dice, sides, num, 4, 0);
			break;
		}

	//�O���[�g�N���b�V���[
	case 10:
		{
			int y, x;
			int dist;
			int dir;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage=plev * 4 + adj_general[p_ptr->stat_ind[A_STR]] * 10 / 3 + chr_adj * 10 / 3  ;

			if(only_info) return  format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if(dir==5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				if(!(r_ptr->flagsr & RFR_IM_COLD)) damage = damage * 3 / 2;
				if(r_ptr->flagsr & RFR_HURT_COLD) damage *= 2;
				monster_desc(m_name, m_ptr, 0);
				msg_format("����ȕX�̃n���}�[��%s�ɒ@�������I",m_name);
				project(0,0,y,x,damage,GF_MISSILE,PROJECT_KILL,0);
				if(cave[y][x].m_idx && !(r_ptr->flags3 & RF3_NO_STUN))
				{
					msg_format("%s�̓t���t���ɂȂ����B",m_name);
					(void)set_monster_stunned(cave[y][x].m_idx,MON_STUNNED(m_ptr)+ 4 + randint0(4) );
				}
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			break;

		}
	//�t���X�g�L���O
	case 11:
		{
			int max_ice = 2;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(plev > 44) max_ice += (plev - 40) / 5;
			if(max_ice > 5) max_ice = 6;
			if(only_info) return format("�ő�:%d",max_ice);
	
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_CIRNO_ICE) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_ice)
			{
				msg_format("����ȏ�z�u�ł��Ȃ��B",num);
				return NULL;
			}

			(void)summon_named_creature(0, py, px, MON_CIRNO_ICE, PM_EPHEMERA);

		}
		break;
	//�p�[�t�F�N�g�t���[�Y
	case 12:
		{
			int dam = plev * 4 + chr_adj * 10;
			if(only_info) return format("����:%d",dam);
			msg_print("��u�A���͂̑S�Ă̕������������~�����E�E");
			project_hack2(GF_ICE, 0,0,dam);
		}

		break;


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::�ޏ��E�_����p�Z*/
class_power_type class_power_priest[] =
{

	{1,5,0,FALSE,FALSE,A_WIS,0,0,"��[",
		"�_�ɃA�C�e��������ċF��B�A�C�e���̉��l�ƃv���C���[�̃��x���ɉ����Čo���l�𓾂���B���������Ɖ��i�̊��ɓ�����o���l�������B���܂�ςȂ��̂������Ɣ����󂯂邱�Ƃ�����B"},
	{30,60,90,FALSE,TRUE,A_WIS,0,0,"�j��",
		"�����ɑ΂��j����^����B"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_priest(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{
		//��[
		case 0:
		{
			int item;
			int amt;
			s32b price, value;
			bool goodrealm = is_good_realm(p_ptr->realm1);
			int exp;
			u32b flgs[TR_FLAG_SIZE];
			object_type forge;
			object_type *q_ptr;
			object_type *o_ptr;
			bool baditem = FALSE;
			cptr q, s;
			char o_name[MAX_NLEN];

			if(only_info) return format("");

			q = "�ǂ̃A�C�e����_�ɕ����܂���? ";
			s = "�_�ɕ�����ꂻ���ȃA�C�e���������Ă��Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else	o_ptr = &o_list[0 - item];

			amt = 1;
			if (o_ptr->number > 1)
			{
				amt = get_quantity(NULL, o_ptr->number);
				if (amt <= 0) return NULL;
			}

			q_ptr = &forge;
			object_copy(q_ptr, o_ptr);
			q_ptr->number = amt;
			if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND)) q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
			object_desc(o_name, q_ptr, 0);

			if(!get_check(format("%s������܂����H",o_name))) return NULL;

			if(goodrealm)
				msg_format("%s�͓V�ɏ����ď������B",o_name);
			else
				msg_format("%s�͗n������Ēn�ʂ֏������B",o_name);
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
			object_flags(q_ptr,flgs);
			value = object_value_real(q_ptr) * q_ptr->number;
			exp = value * 10 / (100 - p_ptr->lev);
			/*:::�_�l�͎����D��*/
			if(q_ptr->tval == TV_ALCOHOL) exp *= 5;
			//�A�[�e�B�t�@�N�g�͌o���l������
			if(object_is_artifact(q_ptr)) exp *= 3;
			/*:::�ςȃA�C�e���������Ɠ{��*/
			if( !goodrealm && have_flag(flgs, TR_BLESSED) ) baditem = TRUE;
			else if((q_ptr->curse_flags & TRC_HEAVY_CURSE) || (q_ptr->curse_flags & TRC_PERMA_CURSE)) baditem = TRUE;		
			else if(q_ptr->tval == TV_ALCOHOL && q_ptr->sval == SV_ALCOHOL_MARISA) baditem = TRUE;
			else if(q_ptr->tval == TV_CHEST && !q_ptr->pval) baditem = TRUE;
			
			else if( !value && one_in_(13)) baditem = TRUE;
		
			if( !exp  ||  baditem)
			{
				if(goodrealm)
					msg_format("�E�E�Ǝv������V���痎���Ă����B");
				else
					msg_format("�E�E�Ǝv������n�ʂ���f���o���ꂽ�B");
				(void)drop_near(q_ptr, -1, py, px);
			}

			if( baditem )
			{
				bool stop_ty = FALSE;
				int count = 0;
				msg_format("���Ȃ��͐_�̓{��ɐG��Ă��܂����悤���I");
				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				}
				while (one_in_(6));
			}
			else if(exp)
			{
				msg_print("�_�̉����������E�E�E");

				if(p_ptr->prace == RACE_ANDROID)
				{
					if(randint0(30000) < exp) acquirement(py,px,3,TRUE,FALSE);
					else if(randint0(10000) < exp) acquirement(py,px,1,TRUE,FALSE);
				}
				else
				{
					gain_exp(exp);
				} 
			}

		}
		break;
		case 1:
		{
			if(only_info) return format("");
			if (!bless_weapon()) return NULL;
		}
		break;

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�M��p�Z*/
class_power_type class_power_seiga[] =
{
	{1,3,5,FALSE,TRUE,A_DEX,0,0,"�ǖx��",
		"���p���w�ŕǂ�h�A�Ɍ����J����B"},
	{12,20,20,FALSE,TRUE,A_DEX,0,0,"���@��",
		"���p���w�ŏ����@��A����̃t���A�֍s���B�N�G�X�g����_���W�����ŉ��w�ł͎g���Ȃ��B"},
	{22,30,50,FALSE,TRUE,A_STR,0,0,"�A��",
		"���p���w�Œ��������@�蔲���A�_���W�����ƒn��̊Ԃ��s��������B�����܂łɂ̓^�C�����O������B"},
	{30,45,10,FALSE,TRUE,A_INT,0,0,"�g�������F��",
		"���p�̃L�����V�[���Ăяo���B�F���͊K�ړ��ŏ����ߊl�s�\�ňێ��R�X�g�������ȊO�͒ʏ�̔z���Ɠ��������ɂȂ�B�|����Ă����x�ł��Ăяo����B"},
	{36,32,20,FALSE,TRUE,A_WIS,0,16,"�����V���I�O�C",
		"�Ԏq�̗���g�����A���E���̐����̂��鑶�݂ɑ΂������_���ɕ�����̒n�������U�����s���B"},
	{42,45,70,FALSE,TRUE,A_INT,0,0,"�E�H�[�������i�[",
		"�L�͈͂̈꒼����̏���ǂɂ���B�G������œ|������͂ł��Ȃ��B"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_seiga(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{

			int x, y;
			cave_type *c_ptr;
			feature_type *f_ptr, *mimic_f_ptr;
			if(only_info) return format("");

			if (!get_rep_dir2(&dir)) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];
			c_ptr = &cave[y][x];
			f_ptr = &f_info[c_ptr->feat];
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				msg_format("%s�Ɍ����J�����B", f_name + f_info[get_feat_mimic(c_ptr)].name);
				cave_alter_feat(y, x, FF_HURT_ROCK);
				p_ptr->update |= (PU_FLOW);
			}
			else
			{
				msg_print("�����ɂ͌����J�����Ȃ��B");
				return NULL;
			}

			break;

		}
	case 1:
		{
			if(only_info) return format("");

			if(!dun_level)
			{
				msg_print("�_���W�����̒��łȂ��Ƃ��̋Z�͎g���Ȃ��B");
				return NULL;

			}
			else if(quest_number(dun_level))
			{
				msg_print("�E�E�H�Ȃ������Ɍ����J���Ȃ��B");
				return NULL;
			}
			else if(dun_level == d_info[dungeon_type].maxdepth)
			{
				msg_print("�����͊��Ƀ_���W�����̍Ő[�w�̂悤���B");
				return NULL;
			}
			else
			{
				msg_print("���Ȃ��͒n�ʂɌ����J���A���̃t���A�ւƍ~��čs�����E�E");
				p_ptr->leaving = TRUE;
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
			}
			break;
		}
	case 2:
		{
			if(only_info) return format("");
			if (!word_of_recall()) return NULL;
		}
		break;
	case 3:
		{
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_YOSHIKA) && (m_ptr->mflag & MFLAG_EPHEMERA) ) cnt++;
			}
			if(cnt)
			{
				msg_format("���łɖF���͂��̃t���A�ɂ���B",num);
				return NULL;
			}

			(void)summon_named_creature(0, py, px, MON_YOSHIKA, PM_EPHEMERA);


			break;
		}
	case 4:
		{
			monster_type *m_ptr;
			monster_race *r_ptr;
			
			bool flag_fire = FALSE;
			int i,j;
			int fire_num = p_ptr->lev / 5;
			sides = p_ptr->lev;
			base = adj_general[p_ptr->stat_ind[A_CHR]]+10;
			if(only_info) return format("����:(%d+1d%d)* %d",base,sides,fire_num);

			for(j=0;j<fire_num;j++)
			{
				int m_idx = 0;
				int cnt = 0;
				for(i=1;i<m_max;i++)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					if (is_pet(m_ptr)) continue;
					if (!projectable(m_ptr->fy, m_ptr->fx, py, px)) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if(!monster_living(r_ptr)) continue;
						
					cnt++;
					if(one_in_(cnt)) m_idx = i;

				}
				if(!m_idx) break;
				else
				{
					flag_fire = TRUE;
					target_who = m_idx;
					m_ptr = &m_list[m_idx];
					target_row = m_ptr->fy;
					target_col = m_ptr->fx;

					fire_ball(GF_NETHER,5,base + randint1(sides),0);
				}

			}
			if(!flag_fire) msg_print("�߂��ɂ͕W�I�ɂȂ�҂����Ȃ��悤���B");
			break;
		}
	case 5:
		{
			if (only_info) return "";
			if (!get_aim_dir(&dir)) return NULL;
			fire_spark(GF_STONE_WALL,dir,0,1);
		
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::����p�Z*/
class_power_type class_power_momizi[] =
{
	{5,10,10,FALSE,FALSE,A_INT,0,0,"���ӊ��m",
		"���͂̒n�`�����m����B"},
	{15,20,20,FALSE,FALSE,A_INT,0,0,"�S���m",
		"���͂̃A�C�e���A�g���b�v�A�K�i�A�h�A�A�����X�^�[�����m����B"},
	{24,25,35,FALSE,FALSE,A_STR,0,0,"���C�r�[�Y�o�C�g",
		"�אڂ��������X�^�[��̂Ƀ_���[�W��^����B�őϐ��̂Ȃ��G�͍����A�N�O���邱�Ƃ�����B"},
	{30,60,90,FALSE,FALSE,A_INT,0,0,"�痢��",
		"�t���A�S�Ă̒n�`�ƃA�C�e�������m����B"},
	{40,45,75,FALSE,FALSE,A_CHR,0,0,"�G�N�X�y���[�Y�J�i��",
		"���͂̓G�����݂̃t���A����Ǖ����悤�Ǝ��݂�B��̒Ǖ����邲�Ƃɋ͂��ȃ_���[�W���󂯂�B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_momizi(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return  format("�͈�:%d",DETECT_RAD_MAP);
			map_area(DETECT_RAD_MAP);
			break;
		}
	case 1:
		{
			if(only_info) return  format("�͈�:%d",DETECT_RAD_DEFAULT);
			detect_all(DETECT_RAD_DEFAULT);
			break;
		}
	case 2:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = 50 + plev * 2 + adj_general[p_ptr->stat_ind[A_STR]] * 5;

			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%s�ɉ��˂����Ă��I",m_name);
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_MISSILE,PROJECT_KILL,-1);
				if(!m_ptr->r_idx) break;

				if(!(r_ptr->flagsr & (RFR_IM_POIS | RFR_RES_ALL)))
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_OLD_CONF,PROJECT_KILL,-1);
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_STUN,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	

	case 3:
		{
			if(only_info) return  format("");
			wiz_lite(FALSE);
			break;
		}
	case 4:
		{
			int rad = 3 + plev / 20;
			int power = plev * 8;
			if( power < 200) power = 200;

			if(only_info) return  format("�͈�:%d ����:�ő�%d",rad,power/2);

			msg_print("���Ȃ��͋���ȈЈ���������I");
			project(0,rad,py,px,power,GF_GENOCIDE,(PROJECT_KILL|PROJECT_HIDE),-1);

		}
		break;
	default:
			if(only_info) return  format("������");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::��������p�Z*/
class_power_type class_power_koishi[] =
{
	{7,10,10,TRUE,FALSE,A_STR,0,0,"�L���b�`�A���h���[�Y",
		"�߂��̓G�ɍU����������΂��B����ȓG�ɂ͌��ʂ������B"},
	{15,20,35,FALSE,TRUE,A_CHR,0,0,"�X�e�B���M���O�}�C���h",
		"���͂̃����_���Ȓn�_�ɐ��_�����̔����𕡐��N�����B�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ������B"},
	{26,48,45,FALSE,TRUE,A_CHR,0,0,"����c�l�����Ă��邼",
		"�^�C�����O��Ɏ��E���̃����_���ȓG��̂ɐ��_�U�������_���[�W��^����B�ő�O�񔭓�����B"},
	{33,30,60,FALSE,TRUE,A_DEX,0,0,"�R���f�B�V�����h�e���|�[�g",
		"��ԋ߂��G�ׂ̗Ƀe���|�[�g���A���̂܂܈ꌂ��������B���_�������Ȃ��G�͑ΏۂɂȂ�Ȃ��B�������d���Ǝg���Ȃ��B"},
	{40,50,65,FALSE,TRUE,A_CHR,0,0,"�C�h�̊J��",
		"���E���S�Ăɐ��_�U�����d�|���N�O�A�����A�����A����m��������B�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ������B"},

	{44,65,70,FALSE,TRUE,A_CHR,0,0,"�X�[�p�[�G�S",
		"���E���S�Ăɐ��_�U�����d�|���ړ��֎~�A�S�\�͒ቺ������B�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ��Ȃ��B" },

	{48,80,75,FALSE,TRUE,A_CHR,0,0,"�َ��̖�",
		"�אڂ����G��̂ɋ��͂Ȑ��_�U���Ǝ��ԋt�]�U�����d�|����B�ʏ�̐��_�������Ȃ��I�ɂ͌��ʂ������B"},

		/*
	{40,70,65,FALSE,TRUE,A_CHR,0,0,"���t���N�X���[�_�[",
		"�����̐^�����甽���v���ɍ��G���s���A���_�����̃��[�U�[�U�����d�|����B"},
		*/


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


//p_ptr->magic_num1[0]���u����c�l�����Ă��邼�v�p�Ɏg��
cptr do_cmd_class_power_aux_koishi(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
		//�L���b�`�A���h���[�Y�@�s���̓d�ԑ����U��
	case 0:
		{
			damage = plev * 2;
			if(damage < 20) damage=20;
			if (only_info) return format("����:%d",damage);

			project_length = 2;
			if (!get_aim_dir(&dir)) return NULL;
			fire_ball_hide(GF_TRAIN, dir, damage, 0);

			break;
		}
		//�X�e�B���M���O�}�C���h
	case 1:
		{
			int rad = 2 + plev / 30;
			damage = plev + adj_general[p_ptr->stat_ind[A_CHR]] * 3;
			if(only_info) return format("����:%d*(10+1d10)",damage);
			cast_meteor(damage, rad, GF_REDEYE);

			break;
		}

		//����c�l�����Ă��邼
	case 2:
		{
			base = 30 + plev + adj_general[p_ptr->stat_ind[A_CHR]] * 2;
			if(only_info) return format("����:%d*2(��񂠂���)",base);
			msg_print("���͂ɓ����ʂ����l�e�����̂��o�������E�E");
			p_ptr->magic_num1[0] = 3;
			break;
		}

		//�R���f�B�V�����h�e���|�[�g
	case 3: 
		{
			monster_type *m_ptr;
			monster_race *r_ptr;

			int dist = 99;
			int range = 25 + plev / 2;
			int tx,ty;
			if(only_info) return  format("����:%d",range);
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				r_ptr = &r_info[m_ptr->r_idx];
				if (is_pet(m_ptr)) continue;
				if (is_friendly(m_ptr)) continue;
				if(r_ptr->flags2 & RF2_EMPTY_MIND) continue;
				dist_tmp = distance(py,px,m_ptr->fy,m_ptr->fx);
				if(dist_tmp > range) continue;
				if(dist_tmp < dist)
				{
					dist = dist_tmp;
					tx = m_ptr->fx;
					ty = m_ptr->fy;
				}
			}
			if(dist == 99)
			{
				msg_print("�N�����Ȃ��̋߂��ɂ��Ȃ��E�E");
			}
			else
			{
				teleport_player_to(ty,tx,0L);
				if(distance(py,px,ty,tx)==1)
				{
					py_attack(ty,tx,HISSATSU_ATTACK_ONCE);
				}
				else
				{
					msg_print("���s�I");
				}
			}

			break;	
		}

		//�C�h�̊J��
	case 4:
		{
			base = plev + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("����:%d+1d%d",base, base);
			msg_format("����ȏՓ����J�����ꂽ�I");
			project_hack2(GF_REDEYE,1, base,base);
			//v1.1.95
			project_hack2(GF_BERSERK, 1, base, base);

			break;
		}
		//�X�[�p�[�G�S
	case 5:
	{
		base = plev + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
		if (only_info) return format("����:%d+1d%d", base, base);
		msg_format("����ȗ}�����������ꂽ�I");

		//v1.1.95
		project_hack2(GF_SUPER_EGO, 1, base, base);

		break;
	}


	case 6: //�َ��̖�
		{
			int x,y;
			base = chr_adj * 20 + plev * 4 ;
			if(base < 300) base=300;
			if(only_info) return format("�ő�%d",base);

			if (!get_rep_dir2(&dir)) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];
			if(!in_bounds(y,x)) return NULL;

			if (cave[y][x].m_idx)
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				if(!use_itemcard)
					msg_format("���Ȃ���%s�ɐG��A�����ƌ��߂��E�E",m_name);

				project(0, 0, y, x, base / 2 , GF_TIME, flg, -1);
				project(0, 0, y, x, base / 2 , GF_COSMIC_HORROR, flg, -1);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}
			break;

		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�A�[�`���[��p�Z*/
class_power_type class_power_archer[] =
{
	{5,0,50,FALSE,FALSE,A_DEX,0,0,"���",
		"������B�߂��ɐX�ђn�`���Ȃ��ƍ��Ȃ��B"},
	{10,0,60,FALSE,FALSE,A_DEX,0,0,"�{���g����",
		"�N���X�{�E�̖�����B�߂��ɐX�ђn�`���Ȃ��ƍ��Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_archer(int num, bool only_info)
{
	feature_type *f_ptr;
	int dir;

	switch(num)
	{

	case 0:
	case 1:
		{
			int xx,yy;
			object_type forge;
			object_type *o_ptr = &forge;

			if(only_info) return format("");

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			yy = py + ddy[dir];
			xx = px + ddx[dir];
			if(!cave_have_flag_bold((yy), (xx), FF_TREE)) 
			{
				msg_format("�����ɂ͖؂��Ȃ��B");
				return NULL;
			}
			object_prep(o_ptr, (int)lookup_kind((num==0)?TV_ARROW:TV_BOLT, SV_AMMO_LIGHT));
			o_ptr->number = 1 + randint0(p_ptr->lev / 5);
			o_ptr->to_d = p_ptr->lev / 5;
			o_ptr->to_h = p_ptr->lev / 5;
			object_aware(o_ptr);
			object_known(o_ptr);
			o_ptr->discount = 99;
			msg_format("���������B");
			(void)drop_near(o_ptr, -1, py, px);
			if(one_in_(10)) cave_set_feat(yy, xx, one_in_(3) ? feat_brake : feat_grass);

			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�����W���[��p�Z*/
class_power_type class_power_ranger[] =
{
	{10,0,10,FALSE,FALSE,A_DEX,0,0,"�������ւ�",
		"�ʏ�̔����̍s���͏���ő����ύX���s���B�������E��ƍ���̑����̂݁B"},
	{25,16,50,FALSE,FALSE,A_INT,0,0,"����",
		"���E���̓G�̔\�͂ɂ��ďڍׂȏ��𓾂�B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_ranger(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			do_cmd_wield(TRUE);
			new_class_power_change_energy_need = 50;
			break;
		}
	case 1:
		{
			if(only_info) return format("");
			msg_format("���͂̓G�𒲍������E�E");
			probing();
			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�T���Ɛ�p�Z*/
class_power_type class_power_rogue[] =
{
	{10,7,20,FALSE,FALSE,A_INT,0,0,"�g���b�v���m",
		"���ӂ̃g���b�v�����m����B"},

	{15,5,40,FALSE,FALSE,A_DEX,0,0,"�g���b�v����",
		"�g���b�v�𔭓�������r�[������B���������g���b�v�Ƀ����X�^�[���������ނ��Ƃ��ł���B�v���C���[���͈͓��ɂ���ƃ_���[�W���󂯂�B" },

	{20,15,20,TRUE,FALSE,A_DEX,30,0,"�q�b�g���A�E�F�C",
		"�G�ɍU�����A���̌��u�ŗ��E����B���s���邱�Ƃ�����B�������d���Ǝ��s���₷���B"},
	{30,20,50,FALSE,FALSE,A_INT,0,0,"�Ӓ�",
		"�������Ă���A�C�e�����Ӓ肷��B�S�Ă̔\�͂�m�邱�Ƃ��ł���킯�ł͂Ȃ��B"},
	{40,27,60,FALSE,FALSE,A_INT,0,0,"����",
		"���E���̓G�̔\�͂ɂ��ďڍׂȏ��𓾂�B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_rogue(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	feature_type *f_ptr = &f_info[cave[py][px].feat];
	bool in_water = FALSE;

	if(have_flag(f_ptr->flags, FF_WATER)) in_water = TRUE;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("�͈�:%d",DETECT_RAD_DEFAULT);			
			detect_traps(DETECT_RAD_DEFAULT, TRUE);
			break;
		}


	case 1:
	{
		int range = 5 + p_ptr->lev / 5;
		if (only_info) return format("�͈�:%d", range);
		if (!get_aim_dir(&dir)) return NULL;

		msg_print("���Ȃ��͍I�݂Ƀ��[�v�𑀂���...");
		fire_beam(GF_ACTIV_TRAP, dir, 0);


		break;
	}

	case 2:
		{
			if(only_info) return format("");	
			if(!hit_and_away()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if (!ident_spell(FALSE)) return NULL;

			break;
		}
	case 4:
		{
			if(only_info) return format("");
			msg_format("���͂̓G�𒲍������E�E");
			probing();
			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}
/*:::�킩�����P��p�Z*/
class_power_type class_power_wakasagi[] =
{
	{5,7,20,FALSE,FALSE,A_DEX,0,3,"�e�C���t�B���X���b�v",
		"�������̃{���g����B���x�����㏸����ƃ{�[���ɂȂ�B����Ŏg�p����ƈЗ͂��オ��B"},
	{16,15,30,FALSE,TRUE,A_STR,0,5,"�X�P�[���E�F�C�u",
		"�����𒆐S�ɐ������̃{�[���𔭐�������B����Ŏg�p����ƈЗ͂��オ��B"},
	{25,20,50,FALSE,TRUE,A_CHR,0,0,"�X�N�[���I�u�t�B�b�V��",
		"���͂̐��������𖣗�����̂��̂��B�̂��Ă���Ԃ�MP�����������B�̂��Ă���Ƃ��ɂ�����x���s����Ɖ̂��~�߂�B" },

	{32,33,50,TRUE,FALSE,A_STR,0,10,"���i�e�B�b�N���b�h�X���b�v",
		"�אڂ��Ă��郂���X�^�[�S�ĂɃ_���[�W��^���A�����ϐ��������Ȃ��񃆃j�[�N�����X�^�[�����m���ō���������B" },

	{36,40,70,FALSE,TRUE,A_CHR,0,0,"�l���̉�",
		"���͂̃����X�^�[�𖣗��E���@�͒ቺ������̂��̂��B�̂��Ă���Ԃ�MP�����������B�̂��Ă���Ƃ��ɂ�����x���s����Ɖ̂��~�߂�B" },

	{40,90,80,FALSE,TRUE,A_CHR,0,0,"�t�؂̑�r�g",
		"���E���ɋ��͂Ȑ������U�����s�����͂̒n�`�𐅂ɂ���B�̗͂������Ă���ƈЗ͂��オ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_wakasagi(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	feature_type *f_ptr = &f_info[cave[py][px].feat];
	bool in_water = FALSE;

	if(have_flag(f_ptr->flags, FF_WATER)) in_water = TRUE;

	switch(num)
	{

	case 0:
		{
			int rad = 2;
			if(p_ptr->lev > 39) rad = 3;
			dice = 2 + p_ptr->lev / 6;
			sides = 8 ;
			base = p_ptr->lev / 2 + 1;
			if(in_water)
			{
				dice *= 2;
				base *= 2;
			}
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���Ȃ��͐��̉���΂����B");
			if(p_ptr->lev < 19) fire_bolt(GF_WATER, dir, base + damroll(dice,sides));
			else  fire_ball(GF_WATER, dir, base + damroll(dice,sides), rad);
			break;

		}
	case 1:
		{
			int rad = 2 + p_ptr->lev / 20;
			base = p_ptr->lev * 4;
			if(base < 60) base = 60;

			if(in_water)
			{
				rad *= 2;
				base *= 2;
			}
			if(only_info) return format("����:�`%d",base / 2);	
			msg_format("���Ȃ��͑傫�����𒵂˔�΂����B");
			project(0, rad, py, px, base, GF_WATER, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}



	case 2://v1.1.75 �X�N�[���I�u�t�B�b�V��
	{

		int power = p_ptr->lev + adj_general[p_ptr->stat_ind[A_CHR]] * 3;
		if (only_info) return  format("����:%d",power);


		//�̂��Ă���Ƃ��s���̓R�X�g������Œ��f
		if (p_ptr->magic_num2[0] == MUSIC_NEW_WAKASAGI_SCHOOLOFFISH)
		{
			stop_singing();
			return NULL;

		}

			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_WAKASAGI_SCHOOLOFFISH, SPELL_CAST);

		break;
	}

	case 3: //v1.1.75 ���i�e�B�b�N���b�h�X���b�v
	{
		damage = p_ptr->lev * 4 + adj_general[p_ptr->stat_ind[A_STR]] * 10;

		if (only_info) return format("����:%d", damage / 2);

		msg_print("���Ȃ��͔͗C���ɔ���U��񂵂��I");
		project(0, 1, py, px, damage, GF_MISSILE, PROJECT_HIDE | PROJECT_KILL, -1);
		project(0, 1, py, px, damage, GF_OLD_CONF, PROJECT_HIDE | PROJECT_KILL, -1);


	}
	break;

	case 4://v2.0.2 �l���̉�
	{

		int power = p_ptr->lev + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
		if (only_info) return  format("����:%d", power);

		//�̂��Ă���Ƃ��s���̓R�X�g������Œ��f
		if (p_ptr->magic_num2[0] == MUSIC_NEW_WAKASAGI_NINGYO)
		{
			stop_singing();
			return NULL;
		}

		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_WAKASAGI_NINGYO, SPELL_CAST);

		break;
	}

	case 5:
		{
			int mult = 1 + ((p_ptr->mhp - p_ptr->chp) * 100 / p_ptr->mhp) / 10;
			int rad = mult;
			base = p_ptr->lev * 2 * mult;
			if(base < 100) base = 100;
			dice = mult;
			if(rad>9) rad=9;
			sides = adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("����:%dd%d+%d",dice,sides,base);
			msg_format("���Ȃ��͑�Ôg�������N�������I");
			project(0, rad, py, px, mult+2, GF_WATER_FLOW, PROJECT_GRID, -1);

			project_hack2(GF_WATER,dice,sides,base);

			if(!in_water)
			{
				f_ptr = &f_info[cave[py][px].feat];
				if(have_flag(f_ptr->flags, FF_WATER) && p_ptr->pclass == CLASS_WAKASAGI)
					msg_print("�͂��N���o��C������I");
				p_ptr->update |= PU_BONUS;
				update_stuff();
			}

			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::��������p�Z*/
/*:::p_ptr->magic_num2[0-27]�𖂖@�J���ς݃t���O�A*/
/*:::p_ptr->magic_num1[0-27]���Ƃɍ��u���Ă��閂�@�̌��A*/
/*:::p_ptr->magic_num2[40-67]�����ݏ������Ă��閂�@�̌��Ɋ��蓖�Ă�B*/
/*:::���@�̊J���ɂ�marisa_magic_power[]�������B*/
/*:::0-27��marisa_magic_table[]�̓Y���Ɉ�v����B*/
/*:::p_ptr->tim_general[0]���}�W�b�N�A�u�\�[�o�[�̃J�E���g�Ɏg���B*/
/*:::*/
///mod160103 p_ptr->magic_num1[30-37]���`�[�g�R�}���h����marisa_magic_power[]�ɂ܂Ƃ߂邱�Ƃɂ���

//v2.0.1 ��p���i�̂Ƃ��J�[�h���l�Ɠ������Z(class_power_card_dealer)�ɂȂ�
//�J�[�h�̔����̃��X�g��magic_num2[80-89]���A�J�[�h����̔��p�ς݃t���O��magic_num1[80-83]���g���Ă���.
//�܂��u���{��`�̃W�����}�v�ɂ�鉿�i�������W����magic_num2[90-99]�ɋL�^

class_power_type class_power_marisa[] =
{
	{1,0,0,FALSE,FALSE,A_INT,0,0,"���@�ɂ��Ċm�F����",
		"����̖��@�̌��ʂƕK�v�Ȗ��͂̎�ނ��m�F����B"},
	{1,0,0,FALSE,TRUE,A_INT,0,0,"���@���g��",
		"����̖��@���g���B���s�͂��Ȃ��B�Ƃ��玝���Ă������̂����g���Ȃ��B"},
	{1,1,0,FALSE,TRUE,A_INT,0,0,"�}�W�b�N�E�~�T�C��",
		"�������̃{���g����B"},
	{5,0,20,FALSE,FALSE,A_INT,0,0,"�L�m�R�Ӓ�",
		"�������̃L�m�R���Ӓ肷��B"},
	{10,7,25,FALSE,FALSE,A_INT,0,0,"��̓���",
		"�߂��̃A�C�e�������m����B"},
	{15,8,30,FALSE,TRUE,A_INT,0,3,"�C�����[�W�������[�U�[",
		"�M�������̃r�[������B"},
	{20,10,50,FALSE,TRUE,A_DEX,0,0,"�ȈՖ��͒��o",
		"�f�ޗނ��疂�@�̑f�ƂȂ閂�͂𒊏o����B����������ōs���̂ɔ�ׂĒ��o�ʂ�������B"},
	{25,12,70,FALSE,FALSE,A_INT,0,0,"�ڗ���",
		"�A�C�e������Ӓ肷��B���x��40�ȍ~��*�Ӓ�*�ɂȂ�B"},
	{30,25,60,FALSE,TRUE,A_INT,0,5,"�}�W�b�N�E�i�p�[��",
		"���͑����̃��P�b�g����B"},

	{40,50,80,FALSE,TRUE,A_CHR,0,14,"�}�X�^�[�X�p�[�N",
		"�j�M�����̋��͂ȃr�[������B"},
	{45,250,75,FALSE,TRUE,A_DEX,0,0,"�����ւ����@",
		"���̏�ŒT�����_�ɃA�N�Z�X���A�A�C�e������ւ��⑕���̕ύX���ł���B��x�ł����_����o����I������̂Œ��ӁB"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_marisa(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];


	switch(num)
	{

	case 0:
		{
			if (only_info) return format("");
			check_marisa_recipe();
			return NULL;//�m�F�݂̂Ȃ̂ōs��������Ȃ�
			break;
		}
	case 1:
		{
			int spell_num;
			if (only_info) return format("");
			spell_num = choose_marisa_magic(CMM_MODE_CAST);
			if(spell_num < 0) return NULL;



			if(!use_marisa_magic(spell_num,FALSE)) return NULL;
			p_ptr->magic_num1[spell_num + MARISA_HOME_CARRY_SHIFT] -= 1;

			break;
		}

	case 2: //�}�W�b�N�~�T�C��
		{
			int dice = 3 + (plev / 5);
			int sides = 4 + chr_adj / 12;
			if (only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			fire_bolt(GF_MISSILE, dir, damroll(dice, sides));


		}
		break;

	case 3: //�L�m�R�Ӓ�
		{
			if (only_info) return format("");
			if (!ident_spell_2(1)) return NULL;
		}
		break;
	case 4:
		{
			if(only_info) return "";
			msg_print("���Ȃ��̓R���N�^�[�̊��𓭂������E�E");
			detect_objects_normal(DETECT_RAD_DEFAULT);
			break;
		}
	case 5:
		{
			int dam = 20 + plev * 2 + chr_adj * 2;
			if (only_info) return format("����:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			fire_beam(GF_LITE, dir, dam);
			break;
		}
	case 6:
		{
			int rate = plev + 30;
			if (only_info) return format("����:%d%%",rate);
			if(!marisa_extract_material(FALSE)) return NULL;
		
			break;
		}
	case 7:
		{
			if(only_info) return format("");
			if(plev < 40)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
			else
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 8: //�}�W�b�N�i�p�[�� 200-400
		{
			int sides = 100 + chr_adj * 5;
			int base = 50+plev * 4;
			if(only_info) return format("����:%d+1d%d",base,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�����̖�r�𓊂������B");
			fire_rocket(GF_MANA,dir, base + randint1(sides),3);

			break;
		}

	case 9: //�}�X�p400-800
		{
			int dice = 15 + p_ptr->lev / 2;
			int sides = 20 + chr_adj / 2;
			if(only_info) return format("����:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�}�X�^�[�X�p�[�N��������I");
			fire_spark(GF_NUKE,dir, damroll(dice,sides),1);

			break;
		}
	case 10: //�����ւ����@
		{
			if(only_info) return format("");

			msg_print("���Ȃ��͉��ɕ�܂ꂽ�E�E");
			hack_flag_access_home = TRUE;
			do_cmd_store();
			hack_flag_access_home = FALSE;
			msg_print("���������Ƃ��Ȃ��̑����͗l�ς�肵�Ă����I");

			break;
		}


	default:
		if(only_info) return format("");
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::����p�Z*/
class_power_type class_power_suika[] =
{
	{3,10,20,FALSE,TRUE,A_DEX,0,0,"�A�C�e��������",
		"�����ɂ���A�C�e���𑫌��Ɉ����񂹂�B"},
	{15,25,45,FALSE,FALSE,A_CON,0,5,"�Ή��̃u���X",
		"����HP��1/3�̈З͂̉��̃u���X��f���B"},
	{20,30,40,FALSE,TRUE,A_WIS,0,0,"�~�b�V���O�p���[",
		"��莞�ԑ̂��傫���Ȃ�A�r�͂Ƒϋv�͂���������B"},
	{26,25,50,FALSE,FALSE,A_STR,0,3,"�ˉB�R����",
		"���𓊂����A�������{�[���U��������B�З͂͘r�͂Ɉˑ�����B"},
	{30,50,60,FALSE,TRUE,A_INT,0,0,"�a�X����",
		"��莞�Ԗ��ɕω�����B�����U���A�؂菝�E���΁E�{���g�������ɂ����Ȃ�B���\�͂��啝�ɏ㏸����B�������g�̔\�͂��啝�ɒቺ�����f�ƑM���Ɨ򉻂̍U���Ɏキ�Ȃ��Ă��܂��B"},
	{35,30,70,FALSE,FALSE,A_DEX,0,0,"�S����̏p",
		"�����X�^�[��̂�������������񂹂ĒZ���Ԉړ��s�\�ɂ��A����ɍ��m���Ŗ��@�͒ቺ��Ԃɂ���B" },
	{38,48,65,FALSE,TRUE,A_DEX,0,0,"�C���v�X�E�H�[��",
		"���^�̕��g�𐔑̌Ăяo���B������K����o��ƕ��g�͏�����B"},

	{43,86,70,TRUE,TRUE,A_STR,0,20,"�f���̉B�`�S",
		"���͂̃O���b�h�S�Ăɒʏ�U�����s���B���̂Ƃ��B���\�͂ɉ������m���œG�ɋ���Ȉꌂ��^����B" },

	{46,80,70,FALSE,TRUE,A_WIS,0,0,"�~�b�V���O�p�[�v���p���[",
		"��莞�ԋ��剻����B���剻���͐g�̔\�͂������I�ɏ㏸���邪�����Ɩ�����g�p�ł��Ȃ��BU�R�}���h�ŋ��剻�������ł���B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};



cptr do_cmd_class_power_aux_suika(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			int weight = p_ptr->lev * 30;
			if (only_info) return format("�d��:%d",weight);
			if (!get_aim_dir(&dir)) return NULL;
			fetch(dir, weight, FALSE);
			break;
		}
	case 1: 
		{
			int dam;
			dam = p_ptr->chp / 3;
			if(dam<1) dam = 1;
			if(dam > 1600) dam=1600;

			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("�J�[�h���牊���f���o���ꂽ�I");
			else
				msg_print("���Ȃ��͉���f�����I");
		
			fire_ball(GF_FIRE, dir, dam, (p_ptr->lev > 35 ? -3 : -2));
			break;	
		}
	case 2:
		{
			int time;
			int percentage;
			base = p_ptr->lev/2;
			sides = p_ptr->lev/2;
			
			if(only_info) return format("����:%d+1d%d",base,sides);
			time = base + randint1(sides);
			percentage = p_ptr->chp * 100 / p_ptr->mhp;
			msg_print("�̂��傫���Ȃ����I");
			set_tim_addstat(A_STR,5,time,FALSE);
			set_tim_addstat(A_CON,5,time,FALSE);		
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			redraw_stuff();
			break;
		}
	case 3:
		{
			int rad = p_ptr->stat_ind[A_STR] / 10;
			dice = p_ptr->lev / 5;
			sides = p_ptr->stat_ind[A_STR]+3;
			base = p_ptr->lev;

			if(rad < 1) rad = 1;
			if(base < 30) base = 30;
			if(dice < 5) dice = 5;

			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			damage = damroll(dice,sides) + base;
			msg_print("���Ȃ��͑��𓊂������I");
			fire_ball(GF_ARROW,dir,damage,rad);

			break;
		}

	case 4:
		{
			int time;
			base = p_ptr->lev/2;
			sides = p_ptr->lev/2;
			
			if(only_info) return format("����:%d+1d%d",base,sides);
			if(!set_mimic(base+randint1(base), MIMIC_MIST, FALSE)) return NULL;
			break;
		}

	case 5:
	{
		int idx;
		monster_type *m_ptr;
		char m_name[80];
		int power = p_ptr->lev * 8;

		if (only_info) return format("����:%d",power);

		if (!teleport_back(&idx, TELEPORT_FORCE_NEXT)) return NULL;
		m_ptr = &m_list[idx];
		monster_desc(m_name, m_ptr, 0);
		if (distance(py, px, m_ptr->fy, m_ptr->fx) > 1) msg_format("%s�������񂹂�̂Ɏ��s�����B", m_name);
		else
		{
			msg_format("%s�ɋS����̍����������Ė��͂��U�炵���I", m_name);
			project(0, 0, m_ptr->fy, m_ptr->fx, power, GF_DEC_MAG, PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE, -1);
			project(0, 0, m_ptr->fy, m_ptr->fx, 10, GF_NO_MOVE, PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE, -1);
		}
	}
	break;


	case 6:
		{
			bool flag = FALSE;
			int i,num;
			if(only_info) return format("");
			num = 5 + randint0(5);
			msg_print("���Ȃ��͔��𐔖{�����Đ������E�E");
			for(i=0;i<num;i++) if(summon_named_creature(0, py, px, MON_MINI_SUIKA, PM_EPHEMERA)) flag = TRUE;
			if(!flag) msg_print("�����������o�Ă��Ȃ������B");
			break;
		}

	case 7: //v2.0.2 �f���̉B�`�S
	{
		if (only_info) return format("");

		msg_print("���Ȃ��͓ˑR��\�ꂵ���I");
		whirlwind_attack(HISSATSU_ONGYOU);
	}
	break;


	case 8:
		{
			int time;
			int percentage;
			base = p_ptr->lev/2+10;
			sides = 25;
			
			if(only_info) return format("����:%d+1d%d",base,sides);
			percentage = p_ptr->chp * 100 / p_ptr->mhp;
			set_mimic(base + randint1(sides), MIMIC_GIGANTIC, FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			redraw_stuff();
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::���C�W�E�n�C���C�W�̖��͐H��*/
class_power_type class_power_mage[] =
{
	{25,1,70,FALSE,TRUE,A_INT,0,0,"���͐H��",
		"��E���@�_�E���b�h���疂�͂��z�����MP���񕜂���B��������邱�Ƃ�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_mage(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if (!eat_magic(p_ptr->lev * 2)) return NULL;
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�ؐ��p�Z*/
class_power_type class_power_kasen[] =
{
	{3,3,10,FALSE,TRUE,A_INT,0,0,"���̋�",
		"����ʂ����o���A���݂��镔���𖾂邭����B"},
	{8,10,20,FALSE,TRUE,A_DEX,0,0,"�A�C�e��������",
		"�����ɂ������͂��̃A�C�e������������o���B"},
	{16,15,40,FALSE,TRUE,A_DEX,0,0,"�����X�^�[������",
		"�����ɂ����͂��̃����X�^�[����������o���B"},
	{21,30,0,TRUE,FALSE,A_STR,0,0,"�劅",
		"�剹��������čU������B���ӂ̓G���N����B"},
	{24,20,30,FALSE,FALSE,A_CHR,0,0,"��������",
		"���͂̓�����������Ĕz���ɂ��悤�Ǝ��݂�B"},
	{30,32,45,FALSE,TRUE,A_INT,0,0,"�����̖���",
		"�����X�^�[��̂ɓd���_���[�W��^���A���̌㖂�@�͒ቺ��Ԃɂ���B" },

	{35,30,55,FALSE,TRUE,A_CHR,0,0,"��������",
		"�����̔z������������B"},
	{38,74,65,TRUE,TRUE,A_STR,0,20,"�h���S���Y�O���E��",
		"���������̃u���X����B�З͂�HP��1/3�ɂȂ�B"},
	{45,70,75,FALSE,TRUE,A_INT,0,0,"���p",
		"��ԑ���̏p���g���A���͂̓G���������������₷���Ȃ�B"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_kasen(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			int dice,sides,base;
			dice=2;sides=1+p_ptr->lev/10;base=2;
			if(only_info) return format("����:%dd%d+%d", dice, sides,base);
			msg_print("���������o���Đݒu�����B");
			lite_area(damroll(dice, sides)+base, p_ptr->lev / 10 + 1);
			break;
		}
	case 1:
		{
			int weight = p_ptr->lev * 15;
			if (only_info) return format("�d��:%d",weight);
			if (!get_aim_dir(&dir)) return NULL;
			fetch(dir, weight, FALSE);
			break;
		}
	case 2: 
		{
			int idx;
			monster_type *m_ptr;
			if(only_info) return "";
			if(!teleport_back(&idx, TELEPORT_FORCE_NEXT)) return NULL;

			break;
		}
	case 3:
		{
			int dam = p_ptr->lev * 4;
			if(only_info) return format("����:�`%d",dam/2);
			msg_format("�u�΁@���@���@�́@�[�@�I�I�I�v");
			project(0, 5 + p_ptr->lev / 15, py, px, dam, GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
			aggravate_monsters(0,FALSE);
			break;
		}
	case 4:
		{
			int power = p_ptr->lev * 2 + adj_general[p_ptr->stat_ind[A_CHR]]*2;
			if(power<50) power = 50;
			if(only_info) return format("����:%d",power);
			msg_format("���Ȃ��͎��߂̂���h����������E�E");
			charm_animals(power);
			break;
		}

	case 5:
	{
		int base = plev * 3;
		int sides = chr_adj * 5;
		int dam;
		int dist = 3 + plev / 10;

		if (only_info) return format("����:%d+1d%d", base, sides);
		if (!get_aim_dir(&dir)) return NULL;

		dam = base + randint1(sides);
		project_length = dist;
		msg_print("���Ȃ��͗��b��������B");
		fire_ball(GF_ELEC, dir, dam, 0);
		//�d���Ő����Ă���Ƃ��ǉ��Ŗ��@�ቺ
		if (cave[target_row][target_col].m_idx)
		{
			fire_ball_hide(GF_DEC_MAG, dir, dam, 0);
		}
		else if (cheat_xtra)
			msg_print("no target");

		break;
	}


	case 6:
		{
			bool flag = FALSE;
			if(only_info) return "";
			for(i=0;i<p_ptr->lev / 10;i++)
			{
				if ((summon_specific(-1, py, px, p_ptr->lev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET))))flag = TRUE;
			}
			if(flag)
				msg_print("�����B���Ăяo�����I");
			else 
				msg_print("�����͌���Ȃ������B");

			break;
		}
	case 7:
		{
			int dam;
			dam = p_ptr->chp / 3;
			if(dam<1) dam = 1;
			if(dam > 800) dam=800;
			if(only_info) return format("����:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			else msg_print("��т̘r���傫���c��A�Ռ��g�������ꂽ�I");
		
			fire_ball(GF_SOUND, dir, dam, -3);
			break;	
		}

	case 8:
		{
			dice = base = 15;
			if(only_info) return format("����:%d+1d%d",base,dice);
			msg_format("��Ԃ𑀂��Đg���B�����E�E");
			set_tim_superstealth(base + randint1(dice),FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}



	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::���P��p�Z*/
class_power_type class_power_kogasa[] =
{
	{1,0,20,FALSE,FALSE,A_STR,0,0,"��������",
		"�吺�ŋ��сA�אڂ����G��̂���������B�Q�Ă���G�ɂ͌����₷���B����̓G���N����B"},
	{12,20,20,TRUE,FALSE,A_STR,0,0,"�t���X�C���O",
		"�P��U��񂵁A�אڃO���b�h�S�ĂɍU������B"},
	{15,10,30,FALSE,FALSE,A_INT,0,0,"�ڗ���",
		"������Ӓ肷��B���x��40�ȍ~�͕���̔\�͂����S�ɒm�邱�Ƃ��ł���B"},

	{20,20,30,TRUE,FALSE,A_DEX,0,0,"��{���s�b�`���[�Ԃ�",
		"�P���\���A��莞�Ԏ�����_�������ł��Ԃ��B"},

	{25,30,25,FALSE,TRUE,A_CHR,0,0,"�n���E�t�H�S�b�g�����[���h",
		"�����𒆐S�ɐ������̋���ȃ{�[���𔭐�������B" },

	{30,50,0,FALSE,FALSE,A_DEX,0,0,"�b��",
		"�������̕���̏C���l����������B�ʏ�̎O�{�̍s���͂������̂Œ��ӁB"},
	{35,32,25,FALSE,TRUE,A_INT,0,0,"���炩�������t���b�V��",
		"�P���猶�f�I�Ȍ�������A���E���̓G�S�Ă�f�킹��B"	},
	{40,55,65,FALSE,TRUE,A_CON,0,0,"�Q�����䕗",
		"���E���S�Ăɑ΂����ƕ��ōU������B"	},

	{44,77,75,FALSE,TRUE,A_CHR,0,0,"�I�[�o�[�E�U�E���C���{�[",
		"�M���Ɛ��̕��������̋��͂ȃr�[������B�M���E�����ϐ��̂Ȃ������X�^�[���m���ɍ���������B" },


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_kogasa(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int power = p_ptr->lev;
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (only_info) return format("����:%d",power);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int food = 0;
					char m_name[80];

					r_ptr = &r_info[m_ptr->r_idx];

					if(MON_CSLEEP(m_ptr)) power *= 3;
					monster_desc(m_name, m_ptr, 0);
					if(one_in_(2))	msg_format("�u����߂���[�I�v");
					else			msg_format("�u�����[�I�I�v");
					msg_print("");
					if(r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flagsr & RFR_RES_ALL)
						msg_format("%s�͖��������B",m_name);
					else if(r_ptr->flags3 & RF3_NO_CONF)
						msg_format("%s�͘f�킳��Ȃ��B",m_name);
					else if(MON_CONFUSED(m_ptr) || MON_MONFEAR(m_ptr))
						msg_format("%s�͂��łɓx�������Ă���B",m_name);
					else if(m_ptr->mflag & MFLAG_SPECIAL)
						msg_format("%s�͂��������Ă���Ȃ������B",m_name);
					else if(r_ptr->level > randint1(power) || is_pet(m_ptr))
						msg_format("%s�͋����Ă���Ȃ������B",m_name);
					else
					{
						if(r_ptr->flags3 & RF3_HUMAN)
						{
							msg_format("%s�͋������I�������c�ꂽ�C������I",m_name);
							food = 5000;
						}
						else if(r_ptr->flags2 & RF2_WEIRD_MIND)
						{
							msg_format("%s�͋������炵���B�ł��S�R���̑����ɂȂ�Ȃ��B",m_name);
							food = 100;
						}
						else
						{
							msg_format("%s�͋������I��������͂�l�Ԃ���Ȃ��Ɣ��������Ȃ��B",m_name);
							food = 500;
						}
						if(p_ptr->food + food > PY_FOOD_MAX-1) set_food(PY_FOOD_MAX - 1);
						else set_food(p_ptr->food + food);

						(void)set_monster_confused(cave[y][x].m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
						project(0,0,y,x,power,GF_DEC_MAG, PROJECT_KILL, 0);
						project(0,0,y,x,power,GF_TURN_ALL,PROJECT_KILL,0);
					}
					/*:::�����G�����x���������Ȃ��悤�Ƀ}�[�N�����Ă���*/
					m_ptr->mflag |= MFLAG_SPECIAL;
					aggravate_monsters(0,FALSE);
				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}
			break;

		case 1:
		{
			int rad = 1;
			base =calc_weapon_dam(0);
			if(only_info) return format("����:%d",base / 2);		
			msg_format("���Ȃ��͎P��傫���U��񂵂��I");
			project(0, rad, py, px, base, GF_MISSILE, PROJECT_KILL , -1);
			break;
		}

		case 2:
		{
			if(only_info) return format("");
			if(p_ptr->lev < 40)
			{
				if (!ident_spell(TRUE)) return NULL;
			}
			else
			{
				if (!identify_fully(TRUE)) return NULL;
			}

		}
		break;

		case 3:
		{
			int time;
			base = p_ptr->lev/5+15;
			sides = p_ptr->lev/3+4;
			
			if(only_info) return format("����:%d+1d%d",base,sides);
			time = base + randint1(sides);
			msg_format("���Ȃ��͎P��������ō\�����E�E");
			set_tim_reflect(time, FALSE);
			break;
		}

		case 4://v1.1.63 �n���E�t�H�S�b�g�����[���h
		{
			int rad = 4 + p_ptr->lev / 15;
			base = p_ptr->lev * 4 + chr_adj * 7;

			if (only_info) return format("����:�`%d", base / 2);
			msg_format("���Ȃ��͐��e���΂�T�����B");
			project(0, rad, py, px, base, GF_WATER, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}

		//���P�b��\��
		case 5:
		{
			int     item,amt;
			cptr    q, s;
			object_type *o_ptr;
			object_type forge;
			object_type *q_ptr = &forge;
			int ty,tx;
			int new_val = p_ptr->lev/5 + 5;
			char	o_name[MAX_NLEN];

			if(only_info) return format("����:%d",new_val);

			item_tester_hook = object_is_metal;

			q = "�ǂ��b�������܂����H ";
			s = "�����ɂ͂��Ȃ����b��������̂͂Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

			if(!object_is_known(o_ptr))
			{
				msg_print("�܂��ӎ����Ȃ��ƒb�����Ȃ��B");
				return NULL;
			}
			if((object_is_weapon_ammo(o_ptr) && o_ptr->to_h >= new_val && o_ptr->to_d >= new_val
			|| object_is_armour(o_ptr) && o_ptr->to_a >= new_val)
			 && !(object_is_cursed(o_ptr) && !(o_ptr->curse_flags & TRC_PERMA_CURSE) && !(o_ptr->curse_flags & TRC_HEAVY_CURSE) ))
			{
				msg_print("���������ȏ�b����͖̂����������B");
				return NULL;
			}

			if (o_ptr->number > 1)
			{
				amt = get_quantity(NULL, o_ptr->number);
				if (amt <= 0) return NULL;
			}
			else amt = 1;

			msg_print("���Ȃ��͒b�蓹������o�����E�E");
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			if(o_ptr->number > 1 && o_ptr->number > amt)
			{
				object_copy(q_ptr,o_ptr);
				q_ptr->number = amt;
				if(!kogasa_smith_aux(q_ptr)) return NULL;
				msg_format("%s��b���������I",o_name);
				if (item >= 0)
				{
					inven_item_increase(item, -amt);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -amt);
					floor_item_optimize(0 - item);
				}
				inven_carry(q_ptr);
			}
			else
			{
				if(!kogasa_smith_aux(o_ptr)) return NULL;
				msg_format("%s��b���������I",o_name);
			}

			new_class_power_change_energy_need = 300;
			p_ptr->window |= (PW_INVEN | PW_EQUIP);
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);
			break;
		}


		case 6:
			{
				int power = p_ptr->lev * 3;
				if (only_info) return format("����:%d",power);
				msg_format("���Ȃ��̎P�͓ˑRῂ��������I");
				stun_monsters(power);
				confuse_monsters(power);
				turn_monsters(power);
				lite_room(py, px);
			}
				break;
		case 7:
			{
				base = p_ptr->lev*2;
				dice = 1;
				sides = 10 + chr_adj * 3;
				if(only_info) return format("����:(%dd%d+%d) * 2",dice,sides,base);
				msg_format("���Ȃ��͋ǒn�I�ȍ��J�Ɩ\�����N�������I");
				project_hack2(GF_WATER,dice,sides,base);
				project_hack2(GF_TORNADO,dice,sides,base);
				break;
			}
			break;
		}


		case 8://v1.1.63 �I�[�o�[�E�U�E���C���{�[
		{
			base = p_ptr->lev * 5 / 2 + chr_adj * 8;
			if (only_info) return format("����:%d", base);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("�_���W�����ɓ����˂������B");
			fire_spark(GF_RAINBOW, dir, base, 2);

			break;
		}



	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}





/*:::������p�Z*/
class_power_type class_power_komachi[] =
{
	{5,3,25,FALSE,FALSE,A_CHR,0,0,"���_�̈Ј�",
		"�G��̂����|������B���҂ƃA���f�b�h�ɂ͌��ʂ������B"	},
	{10,8,35,FALSE,TRUE,A_CHR,0,12,"�����ɂł����镂�V��",
		"���V����Ăяo���G�ɂԂ��Ēn�������̍U��������B���x�����オ��ƌĂяo���삪������B�^�[�Q�b�g�̓����_���Ɍ��܂�B"},
	{15,30,35,TRUE,TRUE,A_STR,0,0,"�̗͂���̂悤��",
		"���p�̑D�ɏ���ă^�[�Q�b�g�ɓˌ����A�������_���[�W��^����B"	},
	{20,16,40,FALSE,FALSE,A_INT,0,0,"�����𑀂�T",
		"���ݒn����ʘH���ʂ������m�̏ꏊ�ֈړ�����B�����ړ��\�͂�����Ƃ��͂���ɉ����ď���s���͂�����B"	},
	{24,30,55,FALSE,TRUE,A_WIS,0,0,"���Ԃ̋���",
		"�אڂ����G��̂̈ړ��ƃe���|�[�g��Z���ԖW�Q����B����ȓG��͋����G�ɂ͌����Â炢�B"},
	{28,30,70,FALSE,TRUE,A_INT,0,0,"�E���̋V",
		"�w�肵���G�ƈʒu����������B���E�O�̓G�ɂ��L���B"},
	{32,32,60,FALSE,FALSE,A_DEX,0,0,"���}���̌���",
		"�w�肵���G�������ׂ̗Ɉ����񂹁A���̂܂܍U������B"},
	{35,27,75,FALSE,TRUE,A_WIS,0,0,"�]���􋖂�����",
		"�אڂ����G���m���ňꌂ�œ|���A���m����HP�𔼕��ɂ���B���҈ȊO�ɂ͌������A��R�����Ɩ����B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},
	{38,44,70,FALSE,TRUE,A_WIS,0,0,"���ґI�ʂ̊�",
		"�G�̓��ォ����̐n�𗎂Ƃ��B�A���f�b�h�̏ꍇ4�{�̃_���[�W��^���č��m���ňꌂ�œ|���B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},

	{40,50,80,FALSE,FALSE,A_DEX,0,0,"�����𑀂�U",
		"��O���b�h���̕ǂ�������蔲����B��������Ƀ����X�^�[������ꍇ���s����B�e���|�[�g�s�n�`�̒��ɂ������B" },

	{43,50,75,FALSE,FALSE,A_STR,0,0,"���z���̑K",
		"��������S�ď���A���E���̑S�Ẵ����X�^�[�Ƀ_���[�W��^����B"},
	{46,80,90,FALSE,TRUE,A_CHR,0,0,"���������̊�",
		"������̓G�ɋ��͂ȍU�����d�|����B���҂ɓ��������ꍇ���m���ňꌂ�œ|���A����ɑ啝�ɑ̗͂��z������B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_komachi(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

		/*:::�Ј��@���҃A���f�b�h���U��GF_TURN_ALL�������ōs���Ă���*/
	case 0:
		{
			int power = p_ptr->lev;
			if (only_info) return format("����:%d�`",power);
			if (!get_aim_dir(&dir)) return NULL;
			fear_monster(dir, power);
			break;
		}
		/*:::����e�@�^�[�Q�b�g�����_���I�胋�[�`�����g�p�@�������葫��Ȃ���������Ȃ�*/
	case 1:
		{
			int rad = 2;
			bool flag = FALSE;
			int num;
			if(p_ptr->lev > 39) rad = 3;
			dice = 1;
			sides = p_ptr->lev;
			base = p_ptr->lev / 2 ;
			num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 5;
			if(only_info) return format("����:(%d+%dd%d) * %d",base,dice,sides,num);

			if(num<2)msg_format("���Ȃ��͕��V����Ăяo�����B");
			if(num<5)msg_format("���Ȃ��͕��V�삽�����Ăяo�����B");
			else msg_format("���Ȃ��͑�ʂ̕��V����Ăяo�����I");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_NETHER, base + damroll(dice,sides),3, rad,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
			break;
		}
		/*:::�̗͂���̂悤�Ɂ@GF_�����w�肵�����g���[�`��*/
	case 2:
		{
			int len=p_ptr->lev / 5;
			if(len < 5) len = 5;
			damage = adj_general[p_ptr->stat_ind[A_STR]] * 5 + p_ptr->lev;
			if(damage < 50) damage = 50;
			if(only_info) return format("�˒�:%d ����:%d",len,damage);
			if (!rush_attack2(len,GF_WATER,damage,0)) return NULL;
			break;
		}
		//v1.1.20 ���E�O�ł����m�̒ʘH���ʂ��Ă���΍s���邱�Ƃɂ���
		//v2.0.1 ������teleport_walk()�ɕ���
	case 3:
		{
			int x, y;
			int cost;
			int dist = 8 + plev / 4;
			if(only_info) return format("�ړ��R�X�g:%d",dist);

			if (!teleport_walk(dist)) return NULL;

			//�����ړ������鎞�ړ��Ɠ����悤�ɏ���s���͂���������
			if(p_ptr->speedster)
				new_class_power_change_energy_need = (75-p_ptr->lev/2);

			break;		
		}
	/*::: -Hack- ���Ԃ̋��ԁ@magic_num1[0]��tim_general[0]���g�p����*/
		//v1.1.95 GF_NO_MOVE�����ɕύX�����͎g��Ȃ��Ȃ���
	case 4:
		{
			int y, x;
			monster_type *m_ptr;

			int power = 10 + plev / 5;

			//base = 5 + p_ptr->lev / 10;
			if(only_info) return format("����:%d",power);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					char m_name[80];	
					monster_desc(m_name, m_ptr, 0);
					msg_format("%s�����Ԃɕ߂����I",m_name);
					project(0, 0, y, x, power, GF_NO_MOVE, PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE, -1);
					//p_ptr->magic_num1[0] = cave[y][x].m_idx;
					//set_tim_general(base,FALSE,0,0);
				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}

			break;
		}
		/*:::�E���̋V�@�ʒu����*/
	case 5:
		{
			bool result;
			if(only_info) return "";

			/* HACK -- No range limit */
			project_length = -1;
			result = get_aim_dir(&dir);
			/* Restore range to default */
			project_length = 0;
			if (!result) return NULL;
			teleport_swap(dir);
			break;
		}
		/*:::���}���̌��Ł@�e���|�[�g�E�o�b�N��Ɨ����[�`�����@�e���|�o�b�N�K�p��^�[�Q�b�g���ׂɂ��Ă���ʏ�U��*/
	case 6: 
		{
			int idx;
			monster_type *m_ptr;
			if(only_info) return "";

			if(!teleport_back(&idx, TELEPORT_FORCE_NEXT)) return NULL;
			m_ptr = &m_list[idx];
			if(distance(py,px,m_ptr->fy,m_ptr->fx) > 1)	msg_format("���܂��ׂɗ��Ȃ������B");
			else py_attack(m_ptr->fy,m_ptr->fx,0);

			break;
		}
		/*:::�]���􋖂������@�A���f�b�h�A�f�[�����A�������A�ꕔ�_�i�ɂ͑��������@�����m���͓Őj�Ɠ����ɂ����@HP�����m���͂�⍂�߁@���j�[�N�ɂ͌����ɂ���*/
	case 7:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			damage = 0;
			if(only_info) return "";
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int bonus = 25;
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(use_itemcard) bonus = 50;

				msg_format("���Ȃ��͑傫������U�艺�낵���B");
				if(!monster_living(r_ptr) || r_ptr->flags1 & RF1_QUESTOR)
				{
					msg_format("%s�ɂ͑S�����ʂ��Ȃ��悤���B",m_name);
					break;
				}
				
				if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
				{
					if((randint1(randint1(r_ptr->level/7)+5) == 1) && randint1(r_ptr->level * 4) < bonus + p_ptr->lev/2)
					{
						msg_format("����%s�ɒ��������I",m_name);
						damage = m_ptr->hp / 2 + 1;
					}
					else
						msg_format("���͋��؂����B",m_name);
				}
				else
				{
					if(randint1(randint1(r_ptr->level/7)+5) == 1)
					{
						msg_format("����%s�̋}�����т����I",m_name);
						damage = m_ptr->hp + 1;
					}
					else if(randint1(r_ptr->level * 3 ) < bonus + p_ptr->lev / 2)
					{
						msg_format("����%s�ɒ��������I",m_name);
						damage = m_ptr->hp / 2 + 1;
					}
					else
						msg_format("���͋��؂����B",m_name);
				}
				if(damage>0) 
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_MISSILE,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	
		/*:::���ґI�ʂ̊��@�ʏ�U��/2��DISP_ALL �A���f�b�h�Ƀ_���[�W4�{�A�񃆃j�[�N�A���f�b�h�ɍ��m������*/
		//v1.1.47 �A���f�b�h�Ƀ_���[�W2�{��4�{�ɂ���
	case 8:
		{
			int y, x;
			int dist;
			monster_type *m_ptr;
			monster_race *r_ptr;

			dist = (p_ptr->lev > 44) ? 3 : 2;
			damage=0;
			//���������Ă���肵���_���[�W�ɎZ�����Ȃ��B����ɕ���(���ȊO�܂�)�������Ă���ꍇ�_���[�W2/3
			if(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) damage += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) damage += calc_weapon_dam(1);
			if(p_ptr->migite && p_ptr->hidarite) damage = damage * 2 / 3;
			damage /= 2;

			if(only_info) return  format("�˒�:%d ����:%d",dist,damage);
			if(damage<1) damage = 1;
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;

			y = target_row;
			x = target_col;
			m_ptr = &m_list[target_who];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("���Ȃ��͑傫������U�艺�낵���B�V��������~�蒍�����I");
				if(r_ptr->flags3 & RF3_UNDEAD && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) && randint1(r_ptr->level * 3 ) < p_ptr->lev)
				{
					msg_format("%s�͌��̒��ɏ������B",m_name);
					damage = m_ptr->hp + 1;
				}
				else if(r_ptr->flags3 & RF3_UNDEAD)
				{
					msg_format("%s�͐r��ȑ������󂯂��B",m_name);
					damage *= 4;
				}
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_DISP_ALL,PROJECT_KILL,-1);
			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	

		//v1.1.65 �����𑀂�U
	case 9:
	{
		int x1, y1, x2, y2;
		bool flag_can_move = FALSE;

		if (only_info) return  format("����:1�O���b�h");

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y1 = py + ddy[dir];
		x1 = px + ddx[dir];
		y2 = y1 + ddy[dir];
		x2 = x1 + ddx[dir];

		//��ʒ[�ɑ΂��Ă͎g���Ȃ�
		if (!in_bounds(y1, x1) || !in_bounds(y2, x2))
		{
			msg_print("������̕��ɂ͐i�߂Ȃ��B");
			return NULL;
		}

		//�܂��אڃO���b�h�������h�A���ǂł��邱�Ƃ��m�F
		if (!cave_have_flag_bold(y1, x1, FF_MOVE) && (cave_have_flag_bold(y1, x1, FF_DOOR) || cave_have_flag_bold(y1, x1, FF_WALL)))
		{
			//����2�O���b�h�ڂɐN���\�ł��邱�Ƃ��m�F
			if (player_can_enter(cave[y2][x2].feat, 0))
			{
				flag_can_move = TRUE;
			}
		}
		if (flag_can_move)
		{
			if (cave[y1][x1].m_idx || cave[y2][x2].m_idx)
			{
				msg_print("�����Ɏז����ꂽ�B");
				flag_can_move = FALSE;
			}

		}
		else
		{
			msg_print("���̏ꏊ�͂��̔\�͂̑Ώۂɂ͂Ȃ�Ȃ��B");

		}


		if (flag_can_move)
		{

			if(one_in_(10))msg_print("�u�����뎯���Ȃ��ċ���𑾋�ƈׂ��I�v");
			else if (one_in_(9))msg_print("�u���̗l�ȏ�ǂőj�܂�Ă��܂����炢�ł͎d���ɂȂ�Ȃ���ł˂��v");
			else msg_print("���Ȃ��͕ǂ̌����𑀍삵�Ă��蔲�����B");

			move_player_effect(y2, x2, (MPE_FORGET_FLOW | MPE_DONT_PICKUP));
		}
	}
	break;

	case 10: //v1.1.20 ���z���̑K
		{
			int dam = p_ptr->au / 666;
			if(dam > 5000) dam = 5000;
			if(only_info) return  format("����:%d",dam);

			if(dam < 1)
			{
				msg_format("���Ȃ��͂��łɕ��������B");
				return NULL;
			}
			if (!get_check_strict("��������0�ɂȂ�܂��B��낵���ł����H ", CHECK_OKAY_CANCEL)) return NULL;

			msg_print("���Ȃ��͋C�O�悭�L����S�Ă𓊂��������I");
			project_hack2(GF_ARROW,0,0,dam);
			p_ptr->au = 0;
			p_ptr->redraw |= PR_GOLD;

		}
		break;
		/*:::���������̊�*/
	case 11:
		{
			int dist = 5; //v1.1.47 ������Ƌ���
			damage=0;
			//���������Ă���肵���_���[�W�ɎZ�����Ȃ��B����ɕ���(���ȊO�܂�)�������Ă���ꍇ�_���[�W2/3
			if(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) damage += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) damage += calc_weapon_dam(1);
			if(p_ptr->migite && p_ptr->hidarite) damage = damage * 2 / 3;
			dice = 1;
			sides = adj_general[p_ptr->stat_ind[A_CHR]] * p_ptr->lev / 5;
			if(only_info) return  format("�˒�:%d ����:%d+%dd%d",dist,damage,dice,sides);
			if(damage < 1) damage = 1;
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���Ȃ��͑傫������U��グ���B�n�ʂ��疳���̐n���˂��o�����I");
			fire_beam(GF_RYUURI, dir, damage + damroll(dice,sides));

			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::�ߋ��p�Z*/
//v1.1.95 �u�H�߂͎��̔@���v��tim_general[0]�g�p
class_power_type class_power_iku[] =
{
	{10,12,25,FALSE,TRUE,A_CHR,0,0,"���_�̓{��",
		"�w�肵���^�[�Q�b�g�ɗ��𗎂Ƃ��čU������B"	},
	{20,30,35,FALSE,TRUE,A_CHR,0,0,"���_����",
		"��莞�ԃg�Q��̃I�[�����܂Ƃ��U�����Ă����G�ɔ�������B�j�Бϐ������G�ɂ͖����B"	},
	{25,32,45,FALSE,TRUE,A_INT,0,0,"���_�̈ꌂ",
		"�������Ă��镐��ɗ����܂Ƃ킹�A�ꎞ�I�ɓd����������������B"	},
	{30,40,55,FALSE,FALSE,A_DEX,30,0,"�H�߂͋�̔@��",
		"�G�̗l�X�ȍU�����󂯗����ׂ��g�\���A�ꎞ�I�Ɏ_�d�Η�̑ϐ���AC�㏸�𓾂�B�������d���Ǝ��s���₷���B"	},
	{33,40,45,FALSE,TRUE,A_CHR,0,0,"�G���L�e���̗��{",
		"�����𒆐S�Ƃ������͂ȓd���̃{�[���𔭐�������B"},
	{36,55,65,FALSE,TRUE,A_CHR,0,10,"���{�̎g���V�j�e",
		"���͂ȓd���̃{�[�����ʂɕ��B�^�[�Q�b�g�̓����_���Ɍ��܂�B"},
	{40,72,80,FALSE,FALSE,A_DEX,0,0,"�H�߂͎��̔@��",
		"�ꎞ�I�ɓG����̗אڍU���ɑ΂��čs������Ȃ��Ŕ�������悤�ɂȂ�B�����̂��т�MP��7�����B�H�߂ɂ��i�����łȂ��Ǝg���Ȃ��B" },

	{45,85,70,FALSE,TRUE,A_CHR,0,0,"���_�C�̗���",
		"���E���̑S�Ă�d���ōU������B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_iku(int num, bool only_info)
{
	int dir,dice,sides,base;
	bool flag_storm = FALSE;


	switch(num)
	{

	case 0:
		{
			int rad = 2;
			if(p_ptr->lev > 39) rad = 3;
			dice = 2 + p_ptr->lev / 5;
			sides = 10 ;
			base = p_ptr->lev ;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_ELEC, dir, base + damroll(dice,sides), rad,"���Ȃ��͓V�Ɍ����Ďw���������B")) return NULL;
			break;
		}
	case 1:
		{
			if(only_info) return format("����:20+1d20");
			msg_format("�g�Q�̂悤�ȃI�[�����܂Ƃ����B");
			  set_dustrobe(20+randint1(20),FALSE);
			break;
		}

	case 2:
		{
			base = p_ptr->lev/2;
			sides = p_ptr->lev/2;
			if(only_info) return format("����:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			{
				msg_print("����������Ă��Ȃ��ƌ��ʂ��Ȃ��B");
				return NULL;//paranoia
			}

			msg_format("��ɂ��Ă��镐�킩��ΉԂ��U�����I");
			set_ele_attack(ATTACK_ELEC, base + randint1(sides));
			break;
		}
	case 3:
		{
			int time;
			base = p_ptr->lev/2+10;
			sides = p_ptr->lev/2;
			if(sides < 10) sides = 10;
			
			if(only_info) return format("����:%d+1d%d",base,sides);
			time = base + randint1(sides);
			msg_format("���Ȃ��͋C����g�ɓZ�����E�E");
			set_shield(time, FALSE);
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			break;
		}

	case 4:
		{
			int rad = 5;
			base = p_ptr->lev * 10 + adj_general[p_ptr->stat_ind[A_CHR]] * 10;
			if(only_info) return format("����:�`%d",base / 2);		
			msg_format("���Ȃ��͓V�Ɍ����Ďw���������B");
			project(0, rad, py, px, base, GF_ELEC, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}

	case 5:
		{
			int rad = 1;
			bool flag = FALSE;
			int num;
			int i;
			dice = 1;
			sides = p_ptr->lev;
			base = p_ptr->lev;
			num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 5;
			if(only_info) return format("����:(%d+%dd%d) * %d",base,dice,sides,num);

			else msg_format("���Ȃ��͑�ʂ̗�����������I");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_ELEC, base + damroll(dice,sides),3, rad,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}
			break;
		}
	case 6:
		{
			int base = 4;

			if (only_info) return format("����:%d+1d%d�^�[��", base,base);

			if (!p_ptr->do_martialarts)
			{
				msg_print("���̑����ł͂��̋Z���g���Ȃ��B");
				return NULL;
			}

			set_tim_general(base+randint1(base), FALSE, 0, 0);
			p_ptr->counter = TRUE;

			break;
		}

	case 7:
		{
			dice = p_ptr->lev / 5;
			sides = adj_general[p_ptr->stat_ind[A_CHR]] * 3;
			if(only_info) return format("����:%dd%d",dice,sides);
			msg_format("���Ȃ��͒��ɕ����ė�����L�����E�E");
			project_hack2(GF_ELEC,dice,sides,0);
			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}




/*:::���ǂ񂰐�p�Z*/
class_power_type class_power_udonge[] =
{
	{5,10,25,FALSE,FALSE,A_CHR,0,0,"�}�C���h�V�F�C�J�[",
		"���E���̃����X�^�[��̂��ɂ�Ő��_���U��������������B���x��30�ȍ~�ł���ɞN�O�ƌ�����^����B���قȐ��_�����G�⃆�j�[�N�����X�^�[�ɂ͌��ʂ������B"	},
	{12,20,55,FALSE,FALSE,A_INT,0,0,"�g������",
		"���͂ɂ�����̂����m����B���x�����オ��Ɗ��m�ł�����̂̎�ނƔ͈͂�������B"	},
	{16,18,30,FALSE,TRUE,A_WIS,0,0,"�C�����[�W���i���B�u���X�g",
		"�����������_�U���̃��[�U�[����B���_�̊󔖂ȓG�⋶�C�������炷�G�A���j�[�N�����X�^�[�ɂ͌��ʂ������B"	},
	{20,30,65,FALSE,FALSE,A_INT,0,0,"�g���f�f",
		"���E���̃����X�^�[�̔g����ǂݎ���Ĕ\�͂�p�����[�^�Ȃǂ𒲍�����B"	},
	{24,40,70,FALSE,FALSE,A_CHR,0,0,"�}�C���h�X�g�b�p�[",
		"���E���̃����X�^�[�S�Ă��ɂ�œ������~������B"	},
	{28,25,60,FALSE,TRUE,A_CHR,0,0,"�f�B�X�J�[�_�[",
		"���E���̃����X�^�[��̂����m���Ŗ��@�͒ቺ��Ԃɂ���B��R�����Ɩ����B" },
	{32,36,50,FALSE,TRUE,A_WIS,0,0,"�}�C���h�G�N�X�v���[�W����",
		"�����������_�U���̃��P�b�g����B���_�̊󔖂ȓG�⋶�C�������炷�G�A���j�[�N�����X�^�[�ɂ͌��ʂ������B"	},
	{36,33,50,TRUE,TRUE,A_CON,0,0,"���m���o�̖�",
		"�i�Ԉ�̋�����𕞗p����B���݉߂���ƁE�E"	},
	{39,45,65,FALSE,TRUE,A_CHR,0,0,"�}�C���h�u���[�C���O",
		"���E���̃����X�^�[��̂�����m��������B��R�����Ɩ����B�N�G�X�g�œ|�Ώۃ����X�^�[�␸�_�������Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B" },
	{42,56,70,FALSE,TRUE,A_INT,0,0,"�r�W���i���`���[�j���O",
		"�����ꎞ�I�Ɏ��̗͂l�X�Ȕg�������킹�A�G���������������₷������B"	},
	{45,65,75,FALSE,TRUE,A_WIS,0,0,"�A�C�T�C�g�N���[�j���O",
		"�L�͈͂ɑ΂������������_�U�����s���B���_�̊󔖂ȓG�⋶�C�������炷�G�A���j�[�N�����X�^�[�ɂ͌��ʂ������B"	},
	{48,85,90,FALSE,TRUE,A_INT,0,0,"�A�L�����[�X�y�N�g��",
		"�����ꎞ�I�ɋ����ƂƂ��ɍs�����A�G����̍U�����󂯂ɂ�������B"	},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_udonge(int num, bool only_info)
{
	int dir,dice,sides,base,damage;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			dice = p_ptr->lev / 5;
			sides = 7 + p_ptr->lev / 10;
			base = p_ptr->lev ;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			damage = damroll(dice,sides) + base;
			if(p_ptr->lev < 30) fire_ball_hide(GF_MIND_BLAST, dir, damage, 0);
			else				fire_ball_hide(GF_BRAIN_SMASH, dir, damage, 0);

			break;
		}
	case 1:
		{
			int rad = p_ptr->lev / 2 + 10;
			if(only_info) return format("�͈�:%d",rad);

			if(use_itemcard)
				msg_format("���͂̐F�X�Ȃ��Ƃ����������C������...");
			else
				msg_format("���͂̔g���𕪐͂����E�E");
			detect_doors(rad);
			if(p_ptr->lev > 4) detect_monsters_normal(rad);
			if(p_ptr->lev > 9) detect_traps(rad, TRUE);
			if(p_ptr->lev < 15)detect_stairs(rad);
			else map_area(rad);
			if(p_ptr->lev > 19) detect_objects_normal(rad);
			break;
		}
	case 2:
		{
			dice = p_ptr->lev / 5;
			sides = 5 + p_ptr->lev / 10;
			base = p_ptr->lev + 10;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���Ȃ��͖ڂ���r�[���𔭎˂����I");
			fire_beam(GF_REDEYE, dir, base + damroll(dice,sides));
			break;
		}
	case 3:
		{
			if(only_info) return "";

			msg_format("���͂̑��݂���������g����ǂݎ�����B");
			probing();
			break;
		}
	case 4:
		{
			int power = p_ptr->lev * 4;
			if(power < 70) power = 70;
			if(only_info) return format("����:%d",power);

			if(use_itemcard)
				msg_format("�J�[�h���Ԃ��������I");
			else
				msg_format("���Ȃ��̖ڂ��Ԃ��������I");
			stasis_monsters(power);
			break;
		}

	case 5:
		{
			base = p_ptr->lev * 5 + chr_adj * 5;
			if (only_info) return format("����:%d", base);

			if (!get_aim_dir(&dir)) return NULL;

			msg_print("������j�Q����g���𑗂����B");
			fire_ball_hide(GF_DEC_MAG, dir, base, 0);

			break;
		}
	case 6:
		{
			int rad = 2;
			if(p_ptr->lev > 39) rad = 3;
			dice = p_ptr->lev / 4;
			sides = 10 + p_ptr->lev / 10;
			base = p_ptr->lev * 2;
			if(only_info) return format("����:%d+%dd%d",base,dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("����Ȍ��e�̒e�ۂ𔭎˂����I");
			fire_rocket(GF_REDEYE, dir, base + damroll(dice,sides), rad);
			break;
		}
	case 7:
		{
			dice = base = 25;
			if(only_info) return format("����:%d+1d%d",base,dice);

			if(!p_ptr->hero)
			{
				msg_format("��𕞗p�����B");
				set_afraid(0);
				set_hero(randint1(dice) + base, FALSE);
			}
			else if (!p_ptr->shield)
			{
				msg_format("��𕞗p�����B");
				set_shield(randint1(dice) + base, FALSE);
			}
			//else if(!p_ptr->fast) set_fast(randint1(dice) + base, FALSE);
			else if(!p_ptr->tim_addstat_count[A_STR] || !p_ptr->tim_addstat_count[A_DEX] || !p_ptr->tim_addstat_count[A_CON])
			{
				int percentage = p_ptr->chp * 100 / p_ptr->mhp;
				int v = randint1(dice) + base;

				msg_format("��𕞗p�����B");
				msg_format("����ȗ͂��N���o���Ă����I");
				set_tim_addstat(A_STR,105,v,FALSE);
				set_tim_addstat(A_DEX,105,v,FALSE);
				set_tim_addstat(A_CON,105,v,FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= (PR_HP );
				redraw_stuff();
			}
			else 
			{
				//int dam = p_ptr->chp * 2 / 3;
				int dam = p_ptr->mhp / 2;
				if(dam<1) dam=1;

				if(p_ptr->pclass == CLASS_EIRIN)
				{
					msg_print("...��͂��߂Ă������B����ȏ�̕��p�͊댯���B");
					return NULL;
				}

				if (p_ptr->warning && !get_check_strict("...���ȗ\��������B�{���Ɉ��݂܂����H", CHECK_OKAY_CANCEL))
				{
					return NULL;
				}
				msg_format("����ł͂����Ȃ��ʂ̖�����݊������I");
				msg_format("���Ȃ��͑唚�������I");
				project(0, 7, py, px, dam * 5 + randint1(dam*5), GF_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
				take_hit(DAMAGE_LOSELIFE, dam, "�t���̖�", -1);
				set_hero(0,TRUE);
				set_shield(0,TRUE);
				set_tim_addstat(A_STR,0,0,TRUE);
				set_tim_addstat(A_DEX,0,0,TRUE);
				set_tim_addstat(A_CON,0,0,TRUE);
			}

			break;
		}



	case 8:
		{
			base = p_ptr->lev * 3 + chr_adj * 5;
			if (only_info) return format("����:%d", base);

			if (!get_aim_dir(&dir)) return NULL;

			msg_print("���C�������炷�g���𑗂����I");
			fire_ball_hide(GF_BERSERK, dir, base, 0);

			break;
		}

	case 9:
		{
			dice = base = 15;
			if(only_info) return format("����:%d+1d%d",base,dice);
			msg_format("���͂̔g�������킹���I");
			set_tim_superstealth(base + randint1(dice),FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}
	case 10:
		{
			damage = p_ptr->lev * 5 + adj_general[p_ptr->stat_ind[A_WIS]] * 5 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("����:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("���Ȃ��̎��E�͐Ԃ����܂����I");
			fire_spark(GF_REDEYE,dir,damage,3);

			break;
		}

	case 11:
		{
			dice = base = 6;
			if(only_info) return format("����:%d+1d%d",base,dice);
			if(use_itemcard) 
				msg_format("�����̑��݂���ʑ������ꂽ�C������...");
			else
				msg_format("���Ȃ��͎����̈ʑ������炵���B");
			set_multishadow(base + randint1(dice),FALSE);
			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}


/*:::�Ă��p�Z*/
class_power_type class_power_tewi[] =
{

	{10,7,25,FALSE,FALSE,A_DEX,0,0,"���Ƃ���",
		"�����ɗ��Ƃ������@��B�N�����Ă��Ȃ��ꏊ�łȂ��Ǝ��s�ł��Ȃ��B���V���Ă��Ȃ��G�͗��Ƃ����ɗ����ċC��A�N�O�A�s���x�����邱�Ƃ�����B"	},
	{20,12,25,FALSE,FALSE,A_INT,0,0,"�����g���b�v",
		"�G���ʂ����甚�����郋�[�������Ɏd�|����B"	},
	{27,20,30,TRUE,FALSE,A_DEX,30,0,"�t���X�^�[�G�X�P�[�v",
		"�ʏ�U�����s���A���̌��u�ŗ��E����B���E�Ɏ��s���邱�Ƃ�����B"	},
	{32,30,65,FALSE,FALSE,A_WIS,0,0,"�匊���x�l�̖�",
		"�_��̎~����ɂ��AHP���񕜂����Đ؂菝�������B"	},
	{38,45,70,FALSE,FALSE,A_INT,0,0,"���[�U�[�g���b�v",
		"���̃t���A����Ń��[�U�[������@�w��ݒu����B"	},
	{44,64,80,FALSE,TRUE,A_CHR,0,0,"�G���V�F���g�f���[�p�[",
		"���E���̃����X�^�[�S�Ă�F�D�I�ɂ���B���������̓��Z�̔����Ɏ��s�����Ƃ��t���A�̃����X�^�[�S�Ă����{����B�N�G�X�g�̓����Ώۃ����X�^�[�ɂ͌��ʂ��Ȃ��B"	},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_tewi(int num, bool only_info)
{
	int dir,dice,sides,base;
	switch(num)
	{
	case 0:
		{
			int i;
			if(only_info) return "";
			if(!cave_clean_bold(py,px))
			{
				msg_print("�����͌@��Ȃ��B");
				return NULL;
			}

			if (check_player_is_seen())
			{
				msg_print("���͌@��Ȃ��B�N���Ɍ����Ă���B");
				return NULL;
			}

			cave_set_feat(py, px, f_tag_to_index_in_init("TEWI_PIT"));
			msg_print("���Ƃ������@�����I");
		}
		break;
	case 1:
		{
			if(only_info) return format("�g���b�v�З�:%d+7d7",p_ptr->lev);
			msg_format("������㩂��d�|�����B");
			explosive_rune();
			break;
		}
	case 2:
		{
			if(only_info) return "";
			if(!hit_and_away()) return NULL;
			break;
		}	
	
	case 3:
		{
			int heal = (adj_general[p_ptr->stat_ind[A_WIS]]/2 + 15) * (5+p_ptr->lev / 10);
			if(heal < 50) heal = 50;
			if(only_info) return format("��:%d",heal);
			msg_print("�_�C�R�N�l�̗D�����ɖ��₳���...");
			hp_player(heal);
			set_cut(0);
			break;
		}
	case 4:
		{
			if(only_info) return format("");

			if(summon_named_creature(0,py,px,MON_SIGN_L,PM_EPHEMERA))
				msg_print("�g���b�v��ݒu�����B");
			else
				msg_print("�ݒu�Ɏ��s�����B");

		}
		break;

	case 5:
		{
			int i, count=0;
			monster_type *m_ptr;
			if(only_info) return "";

			if (p_ptr->inside_arena)
			{
				msg_print("�t���A�̕��͋C�͓G�ΓI���B�����ł̐����͖��Ӗ����B");
				return NULL;
			}

			if(one_in_(4))
				msg_format("���Ȃ��͈��b�痢�̖�������I�����I");
			else if(one_in_(3))
				msg_format("���Ȃ��͌�����o�C�����܂������Ă��I");
			else if(one_in_(2))
				msg_format("���Ȃ��͑s��ȃn�b�^�����Ԃ��グ���I");
			else 
				msg_format("���Ȃ��͈ꐢ���̚V���؂����I");

			for (i = m_max - 1; i >= 1; i--)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR) continue;
				if (!projectable(m_ptr->fy,m_ptr->fx,py,px)) continue;
				if (!is_hostile(m_ptr)) continue;
				set_friendly(m_ptr);
				count++;
			}
			if(count)
				msg_format("�����X�^�[�����͗F�D�I�ɂȂ����I");
			else
				msg_format("...���������肪���Ȃ������B");

			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::�d����p�Z*/
//v1.1.30 �F�X����
//tim_gen[0]���u�C���̌��v�Ɏg�p
class_power_type class_power_youmu[] =
{
	{12,21,25,TRUE,FALSE,A_DEX,50,5,"�����a",
		"�^�[�Q�b�g�ׂ̗܂ň�u�ňړ����A���̂܂܍U������B�������d���Ǝ��s���₷���B"	},
	{17,24,35,FALSE,TRUE,A_WIS,0,0,"�������E�a",
		"�אڂ����^�[�Q�b�g�փt�H�[�X�����̍U�����s���A�A���f�b�h���m���ňꌂ�œ|���B���O���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"	},
	{22,32,40,FALSE,TRUE,A_STR,0,0,"���Î��q�a",
		"�אڂ����^�[�Q�b�g�֒n�������̍U�����s���A�n���ϐ��������Ȃ��������m���ňꌂ�œ|���B�O�ό��𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},
	{25,50,50,TRUE,TRUE,A_STR,0,20,"�C���̌�",
		"��莞�ԉ������A����Ƀ����X�^�[��|�����Ƃ���MP���񕜂���悤�ɂȂ�B"	},
	{28,22,55,FALSE,FALSE,A_STR,0,15,"�n���Ɋy�ő��a��",
		"�߂��̃����_���ȃ����X�^�[�ɖ������r�[���𕡐�����B���������Ă��Ȃ��Ǝg�����񓁗����Ɖ񐔂��{�ɂȂ�B"	},
	{33,50,60,FALSE,TRUE,A_CHR,0,0,"�V�l�̌ܐ�",
		"�����̎��ӎO�}�X�ɑ΂��A�n���̋ƉΑ����̍U������B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"	},
	{37,56,65,TRUE,FALSE,A_DEX,0,15,"�����i���a",
		"�^�[�Q�b�g�ׂ̗܂ň�u�ňړ����A���킳���܂ōU����������B�������y���ꍇ�U�����������啝�ɏ㏸����B"},
	{40,64,70,FALSE,TRUE,A_WIS,0,0,"�H�������������̖@",
		"����̕��g���o���B���g�͊K�ړ����邩�{�̂Ƃ̋��������ꂷ����Ə��ł���B���g�̍U���ɂ��G��|�����ꍇ���o���l�ƃA�C�e���𓾂���B"	},
	{43,72,75,FALSE,TRUE,A_DEX,0,0,"�Z������a",
		"���E���̑S�Ẵ����X�^�[�ɒʏ�U�����s���B�I�[���_���[�W���󂯂Ȃ��B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"	},
	{47,96,80,FALSE,TRUE,A_CHR,0,0,"��O���ʍ�",
		"�����𒆐S�Ƃ������a�܃}�X�ɑ΂��A�ɂ߂ċ��͂ȍU������B�U���̑����͒n���A�J�I�X�A��Ԙc�ȁA�n���̋Ɖ΁A�������A�j�ׂ̂����ꂩ�ɂȂ�B�����{�������Ă��Ȃ��Ǝg���Ȃ��B"	},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_youmu(int num, bool only_info)
{
	int dir,dice,sides,base;
	int plev = p_ptr->lev;
	switch(num)
	{
	case 0:
		{
			int len = plev / 5;
			if(only_info) return format("�˒�:%d",len);
			if(p_ptr->do_martialarts)
			{
				msg_print("����������Ȃ��Ƃ��̋Z�͎g���Ȃ��B");
				return NULL;
			}
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 1:
	{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = (calc_weapon_dam(0) + calc_weapon_dam(1));
			//v1.1.30 �񓁗����Ƃ�����Ƌ�������̂Ŏ�̉�
			if(inventory[INVEN_RARM].k_idx && inventory[INVEN_LARM].k_idx) 
				damage = damage * 2 / 3;

			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;


			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("���F�̌��̒��������������I");

				if(!(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2) && r_ptr->flags3 & RF3_UNDEAD)
				{
					if((randint1(randint1(r_ptr->level/7)+5) == 1) && randint1(r_ptr->level * 4) < p_ptr->lev)
					{
						msg_format("%s�͌��̒��ɏ������B",m_name);
						damage = m_ptr->hp / 2 + 1;
					}
				}
				if(damage>0) 
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_FORCE,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	
	case 2:
	{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage=0;
			if(inventory[INVEN_RARM].name1 == ART_ROUKANKEN) damage = calc_weapon_dam(0) * 2;
			else if(inventory[INVEN_LARM].name1 == ART_ROUKANKEN) damage = calc_weapon_dam(1) * 2;

			if(only_info) return format("����:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];	
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("���Ȃ��͌���傫���U�艺�낵���I");

				if(!(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2) && monster_living(r_ptr) && !(r_ptr->flagsr & RFR_RES_NETH))
				{
					if((randint1(randint1(r_ptr->level/7)+5) == 1) && randint1(r_ptr->level * 4) < p_ptr->lev)
					{
						msg_format("�O�ό��̐n��%s�ɒ��������I",m_name);
						damage = m_ptr->hp / 2 + 1;
					}
				}
				if(damage>0) 
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_NETHER,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("�����ɂ͉������Ȃ��B");
				return NULL;
			}

			break;
		}	

		//v1.1.30�ǉ�
	case 3:
		{
			int time = plev / 2;
			if(only_info) return format("����:%d",time);
			set_tim_general(time,FALSE,0,0);
			set_fast(time,FALSE);
		}
		break;

		//v1.1.30�ǉ�
	case 4:
		{
			bool flag = FALSE;
			int i;
			int num = 0;
			int dam = 0;
			int range = plev / 12;

			if(inventory[INVEN_RARM].tval == TV_KATANA)
			{
				num += plev / 15;
				dam += calc_weapon_dam(0) / p_ptr->num_blow[0];
			}
			if(inventory[INVEN_LARM].tval == TV_KATANA)
			{
				num += (plev - 5) / 15;
				if(dam)
					dam = (dam + calc_weapon_dam(1) / p_ptr->num_blow[1]) / 2;
				else
					dam = calc_weapon_dam(1) / p_ptr->num_blow[1];
			}
			num += 1;
			if(dam<1) dam = 1;

			if(only_info) return format("�˒�:%d ����:%d * %d",range,dam,num);
			msg_print("��������K���Ɏa��܂������I");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_MISSILE, dam,2, 0,range)) flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("�������G����������Ȃ������B");
					break;
				}
			}

		}
		break;
	case 5:
		{
			int dam = 0;

			if(inventory[INVEN_RARM].tval == TV_KATANA)
				dam += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_KATANA)
				dam += calc_weapon_dam(1);

			//v1.1.30 �񓁗����Ƃ�����Ƌ�������̂Ŏ�̉�
			if(inventory[INVEN_RARM].k_idx && inventory[INVEN_LARM].k_idx) 
				dam = dam * 2 / 3;

			dam = dam * (20 + adj_general[p_ptr->stat_ind[A_CHR]]*2/3 ) / 20; 
			if(only_info) return format("����:�`%d",dam/2);
			project(0, (cave_have_flag_bold(py, px, FF_PROJECT) ? 3 : 0), py, px, dam, GF_HELL_FIRE, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);

			break;
		}

	case 6:
		{
			int len = p_ptr->lev / 10;
			if(only_info) return format("�˒�:%d",len);
			if(p_ptr->do_martialarts)
			{
				msg_print("����������Ȃ��Ƃ��̋Z�͎g���Ȃ��B");
				return NULL;
			}
			if (!rush_attack(NULL,len, HISSATSU_EIGOU)) return NULL;
			break;
		}

		//EPHEMERA�t���O�t���̃��j�[�N�𕪐g�Ƃ��ďo���B���j�[�N�o���\���𖳎�����B
	case 7:
		{
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_YOUMU) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt)
			{
				msg_format("���łɏp�����s���Ă���B",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_YOUMU, PM_EPHEMERA))
				msg_print("���Ȃ��̔��삪���̉������I");
			else
				msg_print("�p�Ɏ��s�����B");


			break;
		}
		//v1.1.30�ǉ�
	case 8:
		{
			if(only_info) return format("");
			flag_friendly_attack = TRUE;
			msg_format("�����̕��g���ӂ�ɎU����..");
			project_hack2(GF_YOUMU,0,0,100);
			flag_friendly_attack = FALSE;
		}
		break;

	case 9:
		{
			int dam=0;
			int typ[6] = {GF_NETHER, GF_CHAOS, GF_DISP_ALL, GF_HOLY_FIRE, GF_DISTORTION, GF_HELL_FIRE};
			if(inventory[INVEN_RARM].tval == TV_KATANA) dam += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_KATANA) dam += calc_weapon_dam(1);
			if(dam < 400) dam = 400;
			dam = dam * (60 + adj_general[p_ptr->stat_ind[A_CHR]]*2) / 30; 
			if(only_info) return format("����:�`%d",dam/2);
			project(0, (cave_have_flag_bold(py, px, FF_PROJECT) ? 5 : 0), py, px, dam, typ[randint0(6)], PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);

			break;
		}

	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}

/*:::���[�~�A�p���Z*/
class_power_type class_power_rumia[] =
{
	{6,3,20,FALSE,TRUE,A_CHR,0,1,"�i�C�g�o�[�h",
		"�Í������̃{���g����B"	},
	{17,11,25,FALSE,TRUE,A_CHR,0,7,"���[�����C�g���C",
		"�^�[�Q�b�g���ӂɈÍ������̃��[�U�[�𕡐����B�����ƃ^�[�Q�b�g�ɂ�������B"	},
	{25,20,35,FALSE,TRUE,A_WIS,0,0,"�Ő���",
		"�ꎞ�I�ɔZ���Ȉł��܂Ƃ��AAC�Ɩ��@�h����㏸�����đM���U���ւ̑ϐ��𓾂�B�܂�����2�O���b�h�̃����X�^�[�����X���Ȃ����������B���������Ȃ������͂������Ȃ��Ȃ�B"	},
	{25,0,0,FALSE,TRUE,A_WIS,0,0,"�ŉ���",
		"�Ő�������������B�ʏ�̔������x�̎��Ԃ�������Ȃ��B"	},
	{32,35,45,FALSE,TRUE,A_CON,0,0,"�f�B�}�[�P�C�V����",
		"���E���S�ĂɈÍ��U�����s���A����Ɍ��݂��镔�����Â�����B"},
	{36,45,60,FALSE,TRUE,A_CHR,0,0,"�~�b�h�i�C�g�o�[�h",
		"����ȈÍ������{�[������B"},
	{40,80,85,FALSE,TRUE,A_INT,10,0,"�_�[�N�T�C�h�I�u�U���[��",
		"�ÈłƓ������A�U�����󂯂����̃_���[�W�𔼌�������B����j�ׂ̍U�����󂯂�Ƒ�_���[�W���󂯌��ʂ�������B�u�Ő����v�͒��f�����B"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
cptr do_cmd_class_power_aux_rumia(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int dir,dice,sides,base;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: //�i�C�g�o�[�h
		{
			dice = 3 + plev / 6;
			sides = 5 + chr_adj / 5;
			if(only_info) return format("����:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			fire_bolt(GF_DARK, dir, damroll(dice, sides));
			break;

		}
	case 1:
		{
			dice = 2 + p_ptr->lev / 12;
			sides = 5 + chr_adj / 6;
			if(only_info) return format("����:(%dd%d)*%d",dice,sides,(plev/5));
			if (!get_aim_dir(&dir)) return NULL;
			fire_blast(GF_DARK, dir, dice, sides, (p_ptr->lev / 5), 3,(PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM));
			break;
		}

	/*:::�Ő����@p_ptr->tim_genaral[0]���J�E���^�Ƃ��Ďg��*/
	case 2:
		{
			base = p_ptr->lev;
			if(only_info) return format("����:%d",base);
			set_tim_general(base,FALSE,0,0);
			set_blind(1);
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			set_tim_general(0,TRUE,0,0);
			new_class_power_change_energy_need = 50;
			break;
		}
	case 4:
		{
			base = chr_adj * 4 + plev * 2;
			if(only_info) return format("����:%d",base);
			msg_print("�������łɕ���ꂽ�I");
			project_hack2(GF_DARK,0,0,base);
			(void)unlite_room(py,px);
			break;
		}
	case 5: //v1.1.30 �ǉ�
		{
			int range = 7;
			int dam = plev * 3 + chr_adj * 5;

			if(only_info) return format("�˒�:%d ����:%d",range,dam);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("�Z���Ȉł�@�������I");
			fire_ball(GF_DARK,dir,dam,3);

			break;
		}

	case 6:
		{
			base = p_ptr->lev / 2;
			if(base < 10) base = 10;
			if(only_info) return format("����:%d + 1d%d",base,base);
			set_tim_general(0,TRUE,0,0);
			set_wraith_form(randint1(base) + base, FALSE);
			break;
		}
	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;
	}
	return "";
}



/*:::���C�h�E�����p���Z�ꗗ*/
class_power_type class_power_maid[] =
{
	{	3,3,5,FALSE,TRUE,A_INT,0,0,"�Ɩ�",
		"���݂��镔���𖾂邭�Ƃ炷�B"	},
	{	6,6,10,FALSE,TRUE,A_DEX,0,0,"�{��",
		"�w�肵�������ł��{������B�{�����ꂽ���͉����ł��Ȃ����A�@���󂵂��薂�@�ŊJ���邱�Ƃ͂ł���B"},
	{	8,10,10,TRUE,FALSE,A_INT,0,0,"�A�E�g�h�A�N�b�L���O",
		"�H�ׂ�����̂𒲒B���Čg�ѐH���Ɏd�グ��B"	},
	{	16,12,20,FALSE,FALSE,A_WIS,0,0,"���}�蓖",
		"���������Ă���j�A���A�����Ȃǂ��g���ď���ł�N�O�����Â��A�킸����HP���񕜂���B"	},
	{	24,20,50,FALSE,FALSE,A_INT,0,0,"�]�҂̊��",
		"�l�X�ȗ��q�Œb����ꂽ��͂ɂ��A���E���̓G����u�Œ�������B"	},
	{	30,50,65,TRUE,FALSE,A_STR,0,0,"�A��",
		"�_���W�����̉��[�������i���킸�A������B"	},
	{	40,1,70,FALSE,FALSE,A_CHR,0,0,"�e�B�[�^�C��",
		"�����b�N�X����MP���񕜂���B�����َq�Ƃ��ĉَq�������B"	},

	{	99,0,0,FALSE,FALSE,0,0,0,"dummy",	""	},
};

/*:::���C�h�E�����p���Z���s��*/
/*:::�^�[�Q�b�g�I���ŃL�����Z�������Ƃ��ȂǍs��������Ȃ�����NULL��Ԃ�*/
cptr do_cmd_class_power_aux_maid(int num, bool only_info)
{
	int dir;
	switch(num)
	{
	case 0:
		{
			int dice,sides,base;
			dice=2;sides=p_ptr->lev/10;base=2;
			if(only_info) return format("����:%dd%d+%d", dice, sides,base);
			lite_area(damroll(dice, sides)+base, p_ptr->lev / 10 + 1);
			break;
		}
	case 1:
		{
			if(only_info) return "�w�肵�������{��";
			if (!get_aim_dir(&dir)) return NULL;
			wizard_lock(dir);
			break;
		}
	case 2:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "�H���𐶐�";
			msg_print("�H���𒲒B�����B");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}
	case 3:
		{
			int heal = p_ptr->lev / 2;
			if(only_info) return format("��:%d",heal);
			hp_player(heal);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			break;
		}
	case 4:
		{
			if(only_info) return "";
			probing();
			break;
		}

	case 5:
		{
			if(only_info) return "";
			if (!word_of_recall()) return NULL;
			break;
		}
	/*:::�e�B�[�^�C���@p_ptr->tim_genaral[0]���g��*/
	case 6:
		{
			cptr q,s;
			object_type *o_ptr;
			int item;
			if(only_info) return "";

			if(p_ptr->tim_general[0])
			{
				msg_format("�����͂��������񂾂΂��肾�B");
				return NULL;
			}

			item_tester_tval = TV_SWEETS;
			q = "�ǂ̂��َq���g���܂���? ";
			s = "�����َq���Ȃ��ƃe�B�[�^�C�����y���߂Ȃ��B";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;
			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
				o_ptr = &inventory[item];
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);	
				o_ptr = &o_list[0 - item];
			}
			/*:::�َq��pval/10��MP���񕜂���*/
			msg_format("���Ȃ��͂����🹂�A�ЂƂƂ��̈��炬�ɐZ�����B");
			p_ptr->csp += o_ptr->pval / 10;
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
			set_tim_general(100,FALSE,0,0);
			if (!(prace_is_(RACE_GOLEM) || prace_is_(RACE_ANDROID) || prace_is_(RACE_TSUKUMO) || (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING)))
				(void)set_food(p_ptr->food + o_ptr->pval + 500);
			break;
		}


	default:
		msg_format("ERROR:�������Ă��Ȃ����Z���Ă΂ꂽ num:%d",num);
		return NULL;

	}
	return "";
}





/*:::���Z�̌ʎg�p�۔���֐� TRUE���Ԃ������g�p�\*/
bool check_class_skill_usable(char *errmsg,int skillnum, class_power_type *class_power_table)
{
	
	bool have_melee_weapon = (buki_motteruka(INVEN_RARM) || buki_motteruka(INVEN_LARM));

	if(p_ptr->wizard)//�f�o�b�O���͂��ׂĎg����
	{
		errmsg = "";
		return TRUE; 
	}

	if( (class_power_table+skillnum)->is_magic && dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
		my_strcpy(errmsg, "�_���W���������͂��z�������I", 150);
		return FALSE;
	}
	if((class_power_table+skillnum)->is_magic && (p_ptr->anti_magic))
	{
		my_strcpy(errmsg, "�����@�o���A���ז��ŏW���ł��Ȃ��B", 150);
		return FALSE;
	}

	if(p_ptr->shero)
	{
		if( p_ptr->pseikaku == SEIKAKU_BERSERK); //���i���C
		else if(p_ptr->pclass == CLASS_JUNKO) ; //v1.1.17 ���ς͋���m����Ԃł����Z���g����
		else if(p_ptr->pclass == CLASS_SUIKA  && skillnum == 1);//�u���X
		else if(p_ptr->pclass == CLASS_WAKASAGI  && (skillnum == 3 || skillnum == 5));//�t��
		else if(p_ptr->pclass == CLASS_YOUMU  && (skillnum == 0 || skillnum == 3 || skillnum == 4 || skillnum == 6));//�����a,�����i���a
		else if(p_ptr->pclass == CLASS_KOGASA  && skillnum == 1);//�t���X�C���O
		else if(p_ptr->pclass == CLASS_KASEN && !is_special_seikaku(SEIKAKU_SPECIAL_KASEN) && skillnum == 3);//�劅
		else if(p_ptr->pclass == CLASS_KAGEROU);//�e�T�͋���m�����ɂ����Z���g����
		else if(p_ptr->pclass == CLASS_YOSHIKA  && skillnum == 2);//���ȂȂ��E�l�S
		else if (p_ptr->pclass == CLASS_KOKORO  && skillnum == 4);//��{���y�|�[�b�V����

		else
		{
			my_strcpy(errmsg, "����m�����Ă��ĕ��G�Ȃ��Ƃ��ł��Ȃ��B", 150);
			return FALSE;
		}
	}
	/*:::����ϐg��*/
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)
	{
		if(p_ptr->pclass == CLASS_KAGEROU && p_ptr->mimic_form)
		{
			//�e�T�͕ϐg���ɂ����Z���g����
		}
		else
		{
			my_strcpy(errmsg, "���̎p�ł͂��̋Z�͎g���Ȃ��B", 150);
			return FALSE;
		}
	}
	/*:::�܋�����*/
	if (p_ptr->clawextend)
	{
		//�������ւ�
		if(p_ptr->pclass == CLASS_RANGER  && skillnum == 0)
		{
			my_strcpy(errmsg, "���͕���̎����ւ����ł��Ȃ��B", 150);
			return FALSE;
		}
	}

	//�ȉ��A�E�Ƃ��Ɠ��ꏈ���@����switch�ɂ��Ƃ��΂悩����
	if(p_ptr->pclass == CLASS_SANAE)
	{
		int cons_soma = (p_ptr->magic_num1[0] == CONCENT_SOMA) ? p_ptr->concent : 0;
		int cons_kana = (p_ptr->magic_num1[0] == CONCENT_KANA) ? p_ptr->concent : 0;
		int cons_suwa = (p_ptr->magic_num1[0] == CONCENT_SUWA) ? p_ptr->concent : 0;
		bool kana_unable = FALSE, suwa_unable = FALSE;
		int i;
		for (i = 1; i < m_max; i++)
		{
			if(m_list[i].r_idx == MON_KANAKO && is_hostile(&m_list[i])) kana_unable = TRUE;
			else if(m_list[i].r_idx == MON_SUWAKO && is_hostile(&m_list[i])) suwa_unable = TRUE;
		}


		switch(skillnum)
		{
		case 7: case 10: case 12: case 14: case 16: case 19: case 20:
			if(cons_soma < sanae_minimum_cons[skillnum])
			{
				my_strcpy(errmsg, "���̊�Ղ��N�����ɂ͔�p�̉r��������Ȃ��B", 150);
				return FALSE;
			}
			break;
		case 4: case 11: case 17: case 21:
			if(kana_unable)
			{
				my_strcpy(errmsg, "�_�ގq�l�͗͂�݂��ĉ����肻���ɂȂ��E�E", 150);
				return FALSE;
			}
			if(cons_kana < sanae_minimum_cons[skillnum])
			{
				my_strcpy(errmsg, "���̊�Ղ��N�����ɂ͐_�ގq�l�ɕ�����r��������Ȃ��B", 150);
				return FALSE;
			}
			break;
		case 5: case 8: case 13: case 18:
			if(suwa_unable)
			{
				my_strcpy(errmsg, "�z�K�q�l�͗͂�݂��ĉ����肻���ɂȂ��E�E", 150);
				return FALSE;
			}
			if(cons_suwa < sanae_minimum_cons[skillnum])
			{
				my_strcpy(errmsg, "���̊�Ղ��N�����ɂ͐z�K�q�l�ɕ�����r��������Ȃ��B", 150);
				return FALSE;
			}
			break;
		}
		if(skillnum == 21 && !p_ptr->magic_num1[1])
		{
				my_strcpy(errmsg, "���̊�Ղ��N�����ɂ͏������K�v���B", 150);
				return FALSE;
		}
	}
	//�ɂƂ�@���n�`����A���d�G�l���M�[����
	else if(p_ptr->pclass == CLASS_NITORI)
	{
		if((skillnum == 5 || skillnum == 7) && !cave_have_flag_bold(py,px,FF_WATER))
		{
			my_strcpy(errmsg, "�����ɂ͐����Ȃ��B", 150);
			return FALSE;
		}

		if(nitori_energy[skillnum] > p_ptr->magic_num1[20])
		{
			my_strcpy(errmsg, "�G�l���M�[������Ȃ��B�������܂Ȃ��ƁB", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_BYAKUREN) //���@
	{

		if( (skillnum == 4 || skillnum == 9) && !p_ptr->tim_general[0])
		{
			my_strcpy(errmsg, "�܂��g�̂��������Ȃ��Ɗ댯���B", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_NAZRIN)
	{

		//�y���f�������K�[�h
		if (skillnum == 11 && !check_equip_specific_fixed_art(ART_NAZRIN, FALSE))
		{
			my_strcpy(errmsg, "���̋Z�̓y���f���������������Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
		//�󓃃��[�U�[
		if(skillnum == 15 && !check_equip_specific_fixed_art(ART_HOUTOU,FALSE))
		{
			my_strcpy(errmsg, "���̋Z�͔�����V�̕󓃂��������Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}

	}
	else if(p_ptr->pclass == CLASS_YOUMU)
	{
		if(!have_melee_weapon && ( skillnum == 0 || skillnum == 6)) 
		{
			my_strcpy(errmsg, "���̋Z�͋ߐڕ���𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
		if((inventory[INVEN_RARM].name1 != ART_HAKUROUKEN) && (inventory[INVEN_LARM].name1 != ART_HAKUROUKEN) && ( skillnum == 1)) 
		{
			my_strcpy(errmsg, "���̋Z�͔��O���𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
		if((inventory[INVEN_RARM].name1 != ART_ROUKANKEN) && (inventory[INVEN_LARM].name1 != ART_ROUKANKEN) && ( skillnum == 2)) 
		{
			my_strcpy(errmsg, "���̋Z�͘O�ό��𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}

		if(inventory[INVEN_RARM].tval != TV_KATANA && inventory[INVEN_LARM].tval != TV_KATANA 
			&& (skillnum == 4 || skillnum == 5 || skillnum == 8 ))
		{
			my_strcpy(errmsg, "���̋Z�͓��𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
		if((inventory[INVEN_RARM].tval != TV_KATANA || inventory[INVEN_LARM].tval != TV_KATANA) 
			&& (skillnum == 9 ))
		{
			my_strcpy(errmsg, "���̋Z�͓����{�������Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_MAGIC_KNIGHT )
	{
		if( !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
		{
			my_strcpy(errmsg, "����𑕔����Ă��Ȃ��Ɩ��@�����g���Ȃ��B", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_SYUGEN)
	{
		if(total_friends)
		{
			my_strcpy(errmsg, "�z���̏����ɖZ�������ґz���ł��Ȃ��B", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_TENSHI)
	{
		if((inventory[INVEN_RARM].name1 != ART_HISOU) && (inventory[INVEN_LARM].name1 != ART_HISOU) 
			&& ( skillnum == 0 || skillnum == 5 || skillnum == 10 || skillnum == 11)) 
		{
			my_strcpy(errmsg, "���̋Z�͔�z�̌��𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
	}


	else if (p_ptr->pclass == CLASS_PRIEST)
	{
		if( skillnum == 1 &&  !is_good_realm(p_ptr->realm1))
		{
			my_strcpy(errmsg, "���Ȃ��̐M����_�͏j���Ɩ������B", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_UDONGE)
	{
		if(p_ptr->blind && ( skillnum == 0 || skillnum == 2 || skillnum == 4 || skillnum == 5 || skillnum == 8 || skillnum == 10))
		{
			my_strcpy(errmsg, "���̋Z�͖ڂ������Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_FLAN)
	{
		if(skillnum == 4 && p_ptr->blind)
		{
			my_strcpy(errmsg, "���̋Z�͖ڂ������Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
		else if((skillnum == 1 || skillnum == 7) && !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
		{
			my_strcpy(errmsg, "���̋Z�͋ߐڕ���������Ă��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
	}
	else if((p_ptr->pclass == CLASS_MOMIZI || p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_LIBRARIAN || p_ptr->pclass == CLASS_HATATE))
	{
		if(p_ptr->blind)
		{
			my_strcpy(errmsg, "�ڂ������Ȃ��Ɠ��Z���g���Ȃ��B", 150);
			return FALSE;

		}
	}
	else if(p_ptr->pclass == CLASS_MIKO)
	{
		if( p_ptr->blind && skillnum == 2)
		{
			my_strcpy(errmsg, "���̓��Z�͖ڂ������Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_SHINMYOUMARU)
	{
		if(p_ptr->magic_num1[0] && ( skillnum != 15 )) 
		{
			my_strcpy(errmsg, "���Ƃ͗͂������Ă���B�Ăюg����̂͂��ɂȂ邱�Ƃ��E�E", 150);
			return FALSE;
		}
		
		else if(p_ptr->tim_general[0] && ( skillnum != 15 )) 
		{
			my_strcpy(errmsg, format("���Ƃ͂܂��[�U�����B(����%d�^�[��)",p_ptr->tim_general[0]), 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_KOISHI)
	{
		if(heavy_armor() && ( skillnum == 3 )) 
		{
			my_strcpy(errmsg, "�������������d���Ėʓ|���B", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_CHEN)
	{
		if(p_ptr->magic_num1[0] && ( skillnum != 1 )) 
		{
			my_strcpy(errmsg, "����������Ă��邽�ߏp���g���Ȃ��B", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SUMIREKO && !is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
	{
		bool smapho = FALSE;
		int i;
		if(skillnum == 11)
		{
			for(i=0;i<INVEN_TOTAL;i++) if(inventory[i].tval == TV_SOUVENIR && inventory[i].sval == SV_SOUVENIR_SMARTPHONE) smapho = TRUE;
			if(!smapho)
			{
				my_strcpy(errmsg, "�X�}�z�������ĂȂ��Ǝg���Ȃ��B", 150);
				return FALSE;
			}
		}
		else if(skillnum == 2 && !p_ptr->do_martialarts)
		{
			my_strcpy(errmsg, "������g���Ă�������邱�Ƃ͂ł��Ȃ��B", 150);
			return FALSE;
		}


	}
	else if(p_ptr->pclass == CLASS_SHOU) //�󓃃��[�U�[
	{
		if( skillnum != 1)
		{
			bool houtou = FALSE;
			int i;
			for(i=0;i<INVEN_TOTAL;i++) if(inventory[i].name1 == ART_HOUTOU) houtou = TRUE;

			if(!houtou)
			{
				my_strcpy(errmsg, "���̋Z�͔�����V�̕󓃂��������Ă��Ȃ��Ǝg���Ȃ��B", 150);
				return FALSE;
			}
		}
	}
	else if(p_ptr->pclass == CLASS_CLOWNPIECE) //�w���G�N���v�X
	{
		if(skillnum == 2)
		{
			bool torch = FALSE;
			int i;
			for(i=0;i<INVEN_TOTAL;i++) if(inventory[i].name1 == ART_CLOWNPIECE) torch = TRUE;
			if(!torch)
			{
				my_strcpy(errmsg, "���̋Z�͒n���̏������������Ă��Ȃ��Ǝg���Ȃ��B", 150);
				return FALSE;
			}
		}
	}
	else if (p_ptr->pclass == CLASS_VFS_CLOWNPIECE) 
	{
		if (skillnum == 0 || skillnum == 1 || skillnum == 4)
		{
			bool torch = FALSE;
			int i;
			for (i = 0; i<INVEN_TOTAL; i++) if (inventory[i].name1 == ART_CLOWNPIECE) torch = TRUE;
			if (!torch)
			{
				my_strcpy(errmsg, "���̋Z�͒n���̏������������Ă��Ȃ��Ǝg���Ȃ��B", 150);
				return FALSE;
			}
		}
	}
	else if(p_ptr->pclass == CLASS_DOREMY)
	{
		if( (skillnum==2 || skillnum==4 ||skillnum==7 ||skillnum==9) && !(IN_DREAM_WORLD))
		{
			my_strcpy(errmsg, "���̋Z�͖��̐��E�ł����g���Ȃ��B", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_IKU)
	{
			if(skillnum == 2 && !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			{
				my_strcpy(errmsg, "���̋Z�͕���𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
				return FALSE;
			}
			if (skillnum == 6 && !p_ptr->do_martialarts)
			{
				my_strcpy(errmsg, "���̋Z�͕���𑕔����Ă���Ǝg���Ȃ��B", 150);
				return FALSE;
			}
	}
	else if (p_ptr->pclass == CLASS_KOMACHI)
	{
		if( !(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING ))
		 && !(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING ))
	   	 &&  (skillnum == 7 || skillnum == 8 || skillnum == 11))
			{
				my_strcpy(errmsg, "���̋Z�͑劙�𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
				return FALSE;
			}
	}
	else if (p_ptr->pclass == CLASS_MINORIKO)
	{
		if( !(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING ))
		 && !(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING ))
	   	 &&  (skillnum == 1))
			{
				my_strcpy(errmsg, "���̋Z�͑劙�𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
				return FALSE;
			}
	}

	else if (p_ptr->pclass == CLASS_SAMURAI )
	{
		if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM) && skillnum == 1)
		{
				my_strcpy(errmsg, "������������ɍ\���Ă��Ӗ����Ȃ��B", 150);
				return FALSE;
		}

	}
	else if (p_ptr->pclass == CLASS_SHINMYOU_2 )
	{
		switch(skillnum)
		{
		case 0:
		case 2:
		case 5:
		case 6:
			if(inventory[INVEN_RARM].tval != TV_KNIFE && inventory[INVEN_RARM].tval != TV_SWORD && inventory[INVEN_RARM].tval != TV_KATANA 
				&& !(inventory[INVEN_RARM].tval == TV_OTHERWEAPON && inventory[INVEN_RARM].sval == SV_OTHERWEAPON_NEEDLE))
			{
				my_strcpy(errmsg, "���̋Z�͂��̑����ł͎g���Ȃ��B", 150);
				return FALSE;
			}
			break;
		case 1:
		case 3:
			{
				bool fishing = FALSE;
				int i;
				for(i=0;i<INVEN_TOTAL;i++)
				{
					if(inventory[i].tval == TV_OTHERWEAPON && inventory[i].sval == SV_OTHERWEAPON_FISHING) fishing = TRUE;
					if(inventory[i].name1 == ART_SHINMYOUMARU) fishing = TRUE;
				}

				if(!fishing)
				{
					my_strcpy(errmsg, "���̋Z�͒ނ�Ƃ������������Ă��Ȃ��Ǝg���Ȃ��B", 150);
					return FALSE;
				}
			}
			break;
		case 4:
			{
				if(!is_special_seikaku(SEIKAKU_SPECIAL_SHINMYOUMARU))
				{
					my_strcpy(errmsg, "���̋Z�͈ꕔ�̐��i�̂Ƃ��ɂ����g���Ȃ��B", 150);
					return FALSE;
				}
				else if (p_ptr->cursed & TRC_AGGRAVATE)
				{
					my_strcpy(errmsg, "���̋Z�͔�����������Ă���Ƃ��ɂ͎g���Ȃ��B", 150);
					return FALSE;

				}
			}
		break;

		}

	}
	else if (p_ptr->pclass == CLASS_FUTO)
	{
		if(skillnum == 7 && inventory[INVEN_LARM].tval != TV_BOW && inventory[INVEN_RARM].tval != TV_BOW) 
			{
				my_strcpy(errmsg, "�|���Ȃ��Ƃ��̋Z�͎g���Ȃ��B", 150);
				return FALSE;
			}

	}

	else if (p_ptr->pclass == CLASS_PATCHOULI)
	{
			if(skillnum > 4 && p_ptr->asthma > ASTHMA_2)
			{
				my_strcpy(errmsg, "�b���ŃX�y�����������Ȃ��B", 150);
				return FALSE;
			}
	}
	else if (p_ptr->pclass == CLASS_RUMIA)
	{
		if(skillnum == 2 && p_ptr->wraith_form)
		{
			my_strcpy(errmsg, "���͂��̓��Z���g���Ȃ��B", 150);
			return FALSE;
		}
		if(skillnum == 3 && !p_ptr->tim_general[0])
		{
			my_strcpy(errmsg, "�ł��o���Ă��Ȃ��B", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_MURASA)
	{
		if(skillnum == 3 && !cave_have_flag_bold((py), (px), FF_WATER))
		{
				my_strcpy(errmsg, "���̋Z�͐��̒��łȂ��Ǝg���Ȃ��B", 150);
				return FALSE;
		}
		if(skillnum == 5 && (!cave_have_flag_bold((py), (px), FF_WATER) || !cave_have_flag_bold((py), (px), FF_DEEP)))
		{
				my_strcpy(errmsg, "���̋Z�͐[�����̒��łȂ��Ǝg���Ȃ��B", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_REIMU)
	{
		int rank = osaisen_rank();
		if(		skillnum == 2 && rank < 2
			||  skillnum == 3 && rank < 4
			||  skillnum == 4 && rank < 2
			||  skillnum == 5 && rank < 5
			||  skillnum == 6 && rank < 4
			||  skillnum == 7 && rank < 7
			||  skillnum == 8 && rank < 3
			||  skillnum == 9 && rank < 6
			||  skillnum ==10 && rank < 4
			||  skillnum ==11 && rank < 7
			||  skillnum ==12 && rank < 5
			||  skillnum ==13 && rank < 8
			||  skillnum ==14 && rank < 9
			)
		{
				my_strcpy(errmsg, "�����������܂ł��C�ɂ͂Ȃ�Ȃ��B", 150);
				return FALSE;
		}
		if(skillnum == 11 && (inventory[INVEN_RARM].tval != TV_STICK || inventory[INVEN_RARM].sval != SV_WEAPON_OONUSA) && (inventory[INVEN_LARM].tval != TV_STICK || inventory[INVEN_LARM].sval != SV_WEAPON_OONUSA))
		{
				my_strcpy(errmsg, "����𕏂łĂ݂��������o���C�z�͂Ȃ��B", 150);
				return FALSE;
		}
		if(skillnum == 6)
		{
				if (!in_bounds2(py + 1,px) || !cave_have_flag_grid( &cave[py + 1][px],FF_PROJECT)
				||  !in_bounds2(py - 1,px) || !cave_have_flag_grid( &cave[py - 1][px],FF_PROJECT)
				||  !in_bounds2(py ,px + 1) || !cave_have_flag_grid( &cave[py ][px + 1],FF_PROJECT)
				||  !in_bounds2(py ,px - 1) || !cave_have_flag_grid( &cave[py ][px - 1],FF_PROJECT))
				{
					my_strcpy(errmsg, "�ǂ��ז����I", 150);
					return FALSE;
				}
		}
	}
	else if(p_ptr->pclass == CLASS_KAGEROU)
	{
		if(skillnum == 0 && !(FULL_MOON))
		{
				my_strcpy(errmsg, "�����̔ӂłȂ��Ǝ��͂ŘT�ɂȂ�Ȃ��B", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SEIJA)
	{
		if(skillnum == 2 || skillnum == 8 || skillnum == 11)
		{
				my_strcpy(errmsg, "���̓���͎󓮓I�ɔ���������̂��B", 150);
				return FALSE;
		}
		if(skillnum == 4 && !p_ptr->magic_num1[SEIJA_ITEM_SUKIMA]
		|| skillnum == 5 && !p_ptr->magic_num1[SEIJA_ITEM_DECOY]
		|| skillnum == 6 && !p_ptr->magic_num1[SEIJA_ITEM_BOMB]
		|| skillnum == 9 && !p_ptr->magic_num1[SEIJA_ITEM_ONMYOU]
		|| skillnum == 10 && !p_ptr->magic_num1[SEIJA_ITEM_KODUCHI]
		|| skillnum == 12 && !p_ptr->magic_num1[SEIJA_ITEM_CHOCHIN]	)
		{
				my_strcpy(errmsg, "���̓���͎g�p�񐔂��؂�Ă���B", 150);
				return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_UTSUHO)
	{
		if( !empty_hands(TRUE))
		{
			my_strcpy(errmsg, "�肪�ǂ����Ă��Ċj�Z���𐧌�ł��Ȃ��B", 150);
			return FALSE;

		}
	}

	else if (p_ptr->pclass == CLASS_SUWAKO)
	{
		if( skillnum == 5)
		{
			if(p_ptr->food < PY_FOOD_ALERT)
			{
				my_strcpy(errmsg, "�������󂢂Ė���Ȃ��B", 150);
				return FALSE;
			}
			else if(!cave_have_flag_bold(py,px,FF_MOVE))
			{
				my_strcpy(errmsg, "�����ł͗��������Ė���Ȃ��B", 150);
				return FALSE;
			}
		}
	}
	else if(p_ptr->pclass == CLASS_ICHIRIN)
	{
		if((skillnum == 0 || skillnum == 3 || skillnum == 4 || skillnum == 5 || skillnum == 6 || skillnum == 7) && !p_ptr->do_martialarts)
		{
				my_strcpy(errmsg, "�_�R�Ƃ��܂��A�g�ł��Ȃ��B", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_YUGI)
	{
		if((skillnum == 7) && !p_ptr->do_martialarts)
		{
				my_strcpy(errmsg, "���̋Z�͑f��łȂ��Ǝg���Ȃ��B", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_YOSHIKA)
	{
		if((skillnum == 2) && !p_ptr->do_martialarts)
		{
				my_strcpy(errmsg, "���̋Z�͑f��łȂ��Ǝg���Ȃ��B", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_ALICE)
	{
		if((skillnum == 8) && p_ptr->do_martialarts)
		{
				my_strcpy(errmsg, "�l�`�̑������ł��Ă��Ȃ��B", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SEIRAN)
	{
		if(skillnum == 0
			&& !(inventory[INVEN_RARM].tval == TV_HAMMER  && inventory[INVEN_RARM].sval == SV_WEAPON_KINE)
			&& !(inventory[INVEN_LARM].tval == TV_HAMMER  && inventory[INVEN_LARM].sval == SV_WEAPON_KINE))
		{
			my_strcpy(errmsg, "�n�𑕔����Ă��Ȃ��B", 150);
			return FALSE;
		}
		if(((skillnum == 1) ||(skillnum == 3) ||(skillnum == 4) ||(skillnum == 5) ||(skillnum == 6) || (skillnum == 7) || (skillnum == 8)) && !(CHECK_USE_GUN))
		{
			my_strcpy(errmsg, "�e�𑕔����Ă��Ȃ��B", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_REISEN2)
	{
		if(skillnum == 0
			&& !(inventory[INVEN_RARM].tval == TV_HAMMER  && inventory[INVEN_RARM].sval == SV_WEAPON_KINE)
			&& !(inventory[INVEN_LARM].tval == TV_HAMMER  && inventory[INVEN_LARM].sval == SV_WEAPON_KINE))
		{
			my_strcpy(errmsg, "�n�𑕔����Ă��Ȃ��B", 150);
			return FALSE;
		}
	}


	else if(p_ptr->pclass == CLASS_HINA)
	{
		if(((skillnum == 1) ||(skillnum == 3) ||(skillnum == 6) ||(skillnum == 7) ||(skillnum == 8) ||(skillnum == 9)) && !p_ptr->magic_num1[0])
		{
				my_strcpy(errmsg, "�����Ȃ��Ă��̓��Z���g���Ȃ��B", 150);
				return FALSE;
		}
		if((skillnum == 9) && !dun_level)
		{
				my_strcpy(errmsg, "�n��ł���Ȃ��Ƃ����Ă͂����Ȃ��B", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_YUKARI)
	{
		if(p_ptr->wild_mode && skillnum != 12)
		{
				my_strcpy(errmsg, "�L��}�b�v�ł��̓��Z���g�����Ƃ͂ł��Ȃ��B", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_RINGO)
	{
		if(skillnum == 3 && !p_ptr->magic_num1[2])
		{
			my_strcpy(errmsg, "�����U�����󂯂Ă��Ȃ��B", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_MEIRIN)
	{
		if( (!p_ptr->do_martialarts || p_ptr->riding) && skillnum > 1)
		{
			my_strcpy(errmsg, "���͊i���̋Z���g���Ȃ��B", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SAKUYA)
	{
		if((skillnum == 6 || skillnum == 7) && world_player)
		{
			my_strcpy(errmsg, "��������s�����Ƃ͂ł��Ȃ��B", 150);
			return FALSE;
		}
		if(skillnum == 6 && SAKUYA_WORLD)
		{
			my_strcpy(errmsg, "���łɂ����͂��Ȃ��̐��E���B", 150);
			return FALSE;
		}
		if(skillnum == 7 && !SAKUYA_WORLD)
		{
			my_strcpy(errmsg, "���͎����~�߂Ă��Ȃ��B", 150);
			return FALSE;
		}
		if(skillnum == 14 && inventory[INVEN_LARM].tval != TV_KNIFE && inventory[INVEN_RARM].tval != TV_KNIFE)
		{
			my_strcpy(errmsg, "�Z���𑕔����Ă��Ȃ��Ƃ��̋Z�͎g���Ȃ��B", 150);
			return FALSE;
		}

	}
	else if (p_ptr->pclass == CLASS_BENBEN)
	{
			if(skillnum == 8 && (buki_motteruka(INVEN_RARM) || buki_motteruka(INVEN_LARM)))
			{
				my_strcpy(errmsg, "���̉��t�͕���𑕔����Ă���Ǝg���Ȃ��B", 150);
				return FALSE;
			}
	}
	else if (p_ptr->pclass == CLASS_KAGUYA)
	{
		//�V���ۗL����
		if( skillnum ==  7 && check_have_specific_item(TV_SOUVENIR,SV_SOUVENIR_ILMENITE) < 0
		||	skillnum ==  9 && check_have_specific_item(TV_MATERIAL,SV_MATERIAL_MYSTERIUM) < 0
		||	skillnum == 13 && check_have_specific_item(TV_SOUVENIR,SV_SOUVENIR_ASIA) < 0
		||	skillnum == 15 && check_have_specific_item(TV_SOUVENIR,SV_SOUVENIR_KINKAKUJI) < 0)
		{
			my_strcpy(errmsg, "���̃A�C�e���������Ă��Ȃ��B", 150);
			return FALSE;
		}
	}

	else if (p_ptr->pclass == CLASS_MARTIAL_ARTIST)
	{
		if(skillnum == 16 && p_ptr->tim_general[1])
		{
			my_strcpy(errmsg, "�E�����̘A���g�p�͑̂������Ȃ��B", 150);
			return FALSE;
		}
		if((skillnum == 1 || skillnum == 2 || skillnum == 3 || skillnum == 5 || skillnum == 7 || skillnum == 10
			|| skillnum == 12 || skillnum == 14 || skillnum == 15) && !p_ptr->do_martialarts)
		{
			my_strcpy(errmsg, "���̋Z�͊i�����łȂ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
		switch(p_ptr->prace)
		{
		case RACE_HUMAN:
		case RACE_YOUKAI:
		case RACE_HALF_YOUKAI:
		case RACE_SENNIN:
		case RACE_TENNIN:
		case RACE_FAIRY:
		case RACE_KAPPA:
		case RACE_YAMAWARO:
		case RACE_MAGICIAN:
		case RACE_KARASU_TENGU:
		case RACE_HAKUROU_TENGU:
		case RACE_ONI:
		case RACE_WARBEAST:
		case RACE_GYOKUTO:
		case RACE_YOUKO:
		case RACE_BAKEDANUKI:
		case RACE_NINGYO:
		case RACE_HOFGOBLIN:
		case RACE_NYUDOU:
		case RACE_KOBITO:
		case RACE_DAIYOUKAI:
		case RACE_DEMIGOD:
		case RACE_NINJA:
		case RACE_ULTIMATE:
		case RACE_LUNARIAN:
			if(skillnum == 7)
			{
				my_strcpy(errmsg, "���Ȃ��̎푰�ł͂��̋Z�͎g���Ȃ��B", 150);
				return FALSE;
			}
			break;
		default:
			if(skillnum == 12)
			{
				my_strcpy(errmsg, "���Ȃ��̎푰�ł͂��̋Z�͎g���Ȃ��B", 150);
				return FALSE;
			}
			break;
		}
	}
	else if(p_ptr->pclass == CLASS_RAIKO)
	{
		if(skillnum == 6)
		{
				if (!(in_bounds2(py + 1,px) && cave_have_flag_grid( &cave[py + 1][px],FF_WALL)
				||  in_bounds2(py - 1,px) && cave_have_flag_grid( &cave[py - 1][px],FF_WALL)
				||  in_bounds2(py ,px + 1) && cave_have_flag_grid( &cave[py ][px + 1],FF_WALL)
				||  in_bounds2(py ,px - 1) && cave_have_flag_grid( &cave[py ][px - 1],FF_WALL)))
				{
					my_strcpy(errmsg, "�ǂ̂ƂȂ�ɂ��Ȃ��Ƃ��̋Z�͎g���Ȃ��B", 150);
					return FALSE;
				}
		}
	}
	else if(p_ptr->pclass == CLASS_YORIHIME)
	{
		//�˕P�̋_���l�̌��͌������𑕔����Ă���K�v������
		if(skillnum == 1)
		{
			bool flag_ok = FALSE;
			if( inventory[INVEN_RARM].tval == TV_SWORD || inventory[INVEN_LARM].tval == TV_SWORD) flag_ok = TRUE;
			if( inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) flag_ok = TRUE;
			if(!flag_ok)
			{
				my_strcpy(errmsg, "���̋Z�͌������𑕔����Ă��Ȃ��Ǝg���Ȃ��B", 150);
				return FALSE;
			}


		}

	}
	else if(p_ptr->pclass == CLASS_TEWI)
	{
		if(skillnum == 5 && (p_ptr->cursed & TRC_AGGRAVATE))
		{
			my_strcpy(errmsg, "���͉��������Ă��M�p���ꂻ���ɂȂ��B", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_HECATIA)
	{
		if((skillnum == 3 || skillnum == 6) && !hecatia_body_is_(HECATE_BODY_OTHER))
		{
				my_strcpy(errmsg, "���̋Z�͈يE�̑̂łȂ��Ǝg���Ȃ��B", 150);
				return FALSE;
		}
		if((skillnum == 4 || skillnum == 7) && !hecatia_body_is_(HECATE_BODY_EARTH))
		{
				my_strcpy(errmsg, "���̋Z�͒n���̑̂łȂ��Ǝg���Ȃ��B", 150);
				return FALSE;
		}
		if((skillnum == 5 || skillnum == 8) && !hecatia_body_is_(HECATE_BODY_MOON))
		{
				my_strcpy(errmsg, "���̋Z�͌��̑̂łȂ��Ǝg���Ȃ��B", 150);
				return FALSE;
		}

	}
	else if(p_ptr->pclass == CLASS_JUNKO)
	{
		if( !p_ptr->shero && ( skillnum == 8 || skillnum == 10))
		{
			my_strcpy(errmsg, "���͂����܂ł���قǂ̐�ӂ��Ȃ��B", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SOLDIER)
	{
		if( skillnum == 2 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_CURBING_SHOT)
		||	skillnum == 3 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_SRH_DIS_PLUS)
		||	skillnum == 4 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_SCAN_MONSTER)
		||	skillnum == 6 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_HEAD_SHOT)

		||	skillnum == 7 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETECTING)
		||	skillnum == 8 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_BARRICADE)
		||	skillnum == 9 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_ELEM_BULLET)
		||	skillnum ==11 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_HIT_AND_AWAY)
		||	skillnum ==12 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_PORTAL)
		||	skillnum ==13 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_CONCENT)
		||	skillnum ==14 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_SET_BOMB)
		||	skillnum ==15 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_RAPID_FIRE)
		||	skillnum ==17 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_MEGA_CRASH)
		||	skillnum ==18 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_MATERIAL_BULLET)

		||	skillnum ==19 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_RESIST)
		||	skillnum ==20 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_ADRENALIN_CONTROL)
		||	skillnum ==21 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR)
			
			)
		{
			my_strcpy(errmsg, "���̓��Z�̂��߂̋Z�\���K�����Ă��Ȃ��B", 150);
			return FALSE;
		}




	}

	else if (p_ptr->pclass == CLASS_NEMUNO)
	{
		if (!IS_NEMUNO_IN_SANCTUARY && (skillnum == 2 || skillnum == 6 || skillnum == 7))
		{
			my_strcpy(errmsg, "���̓��Z�͓꒣��̒��ɂ��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_KOSUZU)
	{
		if (skillnum == 6 && inventory[INVEN_RARM].name1 != ART_HYAKKI && inventory[INVEN_LARM].name1 != ART_HYAKKI )
		{
			my_strcpy(errmsg, "�G������Ɏ��Ȃ��Ƃ����Ȃ��B", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_SHION)
	{
		if (SUPER_SHION && (skillnum == 0 || skillnum == 1))
		{
			my_strcpy(errmsg, "���͂��̓��Z���g���Ȃ��B", 150);
			return FALSE;

		}
		else if (SUPER_SHION && (skillnum == 5))
		{
			my_strcpy(errmsg, "���łɕϐg���Ă���B", 150);
			return FALSE;

		}
		else if ((skillnum == 6) && p_ptr->magic_num1[1] < SHION_BINBOUDAMA)
		{
			my_strcpy(errmsg, "�\���ȕs�K�p���[�����܂��Ă��Ȃ��B", 150);
			return FALSE;

		}



		else if (!SUPER_SHION && (skillnum == 7))
		{
			my_strcpy(errmsg, "���͂��̓��Z���g���Ȃ��B", 150);
			return FALSE;

		}

	}
	else if (p_ptr->pclass == CLASS_MAI || p_ptr->pclass == CLASS_SATONO)
	{
		if (p_ptr->riding &&  skillnum == 6)
		{
			my_strcpy(errmsg, "�z���̔w�ォ��ł͂��̓��Z���g���Ȃ��B", 150);
			return FALSE;

		}
	}
	else if (p_ptr->pclass == CLASS_SAKI)
	{
		if (skillnum == 5 && dun_level)
		{
			my_strcpy(errmsg, "���̋Z�͒n��łȂ��Ǝg���Ȃ��B", 150);
			return FALSE;

		}

		if (!(CHECK_USE_GUN) && ((skillnum == 2) || (skillnum == 6) || (skillnum == 9)))
		{
			my_strcpy(errmsg, "�e�𑕔����Ă��Ȃ��B", 150);
			return FALSE;
		}

	}
	else if (p_ptr->pclass == CLASS_TAKANE)
	{
		if (skillnum == 7 && !cave_have_flag_bold((py), (px), FF_TREE))
		{
			my_strcpy(errmsg, "���̋Z�͐X�̒��ɂ��Ȃ��Ǝg���Ȃ��B", 150);
			return FALSE;

		}
	}





errmsg = "";
return TRUE;
}


/*:::�E�Ɠ��Z�̑I���ƎQ�ƂƐ�������*/
void do_cmd_new_class_power(bool only_browse)
{
	class_power_type *class_power_table;
	class_power_type *spell;
	cptr (*class_power_aux)( int, bool);
	cptr power_desc;
	char errmsg[160];
	int num; //�Z�ԍ� 0����J�n
    int cnt; //���݂́��ŗL���ȋZ�̐�
	bool  flag, redraw; //flag�͓��Znum�I���ς݃t���O
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

	int cur_pos;//�R�}���h�E�B���h�E����g�p����Ƃ�(use_menu)�̃J�[�\���ʒu

#ifndef JP
	return;
#endif

	//v1.1.37 ����̉���̓��ꏈ�� �A�C�e���J�[�h�ŒN�ł��ϐg�ł���̂ł����ɋL�q����
	if(p_ptr->mimic_form == MIMIC_KOSUZU_GHOST)
	{
		class_power_table = class_power_kosuzu_ghost;
		class_power_aux = do_cmd_class_power_aux_kosuzu_ghost;
		power_desc = "���Z";
	}
	else		/*:::�e�E�Ƃ��Ƃ̓��Z�̌ď́A�g�p����z��A�g�p����֐����L�q*/
	switch(p_ptr->pclass)
	{
	case CLASS_MAID:
		class_power_table = class_power_maid;
		class_power_aux = do_cmd_class_power_aux_maid;
		power_desc = "���Z";
		break;

	case CLASS_RUMIA:
		class_power_table = class_power_rumia;
		class_power_aux = do_cmd_class_power_aux_rumia;
		power_desc = "���Z";
		break;
	case CLASS_YOUMU:
		class_power_table = class_power_youmu;
		class_power_aux = do_cmd_class_power_aux_youmu;
		power_desc = "���Z";
		break;
	case CLASS_TEWI:
		class_power_table = class_power_tewi;
		class_power_aux = do_cmd_class_power_aux_tewi;
		power_desc = "���Z";
		break;
	case CLASS_UDONGE:
		if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
		{
			class_power_table = class_power_udonge_d;
			class_power_aux = do_cmd_class_power_aux_udonge_d;
		}
		else
		{
			class_power_table = class_power_udonge;
			class_power_aux = do_cmd_class_power_aux_udonge;
		}
		power_desc = "���Z";
		break;
	case CLASS_IKU:
		class_power_table = class_power_iku;
		class_power_aux = do_cmd_class_power_aux_iku;
		power_desc = "���Z";
		break;
	case CLASS_KOMACHI:
		class_power_table = class_power_komachi;
		class_power_aux = do_cmd_class_power_aux_komachi;
		power_desc = "���Z";
		break;
	case CLASS_KOGASA:
		class_power_table = class_power_kogasa;
		class_power_aux = do_cmd_class_power_aux_kogasa;
		power_desc = "���Z";
		break;
	case CLASS_KASEN:
		if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
		{
			class_power_table = class_power_oni_kasen;
			class_power_aux = do_cmd_class_power_aux_oni_kasen;
		}
		else
		{
			class_power_table = class_power_kasen;
			class_power_aux = do_cmd_class_power_aux_kasen;
		}
		power_desc = "���Z";
		break;
	case CLASS_MAGE:
	case CLASS_HIGH_MAGE:
		class_power_table = class_power_mage;
		class_power_aux = do_cmd_class_power_aux_mage;
		power_desc = "���Z";
		break;
	case CLASS_SUIKA:
		class_power_table = class_power_suika;
		class_power_aux = do_cmd_class_power_aux_suika;
		power_desc = "���Z";
		break;
	case CLASS_MARISA:
		//v2.0.1
		if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
		{
			class_power_table = class_power_card_dealer;
			class_power_aux = do_cmd_class_power_aux_card_dealer;

		}
		else
		{
			class_power_table = class_power_marisa;
			class_power_aux = do_cmd_class_power_aux_marisa;
		}
		power_desc = "���Z";
		break;

	case CLASS_WAKASAGI:
		class_power_table = class_power_wakasagi;
		class_power_aux = do_cmd_class_power_aux_wakasagi;
		power_desc = "���Z";
		break;
	case CLASS_ROGUE:
		class_power_table = class_power_rogue;
		class_power_aux = do_cmd_class_power_aux_rogue;
		power_desc = "���Z";
		break;
	case CLASS_RANGER:
		class_power_table = class_power_ranger;
		class_power_aux = do_cmd_class_power_aux_ranger;
		power_desc = "���Z";
		break;
	case CLASS_ARCHER:
		class_power_table = class_power_archer;
		class_power_aux = do_cmd_class_power_aux_archer;
		power_desc = "���Z";
		break;
	case CLASS_KOISHI:
		class_power_table = class_power_koishi;
		class_power_aux = do_cmd_class_power_aux_koishi;
		power_desc = "���Z";
		break;
	case CLASS_MOMIZI:
		class_power_table = class_power_momizi;
		class_power_aux = do_cmd_class_power_aux_momizi;
		power_desc = "���Z";
		break;
	case CLASS_SEIGA:
		class_power_table = class_power_seiga;
		class_power_aux = do_cmd_class_power_aux_seiga;
		power_desc = "���Z";
		break;
	case CLASS_PRIEST:
		class_power_table = class_power_priest;
		class_power_aux = do_cmd_class_power_aux_priest;
		power_desc = "���Z";
		break;
	case CLASS_ORIN:
		//v1.1.77
		if (is_special_seikaku(SEIKAKU_SPECIAL_ORIN))
		{
			class_power_table = class_power_orin2;
			class_power_aux = do_cmd_class_power_aux_orin2;
		}
		else
		{
			class_power_table = class_power_orin;
			class_power_aux = do_cmd_class_power_aux_orin;
		}
		power_desc = "���Z";
		break;
	case CLASS_CIRNO:
		class_power_table = class_power_cirno;
		class_power_aux = do_cmd_class_power_aux_cirno;
		power_desc = "���Z";
		break;
	case CLASS_SHINMYOUMARU:
		class_power_table = class_power_shinmyoumaru;
		class_power_aux = do_cmd_class_power_aux_shinmyoumaru;
		power_desc = "���Z";
		break;
	case CLASS_TSUKUMO_MASTER:
		class_power_table = class_power_tsukumo_master;
		class_power_aux = do_cmd_class_power_aux_tsukumo_master;
		power_desc = "���Z";
		break;
	case CLASS_NAZRIN:
		class_power_table = class_power_nazrin;
		class_power_aux = do_cmd_class_power_aux_nazrin;
		power_desc = "���Z";
		break;
	case CLASS_SYUGEN:
		class_power_table = class_power_syugen;
		class_power_aux = do_cmd_class_power_aux_syugen;
		power_desc = "�ґz";
		break;
	case CLASS_MAGIC_KNIGHT:
		switch(p_ptr->realm1)
		{
		case TV_BOOK_ELEMENT:
			class_power_table = class_power_magic_knight_elem;
			class_power_aux = do_cmd_class_power_aux_magic_knight_elem;
			break;
		case TV_BOOK_FORESEE:
			class_power_table = class_power_magic_knight_fore;
			class_power_aux = do_cmd_class_power_aux_magic_knight_fore;
			break;
		case TV_BOOK_ENCHANT:
			class_power_table = class_power_magic_knight_enchant;
			class_power_aux = do_cmd_class_power_aux_magic_knight_enchant;
			break;
		case TV_BOOK_SUMMONS:
			class_power_table = class_power_magic_knight_summon;
			class_power_aux = do_cmd_class_power_aux_magic_knight_summon;
			break;
		case TV_BOOK_DARKNESS:
			class_power_table = class_power_magic_knight_darkness;
			class_power_aux = do_cmd_class_power_aux_magic_knight_darkness;
			break;
		case TV_BOOK_CHAOS:
			class_power_table = class_power_magic_knight_chaos;
			class_power_aux = do_cmd_class_power_aux_magic_knight_chaos;
			break;
		default:
			msg_format("ERROR:���@���m�̂��̗̈�͓��Z���o�^����Ă��Ȃ�");
			return ;
		}
		power_desc = "���@��";
		break;

	case CLASS_LETTY:
		class_power_table = class_power_letty;
		class_power_aux = do_cmd_class_power_aux_letty;
		power_desc = "���Z";
		break;
	case CLASS_YOSHIKA:
		class_power_table = class_power_yoshika;
		class_power_aux = do_cmd_class_power_aux_yoshika;
		power_desc = "���Z";
		break;
	case CLASS_PATCHOULI:
		class_power_table = class_power_patchouli;
		class_power_aux = do_cmd_class_power_aux_patchouli;
		power_desc = "�X�y��";
		break;
	case CLASS_LIBRARIAN:
		class_power_table = class_power_librarian;
		class_power_aux = do_cmd_class_power_aux_librarian;
		power_desc = "���Z";
		break;
	case CLASS_AYA:
		class_power_table = class_power_aya;
		class_power_aux = do_cmd_class_power_aux_aya;
		power_desc = "���Z";
		break;
	case CLASS_BANKI:
		class_power_table = class_power_banki;
		class_power_aux = do_cmd_class_power_aux_banki;
		power_desc = "���Z";
		break;
	case CLASS_MYSTIA:
		class_power_table = class_power_mystia;
		class_power_aux = do_cmd_class_power_aux_mystia;
		power_desc = "���Z";
		break;
	case CLASS_ENGINEER:
		class_power_table = class_power_engineer;
		class_power_aux = do_cmd_class_power_aux_engineer;
		power_desc = "���Z";
		break;
	case CLASS_FLAN:
		class_power_table = class_power_flan;
		class_power_aux = do_cmd_class_power_aux_flan;
		power_desc = "���Z";
		break;
	case CLASS_SHOU:
		class_power_table = class_power_shou;
		class_power_aux = do_cmd_class_power_aux_shou;
		power_desc = "���Z";
		break;
	case CLASS_MEDICINE:
		class_power_table = class_power_medi;
		class_power_aux = do_cmd_class_power_aux_medi;
		power_desc = "���Z";
		break;
	case CLASS_SAMURAI:
		class_power_table = class_power_samurai;
		class_power_aux = do_cmd_class_power_aux_samurai;
		power_desc = "���Z";
		break;

	case CLASS_MINDCRAFTER:
		class_power_table = class_power_mind;
		class_power_aux = do_cmd_class_power_aux_syugen;
		power_desc = "���\��";
		break;

	case CLASS_CAVALRY:
		class_power_table = class_power_cavalry;
		class_power_aux = do_cmd_class_power_aux_cavalry;
		power_desc = "���Z";
		break;

	case CLASS_YUYUKO:
		class_power_table = class_power_yuyuko;
		class_power_aux = do_cmd_class_power_aux_yuyuko;
		power_desc = "���Z";
		break;
	case CLASS_PALADIN:
		class_power_table = class_power_paladin;
		class_power_aux = do_cmd_class_power_aux_paladin;
		power_desc = "���Z";
		break;
	case CLASS_SH_DEALER:
		class_power_table = class_power_sh_dealer;
		class_power_aux = do_cmd_class_power_aux_sh_dealer;
		power_desc = "���Z";
		break;
	case CLASS_JEWELER:
		class_power_table = class_power_jeweler;
		class_power_aux = do_cmd_class_power_aux_jeweler;
		power_desc = "���Z";
		break;
	case CLASS_SATORI:
		class_power_table = class_power_satori;
		class_power_aux = do_cmd_class_power_aux_satori;
		power_desc = "���Z";
		break;
	case CLASS_KYOUKO:
		class_power_table = class_power_kyouko;
		class_power_aux = do_cmd_class_power_aux_kyouko;
		power_desc = "���Z";
		break;
	case CLASS_TOZIKO:
		class_power_table = class_power_toziko;
		class_power_aux = do_cmd_class_power_aux_toziko;
		power_desc = "���Z";
		break;
	case CLASS_LILY_WHITE:
		class_power_table = class_power_lilywhite;
		class_power_aux = do_cmd_class_power_aux_lilywhite;
		power_desc = "���Z";
		break;
	case CLASS_KISUME:
		class_power_table = class_power_kisume;
		class_power_aux = do_cmd_class_power_aux_kisume;
		power_desc = "���Z";
		break;
	case CLASS_HATATE:
		class_power_table = class_power_hatate;
		class_power_aux = do_cmd_class_power_aux_hatate;
		power_desc = "���Z";
		break;
	case CLASS_MIKO:
		class_power_table = class_power_miko;
		class_power_aux = do_cmd_class_power_aux_miko;
		power_desc = "���Z";
		break;
	case CLASS_KOKORO:
		class_power_table = class_power_kokoro;
		class_power_aux = do_cmd_class_power_aux_kokoro;
		power_desc = "���Z";
		break;
	case CLASS_WRIGGLE:
		class_power_table = class_power_wriggle;
		class_power_aux = do_cmd_class_power_aux_wriggle;
		power_desc = "���Z";
		break;
	case CLASS_NINJA:
		class_power_table = class_power_ninja;
		class_power_aux = do_cmd_class_power_aux_ninja;
		power_desc = "�E�p";
		break;
	case CLASS_YUUKA:
		class_power_table = class_power_yuuka;
		class_power_aux = do_cmd_class_power_aux_yuuka;
		power_desc = "���Z";
		break;
	case CLASS_CHEN:
		class_power_table = class_power_chen;
		class_power_aux = do_cmd_class_power_aux_chen;
		power_desc = "���Z";
		break;
	case CLASS_MURASA:
		class_power_table = class_power_murasa;
		class_power_aux = do_cmd_class_power_aux_murasa;
		power_desc = "���Z";
		break;
	case CLASS_KEINE:
		if(p_ptr->magic_num1[0]) class_power_table = class_power_keine2;
		else class_power_table = class_power_keine1;
		class_power_aux = do_cmd_class_power_aux_keine;
		power_desc = "���Z";
		break;
	case CLASS_YUGI:
		class_power_table = class_power_yugi;
		class_power_aux = do_cmd_class_power_aux_yugi;
		power_desc = "���Z";
		break;

	case CLASS_REIMU:
		class_power_table = class_power_reimu;
		class_power_aux = do_cmd_class_power_aux_reimu;
		power_desc = "�p";
		break;

	case CLASS_KAGEROU:
		class_power_table = class_power_kagerou;
		class_power_aux = do_cmd_class_power_aux_kagerou;
		power_desc = "���Z";
		break;
	case CLASS_SHIZUHA:
		class_power_table = class_power_shizuha;
		class_power_aux = do_cmd_class_power_aux_shizuha;
		power_desc = "���Z";
		break;
	case CLASS_SANAE:
		class_power_table = class_power_sanae;
		class_power_aux = do_cmd_class_power_aux_sanae;
		power_desc = "���Z";
		break;

	case CLASS_MINORIKO:
		class_power_table = class_power_minoriko;
		class_power_aux = do_cmd_class_power_aux_minoriko;
		power_desc = "���Z";
		break;
	case CLASS_REMY:
		class_power_table = class_power_remy;
		class_power_aux = do_cmd_class_power_aux_remy;
		power_desc = "���Z";
		break;
	case CLASS_NITORI:
		class_power_table = class_power_nitori;
		class_power_aux = do_cmd_class_power_aux_nitori;
		power_desc = "���Z";
		break;
	case CLASS_BYAKUREN:
		class_power_table = class_power_byakuren;
		class_power_aux = do_cmd_class_power_aux_byakuren;
		power_desc = "�@��";
		break;
	case CLASS_NUE:
		class_power_table = class_power_nue;
		class_power_aux = do_cmd_class_power_aux_nue;
		power_desc = "���Z";
		break;
	case CLASS_SEIJA:
		class_power_table = class_power_seija;
		class_power_aux = do_cmd_class_power_aux_seija;
		power_desc = "���Z";
		break;
	case CLASS_UTSUHO:
		class_power_table = class_power_utsuho;
		class_power_aux = do_cmd_class_power_aux_utsuho;
		power_desc = "���Z";
		break;

	case CLASS_YAMAME:
		class_power_table = class_power_yamame;
		class_power_aux = do_cmd_class_power_aux_yamame;
		power_desc = "���Z";
		break;

	case CLASS_MARTIAL_ARTIST:
		class_power_table = class_power_martial_artist;
		class_power_aux = do_cmd_class_power_aux_martial_artist;
		power_desc = "�Z";
		break;
	case CLASS_MAGIC_EATER:
		class_power_table = class_power_magic_eater;
		class_power_aux = do_cmd_class_power_aux_magic_eater;
		power_desc = "���Z";
		break;
	case CLASS_SUWAKO:
		class_power_table = class_power_suwako;
		class_power_aux = do_cmd_class_power_aux_suwako;
		power_desc = "���Z";
		break;
	case CLASS_RAN:
		class_power_table = class_power_ran;
		class_power_aux = do_cmd_class_power_aux_ran;
		power_desc = "���Z";
		break;
	case CLASS_TENSHI:
		class_power_table = class_power_tenshi;
		class_power_aux = do_cmd_class_power_aux_tenshi;
		power_desc = "���Z";
		break;
	case CLASS_MEIRIN:
		class_power_table = class_power_meirin;
		class_power_aux = do_cmd_class_power_aux_meirin;
		power_desc = "���Z";
		break;
	case CLASS_PARSEE:
		class_power_table = class_power_parsee;
		class_power_aux = do_cmd_class_power_aux_parsee;
		power_desc = "���Z";
		break;
	case CLASS_EIKI:
		class_power_table = class_power_eiki;
		class_power_aux = do_cmd_class_power_aux_eiki;
		power_desc = "���Z";
		break;
	case CLASS_SHINMYOU_2:
		class_power_table = class_power_shinmyou2;
		class_power_aux = do_cmd_class_power_aux_shinmyou2;
		power_desc = "���Z";
		break;
	case CLASS_SUMIREKO:
		if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			class_power_table = class_power_sumireko_d;
			class_power_aux = do_cmd_class_power_aux_sumireko_d;
			power_desc = "��";
		}
		else
		{
			class_power_table = class_power_sumireko;
			class_power_aux = do_cmd_class_power_aux_sumireko;
			power_desc = "���\��";
		}
		break;
	case CLASS_ICHIRIN:
		class_power_table = class_power_ichirin;
		class_power_aux = do_cmd_class_power_aux_ichirin;
		power_desc = "���Z";
		break;
	case CLASS_MOKOU:
		class_power_table = class_power_mokou;
		class_power_aux = do_cmd_class_power_aux_mokou;
		power_desc = "���Z";
		break;
	case CLASS_KANAKO:
		class_power_table = class_power_kanako;
		class_power_aux = do_cmd_class_power_aux_kanako;
		power_desc = "���Z";
		break;
	case CLASS_FUTO:
		class_power_table = class_power_futo;
		class_power_aux = do_cmd_class_power_aux_futo;
		power_desc = "���Z";
		break;
	case CLASS_SUNNY:
		class_power_table = class_power_sunny;
		class_power_aux = do_cmd_class_power_aux_sunny;
		power_desc = "���Z";
		break;
	case CLASS_LUNAR:
		class_power_table = class_power_lunar;
		class_power_aux = do_cmd_class_power_aux_lunar;
		power_desc = "���Z";
		break;
	case CLASS_STAR:
		class_power_table = class_power_star;
		class_power_aux = do_cmd_class_power_aux_star;
		power_desc = "���Z";
		break;
	case CLASS_ALICE:
		class_power_table = class_power_alice;
		class_power_aux = do_cmd_class_power_aux_alice;
		power_desc = "���Z";
		break;
	case CLASS_LUNASA:
		class_power_table = class_power_lunasa;
		class_power_aux = do_cmd_class_power_aux_lunasa;
		power_desc = "���Z";
		break;
	case CLASS_MERLIN:
		class_power_table = class_power_merlin;
		class_power_aux = do_cmd_class_power_aux_merlin;
		power_desc = "���Z";
		break;
	case CLASS_LYRICA:
		class_power_table = class_power_lyrica;
		class_power_aux = do_cmd_class_power_aux_lyrica;
		power_desc = "���Z";
		break;
	case CLASS_CLOWNPIECE:
		class_power_table = class_power_clownpiece;
		class_power_aux = do_cmd_class_power_aux_clownpiece;
		power_desc = "���Z";
		break;
	case CLASS_DOREMY:
		class_power_table = class_power_doremy;
		class_power_aux = do_cmd_class_power_aux_doremy;
		power_desc = "���Z";
		break;
	case CLASS_HINA:
		class_power_table = class_power_hina;
		class_power_aux = do_cmd_class_power_aux_hina;
		power_desc = "���Z";
		break;
	case CLASS_SAKUYA:
		class_power_table = class_power_sakuya;
		class_power_aux = do_cmd_class_power_aux_sakuya;
		power_desc = "���Z";
		break;
	case CLASS_BENBEN:
		class_power_table = class_power_benben;
		class_power_aux = do_cmd_class_power_aux_benben;
		power_desc = "���t";
		break;
	case CLASS_YATSUHASHI:
		class_power_table = class_power_yatsuhashi;
		class_power_aux = do_cmd_class_power_aux_yatsuhashi;
		power_desc = "���t";
		break;
	case CLASS_3_FAIRIES:

		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			class_power_table = class_power_sangetsu_2;
			class_power_aux = do_cmd_class_power_aux_sangetsu_2;
		}
		else
		{
			class_power_table = class_power_sangetsu;
			class_power_aux = do_cmd_class_power_aux_sangetsu;
		}
		power_desc = "���Z";
		break;
	case CLASS_RAIKO:
		class_power_table = class_power_raiko;
		class_power_aux = do_cmd_class_power_aux_raiko;
		power_desc = "���Z";
		break;
	case CLASS_CHEMIST:
		class_power_table = class_power_chemist;
		class_power_aux = do_cmd_class_power_aux_chemist;
		power_desc = "���Z";
		break;
	case CLASS_MAMIZOU:
		class_power_table = class_power_mamizou;
		class_power_aux = do_cmd_class_power_aux_mamizou;
		power_desc = "���Z";
		break;
	case CLASS_YUKARI:
		class_power_table = class_power_yukari;
		class_power_aux = do_cmd_class_power_aux_yukari;
		power_desc = "���Z";
		break;

	case CLASS_RINGO:
		class_power_table = class_power_ringo;
		class_power_aux = do_cmd_class_power_aux_ringo;
		power_desc = "���Z";
		break;

	case CLASS_SEIRAN:
		class_power_table = class_power_seiran;
		class_power_aux = do_cmd_class_power_aux_seiran;
		power_desc = "���Z";
		break;
	case CLASS_EIRIN:
		class_power_table = class_power_eirin;
		class_power_aux = do_cmd_class_power_aux_eirin;
		power_desc = "���Z";
		break;
	case CLASS_KAGUYA:
		class_power_table = class_power_kaguya;
		class_power_aux = do_cmd_class_power_aux_kaguya;
		power_desc = "���Z";
		break;
	case CLASS_SAGUME:
		class_power_table = class_power_sagume;
		class_power_aux = do_cmd_class_power_aux_sagume;
		power_desc = "���Z";
		break;

	case CLASS_REISEN2:
		class_power_table = class_power_reisen2;
		class_power_aux = do_cmd_class_power_aux_reisen2;
		power_desc = "���Z";
		break;
	case CLASS_TOYOHIME:
		class_power_table = class_power_toyohime;
		class_power_aux = do_cmd_class_power_aux_toyohime;
		power_desc = "���Z";
		break;
	case CLASS_YORIHIME:
		class_power_table = class_power_yorihime;
		class_power_aux = do_cmd_class_power_aux_yorihime;
		power_desc = "�_�~�낵";
		break;
	case CLASS_HECATIA:
		class_power_table = class_power_hecatia;
		class_power_aux = do_cmd_class_power_aux_hecatia;
		power_desc = "���Z";
		break;
	case CLASS_JUNKO:
		class_power_table = class_power_junko;
		class_power_aux = do_cmd_class_power_aux_junko;
		power_desc = "���Z";
		break;
	case CLASS_SOLDIER:
		class_power_table = class_power_soldier;
		class_power_aux = do_cmd_class_power_aux_soldier;
		power_desc = "�Z�\";
		break;

	case CLASS_NEMUNO:
		class_power_table = class_power_nemuno;
		class_power_aux = do_cmd_class_power_aux_nemuno;
		power_desc = "���Z";
		break;

	case CLASS_AUNN:
		class_power_table = class_power_aunn;
		class_power_aux = do_cmd_class_power_aux_aunn;
		power_desc = "���Z";
		break;
	case CLASS_NARUMI:
		if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
		{
			class_power_table = class_power_narumi2;
			class_power_aux = do_cmd_class_power_aux_narumi2;
		}
		else
		{
			class_power_table = class_power_narumi;
			class_power_aux = do_cmd_class_power_aux_narumi;
		}	
		power_desc = "���Z";
		break;
	case CLASS_KOSUZU:

		if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI)
		{
			class_power_table = class_power_kosuzu_hyakki;
			class_power_aux = do_cmd_class_power_aux_kosuzu_hyakki;
		}
		else
		{
			class_power_table = class_power_kosuzu;
			class_power_aux = do_cmd_class_power_aux_kosuzu;
		}
		power_desc = "���Z";
		break;

	case CLASS_LARVA:
		class_power_table = class_power_larva;
		class_power_aux = do_cmd_class_power_aux_larva;
		power_desc = "���Z";
		break;
	case CLASS_MAI:
		class_power_table = class_power_mai;
		class_power_aux = do_cmd_class_power_aux_mai;
		power_desc = "���Z";
		break;
	case CLASS_SATONO:
		class_power_table = class_power_satono;
		class_power_aux = do_cmd_class_power_aux_mai;
		power_desc = "���Z";
		break;
	case CLASS_VFS_CLOWNPIECE:
		class_power_table = class_power_vfs_clownpiece;
		class_power_aux = do_cmd_class_power_aux_vfs_clownpiece;
		power_desc = "���Z";
		break;
	case CLASS_JYOON:
		if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON))//v1.1.92 ���ꐫ�i���Z�ǉ�
		{
			class_power_table = class_power_jyoon_2;
			class_power_aux = do_cmd_class_power_aux_jyoon_2;
		}
		else
		{
			class_power_table = class_power_jyoon;
			class_power_aux = do_cmd_class_power_aux_jyoon;
		}
		power_desc = "���Z";
		break;
	case CLASS_SHION:
		class_power_table = class_power_shion;
		class_power_aux = do_cmd_class_power_aux_shion;
		power_desc = "���Z";
		break;
	case CLASS_OKINA:
		class_power_table = class_power_okina;
		class_power_aux = do_cmd_class_power_aux_okina;
		power_desc = "���Z";
		break;
	case CLASS_EIKA:
		class_power_table = class_power_eika;
		class_power_aux = do_cmd_class_power_aux_eika;
		power_desc = "���Z";
		break;
	case CLASS_MAYUMI:
		class_power_table = class_power_mayumi;
		class_power_aux = do_cmd_class_power_aux_mayumi;
		power_desc = "���Z";
		break;
	case CLASS_KUTAKA:
		class_power_table = class_power_kutaka;
		class_power_aux = do_cmd_class_power_aux_kutaka;
		power_desc = "���Z";
		break;

	case CLASS_URUMI:
		class_power_table = class_power_urumi;
		class_power_aux = do_cmd_class_power_aux_urumi;
		power_desc = "���Z";
		break;

	case CLASS_SAKI:
		class_power_table = class_power_saki;
		class_power_aux = do_cmd_class_power_aux_saki;
		power_desc = "���Z";
		break;

	case CLASS_YACHIE:
		class_power_table = class_power_yachie;
		class_power_aux = do_cmd_class_power_aux_yachie;
		power_desc = "���Z";
		break;

	case CLASS_KEIKI:
		class_power_table = class_power_keiki;
		class_power_aux = do_cmd_class_power_aux_keiki;
		power_desc = "���Z";
		break;

	case CLASS_RESEARCHER:
		class_power_table = class_power_researcher;
		class_power_aux = do_cmd_class_power_aux_researcher;
		power_desc = "���Z";
		break;

	case CLASS_CARD_DEALER:
		class_power_table = class_power_card_dealer;
		class_power_aux = do_cmd_class_power_aux_card_dealer;
		power_desc = "���Z";
		break;

	case CLASS_TAKANE:
		class_power_table = class_power_takane;
		class_power_aux = do_cmd_class_power_aux_takane;
		power_desc = "���Z";
		break;

	case CLASS_MOMOYO:
		class_power_table = class_power_momoyo;
		class_power_aux = do_cmd_class_power_aux_momoyo;
		power_desc = "���Z";
		break;

	case CLASS_SANNYO:
		class_power_table = class_power_sannyo;
		class_power_aux = do_cmd_class_power_aux_sannyo;
		power_desc = "���Z";
		break;

	case CLASS_MIKE:
		class_power_table = class_power_mike;
		class_power_aux = do_cmd_class_power_aux_mike;
		power_desc = "���Z";
		break;

	case CLASS_TSUKASA:
		class_power_table = class_power_tsukasa;
		class_power_aux = do_cmd_class_power_aux_tsukasa;
		power_desc = "���Z";
		break;

	case CLASS_MEGUMU:
		class_power_table = class_power_megumu;
		class_power_aux = do_cmd_class_power_aux_megumu;
		power_desc = "���Z";
		break;

	case CLASS_MISUMARU:
		class_power_table = class_power_misumaru;
		class_power_aux = do_cmd_class_power_aux_misumaru;
		power_desc = "���Z";
		break;

	case CLASS_YUMA:
		class_power_table = class_power_yuma;
		class_power_aux = do_cmd_class_power_aux_yuma;
		power_desc = "���Z";
		break;

	case CLASS_CHIMATA:
		class_power_table = class_power_chimata;
		class_power_aux = do_cmd_class_power_aux_chimata;
		power_desc = "���Z";
		break;

	case CLASS_MIYOI:
		class_power_table = class_power_miyoi;
		class_power_aux = do_cmd_class_power_aux_miyoi;
		power_desc = "���Z";
		break;

	case CLASS_BITEN:
		class_power_table = class_power_biten;
		class_power_aux = do_cmd_class_power_aux_biten;
		power_desc = "���Z";
		break;


	default:
		msg_print("���Ȃ��͐E�Ƃɂ����Z�������Ă��Ȃ��B");
		return;
	}


	if (p_ptr->confused && !only_browse)
	{
		msg_print("�������Ă��ďW���ł��Ȃ��I");
		return;
	}

	screen_save();

	/*:::�����@�t���O*/
	//if(dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) || (p_ptr->anti_magic)) anti_magic = TRUE; 
	flag = FALSE;
    redraw = FALSE;

      num = (-1);
#ifdef ALLOW_REPEAT
	//���s�[�g���ꂽ�ꍇnum���Ăяo���āu���Z�I���ς݁v�t���O��TRUE
	if (repeat_pull(&num))
	{
		///���s�[�g�����Ƃ����̃��x�����O��艺�����ĂċZ���g���Ȃ��Ȃ��Ă���L�����Z�����鏈���炵���B���p�B
		if ((class_power_table + num)->min_lev <=  p_ptr->lev) flag = TRUE;

		// -Hack- �˕P�́u���S���̐_�X�v�͎g���x�ɋ@�\���ς��̂Ń��s�[�g�L�����Z��
	//�܂��Ȃɂ����������H
		if(p_ptr->pclass == CLASS_YORIHIME && num == 0) flag = FALSE;
	}
#endif

/*:::��������A�Z�̈ꗗ��\�����đI�����󂯎��Z�ԍ��𓾂鏈��*/

	for(cnt=0;(class_power_table+cnt)->min_lev <= p_ptr->lev;cnt++);
	if(cnt == 0)
	{
		msg_print("�܂��g������Z���Ȃ��B");
		screen_load();
		return;
	}

	if (only_browse)
		(void) strnfmt(out_val, 78, "(%^s %c-%c, '*'�ňꗗ, ESC) �ǂ�%s�ɂ��Ēm��܂����H", power_desc, I2A(0), I2A(cnt - 1), power_desc);
	else
		(void) strnfmt(out_val, 78, "(%^s %c-%c, '*'�ňꗗ, ESC) �ǂ�%s���g���܂����H",	power_desc, I2A(0), I2A(cnt - 1), power_desc);
	
	choice= (always_show_list ) ? ESCAPE:1;

	cur_pos = 0;

	while (!flag)
	{
		if(choice==ESCAPE) choice = ' '; 
		else if( !get_com(out_val, &choice, TRUE) )break;
		/*:::���j���[�\���p*/

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') )
		{
			/* Show the list */
			if (!redraw)
			{
				char psi_desc[160];

				redraw = TRUE;
				/* Save the screen */
				if (!only_browse ) screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str("���O", y, x + 5);

				//v1.1.89 37��40
				put_str(format("Lv   ����@ �֘A�@���� ����"), y, x + 40);

				/* Dump the spells */
				/*:::�Z�Ȃǂ̈ꗗ��\��*/
				for (i = 0; (class_power_table + i)->min_lev <= p_ptr->lev; i++)
				{
					spell = class_power_table + i;

					cost = spell->cost + (p_ptr->lev - spell->min_lev) * spell->lev_inc_cost / 10;

					//v1.1.85 �ߋ肪���_�̒��ɂ���ƃR�X�g����
					if(p_ptr->pclass == CLASS_IKU && cave_have_flag_bold(py, px, FF_ELEC_PUDDLE)) cost = (cost+1)/2;


					if(cost > 0)
					{
						/*:::����͌��� mod_need_mana�ɒ�`���ꂽMANA_DIV��DEC_MANA_DIV�̒l���g�p*/
						if(!spell->use_hp && spell->is_magic && p_ptr->dec_mana) cost = cost * 3 / 4;
						if(cost < 1) cost = 1;
					}

					chance = spell->fail;

					/*:::���s���v�Z�@���s���ݒ�0�̋Z�̓p�X*/					
					if (chance)
					{
						/*:::���x�����ɂ�鎸�s������*/
						chance -= 3 * (p_ptr->lev - spell->min_lev);
						/*:::�p�����[�^�ɂ�鎸�s������*/
						chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[spell->stat]] - 1);
						
						/*:::�����ɂ�鎸�s���㏸�@�����*/
						if (heavy_armor()) chance += spell->heavy_armor_fail;

						/*:::���i��􂢂ɂ�鐬��������*/
						chance += p_ptr->to_m_chance;
						/*:::���Z�����@�̎��A����͌����Ȃǂ̌��ʂ��󂯂�*/
						if(spell->is_magic) chance = mod_spell_chance_1(chance);

						/* Extract the minimum failure rate */
						minfail = adj_mag_fail[p_ptr->stat_ind[spell->stat]];

						/* Minimum failure rate */
						if (chance < minfail) chance = minfail;

						/* Stunning makes spells harder */
						if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
						else if (p_ptr->stun) chance += 15;

						/* Always a 5 percent chance of working */
						if (chance > 95) chance = 95;
						/*:::�\����hp/mp���Ȃ��Ƃ��@�ǂ݂̂����s�ł��Ȃ����邪*/
						if (spell->use_hp && cost > p_ptr->chp) chance =100;
						if (!spell->use_hp && cost > p_ptr->csp) chance =100;
					}

					/*:::���ʗ��̃R�����g�𓾂�*/
					sprintf(comment, "%s", (*class_power_aux)(i,TRUE));
//msg_format("len(%d):%d",i,strlen((*class_power_aux)(i,TRUE)));


					/*:::�J�[�\���܂��̓A���t�@�x�b�g�̕\��*/
					if (use_menu)
					{
						if(i == cur_pos)
							sprintf(psi_desc, "  �t ");
						else
							sprintf(psi_desc, "     ");

					}
					else
					{
						sprintf(psi_desc, "  %c) ", I2A(i));
					}

					/* Dump the spell --(-- */
					//Hack �H�X�q�ŏI���`�͑S����
//					if(p_ptr->pclass == CLASS_YUYUKO && i==9 || spell->cost < 0)
					if(spell->cost < 0)
					{
						strcat(psi_desc, format("%-35s%2d �S��%s  %s %3d%%  %s",
						      spell->name, spell->min_lev,(spell->use_hp)?"(HP)":"(MP)",
						      stat_desc[spell->stat], chance, comment));
					}
					else
					{
						strcat(psi_desc, format("%-35s%2d %4d%s  %s %3d%%  %s",
						      spell->name, spell->min_lev, cost,(spell->use_hp)?"(HP)":"(MP)",
						      stat_desc[spell->stat], chance, comment));
					}
					//if(anti_magic && spell->is_magic) c_prt(TERM_L_DARK,psi_desc,y+i+1,x);
					if(!check_class_skill_usable(errmsg,i,class_power_table)) 
					{
						if(p_ptr->pclass == CLASS_SOLDIER)
							c_prt(TERM_L_DARK,"---���K��---",y+i+1,x);
						else
							c_prt(TERM_L_DARK,psi_desc,y+i+1,x);
					}
					else prt(psi_desc, y + i + 1, x);
				}

				if(cp_ptr->magicmaster)
				{
					prt("", y + i + 1, x);
					i++;
					prt("(Ctrl+'g'�R�}���h�Łu���E�K�[�h�v���g�p�\�ł��B)", y + i + 1, x);
					i++;
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			/* Hide the list */
			else if (!only_browse)
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if (use_menu)
		{
			char c = choice;
			ask = FALSE;

			if (c == 'x' || c == 'X' || c == '\r' || c == '\n')
			{
				i = cur_pos;
			}
			else if (c == '0')
			{
				break;
			}
			else if (c == '8' || c == 'k' || c == 'K')
			{
				cur_pos--;
				if (cur_pos < 0) cur_pos = cnt - 1;
				choice = ESCAPE;
				if(!only_browse)screen_load();
				redraw = FALSE;
				continue;
			}
			else if (c == '2' || c == 'j' || c == 'J')
			{
				cur_pos++;
				if (cur_pos >= cnt) cur_pos = 0;
				choice = ESCAPE;
				if (!only_browse)screen_load();
				redraw = FALSE;
				continue;
			}
		}
		else
		{

			//�I�����啶���Ȃ�ask(�g���O�Ɋm�F����)�t���O�𗧂Ăď������ɒ����Ă���
			ask = isupper(choice);
			if (ask) choice = tolower(choice);

			//�I�����������A���t�@�x�b�g�Ȃ炻���ԍ��ɒ����Ă���
			i = (islower(choice) ? A2I(choice) : -1);

		}






		/* Totally Illegal */
		if ((i < 0) || (i >= cnt))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = class_power_table + i;

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
			(void) strnfmt(tmp_val, 78, "%s���g���܂����H", spell->name);

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}
		num = i;
		/* Stop the loop */
		flag = TRUE;

#ifdef ALLOW_REPEAT /* TNB */
		if(!only_browse) repeat_push(num);
#endif /* ALLOW_REPEAT -- TNB */

	}

	//���̕�screen_load()��ĕ`��֘A�Ńo�O�c���Ă邩������Ȃ��B

	/* Restore the screen */
	if (redraw && !only_browse) screen_load();
	/* Show choices */
	p_ptr->window |= (PW_SPELL);
	/* Window stuff */
	window_stuff();
	/* Abort if needed */
	if (!flag) 
	{
		screen_load();
		return;
	}
/*:::�Z�̑I���I���B���s������ցB*/


	screen_load();

	//v1.1.22 ���m�Ɍ���A�g�p�s�̓��Z�̐����������Ȃ�
	if(only_browse && p_ptr->pclass == CLASS_SOLDIER && !check_class_skill_usable(errmsg,num,class_power_table))
	{
		msg_format("%s",errmsg);
		return;	
	}

	///pend �Z�̐�������Ƃ��̏�����mind_browse����K���ɃR�s�[�B�@������rumor_new()�݂�����\�Ŏ蓮���s���ĕ\���������B
	if(only_browse)
	{
		int j, line;
		char temp[62*5];
		screen_save();

		for(j=16;j<23;j++)	Term_erase(12, j, 255);

		roff_to_buf(spell->tips, 62, temp, sizeof(temp));
		for(j=0, line = 17;temp[j];j+=(1+strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
		prt("�����L�[�������ĉ������B",0,0);
		(void)inkey();
		screen_load();
		return;
	}

	/*:::�I�����ꂽ�Z�ɂ��ăR�X�g�⎸�s�����Ăьv�Z����@���s�[�g����������̂ōė��p�͂ł��Ȃ�*/
	spell = class_power_table + num;
	chance = spell->fail;
	if (chance)
	{
		chance -= 3 * (p_ptr->lev - spell->min_lev);
		chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[spell->stat]] - 1);
		if (heavy_armor()) chance += spell->heavy_armor_fail;
		chance += p_ptr->to_m_chance;
		if(spell->is_magic) chance = mod_spell_chance_1(chance);
		minfail = adj_mag_fail[p_ptr->stat_ind[spell->stat]];
		if (chance < minfail) chance = minfail;
		if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
		else if (p_ptr->stun) chance += 15;
		if (chance > 95) chance = 95;
	}
	cost = spell->cost + (p_ptr->lev - spell->min_lev) * spell->lev_inc_cost / 10;

	//v1.1.85 �ߋ肪���_�̒��ɂ���ƃR�X�g����
	if (p_ptr->pclass == CLASS_IKU && cave_have_flag_bold(py, px, FF_ELEC_PUDDLE)) cost = (cost + 1) / 2;

	if(cost > 0)
	{
		if(!spell->use_hp && spell->is_magic && p_ptr->dec_mana) cost = cost * 3 / 4;
		if(cost < 1) cost = 1;
	}
	/*:::HP��MP������Ȃ����͋������f�@�ǂ����I�v�V�����g��Ȃ��������߂�ǂ�*/
	if (spell->use_hp && cost > p_ptr->chp)
	{
		msg_print("�g�o������܂���B");
		return;	
	}
	if (!spell->use_hp && ((spell->cost > 0) ? cost : -(cost)) > p_ptr->csp)
	{
		msg_print("�l�o������܂���B");
		return;	
	}
	/*:::�����@����蕐��񏊎��ȂǓ���ȏ������K�v�ȋZ�̎g�p�s���胁�b�Z�[�W*/
	if(!check_class_skill_usable(errmsg,num,class_power_table))
	{
		msg_format("%s",errmsg);
		return;	
	}

	/*:::���Z���s����*/
	if (randint0(100) < chance)
	{
		if (flush_failure) flush();
		sound(SOUND_FAIL);

		/*:::����Ȏ��s�y�i���e�B������ꍇ�����ɏ���*/
		//v1.1.30 �Ă�̃G���V�F���g�f���[�p�[���s
		if(p_ptr->pclass == CLASS_TEWI && num == 5)
		{
			msg_print("���Ȃ��̑�R���΂ꂽ�I");
			for (i = m_max - 1; i >= 1; i--)
			{
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (is_pet(m_ptr)) continue;
				set_hostile(m_ptr);
				(void)set_monster_csleep(i, 0);
				
			}
			aggravate_monsters(0,TRUE);
			msg_print("�t���A�̃����X�^�[�B����ǂ���g�ɂȂ����I");
		}
		else
			msg_format("%s�̎g�p�Ɏ��s�����I",power_desc);

	}

	/*:::���Z��������*/
	else
	{
		/*:::���������Ƀ^�[�Q�b�g�I���ŃL�����Z�������Ƃ��Ȃǂɂ�cptr��NULL���Ԃ�A���̂܂܍s����������I������*/
		if(!(*class_power_aux)(num,FALSE)) return;
	}

	/*:::�^�[����R�X�g����Ȃ�*/

	if(new_class_power_change_energy_need)
	{
		energy_use = new_class_power_change_energy_need;
		if(p_ptr->wizard) msg_format("Energy_use:%d",energy_use);
		new_class_power_change_energy_need = 0;
	}
	else energy_use = 100;

	if (spell->use_hp)
	{
		/*:::�Z�̎g�p�̂Ƃ����炩�̗��R�Ń_���[�W�󂯂�HP������Ȃ��Ȃ������̋~�Ϗ��u*/
		if(p_ptr->chp < cost) cost = p_ptr->chp;
		take_hit(DAMAGE_USELIFE, cost, "�Z�̎g�p�ɂ�镉�S", -1);
		p_ptr->redraw |= (PR_HP);
	}
	else
	{
		if(spell->cost < 0) 
		{
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}
		else
		{
			p_ptr->csp -= cost;
			if (p_ptr->csp < 0) 
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
			}

		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);

	return;

}



//EXTRA�p�T�|�[�g�A�C�e���̃��X�g 
//�u�A�C�e���J�[�h�v��pval�ɂ����̓Y�����i�[�����̂ŏ��ԕύX����Ɖߋ��̃Z�[�u�f�[�^�ƌ݊������Ȃ��Ȃ�B���ӁB
//{rarity,min_lev,max_lev,prod_num,cost,r_idx,*table,*func,func_idx,
//"name(�ꗗ�\���̓s����20�����܂�)","info"}
const support_item_type support_item_list[] =
{
	//���[�����C�g���C
	{40, 10, 50,4,2,	MON_RUMIA,class_power_rumia,do_cmd_class_power_aux_rumia,1,
	"��������","����̓^�[�Q�b�g���ӂɈÍ������̃��[�U�[�𐔖{���B"},
	//�_�[�N�T�C�h�I�u�U���[��
	{100, 50, 128,1,30,	MON_RUMIA,class_power_rumia,do_cmd_class_power_aux_rumia,6,
	"�Âڂ������{��","������g���ƈ�莞�ԗH�̉�����B"},

	//�����a
	{30, 10, 100,5,5,	MON_YOUMU,class_power_youmu,do_cmd_class_power_aux_youmu,0,
	"�����Ȑ؂��","������g���ƃ^�[�Q�b�g�ׂ̗܂ň�u�ňړ������̂܂܍U������B�ߐڕ���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},
	//�����i���a
	{80, 40, 100,4,8,	MON_YOUMU,class_power_youmu,do_cmd_class_power_aux_youmu,6,
	"�������{���^�C","������g���ƃ^�[�Q�b�g�ׂ̗܂ň�u�ňړ����A���킳���܂ŘA���U������B�ߐڕ���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},
	//��O���ʍ�
	{30, 60, 120,3,10,	MON_YOUMU,class_power_youmu,do_cmd_class_power_aux_youmu,9,
	"�召�̓�","����͎����𒆐S�Ƃ������a�܃}�X�ɑ΂��A�n���A�J�I�X�A��Ԙc�ȁA�n���̋Ɖ΁A�������A�j�ׂ̂����ꂩ�̑����̋��͂ȍU������B"},

	//���Ƃ���
	{30, 0, 50,8,3,	MON_TEWI,class_power_tewi,do_cmd_class_power_aux_tewi,0,
	"�d���e�̃X�R�b�v","���Ƃ������@��B���V���Ă��Ȃ������X�^�[�����Ƃ����ɗ�����ƞN�O�A�C��A�s���x�����邱�Ƃ�����B�N�������Ă��鏊�ł͎g�p�ł��Ȃ��B"},
	//�t���X�^�[�G�X�P�[�v
	{30, 20, 100,5,4,	MON_TEWI,class_power_tewi,do_cmd_class_power_aux_tewi,2,
	"�l�Q�̃y���_���g","�ʏ�U�����s���A���̌��u�ŗ��E����B���E�Ɏ��s���邱�Ƃ�����B"},
	//�匊���x�l�̖�
	{30, 30,  90,5,5,	MON_TEWI,class_power_tewi,do_cmd_class_power_aux_tewi,3,
	"���̕�","�����HP�𒆒��x�񕜂����Đ؂菝�������B"},

	//�g������
	{20, 0,  90,5,4,	MON_UDONGE,class_power_udonge,do_cmd_class_power_aux_udonge,1,
	"�e�̎�����","����͎��͂ɂ�����̂����m����B���x�����オ��Ɗ��m�͈̔͂Ǝ�ނ�������B"},
	//�}�C���h�X�g�b�p�[
	{30, 10,  50,4,5,	MON_UDONGE,class_power_udonge,do_cmd_class_power_aux_udonge,4,
	"�l�Q�̃l�N�^�C�s��","����͎��E���̃����X�^�[�̓������~�߂�B"},
	//���m���o�̖�
	{35, 20,  110,7,3,	MON_UDONGE,class_power_udonge,do_cmd_class_power_aux_udonge,7,
	"�i�Ԉ�̖�r","������g���ƎO�i�K�p���[�A�b�v����B���݂�����ƁE�E"},
	//�A�L�����[�X�y�N�g��
	{60, 60,  120,2,16,	MON_UDONGE,class_power_udonge,do_cmd_class_power_aux_udonge,11,
	"�򉮂̕҂݊}","������g���ƒZ���ԕ��g���G����̍U�����󂯂ɂ����Ȃ�B"},

	//���_�̓{��
	{25, 0, 40,6,2,	MON_IKU,class_power_iku,do_cmd_class_power_aux_iku,0,
	"�ѓd���{��","����̓^�[�Q�b�g�̈ʒu�ɓd�������̃{�[���𔭐�������B"},
	//���_�̈ꌂ
	{40, 20, 70,4,3,	MON_IKU,class_power_iku,do_cmd_class_power_aux_iku,2,
	"�ΉԂ��U�炷�l�N�^�C","����͈ꎞ�I�ɕ���ɓd��������t�^����B�ߐڕ���𑕔����Ă��Ȃ��Ǝg���Ȃ��B"},
	//�H�߂͋�̔@��
	{45, 40, 110,4,6,	MON_IKU,class_power_iku,do_cmd_class_power_aux_iku,3,
	"�V���̉H��","������g���ƈꎞ�I�Ɍ��f�ϐ���AC�㏸�𓾂�B"},

	//�̗͂���̂悤��
	{50, 5, 70,5,6,	MON_KOMACHI,class_power_komachi,do_cmd_class_power_aux_komachi,2,
	"�^�C�^�j�b�N�Ɩ��t����ꂽ�{���D","������g���ƃ^�[�Q�b�g�Ɍ����ĒZ������ːi���A�������_���[�W��^����B"},
	//�E���̋V
	{40, 40, 100,4,7,	MON_KOMACHI,class_power_komachi,do_cmd_class_power_aux_komachi,5,
	"���_�̂ۂ����艺��","����͎w�肵���G�ƈʒu����������B���E�O�̓G�ɂ��L���B"},
	//���}���̌���
	{40, 30, 90,6,5,	MON_KOMACHI,class_power_komachi,do_cmd_class_power_aux_komachi,6,
	"�Z���K","������w�肵���G�������ׂ̗Ɉ����񂹁A���̂܂܍U������B"},
	//�]���􋖂�����
	{50, 30, 80,7,4,	MON_KOMACHI,class_power_komachi,do_cmd_class_power_aux_komachi,7,
	"�g�ł劙","����͗אڂ����G���m���ňꌂ�œ|���A���m����HP�𔼕��ɂ���B���҈ȊO�ɂ͖����B"},

	//�����ݒu
	{15, 0, 20,7,1,	MON_KASEN,class_power_kasen,do_cmd_class_power_aux_kasen,0,
	"������","����͌��݂��镔���𖾂邭����B"},
	//��������
	{25, 5, 40,4,2,	MON_KASEN,class_power_kasen,do_cmd_class_power_aux_kasen,4,
	"��l�̃V�j�����L���b�v","����͎��͂̓�����������z���ɂ���B"},
	//�h���S���Y�O���E��
	{35, 40, 100,5,6,	MON_KASEN,class_power_kasen,do_cmd_class_power_aux_kasen,7,
	"��т̗���","����͌���HP��1/3�̈З͂̍��������u���X����B"},

	//�Ή��u���X
	{25, 10, 70,5,4,	MON_SUIKA,class_power_suika,do_cmd_class_power_aux_suika,1,
	"�S�̕Z�\","����͌���HP��1/3�̈З͂̉Ή������u���X����B"},
	//�ˉB�R����
	{20, 15, 50,7,3,	MON_SUIKA,class_power_suika,do_cmd_class_power_aux_suika,3,
	"���","����͖������{�[������B�З͂͘r�͂Ɉˑ�����B"},
	//�~�b�V���O�p�[�v���p���[
	{60, 50, 120,1,30,	MON_SUIKA,class_power_suika,do_cmd_class_power_aux_suika,8,
	"�S�̕���","������g���ƈ�莞�ԋ��剻����B���剻���͐g�̔\�͂������I�ɏ㏸���邪�����Ɩ�����g�p�ł��Ȃ��BU�R�}���h�ŋ��剻�������ł���B"},

	//�}�W�b�N�~�T�C��
	{15, 0, 30,10,2,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,2,
	"���@�̃L�m�R","����͒�З͂Ȗ������{���g����B"},
	//�C�����[�W�������[�U�[
	{20, 20, 60,7,3,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,5,
	"�L�m�R�y�[�X�g","����͑M�������̃r�[������B"},
	//�}�W�b�N�i�p�[��
	{25, 40, 90,5,5,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,8,
	"�L�m�R�G�L�X","����͖��͑����̃��P�b�g����B"},
	//�}�X�p
	{35, 50, 110,3,8,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,9,
	"���T�F","����͋��͂Ȋj�M�����̃r�[������B"},
	//�t�@�C�i���}�X�p
	{100, 80, 128,1,40,	MON_MARISA,class_power_extra,do_cmd_class_power_aux_extra,0,
	"���F�����̔��T�F","����͋ɂ߂ċ��͂ȕ��𑮐��̃r�[������B"},
	//idx30 �����ւ����@
	{75, 25, 128,1,40,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,10,
	"���@�̏��r","������g���Ƃ��̏�ŒT�����_�ɃA�N�Z�X���A�A�C�e������ւ��⑕���̕ύX���ł���B��x�ł����_����o����I������̂Œ��ӁB"},

	//�s�b�`���[�Ԃ�
	{40, 15, 75,3,4,	MON_KOGASA,class_power_kogasa,do_cmd_class_power_aux_kogasa,3,
	"�֎q�F�̉J�P","����͈�莞�Ԕ��˔\�͂������炷�B"},
	//�b��
	{50, 20, 100,1,10,	MON_KOGASA,class_power_kogasa,do_cmd_class_power_aux_kogasa,5,
	"��ڏ��m�̋���","����͋������̕���̏C���l����������B"},

	//�X�P�[���E�F�C�u
	{30, 0, 50,5,1,	MON_WAKASAGI,class_power_wakasagi,do_cmd_class_power_aux_wakasagi,1,
	"�g�͗l�̐�","����͎����𒆐S�ɐ������̃{�[���𔭐�������B����Ŏg�p����ƈЗ͂��オ��B"},
	//�t�؂̑�r�g
	{55, 40, 100,3,12,	MON_WAKASAGI,class_power_wakasagi,do_cmd_class_power_aux_wakasagi,5,
	"�����ꂽ��","����͎��E���ɋ��͂Ȑ������U�����s�����͂̒n�`�𐅂ɂ���B�̗͂������Ă���ƈЗ͂��オ��B"},

	//�L���b�`�A���h���[�Y
	{35, 0, 50,4,3,	MON_KOISHI,class_power_koishi,do_cmd_class_power_aux_koishi,0,
	"�K�N�̂ڂ�","����͋߂��̓G�ɍU����������΂��B����ȓG�ɂ͌��ʂ������B"},
	//�R���f�B�V�����h�e���|�[�g
	{50, 20, 80,4,4,	MON_KOISHI,class_power_koishi,do_cmd_class_power_aux_koishi,3,
	"��b��","������g���ƈ�ԋ߂��G�ׂ̗Ƀe���|�[�g���A���̂܂܈ꌂ��������B���_�������Ȃ��G�͑ΏۂɂȂ�Ȃ��B"},
	//�َ��̖�
	{60, 40, 115,3,10,	MON_KOISHI,class_power_koishi,do_cmd_class_power_aux_koishi,6,
	"�o�̕�����","����͗אڂ����G��̂ɋ��͂Ȑ��_�U���Ǝ��ԋt�]�U�����d�|����B�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ������B"},

	//���C�r�[�Y�o�C�g
	{35, 10, 50,5,2,	MON_MOMIZI,class_power_momizi,do_cmd_class_power_aux_momizi,2,
	"���T�̉�","����͗אڂ��������X�^�[��̂Ƀ_���[�W��^����B�őϐ��̂Ȃ��G�͍����A�N�O���邱�Ƃ�����B"},
	//�痢��
	{50, 40, 80,1,10,	MON_MOMIZI,class_power_momizi,do_cmd_class_power_aux_momizi,3,
	"�叫���̋�","����̓t���A�S�Ă̒n�`�ƃA�C�e�������m����B"},
	//�G�N�X�y���[�Y�J�i��
	{70, 30, 100,4,10,	MON_MOMIZI,class_power_momizi,do_cmd_class_power_aux_momizi,4,
	"���T�V��̑品","����͋߂��̓G�����݂̃t���A����Ǖ����悤�Ǝ��݂�B"},

	//�ǖx��
	{25, 0, 60,12,1,	MON_SEIGA,class_power_seiga,do_cmd_class_power_aux_seiga,0,
	"�א���w","����͗אڂ����ǂ�h�A�Ɍ����J����"},
	//�g�������F��
	{70, 15, 50,2,5,	MON_SEIGA,class_power_seiga,do_cmd_class_power_aux_seiga,3,
	"�L�����V�[�̎D","����͂��̃t���A����ŃL�����V�[�u�{�Á@�F���v���Ăяo���B"},
	//�E�H�[�������i�[
	{85, 40, 100,4,10,	MON_SEIGA,class_power_seiga,do_cmd_class_power_aux_seiga,5,
	"�א�̉H��","����͍L�͈͂̈꒼����̏���ǂɂ���B"},

	//�t���[�Y���Ⓚ�@
	{30, 0, 40,7,2,	MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,1,
	"�������^","����͗אڂ����G��̂ɋɊ������U�����s���B"},
	//�\�[�h�t���[�U�[
	{30, 20, 70,4,2,MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,5,
	"���","����͑������̕���ɑ΂��ꎞ�I�ɗ�C������t�^����B"},
	//�u�ԗⓀ�r�[��
	{50, 30, 100,4,7,MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,8,
	"�p�g����","����͋Ɋ��������[�U�[���L�͈͂ɕ��B�З͂�HP��1/3�ɂȂ�B"},
	//�A�C�V�N���}�V���K��
	{45, 25, 85,8,4,	MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,9,
	"�����X(�݂���)","����͗�C�����̃{���g���^�[�Q�b�g���ӂɘA�˂���B"},
	//�t���X�g�L���O
	{80, 40, 100,1,15,	MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,11,
	"�Ⴞ���","����͂��̃t���A����Łu�X���v����������B�X���͔z�������ŁA�ړ������ɋ��͂ȗ�C�U�����s���B"},

	//������Ƃ��ăh�J�[��
	{55, 40, 120,4,10,	MON_FLAN,class_power_flan,do_cmd_class_power_aux_flan,4,
	"����覐΂̌���","����̓^�[�Q�b�g�Ƃ��̎���3+1d3�O���b�h��*�j��*����B�^�[�Q�b�g�͖����I�Ɏw�肵�Ȃ���΂Ȃ�Ȃ��B(25-���x��/2)%�̊m���őz��O�̑�j�󂪋N����B�n���N�G�X�g�_���W�����ł͎g���Ȃ��B"},
	//�t�H�[�I�u�A�J�C���h
	{80, 20, 80,1,10,	MON_FLAN,class_power_flan,do_cmd_class_power_aux_flan,6,
	"���F�̌���","������g���Ƃ��̃t���A����Łu�t�����h�[���E�X�J�[���b�g�v�̕��g���O�̏o������B���g�͔z�������������߂Ɋւ�炸�v���C���[��͈͍U���Ɋ������ށB"},
	//���[���@�e�C��
	{90, 60, 128,4,18,	MON_FLAN,class_power_flan,do_cmd_class_power_aux_flan,8,
	"�P���ꂽ����","����͎����𒆐S�ɋ��͂Ȓn���̍��Α����̃{�[���𔭐�������B"},

	//�]���r�t�F�A���[
	{70,15, 45,3,3,	MON_ORIN,class_power_orin,do_cmd_class_power_aux_orin,5,
	"�V�g�̗�(�j���t��)","����̓]���r�t�F�A���[����������B"},
	//�H�l����
	{60, 10, 70,8,4,	MON_ORIN,class_power_orin,do_cmd_class_power_aux_orin,6,
	"�ו����ڂ̔L��","����͑�ʂ̉�����������A�߂��̃����_���ȓG�ɜ߂������B"},

	//�傫���Ȃ���
	{30, 15, 70,4,6,	MON_SHINMYOUMARU,class_power_shinmyoumaru,do_cmd_class_power_aux_shinmyoumaru,1,
	"�ŏo�̏���","������g����30�^�[���̊Ԙr�͂Ƒϋv�͂�5��������B�ʏ�̌��E�l�𒴂��Ȃ��B"},
	//�����Ƒ傫���Ȃ���
	{80, 50, 128,2,20,	MON_SHINMYOUMARU,class_power_shinmyoumaru,do_cmd_class_power_aux_shinmyoumaru,9,
	"�ŏo�̏��Ƃf","�����30�^�[���̊Ԙr�͂Ƒϋv�͂�10�����������炷�B�ʏ�̌��E�l�𒴂���B"},

	//�l����
	{50, 5, 40,6,3,	MON_NAZRIN,class_power_nazrin,do_cmd_class_power_aux_nazrin,4,
	"�q�l����","����͐��̗̂d���l��z���Ƃ��ČĂяo���B"},
	//�S���m
	{30, 20, 75,5,3,	MON_NAZRIN,class_power_nazrin,do_cmd_class_power_aux_nazrin,10,
	"�_�E�W���O���b�h","����͎��͂̃A�C�e���E㩁E�����X�^�[�����m����B"},

	//�t�����[�E�B�U���E�F�C
	{40, 20, 50,5,4,	MON_LETTY,class_power_letty,do_cmd_class_power_aux_letty,2,
	"�ނꂽ��","����͎��E���ɗ�C�U������B"},
	//�A���f�����C�V�������C
	{35, 10, 70,5,3,MON_LETTY,class_power_letty,do_cmd_class_power_aux_letty,3,
	"�Ꮧ�̃u���[�`","����͐����ϐ��𖳎����ēG�����m���Ŗ��点��r�[������B���j�[�N�����X�^�[�ɂ͌����Ȃ��B"},

	//���ȂȂ��E�l�S	
	{60, 20, 100,5,8,MON_YOSHIKA,class_power_yoshika,do_cmd_class_power_aux_yoshika,2,
	"���ɉ삦���Œ�","������g���Ǝ��͂̃����X�^�[�S�ĂɗאڍU�����A����ɐ��҂���̗͂��z������B����g�p���ɂ͎g���Ȃ��B"},
	//�h���[�X�s�A
	{20, 0, 30,10,1,	MON_PATCHOULI,class_power_patchouli,do_cmd_class_power_aux_patchouli,0,
	"���яL�����@��","����͋߂��̃^�[�Q�b�g��̂ɕ�����̖������U�����s���B"},
	//�Z���g�G�����s���[
	{40, 25, 85,6,4,	MON_PATCHOULI,class_power_patchouli,do_cmd_class_power_aux_patchouli,3,
	"�ł��L�����@��","����͎w�肵���ʒu�Ƀv���Y�}�����̋���ȃ{�[���𔭐�������B"},
	//�T�C�����g�Z���i
	{40, 40, 100,4,10,	MON_PATCHOULI,class_power_patchouli,do_cmd_class_power_aux_patchouli,7,
	"�O�����̖X�q����","����͗אڂ����G�ɋ��͂ȈÍ������U������B"},
	//���҂̐�
	{150, 70, 128,5,20,	MON_PATCHOULI,class_power_patchouli,do_cmd_class_power_aux_patchouli,10,
	"���҂̐�","����͂��̃t���A����Łu���҂̐΁v����������B���҂̐΂͔z�������ŋ��͂Ȍ��f���@�ōU������B"},

	//�V�瓹�̊J��
	{50, 20, 60,3,7,	MON_AYA,class_power_aya,do_cmd_class_power_aux_aya,2,
	"�V��̗t�c��","����͗��������̍L�͈̓r�[���U������B"},
	//���c�F�̐擱
	{65, 30, 90,4,9,	MON_AYA,class_power_aya,do_cmd_class_power_aux_aya,3,
	"�u�����̎蒟","������g���ƈ꒼����ɗ��������̍U�����s���G�𐁂���΂��ړ����A�ǂ�������΂��Ȃ��G�ɓ�����Ǝ~�܂�B"},
	//���z���r
	{120, 75, 128,1,30,	MON_AYA,class_power_aya,do_cmd_class_power_aux_aya,5,
	"�V��̍�����","����͒Z���Ԃ̊ԍō��̉����������炷�B"},

	//�}���`�v���P�C�e�B�u�w�b�h
	{60, 20, 80,5,7,	MON_SEKIBANKI,class_power_banki,do_cmd_class_power_aux_banki,4,
	"�낭���̃}���g","������g���ƐԔ؊�̓��̕��g���o������B���̋����̓v���C���[�̃��x���Ɉˑ�����B"},

	//���g���o�^���I��
	{40, 0, 60,5,4,	MON_MYSTIA,class_power_mystia,do_cmd_class_power_aux_mystia,4,
	"�鐝�̃s�A�X","����͒��n�̃����X�^�[��z���Ƃ��Đ��̌Ăъ񂹂�B"},

	//�n���O���[�^�C�K�[
	{30, 15, 60,4,5,	MON_SHOU,class_power_shou,do_cmd_class_power_aux_shou,1,
	"�ՎȂ̃x�X�g","������g���Ɛ��O���b�h���꒼���ɓːi���A�����ȓG�͐�����΂��B"},
	//���C�f�B�A���g�g���W���[�K��
	{70, 30, 110,3,12,	MON_SHOU,class_power_shou,do_cmd_class_power_aux_shou,6,
	"������V�̕�","����͋��͂Ȗ������̃��[�U�[���L�͈͂ɕ��B���[�U�[�����������ǂ��΂ɕς���B"},

	//�Ńu���X
	{35, 10, 70,4,6,	MON_MEDICINE,class_power_medi,do_cmd_class_power_aux_medi,2,
	"�闖�̉�","����͌���HP��1/3�̈З͂̓ő����u���X����B"},
	//�K�V���O�K�[�f��
	{50, 30, 100,3,9,	MON_MEDICINE,class_power_medi,do_cmd_class_power_aux_medi,5,
	"�őf�̉�","����͎��E���̑S�Ăɑ΂��ő����U������B"},

	//�S�[�X�g�X�|�b�g
	{30, 10, 50,3,4,	MON_YUYUKO,class_power_yuyuko,do_cmd_class_power_aux_yuyuko,2,
	"�S��̒�","����͎w�肵���^�[�Q�b�g�̎��ӂɑ�ʂ̗H��n�����X�^�[���Ăяo���B"},
	//�M���X�g���h���[��
	{40, 20, 70,4,7,	MON_YUYUKO,class_power_yuyuko,do_cmd_class_power_aux_yuyuko,3,
	"�S��̊z�G�X�q","����͎w�肵���^�[�Q�b�g�ɋ��͂Ȑ��_�U�����d�|����B"},
	//�P����̎���
	{25, 30, 80,4,5,	MON_YUYUKO,class_power_yuyuko,do_cmd_class_power_aux_yuyuko,5,
	"�S��̑��","����͒n�������̋��͂ȃr�[������B"},
	//����̉i��
	{60, 30, 120,3,12,	MON_YUYUKO,class_power_yuyuko,do_cmd_class_power_aux_yuyuko,7,
	"���s�d�̉ԕ�","����͎��͂̓G�����m���Ńt���A�����������B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},

	//�ڗ���
	{30, 0, 100,8,4,	0,class_power_sh_dealer,do_cmd_class_power_aux_sh_dealer,3,
	"�Ó���̊ዾ","����̓A�C�e������Ӓ肷��B���x��30�ȍ~�͊��S��*�Ӓ�*���s����B"},
	//�e���u���X�[���j�[��
	{55, 20, 80,5,8,MON_SATORI,class_power_satori,do_cmd_class_power_aux_satori,2,
	"��O�̖�","����͎��E���̓G�S�Ăɋ��͂Ȑ��_�U�����s���B�A���f�b�h�ɂ͓��Ɍ��ʂ������B"},

	//�}�E���e���G�R�[
	{50, 10, 100,5,4,MON_KYOUKO,class_power_kyouko,do_cmd_class_power_aux_kyouko,2,
	"�R�F�̃`���[�J�[","������g���ƍŌ�ɕ������������I�E���Ԃ����邱�Ƃ��ł���B"},
	//�����O�����W�G�R�[
	{35, 20, 60,4,2,MON_KYOUKO,class_power_kyouko,do_cmd_class_power_aux_kyouko,3,
	"�R�F�̎��","����̓^�[�Q�b�g�̈ʒu�ɍ��������̃{�[���𔭐�������B"},

	//�K�S�E�W�T�C�N����
	{30, 10, 70,5,4,MON_TOZIKO,class_power_toziko,do_cmd_class_power_aux_toziko,1,
	"����̉G�X�q","����͎����̂���ʒu�ɋ��͂ȓd�������̃{�[���𔭐�������B"},
	//�����̗�
	{70, 60, 120,3,10,MON_TOZIKO,class_power_toziko,do_cmd_class_power_aux_toziko,4,
	"�������f�Ă��̚�","����̓^�[�Q�b�g�̈ʒu�ɋ��͂ȓd�������̃{�[���𔭐�������BHP���Ⴂ�قǈЗ͂��オ��B"},

	//�t�̖K��
	{40, 20, 100,7,5,MON_LILY_WHITE,class_power_lilywhite,do_cmd_class_power_aux_lilywhite,2,
	"�t�����̖X�q","����͎��͂̒n�ʂ��Ԕ���X�ɂ���B"},
	//���ň�̒�
	{50, 10, 80,4,8,MON_KISUME,class_power_kisume,do_cmd_class_power_aux_kisume,3,
	"�ޕr���Ƃ��̉�","������g���ƃ^�[�Q�b�g�߂��Ɉړ����A���ӂɕ����_���[�W�Ɛ��_���[�W��^���A����Ɏ��ӂ𐅒n�`�ɂ��A�ꎞ�I�ȉΉ��ϐ��𓾂�B"},
	//�O�ʇT
	{120, 10, 90,16,4,MON_HATATE,class_power_hatate,do_cmd_class_power_aux_hatate,0,
	"�M�^�X�g���b�v�t���J����","����͎��͂̒n�`�E�A�C�e���E�����X�^�[�����m����B�g���b�v�͊��m�ł��Ȃ��B"},

	//�\�����̃��[�U�[
	{60, 20, 80,6,5,MON_MIKO,class_power_miko,do_cmd_class_power_aux_miko,3,
	"�V�q���","����͔j�ב����̃r�[������B"},
	//�k�n�̃}���g
	{40, 35, 128,4,6,MON_MIKO,class_power_miko,do_cmd_class_power_aux_miko,4,
	"��l�̃}���g","������g���Ǝ��E���̔C�ӂ̏ꏊ�փe���|�[�g����B"},
	//�ق������Ă͕K���T��
	{120, 80, 128,2,30,MON_MIKO,class_power_miko,do_cmd_class_power_aux_miko,8,
	"���l�̕�","����͋ɂ߂ċ��͂Ȗ��\�����̃r�[������B"},

	//����̉Βj��
	{35, 10, 40,3,4,MON_KOKORO,class_power_kokoro,do_cmd_class_power_aux_kokoro,6,
	"�Ђ���Ƃ��̖�","����͎��͂̃����_���Ȉʒu�ɉΉ������̃{�[���𐔉񔭐�������B"},
	//�{�����T�̖�
	{40, 30, 70,4,5,MON_KOKORO,class_power_kokoro,do_cmd_class_power_aux_kokoro,7,
	"�T�̖�","����͎g���ƒZ������ːi���A�G�ɓ�����ƋC�����̃_���[�W��^����B"},
	//�V�g�̐_�y���q
	{50, 40, 100,4,7,MON_KOKORO,class_power_kokoro,do_cmd_class_power_aux_kokoro,9,
	"���q���̑���","����͋��͂ȃv���Y�}�����̃r�[������B"},

	//���̒m�点
	{25, 0, 50,5,1,MON_WRIGGLE,class_power_wriggle,do_cmd_class_power_aux_wriggle,0,
	"�u","����͎��͂̃����X�^�[�����m����B"},
	//�c�c�K���V
	{70, 30, 60,2,7,MON_WRIGGLE,class_power_wriggle,do_cmd_class_power_aux_wriggle,5,
	"����","����́u�����̌Q��v����������B"},

	//���z���̊J��
	{70, 40, 120,8,8,MON_YUUKA,class_power_yuuka,do_cmd_class_power_aux_yuuka,0,
	"������","����͎��͂̒n�ʂ��Ԕ���X�ɂ���B"},

	//�P����
	{35, 5, 40,4,3,MON_CHEN,class_power_chen,do_cmd_class_power_aux_chen,0,
	"�d���ݗ�","����͎g���ƍ������ʂ̂���{�[������B"},
	//������ʓV
	{60, 20, 70,5,4,MON_CHEN,class_power_chen,do_cmd_class_power_aux_chen,4,
	"�܂�����","����͎g���ƈꎞ�I�ɕ��V�Ɖ����𓾂�B"},
	//���ٍb
	{50, 20, 80,4,7,MON_CHEN,class_power_chen,do_cmd_class_power_aux_chen,5,
	"�}���q�K�̒��q","����͎g���ƈꎞ�I�Ɏ_�E�d���E�Ή��E��C�U���ւ̑ϐ��𓾂�B"},
	//���Ĕ�����V
	{80, 40, 100,3,9,MON_CHEN,class_power_chen,do_cmd_class_power_aux_chen,6,
	"�d�b�̃s�A�X","����͎g���Ǝ��͂ɖ��͑����̃{�[�����T���U�炵�A���̌��u�ŗ��E����B"},
//100
	//���e
	{35, 0, 30,7,1,MON_MURASA,class_power_murasa,do_cmd_class_power_aux_murasa,0,
	"�Ђ��Ⴍ","����͒�З͂̐������{���g����B"},
	//�f�B�[�v���H�[�e�b�N�X
	{65, 20, 60,4,2,MON_MURASA,class_power_murasa,do_cmd_class_power_aux_murasa,2,
	"�o�P�c","����͎��ӂɐ������U�����s���A����ɒn�`�𐅂ɂ���B���n�`��Ŏg���ƈЗ͂��オ��B"},
	//�D��̃��B�[�i�X
	{85, 40, 100,4,8,MON_MURASA,class_power_murasa,do_cmd_class_power_aux_murasa,4,
	"�M�H��̖X�q","����͎��E���S�Ă̏��𐅖v�����A�G�ɐ������_���[�W��^����B"},

	//���j�B���V
	{80, 20, 100,5,10,MON_KEINE,class_power_keine1,do_cmd_class_power_aux_keine,5,
	"���j�Ƃ̕M","����͎w�肵���V���{���̓G���t���A�����������B������G�̃��x���ɉ����ă_���[�W���󂯂�B��R����邱�Ƃ�����B"},
	//�I�[���h�q�X�g���[
	{40, 20, 100,1,12,MON_KEINE,class_power_keine2,do_cmd_class_power_aux_keine,3,
	"���j�Ƃ̖X�q","����͍�����t���A�̒n�`�ƃA�C�e����S�Ċ��m���A���E���̓G�̏���m��B"},
	//�����L��
	{120, 70, 128,2,16,MON_KEINE,class_power_keine2,do_cmd_class_power_aux_keine,6,
	"����̊p","����͎����𒆐S�ɋɂ߂ċ��͂Ȏ��󑮐��̃{�[���𔭐�������B���������z���̏Z�l�ɂ͌��ʂ��Ȃ��B"},

	//�n�k
	{30, 10, 60,4,1,MON_YUGI,class_power_yugi,do_cmd_class_power_aux_yugi,0,
	"�S�̉���","������g���ƒn�k���N����_���W�����̕ǂ������B"},
	//�S�C���p
	{40, 25, 100,4,8,MON_YUGI,class_power_yugi,do_cmd_class_power_aux_yugi,4,
	"�S�̎�","������g���ƈ�莞�ԋ���m������������B"},

	//�A�z��
	{40, 10, 70,6,4,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,4,
	"�A�z��","����͔j�ב����̃{�[������B"},
	//���z����
	{80, 40, 128,5,12,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,6,
	"�啼","����͎��͂̃����_���ȓG�ɖ������̃{�[����A���ŕ��B���x�����オ��ƈЗ́A���A�������a���㏸����B�ǂׂ̗ɂ���Ǝg���Ȃ��B"},
	//���E�D
	{50, 30, 100,5,9,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,8,
	"���E�D","������g���ƈꎞ�I�Ɍ��f�U���ւ̑ϐ��𓾂�B���x��40�ȍ~��AC�㏸������B"},
	//�A�z�򒹈�
	{70, 70, 128,3,10,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,12,
	"�A�z��(��)","����͋��͂Ȕj�ב����̃��P�b�g����B"},
	//���팶�e
	{200, 80, 128,1,30,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,13,
	"����̛ޏ��̃��{��","������g���Ƃ��̃t���A����Łw����@�얲�x�̌��e���z���Ƃ��Č����B"},

	//�n�C�X�s�[�h�o�E���X
	{60, 30, 110,5,6,	MON_KAGEROU,class_power_kagerou,do_cmd_class_power_aux_kagerou,2,
	"�T���̒�","������g���Ɨ��ꂽ�^�[�Q�b�g�ɔ�т������čU������B"},
	//�t�H�[�����u���X�g
	{50, 30, 60,3,4,	MON_SHIZUHA,class_power_shizuha,do_cmd_class_power_aux_shizuha,1,
	"�����t","����͓˕��̃r�[������B"},
	//�X�C�[�g�|�e�g���[��
	{30, 0, 70,8,3,		MON_MINORIKO,class_power_minoriko,do_cmd_class_power_aux_minoriko,2,
	"����","����̓A�C�e���u�Ă����v�𐶐�����B"},

	//�����̋q��
	{75, 25, 110,5,7,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,10,
	"�����`���ꂽ�l�N�^�C","����̓^�[�Q�b�g�̏ꏊ�ɑM�������̃{�[���𔭐�������B"},
	//�~���N���t���[�c
	{100, 35, 120,6,7,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,12,
	"�u���_�Ёv�Ə����ꂽ��","����̓A�C�e���u�~���N���t���[�c�v�𐶐�����B"},
	//�C��������
	{50, 35, 110,4,7,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,14,
	"���j�̂��P���_","����͓���ȃr�[������B�����̓G�ɑ�_���[�W��^���A���̑��̓G�ɂ͐������_���[�W��^����B"},
	//�_����
	//v1.1.92 ����肶��Ȃ��Ĕ����肾�����B�C��
	{80, 50, 100,3,12,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,17,
	"���ւ̔�����","����͋�Ԙc�ȑ����̋��͂ȃr�[������B"},
	//120 ��ǂ̉��
	{80, 50, 100,3,12,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,18,
	"�^�̔�����","����̓J�I�X�����̋��͂ȃu���X����B"},

	//�}�C�n�[�g�u���C�N
	{50,5, 90,6,3,		MON_REMY,class_power_remy,do_cmd_class_power_aux_remy,1,
	"�z���S�̓��P","������g���ƃA�C�e�������͂ɓ�������B"},
	//�X�J�[���b�g�V���[�g
	{60,25, 90,5,6,		MON_REMY,class_power_remy,do_cmd_class_power_aux_remy,2,
	"�O�i���P�b�g�̖͌^","����͒n���̍��Α����̃��P�b�g����B"},
	//�X�s�A�I�u�O���O�j��
	{70,60, 128,5,8,		MON_REMY,class_power_remy,do_cmd_class_power_aux_remy,7,
	"���̐F�̑�","����͖��\�����̋��͂ȃr�[������B���O���O�j���̑������͈З͂��{�ɂȂ�B"},
	//���b�h�}�W�b�N
	{50,80, 128,1,20,		MON_REMY,class_power_remy,do_cmd_class_power_aux_remy,10,
	"�z���S�̍g��","����͋ɂ߂ċ��͂Ȏ��E���U�����s���B�З͖͂��͂ɑ傫���ˑ����A����ȑ̂����G�ɑ傫�ȃ_���[�W��^����B"},

//125
	{60,25, 120,3,20,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,6,
	"�X�L�}�d���̎��","����͎��E���ɗ����Ă���A�C�e������T�����_�ֈ������荞�ށB"},
	{80,70, 128,4,8,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,9,
	"�ӎU�L���Ȑ�","����͖��@�����������A����ɏ����ꂽ���@���ЂƂ�������B"},
	{60,40, 100,5,7,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,11,
	"���̒��Î�","����͌���HP��MP�𔽓]������B"},
	{75,50, 128,3,12,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,12,
	"���{�����������ꂽ�X�L�}","������g���Ǝw�肵���ꏊ�փe���|�[�g�ł���B�e���|�[�g�ŐN���s�\�Ȓn�`�ɂ͏o���Ȃ��B"},
	{50,30, 110,3,8,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,13,
	"�p�d��","����͈꒼����̓G�Ƀ_���[�W��^���Đ�����΂��B����ȓG�ɂ͌��ʂ������B"},
	{120,10, 128,1,50,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,16,
	"�P���Ȃ��������P","����͌��݈ʒu�ɒT�����_�̓���������o���B�L���ꏊ�łȂ��Ǝg�����A�N�G�X�g���s���̃t���A(100�K��127�K�܂�)�ł͎g���Ȃ��B"},

	//��]�̖ʌ���
	{100, 50, 128,4,15,MON_KOKORO,class_power_extra,do_cmd_class_power_aux_extra,1,
	"��]�̖�(�V)","����̗͑͂�500�񕜂��o���l�E�X�e�[�^�X�𕜊�������B"},

	//��^���S�C
	{50,10, 50,8,2,	MON_NITORI,class_power_nitori,do_cmd_class_power_aux_nitori,6,
	"�͓��̐��S�C","����͐������̃{���g�𗐎˂���B"},
	//�e�ꕶ���R���v���b�T�[
	{60,30, 80,5,6,	MON_NITORI,class_power_nitori,do_cmd_class_power_aux_nitori,10,
	"�G���W�j�A�̃����b�N","����̓}�b�v�㉺�����݂̂ɔ�������Z�˒��̐������{�[������B"},
	//�I�v�e�B�J���J���t���[�W��
	{90,30, 120,1,18,	MON_NITORI,class_power_nitori,do_cmd_class_power_aux_nitori,13,
	"���w���ʃX�[�c","������g���ƒZ���ԓ����ɂȂ��ēG���猩���ɂ����Ȃ�B"},

	//�]��
	{150,70, 128,3,25,	MON_BYAKUREN,class_power_byakuren,do_cmd_class_power_aux_byakuren,0,
	"���l�o��","����͈�莞�Ԑg�̔\�͂�啝�ɏ㏸������B���x��30�ŉ����A���x��35�Ō��f�ꎞ�ϐ����ǉ������B"},
	//�C���h���̗�
	{20,5, 40,4,3,	MON_BYAKUREN,class_power_byakuren,do_cmd_class_power_aux_byakuren,1,
	"���Z�E�̐���","����̓^�[�Q�b�g�̈ʒu�ɓd�������{�[���𔭐�������B"},
	//�n���̐���
	{30,10, 60,3,5,	MON_BYAKUREN,class_power_byakuren,do_cmd_class_power_aux_byakuren,3,
	"�ƌ؋n","����͎��͂̓G��N�O�A������ɂ��悤�Ǝ��݂�B"},
	//�A�[���M���T���F�[�_
	{100,70, 128,3,20,	MON_BYAKUREN,class_power_byakuren,do_cmd_class_power_aux_byakuren,8,
	"�@�̉ԕ�","����͋��͂ȑM���������[�U�[����B"},

	//���̕s���̎�
	{100,20, 80,4,10,	MON_NUE,class_power_nue,do_cmd_class_power_aux_nue,2,
	"��̐G��","����͓G��̂𐳑̕s���ɂ���B���̕s���ɂȂ����G�͑��̓G����U�������B���j�[�N�����X�^�[�ɂ͂������Â炭�A�J�I�X�ϐ������G�ɂ͌����Ȃ��B"},
	//�V�����̒e���w
	{120,30, 110,1,20,	MON_NUE,class_power_nue,do_cmd_class_power_aux_nue,6,
	"UFO","����͎��E���̑S�Ăɑ΂����S�Ƀ����_���ȑ����̋��͂ȍU�����s���B"},

	//���Ԃ̐܂肽���ݎP
	{80, 20, 100,5,8,	MON_SEIJA,class_power_seija,do_cmd_class_power_aux_seija,4,
	"���Ԃ̐܂肽���ݎP","����͕ǂɌ����Ďg���ƕǂ̌������փe���|�[�g����B�}�b�v�[�֌������ꍇ�}�b�v�̔��Α��֏o��B�ʏ�̃e���|�[�g�ŐN���ł��Ȃ��n�`�ɂ͏o�Ă���Ȃ��B"},
	//���o�[�X�q�G�����L�[
	{50, 0, 40,2,8,	MON_SEIJA,class_power_seija,do_cmd_class_power_aux_seija,7,
	"�V�׋S�̃T���_��","����͓G��̂ƈʒu����������B���E�O�̓G�ɂ����ʂ�����B"},
	//���ƃ��v���J
	{60, 30, 80,6,8,	MON_SEIJA,class_power_seija,do_cmd_class_power_aux_seija,10,
	"�ł��o�̏���(���v���J)","����͗אڂ����G��̂ɑ�_���[�W��^����B"},

	//���K�t���A
	{90,30, 100,6,8,	MON_UTSUHO,class_power_utsuho,do_cmd_class_power_aux_utsuho,5,
	"�j��������_","����͋��͂Ȋj�M�����̃{�[������B"},
	//�w���Y�g�J�}�N
	{80,40, 100,3,10,	MON_UTSUHO,class_power_utsuho,do_cmd_class_power_aux_utsuho,10,
	"���@�G�̊�","����͎��͂�*�j��*����B"},
	//�M�K�t���A
	{65,50, 128,1,25,	MON_UTSUHO,class_power_utsuho,do_cmd_class_power_aux_utsuho,12,
	"�l�H���z","����͋ɂ߂ċ��͂Ȋj�M�����̃��[�U�[����B"},

	//�����s���̔M�a
	{60,10, 50,3,7,	MON_YAMAME,class_power_yamame,do_cmd_class_power_aux_yamame,2,
	"�q�w偂̌Q��","����͎��E���̓G�������A�N�O������B"},
	//��������
	{60,30, 80,6,5,	MON_YAMAME,class_power_yamame,do_cmd_class_power_aux_yamame,5,
	"�y�w偂̉�","����͗אڂ����G�Ƀ_���[�W��^����B�őϐ��������Ȃ��G�ɂ͎O�{�̃_���[�W��^���A�U���͂�ቺ�����A��m���ňꌂ�œ|���B"},

	//�z�K����
	{35,20, 70,6,5,	MON_SUWAKO,class_power_suwako,do_cmd_class_power_aux_suwako,2,
	"�ڋʂ̂����}","����͐������̃u���X����B�З͂͌���HP��1/4�ɂȂ�B"},

	//idx150
	{25,10, 90,4,6,	MON_SUWAKO,class_power_suwako,do_cmd_class_power_aux_suwako,3,
	"�^�̐Α�","����͗אڂ��������Ȃ��ꏊ1�O���b�h�Ɋ�𐶐�����B"},
	//��n�̌�
	{50,30, 120,1,15,	MON_SUWAKO,class_power_suwako,do_cmd_class_power_aux_suwako,7,
	"�Ő�","������g���ƈ�莞�ԕǂ𔲂�����悤�ɂȂ�B"},
	//�M��_����
	{90,50, 100,2,15,	MON_SUWAKO,class_power_suwako,do_cmd_class_power_aux_suwako,9,
	"�Ñ�̓S��","����̓����X�^�[�u�M��_�v��z���Ƃ��Ĉ�̌Ăяo���B"},

	//�������肳��
	{30,10, 40,5,4,	MON_RAN,class_power_ran,do_cmd_class_power_aux_ran,0,
	"�v�����V�F�b�g","����͈����E�d���E�A���f�b�h��̂�z���Ƃ��ČĂяo���B"},
	//���[�j���^���R���^�N�g
	{45,25, 80,4,8,	MON_RAN,class_power_ran,do_cmd_class_power_aux_ran,4,
	"���̖X�q","����͓G��̂����m���Ŗ�������B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	//���疜���얀
	{150,70, 128,8,16,	MON_RAN,class_power_ran,do_cmd_class_power_aux_ran,6,
	"�O�r�̐앝�̌v�Z��","����͎��E���S�Ăɑ΂��j�ב����U�����s���B"},

	//���O���z�̋��n
	{45,10, 90,3,6,	MON_TENSHI,class_power_tenshi,do_cmd_class_power_aux_tenshi,6,
	"�响","����͈�莞�ԕ����h��͂Ɩ��@�h��͂��㏸������B"},
	//�J�i���t�@���l��
	{55,30, 70,4,7,	MON_TENSHI,class_power_tenshi,do_cmd_class_power_aux_tenshi,7,
	"���V�v��","����͂��̃t���A����Łu�J�i���t�@���l���v����������B"},
	//�s���y��̌�
	{55,30, 100,3,12,	MON_TENSHI,class_power_tenshi,do_cmd_class_power_aux_tenshi,10,
	"����ȗv��","����͎��͂̋����͈͂�*�j��*����B"},
	//�S�l�ނ̔�z�V
	{60,40, 128,1,45,	MON_TENSHI,class_power_tenshi,do_cmd_class_power_aux_tenshi,11,
	"�V�E�̕�","����͋ɂ߂ċ��͂ȋC�����̃r�[������B"},
	
	//�،���
	{30,20, 60,6,4,	MON_MEIRIN,class_power_meirin,do_cmd_class_power_aux_meirin,2,
	"���傤��","����͋C�����̋ʂ���B"},
	//�ҌՓ���
	{40,40, 100,5,9,	MON_MEIRIN,class_power_meirin,do_cmd_class_power_aux_meirin,8,
	"�����Ƃ̖X�q","����͒ቺ�����r�͂𕜊������A��莞�Ԃ̑啝�Șr�͏㏸�Ǝm�C���g�������炷�B"},

	//�W�F���V�[�{���o�[
	{40,10, 70,6,5,MON_PARSEE,class_power_parsee,do_cmd_class_power_aux_parsee,4,
	"�m�l�`","����͐��_�U�������̃{�[������B�����̗̑͂��Ⴂ�قǈЗ͂��㏸����B"},
	//�ω��Ԃ�
	{160,80, 128,1,50,MON_PARSEE,class_power_parsee,do_cmd_class_power_aux_parsee,9,
	"���P�̓S��","����͎����̂���ꏊ�ɒn���̍��Α����̋���ȃ{�[���𔭐�������B�З͂͂��̃t���A�Ŏ������󂯂��_���[�W�̍��v�ɂȂ�A��x�g���ƃ��Z�b�g�����B"},

	//*�ӎ�*
	{50,20, 90,5,7,MON_EIKI,class_power_eiki,do_cmd_class_power_aux_eiki,3,
	"����Ȓ����̋L�^��","����͎w�肵���A�C�e���̐��\�����S�ɒm��B"},
	//���X�g�W���b�W�����g
	{180,90, 128,3,30,MON_EIKI,class_power_eiki,do_cmd_class_power_aux_eiki,7,
	"����̖_","����͋ɂ߂ċ��͂Ȕj�ב����̃r�[������B"},

	//�w�����e���|�[�g
	{65,20, 70,10,3,MON_SUMIREKO,class_power_sumireko,do_cmd_class_power_aux_sumireko,3,
	"��p�t�̃}���g","������g���Ǝw�肵�������ֈ�苗���̃e���|�[�g���s���B�e���|�[�g�s�\�Ȓn�`�ɏo�悤�Ƃ�������ӂ̃����_���ȏꏊ�֏o��B"},
	//�T�C�R�L�l�V�X�A�v��
	{90,50, 110,4,15,MON_SUMIREKO,class_power_sumireko,do_cmd_class_power_aux_sumireko,11,
	"�ِ��E�̃^�u���b�g","����͎��E���S�Ăɋ��͂Ȏ��󑮐��U�����d�|����B"},

	//���E�̗��_
	{30,10, 50,8,3,MON_ICHIRIN,class_power_ichirin,do_cmd_class_power_aux_ichirin,2,
	"�d���s�҂̌U��","����͎����ׂ̗ɗ��������̃{�[���𔭐�������B"},
	//�_��n���˂�
	{35,20, 75,6,5,MON_ICHIRIN,class_power_ichirin,do_cmd_class_power_aux_ichirin,3,
	"�_�����̌���","����͋C�����̃��P�b�g����B"},

	//�P���V��
	{90,15, 75,8,5,MON_MOKOU,class_power_mokou,do_cmd_class_power_aux_mokou,2,
	"�Ă���","����͊j�M�����̃��P�b�g����B"},
	//�����ʔ��΂̕�
	{70,5, 40,8,5,MON_MOKOU,class_power_mokou,do_cmd_class_power_aux_mokou,5,
	"���Ε��̑�","����͎��͂̃����_���Ȑ������ɔ����̃��[����ݒu����B"},
	//�t�F�j�b�N�X�̔�
	{50,30, 80,8,7,MON_MOKOU,class_power_mokou,do_cmd_class_power_aux_mokou,6,
	"�t�F�j�b�N�X�̔�","����͗אڂ����G��̂ɉΉ������U�����s���A�񃆃j�[�N�̃A���f�b�h�����m���ňꌂ�œ|���B"},
	//�t�W���}���H���P�C�m
	{60,40, 120,1,25,MON_MOKOU,class_power_mokou,do_cmd_class_power_aux_mokou,8,
	"�|�Y","����͎��E���S�Ăɑ΂����͂ȉΉ������U�����s���B"},

	//�G�N�X�p���f�b�h�E�I���o�V��
	{70,20, 80,4,8,MON_KANAKO,class_power_kanako,do_cmd_class_power_aux_kanako,2,
	"�䒌","����͓G�𐁂���΂��r�[������B�e���|�[�g�ϐ��𖳎����邪����ȓG�͐�����΂Ȃ��B"},
	//�_�̊�
	{30,10, 60,4,6,MON_KANAKO,class_power_kanako,do_cmd_class_power_aux_kanako,3,
	"����","����͎��͂̒n�`�E�A�C�e���E�g���b�v�E�����X�^�[�����m���������𖞂����B"},

	//�M����
	{30,0, 50,12,2,MON_FUTO,class_power_futo,do_cmd_class_power_aux_futo,2,
	"�M","����͔j�Б����̔��˂���Ȃ��{���g����B"},
	//���e�ݒu
	{60,30, 80,9,4,MON_3FAIRIES,class_power_star,do_cmd_class_power_aux_star,3,
	"���e","����͑����ɔ��e��ݒu����B���e�́u�ʏ�́v10�^�[���Ŕ��������a3�̔j�Б����̋��͂ȃ{�[���𔭐�������B���̔����ł̓v���C���[���_���[�W���󂯂�B���e�͈ꕔ�̑����U���ɎN�����ƗU�������蔚���^�C�~���O���ς�邱�Ƃ�����B"},

	//���ʌ��̏�C�l�`
	{50,15, 60,5,3,MON_ALICE,class_power_alice,do_cmd_class_power_aux_alice,2,
	"��C�l�`","����͑M�������̃r�[������B"},
	//��݂�H���l�`
	{50,40, 100,5,8,MON_ALICE,class_power_alice,do_cmd_class_power_aux_alice,9,
	"�H���l�`","����͋��͂ȈÍ������̃r�[������B"},
//180
	//�m�C�Y�������R���[
	{60,25, 80,7,5,MON_LUNASA,class_power_lunasa,do_cmd_class_power_aux_lunasa,3,
	"���@�C�I����","����͎��E���̓G�������A�����A���|�A�N�O������B"},
	//�\�E���S�[�n�b�s�[
	{50,20, 70,7,5,MON_MERLIN,class_power_merlin,do_cmd_class_power_aux_merlin,3,
	"�g�����y�b�g","����͈�莞�ԉ����Ɠ��̋����������炷�B���������͂̓G���N�����E���̓G����������B"},
	//�\�E���m�C�Y�t���[
	{50,15, 60,7,5,MON_LYRICA,class_power_lyrica,do_cmd_class_power_aux_lyrica,3,
	"�L�[�{�[�h","����͎��͂̃����X�^�[�A���A㩁A�A�C�e�������m����B"},

	//�w���G�N���v�X
	{90,45, 100,2,12,MON_CLOWNPIECE,class_power_clownpiece,do_cmd_class_power_aux_clownpiece,2,
	"�n���̏���","����͍����镔���𖾂邭���A���E���̓G�ɐ��_�U���Ɨl�X�ȏ�Ԉُ�t�^���s���B"},
	//�X�g���C�v�h�A�r�X
	{120,85, 128,1,25,MON_CLOWNPIECE,class_power_clownpiece,do_cmd_class_power_aux_clownpiece,6,
	"�n���̓����̖X�q","����͎��E���̑S�Ăɑ΂��n���̍��Α����U������B"},

	//��F�̈���
	{25,0, 45,5,3,MON_DOREMY,class_power_doremy,do_cmd_class_power_aux_doremy,1,
	"����","����͓G��̂����m���Ŗ��点��B�����ϐ��𖳎����邪���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	//���F�̏D��
	{45,30, 85,12,4,MON_DOREMY,class_power_doremy,do_cmd_class_power_aux_doremy,3,
	"������","����͓G��̂ɐ��_�U�����d�|����B�����Ă���G�Ɏg���ƈЗ͂������A�܂��G���N�����ɂ����B"},

	//��_�l�̃o�C�I���Y���{��z��
	{40,0, 40,4,5,MON_HINA,class_power_hina,do_cmd_class_power_aux_hina,1,
	"�Q�����A�b�v���P","����͎��ꂽ�i�𑕔����Ă���Ƃ��Ɏg���Ɖ��􂵂ċ󕠂𖞂����B"},
	//�u���[�N���A�~�����b�g
	{75,10, 75,2,5,MON_HINA,class_power_hina,do_cmd_class_power_aux_hina,5,
	"�s�g�Ȋ����̂��邨���","����͎��ꂽ�A�~�����b�g���ЂƂ���A���͂Ȕj�Б����̃{�[���𔭐�������B"},

	//�o�j�V���O�G�u���V���O
	{25,10, 80,5,4,MON_SAKUYA,class_power_sakuya,do_cmd_class_power_aux_sakuya,4,
	"�g�����v�̃J�[�h","������g���ƒ������̃e���|�[�g������B"},
	//�E�l�h�[��
	{70,30, 100,3,10,MON_SAKUYA,class_power_sakuya,do_cmd_class_power_aux_sakuya,8,
	"��̃i�C�t","����͎��͂̃����_���ȓG�ɑ΂���ʂ̔j�Б����{���g����B"},
	//�U�E���[���h
	{160,75, 128,1,50,MON_SAKUYA,class_power_sakuya,do_cmd_class_power_aux_sakuya,17,
	"�~�܂����⎞�v","������g���Ǝ��Ԃ���~���A����A���ōs���ł���B"},

	{30,5, 45,5,4,MON_YATSUHASHI,class_power_yatsuhashi,do_cmd_class_power_aux_yatsuhashi,1,
	"�Âт���","����͓G��̂������A����������B���x��30�ȍ~�͎��E���̓G�S�ĂɌ��ʂ��y�ԁB"},
	{30,5, 45,5,4,MON_BENBEN,class_power_benben,do_cmd_class_power_aux_benben,3,
	"�Âт����i","����͑M�������̃r�[������B"},

	//v1.1.85 �����h�p�[�J�X�����_�̓{��
	{60,25, 95,7,3,MON_RAIKO,class_power_raiko,do_cmd_class_power_aux_raiko,4,
	"�r�[�^�[�u�[�c","����͎����𒆐S�Ƃ�������ȓd�������̃{�[���𔭐������A���͂̒n�`���u���@�̗��_�v�ɕς���B"},
	//�����a���ۃ��P�b�g
	{30,25, 120,4,5,MON_RAIKO,class_power_raiko,do_cmd_class_power_aux_raiko,8,
	"�a����","����͔j�Б����̃��P�b�g����B�З͂͌���HP��1/3�ɂȂ�B"},
	//�ω�
	{80,30, 128,5,8,MON_MAMIZOU,class_power_mamizou,do_cmd_class_power_aux_mamizou,3,
	"�K�̗t����","������g���Ƌ߂��̎w�肵�������X�^�[�Ɉ�莞�ԕϐg����B�ϐg���̓��{���E�A�~�����b�g�ȊO�̑���������������A�ϐg���������X�^�[�̔\�͂�'U'�R�}���h�Ŏg�����Ƃ��ł���B�ϐg�̐������̓��x���A�m�\�A�����A����̃��x���Ō��܂�B"},

	//�򉻍��
	{80,30, 90,3,8,MON_RINGO,class_power_ringo,do_cmd_class_power_aux_ringo,6,
	"�������̒w�","����͎��̗͂d���A�����A�b�����m���Ńt���A������ł�����B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	//�Z�v�e���o�[�t�����[��
	{200,90, 120,1,40,MON_RINGO,class_power_ringo,do_cmd_class_power_aux_ringo,9,
	"���̒c�q","�����9�^�[���̊Ԗ��G�̃o���A�𒣂�B���ʂ��؂��Ƃ����������ł���B"},
	//�c�q�쐬
	{30,0, 50,7,3,MON_SEIRAN,class_power_seiran,do_cmd_class_power_aux_seiran,0,
	"�Ԃ��G�ꂽ�n","������g���ƃA�C�e���u�c�q�v�����B"},

	//idx200 �十�ʓe
	{60,20, 60,4,7,MON_EIRIN,class_power_eirin,do_cmd_class_power_aux_eirin,1,
	"���̍��F","����͎��͂̃����X�^�[�������E�N�O������B"},
	//�⒆�̓V�n
	{80,10, 100,3,8,MON_EIRIN,class_power_eirin,do_cmd_class_power_aux_eirin,2,
	"�傫�Ȗ��","����ɔ�э��ނƉ��̃t���A�ɍs����B���łɍŉ��w�̏ꍇ�����N���Ȃ��B"},
	//���C�t�Q�[��
	{50,40, 110,3,10,MON_EIRIN,class_power_eirin,do_cmd_class_power_aux_eirin,5,
	"���ӈ�̖�","����͗אڂ���������̂����S�ɉ񕜂�����B"},

	//�H���̋ʂ̎}
	{255,100, 128,5,25,MON_KAGUYA,class_power_kaguya,do_cmd_class_power_aux_kaguya,10,
	"�D�܉؂̖~��","����͗l�X�ȑ����̋���ȋ�����B"},
	//�{�k�̏p�T
	{70,40, 100,5,8,MON_KAGUYA,class_power_kaguya,do_cmd_class_power_aux_kaguya,12,
	"���̋���","������g���Ǝw�肵���ꏊ�܂ň�u�ňړ�����B�e���|�[�g�o���A�Ȃǂ𖳎����邪���m�̒ʘH���ʂ����ꏊ�ɂ����s���Ȃ��B"},

	//�q�g�T�m�^�@��
	{50,20, 80,6,5,MON_SAGUME,class_power_sagume,do_cmd_class_power_aux_sagume,1,
	"���̋@��","������g���Ƃ��̃t���A����Ń����X�^�[�u�q�g�T�m�^�@���v���Ăяo���B�@���͕��􂵔j�ב����̔������N�����B"},
	//�З��̔���
	{90,60, 110,2,14,MON_SAGUME,class_power_sagume,do_cmd_class_power_aux_sagume,5,
	"����̉H��","������g���Ƌ��͂ȑM�������̃{�[���𔭐������Ē������e���|�[�g����B���e���|�[�g�ɖW�Q����Ȃ��B"},

	//�t�F���g�t�@�C�o�[
	{120,30, 120,1,30,MON_REISEN2,class_power_reisen2,do_cmd_class_power_aux_reisen2,4,
	"�t�F���g�t�@�C�o�[�̑g�R","����̓����X�^�[��̂𔛂�グ�Ĉړ���W�Q���A���m���ōs�����W�Q����B�f�����G�E�͋����G�E����ȓG�E���j�[�N�����X�^�[�ɂ͌����Â炢�B"},

	//�����̔��U
	{60,20, 80,7,5,MON_REISEN2,class_power_toyohime,do_cmd_class_power_aux_toyohime,6,
	"���҂̕���","����͎��E���̃����X�^�[��̂��w�肵���ꏊ�Ƀe���|�[�g������B�e���|�[�g�ϐ������G�ΓI�ȃ����X�^�[�ɂ͌��ʂ��Ȃ��B"},
	//���̌��]�������
	{255,110, 127,1,100,MON_TOYOHIME,class_power_toyohime,do_cmd_class_power_aux_toyohime,8,
	"���҂̕���","����𔭓�����ƈȌヂ���X�^�[���e���|�[�g�����Ƃ��t���A������ł��A����Ƀ����X�^�[�ɂ�鏢�����@���j�Q�����悤�ɂȂ�B"},

	//�_�~�낵�u���R�F���v
	{65,10, 90,1,20,MON_SANAE,class_power_yorihime,do_cmd_class_power_aux_yorihime,2,
	"�p���W�E�������̒�","������g���Ɓu���R�F���v���~�Ղ��A��莞�ԓG�̐n���ɂ��U����h���B"},
	//�_�~�낵�u�V���P���v
	{120,60, 110,7,6,MON_YORIHIME,class_power_yorihime,do_cmd_class_power_aux_yorihime,6,
	"�����Ē�","������g���Ɓu�V���P���v���~�Ղ��A���E���̃����X�^�[�����̌����ōU������B"},

	//���q�ݐg
	{70,30, 90,3,10,	MON_HECATIA1,class_power_hecatia,do_cmd_class_power_aux_hecatia,4,
	"���_�̎��","����͎��E���̃A���f�b�h�����m���Ŗ�������B"},
	//���i�e�B�b�N�C���p�N�g
	{240,90, 128,3,30,	MON_HECATIA1,class_power_hecatia,do_cmd_class_power_aux_hecatia,8,
	"���ʃN���[�^�[","����͋��͂�覐Α����U�����s���B"},

	//���n�̐_��E
	{70,20, 100,7,7,	MON_JUNKO,class_power_junko,do_cmd_class_power_aux_junko,4,
	"�������̊�","����͗אڂ��������X�^�[��̂���i�K�����N�A�b�v������B"},

	//v1.1.33 �Α���
	{60,10,70,3,6,	MON_AUNN,class_power_aunn,do_cmd_class_power_aux_aunn,3,
	"�����̑��","������g���ƐΑ����������X�^�[����F������Ȃ��Ȃ�B���E���Ƀ����X�^�[�������Ԃł͎g�p�ł��Ȃ��B�_���[�W���󂯂���ҋ@�A�x�e�A���H�ȂǈȊO�̍s��������ƌ��ʂ��؂��B"},

	//v1.1.34 �N���~�i���T�����F�C�V����
	{160,90, 127,1,30,MON_NARUMI,class_power_narumi,do_cmd_class_power_aux_narumi,8,
	"���n���l�̑O�|��","����͈�x�����n���̋ƉΑ����̍U���𖳌�������B"},

	//v1.1.35 �R�W�̕����
	{ 120,30, 127,5,8,MON_NEMUNO,class_power_nemuno,do_cmd_class_power_aux_nemuno,3,
		"�R�W�̓u��","����͈ꎞ�I�ɕ���ɐ؂ꖡ�������炷�B���ɐ؂ꖡ�̗ǂ�����͂���ɋ��������B" },

	//v1.1.37 ���鉅��߈�
	{ 150,40, 80,1,15,MON_KOSUZU,class_power_kosuzu,do_cmd_class_power_aux_kosuzu,4,
		"�Â������̑�","������g���Ɖ���Ɏ��߂����B���߂���Ă���Ԃ͎푰�Ɠ��Z���ꎞ�I�ɕω�����B" },

	//v1.1.39 �����o�ψ�
	{ 100,5, 80,4,8,MON_LARVA,class_power_larva,do_cmd_class_power_aux_larva,7,
	"�A�Q�n�����","������g���ƃ����_���ȓˑR�ψق������B" },

	//v1.1.41 IDX220 �^�i�o�^�X�^�[�t�F�X�e�B�o��
	{ 70,20, 90,6,6,MON_MAI,class_power_mai,do_cmd_class_power_aux_mai,4,
		"��","�����HP�𒆒��x�񕜂��X�e�[�^�X�ُ�����Â���B�R�撆(���邢�̓_���X��)�̔z��������΂���HP�������񕜂���B" },

	//v1.1.43 �V�N���E���s�[�X
	{ 120,10, 60,10,1,MON_CLOWNPIECE,class_power_vfs_clownpiece,do_cmd_class_power_aux_vfs_clownpiece,0,
	"⾉�","����̓����X�^�[��̂����C�ɖ`���B���C�ɖ`���ꂽ�����X�^�[�͋߂��̃����X�^�[���U������B���j�[�N�����X�^�[��ʏ�̐��_�������Ȃ������X�^�[�ɂ͌����Â炢�B" },

	//v1.1.44 �V���ǂ�
	{ 160,80, 120,1,25,MON_UDONGE,class_power_udonge_d,do_cmd_class_power_aux_udonge_d,12,
		"���Ȃт��e��","����͓G����̍U���̃_���[�W���O�񖳌�������o���A�𒣂�B" },

	//v1.1.45 �t�H�Q�b�g���A�l�[��
	{ 80,10, 70,3,10,MON_SATONO,class_power_satono,do_cmd_class_power_aux_mai,2,
	"䪉�","����͗אڂ��������X�^�[�S�Ă����݂̃t���A���獂�m���ŒǕ�����B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B" },

	//v1.1.46 �X���C�u���o�[
	{ 70,10, 90,2,8,MON_JYOON,class_power_jyoon,do_cmd_class_power_aux_jyoon,7,
		"�u�a�_�̘r��","������g���Ɨאڂ��������X�^�[��̂̉����Ɩ��G����D������Ď������������邱�Ƃ��ł���B" },

	//v1.1.48
	{ 120,30, 90,1,20,MON_SHION_2,class_power_shion,do_cmd_class_power_aux_shion,4,
		"���L�̖D�������","����͈ꎞ�I�ɉB���\�͂����߂�B���x��40�ȍ~�͕ǔ����\�͂��ǉ�����B" },
	{ 100,60, 120,1,25,MON_SHION_2,class_power_shion,do_cmd_class_power_aux_shion,7,
		"�n�R�_�̔�","����͎��E���̃����X�^�[��HP�𔼌�������B��R�����Ɩ����B���j�[�N�����X�^�[�ɂ͌��ʂ��Ȃ��B" },

	//v1.1.57 �������B��ށu�����̌��v
	{ 120,30, 100,7,5,MON_OKINA,class_power_okina,do_cmd_class_power_aux_okina,8,
		"�V���̗�","������g���ƈꎞ�I�ɕ���u�������v�𐶐����đ�������B��̑������󂢂Ă���Ƃ��ɂ����g���Ȃ��B" },

	//v1.1.59 �G���V�F���g�V�b�v
	{ 120,0, 40,1,20,MON_FUTO,class_power_futo,do_cmd_class_power_aux_futo,3,
		"�Ñ�̏M","����͋R��p���ꃂ���X�^�[�u�G���V�F���g�V�b�v�v���Ăяo���B" },

	//v1.1.65 �i�Ԃ̒O
	{ 70, 1, 50,7,4,MON_EIRIN,class_power_extra,do_cmd_class_power_aux_extra,2,
		"�i�Ԃ̒O","����̗͑͂�100�񕜂��ŁE�؂菝�E�N�O�E���o�E�b����������Â��X�e�[�^�X�𕜊�������B" },

	//v1.1.65 ���Ԑΐς�
	{ 60,1, 20,1,10,MON_EIKA,class_power_eika,do_cmd_class_power_aux_eika,0,
		"�΂̉͌��̐ςݐ�","����͌��ݒn�Ɏ��̃��[�������B" },

	//v1.1.65 �S�ؐ�@���Ԓn�����
	{ 150, 20, 120,1,30,MON_KASEN,class_power_oni_kasen,do_cmd_class_power_aux_oni_kasen,6,
		"�S�̘r�Ə����ꂽ��","����͎��͂̐����̂��郂���X�^�[���ꌂ�œ|���A���Ȃ��̗̑͂��񕜂���B��R�����Ɩ����B" },

	//v1.1.66
	{ 90,15, 45,1,10,MON_MAYUMI,class_power_mayumi,do_cmd_class_power_aux_mayumi,2,
		"����","����͏��ւ̕��m����������B" },

	//v1.1.67 ���Z�ł͂Ȃ����j���^���_�̗͂�����
	{ 200, 20, 70,4,10,MON_KUTAKA,class_power_extra,do_cmd_class_power_aux_extra,3,
		"�{�̕`���ꂽ�G�n","����͊P���~�߂�B" },

	//v1.1.68 �X�򐶐�
	{ 90, 10, 70,7,4,MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,7,
		"�J�[�v�{�[��","����͗אڂ������ꂩ���̒n�`���u�X��v�ɂ���B" },

	//v1.1.69 �X�g�[���x�C�r�[
	{ 100, 0, 60,1,20,MON_URUMI,class_power_urumi,do_cmd_class_power_aux_urumi,1,
		"�Ԏq�̐Α�","����̓����X�^�[��̂𐅂ɒ��߂�B���ӂ��߂��ɂȂ��Ǝg���Ȃ��B���j�[�N�����X�^�[�␅�ϐ����������X�^�[�ɂ͖����B" },

	//v1.1.71 �t�H���~�[�A���A�t���C�h
	{ 50, 0, 70,1,10,MON_SAKI,class_power_saki,do_cmd_class_power_aux_saki,1,
		"�g���̃X�J�[�t","����̓����X�^�[��̂�z���Ɋ��U����B���|��Ԃ̃����X�^�[�Ɍ��ʂ������B���j�[�N�����X�^�[�ɂ͖����B" },

	//v1.1.71 �g���C�A���O���`�F�C�X
	{ 90, 70, 127,3,15,MON_SAKI,class_power_saki,do_cmd_class_power_aux_saki,7,
		"�V�n�̗�","������g���ƃ����X�^�[��ׂ̗̂܂ň�u�ňړ����A����Ɉꌂ���E����B���E�Ɏ��s���邱�Ƃ�����B" },

	//v1.1.73 �T�b�n��
	{ 60, 30, 60,6,5,MON_YACHIE,class_power_yachie,do_cmd_class_power_aux_yachie,5,
		"�傫�ȍb��","������g���Ǝ��͂̃����X�^�[�ɒx�ݑ����U�����s���B" },

	//v1.1.74 ���`���`�p
		{ 150, 30, 90,7,7,MON_KEIKI,class_power_keiki,do_cmd_class_power_aux_keiki,6,
		"�Õ�","����͎����̎��͂̒n�`����΂ɂ���B" },

	//idx:240
	//v1.1.88 �^�E�؉B��̋Z�p
		{ 120,20, 90,3,10,	MON_TAKANE,class_power_takane,do_cmd_class_power_aux_takane,9,
		"�R���̖��ʕ�","������g���ƒZ���ԓG����F������ɂ����Ȃ�B�������X�̒��ɂ��Ȃ��ƌ��ʂ��Ȃ��B" },

	//v1.1.89 �ς��葱����}�C���_���v
		{ 160,50, 120,3,15,	MON_MOMOYO,class_power_momoyo,do_cmd_class_power_aux_momoyo,6,
		"�z�Ɣp����","����͎��͂̒n�`��Εǂɕς���B" },

	//���E���f�o�t�@�R�@���Z�ɂ��̂܂ܑ��E�Ƃŗ��p�ł������Ȃ��̂��Ȃ��̂ŐV��
		{ 120, 30, 80,8,7,MON_SANNYO,class_power_extra,do_cmd_class_power_aux_extra,4,
		"�V�R�P�O�O���d���̎R�Y�̉���","����͎��͂̃����X�^�[��N�O�A�����A����������B" },

	//v1.1.92 �u�a�_�I�ȓV��@��@
		{ 80,40, 100,5,6,	MON_JYOON,class_power_jyoon_2,do_cmd_class_power_aux_jyoon_2,8,
		"�Ζ����̎w��","������g���Ǝw�肵���^�[�Q�b�g�̋߂��Ɉړ����A���ӂɖ������_���[�W�Ɛ������_���[�W��^���A����Ɏ��ӂ�Ζ��n�`�ɂ���B" },

	//v1.1.93 �~�P�e�Џ���
		{ 120,5, 80,1,25,	MON_MIKE,class_power_mike,do_cmd_class_power_aux_mike,4,
		"�O�єL�̏����L","������g���ƈꎞ�I�ɍK�^��ԂɂȂ邪�A�����ɔ�����ԂɂȂ�B" },

	//v2.0.2 �T�@�x�����̊ǌϒe
		{ 80,40, 80,5,6,	MON_TSUKASA,class_power_tsukasa,do_cmd_class_power_aux_tsukasa,2,
		"�K���X�̊�","������g���Ɣj�Б����̃��P�b�g�ōU������B�U�����󂯂������X�^�[�͍s�����邽�тɃ_���[�W���󂯂�B" },

	//v2.0.3 ���@�V�^ࣖ��̐�
		{ 120,40, 120,1,30,	MON_MEGUMU,class_power_megumu,do_cmd_class_power_aux_megumu,5,
		"�ӓV�V","������g���ƃt���A�S�̂̃����X�^�[���������Ԃɂ��悤�Ǝ��݂�B���j�[�N�����X�^�[�A�͋��������X�^�[�A�_�i�ɂ͌��ʂ������B" },

	//v2.0.4 �����A�z��
		{ 40,30, 80,7,5,MON_MISUMARU,class_power_misumaru,do_cmd_class_power_aux_misumaru,6,
		"���F�̉A�z��","����͓������̃{�[���U������B" },

	//v2.0.6 �ޖ��@�K�C�A�̌��t
		{ 100,50, 120,3,16,	MON_YUMA,class_power_yuma,do_cmd_class_power_aux_yuma,7,
		"�b�_�̃s�A�X","������g���Ƒ̗͂�S�񕜂��A�؂菝�Ɣ\�͒ቺ���������A����ɘr�͊�p�ϋv��Z���ԏ㏸������B�����������x���ʂɏ����B" },

	//v2.0.7 �疒�@���l��
		{ 150,40, 80,7,7,	MON_CHIMATA,class_power_chimata,do_cmd_class_power_aux_chimata,4,
		"���F�̃J�`���[�V��","����͓������̃{�[������B�A�r���e�B�J�[�h�̍����x�ɉ����ĈЗ͂��オ��B" },

	//v2.0.9 �����@�����߂̈�{
		{ 120,30, 70,8,3,	MON_MIYOI,class_power_miyoi,do_cmd_class_power_aux_miyoi,7,
		"�N�W���̌`�������X�q","����̓����X�^�[��̂�F�D�I�ɂ��Ď���ۂ܂���B������{����A��������ɂ���Č��ʂƐ��������ς��B�l�ԂɌ��ʂ������B" },

	//v2.0.11 ���V�@�T������
		{ 90,20, 70,1,20,	MON_BITEN,class_power_biten,do_cmd_class_power_aux_biten,2,
		"��Ⰾ�","����͑�ʂ̉�����������B" },


		

	{0,0,0,0,0,0,NULL,NULL,0,"�I�[�_�~�[",""},
};

//�A�C�e���J�[�h�̃e�X�g�R�}���h�@�f�o�b�O�R�}���h'S'
bool support_item_test(void)
{
	char tmp_val[160];
	int num = 0;
	int table_len;
	class_power_type *spell;
	cptr (*class_power_aux)( int, bool);

	table_len = sizeof(support_item_list) / sizeof(support_item_type)-2;
	sprintf(tmp_val, "0");
	if (!get_string(format("itemcard_Pval(max:%d): ",table_len), tmp_val, 3)) return FALSE;
	num = atol(tmp_val);

	if(num > table_len) num = table_len;
	if(num < 0) num=0;

	class_power_aux = support_item_list[num].func_aux;
	spell = &(support_item_list[num].func_table[support_item_list[num].func_num]);

	use_itemcard = TRUE;

	msg_format("���Ȃ��́u%s�v���g�����I",support_item_list[num].name);
	msg_format("IDX:%d[%s](%s)",num,spell->name,class_power_aux(support_item_list[num].func_num,TRUE));
	class_power_aux(support_item_list[num].func_num,FALSE);

	use_itemcard = FALSE;

	return TRUE;

}


//EXTRA���[�h�p�́u�A�C�e���J�[�h�v�ɑ΂��p�����[�^�t�^����Bapply_magic�̑���ɌĂԁB
//apply_magic()���Ă񂾏ꍇ��tv��TV_ITEMCARD�̏ꍇr_idx=0�ł����ɔ�ԁB
//lev:�������x��
//r_idx:�����X�^�[idx ����idx�Ɉ�v����J�[�h�̂����������x���ɉ����đI�΂��B0�̂Ƃ��������x���ƃ��A���e�B�ɉ����ă����_���ɑI�΂��B
void apply_magic_itemcard(object_type *o_ptr, int lev, int r_idx)
{

	int card_idx = -1;
	int i;

	if(o_ptr->tval != TV_ITEMCARD)
	{
		msg_print("ERROR:apply_magic_itemcard()��TV_ITEMCARD�ȊO���n���ꂽ");
		return;
	}

	if(lev < 1) lev=1;
	if(lev > 128) lev=128;

	if(r_idx)
	{
		int tmp_cnt=0;
		int bonus = randint1(20);//r_idx�w�莞(�Y�������X�^�[��|�����Ƃ�)�͗ǂ��J�[�h���o�₷��
		lev += bonus;
		if(lev>128) lev = 128;
		//�A�C�e���J�[�h���X�g���A���A���e�B0(�I�[�_�~�[)�ɒB����܂Ń��[�v
		for(i=0;support_item_list[i].rarity;i++)
		{
			//idx����v���A�������x��+�{�[�i�X�l�������𖞂����J�[�h���烉���_���ɑI�΂��
			if(support_item_list[i].r_idx != r_idx) continue;
			if(support_item_list[i].min_lev > lev) continue;
			if(support_item_list[i].max_lev < lev) continue;
			tmp_cnt++;
			if(one_in_(tmp_cnt)) card_idx = i;
		}
	}

	//idx����v����J�[�h���ݒ肳��Ȃ������ꍇ�A�S�J�[�h���烉���_���ɑI�΂��B���A���e�B�������قǏo�Â炢�Bmin/max�ɍ���Ȃ��J�[�h�����܂ɏo��
	if(card_idx < 0)
	{
		int tester = 0;

		for(i=0;support_item_list[i].rarity;i++)
		{
			int prob = 1000 / support_item_list[i].rarity;

			if(support_item_list[i].min_lev > lev)
				prob /=  (support_item_list[i].min_lev - lev) / 5 + 2;
			if(support_item_list[i].max_lev < lev)
				prob /=  (lev - support_item_list[i].max_lev) / 5 + 2;
			tester += prob;
		}

		tester = randint1(tester);

		for(i=0;support_item_list[i].rarity;i++)
		{
			int prob = 1000 / support_item_list[i].rarity;

			if(support_item_list[i].min_lev > lev)
				prob /=  (support_item_list[i].min_lev - lev) / 5 + 2;
			if(support_item_list[i].max_lev < lev)
				prob /=  (lev - support_item_list[i].max_lev) / 5 + 2;
			tester -= prob;
			if(tester <= 0) break;
		}
		card_idx = i;
	}
	//��������ΏۂɂȂ�J�[�h���ݒ肳��Ȃ������ꍇ,idx=0�ɂȂ�͂�
	//�O�̂���
	if(!support_item_list[card_idx].rarity)
	{
		msg_print("ERROR:apply_magic_itemcard()��tester�l�v�Z����������");
		card_idx = 0;
	}

	//pval�ɃJ�[�h�z��Y�����i�[
	o_ptr->pval = card_idx;

	//��������
	if(support_item_list[card_idx].prod_num <= 1)
	{
		o_ptr->number = 1;
	}
	else
	{
		//��b�������}30%
		int num;
		int mult = 66 + damroll(4,16);

		if(lev > support_item_list[card_idx].max_lev) mult += (lev -  support_item_list[card_idx].max_lev) * 2;
		num = support_item_list[card_idx].prod_num * mult / 100;
		if(num > 99) num = 99;
		o_ptr->number = num;

	}

}

//�A�C�e���J�[�h�X���b�g�����v�Z����B�Ƃ肠����8�ɂ��Ă�����������������16�܂ő��₷�H
//17�ȍ~��inven_special[]���ʂ̂��ƂɎg���Ă�
int calc_itemcard_slot_size(void)
{
	int num = 8;

	if(inventory[INVEN_RARM].name1 == ART_GRIMOIRE_OF_MARISA ||inventory[INVEN_LARM].name1 == ART_GRIMOIRE_OF_MARISA )	num += 4;

	if(num > INVEN_SPECIAL_ITEM_SLOT_END){ msg_print("ERROR:�A�C�e���J�[�h�X���b�g��������������"); return 8;}
	return num;
}

/*:::�A�C�e���J�[�h�̈ꗗ�\��*/
//cur_pos:use_menu=TRUE�̂Ƃ��J�[�\�����\�������ʒu
void print_itemcard_list(bool show_info, int cur_pos)
{
	int		i;
	char	tmp_val[40];
	char	o_name[MAX_NLEN];
	int		itemslot_max = calc_itemcard_slot_size(); //�A�C�e���J�[�h�X���b�g���� inven_special[]�Y���Ɋ֌W
	object_type *o_ptr;
	cptr (*class_power_aux)( int, bool);


	for(i=1;i<itemslot_max+3;i++) Term_erase(17, i, 255);

	cur_pos += INVEN_SPECIAL_ITEM_SLOT_START;

	/*:::�ǉ��C���x���g���\���@��ɂ��ׂĂ̗�*/
	for (i = INVEN_SPECIAL_ITEM_SLOT_START; i <= itemslot_max; i++)
	{
		o_ptr = &inven_special[i];

		if (use_menu)
		{
			sprintf(tmp_val, "%s ",(i==cur_pos?"�t":"  " ));

		}
		else
		{
			//���x��
			sprintf(tmp_val, "%c)", index_to_label(i) - INVEN_SPECIAL_ITEM_SLOT_START);
		}

		put_str(tmp_val, i + 1, 20);

		//�A�C�e����+����
		if(!o_ptr->k_idx)
		{
			c_put_str(TERM_L_DARK, "[�󂫃X���b�g]", i + 1, 22);
		}	
		else
		{
			class_power_aux = support_item_list[o_ptr->pval].func_aux;
			sprintf(o_name,"�w%s�x(%d)",support_item_list[o_ptr->pval].name,o_ptr->number);
			c_put_str(TERM_ORANGE, o_name, i + 1, 22);

			//�ǉ����
			if(show_info)
			{
				use_itemcard = TRUE;
				sprintf(tmp_val, "%s",class_power_aux(support_item_list[o_ptr->pval].func_num,TRUE) );
				put_str(tmp_val, i + 1, 70);
				use_itemcard = FALSE;
			}
		}
	}

	return ;
}

//�A�C�e���J�[�h��I����inven_special[1-8]�Ɋi�[�@�L�����Z�����Ȃ��Ƃ�TRUE
bool set_itemcard(void)
{
	int i;
	object_type forge;
	object_type *o_ptr,*q_ptr;
	int  item,amt;
	cptr q, s;
	int slot;
	char    o_name[MAX_NLEN];
	bool flag_change = FALSE;
	int		itemslot_max = calc_itemcard_slot_size(); //�A�C�e���J�[�h�X���b�g���� inven_special[]�Y���Ɋ֌W
	int cur_pos = 0;

	q_ptr = &forge;

	//�����̃X���b�g�̓A�C�e���J�[�h�ɂ����g��Ȃ��������v���O�����~�X��炩�����Ƃ��̂��߂Ƀ`�F�b�N���Ƃ�
	for(i=INVEN_SPECIAL_ITEM_SLOT_START;i<=itemslot_max;i++)
	{
		bool flag_err = FALSE;
		o_ptr = &inven_special[i];
		if(o_ptr->k_idx && o_ptr->tval != TV_ITEMCARD)
		{
			msg_format("ERROR:inven_special[%d]�ɃA�C�e���J�[�h�ȊO�������Ă���",i);
			flag_err = TRUE;
		}
		if(flag_err) return FALSE;
	}

	item_tester_tval = TV_ITEMCARD;
	q = "�ǂ̃J�[�h���Z�b�g���܂���? ";
	s = "�A�C�e���J�[�h�������Ă��Ȃ��B";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0)	o_ptr = &inventory[item];
	else o_ptr = &o_list[0 - item];

	screen_save();
	while(1)
	{
		char c;
		slot = 0; //inven_special[0]�͏��P�b��p�Ȃ̂Ŏg��Ȃ�
		print_itemcard_list(TRUE,cur_pos);
		prt("�ǂ̃X���b�g�ɃZ�b�g���܂����H",0,0);

		c = inkey();

		if (c == ESCAPE || c == ' ') break;

		if (use_menu)
		{
			if (c == 'x' || c == 'X' || c == '\r' || c == '\n')
			{
				slot = cur_pos + INVEN_SPECIAL_ITEM_SLOT_START;
				break;
			}
			else if (c == '0')
			{
				break;
			}
			else if (c == '8' || c == 'k' || c == 'K')
			{
				cur_pos--;
				if (cur_pos < 0) cur_pos = itemslot_max - 1;
			}
			else if (c == '2' || c == 'j' || c == 'J')
			{
				cur_pos++;
				if (cur_pos >= itemslot_max) cur_pos = 0;
			}

		}
		else
		{
			if (c >= 'a' && c <= ('a' + itemslot_max - INVEN_SPECIAL_ITEM_SLOT_START))
			{
				slot = c - 'a' + INVEN_SPECIAL_ITEM_SLOT_START;
				break;
			}
		}
	}
	screen_load();
	if(!slot) return FALSE;

	object_desc(o_name, o_ptr, OD_OMIT_INSCRIPTION);

	if(!inven_special[slot].k_idx)//�I�������X���b�g����
	{
		object_copy(&inven_special[slot], o_ptr);
		amt = o_ptr->number;
		msg_format("%s���J�[�h�X���b�g�ɃZ�b�g�����B",o_name);
	}
	else if(inven_special[slot].pval != o_ptr->pval)//�I�������X���b�g���ʎ�̏ꍇ����ւ���
	{
		object_copy(q_ptr, &inven_special[slot]);
		object_copy(&inven_special[slot], o_ptr);
		amt = o_ptr->number;
		msg_format("%s���J�[�h�X���b�g�ɓ���ւ����B",o_name);
		flag_change = TRUE;
	}
	else//�I�������X���b�g������̏ꍇ�����B99���𒴂����������邾�������
	{
		int sum = inven_special[slot].number + o_ptr->number;
		if(inven_special[slot].number == 99)
		{
			msg_print("���̃J�[�h�X���b�g�͊��Ɉ�t���B");
			return FALSE;
		}
		if(sum > 99)
		{
			amt = 99 - inven_special[slot].number;
			inven_special[slot].number = 99;
		}
		else
		{
			inven_special[slot].number += o_ptr->number;
			amt = o_ptr->number;
		}
		msg_format("%s���J�[�h�X���b�g�ɒǉ������B",o_name);
	}

	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		//inven_item_describe(item); ���b�Z�[�W�͏璷�Ȃ̂ŏo���Ȃ�
		inven_item_optimize(item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -amt);
	//	floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
	//����ւ��̏ꍇ�ޔ����Ă����Â��A�C�e�����U�b�N�ɓ���Ȃ���
	if(flag_change)
	{
		inven_carry(q_ptr);
	}

	return TRUE;
}


//�A�C�e���J�[�h�X���b�g������o���@�L�����Z�����Ȃ��Ƃ�TRUE
bool remove_itemcard(void)
{
	int i;
	object_type forge;
	object_type *o_ptr,*q_ptr;
	int slot;
	char    o_name[MAX_NLEN];
	int		itemslot_max = calc_itemcard_slot_size(); //�A�C�e���J�[�h�X���b�g���� inven_special[]�Y���Ɋ֌W

	int cur_pos = 0;
	q_ptr = &forge;

	screen_save();
	while(1)
	{
		char c;
		slot = 0; //inven_special[0]�͏��P�b��p�Ȃ̂Ŏg��Ȃ�
		print_itemcard_list(TRUE,cur_pos);
		prt("�ǂ̃X���b�g����J�[�h�����o���܂����H",0,0);

		c = inkey();

		if (c == ESCAPE || c == ' ') break;

		if (use_menu)
		{
			if (c == 'x' || c == 'X' || c == '\r' || c == '\n')
			{
				slot = cur_pos + INVEN_SPECIAL_ITEM_SLOT_START;
				break;
			}
			else if (c == '0')
			{
				break;
			}
			else if (c == '8' || c == 'k' || c == 'K')
			{
				cur_pos--;
				if (cur_pos < 0) cur_pos = itemslot_max - 1;
			}
			else if (c == '2' || c == 'j' || c == 'J')
			{
				cur_pos++;
				if (cur_pos >= itemslot_max) cur_pos = 0;
			}

		}
		else
		{
			if (c >= 'a' && c <= ('a' + itemslot_max - INVEN_SPECIAL_ITEM_SLOT_START))
			{
				slot = c - 'a' + INVEN_SPECIAL_ITEM_SLOT_START;
				break;
			}
		}
	}
	screen_load();
	if(!slot) return FALSE;

	if(!inven_special[slot].k_idx)//�I�������X���b�g����
	{
		msg_format("���̃X���b�g�͋󂾁B");
		return FALSE;
	}
	else
	{
		object_copy(q_ptr, &inven_special[slot]);
		object_desc(o_name, q_ptr, OD_OMIT_INSCRIPTION);
		object_wipe(&inven_special[slot]);
		msg_format("%s���J�[�h�X���b�g������o�����B",o_name);
		inven_carry(q_ptr);
	}

	return TRUE;
}


//�A�C�e���J�[�h���g�p����@�L�����Z�����Ȃ��Ƃ�TRUE
bool activate_itemcard(void)
{
	int i;
	object_type *o_ptr;
	int slot;
	char    o_name[MAX_NLEN];
	class_power_type *spell;
	cptr (*class_power_aux)( int, bool);
	int power_idx;
	int		itemslot_max = calc_itemcard_slot_size(); //�A�C�e���J�[�h�X���b�g���� inven_special[]�Y���Ɋ֌W
	int cur_pos = 0;

	if (!repeat_pull(&slot))
	{
		screen_save();
		while(1)
		{

			char c;
			slot = 0; //inven_special[0]�͏��P�b��p�Ȃ̂Ŏg��Ȃ�
			print_itemcard_list(TRUE, cur_pos);
			prt("�ǂ̃A�C�e���J�[�h���g�p���܂����H", 0, 0);

			c = inkey();

			if (c == ESCAPE || c == ' ') break;

			if (use_menu)
			{
				if (c == 'x' || c == 'X' || c == '\r' || c == '\n')
				{
					slot = cur_pos + INVEN_SPECIAL_ITEM_SLOT_START;
					break;
				}
				else if (c == '0')
				{
					break;
				}
				else if (c == '8' || c == 'k' || c == 'K')
				{
					cur_pos--;
					if (cur_pos < 0) cur_pos = itemslot_max - 1;
				}
				else if (c == '2' || c == 'j' || c == 'J')
				{
					cur_pos++;
					if (cur_pos >= itemslot_max) cur_pos = 0;
				}

			}
			else
			{
				if (c >= 'a' && c <= ('a' + itemslot_max - INVEN_SPECIAL_ITEM_SLOT_START))
				{
					slot = c - 'a' + INVEN_SPECIAL_ITEM_SLOT_START;
					break;
				}
			}
		}
		repeat_push(slot);
		screen_load();
	}
	if(!slot) return FALSE;

	if(!inven_special[slot].k_idx)//�I�������X���b�g����
	{
		msg_format("���̃X���b�g�͋󂾁B");
		return FALSE;
	}
	else
	{
		bool flag_cancel = FALSE;
		o_ptr = &inven_special[slot];
		power_idx = o_ptr->pval;

		class_power_aux = support_item_list[power_idx].func_aux;
		spell = &(support_item_list[power_idx].func_table[support_item_list[power_idx].func_num]);
		msg_format("�u%s�v�𔭓������I",spell->name);
		use_itemcard = TRUE;
		if(!class_power_aux(support_item_list[power_idx].func_num,FALSE)) flag_cancel = TRUE;
		use_itemcard = FALSE;

		if(flag_cancel) return FALSE;

		o_ptr->number -= 1;
		if(!o_ptr->number) object_wipe(&inven_special[slot]);
	}

	//�s���͕ω������ϐ������Z�b�g
	new_class_power_change_energy_need = 0;
	return TRUE;
}



//�Z�b�g�����A�C�e���J�[�h��ǂށB�s���͂�����Ȃ��B
void read_itemcard(void)
{
	int i;
	int slot;
	char    o_name[MAX_NLEN];
	int		itemslot_max = calc_itemcard_slot_size(); //�A�C�e���J�[�h�X���b�g���� inven_special[]�Y���Ɋ֌W

	int cur_pos = 0;

	screen_save();
	while(1)
	{
		char c;
		slot = 0; //inven_special[0]�͏��P�b��p�Ȃ̂Ŏg��Ȃ�
		print_itemcard_list(TRUE,cur_pos);
		prt("�ǂ̃X���b�g�̃J�[�h��ǂ݂܂����H",0,0);

		c = inkey();

		if(c == ESCAPE || c == ' ') break;

		if (use_menu)
		{
			if (c == 'x' || c == 'X' || c == '\r' || c == '\n')
			{
				slot = cur_pos + INVEN_SPECIAL_ITEM_SLOT_START;
				break;
			}
			else if (c == '0')
			{
				break;
			}
			else if (c == '8' || c == 'k' || c == 'K')
			{
				cur_pos--;
				if (cur_pos < 0) cur_pos = itemslot_max-1;
			}
			else if (c == '2' || c == 'j' || c == 'J')
			{
				cur_pos++;
				if (cur_pos >= itemslot_max) cur_pos = 0;
			}

		}
		else
		{
			if (c >= 'a' && c <= ('a' + itemslot_max - INVEN_SPECIAL_ITEM_SLOT_START))
			{
				slot = c - 'a' + INVEN_SPECIAL_ITEM_SLOT_START;
				break;
			}
		}

	}
	screen_load();
	if(!slot) return;

	if(!inven_special[slot].k_idx)//�I�������X���b�g����
	{
		msg_format("���̃X���b�g�͋󂾁B");
		return;
	}
	else
	{
		screen_object(&inven_special[slot],SCROBJ_FORCE_DETAIL);
	}

	return;
}










