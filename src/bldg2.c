


#include "angband.h"


//新闘技場でランダムチーム編成のとき適当に決まるチーム名
const cptr battle_mon_random_team_name_list[BATTLE_MON_RANDOM_TEAM_NAME_NUM+1] =
{
	"(ダミー)",
	"《きのこ派》",
	"《たけのこ派》",
	"《チーム・即興編成》",
	"《非公式の会合》",
	"《どうしてこうなった？》",
	"《新たな絆》",
	"《闇鍋同好会》",
	"《秘密の趣味仲間》",
	"《厳正なる抽選の結果》",
	"《観客から適当に引っ張ってきた人達》",

	"《蕎麦屋で相席になった縁》",
	"《実は次の異変のボス》",
	"《宴会で意気投合》",
	"《万歳楽ファンクラブ》",
	"《里のカフェテリアの常連》",
	"《シナジーの発揮が課題》",
	"《初めてのオフ会》",
	"《能楽研究会》",
	"《マンネリ打破の試み》",
	"《異色コンセプト》",

	"《イージーカム・イージーゴー》",
	"《ドリームチーム》",
	"《サイケでヒップでバッドでゴー》",
	"《影のパイプ役》",
	"《全てを仕組んだ黒幕》",
	"《幻想郷式花一匁》",
	"《光の三妖怪の四人目以降》",
	"《妖怪の山オリエンテーリングチーム》",
	"《ファーストアルバム近日リリース！》",
	"《合成スペルカードの研究中》",

	"《謎の調査隊》",
	"《狐の界隈で(何じゃそりゃと)話題》",
	"《人妖百物語》",
	"《台風の影響でテンションが高い》",
	"《次の宴会の料理担当》",
	"《香霖堂パーティーゲーム常連》",
	"《人外のフォークロア》",
	"《夢の世界でタッグを組んだ仲》",
	"《納涼弾幕コンテスト》",
	"《博麗屋プレゼンツ》",

	"《一触即発の危機》",
	"《ひまわり不作異変(仮)対策本部》",
	"《神社が騒がしいから監視に来た》",

};

//新闘技場の固定チーム編成　最初の数字はチームが選定されるための最低生成レベル
const	battle_mon_special_team_type	battle_mon_special_team_list[BATTLE_MON_SPECIAL_TEAM_NUM+1] =
{
	{255,"(ダミー)",
		{0, 0, 0, 0, 0, 0, 0, 0}},
	{60,"『紅魔館』",
		{MON_REMY, MON_FLAN, MON_SAKUYA, MON_PATCHOULI, MON_MEIRIN, MON_KOAKUMA, MON_CHUPACABRA,0}},
	{50,"『アンノウン』",
		{MON_NUE, MON_RANDOM_UNIQUE_1, MON_RANDOM_UNIQUE_2, 0, 0, 0, 0, 0}},
	{75,"『最後のトリニティリリージョン』",
		{MON_REIMU, MON_BYAKUREN, MON_MIKO, 0, 0, 0, 0, 0}},
	{60,"『メイガスナイト』",
		{MON_MARISA, MON_PATCHOULI, MON_ALICE, MON_BYAKUREN, MON_NARUMI, 0, 0, 0}},
	{35,"『兎角同盟』",
		{MON_UDONGE, MON_TEWI, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2}},

	{60,"『付喪神連盟』",
		{MON_RAIKO, MON_BENBEN, MON_YATSUHASHI, MON_KOGASA, MON_KOKORO, 0, 0, 0}},
	{55,"『姉連合』",
		{MON_REMY, MON_LUNASA, MON_SHIZUHA, MON_SATORI, MON_BENBEN, MON_SHION_1, 0, 0}},
	{60,"『妹連合』",
		{MON_FLAN, MON_LYRICA, MON_MINORIKO, MON_KOISHI, MON_YATSUHASHI, MON_JYOON, 0, 0}},
	{20,"『幽霊楽団』",
		{MON_LUNASA, MON_MERLIN, MON_LYRICA, 0, 0, 0, 0, 0}},
	{0,"『鳥獣伎楽』",
		{MON_MYSTIA, MON_KYOUKO, 0, 0, 0, 0, 0, 0}},

	{50,"『白玉楼』",
		{MON_YUYUKO, MON_YOUMU, MON_BOU_REI, MON_G_BOU_REI, MON_LOST_SOUL, MON_LOST_SOUL, 0, 0}},
	{70,"『守矢神社』",
		{MON_KANAKO, MON_SUWAKO, MON_SANAE, 0, 0, 0, 0, 0}},
	{65,"『永遠亭』",
		{MON_KAGUYA, MON_EIRIN, MON_UDONGE, MON_TEWI, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, 0}},
	{30,"『ダブルスポイラー』",
		{MON_AYA, MON_HATATE, 0, 0, 0, 0, 0, 0}},
	{75,"『是非曲直庁』",
		{MON_EIKI, MON_KIZINTYOU, MON_KOMACHI, MON_DEATH_3, MON_DEATH_2, MON_DEATH_1, 0, 0}},

	{60,"『地霊殿』",
		{MON_SATORI, MON_KOISHI, MON_UTSUHO, MON_ORIN, 0, 0, 0, 0}},
	{80,"『命蓮寺』",
		{MON_SHOU, MON_BYAKUREN, MON_ICHIRIN, MON_MURASA, MON_KYOUKO, MON_NAZRIN, MON_NUE, MON_MAMIZOU}},
	{75,"『神霊廟』",
		{MON_MIKO, MON_FUTO, MON_TOZIKO, MON_SEIGA, MON_YOSHIKA, MON_D_SHINREI, MON_SENNIN2, MON_SENNIN1}},
	{15,"『河童労働組合』",
		{MON_NITORI, MON_KAPPA_ENGINEER, MON_KAPPA_ENGINEER, MON_KAPPA_OKURI, MON_KAPPA_OKURI, MON_KAPPA, MON_KAPPA, MON_KAPPA}},
	{0,"『サバイバルゲーム』",
		{MON_YAMAWARO, MON_YAMAWARO, MON_YAMAWARO, MON_YAMAWARO, MON_YAMAWARO, MON_YAMAWARO, 0, 0}},

	{10,"『ドールズウォー』",
		{MON_ALICE, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL}},
	{75,"『プロジェクトスミヨシ』",
		{MON_REIMU, MON_MARISA, MON_REMY, MON_SAKUYA, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, 0}},
	{50,"『華扇とゆかいな仲間たち』",
		{MON_KASEN, MON_INFANTDRAGON, MON_TIGER, MON_G_EAGLE, MON_RAIZYU, MON_TAMA, 0, 0}},
	{60,"『幻想の結界チーム』",
		{MON_REIMU, MON_YUKARI, 0, 0, 0, 0, 0, 0}},
	{30,"『禁呪の詠唱チーム』",
		{MON_MARISA, MON_ALICE, 0, 0, 0, 0, 0, 0}},

	{50,"『夢幻の妖魔チーム』",
		{MON_REMY, MON_SAKUYA, 0, 0, 0, 0, 0, 0}},
	{40,"『幽冥の住人チーム』",
		{MON_YUYUKO, MON_YOUMU, 0, 0, 0, 0, 0, 0}},
	{60,"『妖怪の山』",
		{MON_AYA, MON_HATATE, MON_MOMIZI, MON_G_TENGU, MON_HANA_TENGU, MON_KARASU_TENGU, MON_HAKUROU_TENGU, 0}},
	{80,"『妖怪おとぎ話』",
		{MON_HAKUSEN, MON_YUUTEN, MON_ENENRA, MON_TANUKI, MON_G_ONRYOU, MON_TSUKUMO_1, MON_ZYARYUU, MON_OROCHI}},
	{60,"『百鬼夜行の宴会』",
		{MON_MAMIZOU, MON_O_TANUKI, MON_TANUKI, MON_TANUKI, MON_TSUKUMO_1, MON_TSUKUMO_1, MON_TSUKUMO_1, MON_TSUKUMO_1}},

	{80,"『とっておきの魔法』",
		{MON_PATCHOULI, MON_PHILOSOPHER_STONE, MON_PHILOSOPHER_STONE, MON_PHILOSOPHER_STONE, MON_PHILOSOPHER_STONE, MON_PHILOSOPHER_STONE, 0, 0}},
	{75,"『天界』",
		{MON_TENSHI, MON_IKU, MON_SUIKA, MON_TENNIN, MON_TENNIN, 0, 0, 0}},
	{10,"『悪魔の家の裏方』",
		{MON_KOAKUMA, MON_HOHGOBLIN, MON_HOHGOBLIN, MON_HOHGOBLIN, MON_HOHGOBLIN, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID}},
	{60,"『斬ればわかる』",
		{MON_SAKUYA, MON_YOUMU, MON_MOMIZI, MON_SHINMYOUMARU, MON_TENSHI, MON_NEMUNO, MON_MAYUMI, 0}},
	{10,"『猫の棲む里』",
		{MON_CHEN, MON_NEKOMATA, MON_YOUKAI_CAT, MON_YOUKAI_CAT, MON_YAMANEKO, MON_YAMANEKO, MON_YAMANEKO, MON_YASE_CAT}},

	{20,"『蟲の知らせサービス』",
		{MON_WRIGGLE, MON_TARANTULA, MON_KILLER_STAG, MON_SWARM, MON_G_COCKROACH, MON_TUTUGAMUSHI, MON_G_W_CENTIPEDE, MON_KILLER_BEE}},
	{60,"『謎の呑み仲間』",
		{MON_SUIKA, MON_YUGI, MON_KASEN, 0, 0, 0, 0, 0}},
	{20,"『イーグルラヴィッツ』",
		{MON_RINGO, MON_SEIRAN, MON_EAGLE_RABBIT, MON_EAGLE_RABBIT, MON_EAGLE_RABBIT, MON_EAGLE_RABBIT, 0, 0}},
	{50,"『あたいの会』",
		{MON_KOMACHI, MON_ORIN, MON_CIRNO, MON_CLOWNPIECE, 0, 0, 0, 0}},
	{70,"『ウェルカムヘル』",
		{MON_HECATIA1, MON_HECATIA2, MON_HECATIA3, MON_JUNKO, MON_CLOWNPIECE, 0, 0, 0}},

	{40,"『石花見』",
		{MON_YUGI, MON_KISUME, MON_YAMAME, MON_PARSEE, MON_ORIN, MON_G_ONRYOU, MON_G_ONRYOU, 0}},
	{20,"『地霊殿の物言わぬ住人』",
		{MON_PANTHER, MON_BLACK_PANTHER, MON_KOMODO_DRAGON, MON_HASHIBIROKOU, MON_HELL_KARASU, MON_G_ONRYOU, MON_ONRYOU, 0}},
	{70,"『穢れ無き月の民』",
		{MON_SAGUME, MON_YORIHIME, MON_TOYOHIME, 0, 0, 0, 0, 0}},
	{20,"『どう見ても狸が化けている』",
		{MON_TANUKI, MON_TANUKI, MON_TANUKI, MON_TANUKI, 0, 0, 0, 0}},
	{50,"『ホリズムリバー』",
		{MON_RAIKO, MON_LUNASA, MON_MERLIN, MON_LYRICA, 0, 0, 0, 0}},

	{40,"『アットホームな職場です』",
		{MON_SAKUYA, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID}},
	{60,"『秘神御開帳』",
		{MON_OKINA, MON_SATONO, MON_MAI, 0, 0, 0, 0, 0}},
	{45,"『宇佐見菫子は三人いる！』",
		{ MON_SUMIREKO_2, MON_SUMIREKO_2, MON_SUMIREKO_2, 0, 0, 0, 0, 0 } },
	{ 60,"『古い知り合い同士』",
		{ MON_KASEN, MON_YUKARI, MON_OKINA, 0, 0, 0, 0, 0 } },

	{ 30,"『はなさかようせいさん』",
		{ MON_CLOWNPIECE, MON_3FAIRIES, MON_MARISA, 0, 0, 0, 0, 0 } },
	{ 45,"『無尽兵団』",
		{ MON_MAYUMI, MON_HANIWA_F2, MON_HANIWA_F2, MON_HANIWA_A2, MON_HANIWA_A2, MON_HANIWA_C2, MON_HANIWA_C2, 0 } },
	{ 70,"『月虹市場』",
		{ MON_CHIMATA, MON_MEGUMU, MON_MOMOYO, MON_TSUKASA, 0, 0, 0, 0 } },


};

