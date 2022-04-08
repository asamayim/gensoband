/* File: wild.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke,
 * Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Wilderness generation */

#include "angband.h"



static void set_floor_and_wall_aux(s16b feat_type[100], feat_prob prob[DUNGEON_FEAT_PROB_NUM])
{
	int lim[DUNGEON_FEAT_PROB_NUM], cur = 0, i;

	/*:::d_info.txt�̋L�q���@�������ς��ď�OR�ǂ̒n�`�̎�ނ̏o�₷����z��Ɋi�[*/
	lim[0] = prob[0].percent;
	for (i = 1; i < DUNGEON_FEAT_PROB_NUM; i++) lim[i] = lim[i - 1] + prob[i].percent;

	/* Paranoia */
	if (lim[DUNGEON_FEAT_PROB_NUM - 1] < 100) lim[DUNGEON_FEAT_PROB_NUM - 1] = 100;

	for (i = 0; i < 100; i++)
	{
		/*:::�O���[�o���ϐ�floor_type��fill_type�Ƀ_���W�����̎�ނɂ��n�`�o�����ɉ����Ēn�`�f�[�^��100��i�[���Ă���B*/
		while (i == lim[cur]) cur++;
		feat_type[i] = prob[cur].feat;
	}
}

/*
 * Fill the arrays of floors and walls in the good proportions
 */
/*:::�V�K����0,����ȊO�̂Ƃ���dungeon_type�������̂͂�*/
void set_floor_and_wall(byte type)
{
	static byte cur_type = 255;/*:::���̐��l�͊֐����I�����Ă��ێ������*/
	dungeon_info_type *d_ptr;

	//EX���[�h�̂Ƃ����܂Ƀt���A�̕ǂ⏰�̐����K�����㏑������.�N�G�X�g�t���A�ł͂��Ȃ�
	//��d�ɌĂ΂��悤�Ȃ̂őO��ł���Ȃ��悤�ςȋ[�������ɂ���
	if(EXTRA_MODE && dun_level % 10 && type == DUNGEON_ANGBAND && !((dun_level + p_ptr->total_weight)%12))
	{
		int dev = 5 + dun_level / 20;
		if(dev > 9) dev = 9;
		cur_type = 255;

		if(cheat_room) msg_print("EX���[�h���ꏈ���F�_���W���������K���㏑��");

		switch((dun_level + p_ptr->pclass + p_ptr->prace)%dev + 1)
		{
		case 1:
		case 2:
			d_ptr = &d_info[DUNGEON_FOREST];
			break;
		case 3:
		case 4:
			d_ptr = &d_info[DUNGEON_MUEN];
			break;
		case 5:
			d_ptr = &d_info[DUNGEON_GENBU];
			break;
		case 6://�ǑS���X
			{
				feat_prob tmp_fill[3] = {{feat_tree,100},{0,0},{0,0}};

				d_ptr = &d_info[type];
				set_floor_and_wall_aux(floor_type, d_ptr->floor);
				set_floor_and_wall_aux(fill_type, tmp_fill);
				feat_wall_outer = feat_tree;
				feat_wall_inner = feat_tree;
				feat_wall_solid = feat_tree;
				return;
			}
		case 7://�ǑS���[����
			{
				feat_prob tmp_fill[3] = {{feat_deep_water,80},{feat_shallow_water,20},{0,0}};

				d_ptr = &d_info[type];
				set_floor_and_wall_aux(floor_type, d_ptr->floor);
				set_floor_and_wall_aux(fill_type, tmp_fill);
				feat_wall_outer = feat_deep_water;
				feat_wall_inner = feat_deep_water;
				feat_wall_solid = feat_deep_water;
				return;
			}
		case 8://�ǑS���n��
			{
				feat_prob tmp_fill[3] = {{feat_deep_lava,80},{feat_shallow_lava,20},{0,0}};

				d_ptr = &d_info[type];
				set_floor_and_wall_aux(floor_type, d_ptr->floor);
				set_floor_and_wall_aux(fill_type, tmp_fill);
				feat_wall_outer = feat_deep_lava;
				feat_wall_inner = feat_deep_lava;
				feat_wall_solid = feat_deep_lava;
				return;
			}
		case 9://�ǑS���[����
			{
				feat_prob tmp_fill[3] = {{feat_dark_pit,100},{0,0},{0,0}};

				d_ptr = &d_info[type];
				set_floor_and_wall_aux(floor_type, d_ptr->floor);
				set_floor_and_wall_aux(fill_type, tmp_fill);
				feat_wall_outer = feat_dark_pit;
				feat_wall_inner = feat_dark_pit;
				feat_wall_solid = feat_dark_pit;
				return;
			}
		default:
			break;

		}
		set_floor_and_wall_aux(floor_type, d_ptr->floor);
		set_floor_and_wall_aux(fill_type, d_ptr->fill);

		feat_wall_outer = d_ptr->outer_wall;
		feat_wall_inner = d_ptr->inner_wall;
		feat_wall_solid = d_ptr->outer_wall;
		return;
	}

	/* Already filled */
	if (cur_type == type) return;

	cur_type = type;
	/*:::����d_info��winmain�̉���init_angband��d_info_j.raw����ǂݍ��܂ꂽ�炵��*/
	d_ptr = &d_info[type];

	/*:::�O���[�o���ϐ�floor_type��fill_type�Ƀ_���W�����̎�ނɂ��n�`�o�����ɉ����Ēn�`�f�[�^��100��i�[���Ă���B*/
	set_floor_and_wall_aux(floor_type, d_ptr->floor);
	set_floor_and_wall_aux(fill_type, d_ptr->fill);

	feat_wall_outer = d_ptr->outer_wall;
	feat_wall_inner = d_ptr->inner_wall;
	feat_wall_solid = d_ptr->outer_wall;
}