/*:::battle_mon用　指定敵が別の敵に魔法攻撃したときの期待ダメージ*/
int	calc_battle_mon_spell_dam(int r_idx, int enemy_r_idx, int *spell_score_mult, int mode)
{
	int dam;
	int total_damage = 0;
	int spell_cnt = 0;
	int rlev;
	int rhp;
	bool powerful;
	monster_race *r_ptr =  &r_info[r_idx];
	monster_race *enemy_r_ptr =  &r_info[enemy_r_idx];
	int score_mult_plus = 0;

	rlev = r_ptr->level;
	if(rlev < 1) rlev = 1;
	//HP(ブレス威力計算用)は8割くらいで計算
	rhp = r_ptr->hdice * r_ptr->hside / ((r_ptr->flags1 & RF1_FORCE_MAXHP) ? 1:2) * 4 / 5;
	if(rhp < 1) rhp = 1;
	powerful = (r_ptr->flags2 & RF2_POWERFUL);
	

	


	if(r_ptr->flags4 & RF4_DANMAKU)
	{

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else dam = rlev;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_DISPEL)
	{
		if(enemy_r_ptr->flags6 & (RF6_HASTE)) score_mult_plus += 30;
		if(enemy_r_ptr->flags6 & (RF6_INVULNER)) score_mult_plus += 50;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_ROCKET)
	{
		dam = rhp / 4;
		if(dam > 800) dam = 800;
		if(enemy_r_ptr->flagsr & RFR_RES_SHAR) dam /= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_SHOOT) //特殊射撃は計算しない
	{
		dam = r_ptr->blow[0].d_dice * (r_ptr->blow[0].d_side+1) / 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_HOLY)
	{
		dam = rhp / 6;
		if(dam > 500) dam = 500;
		if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
		else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_HELL)
	{
		dam = rhp / 6;
		if(dam > 500) dam = 500;
		if (enemy_r_ptr->flagsr & RFR_HURT_HELL) dam *= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_AQUA)
	{
		dam = rhp / 8;
		if(dam > 350) dam = 350;
		if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
		else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;
		if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
			score_mult_plus += 10;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_ACID)
	{
		dam = rhp / 3;
		if(dam > 1600) dam = 1600;
		if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_ELEC)
	{
		dam = rhp / 3;
		if(dam > 1600) dam = 1600;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_ELEC) dam *= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_FIRE)
	{
		dam = rhp / 3;
		if(dam > 1600) dam = 1600;
		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_COLD)
	{
		dam = rhp / 3;
		if(dam > 1600) dam = 1600;
		if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_POIS)
	{
		dam = rhp / 3;
		if(dam > 800) dam = 800;
		if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 9;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_NETH)
	{
		dam = rhp / 6;
		if(dam > 550) dam = 550;
		if (enemy_r_ptr->flags3 & RF3_UNDEAD) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_LITE)
	{
		dam = rhp / 6;
		if(dam > 400) dam = 400;
		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 5;
		else if (enemy_r_ptr->flagsr & RFR_HURT_LITE) dam *= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_DARK)
	{
		dam = rhp / 6;
		if(dam > 400) dam = 400;
		if (enemy_r_ptr->flagsr & RFR_RES_DARK) dam /= 5;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_WAVEMOTION)
	{
		dam = rhp / 6;
		if(dam > 555) dam = 555;
	
		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags4 & RF4_BR_SOUN)
	{
		dam = rhp / 6;
		if(dam > 450) dam = 450;
		if (enemy_r_ptr->flagsr & RFR_RES_SOUN) dam /= 5;
		else if (!(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
			score_mult_plus += 10;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_CHAO)
	{
		dam = rhp / 6;
		if(dam > 600) dam = 600;
		if (enemy_r_ptr->flagsr & RFR_RES_CHAO) dam /= 3;
		else if (!(enemy_r_ptr->flags3 & RF3_NO_CONF)) 
			score_mult_plus += 50;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_DISE)
	{
		dam = rhp / 6;
		if(dam > 500) dam = 500;
		if (enemy_r_ptr->flagsr & RFR_RES_DISE) dam /= 3;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_NEXU)
	{
		dam = rhp / 7;
		if(dam > 600) dam = 600;
		if (enemy_r_ptr->flagsr & RFR_RES_TIME) dam /= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_TIME)
	{
		dam = rhp / 3;
		if(dam > 150) dam = 150;
		if (enemy_r_ptr->flagsr & RFR_RES_TIME) dam /= 3;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_INER)
	{
		dam = rhp / 6;
		if(dam > 200) dam = 200;
		if (enemy_r_ptr->flagsr & RFR_RES_INER) dam /= 3;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_GRAV)
	{
		dam = rhp / 3;
		if(dam > 200) dam = 200;
		if (enemy_r_ptr->flagsr & RFR_RES_TIME || enemy_r_ptr->flags7 & RF7_CAN_FLY) dam /= 3;
		else if (enemy_r_ptr->flags2 & RF2_GIGANTIC) dam *= 2;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_SHAR)
	{
		dam = rhp / 6;
		if(dam > 500) dam = 500;
		if (enemy_r_ptr->flagsr & RFR_RES_SHAR) dam /= 3;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_PLAS)
	{
		dam = rhp / 4;
		if(dam > 1200) dam = 1200;

		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 3;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BA_FORCE)
	{
		if(powerful)
			dam = ((rhp / 4) > 350 ? 350 : (rhp / 4));
		else
			dam = ((rhp / 6) > 250 ? 250 : (rhp / 6));

		if (!(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
			score_mult_plus += 20;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_MANA)
	{
		dam = rhp / 3;
		if(dam > 250) dam = 250;
	
		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags4 & RF4_SPECIAL2) //フラン特技以外は発生抑止する予定なのでカウントしない
	{
		if(r_idx == MON_FLAN) 
			dam = 1600; //フラン特技
		else
			dam = 0;
	
		total_damage += dam;
		if(dam) spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_NUKE)
	{
		dam = rhp / 4;
		if(dam > 1000) dam = 1000;

		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 3;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BA_CHAO)
	{
		dam = rlev*2 + 50;

		if (enemy_r_ptr->flagsr & RFR_RES_CHAO) dam /= 3;
		else if (!(enemy_r_ptr->flags3 & RF3_NO_CONF)) 
			score_mult_plus += 70;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_DISI)
	{
		dam = rhp / 6;
		if(dam > 150) dam = 150;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_ACID)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 3 / 2 + 15;
		if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_ELEC)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 3 / 4 + 8;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_ELEC) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_FIRE)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 7 / 4 + 10;
		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_COLD)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 3 / 4 + 10;
		if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_POIS)
	{
		if(powerful) dam = 36;
		else dam = 18;
		if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_NETH)
	{
		if(powerful) dam = rlev * 2 + 100;
		else dam = rlev + 100;
		if (enemy_r_ptr->flags3 & RF3_UNDEAD) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_WATE)
	{
		if(powerful) dam = rlev * 3 / 2 + 50;
		else dam = rlev + 50;

		if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
		else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;
		if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
			score_mult_plus += 10;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_MANA)
	{
		dam = rlev * 4  + 100;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_DARK)
	{
		dam = rlev * 4  + 100;
		if (enemy_r_ptr->flagsr & RFR_RES_DARK) dam /= 5;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_DRAIN_MANA)
	{
		dam = rlev / 2 ; //rlev/4くらいだが回復があるのでちょっと上乗せ
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_MIND_BLAST)
	{
		dam = 25;
		if(enemy_r_ptr->flags1 & RF1_UNIQUE || enemy_r_ptr->flags3 & RF3_NO_CONF ||
			(enemy_r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))) dam = 0;
		else 
		{
			if(rlev > 10) dam /= rlev / 10;
			if(rlev < 20) score_mult_plus += 10;
		}

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BRAIN_SMASH)
	{
		dam = 78;
		if(enemy_r_ptr->flags1 & RF1_UNIQUE ||  enemy_r_ptr->flags3 & RF3_NO_CONF ||
			(enemy_r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))) dam = 0;
		else 
		{
			if(rlev > 10) dam /= rlev / 10;
			if(rlev < 20) score_mult_plus += 20;
		}

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CAUSE_1)
	{
		dam = 13;
		if(!monster_living(enemy_r_ptr)) dam = 0;
		else dam = dam * (60 - MIN(rlev,120) / 2) / 100;//強敵にはほとんど刺さらないので高レベルほど減らしとく。


		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CAUSE_2)
	{
		dam = 36;
		if(!monster_living(enemy_r_ptr)) dam = 0;
		else dam = dam * (60 - MIN(rlev,120)  / 2) / 100;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CAUSE_3)
	{
		dam = 80;
		if(!monster_living(enemy_r_ptr)) dam = 0;
		else dam = dam * (60 - MIN(rlev,120)  / 2) / 100;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CAUSE_4)
	{
		dam = 120;
		if(!monster_living(enemy_r_ptr)) dam = 0;
		else dam = dam * (60 - MIN(rlev,120)  / 2) / 100;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_ACID)
	{
		if(powerful) dam = rlev * 2 / 3 + 31;
		else dam = rlev / 3 + 31;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_ELEC)
	{
		if(powerful) dam = rlev * 2 / 3 + 18;
		else dam = rlev / 3 + 18;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_FIRE)
	{
		if(powerful) dam = rlev * 2 / 3 + 36;
		else dam = rlev / 3 + 36;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_COLD)
	{
		if(powerful) dam = rlev * 2 / 3 + 27;
		else dam = rlev / 3 + 27;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_LITE)
	{
		dam = rlev * 4  + 100;
		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 5;
		else if (enemy_r_ptr->flagsr & RFR_HURT_LITE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_NETH)
	{
		if(powerful) dam = rlev * 2 + 45;
		else dam = rlev * 4 / 3 + 45;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		if (enemy_r_ptr->flags3 & RF3_UNDEAD) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;

	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_WATE)
	{
		if(powerful) dam = rlev * 3 / 2 + 50;
		else dam = rlev + 50;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
			score_mult_plus += 10;

		if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
		else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;

	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_MANA)
	{
		dam = rlev * 7 / 4 + 50;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
	
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_PLAS)
	{
		if(powerful) dam = rlev * 3 / 2 + 42;
		else dam = rlev + 42;

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;

		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 3;
		
		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags5 & RF5_BO_ICEE)
	{
		if(powerful) dam = rlev * 3 / 2 + 21;
		else dam = rlev + 21;

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;
		if (!(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
			score_mult_plus += 10;
		
		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags5 & RF5_MISSILE)
	{

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else dam = rlev/3 + 7;
	
		total_damage += dam;
		spell_cnt++;
	}
	//麻痺や混乱などはほとんど雑魚にしか効かないので計算に入れない
	if(r_ptr->flags5 & RF5_BLIND)
	{
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CONF)
	{
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_SLOW)
	{
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_HOLD)
	{
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_HASTE)
	{
		score_mult_plus += 40;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_HAND_DOOM)
	{
		if (enemy_r_ptr->flags1 & RF1_UNIQUE || enemy_r_ptr->level > rlev + 10) dam = 0;
		else dam = 300; //適当
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_HEAL)
	{
		score_mult_plus += 20;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_INVULNER)
	{
		score_mult_plus += 60;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_BLINK)
	{
		if(mode == 2) //四人戦ではテレポ系が有利
			score_mult_plus += 20;
		else
			score_mult_plus += 10;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_TPORT)
	{
		if(mode == 2) //四人戦ではテレポ系が有利 
			score_mult_plus += 40;
		else
			score_mult_plus += 20;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_SPECIAL) 
	{
		if(r_idx == MON_REIMU)
		{
			dam = rlev / 10 * 10 * rlev/8 / 2 ; //霊夢特技
		}
		else if(r_idx == MON_MOKOU)
		{
			int enemy_rhp = enemy_r_ptr->hdice * enemy_r_ptr->hside / ((enemy_r_ptr->flags1 & RF1_FORCE_MAXHP) ? 1:2) * 4 / 5;

			dam = enemy_rhp / 4; //3〜4発当たるとしたらこんなもん？
			if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
			else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;
		}		
		else dam = 0;
		total_damage += dam;
		if(dam) spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_TELE_TO)
	{
		score_mult_plus += 20;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_PSY_SPEAR)
	{
		if(powerful) dam = rlev+180;
		else	dam = rlev * 3 / 4 + 120;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_COSMIC_HORROR)
	{
		dam = (rlev * 4 + 100) / 2;
		if (enemy_r_ptr->flags1 & RF1_UNIQUE || enemy_r_ptr->flags7 & RF7_UNIQUE2 || enemy_r_ptr->flags2 & RF2_ELDRITCH_HORROR || enemy_r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD)) dam = 0;

		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags9 & RF9_FIRE_STORM)
	{
		if(powerful) dam = rlev * 10 + 150;
		else dam = rlev * 7  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_ICE_STORM)
	{
		if(powerful) dam = rlev * 10 + 150;
		else dam = rlev * 7  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_THUNDER_STORM)
	{
		if(powerful) dam = rlev * 10 + 150;
		else dam = rlev * 7  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_ELEC) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_ACID_STORM)
	{
		if(powerful) dam = rlev * 10 + 150;
		else dam = rlev * 7  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_TOXIC_STORM)
	{
		if(powerful) dam = rlev * 8 + 75;
		else dam = rlev * 5  + 37;
		if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_POLLUTE)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 3  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_RES_DISE) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_DISI)
	{
		if(powerful) dam = rlev * 2 + 75;
		else dam = rlev  + 38;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_HOLY)
	{
		if(powerful) dam = rlev * 2 + 100;
		else dam = rlev  + 100;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
		else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_METEOR)
	{
		if(powerful) dam = rlev * 4 + 150;
		else dam = rlev * 2 + 150;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_DISTORTION)
	{
		if(powerful) dam = rlev * 3 + 100;
		else dam = rlev * 3 / 2 + 100;
		if (enemy_r_ptr->flagsr & RFR_RES_TIME ) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_PUNISH_1)
	{
		dam = 13;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY) dam = 0;
		//0でないときセービングスロー、破邪弱点、朦朧付加判定を足してプラマイゼロと乱暴に計算
		else dam = dam * (60 - MIN(rlev,120) / 2) / 50;//強敵にはほとんど刺さらないので高レベルほど減らしとく。

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_PUNISH_2)
	{
		dam = 36;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY) dam = 0;
		else dam = dam * (60 - MIN(rlev,120) / 2) / 50;//強敵にはほとんど刺さらないので高レベルほど減らしとく。
		//0でないときセービングスロー、破邪弱点、朦朧付加判定を足してプラマイゼロと乱暴に計算

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_PUNISH_3)
	{
		dam = 80;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY) dam = 0;
		else 
		{
			dam = dam * (60 - MIN(rlev,120) / 2) / 50;//強敵にはほとんど刺さらないので高レベルほど減らしとく。
			if (!(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
				score_mult_plus += 5;
		}
		//0でないときセービングスロー、破邪弱点、朦朧付加判定を足してプラマイゼロと乱暴に計算

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_PUNISH_4)
	{
		dam = 120;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY) dam = 0;
		else 
		{
			dam = dam * (60 - MIN(rlev,120) / 2) / 50;//強敵にはほとんど刺さらないので高レベルほど減らしとく。
			if (!(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
				score_mult_plus += 5;
		}
		//0でないときセービングスロー、破邪弱点、朦朧付加判定を足してプラマイゼロと乱暴に計算

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BO_HOLY)
	{
		if(powerful) dam = rlev * 3 / 2 + 50;
		else dam = rlev  + 50;

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
		else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags9 & RF9_GIGANTIC_LASER)
	{
		if(powerful) dam = rlev * 8 + 50;
		else dam = rlev * 4 + 50;

		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BO_SOUND)
	{
		if(powerful) dam = rlev * 3 / 2 + 50;
		else dam = rlev  + 16;

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_SOUN) dam /= 5;
		else if (!(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
				score_mult_plus += 20;


		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BEAM_LITE)
	{
		if(powerful) dam = rlev * 4;
		else dam = rlev * 2;

		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 5;
		else if (enemy_r_ptr->flagsr & RFR_HURT_LITE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_HELLFIRE)
	{
		if(powerful) dam = rlev * 4 + 150;
		else dam = rlev * 3 + 80;

		if (enemy_r_ptr->flagsr & RFR_HURT_HELL) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_HOLYFIRE)
	{
		if(powerful) dam = rlev * 4 + 150;
		else dam = rlev * 3 + 80;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
		else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_FINALSPARK)
	{
		dam = rlev * 5 / 2 + 200;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_TORNADO)
	{
		if(powerful) dam = rlev * 3 + 75;
		else dam = rlev * 2 + 37;

		if (enemy_r_ptr->flags2 & RF2_GIGANTIC) dam /= 8;
		else if (enemy_r_ptr->flags7 & RF7_CAN_FLY) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	//この魔法を持ってる敵は隣接攻撃が強いはず？
	if(r_ptr->flags9 & RF9_TELE_APPROACH)
	{
		if(mode != 2)
			score_mult_plus += 15;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_TELE_HI_APPROACH)
	{
		if(mode != 2)
			score_mult_plus += 15;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_MAELSTROM)
	{
		if(powerful) dam = rlev * 3;
		else dam = rlev * 2;

		if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
		else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;
		if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
			score_mult_plus += 20;

		total_damage += dam;
		spell_cnt++;
	}

	*spell_score_mult += score_mult_plus;
	if(!spell_cnt) 
		return 0;
	else 
		return (total_damage / spell_cnt);
}



/*:::battle_mon用　指定敵が別の敵に隣接攻撃したときの期待ダメージ*/
//ほかフラグによるスコア倍率も扱う
int	calc_battle_mon_attack_dam(int r_idx, int enemy_r_idx, int *attack_score_mult, int mode)
{
	int ap_cnt;
	int dam;
	int total_damage = 0;
	int rlev;
	monster_race *r_ptr =  &r_info[r_idx];
	monster_race *enemy_r_ptr =  &r_info[enemy_r_idx];
	int score_mult_plus = 0;

	int hit_chance;
	int enemy_ac = enemy_r_ptr->ac;

	//v1.1.64 Gシンボルの攻撃はハニワには効かない
	if (r_ptr->d_char == 'G' && (enemy_r_ptr->flags3 & RF3_HANIWA))
		return 0;

	rlev = r_ptr->level;
	if(rlev < 1) rlev = 1;
	//フラグを適当に処理
	if((r_ptr->flags2 & RF2_AURA_FIRE) && !(enemy_r_ptr->flagsr & RFR_IM_FIRE)) score_mult_plus += 3; 
	if((r_ptr->flags2 & RF2_AURA_ELEC) && !(enemy_r_ptr->flagsr & RFR_IM_ELEC)) score_mult_plus += 3; 
	if((r_ptr->flags2 & RF2_AURA_COLD) && !(enemy_r_ptr->flagsr & RFR_IM_COLD)) score_mult_plus += 3; 

	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		int effect = r_ptr->blow[ap_cnt].effect;
		int method = r_ptr->blow[ap_cnt].method;
		int d_dice = r_ptr->blow[ap_cnt].d_dice;
		int d_side = r_ptr->blow[ap_cnt].d_side;

		if (r_ptr->flags1 & RF1_NEVER_BLOW) break; 
		if(!method) break;
		if(!effect) break;
		if(effect == RBE_DR_MANA) continue;
		if(method == RBM_SHOOT) continue;

		dam = d_dice * (d_side + 1) / 2 * 100;

		//命中率を適当に計算
		if(effect == RBE_HURT || effect == RBE_SHATTER || effect == RBE_SUPERHURT || effect == RBE_BLEED)
		{
			hit_chance = ((60 + rlev * 3) - ( enemy_ac * 3 / 4)) * 100 / (60 + rlev * 3);
			if(effect == RBE_SUPERHURT)
				dam -= (dam * ((enemy_ac < 150) ? enemy_ac : 150) / 350); //乱暴な概算
			else
				dam -= (dam * ((enemy_ac < 150) ? enemy_ac : 150) / 250);
		}
		else
			hit_chance = ((10 + rlev * 3) - ( enemy_ac * 3 / 4)) * 100 / (60 + rlev * 3);

		if(hit_chance < 5) hit_chance = 5;
		if(hit_chance > 95) hit_chance = 95;

		switch (effect)
		{
		case RBE_POISON:
		case RBE_DISEASE:
			if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 9;
			break;

		case RBE_ACID:
			if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;
			break;

		case RBE_ELEC:
			if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
			else if (enemy_r_ptr->flagsr & RFR_HURT_ELEC) dam *= 2;
			break;

		case RBE_FIRE:
			if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
			else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;
			break;

		case RBE_COLD:
			if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
			else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;
			break;

		case RBE_UN_BONUS:
		case RBE_UN_POWER:
			if (enemy_r_ptr->flagsr & RFR_RES_DISE) dam /= 3;
			break;

		case RBE_CONFUSE: case RBE_INSANITY:
			if (enemy_r_ptr->flags3 & RF3_NO_CONF) dam /= 3;
			else score_mult_plus += 150 * hit_chance / 100;
			break;
		case RBE_EXP_10:
		case RBE_EXP_20:
		case RBE_EXP_40:
		case RBE_EXP_80:
		case RBE_KILL:
		case RBE_CURSE:
			if (enemy_r_ptr->flags3 & RF3_UNDEAD) dam = 0;
			else if (enemy_r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
			else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;
			break;

		case RBE_TIME:
			if (enemy_r_ptr->flagsr & RFR_RES_TIME) dam /= 3;
			break;

		case RBE_INERTIA:		
			if (enemy_r_ptr->flagsr & RFR_RES_INER) dam /= 3;

			break;

		case RBE_STUN:
			if (enemy_r_ptr->flagsr & RFR_RES_SOUN) dam /= 5;
			else if (!(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
				score_mult_plus += 70 * hit_chance / 100;

		case RBE_CHAOS: case RBE_MUTATE:
			if (enemy_r_ptr->flagsr & RFR_RES_CHAO) dam /= 3;
			else if (!(enemy_r_ptr->flags3 & RF3_NO_CONF)) 
				score_mult_plus += 150 * hit_chance / 100;
			break;
		case RBE_ELEMENT: //概算値
			if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam = dam * 4 / 7;
			if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam = dam * 4 / 7;
			if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam = dam * 4 / 7;
			if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam = dam * 4 / 7;
			break;
		case RBE_SMITE:
			if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
			else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
			else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;
			break;
		case RBE_DROWN:
			if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
			else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;
			if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN)) 
				score_mult_plus += 60 * hit_chance / 100;
			break;
		case RBE_EAT_GOLD:
		case RBE_EAT_ITEM:
		case RBE_PHOTO:
			if(mode == 2) score_mult_plus += 50;
			break;

		default:
			break;
		}


		total_damage += dam * hit_chance / 10000;

	}
	
	*attack_score_mult += score_mult_plus;
	return (total_damage);
}

/*:::新闘技場専用のモンスター選定関数 今のところ幻想郷の存在+ランダムユニークのみ*/
static bool monster_hook_battle_mon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	if(r_ptr->flagsr & RFR_RES_ALL) return FALSE; //はぐれメタルは出ない
	if(r_ptr->flags7 & RF7_CHAMELEON) return FALSE; //妖怪狐は出ない
	if(r_ptr->flags3 & RF3_WANTED) return FALSE; //お尋ね者は出ない

	if(r_idx == MON_KOSUZU) return FALSE; //v1.1.37 小鈴(取り憑かれ中)は出ない

	//v1.1.42 紫苑2は出ない
	if (r_idx == MON_SHION_2) return FALSE; 

	//v1.1.98 瑞霊は出ない
	if (r_idx == MON_MIZUCHI) return FALSE;

	if(r_idx == MON_MASTER_KAGUYA) return FALSE;//輝夜(可変)は出ない

	if(r_ptr->flags7 & RF7_AQUATIC) return FALSE; //今のところ水棲は不可
	if((r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags7 & RF7_VARIABLE)) return TRUE; //ランダムユニークも出してみる？

	if (r_idx == MON_MIYOI) return FALSE; //v1.1.78 美宵は出ない

   //生成基本レベルの半分以下のモンスターは出ない
	if(r_ptr->level < battle_mon_base_level / 2) return FALSE;

	if(r_ptr->flags3 & (RF3_GEN_MASK)) return TRUE;

	return FALSE;
}

//所属チームのフラグ値から下のルーチン用の配列引数を得る
static int calc_team_idx(u32b team)
{
	if(team & MFLAG_BATTLE_MON_TEAM_A) return 0;
	else if(team & MFLAG_BATTLE_MON_TEAM_B) return 1;
	else if(team & MFLAG_BATTLE_MON_TEAM_C) return 2;
	else if(team & MFLAG_BATTLE_MON_TEAM_D) return 3;
	else msg_print("ERROR:calc_team_idxの引数がおかしい");
	return 0;
}

/*:::新闘技場　一番上*/
void battle_mon_gambling(void)
{
	int r1_idx;
	int r2_idx;
	int r3_idx,r4_idx;
	char tmp_val[10] = "";
	monster_race *r1_ptr;
	monster_race *r2_ptr;
	int i,j;

	int tot_dam[4],need_turn[4],all_enemy_hp[4],ave_enemy_hp[4],odds[4];
	int team_idx;
	int row, printingteam, max_bet;

	int r1spd,r2spd;
	int r1_useturn, r2_useturn;
	int r1_dd, r2_dd;

	int dummy;
	int r1_atk, r2_atk;
	int r1_mag,r2_mag;
	int fail_count = 0;
	int min_turn, temp_val, penalty_ef;

	byte mode = randint1(3);
	if(mode==3) mode++;//2対2はちょっと稼ぎやすいし表示も面倒なのでやめた


	if (is_special_seikaku(SEIKAKU_SPECIAL_MEGUMU))
	{
		msg_print("この資金をギャンブルに使うわけにはいかない。");
		return;
	}
/*
	if(!p_ptr->wizard)
	{
		msg_print("雀斑の河童娘「悪いね。まだ準備中なんだ。」");
		return;
	}
*/
	/*::: Hack - 博麗神社の「我が家」の「最後に訪れたターン」の値を使って闘技場連続使用を抑止*/
	if((town[TOWN_HAKUREI].store[STORE_HOME].last_visit > turn - (TURNS_PER_TICK * BATTLE_MON_TICKS)) && !p_ptr->wizard)
	{
		msg_print("雀斑の河童娘「次の試合の組がまだ決まってないんだ。もう少し待っとくれ。」");
		return;
	}
	town[TOWN_HAKUREI].store[STORE_HOME].last_visit = turn;

	//可変パラメータモンスターのパラメータ計算のためこの段階でinside_battleをTRUEにする。
	//キャンセルやエラーなど試合を始めず出る場合必ずFALSEにして出ること
	p_ptr->inside_battle = TRUE;

	if(one_in_(4))
		battle_mon_base_level = randint1(p_ptr->lev * 2);
	else
		battle_mon_base_level = p_ptr->lev + randint1(p_ptr->lev);

	if(battle_mon_base_level < 30) battle_mon_base_level = 30;
	get_mon_num_prep(monster_hook_battle_mon,NULL);


	//モンスターを選定し倍率を計算する
	while(1)
	{
		bool flag_fail = FALSE;

		//変数初期化
		for(i=0;i<BATTLE_MON_LIST_MAX;i++) battle_mon_list[i].r_idx=0;
		for(i=0;i<BATTLE_MON_LIST_MAX;i++) battle_mon_list[i].team=0L;
		for(i=0;i<4;i++) battle_mon_team_type[i]=0;
		battle_mon_team_bet = 0L;
		for(i=0;i<4;i++) tot_dam[i]=0;
		for(i=0;i<4;i++) need_turn[i]=0;
		for(i=0;i<4;i++) all_enemy_hp[i]=0;
		for(i=0;i<4;i++) ave_enemy_hp[i]=0;
		for(i=0;i<4;i++) odds[i]=0;
		min_turn = -1;
		temp_val = 0;

		if(fail_count++ > 10000)
		{
			msg_print("WARNING:新闘技場にてチーム選定試行回数が10000回を超えた");
			get_mon_num_prep(NULL, NULL);
			p_ptr->inside_battle = FALSE;
			return;
		}

		//出場モンスターを選定しリストへ格納
		//モンスターはチーム順にソートされていなければならない
		switch(mode)
		{
		case 1: //1対1
			battle_mon_list[0].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[0].team = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_team_type[0] = 0;

			battle_mon_list[1].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[1].team = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_team_type[1] = 0;
			break;
		case 2: //4人
			battle_mon_list[0].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[0].team = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_team_type[0] = 0;
			battle_mon_list[1].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[1].team = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_team_type[1] = 0;
			battle_mon_list[2].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[2].team = MFLAG_BATTLE_MON_TEAM_C;
			battle_mon_team_type[2] = 0;
			battle_mon_list[3].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[3].team = MFLAG_BATTLE_MON_TEAM_D;
			battle_mon_team_type[3] = 0;
			break;
		case 3: //2対2
			battle_mon_list[0].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[0].team = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_team_type[0] = 0;
			battle_mon_list[1].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[1].team = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_team_type[1] = 0;
			battle_mon_list[2].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[2].team = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_team_type[2] = 0;
			battle_mon_list[3].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[3].team = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_team_type[3] = 0;
			break;
		case 4: //チーム
			{
				int cnt = 0;
				u32b team_flag;
				for(i=0;i<2;i++)
				{
					if(i==0) team_flag = MFLAG_BATTLE_MON_TEAM_A;
					else	 team_flag = MFLAG_BATTLE_MON_TEAM_B;

					if(one_in_(2)) //固定チーム
					{
						int special_team_idx;
						do
						{
							special_team_idx = randint1(BATTLE_MON_SPECIAL_TEAM_NUM);
						}while(battle_mon_special_team_list[special_team_idx].level > battle_mon_base_level
							|| i == 1 && (battle_mon_team_type[0] + special_team_idx == 0));

						battle_mon_team_type[i] = 0 - special_team_idx;
						for(j=0;j<8;j++)
						{
							if(!battle_mon_special_team_list[special_team_idx].r_idx_list[j]) break;
							battle_mon_list[cnt].r_idx =battle_mon_special_team_list[special_team_idx].r_idx_list[j] ;
							battle_mon_list[cnt++].team = team_flag;
						}

					}
					else //ランダムチーム
					{
						int num = 1 + randint1(3);
						int random_team_name;
						do
						{
							random_team_name = randint1(BATTLE_MON_RANDOM_TEAM_NAME_NUM);
						}while(i == 1 && random_team_name == battle_mon_team_type[0]);
						battle_mon_team_type[i] = random_team_name;
						if(one_in_(3)) num += randint1(4);
						for(j=0;j<num;j++)
						{
							battle_mon_list[cnt].r_idx = get_mon_num(battle_mon_base_level);
							battle_mon_list[cnt++].team = team_flag;

						}
					}
				}
			}
			break;

		default:
			msg_format("ERROR:新闘技場チーム選定モード値(%d)が処理未定義",mode);
			get_mon_num_prep(NULL, NULL);
			p_ptr->inside_battle = FALSE;
			return;
		}

		//ユニークモンスターの重複があるときモンスター選定からやり直し
		//mode1,2のときはユニークでなくても重複不可
		for(i=0;i<BATTLE_MON_LIST_MAX-1;i++)
		{
			if(flag_fail || !battle_mon_list[i].r_idx) break;


			for(j=i+1;j<BATTLE_MON_LIST_MAX;j++)
			{
				if(!battle_mon_list[j].r_idx) break;
				if(battle_mon_list[i].r_idx == battle_mon_list[j].r_idx)
				{
					if(mode == 1 || mode == 2 || mode == 3)
					{
						flag_fail = TRUE;
						break;
					}
					else if(r_info[battle_mon_list[i].r_idx].flags1 & RF1_UNIQUE || r_info[battle_mon_list[i].r_idx].flags7 & RF7_UNIQUE2)
					{
						//v1.1.42 闘技場用菫子だけは重複してOKにする。ひょっとしたらチーム戦で六人になるかもしれないがそれはそれで面白い
						if (battle_mon_list[i].r_idx != MON_SUMIREKO_2)
						{
							flag_fail = TRUE;
							break;
						}
					}
				}
			}
			if(battle_mon_list[i].r_idx == MON_MARISA)
			{
				//魔理沙はきのこ派
				if(battle_mon_team_type[calc_team_idx(battle_mon_list[i].team)] == 2) flag_fail = TRUE;
			}
		}
		if(flag_fail) continue;
		//可変パラメータモンスターのパラメータ計算をする
		for(i=0;i<BATTLE_MON_LIST_MAX;i++)
		{
			if(battle_mon_list[i].r_idx == MON_REIMU) apply_mon_race_reimu();
			else if(battle_mon_list[i].r_idx == MON_MARISA) apply_mon_race_marisa();
			else if(IS_RANDOM_UNIQUE_IDX(battle_mon_list[i].r_idx)) apply_random_unique(battle_mon_list[i].r_idx);
		}
		if(cheat_xtra) msg_format("");
		//ここから全チームの戦力評価と勝率計算をし、倍率を設定する
		//各チームの敵全てのHPを合計する
		for(i=0;i<4;i++)
		{
			int enemy_cnt = 0;
			for(j=0;j<BATTLE_MON_LIST_MAX;j++)
			{
				int mon_team_idx;

				if(flag_fail || !battle_mon_list[j].r_idx) break;
				mon_team_idx = calc_team_idx(battle_mon_list[j].team);

				if(i == mon_team_idx) continue;
				enemy_cnt++;
				r2_ptr =  &r_info[battle_mon_list[j].r_idx];

				if(r2_ptr->flags1 & RF1_FORCE_MAXHP)
					all_enemy_hp[i] += r2_ptr->hdice * r2_ptr->hside;
				else
					all_enemy_hp[i] += r2_ptr->hdice * (r2_ptr->hside+1)/2;
			}
			if(enemy_cnt) ave_enemy_hp[i] = all_enemy_hp[i] / enemy_cnt;
		}
		if(cheat_xtra) msg_format("EnemHP:%d:%d:%d:%d",all_enemy_hp[0],all_enemy_hp[1],all_enemy_hp[2],all_enemy_hp[3]);
		if(cheat_xtra) msg_format("Ave_HP:%d:%d:%d:%d",ave_enemy_hp[0],ave_enemy_hp[1],ave_enemy_hp[2],ave_enemy_hp[3]);

		//各チーム構成員による敵への平均攻撃力概算値を算出
		for(i=0;i<BATTLE_MON_LIST_MAX;i++)
		{
			int r1_hp, low_hp_mult;
			if(flag_fail || !battle_mon_list[i].r_idx) break;
			team_idx = calc_team_idx(battle_mon_list[i].team);
			r1_ptr =  &r_info[battle_mon_list[i].r_idx];

			if(r1_ptr->flags1 & RF1_FORCE_MAXHP)
				r1_hp = r1_ptr->hdice * r1_ptr->hside;
			else
				r1_hp = r1_ptr->hdice * (r1_ptr->hside+1) / 2;

			//4人戦かチーム戦のときHPが低いキャラにペナルティ
			low_hp_mult = 50 * (r1_hp + ave_enemy_hp[team_idx])/ ave_enemy_hp[team_idx];
			if(low_hp_mult > 100) low_hp_mult = 100;

			//このキャラクターによるすべての敵への攻撃平均ダメージを算出
			for(j=0;j<BATTLE_MON_LIST_MAX;j++)
			{
				int enemy_hp;
				int mult = 100, tmp_atk=0, tmp_mag=0, tmp_tot_dam, dist_mult;
				if(flag_fail || !battle_mon_list[j].r_idx) break;
				if(battle_mon_list[i].team & battle_mon_list[j].team) continue; //同チームは計算しない
				r2_ptr =  &r_info[battle_mon_list[j].r_idx];
				
				if(r2_ptr->flags1 & RF1_FORCE_MAXHP)
					enemy_hp = r2_ptr->hdice * r2_ptr->hside;
				else
					enemy_hp = r2_ptr->hdice * (r2_ptr->hside+1) / 2;

				//この敵に対する隣接攻撃期待ダメージ概算
				tmp_atk += calc_battle_mon_attack_dam(battle_mon_list[i].r_idx, battle_mon_list[j].r_idx, &mult,mode); 
				//この敵に対する遠隔攻撃期待ダメージ概算
				tmp_mag += calc_battle_mon_spell_dam(battle_mon_list[i].r_idx, battle_mon_list[j].r_idx, &mult,mode);
				//動かないが強力な魔法を持っている敵に補正
				///mod160206 ちょっと強化
				//v1.1.64 もっと強化
				if ((r1_ptr->flags1 & RF1_NEVER_MOVE) && tmp_mag > r1_ptr->level) mult += 200; 

				//4人戦かチーム戦のときHPが低いキャラにペナルティ
				if(mode == 2 || mode == 4)
				{
					mult = mult * low_hp_mult / 100;
					if(mult < 1) mult = 1;
				}
				//低レベルであるほど近距離攻撃が弱い設定にしておく
				dist_mult = MIN(80, (10 + r1_ptr->level * 4 / 3));
				if(((r1_ptr->flags1 & (RF1_RAND_50 | RF1_RAND_25)) == (RF1_RAND_50 | RF1_RAND_25))) dist_mult /= 4; 
				else if(r1_ptr->flags1 & (RF1_RAND_50)) dist_mult /= 2; 
				else if(r1_ptr->flags1 & (RF1_RAND_25)) dist_mult = dist_mult * 3 / 4; 

				//この敵に対する期待ダメージ　隣接は近づくまでの隙がある分ちょっと減らす。加速、魔法使用率、補助的倍率考慮
				tmp_tot_dam = ( tmp_atk * (100 - r1_ptr->freq_spell) * dist_mult / 100 + tmp_mag * (r1_ptr->freq_spell)) / 100 * SPEED_TO_ENERGY(r1_ptr->speed) / 10 * mult / 100;
				//敵HP合計に対するこの敵のHPの割合によってダメージを減じる
				tmp_tot_dam = tmp_tot_dam * enemy_hp / all_enemy_hp[team_idx];

				if(!tmp_tot_dam) tmp_tot_dam = 1;
				tot_dam[team_idx] += tmp_tot_dam;
			}
		}
		if(cheat_xtra) msg_format("Tot_dam:%d:%d:%d:%d",tot_dam[0],tot_dam[1],tot_dam[2],tot_dam[3]);

		//このチームが全ての敵を倒すまでの所要ターン平均(*100)を算出
		//(全ての敵のHP合計)/(全ての敵への期待ダメージ平均値(敵HPによる加重平均)のチーム合計) 全ての敵を倒すまでの期待ターン数(*100)が出るはず
		for(i=0;i<4;i++)
		{
			if(!tot_dam[i]) break;//存在しないチーム
			need_turn[i] = all_enemy_hp[i] * 100 / tot_dam[i];

			if(need_turn[i] > 100000)//敵を倒すのに1000ターン以上かかるチームが出たらモンスター選定からやり直し
			{
				flag_fail = TRUE;
				break;
			}
			if(min_turn < 0) min_turn = need_turn[i];
			else if(min_turn > need_turn[i]) min_turn = need_turn[i];
		}
		if(flag_fail) continue;

		if(cheat_xtra) msg_format("Need_turn:%d:%d:%d:%d",need_turn[0],need_turn[1],need_turn[2],need_turn[3]);
		
		//ペナルティ係数　不測の事態が起こりにくいゲームほどペナルティ多め
		if(mode == 1) penalty_ef = 35;
		else if(mode == 2) penalty_ef = 3;
		else if(mode == 3) penalty_ef = 20;
		else if(mode == 4) penalty_ef = 7;

		//予想所要ターンの多いチームに対してペナルティ適用(予想所要ターンの多いチームは高確率で所要ターン前に負けるので)
		//同ターン差ならターンが多いほど差が小さく、同ターン比率ならターンが多いほど差が大きくなるようにしとく
		//ペナルティ適用後のターン数に±20%のランダム補正
		for(i=0;i<4;i++)
		{
			int delay;
			if(!tot_dam[i]) break;//存在しないチーム
			if(need_turn[i] > min_turn)
			{
				delay = (need_turn[i] - min_turn) / 100;

				need_turn[i] += delay * delay * 100 * penalty_ef / 10;
				//ペナルティ適用後5000ターン以上かかるチームや5ターン以下しかかからないチームが出たらモンスター選定からやり直し
				if(need_turn[i] > 500000 || need_turn[i] < 500)
				{
					flag_fail = TRUE;
					break;
				}
			}
			need_turn[i] = need_turn[i] * (75 + damroll(5,9)) / 100;

			temp_val += 1000000 / need_turn[i];
		}
		if(flag_fail) continue;
		if(cheat_xtra) msg_format("Needturn2:%d:%d:%d:%d",need_turn[0],need_turn[1],need_turn[2],need_turn[3]);

		//ペナルティ適用済みのターン数を適当に計算して倍率(*100)を算出する
		for(i=0;i<4;i++)
		{
			int delay;
			if(!tot_dam[i]) break;//存在しないチーム
			//オーバーフローするかもしれんので一応例外処理
			if(need_turn[i] < 100000)
				odds[i] = need_turn[i] * temp_val / 10000;
			else
				odds[i] = need_turn[i] / 100 * temp_val / 100;

			//あまり倍率が高すぎたり低すぎたりするチームが出るとやり直し
			switch(mode)
			{
			case 1:
				if(odds[i] >= 10000 || odds[i] < 110 ) flag_fail = TRUE;
				break;
			case 2:
				if(odds[i] >= 100000 || odds[i] < 110 ) flag_fail = TRUE;
				break;
			case 3:
				if(odds[i] >= 10000 || odds[i] < 110 ) flag_fail = TRUE;
				break;
			case 4:
				if(odds[i] >= 10000 || odds[i] < 110 ) flag_fail = TRUE;
				break;
			default:
				break;
			}
		}

		if(cheat_xtra) msg_format("Odds:%d:%d:%d:%d",odds[0],odds[1],odds[2],odds[3]);

		if(flag_fail) continue;
		break;
	}
	get_mon_num_prep(NULL, NULL);

	if(cheat_xtra) msg_format("fail_count:%d",fail_count);

	screen_save();
	clear_bldg(4,22);
	printingteam = -1;
	row = 4;
	for(i=0;i<BATTLE_MON_LIST_MAX;i++)
	{
		char buf[160];
		char tmp_str[200];

		if(!battle_mon_list[i].r_idx) break;

		team_idx = calc_team_idx(battle_mon_list[i].team);

		if(!battle_mon_team_type[team_idx]) //単独チーム　キャラ名とオッズをセットで表記
		{
			printingteam = team_idx;
			if(IS_RANDOM_UNIQUE_IDX(battle_mon_list[i].r_idx))
				sprintf(buf, " %s ", random_unique_name[battle_mon_list[i].r_idx - MON_RANDOM_UNIQUE_1]);
			else if(battle_mon_list[i].r_idx == MON_REIMU || battle_mon_list[i].r_idx == MON_MARISA)
				sprintf(buf, " %s(Lv%d) ", r_name+r_info[battle_mon_list[i].r_idx].name, r_info[battle_mon_list[i].r_idx].level);
			else 
				sprintf(buf, " %s ", r_name+r_info[battle_mon_list[i].r_idx].name);
			sprintf(tmp_str, "[%c](x%3d.%02d) %s",'a'+team_idx, odds[team_idx]/100,odds[team_idx]%100, buf );
			prt(tmp_str, row, 1);

		}
		else //複数チーム
		{
			if(printingteam != team_idx) //このチームの初めて表示するキャラのとき、その前にチーム名表示
			{
				printingteam = team_idx;
				if(battle_mon_team_type[team_idx] < 0)
					sprintf(buf, " %s ", battle_mon_special_team_list[0-battle_mon_team_type[team_idx]].name);
				else
					sprintf(buf, " %s ", battle_mon_random_team_name_list[battle_mon_team_type[team_idx]]);
				sprintf(tmp_str, "[%c](x%3d.%02d) %s",'a'+team_idx, odds[team_idx]/100,odds[team_idx]%100, buf );
				prt(tmp_str, row, 1);
				row++;
			}
			if(IS_RANDOM_UNIQUE_IDX(battle_mon_list[i].r_idx))
				sprintf(buf, " %s ", random_unique_name[battle_mon_list[i].r_idx - MON_RANDOM_UNIQUE_1]);
			else if(battle_mon_list[i].r_idx == MON_REIMU || battle_mon_list[i].r_idx == MON_MARISA)
				sprintf(buf, " %s(Lv%d) ", r_name+r_info[battle_mon_list[i].r_idx].name, r_info[battle_mon_list[i].r_idx].level);
			else 
				sprintf(buf, " %s ", r_name+r_info[battle_mon_list[i].r_idx].name);
			sprintf(tmp_str, "               %s", buf );
			prt(tmp_str, row, 1);
		}
		row++;
	}

	//倍率表示とチーム選択
	max_bet = p_ptr->lev;
	if(p_ptr->total_winner) max_bet = 100;
	if(max_bet > p_ptr->au / 1000) max_bet = p_ptr->au / 1000;


	if(p_ptr->psex == SEX_MALE) msg_print("雀斑の河童娘「やあ兄さん、どうだい？」");
	else msg_print("雀斑の河童娘「やあ姐さん、どうだい？」");

	while(1)
	{
		char c;
		battle_mon_team_bet = 0L;
		
		c = inkey();
		if(c == ESCAPE)
		{
			msg_print("「何だ、賭けないのかい？」");
			break;
		}
		if(c == 'a' || c == 'A') 
		{
			battle_mon_team_bet = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_odds = odds[0];
		}
		else if(c == 'b' || c == 'B')
		{
			battle_mon_team_bet = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_odds = odds[1];
		}
		else if((c == 'c' || c == 'C') && odds[2])
		{
			battle_mon_team_bet = MFLAG_BATTLE_MON_TEAM_C;
			battle_mon_odds = odds[2];
		}
		else if((c == 'd' || c == 'D') && odds[3])
		{
			battle_mon_team_bet = MFLAG_BATTLE_MON_TEAM_D;
			battle_mon_odds = odds[3];
		}
		else continue;

		if(!max_bet)
		{
			battle_mon_team_bet = 0L;
			msg_print("「何だい、もうオケラじゃないか。」");
			break;
		}
		battle_mon_wager = get_quantity(format("「一口$1000だよ。何口賭けるね？」(MAX:%d)",max_bet),max_bet) * 1000;
	
		if(!battle_mon_wager)
		{
			msg_print("「選び直すのかい？」");
			continue;
		}
		break;
	}
	screen_load();

	if(!battle_mon_team_bet)
	{
		p_ptr->inside_battle = FALSE;
		return;
	}

	//ここでなく試合終了後に所持金減少処理することにした。なぜか試合開始直後に闘技場が終了することがあるため。
	//p_ptr->au -= battle_mon_wager;
	set_action(ACTION_NONE);
	prepare_change_floor_mode(CFM_SAVE_FLOORS);
	p_ptr->leaving = TRUE;

}



static bool item_tester_hook_marisa_good_book(object_type *o_ptr)
{
	if(o_ptr->tval >= TV_BOOK_ELEMENT && o_ptr->tval <= TV_BOOK_CHAOS && o_ptr->sval > 1) return (TRUE);
	else return (FALSE);
}

//魔理沙専用 魔法書tvalとsvalからmagic_num[]添字を返す
static int marisa_book_to_magic_num(int tval, int sval)
{
	if(tval > MAX_MAGIC || tval < MIN_MAGIC || sval < 2)
	{
		msg_format("ERROR:marisa_book_to_magic()に3,4冊目魔法書以外の物が渡された");
		return -1;
	}

	return (((sval)-2) * 12 + (tval) - 1);

}

/*::魔理沙の魔法習得*/
void marisa_make_magic_recipe(void)
{
	object_type forge;
	object_type *q_ptr = &forge;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int item, magic_num;
	int i;
	char magic_name[80];

	if(p_ptr->pclass != CLASS_MARISA)
	{ 
		msg_print("ERROR:魔理沙以外でmarisa_make_magic()が呼ばれた");
		return;
	}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print("今は闇市場の調査中だ。アビリティカードを探しに行こう。");
		return;
	}

	if(p_ptr->magic_num2[27])
	{
		msg_print("もう魔法のアイデアは十分に集めた。");
		return;
	}

	q = "どの魔法書を研究しますか？";
	s = "面白そうな本がない。";
	item_tester_hook = item_tester_hook_marisa_good_book;

	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	magic_num = marisa_book_to_magic_num(o_ptr->tval, o_ptr->sval);

	if(magic_num < 0 || magic_num > 23) { msg_format("ERROR:魔理沙magic_numがおかしい(%d)",magic_num); return;}

	if(p_ptr->magic_num2[magic_num])
	{
		msg_print("その本は既に読了済みだ。");
		return;
	}
	if(one_in_(4)) c_put_str(TERM_WHITE,"あなたは本を片手に魔法薬やキノコを混ぜ始めた・・",8 , 6);
	else if(one_in_(3)) c_put_str(TERM_WHITE,"あなたは本を読んでインスピレーションを得た。",8 , 6);
	else if(one_in_(2))  c_put_str(TERM_WHITE,"あなたは本をヒントに新たなレシピを考え始めた・・",8 , 6);
	else c_put_str(TERM_WHITE,"あなたは本を読みながら様々な実験を始めた・・",8 , 6);
	(void)inkey();

	sprintf(magic_name,"%s%sを作れるようになった！",marisa_magic_table[magic_num].name,(marisa_magic_table[magic_num].is_drug) ? "" : "の魔法" );

	p_ptr->magic_num2[magic_num] = 1;
	c_put_str(TERM_WHITE,magic_name,9 , 7);

	//注：マジックナンバー多数
	if(magic_num >= 12 && magic_num <= 15 && p_ptr->magic_num2[12]  && p_ptr->magic_num2[13] && p_ptr->magic_num2[14] && p_ptr->magic_num2[15])
	{
		p_ptr->magic_num2[24] = 1;
		c_put_str(TERM_WHITE,format("さらに%sの魔法を作れるようになった！",marisa_magic_table[24].name),10 , 8);
	}
	else if(magic_num >= 16 && magic_num <= 19 && p_ptr->magic_num2[16]  && p_ptr->magic_num2[17] && p_ptr->magic_num2[18] && p_ptr->magic_num2[19])
	{
		p_ptr->magic_num2[25] = 1;
		c_put_str(TERM_WHITE,format("さらに%sの魔法を作れるようになった！",marisa_magic_table[25].name),10 , 8);
	}
	else if(magic_num >= 20 && magic_num <= 23 && p_ptr->magic_num2[20]  && p_ptr->magic_num2[21] && p_ptr->magic_num2[22] && p_ptr->magic_num2[23])
	{
		p_ptr->magic_num2[26] = 1;
		c_put_str(TERM_WHITE,format("さらに%sの魔法を作れるようになった！",marisa_magic_table[26].name),10 , 8);
	}
	
	for(i=0;i<27;i++) if(!p_ptr->magic_num2[i]) break;
	if(i == 27)
	{
		p_ptr->magic_num2[27] = 1;
		c_put_str(TERM_WHITE,format("そして%sの魔法を作れるようになった！",marisa_magic_table[27].name),11 , 9);
	}

	return;
}


/*:::魔理沙が指定した魔法をいくつ作れるかチェック*/
static int check_marisa_can_make_spell(int spell_num)
{
	int i;
	int num = 99;

	if(p_ptr->pclass != CLASS_MARISA)
	{
		msg_print("ERROR:marisa_can_make_spell()が魔理沙以外で呼ばれた"); 
		return 0;
	}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print("今は闇市場の調査中だ。アビリティカードを探しに行こう。");
		return 0;
	}

	//未開発の魔法(ここには来ないはず)
	if(!p_ptr->magic_num2[spell_num]) return 0;

	///mod151224 魔理沙デバッグモードなど用
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN) return 99;

	//材料が足りているかチェックする
	for(i=0;i<8;i++)
	{
		int num_tmp;
		if(!marisa_magic_table[spell_num].use_magic_power[i]) continue;
		
		if(p_ptr->noscore)
			num_tmp = p_ptr->magic_num1[30+i] / marisa_magic_table[spell_num].use_magic_power[i];
		else
			num_tmp = marisa_magic_power[i] / marisa_magic_table[spell_num].use_magic_power[i];

		if(!num_tmp) return 0;
		if(num_tmp < num) num = num_tmp;
	}

	return num;
}

/*:::魔理沙の魔法選定 選択時にmarisa_magic_table添字、キャンセル時は-1が返る*/
int choose_marisa_magic(int mode)
{
	int num = -1;
	int i;
	int carry_num = 0;
	bool flag_repeat = FALSE;

	for(i=0;i<MARISA_MAX_MAGIC_KIND;i++) if(p_ptr->magic_num2[i]) break;
	if(i == MARISA_MAX_MAGIC_KIND)
	{
		msg_print("まだ一つも魔法のアイデアがない。");
		return -1;
	}
	if(mode == CMM_MODE_CARRY)
	{
		for(i=0;i<MARISA_MAX_MAGIC_KIND;i++) carry_num += p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT];
	}

	screen_save();
	//建物内かそうでないかで画面クリアする領域を変えておく。上のscreen_save()内でcharacter_icky++されるのでTRUE/FALSEでは判定できない。
	if(character_icky > 1)
		clear_bldg(4,22);
	else
		for(i=4;i<22;i++) Term_erase(17, i, 255);


	if(mode == CMM_MODE_CHECK)	
		prt("「どの魔法について確認しよう？」(ESC:キャンセル)",4,18);
	else if(mode == CMM_MODE_CARRY)	
	{
		prt("「どの魔法を持って行こう？」",4,18);
		prt(format(" (あと%d ESC:選択終了　'R':所持魔法を全て家に戻す)",(MARISA_CARRY_MAGIC_NUM-carry_num)),5,18);
	}
	else if(mode == CMM_MODE_MAKE)
		prt("「どの魔法を作ろう？」(ESC:キャンセル)",4,18);
	else				
		prt("「どの魔法を使おう？」(ESC:キャンセル)",4,18);

	for(i=0;i<MARISA_MAX_MAGIC_KIND;i++)
	{
		char magic_desc[80];
		char sym = (i < 26)?('a'+i):('A'+i-26);

		//未作成レシピは表示しない
		if(!p_ptr->magic_num2[i]) continue;

		if(mode == CMM_MODE_CARRY || mode == CMM_MODE_CHECK)
		{
			sprintf(magic_desc,"%c)%s(%d/%d)",sym,marisa_magic_table[i].name,p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT],p_ptr->magic_num1[i]);
		}
		else if(mode == CMM_MODE_CAST)
		{
			sprintf(magic_desc,"%c)%s(%d)",sym,marisa_magic_table[i].name,p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT]);
		}
		else
		{
			sprintf(magic_desc,"%c)%s%s(%d/%d)",sym,marisa_magic_table[i].name,(marisa_magic_table[i].is_drug)?"生成":"",p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT],p_ptr->magic_num1[i]);
		}

		if(mode == CMM_MODE_CARRY)
		{
			if(carry_num >= MARISA_CARRY_MAGIC_NUM || !p_ptr->magic_num1[i])
				c_prt(TERM_L_DARK, magic_desc,7+(i%14),18+(i/14) * 36);
			else
				prt(magic_desc,7+(i%14),18+(i/14) * 36);
		}
		else if(mode == CMM_MODE_CAST)
		{
			if(!p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT])
				c_prt(TERM_L_DARK, magic_desc,7+(i%14),18+(i/14) * 36);
			else
				prt(magic_desc,7+(i%14),18+(i/14) * 36);
		}
		else if(mode == CMM_MODE_MAKE)
		{
			//魔法作成時、材料が足りない魔法は灰字
			if(!check_marisa_can_make_spell(i))
				c_prt(TERM_L_DARK, magic_desc,7+(i%14),18+(i/14) * 36);
			else
				prt(magic_desc,7+(i%14),18+(i/14) * 36);
		}
		else
		{
			prt(magic_desc,7+(i%14),18+(i/14) * 36);
		}

	}