/*
 * Helper for plasma generation.
 */
static void perturb_point_mid(int x1, int x2, int x3, int x4,
			  int xmid, int ymid, int rough, int depth_max)
{
	/*
	 * Average the four corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough*2 + 1;
	int tmp = randint1(tmp2) - (rough + 1);

	int avg = ((x1 + x2 + x3 + x4) / 4) + tmp;

	/* Division always rounds down, so we round up again */
	if (((x1 + x2 + x3 + x4) % 4) > 1)
		avg++;

	/* Normalize */
	if (avg < 0) avg = 0;
	if (avg > depth_max) avg = depth_max;

	/* Set the new value. */
	cave[ymid][xmid].feat = avg;
}


static void perturb_point_end(int x1, int x2, int x3,
			  int xmid, int ymid, int rough, int depth_max)
{
	/*
	 * Average the three corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough * 2 + 1;
	int tmp = randint0(tmp2) - rough;

	int avg = ((x1 + x2 + x3) / 3) + tmp;

	/* Division always rounds down, so we round up again */
	if ((x1 + x2 + x3) % 3) avg++;

	/* Normalize */
	if (avg < 0) avg = 0;
	if (avg > depth_max) avg = depth_max;

	/* Set the new value. */
	cave[ymid][xmid].feat = avg;
}


/*
 * A generic function to generate the plasma fractal.
 * Note that it uses ``cave_feat'' as temporary storage.
 * The values in ``cave_feat'' after this function
 * are NOT actual features; They are raw heights which
 * need to be converted to features.
 */
static void plasma_recursive(int x1, int y1, int x2, int y2,
			     int depth_max, int rough)
{
	/* Find middle */
	int xmid = (x2 - x1) / 2 + x1;
	int ymid = (y2 - y1) / 2 + y1;

	/* Are we done? */
	if (x1 + 1 == x2) return;

	perturb_point_mid(cave[y1][x1].feat, cave[y2][x1].feat, cave[y1][x2].feat,
		cave[y2][x2].feat, xmid, ymid, rough, depth_max);

	perturb_point_end(cave[y1][x1].feat, cave[y1][x2].feat, cave[ymid][xmid].feat,
		xmid, y1, rough, depth_max);

	perturb_point_end(cave[y1][x2].feat, cave[y2][x2].feat, cave[ymid][xmid].feat,
		x2, ymid, rough, depth_max);

	perturb_point_end(cave[y2][x2].feat, cave[y2][x1].feat, cave[ymid][xmid].feat,
		xmid, y2, rough, depth_max);

	perturb_point_end(cave[y2][x1].feat, cave[y1][x1].feat, cave[ymid][xmid].feat,
		x1, ymid, rough, depth_max);


	/* Recurse the four quadrants */
	plasma_recursive(x1, y1, xmid, ymid, depth_max, rough);
	plasma_recursive(xmid, y1, x2, ymid, depth_max, rough);
	plasma_recursive(x1, ymid, xmid, y2, depth_max, rough);
	plasma_recursive(xmid, ymid, x2, y2, depth_max, rough);
}


#define MAX_FEAT_IN_TERRAIN 18

/*
 * The default table in terrain level generation.
 */
static s16b terrain_table[MAX_WILDERNESS][MAX_FEAT_IN_TERRAIN];

/*:::�r��p�����_���n�`�������[�`��*/
static void generate_wilderness_area(int terrain, u32b seed, bool border, bool corner)
{
	int x1, y1;
	int table_size = sizeof(terrain_table[0]) / sizeof(s16b);
	int roughness = 1; /* The roughness of the level. */
	u32b state_backup[4];

	/* Unused */
	(void)border;

	/* The outer wall is easy */
	if (terrain == TERRAIN_EDGE)
	{
		/* Create level background */
		for (y1 = 0; y1 < MAX_HGT; y1++)
		{
			for (x1 = 0; x1 < MAX_WID; x1++)
			{
				cave[y1][x1].feat = feat_permanent;
			}
		}

		/* We are done already */
		return;
	}


	/* Hack -- Backup the RNG state */
	Rand_state_backup(state_backup);

	/* Hack -- Induce consistant flavors */
	Rand_state_init(seed);

	if (!corner)
	{
		/* Create level background */
		for (y1 = 0; y1 < MAX_HGT; y1++)
		{
			for (x1 = 0; x1 < MAX_WID; x1++)
			{
				cave[y1][x1].feat = table_size / 2;
			}
		}
	}

	/*
	 * Initialize the four corners
	 * ToDo: calculate the medium height of the adjacent
	 * terrains for every corner.
	 */
	cave[1][1].feat = randint0(table_size);
	cave[MAX_HGT-2][1].feat = randint0(table_size);
	cave[1][MAX_WID-2].feat = randint0(table_size);
	cave[MAX_HGT-2][MAX_WID-2].feat = randint0(table_size);

	if (!corner)
	{
		/* Hack -- preserve four corners */
		s16b north_west = cave[1][1].feat;
		s16b south_west = cave[MAX_HGT - 2][1].feat;
		s16b north_east = cave[1][MAX_WID - 2].feat;
		s16b south_east = cave[MAX_HGT - 2][MAX_WID - 2].feat;

		/* x1, y1, x2, y2, num_depths, roughness */
		plasma_recursive(1, 1, MAX_WID-2, MAX_HGT-2, table_size-1, roughness);

		/* Hack -- copyback four corners */
		cave[1][1].feat = north_west;
		cave[MAX_HGT - 2][1].feat = south_west;
		cave[1][MAX_WID - 2].feat = north_east;
		cave[MAX_HGT - 2][MAX_WID - 2].feat = south_east;

		for (y1 = 1; y1 < MAX_HGT - 1; y1++)
		{
			for (x1 = 1; x1 < MAX_WID - 1; x1++)
			{
				cave[y1][x1].feat = terrain_table[terrain][cave[y1][x1].feat];
			}
		}
	}
	else /* Hack -- only four corners */
	{
		cave[1][1].feat = terrain_table[terrain][cave[1][1].feat];
		cave[MAX_HGT - 2][1].feat = terrain_table[terrain][cave[MAX_HGT - 2][1].feat];
		cave[1][MAX_WID - 2].feat = terrain_table[terrain][cave[1][MAX_WID - 2].feat];
		cave[MAX_HGT - 2][MAX_WID - 2].feat = terrain_table[terrain][cave[MAX_HGT - 2][MAX_WID - 2].feat];
	}

	/* Hack -- Restore the RNG state */
	Rand_state_restore(state_backup);
}