#ifdef ALLOW_REPEAT
	if (mode == CMM_MODE_CAST && repeat_pull(&num))
	{
		flag_repeat = TRUE;
	}
#endif

	while(1)
	{

		char c;

		if(!flag_repeat)
		{
			num = -1;
			c = inkey();
			if(c == ESCAPE) break;
			else if(c >= 'a' && c <= 'z') num = c - 'a';
			else if(c == 'A' || c == 'B') num = c - 'A' + 26;
			else if(c == 'R')
			{
				if(mode == CMM_MODE_CARRY)
				{
					num = 99;//所持魔法リセット
					break;
				}
				else continue;
			}
			else continue;
		}

		//未作成レシピは選択されない
		if(!p_ptr->magic_num2[num]) continue;
		//魔法作成時、材料が足りない魔法は選択されない
		if(mode == CMM_MODE_MAKE  && !check_marisa_can_make_spell(num))
		{
			msg_print("それを作るには材料が足りない。");
			flag_repeat = FALSE;
			msg_print(NULL);
			continue;
		}
		else if(mode == CMM_MODE_CAST && !p_ptr->magic_num1[num+MARISA_HOME_CARRY_SHIFT])
		{
			msg_print("その魔法は手持ちがない。");
			num = -1;
			break;
		}
		else if(mode == CMM_MODE_CARRY)
		{
			if(!p_ptr->magic_num1[num])
			{
				msg_print("魔法の作り置きがない。");
				flag_repeat = FALSE;
				msg_print(NULL);
				continue;
			}
			if(carry_num >= MARISA_CARRY_MAGIC_NUM)
			{
				msg_print("もう持てない。");
				flag_repeat = FALSE;
				msg_print(NULL);
				continue;
			}
		}
#ifdef ALLOW_REPEAT /* TNB */
		if(!flag_repeat) repeat_push(num);
#endif /* ALLOW_REPEAT -- TNB */


		break;

	}

	screen_load();
	return num;

}

//魔理沙の魔法作成
bool make_marisa_magic(void)
{
	int spell_num;
	int amount;
	int max;
	int i;

	if(p_ptr->pclass != CLASS_MARISA)
	{msg_print("ERROR:make_marisa_magic()が魔理沙以外で呼ばれた"); return FALSE;}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print("今は闇市場の調査中だ。アビリティカードを探しに行こう。");
		return FALSE;
	}

	//作成する魔法を選択　材料が足りていることも確認
	spell_num = choose_marisa_magic(CMM_MODE_MAKE);
	if(spell_num < 0) return FALSE;
	if(p_ptr->magic_num1[spell_num] >= 9999)
	{
		msg_print("もうそれは使い切れないほど作り置いてある。");
		return FALSE;
	}
	max = check_marisa_can_make_spell(spell_num);

	if(p_ptr->magic_num1[spell_num] + max > 9999) max = 9999 - p_ptr->magic_num1[spell_num];

	amount = get_quantity(format("%sをいくつ作ろう？(1-%d)",marisa_magic_table[spell_num].name,max),max);

	if(amount < 1 || amount > 99) return FALSE;

	prt("あなたは調合に取り掛かった・・",8,20);//ここもっとメッセージ増やそうか
	inkey();

	prt(format("%s%sが完成した！",marisa_magic_table[spell_num].name,(marisa_magic_table[spell_num].is_drug)?"":"の魔法" ),10,20);
	p_ptr->magic_num1[spell_num] += amount;


	///mod151224 魔理沙デバッグモードなど用
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN) return TRUE;
	///mod160103 性格いかさま以外のチートオプション使用時は別領域を使うことにした
	if(p_ptr->noscore)
	{
		for(i=0;i<8;i++)
		{
			if((int)p_ptr->magic_num1[30+i] < amount * marisa_magic_table[spell_num].use_magic_power[i])
			{
				msg_print("ERROR:魔力備蓄が足りない！");
				return FALSE;
			}
			p_ptr->magic_num1[30+i] -= amount * marisa_magic_table[spell_num].use_magic_power[i];
		}
	}
	else
	{
		for(i=0;i<8;i++)
		{
			if((int)marisa_magic_power[i] < amount * marisa_magic_table[spell_num].use_magic_power[i])
			{
				msg_print("ERROR:魔力備蓄が足りない！");
				return FALSE;
			}
			marisa_magic_power[i] -= amount * marisa_magic_table[spell_num].use_magic_power[i];
		}
	}

	return TRUE;
}

//魔力の備蓄を確認
void marisa_check_essence(void)
{
	int i;
	if(p_ptr->pclass != CLASS_MARISA)
	{msg_print("ERROR:marisa_check_essence()が魔理沙以外で呼ばれた"); return ;}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print("今は闇市場の調査中だ。アビリティカードを探しに行こう。");
		return;
	}


	screen_save();
	for(i=4;i<14;i++) Term_erase(17, i, 255);

	prt("現在魔力の備蓄はこれだけある。",4,18);
	for(i=0;i<8;i++)
	{

		///mod151224 魔理沙デバッグモードなど用
		if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
			prt(format("%sの魔力:(*****)",marisa_essence_name[i]),6+i,18);
		else if(p_ptr->noscore)
			prt(format("%sの魔力:(%d)",marisa_essence_name[i],p_ptr->magic_num1[30+i]),6+i,18);
		else
			prt(format("%sの魔力:(%d)",marisa_essence_name[i],marisa_magic_power[i]),6+i,18);
	}
	inkey();
	screen_load();

}

//魔理沙の作成した魔法を16個まで持つ

bool carry_marisa_magic(void)
{
	int spell_num;
	int amount;
	int max;
	int i;

	if(p_ptr->pclass != CLASS_MARISA)
	{msg_print("ERROR:carry_marisa_magic()が魔理沙以外で呼ばれた"); return FALSE;}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print("今は闇市場の調査中だ。アビリティカードを探しに行こう。");
		return FALSE;
	}


	while(1)
	{
		spell_num = choose_marisa_magic(CMM_MODE_CARRY);
		if(spell_num < 0) return TRUE;
		if(spell_num == 99)//選択画面でスペースを押した時所持魔法を全部家に戻して選択しなおし
		{
			for(i=0;i<MARISA_MAX_MAGIC_KIND;i++)
			{
				p_ptr->magic_num1[i] += p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT];
				p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT] = 0;
			}
			continue;
		}

		p_ptr->magic_num1[spell_num] -=1 ;
		p_ptr->magic_num1[spell_num+40] +=1 ;
	}

	return TRUE;
}



/*:::武器攻撃の威力を計算 hand:右手の時0,左手の時1*/
int calc_weapon_dam(int hand)
{
	int total_damage = 0;
	int basedam;
	int dd,ds;
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	if(hand < 0 || hand > 1)
	{
		msg_print("ERROR:calc_weapon_dam()がhand=0,1以外で呼ばれた");
		return 0;
	}
	if (!buki_motteruka(INVEN_RARM+hand)) return 0;
	o_ptr = &inventory[INVEN_RARM+hand];
	dd = o_ptr->dd + p_ptr->to_dd[hand];
	ds = o_ptr->ds + p_ptr->to_ds[hand];
	basedam = (dd * (ds + 1)) * 50;
	total_damage = o_ptr->to_d * 100;
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_EX_VORPAL))
	{
		basedam *= 5;
		basedam /= 3;
	}
	else if (have_flag(flgs, TR_VORPAL))
	{
		basedam *= 11;
		basedam /= 9;
	}
	total_damage += basedam;
	total_damage += p_ptr->to_d[hand] * 100;
	total_damage *= p_ptr->num_blow[hand];
	total_damage /= 100;
	return total_damage;
}


/*:::雛が無人販売所から資金を得る*/
/*p_ptr->magic_num1[1]を稼ぎとして使う*/
void hina_at_work(void)
{
	
	int gain;

	if(p_ptr->pclass != CLASS_HINA)
	{ 
		msg_print("止めておこう。何が起こるか分かったものじゃない。"); 
		return; 
	}

	gain = p_ptr->magic_num1[1];

	screen_save();
	clear_bldg(4,22);

	if(gain < 100)
	{
		prt(format("人形はまだ全く売れていない。"),10,20);
	}
	else if(gain < 1000)
	{
		prt(format("人形はほとんど売れていない・・"),10,20);
	}
	else if(gain < 5000)
	{
		prt(format("人形は少しは売れたようだ。"),10,20);
	}
	else if(gain < 50000)
	{
		prt(format("人形の売れ行きは上々だ。"),10,20);
	}
	else if(gain < 100000)
	{
		prt(format("人形がほとんど売れている！"),10,20);
	}
	else 
	{
		gain = 100000;
		prt(format("人形が全て売れていた。人里で何が起こっているのだろうか。"),10,20);
	}
	inkey();

	if(gain >= 100)
	{
		gain -= gain % 100;
		msg_format("$%dを回収した。",gain);
		p_ptr->au += gain;
		p_ptr->magic_num1[1] -= gain;
		building_prt_gold();
		if(gain == 100000) msg_format("あなたは新たな在庫を取り出した。");
		else msg_print("あなたは在庫を補充した。");

	}
	screen_load();

}



/*:::調剤室で材料として使えるアイテムを判定*/
static bool item_tester_hook_drug_material(object_type *o_ptr)
{
	int i;
	if(!object_is_aware(o_ptr)) return FALSE;

	for(i=0;drug_material_table[i].tval;i++)
	{
		if(o_ptr->tval == drug_material_table[i].tval && o_ptr->sval == drug_material_table[i].sval) return TRUE;
	}
	return FALSE;

}



/*:::薬師などが薬を合成する*/
/*:::p_ptr->magic_num2[100-107]が薬作成済みフラグとして使われる*/
void compound_drug(void)
{
	object_type forge;
	object_type *prod_o_ptr = &forge;//生成済みアイテム
	object_type *m1_o_ptr,*m2_o_ptr,*m3_o_ptr;//材料1〜3
	int item1,item2,item3; //材料1〜3のインベントリ内番号
	int m1,m2,m3; //材料1〜3のdrug_material_table添字
	bool flag1,flag2,flag3; //材料1〜3を選択したかどうかのフラグ 正直こんな沢山の変数使わず配列か構造体にして整頓しとけばもっとスッキリしたコードになったのにと思うがもう遅い
	bool flag_make_drug_ok = FALSE;
	bool flag_success;
	bool flag_exp; //作ったことのあるアイテムの場合TRUE
	bool flag_have_recipe; //対応する秘伝書がある

	char c;
	char o_name[MAX_NLEN];
	char m1_o_name[MAX_NLEN], m2_o_name[MAX_NLEN], m3_o_name[MAX_NLEN];
	int chance; //成功率

	cptr q = "どの材料を調合に使いますか？";
	cptr s = "薬の材料になりそうなものを持っていない。";
	int i;
	int elem_power[8];//材料属性値の合計
	int compound_result_num;//完成する品(drug_compound_table[]の添字)
	int amt; //作成数

	int cost;
	int total_cost;//選択した材料*個数　調剤室への支払金額
	int rtv,rsv; //最終的に完成するもののTvalとSval


	if(!CHECK_MAKEDRUG_CLASS)
	{
		msg_print("あなたは調剤室を借りられなかった。");
		return;
	}
	else if(inventory[INVEN_PACK-1].k_idx)
	{
		msg_print("あなたのザックはすでに一杯だ。");
		return;
	}
	screen_save();

	flag1 = FALSE;
	flag2 = FALSE;
	flag3 = FALSE;

	//材料を3つまで選定する
	while(1)
	{

		clear_bldg(0, 20);
		c_put_str(TERM_WHITE,format("素材にするものを3つまで選んで下さい:"),8,12);
		//プロンプト表示
		if(flag1)
		{
			object_desc(m1_o_name, m1_o_ptr, (OD_NO_FLAVOR | OD_OMIT_PREFIX | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,format("a)素材1:%s", m1_o_name),10,10);
		}
		else
		{
			c_put_str(TERM_WHITE,format("a)素材1:未選択"),10,10);
		}
		if(flag2)
		{
			object_desc(m2_o_name, m2_o_ptr, (OD_NO_FLAVOR | OD_OMIT_PREFIX | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,format("b)素材2:%s", m2_o_name),11,10);
		}
		else
		{
			c_put_str(TERM_WHITE,format("b)素材2:未選択"),11,10);
		}
		if(flag3)
		{
			object_desc(m3_o_name, m3_o_ptr, (OD_NO_FLAVOR | OD_OMIT_PREFIX | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,format("c)素材3:%s", m3_o_name),12,10);
		}
		else
		{
			c_put_str(TERM_WHITE,format("c)素材3:未選択"),12,10);
		}

		//材料は二つ必要
		if(flag1 && flag2 || flag1 && flag3 || flag2 && flag3)
		{
			if(p_ptr->pclass == CLASS_EIRIN && p_ptr->town_num == TOWN_EIENTEI)
				c_put_str(TERM_WHITE,format("d)この材料から薬を作る(施設使用料:無料)"),14,10);
			else
				c_put_str(TERM_WHITE,format("d)この材料から薬を作る(施設使用料:一つ$%d)",cost),14,10);
		}

		c = inkey();

		if(c == ESCAPE)
		{
			break;
		}
		else if(c == 'a' || c == 'A' || c == 'b' || c == 'B' || c == 'c' || c == 'C')
		{
			int item_tmp;

			item_tester_hook = item_tester_hook_drug_material;
			if (get_item(&item_tmp, q, s, (USE_INVEN)))
			{
				if(c == 'a' || c == 'A')
				{
					item1 = item_tmp;
					m1_o_ptr = &inventory[item1];
					flag1 = TRUE;
					//選択したアイテムが材料テーブルの何番目に来るか数えている
					for(m1=0;drug_material_table[m1].tval != m1_o_ptr->tval || drug_material_table[m1].sval != m1_o_ptr->sval;m1++);
				}
				else if(c == 'b' || c == 'B')
				{
					item2 = item_tmp;
					m2_o_ptr = &inventory[item2];
					flag2 = TRUE;
					for(m2=0;drug_material_table[m2].tval != m2_o_ptr->tval || drug_material_table[m2].sval != m2_o_ptr->sval;m2++);
				}
				else
				{
					item3 = item_tmp;
					m3_o_ptr = &inventory[item3];
					flag3 = TRUE;
					for(m3=0;drug_material_table[m3].tval != m3_o_ptr->tval || drug_material_table[m3].sval != m3_o_ptr->sval;m3++);
				}

			}
			else
			{
				if(c == 'a' || c == 'A') flag1 = FALSE;
				else if(c == 'b' || c == 'B') flag2 = FALSE;
				else flag3 = FALSE;
			}
			//一つ当たり費用計算
			cost = 0;
			if(flag1) cost += drug_material_table[m1].cost;
			if(flag2) cost += drug_material_table[m2].cost;
			if(flag3) cost += drug_material_table[m3].cost;

		}
		else if((c == 'd' || c == 'D') && (flag1 && flag2 || flag1 && flag3 || flag2 && flag3))
		{

			int max = 99;
			//最大作成可能数を計算する
			if(flag1) max = MIN(max,m1_o_ptr->number);
			if(flag2) max = MIN(max,m2_o_ptr->number);
			if(flag3) max = MIN(max,m3_o_ptr->number);
			//同じ材料を二つ以上使った時作成可能数をその分減らす
			if(flag1 && flag2 && flag3 && item1==item2 && item2==item3) max /= 3;
			else if(flag1 && flag2 && item1==item2) max = MIN(max,m1_o_ptr->number/2);
			else if(flag1 && flag3 && item1==item3) max = MIN(max,m1_o_ptr->number/2);
			else if(flag2 && flag3 && item2==item3) max = MIN(max,m2_o_ptr->number/2);

			if(!max)
			{
				msg_print("材料の数が足りない。");
				continue;
			}
			else if(max > 1)
			{
				amt = get_quantity(format("いくつ作りますか？(1-%d)",max),max);
				if(!amt) continue;
			}
			else amt = 1;



			total_cost = cost * amt;

			//永琳は永遠亭の使用料無料
			if(p_ptr->pclass == CLASS_EIRIN && p_ptr->town_num == TOWN_EIENTEI) total_cost = 0;

			if(p_ptr->au < total_cost)
			{
				msg_print("所持金が施設使用料に足りないようだ。");
				continue;
			}

			//属性値計算
//竜の爪は属性値を倍にする？

			for(i=0;i<8;i++) elem_power[i] = 0;
			if(flag1) for(i=0;i<8;i++) elem_power[i] += drug_material_table[m1].elem[i];
			if(flag2) for(i=0;i<8;i++) elem_power[i] += drug_material_table[m2].elem[i];
			if(flag3) for(i=0;i<8;i++) elem_power[i] += drug_material_table[m3].elem[i];

			compound_result_num = 0;

			//合成結果テーブルを走査し完成するアイテムを決定。条件を満たしていれば下のものほど優先される
			for(i=1;drug_compound_table[i].tval;i++)
			{
				bool flag_ok1 = TRUE;
				bool flag_ok2 = TRUE;
				int j;

				if(i >= 63)//一応オーバーフロー防止
				{
					msg_print("ERROR:drug_compound_table[]の登録数が多くて作成済みフラグビットに足りない!");
					return;
				}

				//属性値条件判定
				for(j=0;j<8;j++)
				{
					if(drug_compound_table[i].elem[j] < 0 && elem_power[j] > drug_compound_table[i].elem[j]) flag_ok1 = FALSE;
					if(drug_compound_table[i].elem[j] > 0 && elem_power[j] < drug_compound_table[i].elem[j]) flag_ok1 = FALSE;
				}
				//必要特殊素材判定
				if(drug_compound_table[i].special_material_tval)
				{
					flag_ok2 = FALSE;
					
					if(flag1 && m1_o_ptr->tval == drug_compound_table[i].special_material_tval 
						&& m1_o_ptr->sval == drug_compound_table[i].special_material_sval) flag_ok2 = TRUE;
					if(flag2 && m2_o_ptr->tval == drug_compound_table[i].special_material_tval 
						&& m2_o_ptr->sval == drug_compound_table[i].special_material_sval) flag_ok2 = TRUE;
					if(flag3 && m3_o_ptr->tval == drug_compound_table[i].special_material_tval 
						&& m3_o_ptr->sval == drug_compound_table[i].special_material_sval) flag_ok2 = TRUE;
					
				}

				if(!flag_ok1 || !flag_ok2) continue;

				compound_result_num = i;

			}
			//すでに作ったことのある品のフラグを得る
			flag_exp = ((p_ptr->magic_num2[100+compound_result_num/8] >> compound_result_num % 8) & 1L);
			//レシピがあるかチェックする
			//永琳は全てのレシピを知っている
			if(p_ptr->pclass == CLASS_EIRIN)
			{
				flag_have_recipe = TRUE;
				flag_exp = TRUE;
			}
			else
			{
				flag_have_recipe = FALSE;
				for(i=0;i<32;i++)
				{
					if(drug_recipe_table[i].tval == drug_compound_table[compound_result_num].tval
						&& drug_recipe_table[i].sval == drug_compound_table[compound_result_num].sval)
						break;
				}
				i /= 8;
				if(i < 4) //↑のループでiが32になるとレシピにない合成品なのでパス
				{
					int j;
					for(j=0;j<INVEN_PACK;j++)
					{
						if(!inventory[j].k_idx) break;
						if(inventory[j].tval == TV_BOOK_MEDICINE && inventory[j].sval == i)
						{
							flag_have_recipe = TRUE;
						}
					}
				}

			}

			//成功率を計算する。レシピ有り、作成済み、作成場所により難度変化
			chance = 60 + p_ptr->lev * 2 + (p_ptr->stat_ind[A_INT]+3) * 2 - drug_compound_table[compound_result_num].difficulty ;
			if(flag_have_recipe) chance += 20;
			if(flag_exp) chance += 10;
			if(p_ptr->town_num == TOWN_EIENTEI) chance += 10;//永遠亭調剤室では成功しやすい
			if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))	chance -= 20; //v1.1.44 うどんげも調剤できるが成功率は低い
			if(chance < 5) chance = 5;

			object_prep(prod_o_ptr, lookup_kind(drug_compound_table[compound_result_num].tval,drug_compound_table[compound_result_num].sval ));
			apply_magic(prod_o_ptr,p_ptr->lev,0L);//要らんと思うが一応
			prod_o_ptr->number = 1;

			
			object_desc(o_name, prod_o_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));

			if(flag_have_recipe || flag_exp)
				c_put_str(TERM_WHITE,format("完成品:%s", o_name),16,10);
			else
				c_put_str(TERM_WHITE,format("完成品:????"),16,10);
			c_put_str(TERM_WHITE,format("成功率:%d%%", MIN(chance,100)),17,10);

			if(!flag_have_recipe && !flag_exp)
				msg_format("何か作ったことのないものが出来そうだ！");
			else if(!compound_result_num)//炭
				msg_format("これでは%sにしかならない。",o_name);
			else if(flag_have_recipe && flag_exp)
				msg_format("%sを作る。",o_name);
			else 
				msg_format("%sが出来るはずだ。",o_name);

			if(!get_check_strict("調剤を開始しますか？", CHECK_OKAY_CANCEL)) continue;

			flag_make_drug_ok = TRUE;
			break;
		}

	}	
	screen_load();
	if(!flag_make_drug_ok) return;
	msg_print(NULL);

	//所持金と材料アイテムを減らす
	p_ptr->au -= total_cost;
	building_prt_gold();

	if(flag1)
	{
		inven_item_increase(item1, -amt);
		//inven_item_describe(item1);
		//inven_item_optimize(item1);
	}
	if(flag2)
	{
		inven_item_increase(item2, -amt);
		//inven_item_describe(item2);
		//inven_item_optimize(item2);
	}
	if(flag3)
	{
		inven_item_increase(item3, -amt);
		//inven_item_describe(item3);
		//inven_item_optimize(item3);
	}
	// -Hack- optimize()を1つずつ行うと、材料1が切れてアイテム欄が詰められ材料2の減少処理のスロットがずれる、ということが起こるためあとにまとめる
	//optimize()が必要なアイテムが並んでいると処理が飛ばされるので処理追加
	for(i=0;i<INVEN_PACK;i++)
	{
		if(!inventory[i].k_idx) break;
		if(!inventory[i].number)
		{
			inven_item_optimize(i);
			i--;
		}
	}

	screen_save();
	clear_bldg(0, 20);

	//作成中メッセージ
	c_put_str(TERM_WHITE,"あなたは調剤に取り掛かった・・",8 , 6);
	(void)inkey();


	if(randint0(100) < chance)//成功
	{
		rtv = drug_compound_table[compound_result_num].tval;
		rsv = drug_compound_table[compound_result_num].sval;

		c_put_str(TERM_WHITE,"調剤に成功した！",9 , 8);

		//作成済みフラグを立てる
		p_ptr->magic_num2[100+compound_result_num/8] |= (1L << compound_result_num % 8);
		flag_success = TRUE;

	}
	else//失敗
	{
		int cost_mod = cost / 2 + randint1(cost);

		if(cost_mod < 50)
		{
			rtv = TV_MATERIAL;
			rsv = SV_MATERIAL_COAL;
			c_put_str(TERM_WHITE,"材料が焦げてしまった。",9 , 8);
		}
		else if(cost_mod < 100)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_POISON;
			c_put_str(TERM_WHITE,"何だか変な臭いがしてきた・・",9 , 8);
		}
		else if(cost_mod < 300)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_TSUYOSHI;
			if(one_in_(3))c_put_str(TERM_WHITE,"調合中の薬から赤色の泡が噴き出した・・",9 , 8);
			else if(one_in_(2))c_put_str(TERM_WHITE,"調合中の薬から青色の泡が噴き出した・・",9 , 8);
			else c_put_str(TERM_WHITE,"調合中の薬から紫色の泡が噴き出した・・",9 , 8);
		}
		else if(cost_mod < 600)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_NEO_TSUYOSHI;
			c_put_str(TERM_WHITE,"何やら逞しい神様の大らかでセクシーな笑顔が見えた気がした・・",9 , 8);
		}
		else if(cost_mod < 1000)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_POISON2;
			if(one_in_(2)) 
				c_put_str(TERM_WHITE,"調合中の薬からドクロのような黒い煙が立ち昇った・・",9 , 8);
			else
				c_put_str(TERM_WHITE,"部屋のあちこちから鼠が這い出して一斉に逃げ出した。",9 , 8);
		}

		else if(cost_mod < 1200)
		{
			rtv = TV_COMPOUND;
			rsv = SV_COMPOUND_NECOMIMI;
			c_put_str(TERM_WHITE,"突如フラスコが爆発した！",9 , 8);
		}

		else if(cost_mod < 5000)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_POLYMORPH;
			if(one_in_(3))c_put_str(TERM_WHITE,"乳鉢が溶けて崩れた・・",9 , 8);
			else if(one_in_(2))c_put_str(TERM_WHITE,"蒸留器が虹色の液を吐き出した！",9 , 8);
			else c_put_str(TERM_WHITE,"大釜が不自然に捻くれた・・",9 , 8);
		}
		else if(cost_mod < 10000)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_RUINATION;
			if(one_in_(10)) 
				c_put_str(TERM_WHITE,"空からミスティアが落ちてきた。",9 , 8);
			else if(one_in_(2)) 
				c_put_str(TERM_WHITE,"空から鳥が落ちてきた。",9 , 8);
			else if(p_ptr->town_num == TOWN_EIENTEI)
				c_put_str(TERM_WHITE,"窓の外の竹林が一斉に枯れた。",9 , 8);
			else 
				c_put_str(TERM_WHITE,"窓の外の木々が一斉に枯れた。",9 , 8);
		}

		else if(cost_mod < 30000)
		{
			rtv = TV_COMPOUND;
			rsv = SV_COMPOUND_MAGICAL_ECO_BOMB;
			if(one_in_(2)) 
				c_put_str(TERM_WHITE,"調剤室の屋根が吹き飛んだ！",9 , 8);
			else if(EXTRA_MODE)
				c_put_str(TERM_WHITE,"爆音がダンジョンを揺るがした！",9 , 8);
			else if(p_ptr->town_num == TOWN_EIENTEI)
				c_put_str(TERM_WHITE,"爆音が竹林を揺るがした！",9 , 8);
			else 
				c_put_str(TERM_WHITE,"爆音が街を揺るがした！",9 , 8);
		}

		else if(cost_mod < 50000)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_DEATH;
			if(EXTRA_MODE)
				c_put_str(TERM_WHITE,"急にダンジョンが静けさに包まれた気がした。",9 , 8);
			else if(one_in_(2)) 
				c_put_str(TERM_WHITE,"空から妹紅が落ちてきた。",9 , 8);
			else if(p_ptr->town_num == TOWN_EIENTEI)
			{
				if(p_ptr->pclass == CLASS_EIRIN)
					c_put_str(TERM_WHITE,"ウドンゲが血相を変えて飛んできた！",9 , 8);
				else
					c_put_str(TERM_WHITE,"永琳が血相を変えて飛んできた！",9 , 8);
			}
			else 
				c_put_str(TERM_WHITE,"霊夢が「また薬屋か！」と言いながら扉を蹴破ってきた！",9 , 8);
		}
		else
		{
			rtv = TV_MATERIAL;
			rsv = SV_MATERIAL_MYSTERIUM;
			if(one_in_(2)) 
				c_put_str(TERM_WHITE,"調剤室の品々が瞬時に極彩色に染まった！",9 , 8);
			else
				c_put_str(TERM_WHITE,"調剤室の品々がブラックホールに吸い込まれた！",9 , 8);
		}

		c_put_str(TERM_WHITE,"調剤に失敗した・・",10 , 8);
		flag_success = FALSE;

	}

	object_prep(prod_o_ptr, lookup_kind(rtv,rsv));
	apply_magic(prod_o_ptr,p_ptr->lev,0L);//要らんと思うが一応
	if(flag_success && flag_exp && drug_compound_table[compound_result_num].max_num > 1
		&& cost > drug_compound_table[compound_result_num].base_cost) //既作成功時、難度と材料品質に応じて生成数が増える
	{
		int add=0;
		int max_num = drug_compound_table[compound_result_num].max_num;
		int mult = cost * (chance+100) / 2 / drug_compound_table[compound_result_num].base_cost;

		add = randint1(amt * mult)/100;
		if(add > (max_num-1) * amt) add = (max_num-1) * amt;
		amt += add;
		if(p_ptr->wizard) msg_format("cost:%d base_cost:%d max_num:%d mult:%d add:%d",cost,drug_compound_table[compound_result_num].base_cost,max_num,mult,add);
	}
	prod_o_ptr->number = amt;
	if(prod_o_ptr->number > 99) prod_o_ptr->number = 99;

	if(flag_success && flag_have_recipe) //レシピ通りに作った場合*鑑定*済みにする
	{
		object_aware(prod_o_ptr);
		object_known(prod_o_ptr);
		prod_o_ptr->ident |= (IDENT_MENTAL);
	}
	object_desc(o_name, prod_o_ptr, 0L);

	inven_carry(prod_o_ptr);
	msg_format("%sを入手した。",o_name);

	screen_load();
}


//小傘に対する鍛冶依頼
bool kogasa_smith(void)
{
	int     item,amt;
	cptr    q, s;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int	price;
	int new_val = p_ptr->lev/5 + 5;
	char	o_name[MAX_NLEN];

	if(p_ptr->pclass == CLASS_KOGASA)
	{msg_print("ERROR:小傘でプレイ中にkogasa_smith()が呼ばれた"); return FALSE;}


	//既に預けているものがある場合時間をチェックし半日経ってれば渡す
	if(inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH].k_idx)
	{
		int time_div;
		if(EXTRA_MODE) time_div = 8;
		else time_div = 1;

		// -Hack- 博麗神社武器屋の「最後に訪れたターン」の値を日数カウントに使っている	
		if((town[TOWN_HAKUREI].store[STORE_WEAPON].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) > turn ) && !p_ptr->wizard)
		{
			if(EXTRA_MODE)
				msg_print("小傘は作業中だ。");
			else
				msg_print("小傘はいない。");

			return FALSE;
		}
		else if(inventory[INVEN_PACK-1].k_idx)
		{
			msg_print("「バッチリ出来ていますよ！でも荷物を空けて来てください！」");
			return FALSE;
		}
		else
		{
			object_copy(q_ptr, &inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH]);
			object_wipe(&inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH]);
			//ついでに*鑑定*する
			identify_item(q_ptr);
			q_ptr->ident |= (IDENT_MENTAL);
			object_desc(o_name, q_ptr, (OD_OMIT_INSCRIPTION));

			switch(p_ptr->prace)
			{
			case RACE_HUMAN:
			case RACE_SENNIN:
			case RACE_TENNIN:
			case RACE_DEMIGOD:
			case RACE_JK:
				msg_print("「他の妖怪には内緒ですよ？」");
				break;
			default:
				msg_print("「バッチリ出来ていますよ！」");
				break;
			}

			msg_format("%sを受け取った。",o_name);

			inven_carry(q_ptr);

		}


	}
	//小傘に金を払って品物を預ける
	else
	{


		if(one_in_(4)) 		q = "「驚きの仕事しますよ」";
		else if(one_in_(3))	q = "「是非私に任せてくれませんか？」";
		else if(one_in_(2))	q = "「そこで私の出番ですよー！」";
		else			q = "「私は鍛冶も得意なんですよ」";
		s = "鍛冶を頼めるような金物を持っていない。";

		item_tester_hook = object_is_metal;
		if (!get_item(&item, q, s, (USE_INVEN))) return FALSE;

		if (item >= 0)	o_ptr = &inventory[item];
		else {msg_print("ERROR:kogasa_smith()のitemがマイナス"); return FALSE;}


		//軽い呪いでなく修正値が限界以上なら断られる
		if((object_is_weapon_ammo(o_ptr) && o_ptr->to_h >= new_val && o_ptr->to_d >= new_val
		|| object_is_armour(o_ptr) && o_ptr->to_a >= new_val)
		 && !(object_is_cursed(o_ptr) && !(o_ptr->curse_flags & TRC_PERMA_CURSE) && !(o_ptr->curse_flags & TRC_HEAVY_CURSE) ))
		{
			msg_print("「それはちょっとこれ以上鍛えられないかも...」");
			return FALSE;
		}

		if (o_ptr->number > 1)
		{
			amt = get_quantity(NULL, o_ptr->number);
			if (amt <= 0) return FALSE;
		}
		else amt = 1;

		//費用計算

		screen_save();
		clear_bldg(4,22);	

		object_copy(q_ptr, o_ptr);
		q_ptr->number = amt;
		object_desc(o_name, q_ptr, (OD_NAME_ONLY));


		if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_BROKEN_NEEDLE)
		{
			price = (new_val * new_val * 20 + 500) * amt;
			c_put_str(TERM_WHITE,format("%sを修復:(費用$%d)", o_name,price),10,10);
		}
		else
		{
			int base_value = object_value_real(o_ptr);
			if(base_value < 100) base_value = 100;
			if(object_is_artifact(o_ptr)) base_value += 10000;

			if(base_value > 100000) base_value = 100000;

			price = base_value / 100 * (new_val * 3 + 5);
			if(price < new_val * 100) price = new_val * 100;

			price *= amt;
			c_put_str(TERM_WHITE,format("%sを+%dまで強化:(費用$%d)", o_name,new_val,price),10,10);

		}
		if(one_in_(2))c_put_str(TERM_WHITE,"小傘「得意な鍛冶で何とか喰っているって感じ？」" ,9,9);
		else c_put_str(TERM_WHITE,"小傘「お代は勉強させて頂きます」" ,9,9);
		
		if(get_check_strict("依頼しますか？", CHECK_OKAY_CANCEL))
		{
			if(p_ptr->au < price)
			{
				msg_print("「悪いけど、もう前払いでしか受けないことにしたの。」");
			}
			else
			{
				p_ptr->au -= price;
				msg_format("$%d支払って小傘にアイテムを預けた。",price);
			
				inven_item_increase(item, -amt);
				inven_item_describe(item);
				inven_item_optimize(item);
				building_prt_gold();
				kogasa_smith_aux(q_ptr);
				if(EXTRA_MODE)
					msg_print("「お任せあれ！三時間後には出来ていますよ！」");
				else
					msg_print("「お任せあれ！明日の今頃には出来ていますよ！」");

				//q_ptrを一時インベントリに追加
				object_copy(&inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH], q_ptr);

				// -Hack- 武器屋の「最後に訪れたターン」の値を日数カウントに使う
				town[TOWN_HAKUREI].store[STORE_WEAPON].last_visit = turn;
			}

		}
		screen_load();

	}
	return TRUE;

}