/*
 * Load a town or generate a terrain level using "plasma" fractals.
 *
 * x and y are the coordinates of the area in the wilderness.
 * Border and corner are optimization flags to speed up the
 * generation of the fractal terrain.
 * If border is set then only the border of the terrain should
 * be generated (for initializing the border structure).
 * If corner is set then only the corners of the area are needed.
 */
/*:::���[���h�}�b�v��Y,X�̈ʒu�̍r��n�`���𐶐�����B*/
/*:::border��corner�͒[������������i�ׂ̃}�b�v��`�悷��Ƃ��ɍŊO���Ɏg���j*/
/*:::p_ptr->town_num�̑���Ȃǈꕔ�ϐ������������ōs���Ă���i�܂�}�b�v���ӂ��ɍ��Ȃ��Ƃ����Ȃ��H)*/

static void generate_area(int y, int x, bool border, bool corner)
{
	int x1, y1;

	/* Number of the town (if any) */
	p_ptr->town_num = wilderness[y][x].town;

	/* Set the base level */
	base_level = wilderness[y][x].level;

	/* Set the dungeon level */
	dun_level = 0;

	/* Set the monster generation level */
	monster_level = base_level;

	/* Set the object generation level */
	object_level = base_level;


	/* Create the town */
	/*:::����wilderness[][].town�ɐݒ肳�ꂽ�ꏊ�ɂ���Ƃ�*/
	if (p_ptr->town_num)
	{
		/* Reset the buildings */
		/*:::building[]������������*/
		init_buildings();

		/* Initialize the town */
		if (border | corner)
			init_flags = INIT_CREATE_DUNGEON | INIT_ONLY_FEATURES;
		else
			init_flags = INIT_CREATE_DUNGEON;
		/*:::�X�̒n�`��cave[][]�Ɋi�[*/
		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);
		/*:::�K�ꂽ���̂���X�t���O�𗧂Ă�*/
		if (!corner && !border) p_ptr->visit |= (1L << (p_ptr->town_num - 1));
	}
	else
	{
		int terrain = wilderness[y][x].terrain;
		u32b seed = wilderness[y][x].seed;
		/*:::�r��n�`�����_������*/
		generate_wilderness_area(terrain, seed, border, corner);
	}
	/*:::���̔z�u*/
	if (!corner && !wilderness[y][x].town)
	{
		/*
		 * Place roads in the wilderness
		 * ToDo: make the road a bit more interresting
		 */
		if (wilderness[y][x].road)
		{
			cave[MAX_HGT/2][MAX_WID/2].feat = feat_floor;

			if (wilderness[y-1][x].road)
			{
				/* North road */
				for (y1 = 1; y1 < MAX_HGT/2; y1++)
				{
					x1 = MAX_WID/2;
					cave[y1][x1].feat = feat_floor;
				}
			}

			if (wilderness[y+1][x].road)
			{
				/* North road */
				for (y1 = MAX_HGT/2; y1 < MAX_HGT - 1; y1++)
				{
					x1 = MAX_WID/2;
					cave[y1][x1].feat = feat_floor;
				}
			}

			if (wilderness[y][x+1].road)
			{
				/* East road */
				for (x1 = MAX_WID/2; x1 < MAX_WID - 1; x1++)
				{
					y1 = MAX_HGT/2;
					cave[y1][x1].feat = feat_floor;
				}
			}

			if (wilderness[y][x-1].road)
			{
				/* West road */
				for (x1 = 1; x1 < MAX_WID/2; x1++)
				{
					y1 = MAX_HGT/2;
					cave[y1][x1].feat = feat_floor;
				}
			}
		}
	}
	/*:::�_���W�����ւ̊K�i��z�u*/
	if (wilderness[y][x].entrance && !wilderness[y][x].town && (p_ptr->total_winner || !(d_info[wilderness[y][x].entrance].flags1 & DF1_WINNER)))
	{
		int dy, dx;
		u32b state_backup[4];

		/* Hack -- Backup the RNG state */
		Rand_state_backup(state_backup);

		/* Hack -- Induce consistant flavors */
		Rand_state_init(wilderness[y][x].seed);

		dy = rand_range(6, cur_hgt - 6);
		dx = rand_range(6, cur_wid - 6);

		cave[dy][dx].feat = feat_entrance;
		cave[dy][dx].special = wilderness[y][x].entrance;

		/* Hack -- Restore the RNG state */
		Rand_state_restore(state_backup);
	}
}


/*
 * Border of the wilderness area
 */
static border_type border;


/*
 * Build the wilderness area outside of the town.
 */
/*:::�X�ƒn��n�`�𐶐�*/
void wilderness_gen(void)
{
	int i, y, x, lim;
	cave_type *c_ptr;
	feature_type *f_ptr;

	/* Big town */
	cur_hgt = MAX_HGT;
	cur_wid = MAX_WID;

	/* Assume illegal panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Init the wilderness */
	/*:::wilderness[][]�ɒn�`�f�[�^������*/
	process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);

	x = p_ptr->wilderness_x;
	y = p_ptr->wilderness_y;

	//v1.1.32 ����X�̃����X�^�[��������̂��߁A���̎��_��p_ptr->town_num�����������邱�Ƃɂ���
	p_ptr->town_num = wilderness[y][x].town;

	/* Prepare allocation table */
	/*:::�r��̒n�`�ɍ��킹��hook��ύX�������X�^�[��������*/
	get_mon_num_prep(get_monster_hook(), NULL);

	/* North border */
	generate_area(y - 1, x, TRUE, FALSE);

	for (i = 1; i < MAX_WID - 1; i++)
	{
		border.north[i] = cave[MAX_HGT - 2][i].feat;
	}

	/* South border */
	generate_area(y + 1, x, TRUE, FALSE);

	for (i = 1; i < MAX_WID - 1; i++)
	{
		border.south[i] = cave[1][i].feat;
	}

	/* West border */
	generate_area(y, x - 1, TRUE, FALSE);

	for (i = 1; i < MAX_HGT - 1; i++)
	{
		border.west[i] = cave[i][MAX_WID - 2].feat;
	}

	/* East border */
	generate_area(y, x + 1, TRUE, FALSE);

	for (i = 1; i < MAX_HGT - 1; i++)
	{
		border.east[i] = cave[i][1].feat;
	}

	/* North west corner */
	generate_area(y - 1, x - 1, FALSE, TRUE);
	border.north_west = cave[MAX_HGT - 2][MAX_WID - 2].feat;

	/* North east corner */
	generate_area(y - 1, x + 1, FALSE, TRUE);
	border.north_east = cave[MAX_HGT - 2][1].feat;

	/* South west corner */
	generate_area(y + 1, x - 1, FALSE, TRUE);
	border.south_west = cave[1][MAX_WID - 2].feat;

	/* South east corner */
	generate_area(y + 1, x + 1, FALSE, TRUE);
	border.south_east = cave[1][1].feat;


	/* Create terrain of the current area */
	/*:::������G���A�̒n�`��cave[][]�Ɋi�[�B�X�Ȃ�t*.txt �r��Ȃ�V�[�h�����Ƃɋ[��������������*/
	generate_area(y, x, FALSE, FALSE);

	/*:::���[���h�}�b�v�[�͉i�v�ǂɂ���*/
	/* Special boundary walls -- North */
	for (i = 0; i < MAX_WID; i++)
	{
		cave[0][i].feat = feat_permanent;
		cave[0][i].mimic = border.north[i];
	}

	/* Special boundary walls -- South */
	for (i = 0; i < MAX_WID; i++)
	{
		cave[MAX_HGT - 1][i].feat = feat_permanent;
		cave[MAX_HGT - 1][i].mimic = border.south[i];
	}

	/* Special boundary walls -- West */
	for (i = 0; i < MAX_HGT; i++)
	{
		cave[i][0].feat = feat_permanent;
		cave[i][0].mimic = border.west[i];
	}

	/* Special boundary walls -- East */
	for (i = 0; i < MAX_HGT; i++)
	{
		cave[i][MAX_WID - 1].feat = feat_permanent;
		cave[i][MAX_WID - 1].mimic = border.east[i];
	}

	/* North west corner */
	cave[0][0].mimic = border.north_west;

	/* North east corner */
	cave[0][MAX_WID - 1].mimic = border.north_east;

	/* South west corner */
	cave[MAX_HGT - 1][0].mimic = border.south_west;

	/* South east corner */
	cave[MAX_HGT - 1][MAX_WID - 1].mimic = border.south_east;

	/* Light up or darken the area */
	/*:::���͑S�G���A���邭�A��́E�E�U���n�`�����H�ڍוs��*/
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Get the cave grid */
			c_ptr = &cave[y][x];

			if (is_daytime())
			{
				/* Assume lit */
				c_ptr->info |= (CAVE_GLOW);

				/* Hack -- Memorize lit grids if allowed */
				if (view_perma_grids) c_ptr->info |= (CAVE_MARK);
			}
			else
			{
				/* Feature code (applying "mimic" field) */
				f_ptr = &f_info[get_feat_mimic(c_ptr)];

				/*:::�����u����Ă��炸�K�i�ł��Ȃ����͈Â�����*/
				if (!is_mirror_grid(c_ptr) && !have_flag(f_ptr->flags, FF_QUEST_ENTER) &&
				    !have_flag(f_ptr->flags, FF_ENTRANCE))
				{
					/* Assume dark */
					c_ptr->info &= ~(CAVE_GLOW);

					/* Darken "boring" features */
					if (!have_flag(f_ptr->flags, FF_REMEMBER))
					{
						/* Forget the grid */
						c_ptr->info &= ~(CAVE_MARK);
					}
				}
				else if (have_flag(f_ptr->flags, FF_ENTRANCE))
				{
					/* Assume lit */
					c_ptr->info |= (CAVE_GLOW);

					/* Hack -- Memorize lit grids if allowed */
					if (view_perma_grids) c_ptr->info |= (CAVE_MARK);
				}
			}
		}
	}

	//v1.1.32 ����
	//v1.1.91 �n�������
	if(teleport_town_mode)
	{
		int building_num;

		if (teleport_town_mode == TELE_TOWN_MODE_ROPEWAY)
		{
			//�o�Ă��錚���̔ԍ� t00000xx.txt�Q��
			if (p_ptr->town_num == TOWN_MORIYA)
				building_num = 1;
			else
				building_num = 11;
		}
		else
		{
			if (p_ptr->town_num == TOWN_CHITEI)
				building_num = 6;
			else
				building_num = 3;

		}


		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				c_ptr = &cave[y][x];
				f_ptr = &f_info[c_ptr->feat];

				if (have_flag(f_ptr->flags, FF_BLDG))
				{
					if (f_ptr->subtype == building_num) 
					{
						if (c_ptr->m_idx) delete_monster_idx(c_ptr->m_idx);
						p_ptr->oldpy = y;
						p_ptr->oldpx = x;
					}
				}
			}
		}

		teleport_town_mode = 0;
		p_ptr->teleport_town = FALSE;
	}
	/*:::�X�ړ��Œ��ɗ����ꍇ�H*/
	else if (p_ptr->teleport_town)
	{
		/*:::Hack �_�q���̂Ȃ��X�ֈړ����Ă����ꍇ�̃f�t�H���g�ʒu��ݒ�*/
		p_ptr->oldpy = 40;
		p_ptr->oldpx = 85;

		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				/* Get the cave grid */
				c_ptr = &cave[y][x];

				/* Seeing true feature code (ignore mimic) */
				f_ptr = &f_info[c_ptr->feat];

				/*:::�����ƂɌ����̓���������̈ʒu�Ƃ��A�����X�^�[��������폜���Ă���H*/
				if (have_flag(f_ptr->flags, FF_BLDG))
				{


					if ((f_ptr->subtype == 2) 
						|| ((p_ptr->town_num == TOWN_HITOZATO || p_ptr->town_num == TOWN_KOURIN) && (f_ptr->subtype == 3))
						|| (p_ptr->town_num == TOWN_KOUMA  && (f_ptr->subtype == 1)))
					{
						if (c_ptr->m_idx) delete_monster_idx(c_ptr->m_idx);
						p_ptr->oldpy = y;
						p_ptr->oldpx = x;
					}
				}
			}
		}
		p_ptr->teleport_town = FALSE;
	}
	/*:::�A�҂ŊX��r��ɗ����ꍇ�H�@�K�i�Ƀ����X�^�[��������폜���Ă���H*/
	else if (p_ptr->leaving_dungeon)
	{
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				/* Get the cave grid */
				c_ptr = &cave[y][x];

				if (cave_have_flag_grid(c_ptr, FF_ENTRANCE))
				{
					if (c_ptr->m_idx) delete_monster_idx(c_ptr->m_idx);
					p_ptr->oldpy = y;
					p_ptr->oldpx = x;
				}
			}
		}
		/*:::leaving_dungeon�łȂ�teleport_town�Ȃ̂��H*/
		p_ptr->teleport_town = FALSE;
	}
	/*:::����z�u*/
	player_place(p_ptr->oldpy, p_ptr->oldpx);
	/* p_ptr->leaving_dungeon = FALSE;*/

	lim = (generate_encounter==TRUE)?40:MIN_M_ALLOC_TN;

	/* Make some residents */
	for (i = 0; i < lim; i++)
	{
		u32b mode = 0;

		/*:::�r���generate_encounter(�P����?)�Ȃ烂���X�^�[����ɋN���Ă���B�����łȂ��Ȃ�1/2�ŋN���Ă���B�X�ł͐Q�Ă���H*/
		if (!(generate_encounter || (one_in_(2) && (!p_ptr->town_num))))
			mode |= PM_ALLOW_SLEEP;

		/* Make a resident */
		(void)alloc_monster(generate_encounter ? 0 : 3, mode);
	}