/*:::Exダンジョン建物の汎用ルーチン*/
void exbldg_search_around(void)
{
	object_type forge;
	object_type *o_ptr = &forge;
	cptr msg1,msg1_2,msg2,msg3;
	char o_name[MAX_NLEN];
	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];
	int srh = p_ptr->skill_srh;
	int stl = p_ptr->skill_stl;
	bool flag_max_inven = FALSE;
	int price;
	bool houtou = FALSE;


	if(inventory[INVEN_PACK-1].k_idx) flag_max_inven = TRUE;


	switch(ex_bldg_idx)
	{
	case BLDG_EX_FLAN: //フランの地下室
		msg1 = "地下室だ。頑丈な作りだがあちこちに真新しいヒビが入っている・・";
		msg1_2 = "";
		msg2 = "辺りを探してみますか？";
		msg3 = "ここにはもう何もない。";
		break;

	case BLDG_EX_UDONGE: //うどんげの行商
		msg1 = "ダンジョンにホワイトボードが置かれ、兎の薬売りが脇に立っている。";
		msg1_2 = "";
		msg2 = "話しかけますか？";
		msg3 = "薬売りは帰っていった。";
		break;
	case BLDG_EX_SUWAKO: //諏訪子
		msg1 = "諏訪子が冬眠している。";
		msg1_2 = "";
		msg2 = "つついてみますか？";
		msg3 = "諏訪子はさらに深くへと潜って行った。";
		break;
	case BLDG_EX_IKUCHI: //キノコ
		msg1 = "沢山のキノコが輪のような形に連なって生えている。";
		msg1_2 = "";
		msg2 = "採取していきますか？";
		msg3 = "キノコの採取は済んだ。";
		break;

	case BLDG_EX_CHAOS_BLADE: //混沌の剣
		msg1 = "何かの内臓のような質感の部屋だ。";
		msg1_2 = "禍々しいオーラを放つ剣が一本浮かんでいる・・";
		msg2 = "触れてみますか？";
		msg3 = "もう何もない。";
		break;
	case BLDG_EX_ABANDONED: //放棄された実験室
		msg1 = "かなり荒れ果てているが、もとは魔法使いの研究室だったようだ。";
		msg1_2 = "";
		msg2 = "家探ししてみますか？";
		msg3 = "目ぼしいものは粗方探し終えた。";
		break;
	case BLDG_EX_AKISISTERS: //秋姉妹の屋台
		msg1 = "「私達、焼き芋屋さんを始めてみたの！」";
		msg1_2 = "";
		msg2 = "$100らしい。買いますか？";
		msg3 = "もう焼き芋は売り切れのようだ。";
		break;
	case BLDG_EX_SUKIMA: //結界のほころび
		msg1 = "結界の綻びがある。";
		msg1_2 = "";
		if(p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI)
			msg2 = "修復しますか？";
		else
			msg2 = "触れてみますか？";
		msg3 = "結界の綻びは消えた。";
		break;
	case BLDG_EX_PRINT_TENGU: //天狗の印刷所
		price = dun_level * 300;
		msg1 = "山伏天狗たちは休憩中のようだ。";
		msg1_2 = format("$%dでアイテムカードを複製してくれるらしい。",price);
		msg2 = "依頼しますか？";
		msg3 = "山伏天狗たちは再び忙しそうに仕事を始めている。";
		break;
	case BLDG_EX_ORC: //オーク
		msg1 = "ひどい悪臭のする小屋だ・・";
		msg1_2 = "";
		msg2 = "家探ししてみますか？";
		msg3 = "目ぼしいものは粗方探し終えた。";
		break;
	case BLDG_EX_OKINA: //v1.1.32 隠岐奈
		msg1 = "闇の中に大量の扉が浮いている空間だ。誰かが佇んでいる...";
		if(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_MARISA || p_ptr->pclass == CLASS_AYA || p_ptr->pclass == CLASS_CIRNO)
			msg1_2 = "隠岐奈「おや、また来たのか。加護でも授けてほしいのか？」";
		else if(p_ptr->pclass == CLASS_MAI || p_ptr->pclass == CLASS_SATONO)
			msg1_2 = "隠岐奈「任務は順調のようだな。それとも手助けが欲しいのか？」";
		else if (p_ptr->pclass == CLASS_SUMIREKO)
			msg1_2 = "隠岐奈「やあ、外来人。元気にしてた？」";
		else
			msg1_2 = "　「二童子の奴は何をしているんだ？妙な奴が迷い込んできたぞ。」";
		msg2 = "この場に留まりますか？";
		msg3 = "奇妙な空間はもうない。";
		break;

	case BLDG_EX_NAZRIN: //ナズーリンの掘っ立て小屋
		{
			int i;

			for(i=0;i<INVEN_TOTAL;i++) if(inventory[i].name1 == ART_HOUTOU) houtou = TRUE;
			if(p_ptr->pclass == CLASS_SHOU || p_ptr->pclass == CLASS_BYAKUREN) houtou = FALSE;

			if(houtou)
			{
				msg1 = "「…物は相談なんだが。";
				msg1_2 = "何も言わずにその宝塔を$1,000,000で売ってほしい。」";
				msg2 = "売りますか？";
				msg3 = "すでに宝塔を手放した。";
			}
			else
			{
				price = 100 + dun_level * 20;
				if(p_ptr->pclass == CLASS_SHOU || p_ptr->pclass == CLASS_BYAKUREN) price /= 5;
				msg1 = "「やあ、このフロアの地図があるんだが見ていくかい？」";
				msg1_2 = "";
				msg2 = format("$%dらしい。買いますか？",price);
				msg3 = "すでに地図を見せてもらっている。";
			}
		}
		break;
	case BLDG_EX_PUFFBALL: //オニフスベ
		msg1 = "大きな白い塊がある。これはレアキノコのオニフスベだ！";
		msg1_2 = "";
		msg2 = "採取していきますか？";
		msg3 = "キノコの採取は済んだ。";
		break;

	case BLDG_EX_LARVA: //ラルバの別荘
		if (p_ptr->pclass == CLASS_LARVA)
		{
			msg1 = "ここはあなたの別荘だ。";
			msg1_2 = "";
			msg2 = "使えそうなものを持っていきますか？";
			msg3 = "ここでの用事は済んだ。";
		}
		else
		{
			msg1 = "ここは妖精の住処のようだ。";
			msg1_2 = "木の葉や切り株の家具が設えられた暖かい小部屋だ。";
			msg2 = "家探ししていきますか？";
			msg3 = "もう使えそうなものはない。";

		}
		break;

	case BLDG_EX_KEIKI: //霊長園
		msg1 = "「新しい鎧を開発しているところよ。」";
		msg1_2 = "「もう一味違った素材を混ぜてみたいんだけど……」";
		msg2 = "素材を提供しますか？";
		msg3 = "造形神は気分良さげに別のものを作り始めている。";
		break;


	case BLDG_EX_ZASHIKI: //座敷わらしのテレワーク

		msg1 = "眼鏡をかけた座敷わらしが話しかけてきた...";
		if (p_ptr->prace == RACE_ZASHIKIWARASHI)
		{
			if(one_in_(3)) 
				msg1_2 = "「あなたが留守の間、私が代理で拠点を担当しますね」";
			else if (one_in_(2)) 
				msg1_2 = "「やはり時代はリモートね！でも快適なのにすごく仕事が増えている気がするの」";
			else 
				msg1_2 = "「テレワークもいいけどあなたのようなモバイルワークも楽しそうね」";

		}
		else
		{
			if (one_in_(3))
				msg1_2 = "「私があなたの探索拠点の担当になりました。お世話します。」";
			else if (one_in_(2))
				msg1_2 = "「少しだけあなたの探索をお手伝いしましょう」";
			else
				msg1_2 = "「あなたの家に泥棒が入らないのは私がちゃんと見張っているからですよ？」";

		}

		msg2 = "拠点を使いますか？";
		msg3 = "「それでは、探索頑張ってくださいね。」";
		break;




		default:
		msg_print("ERROR:exbldg_search_around()にこのidxのメッセージが登録されていない");
		return;
	}




	if(ex_buildings_param[ex_bldg_num] == 255 && !houtou) //捜索済みフラグ
	{
		prt(msg3,5,20);
		inkey();
		clear_bldg(4,18);
		return;
	}
	clear_bldg(4, 18);	

	//msgを表示し何もしないなら終了
	prt(msg1,10,10);
	prt(msg1_2,11,10);
	if (!get_check_strict(msg2, CHECK_DEFAULT_Y))
	{
		clear_bldg(4, 18);
		return;
	}

	//各処理部
	switch(ex_bldg_idx)
	{
	case BLDG_EX_FLAN: //フランの地下室
		{
			if(randint1(66) > srh || flag_max_inven)
			{
				msg_print("コインいっこ手に入れた。");
				p_ptr->au += 1;
			}
			else
			{
				object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
				apply_magic_itemcard(o_ptr,dun_level,MON_FLAN);//フランのカード入手
				object_desc(o_name,o_ptr,0);
				msg_format("%sを見つけた！",o_name);
				inven_carry(o_ptr);
			}
		}
		break;


	case BLDG_EX_UDONGE:
		{
			int tv,sv,num;

			//キャンセルされたときに再選定しないようパラメータを保存　薬を買ったら255になる
			if(!ex_buildings_param[ex_bldg_num]) ex_buildings_param[ex_bldg_num] = (randint1(dun_level)+randint1(127))/24;
			switch(ex_buildings_param[ex_bldg_num]) //0-10
			{
			case 0: //ウルトラソニック眠り猫
				tv = TV_SOUVENIR;
				sv = SV_SOUVENIR_ULTRASONIC;
				price = 980;
				num = 1;
				break;
			case 1: //筋肉増強剤
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_MUSCLE_DRUG;
				price = 100;
				num = 2+dun_level / 20;
				break;
			case 2: //幸福薬
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_HAPPY;
				price = 150;
				num = 4+dun_level/16;
				break;
			case 3: //火吹き薬
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_BREATH_OF_FIRE;
				price = 300;
				num = 2+dun_level/20;
				break;
			case 4: //ジキル薬
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_JEKYLL;
				price = 1000;
				num = 2+dun_level/30;
				break;
			case 5: //獣化薬(変身後Hp50%保証する？)
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_METAMOR_BEAST;
				price = 5000;
				num = 1+dun_level/40;
				break;
			default: //*爆発*の薬
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_STAR_DETORNATION;
				price = 10000;
				num = 2+dun_level / 40;
				break;
			}



			object_prep(o_ptr, (int)lookup_kind(tv, sv));
			apply_magic(o_ptr,dun_level,0L);
			o_ptr->number = num;
			price *= num;
			object_desc(o_name,o_ptr,0);

			clear_bldg(4, 18);
			prt(format("《 %s 特価！ $%d  》",o_name,price),6,21);	
			if(one_in_(3))prt("「お安くしますがいかがでしょう？」",7,22);	
			else if(one_in_(2))prt("「これこそ永琳様の手を借りずに私が開発した...」",7,22);	
			else prt("「私の能力を持ってすれば造作もない」",7,22);	

			if (get_check_strict("買いますか？", CHECK_DEFAULT_Y))
			{
				clear_bldg(4,18);
				if(p_ptr->au < price)
				{
					msg_print("「悪いけどここではツケは利かないの。」");
					return;
				}
				if(flag_max_inven)
				{
					msg_print("「荷物が多すぎるようね。」");
					return;
				}
				msg_format("%sを購入した。",o_name);
				p_ptr->au -= price;
				inven_carry(o_ptr);
				

			}
			else
			{
				if(p_ptr->prace == RACE_GYOKUTO || p_ptr->prace == RACE_LUNARIAN)
					msg_print("「このことは永琳様には内緒にしておいて下さい...」");
				else
					msg_print("「でしょうね。貴方に月の科学なんて理解できるはずもない。」");
				clear_bldg(4,18);
				return;
			}

		}
		break;
	case BLDG_EX_SUWAKO:
		{
			if(one_in_(13) || flag_max_inven)
			{
				msg_print("諏訪子はあなたをぼんやりと見た。");
				gain_random_mutation(188);

			}
			else
			{
				msg_print("「あーうー起こさないでよー。これあげるからー。」");
				object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
				apply_magic_itemcard(o_ptr,dun_level,MON_SUWAKO);//諏訪子のカード入手
				object_desc(o_name,o_ptr,0);
				msg_format("%sを受け取った。",o_name);
				inven_carry(o_ptr);
			}
		}
		break;
	case BLDG_EX_IKUCHI:
		{
			int tv,sv,num;
			tv = TV_MUSHROOM;
			sv = randint0(22); //POISON〜BERSERK
			num = 10+randint1(10);
			if(weird_luck()) num += 10+randint1(10);
			if(sv == SV_MUSHROOM_RESTORING) num /= 2;

			object_prep(o_ptr, (int)lookup_kind(tv, sv));
			apply_magic(o_ptr,dun_level,0L);
			o_ptr->number = num;
			object_desc(o_name,o_ptr,0);

			if(!flag_max_inven)
			{
				msg_format("%sを採取した！",o_name);
				inven_carry(o_ptr);
			}
			else
			{
				msg_format("キノコの妖精に睨まれた。やめておこう。");
			}

		}
		break;
	case BLDG_EX_PUFFBALL:
	{
		int tv, sv, num;
		tv = TV_MUSHROOM;
		sv = SV_MUSHROOM_PUFFBALL;
		num = randint1(3) + randint1(7);
		if (weird_luck()) num += 10;

		object_prep(o_ptr, (int)lookup_kind(tv, sv));
		apply_magic(o_ptr, dun_level, 0L);
		o_ptr->number = num;
		object_desc(o_name, o_ptr, 0);

		if (!flag_max_inven)
		{
			msg_format("%sを採取した！", o_name);
			inven_carry(o_ptr);
		}
		else
		{
			msg_format("キノコの妖精に睨まれた。やめておこう。");
		}

	}
	break;


	case BLDG_EX_CHAOS_BLADE:
		{
			if(one_in_(2) && !flag_max_inven)
			{
				object_prep(o_ptr, (int)lookup_kind(TV_SWORD, SV_WEAPON_BLADE_OF_CHAOS));
				apply_magic(o_ptr,dun_level,(AM_GOOD|AM_GREAT|AM_NO_FIXED_ART));
				msg_format("剣は宙を舞い、あなたの手に納まった。");
				inven_carry(o_ptr);

			}
			else
			{

				c_put_str(TERM_RED,"剣は宙を舞い、あなたに斬りかかってきた！",12,12);	
				hack_ex_bldg_summon_idx = MON_CHAOS_BLADE;
				hack_ex_bldg_summon_mode = (PM_NO_PET | PM_NO_ENERGY);
				inkey();
			}
		}
		break;
		//v1.1.32
	case BLDG_EX_OKINA:
		{
			bool flag_done = FALSE;

			clear_bldg(4, 18);
			prt("「良いだろう、気に入った！お前には...", 6, 21);
			inkey();
			while(!flag_done)
			{

				switch(randint1(6))
				{
				case 1:

					prt("後戸の神として、守護を授けてやろう！」",7,21);
					remove_all_curse();
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
					set_asthma(0);
					set_food(PY_FOOD_MAX-1);
					set_blessed(5000, FALSE);
					set_resist_magic(5000, FALSE);
					set_protevil(5000, FALSE);

					flag_done = TRUE;
					break;

				case 2:
					if (dun_level > 69 && !r_info[MON_OKINA].r_akills)
						hack_ex_bldg_summon_idx = MON_OKINA;
					else if (dun_level > 39 && !r_info[MON_MAI].r_akills)
						hack_ex_bldg_summon_idx = MON_MAI;
					else if (dun_level > 39 && !r_info[MON_SATONO].r_akills)
						hack_ex_bldg_summon_idx = MON_SATONO;
					else
						break;

					c_put_str(TERM_RED, "障碍の神として、試練をくれてやろう！」", 12, 21);

					hack_ex_bldg_summon_mode = (PM_NO_PET | PM_NO_ENERGY);
					flag_done = TRUE;
					break;
				case 3:
					{
						int stat = randint0(6);

						if (p_ptr->stat_max[stat] == p_ptr->stat_max_max[stat]) break;
						prt("能楽の神として、潜在能力を引き出してやろう！", 7, 21);
						do_inc_stat(stat);
						flag_done = TRUE;
						break;
					}
				case 4:
					{
						int check_num;
						int skill_index;

						//レベル1以上50未満の技能をランダムに選ぶ
						for (check_num = 50; check_num; check_num--)
						{
							skill_index = randint0(SKILL_EXP_MAX);
							if (p_ptr->skill_exp[skill_index] >= SKILL_LEV_TICK * 50)
								continue;
							if (p_ptr->skill_exp[skill_index] >= SKILL_LEV_TICK)
								break;
						}

						//何の技能も選ばれずループ終えたら終了
						if (!check_num)
							break;

						prt("宿神として、技能を高めてやろう！", 7, 21);

						//技能レベルが20上昇
						p_ptr->skill_exp[skill_index] += SKILL_LEV_TICK * 20;
						if (p_ptr->skill_exp[skill_index] > SKILL_LEV_TICK * 50)
							p_ptr->skill_exp[skill_index] = SKILL_LEV_TICK * 50;

						//職業適性による限界値を超えるようにしてみよう
						if (p_ptr->skill_exp[skill_index] > cp_ptr->skill_aptitude[skill_index] * SKILL_LEV_TICK * 10)
							msg_format("なんと、%sの技能が限界を超えて上がった！", skill_exp_desc[skill_index]);
						else
							msg_format("%sの技能が上がった！", skill_exp_desc[skill_index]);

						flag_done = TRUE;
						p_ptr->update |= PU_BONUS;
					}
					break;

				case 5:
					{

						prt("星神として、行く手の闇を払ってやろう！",7,21);	
						mass_genocide_3(0, FALSE, TRUE);
						wiz_lite(FALSE);

						flag_done = TRUE;
						break;
					}

				case 6:
					{
						if (flag_max_inven) break;

						prt("この幻想郷を創った賢者の一人として、力を貸してやろう！", 7, 21);
						object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
						apply_magic_itemcard(o_ptr, dun_level, MON_OKINA);//隠岐奈のカード入手
						object_desc(o_name, o_ptr, 0);
						msg_format("%sを受け取った。", o_name);
						inven_carry(o_ptr);
						flag_done = TRUE;

						break;
					}
					}
			}
			inkey();

		}
		break;

	case BLDG_EX_ABANDONED:
		{
			int power = randint1(srh);

			if(flag_max_inven)
			{
				msg_print("荷物が一杯だ。あとにしよう。");
				return;
			}
			clear_bldg(4,18);

			prt("あなたは荒れ果てた実験室を調べ始めた・・",7,20);	
			inkey();

			if(power > 50)
			{
				int book_tv = 1;
				int book_sv;

				if(one_in_(2)) book_tv = randint1(MAX_MAGIC);
				else if(cp_ptr->realm_aptitude[0] == 1 && p_ptr->realm1 != TV_BOOK_HISSATSU
					 || cp_ptr->realm_aptitude[0] == 3) book_tv = p_ptr->realm1;
				else if(p_ptr->pclass == CLASS_CHEMIST) book_tv = TV_BOOK_MEDICINE;
				else if(cp_ptr->realm_aptitude[0] == 2)
				{
					int j,cnt=0;
					for(j=1;j<=MAX_MAGIC;j++)
					{
						if(cp_ptr->realm_aptitude[j] >1 && rp_ptr->realm_aptitude[j])
						{
							cnt++;
							if(one_in_(cnt)) book_tv = j;
						}
					}
				}
				else book_tv = randint1(MAX_MAGIC);
				book_sv = (MIN(dun_level,100)/2 + randint0(100)) / 50+1;

				object_prep(o_ptr, lookup_kind(book_tv,book_sv));
				prt("前の住人の残した魔法書を見つけた！",8,21);
				inven_carry(o_ptr);

			}
			else if(power > 10 || dun_level < 30)
			{
				int cnt;

				for(cnt=10000;cnt>0;cnt--)
				{
					int value;
					object_wipe(o_ptr);
					make_object(o_ptr, 0L);
					value = object_value_real(o_ptr);
					if((o_ptr->tval == TV_POTION || o_ptr->tval == TV_SCROLL || o_ptr->tval == TV_STAFF || o_ptr->tval == TV_WAND || o_ptr->tval == TV_ROD )
					&& value > (srh+dun_level)*(p_ptr->lev / 5 + 2) ) break;
				}
				if(!cnt)
				{
					msg_print("WARNING:10000回生成しても使えそうな道具が出なかった");
					return;
				}	
				prt("使えそうな道具が残っているのを見つけた。",8,21);	
				prt("・・何かに見られている気がしたが、気のせいだろう。",9,21);	
				object_desc(o_name,o_ptr,0);
				msg_format("%sを入手した。",o_name);
				inven_carry(o_ptr);

			}
			else
			{
				c_put_str(TERM_RED,"ああ、窓に、窓に！",12,20);	
		
				hack_ex_bldg_summon_mode = (PM_ALLOW_UNIQUE | PM_NO_PET | PM_NO_ENERGY);
				hack_ex_bldg_summon_type = SUMMON_INSANITY;
			}
			inkey();

		}
		break;

		case BLDG_EX_AKISISTERS:
		{
			int tv,sv,num;

			tv = TV_SWEETS;
			sv = SV_SWEET_POTATO;
			price = 100;
			num = 1+randint1(2);

			object_prep(o_ptr, (int)lookup_kind(tv, sv));
			apply_magic(o_ptr,dun_level,0L);
			o_ptr->number = num;
			object_desc(o_name,o_ptr,0);

			clear_bldg(4, 18);

			if(flag_max_inven)
			{
				msg_print("「あら、荷物が多すぎるようね。」");
				return;
			}
			if(p_ptr->au < price)
			{
				msg_print("「お腹を空かしてる人を見捨てたら神の名が廃るわ！いいから持っていきなさい！」");
				//v1.1.47 所持金がマイナスのときに借金0にならないよう条件追加
				if(p_ptr->au > 0)	p_ptr->au = 0;
			}
			else
			{
				msg_print("「ちょっとオマケをつけてあげるわ。」");
				msg_format("%sを購入した。",o_name);
				p_ptr->au -= price;
			}
			inven_carry(o_ptr);
		}
		break;

		case BLDG_EX_SUKIMA:
		{
			int i;
			int tester;
			for(i=100;i>0;i--)
			{
				tester = randint1(p_ptr->skill_dis);//職によるが30-100くらい
				if(tester > 25)
				{
					msg_print("スキマの中に探索拠点の光景が見える。一度だけ拠点を利用できそうだ！");
					inkey();
					hack_flag_access_home = TRUE;
					do_cmd_store();
					hack_flag_access_home = FALSE;
					//建物から別の建物に入ったのでフラグ再設定と再描画
					character_icky = TRUE; 
					show_building(&building[ex_bldg_num]);
				}
				else if(tester > 10 || p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI)
				{
					if(flag_max_inven) continue; //アイテムがいっぱいだったら再判定

					msg_print("何かがスキマからこぼれ落ちた。");
					object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
					apply_magic_itemcard(o_ptr,dun_level,MON_YUKARI);//紫のカード入手
					object_desc(o_name,o_ptr,0);
					msg_format("%sを見つけた！",o_name);
					inven_carry(o_ptr);

				}
				else
				{
					msg_print("あなたはスキマに呑み込まれた！");
					gain_random_mutation(0);
				}
				break;
			}
		}
		break;
		case BLDG_EX_PRINT_TENGU:
		{
			int item;
			cptr q, s;
			if(p_ptr->au < price)
			{
				msg_print("お金が足りない。");
				return;
			}

			q = "どれを複製してもらいますか？";
			s = "アイテムカードを持っていない。";

			item_tester_tval = TV_ITEMCARD;
			if (!get_item(&item, q, s, (USE_INVEN))) return ;

			if(inventory[item].number >= 99)
			{
				msg_print("それはもう持ちきれないほどある。");
				return;
			}
			o_ptr = &inventory[item];
			o_ptr->number += support_item_list[inventory[item].pval].prod_num;
			if(o_ptr->number > 99) o_ptr->number = 99;
			object_desc(o_name,o_ptr,OD_OMIT_PREFIX);
			msg_format("%sを複製してもらった！",o_name);
			p_ptr->au -= price;

		}
		break;
		case BLDG_EX_ORC:
		{
			int power = randint1(stl);

			if(flag_max_inven)
			{
				msg_print("荷物が一杯だ。あとにしよう。");
				return;
			}
			clear_bldg(4,18);

			prt("あなたは悪臭漂う小屋を調べ始めた・・",7,20);	
			inkey();

			if(power < randint1(3))
			{
				c_put_str(TERM_RED,"ここはオークの基地だった。戻ってきたオーク達に囲まれている！",12,20);	
		
				hack_ex_bldg_summon_mode = (PM_ALLOW_UNIQUE | PM_ALLOW_GROUP | PM_NO_PET | PM_NO_ENERGY);
				if(dun_level < 15) hack_ex_bldg_summon_idx = MON_ORC_CAPTAIN;
				else hack_ex_bldg_summon_type = SUMMON_ORCS;
			}
			else if(weird_luck())
			{
				object_prep(o_ptr, (int)lookup_kind(TV_ARMOR, SV_ARMOR_MITHRIL_CHAIN));
				apply_magic(o_ptr,dun_level,AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
				object_desc(o_name,o_ptr,0);
				prt("見事な造りの鎖帷子を見つけた！",8,21);	
				inven_carry(o_ptr);
			}
			else if(one_in_(2))
			{
				object_prep(o_ptr, (int)lookup_kind(TV_FOOD, SV_FOOD_VENISON));
				o_ptr->number = 10+randint1(10);
				prt("ちょっと臭う食料を見つけた。",8,21);	
				inven_carry(o_ptr);

			}
			else
			{
				object_prep(o_ptr, (int)lookup_kind(TV_ALCOHOL, SV_ALCOHOL_ORC));
				o_ptr->number = 2+randint1(3);
				prt("匂いのきつい酒のような液体を見つけた。",8,21);	
				inven_carry(o_ptr);
			}
			inkey();

		}
		break;

		case BLDG_EX_NAZRIN:
		{

			clear_bldg(4, 18);

			if(houtou)
			{
				int item;
				for(item=0;item<INVEN_TOTAL;item++) if(inventory[item].name1 == ART_HOUTOU) break;

				msg_print("宝塔を手放した。");
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
				clear_bldg(4,18);
				p_ptr->update |= (PU_BONUS);
				p_ptr->window |= (PW_INVEN | PW_EQUIP);

				p_ptr->au += 1000000;
				building_prt_gold();


				return;//建物利用可能フラグを建てない
			}
			else
			{
				if(p_ptr->au < price)
				{
					msg_print("「何だ文無しか。悪いがこちらも商売なんでね。」");
					return;
				}
				else
				{
					msg_format("地図を見せてもらった。",o_name);
					wiz_lite(FALSE);
					p_ptr->au -= price;
				}
			}
		}
		break;
		case BLDG_EX_LARVA:
		{
			int power = randint1(4);

			if (p_ptr->pclass == CLASS_LARVA) power += randint1(2);

			if (flag_max_inven)
			{
				msg_print("荷物が一杯だ。あとにしよう。");
				return;
			}
			clear_bldg(4, 18);

			prt("あなたは部屋を調べ始めた・・", 7, 20);
			inkey();

			switch (power)
			{
			case 1:
			{
				object_prep(o_ptr, (int)lookup_kind(TV_FOOD, SV_FOOD_VENISON));
				o_ptr->number = 3 + randint1(3);
				prt("食料を見つけた。", 8, 21);
				inven_carry(o_ptr);
				break;
			}
			case 2:
			{
				object_prep(o_ptr, (int)lookup_kind(TV_MUSHROOM, SV_MUSHROOM_MANA));
				o_ptr->number = 3 + randint1(3);
				prt("美味しそうなキノコを見つけた。", 8, 21);
				inven_carry(o_ptr);
				break;
			}
			case 3:
			{
				object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
				apply_magic_itemcard(o_ptr, dun_level, MON_LARVA);
				object_desc(o_name, o_ptr, 0);
				msg_format("%sを見つけた。", o_name);
				inven_carry(o_ptr);
				break;
			}
			default:
			{
				object_prep(o_ptr, (int)lookup_kind(TV_LITE, SV_LITE_FEANOR));
				apply_magic(o_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD);
				prt("魔法のランタンを見つけた。", 8, 21);
				inven_carry(o_ptr);
				break;
			}
			}
			inkey();

		}
		break;

		case BLDG_EX_KEIKI:
		{
			int tv, sv, num, i , power;
			int item;
			int am_mode;
			cptr q, s;
			object_type *tmp_o_ptr;

			tv = TV_ARMOR;
			sv = SV_ARMOR_FINE_CERAMIC;

			clear_bldg(4, 18);

			if (flag_max_inven)
			{
				msg_print("「荷物が多すぎるようねぇ。」");
				return;
			}

			q = "何か素材を提供しますか？";
			s = "丁度よい素材を持っていない。";

			//とりあえず武器修復と同じ素材を使う。ただし武器は選択できない
			item_tester_hook = item_tester_hook_repair_material;
			if (!get_item(&item, q, s, (USE_INVEN))) return;
			tmp_o_ptr = &inventory[item];

			//武器修復素材テーブルを見てSV決定用パワーを開発パワーに転用
			power = -1;
			for (i = 0; repair_weapon_power_table[i].tval; i++)
				if (repair_weapon_power_table[i].tval == tmp_o_ptr->tval && repair_weapon_power_table[i].sval == tmp_o_ptr->sval)
					power = repair_weapon_power_table[i].type;
			if (power == -1) { msg_format("ERROR:武器修復でこの修復素材のsv_powerが登録されていない"); return ; }

			//素材を減らす
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);

			//ファインセラミックスの鎧のパラメータ処理
			//素材が低品質だと呪われやすいようにしてみる(apply_magic()の処理内部の都合上常に呪われるわけではないらしいがあまり見てない)
			am_mode = AM_GOOD;
			if(randint0(power)<3)am_mode = AM_CURSED;

			object_prep(o_ptr, (int)lookup_kind(tv, sv));
			apply_magic(o_ptr, dun_level, am_mode);
			o_ptr->number = 1;

			//power/4回上位耐性付与　確率で全耐性
			for (i=4; i<power; i += 4)
			{
				if (weird_luck())
				{
					add_flag(o_ptr->art_flags, TR_RES_ACID);
					add_flag(o_ptr->art_flags, TR_RES_ELEC);
					add_flag(o_ptr->art_flags, TR_RES_FIRE);
					add_flag(o_ptr->art_flags, TR_RES_COLD);
					if(one_in_(3)) 		add_flag(o_ptr->art_flags, TR_RES_POIS);

				}
				else if (one_in_(4))
				{
					one_ele_resistance(o_ptr);
				}
				else
				{
					one_high_resistance(o_ptr);
				}
			}
			//鍛冶品扱いにする
			o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER;

			clear_bldg(4, 18);

			if(object_is_cursed(o_ptr))
				prt("Oh!", 10, 25);
			else 
				prt("Create!", 10, 25);

			inkey();

			inven_carry(o_ptr);

		}
		break;

		case BLDG_EX_ZASHIKI://座敷わらし　拠点利用
		{
			hack_flag_access_home = TRUE;
			do_cmd_store();
			hack_flag_access_home = FALSE;
			//建物から別の建物に入ったのでフラグ再設定と再描画
			character_icky = TRUE;
			show_building(&building[ex_bldg_num]);
			break;
		}
		default:
		msg_print("ERROR:exbldg_search_around()にこの建物の処理が登録されていない");
	}

	//建物使用完了フラグ
	ex_buildings_param[ex_bldg_num] = 255;

	p_ptr->redraw |= (PR_GOLD);
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	building_prt_gold();
	clear_bldg(4, 18);
	
	
	return;

}