//	msg_format("chk:monster_level:%d", monster_level);

	if(generate_encounter) ambush_flag = TRUE;
	generate_encounter = FALSE;

	/* Fill the arrays of floors and walls in the good proportions */
	set_floor_and_wall(0);

	/* Set rewarded quests to finished */
	///quest
	/*:::���̕`��ŃN�G�X�g�����񍐍ς݂ɂ��A�C�e���������ς񂾂̂ŃN�G�X�g��Ԃ��I���ɂ��Ă���*/
	for (i = 0; i < max_quests; i++)
	{
		if (quest[i].status == QUEST_STATUS_REWARDED)
		{
			quest[i].status = QUEST_STATUS_FINISHED;
			if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
			set_deity_bias(DBIAS_REPUTATION, 2);
			set_deity_bias(DBIAS_COSMOS, 1);

			//���ׂ��N�G�X�g�B���y�i���e�B���󂯂邽�߂̃t���O
			if(p_ptr->pclass == CLASS_SEIJA && i != QUEST_HOME1 && i != QUEST_HOME2 && i != QUEST_HOME3) flag_seija_quest_comp = TRUE;
		}
	}
}


static s16b conv_terrain2feat[MAX_WILDERNESS];

/*
 * Build the wilderness area.
 * -DG-
 */
/*:::���[���h�}�b�v�𐶐����A���̒n�`����cave[][]�Ɋi�[*/
void wilderness_gen_small()
{
	int i, j;

	/* To prevent stupid things */
	/*:::���[���h�}�b�v�����O�ɔO�̂��ߑS�ĉi�v�ǂɂ��Ă���H�ǂ����Ȃ�place_solid_perm_bold()�ł��g���Ηǂ������Ȃ��񂾂�*/
	for (i = 0; i < MAX_WID; i++)
	for (j = 0; j < MAX_HGT; j++)
	{
		cave[j][i].feat = feat_permanent;
	}

	/* Init the wilderness */
	/*:::wilderness[][]�ɒn�`�f�[�^������*/
	process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);

	/* Fill the map */
	for (i = 0; i < max_wild_x; i++)
	for (j = 0; j < max_wild_y; j++)
	{

		if (wilderness[j][i].town)
		{
			// -HACK- �͓��̃o�U�[�����[���h�}�b�v���猩���Ȃ�����
			//v1.1.91 �������낻��B���X�ɂ��Ȃ��Ă��������B���܂胆�[�U�t�����h���[�ƌ����񂵌���ł�����قǉB��Ă͂��Ȃ�
			/*
			if (wilderness[j][i].town != TOWN_KAPPA)
			{
				cave[j][i].feat = feat_town;
			}
			else
			{
				cave[j][i].feat = conv_terrain2feat[wilderness[j][i].terrain];
			}
			*/

			cave[j][i].feat = feat_town;
			cave[j][i].special = wilderness[j][i].town;
		}
		else if (wilderness[j][i].road) cave[j][i].feat = feat_floor;
		else if (wilderness[j][i].entrance && (p_ptr->total_winner || !(d_info[wilderness[j][i].entrance].flags1 & DF1_WINNER)))
		{
			cave[j][i].feat = feat_entrance;
			cave[j][i].special = (byte)wilderness[j][i].entrance;
		}
		/*:::�n�`���C���f�b�N�X��ϊ����Ă���炵���B���̔z���init_terrain_table()�ŏ���������Ă���悤��*/
		else cave[j][i].feat = conv_terrain2feat[wilderness[j][i].terrain];

		cave[j][i].info |= (CAVE_GLOW | CAVE_MARK);
	}

	cur_hgt = (s16b) max_wild_y;
	cur_wid = (s16b) max_wild_x;

	if (cur_hgt > MAX_HGT) cur_hgt = MAX_HGT;
	if (cur_wid > MAX_WID) cur_wid = MAX_WID;

	/* Assume illegal panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Place the player */
	px = p_ptr->wilderness_x;
	py = p_ptr->wilderness_y;

	p_ptr->town_num = 0;
}


typedef struct wilderness_grid wilderness_grid;

struct wilderness_grid
{
	int		terrain;    /* Terrain type */
	int		town;       /* Town number */
	s16b	level;		/* Level of the wilderness */
	byte	road;       /* Road */
	char	name[32];	/* Name of the town/wilderness */
};


static wilderness_grid w_letter[255];


/*
 * Parse a sub-file of the "extra info"
 */
/*:::w_info.txt�����߂��镔���@x/y min/max : ���������}�b�v�̗̈�H x,y: �����ł�xmin��ymin�̒l���i�[����Ă���*/
///system �͓��̃o�U�[�Ƃ��g���قƂ��׍H����Ȃ炱�̕�
errr parse_line_wilderness(char *buf, int ymin, int xmin, int ymax, int xmax, int *y, int *x)
{
	int i, num;
	char *zz[33];

	/* Unused */
	(void)ymin;
	(void)ymax;

	/* Paranoia */
	if (!(buf[0] == 'W')) return (PARSE_ERROR_GENERIC);

	switch (buf[2])
	{
		/* Process "W:F:<letter>:<terrain>:<town>:<road>:<name> */

		/*:::W�FF(J)�F<letter>�F<terrain>�F<level>�F<town>�F<road>�F<name>*/
#ifdef JP
		/*:::E:�p��p�X*/
	case 'E':
		return 0;
	case 'F':
	case 'J':
#else
	case 'J':
		return 0;
	case 'F':
	case 'E':
#endif
	{
		if ((num = tokenize(buf+4, 6, zz, 0)) > 1)
		{
			int index = zz[0][0];
			
			if (num > 1)
				w_letter[index].terrain = atoi(zz[1]);
			else
				w_letter[index].terrain = 0;
			
			if (num > 2)
				w_letter[index].level = atoi(zz[2]);
			else
				w_letter[index].level = 0;
			
			if (num > 3)
				w_letter[index].town = atoi(zz[3]);
			else
				w_letter[index].town = 0;
			
			if (num > 4)
				w_letter[index].road = atoi(zz[4]);
			else
				w_letter[index].road = 0;
			
			if (num > 5)
				strcpy(w_letter[index].name, zz[5]);
			else
				w_letter[index].name[0] = 0;
		}
		else
		{
				/* Failure */
			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}
		
		break;
	}
	
	/* Process "W:D:<layout> */
	/* Layout of the wilderness */
	case 'D':
	{
		/* Acquire the text */
		char *s = buf+4;
		
		/* Length of the text */
		int len = strlen(s);
		
		for (*x = xmin, i = 0; ((*x < xmax) && (i < len)); (*x)++, s++, i++)
		{
			/*:::W:D:�̂��Ƃ̕�������ÂH����wilderness[][]��w_letter�̏����ʂ��Ă���B*/
			/*:::y�͏�[����n�܂�*/
			int idx = s[0];
			
			wilderness[*y][*x].terrain = w_letter[idx].terrain;
			
			wilderness[*y][*x].level = w_letter[idx].level;
			
			wilderness[*y][*x].town = w_letter[idx].town;
			
			wilderness[*y][*x].road = w_letter[idx].road;
			
			strcpy(town[w_letter[idx].town].name, w_letter[idx].name);
		}
		
		(*y)++;
		
		break;
	}
	
	/* Process "W:P:<x>:<y> - starting position in the wilderness */
	/*:::�r��ł̏����ʒu�����߂Ă�炵���B�Ӌ��̒��̍��W���������̃f�[�^�͎g���Ă���̂��H*/
	case 'P':
	{
		if ((p_ptr->wilderness_x == 0) &&
		    (p_ptr->wilderness_y == 0))
		{
			if (tokenize(buf+4, 2, zz, 0) == 2)
			{
				p_ptr->wilderness_y = atoi(zz[0]);
				p_ptr->wilderness_x = atoi(zz[1]);
				
				if ((p_ptr->wilderness_x < 1) ||
				    (p_ptr->wilderness_x > max_wild_x) ||
				    (p_ptr->wilderness_y < 1) ||
				    (p_ptr->wilderness_y > max_wild_y))
				{
					return (PARSE_ERROR_OUT_OF_BOUNDS);
				}
			}
			else
			{
				return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
			}
		}
		
		break;
	}
	
	default:
		/* Failure */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}
	
	/*:::d_info���Q�Ƃ��ă_���W�����ւ̊K�i�̂���n�_�Ƀ_���W�����C���f�b�N�X�����Ă���*/
	for (i = 1; i < max_d_idx; i++)
	{
		if (!d_info[i].maxdepth) continue;
		wilderness[d_info[i].dy][d_info[i].dx].entrance = i;
		/*:::�_���W�������X�ɏd�Ȃ��Ă�ꍇ�i�C�[�N�A�j�́A���̃}�X�̍r�샌�x�����_���W�����̍Œ჌�x���ɐݒ肷��H�@*/
		if (!wilderness[d_info[i].dy][d_info[i].dx].town)
			wilderness[d_info[i].dy][d_info[i].dx].level = d_info[i].mindepth;
	}

	/* Success */
	return (0);
}