/*:::Exモード限定。お燐が家からアイテムを運んでくれる。依頼時にTRUE(料金発生)*/
//これを呼ぶ前にhack_flag_access_home_orinを切り替えること
bool orin_deliver_item(void)
{
	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];
	object_type *o_ptr = &inven_special[INVEN_SPECIAL_SLOT_ORIN_DELIVER];

	//このフロアで実行済み
	if(ex_buildings_param[ex_bldg_num] == 255)
	{
		prt("火焔猫燐「ここには活きのいい死体がありそうだなー。",5,10);
		prt("一仕事済んだしちょいと探しに行こうか。」",6,10);
		inkey();
		clear_bldg(4,18);
		return FALSE;
	}
	//依頼していない
	else if(!o_ptr->k_idx)
	{
		//hack_flag_access_home_orinがTRUEになっていれば専用の選択モードになる
		do_cmd_store();
		//建物から別の建物に入ったのでフラグ再設定と再描画
		character_icky = TRUE; 
		show_building(&building[ex_bldg_num]);

		if(!o_ptr->k_idx) return FALSE;

		if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
			msg_print("「それじゃ行ってきます。一時間くらいで戻りますんで。」");
		else
			msg_print("「そんじゃ取ってくるよ。一時間くらい掛かるかな？」");

		return TRUE;
	}
	else
	{
		int time_div = 24; //一時間
		//Hack - 博麗神社雑貨屋のタイムカウンタを使う
		if((town[TOWN_HAKUREI].store[STORE_GENERAL].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) > turn ) && !p_ptr->wizard)
		{
			prt("誰もいない...",5,20);
			inkey();
			clear_bldg(4,18);
		}
		else if (!inven_carry_okay(o_ptr))
		{
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				prt("火焔猫燐「持ってきましたよー。ありゃ、持ちきれないんですか？」",5,20);
			else
				prt("火焔猫燐「ちょっとちょっと、折角はるばる運んできたってのに受け取れないのかい？」",5,20);
			inkey();
			clear_bldg(4,18);
		}
		else
		{
			char o_name[MAX_NLEN];
			object_desc(o_name, o_ptr, 0L);
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI || p_ptr->pclass == CLASS_UTSUHO)
				prt("お燐がアイテムを持って戻ってきた。",5,20);
			else
				prt("火車がアイテムを持って戻ってきた。",5,20);
			inkey();
			msg_format("%sを受け取った。ちょっと腐臭がする..",o_name);
			clear_bldg(4,18);
			inven_carry(o_ptr);
			object_wipe(o_ptr);

			//建物使用完了フラグ
			ex_buildings_param[ex_bldg_num] = 255;

		}

		return FALSE;	
	}

}

//1.1.51 新アリーナ関連
//今回見る悪夢をクリアしたら得られるポイントをnightmare_mon_idx[]から計算する。可変パラメータユニークのパラメータ計算が済んでいること
static	s32b	calc_nightmare_points(void)
{

	int i;
	s32b total_points = 0;

	for (i = 0; i<NIGHTMARE_DIARY_MONSTER_MAX; i++)
	{
		int lev;
		int r_idx = nightmare_mon_r_idx[i];

		if (r_idx < 0 || r_idx >= max_r_idx)
		{
			msg_format("ERROR:nightmare_mon_r_idx[]に記録されたr_idx値が不正(%d)", r_idx);
			return 0;
		}

		lev = r_info[r_idx].level;

		total_points += (lev * lev * lev / 10);
		/*
		lev10:100
		lev20:800
		lev30:2700
		lev40:6400
		lev50:12500
		lev60:21600
		lev70:34300
		lev100:100000
		lev127:200000
		*/

	}

	return total_points;

}



/*:::新アリーナ(夢の世界)用のモンスター選定関数*/
//ここではnightmare_spellname_table[]にスペカ名を登録したキャラのみ選定することにする。
//is_gen_unique()で判定するとスペカを持たないモンスターも選定されてしまうため、
//不本意ながらハードコーディングで設定することにした。
//今後スペカ持ちの敵キャラを増やしたらこことnightmare_spellname_table[]に追記する。
static bool monster_hook_nightmare_diary(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_idx == MON_MASTER_KAGUYA) return FALSE;//輝夜(可変)は出ない
	if (r_idx == MON_SEIJA) return FALSE;//正邪は出ない
	if (r_idx == MON_SUMIREKO) return FALSE; //菫子(お尋ね者バージョン)は出ない

	//v1.1.98 瑞霊は出ない
	if (r_idx == MON_MIZUCHI) return FALSE;

	//＠が霊夢と魔理沙のときは可変パラメータのMON_REIMUとMON_MARISAを自分として出す。
	//それ以外のときはランダムユニーク1を自分として出し、monster_is_you()に当てはまる自分モンスターは出さない。
	//ただしドレミーと菫子新性格の場合はどちらも出ない。
	//なお、monster_is_you()では演出の都合上自分以外にも2P扱いになるモンスターがいる(永琳プレイ中の輝夜など)ので、
	//それらのモンスターも悪夢世界で発生抑止される。
	if (p_ptr->pclass != CLASS_REIMU && p_ptr->pclass != CLASS_MARISA)
	{
		if (monster_is_you(r_idx)) return FALSE;
		if (p_ptr->pclass != CLASS_DOREMY && !is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO) && r_idx == MON_RANDOM_UNIQUE_1) return TRUE;
	}

	//菫子は闘技場用のを出す
	if (p_ptr->pclass != CLASS_SUMIREKO)
	{
		if (r_idx == MON_SUMIREKO_2) return TRUE; 
	}

	if (r_idx >= MON_RUMIA && r_idx <= MON_SUWAKO) return TRUE;
	//リリーホワイト除く
	if (r_idx >= MON_KISUME && r_idx <= MON_SUIKA) return TRUE;
	//ミニ萃香除く
	if (r_idx >= MON_IKU && r_idx <= MON_EIKI) return TRUE;
	//アリス人形除く
	if (r_idx == MON_HATATE) return TRUE;
	if (r_idx == MON_REIMU) return TRUE;
	if (r_idx == MON_MARISA) return TRUE;
	if (r_idx == MON_MAMIZOU) return TRUE;

	if (r_idx == MON_3FAIRIES) return TRUE;

	if (r_idx >= MON_SEIRAN && r_idx <= MON_HECATIA3) return TRUE;
	//依姫豊姫レイセン�Uはスペカがないので出せない
	if (r_idx >= MON_LARVA && r_idx <= MON_OKINA) return TRUE;
	if (r_idx >= MON_JYOON && r_idx <= MON_SHION_2) return TRUE;

	//v1.1.64 鬼形獣
	if (r_idx >= MON_EIKA && r_idx <= MON_SAKI) return TRUE;

	//美宵は出せない

	//v1.1.86 虹龍洞
	if (r_idx >= MON_MIKE && r_idx <= MON_MOMOYO) return TRUE;

	//v1.1.91 剛欲異聞
	if (r_idx == MON_YUMA) return TRUE;

	return FALSE;
}






//v1.1.51 新アリーナ開始ルーチン
//メニューから悪夢レベルを選択し、そのレベルに応じたモンスター最大5体を選定してnightmare_mon_r_idx[]に記録する。
//さらに勝ったときに得られるイイッすね！ポイント(仮)を計算してnightmare_pointに記録する。
//キャンセルしたときはFALSE
bool	nightmare_diary(void)
{
	int i;
	int monster_num;
	int nightmare_lev_limit;
	int nightmare_level;
	char c;
	int loop_count;
	bool	flag_err = FALSE;

	//クラウンピース(HP30000のほう)は参加不可にしとく
	if (p_ptr->pclass == CLASS_CLOWNPIECE)
	{
		msg_print("あなたは本を見せてもらえなかった。");
		return FALSE;
	}


	screen_save();


	//挑戦可能な悪夢レベルを判定
	for (i = 0; i<NIGHTMARE_DIARY_STAGE_LEV_MAX; i++)
	{
		if (nightmare_total_point < nightmare_stage_table[i].need_points) break;
	}
	nightmare_lev_limit = i - 1;

	if (p_ptr->wizard) nightmare_lev_limit = NIGHTMARE_DIARY_STAGE_LEV_MAX - 1;

	//悪夢ステージ一覧をメニュー表示
	clear_bldg(4, 22);
	if (p_ptr->pclass == CLASS_DOREMY)
		prt("「どんな夢を食べようかしら...」", 7, 20);
	else if (nightmare_lev_limit == (NIGHTMARE_DIARY_STAGE_LEV_MAX - 1))
	{
		prt("ドレミー「可哀想に。貴方はもう悪夢から逃れられない……」", 7, 20);
	}
	else if (nightmare_total_point > 100000)
	{
		prt("ドレミー「また来てしまったのですね……覚悟は出来てますか？」", 7, 20);
	}
	else
	{
		prt("ドレミー「あら、この本に書かれていることがそんなに気になるのですか？」", 7, 20);
	}
	for (i = 0; i <= nightmare_lev_limit; i++)
	{
		prt(format("%c) %s", ('a' + i), nightmare_stage_table[i].desc), 9 + i, 22);
	}
	prt(format("戦歴:%d戦%d勝", nightmare_diary_count, nightmare_diary_win), 18, 30);
	prt(format("累計評価点:%d", nightmare_total_point), 19, 30);

	//選択を受け付けて悪夢レベルを決定
	while (1)
	{
		c = inkey();
		if (c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		nightmare_level = c - 'a';
		if (nightmare_level < 0 || nightmare_level > nightmare_lev_limit) continue;

		break;
	}

	//get_mon_num_prep()に使用するのでこの時点でinside_arenaフラグを立てる。途中終了する場合FALSEにしてから出る
	p_ptr->inside_arena = TRUE;


	//悪夢レベルに合わせてモンスターを最大5体選定
	for (loop_count = 100; loop_count; loop_count--)
	{
		int j, k;
		bool flag_ng = FALSE;

		//変数初期化
		tmp_nightmare_point = 0;
		for (i = 0; i<NIGHTMARE_DIARY_MONSTER_MAX; i++)nightmare_mon_r_idx[i] = 0;

		//出てくるモンスター数決定
		switch (nightmare_level)
		{
		case 0:		case 1:		case 2:
			monster_num = 2;
			break;
		case 3:
			monster_num = 2 + randint0(2);
			break;
		case 4:
			monster_num = 2 + randint0(3);
			break;
		case 5:		case 6:
			monster_num = 3 + randint0(3);
			break;
		default:
			monster_num = 5;
			break;
		}

		//モンスターレベルを設定
		nightmare_mon_base_level = nightmare_stage_table[nightmare_level].level;
		if (nightmare_level == (NIGHTMARE_DIARY_STAGE_LEV_MAX - 1)) nightmare_mon_base_level += randint0(30);
		if (nightmare_mon_base_level > 127) nightmare_mon_base_level = 127;

		// -Hack- 取得ポイント計算とモンスター選定のために可変パラメータモンスターのレベル値だけを強引に書き換える。実際のパラメータ決定はnightmare_gen()でのモンスター配置時に行う。
		r_info[MON_REIMU].level = nightmare_mon_base_level;
		r_info[MON_MARISA].level = nightmare_mon_base_level;
		r_info[MON_RANDOM_UNIQUE_1].level = nightmare_mon_base_level;

		//msg_format("mon_lev:%d", nightmare_mon_base_level);

		//モンスターを選定
		get_mon_num_prep(monster_hook_nightmare_diary, NULL);
		for (j = 0; j<monster_num; j++)
		{
			nightmare_mon_r_idx[j] = get_mon_num(nightmare_mon_base_level);
		}

		//for (j = 0; j < monster_num; j++)			msg_format("gmn  r_idx:%d",nightmare_mon_r_idx[j]);

		//同じモンスターが被るとやり直し 紫苑とスーパー紫苑も被らないようにする
		for (j = 0; j<monster_num - 1; j++)
		{
			for (k = j+1; k<monster_num; k++)
			{
				if (nightmare_mon_r_idx[j] == nightmare_mon_r_idx[k]) flag_ng = TRUE;
				if (nightmare_mon_r_idx[j] == MON_SHION_1 && nightmare_mon_r_idx[k] == MON_SHION_2) flag_ng = TRUE;
				if (nightmare_mon_r_idx[j] == MON_SHION_2 && nightmare_mon_r_idx[k] == MON_SHION_1) flag_ng = TRUE;
			}
		}

		//v1.1.52 モンスターが2体しかいないときに夢の世界の＠は出ないようにする。
		//合成スペカ名が登録されてない職業のとき文字数が足りずにエラーになるため。
		for (j = 0; j < monster_num; j++)
		{
			if (monster_num < 3 && nightmare_mon_r_idx[j] == MON_RANDOM_UNIQUE_1) flag_ng = TRUE;
		}

		if (flag_ng) continue;

		//このステージをクリアして得られるポイントが指定範囲外だとやり直し
		tmp_nightmare_point = calc_nightmare_points();

//msg_format("points:%d", tmp_nightmare_point);
		if (tmp_nightmare_point < nightmare_stage_table[nightmare_level].min_points || tmp_nightmare_point > nightmare_stage_table[nightmare_level].max_points) flag_ng = TRUE;
		if (flag_ng) continue;

		break;
	}

	if (!loop_count)
	{
		msg_format("ERROR:悪夢レベル%dにおいてモンスター生成処理試行回数が規定値を超えた", nightmare_level);
		screen_load();
		p_ptr->inside_arena = FALSE;
		return FALSE;
	}

	if (cheat_xtra) msg_format("nightmare_lev:%d", nightmare_level);

	//test
	/*
	nightmare_mon_base_level = 30;
	nightmare_mon_r_idx[0] = MON_REIMU;
	nightmare_mon_r_idx[1] = MON_MARISA;
	nightmare_mon_r_idx[2] = MON_RANDOM_UNIQUE_1;
	nightmare_mon_r_idx[3] = 0;
	nightmare_mon_r_idx[4] = 0;
	*/

	tmp_nightmare_point = calc_nightmare_points();

	//相手が決定してr_idxとポイントを記録したら、アリーナ関係のフラグを立てて建物から出る
	screen_load();

	p_ptr->exit_bldg = FALSE;
	reset_tim_flags();
	prepare_change_floor_mode(CFM_SAVE_FLOORS);

	p_ptr->leaving = TRUE;

	//v1.1.55 アリーナに入るときHPを記録しておいて出るとき復旧することにした。
	//アリーナ内で敗北したらHP全快で出ることにしていたが、アリーナ内で瀕死になって出た場合審判の宝石とかでゲームオーバーになる可能性があるのは変な話なので再修正
	nightmare_record_hp = p_ptr->chp;

	//新アリーナ挑戦回数に加算
	nightmare_diary_count++;

	return TRUE;

}



/*:::v1.1.56
*真・勝利後限定で一品限りの珍品を買い集める建物用コマンドを追加。
*単に長期プレイヤー向けに金の使い道を提供する(兼これまで出しづらかった一品物の公式珍品ネタを無理やり使う)目的なので珍品に用途はない。
*一部ユニーククラスでは自分が持っているはずの品を高値で買わされるかもしれないがご容赦願おう。
*アイテムはTV=STRANGE_OBJECT,SV=p_ptr->kourindou_numberの品がSV=0から順に売り出される。
*アイテムを追加するにはk_info.txtに追記してからSV_STRANGE_OBJ_MAXを書き換える。
*/
void	bact_buy_strange_object(void)
{

	char o_name[256];
	object_type	forge;
	object_type 	*o_ptr = &forge;
	int		k_idx, price;

	//アイテム紹介時に適当に表示する文句
	cptr random_phrase[] =
	{
		"この僕も初めてお目にかかる逸品",		"天竺に二つと無しと称された品",		"天下の名品と誉れ高い品",
		"もはや現存しないとされていた品",		"かの松永秀久も愛でたという品",		"世に並ぶものなき珍品",
		"なんと外界の品",						"なんと異界の品",
	};

	if (!PLAYER_IS_TRUE_WINNER && !p_ptr->wizard)
	{
		msg_print("ERROR:*真・勝利*していないのにbuy_strange_object()が呼ばれた");
		return;
	}

	//既に全ての珍品を買い取り終えた場合
	if (p_ptr->kourindou_number > SV_STRANGE_OBJ_MAX)
	{
		prt("霖之助「申し訳ないがもう粗方の名品珍品は君に売ってしまった。", 7, 10);
		prt("いずれ何か仕入れておくよ。」", 8, 20);
		return;
	}

	/*::: Hack - 香霖堂の「我が家」の「最後に訪れたターン」の値を使って一日一度しか買えないことにする*/
	/*..と思ったがこんなところに来るプレーヤーはターン上限に達してるかもしれんのでやめとく
	if ((town[TOWN_KOURIN].store[STORE_HOME].last_visit > turn - (TURNS_PER_TICK * TOWN_DAWN)) && !p_ptr->wizard )
	{
		prt("霖之助「先ほどの名品はお気に召したかな？また来てくれたまえ。」", 7, 10);
		return;
	}
	clear_bldg(4, 18);
	*/

	k_idx = lookup_kind(TV_STRANGE_OBJ, p_ptr->kourindou_number);
	if (!k_idx)
	{
		msg_format("ERROR:SV=%dのアイテムが実在しない", p_ptr->kourindou_number);
		return;
	}

	//リストからアイテム生成
	object_prep(o_ptr, k_idx);
	object_desc(o_name, o_ptr, 0);
	price = k_info[k_idx].cost;

	//画面に表示し買い取り確認
	prt(format("「おっと、この%sに目を付けるとはお目が高い。",o_name), 7, 10);
	prt(format("　これは%sで、", random_phrase[randint0(sizeof(random_phrase) / sizeof(cptr))] ), 8, 10);
	prt(format("　値段は$%dだ。」", price), 9, 10);
	if (!get_check_strict("この品を買い取りますか？", CHECK_DEFAULT_Y))
	{
		clear_bldg(4, 18);
		return;
	}

	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print("荷物が一杯だ。");
		clear_bldg(4, 18);
		return;
	}

	if (p_ptr->au < price)
	{
		msg_print("お金が足りない。");
		clear_bldg(4, 18);
		return;
	}

	p_ptr->au -= price;
	building_prt_gold();
	p_ptr->kourindou_number++;
	inven_carry(o_ptr);
	msg_print("珍品を買い取った！");

	clear_bldg(4, 18);

	/*::: Hack - 香霖堂の「我が家」の「最後に訪れたターン」の値を使って一日一度しか買えないことにする*/
	//town[TOWN_KOURIN].store[STORE_HOME].last_visit = turn;
}