/*
 * Generate the random seeds for the wilderness
 */
void seed_wilderness(void)
{
	int x, y;

	/* Init wilderness seeds */
	for (x = 0; x < max_wild_x; x++)
	{
		for (y = 0; y < max_wild_y; y++)
		{
			wilderness[y][x].seed = randint0(0x10000000);
			wilderness[y][x].entrance = 0;
		}
	}
}


/*
 * Pointer to wilderness_type
 */
typedef wilderness_type *wilderness_type_ptr;

/*
 * Initialize wilderness array
 */
/*:::�r��̏�����*/
errr init_wilderness(void)
{
	int i;

	/* Allocate the wilderness (two-dimension array) */
	C_MAKE(wilderness, max_wild_y, wilderness_type_ptr);
	C_MAKE(wilderness[0], max_wild_x * max_wild_y, wilderness_type);

	/* Init the other pointers */
	for (i = 1; i < max_wild_y; i++)
		wilderness[i] = wilderness[0] + i * max_wild_x;

	generate_encounter = FALSE;

	return 0;
}


static void init_terrain_table(int terrain, s16b feat_global, cptr fmt, ...)
{
	va_list vp;
	cptr    p;
	int     cur = 0;
	char    check = 'a';
	s16b    feat;
	int     num;

	/* Begin the varargs stuff */
	va_start(vp, fmt);

	/* Wilderness terrains on global map */
	conv_terrain2feat[terrain] = feat_global;

	/* Wilderness terrains on local map */
	for (p = fmt; *p; p++)
	{
		if (*p == check)
		{
			int lim;

			feat = (s16b)va_arg(vp, int);
			num = va_arg(vp, int);
			lim = cur + num;

			for (; (cur < lim) && (cur < MAX_FEAT_IN_TERRAIN); cur++)
				terrain_table[terrain][cur] = feat;
			if (cur >= MAX_FEAT_IN_TERRAIN) break;

			check++;
		}
		else /* Paranoia */
		{
			plog_fmt("Format error");
		}
	}

	/* Paranoia */
	if (cur < MAX_FEAT_IN_TERRAIN)
	{
		plog_fmt("Too few parameters");
	}

	/* End the varargs stuff */
	va_end(vp);
}


/*
 * Initialize arrays for wilderness terrains
 */