//v2.0.5
//はたての家で念写を使って人探しをしてもらう
//内部的には指定された文字列でユニークモンスターを検索し、その居場所としてランダムなダンジョンとフロアを設定し、
//＠がそこに行ったときそのモンスターを生成する。
//p_ptr->hatate_mon_search_ridx:対象ユニークモンスターのr_idx値
//p_ptr->hatate_mon_search_dungeon: (ダンジョンidx*1000) + (ダンジョン階層)
//日付が変わったらこの値はリセットされる。生成しただけではリセットされない
//成功(費用発生)のときTRUEを返す
#define HATATE_SEARCH_MON_LIST_MAX 10 //候補リスト最大長
bool hatate_search_unique_monster(void)
{

	bool hatate = (p_ptr->pclass == CLASS_HATATE);

	//すでに実行済みのとき翌日まで利用不可　
	//＠がはたてのときは何度でもできるようにする
	if (p_ptr->hatate_mon_search_ridx)
	{
		if (hatate)
		{
			prt("すでに追跡中の相手がいるが...", 7, 20);
			prt("　やめて他の人を探そうか？", 8, 20);
			if (!get_check_strict("変更しますか？ ", CHECK_DEFAULT_Y)) return FALSE;
		}
		else if(!p_ptr->wizard)
		{
			if(EXTRA_MODE)
				prt("「もう疲れちゃった。また今度ね。」", 7, 20);
			else
				prt("「今日はもう疲れちゃった。また明日ね。」", 7, 20);

			return FALSE;
		}
	}

	clear_bldg(4, 18);

	if (hatate)
		prt("誰を探そう？", 7, 20);
	else if(one_in_(4))
		prt("「おや冒険者さん。ようこそ我がアジトへ」", 7, 20);
	else
		prt("「私に誰かを探してほしいのね？」", 7, 20);

	//完了かキャンセルまで繰り返す
	while (TRUE)
	{
		int i, xx;
		char temp[120] = "";//検索キーワード
		char m_name[120];//モンスター名称候補
		char m_ename[120];//モンスター名称候補(英字)

		monster_race *r_ptr;
		int r_idx_list[HATATE_SEARCH_MON_LIST_MAX];
		int r_idx_list_len = 0;
		int search_r_idx = 0;
		int search_dungeon = 0;
		int search_floor = 0;

		bool flag_too_much = FALSE;

		//検索キーワード入力
		if (!get_string("ユニーク・モンスターの名前(部分一致):", temp, 100))
		{
			clear_bldg(4, 18);
			return FALSE;
		}
		//キーワードの大文字を小文字にしている
		for (xx = 0; temp[xx] && xx < 120; xx++)
		{
			if (iskanji(temp[xx]))
			{
				xx++;
				continue;
			}
			if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
		}

		//全モンスター名称をサーチしてキーワードに部分一致するものを最大10件リストに登録
		for (i = 1; i<max_r_idx; i++)
		{

			r_ptr = &r_info[i];

			//非ユニークは除外(レイマリ除く)
			if (!(r_ptr->flags1 & RF1_UNIQUE) && i != MON_REIMU && i != MON_MARISA) continue;

			//菫子闘技場専用ダミーを除外
			if (i == MON_SUMIREKO_2) continue;

			//紫苑とモズグスは打倒状況により変身前後を排他除外
			if (i == MON_SHION_2 && r_info[MON_SHION_1].r_akills == 0) continue;
			if (i == MON_SHION_1 && r_info[MON_SHION_1].r_akills  > 0) continue;
			if (i == MON_MOZGUS2 && r_info[MON_MOZGUS].r_akills == 0) continue;
			if (i == MON_MOZGUS && r_info[MON_MOZGUS].r_akills  > 0) continue;

			//日本語モンスター名
			strcpy(m_name, r_name + r_ptr->name);

			//英字モンスター名(大文字は小文字にする)
			strcpy(m_ename, r_name + r_ptr->E_name);
			for (xx = 0; m_ename[xx] && xx < 80; xx++) if (isupper(m_ename[xx])) m_ename[xx] = tolower(m_ename[xx]);

			clear_bldg(4, 18);

			//検索キーワードと一致したらr_idxを最大10リストに記録
			if (my_strstr(m_name, temp) || my_strstr(m_ename, temp))
			{
				//リスト上限到達
				if (r_idx_list_len == HATATE_SEARCH_MON_LIST_MAX)
				{
					flag_too_much = TRUE;
					break;
				}
				r_idx_list[r_idx_list_len] = i;
				r_idx_list_len++;

			}

		}
		if (flag_too_much)
		{
			if (hatate)
				prt(format("候補が多すぎるようだ。キーワードを変えてやり直そう。"), 7, 20);
			else
				prt(format("「候補が多すぎね。もう少し絞ってもらえる？」"), 7, 20);
			continue;

		}

		if (!r_idx_list_len)
		{
			if (hatate)
				prt(format("該当なしだ。キーワードを変えてやり直そう。"), 7, 20);
			else
				prt(format("「該当なしね。キーワードを変えてもらえる？」"), 7, 20);

			continue;
		}

		clear_bldg(4, 18);

		if (hatate)	prt(format("%d件該当した！", r_idx_list_len), 8, 20);
		else		prt(format("「...%d件見つかったわ！」", r_idx_list_len), 8, 20);
		//見つかったr_idxのリストの中から探す相手を選ぶ
		//リスト表示するのが面倒なのでリスト先頭から一人ずつYes/Noで確認 たぶんリストにしても大して意味は無いだろう
		for (i = 0; i<r_idx_list_len; i++)
		{
			r_ptr = &r_info[r_idx_list[i]];

			clear_bldg(9, 18);

			//日本語モンスター名を得る
			strcpy(m_name, r_name + r_ptr->name);

			if (hatate)
				prt(format("【%s】を探そうか？", m_name), 9, 20);
			else
				prt(format("「【%s】でいいかしら？」", m_name), 9, 20);

			if (!get_check_strict("探しますか？ ", CHECK_DEFAULT_Y)) continue;

			search_r_idx = r_idx_list[i];
			break;

		}

		clear_bldg(4, 18);
		if (!search_r_idx) return FALSE;

		if (hatate)
			prt(format("%sの居場所は...", m_name), 8, 20);
		else
			prt(format("「%sの居場所はね...", m_name), 8, 20);

		inkey();

		//対象外特殊処理
		//はたて
		if (search_r_idx == MON_HATATE)
		{
			if (hatate)
				prt("もちろんこの部屋が写っている。", 9, 20);
			else
				prt("　あなたの目の前よ？」", 9, 20);
			inkey();
			return FALSE;
		}
		//自分　＠がはたてのときは一つ前に行くはず
		if (monster_is_you(search_r_idx))
		{
			prt("　見つけたわ！私の目の前ね！」", 9, 20);
			inkey();
			return FALSE;
		}

		//打倒済み
		if (r_ptr->r_akills)
		{
			if (hatate)
				prt("　自分が格好良く倒したシーンが写っていた。", 9, 20);
			else
				prt("　あなたにやられてるところが写ってるんだけど...」", 9, 20);
			inkey();
			return FALSE;
		}

		//通常出現しないレアリティ値 分裂後のバーノールルパート、没になった大ムカデが該当
		if (r_ptr->rarity > 100)
		{
			if (hatate)
				prt("画像がぐちゃぐちゃに乱れている。普通に出会える相手ではないようだ。", 9, 20);
			else
			{
				prt("　うーん、わからないわ。", 9, 20);
				prt("　なぜかちゃんと写らないの。」", 10, 20);
			}

			inkey();
			return FALSE;
		}

		//ダンジョンボス
		if (r_ptr->flags7 & RF7_GUARDIAN)
		{
			for (i = 1; i < DUNGEON_MAX; i++)
			{
				if (d_info[i].final_guardian == search_r_idx)
				{
					if (hatate)
						c_put_str(TERM_WHITE, format("%sの最深層を守っているようだ。", (d_name + d_info[i].name)), 9, 20);
					else
					{
						c_put_str(TERM_WHITE, format("そいつは%sの最深層で待ち構えているわ。", (d_name + d_info[i].name)), 9, 20);
						prt("　今の情報は無料でいいよ。」", 10, 20);
					}

					inkey();
					return FALSE;

				}
			}
		}

		//QUESTOR ランクエ対象、クエスト専用、シナリオボス、ゲートキーパーなど
		if (r_ptr->flags1 & RF1_QUESTOR)
		{

			if (hatate)
				prt("　不明だ。どこかを守っているようだ。", 9, 20);
			else
			{
				prt("　よく分からないわ。", 9, 20);
				prt("　じっと動かずにどこかを守っているみたい。」", 10, 20);
			}
			inkey();
			return FALSE;

		}
		//お尋ね者　該当クエスト受領まで探せない
		if (r_ptr->flags3 & RF3_WANTED)
		{
			int cnt;
			bool flag_can_alloc = FALSE;
			for (cnt = max_quests - 1; cnt > 0; cnt--)
			{
				if (quest[cnt].status != QUEST_STATUS_TAKEN || quest[cnt].type != QUEST_TYPE_BOUNTY_HUNT)continue;
				if (quest[cnt].r_idx != search_r_idx) continue;

				flag_can_alloc = TRUE;
				break;
			}

			if (!flag_can_alloc)
			{
				if (hatate)
					prt("　分からない。画面は真っ暗だ。", 9, 20);
				else
				{
					prt("　よく分からないわ。", 9, 20);
					prt("　どこか真っ暗なところに隠れているのかしら？」", 10, 20);
				}
				inkey();
				return FALSE;
			}
		}

		//出現ダンジョンを決定

		if (EXTRA_MODE)
		{
			//EXTRAでは鉄獄と混沌の領域限定
			search_dungeon = dungeon_type;

			//特定ダンジョンにしか出ないモンスターは失敗
			//v2.0.8 FORCE_DEPTH付きでレベル100を超えるモンスターを鉄獄で指定したときも同じく失敗
			if ((search_dungeon == DUNGEON_ANGBAND && (r_ptr->flags1 & RF1_FORCE_DEPTH) && r_ptr->level > 100)
				|| ((r_ptr->flags8 & RF8_STAY_AT_HOME3) && r_ptr->extra != search_dungeon))
			{
				if (hatate)
					prt("　このダンジョンにいないようだ。", 9, 20);
				else
				{
					prt("　うーん、このダンジョンには居ないわね。」", 9, 20);
				}
				inkey();
				return FALSE;
			}

		}
		else
		{
			//特定ダンジョンにしか出ないならそこ
			if (r_ptr->flags8 & RF8_STAY_AT_HOME3)
				search_dungeon = r_ptr->extra;
			//v2.0.8 レベル100以上のFORCE_DEPTHモンスターは混沌の領域
			else if ((r_ptr->flags1 & RF1_FORCE_DEPTH) && r_ptr->level > 100)
				search_dungeon = DUNGEON_CHAOS;
			//特定ダンジョンによく出るなら高確率でそこ AT_HOME1は無視
			else if (r_ptr->flags8 & RF8_STAY_AT_HOME2 && !one_in_(8))
				search_dungeon = r_ptr->extra;
			//レイマリは鉄獄のみ(place_monster_one()でそう設定しているので)
			else if(search_r_idx == MON_REIMU || search_r_idx == MON_MARISA)
				search_dungeon = DUNGEON_ANGBAND;
			//水棲なら玄武の沢
			else if (r_ptr->flags7 & RF7_AQUATIC)
				search_dungeon = DUNGEON_GENBU;
			//動物系なら中確率で魔法の森
			else if(r_ptr->flags3 & RF3_ANIMAL && one_in_(2))
				search_dungeon = DUNGEON_FOREST;
			//亜人系なら中確率で紅魔館
			else if (r_ptr->flags3 & RF3_DEMIHUMAN && one_in_(2))
				search_dungeon = DUNGEON_KOUMA;
			//地獄関係者なら中確率で地獄か無縁塚
			else if (r_ptr->flags7 & RF7_FROM_HELL && one_in_(2))
			{
				if(one_in_(2))
					search_dungeon = DUNGEON_MUEN;
				else
					search_dungeon = DUNGEON_HELL;
			}
			//幻想郷ユニークはフロアレベルが合うとき高確率で鉄獄になりそれ以外のとき各地のどこか
			else if(is_gen_unique(search_r_idx))
			{
				int ang_dlv = max_dlv[DUNGEON_ANGBAND];

				if ((ang_dlv < r_ptr->level + 20) && (ang_dlv > r_ptr->level - 10) && !one_in_(3))
				{
					search_dungeon = DUNGEON_ANGBAND;
				}
				else
				{
					switch (randint1(6))
					{
					case 1:
						search_dungeon = DUNGEON_TAISHI;
						break;
					case 2:
						search_dungeon = DUNGEON_GENBU;
						break;
					case 3:
						search_dungeon = DUNGEON_KOUMA;
						break;
					case 4:
						search_dungeon = DUNGEON_ZIGOKUDANI;
						break;
					case 5:
						search_dungeon = DUNGEON_MUEN;
						break;
					default:
						search_dungeon = DUNGEON_ANGBAND;
						break;
					}

				}
			}
			//それ以外は鉄獄
			else 
			{
				search_dungeon = DUNGEON_ANGBAND;
			}
		}

		//出現フロアを決定

		if (EXTRA_MODE)
		{
			//現在フロアより5〜10深い場所
			search_floor = dun_level + 4 + randint1(6);

			//敵レベルより10レベル以上浅くはならない
			if (r_ptr->level -10 > search_floor ) search_floor = r_ptr->level - 10;

			//EXTRAクエストフロアには出ない
			if ((search_floor != 100) && (search_floor % 10 == 0)) search_floor++;

			//上限
			if (search_floor > d_info[search_dungeon].maxdepth) search_floor = d_info[search_dungeon].maxdepth;

		}
		else
		{

			//v2.0.8　鉄獄が対象になったら出現フロアを＠の帰還フロア近くにすることにした
			if (search_dungeon == DUNGEON_ANGBAND)
			{
					search_floor = max_dlv[DUNGEON_ANGBAND] + randint1(8) - 6;
			}
			//それ以外のダンジョンではモンスターレベル±5
			else
			{
				search_floor = r_ptr->level + randint1(11) - 6;
			}

			//上限
			if (search_floor > d_info[search_dungeon].maxdepth)
			{
				search_floor = d_info[search_dungeon].maxdepth - randint0(3);
			}

			//下限
			if (search_floor < d_info[search_dungeon].mindepth)
			{
				search_floor = d_info[search_dungeon].mindepth + randint0(3);
			}

			//v2.0.12 40レベル以上浅いフロアには出ないことにする
			if (r_ptr->level > search_floor + 40)
			{
				if (hatate)
					prt("　画像が不鮮明だ。自分が行ける場所から遠すぎるのかもしれない。", 9, 20);
				else
				{
					prt("　ちょっと画像が不鮮明ね...", 9, 20);
					prt("　もう少し探索範囲を広げてくれれば分かるかも。」", 10, 20);
				}
				inkey();
				return FALSE;

			}

		}

		if (hatate)
			c_put_str(TERM_WHITE, format("見つけた!%sの%d階にいる！", (d_name + d_info[search_dungeon].name), search_floor), 9, 20);
		else
			c_put_str(TERM_WHITE, format("見つけたわ!%sの%d階よ！」", (d_name + d_info[search_dungeon].name), search_floor), 9, 20);

		inkey();

		//ダンジョンとフロアを専用変数に記録
		p_ptr->hatate_mon_search_ridx = search_r_idx;
		p_ptr->hatate_mon_search_dungeon = search_dungeon*1000 + search_floor;

		break;
	}

	clear_bldg(4, 18);

	return TRUE;

}

#undef HATATE_SEARCH_MON_LIST_MAX

//v2.0.8
//倒した素材モンスターを鯢呑亭に持ち込んで料理してもらう
//素材モンスターはRF8_FOODが設定されており、倒したときにp_ptr->cooking_material_flagに記録されている
//flag_self:自分で料理するときTRUE メッセージのみ変化
//行動順消費するときTRUE(美宵特技以外では関係ない)
bool geidontei_cooking(bool flag_self )
{

	int material_list[16];
	int material_num = 0;
	int food_list_idx;
	int i;

	//まず素材を持っていて提供可能な料理をリストする
	for (i = 0; monster_food_list[i].r_idx; i++)
	{

		if (material_num >= 16)
		{
			msg_print("ERROR:geidontei_cooking()で料理選択画面のページ送りが未実装");
			return FALSE;
		}

		//該当の素材をもっているとき
		if ((1L << i) & p_ptr->cooking_material_flag)
		{
			material_list[material_num] = i;
			material_num++;
		}
	}


	if (!material_num)
	{
		if(!flag_self)
			msg_print("持ち込む素材がない。");
		else
			msg_print("料理の材料がない。");


		return FALSE;
	}

	screen_save();
	for (i = 3; i<22; i++) Term_erase(17, i, 255);


	//一種類のみ作れるとき　y/nで確認
	if (material_num == 1)
	{

		food_list_idx = material_list[0];

		if(!flag_self)
			prt(format("「%s」を作ってくれるようだ。", monster_food_list[food_list_idx].desc), 8, 20);
		else
			prt(format("「%s」が作れそうだ。", monster_food_list[food_list_idx].desc), 8, 20);

		if (!get_check_strict(flag_self ? "作りますか？": "注文しますか？", CHECK_DEFAULT_Y))
		{
			screen_load();
			return FALSE;
		}
	}
	//複数作れるときは料理をリストして選択
	else
	{
		int choose;

		if(!flag_self)
			prt("何を作ってもらいますか？(ESC:キャンセル)", 4, 20);
		else
			prt("何を作りますか？(ESC:キャンセル)", 4, 20);

		for (i = 0; i<material_num; i++)
		{
			prt(format("%c) %s", 'a' + i, monster_food_list[material_list[i]].desc), 5 + i, 20);
		}

		while (TRUE)
		{
			char c;

			c = inkey();

			if (c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}

			choose = c - 'a';

			if (choose < 0 || choose >= material_num) continue;

			break;
		}

		food_list_idx = material_list[choose];

	}

	if(flag_self)
		msg_print("あなたは鼻歌を歌いながら料理を始めた...");
	else if (one_in_(3))
		msg_print("「これ美味しいよー」");
	else if(one_in_(3))
		msg_print("「料理のことならこの奥野田美宵にお任せあれ！」");
	else
		msg_print("「喜んでー！」");


	for (i = 3; i<22; i++) Term_erase(17, i, 255);

	switch (monster_food_list[food_list_idx].r_idx)
	{

	case MON_SUPPON:
		prt("野趣味あふれる液体を一息に呷った！", 8, 20);
		set_hero(5000, FALSE);
		set_alcohol(p_ptr->alcohol + 2000);
		break;

	case MON_WILD_BOAR:
		prt("驚くほど上品な味になったスープを堪能した。", 8, 20);
		prt("体の動作が機敏になった気がする！", 9, 20);
		set_tim_addstat(A_DEX, 2, 5000, FALSE);
		break;

	case MON_WILD_BOAR_2:
		if(!dun_level) //店かダンジョン内かを区別するのにflag_selfだとまだ都合が悪いので適当に判定する
			prt("巨大な肉塊を皆に振る舞って豪快に頬張った！", 8, 20);
		else
			prt("巨大な肉塊を豪快に焼き上げた！", 8, 20);

		prt("力があふれる気がする！", 9, 20);
		set_tim_addstat(A_STR, 4, 5000, FALSE);

		set_deity_bias(DBIAS_REPUTATION, 1);
		set_deity_bias(DBIAS_WARLIKE, 1);
		break;

	case MON_BADGER:
		prt("脂が乗っておいしい！活力が湧き出るようだ！", 8, 20);
		set_tim_addstat(A_CON, 2, 5000, FALSE);
		break;

	case MON_RIVER_CRAB:
		prt("香ばしくておいしい！", 8, 20);
		prt("少し意識が飛んで柿の木の夢を見た気がする...", 9, 20);
		set_tim_res_water(5000, FALSE);
		break;

		//イモリのスープ
	case MON_NEWT:
		prt("元気が湧き出る気がする！", 8, 20);
		set_tim_regen(5000, FALSE);
		break;

		//兎鍋
	case MON_VORPAL_BUNNY:
		prt("あっさりして体に良さそうな味だ。", 8, 20);
		set_oppose_pois(5000, FALSE);
		if(one_in_(3)) prt("兎角同盟から反感を買った気がする...", 9, 20);//別に何も起こらない
		break;

		//球電のキモ
	case MON_BALL_LIGHTNING:
		prt("食べたら体がビリビリし始めた。", 8, 20);
		set_oppose_elec(5000, FALSE);
		break;

		//ヤツメウナギ
	case MON_LAMPREY:
		prt("目がスッキリよく見える気がする。", 8, 20);
		set_blind(0);
		set_tim_invis(5000, FALSE);

		break;

		default:
			msg_format("ERROR:この料理(idx:%d)を食べたときの処理が定義されていない", food_list_idx);
			return FALSE;

	}

	inkey();

	//満腹度処理
	set_food(p_ptr->food + monster_food_list[food_list_idx].food_amount);

	//素材記録リストから使った素材を削除する
	p_ptr->cooking_material_flag &= ~((u32b)(1L << food_list_idx));

	screen_load();

	return TRUE;



}