void init_wilderness_terrains(void)
{
	init_terrain_table(TERRAIN_EDGE, feat_permanent, "a",
		feat_permanent, MAX_FEAT_IN_TERRAIN);

	init_terrain_table(TERRAIN_TOWN, feat_town, "a",
		feat_floor, MAX_FEAT_IN_TERRAIN);

	init_terrain_table(TERRAIN_DEEP_WATER, feat_deep_water, "ab",
		feat_deep_water, 12,
		feat_shallow_water, MAX_FEAT_IN_TERRAIN - 12);

	init_terrain_table(TERRAIN_SHALLOW_WATER, feat_shallow_water, "abcde",
		feat_deep_water, 3,
		feat_shallow_water, 12,
		feat_floor, 1,
		feat_dirt, 1,
		feat_grass, MAX_FEAT_IN_TERRAIN - 17);

	init_terrain_table(TERRAIN_SWAMP, feat_swamp, "abcdef",
		feat_dirt, 2,
		feat_grass, 3,
		feat_tree, 1,
		feat_brake, 1,
		feat_shallow_water, 4,
		feat_swamp, MAX_FEAT_IN_TERRAIN - 11);

	init_terrain_table(TERRAIN_DIRT, feat_dirt, "abcdef",
		feat_floor, 3,
		feat_dirt, 10,
		feat_flower, 1,
		feat_brake, 1,
		feat_grass, 1,
		feat_tree, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_GRASS, feat_grass, "abcdef",
		feat_floor, 2,
		feat_dirt, 2,
		feat_grass, 9,
		feat_flower, 1,
		feat_brake, 2,
		feat_tree, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_TREES, feat_tree, "abcde",
		feat_floor, 2,
		feat_dirt, 1,
		feat_tree, 11,
		feat_brake, 2,
		feat_grass, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_DESERT, feat_dirt, "abc",
		feat_floor, 2,
		feat_dirt, 13,
		feat_grass, MAX_FEAT_IN_TERRAIN - 15);

	init_terrain_table(TERRAIN_SHALLOW_LAVA, feat_shallow_lava, "abc",
		feat_shallow_lava, 14,
		feat_deep_lava, 3,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 17);

	init_terrain_table(TERRAIN_DEEP_LAVA, feat_deep_lava, "abcd",
		feat_dirt, 3,
		feat_shallow_lava, 3,
		feat_deep_lava, 10,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 16);

	/*
	init_terrain_table(TERRAIN_MOUNTAIN, feat_mountain, "abcdef",
		feat_floor, 1,
		feat_brake, 1,
		feat_grass, 2,
		feat_dirt, 2,
		feat_tree, 2,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 8);
	*/
	//v1.1.78 �R�n�`�������ƐX�����ɂ���
	init_terrain_table(TERRAIN_MOUNTAIN, feat_mountain, "abcdef",
		feat_floor, 1,
		feat_brake, 2,
		feat_grass, 2,
		feat_dirt, 5,
		feat_tree, 3,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 13);

	//v1.1.32�ǉ��@����X�@�������̊֐���������Ă�̂��S�R�킩��Ȃ����ǊX�̏ꍇterrain_table���g��Ȃ��͂�������K���ł����v���Ǝv���B���Ԃ�B
	init_terrain_table(TERRAIN_SPECIAL_TOWN, feat_town, "a",
		feat_floor, MAX_FEAT_IN_TERRAIN);

}

/*:::���C���h���[�h�ƍr��}�b�v��؂�ւ���@�ڍז���*/
bool change_wild_mode(void)
{
	int i;
	bool have_pet = FALSE;

	/* It is in the middle of changing map */
	if (p_ptr->leaving) return FALSE;


	if (lite_town || vanilla_town)
	{
#ifdef JP
		msg_print("�r��Ȃ�ĂȂ��B");
#else
		msg_print("No global map.");
#endif
		return FALSE;
	}

	if (p_ptr->wild_mode)
	{
		/* Save the location in the global map */
		p_ptr->wilderness_x = px;
		p_ptr->wilderness_y = py;

		/* Give first move to the player */
		p_ptr->energy_need = 0;

		/* Go back to the ordinary map */
		p_ptr->wild_mode = FALSE;

		/* Leaving */
		p_ptr->leaving = TRUE;

		/* Succeed */
		return TRUE;
	}

///mod140402 �_���[�W���󂯂�n�`�ōL��}�b�v�ɓ��낤�Ƃ����Ƃ��L�����Z��
	{
		bool flag_danger = FALSE;

		if(wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain == TERRAIN_DEEP_WATER)
		{
			if(prace_is_(RACE_VAMPIRE) && !p_ptr->levitation && !p_ptr->can_swim) flag_danger = TRUE;
			else if(!p_ptr->resist_water && !p_ptr->levitation && p_ptr->total_weight > weight_limit() && !p_ptr->can_swim) flag_danger = TRUE;
		}
		if((wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain == TERRAIN_SHALLOW_LAVA || wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain == TERRAIN_DEEP_LAVA ) && p_ptr->immune_fire) flag_danger = TRUE;
		if(flag_danger)
		{
			msg_print("���̊댯�ȏꏊ����͍L��}�b�v�ɓ���Ȃ��I");
			return FALSE;
		}
	}


	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (is_pet(m_ptr) && i != p_ptr->riding) have_pet = TRUE;
		if (MON_CSLEEP(m_ptr)) continue;
		if (m_ptr->cdis > MAX_SIGHT) continue;
		if (!is_hostile(m_ptr)) continue;
#ifdef JP
		msg_print("�G�������߂��ɂ���Ƃ��͍L��}�b�v�ɓ���Ȃ��I");
#else
		msg_print("You cannot enter global map, since there is some monsters nearby!");
#endif
		energy_use = 0;
		return FALSE;
	}

	if (have_pet)
	{
#ifdef JP
		cptr msg = "�z����u���čL��}�b�v�ɓ���܂����H";
#else
		cptr msg = "Do you leave your pets behind? ";
#endif

		if (!get_check_strict(msg, CHECK_OKAY_CANCEL))
		{
			energy_use = 0;
			return FALSE;
		}
	}

	/* HACK */
	energy_use = 1000;

	if(SAKUYA_WORLD) energy_use = 0;

	/* Remember the position */
	p_ptr->oldpx = px;
	p_ptr->oldpy = py;

	/* Cancel hex spelling */
	if (hex_spelling_any()) stop_hex_spell_all();

	/* Cancel any special action */
	set_action(ACTION_NONE);

	/* Go into the global map */
	p_ptr->wild_mode = TRUE;

	/* Leaving */
	p_ptr->leaving = TRUE;

	/* Succeed */
	return TRUE;
}
